# Lahan Equip description renderer `func_801DFF5C`

Date: 2026-09-09

## Retail range
- `801DFF5C..801E0430` (1240 bytes, 310 instruction words)
- menu.bin SHA256: `82f84a24ac1fd1979754e1cc9c861405fb59ce95dd78db00a76f00c8f1bd177d`
- Code SHA256: `1c1691d436ba8a98028f0236a10ad394f3071739f6fc23b1354debc78172cdaa`

## Behavior (from ASM)
- List index = row+page → `D_801EA730[index]`; modeFlag≠0 forces id `0xFF` then replaces from equipped slots
- kind = (group==0 && category!=0 ? 1 : 0) + gearMode*2 → description banks at listBuf+0xA00/A04/A08/A0C
- Three GetStringEntry lines at itemId*3..+2 staged at listBuf+0x880; visible flag listBuf+0xA18

## Port
- `#ifndef XENO_PC_PORT` keeps INCLUDE_ASM (no exact C match claimed)
- `#else` native body in `src/menu/main/misc.c`
- `-DXENO_EQUIP_DESC_TEST` skips E7C50/C851C (PSX-width MenuString staging); production calls both

## Gate
- `bash pc_port/tests/run_menu_equip_desc_test.sh`
- 1920 cases × O0 + UBSan PASS
- 4 negative controls: kind_bias, entry_stride, visible_flag, equip_weapon

## Notes
- Guest MenuManager must be mapped before inflated native SystemMenu (sizeof 0x2318) or character-id reads alias into menu.
- Native `build_port.sh` LINK OK; `func_801DFF5C` / `func_801DE5CC` absent from the 72 function stubs.
