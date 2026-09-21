# Post-Swap member-menu matching follow-up

Date: 2026-09-06

Scope: private scratch checkout and standalone compiler probes only. The shared
checkout was not modified by this task. Existing baseline logs were preserved.

## Whole-file result after root's Swap install

I copied the current shared `src/member_change_menu/main/misc.c` into the
private matching checkout after saving its previous placeholder source as
`pre-swap-private-misc.c` (old SHA-256
`10150ba1f15bebfb54503cb47ff287ed708f5cee95e1dcb1bf9cbcf11ffe8542`). The
copied source and shared source both hash
`6ade4c3c572a352fc7361f21dc3bebe8070a394d22740da0511f296d2cc0c8f2`.

The fresh isolated container run is recorded in:

- `container-post-swap-make-check.log`, SHA-256
  `c36c8a16fbecce5cbc5fb9a3ac512fdfff056434ef3b9294b8eb714cdb894a66`
- `container-post-swap-make-check.rc`, exit `2`

All 472/472 tasks completed. The result still fails only the four checksum
entries:

| artifact | generated SHA-256 | retail SHA-256 |
|---|---|---|
| `slus_006.64` | `2eb8cb4bdba6aed54e1f0734bfb9c72d702ab5ee830139a58e70b432237cacca` | `dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119` |
| `field.bin` | `ac772e7e9e8feb5dd73856a5bf64980b4730f373ef90303eac021805551283d8` | `38a1ce829a6f094c505f67143d6ace2d328418c65425a7383991179467e1fdfc` |
| `member_change_menu.bin` | `d99362feb687041a9176e88c3591162d51c758338f382bc74702bf2bdab8d61d` | `3b9e2b890c27ae0de97fe343ac75cd35c05fe7f9605ae387d2166da0be78109c` |
| `shop_menu.bin` | `0f22fa9ae02881a6e18f0152034ed0e76aa02e6e2a57ed7fb89c68c88e4537cc` | `7890e14bcabddcf85368de10783ecff8daa166dc5ac254e06db5e27959cab4cf` |

The member-menu artifact changed from the prior `242c5450...` to
`d99362fe...`, but the exact Swap slice is not yet at the retail offset. The
map reports:

```text
func_801C59E0                    0x801C59E0
func_801C5B90                    0x801C5B70
MemberChangeMenuSwapCharacters   0x801CAB28
MemberChangeMenuMainLoop         0x801CACF4
```

The production Swap body is 460 bytes and differs from the standalone exact
candidate only in its internal jump target because it is currently 0x20 bytes
early. Once `func_801C59E0` grows from its current 0x190-byte body to the
retail 0x1B0-byte body, Swap and MainLoop should move to `0x801CAB48` and
`0x801CAD14`; this is a placement consequence, not evidence that the Swap C
body itself is wrong.

## 801C59E0 candidate and retail authority

Retail `func_801C59E0` is `0x1B0` bytes, from overlay offset `0x9E0` through
`0xB8F`, followed by `func_801C5B90` at `0xB90`. The standalone
`candidate-array.c` probe compiles to 432 bytes and matches the retail body in
all but four 32-bit words. Its result is:

```text
candidate-array.bin SHA-256 1f2e066387ef48093f08b7a50e372a68885bc5aa80a1ec9e24167f0ed8f8ede2
retail.bin          SHA-256 c88fecbc1a9c3f3eaf902986cde424f2ab5e807ec84b99dbbbc07ac7dc385c05
```

The four differences are exactly the loop-pointer scheduling pair:

```text
offset 0x03c: candidate 2190a000, retail 2180c002
offset 0x040: candidate 2180c002, retail 2190a000
offset 0x150: candidate 02005226, retail 00011026
offset 0x15c: candidate 00011026, retail 02005226
```

These decode as the preheader copies of `base`/`strIndices` and the tail
updates of the item/index pointers. The candidate is therefore a useful
432-byte scheduling baseline, but it is not an exact retail function yet.

The retail loop sequence is authoritative:

```text
801C5A1C  s0 = base; s2 = strIndices
...
801C5B30  item pointer += 0x100
801C5B3C  index pointer += 2
```

I compiled source-only variants in the same GCC 2.6.0/MASPSX container for
local declarations, explicit pointer initialization, `for`/`while`/`do`
forms, update-clause order, and `register` qualifiers. None produced the
retail ordering while preserving the other 428 words. No binary patch, inline
assembly, or compiler-flag change was used. The closest source remains
`candidate-array.c`; the remaining four words are an unresolved GCC 2.6
induction-variable scheduling issue.

The raw retail function bytes remain pinned by
`/tmp/xeno-member-labels-c-20260906/retail.bin` and the local disc module
`disc/member_change_menu.bin[0x9E0:0xB90]`. The standalone candidate source is
`candidate-array.c`, SHA-256
`10341439119e3d6c77f66cf1feb56b3d76bd2f5f17b344d27038709902806b66`.

## Native data-path caution

The raw retail work buffer is `g_Menu + 0x558`, which corresponds to the first
`MenuString`'s `pVramBuffer` slot at `g_Menu->unk4E0[0].pVramBuffer` in the
current typed menu model. The native probe owner reports that nested C calls
can evaluate the work-buffer load before `GetStringEntry`, unlike retail's
instruction order. Any native-facing production source should use an explicit
entry temporary for each of the two `GetStringEntry` calls, while the exact
MIPS candidate must be recompiled and compared after that source change.

## Recheck command

After resolving the four scheduling words and installing the 0x1B0-byte body,
rerun the same isolated command against the private checkout:

```sh
podman run --rm --userns=keep-id --security-opt label=disable \
  -v /tmp/xeno-global-jtbl-audit-20260906/xenogears-decomp:/tmp/xeno-global-jtbl-audit-20260906/xenogears-decomp:rw \
  -v /var/home/blizz/Projects/xenogears-decomp-ai:/var/home/blizz/Projects/xenogears-decomp-ai:ro \
  -w /tmp/xeno-global-jtbl-audit-20260906/xenogears-decomp \
  localhost/xenogears-dev-toolchain:current \
  bash -lc 'source /.venv/bin/activate; make check'
```

The current run proves that Swap installation removes the old placeholder
without introducing link failures; it does not yet establish a matching
member-menu overlay because the preceding 0x20-byte label function remains
short and the four checksum reds remain.
