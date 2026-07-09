# Active Frontier

> **Keep this page short.** Update when the real next gate changes.
> Canonical detail: [`ACTIVE_HANDOFF.md` — July 8 actor 20 var0408 / rand range`](https://github.com/Blizz127/xenogears-decomp-ai/blob/main/docs/ai_context/ACTIVE_HANDOFF.md)

## Current real frontier

**Map1 exit transition stalls because shared door-state variable `0x0408` never leaves state `1` on the PC port.**

The zone-11 exit route itself is solved through op54:

1. Fei reaches zone 11 (entrance 9, synthetic d-pad route) — **proven**
2. Opcode 203 inside path fires — **proven**
3. Opcode 7 allocates actor 18 routine 4 — **committed** (`9e1b667`)
4. Opcode 116 sound cue runs through shim — **committed** (`ae8c753`)
5. Opcode 54 sets local latch var `0x0462` — **proven**

After that, **no fade, map reload, or `func_800A5C40` is reached.**

## Root cause candidate (read-only, docs `8db4fb2`)

Actor 18 routine 1 branches on shared script var **`0x0408`**:

- `== 0` → initial path
- `== 1` → sleep/stop branch (current observed path)
- `== 2` → rotation animation branch
- fallback → alternate rotation path

**Owner of `0x0408` is actor 20 routine 1**, not actor 18. At field load, actor 20 immediately drives `0x0408` via opcode `0xA8` (`FieldScriptVMHandlerMulVariableWithRand`) expecting a value in **`0..4`**, then assigns `1` at IP `0x0997` when all compares miss.

On the PC port, **host `rand()` (glibc)** returns values far outside `0..32767`, so the random chooser always misses and **`0x0408` is forced to `1` every cycle**. Actor 18 therefore stays on the idle branch and never reaches transition-related state paths.

Retail expects PSX `rand()` with `RAND_MAX 32767` (`include/psyq/rand.h`, `src/slus_006.64/psyq/libc.c`).

## Next single action

**Route PC-port `rand()`/`srand()` to the PSX-compatible RNG (`0..32767`), then rerun the actor-20 / zone-11 trace.**

Do **not**:

- Patch actor scripts or force `0x0408`
- Port the full encounter/battle path for this transition hunt
- Implement `func_800A5C40` until the script-state gate is understood

## Quick repro context

```bash
distrobox enter xenogears-dev -- bash -lc 'cd /home/blizz/Projects/xenogears-decomp && \
  SDL_VIDEODRIVER=x11 XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 \
  XENO_FIELD_MAP=1 XENO_FIELD_ENTRANCE=9 XENO_FIELD_0BB_VRAM_UPLOAD=1 \
  ./pc_port/build_native/xeno-port'
```

Zone 11 route: `+X` (`D_800AFE9C=0x1000`) through frame 105, then `+Z` (`0x2000`). Zone 11 fires ~frame 134.

## What this is NOT

| Ruled out | Why |
|-----------|-----|
| Movement / height / XZ for zone 11 | Physically reached and height-valid |
| Opcode 7 slot allocation | Fixed in `9e1b667` |
| `func_800855C8` sound stub | Classified audio-side; shim allows op54 |
| `func_80079288` encounter stub | Per-frame encounter side work, not transition scheduler |
| Var `0x0462` consumer | Local latch on actor 48, not a fade trigger |
| Actor 18 routine 4 | Short door animation only; no fade/map-load opcodes |
