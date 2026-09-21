#!/usr/bin/env python3
"""Turn a map_survey.sh run into a ranked fidelity defect list.

The survey records, per map: whether the field loaded, how many actors/models
it built, and any fail-loud event (assert, unimplemented sprite opcode,
unresolved bridge call, stub, memory fault).  This adds the one thing the shell
could not judge cheaply -- whether the frame actually rendered anything.

Render check: a PNG of a flat frame compresses to almost nothing, so file size
is a reliable first-pass signal (map 25's capture was 1.4 KB of black while a
real field is 40-160 KB).  Anything under the threshold is reported as BLANK
for a human to confirm from the shot, not asserted as broken.

usage: analyze_survey.py <surveydir>
"""
import collections
import pathlib
import sys

BLANK_BYTES = 8000


def main():
    root = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else "survey")
    rows = [l.rstrip("\n").split("\t")
            for l in (root / "results.tsv").read_text().splitlines()[1:]]
    shots = root / "shots"

    status = collections.Counter()
    blanks, defects = [], collections.defaultdict(list)
    loaded_small = []

    for r in rows:
        if len(r) < 4:
            continue
        m, st, actors, dfx = r[0], r[1], r[2], r[3]
        status[st] += 1
        if dfx and dfx != "-":
            for d in dfx.strip(",").split(","):
                if d:
                    defects[d].append(m)
        if st == "LOADED":
            p = shots / f"map-{int(m):04d}.png"
            if p.exists() and p.stat().st_size < BLANK_BYTES:
                blanks.append((m, p.stat().st_size))
            if actors and actors != "-":
                try:
                    built = int(actors.split("count=")[1].split(" ")[0])
                    if built == 0:
                        loaded_small.append(m)
                except (IndexError, ValueError):
                    pass

    print(f"maps surveyed: {len(rows)}")
    for k, v in status.most_common():
        print(f"  {k:8s} {v}")

    print(f"\nrendered blank (<{BLANK_BYTES}B png), needs eyes: {len(blanks)}")
    for m, sz in blanks[:40]:
        print(f"  map {m:>4s}  {sz}B")
    if len(blanks) > 40:
        print(f"  ... and {len(blanks)-40} more")

    if loaded_small:
        print(f"\nloaded but built 0 models: {' '.join(loaded_small[:40])}")

    print("\ndefects by frequency (maps affected):")
    for d, maps in sorted(defects.items(), key=lambda kv: -len(kv[1])):
        head = " ".join(maps[:12]) + (" ..." if len(maps) > 12 else "")
        print(f"  [{len(maps):>3}] {d}")
        print(f"        {head}")

    dead = [r[0] for r in rows if len(r) > 1 and r[1] == "DIED"]
    if dead:
        print(f"\nmaps that killed the process ({len(dead)}): {' '.join(dead)}")
        # The shell only records that *an* assert fired.  The exact condition is
        # what makes a defect actionable and is what groups maps into one fix,
        # so recover it from each saved crash log.
        causes = collections.defaultdict(list)
        for m in dead:
            p = root / f"died-{m}.log"
            if not p.exists():
                causes["<no log>"].append(m)
                continue
            txt = p.read_text(errors="replace").splitlines()
            hit = None
            for line in reversed(txt[-40:]):
                for key in ("Assertion", "fault at", "sprite_animation_unimpl"):
                    if key in line:
                        hit = line.strip()[:150]
                        break
                if hit:
                    break
            causes[hit or "silent exit (no diagnostic)"].append(m)
        print("\ncrash causes, most maps first:")
        for c, maps in sorted(causes.items(), key=lambda kv: -len(kv[1])):
            print(f"  [{len(maps):>3}] {c}")
            print(f"        maps: {' '.join(maps[:16])}"
                  + (" ..." if len(maps) > 16 else ""))
    noload = [r[0] for r in rows if len(r) > 1 and r[1] == "NOLOAD"]
    if noload:
        print(f"\nmaps that never loaded: {' '.join(noload)}")


if __name__ == "__main__":
    main()
