Analysis scripts for docs/evidence/field-codegen-20260921/REPORT.md.

  dump.sh      run INSIDE the toolchain container; objdumps every
               build/src/field/**/*.c.o with relocations into
               build/field_disasm.txt
  bytecmp.py   compares each built function against
               asm/field/matchings/<tu>/<func>.s, masking relocated words,
               and regenerates divergent_functions.txt
