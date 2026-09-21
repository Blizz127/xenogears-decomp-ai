#!/usr/bin/env python3
"""Seed a quick-save checkpoint in field 14 (painting room).

Boots with the title-smoke pad schedule, waits for FieldLoad field=14, presses
F7 (quick save) and exits. The checkpoint file is then reusable: later runs boot
and press F8 to land in field 14 in seconds instead of replaying the movie,
prologue and first battle.

    usage: seed_field14.py <out.xgqs> <display> [max-seconds]
"""
import os
import re
import signal
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path("/var/home/blizz/Projects/xenogears-decomp-ai")
QS = Path(sys.argv[1])
DISP = sys.argv[2] if len(sys.argv) > 2 else "85"
LIMIT = int(sys.argv[3]) if len(sys.argv) > 3 else 1500
BIN = os.environ.get("XENO_PORT_BINARY", str(ROOT / "pc_port/build_native/xeno-port"))
OUT = QS.parent
OUT.mkdir(parents=True, exist_ok=True)
log = open(OUT / "seed-field14.log", "w")
schedule = (ROOT / "scratchpad/lahan-natural-visible.A56D5m/schedule.txt").read_text().strip()

xvfb = subprocess.Popen(["Xvfb", f":{DISP}", "-screen", "0", "1280x960x24", "-nolisten", "tcp"],
                        stdout=open(OUT / "seed-xvfb.log", "w"), stderr=subprocess.STDOUT)
time.sleep(2)
env = {k: v for k, v in os.environ.items() if not k.startswith("XENO_")}
env.update(DISPLAY=f":{DISP}", SDL_VIDEODRIVER="x11", XENO_KERNEL_SEL="0",
           XENO_QUICKSAVE_PATH=str(QS), XENO_PAD_TEST_INPUT=schedule)
game = subprocess.Popen(["stdbuf", "-oL", "-eL", BIN], cwd=ROOT, env=env,
                        stdout=log, stderr=subprocess.STDOUT)


def say(msg):
    print(f"{time.strftime('%H:%M:%S')} {msg}", flush=True)


def xdo(*args):
    subprocess.run(["xdotool", *args], env=env, stdout=subprocess.DEVNULL,
                   stderr=subprocess.DEVNULL, check=False)


def key(name, hold=0.15):
    if wid:
        xdo("windowfocus", "--sync", wid)
    xdo("keydown", name)
    time.sleep(hold)
    xdo("keyup", name)


wid = None
start = time.monotonic()
saved = False
try:
    while game.poll() is None and time.monotonic() - start < LIMIT:
        if wid is None:
            p = subprocess.run(["xdotool", "search", "--onlyvisible", "--name", "Xenogears"],
                               env=env, capture_output=True, text=True)
            if p.returncode == 0 and p.stdout.split():
                wid = p.stdout.splitlines()[-1]
                say(f"window {wid}")
            else:
                time.sleep(0.5)
                continue
        text = (OUT / "seed-field14.log").read_text(errors="replace")
        # The battle needs confirm input to advance (a scene stalls on Circle
        # alone, so both buttons go in), otherwise this run parks in the fight.
        if text.count("enter retail battle") > text.count("retail battle returned"):
            for k in ("z", "c"):
                key(k, 0.15)
                time.sleep(0.35)
            continue
        if not saved and "FieldLoad begin field=14" in text:
            say("field 14 reached; clearing the aftermath dialogue")
            # The checkpoint commits only when field control is free (no
            # dialogue/menu): mash confirm buttons, then save and poll the file.
            mash_until = time.monotonic() + 45
            while time.monotonic() < mash_until:
                for k in ("z", "c"):
                    key(k, 0.15)
                    time.sleep(0.4)
            deadline = time.monotonic() + 180
            attempt = 0
            while time.monotonic() < deadline and not QS.exists():
                attempt += 1
                key("F7", 0.15)
                say(f"F7 attempt {attempt}")
                for _ in range(24):
                    time.sleep(1.0)
                    if QS.exists():
                        break
                    key("z", 0.12)
            saved = QS.exists()
            say(f"checkpoint present={saved} size={QS.stat().st_size if saved else 0}")
            break
        time.sleep(0.5)
finally:
    if game.poll() is None:
        game.send_signal(signal.SIGTERM)
        time.sleep(3)
        game.kill()
    xvfb.kill()
text = (OUT / "seed-field14.log").read_text(errors="replace")
route = re.findall(r"FieldLoad begin field=(\d+)", text)
say("route: " + " -> ".join(route))
say(f"quicksave present: {QS.exists()} size={QS.stat().st_size if QS.exists() else 0}")
