# Equip preview item writer

Native production func_801DFB68 now writes the selected list byte into the retail character/Gear equipment bank. PSX retains original assembly; exact C matching NOT claimed.

Authority menu.bin SHA256 `82f84a24ac1fd1979754e1cc9c861405fb59ce95dd78db00a76f00c8f1bd177d`. Code801DFB68..801DFE2C,708 bytes, SHA256 `73babb97199e63bfb0b892ee70a146589841ec860448ab885fb6c67b02fe1ddb`. Runner verifies every listing instruction against retail.

Primary group rejects signed categories outside0..3. Category0 writes the primary weapon, categories1..3 write accessories. Nonzero group writes the alternate equipment bank plus the full signed category. Party slot, group and mode use low bytes. Row+scroll uses32-bit arithmetic. No inventory validation was invented inside this helper; caller bounds remain its responsibility.

Test: bash pc_port/tests/run_menu_equip_preview_test.sh. 14,336 cases each O0/O2/UBSan PASS; four negative controls rejected. Full production translation unit versus original MIPS, no helper fixtures. Whole game state, list, menu and manager compared. Declared character/Gear IDs distributed across512 seed/index cases, three party slots, categories-1..5, both modes/groups, high argument bits, wrapping row+scroll sum. The512-byte list is a synthetic isolated buffer; this does not prove the live caller's list bounds or list generation.

Native and full PSX builds PASS. No runtime acceptance or exact C proof. Remaining DE5CC(list builder), DF0D4(no direct calls), DFF5C(description/rendering) plus allocation/resource/stat helper audits remain. Full Lahan goal ACTIVE. No commit/push.
