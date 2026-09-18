# Gear model-list and battle bridge continuation, 2026-09-05

This continues the account-limited session `01a0728c-aaea-7000-a871-ee47f1ec37e5`
in `xenogears-decomp-ai`, branch `experiment/worldmap-open-gates-20260823`,
HEAD `3a3e7aac03a2f166fb924945a489e392d706f282`. The user requested continued
Gear investigation with retail accuracy. Existing dirty work, saves and
recordings were preserved. No commit or push was made.

## Recovered stopping point

The previous session's clip-13, sprite-B5 and packed sprite-stack repairs were
already present. Its latest asset trace,
`pc_port/build_native/opening-gear-selection-trace-89gp7lfo/run.log`, records
24 mesh groups but only two constructed nodes. The source and executable were
unchanged since that session. The initial tracked diff, status and affected
source/executable copies are preserved in
`/tmp/xeno-gear-resume-20260905-l12d_42r/`.

The last trace with `XENO_FIELD_DIAG=1` also ended in stack smashing inside
`func_8002DDE4`. Its existing diagnostic CLUT readback accepts a rectangle up
to 256 by 4 pixels but uses a 256-halfword buffer. That diagnostic failure is
separate from the skeleton-list selection. This continuation leaves it
unchanged and uses read-only debugger observations without that diagnostic
environment variable.

## Change and retail authority

The field change is limited to list selection in
`pc_port/src/field_object_overlay.c`:
`func_801E742C` now passes the already captured texture-archive end pointer
(`sliceEnd`) to the node constructor. It previously passed the animation
archive's `+8` pointer table as though it were a skeleton list.

Retail archive 6B9 loads `*(pTexArc + 0xC)` into `s0` at `801E75A8`, retains it
over the mesh-copy and registry calls, and passes it in `a1` at
`801E775C`/`801E77B8` to `801DC2D0`. The fix also avoids rereading an archive
header that a later helper could have changed.

| Retail input | Raw sector / bytes | SHA-256 |
| --- | --- | --- |
| Archive 6B9 including sector padding | 231361 / 51200 | `14395a9f54c124fed016f07906cc882b2a9256d5ade2d10eef99e10fe9366523` |
| Loader instructions `[801E742C,801E77D8)` | within 6B9 | `465d5701dffb89137ea6fcc129a4ad284eab40119fb37c5c4df4402c227510c5` |
| Animation archive | 232272 / 1256 | `9fe50ec9a6b732c50f4ff2e41baa6e76a297fd9eff3d9814c3087e4c11cf7a92` |
| Mesh/texture/skeleton archive | 232273 / 44984 | `f1396a01be36f92a682321372e0f4e558f78e2f30af4811c9a50cef55c6bc5cd` |

The asset's mesh offset is 25084 and list offset is 44764. Retail constructs
46 children and one root, totaling 47 nodes. Payloads stay in local ignored
evidence; no disc bytes are included in this report.

## Verification

`bash pc_port/tests/run_field_gear_model_list_retail_test.sh`:

- Before the fix, O0/O2/UBSan all reproduce the wrong pointer and 2 versus 47
  nodes. Artifacts: `pc_port/build_native/field_gear_model_list_retail_test.CKilyRAm/`.
- After the fix, 48 cases pass at each setting. Artifacts:
  `pc_port/build_native/field_gear_model_list_retail_test.NT4ShHb9/`.
- All seven negative controls fail for the intended selection/topology
  mismatch, including the original archive selection and a late header read.
- Cases include the actual asset, moved list offsets, empty/1/3/7/46-record
  lists, flags 0/0x40, object flag 4, and a helper mutation after the pointer
  is captured. Each node's parent, model index and node index is compared.

The runner extracts the production loader prefix verbatim through its node
call and redirects only that call to a recording wrapper. The real native
node constructor runs. The retail interpreter starts at `801E758C` with
relocated archives, then runs the retail node constructor. Heap, geometry
parsing, texture upload and packet setup are controlled boundaries. This
proves list selection and topology, not complete loader, animation or renderer
parity. Coordinates are compared only when setup consumes them.

Existing regressions pass at O0/O2/UBSan: node construction (5760 cases and
nine mutants), model registry (2053 cases and six mutants), and object cleanup
(39 checks and three mutants). Logs are in the preserved `/tmp` directory.

The canonical native build passes with `compiled=49 skipped=0` and `LINK OK`.
The existing census remains 650 unresolved symbols, with 79 function stubs and
570 data symbols. Build log:
`pc_port/build_native/gear-model-list-port-build-20260905.log`.
Executable SHA-256:
`65ca115af45dedeab51c3e061b9bda83dae66705984d871657642d6c23e644f3`.
Changed production source SHA-256:
`659977a6cd02b55d5d57304b82fea14029bf66badf586d2c0d5662f7cf20dc8e`.

The matching `make check` baseline was run in an isolated copy because its
clean step rewrites generated files. With the container's existing `/.venv`
activated, it fails before linking: the existing `temp1.c` and
`animation_scripts.c` use undeclared `stderr` in matching compilation. No
matching source, symbol, linker or checksum repair is part of this change.
Log: `/tmp/xeno-gear-resume-20260905-l12d_42r/make-check.venv.before.log`.

## Runtime

Visible normal boot/New Game verification completed in
`pc_port/build_native/opening-gear-model-list-snn15vhe/`. Its copied executable,
source pin, input actions, read-only debugger script and captures are retained.
The existing Circle schedule advances field dialogue; no scene/map override
or direct game-state mutation was used.

The first field asset now returns 47 nodes for 24 mesh groups, and the second
returns 52 nodes for 20 mesh groups. Before the fix both returned only two.
`nodes.jsonl` and `nodes-00.bin` / `nodes-01.bin` retain the constructor results.
An independent comparison of the first live node array with the retail asset
passes all 138 child parent/model/index comparisons and three root checks;
see `live-topology-verification.json`.

The natural opening continues through maps 490, 4 and 2 into retail battle
entry `80070F40`. The battle capture `battle-visible-final.png` still shows
Citan, a Fuel HUD and a black background, with no visible Gear. This repair
therefore passes the field skeleton topology gate, while the requested Gear
presentation remains unresolved.

A read-only running-process snapshot (explicitly non-atomic) distinguishes
battle loading from presentation: slot 0 of retail registry `800D3368` holds
a model with 20 groups, 52 nodes, allocated geometry packets and a pose;
slot 31 holds nine groups and ten nodes. `battle-live-observation.json`,
`battle-live-nodes.json` and the accompanying RAM/object/node dumps preserve
that observation. The field-only list repair is not a repair to the battle's
separate retail loader.

The observed process was stopped by its verified PID after preserving the
final capture and state. A second normal-opening observation,
`pc_port/build_native/opening-battle-gear-draw-og9h4wwh/`, records native
model draws and atomic RAM snapshots at the battle GPU submission boundary.
The owned run was stopped after collecting frames 1, 2, 60 and 120.


The second trace narrows the missing presentation to transforms upstream of
culling. Frame 2's GPU DMA chain contains 202 Gear packet tags. By frames 60
and 120, it contains none. All 29 Gear model calls are culled in the later
samples; the nine background calls also emit no geometry. The first Gear
model's view translation changes from `(-993,986,13962)` to
`(-7487,3160,11193)` and then `(7717,3235,-10161)`, with all projected depths
zero in the last sample. Camera eye/target values and matrices are retained
in the RAM dumps. See `draw-chain-analysis.json`,
`draw-transform-analysis.json`, `model-draws.jsonl` and the per-frame DMA
walks. No camera pose was planted and no culling bypass was added.


## Battle trig bridge repair

The dynamic bridge bypassed the source-level trig shim already used by native
game code. It resolved decomp symbol `rcos` at `8003F8B0` to conventional host
cosine, although that retail entry reads the sine halfwords at `800523F0`.
Likewise, retail `rsin` at `8003F8CC` reads cosine at `800523F2`. The bridge also
ran the scalar angle through generic pointer translation, changing values in
KSEG0, KSEG1 and scratchpad-shaped ranges.

`pc_port/src/battle_mips_runtime.c` now resolves those two addresses to the
corresponding conventional host exports. A dedicated scalar branch applies
retail's `angle & 0xFFF` before calling the resolved function. This also avoids
host signed negation of `INT_MIN`. All other calls retain their existing
resolution and argument handling.

The independently written test exercises the actual production initializer,
`dlsym` exports and dispatch. It executes the retail instructions/table as its
oracle. Before the repair, all three build settings report 139190 mismatches
out of 139264; angle zero returns 4096 instead of 0 at `8003F8B0` and 0 instead
of 4096 at `8003F8CC`. Evidence:
`pc_port/build_native/battle_trig_bridge_retail_test.nVju7OZd/`.

The final source passes 139264 bridge comparisons and a separate 8192-value
host-table control at each of O0, O2 and UBSan. Cases cover 16 complete angle
cycles, signed and pointer-shaped ranges, and 4096 deterministic 32-bit input
samples. Four negative controls reject original dispatch, wrong export
binding, translating the angle before masking, and omitting the mask. The
last triggers UBSan on host `INT_MIN` negation. Evidence:
`pc_port/build_native/battle_trig_bridge_retail_test.dZoldDBJ/`.
The fixture compares the defined scalar result, not all caller-saved registers
or rendering output.

| Retail authority | SHA-256 |
| --- | --- |
| SLUS_006.64 | `dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119` |
| Trig instructions `[8003F8B0,8003F8E8)` | `9687ca637b21311f891dd33fa756f8f1c31ff274ccf92923c699b087b44e4c92` |
| Trig table `[800523F0,800563F0)` | `b40c47b014ca8650c539760fd4c519cabce6f091628566ca0dd22318bb2c5b7a` |

Existing battle MIPS adapter, graphics ABI, nested callback stack and GTE
regressions pass. Their logs are `battle-adapter.after.log`,
`battle-graphics.final.log` and `battle-gte.final.log` in the preserved `/tmp`
directory. An intermediate direct-call version exposed unresolved trig symbols
in the isolated graphics fixture. The final implementation retains dynamic
binding; no graphics fixture was weakened or supplemented with no-op stubs.

The final canonical native build passes, with the same 650-symbol/79-function-
stub census. Log: `pc_port/build_native/gear-trig-bridge-port-build-20260905.log`.
Final runtime source SHA-256:
`bcd5067178389d5021fac2b485f22a00e5f0ca01e89e432caf84a2502c2c4125`.
Final executable SHA-256:
`c445dc772b4a70eb84aa7c1740bb65dd5a1ec06a38f61154537a5e28addd300f`.
The earlier runtime source is preserved at
`/tmp/xeno-gear-resume-20260905-l12d_42r/battle_mips_runtime.before.c`.

Final normal-opening verification uses
`pc_port/build_native/opening-battle-gear-trig-qljti8io/`, with both production
source pins and the copied final executable recorded in `run.json`.
