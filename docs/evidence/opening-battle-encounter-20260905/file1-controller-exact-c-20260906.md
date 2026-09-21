# File-1 controller: exact C result frozen

**PASS: all 1260 retail bytes match.** Candidate `candidate.c` SHA256 is `a1dfab198ebc494cb3db6f41da124d55c442ae3c49bee543ea2ebbbbf89c2f45`. Standalone function symbol is **801E6CE8**, size **1260 (0x4EC)**, exclusive end **801E71D4**. `final/candidate.bin` and `retail.bin` both hash to `1f11c9ac7117c0d702c6829cb3fb57806d73c6413eaec5f9fbf1ecca2b427a2e`. There are no remaining differing bytes or instruction words in this function.

All writes stayed inside this scratch directory. Repository/source/config/runtime/UI and previous frozen scratch evidence were not changed. Candidate and final matching artifacts are frozen.

## Authority and pipeline

This is archive `(0x20,0)` file 1, payload `retail-module.bin`, 19516 bytes, SHA256 `64668d85bf48dea46cf6d5f04b38e38ca887877cfef53961f0c26cd2d363b670`. Payload bytes `[0x1CE8,0x21D4)` are the controller; dynamic payload identity must accompany its 801E address. Disc SHA256 is `39c547a9afc6da15d847ef81a2c6cea1a6516bdfa562cf13b0999b04e8598bda`.

The unchanged tool pipeline is repository PSYQ GCC2.6.0 `-O2 -G0 -S`; repository MASPSX default version with `--use-comm-section --run-assembler -EL -O2 -G0 -march=r3000 -mtune=r3000 -no-pad-sections`; GNU MIPS linker with `slice.ld` and `.text 0x801e6ce8 : SUBALIGN(4)`; objcopy of `.text`. Exact commands are in `final/comparison.json`; `build.py` reproduces them. `final/readelf-symbols.txt` confirms actual symbol entry and size rather than assuming the linker section start. No handwritten assembly, inline assembly, register-assembly bindings, volatile access, new optimization switch, linker-byte trick, or binary patch produced this result.

The starting installed source was verified as `c84a38eba6ec039994841cb2368e78d15bb9a54828001e07bb29ec9b12506951`, 302/315 matching words. The preceding task's corrected alignment baseline and earlier results remain unchanged in their prior scratch directory.

## What resolved the 13 differences

The raw halfword pointer macros lost the compiler's aggregate-member memory classification. Named structure-member accesses preserve it. The new typed partial record view plus placing the sentinel-Y source read after `cycle = 4` produces the exact retail scheduling, without changing flags or adding barriers.

Two controlled changes isolate the result:

| Experiment | Exact words | Interpretation |
| --- | ---: | --- |
| Starting c84a raw macros | 302/315 | Three Y-region and ten constructor-region differences |
| Named control fields, original sentinel-before-cycle source order (`struct-all-before`) | 312/315 | All ten constructor scheduling differences disappear |
| Typed sentinel after cycle only (`struct-sentinel-after`) | 305/315 | All three Y-region differences disappear |
| Named fields and sentinel after cycle (`struct-all-after`, final) | 315/315 | Exact |

Compiler diagnostic dumps support this explanation: `rtl-raw/baseline.c.rtl` shows `(mem:HI ...)` for the control fields; `rtl-typed/candidate.c.rtl` shows `(mem/s:HI ...)` at identical member offsets. For example units loads are at lines63-64 in both files, and constructor units/rows loads at lines971-973/1029-1031. The `/s` aggregate-memory classification is the observable RTL distinction; attribution of the scheduling freedom to aggregate alias/dependency treatment follows from these controlled experiments. The dumps through cse, sched, sched2, and dbr are preserved. They were generated separately with `-da` in owned scratch directories, not used to patch the final build.

The final source retains the already-tested constructor comma expressions and the **retail seven-argument constructor ABI**. GetStringEntry remains a separate full expression before the queue call reloads the global window pointer. No arithmetic, branch, helper ordering, wait, teardown or return behavior was intentionally changed.

Exploration of different scalar signedness, register declarations and constructor prototypes did not improve the result. Full-expression argument/local experiments also failed to improve it. Some deliberately exploratory `loads-*-args` shapes read locals in argument expressions that were assigned by other arguments; these unsequenced forms are **rejected**, were never selected, and have no validation claim. The final candidate has none of those forms. All build comparisons, including rejected experiments, are retained in `experiment-summary.json`.

## Typed layout authority

`struct Xbcf1ControlFields` is a **partial view**, not a claim that the entire battle control allocation is 0x804 bytes. Every named field corresponds directly to retail memory operands:

| Member | Offset | Width | Retail instruction example |
| --- | ---: | ---: | --- |
| x | 0x7F6 | u16 | 801E6D2C LHU s5,0x7F6(a2) |
| y | 0x7F8 | u16 | 801E6D34 LHU s7,0x7F8(a2); reload at801E6D64 |
| units | 0x7FA | u16 | 801E6D30 LHU v0,0x7FA(a2); constructor reload801E6F94 |
| rows | 0x7FC | u16 | 801E6D94 LHU v0,0x7FC(v1); constructor reload801E6FB0 |
| flags | 0x7FE | u16 | 801E6D50 LHU s6,0x7FE(a2) |
| phase | 0x802 | u8 | 801E6D54 LBU v0,0x802(a2); stores801E7038/801E7174 |

The byte arrays `before_x[0x7F6]` and `before_phase[2]` represent uninterpreted existing storage at offsets before the observed fields and 0x800..0x801. They are not invented zero-fill operations or claims about the meaning of those bytes. No field accesses those arrays. The last observed member ends at0x803; ordinary halfword alignment makes the C partial view size0x804, including one trailing alignment byte. It does not infer a retail allocation size.

`layout.c` includes the **actual candidate**, asserts each offset, 2-byte halfwords and size0x804, and reports PASS in `layout.log`. The standalone PSYQ binary's exact load/store operands independently pin the target-compiler layout. No packed attribute is needed. The persistent D_800D3278 pointer's declaration remains unchanged; the field macro casts each access and does not cache across helpers. This retains existing reload behavior. Existing heap-record, alignment and disjoint-global assumptions are the same assumptions required by the retail record accesses; no source claim is made for deliberately overlapping the record with standalone cycle/X/Y globals.

## Differential and semantic controls

`validation/manifest.json` is **PASS**. The current durable fixture executes the pinned raw retail controller in the MIPS interpreter and compares the actual native candidate under explicit external-helper boundary stubs. At O0, O2 and Clang UBSan(O1), each run passes **463 cases**, **AUDIT_YIELDS=3**, all **315 retail instruction slots**, and both outcomes of all **21 conditional branches**. Logs report a maximum10 helper events. All **11 negative controls** compile and exit1 with the required explicit `DIFF FAIL` marker.

Coverage includes opening/controller creation paths, centered and high/wrapped layouts, fallback flags, phase reuse, waiting, completion, teardown, defaults restore, poisoned saved coordinates and GetStringEntry's window-pointer rebinding. The fixture compares normalized helper pointer/scalar arguments and ordering, full tracked control/UI/window/alternate-window buffers, relevant global states and return values. Helper bodies are stubbed boundaries; fixture success is not proof of their implementation or renderer behavior.

Only **one new mutation anchor** is needed relative to the now-installed c84a runner: `wrong_constructor_rows` must match `XBCF1_CONTROL(D_800D3278)->rows)));` and replace `rows` with `units`. Exact strings are in `mutation-anchor-updates.json`. `validate.py` imports the repository runner and changes that anchor in memory only; it neither edits the runner nor weakens assertions. Runner, fixture, adapter, source and wrapper hashes are pinned in `validation-inputs.json` and the manifest. The prior task's four anchor changes are already present in the runner used here and were not repeated or reverted.

## Reproduction and gates

From this scratch directory, `python3 build.py candidate.c NEW_EMPTY_BUILD_NAME` rebuilds the C function with the recorded pipeline. `validate.py` records the differential invocation; its output directory is already occupied intentionally, so reproduce using a fresh output path rather than overwriting frozen evidence. `layout.c` was built with Clang C17, `-O2 -ffunction-sections -fdata-sections -Wl,--gc-sections`; unrelated controller code is discarded for this layout-only executable.

- Controller C byte exactness: **PASS**,1260/1260 bytes.
- Structural native semantics under the pinned helper-boundary differential: **PASS**.
- Full-module build with this C candidate: **NOT_RUN by this lane; root owns independent integration verification**.
- Native adoption, raw-global pointer adaptation and live opening: **NOT_RUN by this lane**.

`final-pins.json` freezes the handoff artifacts. The final candidate needs no further matching work within this controller range.
