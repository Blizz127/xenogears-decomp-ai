# Gear preview real-callee integration

bash pc_port/tests/run_menu_gear_integration_test.sh:19,200 cases at each O0/O2/UBSan PASS; two negative controls rejected (skip initial weapon update, wrong mirrored equipment slot).

Entry801DFE2C executes actual801E3ECC/801E433C/801E4928/801E4754/801E3C2C implementations, compiled from the full production menu TU. Guest executes corresponding original menu.bin code with no bridge/callback fixtures. Retail overlay hash pinned by runner. Reads/writes compared across whole0x4600 GameState backing, both item tables, Gear-character mapping, native resource, menu and manager.

320 patterned cases x all20 declared Gears x three party slots. First256 cover accessory effect type byte values0..255;64 mixed tables. All gear-special/mirroring branches included. Synthetic input tables have valid mapping IDs; no claim for invalid allocations or all possible inventory combinations. This proves the composed computation on covered inputs, not visual Equip/menu state-machine acceptance. Previous isolated-helper evidence remains separate.

Production changes associated with this composition are the preceding Gear stat/effect/caller repairs and this pass's weapon pointer and4928 divisor repair. Native and PSX menu builds passed after those changes. Full matching byte parity and fresh natural game acceptance remain unproven.
