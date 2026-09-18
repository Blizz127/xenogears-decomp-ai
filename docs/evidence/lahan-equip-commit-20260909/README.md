# Equip selection commit and inventory reconciliation

Native func_801DF0D4 implemented from retail. PSX retains original assembly; no exact C match claimed.

menu.bin SHA256 `82f84a24ac1fd1979754e1cc9c861405fb59ce95dd78db00a76f00c8f1bd177d`. Code801DF0D4..801DF5D0,1276 bytes, SHA256 `0e1c4e791ef46943efacca04584ae9616553328af1e2d79f44e9dbae091bbc1a`. Runner verifies every instruction against retail bytes.

Retail behavior: restore the saved equipment and return0 on zero primary/alternate selection; update alternate-selection marker to100; decrement first matching current-item quantity; return the previous item to first match or first empty ID slot; clear zero-count IDs and cap counts>=100 to99. Byte arithmetic wraps before cleanup. Return1 only for nonzero primary selection by characterID4 (including Gear mode).

GameState is only a0x22B8 prefix declaration. Existing native data_game_state.c allocates0x4600. The helper's explicit native assembly-name alias describes that backing storage for tail marker reads/writes without suppressing bounds checking. Initial prefix-only test fault and subsequent UBSan fault led to this correction. Tests compare the complete0x4600 storage.

bash pc_port/tests/run_menu_equip_commit_test.sh:10,240 cases each O0/O2/UBSan PASS; four negative controls rejected. Full production translation unit vs original MIPS with no external helper fixtures; full backing storage, work, resource, manager and menu comparisons plus exact return value. Includes primary/alternate groups, character/Gear modes, categories0..4, high argument bits, distributed declared character/Gear IDs, absent/duplicate/first/last inventory matches, empty/full tables, marker thresholds0/99/100/255, zero selections, wrapping quantities and cleanup caps. These synthetic states establish bounded semantic evidence, not natural menu acceptance.

Remaining DE5CC list builder and DFF5C description/render helper, resource7/17, init/cleanup raw434 pointer and existing stat helper stride audits remain. Full Lahan goal ACTIVE. No runtime replacement, commit or push.
