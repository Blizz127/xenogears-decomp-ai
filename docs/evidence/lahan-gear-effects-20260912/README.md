# Gear accessory effects func_801E433C

Native stub replaced from retail; PSX retains original assembly. First argument corrected to pointer. Resource14 uses the existing32-bit slot in native MenuUnk6.unk8[0xC], while Gear records use0xA4 strides.

menu.bin SHA256 `82f84a24ac1fd1979754e1cc9c861405fb59ce95dd78db00a76f00c8f1bd177d`. Code801E433C..801E4754,1048 bytes SHA256 `c0b2aa6b7f9155f6fc0f8f8189638cfa845c8c22e4f2e47149e655d81d5aaf98`. Jump table801C524C..801C5278,44 bytes. Runner verifies all instruction/table words.

Preserved behavior: reset explicit bonus fields and16-element resistance array; retain high mask bits; clear mapped character flags withFB6F; aggregate three28-byte accessories; process all11 effect types including9->10 fallthrough and three paired-bit prerequisites. Byte/halfword sums wrap. One4928 callback supplies byte4A. Final status bit8000 update uses the old status destination but reloads current character mapping after callback.

bash pc_port/tests/run_menu_gear_effects_test.sh:32,896 cases per O0/O2/UBSan PASS, four negative controls rejected. Verbatim production function extracted and compiled against headers, compared with original MIPS. Full0x4600 GameState backing, item table, mapping and resource compared. Covers all256 effect types plus mixed tables, single-bit effect masks, all64 paired-flag combinations, distributed Gear IDs, callback mapping mutation, and return truncation. Callback4928 is a shared fixture; its implementation and complete callee integration are not proved here. Whole production TU separately build-checked.

Next: audit native801E4754 resource pointer and801E4928 against retail; then integrate Gear/Equip path. Exact C matching and fresh natural runtime acceptance remain unproven. Full retail goal incomplete. No commit/push in this pass.

Native build PASS66function/572data stubs; PSX menu overlay PASS. Full PSX all-overlay build not rerun (previous separate battle/main27 failure).
