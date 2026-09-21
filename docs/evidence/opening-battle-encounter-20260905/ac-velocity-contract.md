# AC and horizontal velocity implementation contract

Retail image: SLUS_006.64, SHA-256 dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119.
AC handler 80021644..80021698; angle setter 80021FE0..80022000;
velocity helper 80022974..80022A00. Retail instruction ranges were independently disassembled from that image.

AC must call RNG before reading its unsigned range (including RNG-seed
aliases), multiply the low random byte by that byte, shift 8, subtract
range>>1, scale by 16, add sprite angle +32 and narrow signed 16 bits.
It delegates the angle store and velocity calculation to the actual helpers.

Velocity loads radius +18 and divisor bits +AC before trig. Radius is
arithmetic-shifted 4 then logical-shifted 8 with 32-bit wrapping. Signed
DIV by a nonnegative 12-bit divisor returns the quotient; zero divisor
preserves R3000 LO semantics: negative numerator => 1, else FFFFFFFF.
Retail trig masks its signed angle to the low 12 bits for direct table lookup.
Decomp rsin names retail cosine; decomp rcos names retail sine.
Cosine is arithmetic-shifted 2, multiplied by quotient with 32-bit wrapping,
then arithmetic-shifted 6 and stored to +C. Angle is reloaded before that
store for the second trig call. Sine is arithmetic-shifted 2, multiplied
with wrapping, negated modulo 2^32 BEFORE arithmetic shift 6, stored +14.
Do not add clamping, saturation, epsilon or a silent zero-divisor fallback.

Acceptance: full dispatcher/helper retail-instruction differential fixture,
actual native RNG and trig, complete guarded sprite/operands and seed,
O0/O2/UBSan and meaningful mutations. Then native build and ordinary
opening replay; this local contract alone is not runtime or visual proof.
