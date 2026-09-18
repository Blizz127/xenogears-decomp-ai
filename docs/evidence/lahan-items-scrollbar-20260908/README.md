# Items scrollbar creation

Natural Items entry in the prior items-layout run crashed in func_8002675C with tableNULL, called by func_801D3C4C. The core and crash-gdb.log remain under that run. Native raw menu offsets selected the wrong atlas/window/render-context fields.

Retail func_801D3C4C is801D3C4C..801D3DB0,89 instructions/356bytes. All annotated words match pinned disc/menu.bin (provenance.json). Its fifth argument is loaded at801D3C74 from sp+58 after a48-byte frame; the previous four-parameter C definition and caller comment incorrectly claimed it was unused. The caller now passes width and height as separate fourth/fifth arguments; width is unused by this callee.

The production routine now uses named MenuWindow members and g_Menu atlas/context fields, reloading g_Menu between atlas helper calls while retaining the original window pointer. It restores bottom cap Y=u16(y)+u16(h)-8, empty-bar height=u16(h-8), and the retail zero-extended FFF8 helper argument. Vertex-coordinate sums wrap to16bits; texture helper sums retain their wider arithmetic.

The extracted production-body test uses real native structures and six explicit helper boundaries.131072 cases each O0/O2/UBSan cover all16-bit heights, varying coordinates with high signed bits, all seven valid windows, and stable/helper-replaced globals. Six controls reject width-as-height, missing8 subtraction, signed cap height, missing coordinate wrap, wrong cap pair, and cached menu globals. The caller is checked for the five-argument source call; there is no caller instruction-execution oracle here. This does not execute the atlas/GTE/vertex helpers or prove rendered parity. The test-runner pin insertion initially caused a NameError in its mutant-generation block; import was corrected and the final entire run passed (tests.log).

Native and fullPSX builds pass (73nativefunctionstubs). Exact match remains FAIL:352compiled bytes versus356retail. No relocation-normalized byte equivalence is claimed for this routine. Initializer/teardown exact matches from the prior evidence remain function-level results, not whole-ROM proof.

Fresh runtime acceptance is PENDING in scratchpad/lahan-natural-20260908-items-scrollbar, frozen SHA39fa0f9bec6f0d6dca9aaf0a951d1df8ac15dcd2a2cd036ea04ba56f29525d61. Driver session13356 runs ordinary NewGame input tofield14 then stops. No state forcing or commit/push. FullLahan accuracy remains unfinished.

Runtime attempt progressed to handle submission but failed there; see subsequent lahan-scroll-handle-20260908 evidence. The scrollbar build has not received a visual acceptance pass. compare-object.py now also resolves its12 relocations; object-comparison.json confirms instruction differences and exactFAIL.
