#!/usr/bin/env python3
"""Count unfilled delay-slot `nop`s, retail vs built, across the cohort.

Retail slus was assembled with load-delay nops left in place.  maspsx/gas in
this build schedules them away or fills the slot, so a function can be
hundreds of bytes shorter while executing exactly the same operations.  This
is the slus analogue of the base-address re-materialization that dominated
the field cohort.

Usage, inside the toolchain container from the project root:
    python3 docs/evidence/slus-codegen-triage-20260921/nop_delta.py
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, "..", "slus-byte-parity-20260921"))
sys.path.insert(0, HERE)
import strict   # noqa: E402
import triage   # noqa: E402

HELPERS = {"func_80034FFC": ["func_80034FFC_row0", "func_80034FFC_row1"]}
TSV = "docs/evidence/slus-byte-parity-20260921/short-functions.tsv"


def main():
    rasm = triage.retail_asm_lines()
    built = {}
    for obj in strict.objects():
        for n, (w, r) in strict.load_obj(obj).items():
            built.setdefault(n, (w, r))

    out = []
    A = out.append
    A("== unfilled delay-slot nops, retail vs built ==")
    A(f"{'function':<26}{'deficit':>8}{'rNOP':>6}{'bNOP':>6}"
      f"{'nopD':>6}{'nopBytes':>9}{'%ofDef':>8}")
    for line in open(TSV).read().splitlines()[1:]:
        f = line.split("\t")
        d, n, r, b = int(f[0]), f[1], int(f[2]), int(f[3])
        if not r or b / r >= 0.75:
            continue
        if n not in rasm or n not in built:
            continue
        rn = sum(1 for l in rasm[n] if triage.mnemonic(l) == "nop")
        grp = [n] + HELPERS.get(n, [])
        bn = sum(sum(1 for w in built[g][0] if w == 0)
                 for g in grp if g in built)
        # for the helper case the deficit is the helper-aware one
        eff_def = 52 if n == "func_80034FFC" else d
        nb = (rn - bn) * 4
        pct = (100.0 * nb / eff_def) if eff_def else 0
        A(f"{n:<26}{eff_def:>8}{rn:>6}{bn:>6}{rn-bn:>6}{nb:>9}{pct:>7.0f}%")
    txt = "\n".join(out)
    open(os.path.join(HERE, "nop-delta.txt"), "w").write(txt + "\n")
    print(txt)


if __name__ == "__main__":
    main()
