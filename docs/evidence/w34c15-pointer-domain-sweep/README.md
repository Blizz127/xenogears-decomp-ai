# W34C15 — pointer-domain sweep

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `848d23d1fd920978d639ee15570da46dae33ecbf`
- Date: 2026-08-27

## Verdict

`DEFECT=3`, `SPLIT_BRAIN=3`, `UNREACHABLE=1`,
`UNDETERMINED=1`.

No production file was changed.  This is a static enumeration and
classification pass; every repair is deliberately left for its own bounded
rung.

The most urgent result is a family of three MIPS-stack transcription defects.
Native stack arrays are truncated to `u32` and passed to helpers which interpret
the value as a guest address and rebase it through `PSX_ADDR`.  One sits in the
forced route's slot-9 callback.  Two sit in state-specific arms of the forced
route's slot-7 callback.  The callbacks are live; execution of the individual
arms was not asserted by this read-only sweep.

## Scope and method

The mechanical sweep covered every hand-maintained C/C++ source or header under
`pc_port/src`: **259 files** (`*.c`, `*.h`, `*.cc`, `*.cpp`, `*.hpp`).  It was not
limited to `world_map_*`.

The sweep enumerated:

- all explicit pointer-to-`u32` casts and nearby guest stores;
- all **53 executable `HeapAlloc` call expressions** in `pc_port/src` (after
  excluding declarations, comments, and diagnostic strings), then traced each
  result to its publication, comparison, free, or native-only consumer;
- all **795 `PSX_ADDR` uses**, with a second pass over values loaded from guest
  memory and subsequently converted to or compared with native pointers;
- the guest tables and native object graphs implicated by those candidates;
- separately generated symbol storage where it was needed to establish a
  host/guest twin (`pc_port/build_native/stubs.c` was inspected, not included in
  the hand-maintained-source count);
- compiled upstream consumers in `src/` only to establish which side of a
  split-brain global they actually read.

Excluded from the mechanical coverage were `pc_port/tests` (isolated certificate
RAM and intentional mutants), generated/build products other than the symbol
check above, `pc_port/extern/PsyCross`, scratchpad probes, and the full upstream
`src/` corpus.  Upstream functions compiled into the port can therefore contain
additional raw 32-bit pointer assumptions not enumerated here.  This report is a
complete sweep of **port-owned source**, not a claim about every translated or
vendor translation unit in the executable.

## Prioritized findings

### 1. Forced-world callback family

#### F1 — `wm_800914D0` passes a native stack vector as a guest address

- Classification: **DEFECT**
- Store/call site: `pc_port/src/world_map_callback_914d0.c:246`
- Producer: local `u32 delta_vec[3]`
- Consumer: `wm_80093484`, whose `w84_lw/w84_sw` access
  `PSX_ADDR(pos_vec)` and `PSX_ADDR(pos_vec + 8)`
- Route: scheduler slot 9 callback `0x800914D0` is repeatedly dispatched on the
  forced world route.  Its bad call is additionally gated by slot control state
  `3` or `0x10` and a position mismatch; this sweep did not prove those inner
  predicates on HEAD.

Current behavior: the low 21 bits of the truncated native stack address select
an unrelated location in `g_PsxRam`.  The helper wraps that guest alias, while
the host `delta_vec` remains unchanged.  The caller then scales and applies the
unwrapped host values.

Retail expectation: the MIPS stack address is a valid guest address.  A port
repair needs guest scratch/frame storage (the pattern already used by
`wm_80095414` for retail's `0x18($sp)` slot), or a host-native helper contract.

Runtime observation for immediate impact: break at the call with slot state
`3/0x10`, record the raw cast, its `& 0x1fffff` guest alias, and whether the
position-mismatch arm fires.

#### F2 — `wm_8008E0F0` passes a native direction array to a guest-vector helper

- Classification: **DEFECT**
- Store/call site: `pc_port/src/world_map_helper_8e0f0.c:28`
- Producer: local `u32 dir[3] = { rcos(angle), 0, -rsin(angle) }`
- Consumer: `wm_80095414`, whose vector loads are guest-memory loads
- Route: `wm_8008E76C` is the slot-7 cb1 selected by the natural/forced
  second scheduler pass (`EE68=0x4003`, variant 3).  This particular call is the
  callback's slot-control-state-7 arm; execution of that arm is unproven.

Current behavior: `wm_80095414` samples a `g_PsxRam` alias instead of the freshly
computed direction.  It should see the three computed direction words.

Runtime observation for immediate impact: log slot 7's `+0x20` control value at
cb1 entry and stop if it becomes 7; compare the three host words with the three
guest-alias words used by `wm_80095414`.

#### F3 — `wm_80095CD4` gives `wm_80084D00` a native stack output buffer

- Classification: **DEFECT**
- Store/call site: `pc_port/src/world_map_helper_95cd4.c:72`
- Producer/destination: local `u16 attr_buf[8]`
- Consumer/writer: `wm_80084D00`, which stores the selected attribute through
  `PSX_ADDR(out_attr)`
- Later reader: `wm_80095CD4` reads `attr_buf[i / 2]` and passes it to
  `wm_80085418`
- Route: the containing `wm_8008E76C` callback is live; this call is its
  main-dispatch state-2 movement arm.  The arm's execution on the current forced
  schedule was not proven here.

Current behavior: a successful region scan writes a halfword into an unrelated
guest alias, then the caller reads an uninitialized native stack element.  A
zero region count hides the defect.

Retail expectation: `out_attr` points at the MIPS stack.  The existing
`WM_95414_FRAME_ATTR=0x801FFE18` adaptation demonstrates the bounded guest-frame
pattern suitable for a future repair.

Runtime observation for immediate impact: at the state-2 call, record
`0x8009D7E0` (region count), `wm_80084D00`'s return, and the guest-alias store.

#### F4 — AudioManager host symbol and guest twin diverge

- Classification: **SPLIT_BRAIN**
- Host authority: `D_80062528` in generated native storage; written and read by
  `wm_mode_audio_setup` in `world_map_init.c`
- Guest twin: `0x80062528`; read by `wm_8008E76C`'s sound-state arm
- Consumer today: the guest value is passed to local
  `stub_func_8003A89C`, which ignores it
- Route: the slot-7 callback is reached by the forced world route; the sound arm
  is state-gated.  Field code outside the world callback reads the host symbol.

Current behavior: creating/loading the manager updates only the host symbol.
The guest callback can therefore see zero/stale guest data where retail sees the
manager pointer.  There is no current observable effect because the direct
consumer is still a no-op stub, but retiring that stub without resolving the
authority split would activate the bad value.

Queue: resolve the `D_80062528` authority in the same rung that retires the sound
consumer, with a runtime witness of the manager value on both sides.

### 2. Environment-gated world route

#### F5 — `g_GameCurLoadedWDS` is published only to the guest twin

- Classification: **SPLIT_BRAIN**
- Writer: `wm_first_wds_consumer`, `world_map_init.c:6775`
- Guest destination: `0x8006258C`
- Stored value: raw native `SoundWDSEntry *` bits from `SoundLoadWdsFile`
- Native twin: generated host symbol `g_GameCurLoadedWDS`
- Live consumers: `src/slus_006.64/system/temp3.c` and field misc code read the
  native symbol, not `g_PsxRam[0x6258C]`
- Route: not part of the accepted forced route; enabled by
  `XENO_WORLD_FIRST_WDS_CONSUMER` and later historical gate stacks

Current behavior: the guest twin receives a native pointer with no conversion,
while the host twin used by native consumers remains zero/stale.  A guest reader
would also misinterpret the raw native bits unless the field were explicitly
declared native-domain.

Queue: first prove whether the intended authority for the sound subsystem is
host or guest, then publish exactly once to that authority.  Do not simply wrap
the existing store with `PsxMemory_GuestAddr`: that would leave the live native
consumer stale.

#### F6 — `D_8006259C` has an intentional host authority but a stale guest twin

- Classification: **SPLIT_BRAIN**
- Host writer: third-wave allocation in `world_map_init.c:4138`
- Live readers: world audio setup, menu, member-change menu, and field sound
  code use the native `D_8006259C` symbol
- Guest twin: initialized static data at `0x8006259C`; no port-owned guest reader
  was found and the allocation is not mirrored there
- Routes: forced world and several W34C14-exercised field/menu paths use the
  host side successfully

This split is structurally real but currently coherent because every located
live consumer uses the named host authority.  It is not an immediate repair
queue item.  A future guest-side consumer must either establish a synchronized
guest representation or explicitly remain on the host side.

### 3. W34C14 field/kernel paths

No new live domain defect was found in the W34C14-exercised KernelMenu,
Map0/Map1, Lahan, Map014, or W18B paths.

The raw pointer graphs in `game_overrides.c` and `work_list_port.c` are native
object graphs, not guest tables: `SpriteData`, work-list entries, render work
buffers, and their parent/back links are allocated as native objects and read
as native objects.  Their 32-bit representation depends on the canonical
non-PIE/low-address build.  The world scheduler's slot `+0x4C` field is the same
explicit mixed-domain contract: the slot record is guest memory, but `+0x4C`
stores checked raw-low-native `SpriteData *` bits and all consumers treat that
field as native.  W34B73's runtime domain proof independently observed those
low native values.  Classification: **CORRECT**, under the enforced 32-bit-fit
invariant.

### 4. Unharnessed overlay route

#### F7 — overlay model table `D_801E8670` has an unconverted guest-pointer contract

- Classification: **UNREACHABLE**
- Consumer: `func_801E72CC` in `game_overrides.c`
- Declared contract: `D_801E8670` is a packed table of 32-bit PSX pointers;
  entries and their `+4` resource word are cast directly to native pointers
- Why unreachable now: the overlay model-instantiation function
  `func_801E742C` is unported, so the generated host table remains NULL and the
  dereference branch returns early
- Route needed: the model-resource/member-change overlay after its object
  instantiator populates `D_801E8670`

If the future producer writes guest/KSEG values as documented, the consumer
will attempt to dereference addresses such as `0x801xxxxx` directly rather than
`PSX_ADDR`-rebasing them.  Before enabling that overlay, classify the producer's
actual domain and make both table levels agree.  This is real but not urgent
while the only producer is absent.

### 5. Domain-polymorphic archive queue

#### F8 — archive queue accepts either KSEG or raw-low-native `pData`

- Classification: **UNDETERMINED**
- Consumer: `ArchiveReadPsxStreamQueue` in `archive_port.c`
- Behavior: `[0x80000000,0x80200000)` is rebased through `PSX_ADDR`; every other
  nonzero value is cast directly to a host pointer
- Current producers checked in the forced world path publish KSEG values via
  `host_ptr_to_psx_u32`/`PsxMemory_GuestAddr`; W34C14 exercised archive paths
  without a fault

Static inspection cannot prove that every upstream queue producer obeys one of
the two accepted domains.  A runtime census of every nonzero queue `pData`, with
an assertion that it is either KSEG or a native address inside `g_PsxRam`, would
settle this.  No change is queued from the present evidence.

## Correct conversion families

The following high-risk families were traced end to end and classified
**CORRECT**:

| family | publication | consumer/domain |
|---|---|---|
| `0x8009C184` terrain tile slots | `wm_80097DC0` / `wm_80098CC0` use `PsxMemory_GuestAddr` | `wm_8009932C` uses guest offsets; eviction rebases through `PSX_ADDR` |
| `BE08`, `D3C0`, `D7D4` loader buffers | `host_ptr_to_psx_u32` | `wm_8009623C`, `wm_80096328`, `wm_800965A4`, `wm_800966CC` use guest values |
| OT roots `BC38`, `BCB0` | `host_ptr_to_psx_u32` | guest OT clear/link/draw adapters |
| scheduler pool `BE24` | `host_ptr_to_psx_u32` | scheduler/callback record access through guest addresses; field `+0x4C` is explicitly native as described above |
| package mirrors `CD34`, `CD38`, `CD3C`, `BDF8` | KSEG guest values | package consumers rebase with `PSX_ADDR`; created sprite objects are explicitly native |
| common-tail allocations (`978FC`, `8901C`, `865A0`, `85FE0`) | `wm_host_to_psx` | later table traversal remains guest-domain |
| world-init allocation waves, FT4 pools, upload records, object table, random tables | `host_ptr_to_psx_u32`, with round-trip checks on critical tables | guest tables retain KSEG; temporary GPU decode buffers stay native and are never published as guest pointers |

All 53 executable `HeapAlloc` occurrences fall into one of those converted publications or
remain native-only (archive stream objects, decompression scratch, SpriteData /
work-list graphs, temporary GPU buffers, and diagnostic/bootstrap buffers).
No additional direct HeapAlloc-result domain crossing survived tracing.

`D_80050100` was also rechecked because it was the W34C2 split-brain warning:
the live native rendering consumers and `wm_80073B04` now share the host symbol,
while W34C2 initializes the guest twin to the same value.  The guest twin is not
used as a competing live authority.  Classification: **CORRECT (resolved)**.

## Rejected mechanical false positives

- OT/tag stores such as `(u32)(uintptr_t)prim & 0x00ffffff` are PSX GPU link
  encoding, not guest/native pointer publication.
- `world_map_helper_98cc0` declares `ArchiveDecodeSector` with a pointer-shaped
  prototype and casts `0x8009BCD8` to `void *`, but the real compiled function
  takes an integer archive entry index and does not dereference it.  The bits
  arrive unchanged under the current ABI.  The prototype mismatch is separate
  technical debt, not a pointer-domain crossing established by this rung.
- Host-only probe buffers in `port_main.c` and native-only work-list/render
  object links never enter `PSX_ADDR`/`WM_U32` tables.

## Queue, in order

1. Repair and certify the forced-route `wm_800914D0` stack-vector crossing,
   after a one-shot predicate witness.
2. In one bounded slot-7 rung, witness the state-7 and state-2 arms, then repair
   whichever of `wm_8008E0F0` and `wm_80095CD4` is naturally exercised; keep
   separate mutants for the direction input and attribute output.
3. Resolve `D_80062528` when the sound consumer stub is retired.
4. Resolve `g_GameCurLoadedWDS` before enabling the gated first-WDS consumer in
   a maintained route.
5. Audit `D_801E8670` only when its overlay instantiator is ported.  It is not
   urgent on any currently runnable path.

The `D_8006259C` host authority and polymorphic archive queue are watch items,
not production-fix rungs on the current evidence.
