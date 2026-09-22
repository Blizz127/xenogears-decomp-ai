#!/usr/bin/env python3
"""Strict parity pass: relocations are RESOLVED, not masked away.

analyze.py masks relocation-bearing immediates outright, so a `lui/%lo` pair
pointing at the WRONG global still scores byte-exact.  That makes its figure an
upper bound.

This pass reads the unlinked objects with `objdump -dr`, resolves each
relocation's symbol to its RETAIL address, recomputes the immediate the linker
would write, patches it into the built word, and then compares the full 32-bit
word against retail.  Retail addresses are used deliberately: our own link
places .text at different addresses, and charging that to codegen would
conflate the layout shift with real divergence.  Data symbols are at fixed
retail addresses in both, so they compare meaningfully either way.

Resolving rather than name-matching also avoids two large classes of false
mismatch: the project renames retail symbols (g_InterruptEnvironment vs
D_800578A4), and spimdisasm sometimes prints a bare constant where our object
carries a relocation.

Result is directly comparable to objdiff's matched_code_percent.

Usage, inside the toolchain container from the project root:
    python3 docs/evidence/slus-byte-parity-20260921/strict.py
"""
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import analyze  # noqa: E402

OUT = os.path.dirname(os.path.abspath(__file__))
OBJROOT = "build/src/slus_006.64"
GP = 0x80059170

SYM = re.compile(r"^([0-9a-f]+) <([^>]+)>:")
INS = re.compile(r"^\s*([0-9a-f]+):\s+([0-9a-f]{8})\s")
REL = re.compile(r"^\s*([0-9a-f]+):\s+(R_MIPS_\w+)\s+(\S+)")
ADDRNAME = re.compile(r"_(8[0-9A-Fa-f]{7})$")
SYMLINE = re.compile(r"^\s*(\S+)\s*=\s*0x([0-9A-Fa-f]+)\s*;")


def load_symbol_addrs():
    """retail symbol name -> retail address, from the splat symbol lists"""
    table = {}
    for path in ("config/symbol_addrs.slus_006.64.txt",):
        with open(path, errors="replace") as fh:
            for line in fh:
                m = SYMLINE.match(line)
                if m:
                    table[m.group(1)] = int(m.group(2), 16)
    return table


def objects():
    for dirpath, _d, files in os.walk(OBJROOT):
        for fn in sorted(files):
            if fn.endswith(".o"):
                yield os.path.join(dirpath, fn)


NMLINE = re.compile(r"^([0-9a-f]+)\s+\S\s+(\S+)$")


def text_symbols(path):
    """offset within .text -> symbol name, for this object"""
    txt = subprocess.run(["mips-linux-gnu-nm", path],
                         capture_output=True, text=True).stdout
    out = {}
    for line in txt.splitlines():
        m = NMLINE.match(line.strip())
        if m:
            out.setdefault(int(m.group(1), 16), m.group(2))
    return out


def load_obj(path):
    """name -> (words, {idx: (reltype, relsym, addend)})"""
    txt = subprocess.run(["mips-linux-gnu-objdump", "-dr", path],
                         capture_output=True, text=True, check=True).stdout
    funcs, cur = {}, None
    for line in txt.splitlines():
        m = SYM.match(line)
        if m:
            cur = m.group(2)
            funcs[cur] = [[], {}, int(m.group(1), 16)]
            continue
        if cur is None:
            continue
        m = INS.match(line)
        if m:
            funcs[cur][0].append(int(m.group(2), 16))
            continue
        m = REL.match(line)
        if m:
            idx = (int(m.group(1), 16) - funcs[cur][2]) // 4
            raw = m.group(3)
            addend = 0
            if "+0x" in raw:
                raw, a = raw.split("+0x", 1)
                addend = int(a, 16)
            elif "-0x" in raw:
                raw, a = raw.split("-0x", 1)
                addend = -int(a, 16)
            if 0 <= idx < len(funcs[cur][0]):
                funcs[cur][1][idx] = (m.group(2), raw, addend)
    return {k: (v[0], v[1]) for k, v in funcs.items() if v[0]}


class Resolver:
    def __init__(self, table):
        self.table = table
        self.unresolved = set()

    def __call__(self, name):
        m = ADDRNAME.search(name)
        if m:
            return int(m.group(1), 16)
        if name in self.table:
            return self.table[name]
        self.unresolved.add(name)
        return None


def patch(word, reltype, target):
    """Write the immediate the linker would write for this relocation."""
    if reltype == "R_MIPS_HI16":
        return (word & 0xFFFF0000) | (((target + 0x8000) >> 16) & 0xFFFF)
    if reltype in ("R_MIPS_LO16",):
        return (word & 0xFFFF0000) | (target & 0xFFFF)
    if reltype in ("R_MIPS_GPREL16", "R_MIPS_LITERAL"):
        return (word & 0xFFFF0000) | ((target - GP) & 0xFFFF)
    if reltype == "R_MIPS_26":
        return (word & 0xFC000000) | ((target >> 2) & 0x03FFFFFF)
    return None


def compare(rw, bw, brels, res, why=None, tsyms=None):
    if len(rw) != len(bw):
        return "size", None

    def note(k):
        if why is not None:
            why[k] = why.get(k, 0) + 1

    bad = 0
    for i, (a, b) in enumerate(zip(rw, bw)):
        rel = brels.get(i)
        if rel is not None:
            reltype, name, addend = rel
            if name == ".text" and tsyms is not None and addend in tsyms:
                # a call/reference to a static function in the same object:
                # objdump names the section, so recover the symbol first
                tgt = res(tsyms[addend])
            else:
                tgt = res(name)
            if tgt is None:
                note("unresolvable relocation symbol")
                bad += 1
                continue
            p = patch(b, reltype, tgt + addend)
            if p is None:
                note(f"unhandled {reltype}")
                bad += 1
                continue
            if p != a:
                note("relocated word differs")
                bad += 1
            continue
        if a != b:
            note("plain instruction differs")
            bad += 1
    return ("exact" if bad == 0 else "codegen"), bad


def main():
    retail = analyze.load_retail()
    inc_names, _n = analyze.load_include_asm()
    res = Resolver(load_symbol_addrs())

    ex = cg = sz = 0
    ex_b = tot_b = 0
    ex_c = ex_c_b = 0
    seen, rows, why = set(), [], {}
    for obj in objects():
        tsyms = text_symbols(obj)
        for name, (bw, brels) in load_obj(obj).items():
            if name not in retail or name in seen:
                continue
            seen.add(name)
            rw = retail[name][1]
            v, bad = compare(rw, bw, brels, res, why, tsyms)
            nb = len(rw) * 4
            tot_b += nb
            if v == "exact":
                ex += 1
                ex_b += nb
                if name not in inc_names:
                    ex_c += 1
                    ex_c_b += nb
            elif v == "codegen":
                cg += 1
            else:
                sz += 1
            rows.append((name, v, bad, nb, obj))

    lines = []
    A = lines.append
    A("== STRICT parity: relocations resolved to retail addresses ==")
    A(f"functions compared (in a C object and in retail asm): {len(seen)}")
    A(f"byte-exact:                {ex:5d}  ({ex_b} B, "
      f"{100.0*ex_b/tot_b:.2f}% of {tot_b} comparable bytes)")
    A(f"  of which INCLUDE_ASM:    {ex - ex_c:5d}")
    A(f"  genuinely from C:        {ex_c:5d}  ({ex_c_b} B, "
      f"{100.0*ex_c_b/tot_b:.2f}%)")
    A(f"same size, different code: {cg:5d}")
    A(f"different size:            {sz:5d}")
    A("")
    A("differing words by cause:")
    for k, v in sorted(why.items(), key=lambda kv: -kv[1]):
        A(f"    {v:>7}  {k}")
    A("")
    A(f"unresolvable relocation symbols: {len(res.unresolved)}")
    for s in sorted(res.unresolved)[:15]:
        A(f"    {s}")

    with open(os.path.join(OUT, "strict-summary.txt"), "w") as fh:
        fh.write("\n".join(lines) + "\n")
    with open(os.path.join(OUT, "strict-per-function.tsv"), "w") as fh:
        fh.write("symbol\tverdict\tbad_words\tretail_bytes\tobject\n")
        for r in sorted(rows):
            fh.write("\t".join(str(x) for x in r) + "\n")
    print("\n".join(lines))


if __name__ == "__main__":
    main()
