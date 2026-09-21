# Gear weapon/callback repairs

func_801E4754 native resource+18 pointer now reads the retained32-bit slot MenuUnk6.unk8[0x10]. Weapon writes, mask handling and special Gear5/13 paths retain retail behavior.

func_801E4928 divisor corrected15->120. Retail unsigned multiply0x88888889 high word shifted6 implements division120 for this16-bit domain. Difference from gear75 is halved toward zero, negative results clamp0, return truncates to byte. Original test fails val30/base0(native1,retail0).

Authority menu.bin SHA256 `82f84a24ac1fd1979754e1cc9c861405fb59ce95dd78db00a76f00c8f1bd177d`.4754..4928:468bytes SHA256 `e7e1e4ec9a790cd9682cfc19dfba373d415666ceafb734ca7e04f5313b6c6453`.4928..4998:112bytes SHA256 `3b9d16dbf519831e62b986577f59e4b0b0474e62c9e98cc7d5192745cda4c719`. Runner verifies every listing instruction.

bash pc_port/tests/run_menu_gear_weapon_test.sh:262,144 callback cases plus10,240 weapon cases per O0/O2/UBSan PASS; four negative controls rejected. Full production TU vs actual retail MIPS with no callee fixtures. Callback covers every16-bit numerator with bases0/1/255/low-byte quotient; weapon tests all declared Gear IDs, patterned20-byte records, mask condition on/off, full state/resources/table. No exhaustive claim for every possible byte combination.

Native and PSX menu builds PASS. Full all-overlay PSX build not rerun (earlier separate battle/main27 failure). No exact C matching or natural game/visual acceptance claim. No commit/push.
