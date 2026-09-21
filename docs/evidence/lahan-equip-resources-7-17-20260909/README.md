# Equip resource modes 7 / 0x17 (description banks)

Date: 2026-09-09

## Retail
- Dispatcher `func_801C72BC` jump-table entries:
  - mode 7 → `801C76FC..801C7788`: LZSS decompress archive[0x35..0x38] into listBuf (`menu+0x434`) at +0xA00/+0xA04/+0xA08/+0xA0C
  - mode 0x17 → `801C7A54..801C7ACC`: HeapFree those four slots in the same order
- Called from Equip init `func_801DE2C8` (7) and teardown `func_801DE36C` (0x17)
- menu.bin SHA256: `82f84a24ac1fd1979754e1cc9c861405fb59ce95dd78db00a76f00c8f1bd177d`
- Dispatcher body SHA256 (unchanged): `2f315236e4074f75cf1121fc8e865958635fabf229c3229552d49636857971c1`

## Port
- Native arms added beside existing 3/0x13 in `src/menu/main/misc.c`
- listBuf pointer and bank slots use PSX-width u32 stores (unk42C[2] at +0x434)

## Gate
- `bash pc_port/tests/run_menu_equip_resources_test.sh`
- Now covers modes 3, 0x13, 7, 0x17 × 64 seeds × 4 upper-byte patterns = **1024 cases** at O0/O2/UBSan
- 4 negative controls: weapon_entry, extra_bank, desc_bank7, free17_order

## Not proven
- Fresh Lahan Equip runtime acceptance
- DE2C8 still writes `*(void**)` into +0x434 (8-byte on host); LE low half matches retail 4-byte slot but may clobber +0x438

- Also fixed DE2C8/DE36C to use MenuStoreRawPointer/MenuRawPointer at +0x434 (PSX-width).
