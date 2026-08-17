# W34B24-PRE3 — Read-Only Audit of wm_8008A72C (Slot-1 World Player Callback)

Mode: READ ONLY. No source changes, nothing committed.

## 1. Boundary (independently verified)

| Field | Value |
|---|---|
| target | `wm_8008A72C` (slot-1 cb1, the measured live callback frontier) |
| boundary | `[0x8008A72C, 0x8008B2BC)` |
| bytes / insns | 2960 / **740** (capstone 740/740, no undecoded words) |
| retail_sha256 | `1d58efac94432cb6892462260a1d7679dfcfddc9d0568b08244b1fd7cf1b4b3f` |
| file offset | `0x1AC3C` in `disc/world_map.bin` (base `0x8006FAF0`) |
| edges | `jr $ra; nop` before start; body ends `jr $ra; nop`; next word `0x27BDFFD8` = fresh prologue |

## 2. ABI

```
signature  = s32 wm_8008A72C(s32 slot_idx)     /* scheduler cb1: a0 = slot index (observed a0=1) */
return     = CONSTANT 1 ($s5=1 from the prologue, returned on EVERY path; the
             scheduler stores (s16)1 to slot+0x00 keeping state 1 -> this is the
             persistent per-frame world PLAYER handler)
frame      = 0x38; saves $s0-$s5, $ra
slot rec   = $s0 = *(D_8009BE24) + slot_idx*128     (128-byte pool records)
$s1        = preserved slot_idx (late use: object id = slot_idx + 0x2E)
$s2        = 0x1F800000 scratch (uses +0x00..0x0C, +0x30..0x3C, +0x90..0x9C, +0xA0)
COP2       = none in the body
```

## 3. Slot-record field map (s0-relative)

| Off | Width | Meaning |
|---|---|---|
| +0x04 | s16 | transient substate (3 -> state 1; 2 -> state 0x28; 6 -> lap-counter bump w/ D_8009C170 gate) |
| +0x20 | s16 | MAIN STATE (JT1 index 0..0x40) |
| +0x24 | s16 | suppress-registration flag (tail calls 74794 only when 0) |
| +0x28..0x34 | s32x4 | position (20.12) |
| +0x38..0x44 | s32x4 | velocity |
| +0x48 | u16 | heading |
| +0x4A | s16 | speed (<<12 = 95414 scale arg) |
| +0x4C | ptr | actor ptr (s8 anim flag +0xAF; passed to func_800245D8) |
| +0x50/+0x54 | s32 | approach target x/z (>>12) |
| +0x58 | s32 | lap/progress counter |
| +0x5C | s32 | raw warp heading |
| +0x86/+0x106/+0x286/+0x306 | s16 | writes into NEIGHBOR slot records (slot2/3 handshakes) |

## 4. Globals / tables

- `D_8009BE24` pool base (neighbor access +0x228/22C/230/248, +0x3A8/3AC/3B0/3C8).
- `D_8009C170` mode word; `D_8009BE10` mode flag (written 1; forwarded as 95414 5th arg);
  `D_8009BD04` s16; `D_8009BD60` current-area byte; `D_8009D738` boundary byte (8C040 out).
- Breadcrumb gate table `0x8009B180` (initialized overlay data): `lhu [0x8009B180 + signext8(area)<<1]`
  — SIGNED byte index (sll 0x10 / sra 0xF). First 16 halfwords: 1,1,0,1,1,1,256,256,256,0,509,508,507,0,320,352.
- Breadcrumb ring: 32 x 0x14 at `0x8009CEC4`, u16 index `0x8009D154` ((i+1)&0x1F), entry = pos 4 words + heading at +0x10.
- Mirrors: `D_8009D554`, `D_8009D55C` block, `D_8009D52C`, `D_8009D7CC`.
- Resident (below overlay base): bytes `0x8006F8E5/E6/E7` (resync + slot2/3 busy),
  `0x8006F368/369/36A` (slot4/2/3 presence, 0xFF = absent), u16 `0x8006EF90/EF92` (warp target),
  `0x8006EE5A` (warp heading), output mirror `0x8006EE54/EE56/EE58` (pos.x>>12, pos.z>>12, heading — every dispatch).

## 5. Jump tables (full decode in JUMP_TABLES.csv)

- JT1 @ `0x80070518`, 65 entries, index = `lh slot[+0x20]`, `sltiu 0x41` guard, OOR -> common tail.
  19 live targets; 46 slots park (states 4-7, 0xB-0xC, 0x13-0x27, 0x2E-0x40).
- JT2 @ `0x80070620`, 5 entries, index = `wm_80090A84(slot) - 1`, `sltiu 5` guard;
  out-of-range (result 0 or >=6) IS the live movement path, not an error.
  Both tables outside the body, null-terminated in the image.

## 6. The movement block (live path)

```
vel==0: actor[0xAF]!=0 -> 245D8(actor,0); 894C8(0x2F)
vel!=0: actor[0xAF]!=1 -> 245D8(actor,1); then 8C1DC(0x2F, slot, 0x1F800000)
r = (s16)95414(slot+0x28, slot+0x38, 0x1F800090, (lh slot[+0x4A])<<12, *(D_8009BE10));
r==0: slot[+0x38..0x44] = scratch[0x90..0x9C]; r = (s16)95414(same);
      r==0 again: slot[+0x40]=0; slot[+0x38]=0        /* note: NOT +0x3C/+0x44 */
r==1: 8C040(0x1F800090, 0x10, 0x20, D_8009D738, D_8009BD60);
      area = lbu[D738] (+3 if lbu[BD60]==7);
      if lhu[0x8009B180 + signext8(area)*2] != 0:
          commit pos <- scratch[0x90..0x9C];
          if vel.x|vel.z: ring-append pos+heading; 7528C(slot+0x28)
else: 8C040(slot+0x28, 0x10, 0x20, D738, BD60)
then: 94238(slot+0x28, 0); zero vel (+0x40,+0x3C,+0x38); pos -> D_8009D55C;
      heading -> D_8009D52C; pre-tail; common tail (mirror + conditional 74794).
```

The r==0 retry is the wall-slide protocol the 951A8/94A5C ladder was built for.

## 7. Dependency closure (G3 verdict)

15 unique callees / 38 JAL sites, verified against a canonical-content build
(tree `ea03c4c`, nm on the built binary):
14/14 non-keystone callees have exactly one strong T (90A84, func_800245D8,
894C8, 8C1DC, 8C040, 7528C, 94238, 97770, 941C4, 93978, rcos, rsin, 8BEC8,
74794). wm_80095414 = CANONICAL-PENDING (lane I7). **MISSING = 0.**

## 8. a0=1 first-dispatch live path

Fresh record -> state 0 -> JT1 slot 0 -> resync byte 0 -> JT2 on 90A84;
class 0/>=6 enters the movement block, so **95414 is exercised on the first
natural dispatch**. Common tail runs every dispatch (mirror to 0x8006EE54..
+ 74794 while +0x24==0). Return always 1 -> re-dispatched every pass.

## 9. Implementability

**READY_LARGE** (pending 95414 landing); zero unresolved deps; no COP2; both
tables fully decoded; no undecoded words.

Certification strategy: seams for all 15 callees (90A84 drives JT2 coverage,
95414 drives the retry matrix, 8BEC8==3 advances approach states, 97770 gates
handshakes); oracle classes (a) substates 2/3/6 incl. C170 lap gate,
(b) every live JT1 slot + parked + OOR, (c) JT2 1..5 + both OOR classes,
(d) movement matrix r in {1, 0->1, 0->0} x breadcrumb {0,!=0} x vel {0,!=0}
incl. exact copy/commit sets, (e) ring append/wrap + 32-entry refills with
s16-faithful counters, (f) neighbor writes + resident 0xFF arms, (g) tail
mirrors + 74794 gate, (h) constant-return-1. Mutants: guard 0x41, table slot
swap, substate polarity, 90A84 bias(-1), retry copy set, breadcrumb sign
extension, ring mask, neighbor offset, resident byte width, heading width,
commit-set completeness, return booleanization, 74794 gate polarity.

## 10. Top hazards

1. Neighbor slot-record writes (pool-relative +0x286/+0x306; pool+curbyte*128) — never s0-relative.
2. 95414 retry protocol: velocity rewrite copies 4 words (+0x38..0x44) but the
   final zero-fallback writes ONLY +0x40 and +0x38.
3. Signed breadcrumb index (area bytes >=0x80 index BELOW 0x8009B180).
4. Constant return 1 on every path (scheduler state machine depends on it).
5. s16-faithful ring loop counters and the u16 (i+1)&0x1F ring index.
