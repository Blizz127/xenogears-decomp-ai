#!/usr/bin/env python3
"""Pairwise SPU register-page comparison for the field14 slice control.

  field14_spu_pairs.py OUT.json RUN_DIR...

Each RUN_DIR holds the spu.log written by run_field14_step_slice.sh
(XENO_FIELD_STEP_SPU_LOG: vblank-index, step, FNV of the 0x200-byte SPU
register page).  For every pair of runs: identical per-vblank stream?, first
per-vblank divergence, identical sequence of DISTINCT states?, its first
divergence, and a match ratio over the first 4000 distinct states.

Interpretation (coordinator's control): if two runs of the SAME mode diverge,
the SPU stream depends on host real time (sound-tick nondeterminism), not on
native vs interpreted execution; if same-mode pairs match and only
cross-mode pairs diverge, the native path changes the sound driver's input.
"""
import difflib
import itertools
import json
import sys
from pathlib import Path


# Compare a fixed game-time window, not run length: runs end at a host
# timeout at slightly different points, so trailing rows past the last
# checkpoint step are not evidence of anything.
WINDOW_LAST_STEP = 1000


def load(run):
    rows = [l.split() for l in Path(run, 'spu.log').read_text().splitlines()]
    rows = [r for r in rows if int(r[1]) <= WINDOW_LAST_STEP]
    distinct, prev = [], None
    for row in rows:
        if row[2] != prev:
            distinct.append(row[2])
            prev = row[2]
    return [r[2] for r in rows], distinct


def first_diff(a, b):
    n = min(len(a), len(b))
    return next((i for i in range(n) if a[i] != b[i]), None if len(a) == len(b) else n)


def main():
    out, runs = sys.argv[1], sys.argv[2:]
    data = {r: load(r) for r in runs}
    pairs = []
    for a, b in itertools.combinations(runs, 2):
        (va, da), (vb, db) = data[a], data[b]
        ratio = difflib.SequenceMatcher(None, da[:4000], db[:4000], autojunk=False).ratio()
        row = {'a': Path(a).name, 'b': Path(b).name,
               'vblank_identical': va == vb, 'vblank_first_divergence': first_diff(va, vb),
               'distinct_identical': da == db, 'distinct_first_divergence': first_diff(da, db),
               'distinct_lengths': [len(da), len(db)], 'ratio_first_4000': round(ratio, 4)}
        pairs.append(row)
        print('%-10s vs %-10s vblank_identical=%s distinct_identical=%s first_distinct_div=%s ratio=%.4f'
              % (row['a'], row['b'], row['vblank_identical'], row['distinct_identical'],
                 row['distinct_first_divergence'], ratio))
    Path(out).write_text(json.dumps({'pairs': pairs}, indent=2) + '\n')


if __name__ == '__main__':
    main()
