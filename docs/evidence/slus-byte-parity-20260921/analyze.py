#!/usr/bin/env python3
"""slus_006.64 byte-parity analysis.

Compares every built function in build/out/slus_006.64.elf against the retail
instruction words recorded in the splat-generated asm/slus_006.64/**/*.s, with
relocation-bearing immediate fields masked out.

Run inside the toolchain container, from the project root:
    python3 docs/evidence/slus-byte-parity-20260921/analyze.py
Writes its tables next to itself.
"""
import os
import re
import subprocess
import sys
from collections import defaultdict

ROOT = os.getcwd()
OUT = os.path.join(ROOT, "docs/evidence/slus-byte-parity-20260921")
ELF = "build/out/slus_006.64.elf"
ASMDIR = "asm/slus_006.64"
SRCDIR = "src/slus_006.64"

# ---------------------------------------------------------------- retail side

# "    /* 15FF0 800257F0 60FFBD27 */  addiu ..."  -> word stored little-endian
ASM_LINE = re.compile(r"^\s*/\* [0-9A-Fa-f]+ ([0-9A-Fa-f]{8}) ([0-9A-Fa-f]{8}) \*/")
GLABEL = re.compile(r"^(?:glabel|jlabel|dlabel)\s+(\S+)")


def le_bytes_to_word(h):
    """'60FFBD27' is the four bytes in file order; the instruction is LE."""
    b = bytes.fromhex(h)
    return int.from_bytes(b, "little")


def load_retail():
    """name -> (vram, [instruction words], source .s path)

    A function can appear twice: once in the per-function
    asm/slus_006.64/matchings/<tu>/<name>.s and once in the whole-TU
    asm/slus_006.64/<tu>.s.  Both carry the same retail words, so the first
    file wins and the duplicate is discarded (appending both would silently
    double every length).
    """
    funcs = {}
    for dirpath, _dirs, files in os.walk(ASMDIR):
        if os.sep + "data" in dirpath + os.sep:
            continue
        for fn in sorted(files):
            if not fn.endswith(".s"):
                continue
            path = os.path.join(dirpath, fn)
            local = {}
            order = []
            cur = None
            with open(path, errors="replace") as fh:
                for line in fh:
                    m = GLABEL.match(line.strip())
                    if m:
                        cur = m.group(1)
                        if cur.startswith("D_"):
                            cur = None
                        else:
                            local.setdefault(cur, [None, [], {}, {}])
                            order.append(cur)
                        continue
                    m = ASM_LINE.match(line)
                    if m and cur:
                        vram = int(m.group(1), 16)
                        rec = local[cur]
                        if rec[0] is None:
                            rec[0] = vram
                        word = le_bytes_to_word(m.group(2))
                        idx = len(rec[1])
                        rec[1].append(word)
                        op = word >> 26
                        if op == 0x03:                       # jal <name>
                            mm = re.search(r"\bjal\s+(\S+)", line)
                            if mm:
                                rec[2][idx] = mm.group(1)
                        elif op == 0x02:                     # j -> offset in func
                            tgt = ((vram & 0xF0000000) | ((word & 0x03FFFFFF) << 2))
                            rec[3][idx] = tgt - rec[0]
            for name in order:
                vram, words, jals, rels = local[name]
                if words and name not in funcs:
                    funcs[name] = (vram, words, path, jals, rels)
    return funcs


# ----------------------------------------------------------------- built side

OBJD_SYM = re.compile(r"^([0-9a-f]{8}) <([^>]+)>:")
OBJD_INS = re.compile(r"^\s*([0-9a-f]+):\s+([0-9a-f]{8})\s")


def load_built():
    txt = subprocess.run(
        ["mips-linux-gnu-objdump", "-d", "--section=.main", ELF],
        capture_output=True, text=True, check=True).stdout
    funcs = {}
    cur = None
    for line in txt.splitlines():
        m = OBJD_SYM.match(line)
        if m:
            cur = m.group(2)
            funcs[cur] = [int(m.group(1), 16), [], {}, {}]
            continue
        m = OBJD_INS.match(line)
        if m and cur:
            rec = funcs[cur]
            word = int(m.group(2), 16)
            idx = len(rec[1])
            rec[1].append(word)
            op = word >> 26
            if op == 0x03:                                   # jal ... <name>
                mm = re.search(r"<([^>+]+)(?:\+0x[0-9a-f]+)?>", line)
                if mm:
                    rec[2][idx] = mm.group(1)
            elif op == 0x02:
                vram = int(m.group(1), 16)
                tgt = ((vram & 0xF0000000) | ((word & 0x03FFFFFF) << 2))
                rec[3][idx] = tgt - rec[0]
    return {k: tuple(v) for k, v in funcs.items() if v[1]}


# -------------------------------------------------------- relocation masking

# opcodes whose low 16 bits carry an R_MIPS_HI16/LO16/GPREL16 immediate
IMM16_OPS = {
    0x08, 0x09, 0x0C, 0x0D, 0x0E, 0x0F,              # addi addiu andi ori xori lui
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,        # lb lh lwl lw lbu lhu lwr
    0x28, 0x29, 0x2A, 0x2B, 0x2E,                    # sb sh swl sw swr
    0x31, 0x32, 0x33, 0x35, 0x36, 0x37,              # lwc1 lwc2 lwc3 ldc1 ldc2 ldc3
    0x39, 0x3A, 0x3B, 0x3D, 0x3E, 0x3F,              # swc1 swc2 swc3 sdc1 sdc2 sdc3
}
JUMP_OPS = {0x02, 0x03}                              # j jal


def same_modulo_reloc(a, b, rjal, bjal, rrel, brel):
    """Compare one instruction word.

    Immediates that carry a relocation are masked, because the link assigns
    them and they legitimately differ between the retail image and ours.
    Jumps are NOT blanket-masked: `jal` must reach the same *symbol*, and a
    local `j` must reach the same offset within the function.  Masking those
    outright would score a call to the wrong function as byte-exact.
    """
    op_a, op_b = a >> 26, b >> 26
    if op_a != op_b:
        return False
    if op_a == 0x03:                                  # jal -> same callee name
        return rjal is not None and rjal == bjal
    if op_a == 0x02:                                  # j -> same in-function offset
        return rrel is not None and rrel == brel
    if a == b:
        return True
    if op_a in IMM16_OPS:
        return (a >> 16) == (b >> 16)                 # only the immediate differs
    return False


def compare(rw, bw, rjal, bjal, rrel, brel):
    """-> (verdict, n_words_differing_after_mask)"""
    if len(rw) != len(bw):
        return "size", None
    bad = 0
    for i in range(len(rw)):
        if not same_modulo_reloc(rw[i], bw[i], rjal.get(i), bjal.get(i),
                                 rrel.get(i), brel.get(i)):
            bad += 1
    return ("exact" if bad == 0 else "codegen"), bad


# ------------------------------------------------------------ INCLUDE_ASM set

INC = re.compile(r'^\s*INCLUDE_ASM\("([^"]+)",\s*(\w+)\)')


def load_include_asm():
    names, live_lines = set(), 0
    for dirpath, _dirs, files in os.walk(SRCDIR):
        for fn in files:
            if not (fn.endswith(".c") or fn.endswith(".h")):
                continue
            path = os.path.join(dirpath, fn)
            in_block = False
            with open(path, errors="replace") as fh:
                for raw in fh:
                    line = raw
                    # strip /* ... */ block comments spanning lines
                    if in_block:
                        if "*/" in line:
                            line = line.split("*/", 1)[1]
                            in_block = False
                        else:
                            continue
                    while "/*" in line:
                        pre, rest = line.split("/*", 1)
                        if "*/" in rest:
                            line = pre + rest.split("*/", 1)[1]
                        else:
                            line = pre
                            in_block = True
                            break
                    if line.lstrip().startswith("//"):
                        continue
                    m = INC.match(line)
                    if m:
                        names.add(m.group(2))
                        live_lines += 1
    return names, live_lines


# ------------------------------------------------------------------ TU tables

MAP = "build/out/slus_006.64.map"


def load_map_text():
    """object path -> bytes of .text contributed"""
    sizes = defaultdict(int)
    cur_out = None
    with open(MAP, errors="replace") as fh:
        lines = fh.read().splitlines()
    i = 0
    while i < len(lines):
        ln = lines[i]
        m = re.match(r"^ (\.\w[\w.]*)", ln)
        if m:
            cur_out = m.group(1)
        m = re.match(r"^ (\.text[\w.$]*)\s*$", ln)
        if m and i + 1 < len(lines):
            nxt = lines[i + 1]
            m2 = re.match(r"^\s+0x([0-9a-f]+)\s+0x([0-9a-f]+)\s+(\S+)", nxt)
            if m2:
                sizes[m2.group(3)] += int(m2.group(2), 16)
                i += 2
                continue
        m2 = re.match(r"^ (\.text[\w.$]*)\s+0x([0-9a-f]+)\s+0x([0-9a-f]+)\s+(\S+)", ln)
        if m2:
            sizes[m2.group(4)] += int(m2.group(3), 16)
        i += 1
    return sizes


CODE_SUB = re.compile(r"^\s*- \[0x([0-9A-Fa-f]+), (c|asm|hasm)(?:, ([\w/.]+))?\]")


def load_yaml_tus():
    """ordered [(offset, kind, name)] for the slus code subsegments"""
    rows = []
    with open("config/slus_006.64.yaml", errors="replace") as fh:
        for line in fh:
            m = CODE_SUB.match(line)
            if m:
                rows.append((int(m.group(1), 16), m.group(2), m.group(3)))
            elif re.match(r"^\s*- \[0x([0-9A-Fa-f]+), (data|rodata|sdata|bss|sbss)", line):
                rows.append((int(re.match(r"^\s*- \[0x([0-9A-Fa-f]+)", line).group(1), 16),
                             "end", None))
    return rows


def main():
    retail = load_retail()
    built = load_built()
    inc_names, inc_lines = load_include_asm()

    common = sorted(set(retail) & set(built))
    results = {}
    for name in common:
        rv, rw, rsrc, rjal, rrel = retail[name]
        bv, bw, bjal, brel = built[name]
        verdict, bad = compare(rw, bw, rjal, bjal, rrel, brel)
        results[name] = dict(retail=len(rw) * 4, built=len(bw) * 4,
                             verdict=verdict, bad=bad,
                             inc=name in inc_names, bvram=bv, src=rsrc)

    # ---- headline counts
    n_exact = sum(1 for r in results.values() if r["verdict"] == "exact")
    n_exact_c = sum(1 for r in results.values()
                    if r["verdict"] == "exact" and not r["inc"])
    n_size = sum(1 for r in results.values() if r["verdict"] == "size")
    n_cg = sum(1 for r in results.values() if r["verdict"] == "codegen")
    tot_bytes = sum(r["retail"] for r in results.values())
    ex_bytes = sum(r["retail"] for r in results.values() if r["verdict"] == "exact")
    exc_bytes = sum(r["retail"] for r in results.values()
                    if r["verdict"] == "exact" and not r["inc"])

    lines = []
    A = lines.append
    A("== function-level parity (relocations masked) ==")
    A(f"retail functions parsed from asm/: {len(retail)}")
    A(f"built .text symbols:               {len(built)}")
    A(f"comparable (present in both):      {len(results)}")
    A("")
    A(f"byte-exact:                {n_exact:5d}  ({ex_bytes} retail bytes, "
      f"{100.0*ex_bytes/tot_bytes:.2f}% byte-weighted)")
    A(f"  of which INCLUDE_ASM:    {n_exact - n_exact_c:5d}  (retail asm pasted back in)")
    A(f"  genuinely from C:        {n_exact_c:5d}  ({exc_bytes} retail bytes, "
      f"{100.0*exc_bytes/tot_bytes:.2f}% byte-weighted)")
    A(f"different size:            {n_size:5d}")
    A(f"same size, different code: {n_cg:5d}")
    A(f"comparable retail bytes:   {tot_bytes}")
    A("")
    A(f"live INCLUDE_ASM directives in {SRCDIR} (comments excluded): {inc_lines}")
    A(f"distinct INCLUDE_ASM function names:                        {len(inc_names)}")
    A("")

    # ---- per-TU deltas, from the yaml subsegment table vs the linker map
    rows = load_yaml_tus()
    mapsz = load_map_text()
    A("== per-TU .text size, retail (yaml span) vs built (linker map) ==")
    A(f"{'delta':>7}  {'retail':>7}  {'built':>7}  unit")
    tu_rows = []
    for i, (off, kind, name) in enumerate(rows):
        if kind not in ("c", "hasm", "asm") or name is None:
            continue
        nxt = rows[i + 1][0] if i + 1 < len(rows) else None
        if nxt is None:
            continue
        rsize = nxt - off
        if kind == "c":
            obj = f"build/src/slus_006.64/{name}.c.o"
        else:
            obj = f"build/asm/slus_006.64/{name}.s.o"
        bsize = mapsz.get(obj, 0)
        tu_rows.append((bsize - rsize, name, rsize, bsize, kind))
    tu_rows.sort()
    for d, name, r, b, kind in tu_rows:
        if d:
            A(f"{d:>7}  {r:>7}  {b:>7}  {name} ({kind})")
    A("")
    A(f"sum of per-TU deltas: {sum(d for d, *_ in tu_rows)}")
    A("")

    # ---- attribute every retail function to its TU by file offset
    # .main VMA 0x80010000 sits at file offset 0x800, so
    #   fileoff = vram - 0x80010000 + 0x800
    spans = []
    for i, (off, kind, name) in enumerate(rows):
        if kind == "end" or name is None:
            continue
        nxt = rows[i + 1][0] if i + 1 < len(rows) else None
        if nxt is not None:
            spans.append((off, nxt, kind, name))

    def tu_of(vram):
        fo = vram - 0x80010000 + 0x800
        for s, e, _k, n in spans:
            if s <= fo < e:
                return n
        return None

    tu_members = defaultdict(list)
    for n, r in results.items():
        t = tu_of(retail[n][0])
        if t:
            tu_members[t].append(n)

    def tu_report(tu):
        obj = f"build/src/slus_006.64/{tu}.c.o"
        members = tu_members.get(tu, [])
        rb = sum(results[n]["retail"] for n in members)
        ex = [n for n in members if results[n]["verdict"] == "exact"]
        cg = [n for n in members if results[n]["verdict"] == "codegen"]
        sz = [n for n in members if results[n]["verdict"] == "size"]
        exb = sum(results[n]["retail"] for n in ex)
        A(f"{tu}: {len(members)} comparable functions, {rb} retail bytes")
        A(f"    exact {len(ex)} ({exb} B) | same-size-different {len(cg)} "
          f"| different-size {len(sz)}")
        A(f"    built .text {mapsz.get(obj, 0)}")
        return ex, cg, sz, rb, exb

    A("== blast radius: the two TUs removed from gears.toml SdataSmall ==")
    blast_cg = 0
    for tu in ("system/temp1", "system/animation_scripts"):
        ex, cg, sz, rb, exb = tu_report(tu)
        blast_cg += len(cg) + len(sz)
    A("")

    A("== per-TU parity rollup (all code TUs) ==")
    A(f"{'exact':>5} {'cg':>5} {'size':>5}  {'retailB':>8} {'exactB':>8}  unit")
    roll = []
    for tu, members in tu_members.items():
        ex = sum(1 for n in members if results[n]["verdict"] == "exact")
        cg = sum(1 for n in members if results[n]["verdict"] == "codegen")
        sz = sum(1 for n in members if results[n]["verdict"] == "size")
        rb = sum(results[n]["retail"] for n in members)
        exb = sum(results[n]["retail"] for n in members
                  if results[n]["verdict"] == "exact")
        roll.append((rb - exb, tu, ex, cg, sz, rb, exb))
    roll.sort(reverse=True)
    for _gap, tu, ex, cg, sz, rb, exb in roll:
        A(f"{ex:>5} {cg:>5} {sz:>5}  {rb:>8} {exb:>8}  {tu}")
    A("")

    # ---- shift vs codegen: is the image merely relocated?
    A("== placement: built address vs retail address, per function ==")
    deltas = defaultdict(int)
    for n, r in results.items():
        deltas[r["bvram"] - retail[n][0]] += 1
    at_zero = deltas.get(0, 0)
    A(f"functions at their retail address: {at_zero} of {len(results)} "
      f"({100.0*at_zero/len(results):.1f}%)")
    A(f"distinct shift values: {len(deltas)}")
    A("most common shifts (bytes, count):")
    for d, c in sorted(deltas.items(), key=lambda kv: -kv[1])[:12]:
        A(f"    {d:>+8}  {c}")
    A("")

    # ---- where the differing bytes sit in the file
    A("== differing-byte density vs disc/SLUS_006.64, 16 KiB buckets ==")
    with open("disc/SLUS_006.64", "rb") as fh:
        ret = fh.read()
    with open("build/out/slus_006.64", "rb") as fh:
        got = fh.read()
    n = min(len(ret), len(got))
    BUCKET = 16384
    buckets = defaultdict(int)
    total_diff = 0
    for i in range(n):
        if ret[i] != got[i]:
            buckets[i // BUCKET] += 1
            total_diff += 1
    A(f"compared {n} bytes (retail {len(ret)}, built {len(got)})")
    A(f"differing: {total_diff}")
    A(f"{'offset':>10} {'diff':>6} {'pct':>6}")
    for b in sorted(buckets):
        cnt = buckets[b]
        size = min(BUCKET, n - b * BUCKET)
        A(f"  0x{b*BUCKET:07X} {cnt:>6} {100.0*cnt/size:5.1f}%")
    A("")

    # ---- section accounting for the -388 byte file deficit
    A("== section accounting ==")
    secs = subprocess.run(["mips-linux-gnu-objdump", "-h", ELF],
                          capture_output=True, text=True, check=True).stdout
    for line in secs.splitlines():
        m = re.match(r"\s+\d+ (\S+)\s+([0-9a-f]{8})\s+([0-9a-f]{8})", line)
        if m:
            A(f"  {m.group(1):<14} size 0x{m.group(2)} "
              f"({int(m.group(2),16)})  vma 0x{m.group(3)}")
    A(f"  built file {len(got)}  retail file {len(ret)}  "
      f"delta {len(got)-len(ret)}")
    A("")

    # ---- short-function cohort (missing behaviour, not regalloc)
    short = [(r["retail"] - r["built"], n, r["retail"], r["built"])
             for n, r in results.items()
             if r["verdict"] == "size" and r["retail"] > r["built"]]
    long_ = [(r["built"] - r["retail"], n, r["retail"], r["built"])
             for n, r in results.items()
             if r["verdict"] == "size" and r["built"] > r["retail"]]
    short.sort(reverse=True)
    long_.sort(reverse=True)
    A("== size-mismatched functions ==")
    A(f"short of retail: {len(short)} functions, {sum(d for d, *_ in short)} bytes missing")
    A(f"longer than retail: {len(long_)} functions, {sum(d for d, *_ in long_)} bytes over")
    A("")

    with open(os.path.join(OUT, "summary.txt"), "w") as fh:
        fh.write("\n".join(lines) + "\n")

    def tsv(path, rows_, header):
        with open(os.path.join(OUT, path), "w") as fh:
            fh.write(header + "\n")
            for d, n, r, b in rows_:
                inc = "INCLUDE_ASM" if results[n]["inc"] else "C"
                fh.write(f"{d}\t{n}\t{r}\t{b}\t{inc}\n")

    tsv("short-functions.tsv", short, "deficit\tsymbol\tretail\tbuilt\tsource")
    tsv("oversized-functions.tsv", long_, "surplus\tsymbol\tretail\tbuilt\tsource")

    with open(os.path.join(OUT, "per-function.tsv"), "w") as fh:
        fh.write("symbol\tverdict\tretail\tbuilt\tbad_words\tsource\n")
        for n in sorted(results):
            r = results[n]
            fh.write(f"{n}\t{r['verdict']}\t{r['retail']}\t{r['built']}\t"
                     f"{r['bad'] if r['bad'] is not None else ''}\t"
                     f"{'INCLUDE_ASM' if r['inc'] else 'C'}\n")

    print("\n".join(lines))


if __name__ == "__main__":
    sys.exit(main())
