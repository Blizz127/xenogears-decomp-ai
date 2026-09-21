# Type-9 render callback audit — 2026-09-06

Read-only audit of `/var/home/blizz/Projects/xenogears-decomp-ai`. No production/test edits, build, desktop interaction, runtime replay, commit, or process manipulation. Sole authored artifact is this report. Scope: retail `D_8004FD40[9] -> 80025544`. Root retains all production ownership.

## Result

Retail callback emits an untextured square TILE and an E1 draw-mode packet. It is not a text renderer. There are no trig, font, dialogue, texture-upload, model, or battle-overlay calls in its exact extent. The existing `temp1.c` implementation is disconnected and materially differs from retail; simply wiring it to native table slot 9 is incorrect. A small corrected native leaf plus slot-9 binding is sufficient at the inspected API boundary; full runtime visibility and relation to the scripted battle remain unproven.

## Pins and inspected state

- Branch `experiment/worldmap-open-gates-20260823`; HEAD `3a3e7aac03a2f166fb924945a489e392d706f282`, verified live. Extensive pre-existing dirty tracked/untracked work preserved. The root is the known concurrent writer. A process-name snapshot found no running xeno-port, build_port, or Claude process; this does not establish absence of other agents/editors.
- Retail `disc/SLUS_006.64` SHA-256 `dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119`, verified live. File offset for PC = `PC - 8000F800`.
- Retail `disc/battle.bin` SHA-256 `1830b4ef1fe37129972fc310dfad534f8161d6c0b123e74254c3711334a3e291`, verified live; this callback belongs to SLUS, not battle.bin.
- Exact callback bytes `[80025544,80025710)` = 0x1CC bytes, SHA-256 `66f10552f9af792cfe90720e086429dec9fe4801fa0cc31079a8fde65a34eba8`. Last callback instruction is return delay-slot NOP at 8002570C. `80025710` begins the separate empty callback.
- Read raw retail table at `[8004FD40,8004FD80)`: `80025258 80025710 80025718 0 0 80025258 80025258 80025718 8002541C 80025544 0 0 0 0 80025258 800257F0`. Slot 9 is at 8004FD64.
- Run directory `pc_port/build_native/opening-c0-transform-5-0qxtsrd_` exists. Its root-provided unsupported-A5/type-9 warning is context, not an independently reproduced observation in this audit.

## Exact callback mapping

| Retail PC | Behavior |
|---|---|
| 2555C–25570 | Load **32-bit** sprite pointer from task+4. Return if sprite unsigned-halfword +34 is nonzero. |
| 25574–25588 | Read unsigned-halfword size from sprite+36. Work cursor from GP+410 (`g_GfxCurWorkBuffer`, 80059580); end from GP+3C4 (`g_GfxCurWorkBufferEnd`, 80059534). Require unsigned `(cursor+16) < end`, strictly; otherwise return. |
| 25590–255C0 | Build SVECTOR from signed halfwords sprite+02/+06/+0A, write cursor+16, call SetRotMatrix(80049EFC) and SetTransMatrix(80049F8C), both on `D_8004FBB8`. |
| 255C4–2561C | Copy v0 to v1; replace v1.x with low16(v0.x + unsigned size), preserving signed 16-bit interpretation at GTE load. Pass **v0, v1, v0** to RotTransPers3(8004A67C). sxy0 writes directly tile+8, sxy1 to stack+30, sxy2 to stack+38. Both p and flag addresses equal stack+3C. |
| 25620–25630 | `depth = (s32)RTP3_result >> (D_80050100 & 31)` via SRAV. Store depth low16 into sprite+2E. Unlike billboard callback, there is **no FLAG/depth rejection and no sprite+30 bias**. |
| 25634–25658 | `dimension = (s16)xy1.x - (s16)xy0.x`; if zero use 1, then absolute value. Difference range fits s32; final positive dimension is 1..65535. |
| 25660–2567C | Half = dimension/2, truncation/floor for this positive value. Subtract half from both original projected x0 and y0, then write low16 of each to tile+8/+A. A dimension of 1 has half=0; odd 3 has half=1. |
| 25680–256A8 | tile[3]=3; copy all four sprite bytes +28..+2B into tile+4 (RGB and GP0 opcode). Write dimension low16 to both tile+C and tile+E. Load `g_GfxCurOT` (8005956C), add **depth shifted left 2 as 32-bit arithmetic**, then AddPrim(80043B48). |
| 256AC–256C4 | Reload cursor/end. Require unsigned `(cursor+8) < end`, strictly. If insufficient, return after keeping already-linked TILE and cursor advance. Otherwise advance cursor by 8. |
| 256C8–256EC | mode[3]=1; mode word +4 = `E1000000 | (sprite_u32_at_3C & 60)`. AddPrim to the same depth slot. LIFO insertion leaves **mode -> tile -> prior head**. |

Retail dependencies are only the sprite, task, camera matrix, depth-shift global, current work-buffer pointers and OT plus four SDK leaves. The caller must provide valid buffers/OT depth; retail imposes no additional depth clamp here. Adding a billboard-style 1..4095 filter would change semantics.

Type-9 post-init in `game_overrides.c:3238` currently sets +36=3, +2B=60, +34=1. Thus callback registration alone does not guarantee a tile: a later update must bring +34 to zero. This post-init is an inspected native-source observation, not an independent retail audit of 80024730. The sprite/script responsible for the current run's type-9 warning and the update that opens +34 remain unresolved.

## GTE and trig

There are no trig calls in this leaf. Existing shim rsin/rcos remapping is irrelevant to this callback; camera matrix provenance remains a separate caller dependency.

Retail `RotTransPers3` extent `[8004A67C,8004A6D0)` SHA-256 `6dd2d4421dbd97f4d19df2c97fcc9c62ae767fbc5ffe3650e46470f37489e9e3` loads GTE data registers 0..5 from the three SVECTORs, executes raw word `4A280030` (RTPT), then stores SXY0/1/2 (regs12/13/14) and IR0 (reg8), reads FLAG (control31), reads SZ3 (data19), writes FLAG, and returns `SZ3 >> 2` in the JR delay slot. It does **not** AVSZ3-average the depths. Callback then applies the additional variable shift.

Because v2 repeats v0, returned SZ3 and final RTPT IR0 follow v0, not a y-offset vertex. Preserve packed XY as one 32-bit word: x=(s16)xy and y=(s16)((u32)xy >>16). For native PsyCross, use local host-long output storage and explicitly copy low32 into packed primitive fields; do not hand it arbitrary four-byte packed output addresses because native long can be eight bytes. Existing battle `bridge_rot_trans_pers3` at runtime.c:441 already marshals host-long outputs back as four-byte stores and preserves output write order/aliasing. For native code, an unused local shared p/flag argument can preserve the retail alias.

## Existing draft defects

`src/slus_006.64/system/temp1.c:1851` currently:

1. Reads `*(u8**)(task+4)` (8 bytes on LP64), crossing into packed task+8 callback data, and omits guest alias translation.
2. Uses v2=(x,y+size,z), whereas retail uses v0 again.
3. Reads `pxy0[1]` for y, despite y being high16 of the first packed word; the second host-long element is not written by RTP3.
4. Sets half to `(abs(dx)+1)/2`; retail uses `abs(dx)/2` after zero->1.
5. Writes raw sprite size to width/height, whereas retail writes the projected absolute x difference.
6. Computes OT address as `g_GfxCurOT + otz*4`, multiplying again by host `sizeof(u_long)`; native packed OT needs a byte pointer plus `(u32)depth << 2`.
7. Calls plain AddPrim on a potentially guest-resident OT/primitive. PsyCross's nonextended AddPrim stores low24 of its provided host pointer; this does not translate g_PsxRam host offsets to guest links.
8. Uses unmasked C variable shift; MIPS SRAV masks shift count to five bits.

Correct code must retain the strict buffer boundary checks and the partial-success TILE-only path already visible in the draft. No tests were executed against the draft; the defects above are direct raw-instruction comparisons.

## Packed primitive / OT compatibility

Retail AddPrim `[80043B48,80043B84)` is two **four-byte** tag writes: `prim.tag=(prim.tag&FF000000)|(ot.tag&FFFFFF)` and `ot.tag=(ot.tag&FF000000)|(prim_address&FFFFFF)`. It preserves both tags' high-byte lengths.

`pc_port/src/guest_prim_link.c:43` implements exactly these link semantics with address-domain translation when OT and primitive are inside g_PsxRam. Use `PcPort_AddPrimDomainAware` on resolved host pointers for this native leaf. Both packets live in the graphics work buffer; if the OT is guest-resident, the packet must also be in guest RAM or the helper rejects it. Preserve this existing fail-closed behavior.

Build uses `USE_EXTENDED_PRIM_POINTERS=0`; PsyCross `libgpu.h` also adds an eight-byte native `OT_TAG` padded for compiled host u_long[] tables. Do not infer retail OT stride from `OT_TAG`, `u_long`, or host SDK primitive structure sizes. Explicit packets are TILE=16 bytes (3 GP0 words) and mode=8 bytes (1 GP0 word). g_GfxCurWorkBuffer is declared as pointer storage in temp1.c but as u32 in game_overrides.c; follow the existing owning translation unit rather than introducing another conflicting extern declaration.

Battle `bridge_draw_otag` (runtime.c:603) walks four-byte guest DMA tags, resolves packet addresses, passes GP0 payload bytes to PsyCross, and draws at the terminator. It already handles length3 and length1 payloads generically; no new renderer command or draw callback is indicated by this audit. Actual tile pixels remain NOT_OBSERVED.

## Smallest implementation path

1. Correct the existing temp1.c leaf with 32-bit packed pointer resolution (reuse SpriteRenderAddress), exact v0/v1/v0 projection, width/centering/XY extraction, 32-bit shift/OT arithmetic, and domain-aware AddPrim. Add required include/declaration in its owning unit; no duplicated native implementation is necessary.
2. Bind only native table slot9 in game_overrides.c to this function. Keep actual retail NULL slots and unrelated unported slots unchanged. Existing billboard binding test expects slot9=NULL, so its binding expectation needs a scoped adjustment accompanying the real new binding.
3. Validate below before naturally resuming the opening. This is a rendering omission repair, not evidence that it fixes the A5 dispatch, Fei text, or full battle exit.

Alternative: exact guest leaf execution would require explicit narrowly bounded main-EXE dispatch and byte residency for `[80025544,80025710)`, main-SLUS shared-global mappings, native/guest pointer reconciliation, four-argument/stack RTP3 bridge handling and AddPrim domain handling. `target_is_guest_code` (runtime.c:374) currently permits battle and overlay ranges only; `PcPort_BattleMipsDispatchCallback(80025544,...)` therefore returns 0. `runtime_bridge_call` otherwise resolves this address to a host function or fallback name. Broadly declaring all main EXE addresses guest-executable would change ownership for unrelated functions and is not a minimal change. The existing isolated retail oracle tests can execute the exact leaf through a local test bridge without changing production dispatch ranges.

## Bounded validation strategy (NOT_RUN)

- Pattern after battle_child_billboard_retail_test: load exact 0x1CC disc slice, assert its SHA, run via existing MIPS adapter with controlled SDK leaves; compare the production leaf's complete sprite bytes, work-cursor updates, primitive bytes, all touched OT words, matrix call order, GTE inputs and output alias order. Pin extracted production source and link helper too.
- Test +34 nonzero; first allocation cursor+16==end and >end (no state change); exact one-byte spare success; second cursor+8==end (TILE retained, no mode), >end and <end. Use poisoned allocation/OT neighbors.
- Projected dx 0,+1,-1,+3,-3, large positive/negative differences; nonzero packed y distinct from x; raw sprite size distinct from projected dimension. Test x+size signed-halfword wrap, repeated third vector, flags set (still emits), SZ3>>2 plus shift counts0/1/31/32/33, and zero/negative depth with an intentionally valid oversized fixture region to observe address calculation without inventing a production clamp.
- Test KSEG0, KSEG1 and resolved native RAM pointers, task+8 poisoned to kill widened pointer reads, mode bits00/20/40/60 plus unrelated flags, RGB/opcode copy, preserved prior tag high bytes, and exact mode->tile->old links. Guest OT guard cells distinguish 4-byte addressing from host-scaled stride.
- Negative controls must reject NULL slot9, raw-size dimensions, ceil half, y from next host long, v2.y offset, 64-bit task read, unmasked shift, incorrect OT stride, plain host-pointer AddPrim, swapped insertion order, inclusive allocation, added depth/FLAG rejection.
- Separate real-GTE integration: use actual PsyCross GTE, camera/projection state and actual retail RTP3 instruction semantics for selected rotated/perspective vectors; compare packet bytes and GTE state. Controlled-leaf parity alone does not prove projection correctness.
- Natural runtime: record which type9 child gets registered, script pointer, task/sprite, +34 transition to0, mode/TILE emission and consumed OT links. Then capture pixels. Only after normal scripted progression can opening Gear battle / Fei text / full exit be evaluated. No planted host state, forced frame field, or jump to an exit gate.

## Outstanding uncertainty

No runtime replay, pixel proof, implementation test, or dialogue causal proof was performed. Current unsupported A5 may prevent future type9 activity; neither omission is proven to cause Fei text failure. The exact type9 child, its +34 enabling script, its current camera/depth values, and guest OT/work-buffer domains at the relevant live frame are unobserved. Full battle exit remains outside this static audit's proof.

## Snapshot source hashes

- `src/slus_006.64/system/temp1.c`: `494f6f78dc51aaf65e57d1bad5bc7a06e288059134de89c907648e709032d441`
- `pc_port/src/game_overrides.c`: `62088396db6b712eb2030eb650366e1cf7680c06c17bafcb052cc28bdd28d733`
- `pc_port/src/guest_prim_link.c`: `b46642a65411847537abf1f64a0681dd8cf4ed5beebde5e9509186b8bd224dc8`
- `pc_port/src/battle_mips_runtime.c`: `fd4fae9b90677538e8ec7b754e814690d2de6434c39165a512c32b7ca07a6d80`
- `pc_port/extern/PsyCross/include/psx/libgpu.h`: `97dc22c5d2830eed91cf0ca74f608d67d4cc80c7d620b2dffbb963772214626b`
- `pc_port/extern/PsyCross/include/psx/gtemac.h`: `80317aed820dcf15af477ce9fccef0fe10c014bd667db27a103ef76feec2bb1e`
