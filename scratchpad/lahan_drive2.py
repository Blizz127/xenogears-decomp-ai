#!/usr/bin/env python3
"""Natural-input driver for the boot -> Lahan route (2026-09-18).

Usage: lahan_drive2.py <tag> <display> <seconds> [schedule-file]

Phase 1 (settle seconds, default 260) lets the scripted pad schedule drive the
proven boot route (title -> New Game -> prologue -> Lahan). Phase 2 adds live
xdotool input on top of it: Circle to talk/advance, d-pad to walk, Cross to
fight when a retail battle is up. Ordinary keys only, no game-state writes.

    Z = Circle (confirm/talk)   C = Cross (attack/confirm)
    V = Triangle (menu)         Enter = Start (skip movie)   Space = Select
"""
import os
import re
import shutil
import signal
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path("/var/home/blizz/Projects/xenogears-decomp-ai")
TAG = sys.argv[1]
OUT = Path("/var/tmp/xeno-leaf-push/lahan-" + TAG)
DISP = sys.argv[2] if len(sys.argv) > 2 else "82"
SECONDS = int(sys.argv[3]) if len(sys.argv) > 3 else 2400
SCHED_FILE = sys.argv[4] if len(sys.argv) > 4 else ""
SETTLE = int(os.environ.get("XENO_DRIVE_SETTLE", "260"))
BIN = os.environ.get("XENO_PORT_BINARY", str(ROOT / "pc_port/build_native/xeno-port"))

(OUT / "shots").mkdir(parents=True, exist_ok=True)
shutil.copy2(BIN, OUT / "xeno-port")
log = (OUT / "run.log").open("w")
xvfb = subprocess.Popen(["Xvfb", f":{DISP}", "-screen", "0", "1280x960x24", "-nolisten", "tcp"],
                        stdout=open(OUT / "xvfb.log", "w"), stderr=subprocess.STDOUT)
time.sleep(2)
env = {k: v for k, v in os.environ.items() if not k.startswith("XENO_")}
env.update(DISPLAY=f":{DISP}", SDL_VIDEODRIVER="x11", XENO_KERNEL_SEL="0")
if SCHED_FILE:
    env["XENO_PAD_TEST_INPUT"] = Path(SCHED_FILE).read_text().strip()
game = subprocess.Popen(["stdbuf", "-oL", "-eL", str(OUT / "xeno-port")],
                        cwd=ROOT, env=env, stdout=log, stderr=subprocess.STDOUT)
(OUT / "process.json").write_text(f'{{"game": {game.pid}, "xvfb": {xvfb.pid}, "display": "{DISP}"}}\n')


def note(msg):
    line = f"{time.strftime('%H:%M:%S')} {msg}"
    print(line, flush=True)
    with open(OUT / "driver.log", "a") as fh:
        fh.write(line + "\n")


def xdo(*args):
    subprocess.run(["xdotool", *args], env=env,
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=False)


def key(name, hold=0.15):
    if WID:
        xdo("windowfocus", "--sync", WID)
    xdo("keydown", name)
    time.sleep(hold)
    xdo("keyup", name)


def shot(name):
    if WID:
        subprocess.run(["import", "-window", WID, str(OUT / "shots" / name)], env=env,
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=False)


WID = None
start = time.monotonic()
seen_fields: set[str] = set()
battles_in = battles_out = 0
last_confirm = last_walk = last_start = 0.0
direction = 0
note(f"start pid={game.pid} display={DISP} schedule={SCHED_FILE or 'none'} binary={BIN}")

try:
    while game.poll() is None and time.monotonic() - start < SECONDS:
        if WID is None:
            p = subprocess.run(["xdotool", "search", "--onlyvisible", "--name", "Xenogears"],
                               env=env, capture_output=True, text=True)
            if p.returncode == 0 and p.stdout.split():
                WID = p.stdout.splitlines()[-1]
                note(f"window {WID}")
            else:
                time.sleep(0.5)
                continue
        text = (OUT / "run.log").read_text(errors="replace")
        for field in re.findall(r"FieldLoad begin field=(\d+)", text):
            if field not in seen_fields:
                seen_fields.add(field)
                note(f"FIELD {field}")
                shot(f"field-{field}.png")
        if text.count("enter retail battle") > battles_in:
            battles_in = text.count("enter retail battle")
            note(f"BATTLE ENTER #{battles_in} (fields so far: {' '.join(sorted(seen_fields))})")
            shot(f"battle{battles_in}-enter.png")
        if text.count("retail battle returned") > battles_out:
            battles_out = text.count("retail battle returned")
            note(f"BATTLE RETURN #{battles_out}")
            shot(f"battle{battles_out}-return.png")

        now = time.monotonic()
        elapsed = now - start
        in_battle = battles_in > battles_out
        if elapsed < SETTLE:
            # Let the schedule drive the boot route. Do NOT press Start here:
            # Start opens the pause handler (func_8008A3EC, which loops on
            # ControllerPopState and draws no letters), and the run then looks
            # frozen -- Circle/Cross do not close it, only Start does. That cost
            # one run on 2026-09-18 (unpaused by hand, battle then completed).
            pass
        elif in_battle:
            if now - last_confirm > 1.1:
                # Circle advances battle dialogue/cutscenes; Cross confirms the
                # attack command and target. A scene in the burning-Lahan
                # sequence sat on "Fei Hyaaaa!" while only Cross was sent and
                # advanced once Circle was sent too (observed 2026-09-18), so
                # both are sent every cycle.
                key("z")
                time.sleep(0.3)
                key("c")
                last_confirm = now
        else:
            if now - last_confirm > 1.5:    # Circle: talk / advance dialogue
                key("z")
                last_confirm = now
            if now - last_walk > 2.6:       # walk to keep the story moving
                key(["Up", "Right", "Down", "Left"][direction % 4], hold=0.5)
                direction += 1
                last_walk = now
        time.sleep(0.25)
    note(f"loop end rc={game.poll()}")
except Exception as exc:  # noqa: BLE001
    note(f"driver exception {exc!r}")
finally:
    if game.poll() is None:
        shot("end.png")
        game.send_signal(signal.SIGTERM)
        time.sleep(3)
        game.kill()
    xvfb.kill()
    text = (OUT / "run.log").read_text(errors="replace")
    fields = re.findall(r"FieldLoad begin field=(\d+)", text)
    note("route: " + " -> ".join(fields))
    note(f"battles in={text.count('enter retail battle')} out={text.count('retail battle returned')}")
    unres = sorted(set(re.findall(r"unresolved native call target=0x[0-9a-f]+", text)))
    if unres:
        note("unresolved: " + "; ".join(unres[:6]))
    stubs = sorted(set(re.findall(r"\[stub\] (\w+)", text)))
    if stubs:
        note("stubs first10: " + " ".join(stubs[:10]))
