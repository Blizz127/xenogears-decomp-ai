# Gear stat aggregation func_801E3C2C

Native stub replaced with retail transcription; PSX original ASM retained. First argument is a pointer (prototype corrected from s32). Resource output offsets9C..B6 map into native MenuUnk6.unk20+7C, accounting for widened pointers.

menu.bin SHA256 `82f84a24ac1fd1979754e1cc9c861405fb59ce95dd78db00a76f00c8f1bd177d`. Retail range801E3C2C..801E3ECC (672 bytes), SHA256 `0c1295785fcf40aa53b3ca5026e6d92fb4d8746225a55f6794f352273d716eb1`. Runner verifies every assembly listing instruction against retail bytes.

Preserved effects: flag1000 at GameState22B6 changes D_801E9808[9] to10; Gear7 updates six game-state fields from the retail character data; aggregate reads mapped character bonuses; Gear5/13 dual-weapon term divides by10. Byte and halfword outputs truncate rather than inventing caps. Gear records retain0xA4 strides.

bash pc_port/tests/run_menu_gear_stats_test.sh:30,720 cases each O0/O2/UBSan PASS; four negative controls rejected (HP scaling, division, mapping target, subtraction). Whole production translation unit vs original MIPS; no helper call fixtures. Tests all declared Gear IDs, flag on/off,256 uniform and512 varied byte patterns, whole game-state/resource/mapping side effects. Mapping entries are bounded synthetic character IDs; this does not prove every runtime caller or resource lifecycle.

Native caller func_801DFE2C still has old0x28 strides and raw native offsets, and func_801E433C remains missing. These are the next dependencies; this helper alone does not establish Gear or Equip runtime acceptance. No exact C claim, no commit/push. Full retail fidelity remains incomplete.

Build results: PSX menu PASS. First native LINK OK was rejected as runtime evidence: only menu.elf existed, so generator assumed586 unclassified symbols were functions. Rebuilt all four remaining stub-classification overlays successfully, then rebuilt native successfully (native-final-build.log); use only the final binary. Full PSX all-overlay build was not rerun: preceding battle/main27 C-declaration failure remains outside this lane. Build-script partial-ELF classification guard still needs hardening.
