#!/usr/bin/env python3
"""Relocation-aware, per-function instruction diff: built .o vs retail splat .s.

Usage:
  asm_func_diff.py --obj build/src/field/dialogue/text_box_render.c.o \
                   --asm-dir asm/field/matchings/dialogue/text_box_render \
                   [--func NAME] [--context N] [--summary]

Retail side: each `/* OFF ADDR WORD */` comment line in the .s carries the exact
linked 32-bit word (bytes as printed, little-endian). Built side: objdump -dr of
the object. Words are compared directly, except:
  * HI16/LO16 relocated instructions: immediate masked to 0, and the resolved
    (symbol address + addend) is compared against the retail `%hi(sym + off)` /
    `%lo(sym + off)` resolved through the project symbol tables. This makes
    `D_800ADF04 + 2` and `D_800ADF06` compare equal, as they link identically.
  * jal (R_MIPS_26 to a symbol): compares target address.
  * j to a local label: compares offset relative to function start.
Branch words are compared raw (offsets are pc-relative so identical layout
means identical words). Alignment uses difflib so insertions/deletions show
as such rather than cascading.
"""
import argparse
import difflib
import glob
import os
import re
import struct
import subprocess
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
OBJDUMP = os.environ.get("MIPS_OBJDUMP", "mips-linux-gnu-objdump")

SYM_FILES = [
    "config/symbol_addrs.field.txt",
    "config/symbol_addrs.slus_006.64.txt",
    "linker/undefined_syms_auto.field.txt",
    "linker/undefined_funcs_auto.field.txt",
]

_sym_cache = None


def load_symbols():
    global _sym_cache
    if _sym_cache is not None:
        return _sym_cache
    syms = {}
    rx = re.compile(r"^\s*([A-Za-z_]\w*)\s*=\s*(0x[0-9A-Fa-f]+)\s*;")
    for rel in SYM_FILES:
        p = os.path.join(ROOT, rel)
        if not os.path.exists(p):
            continue
        with open(p) as f:
            for line in f:
                m = rx.match(line)
                if m:
                    syms.setdefault(m.group(1), int(m.group(2), 16))
    _sym_cache = syms
    return syms


def resolve(name):
    syms = load_symbols()
    if name in syms:
        return syms[name]
    m = re.match(r"^(?:D_|func_|jtbl_|jpt_)([0-9A-Fa-f]{8})$", name)
    if m:
        return int(m.group(1), 16)
    return None


def sext16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


# ---------------------------------------------------------------- retail side
RETAIL_LINE = re.compile(
    r"^\s*/\*\s*([0-9A-F]+)\s+([0-9A-F]{8})\s+([0-9A-F]{8})\s*\*/\s+(\S+)\s*(.*)$"
)
LABEL_LINE = re.compile(r"^\s*(\.L[0-9A-Fa-f]+):\s*$")
GLABEL_LINE = re.compile(r"^\s*glabel\s+(\w+)")
RELOC_EXPR = re.compile(r"%(hi|lo)\(([A-Za-z_]\w*)(?:\s*\+\s*(0x[0-9A-Fa-f]+|\d+))?\)")


def parse_retail(path):
    """Returns (name, start_addr, [insn dicts])."""
    name = None
    start = None
    insns = []
    with open(path) as f:
        for line in f:
            m = GLABEL_LINE.match(line)
            if m and name is None:
                name = m.group(1)
                continue
            m = RETAIL_LINE.match(line)
            if not m:
                continue
            addr = int(m.group(2), 16)
            word = struct.unpack("<I", bytes.fromhex(m.group(3)))[0]
            mnem = m.group(4)
            ops = m.group(5).strip()
            if start is None:
                start = addr
            insns.append({"addr": addr, "word": word, "mnem": mnem, "ops": ops})
    for ins in insns:
        ins["key"] = retail_key(ins, start)
        ins["text"] = f"{ins['mnem']:<8} {ins['ops']}"
    return name, start, insns


def retail_key(ins, start):
    word = ins["word"]
    mnem = ins["mnem"]
    ops = ins["ops"]
    m = RELOC_EXPR.search(ops)
    if m:
        kind, sym, off = m.group(1), m.group(2), m.group(3)
        base = resolve(sym)
        if base is None:
            return f"{kind}|{word & 0xFFFF0000:08x}|{sym}+{off or 0}"
        addend = int(off, 0) if off else 0
        return f"{kind}|{word & 0xFFFF0000:08x}|{base + addend:08x}"
    if mnem == "jal":
        tgt = resolve(ops.strip())
        if tgt is None:
            return f"jal|{ops.strip()}"
        return f"jal|{tgt:08x}"
    if mnem == "j":
        lm = re.match(r"\.L([0-9A-Fa-f]+)", ops.strip())
        if lm:
            return f"j|+{int(lm.group(1), 16) - start:x}"
    return f"{word:08x}"


# ----------------------------------------------------------------- built side
OBJ_FUNC = re.compile(r"^([0-9a-f]+) <([^>]+)>:\s*$")
OBJ_INSN = re.compile(r"^\s*([0-9a-f]+):\s+([0-9a-f]{8})\s+(\S+)\s*(.*)$")
OBJ_RELOC = re.compile(r"^\s*([0-9a-f]+):\s+(R_MIPS_\w+)\s+(\S+)\s*$")


def parse_obj(path):
    out = subprocess.run(
        [OBJDUMP, "-dr", path], check=True, capture_output=True, text=True
    ).stdout
    funcs = {}
    order = []
    cur = None
    cur_start = 0
    for line in out.splitlines():
        m = OBJ_FUNC.match(line)
        if m:
            cur_start = int(m.group(1), 16)
            cur = m.group(2)
            funcs[cur] = {"start": cur_start, "insns": []}
            order.append(cur)
            continue
        if cur is None:
            continue
        m = OBJ_INSN.match(line)
        if m:
            off = int(m.group(1), 16)
            word = int(m.group(2), 16)
            funcs[cur]["insns"].append(
                {"off": off, "word": word, "mnem": m.group(3), "ops": m.group(4).strip(), "reloc": None}
            )
            continue
        m = OBJ_RELOC.match(line)
        if m and funcs[cur]["insns"]:
            ins = funcs[cur]["insns"][-1]
            if int(m.group(1), 16) == ins["off"]:
                ins["reloc"] = (m.group(2), m.group(3))
    for name in order:
        f = funcs[name]
        assign_built_keys(f)
    return funcs, order


def assign_built_keys(f):
    insns = f["insns"]
    start = f["start"]
    # Pair HI16 with the next LO16 of the same symbol to recover the addend.
    pending_hi = []
    for ins in insns:
        r = ins["reloc"]
        if r and r[0] == "R_MIPS_HI16":
            pending_hi.append(ins)
        elif r and r[0] == "R_MIPS_LO16":
            lo_imm = sext16(ins["word"])
            sym = r[1]
            base = resolve(sym.split("+")[0]) if not sym.startswith(".") else None
            # Consume all pending HI16 with this symbol.
            matched = [h for h in pending_hi if h["reloc"][1] == sym]
            for h in matched:
                hi_imm = h["word"] & 0xFFFF
                addend = (hi_imm << 16) + lo_imm
                h["resolved"] = (base + addend) if base is not None else None
                h["sym_add"] = (sym, addend)
                pending_hi.remove(h)
            ins["resolved"] = (base + lo_imm) if base is not None else None
            ins["sym_add"] = (sym, lo_imm)
    for ins in insns:
        ins["key"] = built_key(ins, start)
        rtxt = ""
        if ins["reloc"]:
            sa = ins.get("sym_add")
            rtxt = f"  ; {ins['reloc'][0][7:]} {sa[0]}+{sa[1]:#x}" if sa else f"  ; {ins['reloc'][0][7:]} {ins['reloc'][1]}"
        ins["text"] = f"{ins['mnem']:<8} {ins['ops']}{rtxt}"


def built_key(ins, start):
    word = ins["word"]
    r = ins["reloc"]
    if r:
        kind = r[0]
        if kind in ("R_MIPS_HI16", "R_MIPS_LO16"):
            k = "hi" if kind == "R_MIPS_HI16" else "lo"
            res = ins.get("resolved")
            if res is None:
                sa = ins.get("sym_add", (r[1], 0))
                return f"{k}|{word & 0xFFFF0000:08x}|{sa[0]}+{sa[1]}"
            return f"{k}|{word & 0xFFFF0000:08x}|{res:08x}"
        if kind == "R_MIPS_26":
            if ins["mnem"] == "jal":
                tgt = resolve(r[1])
                if tgt is None:
                    # local static callee: no retail counterpart, keep name
                    return f"jal|{r[1]}"
                return f"jal|{tgt:08x}"
            if ins["mnem"] == "j":
                # objdump prints absolute section offset as target
                m = re.match(r"0x([0-9a-f]+)|([0-9a-f]+)", ins["ops"])
                if m:
                    tgt = int(m.group(1) or m.group(2), 16)
                    return f"j|+{tgt - start:x}"
    if ins["mnem"] == "j":
        m = re.match(r"0x([0-9a-f]+)|([0-9a-f]+)", ins["ops"])
        if m:
            tgt = int(m.group(1) or m.group(2), 16)
            return f"j|+{tgt - start:x}"
    return f"{word:08x}"


# ---------------------------------------------------------------------- diff
def diff_function(rname, rstart, rins, bname, bfunc, context):
    bins = bfunc["insns"]
    rkeys = [i["key"] for i in rins]
    bkeys = [i["key"] for i in bins]
    sm = difflib.SequenceMatcher(a=rkeys, b=bkeys, autojunk=False)
    equal = sum(blk.size for blk in sm.get_matching_blocks())
    lines = []
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == "equal":
            if context >= 0 and (i2 - i1) > 2 * context + 1 and context > 0:
                for k in range(i1, i1 + context):
                    lines.append(fmt_pair(" ", rins[k], bins[j1 + (k - i1)]))
                lines.append(f"      ... {i2 - i1 - 2 * context} identical ...")
                for k in range(i2 - context, i2):
                    lines.append(fmt_pair(" ", rins[k], bins[j1 + (k - i1)]))
            elif context != 0:
                for k in range(i1, i2):
                    lines.append(fmt_pair(" ", rins[k], bins[j1 + (k - i1)]))
        else:
            for k in range(i1, i2):
                lines.append(fmt_pair("-", rins[k], None))
            for k in range(j1, j2):
                lines.append(fmt_pair("+", None, bins[k]))
    return equal, lines


def fmt_pair(mark, r, b):
    left = f"{r['addr']:08x} {r['text']}" if r else ""
    right = f"{b['off']:05x} {b['text']}" if b else ""
    return f"{mark} {left:<58} | {right}"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--obj", required=True)
    ap.add_argument("--asm-dir", required=True)
    ap.add_argument("--func", action="append", default=[])
    ap.add_argument("--context", type=int, default=3, help="-1 = full listing")
    ap.add_argument("--summary", action="store_true")
    args = ap.parse_args()

    funcs, order = parse_obj(args.obj)
    retail = {}
    for p in sorted(glob.glob(os.path.join(args.asm_dir, "*.s"))):
        name, start, insns = parse_retail(p)
        if name:
            retail[name] = (start, insns)

    rows = []
    total_r = total_b = total_eq = 0
    for name in order:
        b = funcs[name]
        if name not in retail:
            rows.append((name, 0, len(b["insns"]) * 4, 0, "no retail counterpart (static helper?)"))
            total_b += len(b["insns"]) * 4
            continue
        rstart, rins = retail[name]
        eq, _ = diff_function(name, rstart, rins, name, b, 0)
        rb, bb = len(rins) * 4, len(b["insns"]) * 4
        total_r += rb
        total_b += bb
        total_eq += eq * 4
        status = "EXACT" if (eq == len(rins) == len(b["insns"])) else ""
        rows.append((name, rb, bb, eq * 4, status))
    for name in retail:
        if name not in funcs:
            rstart, rins = retail[name]
            rows.append((name, len(rins) * 4, 0, 0, "NOT BUILT"))
            total_r += len(rins) * 4

    if args.summary or not args.func:
        rows.sort(key=lambda r: -(r[1] - r[2]))
        print(f"{'deficit':>8} {'retail':>7} {'built':>7} {'eqB':>7}  function")
        for name, rb, bb, eqb, st in rows:
            print(f"{rb - bb:>8} {rb:>7} {bb:>7} {eqb:>7}  {name} {st}")
        print(f"{total_r - total_b:>8} {total_r:>7} {total_b:>7} {total_eq:>7}  TOTAL")

    for fn in args.func:
        if fn not in funcs or fn not in retail:
            print(f"\n== {fn}: missing on one side", file=sys.stderr)
            continue
        rstart, rins = retail[fn]
        eq, lines = diff_function(fn, rstart, rins, fn, funcs[fn], args.context)
        print(f"\n== {fn}: retail {len(rins)} insns, built {len(funcs[fn]['insns'])} insns, {eq} aligned-equal")
        print("\n".join(lines))


if __name__ == "__main__":
    main()
