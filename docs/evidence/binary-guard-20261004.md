# Binary guard: 19 retail runs to 0 (task 6be4993b0e61), 2026-10-04

`retail_binary_guard` blocked every private build: the linked `xeno-port` carried
19 retail byte runs (25,068 bytes). The port binary now carries none. Every
table it needs is zero storage, filled before `main()` from the user's own disc
files by the `retail_data.h` loader (SHA-256 checked, no bytes shipped), or read
where retail reads it.

## Where the runs came from and what changed

| Group | Runs / bytes | Source | Change |
|---|---|---|---|
| A | 6 / 18,000 | PsyCross `LIBGTE.C`: `rcossin_tbl`, `ratan_tbl` and `SQRT`, all bit-identical to SLUS 0x800523F0 / 0x80057030 / 0x80056A00; the SDK PushMatrix/PopMatrix text | `psycross_gte_tables_from_disc.patch` loads rcossin/ratan from SLUS. SQRT is not compiled in (SquareRoot0 already reads guest memory). `psycross_gte_stack_messages.patch` is an add-only zero-context patch that compiles the SDK text out and prints the port's own message, so no patch file quotes it |
| B | 3 / 6,420 | `sound.c` port arm: `D_80050B78 D_80050BF0 D_80050A94 D_800509B0 D_80050824` | Loaded from SLUS in the TU |
| C | 1 / 144 | `animation_scripts.c` port arm: `D_8004FC40` | Loaded from SLUS in the TU |
| D | 1 / 52 | `memory.c`: heap-dump strings `D_80059248..D_800592B0` | Port arm loads them from SLUS; the matching arm is unchanged |
| E1 | 6 / 356 | `src/movie/main.c` developer menu: `FontPrintf` literals | `MV_STR(vaddr, literal)` reads movie.bin's own .rodata from guest RAM, where main_loop decompresses the overlay before MovieMain runs (as on retail). The literal is not compiled in, and no new data file is required |
| E2 | 2 / 96 | `world_map_scheduler.c`: callback list in retail order | Table A's 15 records (0x80099E8C) and Table B's selector-0 record (0x80099F0C) load from world_map.bin. The 74 remaining literals are function-entry addresses that the mode initializers build from lui/addiu immediates: address facts, not data in any retail file |

The B, C and D tables load inside the TU that owns them, not in a `pc_port`
data TU, because about 40 unit tests build these TUs standalone and must keep
linking. For the same reason, LIBGTE.C loads its own tables (29 tests compile
it standalone). `retail_data.h` now takes the libc `readlink` prototype when
included from C++.

Guard data: exactly one row was deleted from
`tools/analysis/retail_data_guard_allow.txt` (`sound.c 260`). The guard reported
it stale ("lower or delete") once the sound.c literals were gone; the allowlist
must only shrink. The coordinator approved this. No other guard file changed.

## Verification (blizzpc runner, xenogears-dev:clang)

- **Step 0** (job …-2235474, c693a844): confirmed 19 runs / 25,068 B and mapped each to its symbol.
- **Step 1+2 gate** (job `20261004-080239-…-3777331`, 3486cab3), overall_rc=0:
  - rom-check, build and `make port-guards` PASS. retail_data_guard: "OK: 2882 file(s), no retail byte runs". psyq_port_guard PASS. retail_binary_guard: "OK: no retail byte runs in pc_port/build_native/xeno-port (6,726,584 bytes scanned)".
  - `run_retail_tables_loaded_test.sh` PASS: 26 tables, each `.bss` of the retail size, and each loader dump equals the disc window. A flipped-byte negative control is included.
  - `run_movie_overlay_strings_test.sh` PASS:
    - all 54 MV_STR addresses hold their literals in disc/movie.bin;
    - no bare literal remains, and the strings are absent from the binary;
    - the guest-RAM overlay's .rodata at MovieMain entry equals disc/movie.bin. The whole 29,779-byte overlay differs only at file offsets 0x744F–0x7451, past .rodata, where MovieMain's state block starts.
  - Unit tests rc=0: data_slus_sdata, battle_trig_bridge, rotation_zyx, battle_gte, sprite_dispatch_c1, sprite_opcode_a9, sound_handle_error, w34b68_second_scheduler, w34b22_a2_94060.
- **Pre-existing failures, not caused here.** Job …-3556289 ran them on a c693a844 export and they fail identically there:
  - `run_battle_target_direction_retail_test`: main35.c "D_800D3014 / D_800C3EAC undeclared";
  - `run_card_upload_heap_extent_test`: gcc-14 -Wint-conversion in the lifted HeapAlloc.
  - These are tracked separately.
- **Step 3 identity** (job `20261004-081045-…-4161884`, 3486cab3, binary 09a236cb…): the field14 walk slice under the same deterministic test clocks (a jitter-free test-determinism clock, not a retail-timing claim). Reference ×2 and interpreter-disabled ×2, overall_rc=0.
  - All 36 checkpoint frames and the 4 F11 saves are byte-identical to the accepted re-run (job 4085031); save 63929945… is unchanged.
  - Both summaries end "FIELD14 SLICE PASS", with zero interpreted instructions, no refusals and no stubs. The summary text differs from the accepted one only by the c4cc953a wording and the added player-position check line; the spu JSON line is identical.
  - 6/6 SPU pairs are identical on every vblank. With the absolute vblank index column removed, all four spu.log files are identical (51382ef7…). ref1's log starts at index 21 instead of 23 because the split clock is host-paced while loading.
  - The loaded-tables test also passes on this binary.

Receipts: each job's `out/` stays in its runner work directory on blizzpc. The
devbox copies of the small receipt files are in `/tmp/xeno-e52f-receipts/`, which
the hourly sweep may move.

Commits: d7844ee2, e26ea942, 644f0976, 0346dc29, dacc2dc7, 23cbee73, 3486cab3
(local only, not pushed).

Open: the two pre-existing test failures above. The PushMatrix/PopMatrix
console text is the port's own wording (console only).
