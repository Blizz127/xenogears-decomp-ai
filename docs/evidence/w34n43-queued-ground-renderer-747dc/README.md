# W34N43 — complete retail `wm_800747DC` queued ground-object renderer

Verdict: **FULL_BODY_TRANSCRIBED_AND_CERTIFIED**.

## Anchor

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `d195cf0afd61490e383a7da57e0c072aff692438`
- Retail authority: `disc/world_map.bin`, direct mapping
  `runtime = 0x8006FAF0 + file offset`, disassembled with
  `mips-linux-gnu-objdump`
- Correct retail boundary: `[0x800747DC,0x80074E58)`, `0x67C` bytes / 415
  instructions
- Date: 2026-08-30

The retired body and header incorrectly described this as an eight-section
terrain renderer extending to `0x800758C0`. A first-frame witness on the
accepted natural route instead established a live compact queue:

```text
BE38=4
D30C=0x800f1f30
records=(mode 0, mode 0, mode 1, mode 1)
packet roots=(0x800f1fb8,0x800f2240)
```

The old approximation returned with `BE38=4`; retail clears it at
`0x80074E1C`.

## Completed retail sequence

The helper now:

1. returns immediately for a zero queue count and otherwise constructs the
   fixed `(-16..16)` four-vertex quad in scratchpad;
2. copies the live camera matrix to scratch `+0x130`, installs the fixed
   `(0,0,4096)` axis, selects the active packet root, and walks exactly the
   `BE38` 8-byte records rooted through `D30C`;
3. expands each signed record X/Z by 12, samples its terrain height with
   `wm_80093978`, and obtains the terrain normal from `wm_80093740`;
4. constructs the orthogonal terrain basis through the retail two
   `OuterProduct12` / two `VectorNormal` chain;
5. preserves all three record modes: ordinary terrain alignment, uniform
   `0x1800` scale, and the mode-2 `RotMatrixY` / `MulMatrix` chain with
   `(0x1800,0x1000,0x4800)` scale;
6. composes camera R with the basis and transforms the signed camera-relative
   placement `(worldX-cameraX, height, cameraZ-worldZ)` through camera R/T;
7. projects the first three vertices, applies the signed FLAG gate, projects
   the fourth, selects the minimum of all four SZ values, and rejects depths
   at or beyond `0x1000`;
8. links accepted compact `0x28`-byte FT4 packets through the domain-aware
   guest OT seam and finally clears `D_8009BE38`.

The high-level `MulMatrix0` and `RotTrans` calls preserve the retail GTE
instruction semantics used here: PsyCross `MulMatrix0` computes the same
camera-left matrix product, and `RotTrans` stores MAC1..3 (not saturated
IR1..3) into the output VECTOR.

## Certificate

`pc_port/tests/run_w34n43_747dc.sh` passes at O0, O2, and nonrecovering UBSan
with strict warnings. It exercises zero and nonzero queues, all three retail
record modes plus the default mode, a negative RTPT FLAG, a `0x1000` depth
boundary rejection, two accepted records, and compact packet reuse.

Fifteen named mutants are detected:

| mutant | failure mode |
|---|---|
| M1 | fixed queue address instead of loading `D30C` |
| M2 | omit the last record |
| M3 | use a `0x10` rather than `0x08` record stride |
| M4 | pass X as the terrain-normal Z coordinate |
| M5 | reverse the second terrain-basis cross product |
| M6 | omit mode-1 uniform scaling |
| M7 | omit the mode-2 rotation/multiplication chain |
| M8 | omit camera subtraction and Z reversal |
| M9 | project the wrong third vertex |
| M10 | ignore the signed RTPT FLAG gate |
| M11 | select maximum rather than minimum depth |
| M12 | use a `0x0C00` rather than `0x1000` depth ceiling |
| M13 | use a `0x20` rather than `0x28` compact packet stride |
| M14 | omit guest OT publication |
| M15 | leave the queue count uncleared |

## Natural acceptance

The same first-frame state-only witness now reports:

```text
W34N43_747DC_PRE BE38_BEFORE=4
W34N43_747DC_POST BE38_AFTER=0 RETAIL_EXPECTED=0
```

The normal product build reports `LINK OK`. The detached accepted route used
the unseeded frame-601 exit schedule and naturally reached
`frames=601 D7CC=0`. Same-frame capture fulfillment and all standing hashes
remain exact:

```text
frame 60  4310197d3881367bc7859ef174dcf4b9afa8759f0b7dc2ca3eea94b0f84f1521
frame 120 8039c2f96b88e09491eaf1cba049d0efd829f146aa943da744f56c93e85b1c16
frame 600 2b636ae21af4bf0003f60d5e53f8ffd9a042fc5a72457e4c096cff19814e1a38
```

The route completed without a primitive-link or OT-adapter abort. The neutral
capture hashes are expected evidence that these small queued ground quads do
not alter the accepted view; the live `4 -> 0` queue transition proves the
completed body runs naturally.

## Bound

Only `[0x800747DC,0x80074E58)` is retired here. The next target must be chosen
from a fresh natural-route inventory; the former header's claimed tail through
`0x800758C0` belongs to separate retail functions and is not part of this
implementation.
