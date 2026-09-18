Integration note: root installed this nonmatching candidate and updated the
durable runner's four mutation anchors. The runner now requires three yields
and asserts all315 slots/all21 branches both ways in every positive mode.
Root final output: /tmp/xeno-file1-controller-retail-h5bfxx02. Root full-module
comparison: /tmp/xeno-file1-build-check-xceyjpdv (assembly exact, C13 words
different). The report below preserves the original lane findings.

# File-1 controller C matching: frozen bounded result

The improved C candidate is **functionally validated under the differential fixture, but NONMATCHING**. No production/config/UI/runtime files were edited. Ownership was limited to this scratch directory. Candidate SHA256: `c84a38eba6ec039994841cb2368e78d15bb9a54828001e07bb29ec9b12506951`.

## Authority and exact-match gate

Retail archive `(0x20,0)` file 1 is the 19,516-byte payload SHA256 `64668d85bf48dea46cf6d5f04b38e38ca887877cfef53961f0c26cd2d363b670`; controller bytes `[0x1CE8,0x21D4)` loaded at `[0x801E6CE8,0x801E71D4)` have SHA256 `1f11c9ac7117c0d702c6829cb3fb57806d73c6413eaec5f9fbf1ecca2b427a2e`. These are copied in `retail-module.bin` and `retail.bin`. Disc and interpreter pins are in `validation/manifest.json`.

`final/candidate.elf` symbol entry is **801E6CE8**, symbol size and `.text` size are **1260 bytes**. Candidate binary SHA256 is `bd7e99b1b04069c02429dee84e6d8b01f762061f484146b8c6b61b7b9d27b49b`. Result: **302/315 words identical at the same offset; 1209/1260 bytes identical; exact match FAIL**. The 308 aligned matching-word metric in `comparison.json` uses Python SequenceMatcher and is supplementary, not a byte-exact gate or optimal LCS claim. Every JAL and conditional branch instruction is at its retail offset and identical; remaining mismatches are listed below.

## Corrected initial baseline

The original root standalone linker placed the function at **801E6CF0**, with eight leading alignment bytes in a 1256-byte `.text`. Its offset-zero mismatch was alignment, not evidence of an ADDIU/SUBU compiler difference. `baseline-original/` reproduces root binary SHA `80f4dabf582b5f6c30a8921ea38c1cdf0c009ca9320b08ff1c7c8228d16bafaf`.

Using `.text 0x801e6ce8 : SUBALIGN(4)` gives the original a308 source a correct **1248-byte** symbol span at **801E6CE8**, 38 same-offset words and 264 SequenceMatcher aligned words (`baseline-corrected/`). Root accepted this baseline correction. Historical 1256-byte comparisons should not be used as the function-span baseline.

## Pipeline and source changes

`build.py` records exact commands in each comparison JSON: repository PSYQ GCC 2.6.0, `-O2 -G0 -S`; repository MASPSX default version, `--use-comm-section --run-assembler -EL -O2 -G0 -march=r3000 -mtune=r3000 -no-pad-sections`; GNU MIPS linker using `slice.ld` with SUBALIGN(4); objcopy `.text`. MASPSX reads the compiler-produced assembly from stdin. Local static PSYQ binaries reproduced the container baseline. No handwritten assembly replacement or byte patch was used. Probes of compiler 2.7.2 variants and scheduling switches did not improve the selected result; final flags remain the original 2.6 settings.

`candidate-source.diff` shows every change from frozen a308. The selected C narrows layout locals to retail halfword/byte values, removes unnecessary persistent pointer locals, retains a single restore-loop control pointer, expresses the row clamp with the retail default-four branch shape, and places completion after optional cleanup. The constructor keeps the **retail seven-argument ABI**. Its final rows argument uses comma expressions to save Y then X before the call, preserving the scalar rows value while improving compiler scheduling. `GetStringEntry` remains a separate full expression before reloading the window for queueing. No selected declaration is volatile.

The change preserves modulo-16-bit layout arithmetic and modulo-32-bit saved-coordinate arithmetic already present in the structural source. Tests specifically exercise high fields, centered negative X, alternate-layout wrap, maximum Y, poisoned saved coordinates and phase state. Narrow Y still promotes before `+8`; it does not truncate that constructor argument to 16 bits.

## Exact remaining differences

Word bytes below are in file little-endian order. The first three differences reflect replacing a retail control-record Y reload with an ANDI of cached Y, alongside moved cycle-store setup. The ten constructor-region differences are argument setup and saved-X store scheduling. They remain a real exact-match failure even though the bounded differential passes.

| Address | Retail bytes and instruction | Candidate bytes and instruction |
| --- | --- | --- |
| `801E6D64` | `f807c394` lhu $v1, 0x7f8($a2) | `1f80013c` lui $at, 0x801f |
| `801E6D68` | `1f80013c` lui $at, 0x801f | `1c9c22ac` sw $v0, -0x63e4($at) |
| `801E6D6C` | `1c9c22ac` sw $v0, -0x63e4($at) | `ffffe332` andi $v1, $s7, 0xffff |
| `801E6F78` | `00010634` ori $a2, $zero, 0x100 | `0800e226` addiu $v0, $s7, 8 |
| `801E6F8C` | `0800e226` addiu $v0, $s7, 8 | `ffffa732` andi $a3, $s5, 0xffff |
| `801E6F98` | `ffffa732` andi $a3, $s5, 0xffff | `0c00e724` addiu $a3, $a3, 0xc |
| `801E6FA4` | `40100300` sll $v0, $v1, 1 | `1f80013c` lui $at, 0x801f |
| `801E6FA8` | `21104300` addu $v0, $v0, $v1 | `309c27ac` sw $a3, -0x63d0($at) |
| `801E6FAC` | `1400a2af` sw $v0, 0x14($sp) | `40100300` sll $v0, $v1, 1 |
| `801E6FB0` | `fc070295` lhu $v0, 0x7fc($t0) | `21104300` addu $v0, $v0, $v1 |
| `801E6FB4` | `0c00e724` addiu $a3, $a3, 0xc | `1400a2af` sw $v0, 0x14($sp) |
| `801E6FB8` | `1f80013c` lui $at, 0x801f | `fc070295` lhu $v0, 0x7fc($t0) |
| `801E6FBC` | `309c27ac` sw $a3, -0x63d0($at) | `00010634` ori $a2, $zero, 0x100 |

Full disassemblies, unified and aligned diffs are in `final/`; machine-readable differences are in `remaining-differences.json`. All remaining differences lie before constructor JAL at 801E6FC0; code from that JAL through the return is identical. No broader matching claim is made for adjacent helpers or the full module.

## Functional verification

`validation/manifest.json`: **PASS**, current durable 463-case raw-retail-controller differential against the actual candidate body, compiled at O0, O2 and Clang UBSan (O1). All **11 semantic controls** compile and return exit 1 with explicit `DIFF FAIL`. Inputs, source, fixture and adapter are pinned. External helpers are explicit boundary stubs; normalized pointer arguments, order, return state, control/UI/window records, alternate window state, cycle and saved coordinates are compared. Helper implementation correctness, native adoption, rendered glyphs and in-game behavior are outside this fixture's claim.

The durable default uses `AUDIT_YIELDS=1`: all 315 retail slots execute, but one wait branch lacks both outcomes. Supplemental `validate_yields.py` compiles the same unmodified fixture and candidate with **AUDIT_YIELDS=3** at O2 and Clang UBSan. `validation-yields3/manifest.json`: **PASS**, all463 cases, all315 slots, all21 conditional branches with both outcomes. This explicitly checks repeated waiting without weakening any assertions. No native application was run.

## Minimal durable runner integration edits

The new source needs only four textual negative-control anchor updates, listed exactly in `mutation-anchor-updates.json`: `wrong_constructor_window`, `wrong_constructor_rows`, `missing_window_reset`, and `stale_queue_window`. The stale-window mutant now declares its own cached pointer before GetStringEntry, because the selected candidate intentionally has no general `window` local. The other seven anchors and all failure assertions stay intact. `validate.py` imports the repository runner and changes these four anchors in memory only; repository test files were not edited. `validation-inputs.json` pins the exact runner used.

To repeat in another empty output directory, use the recorded runner and the same four updates (the current wrapper deliberately targets the already-existing `validation/`, so do not overwrite evidence). Build reproduction is `python3 build.py candidate.c NEW_BUILD_DIRECTORY_NAME` from this scratch directory; it never writes production. `validate_yields.py` contains exact supplemental compile and run commands through the durable runner API.

## Acceptance separation

- Structural candidate under pinned helper-boundary fixture: **PASS**.
- Standalone controller C byte exactness: **FAIL**, 13 words remain.
- Full-module C exactness: **NOT_RUN by this lane**.
- Native adoption and live opening behavior: **NOT_RUN by this lane**.

Candidate and final matching artifacts are frozen; no further matching experiments are pending in this bounded task. Root may retain this candidate as nonmatching and integrate the four anchor changes independently.
