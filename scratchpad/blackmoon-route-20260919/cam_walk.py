#!/usr/bin/env python3
"""Camera-analytic walkmesh walker (map 23, Blackmoon Forest basin).

Rounds 17-18 got the walkmesh route and the triangle telemetry right but kept
failing on one thing: the field's direction keys are CAMERA-RELATIVE and the
camera rotates as the player moves, so a direction's world delta learned at one
spot is wrong at the next (the table-based walker drifted east through
tri40/tri237/tri234, and the adaptive version deadlocked with nothing to rank by).

POSDIAG now also prints the field camera (`eye=`/`at=`, 16.16 world units), so
the mapping is a pure rotation and can be computed instead of learned:

    forward = normalize(at - eye)            in xz
    right   = (forward.z, -forward.x)        up to one global sign

The sign is fixed once with a single probe of the Right key.  Each step then
re-plans from the player's actual triangle (so drift cannot accumulate) and
presses the key whose camera-space direction best matches the desired world
delta.

Usage: cam_walk.py <log> [steps]
"""
import heapq
import math
import os
import re
import struct
import subprocess
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
KEY = HERE / "key.py"
ZONE2 = ((-978, -1793), (-606, -1314))
LAYER = 0
THRESH = 0.4

# The walker was written against map 23 with a hand-extracted mesh and a
# hard-coded zone-2 rectangle.  XENO_WALKMESH_DUMP now writes the same files
# for whatever field is loaded, so the map, the mesh directory and the goal
# rectangle are all selectable and the same planner drives any map.
WALK_MAP = int(os.environ.get("XENO_WALK_MAP", "23"))
MESH_DIR = Path(os.environ.get("XENO_WALK_MESH_DIR", str(HERE)))
_GOAL_XZ = os.environ.get("XENO_WALK_GOAL_XZ")
if _GOAL_XZ:
    _g = [int(v) for v in _GOAL_XZ.split(",")]
    ZONE2 = ((_g[0], _g[1]), (_g[2], _g[3]))

POS_RE = re.compile(
    r"POSDIAG map=(\d+) pos=\((-?\d+),(-?\d+),(-?\d+)\).*?"
    r"canRun=(\d+) owner=(0x[0-9a-f]+).*?"
    r"tri=(-?\d+) layer=(-?\d+) triV=-?\d+,-?\d+,-?\d+ "
    r"eye=\((-?\d+),(-?\d+),(-?\d+)\) at=\((-?\d+),(-?\d+),(-?\d+)\)")

LOG = None


def tap(keys, hold=0.5):
    for k in (keys if isinstance(keys, (list, tuple)) else [keys]):
        subprocess.run([sys.executable, str(KEY), k, str(hold)],
                       check=False, stdout=subprocess.DEVNULL,
                       stderr=subprocess.DEVNULL)


def read_log():
    try:
        return LOG.read_text(errors="replace")
    except OSError:
        return ""


def position(text=None):
    text = read_log() if text is None else text
    hits = POS_RE.findall(text)
    if not hits:
        return None
    (m, x, y, z, canrun, owner, tri, layer,
     ex, ey, ez, ax, ay, az) = hits[-1]
    return {"map": int(m), "x": int(x), "y": int(y), "z": int(z),
            "canrun": int(canrun), "owner": owner, "tri": int(tri),
            "eye": (int(ex), int(ey), int(ez)),
            "at": (int(ax), int(ay), int(az))}


def pos_count():
    return read_log().count("[xeno-port][test] POSDIAG ")


def fresh_position(timeout=3.0):
    """POSDIAG prints every 60 frames, so a short press can be measured with a
    stale sample and look like a zero move.  Wait for a NEW line instead."""
    before = pos_count()
    deadline = time.time() + timeout
    while time.time() < deadline:
        if pos_count() > before:
            return position()
        time.sleep(0.1)
    return position()


def battle_active(text=None):
    text = read_log() if text is None else text
    return text.count("enter retail battle.bin") > \
        text.count("retail battle returned")


def fight_until_done(limit=60):
    """Include Square and stop sending inputs once the battle returns.

    The captured round-40 fight resumed on X/Square, which the old driver
    never sent. Z confirms menus; V and X cover the other attack inputs.
    This bounded driver is not proof that every waiting battle is healthy.
    """
    for _ in range(limit):
        for key, pause in (("z", 1.2), ("v", 1.1), ("x", 1.1), ("z", 1.6)):
            if not battle_active():
                return True
            tap(key, 0.15)
            time.sleep(pause)
    return not battle_active()


def close_menu_if_open():
    for _ in range(6):
        p = position()
        if p is None or p["owner"] != "0x80":
            return
        tap("Down", 0.4); time.sleep(0.8)
        tap("z", 0.5); time.sleep(2.0)


def settle():
    for _ in range(40):
        if battle_active():
            fight_until_done()
            continue
        p = position()
        if p and p["canrun"] == 1 and p["owner"] == "0xff":
            return p
        close_menu_if_open()
        time.sleep(1.0)
    return position()


# ---- walkmesh ----
def load_layer(L):
    tb = (MESH_DIR / f"map{WALK_MAP}-tris{L}.bin").read_bytes()
    vb = (MESH_DIR / f"map{WALK_MAP}-verts{L}.bin").read_bytes()
    verts = [struct.unpack_from("<4h", vb, j * 8)[:3]
             for j in range(len(vb) // 8)]
    out = []
    for i in range(len(tb) // 14):
        a = struct.unpack_from("<7H", tb, i * 14)
        out.append({"v": [verts[k] for k in a[:3]], "n": a[3:6],
                    "idx": a[:3], "mat": a[6]})
    return out


TRIS = load_layer(LAYER)

# Walkability is the GAME's rule, not an arbitrary slope threshold: the field's
# edge search (func_8007BEF4) refuses a triangle whose material flags carry
# 0x00400000 when the new surface is below the current one.  Using a 0.4 slope
# filter instead excluded tri191/tri758 - the only non-ledge descent - and made
# zone 2 look unreachable.  Materials come from the same extraction the planner
# uses (map23-materials.bin = D_800AFB20 rows).
try:
    import struct as _struct
    _MAT = (MESH_DIR / f"map{WALK_MAP}-materials.bin").read_bytes()

    def _mat_flags(mat):
        return (_struct.unpack_from("<I", _MAT, mat * 4)[0]
                if mat * 4 + 4 <= len(_MAT) else 0)
except OSError:
    def _mat_flags(mat):
        return 0


def centroid(t):
    return tuple(sum(v[k] for v in t["v"]) / 3 for k in range(3))


def walkable(t):
    v = t["v"]
    e = [v[1][k] - v[0][k] for k in range(3)]
    f = [v[2][k] - v[0][k] for k in range(3)]
    n = (e[1] * f[2] - e[2] * f[1], e[2] * f[0] - e[0] * f[2],
         e[0] * f[1] - e[1] * f[0])
    return abs(n[1]) >= math.hypot(n[0], n[2]) * THRESH


# Walkability: the game refuses a flagged triangle when the actor layer is 0
# (0x800000) unconditionally, but 0x400000 is CONDITIONAL -- func_8007BEF4 only
# refuses it when the new surface is BELOW the current one.  Treating 0x400000
# as an absolute block (which is what the map-23 route needed, and what this
# planner used to do) walls off map 22 completely: from the arrival triangle it
# left 176 of 1318 triangles reachable and neither exit zone among them.  So
# 0x800000 is a node rule and 0x400000 is an EDGE rule, evaluated against the
# triangle actually being stepped off.
OK = [(_mat_flags(t["mat"]) & 0x00800000) == 0 and t["v"] is not None
      for t in TRIS]
CEN = [centroid(t) for t in TRIS]
_STEPDOWN = [(_mat_flags(t["mat"]) & 0x00400000) != 0 for t in TRIS]


def passable(i, j):
    """Can the player step from triangle i onto triangle j?"""
    if not OK[j]:
        return False
    # Lower ground is more negative y here (the map-23 descent runs 0 -> -140).
    if _STEPDOWN[j] and CEN[j][1] < CEN[i][1]:
        return False
    return True
# XENO_WALK_GOAL_TRI lets the route aim at one specific triangle (the map's real
# walkable descent lands in the north-east low ground, which is not a trigger
# zone), while the default stays the zone-2 rectangle.
_GOAL_TRI = os.environ.get("XENO_WALK_GOAL_TRI")
if _GOAL_TRI is not None:
    ZONE_TRIS = {int(_GOAL_TRI)}
else:
    ZONE_TRIS = {i for i in range(len(TRIS)) if OK[i] and
             min(ZONE2[0][0], ZONE2[1][0]) <= CEN[i][0] <=
             max(ZONE2[0][0], ZONE2[1][0]) and
             min(ZONE2[0][1], ZONE2[1][1]) <= CEN[i][2] <=
             max(ZONE2[0][1], ZONE2[1][1])}


def next_hop(start):
    if start in ZONE_TRIS:
        return "ZONE", None, []
    dist = {start: 0.0}
    prev = {}
    q = [(0.0, start)]
    while q:
        d, i = heapq.heappop(q)
        if d != dist.get(i):
            continue
        for j in TRIS[i]["n"]:
            if j >= len(TRIS) or not passable(i, j):
                continue
            nd = d + math.dist(CEN[i], CEN[j])
            if nd < dist.get(j, 1e30):
                dist[j] = nd
                prev[j] = i
                heapq.heappush(q, (nd, j))
    reach = [g for g in ZONE_TRIS if g in dist]
    if not reach:
        return None
    goal = min(reach, key=lambda i: dist[i])
    path = [goal]
    while path[-1] != start:
        path.append(prev[path[-1]])
    path.reverse()
    nxt = path[1]
    shared = set(TRIS[start]["idx"]) & set(TRIS[nxt]["idx"])
    pts = [TRIS[start]["v"][TRIS[start]["idx"].index(i)] for i in shared]
    mid = (tuple(sum(p[k] for p in pts) / len(pts) for k in range(3))
           if pts else CEN[nxt])
    # Steer at the neighbour's CENTROID, not the shared-edge midpoint: the keys
    # move at 45 degrees, so a target sitting on the edge is orbited rather than
    # crossed, and "60 units past the edge" can even land on top of the player.
    # The centroid is unambiguously inside the neighbour, so any step towards it
    # crosses the edge.
    mid = CEN[nxt]
    return nxt, mid, path


def camera_basis(p):
    """Unit forward/right in the xz plane from the live camera."""
    fx = p["at"][0] - p["eye"][0]
    fz = p["at"][2] - p["eye"][2]
    n = math.hypot(fx, fz) or 1.0
    fx, fz = fx / n, fz / n
    return (fx, fz)


def main():
    global LOG
    LOG = Path(sys.argv[1]).resolve()
    steps = int(sys.argv[2]) if len(sys.argv) > 2 else 60

    cached_deltas = {}
    tri_hist = []
    stuck = 0

    for step in range(steps):
        here = settle()
        if here is None:
            print("no telemetry", flush=True)
            return 1
        # A wipe returns to the title (map 490), whose walkmesh has nothing to do
        # with map 23; the walker used to keep planning there and chase tri18
        # across the title screen.
        if here["map"] != WALK_MAP or here["canrun"] != 1:
            print(f"LEFT MAP {WALK_MAP} (map={here['map']} tri={here['tri']}); stopping",
                  flush=True)
            return 3
        hop = next_hop(here["tri"])
        if hop is None:
            print(f"NO ROUTE from tri{here['tri']}", flush=True)
            return 2
        nxt, mid, path = hop
        # Aiming at the ADJACENT triangle's centroid stalls when that centroid
        # is only a few units away: the shortest legal press overshoots past it
        # and the next plan aims back, so the walker orbits a triangle border
        # (observed cycling 396/397/403 on map 22 for 30 legs).  After a few
        # failures, aim several hops down the planned path instead -- a distant
        # target gives every step the same direction and the intermediate
        # triangles get crossed on the way.
        if stuck >= 2 and len(path) > 2:
            look = min(1 + stuck, len(path) - 1)
            mid = CEN[path[look]]
        if nxt == "ZONE":
            print(f"IN ZONE at ({here['x']},{here['y']},{here['z']}) "
                  f"tri={here['tri']}", flush=True)
            return 0
        # Measure the two cardinal keys' actual world deltas from where the
        # player stands.  The camera yaw is close to the movement basis but not
        # identical (aiming at the edge midpoint with the yaw basis oscillated
        # around tri247 without ever crossing into tri129), so measure instead of
        # assuming.  Undo drift does not matter: the plan is recomputed from the
        # player's real triangle next step.
        # Measuring costs ~5 s per step (two probe+undo pairs), which dominates
        # the walk's wall clock and matters because the route is 42 hops across an
        # encounter-heavy map.  The camera turns slowly, so reuse the previous
        # measurement and only re-measure on a schedule or when a step fails.
        if not cached_deltas or step % 8 == 0:
            deltas = {}
            for probe_key, undo_key in (("Up", "Down"), ("Right", "Left")):
                b0 = position()
                tap(probe_key, 0.4)
                if battle_active():
                    fight_until_done()
                    break
                a0 = fresh_position()
                tap(undo_key, 0.4)
                if battle_active():
                    fight_until_done()
                if a0 is not None and b0 is not None:
                    d = (a0["x"] - b0["x"], a0["z"] - b0["z"])
                    deltas[probe_key] = d
                    deltas[undo_key] = (-d[0], -d[1])
            cached_deltas = deltas
        else:
            deltas = cached_deltas
        dx0 = mid[0] - here["x"]
        dz0 = mid[2] - here["z"]
        want = math.hypot(dx0, dz0)
        # Rank all eight key combinations by the cosine between the move they
        # would produce and the direction we want.  Picking the best vertical and
        # the best horizontal independently chose Down+Right for a
        # mostly-southward target (both Left and Right are near-perpendicular
        # there, so noise decided), which pushed the player east every step.
        combos = [(), ("Up",), ("Down",), ("Right",), ("Left",),
                  ("Up", "Right"), ("Up", "Left"),
                  ("Down", "Right"), ("Down", "Left")]
        best_combo, best_cos = None, -2.0
        for combo in combos:
            px = sum(deltas.get(k, (0, 0))[0] for k in combo)
            pz = sum(deltas.get(k, (0, 0))[1] for k in combo)
            n = math.hypot(px, pz)
            if n == 0 or want == 0:
                continue
            cos = (dx0 * px + dz0 * pz) / (n * want)
            if cos > best_cos:
                best_cos, best_combo = cos, combo
        keys = list(best_combo) if best_combo else []
        pred = (sum(deltas.get(k, (0, 0))[0] for k in keys),
                sum(deltas.get(k, (0, 0))[1] for k in keys))
        print(f"leg {step}: tri={here['tri']} ({here['x']},{here['y']},"
              f"{here['z']}) -> tri{nxt} mid=({mid[0]:.0f},{mid[2]:.0f}) "
              f"want={want:.0f} keys={keys} deltas={deltas}", flush=True)
        if not keys:
            # Already on the edge midpoint: step straight at the neighbour.
            keys = ["Up"]
        combo = "+".join(keys)
        # Step size follows the distance left: a 0.6 s press moves ~90 units and
        # hops straight over the neighbour triangle when only a few units away
        # (observed sitting 7 units from the tri241/tri61 edge and overshooting).
        hold = min(0.75, max(0.12, want / 140.0))
        before = position()
        tap(combo, hold)
        if battle_active():
            fight_until_done()
            continue
        after = fresh_position()
        if (after is not None and before is not None and
                after["x"] == before["x"] and after["z"] == before["z"]):
            cached_deltas = {}          # the mapping moved: force a re-measure
        # A zero-move is not the only way the cached basis goes wrong.  At
        # tri314 the walker moved a full step every time and still ping-ponged
        # into tri317, because the camera had rotated and the 8-step-old
        # deltas predicted (+60,0) for a move that actually went (+81,+80).
        # Re-measure whenever the achieved move disagrees with the prediction.
        elif after is not None and before is not None:
            ax, az = after["x"] - before["x"], after["z"] - before["z"]
            na, np_ = math.hypot(ax, az), math.hypot(*pred)
            if na > 1.0 and np_ > 1.0 and \
                    (ax * pred[0] + az * pred[1]) / (na * np_) < 0.7:
                cached_deltas = {}
        if after is not None and after["tri"] != before["tri"]:
            print(f"  {combo}: tri {before['tri']} -> {after['tri']}",
                  flush=True)
        if after is not None and after["tri"] == nxt:
            stuck = 0
        else:
            stuck = min(stuck + 1, 8)
        # Ping-pong guard: A->B->A->B never converges, and the cache reset
        # above cannot help when both legs move as predicted but the planner
        # keeps re-aiming.  Detect the repeated pair and force a re-measure
        # plus a shorter, more careful step.
        if after is not None:
            tri_hist.append(after["tri"])
            del tri_hist[:-6]
            if len(tri_hist) == 6 and len(set(tri_hist)) <= 2:
                print(f"  ping-pong {set(tri_hist)}: forcing re-measure",
                      flush=True)
                cached_deltas = {}
                tri_hist.clear()
    return 0


if __name__ == "__main__":
    sys.exit(main())
