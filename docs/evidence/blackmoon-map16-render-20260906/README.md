# MAP16 (field map 16) black render — root cause and retail-faithful fix — 2026-09-06

Working tree: branch `experiment/worldmap-open-gates-20260823`, HEAD `3a3e7aac`,
nothing staged, committed or pushed. Container `localhost/xenogears-dev-toolchain:current`
(`--userns=keep-id`, `TMPDIR=/var/tmp`). Final port binary
`pc_port/build_native/xeno-port` SHA-256
`a45f2bf15821feff7392441172c1d3b00b49d48429ad028a971515423d413a47`, `LINK OK`,
74 function stubs (unchanged count).

Evidence classes: **proven** (byte comparison, reproduced hash, or a
differential test against retail bytes), **build-verified only**,
**observed** (seen at runtime), **inferred**.

## 0. Summary

MAP16 is not a forest environment made of objects and it was not being
corrupted progressively. It is a short scripted cutscene: a fixed camera looks
through two foreground foliage models while four archive-6B9 objects (one of
type 0, three of type 8 — one leader and three identical followers, i.e. a
Gear flight) are lerped by the script from (-6000,4500,0)/(-7000,4500,±600)
to the origin, one after another, and then the map warps to field 15. The
"progressive gte31 collapse" is each object passing behind the camera as it
arrives (**proven**, §2). Everything the port drew of those objects was nearly
black because the port's `func_801E7D14` was an approximation that never set
the object colour/light matrices retail sets, composed node-local instead of
root-view × node-world matrices, ignored the object scale, and drew with walker
variant 0 instead of 1; on top of that a wrong blob offset in
`misc2.c FieldUpdateObjectActor` scaled every object by garbage and a
host-pointer-stride bug in `main.c` left the scale table unwritten for slots
1..3 (**proven**, §3). With retail's six-pass `7D14` and the retail
`801DCEC8` object draw transcribed, plus the two table fixes and the missing
variant-1 walker `0x8002ED20`, the objects render as lit, textured Gears
(**observed**, §5; frames in `frames/`).

## 1. What was actually black (observed, then proven)

`XENO_CULL_CAM_LOG` on the pre-fix binary, split on `FieldLoad begin field=`
(all rows below are field 16; the warp to field 15 happens at cull frame ≈389,
run `m16A`, `logs/before-A-cull-with-submitter-tags.log`). A temporary
per-submitter tag (since removed) split the counters:

| cull f | field models: seen/emit/overlap/gte31 | objects: seen/emit/overlap/gte31 |
| --- | --- | --- |
| 0 | 0 / 0 / 0 / 0 | 3312 / 1050 / 978 / 0 |
| 2 | 56 / 1 / 54 / 40 | 3312 / 10 / 3290 / 4 |
| 59 | 56 / 1 / 54 / 40 | 3312 / 97 / 3035 / 16 |
| 119 | 56 / 1 / 54 / 40 | 3312 / 2 / 3308 / 947 |
| 209 | 56 / 1 / 54 / 40 | 3312 / 599 / 1679 / 1079 |
| 269–359 | 56 / 1 / 54 / 40 | 3312 / 2 / 3308 / 3165 |

- **proven**: all 3312 forest-sized primitives come from the object overlay;
  the map's own field models are 56 primitives (two foliage fragments at the
  camera, one of them the lit teal clump in every capture). With the object
  draw skipped (`m16B`, `logs/before-B-objects-skipped-cull.log`) the field
  contribution is identical and constant for 360 frames.
- **proven**: the camera is what the script encodes. gdb on the port
  (`traces/gdb-trace.txt`): opcode `63 30f8 0000 dc05 e0` → at = (-2000, 1500, 0),
  `a3 5efa c6ff 7e06 e0` → eye = (-1442, 1662, -58), both started with
  duration 0 (`ac 00 0080`, `ac 01 0080`). It never moves afterwards.
- **proven**: nothing accumulates. Per-30-frame root dumps
  (`logs/before-C-run-filtered.log`, `[obj-ovly] state`): slot 0 travels
  (-6000,4500,0)→(-4565,3423,0)→(-3026,2269,0)→(-1488,1115,0)→(0,0,0) between
  frames 1 and 121; slot 1 does the same between 121 and 271; slots 2/3 end
  at (-1000,0,±600). The mover is the script's opcode `0x10`
  (`func_80098CAC`, `traces/gdb-trace2.txt`: `10 00 | 0000 | 0000 | 0000 | e0`
  = "move to (0,0,0)", 117 steps). The camera at x=-1442 looks toward -X, so
  each object crosses it and ends behind it — exactly when `gte31` steps up
  by ≈950 (one object's primitives).
- **observed**: presented frames 150–187 are the fade-in from black; after
  it the pre-fix scene is 2–7% non-black with 0.0% of pixels above 64/255
  except the foliage clump (`frames/before-C-frame-000230-near-black.png`,
  `frames/before-A-frame-000240-gear-passing-near-black.png`: a Gear filling
  the screen at colour ≈(0,0,40)).

## 2. Root cause chain (proven unless marked)

1. **`func_801E7D14` (port) was an approximation, not retail.** Retail
   `[801E7D14,801E7FD4)` (704 bytes, SHA-256 `2e84156f…`, `disasm/dis_7d14.txt`,
   extracted from `disc/disc1.bin` sectors 231361..231385 as archive 6B9, payload
   SHA-256 `14395a9f…`) runs six passes; the port's version rebound actor
   positions (`OvlyBindActorPos`, port-only), skipped the linked-object,
   lighting and auxiliary passes, never called `SetColorMatrix`, and replaced
   `801DCEC8` with a `CompMatrix(w2s, node+0x0C)` loop through
   `func_8002C700(..., 0)`.
2. **Lighting.** Retail pass 4 does `SetColorMatrix(D_801E8644)` where
   `D_801E8644 = &D_800B2264 - 0x28 = D_800B223C` (the field light-colour
   table), and `801DCEC8` sets `SetLightMatrix(D_800B221C × root.world × node.world)`
   for every mesh node, then draws with **variant 1** (the NCS/NCCS lit
   walkers). The port drew with variant 0 and whatever colour matrix was
   left over: at draw time the field's own colour matrix `D_80059F84` is all
   zeros on MAP16 (`traces/gdb-trace2.txt`: `fieldLCM 0 0 0 0 0 0 0 0 0`;
   the map's light records are zero) — so only the (30,30,30) back colour
   survived. Retail's tables at that moment: `LLM D_800B221C = 504 -4033 -504 | 0 0 0 | 0 0 14654`,
   `LCM D_800B223C = 2048 0 0 | 2048 0 0 | 0 0 14654`.
3. **Object scale.** `misc2.c FieldUpdateObjectActor` (retail 80076300)
   computes `obj+0x1C = actorScale × table[slot] >> 12` with the table read
   as `g_FieldBss_800B20A8 + 0x164 + slot*4`. The packed block named
   `g_FieldBss_800B20A8` starts at retail `0x800B2078` (`g_FieldEffects`,
   `config/symbol_addrs.field.txt:423`; `g_FieldBss_800B2174 = block + 0xFC`
   in `pc_port/src/data_field.c:1159`), so `+0x164` is `0x800B21DC` — the
   `spriteId<<1` table written by the byte-exact `func_800A1364`
   (`traces/gdb-trace6.txt`: the words `0x00100000 0x00100010 …` are that
   table, watchpoint-attributed) — not `0x800B220C`. Result: `obj+0x1C` =
   28672/28695/30481/30465 (≈7.0×) instead of 468/644 (`traces/gdb-trace4.txt`,
   `gdb-trace9.txt`). The port's old draw ignored the scale entirely, hiding
   this; the faithful hierarchy update applies it.
4. **Scale table never written for slots 1..3.** `src/field/main/main.c`
   declared `extern void* D_801E8670[]` — an 8-byte stride on the host for a
   PSX word table — so `func_80077AB4` saw slots 1..3 as NULL (`main.c:239`
   guard) and skipped `D_800B220C[i] = obj+0x1C`
   (`traces/gdb-trace7.txt` shows the store executing for i=0 only).
5. **Missing walker.** `D_8004FE50[0].proc[1]` (retail `0x8002ED20`, the lit
   small-tri walker) was NULL; the follower models dispatch 33 row-0 groups
   each with variant 1 (`cmd0=00` in the C700 census), which the dispatcher
   would have skipped loudly.

Retail `742C` stores `obj+0x1C = *(u16*)(modelHeader+8)` (`lhu $v0,8($s2); sh $v0,0x1c($s3)`
at `801e792c`) exactly as the port does (320 / 440 here), so the object's
native scale is not in question (**proven**).

## 3. The fix (retail-faithful transcriptions; all build-verified, DCEC8 proven by test)

`pc_port/src/field_object_overlay.c` (SHA-256 `5ce9d73f…`):

| function | retail slice | bytes | SHA-256 | status |
| --- | --- | --- | --- | --- |
| `func_801E7D14` | `[801E7D14,801E7FD4)` | 704 | `2e84156fa452d4cfcb0c90c98f11650394193b0bd2403d10da52c4950140d419` | six passes transcribed from `disasm/dis_7d14.txt`; `OvlyBindActorPos` removed (retail's actor→object sync is `misc2.c FieldUpdateObjectActor`, which already runs earlier in the frame) |
| `func_801DCEC8` | `[801DCEC8,801DDBF8)` | 3376 | `bcbea29d659d3aa05fe447139193d17267308952b37b8151181b105796f173eb` | new; ground quad, per-node light/view, billboard override, variant pass-through, the three attached-effect lists (`disasm/dis_dcec8.txt`) |
| `func_801E0398` | `[801E0398,801E0698)` | 768 | `04c93d0db38309d9c0a17505a536a310614cf23b97bb6cc447385a48a868354f` | new; auxiliary sprite pool draw (`disasm/dis_e0398.txt`) |
| `func_801E36BC` | `[801E36BC,801E37D0)` | 276 | `9277585ee654c29ae0768e3551fcab8ef33c94b168c7eedd832f38cbffc71fe3` | five-argument retail body: 7298 → DC848/DC5C0 by obj+0x37 → per tick DDBF8 ∥ E5D44 → E39F0(status, ticks, arg5); replaces the port-only `OvlyClipTick`+`OvlyRebuildTree` pair (`disasm/dis_36bc.txt`) |
| `func_801E5D44` | `[801E5D44,801E632C)` | 1512 | `760f9f457085e59b63a6ae781297309641b8a4e429cc7ced348c1cda0f6c4c19` | new; gate, list walk, counter advance and rewind transcribed; the nine opcode bodies (owners 801E0844/0A00/165C/34BC missing) stop loudly — **explicit boundary**, unreachable on MAP16 (`obj+0x98 = -1`, `traces/gdb-trace8.txt`) |
| `D_801E8698` | `0x801E8698` | — | — | new global (sway magnitude) |

Also removed from that file: `OvlyRebuildNodeMatrix`, `OvlyRebuildTree`
(flat two-level rebuild that ignored scale and left every `node+0x2C` zero).
`OvlyClipTick` is still used by the clip-bind path and is untouched.

`pc_port/src/model_prim_ed20.c` (new, SHA-256 `de1f7ed9…`): `func_8002ED20`,
retail `[8002ED20,8002EEF8)` (472 bytes, SHA-256 `f0f3d3d3…`,
`disasm/dis_ed20.txt`), the variant-1 lit small-tri walker (packet 0x14, tag
0x04000000, 12-byte colour+normal record per primitive, NCCS), modelled on the
existing `func_8002F2E0`. Registered in `pc_port/build_port.sh` `PORT_SOURCES`
and wired as `D_8004FE50[0x00].proc[1]` in `pc_port/src/game_overrides.c`
(one prototype line and the row-0 initialiser; the temporary per-submitter
cull tag added during diagnosis was reverted).

`src/field/main/misc2.c` — one region only, `FieldUpdateObjectActor`
(port-only, inside `#ifdef XENO_PC_PORT`): the table read is now
`(u32)D_800B220C[slot]` (blob alias at retail `0x800B220C`), with
`extern s32 D_800B220C[];` replacing the unused `extern u8 g_FieldBss_800B20A8[];`.
Nothing near `func_800726E8` was touched; the diagnostic prints added during
this pass were removed again. (**build-verified**; byte-neutral for the
matching build by construction since the function is port-only.)

`src/field/main/main.c` — `D_801E8670` is declared `u32[]` under
`#ifdef XENO_PC_PORT` (the `void*[]` form is kept for the MIPS build), the
port-only NULL guard compares with 0, and the store casts through
`(uintptr_t)` (a no-op on MIPS). (**build-verified**; the matching-build
bytes are unaffected by construction — the shared line's casts do not change
codegen — but `make check` was not rerun in this pass.)

## 4. Regression test (proven)

`bash pc_port/tests/run_object_draw_retail_test.sh`
(`pc_port/tests/object_draw_retail_test.c`), modelled on
`run_object_link_retail_test.sh`: the 3376 retail bytes of `func_801DCEC8`
run on the project's MIPS interpreter against the native body, sharing one
PsyCross GTE, recording spies for `CompMatrix`/`MulMatrix0`/`MulMatrix`/
`RotMatrix`/`SetRotMatrix`/`SetTransMatrix`/`SetLightMatrix`/`ratan2` and
`func_8002C700`, the retail inline OT link, and comparing the whole
object/node/OT fixture, the ordered boundary record, the scratchpad and the
GTE register file.

- **O0 / O2 / UBSan: 1536 cases each, all PASS.** Case space: visibility ×
  ground-quad flag × render context × scale {0,320,644,4096} × object/root
  heights (negative gap, clamp) × extents × OT shift {2,4} × draw variant
  {1,2} × 2..5 nodes with random mesh/visibility/billboard mode.
- **Seven negative controls rejected at runtime, all of which compiled**
  (the runner treats a compile failure as its own error, not a rejection):
  `nolight` (drop `SetLightMatrix`), `localmatrix` (`node+0x0C` instead of
  `node+0x2C`, the old port composition), `novisibility` (ignore `node[7]`),
  `variant` (draw with 0 instead of the caller's variant), `shade`
  (`obj+0x39` divisor), `billboard` (wrong rootView column), `rootlight`
  (`root+0x0C` instead of `root+0x2C`).
- Limits: the attached-effect lists are held at zero (their owners are
  separate units with their own tests); `7D14`, `36BC`, `E0398`, `E5D44`
  and `ED20` are transcribed but not covered by this test.

## 5. Runtime after the fix (observed; final binary `a45f2bf1…`)

Runs `m16D2` (30 s, captures every 15 presented frames) and `m16E2` (8 s,
every frame); no assert/SEGV; no `missing D_8004FE50`, no `E39F0 unsupported`,
no `E5D44` boundary hit; the map still warps to field 15 at the same time
(cull frame ≈389). The cull log no longer sees the objects because the
variant-1 walkers are not instrumented — the OT emit counter is: `otEmit`
457 at cull frame 59 and 261 at 179 (object passes) against 9 for the field
alone (`logs/after-D2-final-binary-cull.log`).

| presented frame | non-black % / bright(>64) % — before (`m16C`) | after (`m16E2`/`m16D2`) |
| --- | --- | --- |
| 195–213 (post fade) | 2.1 / 0.0 | 2.1 / 0.0 (foliage clump only) |
| 225 | 6.7 / 0.0 | 6.7 / 2.3 |
| 238 | 45.1 / 0.0 | 16.2 / 6.5 (leader passing, `frames/after-E2-…000238…png`) |
| 240 | 68.9 / 0.0 | 13.6 / 5.1 |
| 360 | 6.7 / 0.1 | 16.1 / 7.8 (three followers, `frames/after-D2-…000360…png`) |
| 375 | 0.6 / 0.0 | 27.8 / 14.8 (`frames/after-D2-…000375…png`) |

Before, the passing object covered up to 69% of the screen at ≈(0,0,40)
(7.0× scale, unlit); after, the Gears are at their retail scale (0.114× /
0.157× of model units) and show armour detail, blue key light and green/teal
highlights. Between passes the frame is legitimately dark: two foliage
fragments in front of a fixed camera at night, no terrain — **the map's
darkness between passes is authored** (the map archive is 112,640 bytes;
its own geometry is 56 primitives).

Object state after the fix (`traces/gdb-trace9.txt`): `obj+0x1C` = 468 /
644 / 644 / 644 from `D_800B220C` = 320 / 440 / 440 / 440; root local
`(0,0,-468)`; node world matrices populated (e.g. node 7 `W0=(4015,260,771)
Wt=(-671,-2081,2645)`), where before every `node+0x2C` was zero.

Regression checks on the same binary (`m17F`, `m2G`, previous build of the
same sources): map 17 seen/emit series 864→1801 / 27→418 and 96–99.8%
non-black after the fade; map 2 renders 44–89% non-black; no crashes.

## 6. Open items and honest limits

- **Light-table ninth halfword is stack garbage** (proven for the port,
  inferred for retail): `misc3.c:1107/1108` call `func_80077844(dst, 8 values)`
  but the callee fills nine halfwords, so `m[2][2]` of both `D_800B223C`
  (light 2's blue) and `D_800B221C` (light 2's Z direction) is whatever sits
  in the caller's tenth-argument slot — 13823 in one run, 14654 in another
  (`traces/gdb-trace11.txt`). Retail does the same read from its own PSX
  stack (deterministic there, value unknown). This decides how blue the
  Gears are; it is pre-existing, outside this pass's files, and the correct
  resolution is to determine retail's slot contents, not to pick a value.
- **Animation**: the objects are drawn in their bound pose and do not
  animate across the pass (node rotations identical at 7D14 calls 3 and
  120). The retail chain (DDBF8 tracks, E39F0 clip VM) now runs; `DDBF8`
  returned status 0 with `obj+0x3C = 0`. Whether retail animates them here
  is not established; the previous port path did not either.
- `func_801E5D44`'s nine event opcodes and their owners are not ported
  (explicit loud boundary; unreachable on MAP16).
- `src/field/main/misc8.c:1238` indexes `D_801E8670` through a `void*`
  declaration (same 8-byte-stride bug class as `main.c`); not on the MAP16
  path, not changed here (file is in flight for another task).
- `D_8004FE50` rows 0x00/0x03 `proc[3]`, row 0x02 `proc[1..3]` remain NULL as
  before.
- The temporary `[obj-ovly] draw-pkt / C700 / xform` diagnostics that lived
  in the old `func_801E7D14` are gone with it.
- `OPEN_ISSUES.md` item 8 (and its "object loader stub" cause) was not
  edited; it should now read: MAP16 is the Gear-flight cutscene, rendered;
  remaining fidelity items are the two bullets above.

## 7. Harness notes reaffirmed

- `XENO_FIELD_MAP=16` warps to field 15 after ≈6.5 s of field time; split
  every log on `FieldLoad begin field=` (archive `0xd9` = map 16, `0xd7` = 15).
- Captures named `field-frame-*.png` are 640×480 24-bit BMPs.
- Presented frames 150–187 of a MAP16 run are the fade-in; judge frames ≥195.
- The variant-1 walkers (`func_8002F2E0/F4B4/FAE8/ED20`) are not covered by
  `XENO_CULL_CAM_LOG`; use `otEmit` (`D_80059578`) for object activity.
- Scratch for this pass: `/var/tmp/xeno-m16/` (fragments, disassemblies,
  gdb scripts/outputs); run logs and captures under
  `/var/home/blizz/.cache/xeno-run/m16{A,B,C,D,E,D2,E2}`, `m17F`, `m2G`.

## 2026-09-08 correction to the light-table open item

The retail-stack inference in section6 was incorrect. The original caller explicitly supplies the ninth zero in both JAL delay slots; it also supplies0x800 for color element6. The native transcription omitted these values. The retail-byte audit, repair, differential tests and remaining visual limits are recorded in [the lighting correction](../lahan-light-init-20260908/README.md).
