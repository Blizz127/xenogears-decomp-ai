Erratum: the historical opcode-length-table SHA in this audit has an extra
trailing `1f`. The authoritative runner and provenance use the correct SHA256
`f329de9c682e328cd9ca4718972691f3bb716046f2f57803c3a24e9b19326c0a`.

# Retail animation opcode 0xA9 audit — 2026-09-06

## Outcome

**BOUNDED CANDIDATE READY.** Retail opcode `0xA9` is a two-byte instruction whose one-byte signed operand produces a scaled relative update to sprite word `+0x00`. The handler is the self-contained retail range `0x80021698..0x800216F4` and its only call is the already-known `func_80022CAC`. The handler does not update the animation PC. The shared `func_800248D4` tail advances sprite `+0x64` by `D_8004FC40[0xA9] == 2` and re-enters the interpreter.

Scratch candidate and raw-retail differential are under `/tmp/xeno-animation-a9-audit-20260906/`. No repository file was written.

## Runtime trigger

`docs/evidence/opening-battle-encounter-20260905/window-and-return-runtime-20260906.json` records the terminal condition after the already-accepted opening battle and map-14 return: a later battle aborted on opcode `0xA9`, dispatch index 31. The pinned run log has exactly one animation-dispatch failure:

```json
{"event":"sprite_animation_unimplemented","raw_opcode":169,"opcode":169,"dispatch_index":31,"sprite":"0x731f04","operands":"0x71e4ae"}
```

Authority pins:

- runtime evidence SHA-256 `68dcd3b970b6a1cb5520037bcee895c3abfd77b16850f72d77fd322c1a417df2`
- final run log SHA-256 `9915ba3a2c714a4b68ce0b9e3669edb84d51fa51202e6b2fbe2fce76c59a1954`
- run-time `animation_scripts.c` pin recorded in `run.json`: `51cef2ac3afc4ffce35925b28701370d1ac32843951bf207ffec65c6d4da4b53`
- currently inspected working source SHA-256 `7795145bda9f0e39577be0073110db2e7564fe9adab0a7cf4ef29e9bab357acf`; it remains modified by concurrent work and still has no `0xA9` case, reaching the hard stop at current lines 1204–1214.

The ended process left no crash-time guest script/RAM snapshot. Because execution stopped on the first unsupported command, no later opcode from this same natural battle script is evidenced. The current outer interpreter separately lists dedicated unfinished commands `0x85, 0x8E, 0x98, 0xC8, 0xD4, 0xE2, 0xFA`, but the run does not establish that any of them occur in this script.

## Retail authority and dispatch

The authority is `disc/SLUS_006.64`, 303,104 bytes, SHA-256 `dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119`. Its PS-X EXE header gives text VMA `0x80010000` at file offset `0x800`, so the byte mapping used here is `file_offset = VMA - 0x8000F800`.

Pinned ranges and values (full manifest: `/tmp/xeno-animation-a9-audit-20260906/pins.json`):

| Item | Retail range/value | SHA-256 |
|---|---:|---|
| full `func_8001FBE4` | `0x8001FBE4..0x80021AD8` | `7431f354f172d1b450e6c231528af93546595e8aa6d750d10ca489d75e88442c` |
| dispatch table | `0x800183D8..0x800185A4` | `ee73be97683cbf2affa5db0d128e5083ac4b273da3bc5c2f0bfc12bfb75ee0f8` |
| entry 31 / opcode A9 | word at `0x80018454` = `0x80021698` | `2a4aa2337bd40a56f022fa0bc1e006e85dfc40308061dc38f6014bf92d1505e8` |
| A9 handler | `0x80021698..0x800216F4` (92 bytes) | `9c729396bad3f859aa200f99af71826f25260c7f0fd8d8668dda58519b9caf83` |
| sole helper | `0x80022CAC..0x80022CDC` (48 bytes) | recorded in `pins.json` |
| shared call/advance tail | `0x80024EC8..0x80024F00` | recorded in `pins.json` |
| opcode length table | `0x8004FC40..0x8004FD40` | `f329de9c682e328cd9ca4718972691f3bb716046f2f57803c3a24e9b19326c0a1f` |
| A9 length | byte at `0x8004FCE9` = `2` | included in table pin |

Inspectable disassemblies:

- `/tmp/xeno-animation-a9-audit-20260906/retail-a9.disasm.txt`
- `/tmp/xeno-animation-a9-audit-20260906/retail-pc-tail.disasm.txt`

## Exact handler behavior

At `0x80021698`, retail performs these operations in this order:

1. Signed-load `operands[0]` (`lb`) and signed-load sprite halfword `+0x2C` (`lh`).
2. Signed multiply. The low 32-bit result is rounded toward zero for Q12 conversion: add `0xFFF` only when the low word is negative, then arithmetic-shift right 12.
3. Call `func_80022CAC(sprite, converted_value)`.
4. After the call, reload sprite word `+0xAC`; bit 2 controls sign reversal.
5. Shift the helper result left 16. If `(+0xAC & 4) != 0`, negate that shifted 32-bit value.
6. Add it modulo 2^32 to sprite word `+0x00` and store only that word.
7. Return through the common `0x80021AB8` epilogue.

The helper `func_80022CAC` reads an unsigned halfword at sprite `+0x3A`. Zero passes its input through. Otherwise it multiplies the signed input by that factor, adds `0x3FF` when the low 32-bit product is negative, then arithmetic-shifts right 10. Current `src/slus_006.64/system/temp1.c:102` and the native duplicate in `pc_port/src/game_overrides.c:3295` already express this helper; no new helper is required for the A9 body.

The outer interpreter supplies `operands = pc + 1`. Its shared default path at retail `0x80024EC8` calls `func_8001FBE4(sprite, opcode, operands)`, indexes the length table with the original opcode, adds the table byte to sprite `+0x64`, stores it, and jumps back to `0x8002490C`. Thus A9 consumes bytes `[0xA9, signed_operand]`. A handler-local PC write or invented skip would be wrong.

## Scratch differential

`/tmp/xeno-animation-a9-audit-20260906/candidate.c` implements only the A9 body and the bounded helper semantics. It uses bytewise little-endian accesses and explicit modulo-32 arithmetic so host signed overflow is not part of the candidate.

`/tmp/xeno-animation-a9-audit-20260906/differential.c` loads the pinned original SLUS into `PcPortMipsRun` and executes the actual full dispatcher at `0x8001FBE4`, including the retail A9 body and retail `func_80022CAC`. It compares the complete sprite/guard fixture with the candidate and records both retail and candidate helper entry, sprite pointer, and input value.

Coverage is finite rather than a full sprite-state proof:

- all 256 operand bytes, including signed negative values;
- 18 deterministic variants per operand (4,608 comparisons);
- representative signed `+0x2C` scales, unsigned `+0x3A` helper factors, wrap-boundary `+0x00` words, bit-2/bit-3-distinguishing `+0xAC` flags, high opcode register bits, and nine operand aliases into the sprite/external buffer;
- a separate raw `func_800248D4` sequence `[A9, 0x21, 0x86]` proving A9 advances exactly two bytes before the deliberately waiting `0x86`;
- Clang O0, O2, and UBSan; each reports `ANIMATION A9 PC ADVANCE PASS stride=2` and `ANIMATION A9 DIFFERENTIAL PASS cases=4608`;
- five controls are rejected: unsigned operand, missing Q12 negative rounding, wrong mirror bit, wrong destination word, and wrong helper input.

Run command:

```sh
/tmp/xeno-animation-a9-audit-20260906/run.sh
```

Scratch hashes:

- `candidate.c`: `b3e5119677246f831e49a463ea0a4c54f0d0f3ae5bed306bc1c410e18373ee9e`
- `differential.c`: `6816b06421349dbc56493c768a9b43f5cb8f426b4054dc51ba658e6f5ce09cfc`
- `run.sh`: `3d2018e64ac03412a769a94ad69d9f8b920d6861523557827adb8f8c6ab36b37`
- `pins.json`: `24270002224e48b2975f3e9f9427402fbf66ec5a3a2871c34e40330293f30583`

These hashes describe the final successful fixture and five-control run. `pins.json` also records every retained per-mode and control log.

## Minimal integration boundary and limits

A minimal source repair is one `case 0xA9` in `func_8001FBE4`, preserving the operation order above and calling the existing `func_80022CAC`. `func_800248D4` already owns the correct generic PC advancement and should remain unchanged for this opcode. Root owns integration; this audit made no production edit.

The differential proves the isolated handler, actual helper execution, bounded aliases, and one interpreter PC sequence. It does not prove later commands in the natural battle script, visual/gameplay completion, arbitrary helper re-entry side effects, or the entire animation interpreter. A replay may expose the next unsupported opcode only after this retail-backed handler is integrated.
