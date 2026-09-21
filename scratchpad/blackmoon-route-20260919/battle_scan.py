#!/usr/bin/env python3
"""Scan battle ids through the XENO_BATTLE_WARP_FILE launcher and capture one
frame of each, so a named fight (Id) can be found without playing to it.

The port's warp writes the same five field words the script VM's start-battle
opcode writes, so every id here boots through the retail path.  GOD mode is
expected to be ON: a level-7 party cannot survive a late-disc boss, and the
point of the scan is to SEE the formation, not to win it.

usage: battle_scan.py <log> <first> <last> [step]
"""
import os
import re
import subprocess
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
KEY = HERE / "key.py"
REQ = HERE / "warp.req"
SHOTS = HERE / "battle-scan"
WID = os.environ.get("XENO_WID", "")
ENV = dict(os.environ, DISPLAY=os.environ.get("XENO_DISPLAY", ":95"))
if "XENO_XAUTHORITY" in os.environ:
    ENV["XAUTHORITY"] = os.environ["XENO_XAUTHORITY"]

LOG = None


def text():
    try:
        return LOG.read_text(errors="replace")
    except OSError:
        return ""


def counts():
    t = text()
    return t.count("enter retail battle.bin"), t.count("retail battle returned")


def tap(k, hold=0.15):
    subprocess.run([sys.executable, str(KEY), k, str(hold)],
                   check=False, stdout=subprocess.DEVNULL,
                   stderr=subprocess.DEVNULL)


def shot(path):
    subprocess.run(["import", "-window", WID, str(path)], env=ENV, check=False)


def in_field(timeout=90):
    """Wait until every entered battle has returned."""
    end = time.time() + timeout
    while time.time() < end:
        a, b = counts()
        if a == b:
            return True
        # GOD mode makes one landed hit lethal, but only if an attack actually
        # lands.  Xenogears' attack ring spends AP on Cross (weak), Circle
        # (medium) and Triangle (strong) -- 'c', 'z', 'v' here.  The acceptance
        # driver deliberately omits Cross because it doubles as Cancel; a
        # scanner has no such scruples, and without it a low-AP turn could not
        # attack at all and formation 4 never ended.  'Right' nudges the ring
        # off Defense onto Attack.
        for k in ("z", "Right", "z", "c", "v", "z"):
            a, b = counts()
            if a == b:
                return True
            tap(k)
            time.sleep(1.0)
    return False


def main():
    global LOG
    LOG = Path(sys.argv[1]).resolve()
    first, last = int(sys.argv[2]), int(sys.argv[3])
    step = int(sys.argv[4]) if len(sys.argv) > 4 else 1
    SHOTS.mkdir(exist_ok=True)

    for bid in range(first, last + 1, step):
        if not in_field():
            print(f"id={bid}: could not get back to the field; stopping",
                  flush=True)
            return 1
        before, _ = counts()
        REQ.write_text(f"{bid}\n")
        # The warp polls every 30 frames and the battle overlay load is the
        # slow part; 45 s is generous even under llvmpipe.
        end = time.time() + 45
        started = False
        while time.time() < end:
            if counts()[0] > before:
                started = True
                break
            time.sleep(0.5)
        if not started:
            print(f"id={bid}: no battle started (guarded or invalid)",
                  flush=True)
            REQ.write_text("")
            continue
        time.sleep(8.0)          # let the formation finish appearing
        out = SHOTS / f"battle-{bid:03d}.png"
        shot(out)
        print(f"id={bid}: captured {out.name}", flush=True)

    in_field()
    return 0


if __name__ == "__main__":
    sys.exit(main())
