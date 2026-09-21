# W34B24-PRE2 — Read-Only Audit of wm_80084D00 + wm_80084DB8 + func_8004A70C

Mode: READ ONLY. No implementation, no source/build/test changes, no commits.

## Boundaries (independently recovered from disc/world_map.bin, base 0x8006FAF0)

| Function | Boundary | Size | SHA-256 (slice) |
|---|---|---|---|
| `wm_80084D00` | `[0x80084D00, 0x80084DB8)` | 184 B / 46 insns | `e8b73e51871b51f560c4ac5c5c02f533fc8d1a07d2c58792934967cc3c2487fc` |
| `wm_80084DB8` | `[0x80084DB8, 0x80085158)` | 928 B / 232 insns | `d113a209a78c12841424504119758295cdd5509f19b621f187cb4f897ea13d87` |
| `func_8004A70C` (resident) | `[0x8004A70C, 0x8004A730)` | 36 B / 9 insns | in `disc/SLUS_006.64` (load 0x80010000, file off = VA - 0x80010000 + 0x800) |

The "6 undecoded words" in 84DB8 are GTE COP2 instructions, not data:
3 x MVMVA (0x4A480012: sf=1, rot matrix x V0 + TR) at 0x80084F8C /
0x80084FC8 / 0x80085008, and 3 x `cfc2 $t4, FLAG` (0x484CF800) at
0x80084F9C / 0x80084FDC / 0x8008501C (FLAG stored to sp[0x10], never
read — incidental dead store).

## func_8004A70C IDENTITY: it is NormalClip

`config/symbol_addrs.slus_006.64.txt` names `0x8004A70C = NormalClip`; the
SLUS disassembly confirms the canonical 9-insn libgte NCLIP wrapper (mtc2
SXY0/SXY2/SXY1, NCLIP, mfc2 MAC0). Full contract in A70C_CONTRACT.md.

Contract: `s32 NormalClip(s32 sxy0, s32 sxy1, s32 sxy2)`, each arg packs
(y<<16)|(x&0xFFFF) (world usage: y=Z, x=X). Returns
MAC0 = x0*(y1-y2) + x1*(y2-y0) + x2*(y0-y1) — twice the signed 2D triangle
area. $a3 is unused (the W34B22-I5 report's "a3=mode" was a call-site
register residue, now corrected).

Implementability: READY_SMALL (binding, not new code) — PsyCross already
exports a real NormalClip (strong T in the current binary), and the
accepted wm_80085760 already binds its three 0x8004A70C call sites by that
name. Only world_map_func_94a5c.c still externs the address-name
func_8004A70C, which the auto-stub resolves to `return 0`.

EXISTING FIDELITY GAP FOUND: at runtime today wm_80094A5C's probe (complex
cases 5/6/9/10) always sees 0 ("straight" order) because the stub returns
0. Fix = bounded rebinding rung (rebind the extern or alias the symbol),
plus a one-time check that PsyCross's NormalClip wraps (retail MAC0 read
wraps to 32 bits; s16 x s16 x 3 sums can reach ±3.2e9 > s32 range —
wrapping required, never saturation/widening).

## wm_80084D00 — region scanner

    s32 wm_80084D00(u32 pos_vec /*$a0*/, u32 out_attr /*$a1, s16* */)
      count = lh [0x8009D7E0]        (s16 region count; RELOADED each iter)
      rec   = lw [0x8009C620]        (region record base, 84-byte stride —
                                      same globals as wm_80095414 case-3)
      for (i = 0; (s16)i < count; i++, rec += 0x54) {
          if (lhu rec[4] & 1) {              (region enabled flag, bit 0)
              r = (s16)wm_80084DB8(pos_vec, (s16)i);
              if (r != 0) { *(s16*)out_attr = i (sh); return r; }
          }
      }
      return 0;

Returns 84DB8's s16-truncated result = 2 x (containing-triangle count);
out_attr receives the region index — confirms the caller-side reading in
W34B24-PRE (sp[0x18] = attr halfword, count stepped by 2).

## wm_80084DB8 — point-in-triangle scan over one region's nav mesh

    s32 wm_80084DB8(u32 pos_vec /*$a0*/, s32 region_idx /*$a1, s16*/)
      rec = *[0x8009C620] + 84*region_idx
      dx = (pos[0] >> 12) - rec[0x08]; scratch[0x40] = dx  (stored BEFORE test)
      if (|dx| >= 0x800) return 0
      dz = rec[0x10] - (pos[8] >> 12); scratch[0x48] = dz  (operand order
                                                            OPPOSITE of dx!)
      if (|dz| >= 0x800) return 0
      copy rec[0x20..0x3F] (region MATRIX, 32 B) -> scratch 0xF0
      scratch[0x10C] = 0; scratch[0x104] = 0    (trans.x = trans.z = 0)
      scale vec (0x800,0x800,0x800) -> scratch +8, +4, +0 (that store order)
      scratch[0x108] = rec[0x0C]                (trans.y = region height)
      ScaleMatrix(0x1F8000F0, 0x1F800000); SetRotMatrix; SetTransMatrix
      hdr = rec[0x44]; count = hdr[0] (s32); verts = hdr[4] (SVECTOR, 8 B)
      point = (dz<<16) | (dx via lhu, u16 low) -> scratch[0x38]
      for (n = 0; n < count; n++)  (node records hdr+8, 14-byte stride) {
          transform verts[node+0], verts[node+2], verts[node+4] via MVMVA
            (sf=1, rot x V0 + TR) -> MAC1/2/3 stored to scratch
            0x00/0x10/0x20 blocks; cfc2 FLAG -> sp[0x10] after each (dead)
          3 edge tests: NormalClip(pack(Vi), pack(Vj), point) for pairs
            (V0,V1), (V1,V2), (V2,V0); pack = (z<<16)|(x&0xFFFF) staged
            through scratch[0x30]/[0x34]
          any result > 0 (bgtz) -> outside, skip node
          else: [0x8009D718+4k] = (s16)n (id); [0x8009D71A+4k] = node[+0xC]
                (type, lhu); k++; result += 2
      }
      return result   (== 2 x candidates recorded)

The candidate array layout (id@+0, type@+2, stride 4 at 0x8009D718) is
exactly what wm_80095414 loops 1-2 consume; node records are the same
14-byte triangle records (vertex indices +0/+2/+4, type +0xC) that 95414
case-3 walks via links +6/+8/+0xA.

## Dependency classification

| Callee | Status |
|---|---|
| wm_80084DB8 (from 84D00) | this ladder |
| ScaleMatrix / SetRotMatrix / SetTransMatrix | CANONICAL (PsyCross) |
| func_8004A70C = NormalClip | CANONICAL via PsyCross NormalClip; needs name rebinding + wrap check |
| inline GTE MVMVA x3 | no callee — needs native equivalent in the 84DB8 rung (main hazard) |

Callers (full-overlay JAL scan): 84D00 <- 0x800955F4 (95414 case-1) and
0x80095E78 (inside the NEXT function 0x80095CD4+, not yet audited);
84DB8 <- 84D00 only; 0x8004A70C <- 84DB8 x3, wm_80085760 x6 (accepted,
already name-bound), wm_80094A5C x4 (stub-bound today — the gap above).

## Hazards for the implementation rung

1. Inline GTE MVMVA — first ladder function with raw COP2 math; must
   reproduce MVMVA sf=1 (s16 rot x s16 vec + TR<<12, >>12; MAC1..3 read
   unsaturated). Verify against PsyCross MVMVA or transcribe exact math.
2. NormalClip wrap — certify PsyCross returns wrapped 32-bit MAC0.
3. dx = pos-rec but dz = rec-pos (asymmetric operand order).
4. Point pack reads dx back via lhu (u16) but dz via lw + sll 16.
5. Region flag gate (rec[4] bit 0); count reloaded per iteration in 84D00.
6. cfc2 FLAG stores are dead — transcribe or document, do not invent uses.
