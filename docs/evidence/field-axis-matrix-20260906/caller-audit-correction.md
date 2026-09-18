# Field axis/caller audit — 2026-09-06

Scope: read-only trace of the field rotated-actor assertion and the native
post-transform tail. No repository files were changed.

## Conclusion

The prior wording that the native rotated-actor caller remains asserted is
stale. The assertion at `src/field/main/misc2.c:2618` is in the `#else`
matching-build path. Under `XENO_PC_PORT`, `func_80075B44` dispatches to the
real native `FieldRenderActorSpriteTail` at line 2576; it does not hit that
assertion.

The native tail covers the retail `[0x80076118,0x80076300)` post-transform
branch: hidden actors, ordinary draws, the `0x20` split draw, and the
independent `0x40` split draw. Both split bits may run. It uses the actual
`func_8001E2F8`/`func_8001E368` wrappers and re-reads actor flags after the
first draw. The durable tail differential has 43,200 cases per O0/O2/UBSan
regime and rejects five semantic mutants. Its pinned retail slice is 488
bytes, SHA-256 `66b6038c32ce09208817040ebddbacc8ff85835886f5b1a116e903a277a9b342`.

That proves the native tail branch, not the entire `func_80075B44` frame. The
full caller still has unproven transform/fog/filter integration and its
matching-build `#else` branch remains an intentional assert. No assertion was
silenced for native execution, and no natural rotated-actor scene invocation
is established by this audit.

## `func_800759E4` ownership and callers

Field retail `func_800759E4` is a separate exact 292-byte helper at
`0x800759E4..0x80075B08`, slice SHA-256
`b4f76d0b3dd362838d4eb82c2dfefab068628db24f11de43f7fe0504c8b04cf3`.
The current `misc2.c` implementation is exact against `disc/field.bin` and
constructs rows from a supplied axis using two `OuterProduct12` calls and two
`VectorNormal` calls.

A textual and encoded-JAL search across the field source/ASM finds no direct
caller of `func_800759E4`; its label appears only in its own field body and
source/test evidence. Therefore it is not the missing operation behind the
`func_80075B44` rotated-actor assertion. The same address can appear in a
different overlay (for example battle assembly); those bytes are not field
caller authority.

The matrix operation is duplicated inline in native `func_800764B4`, which is
called from `func_800752C8` after `func_80075B44`. Retail `func_800764B4`
performs the same two GTE cross-product operations at `0x80076620` and
`0x80076678`, seeded with world-Z `{0,0,0x1000}` and the actor direction
vector at `pActorData+0x50`. Native `misc2.c` reproduces that operation with
`worldZ`, `up`, `cross`, `normVec1`, and `normVec2` around lines 2700–2735,
then applies the world-to-screen matrix column by column. This is the actual
renamed/duplicated native owner of that billboard-basis operation; it does
not call `func_800759E4`.

`func_80075B44` itself uses a different transform path: retail gathers three
actor-matrix columns at `pActor+0x0C/0x0E/0x10`, applies the world-to-screen
GTE transform, then reaches the post-transform tail. Native uses
`ApplyMatrixSV` for those columns and then `FieldRenderActorSpriteTail`.
Consequently, replacing the remaining matching assert with a call to
`func_800759E4` would be source-incorrect: it would substitute the unrelated
axis helper for the caller's direct column transforms.

## Evidence pins

- Current `src/field/main/misc2.c`: SHA-256
  `c2539494ca3d29971c4af8b2815a5c61ca10452839ef05b338edcec3ed3ecaea`.
- Current actor-tail test: SHA-256
  `1677b170583a6756a83b3c745daeb1c2766e32283b9e650074f19f2d87bef796`.
- Current actor-tail runner: SHA-256
  `3e4b0d4a9b139dee562dedacabebb566e7356e422bc98dc3cb5227e3241b9a00`.
- Retail `disc/field.bin`: SHA-256
  `38a1ce829a6f094c505f67143d6ace2d328418c65425a7383991179467e1fdfc`.
- Full retail `func_80075B44..func_800764B4` span is 2,340 bytes; the
  bounded tail oracle covers only its final 488-byte branch region.
