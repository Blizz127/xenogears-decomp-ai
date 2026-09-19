#!/usr/bin/env python3
"""Decide, per warp-probe point, whether the player can move there.

Splits a run log into WARPPROBE segments and asks of each: with the control
lock down (`canRun=1`) and a direction held, did the position ever change?

  moved      -> there IS walkable surface at that point
  pinned     -> the player was placed somewhere with no surface under him
  no-input   -> inconclusive; the driver never got a gated direction in

That distinction is what separates "the post-battle spawn is wrong" (surface
exists elsewhere, the spawn is just in the wrong pocket) from "the room's
walkable geometry is short" (no surface exists outside the strip at all).

    usage: analyze_geom_probe.py <run.log>
"""
import re
import sys
from pathlib import Path

POS = re.compile(
    r"POSDIAG map=(\d+) pos=\((-?\d+),(-?\d+),(-?\d+)\).*?"
    r"held=0x([0-9a-f]+) newpress=0x[0-9a-f]+ canRun=(-?\d+)")
WARP = re.compile(r"WARPPROBE point (\d+)/(\d+) -> \((-?\d+),(-?\d+)\)")

text = Path(sys.argv[1]).read_text(errors="replace")

# Walk the log in order, tagging POSDIAG samples with the active probe point.
segments = []   # (label, target, [samples])
current = None
for line in text.splitlines():
    w = WARP.search(line)
    if w:
        idx, total, x, z = w.groups()
        current = (f"{idx}/{total}", (int(x), int(z)), [])
        segments.append(current)
        continue
    m = POS.search(line)
    if m and current is not None:
        mp, x, _y, z, held, can = m.groups()
        if mp == "14":
            current[2].append((int(x), int(z), int(held, 16), int(can)))

if not segments:
    print("no WARPPROBE segments in this log")
    raise SystemExit(1)

print(f"{'point':<7} {'target':>14} {'n':>4} {'gated-dir':>10} "
      f"{'positions':>10}  verdict")
for label, target, samples in segments:
    # Samples where the lock was down AND a direction was actually held.
    gated = [s for s in samples if s[3] == 1 and (s[2] & 0xF000)]
    positions = {(s[0], s[1]) for s in samples}
    if not samples:
        verdict = "no-samples"
    elif not gated:
        verdict = "no-input (inconclusive)"
    elif len(positions) > 1:
        verdict = "MOVED -> surface exists here"
    else:
        verdict = "PINNED -> no surface here"
    print(f"{label:<7} {str(target):>14} {len(samples):>4} {len(gated):>10} "
          f"{len(positions):>10}  {verdict}")
    if positions and len(positions) <= 6:
        print(f"{'':>7} {'':>14} seen: {sorted(positions)}")
