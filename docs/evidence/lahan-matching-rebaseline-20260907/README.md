# Fresh matching rebaseline

Current dirty checkout at3a3e7aac, no source edits during this rebaseline.
Toolchain: localhost/xenogears-dev-toolchain:current, pinned splat64 0.33.2,
spimdisasm1.33.0, rabbitizer1.12.6. TMPDIR=/var/tmp. Live game executes its own
frozen native binary, so the matching build did not alter its execution image.

`make check` completes the clean build but fails the unchanged retail checksum
gate for slus_006.64, field.bin, and shop_menu.bin. Member-change and menu pass.
Before/after hashes and full build log are retained. No checksums were repinned.

A fresh per-handler field audit compares483 table slots/481 unique handlers:
- Dispatch target census: zero table mismatches.
- Instruction comparisons:346 match,54 code mismatches,78 size mismatches,
  3 jump-table relocation target comparisons unresolved.
- After considering native ownership, classes are342 RETAIL_MIPS_MATCH,
  131 C_NONMATCHING_UNPROVEN,8 PORT_OVERRIDE_UNPROVEN.
- Native dependency and full retail gates both FAIL. Broad dependency counts
  include conservative static results and are not a count of gameplay defects.

The three relocation cases are ConditionalJmp, func_8008E59C, func_800947B0.
The tool reports missing current relocations for named retail jump tables;
that alone does not establish a wrong semantic target. Investigate compiler
local-table representation before changing their game code.

This replaces stale or inferred percentages with a current worklist, not a
claim of complete Lahan fidelity. Metadata and audit details are in field-vm.json.
