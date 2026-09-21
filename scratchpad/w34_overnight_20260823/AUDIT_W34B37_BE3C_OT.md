# W34B37 audit — BE3C+0x70 leak and held-backedge census

Date: 2026-08-23

## Starting proof

HEAD was `0cfeaaeb`. Tracked dirt was exactly the quarantined
`include/psyq/inline_c.h` and `pc_port/src/game_overrides.c`. The rebuilt
W34B36 route reproduced frame 916, upload counts 2 and 3, scheduler pass 2
29 executed / 0 missing, and the held `D554=1` backedge at `0x800719C8`.

## 1. Retail BE3C+0x70 mechanism

Retail `D_8009BE3C` is not an allocation pointer and `+0x70` is not a
rotating slot. It selects one of two fixed `WorldFrameBuffer` records:

```text
buffer A = 0x8009BBC8, stride 0x78, OT root field +0x70 = 0x8009BC38
buffer B = 0x8009BC40, stride 0x78, OT root field +0x70 = 0x8009BCB0
```

The record contains the draw environment at `+0x00`, display environment at
`+0x5C`, OT root at `+0x70`, and a second packet/OT buffer pointer at `+0x74`.
Retail `0x8007369C` allocates two `0x1000`-byte OT arrays. The frame head
selects and flips BE3C between A and B; `0x80071448` reads the selected
`+0x70` for `ClearOTagR(ot, 0x400)`, and `0x800719B0` reads it again for
`DrawOTag(ot + 0xFFC)`. The `0x1000` allocation exactly matches 1024 OT
words.

## 2. Current-port writer inventory

| source / site | operation | finding |
| --- | --- | --- |
| `world_map_init.c:2198-2201`, `wm_8007369C` | two `HeapAlloc(0x1000, 0)` results stored at `BC38` and `BCB0` | raw `(u32)(uintptr_t)p`; truncates the host pointer instead of publishing KUSEG |
| `world_map_frame_driver.c:293,483,488` | writes BE3C | writes fixed guest `0x8009BC40` and flips to guest `0x8009BBC8/0x8009BC40`; correct |
| `world_map_callback_925a0.c` | reads `context+0x70`, links quad/DR_TPAGE tags | consumer only; does not assign the field |
| `world_map_helper_73b04.c` | reads DB `+0x70`, inserts projected packets | consumer only; does not assign the field |
| `world_map_frame_driver_712d0.c` | legacy full driver | stale/generated body with raw host-boundary calls; natural prologue uses `wm_800712D0_frame_prologue`, and the old entry has the existing `wm_800712D0_should_not_run` guard |

The source already contains `host_ptr_to_psx_u32`, and other newer allocation
paths use it correctly. BE3C itself is initialized on the natural route with
guest addresses and remains semantically correct. The observed
`0x005f1068` was the truncated host result of the OT allocation, not a bad
BE3C value.

Classification: class (a), a single writer conversion defect.

## 3. Attempted minimal fix and stop condition

The working tree contains the minimal writer conversion and the necessary
downstream `ClearOTagR` map at the existing `0x80071468` host boundary:

```c
WM_U32(WM_ALLOC_BC38_ABS) = host_ptr_to_psx_u32(p);
WM_U32(WM_ALLOC_BCB0_ABS) = host_ptr_to_psx_u32(p);
ClearOTagR((u32*)PSX_ADDR(ot), WM_FP_OT_COUNT);
```

The focused production-linked conversion certificate passed O0/O2/UBSan with
asymmetric guest fixtures. The production rebuild was `LINK OK`.

With only the writer conversion, the natural route exposed the next raw
guest-pointer defect: `ClearOTagR` crashed before the tail. After the minimal
reader map, the natural route reached `DrawOTag` with `p=0x5f2064`, the host
mapping of a valid guest OT submission (`g_PsxRam+668196`), and then crashed
inside `ParsePrimitivesLinkedList` at `PsyCross/src/psx/LIBGPU.C:456`.

This is the explicit stop condition. It proves the writer leak is fixed far
enough to make DrawOTag fire, but the OT linked-list representation consumed
by PsyCross is not yet valid. No workaround, fake OT, or second backedge was
implemented. The attempted production changes remain uncommitted because the
natural run did not complete with `rc=0`.

## 4. Retail backedge re-derivation

Retail `0x800719C8` is `bnez v0,0x8007130C`, with `v0` loaded from D554 at
`0x800719C0`. `0x8007130C` is the frame-head pad-accumulator clear, after the
one-time D554 seed at `0x80071308`; it is not the scheduler call itself. The
frame head then drains input and reaches scheduler pass-start at the normal
`0x80071064` caller site. A real backedge would therefore re-enter the frame
head and then the scheduler on the next frame.

## 5. Full scheduler slot census at the held tail

Pool base at the census was `0x800D7538`; stride is `0x80`; state is signed
halfword `+0x00`, cb0 `+0x18`, cb1/occupancy `+0x1C`.

| slot | state | timer | cb0 | cb1 / occupancy | next action |
| ---: | ---: | ---: | --- | --- | --- |
| 0 | 1 | 0 | `0x800923A8` | `0x800925A0` | cb1 `0x800925A0` |
| 1 | 1 | 0 | `0x8008A2C8` | `0x8008A72C` | cb1 `0x8008A72C` |
| 2 | 1 | 0 | `0x8008B2BC` | `0x8008B644` | cb1 `0x8008B644` |
| 3 | 3 | 0 | `0x8008BB40` | `0x8008B644` | dormant |
| 4 | 1 | 0 | `0x8008C530` | `0x8008C844` | cb1 `0x8008C844` |
| 5 | 1 | 0 | `0x8008D3F0` | `0x8008D678` | cb1 `0x8008D678` |
| 6 | 3 | 0 | `0x8008DD6C` | `0x8008D678` | dormant |
| 7 | 3 | 0 | `0x8008E190` | `0x8008E76C` | dormant |
| 8 | 1 | 0 | `0x800906E0` | `0x800907F4` | cb1 `0x800907F4` |
| 9 | 1 | 0 | `0x80091430` | `0x800914D0` | cb1 `0x800914D0` |
| 10 | 1 | 0 | `0x80091B54` | `0x80091C18` | cb1 `0x80091C18` |
| 11 | 3 | 0 | `0x80092234` | `0x800922AC` | dormant |
| 12 | 1 | 0 | `0x80092BE4` | `0x80092C70` | cb1 `0x80092C70` |
| 13 | 1 | 0 | `0x80092DF8` | `0x80092FD8` | cb1 `0x80092FD8` |
| 14 | 1 | 0 | `0x80071A50` | `0x80071A58` | cb1 `0x80071A58` |
| 15 | 1 | 0 | `0x80087710` | `0x80087734` | cb1 `0x80087734` |

All twelve predicted cb1 addresses resolve to implemented current-port
bodies. No callback was newly exposed by the census; no callback slice is
queued. The backedge is callback-safe in principle, but the DrawOTag/OT
representation crash must be reviewed first.
