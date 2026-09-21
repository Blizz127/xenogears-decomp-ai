# W34N47 — retail tiled-object renderer `0x80086798`

## Result

`RESTORED_VERIFIED`

Starting HEAD was `f0f1bac60194164287642d52302aaaa9f29a7935`, equal to
`origin/experiment/worldmap-open-gates-20260823`.  The naturally reached
139-line sketch has been replaced by the bounded retail renderer
`[0x80086798, 0x80087710)` (990 instructions / 3960 bytes).

The old header's `[0x80086798, 0x80087904)` / 1115-instruction boundary was
wrong.  The fresh world-overlay call graph establishes `0x80087710` as the
next callback boundary.

## Retail authority

The authoritative input is `disc/world_map.bin`, decoded with zero runs
enabled so the GTE-heavy body is not elided:

```text
mips-linux-gnu-objdump -Dz -b binary -m mips:3000 -EL \
  --adjust-vma=0x8006faf0 \
  --start-address=0x80086798 --stop-address=0x80087710 \
  disc/world_map.bin
```

The direct always-live base-world call is retail `0x80071B60`.  Fresh
call-graph authority is `scratchpad/world-callgraph-v2/FUNCTIONS.csv`.
`scratchpad/w34n47_86798_full.objdump` was the line-by-line decode used for
the transcription.  An official `m2c` lift was used only as a control-flow
cross-check because it does not decode the GTE instructions.

## Retail behavior restored

The function now performs the complete retail sequence:

1. dispatches the guest callback at `D_8009CD40`; the naturally installed
   `0x80086700` record-advance callback retains the exact W34B25 body;
2. copies the 48 mid-detail quads, 12 far-detail quads, and eight UV roots
   into their retail scratch locations;
3. records both camera world coordinates and both distinct tile coordinates
   (`scratch+0x220` X, `scratch+0x228` Z);
4. constructs the pitch/heading object matrix and the two packed retail
   wedge boundaries;
5. advances and classifies 80 records relative to the wrapping world camera;
6. composes `camera × object`, installs the result in the GTE, and projects
   each surviving record origin;
7. selects the retail distance LOD:
   - depth `>= 1409`: three 64-pixel far quads;
   - depth `1025..1408`: twelve 32-pixel mid quads;
   - depth `<= 1024`: three groups of sixteen 16-pixel near cells;
8. preserves each path's distinct FLAG mask, screen test, UV subdivision,
   and far-depth early exits;
9. emits 40-byte, length-9 `POLY_FT4` packets through the established
   guest-domain primitive linker;
10. applies the retail packet ceiling only at the 80-record outer boundary.
    A record entered at count 240 may therefore fill the entire 288-packet
    allocation before the next record observes the `>= 241` stop.

The retail manual three-column matrix multiply plus translated-origin MVMVA
is exactly `CompMatrix(camera, object, output)` at this seam.  The focused
certificate asserts that operand order and the object translation handed to
it.

## Focused certificate

Runner: `pc_port/tests/run_w34n47_86798.sh`

```text
CERTIFICATE O0/O2/UBSan PASS; strict warnings clean
M1  callback omitted                 DETECTED
M2  Z tile overwrites X tile         DETECTED
M3  matrix composition reversed      DETECTED
M4  far LOD routed to near path       DETECTED
M5  packet publication omitted       DETECTED
M6  outer packet ceiling omitted     DETECTED
M7  far path uses strict FLAG mask   DETECTED
M8  mid UV shifts reduced            DETECTED
M9  near grid step 0x60 -> 0x80      DETECTED
M10 packet stride 40 -> 36           DETECTED
W34N47 0x80086798 FULL CERTIFICATE PASS; M1-M10 DETECTED
```

The certificate exercises all three LODs, exact template copies, callback
advancement, camera slots, rotation/multiply order, composed translation,
matrix installation, origin and quad projection seams, path-specific flags,
UV layouts, near-cell coordinates, tag/link behavior, packet stride, and the
288-packet maximum reached from the retail outer-boundary ceiling.

## Natural first-call witness

The normal binary's first natural call reported:

```text
callback       = 0x80086700
record root    = 0x800F24C8
aux root       = 0x800F29D0
packet roots   = 0x801CB78C / 0x801C8A84
buffer index   = 0
attempts       = 420
packets        = 1
link delta     = guest/native/rejected = 1/0/0
last FLAG      = 0x00001000
last origin SZ = 1418
```

The published packet was joined by its guest address:

```text
address = 0x801CB78C
tag     = 0x090A2268
XY      = FF970000 / FFC30046 / FFC3FFBA / 00000000
UV      = 3030 / 303F / 3F30 / 3F3F
```

The `0x0F` UV extent proves this is a naturally accepted near-grid packet.
It is not a native-pointer rejection or an incorrectly labeled far/mid
packet.

## Natural 601-frame acceptance

The final normal binary reached the accepted detached natural state exit:

```text
natural state exit: frames=601 D7CC=0
capture requests fulfilled at frames 60, 120, and 600
adapter abort/boundary diagnostics: none
upload unknowns: 0
```

Hashes remain byte-identical to W34N46:

```text
frame 60  cdc95854e30d798f5634defcdd138ac9f5963afa4de8872981343e77068a46e8
frame 120 d5483c023f2fdecb030c6d335774048d98bc4684e1c524d7a7c3622e175f26bd
frame 600 6e861aecdf5e804bcbf6bb46c002f872308bfe831763ca663e8f2254294c3b1e
```

This is a neutral visual result, not a dead-path result: the state witness
proves one packet is linked naturally, but it does not alter a captured pixel
at the accepted camera state.  Visual inspection confirms the same coherent
terrain/minimap scene banked by W34N46.  No capture hash is retired by this
rung.

The process reached the known post-world `SoundHandleError` stub after the
natural state-exit marker and was terminated only then; no whole-process
`rc=0` is claimed.
