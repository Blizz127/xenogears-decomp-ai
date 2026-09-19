#!/usr/bin/env python3
"""Characterise field-14 player movement (2026-09-19).

Boots to the painting room, waits until the player actor parks on opcode 0x0C
(free control), then holds each of the eight directions for a fixed time, with
and without Cross (the run button), and reports the displacement each produced.

Why: the recorded 2026-09-08 route moves the player with DIAGONAL strides
(`Up+Left 4.8s` etc.) and replaying it never leaves the room, while single-key
strides in an earlier probe clearly did move him.  That is three different
hypotheses -- diagonals not reaching the pad, walk speed far too low, or walls
-- and they are only separable by measuring each direction under identical
conditions, twice, from a known-free state.

    usage: field14_input_char.py <tag> <display> [hold-seconds]

Ordinary keys only, no game-state writes.
"""
import os
import re
import signal
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path("/var/home/blizz/Projects/xenogears-decomp-ai")
TAG, DISP = sys.argv[1], sys.argv[2]
HOLD = float(sys.argv[3]) if len(sys.argv) > 3 else 3.0
SCHEDULE = ROOT / "scratchpad/lahan-natural-visible.A56D5m/schedule.txt"
OUT = Path("/var/tmp/xeno-leaf-push/inputchar-" + TAG)
OUT.mkdir(parents=True, exist_ok=True)
log = open(OUT / "run.log", "w")

xvfb = subprocess.Popen(["Xvfb", f":{DISP}", "-screen", "0", "1280x960x24", "-nolisten", "tcp"],
                        stdout=open(OUT / "xvfb.log", "w"), stderr=subprocess.STDOUT)
time.sleep(2)
env = {k: v for k, v in os.environ.items() if not k.startswith("XENO_")}
env.update(DISPLAY=f":{DISP}", SDL_VIDEODRIVER="x11", XENO_KERNEL_SEL="0",
           XENO_PAD_TEST_INPUT=SCHEDULE.read_text().strip(),
           XENO_FIELD_POS_DIAG="10",
           XENO_PAD_TEST_STOP_FIELD="14",
           # The free-control check below looks for the player actor parked on
           # opcode 0x0C, which only appears in the log if the VM tracer is on.
           # Without these the check silently reports False for every run and
           # the trials run against a possibly still-locked player.
           XENO_VM_TRACE="1", XENO_VM_TRACE_REPEAT="1",
           XENO_VM_TRACE_FIELD="14", XENO_VM_TRACE_MAX="40000")
game = subprocess.Popen(["stdbuf", "-oL", "-eL", str(ROOT / "pc_port/build_native/xeno-port")],
                        cwd=ROOT, env=env, stdout=log, stderr=subprocess.STDOUT)
wid = None
POS_RE = re.compile(r"POSDIAG map=(\d+) pos=\((-?\d+),(-?\d+),(-?\d+)\)")


def say(m):
    line = f"{time.strftime('%H:%M:%S')} {m}"
    print(line, flush=True)
    with open(OUT / "driver.log", "a") as fh:
        fh.write(line + "\n")


def text():
    return (OUT / "run.log").read_text(errors="replace")


def xdo(*a):
    subprocess.run(["xdotool", *a], env=env, stdout=subprocess.DEVNULL,
                   stderr=subprocess.DEVNULL, check=False)


def pos():
    h = POS_RE.findall(text())
    return (int(h[-1][1]), int(h[-1][3])) if h else None


try:
    say("boot; waiting for field 14")
    start = time.monotonic()
    while game.poll() is None and time.monotonic() - start < 900:
        if wid is None:
            p = subprocess.run(["xdotool", "search", "--onlyvisible", "--name", "Xenogears"],
                               env=env, capture_output=True, text=True)
            if p.returncode == 0 and p.stdout.split():
                wid = p.stdout.splitlines()[-1]
            else:
                time.sleep(0.5)
                continue
        t = text()
        if re.search(r"FieldLoad begin field=14\b", t):
            break
        if t.count("enter retail battle") > t.count("retail battle returned"):
            for k in ("z", "c"):
                xdo("windowfocus", "--sync", wid)
                xdo("key", k)
                time.sleep(0.35)
        else:
            time.sleep(0.5)
    else:
        say("never reached field 14")
        raise SystemExit

    say("clearing the post-battle scene with Circle for 60 s")
    t0 = time.monotonic()
    while time.monotonic() - t0 < 60 and game.poll() is None:
        xdo("windowfocus", "--sync", wid)
        xdo("key", "z")
        time.sleep(1.0)
    # Keep confirming until the player actor actually parks on the free-control
    # opcode; report honestly if it never does rather than measuring a locked
    # player and calling the zeros "movement data".
    free = False
    t0 = time.monotonic()
    while time.monotonic() - t0 < 150 and game.poll() is None:
        if re.search(r"actor=1 ip=\d+ op=0C", text()):
            free = True
            break
        xdo("windowfocus", "--sync", wid)
        xdo("key", "z")
        time.sleep(1.0)
    say(f"free-control opcode (0x0C) seen: {free} after {time.monotonic() - t0:.0f}s")
    if not free:
        say("player never reached free control -- movement numbers below would "
            "measure the LOCK, not the walk")

    dirs = [("Up", ["Up"]), ("Down", ["Down"]), ("Left", ["Left"]), ("Right", ["Right"]),
            ("Up+Left", ["Up", "Left"]), ("Up+Right", ["Up", "Right"]),
            ("Down+Left", ["Down", "Left"]), ("Down+Right", ["Down", "Right"])]
    say(f"hold={HOLD}s per trial; 'run' adds Cross (c)")
    say(f"{'direction':<12} {'trial':<6} {'from':>16} {'to':>16} {'dx':>6} {'dz':>6} {'dist':>6} {'u/s':>6}")
    for run in (False, True):
        for name, keys in dirs:
            for trial in (1, 2):
                if game.poll() is not None:
                    raise SystemExit
                k = keys + (["c"] if run else [])
                before = pos()
                xdo("windowfocus", "--sync", wid)
                xdo("keydown", *k)
                time.sleep(HOLD)
                xdo("keyup", *reversed(k))
                time.sleep(0.8)
                after = pos()
                if before is None or after is None:
                    continue
                dx, dz = after[0] - before[0], after[1] - before[1]
                d = (dx * dx + dz * dz) ** 0.5
                label = name + ("+run" if run else "")
                say(f"{label:<12} {trial:<6} {str(before):>16} {str(after):>16} "
                    f"{dx:>6} {dz:>6} {d:>6.0f} {d / HOLD:>6.1f}")
    say("final: " + str(pos()))
except SystemExit:
    pass
except Exception as exc:  # noqa: BLE001
    say(f"driver exception {exc!r}")
finally:
    if game.poll() is None:
        game.send_signal(signal.SIGTERM)
        time.sleep(3)
        game.kill()
    xvfb.kill()
    say("done")
