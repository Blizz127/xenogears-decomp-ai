# W34B24-PRE4 — Callee-closure audit of wm_80071A58 (slot-14 cb1)

Mode: READ ONLY for `0x80071A58`. No `wm_80071A58` source.

Canonical at audit: `822b40253631f04c477416da8eb22cef1ccbecfc`
(`Implement world slot-13 callback 0x80092FD8`).

Natural runtime has reached this body (`92FD8_RETURN=1` → slot-14 cb1).
Implementation of `0x80071A58` is allowed only after `MISSING_CALLEES=0`.

## 1. Boundary (independently verified)

| Field | Value |
|---|---|
| target | `wm_80071A58` (slot-14 Table-A cb1; partner of accepted 2-insn leaf `wm_80071A50`) |
| boundary | `[0x80071A58, 0x80071B9C)` |
| bytes / insns | 324 / **81** (capstone 81/81, no undecoded words) |
| retail_sha256 | `2b55700fe427e01935aa796cd4f9171d2b6fae2cbaa472bf1b40b1556d601ccf` |
| file offset | `0x1F68` in `disc/world_map.bin` (base `0x8006FAF0`) |
| edges | previous function `wm_80071A50` is `jr $ra; addiu $v0, $zero, 1`; this body ends `jr $ra; nop`; next word is a new `lui`/`beq` prologue |

`world_map.bin` SHA-256 `4c15fd32b3a03d7cd5ea4403dcaabc70abaf99b6aaca65d63d6866803edaac70`, size 180422.

## 2. ABI

```
signature  = s32 wm_80071A58(s32 slot_idx)   /* scheduler cb1; $a0 unused */
return     = CONSTANT 1  (addiu $v0, $zero, 1 before the optional 740B8 tail)
frame      = 0x20; saves $s0, $ra
```

Control:

1. `lw` `0x8009D144`. Zero → `wm_80097440(0x8009BD40)`; else `wm_80097244(0x8009BD40)`.
2. Unconditional: `89748`, `89C78`, `85CDC`, `8615C`, `747DC`, `848F4`.
3. `wm_800980D4(0x8009BBB4)`.
4. If `lh 0x8009D558 != 0`: `981C8(0x8009BE28)`, `96130`, `98CC0`.
5. `983A0(0x8009BE28)`.
6. `9932C(*(*0x8009BE3C+0x70), *(*0x8009BE3C+0x74), 0x8009BE28)`.
7. `*0x8009C5BC += 0x40`.
8. `73B04`, `737EC`, `86798`.
9. If `lhu 0x8006EE76 == 0`: `740B8`. Then `$v0 = 1`.

Eighteen overlay `jal` sites; eighteen unique targets. No `jalr` / jump table inside 71A58.

## 3. Closure table (18 overlay JALs)

`ACCEPTED` = strong production `wm_*` definition in `pc_port/src` and listed in `build_port.sh`.
PsyQ / SLUS `0x8001xxxx–0x8006xxxx` residents are not overlay missing-callees.

Prior note that `737EC` / `86798` / `740B8` were already accepted is **false**.
Only `73B04` is present on canonical.

| # | VA | Boundary | B / I | SHA-256 | File off | Canonical | Overlay callees | Verdict |
|---|---|---|---|---|---|---|---|---|
| 1 | `0x80097440` | `[0x80097440, 0x8009766C)` | 556 / 139 | `6bce51ba8bb3a013e923fcc2b66b1bc59646522ac76aad21923171cb89ddfd4b` | `0x27950` | MISSING | none (PsyQ: RotMatrixX/Y/Z, MulMatrix0×2, ApplyMatrix, TransMatrix) | **IMPLEMENTABLE** |
| 2 | `0x80097244` | `[0x80097244, 0x80097440)` | 508 / 127 | `2d0e27a454a833ee8c31bc741150c0ec4a6491fd932046bbed8646628fb341f5` | `0x27754` | MISSING | none (PsyQ: VectorNormal×3, OuterProduct12×2, ApplyMatrix, TransMatrix) | **IMPLEMENTABLE** |
| 3 | `0x80089748` | `[0x80089748, 0x80089C78)` | 1328 / 332 | `bfa19d4b844d7814bedb336f1e9837944f935d5dad029a681557b5a65891da73` | `0x19C58` | MISSING | `0x80089580` | BLOCKED on 89580 |
| 4 | `0x80089C78` | `[0x80089C78, 0x8008A2C8)` | 1616 / 404 | `693edc23d4e33a80b08c68767a9984fe3ebfc274e980a10693aead42ad5944a2` | `0x1A188` | MISSING | `0x80093534` ACCEPTED | bounded; COP2 words |
| 5 | `0x80085CDC` | `[0x80085CDC, 0x80085FE0)` | 772 / 193 | `352a8af52541f4373599db52b1b57263ebeee5942f9ac1e11bdf691169e59d54` | `0x161EC` | MISSING | `0x80093484` ACCEPTED | bounded; COP2 words |
| 6 | `0x8008615C` | start clear (after `[86124,8615C)` free); first frame-0x28 return `0x800863D8` | UNRESOLVED end vs `0x800865A0` | — | `0x1666C` | MISSING | `0x80099BFC` | BLOCKED (99BFC + end) |
| 7 | `0x800747DC` | starts after accepted `74794`; several returns before accepted `75228` | UNRESOLVED exact end | — | `0x4CEC` | MISSING | `0x80093740`, `0x80093978` ACCEPTED | identity not locked |
| 8 | `0x800848F4` | `[0x800848F4, 0x80084D00)` | 1036 / 259 | `670f809b7d9c0bcbd32463495e24c057bda8469220f12f3d6de19b893830e93b` | `0x14E04` | MISSING | `0x80093534` ACCEPTED | bounded; COP2 words |
| 9 | `0x800980D4` | `[0x800980D4, 0x800981C8)` | 244 / 61 | `b2ad8897db6a2b556897d0e5c726d049e252e78907dedac61642d121d7d90b12` | `0x285E4` | MISSING | none | **IMPLEMENTABLE leaf** |
| 10 | `0x800981C8` | `[0x800981C8, 0x800983A0)` | 472 / 118 | `90e483f10929f9022a21582a9d8046cb978c9cf09927d848580f8a427905a6fe` | `0x286D8` | MISSING | none | **IMPLEMENTABLE leaf** |
| 11 | `0x80096130` | `[0x80096130, 0x8009623C)` | 268 / 67 | `92c4c23f0e129166ce0c21cca5431851a5b3c6662ce293682e2b947c24562bc0` | `0x26640` | MISSING | `0x800967E4` | BLOCKED on 967E4 |
| 12 | `0x80098CC0` | `[0x80098CC0, 0x8009932C)` | 1644 / 411 | `5a5d9b8f7fcee4651a39633c852c1415a8de66356140702c9eb9d85bd0fd62a4` | `0x291D0` | MISSING | `9623C`, `962B0`, `96328`, `965A4` | BLOCKED |
| 13 | `0x800983A0` | start clear; first frame-0x70 return `0x800987A4`; later code to `98CC0` | UNRESOLVED | — | `0x288B0` | MISSING | `0x800987AC` | BLOCKED |
| 14 | `0x8009932C` | start clear; first frame-0x38 return `0x80099700` | UNRESOLVED | — | `0x2983C` | MISSING | `0x80099708` | BLOCKED |
| 15 | `0x80073B04` | `[0x80073B04, 0x80073E30)` | 812 / 203 | `87f9ff21fffad640d8fb7ec9a96da4f5d29a71275c4b15452d49677b6ad23197` | `0x4014` | **ACCEPTED** | none (PsyQ) | ACCEPTED |
| 16 | `0x800737EC` | `[0x800737EC, 0x800739B8)` | 460 / 115 | `e967f7509aa004e89b3876a53b9673a0bac961055e2f298012d8abf01be9d621` | `0x3CFC` | MISSING | none (PsyQ: RotMatrixYXZ, CompMatrix, SetRot/Trans, RotTransPers4) | **IMPLEMENTABLE** |
| 17 | `0x80086798` | start clear (after wrap leaf `86700`); `jalr $v0` at `0x800867CC` | UNBOUNDED | — | `0x16CA8` | MISSING | JALR through `lw` `0x8009CD40` | **genuine blocker** (indirect) |
| 18 | `0x800740B8` | `[0x800740B8, 0x80074794)` | 1756 / 439 | `5ce110d0b87852c819c51394eee5c53b6fd4a37b6c4576a8e33f524eefe0fcf2` | `0x45C8` | MISSING | none (SLUS/PsyQ + COP2 `mtc2`) | bounded but large |

`71A58_MISSING_CALLEES=17` (every overlay target except accepted `73B04`).

## 4. First implementable missing prerequisite

**`0x80097440`** — first 71A58 `jal` (cold arm when `*0x8009D144 == 0`).

- Missing on canonical (no `wm_80097440`).
- Retail-bounded: previous function `97244` ends `jr $ra; nop`; this body restores frame `0x20` and `jr $ra; nop`; next function starts `addiu $sp, $sp, -0x18` at `0x8009766C`.
- `MISSING_CALLEES=0` (PsyQ only).
- Required directly by 71A58.

`0x80097244` is the sibling warm arm and is also MISSING=0. `980D4` / `981C8` / `737EC` are later implementable leaves. `86798`'s `jalr` is the first genuine semantic blocker on the tail; it does not block implementing the PsyQ/leaf prerequisites.

Nearby `0x800976C8` is a separate 12-insn pool-clear leaf (not a 97440 callee).

## 5. 71A58 is not started

Do not implement `0x80071A58` until the overlay missing set is empty.
No forced PC / callback / slot / state / scheduler / resolver work.
