#!/usr/bin/env python3
"""Regression probe: F8 (quick load) at the title screen must not crash.

Pressing F8 on the title used to queue a load and then SIGSEGV. The cause was
`player_actor()` in pc_port/src/quick_checkpoint.c bounding the player index
only against the array pointer, never against the live actor count: the index
persists across field changes, so on the title map (490) it read past the end
of the actor table and `checkpoint_is_safe()` dereferenced the garbage
pActorData it got back.

Expected after the fix: the game stays alive and either logs
"F8 ignored: not in a field" or queues and waits, but does not fault.

    usage: f8_title_probe.py <tag> <display>
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
OUT = Path("/var/tmp/xeno-leaf-push/f8-" + TAG)
OUT.mkdir(parents=True, exist_ok=True)
log = open(OUT / "run.log", "w")

xvfb = subprocess.Popen(["Xvfb", f":{DISP}", "-screen", "0", "1280x960x24", "-nolisten", "tcp"],
                        stdout=open(OUT / "xvfb.log", "w"), stderr=subprocess.STDOUT)
time.sleep(2)
env = {k: v for k, v in os.environ.items() if not k.startswith("XENO_")}
# No pad schedule on purpose: we want to sit on the title, not advance past it.
env.update(DISPLAY=f":{DISP}", SDL_VIDEODRIVER="x11", XENO_KERNEL_SEL="0")
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
    say("boot: waiting for the title field (490)")
    start = time.monotonic()
    while game.poll() is None and time.monotonic() - start < 400:
        if wid is None:
            p = subprocess.run(["xdotool", "search", "--onlyvisible", "--name", "Xenogears"],
                               env=env, capture_output=True, text=True)
            if p.returncode == 0 and p.stdout.split():
                wid = p.stdout.splitlines()[-1]
                say(f"window {wid}")
            else:
                time.sleep(0.5)
                continue
        if re.search(r"FieldLoad begin field=490\b", text()):
            break
        time.sleep(0.5)
    else:
        say("never reached the title field")
        raise SystemExit

    say("title reached; letting it settle 20 s")
    time.sleep(20)
    if game.poll() is not None:
        say(f"game already exited before F8 (rc={game.returncode})")
        raise SystemExit

    say("pressing F8 x5")
    for i in range(5):
        xdo("windowfocus", "--sync", wid)
        xdo("key", "F8")
        time.sleep(1.5)
        if game.poll() is not None:
            say(f"*** GAME DIED after F8 #{i + 1} (rc={game.returncode}) -- STILL CRASHING")
            raise SystemExit
    say("surviving 20 s after F8")
    time.sleep(20)
    if game.poll() is not None:
        say(f"*** GAME DIED in the settle window (rc={game.returncode})")
        raise SystemExit
    say("PASS: game alive after F8 at the title")
    for line in text().splitlines():
        if "[quick]" in line:
            say("  " + line.strip())
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
