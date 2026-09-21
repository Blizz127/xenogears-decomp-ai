#!/usr/bin/env python3
"""Re-measure the cohort with static helpers attributed to their caller.

`nm` gives a static helper its own symbol, so a TU that factors retail's
inlined code into helpers shows a large fake deficit on the parent.  The field
report flagged this as a caveat; for slus it turns out to be the DOMINANT
effect in the worst offender.

For each cohort function this walks its local (`.text`-relative) call targets
transitively, and adds the size of any callee that has no retail counterpart
of its own -- i.e. a helper this project invented, which retail had inline.

Usage, inside the toolchain container from the project root:
    python3 docs/evidence/slus-codegen-triage-20260921/helper_aware.py
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, "..", "slus-byte-parity-20260921"))
import analyze  # noqa: E402
import strict   # noqa: E402

COHORT_TSV = "docs/evidence/slus-byte-parity-20260921/short-functions.tsv"


def main():
    retail = analyze.load_retail()

    # per-object: symbol -> words, and offset -> symbol for local resolution
    objs = {}
    for obj in strict.objects():
        fns = strict.load_obj(obj)
        tsyms = strict.text_symbols(obj)
        objs[obj] = (fns, tsyms)

    where = {}
    for obj, (fns, _t) in objs.items():
        for n in fns:
            where.setdefault(n, obj)

    def local_callees(obj, name):
        fns, tsyms = objs[obj]
        w, rels = fns[name]
        out = set()
        for i, word in enumerate(w):
            if (word >> 26) != 0x03:
                continue
            rel = rels.get(i)
            if not rel:
                continue
            _t, sym, addend = rel
            if sym == ".text":
                # R_MIPS_26 against a section symbol carries no addend in the
                # relocation: the target offset lives in the instruction's own
                # 26-bit field.  Reading `addend` here finds 0 every time and
                # silently reports "no helpers".
                off = (word & 0x03FFFFFF) << 2
                if off in tsyms:
                    out.add(tsyms[off])
            else:
                out.add(sym)
        return out

    def helper_bytes(obj, name, seen):
        """size of invented (non-retail) helpers reachable from `name`"""
        total, names = 0, []
        for c in local_callees(obj, name):
            if c in seen or c in retail:
                continue
            seen.add(c)
            o = where.get(c)
            if o is None or c not in objs[o][0]:
                continue
            sz = len(objs[o][0][c][0]) * 4
            total += sz
            names.append((c, sz))
            sub, subn = helper_bytes(o, c, seen)
            total += sub
            names += subn
        return total, names

    rows = []
    with open(COHORT_TSV) as fh:
        next(fh)
        for line in fh:
            f = line.rstrip("\n").split("\t")
            deficit, name, r, b = int(f[0]), f[1], int(f[2]), int(f[3])
            if r and b / r < 0.75:
                rows.append((deficit, name, r, b))

    out = []
    A = out.append
    A("== cohort re-measured with static helpers folded into their caller ==")
    A(f"{'function':<26}{'retail':>7}{'built':>7}{'helpers':>8}"
      f"{'eff':>7}{'realDef':>8}{'ratio':>7}  invented helpers")
    tot_raw = tot_real = 0
    for deficit, name, r, b in rows:
        obj = where.get(name)
        if obj is None:
            A(f"{name:<26}{r:>7}{b:>7}   (no built object)")
            continue
        hb, hn = helper_bytes(obj, name, set())
        eff = b + hb
        real = r - eff
        tot_raw += deficit
        tot_real += max(real, 0)
        A(f"{name:<26}{r:>7}{b:>7}{hb:>8}{eff:>7}{real:>8}"
          f"{100.0*eff/r:6.0f}%  "
          + ", ".join(f"{n}({s})" for n, s in hn))
    A("")
    A(f"cohort deficit as first reported : {tot_raw}")
    A(f"cohort deficit, helper-aware     : {tot_real}")
    txt = "\n".join(out)
    open(os.path.join(HERE, "helper-aware.txt"), "w").write(txt + "\n")
    print(txt)


if __name__ == "__main__":
    main()
