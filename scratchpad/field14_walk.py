#!/usr/bin/env python3
"""Field-14 free-control probe (2026-09-19).

Boots the port to the painting room, clears the post-battle dialogue, waits
until the player actor's script parks on opcode 0x0C (func_8009F5A8 = the
player idle-hold wrapper, i.e. Fei IS under player control), then walks one
long stride per direction and reports the position after each.

This exists because "field 14 keeps Fei script-locked" was an inference from
the script not leaving the field, never a measurement of the player's own
script state or position.  A stride that moves the actor disproves the lock; a
stride that does not, with actor 1 parked on 0x0C, localises the fault to
input or collision rather than to the scene script.

    usage: field14_walk.py <tag> <display> [stride-seconds]

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
TAG = sys.argv[1]
DISP = sys.argv[2]
STRIDE = float(sys.argv[3]) if len(sys.argv) > 3 else 3.5
SCHEDULE = ROOT / "scratchpad/lahan-natural-visible.A56D5m/schedule.txt"
BIN = ROOT / "pc_port/build_native/xeno-port"
OUT = Path("/var/tmp/xeno-leaf-push/walk14-" + TAG)
(OUT / "shots").mkdir(parents=True, exist_ok=True)
log = open(OUT / "run.log", "w")

xvfb = subprocess.Popen(["Xvfb", f":{DISP}", "-screen", "0", "1280x960x24", "-nolisten", "tcp"],
                        stdout=open(OUT / "xvfb.log", "w"), stderr=subprocess.STDOUT)
time.sleep(2)
env = {k: v for k, v in os.environ.items() if not k.startswith("XENO_")}
env.update(DISPLAY=f":{DISP}", SDL_VIDEODRIVER="x11", XENO_KERNEL_SEL="0",
           XENO_PAD_TEST_INPUT=SCHEDULE.read_text().strip(),
           XENO_FIELD_POS_DIAG="30", XENO_VM_TRACE="1", XENO_VM_TRACE_REPEAT="1",
           XENO_VM_TRACE_FIELD="14", XENO_VM_TRACE_MAX="40000")
game = subprocess.Popen(["stdbuf", "-oL", "-eL", str(BIN)], cwd=ROOT, env=env,
                        stdout=log, stderr=subprocess.STDOUT)
wid = None


def say(msg):
    line = f"{time.strftime('%H:%M:%S')} {msg}"
    print(line, flush=True)
    with open(OUT / "driver.log", "a") as fh:
        fh.write(line + "\n")


def xdo(*args):
    subprocess.run(["xdotool", *args], env=env, stdout=subprocess.DEVNULL,
                   stderr=subprocess.DEVNULL, check=False)


def text():
    return (OUT / "run.log").read_text(errors="replace")


def pos():
    """Latest player position + occupied trigger zones from the pos diag."""
    hits = re.findall(r"map=(\d+)[^\n]*?pos=\(([-\d]+),\s*([-\d]+),\s*([-\d]+)\)"
                      r"[^\n]*?(zones=\S*)?", text())
    return hits[-1] if hits else None


def raw_pos_line():
    lines = [l for l in text().splitlines() if "pos=(" in l]
    return lines[-1] if lines else "(no pos line)"


def press(keys, duration):
    if wid:
        xdo("windowfocus", "--sync", wid)
    xdo("keydown", *keys)
    try:
        time.sleep(duration)
    finally:
        xdo("keyup", *reversed(keys))
    time.sleep(0.6)


try:
    say("boot: waiting for field 14")
    start = time.monotonic()
    while game.poll() is None and time.monotonic() - start < 900:
        if wid is None:
            p = subprocess.run(["xdotool", "search", "--onlyvisible", "--name", "Xenogears"],
                               env=env, capture_output=True, text=True)
            if p.returncode == 0 and p.stdout.split():
                wid = p.stdout.splitlines()[-1]
                say(f"window {wid}")
            else:
                time.sleep(0.5)
                continue
        t = text()
        if re.search(r"FieldLoad begin field=14\b", t):
            break
        if t.count("enter retail battle") > t.count("retail battle returned"):
            for k in ("z", "c"):
                if wid:
                    xdo("windowfocus", "--sync", wid)
                xdo("key", k)
                time.sleep(0.35)
        else:
            time.sleep(0.5)
    else:
        say("never reached field 14")
        raise SystemExit

    say("field 14 loaded; clearing post-battle dialogue with Circle for 40 s")
    t0 = time.monotonic()
    while time.monotonic() - t0 < 40 and game.poll() is None:
        if wid:
            xdo("windowfocus", "--sync", wid)
        xdo("key", "z")
        time.sleep(1.0)

    # Wait for the player actor to park on the free-control opcode.
    say("waiting for actor 1 to park on op=0C (player idle-hold)")
    t0 = time.monotonic()
    free = False
    while time.monotonic() - t0 < 90 and game.poll() is None:
        if re.search(r"\[vm-trace\].*actor=1 ip=\d+ op=0C", text()):
            free = True
            break
        time.sleep(1.0)
    say(f"free-control opcode seen: {free}")
    say("position before walking: " + raw_pos_line())

    directions = [("Up", ["Up"]), ("Right", ["Right"]), ("Down", ["Down"]),
                  ("Left", ["Left"]), ("Up+Right", ["Up", "Right"]),
                  ("Down+Left", ["Down", "Left"])]
    for name, keys in directions:
        if game.poll() is not None:
            break
        before = raw_pos_line()
        press(keys, STRIDE)
        after = raw_pos_line()
        say(f"stride {name:<10} {STRIDE}s")
        say(f"   before {before}")
        say(f"   after  {after}")
        say(f"   moved: {before != after}")
        subprocess.run(["import", "-window", wid, str(OUT / "shots" / f"{name}.png")],
                       env=env, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
                       check=False)

    say("fields visited: " + " -> ".join(re.findall(r"FieldLoad begin field=(\d+)", text())))
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
