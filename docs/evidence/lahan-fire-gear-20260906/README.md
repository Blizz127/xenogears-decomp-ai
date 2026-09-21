# Lahan fire (field map 2) — "Fei's Gear is not showing" — 2026-09-06

Working tree: branch `experiment/worldmap-open-gates-20260823`, HEAD `3a3e7aac`,
nothing staged, committed or pushed. Container `localhost/xenogears-dev-toolchain:current`
(`--userns=keep-id`, `TMPDIR=/var/tmp`). Builds `LINK OK`, 74 function stubs
throughout. Run directories under `/var/home/blizz/.cache/xeno-run/m2fix/`
(`runA` first DIAG run, `runB` interleaved log, `runC` per-frame captures 676–1000
with the battle OT histogram, `samp-*` packet dumps, `nat` natural New Game route,
`runF` final-binary check).

Evidence classes: **proven** (byte comparison, reproduced hash, packet dump, or a
differential test against retail bytes), **build-verified only**, **observed**
(seen at runtime), **inferred**.

## 0. Summary

The Gear (archive 6B9 object, 47 nodes then 52 nodes) **does render in the
burning-village field scene**, both on the `XENO_FIELD_MAP=2` harness route and on
the natural New Game route (observed, frames in `frames/`; the caller's own
`lahan`/`lahanB` captures at presented frames 420/540/600/660/675 are
byte-identical to mine, **proven** by SHA-256). The two captures that motivated
this task are not field frames at all: the field module tears down (`[FieldMain]
teardown exitCode=0`) and `enter retail battle.bin at 0x80070f40` fires between
presented frames 675 and 690 (**proven**, `logs/harness-map2-diag-interleaved.log`
lines 389–392). Both symptoms are what retail `battle.bin`, executing as retail
instructions on the MIPS bridge, tells the GPU to draw:

1. **"Geometry corruption" (frame 690)** is the retail field→battle
   screen-shatter transition: every battle DrawOTag frame from presented 680 to
   ≈740 contains only `0x24` (POLY_FT3, textured, opaque) packets — 560 of them
   at first, decaying as the shards leave the screen — textured from 15-bit
   direct-colour VRAM pages (`tpage 0x013F/0x013E/0x013B`, x=704..1023, y=256)
   with a per-frame fading modulation (`ededed` → `e7e7e7`) over a white→black
   flash (**proven**, §2).
2. **"Washed-out ghost Gear" (frame 900)** contains **no Gear packets**: the OT
   holds 14–17 packets per frame (two 8-bit horizon strips, three fire quads,
   seven additive smoke triangles at screen centre, tpage packets) plus, for
   presented 892–910 only, one `0x2A` untextured semi-transparent full-screen
   quad drawn additively (`DR_TPAGE abr=1`, colour `c0c0c0`, corners
   (-32,-32)…(320,240)) — the "grey wash" (**proven**, §3). The "ghost" is the
   smoke sprite. The battle's Gear model (≈470 `0x24` + ≈80 `0x2D` packets per
   frame) first appears at presented ≈954 and is lit and textured by 990
   (**observed**, `frames/harness-map2-frame-000990.png`).

The one code defect on this path was diagnostic-only and is fixed: the
`XENO_FIELD_DIAG` CLUT readback in `func_8002DDE4` accepted a 256×4 rectangle
into a 256-halfword stack buffer, so DIAG on map 2 aborted with `*** stack
smashing detected ***` (rc 134) on the Gear texture's 256×4 CLUT. It now uses a
`256*4` buffer; DIAG runs to a clean timeout (rc 124) and a retail-differential
test with the old size as a mutant reproduces the smash (**proven**, §5).

Whether the user's report predates today's `func_801E7D14` transcription
(`../blackmoon-map16-render-20260906/`) is **inferred** only: the pre-fix
natural-route replays that exist (`opening-constructor-virtual-*`, 04:00 UTC)
captured every 60 or 300 frames and hold no frame inside the Gear window except
the scripted flash at 5160, which does show a Gear silhouette
(`frames/prefix-natural-route-ysi05oci-field2-4800-5580.png`). No pre-fix
binary was rebuilt (the fix is uncommitted and must not be reverted).

## 1. Harness timeline (`XENO_FIELD_MAP=2`, presented frames; proven from the interleaved log unless marked)

| presented | event / content | source |
| --- | --- | --- |
| 150 | `FieldLoad begin field=2` (only FieldLoad in the run; no warp) | log line 48 |
| ≈160 | `742C slot=0 model=0x7abd18` — 47 nodes, 24 groups, `[DDE4]` uploads the 160×1 CLUT + seven texture blocks at (576,256) | lines 168–195 |
| ≈160 | `[xeno-port][variant5] … resume F13 retail FIFO/culling gate` tripwire fires (8 lines) — field walkers, not the object | lines 201–208 |
| 210–390 | burning village, villagers, fire | `frames/harness-map2-contact-sheet-210-1095.png` (observed) |
| 420 | Gear legs (47-node object) beside a building | observed |
| ≈440 | slot 0 rebuilt: `742C slot=0 model=0x7acf54` — 52 nodes, 20 groups (208×1 CLUT) | lines 346–371 |
| 450–540 | village; 540 Gear close-up in blue | observed |
| 555 | scripted full-screen flash (100 % bright, pink/white) with the Gear visible; the same flash is red-washed in the pre-fix natural route (5160) | observed |
| 570–675 | Gear standing amid fire; `Fei "Huff…huff…"` at 600 | observed, `frames/harness-map2-frame-000600.png`, `…000660.png`, `…000675.png` |
| 675→690 | `[FieldMain] teardown exitCode=0`, `enter retail battle.bin at 0x80070f40` | lines 389–392 |
| 680–≈740 | battle frames 1–60: shatter (§2) | `logs/harness-map2-battle-ot-histogram-676-1000.log` |
| 741–849 | black (packet count falls to the backdrop set) | runC stats |
| 850–891 | fire line fades in (additive quads/tris), smoke tris at centre | §3 |
| 892–910 | additive flash quad; peak 894 = (193,189,187), then (70,33,124), (70,33,31), (21,17,31)… | §3 |
| ≈954+ | Gear model packets appear (`0x24` 86 → 474 by 990) | histogram |
| 990 | lit purple Gear standing in the arena; 1095 `Fei "Hiyaaaaa!"` battle dialogue | observed |

Battle frame *n* (1-based `bridge_draw_otag` calls) = presented 679 + *n* in this
harness run (battle frame 1 = presented 680, 213 = 892) (**proven**, histogram
lines carry both counters).

## 2. Symptom 1: the shatter is retail's transition (proven)

`logs/battle-ot-samples-shatter-f11-12.log` (battle frames 11–12 = presented
690–691, the reported capture). Every packet in both frames is `0x24`; the first
six of frame 11 decoded:

| words | meaning |
| --- | --- |
| `24ededed 000c015e 00581030 000f014a 013f2030 00040148 78802020` | POLY_FT3, rgb (237,237,237); v0 (350,12) uv (48,16) clut 0x0058; v1 (330,15) uv (48,32) **tpage 0x013F**; v2 (328,4) uv (32,32) |
| `24e7e7e7 fff1010f a0230040 …013e1040…` | frame 12 of the same shard: rgb (231,231,231), tpage 0x013E |
| `24ededed 001cffcf 80003000 001dffe0 013b3010 …` | tpage 0x013B |

`tpage 0x013F`: tp = 2 (15-bit direct colour), abr = 1, ty = 256, tx = 15×64 = 960;
`0x013E` → x 896; `0x013B` → x 704. Shards therefore sample 15-bit pages in the
VRAM band x=704..1023, y=256..511 — a frame-buffer image, not a model texture
(CLUT-indexed model textures upload at (576,256)/(832,…) per the `[DDE4]` lines).
Vertex coordinates include off-screen values (`fff1` = -15, `01000008` = (8,256)),
i.e. shards in flight. Packet count decays 560 → 557 (699) → 431 (709) → 328
(739) as shards leave the visible area (`logs/harness-map2-battle-ot-histogram-676-1000.log`).
Per-frame stats (`runC`): 681 is a full white frame (243,243,243); 682–692 the
mean rises 22 → 143 as the background behind the shards brightens; 693–720 it
falls to 12; 723+ black. No background primitive is in the OT, so the ramp is a
frame-buffer clear/flash issued outside the OT by the same retail code
(**inferred** for the mechanism, **proven** that nothing in the OT draws it).
Every frame of the sequence is coherent motion of the same textured shards
(`frames/harness-map2-shatter-every3-680-716.png`), not per-frame garbage.

Nothing in the port composes these triangles: `bridge_draw_otag`
(`pc_port/src/battle_mips_runtime.c`) walks the guest DMA chain and passes each
packet's guest words to PsyCross byte-for-byte; only the host link tag is
rewritten (**proven** from source).

## 3. Symptom 2: the grey wash and the "ghost" contain no Gear (proven)

`logs/battle-ot-samples-flash-f213-214.log` (battle frames 213–214 = presented
892–893). All 16 packets of frame 213:

| packet | count | decoded |
| --- | --- | --- |
| `2d…` POLY_FT4 raw-textured opaque | 2 | x 68..320 / 0..68, y 135..231, tpage `0x0087`/`0x0088` (8-bit, x 448/512, y 0) — the horizon strips the log uploads as `[horizon] DIAG 2709C a0=448 … w=256 h=96` |
| `27…` POLY_FT3 raw-textured semi-transparent | 7 | x 109..269, y 109..162, tpage `0x002B` (4-bit, **abr 1 additive**, x 704, y 0) — the smoke/flame column at screen centre, i.e. the "ghost" (capture-space x≈220–540, y≈220–325) |
| `e1000020` DR_TPAGE | 1 | sets abr = 1 (additive) for the next primitive |
| `2ac0c0c0 ffe0ffe0 ffe00140 00f0ffe0 00f00140` POLY_F4 semi-transparent | 1 | **full-screen additive quad, rgb (192,192,192)**, corners (-32,-32) (320,-32) (-32,240) (320,240) |
| `e100008f e20f0000`, `e100008f e2000000` DR_TPAGE + DR_TWIN | 2 | texture window for the fire quads |
| `2e04040c …` POLY_FT4 semi-transparent | 3 | rgb (12,4,4), vertices spanning x -398..715 with y 1023 (clipped), tpage `0x004E` (4-bit, abr 2, x 896) — the fire line along the bottom |

The `0x2A` quad is present only for battle frames 213–231 (presented 892–910,
histogram `2a=1`), exactly the frames whose mean jumps to (193,189,187) at 894 and
then flickers (70,33,124) → (70,33,31) → (21,17,31) → (36,32,31) → (21,17,16) →
dark at 912 — a scripted, colour-cycling flash. The frame 900 capture in this
directory (`map2-gear-washed-out.png`) is that quad at rgb ≈ (62,31,31) over the
smoke column. There is **no** `0x24`/`0x2C`-class model geometry in any frame
from 850 to 951: the Gear's packets begin at battle frame 275 (presented 954,
`24=86`) and settle at ≈470 `0x24` + ≈80 `0x2D` by 972 (histogram).

So the Gear is neither washed out nor faint here: it has not been submitted yet.
Whether retail shows the Gear earlier in its intro cannot be decided from the
port alone (no retail pixel reference exists — the retail-emulator lane in
`../opening-battle-encounter-20260905/retail-input-capture-20260906.md` is
UNRESOLVED); but every packet in these frames is emitted by retail code
executing on the bridge, so the ordering is retail's (**inferred** from that
architecture, **proven** for the packet contents).

## 4. Natural New Game route (observed; final tree, this binary)

`bash pc_port/tests/run_title_newgame_smoke.sh` with `XENO_FIELD_DIAG=1`,
captures every 30, 420 s, headless (`logs/natural-route-newgame-diag.log`,
`logs/natural-route-newgame-smoke-summary.txt`): Map 490 → `title confirm
choice=2` → Map 4 → Map 2 (`FieldLoad begin field=2` line 4133; `742C` 47 nodes
line 4252, 52 nodes line 4477 — the same two constructions as the harness) →
`teardown exitCode=0` / `enter retail battle.bin` (4640/4645) → `FieldLoad begin
field=14` (6082, the painting room after the battle returned). The field-2 window
is presented 6810–7320; the Gear is on screen at 7050, 7080 and 7170–7320
(`frames/natural-route-field2-6810-7410.png`,
`frames/natural-route-frame-007290-gear-in-fire.png`,
`frames/natural-route-frame-007230-huff.png` with `Fei "Huff…hu"`), followed by
the same shatter at 7350/7380. Blue-pixel share (B > R+30, B > 40) in the Gear
frames: 15.2 % (7050), 27.7 % (7080), 14.5 % (7170), 13.8 % (7260), 15.9 % (7290).
The smoke's only FAIL gate is `script.reaches.fe57`, a VM-trace log marker
(`op=FE57`) that did not appear with `XENO_FIELD_DIAG=1` set; every chain gate
(fe60 exit, request 2, title loop, New Game, Map 4, Map 2, no fatal markers)
passed.

## 5. The fix: DIAG CLUT readback overflow (proven)

`pc_port/src/game_overrides.c`, `func_8002DDE4` (retail 0x8002DDE4 transcription):
the diagnostic branch reads the just-uploaded CLUT back with `StoreImage(&rd,
clutBuf)` under the guard `rect.w <= 256 && rect.h <= 4`, but declared `u16
clutBuf[256]`. The Gear texture's CLUT block is 256×4 (`[DDE4] blk[0] magic=1101
… 256x4`… in the caller's earlier DIAG run; 160×1 and 208×1 in this run's two
objects, 256×15/256×9/256×2 in the battle's uploads, which the guard skips), so
1024 halfwords went into a 512-byte buffer: `*** stack smashing detected ***`,
rc 134 (caller-reproduced; documented in `../gear-model-list-resume-20260905/`).
Now `u16 clutBuf[256 * 4]`. Effect: `runA`/`runB`/`runF` with `XENO_FIELD_DIAG=1`
on map 2 run to the 45 s / 34 s timeout (rc 124) with the full `[obj-ovly]` and
`[DDE4]` trace (`logs/harness-map2-diag-interleaved.log`).

Regression test `bash pc_port/tests/run_dde4_clut_diag_test.sh`
(`pc_port/tests/dde4_clut_diag_test.c`): the retail bytes `[8002DDE4,8002DFE0)`
(508 bytes from `disc/SLUS_006.64` at file offset 0x1E5E4, SHA-256
`758f48b55e8638ed3f177d4aad90c94501937acd9b6f690ae2e98ca6c45e4d8e`; SLUS SHA-256
`dc0b2dd7…`) run on the MIPS interpreter with `LoadImage` (0x80044894) bridged
to a recording spy, against `func_8002DDE4` extracted verbatim from
`game_overrides.c` by the runner. Each case compares the ordered LoadImage
records (rect, blob offset, payload FNV hash), the return value and the untouched
blob, natively twice: `XENO_FIELD_DIAG=0` and `=1`. The `StoreImage` spy writes
the full `w*h` halfwords; O0/O2 build with `-fstack-protector-all`, the third
build is ASan+UBSan.

- **171 cases × {O0, O2, ASan} PASS**: 11 fixed shapes (the map-2 Gear layout
  160×1 + 64×136 etc. at mode 1/1 with (576,256)/(0,252); 256×4 CLUTs in
  modes 0/1/2 with u16-wrapping clut coordinates 0xFFF0/0x1FFFC; 256×8 (guard
  boundary); the battle's 256×15 + 64×204; a bad-magic block after a good one;
  magic 0) plus 160 seeded random cases (1–6 blocks, modes incl. `0x10001` /
  `0x7FFF0001` / -1 to exercise the s16 casts, negative texX/Y, random u32
  clut coordinates). The diag pass performed 7 readbacks, max 1024 halfwords
  (asserted ≥ 1024 so the overflow path is provably exercised).
- **7 negative controls, each compiled at O2 and ASan, each rejected at runtime
  under both** (a compile failure is reported as a harness error, never as a
  rejection): `oldbuf` (`clutBuf[256]`, the original bug → `*** stack smashing
  detected ***` rc 134 at O2, `AddressSanitizer: stack-buffer-overflow` under
  ASan; `logs/dde4-mutant-oldbuf-*.log`), `guard` (`h <= 8` → same smash on the
  256×8 case), `stride` (`pBlock += 6`), `mode2` (drop `baseX` in tex mode 2),
  `badmagic` (return 0), `clutmode` (`== 3`), `payload` (`* 1` stride).
- Limits: block sizes below 0x8000 and word-aligned payloads (retail reads the
  next header with `lw`; an odd halfword count traps on hardware too); the spies
  model the call sequence and payload bytes, not VRAM.

## 6. Files changed (nothing staged/committed)

| file | change | class |
| --- | --- | --- |
| `pc_port/src/game_overrides.c` | `func_8002DDE4`: `clutBuf[256]` → `clutBuf[256 * 4]` (fix). `PcPort_FieldCaptureOnVsync`: new env knobs `XENO_FIELD_CAPTURE_FROM` / `XENO_FIELD_CAPTURE_TO` (presented-frame window) and exported counter `g_PcPortPresentedFrames` (additive diagnostics). The `D_8004FE50` rows were not touched. | proven / build-verified |
| `pc_port/src/battle_mips_runtime.c` (untracked file owned by the battle lane) | additive, env-gated TEMP-DIAG in `bridge_draw_otag`: `XENO_BATTLE_OT_DIAG=from:to` prints one GP0-command histogram line per battle DrawOTag frame (with the presented-frame counter); `XENO_BATTLE_OT_DIAG_SAMPLES=n` dumps the first *n* packets' guest words. Read-only; the walk and payload are unchanged. Marked `TEMP-DIAG` for the owner to drop. | build-verified |
| `pc_port/tests/dde4_clut_diag_test.c`, `pc_port/tests/run_dde4_clut_diag_test.sh` | new regression test (§5) | proven |
| `pc_port/src/field_object_overlay.c` | **not modified** (SHA-256 `5ce9d73f…`, as left by the map16 pass) | — |

Build note: the first two builds in this pass aborted on
`src/field/main/misc5.c:1252: 'stdout' undeclared` — the other agent's in-flight
file. Rather than touch it, those binaries were built with
`XENO_DIAG_DEFINES="-include /logs/m2fix/stdout_shim.h"` (a forward declaration
of `extern struct _IO_FILE* stdout;`). `misc5.c` has since gained `#include
<stdio.h>` (15:58), and the final binary is a clean `./pc_port/build_port.sh`
without the shim (§7).

## 7. Final binary check (observed)

Clean `./pc_port/build_port.sh` (no shim) → `pc_port/build_native/xeno-port`
SHA-256 `0db43f6292ad82fea5e18acf53c2c9cc4cc25320baf4063aa772ea743a65dcec`,
`LINK OK`, 74 stubs. `runF` (map 2 harness, `XENO_FIELD_DIAG=1`, 34 s): rc 124,
no stack smash, both constructions (47/52 nodes) at the same log lines, teardown
389 / battle entry 392 as in `runB`. Frame hashes differ from `runA` because the
tree moved under the build (`src/field/main/misc5.c` 15:58, `src/field/scripts/virtual_machine.c`
16:07 — both the other agent's files); the content does not: presented 540/600/
660/675/690 differ in 0.03–0.06 % of pixels, none by more than 16 levels, with
identical channel means and non-black shares (`runF/cmp-A-vs-F.png`); frame 900
differs entirely because the additive flash quad is in a different colour phase
(mean 132 vs 70). The Gear frames of §1 therefore hold for the final binary.

## 8. Open items and honest limits

- No retail pixel reference for the battle intro or the shatter exists in the
  project; the retail-emulator capture lane is UNRESOLVED (`../opening-battle-encounter-20260905/`).
  The claims here are about what the port draws and who emits it, not about
  pixel parity with a PS1.
- The `[xeno-port][variant5] … resume F13 retail FIFO/culling gate` tripwire
  fires on map 2 for the field's own depth-cued walkers (8 entries logged
  before the Gear's first frame). The field scene renders plausibly, but that
  deferred gate is a known port boundary and was not investigated here.
- The field-side flash at presented 555 (harness) / 5160 (natural route) is
  100 % bright and tinted differently between runs (pink-white vs red); it is
  scripted by the field VM (observed in pre-fix and post-fix runs alike), not
  examined further.
- The battle's 275-frame (~4.6 s) gap between battle entry and the first Gear
  packets is what the retail code emits through the bridge; whether retail
  hardware would show the Gear earlier (e.g. if model loads are CD-timed) is
  not established.
- `docs/evidence/lahan-fire-gear-20260906/map2-geometry-corruption.png` and
  `map2-gear-washed-out.png` (the two motivating captures, kept unchanged) are
  presented frames 690 and 900 of the caller's `lahan` run and are
  byte-identical to `runA` frame 690 (SHA-256 `25ddabec…`); frame 900 differs
  run-to-run only by the flash colour phase.

## 9. Harness notes

- `XENO_FIELD_CAPTURE_FROM=676 XENO_FIELD_CAPTURE_TO=1000 XENO_FIELD_CAPTURE_EVERY=1`
  gives a per-frame window without filling the disk (325 × 900 KB BMPs here).
- `XENO_BATTLE_OT_DIAG=1:340` (+ `XENO_BATTLE_OT_DIAG_SAMPLES=n`) writes
  `[battle-ot] frame=N presented=P packets=K hist: cmd=count …` to stderr; GP0
  command bytes: `0x24/0x25/0x26/0x27` FT3 (opaque/raw/semi/semi-raw),
  `0x2C..0x2F` FT4, `0x28/0x2A` untextured F4, `0xE1` DR_TPAGE (abr = bits 5–6),
  `0xE2` DR_TWIN.
- The natural New Game route is drivable headless with
  `pc_port/tests/run_title_newgame_smoke.sh` (pad schedule at the BIOS pad
  buffer); with `XENO_FIELD_DIAG=1` the `op=FE57` marker gate fails while the
  chain itself passes.
- Split every log on `FieldLoad begin field=` and on `enter retail battle.bin`
  before attributing a capture; the battle module keeps using the field
  capture hook, so `field-frame-*.png` names continue across the handoff.
