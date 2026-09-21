#!/usr/bin/env python3
"""Send ordinary keyboard input to the live port window on Xvfb :95.

usage: key.py <KEY+KEY...> <hold_seconds>
Appends the request to the route inputs.log and focuses the live window first.
"""
import os
import subprocess
import sys
import time

ROOT = "/var/home/blizz/Projects/xenogears-decomp-ai"
DIR = os.path.join(ROOT, "scratchpad/blackmoon-route-20260919")
LOG = os.environ.get("XENO_INPUT_LOG", os.path.join(DIR, "inputs.log"))
# The route harness runs the game on Xvfb :95, but it can also be run on the
# real desktop so a human can watch; XENO_DISPLAY selects which.
ENV = dict(os.environ, DISPLAY=os.environ.get("XENO_DISPLAY", ":95"))
if "XENO_XAUTHORITY" in os.environ:
    ENV["XAUTHORITY"] = os.environ["XENO_XAUTHORITY"]

keys = sys.argv[1].split("+")
hold = float(sys.argv[2]) if len(sys.argv) > 2 else 0.15

p = subprocess.run(["xdotool", "search", "--onlyvisible", "--name", "Xenogears"],
                   env=ENV, capture_output=True, text=True)
wids = p.stdout.split()
if not wids:
    sys.exit("no live Xenogears window")
wid = wids[-1]

with open(LOG, "a") as f:
    f.write(f"{time.time()} {sys.argv[1]} {hold} window={wid}\n")

# windowfocus alone is not enough on a real desktop: under KWin another
# application (or the compositor's own focus stealing) can hold the input
# focus, and PsyCross reads HELD buttons from SDL_GetKeyboardState, which is
# empty without focus -- so taps appear to work while movement silently does
# nothing.  windowactivate raises AND focuses, which is what the game needs.
subprocess.run(["xdotool", "windowactivate", "--sync", wid], env=ENV,
               check=False)
subprocess.run(["xdotool", "windowfocus", "--sync", wid], env=ENV, check=False)
try:
    subprocess.run(["xdotool", "keydown", *keys], env=ENV, check=True)
    time.sleep(hold)
finally:
    subprocess.run(["xdotool", "keyup", *keys], env=ENV, check=False)
print(f"sent {sys.argv[1]} hold={hold} wid={wid}")
