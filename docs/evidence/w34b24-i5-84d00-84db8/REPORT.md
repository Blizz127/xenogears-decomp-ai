# W34B24-I5 — World Nav-Mesh Probes 0x80084DB8 + 0x80084D00 Implementation Report

## Lane

| Field | Value |
|---|---|
| START_CANONICAL | `7b378da39474b27df4a763bc4816401f364bb663` (fetched, local == origin) |
| branch | `candidate/w34b24-i5-84d00-84db8` (worktree `/tmp/w34b24-i5-84d00`) |
| audit basis | `docs/evidence/w34b24-pre2-84d00-84db8/` (committed with this rung for provenance) |

These are the LAST missing dependencies of keystone `wm_80095414`.

## Identity (independently re-verified)

| Function | Boundary | Size | File offset | SHA-256 (slice) |
|---|---|---|---|---|
| `wm_80084D00` | `[0x80084D00, 0x80084DB8)` | 184 B / 46 insns | 0x15210 | `e8b73e51871b51f560c4ac5c5c02f533fc8d1a07d2c58792934967cc3c2487fc` |
| `wm_80084DB8` | `[0x80084DB8, 0x80085158)` | 928 B / 232 insns | 0x152C8 | `d113a209a78c12841424504119758295cdd5509f19b621f187cb4f897ea13d87` |

Both match the PRE2 audit; slice SHAs re-verified by the run script on every
invocation. The 6 undecodable words in 84DB8 are COP2 instructions
(3x MVMVA `0x4A480012`, 3x `cfc2 FLAG`), not data.

## Retail contracts

### wm_80084D00 — region scanner

    s32 wm_80084D00(u32 pos_vec, u32 out_attr /* s16* */)
      count = lh[0x8009D7E0]; if (count <= 0) return 0
      rec = lw[0x8009C620]; i = 0
      loop: if (lhu rec[4] & 1) {
                r = sign16(wm_80084DB8(pos, sign16(i)))
                if (r != 0) { sh i -> *out_attr; return r; }
            }
            i++; rec += 0x54
            while (sign16(i) < lh[0x8009D7E0])   <- count RELOADED per pass
      return 0

Retail quirks preserved: s16 truncation of the probe result (0x10000
truncates to 0 and the scan CONTINUES), the sh store width, and the
per-iteration count reload.

### wm_80084DB8 — nav-mesh point-in-triangle probe

    s32 wm_80084DB8(u32 pos_vec, s32 region_idx)
      rec = *(0x8009C620) + 84*idx (u32 wrap)
      dx = (pos.X sra 12) - rec[8] -> sc[0x40]; dz = rec[0x10] - (pos.Z sra 12)
      -> sc[0x48] (ASYMMETRIC operand order; both stored even on reject)
      reject unless |dx| < 0x800 && |dz| < 0x800 (negu-wrap abs: INT_MIN
      wraps to itself and PASSES the signed slti — faithful)
      MATRIX copy rec[0x20..0x3F] -> 0xF0; t=(0, rec[0xC], 0); scale
      (0x800,0x800,0x800) at scratch 0 (stores +8,+4,+0);
      ScaleMatrix(0xF0, 0x1F800000) -> SetRotMatrix -> SetTransMatrix
      hdr=rec[0x44]; count=hdr[0] (CACHED in a stack slot, not reloaded);
      verts=hdr[4]; point=(dz<<16)|(lhu dx) -> sc[0x38]
      per 14-byte node (hdr+8): transform the 3 verts (lh s16 indices at
      +0/+2/+4, addr = verts + idx*8 with sign wrap) -> scratch
      0x00/0x10/0x20; packs pV = (z<<16)|(x&0xFFFF) (V0.x re-read via lhu
      both times); NormalClip(pV0,pV1,point), (pV1,pV2,point),
      (pV2,pV0,point) staged through sc[0x30]/sc[0x34]; any result > 0
      (bgtz) -> skip node; else append s16 id -> [0x8009D718+4k],
      u16 type(node[0xC]) -> [0x8009D71A+4k], result += 2
      return result

## MVMVA implementation decision

The three inline COP2 sequences (`lwc2 VXY0/VZ0`, MVMVA `0x4A480012`
[sf=1, mx=rot, v=V0, cv=TR, lm=0], `swc2 MAC1/MAC2/MAC3`, dead
`cfc2 FLAG`) are BOUND TO THE CANONICAL libgte `RotTrans`: PsyCross's
`RotTrans` body is `gte_ldv0; doCOP2(0x0480012); gte_stlvnl; gte_stflg` —
the IDENTICAL opcode payload executed against the same GTE state that
`SetRotMatrix`/`SetTransMatrix` loaded, with the identical MAC1..3 store
pattern. The retail `cfc2 FLAG -> sp[0x10]` store is provably dead (never
read); it maps to RotTrans's flag out-parameter written to a dead local.
This follows the accepted convention (call real libgte where behaviorally
identical) rather than reimplementing GTE integer math with private
state. `NormalClip` binds the real native symbol proven by the W34B24-I4
rebind.

## Certification

- `run_w34b24_i5_84d00_84db8.sh`: retail full-file + two slice SHA gates
  (fixture size check uses `stat -L` so symlinked fixtures also work);
  focused oracle at `-O0`, `-O2`, `-O2 -fsanitize=undefined
  -fno-sanitize-recover=all`; normalized stdout byte-identical; empty
  stderr.
- Independent oracle (hand-derived, synthetic region/mesh fixture in
  emulated RAM; recording scripted GTE/NCLIP seams per the accepted
  85760 pattern): gate boundaries at +-0x7FF/+-0x800 both signs plus the
  INT_MIN abs-wrap pass case; asymmetric delta directions asserted from
  stored scratch; matrix/scale/trans content + call order + pointer
  identity at ScaleMatrix time; point/edge packs asserted exactly incl.
  z<<16 truncation (0x54321 -> 0x4321xxxx), negative-x lhu low-u16, and
  the V0 lhu re-pack on edge 3; negative vertex index addressing;
  accept-on-zero (NCLIP result 0 is NOT > 0); skip-on-first/third edge;
  candidate append layout/stride/canary; 84D00 flag gating, first-hit
  return, s16-truncation miss (0x10000 -> continue), count-reload
  side-effect, sh width canary, empty table; end-to-end 84D00 -> real
  84DB8 integration.
- Two test-harness fixes during bring-up (test file / -Werror only, no
  production logic changes): a negative-value left shift in the fixture
  setup (UBSan-caught), and `__attribute__((unused))` on two helpers that
  mutant builds leave unreferenced.
- Mutants: 13/13 KILLED, distinct assertions (see MUTANTS.csv).
- Build: registered in `PORT_SOURCES`; clean + incremental `LINK OK`;
  exactly one strong `T` each; no stub shadow; stubs 240/524 unchanged.
- Suites: 20/20 PASS in the worktree (all accepted suites + this rung;
  73b04 with its documented PSYCROSS_LIB override).

## Runtime payoff

None yet by design: both callers of 84D00 (`wm_80095414` case-1 and the
unaudited `0x80095CD4` sibling) still lack bodies. This rung closes the
keystone's dependency DAG: wm_80095414 now has zero missing direct
dependencies (93354, 93978, 951A8, 85760, 84D00, 85158, 85418, 952B0,
95324, libgte/NormalClip all canonical) — G1 of the world-map completion
goal.
