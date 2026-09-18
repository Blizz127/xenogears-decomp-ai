# Matching checksum mismatch inventory

Date: 2026-09-06  
Scope: read-only analysis of the completed private container build in `/tmp/xeno-global-jtbl-audit-20260906/xenogears-decomp`.

The build completed all 472 tasks and linked successfully. The only remaining gate failures are the four artifact checksum mismatches below. The comparison used the generated output bytes against the pinned retail payload; the range mapping used each artifact's linker map and ELF symbols. No shared checkout, source, configuration, checksum, or native runtime was changed.

## Four remaining artifacts

| artifact | generated size / SHA-256 | retail size / SHA-256 | overlap mismatch | first mismatch | last mismatch in overlap |
|---|---:|---:|---:|---:|---:|
| `slus_006.64` | 301632 / `2eb8cb4bdba6aed54e1f0734bfb9c72d702ab5ee830139a58e70b432237cacca` | 303104 / `dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119` | 226025 | `0x8884` | `0x494f9` |
| `field.bin` | 242330 / `ac772e7e9e8feb5dd73856a5bf64980b4730f373ef90303eac021805551283d8` | 260862 / `38a1ce829a6f094c505f67143d6ace2d328418c65425a7383991179467e1fdfc` | 213181 | `0x1b8` | `0x3a6d5` |
| `member_change_menu.bin` | 26592 / `242c5450a3faba50fee01d5924f21a64091e7dd27223de5c9c860925b361f2bc` | 26624 / `3b9e2b890c27ae0de97fe343ac75cd35c05fe7f9605ae387d2166da0be78109c` | 22513 | `0x0` | `0x6149` |
| `shop_menu.bin` | 55300 / `0f22fa9ae02881a6e18f0152034ed0e76aa02e6e2a57ed7fb89c68c88e4537cc` | 55296 / `7890e14bcabddcf85368de10783ecff8daa166dc5ac254e06db5e27959cab4cf` | 47790 | `0x10` | `0xcf38` |

The output-to-retail size deltas are respectively `-0x5c0`, `-0x4864`, `-0x20`, and `+4`. These are layout/segment-size symptoms as well as byte mismatches; a checksum failure cannot be attributed to one function from the hash alone.

## Map-backed candidates

### `slus_006.64`

The first isolated clusters are literal data at VMA `0x80018084` (main-state globals), `0x80018664` (`jtbl_80018664`/`jtbl_800186A4`), `0x800188f4` (four `D_800189*` tables and `jtbl_80018980`), and `0x80018b58` (`jtbl_80018B58`/`jtbl_80018C18`). The large drift begins at raw `0x9ce4` / VMA `0x800194e4` and crosses the main rodata/text boundary. The current map places `start` at `0x800199d0`; the retail map authority expects `0x80019524`, a `0x4ac` pre-text rodata difference. The current extra rodata is attributed by the map to `menu.c.o` (0x30), `rendering.c.o` (0x30), `animation_scripts.c.o` (0x40c), and `system.c.o` (0x40) before `start`. This is the strongest concrete layout lead. It is already covered by the known-red ledger's `MenuExecute nonmatching decompile + jtbl literal-data` entry; it is not a newly proven repair target.

### `field.bin`

The first mismatch cluster is raw `0x1b8..0x293`, VMA `0x8006fca8`, in `misc11.c.o` rodata and the following `D_8006FD1C`, `jtbl_8006FD30`, and `D_8006FD44`. The next cluster starts at raw `0x2e8` / VMA `0x8006fdd8` and crosses `jtbl_8006FDD8`, `field_RODATA_END`, and `field_TEXT_START`; the generated segment ends at raw `0x3b298`, while retail continues to `0x3fafe`, leaving `0x4864` bytes absent. The ledger already marks field red at HEAD and names the `jtbl_8006FC88` literal-u32 era, so this is provenance for existing red rather than a new isolated fix.

### `member_change_menu.bin`

The mismatch starts at byte zero. The current map begins text at VMA `0x801c5018` with `MemberChangeMenuSwapCharacters`, then `MemberChangeMenuIsCharacterFlagSet` at `0x801c51e4`. This is not evidence that the C source order is wrong: `MemberChangeMenuSwapCharacters` remains at source line 1982 as a file-scope `INCLUDE_ASM`, and GCC 2.6 emits that `#APP` block before the generated C `.text` in `build/src/member_change_menu/main/misc.c.s`. The input C object consequently places `SwapCharacters` at offset `0x0` with size `0x1cc` (460 bytes), while the retail assembly object places it at offset `0x5b30` with the same size. Retail symbol authority places `MemberChangeMenuIsCharacterFlagSet` at `0x801c5018` and `MemberChangeMenuSwapCharacters` at `0x801cab48`.

After accounting for the hoisted `SwapCharacters` block, the next concrete layout drift is `func_801C59E0`: current C object size `0x190` (400 bytes) versus retail assembly size `0x1b0` (432 bytes), a missing `0x20` from `0x801c59e0..0x801c5b8f`. All later common symbols through `MemberChangeMenuFreeCursors` are shifted by that `0x20`; `MemberChangeMenuMainLoop` remains the same size (`0x394`) but is correspondingly early. A concrete repair candidate is therefore: first model the mid-file `SwapCharacters` assembly as a separately ordered link input (or use the complete retail assembly module while this overlay remains nonmatching), then diagnose `func_801C59E0` against its retail byte range. The ledger marks this artifact red since `310c391` and establishes provenance only; it does not rule out either repair.

### `shop_menu.bin`

The mismatch starts at raw `0x10` and covers the current text/data body. The map puts `ShopMenuLoadResources` at VMA `0x801c5040`; retail symbol authority places it at `0x801c54b4` and `ShopMenuIsCharacterFlagSet` at `0x801c50b0` (current is much later). The output is four bytes longer than retail (`0xd804` versus `0xd800`). The ledger attributes this known red to shared `include/system/menu.h` evolution in `(310c391..48ea933]`; the exact first-red commit remains unbisected.

## Known-red status and next evidence

All four artifacts are already named by `tools/scripts/check_rom_hashes.sh`'s informational known-red ledger. Therefore there is no unclassified checksum mismatch in this build. The next exact-match work should start with the map-backed leads above and require retail-byte/function evidence: the `0x4ac` pre-text rodata ownership in `slus_006.64`, the `misc11.c`/jump-table and missing-tail boundaries in `field.bin`, the member `SwapCharacters` placement plus `func_801C59E0` size, and the shop shared-header/function ordering. This report does not claim any of those leads is a completed repair or a full-function match.

## Input provenance qualification

The private copy's `include/include_asm.h` is stale by one guard relative to the shared checkout: private SHA-256 `b07191b5b52fe5a0cc20e2114349a7ccfb537bba948ba277f375d9de2559a16e`, shared SHA-256 `2d6d960c6002fcda260e3ead35a2707da29c2913f98f349d3285fd28522c40dd`. Shared adds `&& !defined(SKIP_ASM)` to the outer guard. Matching flags define no `SKIP_ASM`. A container preprocessor overlay probe with the live versus private header produced identical preprocessed `system.c` and `animation_scripts.c` outputs, so the stale header is matching-equivalent under this run's flags. The run is therefore not an identical-input claim; no rebuild was required. Detailed probe hashes and the full build result are in `current-make-check-report.md`.

Evidence scripts and machine-readable results: `inventory_mismatches.py`, `inventory.jsonl`, `map_diff_inventory.py`, and `map_diff_inventory.jsonl` in this same directory. The focused member-ordering supplement is `member-change-ordering-followup.md`. Full build log: `container-current-make-check.log` (SHA-256 `b76b6296cce8d070ee4fbf0fbaa01da8a6cbdd0010fc55ef08bf9339b8b11381`).
