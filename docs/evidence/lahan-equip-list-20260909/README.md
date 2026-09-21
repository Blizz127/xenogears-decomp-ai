# Equip list builder (func_801DE5CC)

Native production func_801DE5CC implemented from retail. PSX retains INCLUDE_ASM; no exact C match claimed.

Authority menu.bin SHA256 `82f84a24ac1fd1979754e1cc9c861405fb59ce95dd78db00a76f00c8f1bd177d`.
Code 801DE5CC..801DF0D0, 2824 bytes, SHA256 `a37ca83d0b33844cee741dd5aba842334fb92be59a5465aff4d30b78736deff5`.
Runner verifies every listing instruction word against retail bytes.

Behaviour (retail ASM): clear D_801EA730/D_801EA7F8; filter character/gear weapon and accessory inventories into those lists with equip-mask checks (func_801C865C / func_801C8678); render up to eight rows from `page` via name/quantity string helpers; return max(0, filled-8). Accessory categories reserve list index 0 (filled starts at 1). Resource table pointers at MenuUnk6 +0/+4/+0x14/+0x18 are read as PSX-width u32 slots.

Test: `bash pc_port/tests/run_menu_equip_list_test.sh`.
352 cases each O0 and UBSan (O0) PASS; four negative controls rejected (weapon_id_gate, accessory_start, gear_stride, return_bias).
Coverage: character weapons/accessories, gear weapons, both groups for categories 0..3 (and cat 4 group 0). Open gaps: gear accessories (mode&&cat), group!=0 with category==4 (retail type=5 without mask init), O2 owner optimization still diverges on some gear-weapon seeds, and native D36E0 vs PSX-width listBuf+0x800 (harness uses -DXENO_EQUIP_LIST_TEST to skip that call).

Native full port rebuild and Lahan Equip runtime acceptance still PENDING. Full goal ACTIVE. No commit/push.

Native podman build: LINK OK, 73 function stubs (DE5CC no longer stubbed). Runtime acceptance still PENDING.
