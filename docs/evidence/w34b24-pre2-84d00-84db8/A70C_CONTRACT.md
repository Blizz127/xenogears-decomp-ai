# func_8004A70C — Retail Contract (RESOLVED: NormalClip)

## Identity

- `config/symbol_addrs.slus_006.64.txt`: `NormalClip = 0x8004A70C;`
- Resident SLUS_006.64 (load 0x80010000; file offset = VA − 0x80010000 + 0x800).
- Boundary `[0x8004A70C, 0x8004A730)`, 36 bytes / 9 instructions (padded
  with nops to 0x8004A73C where the next libgte routine begins).

## Retail disassembly

```
8004a70c  mtc2 $a0, SXY0        ; packed (y<<16)|(x&0xFFFF)
8004a710  mtc2 $a2, SXY2
8004a714  mtc2 $a1, SXY1
8004a718  nop
8004a71c  nop
8004a720  cop2 0x1400006        ; NCLIP
8004a724  mfc2 $v0, MAC0
8004a728  jr   $ra
8004a72c  nop
```

## Contract

```
s32 NormalClip(s32 sxy0, s32 sxy1, s32 sxy2)
  xi = (s16)(sxyi & 0xFFFF); yi = (s16)(sxyi >> 16)
  return MAC0 = x0*(y1 - y2) + x1*(y2 - y0) + x2*(y0 - y1)
```

- Twice the signed area of the 2D triangle (positive = counter-clockwise
  in screen convention). World code packs X in the low half and Z in the
  high half.
- **$a3 is not read.** The W34B22-I5 (94A5C) report listed "a3 = mode"
  from the call site; that is a caller-side register residue, not an
  argument. 84DB8's call sites pass only 3 args.
- Width: GTE MAC0 mathematically needs up to ~33 bits for s16 inputs
  (3 × 32768² ≈ 3.2e9); the register read wraps to 32 bits. A native
  implementation must use wrapping 32-bit accumulation (or s64 truncated
  to s32), never saturation.

## Port status / decision

- PsyCross exports a real `NormalClip` (strong `T` in xeno-port).
- Accepted `wm_80085760` already binds its three 0x8004A70C sites by the
  name `NormalClip`.
- `world_map_func_94a5c.c` still externs `func_8004A70C`, which the
  auto-stub satisfies with `return 0` — a live fidelity gap in 94A5C's
  complex dispatch (cases 5/6/9/10 always take the v==0 route natively).
- DECISION for the ladder: treat 0x8004A70C as CANONICAL via NormalClip.
  Before the 84DB8 rung: (a) rebind 94A5C's extern (bounded one-line
  integration fix + retest), (b) verify PsyCross NormalClip wrap behavior
  against the formula above (certificate vector: inputs forcing |result|
  > 2^31 must wrap, not saturate).
