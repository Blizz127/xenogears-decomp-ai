# Gear preview caller func_801DFE2C

Corrected record stride0x28->0xA4, native resource-pointer access, and six copies from Gear aggregate to preview stats. Retail reloads manager/resource/gear mapping between callbacks and resource again after the second callback; production follows that order. Copies are three halfwords and three zero-extended bytes.

Retail menu.bin SHA256 `82f84a24ac1fd1979754e1cc9c861405fb59ce95dd78db00a76f00c8f1bd177d`. Range801DFE2C..801DFF5C (304 bytes), SHA256 `2c8a9abd50d9df5b7b9bc1db072460e5c856cf04da26dc5e80695753ebd4e127`. Runner verifies all original assembly instruction words.

bash pc_port/tests/run_menu_gear_caller_test.sh:16,896 cases each O0/O2/UBSan PASS. Original MIPS compared with verbatim-extracted production caller body and two shared callback fixtures. Cases cover256 patterned resources, all character IDs, three party slots, unchanged/changed callback ownership, gear mapping, event order/arguments, full resource/state/menu/manager effects. Native full translation unit is separately build-checked. Callee behavior itself is NOT verified here.

Four negative controls rejected: wrong stride, stale resource after second callback, wrong source field, halfword read instead of byte. Initial reload mutant retained the first resource, which happened to equal the final resource after two switches; that mutant was corrected to retain the pre-second-call resource. Final mutant now differs observably and is rejected.

Next: func_801E433C remains unported and func_801E4754 resource pointer still needs audit. Build partial-ELF classification guard remains unresolved. No exact compiled-byte claim or fresh natural runtime acceptance. No commit/push.

Native build PASS (67 function stubs,572 data symbols). PSX menu overlay PASS. Full all-overlay PSX build not rerun; prior battle/main27 failure remains outside this pass.
