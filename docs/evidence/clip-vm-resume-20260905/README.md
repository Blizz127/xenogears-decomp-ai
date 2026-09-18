# Clip VM and battle-entry resume, 2026-09-05

This resumes the existing native-port work in `xenogears-decomp-ai` on
`experiment/worldmap-open-gates-20260823`, HEAD
`3a3e7aac03a2f166fb924945a489e392d706f282`. It repairs retail clip state,
implements clip opcode `13`, restores its initialization mode, and connects
sprite opcode `B5` to the existing scale helper. It also repairs packed sprite
stack accesses exposed by the next visible battle run. This is bounded progress;
complete decompilation, Gear rendering, and battle gameplay are not certified.

## Preserved starting state

The checkout already had 62 modified tracked files and 439 untracked status
entries. The starting status, tracked diff, and overwritten clip sources/tests
were saved under `/tmp/xeno-resume-20260905-fv4kg5fk`. Existing work was retained.
No commit, push, reset, or licensed-data addition was performed. Generated
executables, extracted test data, captures, and logs remain in ignored evidence
directories. An ignore entry was added only for the new baseline capture under
`scratchpad/opening-resume-baseline-gfr8e58g/` in `.git/info/exclude`.

## Production changes

- `pc_port/src/field_clip_control.h` retains the entry object separately from
  the active object. In retail these are stack `+E0` and register `s4`.
- `pc_port/src/field_object_overlay.c` initializes that entry identity and
  stores the final script pointer through the active object, as retail
  `801E5984` does. It owns the zero-initialized `D_801E85CC` and its retail
  setter `801E7378`, which stores only input bit 0.
- `src/field/main/misc4.c` restores the existing retail calls that enable that
  mode before initial animation setup and clear it afterwards.
- `pc_port/src/field_clip_data.c` repairs `1D` working operand/limit state;
  resolves `1F` relative to the entry object and retains the returned mask;
  fixes `25` resolver/read/store ordering and the attachment destination
  (`target+5C`, not a dereference through the target's first word); and retains
  the mask for `26`. Native `1F` registry indices outside 0–9 abort explicitly.
- Clip `13` reads control/blend words before lookup, deliberately ignores the
  lookup flag, reads the root and mode after lookup, and calls the existing
  direct-pose or track-blend helper. It preserves retail argument widths,
  callback order, limit `-1`, and the final root-sentinel call.
- `src/slus_006.64/system/animation_scripts.c` implements sprite `B5`: read a
  signed operand byte in Q8, test sprite flags `+3C & 3`, and call the existing
  `SpriteSetScale`. That helper writes the sprite scale and three transform
  scales and sets dirty bit 28 when the transform pointer is nonnull. Its
  packed accesses now preserve overlapping halfword/word stores under O2;
  typed accesses had allowed stale flag bits to survive an overlapping scale
  write.
- The same translation unit now uses retail index `+8C` and stack bytes `+8E`
  in all six push/pop helpers, retaining signed indices, byte wrapping, and
  index reloads after aliased writes. `func_8001FBA4` resolves stack arguments
  and the packed pointer at `+88` without the host-width struct layout.
  `AnimScriptStackPopU16` returns a sign-extended 32-bit value so the native
  battle bridge receives the complete retail `V0` value.

Opcode `25`'s GTE branch still aborts when selected for a nonnull actor. Its
common prefix and direct-orientation branch are covered; the GTE transform is
not approximated. Other unsupported handlers retain their explicit failures.

## Retail authority

The clip fixtures read disc-1 raw sectors 231361–231385, archive entry `6B9`,
overlay base `801DC000`. They execute the retail instructions with the existing
MIPS interpreter and compare the native results. No guessed expected pose or
retail payload is checked into this report.

| Byte range or object | SHA-256 |
| --- | --- |
| Complete clip overlay | `14395a9f54c124fed016f07906cc882b2a9256d5ade2d10eef99e10fe9366523` |
| Clip VM `801E39F0..801E59D4` | `e6e99115c22e250c6fb388ac09e484537fea5412a2b5f7852639df03354900c5` |
| Clip dispatch table | `af846cb60b1b8b9f6959042945ebabc40c84f0c189789b34948e3a28ee409e2c` |
| Clip `13`, `801E3FD4..801E4068` | `36059f865486809e08c946d3bdaf4b4a5cf45bf469f9c2bac3a370097fbcc290` |
| Clip exit, `801E5974..801E59A0` | `2e3d2766000059495df5c62d94c134baa9d649619ee6074cda73cde67fa45d4b` |
| Mode setter, `801E7378..801E738C` | `f23ba5e9842ef3fc55b69891235391be5c40c4ba98b7453588d0f63bd3576666` |
| Sprite dispatch table, `800183D8..800185A4` | `ee73be97683cbf2affa5db0d128e5083ac4b273da3bc5c2f0bfc12bfb75ee0f8` |
| Sprite dispatcher, `8001FBE4..80021AD8` | `7431f354f172d1b450e6c231528af93546595e8aa6d750d10ca489d75e88442c` |
| Sprite `B5`, `80021318..80021340` | `99da33f91badd4b1f9b7af0266b3d94849c16872c0bf589b80caeab77197c1c8` |
| Scale helper, `80022000..80022038` | `daa73a955905fe34d31204c01207f55d4da7dadd4419a5437ba90afcd23d70db` |
| Six sprite stack helpers, `80021C20..80021D3C` | `35be16ffc842a31b0180ee9f44c4c26c70c561d8a083cbca3b5f72baa3374625` |
| Sprite argument lookup, `8001FBA4..8001FBE4` | `0dd76ad93a5ccc0795578b35f4ac3d70c48f39765823740874f7cc66c55c04c1` |

Ranges are start-inclusive and end-exclusive. Sprite bytes come from
`disc/SLUS_006.64`, file offset = virtual address minus `8000F800`.
`disc/field.bin` has base `8006FAF0`. Its call to `801E7378` at `80078EA8`
passes 1; the 28 bytes at `80078E98` hash to
`8b5b84c8acc18aa5cbe1882dcd4286c2408bd6aa5878ab7c7506c121cba0265d`.
Its call at `80079234` passes 0; the 28 bytes at `80079224` hash to
`eafeb46ab7541d2b8abcc34a450d0a2db4bb59fa734bdc82a23199fe9de90a06`.
The overlay's initial four bytes at `801E85CC` are zero, and its only store
instruction targeting that word is the setter.

## Reproduction and checks

The restored track fixture first reproduced four instruction failures in
`clip-track-opcodes-red.log`; the former fixture had collapsed to a trivial
handled-return check. `clip-vm-owner-red.log` reproduces the wrong exit store.
`clip-mode-owner-red.log` records the missing setter/data owner.
`clip-blend-opcode-red.log` records missing `13` handling and callbacks.
`sprite-b5-current-red.log` executes a freshly compiled pre-repair dispatcher
and aborts on `B5`. The subsequent `sprite-b5-alias-red.log` reproduces the
O2 scale-helper alias failure: with transform = sprite `+34`, byte 0, mode 1,
the native low flags byte remained 1 while retail stored 0. The failing
source/object/executable/log are frozen in
`sprite_dispatch_data_retail_test/b5_alias/red/`. These paths are under
`pc_port/build_native/`.

`sprite-stack-red.log` independently reproduces eight failures before the
stack-layout repair: all six stack operations and both argument-pointer modes.
It compares actual retail execution, native returns, and full fixture memory.
The pre-repair source is retained in
`sprite_stack_retail_test/before-fix-animation_scripts.c`, SHA-256
`432758e36e685dad93404e38e1b01f47465bb8d2232bf1938523eb59098d906a`.

All counts below are **per configuration**, with O0, O2, and UBSan passing.
Mutation controls must fail with the fixture's mismatch diagnostic, rather
than merely fail compilation. Counts exclude skipped dependency instructions.

| Runner under `pc_port/tests/` | Checked cases / boundary | Rejected mutations | Log under `pc_port/build_native/` |
| --- | --- | --- | --- |
| `run_clip_vm_owner_test.sh` | 65,536 mode values, entry identity and retail exit store; controlled prelude/handlers | 3 | `clip-vm-owner-final.log` |
| `run_clip_track_opcode_retail_test.sh` | 14,656 full state/memory/callback cases for `1D/1F/23/25-direct/26/27` | 23 | `clip-track-opcodes-final.log` |
| `run_clip_blend_opcode_retail_test.sh` | 264,065 `13` state/memory/global/callback cases | 23 | `clip-blend-opcode-final.log` |
| `run_clip_data_retail_test.sh` | 19,568,680 verified cases, 30 direct handlers | 14 | `clip-data-after-blend.log` |
| `run_clip_control_retail_test.sh` | 786,432 control/state cases | 9 | `clip-control-after-resume.log` |
| `run_clip_pose_opcode_retail_test.sh` | `10/11/18` call-chain checks | — | `clip-pose-after-resume.log` |
| `run_clip_release_opcode_retail_test.sh` | `08` pool-release/root-sentinel call-chain checks | — | `clip-release-after-resume.log` |
| `run_pose_track_bind_retail_test.sh` | 31,104 cases; real pool claim/release, complete memory and return | 7 | `pose-track-bind-after-clip13.log` |
| `run_pose_apply_retail_test.sh` | 36,864 cases; real helper, no recording call boundary | 8 | `pose-apply-after-clip13.log` |
| `run_clip_bind_retail_test.sh` | 16 resets + 16,384 binds; recording VM boundary | 6 | `clip-bind-resume-final.log` |
| `run_cross_clip_bind_retail_test.sh` | 136,112 selector/binder cases; recording VM boundary | 6 | `cross-clip-bind-resume-final.log` |
| `run_sprite_dispatch_data_retail_test.sh` | 7,340,032 cases across 13 handlers, plus the focused B5 census below | 13 | `sprite-data-b5-packed-final.log` |
| `run_sprite_dispatch_data_retail_test.sh --b5` | 208,896 focused `B5` cases, including real scale helper and selected overlaps | 4 (included above) | `sprite-b5-alias-final.log` |
| `run_sprite_dispatch_noop_retail_test.sh` | 1,024 classifications + 144 native/retail no-op executions | 4 | `sprite-noop-after-b5.log` |
| `run_sprite_dispatch_byte_data_retail_test.sh` | 512 `A2/BF` cases | 2 | `sprite-byte-after-b5.log` |
| `run_sprite_stack_retail_test.sh` | 936,719 complete-memory/return cases for six stack helpers and argument lookup | 20 | `sprite-stack-final.log` |

The track and blend fixtures record dependency calls and memory at call time;
they do not claim proof of those dependency bodies. Track coverage includes
null/shared/self slots, successive `1F`, signed values, masks, overlapping
operands, resolver-mutated script data, and store ordering. The separate
native registry guard tests require SIGABRT and the exact diagnostic for slots
10 and 255, with no retail parity claim for those out-of-registry cases.

The blend fixture covers all control/blend halfwords, instruction high byte,
ignored lookup flags, helper mutations, null pose/pool values, call ordering,
and full memory/global state. The real pose-apply and track-bind helpers also
pass their own unchanged production-body suites listed above.

The two binder fixtures needed a test-only link seam after the real VM owner
was introduced: a weak, noinline declaration in the separate overlay test
unit permits the fixture's strong recording VM to win. Production source is
compiled unchanged. An O2 relocation check confirms the binder still calls
the replaceable symbol. The former missing-VM gate was replaced by a real
owner unsupported-dispatch check with controlled rejecting handlers, requiring
SIGABRT and exact object/IP/opcode/limit/ticks/mode diagnostics.

The sprite suite executes the real scale helper on both sides and compares the
whole sprite/transform/auxiliary fixture. Its focused census adds 206,848 cases
to the original 2,048: 200,704 independent byte/mode cases and 6,144 physically
coupled flag-byte cases. It covers five layouts, eleven operand-byte locations,
all 256 operand bytes, four modes, two initial flag patterns, and two raw-opcode
prefixes. An operand at the low flag byte necessarily constrains the mode;
impossible combinations are excluded explicitly. Selected aligned overlaps
place the transform at sprite `+24`, `+34`, or `+38`, alongside separate and
null layouts. This is not an arbitrary-pointer or register-state proof.

The four mutations break the `B5` mode mask, scale multiplier, dirty bit, and
dirty-store order. The order mutant is caught by the new overlapping-field
fixture. The remaining-dispatch failure check now uses genuinely unsupported
`CF`; its previous `8C` choice already had an implementation.

The stack fixture covers 784,902 byte-census cases (784,896 independent and
six physically index-coupled), 131,328 argument cases, 20,480 mixed-sequence
steps, and nine representatives. All 256 index bytes are covered; each payload
byte varies independently with fixed companions. This is not the Cartesian
24-bit value space. Selected overlaps verify reload and store order. The
separate `sprite-stack-abi-red.log` catches a native `0000C285` return where
retail returns `FFFFC285`, motivating the 32-bit signed return declaration.

## Native build and visible execution

Canonical build command:

```sh
podman run --rm --userns=keep-id --security-opt label=disable \
  -v /var/home/blizz/Projects/xenogears-decomp-ai:/var/home/blizz/Projects/xenogears-decomp-ai \
  -w /var/home/blizz/Projects/xenogears-decomp-ai \
  localhost/xenogears-dev-toolchain:current bash pc_port/build_port.sh
```

Toolchain image ID:
`94d6a0111bd27464273aa464784d43b08e7d3750ce371ef83bea6db9585e9f29`.
`clip-vm-sprite-stack-port-build.log` ends with verified port-owned addresses and
`LINK OK`. The stub generator still reports 79 function stubs and 570 data
symbols. Successful linking is not full-port or gameplay proof.

Each run uses a preserved executable and metadata. Runtime is visible X11 SDL,
normal title-menu New Game, then scheduled Circle presses using
`n5_opening_harness/schedule.py --pulses 2000 --on 2 --off 12`. No field-map or
scene override is used. Captures are requested every 60 frames. Despite their
`.png` suffix, raw capture files contain BMP data; inspected copies are PNG
conversions without pixel changes.

| Run | Executable SHA-256 | Observation |
| --- | --- | --- |
| `scratchpad/opening-resume-baseline-gfr8e58g/` | `faed9401252635552b454a9aa9d47f1bca2c854c7811150b048228b02fc6c142` | Normal title → New Game → map 4 narration → map 2; abort at clip `13`, IP `007A2156`, object `007A2ED4`. |
| `pc_port/build_native/opening-clip-vm-resume-kqcdic4a/` | `f5206ef528fad572a7632f10d2818ecb47c9e657967c5edeaf5a99142be2a38f` | Passes the earlier clip stop; dialogue 52 then retail battle entry `80070F40`; abort at sprite `B5`, sprite `0072BA58`, operands `00727645`. |
| `pc_port/build_native/opening-clip-b5-resume-prz1dyt0/` | `19241e01a51b93679c4dd5d97dc4fa3261d3967d95e3362860625b29435286fc` | Normal title → map 4 → map 2 → retail battle entry. Passes `B5`; later aborts in callback `800C11CC`, `read8` at zero, PC `800C120C`. Last inspected battle frame is black. |
| `pc_port/build_native/opening-b5-packed-trace-t8g3vwzz/` | `1a6d89bf51f15d57558efcc65ba82ea2ccfbc879b065606427cde9ebe3e8675f` | Normal opening with the packed scale fix and debugger observations. Reproduces the null script and captures the actual return instruction and sprite memory. |
| `pc_port/build_native/opening-sprite-stack-resume-gbfdmqlv/` | `c916d59b73ec1fd9e4f8d3cae30d3fd8073091773b83f52aaaf05fe8bd43e1eb` | Passes the null-script failure. Inspected `last-view.png` shows a Citan sprite on black, a Fuel display, and battle commands. The main battle stops after 12,050,538 instructions at generated stub `800263E4`. Gear models are not observed. |

Each directory contains `run.json`, `run.log`, the copied executable, and
captures. The final run's metadata pins all five touched production sources
in addition to HEAD. No runtime success is inferred from a test count or an
executable hash. Human acceptance and verified Gear/battle presentation remain
pending.

The intermediate runtime boundary was the battle interpreter reading the instruction
pointer from sprite `+64` at `800C1204`, then dereferencing it at `800C120C`.
This identifies the failed consumer, not the cause of the null script.
`func_800BA8F4` also reports its pre-existing unported no-op during this run;
its effect is not certified. A further visible debugger run under
`pc_port/build_native/opening-null-script-trace-58t75vg5/` uses the same
executable and read-only breakpoints to trace the script pointer's provenance.

The packed-scale run then captured the failure before the interpreter returned:
sprite `007410D8`, previous opcode `85`, script header `0072BDDA`. The bridge
trace first records `AnimScriptStackPushU24(sprite, 0072C285)` and later
`AnimScriptStackPopU24(sprite)`. The latter returns zero; retail `800C1D00`
combines that return with the old instruction pointer's high byte, yielding
zero and causing the next fetch to fault. The compiled native pop reads index
`+C8` and bytes `+CA`, while retail reads index `+8C` and bytes `+8E`.
`SpriteData` contains host-width pointers, so direct C member access is wrong
for the packed sprite used here. This is the observed stack-layout defect;
no null guard or fabricated return address was added.

`opening-b5-packed-trace-t8g3vwzz/null-script-provenance.txt` retains selected
trace lines and native disassembly. `sprite-at-fault.bin` is 256 bytes, SHA-256
`6edf40a442522019eabb8448071e10c7143a1ab85d8984e8f02d32d5b3085ab0`;
the animation-header snapshot is 512 bytes, SHA-256
`5681a1a6a0cbe9e289876b00100d0dc4f11768570fab47864a09e110068eac94`.
Both remain in ignored runtime evidence.
