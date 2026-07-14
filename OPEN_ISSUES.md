# Open Issues

Rules:

- If it's in this file, it's OPEN. Closing means DELETING the entry, not annotating it.
- Every item carries a reproducer command, not just a description.
- Every item carries `Last verified @ <commit>`. Stale verification = suspect claim.
- Every claim is tagged by evidence class:
  proven | build-verified only | observed | inferred

---

## Unimplemented opcodes in func_800248D4

Opcodes: 0x85, 0x8E, 0x98, 0xBE, 0xC8, 0xD4, 0xE2, 0xFA
Evidence: proven (assertion in source, enumerated)
Repro: unknown which scenes dispatch these — needs a dispatch sweep first
Last verified @ ed61298

## CompMatrix PsyQ decomp match

Audit is DONE (semantics verified vs retail 0x8004931C-0x80049478).
The MATCH is outstanding — handwritten GTE sequence, codegen-focused work.
Evidence: proven (audit); match not attempted
Last verified @ ed61298

## func_8002C700 / ModelPrimQuadFT4Variant0

Host-specific structural rewrites, no byte-match proof. Hardest remaining.
Evidence: proven (known port-only, not matched)
Last verified @ ed61298

## Map015 entrance 0 — func_8008399C assertion

Repro: launch Map015 entrance 0; assertion fires, map not runnable
Evidence: observed (surfaced during ABR scene sweep)
Last verified @ ed61298

## LIBGTE.C invalid UTF-8 byte

Blocks patch-editor modification of the file. Maintenance item.
Repro: iconv -f utf-8 -t utf-8 < pc_port/extern/PsyCross/src/psx/LIBGTE.C > /dev/null
Evidence: proven
Last verified @ ed61298
