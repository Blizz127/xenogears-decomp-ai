# Interactive target controller at80084B40

Status: shared C controller implemented and native/retail differential tested
with explicitly simulated callbacks. Exact matching remains INCOMPLETE;
native adoption, full-callee integration and natural gameplay acceptance
remain NOT IMPLEMENTED/NOT OBSERVED.

Latest exact gate: VDmYoa FAIL115/122, size1E8 and table8/8. The seven
remaining words are setup offsets1C..30 and the actor argument at10C.
Earlier106/122 results below are historical.

## Authority and control flow

Pinned battle SHA256:
`1830b4ef1fe37129972fc310dfad534f8161d6c0b123e74254c3711334a3e291`.
Body offset15050, size1E8 hex; jump table offset720, size20 hex, loaded at
80070210. The fixture executes this body and table without replacing either
on positive runs.

The actor uses its low byte and64-byte row stride. Initial selection is the
byte at the current context+2E8, captured once. The loop writes event8 and
waits while the event remains8. Each wait iteration calls:

1. 89C08(actor), then89C08(selection).
2. BC404 with the low16-bit OR of those two masks.
3. 89C08(selection), thenBCD98 with its low16-bit mask.
4. 716D8, the frame/input callback; then rereads the event byte.

| Event | Action |
| --- | --- |
| 0..3 | Call84854(selection low byte,event); retain its result as selection. |
| 4,6,7 | Reload current context, store selection byte in actor row+3C; return1. |
| 5 | Cancel display sequence described below; return0. |
| 8 | Continue waiting. |
| 9..255 | No selection action; reset event8 and resume waiting. |

After dispatch, event is reset to8, including before return. Only status2
continues the outer loop. A finite scripted callback stream in the fixture
does not authorize a timeout or iteration cap in a C replacement.

Cancel calls89C08(actor), reloads the current context row target, calls89C08
on that byte, thenBC404 on their low16-bit OR. It reloads the context and row
target again afterBC404 before89C08/BCD98. Do not cache the context or target
across those callbacks. Confirm writes the retained selection, not a newly
read context+2E8 value.

## Test boundary and evidence

`run_battle_target_ui_contract_test.sh`, rJezVs terminal0:6144 cases per
O0/O2/UBSan. Three dirty-upper actor arguments (low bytes0,3,255), all256
initial selection bytes, terminal events4..7, and owner-swapping on/off.
Each stream contains8,9,255,0,1,2,3, then its terminal event.

All five external functions are explicit simulations: lookup uses a
deterministic nontrivial mask; display may switch owners and mutate the new
row target; highlight records its mask; frame supplies the script; picker
supplies a deterministic next selection. This proves controller call/state
behavior under that contract, not the correctness of those callees. Separate
84854 tests use real retail/native atan; they do not upgrade this fixture's
simulated picker boundary to full integration.

Every callback is checked against the expected ordered function/arguments.
Caller-saved registers are deliberately clobbered. Final full context images,
owner, event, result, saved registers, SP and RA are checked; retail stores
are restricted to its stack frame, event byte and current actor target.
A copied-table mutation routing cancel to confirm is rejected at the final
trace/result assertion. No retail file is modified.

Initial7BaKN1 failed because the fixture incorrectly required caller-saved
t8/t9 preservation; corrected to s0..s7/fp plus SP/RA. This was a fixture
error, not a retail/controller repair.

## Review-driven coverage corrections

The initial eight-event stream returned to the original owner after an even
number of swaps. Added a seven-event stream omitting the initial8, so confirm
also writes to the alternate owner. Cases now total12288 per build. The
target-write observer requires exactly one byte write on confirm, only after
all callbacks, to the current owner with the expected value; cancel permits
none. This rejects premature controller target writes, not merely bad final
state.

04jgq1 terminal0: O0/O2/UBSan each12288PASS, all three controls rejected:
cancel-as-confirm, stale initial owner (instruction-copy mutation), and an
explicit early-write harness fault testing the write observer. EarlierjiUfDA
positive runs and two controls passed, but the early-write control failed to
compile under Werror due to missing arithmetic parentheses; corrected before
the final full rerun. The review corrections do not broaden the simulated
callback boundary into native or gameplay acceptance.

## Shared C controller and remaining integration boundaries

`src/battle/target_ui_impl.inc` is included by main35 and by the native test
fixture. The native link first failed with missing func_80084B40 (RED), then
passed after implementing the body. It preserves context reloads and the
unbounded callback-driven loop; no synthetic iteration cap is added.

egqpMe terminal0: O0/O2/UBSan and expanded O2 each12288 native and retail
cases pass. Five native source mutations are rejected: actor mask, stale
confirmation owner, premature target write, premature event reset, and bad
result. All three earlier retail/harness controls also pass. Native callback
entry now checks entire context images and the visible event independently
of FRAME's output event. LOOKUP is explicitly read-only, as in the table-only
implementation at main41.c; arbitrary owner changes in LOOKUP are not tested.

The exact write observer applies only to interpreted retail execution. Native
checks prove callback-visible and final state, not exact store counts or
duplicate/same-value writes between callbacks. That access-trace boundary
remains open; these snapshots do not silently close it.

UtoMm5 isolated exact gate FAIL106/122, size1E8 and jump table8/8. The runner
separates compiler-emitted UI text/table into sections without instruction
changes. Only handler.elf places UI at84B40/table70210; the selection-only
link parks UI at89000 and is not UI placement evidence. Attack/selection
gates still pass. A wider lookup prototype trial xnCxr3 reached113/122 but
was reverted: actual main41 defines u16 func_80089C08(u8), and an incompatible
cross-translation-unit prototype is not an acceptable matching shortcut.

BC404's previously observed pointer-parameter mismatch in mainc114 is now
corrected to a scalar u32 mask after inspecting its retail BC460 consumer;
see BATTLE_TARGET_DISPLAY_CONTRACT.md for wrapper tests and evidence. Its
downstream calls remain simulated there. The default UI mode simulates
DISPLAY; the additional mode below executes its actual wrapper. Full
native-callee integration remains open. No dispatch guards were
changed.

WHoQ86 positives passed, but the first expanded native baseline failed to
link because its generated input/output filenames collided. Corrected to
separate native-expanded.c from native-baseline.c, then reran egqpMe fully.

Final QALoCB terminal0 adds callback-entry owner assertions on both paths,
captured before DISPLAY's scripted toggle. O0/O2/UBSan plus expanded O2 each
pass12288 cases; six native mutations and three retail/harness controls are
rejected. The new entry-owner mutation fails at the pointer assertion.
Independent review verified the corrected event/owner checks; remaining
native store-trace and real-callee integration limits stay explicit.

## Typed mask locals and explicit row reloads

Using u16 mask locals preserves the actual u16(u8) lookup ABI and lets the
compiler combine truncation at the mask consumer. Explicit row pointers
incremented by the actor offset reproduce retail address operand order.
The second cancel row pointer is declared in a fresh scope after DISPLAY,
so neither owner nor target is retained across that callback.

SPnqdp and final VDmYoa reach115/122 without prototype changes. IMZOPL
terminal0: native/retail O0/O2/UBSan and expanded O2 each12288 cases pass;
all six native mutations and three retail/harness controls are rejected.
Mutation patterns were updated for the row-store expression and u16 local,
with the existing no-change guards retained. No callback or native-access
boundary is broadened by the improved instruction match.

## Controller plus actual display wrapper

The runner additionally builds XBT_UI_REAL_DISPLAY with XBT_UI_NATIVE. Its
retail path executes BC404 at the actual address, and the native controller
calls the actual mainc114 C body through a test-only symbol rename. BC404 is
not bridged to the old atomic DISPLAY simulation in this mode. XGPo9D
established12288 passing cases per O0/O2/UBSan before integrated mutants.

BC2F0 and BC460 remain explicit simulations. BC2F0 sets the gate nonzero and
changes the source; BC460 consumes the expected display mask/owner effects,
clears the gate for the next scripted iteration and changes the source again.
The real wrapper must not recheck the changed gate, and must copy the final
halfword after BC460. Wrapper entry counts, downstream call order, destination
state at subsequent callbacks, and neighboring guards are checked. Retail
also checks nested stack-write addresses and destination-store count.

This removes one simulated function from the integrated controller path;
it does not establish real rendering, arbitrary display-state evolution,
native exact write counts, the real picker, or full native dispatch admission.
The older all-simulated mode remains as an independent controller contract.

Historical review limitation: integrated wrapper entry had gate zero;
BC2F0 sets it nonzero and BC460 clears it again. Nonzero-entry controller
composition was NOT PROVEN by that run. The separate786432-case wrapper suite covers
nonzero gates, but cannot substitute for that missing integrated scenario.
Native source preservation is now checked immediately at wrapper return,
before a later callback can overwrite it; a post-copy source-corruption
mutation explicitly exercises that check.

Final NssCPP terminal0: O0/O2/UBSan each12288 cases in all-simulated and
actual-wrapper modes, with expanded O2 baselines. Four integrated wrapper
mutations are rejected (drop copy, recheck flag, stale source, corrupt source)
alongside the nine existing controller/retail/harness controls. Earlier dJqPcu
passed the first three integrated mutations before the source-preservation
review correction. No production source changed in this integration step.

## Nonzero-gate controller composition

The actual-wrapper mode now crosses every existing controller scenario with
entry gate bytes0,1,255, giving36864 cases per build. Each scenario keeps its
entry gate class fixed across display iterations; arbitrary mixed-gate frame
sequences remain untested. For nonzero gates, the expected owner does not
change through DISPLAY and neither downstream function may execute, but
every wrapper invocation must still copy the source halfword.

Nonzero guest entry is observed using a copy of CPU state, with observer
clobbers discarded. The interpreter still executes every BC404 instruction;
its bridge refuses to replace BC404. Native entry has the corresponding
observation before calling the actual native wrapper. This records the
controller's display request without pretending a skipped BC460 callback ran.

D1Sdt9 terminal0: all integrated O0/O2/UBSan and expanded O2 cases pass,
alongside the original12288-case controller modes. The unconditional-calls
mutation is rejected when gate is nonzero. This closes the earlier missing
nonzero-entry composition cases for these classes, not full rendering or
arbitrary runtime state evolution.

Final 3OCEiV terminal0:36864 integrated cases per O0/O2/UBSan and expanded
O2, original12288-case modes, and all15 controls pass. The new controls cover
unconditional downstream calls and omitted nonzero-gate copying. Failure
patterns are specific: unconditional requires the bridge's entry_gate==0
assertion; recheck/corrupt-source require source preservation; copy failures
require destination preservation. sLpobL passed before this tightening;
aIbuVq exposed an incorrect expected assertion for recheck_flag, corrected
before the final full rerun. No production code or dispatch changed.
