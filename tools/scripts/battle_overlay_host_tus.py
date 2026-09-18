#!/usr/bin/env python3
"""List the src/battle translation units that own the adopted host leaves.

pc_port/src/battle_overlay_host_leaves.inc is the allowlist: the set of overlay
functions the port is permitted to run as native C instead of interpreting
disc/battle.bin.  This maps that list onto the translation units that must be
compiled into the port for the allowlist to mean anything -- without it,
runtime_bridge_call's dlsym() finds no host body, every target falls back to
the interpreter, and the allowlist is inert.

Ownership rule.  A body may appear twice: once guarded by XENO_PC_PORT in
src/battle/main.c (weak, port-only) and once unconditionally in the TU that
decompiled it.  The unconditional definition wins -- it is what the matching
build compiles.  A port-only definition is accepted when it is the only owner,
which is the normal case for battle: the matching build still assembles the
retail bytes from its own asm segment while the C body is finished for the
port and the differential harnesses.

Usage:
    tools/scripts/battle_overlay_host_tus.py            # one TU per line
    tools/scripts/battle_overlay_host_tus.py --verify    # also report coverage
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BATTLE_SRC = ROOT / "src" / "battle"
LEAVES = ROOT / "pc_port" / "src" / "battle_overlay_host_leaves.inc"

# A definition, not a declaration: the parameter list is followed by '{' rather
# than ';'.  Mirrors the recogniser in gen_battle_overlay_guest_ram.py.
RETURN_TYPES = (
    r"void|u32|s32|int|long|short|u16|s16|u8|s8|char|"
    r"u32\s*\*|void\s*\*|u8\s*\*|s16\s*\*|u16\s*\*"
)
DEFINITION_RE = re.compile(
    rf"^[ \t]*(?:__attribute__\(\(weak\)\)[ \t]*)?(?:static[ \t]+)?"
    rf"(?:{RETURN_TYPES})[ \t\*]+(func_\w+)[ \t]*\(",
    re.M,
)
CONDITIONAL_RE = re.compile(
    r"^[ \t]*#[ \t]*(ifdef|ifndef|if|elif|else|endif)\b[^\n]*", re.M
)


def leaves() -> list[str]:
    return re.findall(r'"(func_\w+)"', LEAVES.read_text())


def port_only_spans(text: str) -> list[tuple[int, int]]:
    """Byte ranges whose preprocessor branch is taken only with XENO_PC_PORT."""
    stack: list[list] = []  # [mentions_port, branch_is_port_only, start]
    spans: list[tuple[int, int]] = []
    for m in CONDITIONAL_RE.finditer(text):
        kind = m.group(1)
        if kind in ("ifdef", "ifndef", "if"):
            mentions = "XENO_PC_PORT" in m.group(0)
            # `#ifndef XENO_PC_PORT` selects the branch that is NOT port-only.
            taken = (not mentions) if kind == "ifndef" else mentions
            stack.append([mentions, taken, m.end()])
        elif kind == "else" and stack:
            stack[-1][1] = not stack[-1][1]
        elif kind == "elif" and stack:
            stack[-1][1] = False
        elif kind == "endif" and stack:
            _, taken, start = stack.pop()
            if taken:
                spans.append((start, m.start()))
    return spans


def in_spans(offset: int, spans: list[tuple[int, int]]) -> bool:
    return any(start <= offset < end for start, end in spans)


def is_definition(text: str, paren_end: int) -> bool:
    """A definition's parameter list is followed by '{'; a declaration by ';'."""
    depth = 0
    for i in range(paren_end, min(len(text), paren_end + 400)):
        if text[i] == "(":
            depth += 1
        elif text[i] == ")":
            depth -= 1
            if depth == 0:
                return text[i + 1 :].lstrip().startswith("{")
    return False


def definitions() -> dict[str, list[tuple[str, bool]]]:
    """name -> [(translation unit, is_port_only), ...] over all of src/battle."""
    found: dict[str, list[tuple[str, bool]]] = {}
    for path in sorted(BATTLE_SRC.glob("*.c")):
        text = path.read_text(errors="ignore")
        port_only = port_only_spans(text)
        for m in DEFINITION_RE.finditer(text):
            if not is_definition(text, m.end() - 1):
                continue
            found.setdefault(m.group(1), []).append(
                (path.name, in_spans(m.start(), port_only))
            )
    return found


def owners(name: str, found: dict[str, list[tuple[str, bool]]]) -> list[str]:
    entries = found.get(name, [])
    matching = [tu for tu, port_only in entries if not port_only]
    chosen = matching or [tu for tu, _ in entries]
    return sorted(set(chosen))


def main() -> int:
    found = definitions()
    wanted = leaves()
    tus: list[str] = []
    missing: list[str] = []
    ambiguous: list[str] = []
    for name in wanted:
        where = owners(name, found)
        if not where:
            missing.append(name)
            continue
        if len(where) > 1:
            ambiguous.append(f"{name} in {'/'.join(where)}")
            continue
        if where[0] not in tus:
            tus.append(where[0])

    if missing or ambiguous:
        for name in missing:
            print(
                f"ERROR: adopted leaf {name} has no C definition in src/battle",
                file=sys.stderr,
            )
        for item in ambiguous:
            print(f"ERROR: adopted leaf has rival owners: {item}", file=sys.stderr)
        print(
            "ERROR: the allowlist and src/battle disagree; refusing to emit a TU list.",
            file=sys.stderr,
        )
        return 1

    if "--leaves" in sys.argv:
        for name in wanted:
            print(name)
        return 0

    for tu in sorted(tus):
        print(f"src/battle/{tu}")
    if "--verify" in sys.argv:
        print(
            f"# {len(wanted)} adopted leaves across {len(tus)} translation units",
            file=sys.stderr,
        )
    return 0


if __name__ == "__main__":
    sys.exit(main())
