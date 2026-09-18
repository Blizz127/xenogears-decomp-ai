# Session 2026-09-12: 90%-decompile / 50%-port push — environment + loop notes

Goal: decompile %done (MATCHED+COEX per tools/scripts/decomp_status.py) 78.8% -> 90%;
port: retail-accuracy-1:1 native coverage -> 50% (metric: distinct port-relevant
functions exercised by a PASSING accuracy test; baseline being swept).

## Baseline (decomp_status.py, authoritative)
Universe 3335 (incl. data .s). MATCHED 2491 + COEX 138 = 78.8% done; UNPORTED 706.
True function work items (excl. coex/matched/data): 690 (battle 612 + other 78).
Need +373 done for 90%.

## This sandbox CANNOT run the retail toolchain
- tools/gcc-*/cc1 (32-bit static) die with SIGSYS silently (exit 159) under the
  sandbox seccomp filter. All variants, also under linux32/setarch. No strace.
- podman unusable (runroot/storage on read-only /run; userns mount ops denied).
- No network egress (proxy tunnel timeout). No qemu-i386 anywhere.
=> Byte-exact matching verification is IMPOSSIBLE here. Semantic 1:1 via the
new retail-byte emulator is the strongest available proof; byte-match must be
confirmed later where cc1 runs (CI/user machine).

## New: tools/remu (MIPS-I retail-byte oracle)
tools/remu/{remu.c,remu.h,smoke_test.c,remu_sweep.c,run_diff.sh,tests/}.
- Executes retail bytes parsed from splat .s `/* ROMOFF VRAM BYTES */` comments
  (BYTES are the LE sequence in display order, NOT a word: E0FFBD27 = bytes
  E0 FF BD 27 = word 0x27BDFFE0).
- Correct single-step model: delay slot executes ONLY after a branch; plain
  instructions advance pc+=4. (First version pair-stepped and silently took
  wrong paths - caught by its own differential test. Sweep numbers from before
  the fix are VOID.)
- 2MB RAM mirrors + 1KB scratchpad, touched-tracking (rc=3 on uninit read),
  external jal targets become logged stubs (v0=0). GTE/cop1/HW-regs fault LOUDLY.
- Validation: smoke (9-short store incl. delay-slot store) OK; nested-jal OK;
  sweep of 1515 matched funcs: 1297 rc0, 16 budget, 180 memfault(HW), 9 unknown
  (mostly .word-data labels), 12 cop(GTE, unsupported by design).
- Per-function loop (proven on battle func_800AF400, 325 seeds x O0/O2/UBSan +
  mutant rejected, dashboard 2491->2492):
  1. transcribe C in src TU (replace INCLUDE_ASM in place, add externs w/ widths
     from access: lhu->u16 lh->s16 lw->u32/s32 lb->s8 lbu->u8)
  2. tools/remu/tests/diff_<addr>.c (host links REAL TU built with
     tools/remu/tests/remu_shim.h neutralizing INCLUDE_ASM; retail via remu;
     edges + single-bits + ~200-300 LCG seeds; require rc==0, zero stubs)
  3. tools/remu/tests/mut_<test>.sh (test MUST fail on the mutant)
  4. tools/remu/run_diff.sh diff_<addr> src/<ov>/<tu>.c (clang; needs
     -DXENO_PC_PORT for types.h uintptr_t; UBSan included)
- extern decls must precede first use in the TU (C-order, not retail order).

## Matching-build mechanics learned (important)
- asm/ is GENERATED (gitignored). Linker scripts link .text ONLY from
  build/src/**/*.c.o (+ data/header asm). Combined asm/<unit>/<tu>.s are
  objdiff-only. => Decompiling = edit src only; no asm-tree or build.ninja
  changes needed. gears can't run here anyway (find_base_path requires the
  checkout dir to be named exactly `xenogears-decomp`; ours is -ai).
- decomp_status.py is src-driven; the matchings/nonmatchings split is lagging.
  Leave transcribed files where they are (honest signal until byte-verified).

## Port build on THIS host (works, LINK OK 2026-09-12)
- Brew prefix include/SDL2 is BROKEN (3 stray files; SDL_main.h missing).
  Real headers: /home/linuxbrew/.linuxbrew/Cellar/sdl2-compat/2.32.72/include.
- Fixes (all in /tmp, recreate if lost):
  /tmp/sdlfix/SDL2Config.cmake shadow package (repairs SDL2_INCLUDE_DIRS +
  SDL2::SDL2 INTERFACE_INCLUDE) -> cmake -DSDL2_DIR=/tmp/sdlfix
  /tmp/pcfix/sdl2.pc (fixed includedir) -> PKG_CONFIG_PATH=/tmp/pcfix:...
- Maintained flow: ./pc_port/build_port.sh (NOT the stale pc_port/CMakeLists;
  its port_main.c references a nonexistent test_input.h). Result: LINK OK,
  68 function stubs (was 74).
- Battle TUs are REFERENCE_ONLY in the port (MIPS-emu owned); battle
  transcription = matching progress, not port behavior. Port behavior moves via
  field/slus/menu/shop native units + stub/override retirement.

## Wave 1 fan-out (5 subagents, file-disjoint TU partitions, max ~12/~10 DONE each)
A: battle main..main35 | B: main36..main60 | C: main61..main135 |
D: mainc*+mainl* | E: menu/slus/shop leftovers (matching-faithful, no port-branch edits).
Work list: /tmp/work_unported.txt (690 items: `tu func instr=N jal=M exotic=K status`).
Background: full port-test sweep (439 runners, -P16) -> /tmp/portsweep/results.txt.

## 2026-09-12 midday: main36 7/7 + main37 3/3 proven (partition B)
- ALL GREEN (O0/O2/UBSan + mutant rejected): 80085388, 80085454, 80085618,
  80085AC4, 80085B58, 80085C48, 80085C88, 80085CCC, 80085D34, 80085E78.
- New tests: diff_80085C48/mut, diff_80085C88/mut (-fno-data-sections),
  diff_80085CCC/mut, diff_80085D34/mut, diff_80085E78/mut,
  tools/remu/tests/76a10_log.s (table-driven logging stub, ret
  0x55/0xAA/0xFF; b walk 0->55->FF->FE covers the add-back wrap).
- 80085B58 TU snapshots entry-rdx via `asm volatile("mov %%edx,%0")`:
  clang O0 ignores `register __asm__("rdx")` locals (spills garbage);
  the disasm was verified (mov %edx first) at O0/O1/O2.
- 80085C88 composition test: S54[52,80) aliases S88[0,28) in guest
  (0x800D2C54+52 == 0x800D2C88): read-only inputs are no-write-checked
  against poke images, not host-vs-retail.
- HARD LESSON: never hand-assemble stub opcodes (5 of 15 wrong in
  76a10_log.s: rs/rt digit slips). Generate with bit-field python
  (R(op,rs,rt,rd,sh,fn)/I(op,rs,rt,imm)) and keep the stub-image assert.
  remu entry = FIRST OPCODE's vram: stub code must live AT the jal target.
- Test-harness bug pattern: save/restore of host-mutated images must come
  AFTER host-side checks, before retail pokes.
- 80085EB4 ALL GREEN (main38): search loops + jtbl_80070280 fall-through
  (a0 = 12-a3) + residue-a1. D_800D2D24 aliases CCD3E+24550 in guest:
  images forced identical up front (v1=67 reads the shared byte),
  union-aware no-write check. Retail ret via remu_get_reg(v0).
- run_npc_event fully green except host-broken gcc UBSan link: O0/O2 gcc
  certificates PASS, UBSan completed with clang (cert PASS, empty stderr,
  normalized output identical to O0), M1-M3 mutants DETECTED. Artifacts in
  pc_port/build_native/npc_event (UBSan_clang*). Shim used for the manual
  UBSan build only (implicit decls incl. func_8009E574/8009E810/8009F5F4/
  8009FD10/800A0C94/800A0D3C/80098CAC/8009C01C/8009CCF8): clang-22 makes
  conflicting-types-from-implicit-decl a hard error gcc -fpermissive
  accepts; real LP64 pointer fixes in port sources are separate work.
