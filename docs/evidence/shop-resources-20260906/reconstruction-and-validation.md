# ShopMenuLoadResources: exact C and native boundary differential

Frozen candidate accepted for parent integration; no shared source or tests were written by this lane. Parent has copied the frozen candidate and two durable test files. **Function C byte match and native boundary differential PASS; whole shop-module exact match and natural shop gameplay are separate gates.**

## Retail authority

- Retail module `disc/shop_menu.bin`: SHA256 `7890e14bcabddcf85368de10783ecff8daa166dc5ac254e06db5e27959cab4cf`.
- Function `ShopMenuLoadResources`: `801C54B4..801C58F4`, raw `0x4B4..0x8F4`, 1088 bytes /272 instructions, SHA256 `f791af75ac8be21876feb8f706399780e3010ab17b780e8337fd328dcbfb2d51`.
- Associated data `D_801C5000`: raw0..16, SHA256 `47773c1d0ec894732fa108868d6ff3db76a6bcb74ce104902a402c9fc3f2cbc8`. The body copies13 bytes (`BISLPS-00800` and its terminator); candidate preserves the entire16-byte retail constant, including trailing `00 03 00`.
- Disassembly authority: `/tmp/xeno-global-jtbl-audit-20260906/xenogears-decomp/asm/shop_menu/nonmatchings/main/misc/ShopMenuLoadResources.s`. Numeric helper/global addresses independently decoded from retail JAL instructions and global load/store sequences are recorded in `retail-pins.json` and `slice.ld`.

## Reconstructed behavior and native types

The function snapshots `D_8005945C`, resolves its archive pointer table, and uses packed four-byte entries1,2,3,4,5,7. Entry6 is not consumed. It decompresses the icon TIM, reads its TIM record into current menu work, copies the retail save name, writes header bytes53/43/11/01, clears0x5C bytes, copies0x20 CLUT bytes and0x80 pixel bytes, then frees the decompression result. Entry2 is decompressed, sent to `func_8002DD20`, and freed. Entry3/4 results go into freshly reloaded menu atlas fields.

Three eight-argument `func_80026338` calls use atlas indices14B/14C/14D and six adjacent32-bit output cells per portrait. The middle portrait texture-X cell adds12 with retail32-bit wrap. Entry5 supplies the portrait TIM pack, stride0xB20. Three current party slots are reloaded through the current menu/manager; only IDFF skips upload. For each selected slot, it opens/reads TIM, writes low-halfword CLUT x/y and texture x/y into the borrowed RECTs, then submits CLUT and pixel uploads. It calls DrawSync(0) before freeing the portrait pack.

With debug enabled, it switches archive to(10,2), allocates the decoded aligned size of file5, stores the SEDS pointer, reads file5 with flags80, synchronizes, restores archive(10,0), and adds the sound entry. It then reloads the shared SEDS pointer into the current menu. Finally it decompresses captured table entry7 into the current shop-entries field and frees the originally captured pointer table.

Native access uses actual `SystemMenu`, `MenuUnk2`, `MenuManager`, `TIM_IMAGE` and RECT fields. No PSX byte offsets are used to address native objects. Packed archive entries remain `u32`, converted through `uintptr_t`; widening the table to host pointers would consume pairs of retail entries. Pointer-returning decompress/heap helpers have explicit declarations. HeapFree, SoundAddSedsEntry, ArchiveDecodeAlignedSize and ArchiveCdDataSync declarations follow their source owners' return/scalar types. `ArchiveReadFileToBuffer` is declared with a pointer second parameter; its pre-existing implementation mismatch is discussed below.

Local output storage is a union with32-bit write cells and16-bit view. Byte offsets are taken from the **whole union object**, preserving valid object-relative access. The compiler thus emits the observed LHU operations rather than full-word LW. Two synchronized indices express retail scheduling: the party-slot counter advances in the FF-test delay slot, while the coordinate index advances at the loop tail. Neither volatile accesses nor inline assembly are used. The `(u32)cell +12` expression prevents signed-overflow UB for adversarial helper outputs while emitting the same retail ADDIU.

## Exact compiler checks

Authoritative compiler was verified in private `build.ninja:861..864`: shop uses **GCC2.6.0 PSX**, not the field lane's2.7.2. Initial experiments used2.7.2; those are retained as historical evidence and are not the authoritative acceptance gate.

Unchanged candidate passes both standalone and **actual full shop translation-unit** compilation under GCC2.6.0, using current actual headers and existing shop settings `-O2 -G8 -mips1 -mcpu=3000 -funsigned-char -fpeephole -ffunction-cse -fpcc-struct-return -fcommon -fverbose-asm -msoft-float -mgas -fgnu-linker`, repository MASPSX and MIPS assembler. `typed-final26/comparison.json` and `full-comparison.json` show all272 instruction words exact; linked symbol is verified801C54B4. Full-TU emitted body SHA equals the retail slice.

`full-data-comparison.json` independently compares the actual full object constant. **Observed D_801C5000 object rodata offset is0**, correctly matching retail placement. An earlier progress message inferred it would follow two hoisted tables; actual GCC2.6 object inspection disproved that inference. The full object rodata is64 bytes, with the two remaining tables after the16-byte constant.

Isolated exact-function linking deliberately places the function at its actual retail address and pins its dependencies to exact retail targets. Unrelated externs receive valid KSEG placeholders so other full-TU relocations can link. The D_801C5000 relocation is retail-pinned, and actual data bytes/offset are checked separately. Unrelated full-TU text/rodata virtual positions in this isolated link are not execution authority. There are no patched output instructions or handwritten replacement assembly. The remaining15 INCLUDE_ASM bodies still affect whole-module text ordering; no full-module match is claimed here.

## Native differential

Frozen durable files ready/copied to `pc_port/tests/`:

- `shop_menu_resources_retail_test.c`, SHA256 `132fc37af23684da303424b302fe3ed1d62bd12e2f9e3ef70033187ec7b689be`.
- `run_shop_menu_resources_retail_test.sh`, SHA256 `4db281032b676024913cedb321b1082aa96d037d5ce9b58e012e721c744638d5`.

The runner defaults to actual production `src/shop_menu/main/misc.c`; no `/tmp` input is required after copying. Source/output/test/root overrides permit independent source runs. It verifies complete retail module and function slice hashes, checks all16 data bytes, pins source/test/runner/headers/interpreter, and rechecks pins after execution. It compiles the **full actual candidate TU** and production `battle_mips_adapter`; section GC retains the tested function. No copied C body is used as an oracle.

Final retained scratch run: `durable-output/`. **O0, O2 and all-Clang UBSan each PASS128 cases,11944 checks, every one of272 retail instruction slots.** One generated test-only forward declaration for unrelated later `func_801D1F10` avoids Clang's implicit-declaration conflict. No production body or tested helper declaration is rewritten. Initial diagnostic is retained in `native-clang-build.log`.

Cases cover eight party-ID layouts (including allFF, holes, repeats, zero,127/128,253/254), debug off/on, two initial byte patterns including high-bit/wrapping atlas outputs, and four pointer-rebinding modes. Menu, work, manager, TIM/resource buffers and allocation returns are **actually mmap'd above4GiB**. The packed resource table itself remains low four-byte storage. Tests compare ordered helper identities and all used scalar/pointer arguments, all six atlas output-pointer offsets, GPU RECT/payload snapshots, both complete0x4A0 work tails, all stored destination pointers, the captured/rebound resource globals and all resource words, and the entire0xB20×255 portrait buffer. Whole native menu/work snapshots guard unrelated fields, padding and prefixes. Rebound menus have valid initial TIM/atlas state, so the adversarial cases do not rely on invalid pointers.

Seven compiled O2 semantic controls are accepted as rejected **only** for exact exit1 plus `SHOP RESOURCES FAIL`: short filename copy, host-width packed pointer table, wrong portrait stride, missing12-pixel adjustment, skipped valid character, reloading resource global instead of captured table, and caching the menu pointer across helper boundaries. `controls.json` and each failure log are retained.

Limits: archive resolution/decompression, heap ownership, atlas unpacking, TIM parsing, CD transfer, sound and GPU calls are explicit boundary spies. This fixture proves caller behavior and output use; it does not establish real decompression, atlas math, physical VRAM upload, audio, invalid-pointer behavior, native lifecycle, or visible shop gameplay. In particular high-address spy allocations must not be mistaken for proof that the real packed archive resolver supports high-address archive tables: its format intentionally contains32-bit pointers.

## Archive pointer-width dependency

At initial audit, actual `src/slus_006.64/system/libarchive.c` declared `ArchiveReadFileToBuffer(s32 index, s32 pBuffer, u32 arg2, u32 flags)`. That second parameter loses high native pointer bits, despite pointer declarations at existing callers. This is distinct from the intentionally packed resource-table format.

The loader was previously a generated native stub, so installing this C newly activates the real shop debug resource-read path and would expose that existing callee defect for a high-address HeapAlloc result. Normal low native heap buffers do not demonstrate the defect's absence. Other menu callers already had the same dependency. The loader fixture intentionally verifies intact high-pointer forwarding to a spy; it does not execute or silently repair the defective callee.

Parent separately owns and reports installing the one-line callee `void*` repair, independently proving full GCC2.6 archive-TU emitted assembly unchanged and native7-case O0/O2/UBSan tests with the old-type control rejected. Parent native build prefix1b82f340 includes that dependency correction. This is parent-reported integration evidence, not this fixture's boundary proof.

## Frozen integration guard

- Source-before/current lookup-repaired shop source: `235990cb1522e1d6ba0c1a193a4319dde94b4bb305af07f668b06a3545964631`.
- Full candidate: `f0fd588c74c7dd9e1473151ae19dda8b2333dacab197bc30428b42a6473cf19a`.
- `candidate.patch`: `f479bad79a6fa6fd3c42c69ee04451b91fac32b2e8b8553cfb6986a71fa9e27a`.

`declarations.inc` + source-before with only the ShopMenuLoadResources INCLUDE_ASM replaced by `data.inc` + `candidate-body.inc` mechanically reconstructs the frozen full candidate. Parent's separate exact68-byte lookup repair is preserved. `final-pins.json` pins every component, compiler proof and test artifact. Parent has now integrated that reconstruction and copied the exact test files; its independent installed run is `/tmp/xeno-shop-menu-resources.ZCWjcXlx` (O0/O2 reported green when this report was frozen). Native/full-global integration and natural-runtime acceptance remain parent-owned. No shared source/test/config/runtime writes were made by this lane.
