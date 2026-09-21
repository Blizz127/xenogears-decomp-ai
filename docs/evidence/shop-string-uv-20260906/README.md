# Shop menu string UV/tpage rebuild — 2026-09-06

`func_801C5A7C` now has a C body instead of an `INCLUDE_ASM` placeholder.
It rebuilds both render-context primitives of a `MenuString`: `SetPolyFT4`,
`SetSemiTrans`, `SetShadeTex`, the vertex colour, the texture page, the four
UV corners and the system palette CLUT, then clears `unk7F`.

## Result

The actual production translation unit `src/shop_menu/main/misc.c`, compiled
with the project's authoritative GCC 2.6 preset and the unmodified
`config`/`linker` inputs, emits all **576 retail bytes / 144 instructions**
for `801C5A7C..801C5CBC`.

| item | value |
| --- | --- |
| retail function SHA-256 | `d71c52a1f841c5be08425bba883a28d3f2ac47acccc4ee1062c84316f1bd3e09` |
| production TU function SHA-256 | `d71c52a1f841c5be08425bba883a28d3f2ac47acccc4ee1062c84316f1bd3e09` |
| bytes | 576 / 576 |
| instructions | 144 / 144 |

The production object `build/src/shop_menu/main/misc.c.o` was relinked so the
function lands on its retail address, because shop function ordering is still
unresolved and the overlay currently places this body at `801C9738`. Only the
six dependencies the body actually calls or reads were pinned to their retail
addresses (`SetPolyFT4`, `SetSemiTrans`, `SetShadeTex`, `GetTPage`,
`g_SystemPalette1`, `g_SystemPalette2`); every other undefined symbol got a
placeholder. This is a function-body claim, not a whole-overlay layout claim.

## The two source-shape facts that were load-bearing

The previous candidate was four bytes short and had two wrong words. Both
came from source shape, not semantics.

1. **`blend` is 16-bit.** With an `s32` accumulator GCC emits
   `or v0,v0,s1`; the retail encoding is `or v0,s1,v0`. Swapping the source
   operand order does nothing, because GCC evaluates the side-effecting
   `GetTPage` call first and swaps the commutative operands itself. Declaring
   `blend` as `u16` restores the retail operand order at zero instruction cost.
2. **The `u2` slot is fed by a mid-block assignment.** Retail spends one extra
   instruction, `move v0,s3`, on the third U corner while the other three read
   the hoisted loop invariant directly. That copy is what loop-invariant motion
   leaves behind when `u = (half & 1) * 128;` sits between the `v0` and `u1`
   stores and the other three corners spell the expression out. Writing all
   four corners from one variable loses the copy; writing all four inline
   costs two instructions instead of one.

Neither fact changes behaviour; both are required for byte parity.

## Build and isolation

A full `make build` completed all **472 tasks with zero failures**, then the
unchanged retail checksum gate failed for `slus_006.64`, `field.bin` and
`shop_menu.bin` — the same three failures as the preceding checkpoint.

A control build of the identical tree with the original `INCLUDE_ASM` restored
was compared module by module:

| module | control | with this change | changed |
| --- | --- | --- | --- |
| `slus_006.64` | `3192a514…` | `3192a514…` | no |
| `field.bin` | `ececa463…` | `ececa463…` | no |
| `member_change_menu.bin` | `3b9e2b89…` | `3b9e2b89…` | no |
| `menu.bin` | `9fc9b811…` | `9fc9b811…` | no |
| `shop_menu.bin` | `06700c87…` | `770921de…` | yes |

The control's `shop_menu.bin` hash `06700c874745b2b66c46e4f9ec85ed5b89d372cc359e8d7041768a768829150f`
reproduces the preceding checkpoint's recorded shop hash exactly, and
`member_change_menu.bin` reproduces its recorded exact-retail hash, so the
build environment used here is faithful. `shop_menu.bin` stays 55,296 bytes;
remaining inline assembly bodies in the shop TU drop from 15 to 14.

`slus_006.64` and `field.bin` hash differently from the values recorded in
`../shop-resources-20260906/matching-after.json`, at identical sizes. They are
byte-identical between the control and this change, so the drift predates this
work and comes from other uncommitted edits in the tree. It is flagged, not
addressed here.

## Limits

- Native `pc_port` link is **NOT_RUN**: `pc_port/build_port.sh` needs the
  `xenogears-dev` container, which is not present in this environment. The
  existing `pc_port/build_native/xeno-port` binary predates this change.
- No shop UI was rendered and no runtime behaviour was observed. There is no
  focused native differential regression test for this body yet; that is the
  natural next step, alongside the remaining 14 shop assembly bodies and the
  unresolved shop function ordering.
- No staging, commit or push.

## Pins

- Source: `src/shop_menu/main/misc.c`, SHA-256 `85a9bdb3a2e835417cc2b20877d55060e4ca49fbe6086a89b77d146b7229f78f`
- Build workspace: `/tmp/xeno-shop-uv-build/xenogears-decomp` (a copy named for
  the directory `tools/gears` hard-codes; this checkout is `xenogears-decomp-ai`)
- Candidate search scratch: `/tmp/xeno-shop-uv-pair-20260906`
- Predecessor scratch (13 non-exact candidates): `/tmp/xeno-shop-texture-pair-c-20260906`
