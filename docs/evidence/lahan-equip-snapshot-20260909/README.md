# Equip preview snapshot and cancel

Native production implementations: `func_801DF5D0` and `func_801DF890` in src/menu/main/misc.c. PSX retains original assembly; exact C matching is NOT claimed.

Authority: disc/menu.bin SHA256 `82f84a24ac1fd1979754e1cc9c861405fb59ce95dd78db00a76f00c8f1bd177d`. Both assembly listings were checked instruction-by-instruction against these retail bytes:

- 801DF5D0..801DF890: 704 bytes, SHA256 `13ac41d116ecc48f263f373d2e1a7f4061f87ca18c9596d7e993fd67ac42ac90`.
- 801DF890..801DFB68: 728 bytes, SHA256 `9967ef12dc1d1d3b1f8fe13b5290152d1a7e50d8ad8104d037e1a233b3bd4762`.

Snapshot copies nine stat halfwords and three equipment banks (five character bytes or four Gear bytes each). Cancel restores five/four bytes in the first two banks and exactly three in the third. It does not restore the stat snapshot. Records use 0xA4 strides. Native resource fields account for widened pointers; the work allocation uses the existing raw 32-bit menu slot convention.

`bash pc_port/tests/run_menu_equip_snapshot_test.sh`: 10,560 independent cases each at O0, O2, UBSan PASS. Executes original retail MIPS without external helper fixtures and compares full game state, work allocation, resource record, menu and manager against full production translation unit. Covers every declared character/Gear ID, three party slots, both modes and operations, four deterministic byte patterns and high argument bits. No aliased/invalid allocations are claimed. Four negative controls (wrong stride, truncated stat copy, excessive cancel restore, wrong Gear bank) all rejected.

Native and full PSX builds PASS. Native function stubs now 77. These tests are semantic evidence, not visual acceptance or exact compiled-byte proof. Remaining Equip helpers and allocation/resource dependencies are still incomplete. No runtime replacement or new natural run in this pass. No commit/push. Full Lahan fidelity goal remains ACTIVE.
