# C4 bounded retail audit, 2026-09-05

Read-only repository audit; sole written artifact is this report. Repository `/var/home/blizz/Projects/xenogears-decomp-ai`, verified HEAD `3a3e7aac03a2f166fb924945a489e392d706f282`, existing extensive dirt preserved. No game control or production/test edits.

## Authority and instruction contract

Verified `disc/SLUS_006.64` SHA-256 `dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119`; VA-to-file subtract `0x8000F800`.

| Half-open retail slice | SHA-256 |
|---|---|
| C4 `800215B8..80021644` | `d7ec4084d92d84a4bf626d4041f06a7f2a180914f393757d7fd3bd4d03faa0cd` |
| RotMatrix `8003F738..8003F8B0` | `7fc22ca6e32404ca5e686638cf10ed556c897420df23d54ce7b2e4fb9b8fe9e1` |
| ApplyMatrixLV `8004947C..800495DC` | `b5cca44ff50bf0ca0e5996e8f5cf1b954740710e6773d57827b764a47fee082e` |

C4 calls RNG once, then loads the unsigned operand byte. Let `p=(rng&255)*range`, `delta=(p>>8)-(range>>1)`, `angle=sign_extend16(delta*16)`. Products are nonnegative, so the retail negative division adjustment is unreachable. Delta spans -127..127; angle is a multiple of 16 in -2032..2032. Construct SVECTOR `(0,0,angle)`, rotate sprite velocity VECTOR at offsets +0C/+10/+14 into local VECTOR, and only then write those three words back. Existing dispatcher owns advancement.

## Exact zero-X/Y matrix

Direct substitution of the signed table loads and multiply/shift sequence in retail RotMatrix yields row-major `[[c,-s,0],[s,c,0],[0,0,4096]]`, with semantic `s=sin(angle)`, `c=cos(angle)`. All intermediate products are exact multiples of 4096 in this specialization. C1's zero-Z rounding warning does not apply here.

Verified all 8192 signed halfwords in native `pc_port/extern/PsyCross/src/gte/rcossin_tbl.h` equal retail table `800523F0..800563F0`. Verified sine odd/cosine even symmetry for all 4096 indices. Native `LIBGTE.C:606` RotMatrix therefore produces precisely the retail matrix for zero X/Y, including negative signed angles. No global RotMatrix repair is necessary. In game translation units the intentional shim swaps the decomp names: semantic sine is `rcos`; semantic cosine is `rsin`. PsyCross's own translation unit remains conventional.

## ApplyMatrixLV exact contract

Retail `8004947C..495DC` loads the matrix into rotation controls, then splits each signed 32-bit component into quotient and remainder by 32768 using signed magnitude. For ordinary negative values this means quotient truncation toward zero and a negative remainder. Retail uses wrapping MIPS negation; INT_MIN is a special case with quotient +65536 and remainder zero.

1. Load each quotient into IR1..3 with MTC2. Multiplication consumes the signed LOW HALFWORD of each IR, not its full 32-bit load.
2. Execute `0x041E012`: rotation matrix, IR vector, no translation, SF=0, LM=0. Read MAC1..3 as 32-bit words.
3. Load remainders into IR1..3; execute `0x049E012`, same operands with SF=1. Read MAC1..3.
4. Each output word is modulo-32 `MAC_remainder + (MAC_quotient << 3)`. Do not read saturated IR outputs. Preserve output pad.

Native `LIBGTE.C:527` follows this same split and op sequence. `inline_c.h:74` loads 32-bit values via MTC2; `gtereg.h:131` and `PsyX_GTE.cpp:23` select signed16 IR inputs. `inline_c.h:271/273` has exactly the retail operation words. `PsyX_GTE.cpp:112` shifts the wide sum before converting to int; no 44-bit overflow is possible in C4. Final GTE state comes from the second operation: matrix is installed, IR/MAC/FLAG reflect remainder rotation, not combined output. Both operations must remain if preserving those effects.

### Precise limitations and UB scope

Native INT_MIN negation is undefined C/C++ behavior. Native multiply-by-eight, positive left shift, and final addition can also overflow for arbitrary matrices, but those sites DO NOT overflow for C4: the largest absolute row sum is 5793 over all table angles (5792 over C4's multiples of 16), and quotient/remainder inputs are signed16 bounded. Combined magnitude remains below 1.519 billion. Thus INT_MIN is the specific arithmetic UB gap in C4, not ordinary observed velocity.

A full 64-bit dot against the original 32-bit vector is not a valid replacement: quotient truncation through IR matters once quotient leaves signed16. Concrete derived retail outputs for identity matrix:

- `(32768,0,0)` -> `(32768,0,0)`.
- `(1073741824,0,0)` -> `(-1073741824,0,0)` because quotient +32768 becomes IR -32768.
- `(INT_MIN,0,0)` -> `(0,0,0)` because quotient +65536 becomes IR zero.

For angle 512, `(-1073741823,1073741823,0)` -> `(-1518338047,0,0)`; the negative residual arithmetic shift must retain floor rounding.

## Smallest recommended implementation

Keep native RotMatrix for `(0,0,angle)`. A C4-local port helper in the root-owned animation translation unit can preserve the exact ApplyMatrixLV sequence while replacing only its undefined scalar arithmetic. Under XENO_PC_PORT include native `<inline_c.h>`; existing `<psyq/inline_c.h>` is not a native shim path. Use SetRotMatrix and MTC2/doCOP2/MFC2, or their existing gte macros.

For input word `u` represented as uint32, the exact split is:

```c
if (u & 0x80000000u) {
    uint32_t magnitude = 0u - u;
    uint32_t shifted = (magnitude >> 15)
        | ((magnitude & 0x80000000u) ? 0xfffe0000u : 0u);
    quotient = 0u - shifted;
    remainder = 0u - (magnitude & 0x7fffu);
} else {
    quotient = u >> 15;
    remainder = u & 0x7fffu;
}
```

MTC2 quotient words to 9/10/11, doCOP2(0x041E012), save uint32 MFC2(25/26/27), MTC2 remainder words, doCOP2(0x049E012), combine uint32 `MFC2(25+i)+(saved[i]<<3)`, then bit-preserving store/memcpy into output signed word. This preserves wrapping, all original matrix/GTE effects, INT_MIN behavior, and output aliasing. Snapshot all three source components before output writes. Leave global PsyCross and renderer callers alone.

## Required bounded validation and negative controls

Differentially execute pinned retail C4 and production command with the same GTE backend; report this as retail-instruction/host-GTE equivalence, not independent PS1 hardware evidence. Compare entire sprite, command pointer/return behavior, RNG count/state, and GTE registers; separately verify output pad and guard bytes.

Cover every unsigned range 0..255 and RNG low byte 0..255 (including range 0 still consuming RNG), signed angles on both sides of zero, and source component boundaries 0, +/-1, +/-32767, +/-32768, +/-32769, +/-1073741823, +/-1073741824, +/-1073741825, INT_MIN, INT_MAX. Include mixed signs/nonzero Z and negative residuals. Run ordinary optimization variants plus UBSan for the scoped helper. Matrix equality can exhaust all 4096 angles independently of the command.

Negative controls must kill signed operand interpretation, wrong trig convention/sign, input narrowing to SVECTOR, direct full32 dot bypassing quotient truncation, SF0/SF1 mistakes, MAC-to-IR saturated stores, wrong negative quotient division, arithmetic shift replaced by truncate-toward-zero division, wrong RNG count, and command advancement errors. Identity +32768 catches naive vector narrowing; identity +2^30 catches full32-dot substitution; INT_MIN catches negation handling; angle512 mixed signs catches negative floor rounding.

No runtime capture or new compiled differential test was performed by this auditor. Full PS1 hardware/GTE accuracy remains outside this bounded audit; the source-derived matrix/table equality and instruction arithmetic above are verified, while root/test worker own production implementation and executable validation.

## Root scratch candidate review

Reviewed `/tmp/xeno-opening-ac-20260905/animation_scripts.c4-candidate.c`, helper lines 48..78 and case C4 lines 306..328. No blocking discrepancy: exact split including INT_MIN, both GTE passes, RNG-before-operand read, matrix specialization, write order and matching-build fallback all conform. The uint32-to-s32 result casts are implementation-defined ISO C, not overflow UB; they preserve bits on the project GCC/Clang targets. A memcpy bitcast is an optional portability improvement. SetRotMatrix reads a matrix-padding upper halfword in its fifth load, as retail does; CTC2 register 4 sign-extends the low halfword, so the upper padding does not affect meaningful GTE control state.
