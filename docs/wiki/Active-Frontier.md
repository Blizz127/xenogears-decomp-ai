# Active Frontier

> **Keep this page short.** Update when the real next gate changes.
> Canonical detail: [`ACTIVE_HANDOFF.md` — July 9 map15 reload campaign entries](https://github.com/Blizz127/xenogears-decomp-ai/blob/main/docs/ai_context/ACTIVE_HANDOFF.md)

## Current real frontier

**Anim-script opcode `0xBC` sub-command `0x16` (move child to parent position) — Blocker.**

On map15's first post-load frame (117), the per-frame child-sprite pump (`func_800752C8` → TimerWorkListUpdate) reaches `0xBC` sub `0x16` and asserts. It is already **Decoded**: `jtbl_800185A8[0x16]` → asm `8002044C` — fetch the parent sprite via the `+0x70` back-link and snap the child's position words (`+0x0/+0x4/+0x8`) to the parent's position halfwords. A trivial add to the existing `dispatchIndex == 0x32` case in `src/slus_006.64/system/animation_scripts.c`; unlike subs `0x24`/`0x25` it reads the parent and must **not** touch the sticky bit, so it gets its own arm.

Runtime evidence: `captures/render_diag/map15_bc25_20260709_run1.log` (child #2 hits sub `0x16` on the same frame-117 tick chain after sub `0x25` cleared).

## How we got here (July 8–9)

The Map1 → Map15 reload path is now **Verified working** end-to-end up to mid-frame 117:

1. Zone-5 `CHANGE_FIELD` out of map1 → `func_800A5C40` + `FieldFree` — **Committed fix** (`4b377be`); heap reclaim **Verified working** after the `FieldFree` leak fix (`fe8933f`, verified `f48a4ad`)
2. `FIELDLOAD #2` map15 @ frame 116; load-time script VM completes — **Verified working** since the opcode `0x94` milestone (`a389755`): 3 type-2 child sprites spawn, tick, and finish their scripts (opcode chain `0xC6/0x96/0xFC/0xE0/0x8D` in `ce61f3d`, `0xF5/0xF6` in `11b2474`, `0xA3` in `f24e035`, `0xBC` sub `0x24` in `de23142`)
3. Frame 117 per-frame actor update `func_80084158` completes for all actors, including the live POLYCHECK miss-path test — **Committed fix** (`3858bd8`)
4. Per-frame child pump: `0xBC` subs `0x24`/`0x25` cleared (`de23142`, `ae30d53`) → **sub `0x16` is the gate**

See [Field Script VM and Opcodes](Field-Script-VM-and-Opcodes) for the opcode details and [Phase Log](Phase-Log) for the commit-by-commit trail.

## Next single action

**Add the sub-`0x16` arm to the `dispatchIndex == 0x32` case, rebuild, and rerun the zone-5 reload probe** (`captures/render_diag/map1_opcode_e0_reload_20260709.gdb`) plus the ent8/ent0/Map0 smokes, then adversarial asm-vs-C verification before commit — same loop as every July 9 pass.

Do **not**:

- Let sub `0x16` share the `0x24`/`0x25` sticky-bit prologue/tail blindly
- Implement the other ~36 `0xBC` subs speculatively — they assert loudly by design and surface one at a time
- Patch around the assert or fake child positions

## Queued behind it

- Possibly more `0xBC` subs as the child positioning preamble unwinds (each partially mapped via `jtbl_800185A8`) — **Decoded**
- `func_80025718` — type-2 render callback (`D_8004FD40[2]`) needed for child-sprite visibility — **Queued** (missing slots currently log once)
- misc8 select-target (`.L80084520`) and standing-on-top (`.L80084570`) machinery — deliberate asserts, **Unresolved**
- Frame-117 rendering not yet proven (abort happens mid-frame) — **Unresolved**
- Soft stubs newly reached, log-once and non-blocking: `func_8001B5E8`, `SoundFreeWdsEntry`, `func_8008E718` (load path); `func_80097954`, `func_8003A450`, `func_8008FB98` (frame 117)

## Quick repro context

```bash
distrobox enter xenogears-dev -- bash -lc 'cd /home/blizz/Projects/xenogears-decomp && \
  XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 \
  XENO_FIELD_MAP=1 XENO_FIELD_ENTRANCE=8 \
  ./pc_port/build_native/xeno-port'
```

Zone-5 one-shot gdb injection probe: `captures/render_diag/map1_opcode_e0_reload_20260709.gdb`. Map15 `FIELDLOAD #2` fires at frame 116; the assert lands on frame 117's child pump. Key logs: `captures/render_diag/map1_opcode_{fc,e0,8d,f5,a3,bc,94}_reload_20260709*.log`, `map15_polycheck_20260709_run3.log`, `map15_bc25_20260709_run1.log`.

## What this is NOT

| Ruled out | Why |
|-----------|-----|
| July 8 frontier: host `rand()` forcing shared var `0x0408` to `1` | No longer the frontier — bypassed, not fixed: the zone-5 probe drives `CHANGE_FIELD` via gdb-injected input; the `rand()` range mismatch itself is still open and still gates the natural door path (see [Current Status](Current-Status)) |
| `FieldLoad` hang on reload | Corrected diagnosis: was a `FieldFree` leak (wrong-width pointer reads + two skipped frees), fixed `fe8933f`, verified `f48a4ad` |
| Load-time script VM | Completes since `a389755`; all three child sprites finish their scripts |
| Work-list garbage-callback crash | `WorkListEntry` PSX 0x1C-byte layout fix landed in `ce61f3d` (see [Matching and Porting Rules](Matching-and-Porting-Rules)) |
| `func_80083288` POLYCHECK branch | Migrated in `3858bd8`; frame-117 actor update completes |
| Baseline smoke stubs (`func_80028B14` family) | `func_80028B14` joined the baseline smoke-stub family on July 9 (`7acb74a`); zero new stubs held across the opcode-chain passes (`ce61f3d` through `ae30d53`) |
