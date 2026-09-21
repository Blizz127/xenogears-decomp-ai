#!/usr/bin/env python3
"""Triage the slus_006.64 "short by a large margin" cohort.

Reuses the structural tests the field worker committed in
docs/evidence/field-codegen-20260921/TRIAGE.md, which found 18 of 18 innocent:

  Test A  does retail CALL anything the C never calls?
  Test B  does retail WRITE a global the C body never names?
  Test C  branch / store / load / call volume accounting
  plus    the lui delta, which was the dominant cause in field: GCC 2.7.2
          retail re-materializes a global's base address at EVERY use
          (lui+addiu per access) where the C hoists it into a local.

Usage, inside the toolchain container from the project root:
    python3 docs/evidence/slus-codegen-triage-20260921/triage.py
"""
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, "..", "slus-byte-parity-20260921"))
import analyze  # noqa: E402
import strict   # noqa: E402

SRCDIR = "src/slus_006.64"
COHORT_TSV = "docs/evidence/slus-byte-parity-20260921/short-functions.tsv"

# retail asm operand spellings
HILO = re.compile(r"%(?:hi|lo)\(([^)]+)\)")
JAL = re.compile(r"\bjal\s+(\S+)")

STORE_MN = ("sb", "sh", "sw", "swl", "swr", "swc2", "swc1", "sdc2")
LOAD_MN = ("lb", "lbu", "lh", "lhu", "lw", "lwl", "lwr", "lwc2", "lwc1", "ldc2")
BRANCH_MN = ("beq", "bne", "blez", "bgtz", "bltz", "bgez", "beqz", "bnez",
             "bltzal", "bgezal", "b")

# MIPS primary opcodes
OP_LUI = 0x0F
OP_JAL = 0x03
STORE_OPS = {0x28, 0x29, 0x2A, 0x2B, 0x2E, 0x39, 0x3A, 0x3D, 0x3E}
LOAD_OPS = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x31, 0x32, 0x35, 0x36}
BRANCH_OPS = {0x01, 0x04, 0x05, 0x06, 0x07,
              0x14, 0x15, 0x16, 0x17}


def cohort():
    rows = []
    with open(COHORT_TSV) as fh:
        next(fh)
        for line in fh:
            f = line.rstrip("\n").split("\t")
            deficit, name, retail, built = int(f[0]), f[1], int(f[2]), int(f[3])
            if retail and built / retail < 0.75:
                rows.append((deficit, name, retail, built))
    return rows


# ------------------------------------------------------------- retail side

def retail_asm_lines():
    """function name -> [raw asm text lines]"""
    out, cur = {}, None
    for dirpath, _d, files in os.walk(analyze.ASMDIR):
        if os.sep + "data" in dirpath + os.sep:
            continue
        for fn in sorted(files):
            if not fn.endswith(".s"):
                continue
            path = os.path.join(dirpath, fn)
            local, order, cur = {}, [], None
            for line in open(path, errors="replace"):
                m = analyze.GLABEL.match(line.strip())
                if m:
                    cur = m.group(1)
                    if cur.startswith("D_"):
                        cur = None
                    else:
                        local.setdefault(cur, [])
                        order.append(cur)
                    continue
                if cur and analyze.ASM_LINE.match(line):
                    local[cur].append(line.rstrip("\n"))
            for n in order:
                if local[n] and n not in out:
                    out[n] = local[n]
    return out


def mnemonic(line):
    body = line.split("*/", 1)[1] if "*/" in line else line
    parts = body.split()
    return parts[0] if parts else ""


def retail_stats(lines):
    calls, stores, loads, branches, luis = [], 0, 0, 0, 0
    stored_syms, all_syms = set(), set()
    for ln in lines:
        mn = mnemonic(ln)
        m = JAL.search(ln)
        if mn == "jal" and m:
            calls.append(m.group(1))
        if mn in STORE_MN:
            stores += 1
            s = HILO.search(ln)
            if s:
                stored_syms.add(s.group(1))
        if mn in LOAD_MN:
            loads += 1
        if mn in BRANCH_MN:
            branches += 1
        if mn == "lui":
            luis += 1
        s = HILO.search(ln)
        if s:
            all_syms.add(s.group(1))
    return dict(calls=calls, stores=stores, loads=loads,
                branches=branches, luis=luis,
                stored_syms=stored_syms, all_syms=all_syms)


def retail_sym_refs(lines):
    """symbol -> number of %hi/%lo references in retail (2 per hoisted access)"""
    counts = {}
    for ln in lines:
        s = HILO.search(ln)
        if s:
            counts[s.group(1)] = counts.get(s.group(1), 0) + 1
    return counts


# -------------------------------------------------------------- built side

def built_stats(words, rels):
    calls, stores, loads, branches, luis = [], 0, 0, 0, 0
    stored_syms = set()
    for i, w in enumerate(words):
        op = w >> 26
        if op == OP_JAL:
            rel = rels.get(i)
            calls.append(rel[1] if rel else "?")
        if op in STORE_OPS:
            stores += 1
            rel = rels.get(i)
            if rel:
                stored_syms.add(rel[1])
        if op in LOAD_OPS:
            loads += 1
        if op in BRANCH_OPS:
            branches += 1
        if op == OP_LUI:
            luis += 1
    return dict(calls=calls, stores=stores, loads=loads,
                branches=branches, luis=luis, stored_syms=stored_syms)


# ------------------------------------------------------------- C body text

def c_body(name):
    """(path, text) of the C definition of `name`, by brace matching."""
    pat = re.compile(r"(^|[^\w])" + re.escape(name) + r"\s*\(")
    for dirpath, _d, files in os.walk(SRCDIR):
        for fn in sorted(files):
            if not fn.endswith(".c"):
                continue
            path = os.path.join(dirpath, fn)
            src = open(path, errors="replace").read()
            for m in pat.finditer(src):
                # a definition has a '{' before the next ';'
                j = src.find("{", m.end())
                k = src.find(";", m.end())
                if j == -1 or (k != -1 and k < j):
                    continue
                depth, i = 0, j
                while i < len(src):
                    if src[i] == "{":
                        depth += 1
                    elif src[i] == "}":
                        depth -= 1
                        if depth == 0:
                            return path, src[m.start():i + 1]
                    i += 1
    return None, None


def main():
    rows = cohort()
    rasm = retail_asm_lines()
    res = strict.Resolver(strict.load_symbol_addrs())

    # index every built object once
    built = {}
    for obj in strict.objects():
        for n, (w, r) in strict.load_obj(obj).items():
            built.setdefault(n, (w, r, obj))

    out = []
    A = out.append
    A("== slus short-function cohort triage ==")
    A("Method: field-codegen-20260921/TRIAGE.md Tests A/B/C + lui delta.")
    A("")
    hdr = (f"{'func':<26}{'defB':>6}{'store':>7}{'brnch':>7}{'call':>6}"
           f"{'luiD':>6}{'luiInsn':>8}  A  B  TU")
    A(hdr)
    detail = []
    for deficit, name, rsize, bsize in rows:
        if name not in rasm or name not in built:
            A(f"{name:<26}{deficit:>6}   NOT COMPARABLE")
            continue
        rl = rasm[name]
        rs = retail_stats(rl)
        bw, brels, obj = built[name]
        bs = built_stats(bw, brels)

        # Test A: retail call targets the C never emits
        bcalls = set()
        for c in bs["calls"]:
            bcalls.add(c)
        path, body = c_body(name)
        a_fail = []
        for t in set(rs["calls"]):
            if t in bcalls:
                continue
            if body and re.search(r"\b" + re.escape(t) + r"\b", body):
                continue
            a_fail.append(t)

        # Test B: globals retail stores to that the C body never names
        b_fail = []
        for s in rs["stored_syms"]:
            if s in bs["stored_syms"]:
                continue
            if body and re.search(r"\b" + re.escape(s) + r"\b", body):
                continue
            b_fail.append(s)

        lui_d = rs["luis"] - bs["luis"]
        A(f"{name:<26}{deficit:>6}"
          f"{bs['stores']-rs['stores']:>7}"
          f"{bs['branches']-rs['branches']:>7}"
          f"{len(bs['calls'])-len(rs['calls']):>6}"
          f"{lui_d:>6}{lui_d*2:>8}"
          f"  {'!' if a_fail else '.'}  {'!' if b_fail else '.'}"
          f"  {os.path.relpath(path, SRCDIR) if path else '?'}")

        refs = sorted(retail_sym_refs(rl).items(), key=lambda kv: -kv[1])[:4]
        detail.append((name, deficit, a_fail, b_fail, refs, rs, bs, path))

    A("")
    A("store/brnch/call columns are BUILT minus RETAIL: a positive call delta")
    A("means the C calls more often than retail, i.e. retail inlined.")
    A("luiD is retail lui minus built lui; luiInsn = 2x that, the instruction")
    A("cost of retail re-materializing base addresses the C hoists.")
    A("'A'/'B' are the decisive tests; '!' means the function FAILS it.")
    A("")
    A("== detail ==")
    for name, deficit, a_fail, b_fail, refs, rs, bs, path in detail:
        A(f"-- {name}  deficit {deficit}  ({path})")
        A(f"   retail: {rs['luis']} lui, {rs['stores']} st, {rs['loads']} ld, "
          f"{rs['branches']} br, {len(rs['calls'])} calls")
        A(f"   built : {bs['luis']} lui, {bs['stores']} st, {bs['loads']} ld, "
          f"{bs['branches']} br, {len(bs['calls'])} calls")
        A(f"   top retail %hi/%lo symbols: "
          + ", ".join(f"{s}x{c}" for s, c in refs))
        A(f"   Test A retail-only callees: {a_fail if a_fail else 'none'}")
        A(f"   Test B retail-only stored globals: {b_fail if b_fail else 'none'}")
    txt = "\n".join(out)
    open(os.path.join(HERE, "triage-raw.txt"), "w").write(txt + "\n")
    print(txt)


if __name__ == "__main__":
    main()
