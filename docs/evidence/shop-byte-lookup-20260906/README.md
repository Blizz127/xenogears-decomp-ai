# Shop byte lookup retail repair — 2026-09-06

`func_801CE8D8` now matches all 68 retail bytes at `801CE8D8..801CE91C`.
The full production shop translation unit was compiled with GCC 2.6, its
unchanged emitted function assembly extracted, and that body linked at the
retail address. Direct comparison against `disc/shop_menu.bin[0x98D8:0x991C]`
passes with SHA-256
`62321bfbdb3756149ced5268ea0e7022afa1688253efb270110f1a15d3d8740c`.
This is function-level proof; the complete shop module remains nonmatching.

The C now checks a positive count before forming the end address or reading
either array, masks the target's low byte at the retail point, and keeps the
retail signed end-address comparison. `intptr_t` preserves native pointer
width; its matching-only fallback is signed 32-bit. No linker/configuration
changes or literal instruction emission were used.

Root reviewed and independently ran
`pc_port/tests/run_shop_byte_lookup_retail_test.sh`. The actual production C
agrees with the raw retail MIPS function in 65 cases / 297 checks, covering
all 17 instructions under O0, O2, and mixed GCC/Clang UBSan. Tests cover first,
middle, last and absent keys, high target bits, nonpositive counts with native
inaccessible arrays, and a safe high native mapping crossing the low-32-bit
signed boundary. Four incorrect variants are rejected: target mask, count
guard, truncated end pointer, and return byte. Output:
`/tmp/xeno-shop-byte-lookup.KmiUKXaA`.

Private global `make check` completed all 472 tasks and still exits 2 with
the same three module checksum failures. Shop size decreases from 55,300 to
the retail 55,296 bytes, but the 16 remaining inline assembly bodies still
cause ordering mismatches. SLUS, field and the already exact member menu have
unchanged hashes. See `matching-after.json`; no whole-game matching claim.

The native build links successfully with SHA-256
`53c04117da90ed9860d2f6fd5f0a9f76d3898732ba8edebcdd916c0341dab49d`.
This build has not been played. The ongoing natural route retains its copied
preceding `cda8623b` binary and must not be restarted merely for this change.
The shop resource loader reconstruction is a separate pending change.

Scratch compiler/build evidence: `/tmp/xeno-shop-scan-c-20260906`.
Licensed payloads and generated binaries are not copied into this evidence
directory. No staging, commit, or push was performed.
