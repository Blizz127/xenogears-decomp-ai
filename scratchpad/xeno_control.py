"""Shared helper: wait for the field to hand the player control.

Field scripts raise the field-control lock while a scripted camera pans and
lower it afterwards -- the painting room's actor 11 does this once per shot,
several shots in a row.  While it is raised, OP_UPDATE_CHARACTER parks the
player and every direction key is discarded, so a driver that waits a fixed
number of seconds and then presses keys races the scene and silently loses
every movement it asked for.  On a loaded machine (this box renders the game
well below 60 fps under llvmpipe) it loses nearly all of them, which is what
made field 14 look like a permanent script lock for three sessions.

The port's POSDIAG telemetry publishes the decisive bit as `canRun=`
(`D_800ADB68`, set to 1 only on the path where the lock is down), so drivers
can wait on the game's own state instead of on a clock.

Requires the game to run with XENO_FIELD_POS_DIAG=<n>.
"""
import re
import time

POSDIAG_RE = re.compile(
    r"POSDIAG map=(\d+) pos=\((-?\d+),(-?\d+),(-?\d+)\).*?"
    r"canRun=(-?\d+)")

# Keys that need free player control. Confirm/cancel deliberately are NOT here:
# dialogue advances while the lock is raised, and that is how a scene is driven
# to its end in the first place.
MOVEMENT_KEYS = {"Up", "Down", "Left", "Right"}


def needs_control(keys):
    """True if this key combination is a movement the lock would swallow."""
    return any(k in MOVEMENT_KEYS for k in keys)


def control_state(text):
    """(map, x, z, canRun) from the newest POSDIAG line, or None."""
    hits = POSDIAG_RE.findall(text)
    if not hits:
        return None
    m, x, _y, z, can = hits[-1]
    return int(m), int(x), int(z), int(can)


def control_is_free(text):
    st = control_state(text)
    return st is not None and st[3] == 1


def wait_for_control(read_text, timeout=90.0, poll=0.4, tap=None, log=None):
    """Block until POSDIAG reports canRun=1. Returns True if it did.

    `read_text` returns the game log so far.  `tap` is an optional callable
    invoked every ~2 s to press Circle, which is what advances a scene that is
    waiting on dialogue rather than on a camera.  Returns False on timeout --
    callers should treat that as "still locked", not as free control, and say
    so, because pressing anyway is exactly the mistake this helper exists to
    prevent.
    """
    start = time.monotonic()
    last_tap = 0.0
    while time.monotonic() - start < timeout:
        if control_is_free(read_text()):
            if log:
                log(f"  control free after {time.monotonic() - start:.1f}s")
            return True
        now = time.monotonic()
        if tap is not None and now - last_tap > 2.0:
            tap()
            last_tap = now
        time.sleep(poll)
    if log:
        log(f"  STILL LOCKED after {timeout:.0f}s (canRun never reached 1)")
    return False
