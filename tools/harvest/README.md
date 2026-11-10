# tools/harvest

Scratch tooling used on branch decomp/public-harvest-20261109 to adopt byte-exact function bodies
from public decomp projects (ex-machina MIT, EtherAura GPL-3.0) over INCLUDE_ASM stubs.

- adopt.py   adopt <module> [--source xem|ea] [--max-insns N] [--diag] [--only f1,f2] [--altcc]: replaces live INCLUDE_ASM
             lines with the other project body (attribution comment, asm-label externs, guarded types), compiles the TU,
             compares every function (relocations resolved to absolute addresses) and the data sections against the base
             build, keeps only what is byte-exact.
- xidx.py    declaration indexer (ex-machina: src/decomp ; EtherAura: include,src) -> xem_decl.json / ea_decl.json
- dedup.py   removes repeated guarded XM_T_/XM_S_ blocks within a file
- gate.sh    from-clean make build, hashes vs baseline.sha, port-preprocess equality for changed files
- commit.sh  git add + commit + push
- Paths are hard-coded to the Devbox scratch layout (~/scratch/xeno-mit-harvest: base_build/, ext/xem, ext/ea, HARVEST.csv, baseline.sha).
- NEVER run make clean; \
tools/gears/prebuilt/gears report; \
grep -q '^ApplyMatrixSV = ' linker/undefined_funcs_auto.field.txt || \
	sed -i '/^ApplyMatrix = /a ApplyMatrixSV = 0x80049D3C;' linker/undefined_funcs_auto.field.txt; \
grep -q '^g_Heap = ' linker/undefined_syms_auto.field.txt || \
	echo 'g_Heap = 0x80059320;' >> linker/undefined_syms_auto.field.txt; \
ninja -t clean; \
ninja -j48; \
mkdir -p expected; \
rm -rf expected/asm; \
mv build/asm expected/asm; \
python3 tools/objdiff/objdiff_generate.py tools/objdiff/config.yaml
make[1]: Entering directory '/home/blizz/dev/xenogears-decomp-public-harvest'

         █▀▀ █▀▀ █▀█ █▀▄ █▀▀    Metabuild system
         █ █ █▀▀ █▀█ █▀▄ ▀▀█    for Xenogears
         ▀▀▀ ▀▀▀ ▀ ▀ ▀ ▀ ▀▀▀   :-----------------------

    
make[1]: Leaving directory '/home/blizz/dev/xenogears-decomp-public-harvest'

         █▀▀ █▀▀ █▀█ █▀▄ █▀▀    Metabuild system
         █ █ █▀▀ █▀█ █▀▄ ▀▀█    for Xenogears
         ▀▀▀ ▀▀▀ ▀ ▀ ▀ ▀ ▀▀▀   :-----------------------

     before adopting/gating: it builds with -DSKIP_ASM and pollutes build/. Do a from-clean make build first.
