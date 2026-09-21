#!/usr/bin/env python3
"""Does field 14's only trigger zone work when you are standing in it?

Boots to the painting room, lets the post-battle scene finish, then relies on
XENO_FIELD_WARP to place the player inside zone 0 (the map 14 -> 13 door box,
x in [308,363], z in [-52,0]) and watches what happens.

DIAGNOSTIC RUN -- this writes the player's position, so its output is evidence
about the ZONE, not about ordinary play.

Outcome to read:
  field changes to 13   -> the exit machinery is fine; the open problem is
                           purely navigation from the spawn to the door.
  inZones=[0] but no change -> the zone is entered and the script ignores it.
  never inZones=[0]     -> the warp target or the zone geometry is wrong.

    usage: field14_warp_probe.py <tag> <display> [x] [z]
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
WX = sys.argv[3] if len(sys.argv) > 3 else "335"
WZ = sys.argv[4] if len(sys.argv) > 4 else "-26"
SCHEDULE = ROOT / "scratchpad/lahan-natural-visible.A56D5m/schedule.txt"
OUT = Path("/var/tmp/xeno-leaf-push/warp-" + TAG)
OUT.mkdir(parents=True, exist_ok=True)
log = open(OUT / "run.log", "w")

xvfb = subprocess.Popen(["Xvfb", f":{DISP}", "-screen", "0", "1280x960x24", "-nolisten", "tcp"],
                        stdout=open(OUT / "xvfb.log", "w"), stderr=subprocess.STDOUT)
time.sleep(2)
env = {k: v for k, v in os.environ.items() if not k.startswith("XENO_")}
env.update(DISPLAY=f":{DISP}", SDL_VIDEODRIVER="x11", XENO_KERNEL_SEL="0",
           XENO_PAD_TEST_INPUT=SCHEDULE.read_text().strip(),
           XENO_FIELD_POS_DIAG="30", XENO_FIELD_WARP=f"14:{WX}:{WZ}")
game = subprocess.Popen(["stdbuf", "-oL", "-eL", str(ROOT / "pc_port/build_native/xeno-port")],
                        cwd=ROOT, env=env, stdout=log, stderr=subprocess.STDOUT)
wid = None


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
    say(f"boot; warp target (14,{WX},{WZ})")
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

    say("field 14 loaded; clearing dialogue 40 s (warp fires as soon as it can)")
    t0 = time.monotonic()
    while time.monotonic() - t0 < 40 and game.poll() is None:
        xdo("windowfocus", "--sync", wid)
        xdo("key", "z")
        time.sleep(1.0)

    say("warp lines: " + "; ".join(l.strip() for l in text().splitlines() if "WARP" in l))
    # Nudge in place so the zone poller re-evaluates, and watch for a change.
    for i in range(30):
        if game.poll() is not None:
            break
        xdo("windowfocus", "--sync", wid)
        for k in ("Up", "Down", "z"):
            xdo("key", k)
            time.sleep(0.3)
        lines = [l for l in text().splitlines() if "POSDIAG" in l]
        if lines:
            say(f"  {i}: {lines[-1].strip()}")
        fields = re.findall(r"FieldLoad begin field=(\d+)", text())
        if fields[-1] != "14":
            say(f"FIELD CHANGED -> {fields[-1]}  (route {fields})")
            break
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
