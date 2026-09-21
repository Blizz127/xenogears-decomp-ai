#!/usr/bin/env python3
"""Re-measure retail function sizes honouring the `.size` directive.

analyze.py's retail parser keys on `glabel` and accumulates instruction lines
until the NEXT glabel.  In the multi-function aggregate `.s` files spimdisasm
emits `.size <name>, . - <name>` and then inter-function `nop` padding before
the next label, so that padding was being charged to the preceding function
and inflating its retail size.

This re-measures with `.size` respected and reports every cohort member whose
retail size changes, so the deficits can be corrected.

Usage, inside the toolchain container from the project root:
    python3 docs/evidence/slus-codegen-triage-20260921/size_check.py
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, "..", "slus-byte-parity-20260921"))
import analyze  # noqa: E402

SIZE = re.compile(r"^\s*\.size\s+(\S+?)\s*,")
TSV = "docs/evidence/slus-byte-parity-20260921/short-functions.tsv"


def load_retail_sized():
    """name -> instruction count, stopping at the symbol's own .size"""
    out = {}
    for dirpath, _d, files in os.walk(analyze.ASMDIR):
        if os.sep + "data" in dirpath + os.sep:
            continue
        for fn in sorted(files):
            if not fn.endswith(".s"):
                continue
            path = os.path.join(dirpath, fn)
            cur, local, order, closed = None, {}, [], set()
            for line in open(path, errors="replace"):
                m = analyze.GLABEL.match(line.strip())
                if m:
                    cur = m.group(1)
                    if cur.startswith("D_"):
                        cur = None
                    else:
                        local.setdefault(cur, 0)
                        order.append(cur)
                    continue
                m = SIZE.match(line)
                if m:
                    closed.add(m.group(1))
                    if cur == m.group(1):
                        cur = None
                    continue
                if cur and analyze.ASM_LINE.match(line):
                    local[cur] += 1
            for n in order:
                if local[n] and n not in out:
                    out[n] = local[n]
    return out


def main():
    old = analyze.load_retail()
    new = load_retail_sized()
    print("== cohort retail sizes, .size-respecting vs original ==")
    print(f"{'function':<26}{'origRetail':>11}{'trueRetail':>11}"
          f"{'padB':>6}{'built':>7}{'trueDef':>9}")
    tot_pad = 0
    for line in open(TSV).read().splitlines()[1:]:
        f = line.split("\t")
        d, n, r, b = int(f[0]), f[1], int(f[2]), int(f[3])
        if not r or b / r >= 0.75:
            continue
        nr = new.get(n, 0) * 4
        pad = r - nr
        tot_pad += pad
        print(f"{n:<26}{r:>11}{nr:>11}{pad:>6}{b:>7}{nr-b:>9}")
    print()
    print(f"total padding wrongly charged to the cohort: {tot_pad} bytes")


if __name__ == "__main__":
    main()
