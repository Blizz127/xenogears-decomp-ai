# W34C2 — PS-X static data load path

Branch `experiment/worldmap-open-gates-20260823`, substrate HEAD `bb88b45a`.
Authorized scope: copy main-exe rodata + sdata into `g_PsxRam` at init, paired
with the `D_80050100` split-brain fix in `wm_80073B04`. Nothing else.

## Defect

`disc/SLUS_006.64` is a PS-X EXE (`t_addr=0x80010000`, `t_size=0x49800`).
The PS1 loader copies the image (file offset `0x800`) to `0x80010000`. The
port runs natively-translated C and never loaded that image, so every port
site that rebases a main-exe data address through `PSX_ADDR` read zeros.
The consumer sweep (`scratchpad/w34c1_static_data_pregate/`) found the
guest-RAM readers: the `wm_80099708` sine table at `0x800523F0` (affected),
`wm_80073B04`'s OT shift at `0x80050100` (host/guest split, below),
`0x80050104` (writer only), and the OT-adapter null sentinel `0x8005698C`
(substituted, unaffected). Everything else in that range was already
migrated to host copies (`data_font.c`, `data_published_logo.c`,
`game_overrides.c`), or is a `.text` address in a comment/trace label.

## Change

- `pc_port/src/psx_memory.c/.h`: `PsxMemory_LoadStaticData()` — locate
  `SLUS_006.64` (`XENO_SLUS`, else `../../disc`, `disc`, `../disc` like the
  disc image), validate `PS-X EXE` + `t_addr`, copy
  rodata `[0x80010000,0x80019524)` and sdata `[0x8004EA90,0x800576E4)`
  from file offset `0x800 + (vaddr - 0x80010000)`. Ranges come from
  `config/slus_006.64.yaml` subsegments.
  Not copied: `.text [0x80019524,0x8004EA90)` (no `PSX_ADDR` consumer reads
  instruction bytes as data) and the `._49AC0` island (ELF places it at
  `[0x80097704,0x80097C44)`, inside the world overlay's text range).
  Retail exe bytes at file `0x49AC0..0x4A000` are all zero, so the ELF's
  island content is not from the retail image anyway.
- `pc_port/src/port_main.c`: call it right after `PsxMemory_Init()`.
- `pc_port/src/world_map_helper_73b04.c/.h`: `wm_73b04_ot_shift()` returns
  the host global `D_80050100` (`game_overrides.c`, `= 2`) instead of reading
  guest `0x80050100`. Retail writes 2 there at `0x800847D8` inside the
  unported overlay function `[0x80084580,0x80084818)`; the natively
  translated prim walkers already use the host global.
- `pc_port/tests/w34b_r4world_73b04_prod_test.c`: per-case shift now drives
  the host global too (its guest store is kept).

### 2026-09-20 correction: the sdata end was wrong by 0x1BD8

The ranges above are superseded. `config/slus_006.64.yaml` places `.sdata` at
`[0x8004EA90,0x800592BC)`, a 4-byte `.data` word at `[0x800592BC,0x800592C0)`,
and `.sbss` at `0x800592C0` — not `0x800576E4`. The retail image is non-zero
right through `0x800592BB` (e.g. `0x800576E4 = c7 01 c7 01 c8 01 …`,
`0x80059000 = ef 06 d2 03 …`), so the old constant silently dropped 7128 bytes
of real initialized data. `PSX_EXE_SDATA_END` and `PSX_EXE_SBSS_START` are now
`0x800592C0`; the loader copies `[0x8004EA90,0x800592C0)`.

This also makes the old certificate line `.sbss [0x800576E4,0x80059800)` zero
circular — it asserted that guest RAM stayed zero where the loader had simply
never written. The range check is now `.sbss [0x800592C0,0x80059800)` zero, and
`w34c2_static_data_prod_test.c` pre-fills `.sbss` with an `0xA5` canary and
asserts it is untouched, so mutant M4 (sdata extended into `.sbss`) is still
detected. Real `.sdata` bytes for 29 of the previously zero-stubbed symbols are
now defined in `pc_port/src/data_slus_sdata.c` with a per-symbol retail memcmp
certificate (`pc_port/tests/run_data_slus_sdata_retail_test.sh`).

Correction to the pre-gate note: retail's *static* `.sdata` word at
`0x80050100` in `disc/SLUS_006.64` is already `2`; the `0x342e342b` value
quoted in the pre-gate came from `build/out/slus_006.64.elf`, whose `.main`
bytes do not match the retail image at that offset (34,147 differing bytes
across rodata+sdata under the `0x20000` file mapping). The loader therefore
does not regress the shift to 11; the split-brain fix stands on authority
grounds (one source for walkers and helper), not on that hazard.
`0x8005698C` in the retail image is `04FFFFFF 00000000 ...` (the null
sentinel packet), not zeros as the pre-gate said; the adapter's end-tag
substitution never links to it, so nothing changes.

## Certificate

`pc_port/tests/run_w34c2_static_data_prod_test.sh` (O0 / O2 / UBSan
nonrecovering, `-Wall -Wextra -Wconversion -Wsign-conversion -Werror`):

- rodata and sdata byte-identical to the file image; last rodata page present
- sine table resident via the port's own arithmetic (`base + index*4`):
  `0x000 = 0000 1000`, `0x400 = 1000 0000`, `0x600 = 0B50 F4B0`,
  `0x800 = 0000 F000`
- `.text` guest bytes zero; `.sbss [0x800576E4,0x80059800)` zero; bytes above
  the image end zero; island range canary (`0xA5`) untouched
- `wm_73b04_ot_shift() == 2` and follows the host global; guest twin `== 2`
- malformed image rejected with RAM untouched

Mutants, each detected by a named assertion:
M1 `.text` copied → `text_zero`; M2 island copied → `island_untouched`;
M3 73b04 on the `PSX_ADDR` read → `ot_shift_host`; M4 sdata extended into
`.sbss` → `sbss_zero` (the PS1 loader's placement of the island bytes at
`0x800592C0` would surface there); M5 rodata short by one page →
`rodata_bytes`. Legacy `run_w34b_r4world_73b04.sh` still PASS (M1–M23).

## Re-gate (read-only, gdb; no production instrumentation)

Substrate: forced route env stack + `XENO_TEST_INPUT=0:0x2000,600:0x4000,
916:0x2000,976:0`, frame limit 120, harness `w34c2_regate.gdb` (banked).
Cell probe enabled only during frame 60, at `wm_80099708` entry and the
vertex store line, for the patch-27 source `tile_data == 0x3CC`.

- Boot: `main-exe static data loaded from ../../disc/SLUS_006.64`.
- Frame 60: 100 `wm_80099708` calls; the probe saw 20 with
  `tile_data == 0x3CC` (81 cells each, 1,620 rows).
- Rung 5f term split: branch taken in 5/81 cells per call (unchanged);
  **term A nonzero in 100/100 branch-taken cells** (Rung 5f: 0/81).
  Sample: `index=5 packed=0x005ef6e8 sine_x_twice=8192 term_a=-10`,
  `index=41 packed=0x0000ffe8 sine_x_twice=-8192 term_a=9`.
  `sine_x_twice` now takes ±8192 / ±5792 (table values 4096 / 2896 × 2).
- Angle inputs at entry: `0x8009C618 = 0x400`, `0x8009C5BC = 0xEC0`.
- Captures: frame-60 SHA-256 `abf1a9636ab4e37138b9e94347f3fbd43b4e6259eaeeebc7d5abc5c4650f88d1`
  (Rung 5 baseline `38c57657…`), frame-120
  `9413ab2b329aa47d8ab138ddab37c90285377980451d39f197ebec946fd7fbcb`
  (baseline `850ce0f1…`). Frame 60 still shows the dense malformed terrain.
  The delta is consistent with the sine term now contributing; no claim is
  made about which pixels changed.
- No fault, signal, or unfamiliar stop in the first post-loader run.

Limitations: the term-split values are producer output computed from the
patch-27 *source* at `g_PsxRam+0x3CC`, which is not authored tile data (see
below); nonzero term A proves the table is resident and consumed, not that
the heights are correct.

## Watch item (recorded regardless of outcome)

Populating `.sdata` makes pointers inside those tables non-null for the
first time. Guest-side code that short-circuited on zero may now dereference
them; no static sweep can enumerate this class. If a later run faults in an
unfamiliar place, this is the first hypothesis to test. This run did not.

## Fault #2 — cause named, not this defect

Patch 27's tile source `0x000003CC` is a NULL heap slot plus the `+0x3CC`
quadrant offset: `wm_8009932C` reads `tile_base = lw(0x8009C184 +
tile_index*4)` (slots owned by the `97dc0`/`98cc0` HeapAlloc path) and
dispatches `wm_80099708` at `+0/+0x144/+0x288/+0x3CC`. With the slot NULL
it reads `g_PsxRam+0x3CC`, the scratchpad alias, which is why Rung 5f's
"packed" words were camera-matrix words. The terrain heights were never
authored data. Supersedes the Rung 5d/5e/5f height-outlier line; queued as
W34C3 (why the slot is unpopulated).

Artifacts: `scratchpad/w34c2_static_data.*/` (regate.log, first-call cells,
captures, certificate.log, build.log, harness).
