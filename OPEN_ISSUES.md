# Open Issues

Rules:

- If it's in this file, it's OPEN. Closing means DELETING the entry, not annotating it.
- Every item carries a reproducer command, not just a description.
- Every item carries `Last verified @ <commit>`. Stale verification = suspect claim.
- Every claim is tagged by evidence class:
  proven | build-verified only | observed | inferred
- For PSX symbol-width findings, classify the retail storage pattern before
  proposing a fix: direct data array, packed 32-bit pointer slot loaded/stored
  with `lw`/`sw`, or embedded array addressed with `lui`/`addiu`. Commit
  `1229ea1` is the reference implementation for all three forms.

---

## Sound cold-init is blocked below the decomp by hollow PsyCross SDK primitives

`src/slus_006.64/system/sound.c` is linked and all ten audited sound layouts are
retail-correct, but the port's field-test boot bypasses retail's
`SoundInitialize(0)` call. Consequently `D_800595D8`, the packed 32-bit current
audio-manager address, remains zero while field code calls the sound API.
Replacing the live oracle stubs with their exact-matched bodies therefore faults
on the first manager-element access. Retail has no null guard; adding one would
hide the missing initialization rather than restore it.

The current gate is below `SoundInitialize`: several PsyCross SDK symbols on
the successful cold-init path link cleanly but are implemented only as
`PSYX_UNIMPLEMENTED()` no-ops. **Symbol-resolved does not mean implemented.**
In particular, the earlier dependency classification called
`SpuSetReverbModeType` "real" because it was not in the generated stub manifest;
that was wrong. Its PsyCross body is a hollow implementation.

Cold-init reaches these hollow SDK primitives without taking an error path:

- `OpenEvent` (`pc_port/extern/PsyCross/src/psx/LIBAPI.C:140`) returns zero and
  retains no callback. Retail uses it at `SoundInitialize` `0x80037C84` to
  register `func_8003C020`.
- `EnableEvent` and `DisableEvent` (`LIBAPI.C:152-161`) are no-ops used by sound
  heap allocation and audio-manager list insertion.
- `SpuSetIRQCallback` (`LIBSPU.C:356`) and `SpuSetIRQ` (`LIBSPU.C:344`) are
  unconditional at `0x80037CCC` and `0x80037CD4`.
- `SpuSetCommonAttr` (`LIBSPU.C:191`) is reached synchronously through the
  cold-init `SoundSetCdAttr` call and later by the timer callback.
- `SpuSetReverbModeDepth` (`LIBSPU.C:236`) is unconditional in
  `func_800386C4`, and `SpuSetReverbModeType` (`LIBSPU.C:230`) is reached by
  the normal first reverb-allocation branch. Both are no-ops.

There is also a missing infrastructure layer rather than a single stub:
PsyCross's `SetRCnt`/`StartRCnt` record counter state, but there is no event pump
that invokes the counter-2 callback. Fixing `OpenEvent` alone would therefore
allow registration while `func_8003C020` still never fires.

This also blocks the three already exact-matched PsyQ leaves
`SpuGetReverbModeType`, `SpuSetReverbModeDelayTime`, and
`SpuSetReverbModeFeedback`: their retail behavior depends on reverb state that
the hollow type/depth functions never maintain. Exposing those three symbols
without the state layer would create a symbol-complete but behaviorally partial
initialization path—the same partial-init trap in a quieter form.

Retail calls `SoundInitialize(0)` from `func_80019578` at
`0x80019668-0x8001966C`. `SoundInitialize` spans
`0x80037B88-0x80037DBC` and stores `func_8003B148(0x10)` into
`D_800595D8` at `0x80037D50-0x80037D68`. Its complete known dependency
tree has four legs:

- **Manager allocation:** `func_8003B148` calls `func_8003B32C` at
  `0x8003B194`; that calls real `SoundInitializeAudioManager` at
  `0x8003B358`; the lower chain `SoundHeapFree`, `func_8003B930`, and
  `func_8003B32C` is now exact-matched. `func_8003B148` remains unported. Its
  allocation-failure exit also calls
  stubbed `SoundHandleError` at `0x8003B184`. That handler is not a leaf: when
  control flags `0x88` are clear, retail `0x8003F6E0-0x8003F724` reaches
  stubbed `SoundLoadWdsFile` and `func_80039E60` in addition to real
  `SoundSpuMemoryFreeBlock`, `SoundAddSedsEntry`, and `func_8003BDFC`.
- **CD mix:** `SoundInitialize` calls unported `func_800386C4` at
  `0x80037D04`; that path reaches unported `SoundSetupCdMix`.
- **Reverb:** real `SoundSetReverbModeWithAllocation` still reaches generated
  stubs for `SpuGetReverbModeType`, `SpuSetReverbModeDelayTime`, and
  `SpuSetReverbModeFeedback`; its symbol-resolved `SpuSetReverbModeType` and
  `SpuSetReverbModeDepth` dependencies are PsyCross no-ops, not real backends.
- **Timer tick:** `SoundInitialize` registers unported `func_8003C020` as the
  recurring callback at `0x80037C7C-0x80037C84`. Its sound-tick graph reaches
  at least `func_8003E900`, `func_8003AE84`, `func_8003A838`,
  `func_8003EBF0`, and `func_8003EB5C`, all still unported.

`SoundSpuMemoryAllocateBlockAtAddress` (`0x800395B8-0x800396DC`) is an
independently portable leaf: its only call is the real
`SoundSpuMemoryGetFreeBlock` at `0x80039678`.

The corrected sequencing is:

1. implement and behaviorally validate the PsyCross SDK integration layer
   (coherent reverb state, `SpuSetCommonAttr`, IRQ state/callbacks, and actual
   event/timer delivery);
2. exact-match the ten cold-init decomp functions;
3. route `SoundInitialize(0)` only as a diagnostic proof, verifying manager
   allocation, timer delivery, reverb-transfer completion, and zero stub hits;
4. then restore and extend the live playback path.

The SDK work has no objdiff oracle; validate it as host integration, like
`game_overrides.c` and the host walkers, using state/behavioral probes. Do not
resume the ten-function decomp or route boot on top of hollow primitives.
Earlier routing would construct a partially initialized manager and register a
non-delivered tick callback, turning a loud null dereference into quiet wrong
state. Four live playback helpers
(`func_8003A20C`, `func_8003A344`, `func_8003A450`, and `func_8003A55C`)
already have objdiff-`{}` C transcriptions, but they must remain oracle-stubbed
until initialization is complete.

Repro: `sed -n '140,245p' pc_port/extern/PsyCross/src/psx/LIBSPU.C` and
`sed -n '135,165p' pc_port/extern/PsyCross/src/psx/LIBAPI.C` show the
`PSYX_UNIMPLEMENTED()` happy-path bodies. `rg 'func_8003C020' pc_port/extern/PsyCross/src`
finds no counter/event delivery path. The original runtime repro remains:
locally replace `func_8003A20C` with its exact-matched C body, rebuild, and run
`timeout 20s ./scratchpad/run_map001.sh`; GDB faults with `D_800595D8 == 0`.
Restore the oracle stub afterward.
Evidence: proven
Last verified @ 609c426

### Scoping pass (design + pricing, no implementation) — dd0dbfe

Read-only design pass extending the map above. No showstopper found; the
project is large and multi-layer. Key facts established this pass:

**Host backend is REAL and can produce audio (not a showstopper).** The SPU
backend is `pc_port/extern/PsyCross/src/audio/PsyX_SPUAL.cpp` — an OpenAL SPU
emulation with the full playback path: voice attributes, key on/off, SPU-RAM
`Write`/`Read`, and reverb via OpenAL EFX effect slots
(`alAuxiliaryEffectSloti`, `g_nAlReverbEffect`). "No output path exists" is
false.  BUT it is entirely dormant: `g_spuInit` is 0 because `SpuInit()` /
`PsyX_SPUAL_InitSound()` is never called in the port boot, and every backend
function early-returns on `if (!g_spuInit)`. So even the *wired* PSYQ
primitives (`SpuSetReverb`, `SpuSetVoiceAttr`, `SpuSetKey`) currently no-op.

**Five distinct blockers, do not conflate:**
- **B0 backend device dormant:** `SpuInit`/`PsyX_SPUAL_InitSound` absent from
  the port boot (`grep` in `pc_port/src/*.c` finds no call). Prereq for
  everything below.
- **B1 cold-init not routed:** retail reaches `SoundInitialize(0)` at
  `func_80019578:0x80019668` (verified in the asm), followed by 4×
  `SoundLoadWdsFile`. `port_main.c`'s `main()` re-implements the other
  `func_80019578` boot duties (HeapInit, state reset, disc) but NOT
  `SoundInitialize`. **Menu-thread trap confirmed live: a perfect layer behind
  an uncalled init is silence.** Insertion point is known and bounded (the
  same `port_main.c` shim), but it is REQUIRED, first-class work.
- **B2 event pump missing (the core architectural gap):** `counters[3]`
  (LIBAPI.C) store value/target/cycle but nothing advances them or dispatches
  on target; `OpenEvent`/`EnableEvent`/`DisableEvent` are hollow (retain no
  callback). The sound tick `func_8003C020` is registered as a **counter-2**
  event and never fires. Leverage: `intrThreadMain` (PsyX_main.cpp) already
  fires `vsync_callback` at the NTSC/PAL timestep on a dedicated SDL thread —
  the pump is an *extension* (event-table registry in OpenEvent + counter-2
  advance/dispatch in that thread), not a from-scratch build.
- **B3 hollow primitives (7):** `SpuSetCommonAttr` (master/mix state),
  `SpuSetReverbModeType` + `SpuSetReverbModeDepth` + `SpuSetReverbModeParam` +
  `SpuSetReverbDepth` (reverb), `SpuSetIRQ` + `SpuSetIRQAddr` (SPU IRQ).
  Reverb is the state-coherence trap the entry above warns of: on/off is wired
  but type/depth are not, and `SpuGetReverbModeType`/`DelayTime`/`Feedback`
  read state the no-ops never set. Design: a shared `SpuReverbAttr`-backed
  state struct + a 10-entry PSX-mode → OpenAL-EFX-param table
  (`SPU_REV_MODE_OFF..PIPE` → AL_REVERB decay/density/gain). Attr structs are
  flat scalars / SPU-RAM offsets (not host pointers) → **low LP64 surface** for
  the primitive layer; the LP64 work already lives in sound.c's 10 audited
  layouts (done).
- **B4 cold-init decomp unported:** `SoundInitialize` and its legs
  (`func_8003B148`, `func_800386C4`, `func_8003C020`, `SoundSetupCdMix`,
  `func_80039E60`, `func_8003E900/AE84/A838/EBF0/EB5C`, `SoundHandleError`) are
  all `INCLUDE_ASM` (sound.c has 132 unported functions total). Unlike the SDK
  layer, these ARE MIPS asm with an objdiff `{}` oracle — normal matched-decomp
  work, not host design.
- **B5 sample data:** `SoundLoadWdsFile` is stubbed — no sound banks load, so
  audible output is gated behind the playback path even after B0-B4.

**Behavioral probe suite (no objdiff oracle for B0-B3; "links != works"):**
1. `func_8003C020` tick actually FIRES — counter/log in the tick; expect
   ~counter-2-rate hits, not zero.
2. Reverb state round-trips — set type/depth then `SpuGetReverbModeType`/etc.
   return the set values (coherence, not defaults).
3. Zero stub hits on the `SoundInitialize` happy path — stub-hit log during
   init must be empty.
4. `D_800595D8` becomes non-zero AND the manager stays coherent (not the
   hollow-init trap: re-read manager fields after init).
5. `g_spuInit == 1` and an actual sample reaches OpenAL — probe backend voice
   activity / AL source state.

**Pricing (phased):**
- Phase 0 — route `SpuInit` at PsyX startup, set `g_spuInit`. Small (~1 call +
  device-init verification).
- Phase 1 — event pump: OpenEvent/Enable/Disable registry + counter-2 dispatch
  in `intrThreadMain`. Medium; bounded by existing infra. No oracle → probe 1.
- Phase 2 — primitive layer: wire 7 hollow primitives + reverb state/EFX table.
  Medium (~7 funcs + 10-entry table). No oracle → probes 2, 5.
- Phase 3 — cold-init decomp: `SoundInitialize` + ~12 named legs + transitive
  deps (est. 20-40 funcs). Large, BUT objdiff `{}` oracle exists.
- Phase 4 — route `SoundInitialize(0)` into `port_main.c` boot. Small.
- Phase 5 — behavioral validation (probes 1-5). Small-medium.
- Phase 6 (separate follow-on) — playback data: `SoundLoadWdsFile` + WDS load +
  the ~132-function playback engine. Large.

Milestone framing: "cold-init proven coherent, timer fires, zero stub hits"
(the entry's phase-3 diagnostic goal) = Phases 0-2 + 4-5 + enough of Phase 3 to
run `SoundInitialize`. "Audible in-game music" = all phases incl. B5/Phase 6.

**Recommendation: FUND, scoped to the SDK-integration milestone (Phases 0-2,
4-5), not the full engine.** Rationale: (1) no showstopper — backend is real,
init is routable; (2) B2 (event pump) is the single highest-leverage bounded
piece — it's reusable infrastructure and the one true "architecture, not a
stub" gap; (3) the SDK layer (B0-B3) is the no-oracle work that everything
above it needs and that only this design pass has de-risked; (4) defer B4
(has an oracle — normal decomp, can proceed independently) and B5/Phase 6
(playback) as follow-ons. Set expectations: audible sound is several phases
out; the fundable near-term win is a *behaviorally-proven initialized sound
subsystem* (manager coherent, timer delivering, reverb state live, zero stubs),
which converts "silent and uninitialized" into "initialized and ticking" — the
prerequisite state for all later audio work.
Evidence: proven (backend/boot/counter code read; init-absence grep-confirmed;
retail SoundInitialize call site verified in func_80019578 asm)
Last verified @ dd0dbfe

### Phase 0 + Phase 1 landed (backend awake + counter-2 event pump)

Implemented Phase 0 (B0) and Phase 1 (B2) of the plan above; the SPU subsystem
is now awake and the event pump ticks. Behaviorally validated (no objdiff
oracle for this layer) via a synthetic-callback probe, deliberately independent
of SoundInitialize (still B1/B4, not touched).

- **Phase 0 (B0):** `port_main.c` now calls `SpuInit()` after `PsyX_Initialise`
  (idempotent, before pad init). Confirmed `g_spuInit == 1` (GDB read of the
  real static + the new `PsyX_SPUAL_IsInit()` accessor), OpenAL device opens
  ("found sound device: OpenAL Soft", "PSX SPU effects ... initialized"). The
  backend early-return gate is now open; wired primitives no longer no-op.
- **Phase 1 rate:** derived + cited, NOT assumed. Retail `SoundInitialize`
  (0x80037C84) does `OpenEvent(RCntCNT2, EvSpINT, EvMdINTR, func_8003C020)` then
  `SetRCnt(RCntCNT2, 0x44E8, EvMdINTR)` + `StartRCnt`. Mode `0x1000` has no
  `RCntMdSC`/0x1 bit, so RCnt2 uses its default clock = system clock / 8 =
  33.8688MHz/8 = 4.2336MHz (the port's own `SetRCnt` confirms via the
  `spec==2 && !(mode&1)` branch). Target 0x44E8 (17640): 4233600/17640 = **240.0
  Hz exactly** — 4x the 60Hz vblank, so it runs on its own cadence.
- **Phase 1 registry + dispatch:** `OpenEvent`/`EnableEvent`/`DisableEvent`/
  `CloseEvent` in `LIBAPI.C` are now a real lock-free event table (was
  `PSYX_UNIMPLEMENTED` no-ops). `intrThreadMain` (PsyX_main.cpp) gained a second
  HPC timer that calls `PsyX_Sys_DispatchCounter2()` at 240Hz, dispatching
  enabled RCntCNT2/EvSpINT events — extending the existing vblank tick, not a
  new loop.
- **Synthetic probe (the pass/fail):** `XENO_SOUND_PUMP_PROBE=1` registers a
  counting callback via the real `OpenEvent` on the counter-2 event, enables it,
  and measures dispatch. Result: **PASS** — 120 ticks / 500ms = 240 Hz when
  enabled, +0 ticks over 200ms after `DisableEvent`, handle > 0. This proves the
  pump architecture (registry + enable-gating + 240Hz dispatch + disable)
  independent of SoundInitialize. When Phase 3/4 land the real func_8003C020
  registration, it flows through this identical, proven path.
- **NOT this pass (unchanged):** SoundInitialize routing/decomp (B1/B4), the 7
  hollow primitives (B3), WDS/playback (B5). The real `func_8003C020` does NOT
  run yet (nothing registers it until Phase 3/4) — the "func_8003C020 fires"
  probe belongs to that later milestone, not here.
- **Deferred concern:** cross-thread safety of a callback *body* against the
  main/field thread (retail used EnterCriticalSection to disable IRQs; the
  port's is a no-op). The synthetic probe touches no shared state; this must be
  addressed before the real sound tick runs concurrently (Phase 2+).

Persistence: PsyCross is a gitignored vendored tree, so the LIBAPI.C /
PsyX_main.* / PsyX_SPUAL.* changes live in `pc_port/patches/psycross_sound_pump.patch`
(marker `_xeno_sound_pump`), applied idempotently by `build_port.sh`.
Map014 tripwire boots clean (52 actors, reaches field main loop) with the pump
active on the interrupt thread.
Repro: `env XENO_SOUND_PUMP_PROBE=1 XENO_FIELD_TEST=1 XENO_FIELD_MAP=5 ...
pc_port/build_native/xeno-port` -> `[sound-probe] ... RESULT: ... PASS`.
Evidence: proven (synthetic probe PASS; g_spuInit confirmed; rate derived from
retail SetRCnt; tripwire clean)
Last verified @ 28f12e4

### Phase 3 call-tree measurement (diagnostic, no decomp) — f79d131

Measured the real transitive call tree under `SoundInitialize` by BFS over the
split asm (`asm/slus_006.64/nonmatchings/system/sound/*.s`) PLUS the C-body
calls of already-decomped functions (so undone functions hidden *below* done
ones are not missed), tagging every node oracle-decomp / already-done / SDK-real
/ SDK-hollow. Method + counts: `scratchpad/soundtree/`.

**Headline: NOT a menu-overlay-style surprise. The 20-40 name estimate is
accurate for the FULL init+tick scope (~23), but it hid a cheap init-proof
sub-milestone (~3-4 functions).** Most of the sound engine is already
decompiled (sound.c has ~94 real C bodies; 40 of the ~76 nodes in the
SoundInitialize tree are DONE), so the remaining work is small and splits
cleanly:

- **INIT happy-path decomp: 3 functions** (+ `SoundInitialize` itself) —
  `func_8003B148` (manager alloc, 50 asm lines; its lower chain
  SoundHeapFree/func_8003B930/func_8003B32C/SoundInitializeAudioManager is
  already {}), `SoundSpuMemoryAllocateBlockAtAddress` (83 lines, a leaf — only
  calls the real SoundSpuMemoryGetFreeBlock), `SoundSetupCdMix`. All small,
  all with an objdiff {} oracle. This is the *near-term* Phase-3 number, far
  below "20-40".
- **INIT full decomp (incl error + CD-mix legs): 13** — adds the error leg
  (`SoundHandleError → SoundLoadWdsFile, func_80039E60, func_8003A65C,
  func_8003B644, func_8003BDFC, func_8003E5BC, func_80039024`), which fires
  only on allocation failure (stub-first, deferrable), and CD-mix (`func_800386C4`,
  gated on control bit 0x4000 — optional at cold init).
- **TIMER-TICK leg decomp: 20** — `func_8003C020` (the 240Hz callback, run by
  the Phase-1 pump) reaches `func_8003E900/AE84/A838/EBF0/EB5C/EEA0/EFE4/
  C4C4/C6E8/CC84` + shared spu-mem/error funcs. This is the real bulk — needed
  for *ongoing audible* sound, but SEPARABLE: the tick body can stay stubbed
  while the init proof is validated (the pump already fires the callback).
- **Union (init-full + tick): 23 oracle-decomp functions** — within the 20-40
  estimate. So the estimate was right for "fully working sound," just not
  broken out by milestone.
- **SDK-HOLLOW (Phase 2, no oracle): 6 touch init** — `SpuSetCommonAttr`,
  `SpuSetReverbModeType`, `SpuSetReverbModeDepth`, `SpuSetIRQ`,
  `SpuSetIRQCallback`, `SpuReadDecodedData`. These are Phase-2 primitive-wiring,
  NOT Phase-3 decomp; do not price them into the decomp count.
- **Unclassified/minor:** `SoundTransferCallbackStore` (no .s, no C body, no
  macro — one node, likely a small helper); `FILE_SIGNATURE` is a macro (noise).
- **No sprawl beyond sound.c** — the tree stays inside the sound engine + the
  SDK primitives; zero genuine other-TU-undone game functions (the earlier
  "OTHER-TU" bucket was done C bodies + libc/macro false positives).

**Re-priced recommendation / sequencing:** the near-term *init-proof* milestone
("SoundInitialize completes, manager coherent, timer fires, reverb state live,
zero stubs on the init happy path") costs only **~3-4 small decomp functions +
6 Phase-2 primitives** — much cheaper than the "20-40" implied. Do Phase 2
(wire the 6 primitives) and the 3-4 init decomp together to reach it; stub the
error leg and defer the 20-function tick behind it. The tick leg is the real
bulk and is where the "20-40" mostly lives — deferrable, incremental, oracle'd.
Net: fund the init-proof milestone next (small, bounded); the full audible-sound
project is ~23 decomp + 6 primitives + WDS/playback (B5), sequenced after.
Evidence: proven (BFS over split asm + C-body edges; sizes from asm line counts;
already-done set cross-checked against sound.c C bodies)
Last verified @ f79d131

### Phase 2 landed (SDK primitives wired + behaviorally validated) + measurement corrections

Wired the init-reached hollow SDK primitives against the now-awake backend and
validated them behaviorally (no objdiff oracle for this layer). Shipped as
`pc_port/patches/psycross_sound_prims.patch` (marker `_xeno_sound_prims`,
applied idempotently by `build_port.sh`; reverse-and-rebuild verified). Two
measurement corrections from the Phase-3 BFS fall out of this pass.

- **Correction 1 — init-reached hollow primitives are 5, not 6.**
  `SpuReadDecodedData` is NOT on the init happy path: its only callers are the
  SPU-command handler at `sound.c:1403-1414` (`pCmd->pSpuData`, the tick/command
  leg), not `SoundInitialize`'s tree. It stays hollow, deferred to the tick leg.
  The 5 that DO touch init (verified by caller-trace): `SpuSetIRQ` +
  `SpuSetIRQCallback` (direct `SoundInitialize` jals), `SpuSetCommonAttr` (via
  `SoundSetCdAttr`, unconditional), `SpuSetReverbModeType` (via
  `SoundSetReverbModeWithAllocation`), `SpuSetReverbModeDepth` (via unconditional
  `func_800386C4` and `SoundSetReverbModeWithAllocation`).
- **Correction 2 — `func_800386C4` is UNCONDITIONAL, so the init-proof decomp
  count is 4, not 3.** The Phase-3 note bucketed `func_800386C4` as the optional
  CD-mix leg, but `SoundInitialize.s` calls it with a plain `jal` (no guard); it
  is the volume/reverb-apply leg. The *thing it optionally calls* is
  `SoundSetupCdMix` (gated on control bit `0x4000`, skipped at cold init).
  `SoundHandleError` is the allocation-failure error leg (skipped on success).
  So the init happy-path decomp set is exactly **4**: `SoundInitialize`,
  `func_8003B148`, `SoundSpuMemoryAllocateBlockAtAddress`, `func_800386C4`.

**What was wired (behavioral, NOT objdiff-matched — claimed as probe results):**
- Shared `SpuReverbAttr`/`SpuCommonAttr` state in `LIBSPU.C` so `Set` and `Get`
  round-trip. `SpuSetReverbModeType`/`Depth`/`ModeParam` write it;
  `SpuGetReverbModeParam` reads it back. A 10-entry PSX-mode → OpenAL-EFX preset
  table (`SPU_REV_MODE_OFF..PIPE` → gain/decay/gainHF) drives the real reverb
  effect via a new backend hook `PsyX_SPUAL_ApplyReverbParams` (gated on
  `g_spuInit` + `g_ALEffectsSupported`, re-commits `g_nAlReverbEffect` into the
  active aux slot).
- `SpuSetCommonAttr` merges per-mask into the shared common-attr state and, when
  `MVOLL/R` is set, drives master output via new backend hook
  `PsyX_SPUAL_SetMasterVolume` (PSX 14-bit master vol → OpenAL listener gain).
- `SpuSetIRQ`/`SpuSetIRQCallback` maintain enable state + registered callback and
  return PsyQ-correct values (IRQ returns arg; Callback returns previous). **No
  SPU IRQ source exists in the port backend**, so the callback is registered but
  never fired. **This is the pre-tick-leg gate** (boundary flagged): SPU-IRQ /
  streaming sync belongs with the tick leg, not init, and cross-thread callback-
  body safety (retail's EnterCriticalSection IRQ-disable is a port no-op) must be
  resolved before any real tick body runs concurrently.

**Behavioral probe — the pass/fail (`XENO_SOUND_PRIM_PROBE=1`, `port_main.c`):**
Exercises the primitives in isolation against the awake backend (as the Phase-1
pump probe validates the pump, independent of `SoundInitialize`). Result:
**PASS** — reverb round-trip `mode=HALL, depthL=0x4000, depthR=0x5000` read back
exactly via `SpuGetReverbModeParam`; `SpuSetCommonAttr` master vol `0x2000` →
listener gain `0.500`; `SpuSetIRQ(ON)` returns ON and the callback register
round-trips (`old1==NULL, old2==cb1`). Backend confirmed EFX-capable at run
("PSX SPU effects are supported and initialized"), so reverb/master calls drove
real OpenAL, not just state. The Phase-0+1 pump probe still PASSes on the same
binary (240Hz, +0 after disable) — no regression.

**NOT this pass (the remaining init-proof work, now precisely scoped):**
- **Phase 3 (4 decomps):** `SoundInitialize`, `func_8003B148`,
  `SoundSpuMemoryAllocateBlockAtAddress`, `func_800386C4` to objdiff `{}` (or the
  d88f13c coexistence pattern if codegen resists — behavior first, `{}` as the
  quality gate). Their lower chains are already `{}`. `func_800386C4` is a
  mode-switch flag machine + the `D_80059518`-gated `func_80038824` leg;
  `SoundSpuMemoryAllocateBlockAtAddress` is a block-search allocator loop.
- **Phase 4:** route `SoundInitialize(0)` into `port_main.c` boot (same shim
  family as the other `func_80019578` duties), keeping `func_8003C020`'s tick
  body stubbed.
- **Phase 5:** init-proof validation — init completes with zero stub hits on the
  happy path, `D_800595D8` non-zero AND manager fields read back coherent (not
  the hollow-init trap), real `func_8003C020` registered and firing at 240Hz
  through the Phase-1 pump (body stubbed).

Persistence: `pc_port/patches/psycross_sound_prims.patch` (LIBSPU.C +
PsyX_SPUAL.cpp/.h), wired into `build_port.sh` after the pump patch. Idempotent
(marker-gated apply; verified: applies from a clean baseline, skips on re-run,
build stays 223 stubs / LINK OK).
Evidence: proven (behavioral probe PASS on the patch-built binary; all 7 wired
symbols resolve to real `T` defs, zero stubbed; caller-trace for the 5-vs-6 and
unconditional-`func_800386C4` corrections from `SoundInitialize.s` +
`func_800386C4.s` + `sound.c`)
Last verified @ 74fe427

### Phase 3-5 landed: cold-init decomped, routed, and behaviorally coherent

The sound subsystem is now **initialized and coherent** in the port: the game's
own `SoundInitialize(0)` is decompiled, routed into boot, and validated to build
a coherent audio manager with the 240Hz tick firing (body still stubbed). This
resolves the top-of-file "sound cold-init blocked" issue for the init milestone
(playback/WDS remain, below).

**The 4 init happy-path decomps** (corrected count from the Phase-2 note):
- `func_8003B148` (audio-manager alloc + per-element voice assignment loop) --
  **objdiff `{}`**.
- `SoundInitialize` (the ~40-store/call top-level init sequence) -- **objdiff
  `{}`** (packed manager pointer via `SOUND_PTR_TO_PSX`; 6 previously-undeclared
  globals + 3 callbacks now declared in sound.h).
- `SoundSpuMemoryAllocateBlockAtAddress` (block-search allocator) --
  **coexistence** (d88f13c). Logic traced 1:1; residual is a gcc-2.7.2
  register-coloring inversion (`addr`->s0 vs retail's s1, driven by `addr` being
  live across `SoundSpuMemoryGetFreeBlock`) + base-pointer caching. Not `{}`.
- `func_800386C4` (reverb-mode flag machine) -- **coexistence**. Logic traced
  1:1; residual is the switch `expand_case` decision tree (retail linear-from-low
  + tail-merged store vs gcc range-split); no switch/if-else variant reproduces
  it. Not `{}`. Its `D_80059518` echo-controller leg is dead at cold init.

`SoundSetupCdMix` (0x4000-gated) and the `SoundHandleError` error leg stay
stubbed/deferred as designed. Matching build green; the 2 `{}` decomps match and
the 2 coexistence functions stay byte-exact via `INCLUDE_ASM`. Pre-existing
`func_8003B32C` mismatch (struct `unk_0x18` width) is unrelated and untouched.

**Port integration (the func_80019578 duty port_main did not replicate):**
`SoundInitialize(0)` routed in `port_main.c` after `SpuInit`, before pad init.
Data-symbol sizing added to `symbol_addrs` for the new .bss the init path now
touches: the sound heap `D_80065B0C` (0x6300) and `g_SoundSpuMemoryTableStart`
(0x24). **Fixed an LP64 landmine:** `SoundClearVoiceDataPointers` strides by
`sizeof(SoundVoiceData*)` (8 on the port vs 4 retail) to offset 184, but
`g_SoundChannels` was stub-sized at the 32-byte default -> the clear overflowed
into the adjacent `g_SoundHeapHead` stub and zeroed it, crashing the second
`SoundHeapAllocate`. Sized `g_SoundChannels` to 0xC0 (24 x 8-byte pointers).

**Reverb round-trip completed:** the init path reaches `SpuGetReverbModeType` /
`SpuSetReverbModeDelayTime` / `SpuSetReverbModeFeedback` via
`SoundSetReverbModeWithAllocation` -- these were absent from PsyCross (stubbed).
Wired them onto the shared `s_reverbAttr` state (extends the prims patch), so the
init happy path hits **zero stubs**.

**Init-proof validation (`XENO_SOUND_INIT_PROBE`, all PASS):**
- Init completes; **zero stub hits on the synchronous happy path** (the
  `func_8003Axxx`/`func_80098430` stubs seen in a GDB window have no sound-tree
  callers -- they are async interrupt/field-thread work, not init).
- Manager coherent (fields read back, not just non-null): `D_800595D8`=0x7e0eb0,
  `elementCount`=0x10, `unk_Flags`=2, `unk_0x18`=0x7F, `unk_0x32`=1, `unk_0x38`=4.
- Real `func_8003C020` **registered** (`g_unk_SoundEvent`=1, callback resolved)
  and **firing at 240Hz** (post-init shadow counter: 120 ticks/500ms), body still
  stubbed -- the graduated tick probe.
- Reverb round-trip + pump + prim probes still PASS; Map014/47/334 tripwires
  boot clean (52/36/40 actors, reach field main loop, no crash) with init routed.

**Gate flagged (not forced):** SPU IRQ has no source in the port and
`EnterCriticalSection` is a no-op; safe while the tick body is stubbed (dispatched
stub touches no shared state). Real tick body + cross-thread/IRQ gating are the
gate before the tick leg. WDS/playback (`SoundLoadWdsFile`) NOT routed.
Evidence: proven (2 objdiff `{}` + 2 coexistence logic-verified; init-proof probe
suite PASS on the built port; tripwires clean; matching build green)
Last verified @ cfd96fb

### Tick-leg scoping (trace + gating design, NO implementation)

Read-only depth-trace of `func_8003C020`'s subtree + the cross-thread gating
design that must land before any real tick body runs. Method: BFS over the split
asm + C-body edges + the `g_SoundScriptHandlers` jalr table.

**(A) The ~20 estimate is a ~3.75x undercount: 75 unported functions.** BFS from
`func_8003C020` (following the `func_8003C6E8 -> g_SoundScriptHandlers[opcode]`
jalr table) reaches **75 unported** functions, not ~20. The estimate captured the
control layer but missed the entire data-dispatched sequence-command layer. Split:
- **Tick-core control: 12 unported** -- the empty-queue tick. Every-tick:
  `func_8003E900`, `func_8003EB5C`. Manager-active-conditional: `func_8003C4C4`,
  `func_8003C6E8`, `func_8003EFE4`, `func_8003EBF0`. Lifecycle-conditional:
  `func_8003AE84`, `func_8003A838`. Plus transitive `func_8003CC84` (<-C6E8),
  `func_8003EEA0` (<-EBF0), `func_8003E5BC`, and `func_8003C020` itself.
- **Sequence-command handlers: 51 unported** (of 97 distinct in the 128-slot
  `g_SoundScriptHandlers` table; 46 already `{}`, 31 slots are
  `SoundScriptDefaultHandler`). Statically ENUMERABLE from the table (not
  "untraceable") but only *invoked* when the tick processes real sequence data
  (needs WDS/B5) -- with an empty queue the tick hits Default/Nop handlers only.
- Handler-subtree helpers: ~4 more. Error/WDS leg (`SoundHandleError`,
  `SoundLoadWdsFile`, `func_80039024/39E60/3A65C/3B644/3BDFC`,
  `SoundSpuMemoryAllocateBlock`): 8, deferrable (folds into B5).
- **Runtime-trace-required (NOT statically enumerable):** `func_8003EFE4`'s
  second jalr is `obj->field0(obj)` -- a per-voice method pointer set at runtime
  (an ADSR/envelope state handler). Its handler set must be runtime-traced; it
  may add to the 75 if those handlers aren't otherwise reached.

**(B) Cross-thread gating design (the prerequisite; SPU-IRQ resolved).**
- *Shared state* the tick body touches: `g_SoundAudioManagerListHead` (manager
  linked list), `g_SoundVolumeController`, `g_SoundControlFlags`,
  `g_SoundCdFadeFramesRemaining`, `D_80059504/40/5C/C4`, and via the subtree the
  voice tables (`g_SoundChannels`), manager fields, and SPU-RAM (SpuSetVoiceAttr/
  SpuSetKey in handlers). The field/main thread touches the same via the sound
  API (SoundReset, heap alloc, manager add/remove, volume/reverb, playback ctl).
- *Retail's primitive:* `func_8003C020` does NOT gate itself -- it runs as the
  counter-2 IRQ handler (atomic on HW). The MAIN thread brackets every non-atomic
  shared-state access with `DisableEvent/EnableEvent(g_unk_SoundEvent)`
  (SoundHeapAllocate, SoundAddAudioManagerToList, etc.).
- *Port design:* map that discipline onto a **recursive** `g_SoundTickMutex`.
  `DisableEvent`/`EnableEvent` on the counter-2/EvSpINT event acquire/release it;
  the pump (`intrThreadMain`) holds it around the `func_8003C020` dispatch (today
  it dispatches under `g_intrMutex` but the main thread only flips the `enabled`
  flag -- no exclusion vs an in-flight tick). **Must be recursive**:
  `func_8003AE84` (on the tick path) re-enters `DisableEvent/EnableEvent`
  (SoundHeap*/EnterCriticalSection are NOT reached from the tick, so that is the
  only re-entry vector). SDL_CreateMutex is reentrant -- verify at impl. No
  deadlock (short critical sections, recursive lock); no stall (tick + sections
  are us-scale). NOT a showstopper -- the pump is already a dedicated thread and
  retail already brackets the accesses; no restructuring needed, tick body
  unchanged.
- *SPU-IRQ question RESOLVED:* the tick is the RCnt2 240Hz **timer** (already
  firing via the pump). `SoundSpuIRQHandler` is a SEPARATE SPU-IRQ path
  (streaming/transfer-completion for WDS/XA). **The tick leg does NOT need an SPU
  IRQ source -- that gap defers to B5, not here.**
- *Concurrency validation (the third regime):* (1) TSan build (-fsanitize=thread)
  over the sound TUs + PsyCross, boot + stress -> reports races directly;
  (2) stress probe: main thread hammers the sound API while the real tick fires
  at 240Hz, with invariant assertions (manager list acyclic/terminated, voice
  indices in [0,24), no manager use-after-free); (3) any un-bracketed multi-word
  access TSan flags is a pre-existing retail race to review.

**(C) Sequence (gate-first, leaf-up) + pricing.** NOT one pass.
1. **Gating pass FIRST** (its own bounded pass): recursive `g_SoundTickMutex` +
   wire DisableEvent/EnableEvent + pump dispatch; validate TSan + stress with the
   tick still STUBBED. Behavioral + concurrency, no oracle. ~1 pass.
2. **Tick-core (12)**, leaf-up: `func_8003E900/EB5C` -> conditional legs
   (`func_8003C4C4/C6E8/EFE4/EBF0`) -> `func_8003AE84/A838` -> `func_8003C020`
   last. objdiff `{}`. ~1-2 passes. Milestone: empty-queue tick runs safely.
3. **Script handlers (51)** batched, objdiff `{}`; enables sequence processing.
   ~2-4 passes. Audible needs B5 (samples) too.
4. **Error/WDS (8) + B5** (SoundLoadWdsFile, SPU streaming, sample banks): the
   large separate "audible" project.
Total tick decomp ~= **63 functions** (12 + 51) to `{}` + the gate-first pass,
vs the ~20 estimate. Recommend: fund gate-first, then tick-core, then handlers.
Evidence: proven (BFS over split asm + C-body + g_SoundScriptHandlers table
edges; gating traced from func_8003C020.s + retail DisableEvent discipline;
SPU-IRQ role read from SoundSpuIRQHandler)
Last verified @ 2ffd372

### Sound tick gate LANDED (tick-leg step 1; tick body still stubbed)

The gating pass above is implemented: `psycross_sound_gate.patch` adds
`g_SoundTickMutex` (SDL recursive mutex, created before the interrupt thread
spawns). `DisableEvent` on the counter-2/EvSpINT tick event acquires AND HOLDS
it (per-thread `__thread` depth); `EnableEvent` releases one level; unpaired
enables (func_80037F44 boot toggle) just flip the flag; `CloseEvent` dissolves
the closing thread's held levels (both port probes Disable -> Close without an
Enable). The 240Hz pump TRY-locks around dispatch: a held bracket DROPS the
tick (retail's disabled-event semantics), and TryLock makes pump stall and
lock-order deadlock structurally impossible. All registry mutations run under
the mutex. func_8003C020's body REMAINS STUBBED -- the mechanism is validated
before any real body can race.

Validation (the concurrency regime, XENO_TSAN=1 + XENO_SOUND_GATE_STRESS=1):
- Gate-stress probe PASS (normal + TSan builds): gated pump 120 ticks/500ms
  (240Hz exact); held bracket = +0 ticks over 200ms while vblank advances
  (pump thread alive); 1.82M main-thread bracket pairs vs 464 ticks with ZERO
  torn multi-word reads on either side; 632 func_8003AE84-style re-entrant
  brackets on the tick path, no self-deadlock; post-CloseEvent dispatch
  resumes (no leaked hold).
- TSan (probe suite + Map001 field boot): ZERO data races on sound shared
  state. Remaining instrumented-code reports are pre-existing PsyX infra
  races to review later: g_psxSysCounters vblank counter (PsyX_main.cpp:190
  vs :165), LIBETC.C:27 ResetCallback registration, PsyX_Shutdown teardown.
- Regression: pump/prim/init probes PASS; manager coherent; five-map
  watchdogs (0/1/14/47/334) boot to field main loop, field-diag tripwire
  lines addr-normalized EXACT vs prior baselines.
- Patch idempotent: reverse-apply clean, build re-applies, rerun no-op.
Honest state: GATING VALIDATED, tick body stubbed, no real tick body yet.
Next: tick-core decomp (12 fns, leaf-up, func_8003C020 last) under the gate.
Evidence: proven (gate-stress + TSan runs; five-map smokes; idempotency cycle)
Last verified @ HEAD of this commit

### Tick-core REAL (tick-leg step 2; dispatching at 240Hz into the proven gate)

The 12 tick-core control-layer functions are decompiled and live, leaf-up,
func_8003C020 last. Matching split: **6 objdiff {}** (func_8003EEA0/A838/
EB5C/E900/AE84/CC84 -- E900 via the volatile SPU register-pair cursor + merged
live-range idioms) and **6 coexistence** (d88f13c pattern: func_8003E5BC/C4C4/
EFE4/EBF0/C6E8/C020). The coexistence residual is ONE uniform class: this
pipeline's cc1 strength-reduces the element-cursor giv family onto a different
anchor register and fills load-latency slack the retail object leaves
unfilled -- same operations at the same absolute element offsets (verified
per-diff); INCLUDE_ASM keeps the matching build byte-exact. Matching build
proven byte-identical: built slus_006.64 hashes EQUAL with and without the
whole change set (A/B rebuild).

Port integration:
- g_pSoundSpuRegisters (retail .sdata -> 0x1F801C00) was a NULL auto-stub --
  safe only while the tick was stubbed. Now backed by a static SpuUnion page
  in sound.c (port-only); the real tick's register writes (key on/off, ADSR,
  pitch) land in real memory, faithfully maintained but not yet wired to the
  OpenAL backend (that is the B5/WDS leg).
- LANDMINE FOUND + FIXED (the SoundClearVoiceDataPointers class): FieldLoad's
  walkmesh LZSS decompress writes ~0x220 bytes into D_800658DC, a default
  32-byte data stub -- every field load silently trampled the neighbouring
  stubs including the sound heap D_80065B0C. Harmless while nothing read the
  heap; the real tick crashed on the corrupted manager list (caught by
  hardware watchpoint). Fixed by sizing D_800658DC to its retail 0x230 in
  symbol_addrs (stub generator honours size:).
- func_8003EFE4's per-voice envelope method pointer stays SoundPsxAddress
  (LP64-safe); its jalr is unreached until the envelope setters land (step 3
  runtime-trace set). g_SoundScriptHandlers dispatch is compiled but
  unreached in-port (no sequence data loaded -- elements stay inactive), so
  the handler table stays a stub until step 3 provides host routing.

Validation (dual regime):
- Matching: 12/12 snddiff EXACT (6 real, 6 via INCLUDE_ASM); whole-object
  instruction diff clean modulo reloc-vs-addend encodings that link
  identically; slus binary A/B hash-identical.
- Live tick: real func_8003C020 dispatching at 240Hz (120 ticks/500ms),
  manager coherent after ticks; pump/prim/init/gate-stress probes all PASS
  with the real body (1.81M bracket pairs vs 462 ticks, zero torn reads,
  630 re-entrant brackets).
- TSan (real bodies): probes + Map001 field boot -- ZERO races in sound.c;
  only the two catalogued pre-existing PsyX infra races (vblank counter,
  shutdown) remain.
- Five-map watchdogs (0/1/14/47/334) boot clean with the real tick running
  during field play; field-diag tripwire lines addr-normalized EXACT.
Honest state: tick core real, dispatching at 240Hz into the proven gate,
TSan-clean; script handlers + envelope setters + WDS still stubbed -- the
tick processes an empty queue, NOT YET AUDIBLE. Next: step 3 (51 script
handlers + host handler-table routing), then error/WDS + B5 for audible.
Evidence: proven (snddiff x12 + A/B binary hash; live-tick probes; TSan;
five-map tripwires; watchpoint root-cause on the stub overrun)
Last verified @ HEAD of this commit

### Handler layer batch 1 + host dispatch table (tick-leg step 3, partial)

The host g_SoundScriptHandlers table is LIVE: 128 entries in retail slot
order rebuilt as host-width function pointers (port-only; matching keeps the
.sdata original). Batch 1 of the 51 unported handlers landed: **12 objdiff {}
(oracle-confirmed fuzzy=100)** -- func_8003CE9C/CD54/DEB4/D370/D3A4/D884/
E4BC/D034/DAB0/DE18/DF3C/D17C -- and **8 coexistence** (d88f13c; scheduling
residual): func_8003CD08/D0E8/D110/DB2C/CE68/D7C8/D13C/D60C. **31 handlers
remain unported** (mid/large: D8B8, D9A4, DD24, DC50, DF78, E04C, ... --
scratchpad/handlers_sized.txt) for the next pass; their table slots dispatch
to stubs until then (unreached without those opcodes in a stream).

BUGS FOUND BY THIS PASS (the live-exercise payoff):
- func_8003C6E8 (fa91aeb coexistence body) had FOUR active_flag-vs-status
  transcription bugs (loop exit, post-loop gate, tie set/clear, gate word all
  read `lhu 0($s2)` = active_flag in retail; the C read status). Matching
  build was never affected (INCLUDE_ASM); the port's empty-queue tick never
  reached them. Fixed + exercised.
- 791f21b's D_800658DC size:0x230 swallowed D_80065ADC (a real symbol at
  +0x200 referenced by field misc4) and broke the matching FIELD link --
  latent because report-mode ninja has no link edges. Corrected: 0x200 +
  D_80065ADC sized 0x30.

Validation: synthetic sequence probe (XENO_SOUND_SEQ_PROBE) feeds a hand-
built command stream to the live 240Hz interpreter -- five opcodes route to
five distinct handlers with distinct observable effects; IP advances exactly
to stream end (routing + operand-length proof); fermata counts down (the
sequencer steps). SYNTHETIC data, labeled: full live exercise awaits WDS/B5.
Matching binary byte-identical (d004692f...) with all 20 bodies; make build
green (slus + field). TSan (probes + seq probe): zero sound races, only the
two catalogued PsyX infra races. Five-map watchdogs boot clean, tripwire
lines EXACT. Gotcha recorded: `make report` leaves build.ninja in report-only
mode (no link edges); run `make build` after to restore the matching
pipeline (gears matching vs gears report modes).

Honest state: dispatch table live and routing-proven via synthetic stream;
20/51 handlers real (12 {} oracle-confirmed + 8 coexistence); 31 handlers
remain; NOT YET AUDIBLE (WDS/B5).

### Handler layer batch 2 (step 3b, partial): 32/51 real

Group A of the remaining 31 landed: **9 objdiff {} oracle-confirmed
(fuzzy=100)** -- func_8003DB58/E40C/CFF0/CFA4/DB98/E308/D7FC/DEE4/E4F0 --
and **3 coexistence** (audited 1:1 + exercised; residual = load-reload
elision / register-copy / arg-setup scheduling): func_8003CEF0/D1BC/D4E4.
Oracle 1227/2292 (+9), code 36.05%, fuzzy 51.62%. Matching binary
byte-exact (d004692f); make build green; TSan clean (pre-existing PsyX
vblank only); five-map tripwires EXACT.

Synthetic stream extended to 10 opcodes: adds vib-accumulator nudge (0xE1),
channel fade (0xA7, interp70 counter/target verified), pitch-slide arm
(0xD4), and a pan FADE (0xEA) that runs to completion through C4C4's fade
path (retail sets the scaled delta on the final step -- expectation model
corrected, handlers verified asm-faithful). Routing + operand-length +
effects PASS; IP advances to stream end.

**19 handlers remain for 3c** (group B: CD8C, CF38, D070, D21C, D3D8, D438,
DBE4, D53C, DEE4-done, E1F8, E180, E360, E44C, E54C; group C large: DC50,
DF78, D8B8, DD24, E04C, D9A4 -- see scratchpad/handlers_3b.txt minus group
A). 0x99 (CF38, loop-continue) will enable a loop-stack test in the stream.
Honest state: 32/51 handlers real, oracle-confirmed, exercised via
synthetic stream; audible awaits WDS/B5.

### Handler layer batch 3 (step 3c, partial): 45/51 real

Group B (13 mid handlers): **3 objdiff {} oracle-confirmed (fuzzy=100)** --
func_8003CF38 (0x99 loop-continue), CD8C (0x90 dal-segno/track-end), E54C
(0xFF release-if-silent) -- and **10 coexistence** (audited 1:1 + residuals
all non-semantic micro-shapes: commutative operand order, delay-slot copy
placement, register reuse): func_8003D070/D21C/D3D8/D438/DBE4/D53C/E180/
E1F8/E360/E44C. Oracle 1230/2292, code 36.12%, fuzzy 51.69%. Binary
byte-exact; TSan clean; five maps EXACT.

FINDINGS:
- **Envelope method table RESOLVED**: func_8003E180 (0xF0) binds
  env->pfnHandler from D_800508A4 (sdata word table) -- the "runtime-trace"
  EFE4 jalr set is statically enumerable after all. The PORT needs host
  routing for that table (g_SoundScriptHandlers pattern) before envelopes
  can arm -- lands with the envelope/B5 pass. Until then streams must not
  arm envelopes (0xF0/0xF6).
- **Loop-stack live test deferred to 3d**: placing 0x98/0xA1/0x99/0x9A
  mid-stream parked the IP at the loop body -- C6E8's post-pass TIE-SCAN
  walks forward from the stored IP and interacts with loop constructs (and
  reads past stream end after a trailing rest -- synthetic streams need a
  scan-safe epilogue). Handlers are asm-faithful (objdiff/audit); this is
  stream-design + interpreter-model work, instrumented in 3d. Stream
  reverted to the 3b-proven form (PASS).

**6 handlers remain (group C large, 53-67 insns)**: func_8003DC50, DF78,
D8B8, DD24, E04C, D9A4 -- unhurried in 3d, plus the loop-test redo.
Honest state: 45/51 handlers real; not yet audible (WDS/B5).
Evidence: proven (oracle fuzzy=100 x3; probes PASS; A/B hash; TSan; 5/5
tripwires)
Last verified @ HEAD of this commit

### HANDLER LAYER COMPLETE (step 3d): 51/51 real

The final 6 group-C handlers landed as **coexistence** (audited 1:1;
residual = register-pressure frame shape, one extra callee-saved reg vs
retail; {}-upgrade candidates for a fresh matching session):
func_8003DC50/DF78/D8B8/DD24/E04C/D9A4 -- the envelope-priming family
(0xD8/0xD9/0xE4/0xE5/0xEC/0xED). They bind env->pfnHandler either directly
to func_8003F240/F2A0 (host fn addresses survive the u32 round-trip under
no-pie) or from D_800508A4 (retail addresses -- host routing deferred to
the envelope/B5 pass; streams must not arm table-bound envelopes).

**LOOP-TEST REDO PASSED**: scan-safe stream (loop-first + guard byte +
0xFD tempo restore after the in-loop 0xA1 zeroes the tempo product).
vol58=0x5B0000 proves the 0x98/0x99 loop ran exactly 2 live iterations --
CEF0 push + CF38 continue/pop LOOP-EXERCISED. Two stream-design findings
(probe-model class, handlers asm-faithful): (1) **0x9A is an early-exit-
INSIDE-loop construct, not a loop terminator** -- placing it after an
exhausted 0x99 pops an empty stack -> NULL ip (crash reproduced + root-
caused); (2) a drained rest makes the interpreter consume the scan guard
as a note (retail-faithful) -- rests must outlast the probe window.

Validation: oracle 1230/2292 (unchanged -- all 6 coexistence), binary
byte-exact (d004692f), make build green, TSan zero non-catalogued races,
probes PASS, five-map tripwires 5/5 EXACT.

**THE TICK ENGINE IS COMPLETE**: gate + tick core + all 51 sequence-command
handlers real and dispatching at 240Hz. Remaining before AUDIBLE: WDS/B5
(sample banks + SPU streaming) + the envelope-method host routing
(D_800508A4 + func_8003F240/F2A0 decomp) + {}-upgrades for the 19
coexistence bodies as desired.
Evidence: proven (loop-test live run; A/B hash; TSan; 5/5 tripwires)
Last verified @ HEAD of this commit

### B5 audible-leg scoping (trace + design, NO implementation)

Read-only trace of the last mile between the complete tick engine and sound
output. B5 is SMALLER than feared -- most of the chain is already real:

(A) WDS LOAD: SoundLoadWdsFile (62 insns, unported) + func_80039024 (72,
unported) = **2 oracle decomps**; every other callee is already-real C
(SoundQueueSpuWriteCommand, SoundSpuMemoryAllocateWDS, SoundHeapSetBlock/
ClearBlockMemory, gate brackets) or the stubbed error leg. Disc access is
REACHABLE: the caller (field misc8 func_80085F30, real C in the port) reads
the WDS via the WORKING archive path (ArchiveDataSync + heap buffer) --
sound reuses the field loader's disc infrastructure, no new I/O needed.

(B) SPU-RAM UPLOAD: **already wired end-to-end** -- SoundQueueSpuWriteCommand
-> SoundProcessTransferCommand -> SpuSetTransferStartAddr + SpuWrite (real in
PsyX LIBSPU.C, writes the backend SPU-RAM image) -> SoundOnTransferCallback
(real, drains the queue). Behavioral verification needed, no construction.

(C) PLAYBACK TRIGGER -- **THE one real gap**: the tick's voice-register
flush (func_8003E900/EB5C) writes key-on/off, pitch, volume, ADSR, start
address into the static SpuUnion backing page (state-faithful, unwired).
The backend is driven via SpuSetKey/SpuSetVoiceAttr (both real; 4x
alSourcePlay sites in PsyX_SPUAL). Wiring = a register->backend translator
(port-side: either #ifdef branches in E900/EB5C calling the Spu* API, or a
post-flush shim reading the backing page). ONE bounded host-wiring pass.

(D) SPU-IRQ -- **DEFINITIVE: NOT needed for audible**. SoundSpuIRQHandler
(real C) is a thin dispatcher to g_SoundSpuIrqCallbackFn -- clients are the
XA/CD STREAMING paths. WDS playback = one-shot SPU-RAM upload + key-on; the
upload uses the transfer-complete callback, not the address-IRQ. The IRQ
source defers AGAIN, to the streaming/XA leg (design it there).

(E) ENVELOPE ROUTING: func_8003F240 (24) + F2A0 (26) decomps + a 16-entry
host table for D_800508A4 (dispatch-table pattern). NOT audible-critical --
hardware ADSR shaping flows through the voice regs in (C); envelope objects
are modulation on top. Quality layer, defer past first-audible.

MINIMAL FIRST-AUDIBLE (B5.1, ~2 passes): decomp the 2 WDS fns (oracle) ->
behavioral-verify upload (SPU-RAM image gets sample bytes) -> wire the
register->backend translator -> drive a real WDS bank + note-on (field map
or synthetic stream with a real bank). B5.2 (~1 pass): envelope routing +
F240/F2A0. B5.3 (separate leg): XA/CD streaming + the SPU-IRQ source.

AUDIO-OUTPUT VERIFICATION REGIME (proving audible, not "ran"): (1) backend
state probe -- AL_SOURCE_STATE==AL_PLAYING + buffers queued (PsyX accessor,
alGetSourcei already used internally); (2) captured-output proof:
ALSOFT_DRIVERS=wave renders to a WAV, assert non-silence (RMS threshold) --
headless/CI-grade evidence; (3) the human ear for the milestone.

No showstoppers: disc access reachable, backend functional, IRQ defers.
Recommendation: fund B5.1 (first audible) next -- 2 decomps + 1 wiring pass.
Evidence: proven (subtree trace over split asm + C-body; transfer/IRQ roles
read from real C; PsyX_SPUAL API confirmed)
Last verified @ a7390b0

### B5.1 pass 1 LANDED: WDS load real, samples verified in SPU-RAM

SoundLoadWdsFile + func_80039024 decomped to **objdiff {} (oracle
fuzzy=100)**; SoundSpuMemoryAllocateWDS's void->u32 signature restored
retail's v0-passthrough (bare `return;` in a u32 fn -- codegen-identical,
still {}). Oracle 1232/2292, code 36.21%, fuzzy 51.78%. Binary byte-exact.
func_80039024 retail quirk (faithful): the heap-full failure path returns
WITHOUT re-enabling the tick event (bracket leaks; unhit in practice).

BEHAVIORAL PROOF (XENO_SOUND_WDS_PROBE): the REAL WDS bank (archive dir
0x1C file 3, 155,120 bytes) reads through the working archive path, parses
(dataOff 0x100, dataSize 0x25CF0), allocates SPU addr 0x12000, links into
g_SoundWdsLinkedList, transfer queue drains, and **SpuRead-back of the
backend SPU-RAM image MATCHES the source** at the first non-silent window
(probe-model fix: ADPCM banks open with silent blocks -- verify at the
first nonzero 16-byte window). Samples are IN SPU-RAM. NOT AUDIBLE -- pass
2 wires the register->backend key-on translator.

Validation: all 5 probes PASS (pump/init/gate/seq/wds); tripwire maps
3/3 EXACT; make build green. TSan: runs blocked by the recurring
PipeWire/OpenAL boot flake (documented since tick-core); the partial log
shows only pre-existing races (LIBETC ResetCallback); full TSan re-verify
deferred to a stable-audio session -- the pass's delta runs entirely under
the proven gate brackets.
Evidence: proven (oracle fuzzy=100 x3; SPU-RAM readback match; A/B hash)
Last verified @ HEAD of this commit
Evidence: proven (oracle fuzzy=100 x9; extended seq-probe incl. fade
convergence; A/B binary hash; TSan; five-map tripwires)
Last verified @ HEAD of this commit
Evidence: proven (oracle fuzzy=100 x12; seq-probe routing; A/B binary hash;
TSan; five-map tripwires)
Last verified @ HEAD of this commit

### B5.1 pass 2 LANDED: FIRST AUDIBLE -- register->backend translator, three-tier proof

The port makes sound. `PcPort_SpuRegFlushTick` (port_main.c) translates the
SpuUnion backing page into PsyX backend calls: registered as a second
counter-2 OpenEvent slot after SoundInitialize, it runs at 240Hz right after
func_8003C020 under the same g_SoundTickMutex bracket. Per pending KON bit it
builds a SpuVoiceAttr {VOLL|VOLR|PITCH|WDSA} from the voice regs (addr =
reg<<3), calls SpuSetVoiceAttr + SpuSetKey (-> alSourcePlay), then clears the
page's KON/KOFF words (write-trigger semantics). No envelopes (B5.2), no
streaming/IRQ (B5.3).

PLAY PROBE (XENO_SOUND_PLAY_PROBE, needs XENO_SOUND_WDS_PROBE): arms element
0 with a synthetic stream -- 0xFC bank+instrument 0, 0xE0 volume (el+0x78
accumulator; 0xA0 is NOT volume), note 0x30, long rest. Probe-model findings
that were engine truths, not bugs: (1) key-on requires arming from the REST
state (active=0x401) -- status bit 0x1 is only set on a rest->note edge;
(2) the voll chain is elVol(el+0x78 hi16) x expression(el+0x76) x manager
level(interp70 hi16) x pan law -- el+0x76 and interp70 are armed by the
UNPORTED song-start path, so the probe stands in for it. Result: real WDS
instrument 0 at SPU 0x12000, pitch 0x1530 from the extracted sdata tables,
voll/volr 12603/8571 through the full retail chain.

THREE-TIER AUDIBLE PROOF: (1) AL source state: SpuGetKeyStatus polled during
the window -> AL_PLAYING observed. (2) Wave-capture RMS: ALSOFT wave backend
(drivers=wave) -> 39.6s float32 capture, max 100ms-window RMS 0.0726
(FS=1.0), peak 0.31; CONTROL run (WDS load, no play probe) is digitally
silent (RMS 0.000000, peak 0.0000) -- the energy is the note. (3) Ear:
capture saved at scratchpad/first_audible_capture.wav (untracked) -- listen.

TSAN RAN CLEAN this pass (flake absent under wave backend) and CAUGHT A REAL
BUG: SoundSpuMemoryAllocateWDS's retail v0-passthrough (bare `return;` in a
u32 fn) is UB on host; native x86 happened to keep the allocator's result in
the return register, TSan's exit instrumentation clobbered it (spuAddr came
back garbage). Fixed via the d88f13c coexistence split: port body has
explicit returns, matching build keeps the retail shape -- binary still
byte-exact (d004692f...). 0/38 TSan reports touch the sound path (rest are
pre-existing Mesa/gallium/SDL boot noise). WDS probe also gained a
drain-poll + bounds guard (fixed-order readback raced the pump under TSan's
~15x slowdown).

Sound sdata tables (D_80050B78/BF0/A94/9B0/824) extracted from
3F290.sdata.s into guarded port-side C (auto-stubs were zeros -> pitch 0).
Validation: all 6 probes PASS (pump/prim/init/seq/wds/play); tripwire maps
3/3 clean (env-noise-only diffs); make build byte-exact. Known limits: notes
are envelope-less (raw ADSR unsupported by PsyX backend; hard-stop on
key-off), one synthetic note != real music, song-start arming still probe-side.
Evidence: proven (three-tier audible incl. silent control; TSan clean run;
A/B binary hash; six probes; three tripwire maps)
Last verified @ HEAD of this commit

### Song-start path SCOPED (read-only trace; the map the B5.1 probe stands in for)

TWO start paths, one driver. Sequenced sound starts EITHER as an SFX pair or
as a music manager; both feed the already-ported tick/handler/translator
engine (the tick's func_8003C020 port body already walks
g_SoundAudioManagerListHead -- multi-manager is free).

SFX pairs (menu/field cues): game APIs func_80039DB8(packed sedId<<16|entry)
/ func_80039F9C(packed, slot, vol, pan) [+E18/EC4 (ported C)/E60/F18] ->
**func_8003B644** (200 insns, the ONLY 0x409 element-armer): find SED in
g_SoundSedsLinkedList, bind WDS bank, arm TWO elements (each SED entry = 2
script-offset halfwords at +0x20, one script per channel), set el+0x14 IP,
active=0x409/0x40B, el+0x78=0x7F<<24, **el+0x76=(reqVol x entry volume
byte at sed[+0x18 table])>>7**, el+0x74=pan, mgr+0x10|=0x8000 -- everything
the B5.1 play probe hand-armed. Handler func_8003CFF0 calls func_80039F18
(script-spawned starts; currently a silent stub under real sequences).

Music managers (field BGM): retail field func_80085C90 (per-frame poller,
port SHIMMED; retail asm in matchings/) buffer-reads the SONG FILE --
ArchiveSetIndex(0x1C,0) + ArchiveReadFileToBuffer(songId*2+0x14, D_80062648,
0, 0x80), the SAME working path the WDS probe used -- then
**func_80039850**(file) create-manager (elementCount=file[0x14], binds file
at mgr+8; B0AC extra block if file[0x15]) -> **func_8003B22C** (copy header
0x10-0x1D: wdsId+0x16, tempo bytes, reverb depths -> mgr,
SoundSetReverbModeWithAllocation + ported SoundInitializeAudioManager) ->
**func_8003B424** (136 insns; music twin of B644's loop: per-element script
IPs from file payload, 0x7F<<24, SoundFindWdsEntry bank bind, E5BC prime,
AssignVoiceAndStop) -> **func_8003A89C**(mgr, level, fade) = the
manager-level (interp70) setter/fader = FE 0E's target (func_8008C84C,
ported, mgr ptr D_80062528). Variants: func_80039910 rebind-same-manager,
func_80039A80 restart, func_80039B68 WDS-rebind+level resume, func_800399D4
free, func_8003AA30 resume-from-mute. interp70 DEFAULT 0x7F<<24 is already
ported (SoundInitializeAudioManager line ~1895) -- the probe's zeros were
el+0x76/el+0x78, both B644-armed.

Data: song files = dir 0x1C file songId*2+0x14 (buffer-read, reachable NOW);
music WDS banks = dir 0x1C file D_800ADFCC[musicIdx*2]*2+0x13 (retail
STREAMS via func_80085560; port's ArchiveReadFile bails on CdlModeStream --
substitute buffered read, host-wiring); common SED = dir 4 file 0xA8
(SoundAddSedsEntry, ported). Field boot already routes: main.c
func_80085B20(map's D_800B2290) at map load; cue opcodes at misc.c
1552-1592 -> func_80085634/func_800855C8 (855C8 = no-op shim).

Port traps identified: D_80062648 (song buffer, unmapped BSS at 0x80062648,
~12.9KB gap to D_800658DC) will auto-stub tiny -- needs size: annotation
(the D_800658DC LZSS lesson, applied proactively). func_80039F18 stub is
callable from ported handler CFF0. No showstoppers found.

Phased price (decomp insns via split asm, all oracle-checkable):
M1 music-side decomp: 10 fns ~574 insns (39850/39910/399D4/39A80/39B68/
B0AC/B22C/B424/A89C/AA30) -- one tick-core-sized pass. M2 first REAL
sequence audible: host probe ~80 lines (buffer-read song+bank, 39850+39A80+
A89C(0x7F)), three-tier audio proof -- rides M1. S1 SFX chain: 11 fns ~606
insns (B644/A65C/F9C/DB8/E60/F18/A20C/A344/A55C/FF8/A094) -- menu+field
cues, un-stubs CFF0 spawns. M3 field un-shim: real func_80085C90 body +
func_800855C8/85678 + CdlModeStream buffered substitute + D_80062648
sizing -- in-game music in MAP000 smoke. B5.2 envelopes (as scoped): host
D_800508A4 routing + F240/F2A0 -- note shaping quality.
Recommended order: M1 -> M2 (proof-of-life) -> S1 -> M3 -> B5.2.
Evidence: traced (asm-level, both paths end-to-end; no implementation)
Last verified @ 4dbf1a0

### Song-start M1 LANDED: music arming layer decomped, 9/10 {} + 1 audited coexistence

The 10 music-side functions from the scoping map are real. **{} (oracle
fuzzy=100 x9)**: func_80039850 (create manager from song file; merged
live-range idiom -- the parameter register is reused for the manager),
func_80039910 (rebind + 0x4000 not-heap-owned), func_800399D4 (destroy;
0x4000-guarded free), func_80039A80 (restart: re-init + re-arm + level under
the bracket), func_80039B68 (WDS-rebind resume; single-cursor loop anchored
on voice_data.flags -- the flags store sits LAST so the induction family
anchors there), func_8003A89C (manager level/fade = interp70; FE 0E's
target), func_8003AA30 (resume: reverb reapply + all-dirty + running),
func_8003B0AC (5-byte override records -> manager tail block),
func_8003B22C (song header -> manager + reverb + common init).
**Coexistence (audited)**: func_8003B424 (the element-arm loop) -- the same
cc1 anchor-rebase residual as C6E8/EBF0 (retail keeps the field cluster on
the voice_data cursor; this cc1 rebases the family onto last-use/derived
anchors, register-permuting the loop; 12 variants tried). Field-by-field
offset map annotated inline; retail-faithful quirk kept: voice numbers lag
the element index (element 0 = conductor, voice 0xFF, bounds-checked away).

New matching idioms recorded: dbr fills a branch delay slot with a preceding
independent copy (place `pF = pFile` BEFORE the error branch); the
alias-variable (manager=(cast)pFile) blocks that placement; st[k]/named-field
choice decides induction-family anchoring (byte-casts off a typed cursor
split families).

D_80062648 (field song buffer) size-annotated 0x3200 (0x62648..0x65848,
flush to the next referenced symbol) BEFORE anything reads into it -- the
D_800658DC lesson applied proactively. Validated under make build.

TRIPWIRE CATCH (MAP014 segfault, fixed): Map014's field script issues FE 0E;
with func_8003A89C now real, the func_80085C90 shim's "song loaded" report
fed it D_80062528 == NULL (retail writes low kernel RAM silently -- no null
guard in the asm). Port-boundary guards (XENO_PC_PORT-only, documented,
remove at M3) added to func_8003A89C and func_800399D4 (misc4.c reaches it
the same way). Matching binary unaffected.

Validation: oracle 1232 -> 1241/2292 (+9 = the {} count), fuzzy 52.10%;
slus + field.bin byte-exact (d004692f... / c4200fdb...); port + TSan builds
green; all 6 probes PASS at 240Hz; TSan 0/38 reports touch sound.c or the
M1 functions (rest = pre-existing gallium/SDL/PsyX families); five-map
tripwires clean (MAP000/001/014 exact; 047/334 diffs are CD-path cosmetics
from the baseline's capture directory).

HONEST STATE: the arming layer is decomped and byte-verified, but NOT yet
exercised against real data -- M2 (next) probe-drives a real sequence
through func_80039850 + func_80039A80 + func_8003A89C and proves audibility
via the three-tier regime. No music-audible claim here.
Evidence: proven (oracle fuzzy=100 x9; coexistence audit; A/B binary hash;
six probes; five tripwire maps; TSan build)
Last verified @ HEAD of this commit

### Song-start M2 LANDED: A REAL XENOGEARS SEQUENCE PLAYS

First real music through the whole stack: archive dir 0x1C pair (WDS bank
file 0x13, 'wds ' magic, id 0x21, self-addressed to SPU 0x38000 -- coexists
with the common bank at 0x12000; song file 0x14, 'smds' magic, 21 elements,
wdsId 0x21 == the bank id) buffer-read via the proven path and driven
through the M1 chain exactly as retail field code does: func_80039850
(create) -> func_80039A80 (start, level 0x7F). The ported tick interprets
the real sequence at 240Hz; the translator keys voices.

Scan first (XENO_SOUND_SONG_SCAN): dir 0x1C = common bank (3) + repeated
(bank, song) pairs from 0x13 -- songs are the 'smds' files at even indices
(elemCnt at +0x14, wdsId at +0x16), banks 'wds ' at the odd ones; several
slots duplicate the same pair (CD streaming locality).

THREE-TIER PROOF (XENO_SOUND_SONG_PROBE): (1) AL: per-voice
SpuGetKeyStatus sampling -- 6-8 voices PLAYING in 16/16 half-second
samples, 22 key-ons / 38 key-offs over 8s of playback (polyphonic,
sustained, evolving = a SEQUENCE, not a note). (2) Wave-capture RMS
profile: 8 consecutive non-silent seconds (1s-window RMS 0.048-0.065,
VARYING), silence lands exactly at the probe's teardown; CONTROL run
(manager created, never started) is digitally silent (RMS 0.0000, peak
0.0000, keyons=0). (3) Ear: scratchpad/first_real_sequence_capture.wav
(untracked) -- which theme file 0x14 is (map->musicIdx mapping lives in
field data D_800ADFCC) is verifiable by listening.

Boundaries measured clean: func_80039F18 (the flagged CFF0/S1 trap) NOT
hit; M1 NULL guards never fired (real manager end-to-end -- create ->
start -> stop -> destroy, teardown verified by post-stop silence);
playback-caused stubs are EXACTLY the three envelope helpers
(func_8003E290/E3E0/F2A0 = B5.2), isolated by play-vs-control stub diff
(the SFX-chain stubs seen in both runs are boot-path, S1).

TSAN CAUGHT ANOTHER PASSTHROUGH BUG (pre-M1 code, first real exercise):
func_80039C4C called SoundReleaseAllVoices() with NO argument -- retail
rides the manager in $a0 (the asm has no move before the jal); native x86
reproduced the luck, TSan clobbered it (SEGV in teardown). Fixed with the
explicit argument -- gcc emits zero extra instructions ($a0 already holds
it): func_80039C4C stays {} and the binary stays byte-exact. The
passthrough family now has two members (v0-return + a0-argument); grep
argless calls of parameterized functions when TSan crashes what natively
works.

Validation: slus byte-exact (d004692f...); all 7 probes PASS (incl. the
new song probe) at 240Hz; TSan rerun clean -- PASS with identical playback
stats, 0/37 reports touch the song path; five-map tripwires clean.

HONEST STATE: a real sequence plays PROBE-DRIVEN and envelope-less (notes
key at full computed volume, no ADSR shaping -- B5.2), with sequence-set
tempo/volume via the 51 real handlers. In-game triggering (field shims,
CdlModeStream substitute) is M3; SFX chain is S1.
Evidence: proven (three-tier incl. silent control + varying-RMS profile;
stub-set isolation; TSan clean rerun; A/B binary hash; five tripwires)
Last verified @ HEAD of this commit

### Song-start M3 LANDED: IN-GAME MUSIC -- maps play their themes on boot, no probe

The field music trigger is real end-to-end. func_80085C90 (per-frame music
poller, called from func_80078B5C while D_8004F308==-1) is un-shimmed: full
retail structure from the matchings asm -- bank-swap completion leg,
common-bank lazy-load leg (func_80085FB8 kick + func_80085F30 complete, dir
0x1C file 3), song-file read (songId*2+0x14 -> D_80062648, the M1-sized
buffer), then the M1 chain (func_80039850 create -> func_80039A80 start
0x7F, plus the muted-start+FE-0E-fade and func_80039B68 bank-rebind resume
branches, D_8004F340/348 state machine as retail).

STREAM SUBSTITUTION (documented, XENO_PC_PORT block in C90's swap leg):
retail streams the per-map music bank in 8-sector CD windows chunk-pumped to
SPU by func_800859DC (func_800380D0 + SoundTransferWdsPart, unported;
func_80028B14 chunk-poll is a stub) -- never resident in main RAM. NOT
substitutable in archive_port.c as scoped (the stream buffer is an 8-sector
window; the chunk consumer is unported); and NOT substitutable by a
HeapAlloc'd whole-file read either -- the field heap cannot fit ~190KB
mid-map-load (caught live: MAP001 requests music 0 -> HeapAlloc(195040)
fails -> GameHandleError(130) spin). The port stages the file in HOST malloc
memory and lands it via SoundLoadWdsFileHostStaged (new port-only sibling of
SoundLoadWdsFile in sound.c): identical SPU-alloc/header-copy/list-append
flow, but the payload goes through synchronous SpuWrite -- the transfer
queue narrows source pointers to PSX addresses, which host memory doesn't
have. Same never-in-PSX-RAM property as retail's stream.

M1 NULL GUARDS REMOVED (the tracked exit criterion): func_8003A89C and
func_800399D4 are guard-free. The real C90 raises D_8004F36C (FE 0E's gate)
only after the manager exists, and misc4's teardown pair is gated on
D_8004F304 which nothing ported sets yet -- NULL exposure closed
structurally; MAP014 (the original guard trigger) now plays music through
FE 0E on a real manager.

Field-test harness stand-ins (XENO_PC_PORT + XENO_FIELD_TEST, misc3 init):
(1) direct map entry skips the exit-transition flow that requests music --
the harness runs FieldMain's own request body (func_8001B66C, D_8004F308=-1,
D_8004F324=D_800B2290, func_80085B20) with the field default id 0x1D;
(2) retail boots D_8004F364=1 ("common bank resident", loaded by the
unported new-game flow) -- the harness clears it so C90's retail leg
lazy-loads the bank (the lie surfaced as a host SEGV in func_8003E5BC when
the sequence's instrument-change ran bankless; retail reads PSX low RAM
harmlessly there).

TSAN CAUGHT A REAL RACE, FIXED AT THE ROOT: SoundQueueTransferCommand's
queue-index/flag writes (main thread, via the now-live func_80085F30 ->
SoundLoadWdsFile) raced the tick's g_SoundControlFlags reads. Retail
protects the section with EnterCriticalSection (interrupt mask); the port's
Enter/ExitCriticalSection were no-ops. Now mapped onto the tick gate
(PsyX_Sys_SoundGateEnterCritical/Exit in LIBAPI.C, recursive hold like a
DisableEvent bracket; psycross_sound_gate.patch regenerated, reverse-check
verified). TSan rerun: 0/38 reports touch the music path.

IN-GAME PROOF (existing smoke launchers, NO probe): MAP000 -- 4s boot
silence (pre-trigger control, RMS 0.0000) then 55/59 non-silent seconds
(1s RMS 0.035-0.074 varying, peak 0.41); key-ons across voices 9-18 with
per-voice stereo volumes, distinct pitches, six instrument addresses; ZERO
sound-path stubs (no func_80039F18, no envelope stubs -- complete playback).
MAP001 (music 0 + bank 0x13 swap): music from t~4s, 85/89 non-silent.
MAP014 (FE 0E): music from t~17s, 72/89. WAVs: scratchpad/
map000_ingame_music.wav (music id 0x1D = song file 0x4E -- identify by ear).

Validation: slus byte-exact (d004692f...); all 7 probes PASS at 240Hz;
five-map tripwires ALL EXACT on the final build (line-buffered captures;
the old runner's "LOG CLOSED" trailer filtered). Known nit: a map with music
running may ignore SIGTERM (MAP334 exits via SIGKILL under timeout -k;
game output exact) -- teardown signal handling, not a field regression.
Remaining boundaries: menu/normal-boot music start (needs the new-game flow
or a KernelMenu-path stand-in), SFX cues (S1), envelopes (B5.2), real CD
streaming (would retire the host-staged substitute).
Evidence: proven (in-game three-tier on three maps incl. pre-trigger
control; TSan root-fix + clean rerun; A/B binary hash; five exact tripwires)
Last verified @ HEAD of this commit

### B5.2 LANDED: envelope shaping -- vibrato/tremolo modulation live, comparative-proven

The envelope layer is real: 10 new {} (oracle fuzzy=100; 1241 -> 1251/2292)
-- func_8003E290 (target packing; switch/expand_case shape), func_8003E3E0
(arm/reset), and the full method family func_8003F1A4/F1EC/F240/F2A0/F308/
F354/F3C0 (gate, alternating set, triangle, ping-pong, sawtooth, random
level, random bipolar) + func_8003F43C (xorshift RNG). F2A0 matched via the
merged counter/reload live range + unmasked decrement + flags-var-reuse
idioms. D_800508A4 (16-entry method table) is wired port-side: runtime-
filled at SoundInitialize with host addresses narrowed through the no-pie
u32 round-trip (a truncating cast is not a valid static initializer);
func_8003EFE4's already-ported dispatch consumes them. The translator
gained a continuous change-detected voll/volr/pitch flush (envelopes
modulate the register page every tick; key-ons alone are edge-triggered).
Bonus fix: func_8003E1F8's coexistence body dropped E290's rate argument
(latent while E290 was a stub).

COMPARATIVE PROOF (same song, M3-baseline binary from a worktree vs B5.2):
(1) VIBRATO, register domain -- song 0x14 arms pitch envelopes (state 0,
0xD8/ping-pong): baseline pitch-mod accumulator flat 0 for 7646 ticks;
shaped run has 133 modulated ticks swinging +/-14 with the SPU pitch
register tracking (0x1071..0x108C around 0x1080). (2) TREMOLO, audio
domain -- song 0x1C arms volume envelopes (state 1, alternating): 567/892
aligned 50ms windows differ >5% with oscillating deltas (+2%..+120%),
identical note timing (18 keyons both) -- the modulation is the only
difference. (3) THE TABLE IS LOAD-BEARING -- song 0x20 (method-6 random
envelope via the table) SEGVs the baseline at pfnHandler=0 inside
func_8003EFE4's dispatch (exact bt) and plays fully on B5.2. Playback
stubs are now ZERO (E290/E3E0/F2A0 were the last). Pan-envelope audio
isolation (song 0x20, state 2) was inconclusive in the full mix -- not
claimed; the dispatch/accumulator path is shared with the proven cases.
IN-GAME: MAP001's music is song 0x14 -> its vibrato now shapes in-game
(85/89 nonsilent seconds on the smoke boot); MAP000's song arms no
envelopes (engine truth, boot unchanged/exact).

M3 CORRECTION -- the critical-section fix was DEAD CODE as shipped:
PsyCross compiles LIBAPI.C as C++, so the gate functions got mangled
linkage and the C callers bound to the auto-stub no-ops (nm shows the
_Z31... twin). Fixed by declaring them in PsyX_main.h's extern-C block
(patch regenerated, reverse-check verified). The blanket
EnterCriticalSection->gate mapping then proved TOO WIDE under TSan
(couples every subsystem's critical sections to the audio lock; TSan
MAP000 regressed) -- narrowed to the actual load-bearing site: the SPU
transfer-queue bracket in SoundQueueTransferCommand takes the gate via
port-only calls; global Enter/ExitCriticalSection stay no-ops. TSan
MAP000 back to M3-parity (RUN=124, full boot, 0 sound-path reports).
Related robustness: the C90 bank staging now records the bank file at
stream START (func_80085B20) instead of deriving from the CURRENT music
index at completion (wrong file if the request changes mid-stream), with
size guard + sector-rounded staging alloc.

KNOWN ISSUE (TSan-only, filed): MAP001 under TSan crashes late in the C90
bank-staging read (memcpy with a garbage-huge count in ArchiveReadFile's
bounce path; the same staging passes on TSan MAP000 and everywhere
native). Suspected archive-global/TSan interaction in the M3-era leg, NOT
a data race (0 sound-path race reports across every B5.2 TSan run) and
NOT reproducible natively (all five tripwires exact). Root-cause deferred.

Validation: slus byte-exact (d004692f...); all 7 probes PASS at 240Hz
(native + song probe under TSan); five-map tripwires ALL EXACT; oracle
1251/2292 (fuzzy 52.34%).

HONEST STATE: notes are envelope-MODULATED (vibrato/tremolo/pan program
from the songs' own scripts). Hardware ADSR attack/release per-note remains
approximated (PsyX has no raw ADSR; key-off is a hard stop) -- the
envelope OBJECTS are done, ADSR emulation is a backend follow-up. SFX (S1),
menu/boot music, CD streaming remain.
Evidence: proven (register + audio comparative A/B against an M3 worktree
binary; negative-space table proof; oracle fuzzy=100 x10; five tripwires)
Last verified @ HEAD of this commit

### S1 LANDED: the SFX chain -- sound effects play through the real pipeline

The SFX twin of the music path is real: 12 functions. **{} (oracle
fuzzy=100 x4; 1251 -> 1255/2292)**: func_80039DB8 (fixed-top-slot API),
func_80039E60 (allocated-slot API), func_80039F18 (the CFF0-spawn/API entry
-- the flagged stub, now REAL), func_80039F9C (field-cue API).
**Coexistence (audited, d88f13c pattern)**: func_8003B644 (the SFX
element-arm core -- the EXPECTED B424-twin cc1 anchor-rebase residual;
every store audited against the split asm offsets), func_8003A65C (slot
allocator: dedupe + downward window scan + oldest-steal; regalloc
permutation residual), func_80039FF8 (stop-all; load-scheduling),
func_8003A094/func_8003A14C (stop-by-SED/stop-by-id; cursor-role
permutation), func_8003A20C/A344/A55C (pair stop/volume/pan; a
two-instruction scheduler placement of the slot masking).

PASSTHROUGH-UB FAMILY, 4th MEMBER: func_8003A65C's steal-scan leaves its
`best` slot variable UNINITIALIZED when no candidate has priority <= 0x20
(retail reads stale $s6 -- PSX wraps harmlessly; the host would index
wild). Port-guarded default (scan start) under XENO_PC_PORT; matching
keeps the retail shape. The linkage trap did not apply (no PsyCross-side
additions this pass).

SFX AUDIO PROVEN (XENO_SOUND_SFX_PROBE): the probe loads the field's
common SED (dir 4 file 0xA8, real SoundAddSedsEntry -- id 0, 468 entries)
plus the common WDS bank, then fires entry 1 through the REAL chain
(func_80039F18 -> A65C slot alloc -> B644 pair-arm -> tick interprets the
SED scripts). Three tiers: (1) AL -- 1 key-on event, 2 voices PLAYING
(the SED pair), 3/20 100ms samples (a ~350ms one-shot); (2) wave-capture
-- a 0.35s attack/decay burst (RMS 0.039->0.078->decay, peak 0.38) with
the CONTROL (SED loaded, nothing fired) digitally silent through that
window; (3) WAV: scratchpad/first_sfx_capture.wav. Probe-model catches en
route: effect scripts bind WDS banks (opcode 0xFC), so the common bank
must be resident (the probe loads it; the field boot's FB8/F30 leg does
in-game) -- bankless was a NULL-instrument crash, engine truth. And a
PROBE-ARTIFACT fix: the B5.1 play probe's teardown zeroed the manager
TEMPO (mgr+0x54), freezing the sequencer for any later sound work in the
same run -- it now restores the saved value (caught because the combined
suite failed only after the play probe).

CFF0-spawn: no measured song uses the CFF0 opcode (M2/M3 test songs
don't), so the in-sequence spawn path is exercised via the probe's direct
func_80039F18 call -- the identical entry the handler invokes.

Validation: slus byte-exact (d004692f...); ALL 8 probes PASS in a single
combined run at 240Hz; TSan SFX run PASS with identical stats, 0/34
reports touch S1 code (the run's exit is the filed TSan-only late
normal-boot staging crash, post-probe); five-map tripwires ALL EXACT.

HONEST STATE: sound effects play, probe-proven, on the same pipeline as
the shaped in-game music. Remaining: in-game SFX triggering (the
func_800855C8/85634 field-cue un-shim -- the M3-equivalent for effects),
menu/boot music (new-game flow), hardware-ADSR per-note polish, CD
streaming, the coexistence {}-upgrade backlog.
Evidence: proven (three-tier incl. window-silent control; TSan clean on
the SFX path; A/B binary hash; five exact tripwires; oracle fuzzy=100 x4)
Last verified @ HEAD of this commit

### IN-GAME SFX LANDED: field cues fire real effects -- M3's twin for the effects domain

The field SFX cue path is un-shimmed. func_800855C8 is the real retail body
(stop the channel pair via func_8003A20C, fire through func_80039F9C -> the
S1 chain); func_80085634 now passes its retail FOURTH argument (the channel,
a1 & 7 -- visible in the matchings asm's $a3 flow, dropped in the old
transcription and latent while the shim no-op'd everything). Both compile
into the field build; no slus changes (oracle unchanged at 1255/2292, slus
byte-exact d004692f...).

REAL IN-GAME CUES FOUND: MAP001 (Lahan) fires villager/scripted cues
(common-SED entries 0x6/0x7/0x37/0x86) -- but actor-AI-nondeterministically
(observed 12 cues in one run, zero in six others; gdb instrumentation
overhead also perturbs script pacing). MAP014 fires a DETERMINISTIC
boot-script cue: id 0x36 at scripted volume 32, chan 3 -- reproduced in
every run. MAP334 fires one. The cue path: field script -> misc.c cue
opcode -> 855C8 -> A20C+F9C -> B644 pair-arm (elements 14/15 -> voices
22/23, unclaimed by anything else) -> tick -> translator.

RESIDENT-PATH FIX (the no-guards rule): firing MAP014's cue crashed at the
effect script's bank bind (func_8003E44C -> E5BC, NULL WDS list) -- the cue
fires BEFORE the per-frame C90 leg lazy-loads the common bank; retail never
had this window (its new-game flow preloads the bank, the D_8004F364=1 boot
state). The harness stand-in now loads the common bank EAGERLY at field
init (func_80085FB8 kick + func_80085F30 completion, bounded retry) --
restoring retail's residency invariant instead of guarding.

IN-GAME PROOF (MAP014 boot, no probe): (1) register/AL tier -- one cue ->
KONs on exactly v22+v23 (the pair) at tick-t 4.3s, both from the common
bank (addr 0x12000), distinct per-channel pitches 0xC7/0x128; (2) audio
tier -- the capture's first sound starts at 4.25s: a sustained quiet
ambient (~0.014 RMS, scripted vol 32) filling the 13-second pre-music
window where the M3-era control capture is DIGITALLY SILENT (0.0000 until
its music at 17s; both captures then converge on the identical music
profile); (3) WAV: scratchpad/map014_ingame_sfx.wav. Diagnostics added:
XENO_SOUND_KON_TRACE (translator key-on trace with tick timestamps) and an
XENO_FIELD_DIAG cue print.

The AMBIENT-SFX chain was scoped but stays gated: func_80085788 (per-map
ambient SED loader, dir 0x1C file ambientId+0x115 -> SoundAddSedsEntry +
the D_800AE060 schedule) and func_80085678 (per-frame scheduled-cue pump ->
func_80039EC4) are decompilable, but their CALLERS (func_800A7C58 map-load,
func_800A732C per-frame) are unported field-load machinery -- ambient
scheduled SFX land when that ports (or with a harness pump stand-in).

Validation: slus byte-exact; all 8 probes PASS; TSan MAP014 full boot with
the cue path, 0/35 reports in cue/SFX code; five-map tripwires ALL EXACT
(the new diagnostics are env-gated, default-off).

HONEST STATE: sound effects fire IN-GAME from real field-script cues
(MAP014 deterministic, MAP001/334 observed). Remaining: ambient scheduled
SFX (caller porting), menu/boot music, hardware-ADSR polish, CD streaming,
the coexistence {}-upgrade backlog.
Evidence: proven (in-game three-tier vs the M3-era silent-window control;
deterministic cue; TSan clean; five exact tripwires; A/B binary hash)
Last verified @ HEAD of this commit

## Audio fidelity: hardware-ADSR scoping map (backend pass, priced, no implementation)

SCOPING PASS (read-only; the audio-fidelity arc's first leg). The user-audible
gap -- "plays right but sounds like a .wav, not a PS1" -- decomposes with
hardware ADSR as the biggest lever: today a note keys on at its computed
volume with NO attack ramp, and key-off is alSourceStop -- a hard cut with NO
release tail. Every note is a rectangle. The SPU's per-note envelope
(attack/decay/sustain/release) is what this pass scoped.

(A) GROUND TRUTH -- the SPU ADSR algorithm (psx-spx, "SPU Volume and ADSR
Generator", https://psx-spx.consoledev.net/soundprocessingunitspu/; the
chapter is hardware-test-derived). ADSR1 (voice reg +0x8): bit15 attack mode
(0=linear/1=exp), 14-10 attack shift, 9-8 attack step, 7-4 decay shift, 3-0
sustain level. ADSR2 (+0xA): bit15 sustain mode, 14 sustain direction, 12-8
sustain shift, 7-6 sustain step, 5 release mode, 4-0 release shift. Envelope
level 0..0x7FFF advanced by a 44.1kHz counter machine: AdsrStep =
(7-step) << max(0,11-shift) (negated for decrease), CounterIncrement =
0x8000 >> max(0,shift-11), a step applies when the counter carries bit15.
Exponential increase is fake (step/4 above level 0x6000, shift-dependent);
exponential decrease scales the step by level/0x8000. Decay is always
exp-decrease, ends at (SL+1)*0x800; attack always increase, ends at 0x7FFF;
release always decrease, ends at 0. KON resets the level to ZERO and starts
attack; KOFF switches to release from any phase. ENVX (+0xC) exposes the
live level.

(B) PLUMBING VERDICT: the params reach the REGISTER PAGE, not the backend.
Retail computes everything -- the instrument load (func_8003E5BC leg)
unpacks per-program ADSR fields from the WDS bank, seq cmds (0xC1 raw ADSR
family) override them, and func_8003E900's dirty-flag flush (bits
0x10/0x20/0x40/0x80/0x100) assembles real ADSR1/ADSR2 words into the
SpuUnion page at +0x8/+0xA per voice, before committing KONs (ordering is
already attr-before-key). The translator (PcPort_SpuRegFlushTick) forwards
only VOLL/VOLR/PITCH/WDSA; the backend ignores ADSR mask bits anyway
(PsyX_SPUAL_SetVoiceAttr's "TODO: ADSR" -- though SPUALVoice already embeds
the full SpuVoiceAttr with ar/dr/sr/rr/sl/adsr1/adsr2 fields, so storage
exists unused). PREREQUISITE (cheap, ~30 lines): change-detected raw-word
forward from the page via the existing SPU_VOICE_ADSR_ADSR1|ADSR2 mask bits.

LATENT BUG FOUND (fixed free by this pass): SpuGetVoiceEnvelopeAttr is an
auto-stub that never writes its out-params -- seq cmd 0xFF (func_8003E54C,
"release the voice once the envelope decays") branches on UNINITIALIZED
stack. The real envelope generator backs this API with true ENVX and makes
the engine's own voice-release logic correct.

(C) APPLICATION POINT: per-tick AL_GAIN at the existing 240Hz translator
tick (counter-2 event, g_SoundTickMutex-serialized -- the advance clock
already fires in the right bracket). No software mix stage exists (PsyX
decodes ADPCM at key-on into one static AL buffer per voice) and none is
needed for phase 1. Composition rule: the envelope MULTIPLIES the
volume-derived gain (effective = baseGain * level/0x7FFF); today
SetVoiceAttr writes AL_GAIN directly from VOLL/VOLR, so the volume path must
store baseGain and let the envelope tick own the final AL_GAIN write (two
writers would fight). GRANULARITY: advance the spec's counter machine at
44.1kHz in a per-tick batch (~184 cycles/voice/tick, ~1.1M iter/s for 24
voices -- trivial), quantize the APPLICATION to 240Hz; OpenAL-soft smooths
gain changes across its mix period, and sub-4ms attacks collapse to
effectively-instant (audibly identical). The risky band is ~5-50ms ramps;
phase 1's capture proof doubles as the stepping measurement. If stepping is
audible, phase 2 is AL_SOFT_callback_buffer streaming (per-sample envelope
in the mixer callback) -- the only leg that touches architecture, deferred
until measured, NOT assumed needed.

(D) STATE MACHINE (per SPUALVoice): {phase Off/Attack/Decay/Sustain/Release,
level, counter}. KON -> level=0, Attack (do not re-latch: rates are read
LIVE from the last-flushed adsr1/adsr2 each advance -- hardware reads the
registers continuously, mid-note changes apply). Attack->Decay at 0x7FFF,
Decay->Sustain at (SL+1)*0x800, KOFF -> Release from any phase; in Release
the source KEEPS PLAYING (no alSourceStop) until level 0, then stops.
Key-status semantics follow: GetKeyStatus stays AL_PLAYING-based, which now
correctly reports SPU_OFF_ENV_ON during tails; mute/pause paths unchanged.

(E) FIDELITY PROOF (curve-match, not "has an envelope"): (1) UNIT -- the
envelope generator as a pure function, golden-diffed cycle-exact against an
independent offline Python implementation of the psx-spx pseudocode over an
AR/DR/SR/RR/SL x mode matrix (phase durations + level trajectories). (2)
CAPTURE -- prim-probe a single note (constant-amplitude synthetic sample,
chosen ADSR params), ALSOFT wave capture, extract gain(t) by windowed RMS
divided by the sample's flat amplitude; assert attack-knee time and release
slope within +/-1 tick (4.2ms), sustain plateau at (SL+1)*0x800/0x8000 of
peak, exponential phases compared in the log domain. (3) IN-GAME A/B --
MAP000 before/after: release tails visible in the waveform where notes now
decay instead of cutting rectangular; by-ear confirmation. HONEST
LIMITATION: the proof standard is "matches the psx-spx-documented algorithm"
(itself derived from hardware tests); a real-console or
DuckStation-reference A/B is an optional stretch, not the milestone gate.

PRICING: BOUNDED backend pass, M2-sized; ZERO decomp (retail's side is
complete and already proven byte-exact). Split: (i) translator plumb ~30
lines (port_main.c); (ii) envelope generator + state machine + gain
composition + key-off/status rework ~150-200 lines in PsyX_SPUAL.cpp --
PATCH-MANAGED (extern/ is gitignored; new psycross_sound_adsr.patch beside
the six existing patches; the C++ linkage trap applies to any new export --
extern-C decl + nm/stubs.c check); (iii) proof tooling ~100 lines (Python
SPU-curve model, envelope extractor, probe ADSR-param hook). Phases: 1 =
plumb + machine + per-tick gain + curve-match proof (THE milestone,
first-audible-fidelity); 2 = per-sample callback path ONLY if phase 1
measures audible stepping; 3 = the other fidelity causes (ADPCM decode,
reverb-vs-EFX, resampling) as separate later passes. SHOWSTOPPERS: none --
both candidates checked and cleared (no mix-stage needed for phase 1;
params unplumbed but cheap to plumb). Risk register: OpenAL-soft's
gain-smoothing behavior is assumed, and phase 1's capture measures it.
Evidence: scoped (read-only trace, spec + backend + translator + retail
flush all read; no implementation)
Last verified @ HEAD of this commit

### ADSR fidelity phase 1 LANDED: hardware ADSR envelope live, release tails in-game

The scoping map above is implemented (psycross_sound_adsr.patch + the
translator plumb). The backend now runs the psx-spx counter machine
per voice: {phase, level, counter} advanced at the FULL 44100Hz envelope
rate in per-tick batches (240Hz translator tick, fractional accumulator --
long-run rate exact), rates read LIVE from the last-flushed ADSR1/ADSR2
words (mid-note changes apply). KON = level 0 + Attack (the source starts
at composed gain 0 and ramps); KOFF = Release with the source KEPT PLAYING
until level 0, then stopped -- the release tail, replacing the legacy hard
alSourceStop rectangle. Composed gain has ONE writer
(ApplyVoiceComposedGain = baseGain * level/7FFFh; the volume path stores
baseGain and routes through it; last-value suppressed). Raw ADSR words are
plumbed at KON + change-detected in the continuous flush.
SpuGetVoiceEnvelopeAttr/SpuGetVoiceEnvelope are REAL (backed by the
generator) -- fixing the latent bug where seq cmd 0xFF (free-voice-on-
envelope-decay) branched on uninitialized stack through the auto-stub.
ALC_REFRESH=240 requested so property updates track the tick cadence.

THREE-LEVEL CURVE-MATCH PROOF:
(1) UNIT -- the production generator (PsyX_SPUAL_AdsrDebugCycle drives the
exact runtime code) vs an INDEPENDENT Python transcription of the psx-spx
pseudocode: 8-entry rate/mode matrix covering linear/exp attack (all three
>6000h slowdown legs), decay, sustain hold/increase/exp-decrease,
never-step rates, small-increment and clamp-to-1 counter legs, lin/exp
release -- ~1.3M cycles, 14,892 sampled points, diff EMPTY (cycle-exact).
(2) CAPTURE -- one note on a constant-|amplitude| synthetic ADPCM square
with known params (lin attack s10 = 53.1ms, decay s7 to SL7 = plateau
0x4000, lin/exp release s12): attack knee 50.6ms vs model 50.4ms (delta
0.1ms); attack+decay+sustain residual vs the 240Hz-quantized model 1.84% of
peak; release SHAPE residual vs the model trajectory 0.42% (linear) / 1.25%
(exp) of plateau; envx state samples matched the closed form exactly
(30870 @ 50ms = 14 steps x 2205 cycles).
(3) IN-GAME -- MAP000 60s A/B vs the 76a4108 baseline: hard-cut cliffs
(>=12dB drop per 5ms window at >5% peak) 54.2/min -> 6.0/min (-89%; the
remainder are legitimately fast-release instruments); max drop 167dB
(digital cut) -> 16.8dB. Tails are audible; captures saved (session
scratchpad adsr_before/after_map000.wav).

STEPPING MEASUREMENT (the phase-2 trigger): at tick-resolution rendering
the max attack-ramp step is 2.78dB (2ms windows, >10% peak) and OpenAL-soft
fades linearly within each period -- piecewise-linear envelope, NO
discontinuities. At DEFAULT backend periods (~20ms) the 240Hz updates
coalesce (measured: knee 34ms vs 50.4, crest -14%) -- the ALC_REFRESH=240
hint recovers most of it (knee 45.3ms, release ramp restored). VERDICT:
phase 2 (per-sample mix stage) NOT warranted by measurement -- remaining
granularity is a playback-device period property, not an envelope defect.

Documented fidelity decisions: exp-decrease scales the step via
arithmetic >>15 (floor) -- psx-spx writes /8000h, but truncation would
stall exp release above zero while floor keeps negative steps <= -1 and
terminates, matching hardware-verified emulator cores; shifts >26 follow
the documented formula (psx-spx notes real hardware degrades oddly there;
games do not use them); the decomposed Psy-Q rate attrs (SPU_VOICE_ADSR_AR
family) stay unimplemented -- the game programs raw register words only,
and a half-faithful repack would be silent wrongness; the captured
plateau/peak reads ~0.53 vs the ideal 0.498 because the 6ms 7FFFh crest
spans ~1 mix period and under-renders ~5% -- a rendering artifact, the
envelope state is exact.

Validation: slus byte-exact (d004692f... unchanged; zero src/ changes);
all 8 sound probes PASS post-change; five-map tripwires EXACT
(addr-normalized, session before/after logs); TSan MAP000 clean on the
envelope path (advance runs under the same gate bracket + backend mutex as
the pre-existing attr writes). Linkage verified (all five new exports
unmangled T, zero in stubs.c). Remaining fidelity causes: ADPCM decode,
reverb-vs-EFX, resampling (phase 3, separate passes); per-note ADSR is
DONE.
Evidence: proven (3-level curve-match: cycle-exact unit diff + capture
knee/plateau/release-shape + in-game cliff A/B)
Last verified @ HEAD of this commit

## Audio fidelity: ADPCM-decode + Gaussian-resampling scoping map (cause #2, priced, no implementation)

SCOPING PASS (read-only; fidelity cause #2 after ADSR landed). Verdict up
front: the ADPCM RECONSTRUCTION is structurally correct and is the SMALL
part; the audible gap in this cause is (1) the SPU's GAUSSIAN INTERPOLATION
being absent -- pitch resampling is delegated to OpenAL-soft's cubic
resampler -- and (2) loop-point defects. And the honest scope-reshaper: the
Gaussian fix REQUIRES the per-voice streaming pipeline (the mix-stage
architecture ADSR phase 2 deferred) because mid-note pitch changes rule out
any key-on-time pre-render.

(A) GROUND TRUTH (psx-spx SPU chapter, "SPU ADPCM Samples"/"SPU ADPCM
Pitch"): 16-byte blocks -- header byte = shift(0-3)/filter(4-6), flag byte
bit0 LoopEnd (set ENDX + jump to repeat address), bit1 LoopRepeat (0 WITH
bit0 = End+Mute: jump + force Release + env 0), bit2 LoopStart (latch
current addr as repeat addr); codes: 0/2 continue, 1 End+Mute, 3
End+Repeat. Decode: s = clamp16((nibble<<12 >> shift) + (f0*prev +
f1*prev2 + 32)>>6), filters {(0,0),(60,0),(115,-52),(98,-55),(122,-60)}/64
(the "same as CD-XA" cross-reference; the backend's own K0/K1 floats are
EXACTLY these fractions -- 0.9375=60/64, 1.796875=115/64, 1.53125=98/64,
1.90625=122/64, -0.8125=-52/64, -0.859375=-55/64, -0.9375=-60/64); shift
13-15 behaves as shift 9 (XA-note edge, encoders emit 0-12). PITCH: 16-bit
counter step = VxPitch (1000h = 44100Hz, step clamped to 4000h = 176.4kHz);
counter bits 12+ select the sample within the block, bits 4-11 are the
8-bit GAUSSIAN INDEX. Interpolation (the "SPU sound"): out =
(gauss[FFh-i]*oldest + gauss[1FFh-i]*older + gauss[100h+i]*old +
gauss[i]*new) each SAR 15, over the 512-entry table (fully transcribed in
the spec; captured to the session scratchpad psxspx_spu.md).

(B) BACKEND CLASSIFICATION (PsyX_SPUAL.cpp @ e96f4cb):
- ADPCM reconstruction (vagToPcm, 583-598): APPROXIMATED-CORRECT. Exact
  coefficient VALUES as floats; structure right (nibble sign-extend, two
  prev taps). Diverges in numerics only: float accumulation with round()
  instead of the integer (+32)>>6 path, pow(2,12-shift) with NO shift 13-15
  clamp, prev-state carried in float. LSB-level -- inaudible alone, but
  blocks sample-exact proof.
- Gaussian interpolation: ABSENT -- DELEGATED. Decode-at-keyon renders the
  whole chain to PCM tagged 44100 (748), AL_PITCH = pitch/4096 does the
  resampling through OpenAL-soft's explicitly-selected CUBIC resampler
  (434: AL_SOURCE_RESAMPLER_SOFT=2). Cubic is brighter with different
  image rejection than the SPU's soft 4-tap Gaussian -- THE character
  difference, and it touches every pitched note (i.e. essentially all).
- Loop flags (decodeSound, 635-684 + 752): WRONG-in-general. LoopStart
  latches at k+26 ("FIXME: is that correct?" in-source) = the END of the
  flagged block; hardware latches the block START -- loop start lands one
  block (28 samples) late. The loop_addr adjustment (752: loopStart +=
  loop_addr - addr) adds a BYTE delta to a SAMPLE index (should be
  bytes/16*28) -- benign only in the common loop_addr==addr case.
  End+Mute (code 1) is simplified to play-to-end/no-loop; hardware jumps +
  forces Release + ENDX (matters more now that ADSR release is real).
- Pitch-step clamp (4000h): absent (AL_PITCH unclamped). Rare; note only.
IMPACT ORDER within this cause: Gaussian >> loop defects > decode numerics
> pitch clamp.

(C)/(D) FIX SHAPE -- two phases, and phase B is the mix-stage question
answered FOR REAL:
- Phase A (bounded, no architecture change): rewrite the decoder
  integer-exact (hardware semantics incl. shift-clamp + (+32)>>6 +
  int-state + clamp16), fix loop-block semantics (latch at block start,
  sample-unit loop math), keep OpenAL cubic. ~80-120 backend lines. Gets
  sample-exact PCM + click-free correct loops; does NOT get the Gaussian
  character.
- Phase B (the lever; scope-reshaper): per-voice STREAMING synthesis via
  AL_SOFT_callback_buffer -- the mixer-thread callback walks the SPU pitch
  counter (step = live voice pitch, clamped 4000h; bits 4-11 Gaussian
  index), decodes ADPCM blocks on demand with hardware loop/ENDX/End+Mute
  semantics, applies the 4-tap Gaussian, outputs fixed-rate 44100 PCM;
  AL_PITCH pinned 1.0. WHY streaming is REQUIRED: sequence pitch changes
  mid-note (vibrato/portamento modulate pitch per tick), so any
  pre-rendered resample at key-on is wrong the moment pitch moves --
  per-sample generation is the only faithful shape. Composition with ADSR:
  clean -- the per-tick composed AL_GAIN (envelope x volume) rides on top
  of whatever the source plays; pan/reverb sends unchanged; KON resets the
  stream cursor where UpdateVoiceSample sits today. Mid-note pitch actually
  IMPROVES (read live per callback chunk vs AL_PITCH quantized at device
  periods). Once the callback pipeline exists, per-sample ADSR (the
  deferred phase 2) becomes a cheap optional upgrade inside the same loop.
  Threading: the callback runs on the mixer thread -- per-voice state
  handoff needs short g_SpuMutex sections or a per-voice seqlock (design
  gate, not a stopper). Emscripten lacks the extension -- keep the legacy
  decode-at-keyon path as a compiled fallback.

(E) PROOF (same bar as ADSR -- matches the spec, sample-exact where
possible): (1) unit decode: hand-built ADPCM vectors (all 5 filters, all
shifts incl. 13-15, clamp edges) + a real WDS instrument (bytes already
readback-proven in SPU-RAM) through the production decoder via a debug
export vs an independent Python integer decoder -- diff EMPTY,
sample-exact. (2) unit Gaussian: the 512-entry table + interpolation at
swept counter positions, C vs Python sample-exact; end-to-end fixed-pitch
resample of known PCM, sample-exact. (3) capture: single-note at
pitch != 1000h, FFT -- alias/image line positions and levels must match the
Python-predicted Gaussian spectrum (current cubic capture is the A/B
baseline); loop-click test on a sustained looped instrument (before:
loop-rate click harmonics from the off-by-a-block start; after: clean).
(4) in-game MAP000 spectral tilt A/B (Gaussian rolls off highs vs cubic)
+ ear. Real-hardware/emulator reference optional, not the gate.

PRICING: ZERO decomp (pure spec->backend, ADSR-shaped). Phase A: ~half an
M2 (decoder rewrite + unit proof + loop capture). Phase B: M3-sized
(~300-450 backend lines: callback pipeline, Gaussian table+counter,
threading, fallback; + probe/analysis extensions) -- bigger than ADSR
phase 1, bounded, not open-ended. RECOMMENDATION: run as one combined pass
(A then B -- B's proof harness subsumes A's); B is where the audible
character lives, A alone won't move the user's complaint much.
SHOWSTOPPERS: none hard. The scope-reshaper is explicit: faithful Gaussian
NEEDS the streaming mix-stage (AL_SOFT_callback_buffer -- available in
OpenAL-soft >= 1.22 natively); the one design-risk gate is mixer-thread
state handoff discipline. After this cause: reverb-vs-EFX and any residual
resampling polish remain (causes #3/#4, separate passes).
Evidence: scoped (read-only trace @ e96f4cb, spec captured + backend
classified line-by-line; no implementation)
Last verified @ HEAD of this commit

## Map014 intro scene renders the wrong geometry (back of Fei's head, not the fire painting)

User-confirmed live at f2d8778: MAP014's intro scene -- the camera zoom-in
on the fire painting, with Fei -- renders the WRONG content. The shot shows
the back of Fei's head where the fire painting the camera is supposed to
zoom into should be. Audio on the same scene is correct (the map's music and
its deterministic boot-script SFX cue were both proven this session); the
defect is visual only.

DISTINCT from the fea685a fix: that commit resolved the Map014 intro
"overdraw smear" (func_8002E688 used RotTransPers4's return value instead of
retail's min(SZ0..SZ3) >> D_80050100 for OT-bucket assignment -- a
depth/ordering defect). The present bug is wrong CONTENT/geometry, not
ordering: a different class -- candidate causes include a separate
render-path defect, wrong model/object selection, or a camera/transform
issue in the zoom sequence. Needs its own investigation; do not assume the
fea685a mechanism.

Repro: `./scratchpad/run_map014.sh` (or XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0
XENO_FIELD_MAP=14 XENO_FIELD_ENTRANCE=0) and watch the intro zoom sequence.

ROOT CAUSE (diagnosed, gdb-only, no code changes; fix is a separate pass):
the intro closeup is a DEDICATED SCREEN-FILLING PAINTING OBJECT -- actor 24,
the "fiery painting" static model -- positioned directly in front of the
scripted closeup camera. Its quads project EXACTLY where retail wants them
(a grid spanning the full screen; per-quad probe at f60: xy from (-64,-116)
to (415,247)) but nearly every screen-filling quad carries GTE FLAG
0x80021000 (bit17 perspective-divide overflow -> bit31) from RTPT on its
near-plane vertices, and ModelPrimQuadFT4Variant0's `rtptFlag < 0` gate
(game_overrides.c:1079) rejects them. Only small edge slivers survive --
the stray fire patches visible at the frame edges early in the scene. With
the painting gone, the raw 3D scene shows through: the script camera
(eye=(141,-75,-431) at=(23,-50,-548), held frames 0-211, script-armed and
faithfully applied) sits 35 units behind the script-placed player at
(115,-1,-455), whose head therefore fills the shot.

PROOF (runtime experiment): clearing the RTPT/RTPS FLAG verdicts for actor
24's quads only (gdb, no code change) recovers the retail shot -- the fire
painting fills the frame through the closeup and the pull-back still works.
Evidence frames: scratchpad/m14_bug_f120_head.png (defect),
m14_rootcause_forced_f120/f180_painting.png (forced-accept recovery),
m14_hidefei_current_f120_wall.png (hide-player intermediate: without the
billboard the camera sees only a wall corner -- the painting content CANNOT
come from the room BG/easel from this camera).

ELIMINATED with evidence: harness-entry placement (spawn records are
(165,-25)/(178,313); the player's (115,-1,-455) is script-placed); camera
mis-decode (arm values logged at FieldScriptStartCameraMovement match the
held camera); BG-angle (cur==target mod 0x1000); actor-visibility gate
(func_800AAA74 passes, dispatch confirmed at f30/f120); stubbed opcode
handlers (zero field-script stubs fire in the current build);
XENO_E688_IGNORE_FLAG (no effect -- it bypasses func_8002E688's gate, but
actor 24 draws through ModelPrimQuadFT4Variant0, a different walker with an
unbypassed FLAG gate).

FIX-PASS QUESTION (the one fork left): retail hardware demonstrably shows
this scene, so either (a) real-GTE RTPT does not overflow the divide here
(port-side SZ3 smaller than hardware's -- a transform/precision divergence
upstream), or (b) PsyX's GTE divide-overflow semantics set bit17 where
hardware wouldn't (UNR divide emulation), or (c) retail's variant-0 FT4
walker asm does not reject on this FLAG pattern (compare the original asm
behind D_8004FE50's proc entry). Next probe: dump SZ0..SZ3 for the rejected
quads vs h=0x80 threshold (overflow iff h/2 >= SZ3 per psx-spx), and read
retail's walker asm gate. The scene's script choreography (extended opcodes
0x26/0x27 = FieldDistortionInitialize/control -- the painting ripple)
composes ON TOP of the billboard via framebuffer feedback and needs no
separate fix.

Related history: the same intro scene previously exposed fea685a (OT-bucket
min-SZ), the BG-angle sign-extension fix (misc2.c:1207), and the
"floating geometry" era (m14_cu_* captures: scattered BG fragments --
resolved by the intervening matrix/CLUT fixes; today's room renders
coherently). This scene is the port's render torture test: one shot
exercises the scripted camera, the BG re-projection, actor models, the
closeup billboard, AND the distortion feedback.
Evidence: root-caused (per-quad walker probe + forced-accept recovery
capture); fix not yet implemented
Last verified @ 4ed19a9

## Map143 dialogue path crashes in the shared tile/sprite renderer

Map143 has legal entrances `{0, 1}`. From entrance 0, real d-pad input can move
the player to shop actor 14 and a real Circle/talk input selects that actor's
authored talk routine. The routine opens dialogue, then remains at
`WAIT_DIALOG` through at least frame 500 even after authentic close inputs. The
unmodified renderer SIGSEGVs at frame 358, before the script reaches its shop
opcode and before any menu entry point or menu oracle stub executes.

The fault stack is:

```text
GR_UpdateVRAM
AddSplit
BeginTexturedSplit
ProcessTileAndSprt
ParsePrimitive
DrawOTag
func_8007554C
FieldMain
```

The frame-340 pre-fault capture at `/tmp/menu143_prefault.png` is a coherent
field scene with the dialogue box still open; it is not a menu frame. This is
therefore an independent field renderer/dialogue-path defect, not a menu
failure. `GR_UpdateVRAM` and the tile/sprite processing path are shared, so the
same mechanism may affect other maps and needs a separate diagnostic pass.

Repro: build the normal port, then run
`env XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 XENO_FIELD_MAP=143
XENO_FIELD_ENTRANCE=0 SDL_VIDEODRIVER=x11 DISPLAY=:0
pc_port/build_native/xeno-port`; use the d-pad to move from the entrance to
actor 14 and press Z (the port's Circle/talk mapping). The authored interaction
opens the dialogue and reaches `WAIT_DIALOG`; capture under GDB to obtain the
stack above. Entrance 1 is structurally legal as well, but entrance 0 is the
verified reproducer.
Evidence: observed (authentic input and authored actor script; fault occurs
before menu dispatch)
Last verified @ e3f4b49

## Normal menu is cold-boot reachable but render is blocked by the stubbed func_800799D4 (live repro)

The scan's "normal menu is cold-boot reachable" is now confirmed at runtime, and
the render block is isolated to a single stub. Live repro (deterministic,
captured via the nested-X harness below):

- map005, entrance 0. Walk the player to actor 15 (the save actor) and press
  Circle. Its authentic talk routine (script 15, routine 2) runs
  `FE 99 00 | FE 55` at script PC `0x68D`: set-menu-open-arg then
  OP_OPEN_NORMAL_MENU, which sets the menu request `D_800ADB64 = 0x00` (mode 0)
  and bumps the wait counter `D_8004F350` to 1.
- `src/field/main/main.c:595` then calls `func_800799D4` — an oracle stub
  (`pc_port/build_native/stubs.c:610`). It returns without loading the menu
  overlay, without running `MenuMain`, and without clearing `D_8004F350`.
- The next opcode `FE 87` (OP_WAIT_MENU, `func_800936E4`) at script PC `0x68F`
  spins: it advances only when `D_8004F350 == 0`, else decrements the IP by 1.
  Because the stub never zeroes the counter, actor-15 script slot 0 parks at
  `currentIP = 0x68F` forever and the player freezes.

Measured at the freeze: `D_800ADB64 = 0xFF` (main.c:599 reset it after the stub
returned), `D_8004F350 = 1`, `D_80059460 = 0` (mode 0), `g_Menu = NULL`, actor-15
slot 0 = `(IP 0x68F, scriptId 2)`. Successive framebuffer captures are
byte-identical — the field is frozen, no menu overlay is drawn. `func_800799D4`
is hit exactly once. This is the first menu render target with a live
reproduction: porting `func_800799D4` (retail body at asm `0x800799D4-0x8007A448`)
is what loads the overlay, runs `MenuMain`, and clears `D_8004F350` so WAIT_MENU
advances.

UPDATE (validated in working tree, pending commit): func_800799D4 is now
decompiled (near-match, 653/671, single register-spill residual) with a
port-side sizing fix, and the stub gate above is CLEARED end-to-end on the
same repro. Two findings supersede the paragraph above once that lands:

1. Retail sizes the overlay buffer as the fixed-map gap
   `(D_800ADB30 & 0xFFFFFF) - 0x1C5008` (overlay region 0x801C5000, next
   allocation above it). Against the port's host-pointer `D_800ADB30` that
   yields a ~5MB bogus size -> `HeapAlloc` fails -> `GameHandleError(130)`.
   The host-correct size is what the buffer actually receives — the decoded
   size of menu overlay file `(menu id + 5)` — the same derivation retail's
   own `D_8004F370` branch and `func_80077884` (main.c:171) use.  Fixed under
   `XENO_PC_PORT` at both fixed-map-idiom sites in the function (overlay
   alloc, and the latent `D_800ADB20` restore at the tail).  Matching form
   verified byte-identical before/after (the ifdef does not leak into mwcc).
2. With the sizing fixed, one authentic talk completes the full round trip in
   ~7s: alloc OK, `MenuMain` RUNS (`g_Menu` non-null, measured `0x5813b0`),
   `MenuExecute` mode 0 hits stub `func_801C62A8` (no menu visuals — the real
   render gate), post-menu tail runs (`func_800798BC`/`func_800A2488` stubs
   log), `D_800ADB64 -> 0xFF`, `D_8004F350 -> 0`, and actor-15's script
   advances past `0x68F` to its STOP.  The mode-0 body `func_801C62A8` is the
   next menu porting target, same repro.

SCOPE CORRECTION (func_801C62A8 is NOT a one-function port): reading the retail
asm from `disc/menu.bin` at offset 0x12A8 shows `func_801C62A8` is an
84-instruction **dispatcher**, not a self-contained render.  It calls setup
`func_801C5F10`/`func_801C7B0C`, sets `g_Menu->shouldDrawMenu` (+0x327) and
`unk32A` (+0x32A), then switches on the menu-mode byte `D_80059460` and
dispatches to overlay-internal sub-functions, always finishing at
`func_801C5FE4`.  The mode-0 path (`D_80059460 == 0`, the map005 repro) calls
`func_801C5F10 -> func_801C7B0C -> func_801D2D38 -> func_801C55A0 ->
func_801C5FE4` — **all five are unported overlay code**, and the actual window
/ content drawing lives in them, not in `func_801C62A8`.  So porting
`func_801C62A8` alone renders nothing and exposes those five as the next gates.

Two premises the prior report carried were wrong, corrected by reading the code:
1. `func_801C62A8` is the ENTRY of the mode-0 render call tree, not "the last
   gate before pixels."
2. **The normal-menu overlay has no decomp infrastructure at all.**  It lives
   in `disc/menu.bin` (archive dir 0x10 / file 5, VRAM base 0x801C5000, 153864
   bytes, stored uncompressed — byte-identical to `disc/menu.bin`).  Unlike
   `member_change_menu`/`shop_menu`, `menu.bin` is NOT in `gears.toml`'s overlay
   list, has no `config/menu.yaml`, no `asm/menu/` split, no `src/menu/`, and no
   matching target.  There is therefore nothing to `objdiff {}` against and no
   `asm/menu/func_801C62A8.s` to `INCLUDE_ASM` — neither the matched-decomp path
   nor the d88f13c coexistence pattern is available until the overlay is brought
   up.

Rendering the normal menu is thus a two-part project, not a single-function
pass: (a) bring up the `menu.bin` overlay TU (add to `gears.toml`; write
`config/menu.yaml` mirroring `config/member_change_menu.yaml` — VRAM 0x801C5000,
gp 0x80059170; seed symbols; splat-split; linker + gears integration; matching
baseline), then (b) decompile `func_801C62A8` + its mode-0 callee tree
(`func_801C5F10`, `func_801C7B0C`, `func_801D2D38`, `func_801C55A0`,
`func_801C5FE4`, and their transitive callees — the draw code).  The map005
nested-X repro validates each layer (does the window/contents draw yet?).
Evidence: proven (retail asm at menu.bin:0x12A8 read in full; overlay identity
byte-verified against disc/menu.bin; build-config absence confirmed).

New downstream defect, deterministic on the same single-tap repro: after the
menu round trip, field re-entry asserts in `func_800248D4`
(`src/slus_006.64/system/temp1.c:968`) — the port's sprite-animation VM
implements only the >=0x80 opcode dispatch, and post-menu re-entry drives an
actor animation into the sub-0x80 frame/delay fallthrough.  Observation
consistent with the stubbed `func_800A2488` party-sprite reload: the
after-cycle frame restores NPC sprites but the player sprite is missing
(`scratchpad/map005_aftercycle.png`).  Hypothesis only — trace before porting.

Reusable nested-X harness (no Wayland focus fight): `Xvfb :99 -screen 0
1024x768x24`, launch the port with `DISPLAY=:99`, then drive authentic input
with `xdotool windowfocus --sync <win>; xdotool keydown/keyup <key>` (plain
XTEST to the focused window updates SDL's `SDL_GetKeyboardState`; `--window`
synthetic events do NOT and are silently dropped). `scratchpad/run_map005.sh`,
`scratchpad/map005_drive.sh` (empirical d-pad calibration + greedy walk to
actor 15), and `scratchpad/map005_attach_probe.gdb` reproduce the capture.

Repro: `Xvfb :99 -screen 0 1024x768x24 &`; `XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0
XENO_FIELD_MAP=5 XENO_FIELD_ENTRANCE=0 SDL_VIDEODRIVER=x11 DISPLAY=:99
pc_port/build_native/xeno-port`; run `scratchpad/map005_drive.sh` to walk+talk;
attach `scratchpad/map005_attach_probe.gdb` — actor-15 slot 0 sits at IP 0x68F,
`D_8004F350 == 1`, `[stub] func_800799D4` appears once in the port's stderr.
Evidence: proven (authentic input, measured freeze state, frozen framebuffer)
Last verified @ c08aaa1

## Member-change/shop menus are state-gated on all 730 maps; normal/load menus are cold-boot reachable

The whole-archive field-script reachability scan proposed by the earlier
surveyed-shop-branches entry is done and authoritative (730/730 maps decoded,
102,382/105,830 routines walked, 3.26% abort rate, all degenerate-scan guards
passed). It corrects two earlier records:

**Correction 1 — there are EIGHT menu-request opcodes, not two.** Prior work
tracked only `FE 56`/`FE 58`. The full set in `g_FieldScriptVMHandlers2`
(dispatch chain proven from this repo: `g_FieldScriptVMHandlers[0xFE]` →
`FieldScriptVM2Run` at `src/field/main/misc8.c:2758` →
`g_FieldScriptVMHandlers2` → handlers at `src/field/main/misc11.c:286-335` set
request `D_800ADB64` → `src/field/main/main.c:595` → `func_800799D4` (retail
asm `80079E40`: `D_80059460 = D_800ADB64 & 0x7F`, `jal MenuMain`) →
`MenuExecute` at `src/slus_006.64/system/menu.c:236`):

- `FE 55` (2B) mode 0 normal menu; `FE 56 aaaa` (4B) mode 1 member-change;
  `FE 57` (2B) mode 2 load-game; `FE 58 aaaa` (4B) mode 3 shop;
  `FE 59 aaaa` (4B) mode 4 normal+`ChangeGameState(1)`; `FE 5A aaaa` (4B)
  mode 5 overlay; `FE CF aaaa bbbb` (6B) mode 1 member-change + map jump;
  `FE DA` (2B) mode 6.
- Noah's `fieldScriptOpcodes_EX` table does NOT register 0x56/0x58/0xCF/0xDA;
  this repo's asm is the only authority for those. The normal menu also opens
  by button press (`src/field/main/main.c:601`, pad bit 0x10 → request 0x80),
  engine-level, gated only by `D_800B21D0` — independent of scripts.

**Correction 2 — "no menu is cold-boot reachable" was too broad.** The split:

- Member-change (`FE 56`/`FE CF`) and shop (`FE 58`): STATE-GATED on all 730
  maps, no exceptions. Every site is behind scenario-var-0/map-flag/party
  checks, a state-gated scheduling chain, or a state-conditional actor-disable
  in an init/manager routine (map205's member-change NPC and map290's
  party-select scene both fall to that last class). The earlier 7-map hand
  audit generalizes. Reaching these authentically requires story state
  (scenario var 0, map flags, and party composition restored through the
  `g_pGameState+0x1930` script-memory block) or an explicitly labeled
  menu-test scaffold — not an ad hoc flag poke.
- Normal menu (modes 0/4/5) and load-game menu (mode 2): UNGATED at 93 sites.
  91 are talk routines (walk up, press Circle); 88 of those have no soft
  condition beyond the talk. Cleanest live target: map005 actor 15 talk
  routine, byte-verified `FE 99 00 | FE 55 | FE 87 | 00` (set save-arg, open
  normal menu, wait menu, stop). Alternative: map490 script 0 routine 1 opens
  the load-game menu from an auto routine behind a pad-input check.

The 12 raw `FE 5x` byte pairs in code no modeled entry reaches (e.g. map247's
three copy-pasted dialog-menu blocks with only the first referenced) are
accounted as dead leftover variants; none is a shop opcode, and each is listed
with hex context in the scan JSON.

Scan artifacts (untracked, session convention): `scratchpad/scan_field_script_menus.py`
(scanner, evidence citations inline), `scratchpad/build_length_model.py` +
`scratchpad/length_model.json` (per-opcode length/control-flow model,
this-repo-first with Noah cross-check), `scratchpad/full_scan.json` +
`scratchpad/full_scan_stdout.txt` (full site table and health).

Repro: `python3 scratchpad/scan_field_script_menus.py --json /tmp/rescan.json`
(3s over `disc/disc1.bin`); health line must show 730 maps, abort ≤5%, and the
site table splits UNGATED 93 / STATE-GATED 342 / PRESENCE-GATED 20 /
GATED-SCHEDULING 5 / INIT-DISABLED 1.
Evidence: proven (static scan; dispatch chain and both member-change survivors
hand-verified at byte level against this repo's asm)
Last verified @ e402f79

## Unported member-change and shop menu overlays

Both menu `misc.c` TUs now compile after the retail-proven `POLY_FT4` window
border correction and the `ShopMenuBuyMenu` declaration fix. Their compile
errors had been masking 15 link-reachable `INCLUDE_ASM` dependencies: three in
the member-change overlay and twelve in the shop overlay. Matching ELFs now
exist and there is no port-symbol collision. Both TUs now compile and link
normally with generated oracle stubs. Port-only routing reaches the real menu
entry bodies; live coverage of the 15 inner stubs now awaits an authentic shop
state as tracked above.

Repro: compile both menu TUs with the port GFLAGS/INC and compare their undefined
symbols against their `INCLUDE_ASM` declarations, or remove the holds locally
and verify that the typed stub manifest regenerates and links.
Evidence: proven
Last verified @ 5b68551

## Work-list runtime routing: decomp source vs. host-safe port override

`src/slus_006.64/system/work_list.c` now compiles, but it cannot be linked
alongside `pc_port/src/work_list_port.c`: nine definitions overlap. The port
file is a deliberate, partial host-layout override, not a full replacement.
It uses PSX-layout 0x1C entries with 32-bit stored pointers because the original
decomp layout is unsafe for the 64-bit host; it was added to restore the
runtime-critical work-list paths that were previously stubbed.

The runtime-layout decision is now made: keep `work_list.c` excluded as
matching/reference source and make `work_list_port.c` the canonical native
owner. Exclusion alone is not completion; remaining exports must be ported into
the packed-u32 implementation as live paths require them. See
[`Port-Coexistence-Architecture`](docs/wiki/Port-Coexistence-Architecture.md).

Repro: compile `src/slus_006.64/system/work_list.c` with the port GFLAGS, then
compare its defined symbols against `pc_port/src/work_list_port.c`; the shared
definitions produce duplicate-definition link errors if both objects are linked.
Evidence: proven (source comments, symbol comparison, and runtime-recovery
history)
Last verified @ fdd86e7

## Port-only source compilation is not fail-closed

The game-TU compile loop aborts on unexpected failures, but the port-only loop
only prints `FAILED` and continues. A failed `game_overrides.c`,
`archive_port.c`, `work_list_port.c`, data source, or other port source is
omitted from `GAME_OBJS`; the trial link can then satisfy its missing symbols
with generated stubs. `port_main.c` likewise reports a compile failure without
aborting or deleting a prior object, allowing a stale entry object to be linked.

Repro: inspect the port-source loop and `port_main.c` compile command in
`pc_port/build_port.sh`, or induce a temporary compile error in an isolated
worktree. The driver reaches the trial link instead of exiting at the failed
compile.
Evidence: proven (control-flow inspection)
Last verified @ fdd86e7

## Unimplemented sprite-animation opcodes in func_800248D4

`func_800248D4` is the **sprite animation-script dispatcher**, not the field
script VM. Its dedicated unimplemented set is now `0x85, 0x8E, 0x98, 0xC8,
0xD4, 0xE2, 0xFA`. Opcode `0xBE` was implemented from the shared retail
`0x80` handler at `0x80024A84-0x80024B9C`.

The strict static scanner in
`tools/scripts/psx/scan_field_anim_opcodes.py` decoded all 730 field maps,
3,234 per-map sprite packages, and 16,382 animation entries with zero aborts.
Only the now-implemented `0xBE` is reachable from a per-map animation entry:
seven distinct sites in Map047 package 0 animations 0/1/2, Map048 package 0
animation 2, and Map334 package 0 animations 0/1/2.

The other seven are **not reachable in any per-map animation package**. Do not
call them unused: global party, battle, and special-animation packages were not
part of this field-map scan and remain a real coverage gap.

Repro: `python3 tools/scripts/psx/scan_field_anim_opcodes.py --json
scratchpad/field_anim_opcode_scan_all.json`; expect 730 maps, 3,234 packages,
16,382 fully-decoded entries, zero aborts, and seven reachable `0xBE` sites.
Evidence: proven for per-map packages; global animation-package coverage open
Last verified @ 4d7cb57

## CompMatrix PsyQ decomp match

Audit is DONE (semantics verified vs retail 0x8004931C-0x80049478).
The MATCH is outstanding — handwritten GTE sequence, codegen-focused work.
Evidence: proven (audit); match not attempted
Last verified @ ed61298

## Map015 entrance 0 — func_8008399C assertion

Repro: launch Map015 entrance 0; assertion fires, map not runnable
Evidence: observed (surfaced during ABR scene sweep)
Last verified @ ed61298

## LIBGTE.C invalid UTF-8 byte

Blocks patch-editor modification of the file. Maintenance item.
Repro: iconv -f utf-8 -t utf-8 < pc_port/extern/PsyCross/src/psx/LIBGTE.C > /dev/null
Evidence: proven
Last verified @ ed61298
