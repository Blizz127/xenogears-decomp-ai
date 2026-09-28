#!/usr/bin/env python3
"""Preservation report for the PC port: what the port runs vs the matched C.

Definitions (docs/ARCHITECTURE-PORT.md records them verbatim):

  matched C            the function is in objdiff's byte-exact set
                       (build/progress.json, fuzzy_match_percent == 100; the
                       tree passes rom-check) AND the body the port compiles
                       is that same source with no XENO_PC_PORT arm inside it.
  matched C with port seam
                       same, but the shared body has an inner
                       #if/#ifdef XENO_PC_PORT (pointer width, host seam).
  port-only C shadowing matched C
                       the port runs a different body for a byte-exact
                       function: a XENO_PC_PORT arm in the game TU, or a
                       definition in pc_port/ (including wm_XXXXXXXX for
                       world_map func_XXXXXXXX).
  port-only C (no retail match yet)
                       a port body for a retail function that has no
                       byte-exact C yet.
  shim/compat          port infrastructure that is not a retail function:
                       psyq_compat, xg_plat, harness, generated stubs.
  PsyCross             the pinned PsyCross library.

Usage:
  port_preservation.py shadows  --progress build/progress.json --objdir OBJ
  port_preservation.py coverage --progress build/progress.json --objdir OBJ \
        --binary BIN --map MAP COVERAGE.txt [COVERAGE.txt ...]
"""
import argparse
import bisect
import json
import os
import re
import subprocess
import sys
from collections import Counter, defaultdict

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
CATS = ["matched C", "matched C with port seam", "port-only C shadowing matched C",
        "port-only C (no retail match yet)", "shim/compat", "PsyCross"]


SDK = set()  # Psy-Q SDK functions (objdiff category "sdk"): the platform
             # layer (PsyCross + psyq_compat) replaces them by design.


def load_progress(path):
    """(matched, universe) of retail GAME functions; SDK names go to SDK."""
    data = json.load(open(path))
    matched, universe = set(), set()
    for unit in data["units"]:
        cats = unit.get("metadata", {}).get("progress_categories", [])
        for fn in unit.get("functions", []):
            if "sdk" in cats:
                SDK.add(fn["name"])
                continue
            universe.add(fn["name"])
            if fn.get("fuzzy_match_percent") == 100.0:
                matched.add(fn["name"])
                MATCHED_IN.add((unit["name"].split("/")[0], fn["name"]))
    return matched, universe


# (overlay, name) pairs that are byte-exact: overlays share address ranges,
# so a func_XXXXXXXX name can exist in several overlays with different status.
MATCHED_IN = set()


def src_matched(src, name, matched):
    """Byte-exact status of `name` as compiled from game TU `src`."""
    parts = src.split("/")
    if len(parts) > 2 and parts[0] == "src":
        return (parts[1], name) in MATCHED_IN
    return name in matched


# --- game TU scan: which XENO_PC_PORT arm each function body lives in -------
DIRECTIVE = re.compile(r"^\s*#\s*(ifdef|ifndef|if|elif|else|endif)\b(.*)")
FUNC_HEAD = re.compile(r"^(?!\s)(?!(?:if|for|while|switch|return|else|do)\b)"
                       r"[A-Za-z_][\w\s\*]*?\b([A-Za-z_]\w*)\s*\(([^;{}]*)\)\s*(\{)?\s*$")


def cond_side(kind, expr):
    """'port', 'retail' or None for a directive's first branch."""
    if "XENO_PC_PORT" not in expr:
        return None
    if kind == "ifdef":
        return "port"
    if kind == "ifndef":
        return "retail"
    e = expr.replace(" ", "")
    if "!defined(XENO_PC_PORT)" in e or "!XENO_PC_PORT" in e:
        return "retail"
    return "port"


def scan_tu(path):
    """name -> list of (region, has_inner_port_directive) for C definitions."""
    defs = defaultdict(list)
    stack = []  # each: [side_of_first_branch, in_else]
    depth = 0
    cur = None  # [name, region, seam]
    pending = None
    try:
        lines = open(path, errors="replace").read().split("\n")
    except OSError:
        return defs

    def region():
        r = "both"
        for side, in_else, *_ in stack:
            if side is None:
                continue
            eff = side if not in_else else ("retail" if side == "port" else "port")
            r = eff
        return r

    for line in lines:
        m = DIRECTIVE.match(line)
        if m:
            kind, expr = m.group(1), m.group(2)
            # Brace depth is tracked per arm: each #else/#elif restarts from
            # the depth at the #if, and #endif resumes with the first arm's
            # end depth (arms may each open a brace, e.g. two loop headers).
            if kind in ("ifdef", "ifndef", "if"):
                stack.append([cond_side(kind, expr), False, depth, None, cur])
                if cur and "XENO_PC_PORT" in expr:
                    cur[2] = True
            elif kind in ("else", "elif") and stack:
                top = stack[-1]
                if top[3] is None:
                    top[3] = (depth, cur)
                top[1] = True
                depth = top[2]
                cur = top[4]
                if cur and top[0] is not None:
                    cur[2] = True
            elif kind == "endif" and stack:
                top = stack.pop()
                if top[3] is not None:
                    depth, cur = top[3]
            continue
        code = re.sub(r"//.*|/\*.*?\*/|\"(\\.|[^\"])*\"|'(\\.|[^'])*'", "", line)
        if depth == 0 and cur is None:
            fm = FUNC_HEAD.match(code)
            if fm and fm.group(1) not in ("INCLUDE_ASM", "INCLUDE_RODATA"):
                pending = [fm.group(1), region(), False]
                if fm.group(3) is None:
                    opened = code.count("{")
                    if not opened:
                        continue
            elif pending is not None and code.strip() and not code.strip().startswith("{"):
                pending = None
        opens, closes = code.count("{"), code.count("}")
        if depth == 0 and opens and pending is not None:
            cur, pending = pending, None
        depth += opens - closes
        if depth <= 0:
            depth = 0
            if cur is not None and closes:
                defs[cur[0]].append((cur[1], cur[2]))
                cur = None
    return defs


def game_tu_scans():
    scans = {}
    for base, _, files in os.walk(os.path.join(ROOT, "src")):
        for f in files:
            if f.endswith(".c"):
                p = os.path.join(base, f)
                scans[os.path.relpath(p, ROOT)] = scan_tu(p)
    return scans


def src_class(name, scan, matched, universe, src=""):
    """Class of a function the port compiles from a game TU."""
    if name not in universe:
        return "shim/compat"  # port helper living in a game TU (PcPort_*, diag)
    entries = scan.get(name, [])
    port_side = [e for e in entries if e[0] in ("port", "both")]
    if not port_side:
        return None
    region, seam = port_side[0]
    if not src_matched(src, name, matched):
        return "port-only C (no retail match yet)"
    if region == "port":
        return "port-only C shadowing matched C"
    return "matched C with port seam" if seam else "matched C"


def retail_name(name, universe):
    if name in universe:
        return name
    m = re.match(r"wm_([0-9A-Fa-f]{8})$", name)
    if m and ("func_" + m.group(1).upper()) in universe:
        return "func_" + m.group(1).upper()
    return None


def port_class(name, matched, universe):
    r = retail_name(name, universe)
    if r is None:
        return "shim/compat", None
    if r in matched:
        return "port-only C shadowing matched C", r
    return "port-only C (no retail match yet)", r


_CU_CACHE = {}


def obj_to_src(objpath, objdir=None):
    """Source file of an object: its DWARF compile-unit name (built with -g)."""
    if objpath in _CU_CACHE:
        return _CU_CACHE[objpath]
    name = os.path.basename(objpath)
    res = subprocess.run(["readelf", "--debug-dump=info", "--dwarf-depth=1", objpath],
                         capture_output=True, text=True)
    m = re.search(r"DW_AT_name\s*:.*?:\s*(\S+)\s*$", res.stdout, re.M)
    if m:
        name = m.group(1)
        if name.startswith("/xenogears-decomp/"):
            name = name[len("/xenogears-decomp/"):]
    _CU_CACHE[objpath] = name
    return name


def defined_symbols(objdir):
    """[(object path, symbol)] for every text symbol in the build's objects."""
    out = []
    for base, _, files in os.walk(objdir):
        for f in files:
            if not f.endswith(".o"):
                continue
            p = os.path.join(base, f)
            res = subprocess.run(["nm", "--defined-only", p], capture_output=True, text=True)
            for line in res.stdout.splitlines():
                parts = line.split()
                if len(parts) == 3 and parts[1] in "Tt":
                    out.append((p, parts[2]))
    return out


def cmd_shadows(args):
    matched, universe = load_progress(args.progress)
    scans = game_tu_scans()
    rows = []
    # (1) XENO_PC_PORT arms in game TUs shadowing a byte-exact function
    for tu, scan in sorted(scans.items()):
        for name, entries in scan.items():
            if tu.startswith("src/slus_006.64/psyq/"):
                continue
            if src_matched(tu, name, matched) and any(e[0] == "port" for e in entries):
                rows.append((name, "port arm in " + tu, name))
    # (2) pc_port definitions of byte-exact retail functions (incl. wm_)
    for obj, sym in defined_symbols(args.objdir):
        src = obj_to_src(obj, args.objdir)
        if src.startswith("src/"):
            continue
        cls, r = port_class(sym, matched, universe)
        if cls == "port-only C shadowing matched C":
            if src.endswith("stubs.c"):
                rows.append((r, "generated logging stub (matched C not linked)", sym))
            else:
                rows.append((r, "pc_port body " + sym + " (" + src + ")", sym))
    rows = sorted(set(rows))
    print("# Port bodies shadowing byte-exact matched C (objdiff 100%)")
    print("# retail function\twhere\tport symbol")
    for r in rows:
        print("\t".join(r))
    print(f"# total: {len(rows)} shadowing bodies for "
          f"{len(set(r[0] for r in rows))} matched functions", file=sys.stderr)


def load_map_ranges(mapfile):
    """sorted [(start, end, object)] for .text input sections."""
    ranges = []
    pend = None
    sec = re.compile(r"^ \.text(?:\.\S+)?\s+0x([0-9a-f]+)\s+0x([0-9a-f]+)\s+(\S+)")
    sec1 = re.compile(r"^ \.text(?:\.\S+)?\s*$")
    cont = re.compile(r"^\s+0x([0-9a-f]+)\s+0x([0-9a-f]+)\s+(\S+\.(?:o|a\(.*\)))\s*$")
    for line in open(mapfile, errors="replace"):
        m = sec.match(line)
        if m:
            s, n = int(m.group(1), 16), int(m.group(2), 16)
            if n:
                ranges.append((s, s + n, m.group(3)))
            pend = None
            continue
        if sec1.match(line):
            pend = True
            continue
        if pend:
            m = cont.match(line)
            if m:
                s, n = int(m.group(1), 16), int(m.group(2), 16)
                if n:
                    ranges.append((s, s + n, m.group(3)))
            pend = None
    ranges.sort()
    return ranges


def cmd_coverage(args):
    matched, universe = load_progress(args.progress)
    scans = game_tu_scans()
    syms = {}
    res = subprocess.run(["nm", "--defined-only", args.binary], capture_output=True, text=True)
    for line in res.stdout.splitlines():
        parts = line.split()
        if len(parts) == 3 and parts[1] in "Tt":
            syms.setdefault(int(parts[0], 16), parts[2])
    ranges = load_map_ranges(args.map)
    starts = [r[0] for r in ranges]
    objdir = os.path.abspath(args.objdir)
    for cov in args.coverage:
        funcs, calls = Counter(), Counter()
        detail = defaultdict(list)
        for line in open(cov):
            a, c = line.split()
            addr, n = int(a, 16), int(c)
            name = syms.get(addr, hex(addr))
            i = bisect.bisect_right(starts, addr) - 1
            obj = ranges[i][2] if i >= 0 and ranges[i][0] <= addr < ranges[i][1] else "?"
            if "libpsycross" in obj:
                cls = "PsyCross"
            else:
                objabs = obj if os.path.isabs(obj) else os.path.join(ROOT, obj)
                src = obj_to_src(objabs, objdir) if objabs.startswith(objdir) else obj
                cls = None
                if src.startswith("src/slus_006.64/psyq/"):
                    cls = "shim/compat"
                elif src.startswith("src/") and src in scans:
                    cls = src_class(name, scans[src], matched, universe, src)
                if cls is None:
                    cls = port_class(name, matched, universe)[0]
            funcs[cls] += 1
            calls[cls] += n
            detail[cls].append((n, name))
        tf, tc = sum(funcs.values()), sum(calls.values())
        game_f = sum(funcs[c] for c in CATS[:4])
        game_c = sum(calls[c] for c in CATS[:4])
        print(f"## {os.path.basename(cov)}")
        print("| class | functions | % | calls | % |")
        print("|---|---:|---:|---:|---:|")
        for c in CATS:
            print(f"| {c} | {funcs[c]} | {100*funcs[c]/max(tf,1):.1f} | {calls[c]} | {100*calls[c]/max(tc,1):.1f} |")
        print(f"| total | {tf} | | {tc} | |")
        mf = funcs[CATS[0]] + funcs[CATS[1]]
        mc = calls[CATS[0]] + calls[CATS[1]]
        print(f"\nmatched-C ratio of executed retail-function code: {mf}/{game_f} functions "
              f"({100*mf/max(game_f,1):.1f}%), {mc}/{game_c} calls ({100*mc/max(game_c,1):.1f}%)"
              f"  [strict matched C only: {funcs[CATS[0]]} functions]\n")
        if args.detail:
            with open(args.detail + "." + os.path.basename(cov) + ".tsv", "w") as f:
                for c in CATS:
                    for n, name in sorted(detail[c], reverse=True):
                        f.write(f"{c}\t{name}\t{n}\n")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    s = sub.add_parser("shadows")
    s.add_argument("--progress", required=True)
    s.add_argument("--objdir", required=True)
    c = sub.add_parser("coverage")
    c.add_argument("--progress", required=True)
    c.add_argument("--objdir", required=True)
    c.add_argument("--binary", required=True)
    c.add_argument("--map", required=True)
    c.add_argument("--detail")
    c.add_argument("coverage", nargs="+")
    args = ap.parse_args()
    (cmd_shadows if args.cmd == "shadows" else cmd_coverage)(args)


if __name__ == "__main__":
    main()
