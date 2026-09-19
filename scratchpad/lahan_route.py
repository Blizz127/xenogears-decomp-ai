#!/usr/bin/env python3
"""Start-to-end-of-Lahan route player (2026-09-18).

Boots the port (title -> New Game via the recorded pad schedule, battle driven
with Circle+Cross), waits for the painting room (field 14), then replays the
named input slices recorded by the 2026-09-08 natural run
(scratchpad/lahan-natural-20260908-compmatrix/manual-input.log), which covers
the rest of the Lahan chapter: painting/Dan, the village, Alice, the mountain
battles, Citan, Yui, the rooftop, the music box, night, the bridge, the Giants,
burning Lahan, the Gear battle and the destruction aftermath.

    usage: lahan_route.py <tag> <display> <seconds> [start-slice] [max-slices]

Ordinary keys only, no game-state writes. Logs every slice, screenshots each
phase change, reports field loads / battles / crashes, and always cleans up.
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
SECONDS = int(sys.argv[3])
START_SLICE = sys.argv[4] if len(sys.argv) > 4 else "painting-close0"
MAX_SLICES = int(sys.argv[5]) if len(sys.argv) > 5 else 100000
SLICE_LOG = ROOT / "scratchpad/lahan-natural-20260908-compmatrix/manual-input.log"
SCHEDULE = ROOT / "scratchpad/lahan-natural-visible.A56D5m/schedule.txt"
BIN = os.environ.get("XENO_PORT_BINARY", str(ROOT / "pc_port/build_native/xeno-port"))
SETTLE = float(os.environ.get("XENO_ROUTE_SETTLE", "0.30"))
WANT_FIELD = os.environ.get("XENO_ROUTE_FIELD", "14")
OUT = Path("/var/tmp/xeno-leaf-push/route-" + TAG)
(OUT / "shots").mkdir(parents=True, exist_ok=True)
log = open(OUT / "run.log", "w")

# The recording's WAIT between slices is part of the route, not noise: the
# human who made it sat through each scene before pressing the next key (26 s
# between painting-final and easel-backoff, 29 s before dan-0). Replaying the
# keys back to back with a fixed settle spends every movement slice while the
# field-14 scene still holds the player, so the actor never moves and every
# later slice is applied from the wrong place. Recover the gaps from the
# timestamps and honour them, capped so one long pause cannot eat the budget.
GAP_CAP = float(os.environ.get("XENO_ROUTE_GAP_CAP", "40"))
steps = []
prev_t = None
for line in SLICE_LOG.read_text().splitlines():
    parts = line.split()
    if len(parts) != 6:
        continue
    day, clock, name, keys, duration, _unit = parts
    try:
        t = time.mktime(time.strptime(f"{day} {clock}", "%Y-%m-%d %H:%M:%S"))
    except ValueError:
        continue
    gap = 0.0 if prev_t is None else max(0.0, min(GAP_CAP, t - prev_t - float(duration)))
    prev_t = t
    steps.append((name, keys.split("+"), float(duration), gap))
start_index = next((i for i, s in enumerate(steps) if s[0] == START_SLICE), None)
if start_index is None:
    raise SystemExit(f"start slice {START_SLICE!r} not in {SLICE_LOG}")
route = steps[start_index:start_index + MAX_SLICES]

xvfb = subprocess.Popen(["Xvfb", f":{DISP}", "-screen", "0", "1280x960x24", "-nolisten", "tcp"],
                        stdout=open(OUT / "xvfb.log", "w"), stderr=subprocess.STDOUT)
time.sleep(2)
env = {k: v for k, v in os.environ.items() if not k.startswith("XENO_")}
env.update(DISPLAY=f":{DISP}", SDL_VIDEODRIVER="x11", XENO_KERNEL_SEL="0",
           XENO_PAD_TEST_INPUT=SCHEDULE.read_text().strip())
# The XENO_ filter above exists so a stale harness variable cannot change the
# route, but diagnostics have to reach the game: forward the trace/diag ones
# explicitly (they only add logging, they never alter game state).
for passthrough in ("XENO_VM_TRACE", "XENO_VM_TRACE_REPEAT", "XENO_VM_TRACE_MAX",
                    "XENO_VM_TRACE_FIELD", "XENO_FIELD_POS_DIAG", "XENO_FIELD_DIAG",
                    "XENO_DISTORTION_DIAG"):
    if os.environ.get(passthrough):
        env[passthrough] = os.environ[passthrough]
game = subprocess.Popen(["stdbuf", "-oL", "-eL", BIN], cwd=ROOT, env=env,
                        stdout=log, stderr=subprocess.STDOUT)
(OUT / "process.json").write_text(f'{{"game": {game.pid}, "xvfb": {xvfb.pid}, "display": "{DISP}"}}\n')


def say(msg):
    line = f"{time.strftime('%H:%M:%S')} {msg}"
    print(line, flush=True)
    with open(OUT / "driver.log", "a") as fh:
        fh.write(line + "\n")


wid = None


def xdo(*args):
    subprocess.run(["xdotool", *args], env=env, stdout=subprocess.DEVNULL,
                   stderr=subprocess.DEVNULL, check=False)


def key(name, hold=0.15):
    if wid:
        xdo("windowfocus", "--sync", wid)
    xdo("keydown", name)
    time.sleep(hold)
    xdo("keyup", name)


def shot(name):
    if wid:
        subprocess.run(["import", "-window", wid, str(OUT / "shots" / name)], env=env,
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=False)


def press(keys, duration):
    if wid:
        xdo("windowfocus", "--sync", wid)
    xdo("keydown", *keys)
    try:
        time.sleep(duration)
    finally:
        xdo("keyup", *reversed(keys))
    time.sleep(SETTLE)


def field_route():
    return re.findall(r"FieldLoad begin field=(\d+)", (OUT / "run.log").read_text(errors="replace"))


start = time.monotonic()
phase = None
battles_in = battles_out = 0
plays = 0
try:
    # --- phase 1: boot to the painting room -----------------------------------
    say(f"boot: schedule={SCHEDULE.name}, waiting for field {WANT_FIELD}")
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
        text = (OUT / "run.log").read_text(errors="replace")
        if re.search(r"FieldLoad begin field=%s\b" % WANT_FIELD, text):
            break
        if text.count("enter retail battle") > text.count("retail battle returned"):
            for k in ("z", "c"):
                key(k, 0.15)
                time.sleep(0.35)
        else:
            time.sleep(0.5)
    else:
        say("boot did not reach the start field")
        raise SystemExit
    say(f"field {WANT_FIELD} reached; route {field_route()}; starting slice replay at {START_SLICE}")
    shot("route-start.png")

    # --- phase 2: replay the recorded slices ---------------------------------
    for index, (name, keys, duration, gap) in enumerate(route, start=1):
        if game.poll() is not None:
            say(f"game exited during slice {name}")
            break
        if time.monotonic() - start > SECONDS:
            say(f"time budget reached at slice {index} ({name})")
            break
        prefix = re.sub(r"-?[0-9]*$", "", name) or name
        if prefix != phase:
            phase = prefix
            say(f"phase {phase} at slice {index} ({name})")
            shot(f"phase-{phase}.png")
        if gap > 0:
            time.sleep(gap)
        press(keys, duration)
        plays += 1
        text = (OUT / "run.log").read_text(errors="replace")
        # Per-slice position. Replay is open-loop, so the only way to see WHERE
        # it diverges from the recording is to print where each slice left the
        # player rather than only which fields were reached.
        posl = [l for l in text.splitlines() if "POSDIAG" in l]
        if posl:
            say(f"  slice {index:<4} {name:<22} {posl[-1].split('POSDIAG')[-1].strip()}")
        new_in = text.count("enter retail battle")
        new_out = text.count("retail battle returned")
        if new_in > battles_in:
            battles_in = new_in
            say(f"BATTLE ENTER #{battles_in} at slice {name}")
            shot(f"battle{battles_in}-enter.png")
        if new_out > battles_out:
            battles_out = new_out
            say(f"BATTLE RETURN #{battles_out} at slice {name}")
            shot(f"battle{battles_out}-return.png")
        if index % 25 == 0:
            say(f"progress {index}/{len(route)} ({name}); fields {field_route()}")
            shot(f"progress-{index}.png")
    say(f"replay done: {plays} slices played")
except SystemExit:
    pass
except Exception as exc:  # noqa: BLE001
    say(f"driver exception {exc!r}")
finally:
    if game.poll() is None:
        shot("route-end.png")
        game.send_signal(signal.SIGTERM)
        time.sleep(3)
        game.kill()
    xvfb.kill()
    text = (OUT / "run.log").read_text(errors="replace")
    say("final route: " + " -> ".join(field_route()))
    say(f"battles in={text.count('enter retail battle')} out={text.count('retail battle returned')}")
    unres = sorted(set(re.findall(r"unresolved native call target=0x[0-9a-f]+", text)))
    if unres:
        say("unresolved: " + "; ".join(unres[:6]))
    stubs = sorted(set(re.findall(r"\[stub\] (\w+)", text)))
    if stubs:
        say("stubs: " + " ".join(stubs[:12]))
