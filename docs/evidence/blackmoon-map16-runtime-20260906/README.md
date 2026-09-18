# MAP16 (Blackmoon Forest) runtime — CORRECTED: map16 is still "mostly black"

Date: 2026-09-06 UTC. Observation only; no source change in this pass.
No commit/stage/push.

## ⚠ Correction notice — read this first

**An earlier revision of this file claimed MAP16's object-loading gap had
closed** (model builds "3 → 67", frame "49.6% non-black" showing terrain, a
tree and a rope bridge). **That claim was WRONG and is retracted.**

Root of the error: a `XENO_FIELD_MAP=16` run does not stay on map 16. The log
shows `FieldLoad begin field=16` → `archive 0xd9` and then, later in the same
run, **`FieldLoad begin field=15`**. Both the "67 model builds" figure and the
good-looking frame were read *after* that transition and are **map 15's**, not
map 16's. The tell that prompted the recheck: a map15 run reported byte-identical
`model-build count=67 / actors=109` — the same numbers "map16" had produced,
which is not a coincidence one should accept.

Archive numbering confirms both maps load correctly and the mapping is
`(mapNum << 1) + 0xB9`: map15 → `0xd7` (380,928 bytes), map16 → `0xd9`
(112,640 bytes), map17 → `0xdb` (395,264 bytes).

## What MAP16 actually does (evidence class: observed)

Splitting `map16-boot.log` at the `FieldLoad begin field=15` line (line 99,880
of 100,061 — i.e. the warp happens near the very end of the run):

| | before the warp (真 map16) | after the warp (map15) |
| --- | --- | --- |
| model builds | **`count=3 / actors=11`** | `count=67 / actors=109` |
| frame content | **0.0 – 12.2% non-black** | 49.6% non-black |

Non-black ratio across the map16 run, in order:
`4.4%, 4.4%, 0.0%, … 12.2% (frame 540) … 49.6% (frames 840/900/960)`.
The jump to 49.6% is the warp into map15, not map16 improving.

`map16-ACTUAL-frame-000540-prewarp.png` is the honest picture of map 16: an
essentially black frame with one small brightly-coloured fragment and a faint
dark-blue region. That is exactly the state `OPEN_ISSUES.md` item 8 describes
("partially black — one foliage clump draws correctly-textured/lit, rest
black"), and `count=3` matches its recorded "model builds = 3 (6 groups) vs
MAP17's 90" precisely.

**MAP16 is therefore NOT fixed. OPEN_ISSUES.md item 8 stands and must not be
closed on this evidence.**

Files here:
- `map16-ACTUAL-frame-000540-prewarp.png` — real map 16, pre-warp.
- `MISATTRIBUTED-was-labelled-map16-is-map15.png` — the frame the earlier
  revision wrongly presented as Blackmoon Forest. Kept deliberately so the
  mistake is auditable rather than silently deleted.
- `map15-frame-000900-after-warp.png` — map 15 from its own run, for comparison.
- `map16-boot.log`, `map17-boot.log`.

## What IS true (evidence class: proven / observed)

These parts of the earlier revision survive the correction:

- **The build container exists** — my earlier "no container here / NOT_RUN"
  conclusion (repeated from several prior checkpoints) was wrong. It is a
  *podman image*, not a distrobox container, which is why `distrobox list` being
  empty misled us. `localhost/xenogears-dev-toolchain:current` has gcc 13.3,
  cmake 3.28.3, SDL2 2.30.0, OpenAL 1.23.1 and Mesa software OpenGL 4.5.
  (`localhost/xenogears-dev-img:latest` lacks cmake/SDL2/OpenAL — not usable.)

  ```bash
  podman run --rm --security-opt label=disable --userns=keep-id \
    -v /var/home/blizz/Projects/xenogears-decomp-ai:/home/blizz/Projects/xenogears-decomp \
    -w /home/blizz/Projects/xenogears-decomp \
    localhost/xenogears-dev-toolchain:current bash -lc './pc_port/build_port.sh'
  ```

  `--userns=keep-id` is required (otherwise the mount is read-only to the
  container user). The mount path drops the `-ai` suffix, which also satisfies
  `tools/gears`'s hard-coded project-directory-name check. Headless running uses
  `SDL_VIDEODRIVER=offscreen`; there is no Xvfb in the image.

- **`build_port.sh` links**: `LINK OK -> pc_port/build_native/xeno-port`,
  74 function stubs after this session's decompilation work (was 77).

- **No crashes anywhere.** Maps 16, 17, 1 and 15 each ran a 30–35 s window at
  `RC=124` (clean timeout) with **zero** asserts, SIGSEGV, `ptag length`
  errors, or `missing D_8004FE50` reports. The only `[stub]` line is the benign
  `SoundHandleError`. Map 16's texture archive drains fully
  (`sectionsLeft=0 stripsLeft=0 done=1`).

- **Map 15 renders well** — 100% non-black, showing a cliff face, a pine tree, a
  rope bridge with hanging vines, the player sprite and water. This is a genuine
  result, just for map 15 rather than map 16.

- Model builds for the maps that do work: map17 `90/132`, map1 `96/143`,
  map15 `67/109`.

## Harness gotchas worth keeping

- **`XENO_FIELD_MAP=N` does not pin the map.** Scripts can warp the scene
  mid-run. ALWAYS split a log on `FieldLoad begin field=` before attributing any
  metric or frame to a map, and cross-check the drained archive number against
  `(mapNum << 1) + 0xB9`.
- **`model-build count=N / actors=M` is a RUNNING total**, so early lines are
  legitimately small — but the LAST line is only meaningful if the run never
  changed maps. This is precisely how the retracted claim was produced.
- **The "PNG" captures are BMP.** `XENO_FIELD_CAPTURE_DIR` writes
  `field-frame-%06d.png` containing 640x480 24bpp BMP data (magic `42 4d`,
  bottom-up rows). Viewers reject them by extension; convert first.
- `/tmp` here is a 12.3G-quota tmpfs. Under quota exhaustion GCC 2.7.2's `cc1`
  has been seen emitting objects with **no function bodies while exiting 0**, so
  set `TMPDIR` off tmpfs before trusting any byte comparison.

## MAP16 root cause has MOVED (evidence class: observed)

`OPEN_ISSUES.md` item 8's stated cause — "the object loader `func_800A1364` is a
no-op stub, so the IP never advances, `D_800B2264` stays 0 and the forest
objects never load" — is **no longer what is happening**. Measured on the
current build:

- The object overlay is fully active on map16: **65,306 `[obj-ovly] draw-pkt`
  and 34,028 `C700` lines** in a single pre-warp run. Slots 0–3 instantiate with
  real models (`742C slot=N ... groups=20 / 16`), nodes transform with sane
  values (`otz` ≈ 2276–3312, stable translations ≈ `(-7000, 1000, 13500)` — no
  runaway accumulation between early and late frames).
- `func_800A1364` is now byte-exact and installed (see
  `docs/evidence/blackmoon-object-overlay-verification-20260906/`).

So objects **do** load. What actually fails is projection/culling, and it
degrades over the run:

| frame | seen | emit | overlap | gte31 |
| --- | --- | --- | --- | --- |
| 60  | 3368 | 110 | 3058 | 56 |
| 120 | 3368 | 107 | 3148 | 1019 |
| 180 | 3368 | **3** | 3362 | **3205** |

By frame 180 essentially every primitive is either overlap-culled or hitting a
GTE bit-31 error, which is exactly the 0.0%-non-black frame measured above.
For contrast map17 at steady state is `seen=1169 emit=46 overlap=1106
gte31=10`.

**The camera was investigated and exonerated.** A first reading of the *last*
log line showed `camMode=1 eye2=0,0,0` and looked like a "camera never armed"
bug; that line is post-warp map15 re-init. Across the actual map16 window the
camera is valid and stable — `eye2 = eye = (-1442, 1662, -58)`,
`at2 = (-2000, 1500, 0)` — from frame 2 onward, and only 149 of 511 sampled
frames have a zero eye. Camera mode 1 with `camFlags=0000` is consistent:
`func_80073230` publishes `g_CamEyeMovementCurrent` into `g_CameraEye2` while
the movement is active and the value legitimately persists after the flag
clears.

Note the scripted framing: `at2.vy = 1500` while the player is at `vy = -540`,
i.e. the mode-1 camera is aimed ~2000 units above the player. Whether that is
retail-correct for this scene, and whether the missing geometry is simply
outside that framing, is **not yet determined** and is the obvious next thread.

A temporary, opt-in diagnostic was added to the existing `XENO_CULL_CAM_LOG`
line in `pc_port/src/game_overrides.c` (`camMode`, `camFlags`, `eyeCur`) and is
what produced the table above. It is gated behind that env var and marked
TEMP-DIAG.

## Still open

- MAP16 still renders essentially black. **But item 8's recorded cause is stale
  and should be rewritten, not closed**: objects load, the byte-exact loader is
  installed, and the failure is now projection/culling (rising `gte31`,
  `overlap` ≈ 99% of `seen`). Next thread: determine whether the mode-1 scripted
  framing (`at2.vy=1500` vs player `vy=-540`) is retail-correct, and why GTE
  bit-31 errors climb from 56 to 3205 over ~120 frames while object transforms
  stay bounded — a growing error count with stable inputs points at accumulating
  GTE/matrix state rather than at the geometry itself.
- Why `XENO_FIELD_MAP=16 XENO_FIELD_ENTRANCE=0` warps to map 15 was not
  investigated; it may be an authored script transition, an edge-spawn warp
  trigger, or a harness artifact of entering without the transition state.
- Whether map 16 is in fact Blackmoon Forest is still unconfirmed independently
  of `OPEN_ISSUES.md`'s label.
- MAP3's SEGV was not exercised in this pass.
