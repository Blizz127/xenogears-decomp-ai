#!/usr/bin/env python3
"""Walk field 14 from a GAME-PLACED spawn, not a teleport.

Warping the player and then testing mobility turned out to be invalid: a
control point teleported to (200,-455) -- a spot the player walks through
freely in ordinary play -- was just as immobile as points far outside the
reachable strip (1047 samples with a direction held, zero movement). Writing
`position` directly evidently desynchronises the actor from whatever the
movement code uses, so "pinned after a warp" says nothing about the floor.

This uses the game's own placement instead: XENO_FIELD_MAP=14 with
XENO_FIELD_ENTRANCE=<n> drives D_8006F954 -> field-script var 2, and the
field-load script picks spawn-table entry n itself. Whatever region the player
can then walk is genuinely walkable from that entrance.

Field 14's spawn table (from the loader's own dump) is:
    spawn[0] x=165  z=-25      spawn[1] x=178  z=313
against a post-battle player position of (115,-455), which matches neither.

    usage: field14_entrance_walk.py <tag> <display> <entrance> [seconds]
"""
import os
import re
import signal
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path("/var/home/blizz/Projects/xenogears-decomp-ai")
TAG, DISP, ENTRANCE = sys.argv[1], sys.argv[2], sys.argv[3]
SECONDS = float(sys.argv[4]) if len(sys.argv) > 4 else 240.0
# Optional "x,z": hill-climb toward that point instead of sweeping. Sweeping
# answers "how big is this area"; a goal answers "does this area reach there",
# which is what the exit zone question needs.
GOAL = None
if len(sys.argv) > 5:
    _gx, _gz = sys.argv[5].split(",")
    GOAL = (int(_gx), int(_gz))
OUT = Path("/var/tmp/xeno-leaf-push/ent-" + TAG)
OUT.mkdir(parents=True, exist_ok=True)
log = open(OUT / "run.log", "w")

xvfb = subprocess.Popen(["Xvfb", f":{DISP}", "-screen", "0", "1280x960x24", "-nolisten", "tcp"],
                        stdout=open(OUT / "xvfb.log", "w"), stderr=subprocess.STDOUT)
time.sleep(2)
env = {k: v for k, v in os.environ.items() if not k.startswith("XENO_")}
env.update(DISPLAY=f":{DISP}", SDL_VIDEODRIVER="x11",
           XENO_FIELD_TEST="1", XENO_KERNEL_SEL="0",
           XENO_FIELD_MAP="14", XENO_FIELD_ENTRANCE=ENTRANCE,
           XENO_FIELD_POS_DIAG="5")
game = subprocess.Popen(["stdbuf", "-oL", "-eL", str(ROOT / "pc_port/build_native/xeno-port")],
                        cwd=ROOT, env=env, stdout=log, stderr=subprocess.STDOUT)
wid = None
DIRS = [["Up"], ["Right"], ["Down"], ["Left"],
        ["Up", "Right"], ["Down", "Right"], ["Down", "Left"], ["Up", "Left"]]


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


POS = re.compile(r"POSDIAG map=(\d+) pos=\((-?\d+),(-?\d+),(-?\d+)\)")

try:
    say(f"direct-boot field 14, entrance {ENTRANCE}")
    start = time.monotonic()
    while game.poll() is None and time.monotonic() - start < 240:
        if wid is None:
            p = subprocess.run(["xdotool", "search", "--onlyvisible", "--name", "Xenogears"],
                               env=env, capture_output=True, text=True)
            if p.returncode == 0 and p.stdout.split():
                wid = p.stdout.splitlines()[-1]
            else:
                time.sleep(0.5)
                continue
        if POS.search(text()):
            break
        time.sleep(0.5)
    else:
        say("no POSDIAG -- field never became active")
        raise SystemExit

    first = POS.findall(text())[-1]
    say(f"game placed the player at ({first[1]},{first[3]})")

    i = 0
    dir_index = 0
    stalls = 0
    deadline = time.monotonic() + SECONDS
    while time.monotonic() < deadline and game.poll() is None:
        if GOAL is None:
            keys = DIRS[i % len(DIRS)]
        else:
            keys = DIRS[dir_index % len(DIRS)]
        i += 1
        before = [(int(x), int(z)) for m, x, _y, z in POS.findall(text()) if m == "14"]
        xdo("windowfocus", "--sync", wid)
        xdo("keydown", *keys)
        time.sleep(2.0)
        xdo("keyup", *reversed(keys))
        time.sleep(0.2)
        if GOAL is not None:
            after = [(int(x), int(z)) for m, x, _y, z in POS.findall(text()) if m == "14"]
            fields_now = re.findall(r"FieldLoad begin field=(\d+)", text())
            if fields_now and fields_now[-1] != "14":
                say(f"FIELD CHANGED to {fields_now[-1]} -- reached the exit from "
                    f"entrance {ENTRANCE}")
                break
            if before and after:
                d0 = abs(before[-1][0] - GOAL[0]) + abs(before[-1][1] - GOAL[1])
                d1 = abs(after[-1][0] - GOAL[0]) + abs(after[-1][1] - GOAL[1])
                if d1 < d0 - 2:
                    stalls = 0
                else:
                    dir_index += 1
                    stalls += 1
                if i % 8 == 0:
                    say(f"  {i}: at {after[-1]} manhattan-d={d1} (stalls={stalls})")
        if i % 16 == 0:
            pts = [(int(x), int(z)) for m, x, _y, z in POS.findall(text()) if m == "14"]
            if pts:
                say(f"  {i} holds; x[{min(a for a,_ in pts)},{max(a for a,_ in pts)}] "
                    f"z[{min(b for _,b in pts)},{max(b for _,b in pts)}] "
                    f"distinct={len(set(pts))}")
    pts = [(int(x), int(z)) for m, x, _y, z in POS.findall(text()) if m == "14"]
    if pts:
        say(f"RESULT entrance {ENTRANCE}: start=({first[1]},{first[3]}) "
            f"x[{min(a for a,_ in pts)},{max(a for a,_ in pts)}] "
            f"z[{min(b for _,b in pts)},{max(b for _,b in pts)}] "
            f"distinct={len(set(pts))} samples={len(pts)}")
    say("fields: " + " -> ".join(re.findall(r"FieldLoad begin field=(\d+)", text())))
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
