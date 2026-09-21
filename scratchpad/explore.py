#!/usr/bin/env python3
"""Field explorer from a quick-load checkpoint (2026-09-18).

Loads a quick-save (F8) instead of replaying boot+prologue+battle, then sweeps
the field with long d-pad holds and Circle presses, logging the POSDIAG
telemetry (map, player position, trigger zones Fei is inside) so progress and
blocked movement are both visible.

    usage: explore.py <tag> <display> <seconds> <checkpoint.xgqs> [field]

Ordinary keys only. Always kills the game and Xvfb on exit.
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
QS = Path(sys.argv[4])
WANT_FIELD = sys.argv[5] if len(sys.argv) > 5 else "14"
OUT = Path("/var/tmp/xeno-leaf-push/explore-" + TAG)
BIN = os.environ.get("XENO_PORT_BINARY", str(ROOT / "pc_port/build_native/xeno-port"))
HOLD = float(os.environ.get("XENO_WALK_HOLD", "4.0"))

(OUT / "shots").mkdir(parents=True, exist_ok=True)
log = open(OUT / "run.log", "w")
xvfb = subprocess.Popen(["Xvfb", f":{DISP}", "-screen", "0", "1280x960x24", "-nolisten", "tcp"],
                        stdout=open(OUT / "xvfb.log", "w"), stderr=subprocess.STDOUT)
time.sleep(2)
env = {k: v for k, v in os.environ.items() if not k.startswith("XENO_")}
env.update(DISPLAY=f":{DISP}", SDL_VIDEODRIVER="x11", XENO_KERNEL_SEL="0",
           XENO_QUICKSAVE_PATH=str(QS), XENO_FIELD_POS_DIAG="40")
# Optional boot schedule: with it the port drives its own title/prologue route
# (the recorded one) and this driver only has to press F8 once a loadable field
# is up. Without it the driver steers the title itself.
SCHEDULE = os.environ.get("XENO_EXPLORE_SCHEDULE", "")
if SCHEDULE:
    env["XENO_PAD_TEST_INPUT"] = Path(SCHEDULE).read_text().strip()
game = subprocess.Popen(["stdbuf", "-oL", "-eL", BIN], cwd=ROOT, env=env,
                        stdout=log, stderr=subprocess.STDOUT)


def say(msg):
    line = f"{time.strftime('%H:%M:%S')} {msg}"
    print(line, flush=True)
    with open(OUT / "driver.log", "a") as fh:
        fh.write(line + "\n")


def xdo(*args):
    subprocess.run(["xdotool", *args], env=env, stdout=subprocess.DEVNULL,
                   stderr=subprocess.DEVNULL, check=False)


def key(name, hold=0.15):
    if wid:
        xdo("windowfocus", "--sync", wid)
    xdo("keydown", name)
    time.sleep(hold)
    xdo("keyup", name)


def walk(direction, seconds):
    """Hold a d-pad direction, tapping Circle so talk/zone triggers fire."""
    if wid:
        xdo("windowfocus", "--sync", wid)
    xdo("keydown", direction)
    end = time.monotonic() + seconds
    while time.monotonic() < end:
        time.sleep(0.8)
        key("z", 0.12)          # Circle: talk / advance dialogue
    xdo("keyup", direction)
    time.sleep(0.4)


def shot(name):
    if wid:
        subprocess.run(["import", "-window", wid, str(OUT / "shots" / name)], env=env,
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=False)


wid = None
start = time.monotonic()
loaded = False
boot_stage = "title"
fields: list[str] = []
last_pos = None
directions = ["Up", "Right", "Down", "Left"]
turn = 0
say(f"start pid={game.pid} checkpoint={QS}")

try:
    while game.poll() is None and time.monotonic() - start < SECONDS:
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

        # Boot phase: title -> New Game -> prologue -> Lahan (field 2), then
        # quick-load the checkpoint. The title loop (func_801C58EC) wraps choice
        # 0..2 and *defaults to choice 1 == Continue*; Circle alone confirms it
        # and enters func_801D9F98, the still-stubbed save/load screen, where the
        # menu then spins. Up moves 1 -> 2 (New Game), the reference sequence.
        # Field names are matched with a word boundary: "field=4" is otherwise a
        # prefix of "field=490" and the whole boot phase misfires.
        if not loaded:
            if re.search(r"FieldLoad begin field=%s\b" % WANT_FIELD, text):
                loaded = True
                say(f"checkpoint loaded (field {WANT_FIELD})")
            elif SCHEDULE:
                # The schedule drives the boot route; as soon as a field with
                # free control is up, quick-load replaces it with the checkpoint.
                if re.search(r"FieldLoad begin field=(4|2)\b", text):
                    key("F8", 0.15)
                    time.sleep(1.5)
                else:
                    time.sleep(0.5)
            elif boot_stage == "title":
                if "title loop enter choice=1" in text:
                    say("title: Up then Circle (New Game)")
                    key("Up", 0.15)
                    time.sleep(0.6)
                    key("z", 0.15)
                    boot_stage = "prologue"
                    time.sleep(1.5)
            elif boot_stage == "prologue":
                key("z", 0.12)
                time.sleep(1.0)
                if re.search(r"FieldLoad begin field=[24]\b", text):
                    boot_stage = "load"
                    say("in the prologue/Lahan field; pressing F8 to load the checkpoint")
            else:
                key("F8", 0.15)
                time.sleep(1.6)
            time.sleep(0.2)
            continue

        for field in re.findall(r"FieldLoad begin field=(\d+)", text):
            if not fields or fields[-1] != field:
                fields.append(field)
                say(f"FIELD {field} (route {' -> '.join(fields)})")
                shot(f"field-{field}.png")

        pos_lines = re.findall(r"POSDIAG map=(\d+) pos=\((-?\d+),(-?\d+),(-?\d+)\) inZones=\[([^\]]*)\]", text)
        if pos_lines:
            map_id, x, y, z, zones = pos_lines[-1]
            current = (x, y, z)
            if current != last_pos:
                say(f"pos=({x},{y},{z}) zones=[{zones}]")
                last_pos = current

        direction = directions[turn % 4]
        turn += 1
        say(f"walk {direction} for {HOLD}s")
        walk(direction, HOLD)
        shot(f"walk{turn}.png")
    say(f"loop end rc={game.poll()}")
except Exception as exc:  # noqa: BLE001
    say(f"driver exception {exc!r}")
finally:
    if game.poll() is None:
        shot("end.png")
        game.send_signal(signal.SIGTERM)
        time.sleep(3)
        game.kill()
    xvfb.kill()
    text = (OUT / "run.log").read_text(errors="replace")
    say("route: " + " -> ".join(re.findall(r"FieldLoad begin field=(\d+)", text)))
    zones = sorted(set(re.findall(r"inZones=\[([^\]]*)\]", text)))
    say("zones seen: " + " | ".join(z for z in zones if z)[:400])
