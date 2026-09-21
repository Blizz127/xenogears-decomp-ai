# Shop-menu matching diagnosis (2026-09-06)

Scope: read-only comparison of the existing final95a0 private build and the
retail `shop_menu.bin`. No repository/config/build inputs were changed. The
scratch compiler probes are under this directory.

## Pinned baseline

The final95a0 isolated run completed 472/472 tasks and left three checksum reds:
`slus_006.64`, `field.bin`, and `shop_menu.bin`; `member_change_menu.bin` was
exact. The relevant artifact pins are:

| artifact | size | SHA-256 |
|---|---:|---|
| generated `build/out/shop_menu.bin` | 55300 (`0xd804`) | `0f22fa9ae02881a6e18f0152034ed0e76aa02e6e2a57ed7fb89c68c88e4537cc` |
| retail `disc/shop_menu.bin` | 55296 (`0xd800`) | `7890e14bcabddcf85368de10783ecff8daa166dc5ac254e06db5e27959cab4cf` |
| generated `build/src/shop_menu/main/misc.c.o` | -- | `c0bbb3eecf1aa0a0e546141550175090e8b797c4184a37ab26ee1eb07072becb` |
| current/private `src/shop_menu/main/misc.c` | -- | `c999438dfc8b1cf68b181b2e6ab76dbfa5223f498b39a80353a7eae4718855e5` |
| retail authority `asm/shop_menu/main/misc.s` | -- | `1ccc18d477bcebf040bbcc4c2d2d359420057dc6f05bab3696becea94a9f8073` |

## Primary cause: object layout/order

The linker map places the generated module at VMA `0x801c5000`. RODATA is
`0x40` bytes. The generated `misc.c.o` contributes one `.text` input of
`0xcf14` bytes at `0x801c5040..0x801d1f54`, followed by `CF50.data.s.o`
starting at artifact offset `0xcf14`. Retail's final function starts at
`func_801D1F10`/artifact offset `0xcf10` and ends at `0xcf50`; its data starts
there. Thus the generated text/data boundary is four bytes late and the module
is four bytes too large.

The source has 16 file-scope `INCLUDE_ASM` blocks (lines 232, 270, 272, 2136,
2189, 2232, 2388, 2580, 2582, 2598, 2600, 2917, 2919, 2921, 2923, 2925).
GCC 2.6 emits these `__asm__` blocks at the front of the translation unit's
`.text` input. The first generated map symbols are therefore
`ShopMenuLoadResources`, `func_801C5A7C`, `func_801C5CBC`, `func_801CBCF0`,
`func_801CC278`, `func_801CC720`, `func_801CCFF4`, `func_801CDD14`,
`func_801CE480`, `func_801CE91C`, `func_801CEB3C`, `func_801CFF58`,
`func_801D05BC`, `ShopMenuHandleSoldItems`, `ShopMenuSellMenu`, and
`ShopMenuSellEquipmentMenu`. Retail instead begins with the C functions at
`func_801C5040`, the manager functions, then `ShopMenuLoadResources` at
`0x801C54B4`, matching the source interleaving.

This is why the raw artifact diff appears to contain broad pointer/code
changes: the data table words at generated offset `0x10` already point at
shifted generated VMAs (`2c901c80...`) while retail points at the retail
layout (`48fe1c80...`). It is a layout relocation effect, not 55 KB of
independent instruction errors.

Moving `INCLUDE_ASM` calls within the same C file will not fix this GCC
file-scope-asm hoisting. The source-backed repair is to preserve retail input
order at the object/linker level: split the C and included ASM into ordered
input objects/sections (or, as a provenance fallback, consume the complete
retail assembly object). Replacing all 16 blocks with C is a larger decomp
lane and is not justified by this audit.

## Secondary genuine size mismatch

After mapping common symbols and sizes, `func_801CE8D8` is the one small C
mismatch: retail is 68 bytes (`0x44`), while current generated assembly is 72
bytes (`0x48`). The current source is `misc.c:2584-2596`; it uses a pointer-end
loop. Retail bytes at module offset `0x98d8..0x991b` are:

```
0e00c018 21180000 ff00e730 2130c500 00008290
00000000 00404710 00000000 000a3904 53a07080
00000000 0100a524 2a10a600 f6ff4014 01008424
0800e003 21106000
```

The retail operation is `blez count` with result zero in the delay slot,
`andi target,0xff`, `addu end,count+pValues`, then a byte scan. The loop uses
signed `slt pValues,end` and advances both pointers. The current generated
function is 72 bytes and uses the equivalent scan but has a different
control/register schedule.

A scratch GCC 2.6 candidate (`v12.c`, SHA-256
`b40e3d4ab29ef4549f2ceab316fb6f5f2b8b2a161b048764473f6c9d808f32cf`) compiled
to the same 68-byte size and matched the retail loop and signed compare; its
only remaining difference is the order of the independent `addu end,...` and
`andi target,...` setup instructions. Its bytes SHA-256 is
`d5a9c995ec9e393e9db22c1dbf09cc134981b0ed4fc96a8434cac47bae2a0401`, versus
retail slice SHA-256
`62321bfbdb3756149ced5268ea0e7022afa1688253efb270110f1a15d3d8740c`.
This is a focused source-shape repair candidate after layout is corrected; it
must be validated in the full source before production use.

## Prioritized action

1. Repair ordered placement of the 16 included ASM bodies relative to the C
   bodies, then re-run the module comparison. This removes the broad relocation
   mismatch and the four-byte text/data displacement.
2. Rework only `func_801CE8D8` toward the retail `blez`/signed-end-loop shape,
   using the pinned retail slice as the exact gate. The tested `v12` shape
   proves the four-byte size reduction and gets within an 8-byte instruction
   schedule difference, but is not itself an exact-match claim.

No complete make check was rerun because no matching input changed.
