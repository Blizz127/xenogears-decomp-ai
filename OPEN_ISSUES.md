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
