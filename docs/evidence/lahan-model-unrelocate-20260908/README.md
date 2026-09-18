# Model pointer unrelocation — 2026-09-08

The model-copy relocation omitted from native `func_801E742C` requires `func_8002C4BC`. Its old C omitted the fourth model pointer subtraction and added the model base to a nested table pointer that was already absolute. This repair restores both operations and the retail backwards nested-entry loop ending at -1.

Retail authority: SLUS_006.64 bytes at file offset0x1CCBC, entry8002C4BC through8002C59C,224bytes/56instructions. Exact input hashes and source pins are in provenance.json. The first valid oracle run fails on old source case1 with no nested table, isolating the missing fourth pointer operation (red.log; before.c).

Full production TU vs raw retail MIPS instructions:1600cases each at O0/O2/UBSan, all56instructions exercised, full64KiB data/guard region and return compared. Five negative controls rejected: missing fourth pointer, missing nested pointer, missing nested entry update, missing flag clear, wrong count return. See green.log. Compiler harness setup errors were resolved before the valid red run; they are not behavioral evidence.

Full PSX build passes. Compiled helper is236bytes versus224retail: exact matchFAIL (compiled-size.json); do not call behavioral coverage a1to1 binary match. Native build log records the rebuilt binary; live frozen runs do not receive this repair.

The constructor relocation was subsequently restored and tested in ../lahan-model-relocation-20260908. Burning-Lahan movie4 OOM is not yet causally verified as fixed. See scratchpad/lahan-natural-20260908-heap-trace2/model-relocation-diagnosis.json for the omitted retail sequence and preserved heap owner. Full start-to-end Lahan retail accuracy remains open. No commit/push.

Further exact-match work: retail-disassembly.txt records the original instruction order. Isolated source candidates in scratchpad/lahan-model-unrelocate-exact remain nonmatching: retail-order228bytes; typed-record with an eight-byte local frame236bytes. Neither was installed. exact-candidates-20260908.json retains comparisons. The later CompMatrix replay did complete STR4/frame563 and reach aftermathfield3; see ../lahan-compmatrix-bridge-20260908/live-validation.json. This supersedes the older pending movie observation above, without establishing full retail parity.
