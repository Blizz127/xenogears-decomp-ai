# Native battle effect progression callbacks

Four previously missing native bodies now execute through the battle host
allowlist: `func_800A3490`, `func_800A3514`, `func_800A3578`, and
`func_800A35C8`. Retail `func_800AA820` selects these callback addresses.
Their behavior comes from the instructions in `disc/battle.bin`, not from a
replacement effect design. The native bridge explicitly treats all three
arguments as signed scalar halfwords. Zero divisors fall through to retail's
BREAK instruction instead of silently producing a value.

The bodies preserve wrap-before-compare behavior, the inclusive upper value 32
in 800A3514, and the existing retail trig-symbol convention (the retail entry
named rsin reads the cosine halfwords). Native-only changes use XENO_PC_PORT;
the MIPS assembly bodies remain intact.

## Evidence

- Pinned battle SHA256:
  `1830b4ef1fe37129972fc310dfad534f8161d6c0b123e74254c3711334a3e291`.
- Pinned SLUS SHA256:
  `dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119`.
- New `run_battle_effect_callbacks_retail_test.sh` compares actual retail
  instructions and trig table against the production runtime's native dispatch.
  It performs 270,932 comparisons per O0/O2/UBSan build (812,796 total), including
  all 65,536 phase halfwords, signed/wrap boundaries, pointer-shaped scalar
  values, and zero-divisor faults. Nonzero cases must execute fewer MIPS steps
  than the retail body, rejecting an interpreter-only false pass.
- Three deliberate native mutations fail: cap 32 changed to 31; removing
  signed-halfword wrap before the lower-bound comparison; swapping trig calls.
- Existing trig bridge and overlay leaf bridge regression suites pass,
  including their negative controls.
- Fresh compilation of the complete MIPS main68 translation unit before/after
  produces identical `.text`, SHA256
  `9c1ca9b17001db0e312f13aac9fa7dcc2dbd05f1700ab7f4d199a5113ece9541`.
  This establishes unchanged MIPS output; it does not claim that native x86
  machine code is byte-identical to MIPS.
- Full native build passes: generated function stubs decrease 82 -> 78;
  adopted battle leaves increase 92 -> 96; no adopted leaf reaches a generated
  stub. Data placeholders remain 577.

Scope: computational and native-dispatch acceptance. This change has not been
observed during a natural in-game battle effect, and is not full-game retail
parity or complete battle decompilation. The unfinished File menu stays guarded.
No saves or generated art changed. No push.

Local raw evidence: `scratchpad/astra-effects-20260919/` (build, differential,
bridge regression, trig regression, fresh MIPS compilation and byte comparison).
