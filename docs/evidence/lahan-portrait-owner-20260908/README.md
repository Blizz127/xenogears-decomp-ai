# Textbox portrait ownership, 2026-09-08

The ordinary NewGame run `scratchpad/lahan-natural-20260908-scroll-handle` reached scenario 15 after Yui, Citan's roof scene, the workshop sequence, dinner and farewell. Descending the mountain triggered the “Giants...?” textbox with a striped Fei portrait. The live process remains on its original frozen executable; no state edits or breakpoints were used.

Read-only captures identify textbox owner 23 and speaker 1. Owner flags at +0x12C are 4 (cache slot 1), speaker flags are 0 (slot 0); the portrait cache contains face 0 in slot 1. The old consumer selected slot 0. Owner dialogFlags are 0x10000, speaker dialogFlags are zero. Captures and the full CPU VRAM mirror are retained in that run as `striped-portrait-*`.

Retail `func_8007F8DC` loads argument 7 into s4 at 8007F8E4 and uses that actor for dialogFlags, face ID, cache selector and portrait text spacing. Argument 8 is used for projected position and the final actor-flags check. The C consumer now follows that separation. `provenance.json` and `retail-function.s.txt` verify all 451 annotated instructions against the pinned retail field binary. This data-flow verification is not an instruction-execution oracle.

The initial 96 distinct-owner/speaker regression cases fail on the saved old source; existing same-actor tests passed and had missed this case. The final suite has 192 cases spanning three cache slots, both orientations, independent face presence, independent actor flags and low/high dialogFlags precedence. It passes at O0 and O2 alongside the existing portrait rendering checks and negative controls. Helpers remain harness boundaries; these tests do not execute a GPU or CD loader.

Native and full PSX builds pass (73 native function stubs remain). Exact match is FAIL: 1396 compiled bytes versus 1804 retail. The size gate deliberately fails; no relocation-normalized equivalence is claimed. Other differences in this large routine remain outside this bounded fix. Fresh natural-run verification of the corrected nighttime portrait is pending. The earlier striped wall-painting observation is not explained or repaired by this finding.

The broader textbox timing tests also pass at O0/O2/UBSan with identical stdout and empty stderr in the toolchain container. The runner stopped afterward because the container lacks rg; equivalent post-validation was completed on the host and recorded in `timing-certificate.txt`. Earlier host attempts are retained: GCC could not link its missing UBSan library, and Clang rejected existing unrelated pointer/integer conversions.
