# Phase Log

Chronological summary of **major committed milestones** and documented investigation checkpoints. Uncommitted read-only proofs are marked.

For session-level detail see [`ACTIVE_HANDOFF.md`](https://github.com/Blizz127/xenogears-decomp-ai/blob/main/docs/ai_context/ACTIVE_HANDOFF.md).

---

## 2026-07-05 — Field visual recovery (Map 0)

| When | Milestone | Commit / evidence |
|------|-----------|-------------------|
| Jul 5 | Work-list path restored; sprite/fade/zoom visible again | `work_list_port.c`; user visual confirmation |
| Jul 5 | `FieldScene` host layout fix (`unk7C` size) | Handoff "Last Changes" |
| Jul 5 | `func_8007254C` projection defaults (`sceneScrZ`, `sceneDIP`, `sceneScale`) | Handoff runtime evidence |
| Jul 5 | `func_8009AD6C` actor direction handler; actor 18 correctly hides | `2627286` |
| Jul 5 | `XENO_FIELD_MAP` harness selector added | `port_main.c` |
| Jul 5 | Map1 stack-smash in `func_80080A74` identified | Read-only; later fixed |
| Jul 5 | `func_800764B4` active-actor quad path implemented | Committed in session |
| Jul 5 | Anim-script opcode 0xA0 implemented (bounded) | Handoff Jul 5 entry |
| Jul 5 | `XENO_FIELD_ENTRANCE` spawn selector (`D_8006F954`) | Handoff Jul 5 entry |
| Jul 5 | Map1 Z-clamp bug in `func_8007CD80` fixed; actor visible | Handoff Jul 5 VISUAL MILESTONE |
| Jul 5 | Map1 camera target corruption root-caused (`func_80072A38`) | Read-only → clamp fix |

## 2026-07-06 — Map1 rendering / VRAM pipeline

| When | Milestone | Commit / evidence |
|------|-----------|-------------------|
| Jul 6 | Field background uploader `func_8002BF38` ported | Handoff VRAM section |
| Jul 6 | Model prim `0x0C` F4 walker fix (`ModelPrimQuadF4Variant0`) | Handoff Bug 6 |
| Jul 6 | `PcPortDrain0xBBToVram` opt-in upload (`XENO_FIELD_0BB_VRAM_UPLOAD=1`) | `archive_port.c` |
| Jul 6 | TR-add proven retail-correct for model matrix (`func_800748E8`) | Read-only proof |

## 2026-07-07 — Player control and field progression

| When | Milestone | Commit / evidence |
|------|-----------|-------------------|
| Jul 7 | `func_8001F8E8` non-delegating sprite-frame path | Committed before 0xA7 |
| Jul 7 | OP_UPDATE_CHARACTER (opcode 0xA7) player branch | `e7b0101` |
| Jul 7 | Walk-step vector `func_80081F80` | `79c7ee7` |
| Jul 7 | Walkmesh triangle lookup + movement | `55a2dda` |
| Jul 7 | Collision auto-move `func_80082620` + `func_800825AC` | `6bc1752` |
| Jul 7 | **First visible field control** — real keyboard moves Fei on Map1 | `b9266c5` |
| Jul 7 | Map1 zone 5 encounter trigger fires (read-only) | `94d8cfc` (docs) |
| Jul 7 | Map1 exit/transition system found (zones 8–11) | `0e2c181` (docs) |

## 2026-07-08 — Map1 exit transition investigation

| When | Milestone | Commit / evidence |
|------|-----------|-------------------|
| Jul 8 | Zone 11 physically reachable; height gate passes | Read-only gdb |
| Jul 8 | Opcode 7 stall: slot-init offset bug in `func_80080A74` | `213b826` (docs) |
| Jul 8 | Slot-init fix exposed `func_80098CAC` pSprite bug | `7441ca9`, `ef1015f` (docs) |
| Jul 8 | Slot init + pSprite fixes; op7 advances to op116/op54 | `9e1b667` |
| Jul 8 | `func_800855C8` classified as sound-side | `36f195d` (docs) |
| Jul 8 | Field transition sound shim committed | `ae8c753` |
| Jul 8 | `func_80079288` classified as encounter bookkeeping | Docs on `ae8c753` |
| Jul 8 | Var `0x0462` decoded as local exit latch | `4856738`, `e13c0e3` (docs) |
| Jul 8 | Actor 18 routine 4 traced — door animation only | `bb31b27` (docs) |
| Jul 8 | Actor 18 routine 1 gated by `var0x0408 == 1` | `0545ef3` (docs) |
| Jul 8 | Actor 20 owns `var0x0408`; host rand range mismatch | `8db4fb2` (docs) |

---

## Milestone demo command (field control)

Documented in `b9266c5`:

```bash
distrobox enter xenogears-dev -- bash -lc 'cd /home/blizz/Projects/xenogears-decomp && \
  SDL_VIDEODRIVER=x11 XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 \
  XENO_FIELD_MAP=1 XENO_FIELD_ENTRANCE=0 XENO_FIELD_0BB_VRAM_UPLOAD=1 \
  ./pc_port/build_native/xeno-port'
```

Fei = actor 1. Arrows = D-pad; C = run/Cross. `XENO_FIELD_ENTRANCE=8` also works.

## Protected baselines

- **Map0 visual baseline:** 6 sprite actors `1, 2, 16, 23, 25, 26`; actor 18 hidden by script.
- **Do not use entrances 6 or 10** on Map1 — harness-invalid spawn indices.
