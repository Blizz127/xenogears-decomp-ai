# Character equipment-effect recalculation

Production native implementation: func_801E36D4 in src/menu/main/misc.c. Original PSX assembly retained; exact C compilation match NOT claimed.

Retail menu.bin SHA256: `82f84a24ac1fd1979754e1cc9c861405fb59ce95dd78db00a76f00c8f1bd177d`.
Code 801E36D4..801E3A80 (940 bytes): `d30a47e2484fea016fafdedeeb1dec1f499592a5e944538136f7dbe4ba3325f0`.
Jump table 801C5204..801C522C (40 bytes): `7183798e87a6dbb586858caef9848fe90ae481ca40f2565ddb80c8f2fa252300`.
Every listing instruction and table word verified against retail; runner repeats this check.

The function clears the retail effect fields, processes three 16-byte accessory records, applies the effect-type switch (type6 has no additional action), and accumulates each enabled high-byte bonus flag with byte wrapping. It writes weapon data, replaces the primary and writes a secondary block for character type4, and combines the primary weapon mask when byte3 equals100. Native resource pointers use typed fields; game records retain retail byte layout.

Verification command: bash pc_port/tests/run_menu_equip_effects_test.sh. Original MIPS runs directly with no helper/callee fixtures. Compares whole game state and both whole 256-record equipment tables, plus the resource structure, against the full production translation unit. Cases cover all256 effect types, all256 bonus-flag combinations, both character-type branches, both weapon-mask branches, declared character IDs distributed across cases, and1024 additional mixed accessory-table patterns. This is not an exhaustive inventory or whole-menu proof.

Native and full PSX builds PASS; native function stubs76. Fresh runtime acceptance remains PENDING. Remaining core dependencies DE5CC,DF0D4,DFB68,DFF5C still INCLUDE_ASM, plus resource7/17 and allocation/cleanup audits. No game-state manipulation, runtime replacement, commit or push. Full Lahan goal remains ACTIVE.

Final stable runner: 263,168 cases each at O0/O2/UBSan PASS; four negative controls rejected. An earlier running shell read was disrupted when its script was edited; the complete stable runner was rerun and exited0 (tests.log).
