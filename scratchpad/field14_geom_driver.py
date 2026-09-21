#!/usr/bin/env python3
"""Dense direction-holder for the warp geometry probe.

The patrol driver spends most of each probe window waiting on the control lock
and produces only a handful of samples with a direction actually held, which
leaves most probe points "inconclusive". This one does the minimum needed to
answer "can the player move HERE": once control is free it holds one direction
after another, continuously, for the whole run, so every probe point gets many
samples with input applied.

    usage: field14_geom_driver.py <tag> <display> [seconds]

Pair with XENO_FIELD_WARP_PROBE and read the result with
scratchpad/analyze_geom_probe.py. DIAGNOSTIC: the probe writes the player's
position.
"""
import os
import re
import signal
import subprocess
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from xeno_control import wait_for_control  # noqa: E402

ROOT = Path("/var/home/blizz/Projects/xenogears-decomp-ai")
TAG, DISP = sys.argv[1], sys.argv[2]
SECONDS = float(sys.argv[3]) if len(sys.argv) > 3 else 600.0
SCHEDULE = ROOT / "scratchpad/lahan-natural-visible.A56D5m/schedule.txt"
OUT = Path("/var/tmp/xeno-leaf-push/geom-" + TAG)
OUT.mkdir(parents=True, exist_ok=True)
log = open(OUT / "run.log", "w")

xvfb = subprocess.Popen(["Xvfb", f":{DISP}", "-screen", "0", "1280x960x24", "-nolisten", "tcp"],
                        stdout=open(OUT / "xvfb.log", "w"), stderr=subprocess.STDOUT)
time.sleep(2)
env = {k: v for k, v in os.environ.items() if not k.startswith("XENO_")}
env.update(DISPLAY=f":{DISP}", SDL_VIDEODRIVER="x11", XENO_KERNEL_SEL="0",
           XENO_PAD_TEST_INPUT=SCHEDULE.read_text().strip(),
           XENO_PAD_TEST_STOP_FIELD="14",
           XENO_FIELD_POS_DIAG="5")
for k in ("XENO_FIELD_WARP_PROBE", "XENO_FIELD_WARP"):
    if os.environ.get(k):
        env[k] = os.environ[k]
game = subprocess.Popen(["stdbuf", "-oL", "-eL", str(ROOT / "pc_port/build_native/xeno-port")],
                        cwd=ROOT, env=env, stdout=log, stderr=subprocess.STDOUT)
wid = None
DIRS = [["Up"], ["Right"], ["Down"], ["Left"]]


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


try:
    say("boot: waiting for field 14")
    start = time.monotonic()
    while game.poll() is None and time.monotonic() - start < 600:
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

    say("field 14 loaded; clearing the setup scene, then holding directions")
    wait_for_control(text, timeout=180.0, tap=lambda: xdo("key", "z"), log=say)

    i = 0
    deadline = time.monotonic() + SECONDS
    while time.monotonic() < deadline and game.poll() is None:
        keys = DIRS[i % len(DIRS)]
        i += 1
        # Tap Circle occasionally so a dialogue-gated scene still advances,
        # but never let it displace the direction holds.
        if i % 8 == 0:
            xdo("windowfocus", "--sync", wid)
            xdo("key", "z")
        xdo("windowfocus", "--sync", wid)
        xdo("keydown", *keys)
        time.sleep(2.5)
        xdo("keyup", *reversed(keys))
        time.sleep(0.2)
        if i % 20 == 0:
            pts = [l for l in text().splitlines() if "WARPPROBE point" in l]
            say(f"  hold #{i}; probe at {pts[-1].split('WARPPROBE')[-1].strip() if pts else '(none)'}")
    say("hold loop finished")
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
