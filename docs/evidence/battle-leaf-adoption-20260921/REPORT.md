# Battle overlay leaf adoption: 96 -> 100

Adopting battle overlay functions as native host C leaves, moving them off
`pc_port/src/battle_mips_runtime.c`. Uses the existing mechanism only:
`pc_port/src/battle_overlay_host_leaves.inc` plus the `[2c/5]` build stage.
No new mechanism, no interpreter change, no `config/*.yaml`.

**Result: 4 adopted, from an eligible pool of 26.** Every one is proven
equivalent to the retail MIPS bytes by the differential prover and has
demonstrable interpreted callers. The other 22 were rejected for stated
reasons, and those reasons are the useful part of this report.

## Selection criterion, in order

1. **Eligible by the repo's own rule engine** —
   `battle_overlay_host_tus.py --eligible`: in the generated candidate set, not
   already adopted, no call path reaching a generated stub, no pointer
   parameters (the sweep cannot vouch for those), has a C body, body closure
   complete. **26 candidates.**
2. **Leaf-ness is already enforced by (1)** — the stub-reach and body-closure
   filters are exactly "calls nothing still-interpreted".
3. **Demonstrably executes** — has at least one `jal` caller in the retail
   battle asm that is itself still interpreted. This is what makes adoption
   change anything: the interpreter only `dlsym`s a leaf when interpreting a
   caller that jumps to it. **20 of 26 pass.**
4. **Survives the build gate** — its owning TU compiles under
   `-DXENO_BATTLE_OVERLAY_HOST_BODIES`, and it is actually defined by a host
   object (not excluded by a `#ifdef`). **10 of 20 pass.**
5. **Proven equivalent to retail** — `run_battle_overlay_host_differential_test.sh`
   runs it twice on the same guest RAM, once as retail bytes out of
   `disc/battle.bin` and once as the host C body, and compares the result and
   every byte of RAM that changed. **4 of 10 pass.**

### On why the MIPS trace was not used for criterion 3

`XENO_BATTLE_MIPS_TRACE` prints at `battle_mips_runtime.c:1051`, which sits
*after* `resolved != NULL`. It only logs targets that already resolve to a
host function, so a still-interpreted candidate never appears in it. Using it
to decide "does this execute" would have produced exactly the false negative
the trace has caused before. Static `jal` callers from the retail asm are used
instead.

## Adopted (4)

| function | owning TU | interpreted callers | retail insn |
|---|---|---|---|
| `func_80085388` | `src/battle/main36.c` | 6 — `func_800793F0`, `func_80079778`, `func_80085B58`, `func_80087AF0`, `func_8008ADD0`, `func_8008B908` | — |
| `func_800800E8` | `src/battle/main33.c` | 2 — `func_80080160`, `func_80080BD0` | 30 |
| `func_80085454` | `src/battle/main36.c` | 1 — `func_80085C88` | — |
| `func_80085AC4` | `src/battle/main36.c` | 1 — `func_80072324` | — |

`func_800800E8` is the standout: it was itself a **generated stub** before
this change, so adopting it replaced a placeholder with a real body.

## Rejected, with reasons (22)

### Owning TU does not compile under `-DXENO_BATTLE_OVERLAY_HOST_BODIES` (9)

The guest-RAM aliases in `pc_port/src/battle_overlay_guest_ram.h` are not
expressible for these TUs. Fixing that means extending the guest-RAM
generator — a second mechanism, which this dispatch forbids.

| function | TU | compiler error |
|---|---|---|
| `func_8007FDEC` | `main32.c` | `D_800D367C = buf;` — alias is an rvalue (`(u8*)PSX_ADDR(*(u32*)PSX_ADDR(...))`); assigning needs a host→guest pointer writeback, and no such idiom exists in the tree |
| `func_80085350`, `func_80084108` | `main35.c` | `D_800D3014` undeclared; `request for member 'rows' in something not a structure` |
| `func_800885D0` | `main39.c` | `battle_overlay_guest_ram.h:122: expected expression before ']'` |
| `func_8009AA44`, `func_8009AB00` | `main55.c` | `battle_overlay_guest_ram.h:172: expected ')' before '*'` |
| `func_800AA760`, `func_800AA788` | `main70.c` | `battle_overlay_guest_ram.h:183: expected ')' before '*'` |
| `func_800BC460` | `mainc115.c` | `battle_overlay_guest_ram.h:205: expected ')' before '*'` |

**This is the single biggest blocker to the next batch: 9 of 26 candidates,
gated by 6 TUs.** Making the guest-RAM generator handle these cases would
unlock them in one stroke.

### Deliberately excluded in source (1)

- `func_800BC2F0` (`mainc114.c`) — its C body is in the `#else` of
  `#ifdef XENO_PC_PORT`, with a recorded reason: *"PC adoption uses the
  explicit resolver/frame API in battle_target_setup.c; retail task pointers
  below must not become unchecked native pointers."* The `[2c/5]` gate caught
  this as "defined by no battle host object", i.e. the allowlist entry would
  have been inert. Not an oversight — respecting the decision.

### Host body diverges from retail (4) — the prover rejected them

| function | TU | divergence |
|---|---|---|
| `func_800BEDE8` | `mainc124.c` | RAM: 2 bytes differ, first at guest `800c3612`, retail `00` vs host `1e` |
| `func_800AA7DC` | `main71.c` | host body faults where the interpreter completes (wild address); and return `v0` retail `00000027` vs host `00000000` |
| `func_800AA79C` | `main71.c` | host body faults where the interpreter completes (write8 at `0x34`, pc `0x800aa7b8`) |
| `func_8008887C` | `main40.c` | RAM: 1 byte differs, first at guest `800c3a9c`, retail `3e` vs host `32` |

These are real reconstruction bugs in existing C bodies. Adopting any of them
would have shipped wrong behaviour into the port. Both `main71.c` functions
fail the same way, which suggests one shared cause in that TU.

### Prover cannot construct a conclusive case (2)

- `func_8008AA40`, `func_8008AB94` — every sweep case is inconclusive
  (`unresolved native call target=0x80028470`), so the harness reports
  `UNPROVEN` and refuses them. Correct: an unproven adoption is exactly the
  "half-verified" outcome to avoid.

### Cannot demonstrate execution (6) — tested anyway, none usable

`func_8008AB4C`, `func_8008AB70`, `func_8009A7B8`, `func_8009E3C8`,
`func_800B7364`, `func_800B16F0` have **zero** `jal` callers in the retail
battle asm, so criterion 3 excludes them (they may be dispatched indirectly
through a callback table, but that cannot be shown from the disassembly).

I added all 6 speculatively to check whether they were worth the exception.
They are not: `func_8009A7B8` FAILs the prover (host body faults where the
interpreter completes) and the other 5 come back `UNPROVEN`. Consistent with
being unreachable — the sweep cannot reach them meaningfully either. Dropped.

## Measured results

| metric | before | after |
|---|---|---|
| adopted leaves | 96 | **100** |
| generated function stubs | 76 | **77** |
| differential checks | 2,347 | **2,431** |
| battle host TUs in the link | 61 | 65 |

### The stub count went UP by one — here is exactly why

Diffing the two generated manifests:

- **removed:** `func_800800E8` — was a stub, now has a real adopted body. A win.
- **added:** `func_800883AC`, `func_80098C6C` — newly *referenced* because
  adoption pulled their TUs into the link, and neither has a C body yet.

76 − 1 + 2 = 77. This is inherent to the mechanism: adopting a leaf pulls its
whole TU in, and any undecompiled sibling that TU references materializes as a
placeholder. The `[2c/5]` gate confirms no *adopted* leaf reaches either new
stub, so they are inert with respect to everything that runs natively. The
honest summary is that the stub count is not a good scoreboard for this work;
adopted-leaf count and differential-check count are.

## Gates

```
==> [5/5] Final link
    LINK OK -> pc_port/build_native/xeno-port (port-owned addresses verified)
    overlay host allowlist verified against the linked binary
# 100 adopted leaves reach no generated stub (77 stubs checked)

BATTLE OVERLAY DIFFERENTIAL PASS checks=2431 inconclusive=442 hand-only=8 leaves=100
BATTLE OVERLAY DIFFERENTIAL bridge-placeholders=25
BATTLE OVERLAY DIFFERENTIAL FAIL func_80079934 ram 1 bytes differ, ...   <- the wrong-result control, correctly rejected
BATTLE OVERLAY DIFFERENTIAL PASS (O0/O2/UBSan + wrong-result control rejected)

BATTLE OVERLAY LEAF PASS checks=40      (O0)
BATTLE OVERLAY LEAF PASS checks=40      (O2)
BATTLE OVERLAY LEAF PASS checks=40      (UBSan)
BATTLE OVERLAY LEAF PASS (O0/O2/UBSan + always-interpret control)

BATTLE MIPS ADAPTER PASS
RETAIL_LEAF_ADAPTERS PASS checks=35
BATTLE RETURN STATE PASS
```

### Byte parity — the constraint, proved not assumed

```
battle.bin size: 343936   retail: 343936
battle.bin IDENTICAL to retail
124703d415f570f9cd8d7dad89c6a4eddfd7e6f0  build/out/battle.bin
124703d415f570f9cd8d7dad89c6a4eddfd7e6f0  disc/battle.bin
battle_command_file1 IDENTICAL
field.bin 249310 diffbytes=218716   (unchanged from the previous dispatch)
```

Splat tracebacks in the matching build: **0**.

## Two dead gates found and repaired

Neither was caused by this change; both were verified to fail identically with
the baseline 96-leaf allowlist before being touched.

1. **`run_battle_overlay_leaf_bridge_test.sh` — was not running at all.**
   Two independent bit-rots: `battle_mips_runtime.c:1103` calls
   `PcPort_GodModeBeforeGuest()` but the fixture never linked `god_mode.c`, so
   it died at link; and once that was fixed, its always-interpret negative
   control asserted, because the injection anchor still expected
   `resolved = find_function(runtime, target);` where the source now has
   `resolved = NULL;` followed by the bridge cache lookup. Both fixed; the
   suite and its control are green for the first time in a while. This is the
   gate that guards the very mechanism being extended here, so it mattered.

2. **`run_battle_native_callback_map_test.sh` — same missing `god_mode.c`.**
   Fixed the link. It now builds and reveals a **real pre-existing failure**:
   `NATIVE CALLBACK FAIL generated stub was dispatched`. Confirmed identical
   with the baseline allowlist, so it is not from this change — it was simply
   invisible behind the link error. Left unfixed and flagged: it is a separate
   defect and outside this dispatch, but it should not keep hiding.

15 fixtures reference `battle_mips_runtime.c` without linking `god_mode.c`;
only those that actually pull in `runtime_bridge` break, which is why the
differential prover in that same list is unaffected. Not fixed wholesale.

## Suites that could not run on this host

- `run_battle_guest_call_test.sh` — **NOT RUN**. Needs
  `asm/battle_command_file1/matchings/message_controller/xeno_battle_command_file1_controller.s`.
  `config/battle_command_file1.yaml` splits that overlay with whole-TU `asm`
  subsegments, so it emits `asm/battle_command_file1/message_controller.s` and
  no per-function `matchings/` tree at all. Pre-existing mismatch between the
  test and that config; config is out of scope for this dispatch.
- `run_textbox_timing.sh` — **NOT RUN**, unchanged from the previous dispatch:
  host lacks `/usr/lib64/libubsan.so.1.0.0`, so the UBSan link fails.
  `run_battle_child_billboard_retail_test.sh` documents the same limitation.

Neither should be read as a pass.

## What the next batch should do first

Fix the guest-RAM alias generator for the 6 TUs listed above. That is 9 of the
26 candidates in one change, and it is the only blocker here that is a tooling
gap rather than a per-function judgement. Second priority: the 4 bodies the
prover rejected are real bugs in already-written C — worth fixing on
correctness grounds regardless of adoption, and `main71.c`'s two look like one
shared cause.
