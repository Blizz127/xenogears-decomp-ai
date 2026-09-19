#!/usr/bin/env python3
"""Closed-loop field navigator (2026-09-19).

Boots the port to a field, reads that field's trigger-zone geometry out of the
port's own ZONEDUMP telemetry, and walks the player to a chosen zone by
measuring what each d-pad direction actually does instead of assuming a
mapping.

Why measure: field input is CAMERA-relative and field 14's camera is scripted,
so a fixed "Up = +X,+Z" table is wrong as soon as the camera moves -- which is
how earlier sweeps concluded the player could not move at all.  Each step tries
the directions, keeps the one that reduces distance to the target most, and
re-measures; a wall simply produces no improvement and the next direction wins.

    usage: field_navigate.py <tag> <display> [field] [zone|all] [budget-s]

`zone` selects a target zone index, or "all" to visit each zone in turn.
Ordinary keys only, no game-state writes.
"""
import math
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
FIELD = sys.argv[3] if len(sys.argv) > 3 else "14"
TARGET = sys.argv[4] if len(sys.argv) > 4 else "all"
BUDGET = float(sys.argv[5]) if len(sys.argv) > 5 else 900.0
SCHEDULE = ROOT / "scratchpad/lahan-natural-visible.A56D5m/schedule.txt"
BIN = ROOT / "pc_port/build_native/xeno-port"
OUT = Path("/var/tmp/xeno-leaf-push/nav-" + TAG)
(OUT / "shots").mkdir(parents=True, exist_ok=True)
log = open(OUT / "run.log", "w")

DIRECTIONS = [("U", ["Up"]), ("R", ["Right"]), ("D", ["Down"]), ("L", ["Left"]),
              ("UR", ["Up", "Right"]), ("DR", ["Down", "Right"]),
              ("DL", ["Down", "Left"]), ("UL", ["Up", "Left"])]

xvfb = subprocess.Popen(["Xvfb", f":{DISP}", "-screen", "0", "1280x960x24", "-nolisten", "tcp"],
                        stdout=open(OUT / "xvfb.log", "w"), stderr=subprocess.STDOUT)
time.sleep(2)
env = {k: v for k, v in os.environ.items() if not k.startswith("XENO_")}
env.update(DISPLAY=f":{DISP}", SDL_VIDEODRIVER="x11", XENO_KERNEL_SEL="0",
           XENO_PAD_TEST_INPUT=SCHEDULE.read_text().strip(),
           XENO_FIELD_POS_DIAG="15")
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


POS_RE = re.compile(r"POSDIAG map=(\d+) pos=\((-?\d+),(-?\d+),(-?\d+)\) inZones=\[([^\]]*)\]")


def state():
    """(map, x, y, z, zones) from the most recent POSDIAG line."""
    hits = POS_RE.findall(text())
    if not hits:
        return None
    m, x, y, z, zones = hits[-1]
    return int(m), int(x), int(y), int(z), zones


def zones_for(field):
    """Parse the one-shot ZONEDUMP for `field` into {index: (cx, cz)}."""
    out = {}
    block = re.search(r"ZONEDUMP map=%s count=(\d+)(.*)" % field, text(), re.S)
    if not block:
        return out
    for line in block.group(2).splitlines():
        m = re.search(r"ZONE\s+(\d+) .*center=\((-?\d+),(-?\d+)\)", line)
        if m:
            out[int(m.group(1))] = (int(m.group(2)), int(m.group(3)))
        elif "ZONEDUMP" in line:
            break
    return out


def actors_for(field):
    """{index: (x, z)} from the newest ACTORDUMP block for `field`."""
    blocks = re.findall(r"ACTORDUMP map=%s count=\d+ player=(\d+)\n(.*?)(?=\[xeno-port\]\[test\] (?:POSDIAG|ACTORDUMP|ZONE))"
                        % field, text(), re.S)
    if not blocks:
        return {}
    player, body = blocks[-1]
    out = {}
    for line in body.splitlines():
        m = re.search(r"ACTOR\s+(\d+) pos=\((-?\d+),(-?\d+),(-?\d+)\)", line)
        if m and m.group(1) != player:
            out[int(m.group(1))] = (int(m.group(2)), int(m.group(4)))
    return out


def press(keys, duration):
    if wid:
        xdo("windowfocus", "--sync", wid)
    xdo("keydown", *keys)
    try:
        time.sleep(duration)
    finally:
        xdo("keyup", *reversed(keys))
    time.sleep(0.45)


def dist(a, b):
    return math.hypot(a[0] - b[0], a[1] - b[1])


try:
    say(f"boot: waiting for field {FIELD}")
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
        if re.search(r"FieldLoad begin field=%s\b" % FIELD, t):
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
        say(f"never reached field {FIELD}")
        raise SystemExit

    say("clearing dialogue with Circle for 40 s")
    t0 = time.monotonic()
    while time.monotonic() - t0 < 40 and game.poll() is None:
        if wid:
            xdo("windowfocus", "--sync", wid)
        xdo("key", "z")
        time.sleep(1.0)

    zmap = zones_for(FIELD)
    say(f"field {FIELD} zones: {zmap}")
    say(f"start state: {state()}")
    if not zmap:
        say("no zone dump -- cannot navigate")
        raise SystemExit

    if TARGET == "patrol":
        # Coverage patrol.  Distance hill-climbing gets trapped in this room's
        # local pockets, and the way out of field 14 is an ACTOR answering
        # Circle rather than a reachable zone -- so sweep the floor and press
        # Circle everywhere instead of trying to path to a point.  Direction
        # choice is least-recently-used, which spreads coverage without
        # needing a map.
        # Run-until-blocked, NOT round-robin: alternating directions each step
        # just walks back and forth (measured: 20 steps covered 3 cells).  Hold
        # one direction while it keeps producing movement, and only rotate when
        # it stops -- that reaches walls and corners, which is where doors are.
        seen = set()
        deadline = time.monotonic() + BUDGET
        step = 0
        dir_index = 0
        blocked = 0
        last = None
        while time.monotonic() < deadline and game.poll() is None:
            step += 1
            name = DIRECTIONS[dir_index % len(DIRECTIONS)][0]
            press(dict(DIRECTIONS)[name], 1.6)
            for _ in range(2):
                if wid:
                    xdo("windowfocus", "--sync", wid)
                xdo("key", "z")
                time.sleep(0.5)
            st = state()
            if st is None:
                continue
            m, x, _y, z, inz = st
            if str(m) != FIELD:
                say(f"FIELD CHANGED to {m} at step {step} after {name}+Circle")
                raise SystemExit
            if inz:
                say(f"ENTERED zone(s) [{inz}] at ({x},{z})")
            seen.add((x // 16, z // 16))
            if last is not None and abs(x - last[0]) + abs(z - last[1]) < 6:
                blocked += 1
                if blocked >= 2:
                    dir_index += 1
                    blocked = 0
            else:
                blocked = 0
            last = (x, z)
            if step % 10 == 0:
                say(f"  patrol step {step} at ({x},{z}) cells={len(seen)} "
                    f"scenario-line: {[l for l in text().splitlines() if 'POSDIAG' in l][-1][-40:]}")
        say(f"patrol finished: {step} steps, {len(seen)} distinct 16-unit cells")
        raise SystemExit

    if TARGET == "actors":
        # Field 14's only zone is outside the reachable floor, so the way out
        # is an actor (door or NPC).  Visit each one and answer with Circle.
        amap = actors_for(FIELD)
        say(f"field {FIELD} actors: {amap}")
        zmap = amap
        targets = sorted(amap)
    else:
        targets = sorted(zmap) if TARGET == "all" else [int(TARGET)]
    deadline = time.monotonic() + BUDGET

    for zi in targets:
        goal = zmap[zi]
        say(f"=== target zone {zi} center={goal} ===")
        stalls = 0
        dir_index = 0
        while time.monotonic() < deadline and game.poll() is None:
            st = state()
            if st is None:
                break
            m, x, _y, z, inzones = st
            if str(m) != FIELD:
                say(f"FIELD CHANGED to {m} while heading for zone {zi}")
                raise SystemExit
            if inzones:
                say(f"ENTERED zone(s) [{inzones}] at ({x},{z})")
                break
            d0 = dist((x, z), goal)
            # Hill-climb, do NOT probe-then-commit: every probe MOVES the
            # actor, so a "best of 8 probes" is measured from eight different
            # places and the commit then runs from the last one.  Instead keep
            # walking the current direction while it helps, and rotate to the
            # next one when it stops helping (a wall simply stops helping).
            name, keys = DIRECTIONS[dir_index % len(DIRECTIONS)]
            press(keys, 1.2)
            st2 = state()
            if st2 is None:
                break
            m2, x2, _y2, z2, inz2 = st2
            if str(m2) != FIELD:
                say(f"FIELD CHANGED to {m2} while walking {name}")
                raise SystemExit
            if inz2:
                say(f"ENTERED zone(s) [{inz2}] at ({x2},{z2}) via {name}")
                break
            d1 = dist((x2, z2), goal)
            say(f"  {name:<2} ({x},{z}) d={d0:.0f} -> ({x2},{z2}) d={d1:.0f}")
            if TARGET == "actors" and d1 < 60:
                say(f"  within {d1:.0f} of actor {zi}; pressing Circle")
                for _ in range(6):
                    if wid:
                        xdo("windowfocus", "--sync", wid)
                    xdo("key", "z")
                    time.sleep(0.8)
                st3 = state()
                if st3 and str(st3[0]) != FIELD:
                    say(f"FIELD CHANGED to {st3[0]} after talking to actor {zi}")
                    raise SystemExit
                say(f"  after Circle at actor {zi}: {st3}")
                break
            if d1 < d0 - 2:
                stalls = 0          # keep this direction
            else:
                dir_index += 1
                stalls += 1
                if stalls >= 2 * len(DIRECTIONS):
                    say(f"  no direction reduces distance to zone {zi}; giving up")
                    break
        subprocess.run(["import", "-window", wid, str(OUT / "shots" / f"zone{zi}.png")],
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
    say("fields: " + " -> ".join(re.findall(r"FieldLoad begin field=(\d+)", text())))
    say("done")
