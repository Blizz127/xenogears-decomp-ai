# SwapCharacters baseline and retail reconstruction

Date: 2026-09-06

Scope: read-only verification for `MemberChangeMenuSwapCharacters`; no shared
checkout source, configuration, build, or test files were changed. The only
new artifact is this report. The standalone candidate supplied by root was
verified against the local retail module bytes.

## Matching baseline

The prior isolated container run is still the valid baseline. It ran in
`/tmp/xeno-global-jtbl-audit-20260906/xenogears-decomp`, completed all 472/472
matching tasks, and exited 2 only at the checksum gate. Evidence is
`container-current-make-check.log` (SHA-256
`b76b6296cce8d070ee4fbf0fbaa01da8a6cbdd0010fc55ef08bf9339b8b11381`) and
`current-make-check-report.md`. It had no compiler errors, undefined
references, `.L800...` jump-table failures, or discarded-`.sdata` diagnostics.

The four remaining checksum reds from that run are:

| artifact | generated SHA-256 | retail SHA-256 |
|---|---|---|
| `slus_006.64` | `2eb8cb4bdba6aed54e1f0734bfb9c72d702ab5ee830139a58e70b432237cacca` | `dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119` |
| `field.bin` | `ac772e7e9e8feb5dd73856a5bf64980b4730f373ef90303eac021805551283d8` | `38a1ce829a6f094c505f67143d6ace2d328418c65425a7383991179467e1fdfc` |
| `member_change_menu.bin` | `242c5450a3faba50fee01d5924f21a64091e7dd27223de5c9c860925b361f2bc` | `3b9e2b890c27ae0de97fe343ac75cd35c05fe7f9605ae387d2166da0be78109c` |
| `shop_menu.bin` | `0f22fa9ae02881a6e18f0152034ed0e76aa02e6e2a57ed7fb89c68c88e4537cc` | `7890e14bcabddcf85368de10783ecff8daa166dc5ac254e06db5e27959cab4cf` |

The live matching input and prior private copy differ in
`include/include_asm.h` only by the shared outer `!defined(SKIP_ASM)` guard.
Live SHA-256 is
`2d6d960c6002fcda260e3ead35a2707da29c2913f98f349d3285fd28522c40dd`; the
private copy is
`b07191b5b52fe5a0cc20e2114349a7ccfb537bba948ba277f375d9de2559a16e`.
The matching flags define no `SKIP_ASM`; the exact container preprocessor
probe produced identical `system.c` and `animation_scripts.c` output for both
headers (recorded in `current-make-check-report.md`). Thus the baseline is
matching-equivalent under its recorded flags, but it is not an identical-input
checkout claim. No second full check was needed for this header difference.

## Retail authority and candidate verification

Retail source assembly is
`asm/member_change_menu/main/misc.s`, with the nonmatching function at
`asm/member_change_menu/nonmatchings/main/misc/MemberChangeMenuSwapCharacters.s`.
The function starts at VMA `0x801CAB48` and ends immediately before
`MemberChangeMenuMainLoop` at `0x801CAD14`: size `0x1CC` (460 bytes), module
offset `0x5B48..0x5D13`. The local disc module is 26624 bytes with SHA-256
`3b9e2b890c27ae0de97fe343ac75cd35c05fe7f9605ae387d2166da0be78109c`.

Root's standalone candidate was compiled with the PSX GCC 2.6.0 path and
MASPSX. Its source SHA-256 is
`285434d5506d1bbf4dd54e94b8b63b80ae813f9d28770da9d8a54e0cdb08d9ca`; the
compiler script is `0060378505d169862b6f606aaaceba46c56be8d9137152b1f454f92756b8dbc0`.
The generated 460-byte candidate is
`a65fe1e11ad3f13b0cd9b732ec90a9a794ba476ef8b09b14ddf0f838a177fbd5`.
An independent byte comparison against the live
`disc/member_change_menu.bin[0x5B48:0x5D14]` returned:

```text
candidate length 460
live slice SHA-256 a65fe1e11ad3f13b0cd9b732ec90a9a794ba476ef8b09b14ddf0f838a177fbd5
candidate == live slice True
live slice == private-oracle slice True
```

The candidate also begins `d8ffbd271800b2af219000001000b0af` and ends
`1000b08f2800bd270800e00300000000`, matching the raw retail range. This is an
exact function-slice result, not a claim that the current production module
now matches; the prior full module checksum remains red until the source is
replaced and the whole overlay is rebuilt.

## Source path and control flow

The current C source is
`src/member_change_menu/main/misc.c` (SHA-256
`10150ba1f15bebfb54503cb47ff287ed708f5cee95e1dcb1bf9cbcf11ffe8542`). It
still has the `INCLUDE_ASM` placeholder at line 1982, and
`MemberChangeMenuMainLoop` calls it with:

```c
MemberChangeMenuSwapCharacters(
    benchedCharWindowActive, curCharIndex, curCharOffset,
    isSelectedCharOnBenchedWindow, selectedCharIndex, selectedCharOffset);
```

Retail chooses `(partyIndex, benchIndex)` as
`(curCharIndex, selectedCharIndex + selectedCharOffset)` when the benched
window is inactive, otherwise
`(selectedCharIndex, curCharIndex + curCharOffset)`. It checks the party byte
at `g_Menu->pManager + 0x30` and the bench byte at `g_Menu + 0x1E14` against
`0xFF`, then calls `MemberChangeMenuIsCharacterFlagSet` for each. The mask is
the halfword at retail `D_8006F94C`, which is `g_GameState + 0x2318` given
`g_GameState = 0x8006D634`; the candidate's direct offset read matches that
retail address without inventing a new alias.

If both checks pass, retail swaps the two bytes, counts non-`0xFF` values in
the three party slots, and returns 1 if the count is nonzero. If all three are
`0xFF`, it swaps the two bytes back and returns 0. The fourth argument
(`isSelectedCharOnBenchedWindow`) is present in the ABI but is not consumed by
this retail body. The retail function's saved-register prologue and branch
delay slots are part of the exact 460-byte match; C rearrangements that change
the split flag tests, initialization order, or typed field access can change
register allocation even when behavior appears equivalent.

## Ordering and verification command

The GCC 2.6 file-scope `INCLUDE_ASM` block is emitted at the beginning of the
C translation unit's `.text`, even though the placeholder appears after
`MemberChangeMenuFreeCursors`. The prior private object therefore put the
placeholder at offset 0 (`SwapCharacters = 0x801C5018` and
`MainLoop = 0x801CACF4`), while retail has `SwapCharacters = 0x801CAB48` and
`MainLoop = 0x801CAD14`. Replacing the placeholder with a C definition at the
existing source location lets the function follow the retail C prefix and
preserves the required following address; moving it to the top to compensate
for the old inline-asm hoist would be the wrong fix.

After root installs the candidate in the private copy, the focused check is:

```sh
podman run --rm --userns=keep-id --security-opt label=disable \
  -v /tmp/xeno-global-jtbl-audit-20260906/xenogears-decomp:/tmp/xeno-global-jtbl-audit-20260906/xenogears-decomp:rw \
  -v /var/home/blizz/Projects/xenogears-decomp-ai:/var/home/blizz/Projects/xenogears-decomp-ai:ro \
  -w /tmp/xeno-global-jtbl-audit-20260906/xenogears-decomp \
  localhost/xenogears-dev-toolchain:current \
  bash -lc 'source /.venv/bin/activate; make clean; ./tools/gears/prebuilt/gears matching; ninja -j$(nproc) build/out/member_change_menu.bin; \
    rg -n "MemberChangeMenu(SwapCharacters|MainLoop)" build/out/member_change_menu.map; \
    python3 - <<"PY"
from pathlib import Path
import hashlib
got = Path("build/out/member_change_menu.bin").read_bytes()[0x5B48:0x5D14]
ref = Path("disc/member_change_menu.bin").read_bytes()[0x5B48:0x5D14]
print(len(got), hashlib.sha256(got).hexdigest(), got == ref)
assert got == ref
PY'
```

This focused command proves the function slice and symbol placement. The
whole-overlay checksum and the four-red ledger still require the normal
isolated `make check` after any production replacement; no live checkout
build was run for this report.
