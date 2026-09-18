# shop_menu.bin is byte-exact: the overlay is 9 translation units, not 1 (2026-09-10)

Worktree: `/var/home/blizz/Projects/xenogears-decomp-ai`, branch
`experiment/worldmap-open-gates-20260823`, HEAD `3a3e7aac`. No commit, stage or
push.

## Outcome

```
rom-check: clean rebuild (rm -rf build linker; make build) ...
PASS  build/out/member_change_menu.bin     3b9e2b890c27ae0d (26624 bytes)
PASS  build/out/shop_menu.bin              7890e14bcabddcf8 (55296 bytes)
PASS  build/out/battle.bin                 1830b4ef1fe37129 (343936 bytes)
FAIL  build/out/slus_006.64                built f7c1f169… vs pin dc0b2dd7…   (unchanged known-red)
FAIL  build/out/field.bin                  built c012e0d8… vs pin 38a1ce82…   (unchanged known-red)
```

`build/out/shop_menu.bin` == `disc/shop_menu.bin` (`cmp` identical; SHA-256
`7890e14bcabddcf85368de10783ecff8daa166dc5ac254e06db5e27959cab4cf`). This
verifies the shop overlay's 104 decompiled C functions byte-for-byte against
retail, not just the file hash.

## Root cause: one TU was carrying both asm and C

`config/shop_menu.yaml` modelled the overlay's whole code region as one `c`
subsegment (`- [0x40, c, main/misc]`) over a source file that interleaves 14
unmatched functions (`INCLUDE_ASM`) with 104 decompiled C functions in retail
address order. `INCLUDE_ASM` expands to a file-scope `__asm__` block, and
cc1 emits **every** file-scope asm block **before every** compiled body.
(Reproduced in isolation with `tools/gcc-2.6.0-psx/cc1`: a file with
`alpha()`, a top-level `__asm__`, then `beta()` emits the asm first.)

So the object was laid out as `[all 14 unmatched functions][all 104 C
functions]`. Measured before the fix:

- every one of the 118 functions already had its exact retail size;
- 111 of them were at a **uniform +15548 bytes** (0x3CBC) from their retail
  addresses;
- and the only byte differences were cross-references (`j`/`jal` targets) to
  those misplaced neighbours.

Retail interleaves the two kinds in 9 C runs and 8 asm runs, so the overlay is
modelled as one subsegment per **retail run pair** (an unmatched region plus the
C run that follows it) - exactly the layout cc1 produces inside one TU:

```yaml
      - [0x40,  c, main/misc]     # C run only (first)
      - [0xCBC, c, main/misc2]    # func_801C5CBC + the C run after it
      - [0x6CF0, c, main/misc3]   # func_801CBCF0 + ...
      - [0x7278, c, main/misc4]
      - [0x7720, c, main/misc5]
      - [0x7FF4, c, main/misc6]
      - [0x8D14, c, main/misc7]
      - [0x991C, c, main/misc8]
      - [0xAF58, c, main/misc9]
```

The C bodies are unchanged; they are redistributed over
`src/shop_menu/main/misc{,2..9}.c`, each with the same declaration preamble and
its own `INCLUDE_ASM` paths (`asm/shop_menu/nonmatchings/main/miscN`, which is
where splat generates them).

## Two follow-on details that were load-bearing

**Compiler preset.** Preset keys in `gears.toml` are substring matches, so
`shop_menu/main/misc.` did not match `misc2.c`..`misc9.c`: the new parts fell
back to the Default `gcc-2.7.2-psx` preset instead of the shop overlay's
`gcc-2.6.0-psx`. Symptom: 15 functions changed size (8 short, 7 long, net -40
bytes) although their source was byte-identical to before. The preset now lists
`shop_menu/main/misc{,2..9}.`.

**One prototype.** `ShopMenuBuyMenu` calls `func_801CCE1C`, whose `u8` second
parameter used to be visible because its *definition* (in misc5.c's region) came
earlier in the one big TU. After the split the call no longer sees it and GCC
passed the argument unmasked (`move a1,s0`) where retail masks
(`andi a1,s0,0xFF`) - a 4-byte file difference. `func_801CCE1C(void*, u8)` is
now declared in the shared preamble.

**One migrated jump table.** `ShopMenuSellMenu`'s switch table lives in the
overlay's `.rodata` block at 0x801C5028, not next to its code, and splat emits
it as a rodata blob that is not linked on its own. It is included from misc9.c
with `INCLUDE_RODATA("asm/shop_menu/nonmatchings/main/misc", jtbl_801C5028)`
(its entries point at labels inside the ShopMenuSellMenu body in that same TU).

An earlier attempt modelled the unmatched regions as `asm` subsegments instead;
that fails here because splat then stops migrating that table into the owning
function, leaving `jtbl_801C5028` undefined.

## Verification

- `make rom-check` (repo gate, from clean): shop_menu **PASS** as above;
  slus/field hashes unchanged (`f7c1f169`, `c012e0d8`), member_change unchanged
  (`3b9e2b89`), battle unchanged (`1830b4ef`).
- Per-function audit after the fix: 118/118 functions at their retail address
  and retail size; 0 byte differences; file `cmp` identical.
- `./pc_port/build_port.sh`: `LINK OK`, 69 function stubs / 547 data symbols -
  identical to before this change (the port does not compile this overlay's C).
- Logs: `scratchpad/shop-split-20260910/build{1..6}.log`, `port-build.log`;
  pre-split source kept at `scratchpad/shop-split-20260910/misc.c.orig`.
- NOT_RUN: any runtime/visual observation of the shop menu.

## Reusable lesson for the remaining red overlays

Any overlay whose TU mixes `INCLUDE_ASM` with compiled C bodies cannot be
byte-exact: the asm blocks are hoisted to the front of the object. The fix is to
model the overlay as one TU per retail run pair (C run, or unmatched run + the C
run after it), and to check that every new source file still matches its retail
compiler preset. `field.bin` and `slus_006.64` are the remaining red overlays
and both mix asm and C in single TUs; `field.bin` additionally has genuine
size deficits (~18 KB short as of 2026-09-06), so ordering alone will not close
it.

