# Equip character stat aggregation repair, 2026-09-12

Rebaseline HEAD f67fe692147e4b1496834ddb9aa444f1c23462f4. Existing list/description/resource7/17 work had advanced beyond the old pickup. This pass modifies only func_801E3A80 and adds its differential runner/evidence.

Retail authority menu.bin SHA256 `82f84a24ac1fd1979754e1cc9c861405fb59ce95dd78db00a76f00c8f1bd177d`; range801E3A80..801E3C2C,428 bytes SHA256 `8d67e5fbf3a3bcdb026e0fddda554d8d626f9dacbbb213d2d15805744db9adca`. Runner checks every assembly listing instruction against retail bytes.

Two defects fixed: native destination now starts at MenuUnk6.unkB8, accounting for widened resource pointers; special character-type4 calculation uses (weaponA+weaponB)*6/10. Retail mult by0x66666667 followed by high-word shift2 implements division by10 for these nonnegative values. Prior C divided by5. All seven outputs and their retail caps retained, including values>=21 becoming16.

Red: original production fails first whole-resource comparison. Final:16,896 cases each O0/O2/UBSan PASS; four negative controls reject old layout, old divisor, wrong cap, wrong bonus field. Full production translation unit against original MIPS, no helper call fixtures. Covers every declared character ID, both type branches,256 uniform byte patterns and512 varied patterns; whole resource/game-state compared. Does not establish aliasing/invalid-input or whole-menu visual parity.

Native build PASS. PSX menu overlay build PASS. Full PSX build FAIL in separate battle/main27.c func_8007D478 (parse error before h); concurrent battle work preserved. Linked function length428 equals retail length428, but bytes differ from offset4: exact match FAIL (exact.json). No byte-exact or end-to-end completion claim.

Next: verify remaining stat/gear helpers and natural Equip flow, and strengthen list/description renderer integration tests (existing description test skips two rendering helpers). Full original retail-accuracy goal remains incomplete. No commit/push or live game manipulation in this pass.
