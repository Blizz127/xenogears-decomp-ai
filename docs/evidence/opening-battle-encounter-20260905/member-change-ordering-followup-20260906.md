# Member-change ordering follow-up

Date: 2026-09-06  
Scope: read-only inspection of the completed private build; no production or shared-checkout edits and no rebuild.

## Finding

The apparent source-order inversion is caused by GCC 2.6 file-scope inline-assembly emission. `src/member_change_menu/main/misc.c` keeps `INCLUDE_ASM("asm/member_change_menu/nonmatchings/main/misc", MemberChangeMenuSwapCharacters);` at line 1982, after `MemberChangeMenuFreeCursors` and before `MemberChangeMenuMainLoop`. However, the compiler output places the corresponding `#APP` block before the generated C function bodies: `build/src/member_change_menu/main/misc.c.s` emits the `.include` block before the first C `.text` body. The input object confirms this ordering:

| input object | symbol | offset | size |
|---|---|---:|---:|
| `build/src/member_change_menu/main/misc.c.o` | `MemberChangeMenuSwapCharacters` | `0x0` | `0x1cc` |
| same | `MemberChangeMenuIsCharacterFlagSet` | `0x1cc` | `0x1c` |
| same | `MemberChangeMenuFreeCursors` | `0x5c98` | `0x44` |
| same | `MemberChangeMenuMainLoop` | `0x5cdc` | `0x394` |

The current link map therefore starts `SwapCharacters` at VMA `0x801c5018` and `IsCharacterFlagSet` at `0x801c51e4`. The standalone retail assembly object has the expected order and matching Swap size:

| input object | symbol | offset | size |
|---|---|---:|---:|
| `build/asm/member_change_menu/main/misc.s.o` | `MemberChangeMenuIsCharacterFlagSet` | `0x0` | `0x1c` |
| same | `MemberChangeMenuFreeCursors` | `0x5aec` | `0x44` |
| same | `MemberChangeMenuSwapCharacters` | `0x5b30` | `0x1cc` |
| same | `MemberChangeMenuMainLoop` | `0x5cfc` | `0x394` |

Retail symbol authority agrees: `IsCharacterFlagSet = 0x801C5018`, `SwapCharacters = 0x801CAB48`, and `MainLoop = 0x801CAD14`. The nonmatching Swap assembly has the exact retail range `0x801CAB48..0x801CAD13` and `.size` `0x1cc`.

## Independent remaining drift

Comparing the current C object against the complete retail assembly object after subtracting the hoisted `SwapCharacters` block shows exact offsets and sizes through `MemberChangeMenuResetRenderContext`. The first independent size difference is `func_801C59E0`: C `0x190` (400 bytes), retail `0x1b0` (432 bytes). Every following common symbol through `MemberChangeMenuFreeCursors` is then `0x20` early; `MainLoop` and `Main` retain sizes `0x394` and `0xd8`.

## Remedy candidate

Moving the `INCLUDE_ASM` line within the C file cannot repair placement under this compiler. The narrow structural remedy is to model `SwapCharacters` as a separately ordered link input (or temporarily use the complete retail assembly module) between the C prefix and suffix. That remedy must be evaluated together with a retail-backed repair of `func_801C59E0`'s missing `0x20`; reordering alone leaves the `0x20` drift. The ledger entry `known red since 310c391` is provenance, not a decision against repairing this module.

Pins:

- C source SHA-256: `10150ba1f15bebfb54503cb47ff287ed708f5cee95e1dcb1bf9cbcf11ffe8542`
- generated C assembly SHA-256: `2aa1a1707a3cfb5c9e548f5f1e86cec7249650c55b02fedc7be197ec7b9bf535`; input C object SHA-256: `f31fac45615f0042decfe4eda85fe5dd7969eb6cfaf6a2cb3911234c9ef650e8`
- retail full assembly source SHA-256: `b93a96c63a77993d2e5fa10fc9eeb808cc4923f2cc5f9cf69e18988c5998104a`
- nonmatching Swap source SHA-256: `ba582990c8d001a907c00ca3b4a282b9629d8341aff628099b465966d6ddba6e`
- current map: `build/out/member_change_menu.map`, SHA-256 `41de8717548ee6f084b0ae06eebf6934c7df343d78ed53fbc8419ec335dffa66`


## Root continuation choice

The separate-link-input approach above is an unimplemented option. The user's
full-decomp objective also supports reconstructing `SwapCharacters` as exact C
at its existing source position, which would remove this assembly-hoisting
cause without introducing a new module split. Evaluate that path first when
this menu lane is taken up. Its 0x1CC-byte retail body and the independent
`func_801C59E0` difference require exact-byte checks; neither is repaired here.
