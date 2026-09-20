# Opening through Blackmoon Forest — active route acceptance

Goal: retail-faithful normal play from the opening through Blackmoon Forest.
Status: **IN_PROGRESS**, not a parity certificate.

Fresh run starts at commit 7d50c62a using normal retail boot and title New Game.
The initial run used no field-map override, scenario override, checkpoint load,
forced battle result, teleport or game-state patch. A later process restart and
earned-checkpoint continuation are recorded below. HD-2D Fei remains enabled as requested;
its optional presentation changes are separate from retail simulation fidelity.

Local evidence is `scratchpad/blackmoon-route-20260919/`: run.json pins the
binary and launch environment; runtime.log records transitions; inputs.log
records ordinary keyboard inputs. route.xgqs is reserved for checkpoints earned
on this run, keeping user saves untouched. PIDs are historical hints; revalidate
process ownership before attaching or sending input.

Observed on this fresh run: title New Game -> field4 narration/city sequence
-> field2 opening Gear battle -> field14 painting room -> field13 downstairs.
The opening battle returned after 71,737,910 interpreted instructions. Field14
control unlocked at scenario6 and ordinary directional input reached its exit.
Field13 triggered Dan's conversation naturally. No runtime stub or unresolved
call was logged along this segment. This proves progression, not audiovisual
identity against a retail hardware capture.

Required route acceptance still outstanding:

- Alice's upstairs event is REQUIRED for the subsequent Citan story chain; the
  earlier optional classification was incorrect (see prerequisite correction below).
  Downstairs, village, and mountain traversal are observed.
- Citan's house, story events, night return, burning Lahan and Gear combat.
- Destruction, aftermath and normal departure to the world map.
- Natural entry into Blackmoon Forest, its required story events and battles,
  and traversal through the forest. Establish map identity from the actual
  route and retail assets; older notes inconsistently label map 16.
- Movement, collision, dialogue, menu return, battle victory/return and required
  transitions throughout; record rendering/audio discrepancies when observed.

A passed unit test, forced map inspection, old walkthrough or isolated screen
capture cannot complete this route. Newly discovered defects require retail-
backed diagnosis, relevant differential/byte checks and natural-path retesting.

## First mountain encounter checkpoint

The fresh route continued through Dan's dialogue (scenario7), field1 Lahan and
its mountain exit into field15. Navigation used ordinary keys; an older input
sequence stopped against a village rock and was corrected by walking around it.
This was navigation, not a detected collision defect. Alice's visit was not
exercised in this segment; later script inspection established it is required.

A random encounter started naturally on the mountain: Fei entered with 50/50 HP
and fought three enemies through the standard command/attack interface. The
victory rewards screen was observed, and the battle returned after 77,228,928
interpreted instructions. After return, Left input changed field position from
(660,-15,-840) to (555,-40,-737). F7 then wrote the route checkpoint normally.

Checkpoint files, deliberately local and not committed:
- mountain-entry.xgqs: first arrival in field15.
- first-victory.xgqs: earned progress after the first normal mountain victory.
- route.xgqs: latest route checkpoint (same as first-victory at this entry).

Continuation: owned game PID1767877 on Xvfb :95, window2097204; verify live state
before controlling it. Input scripts have finished and all keys are released.
The binary is still commit7d50c62a. Continue walking the mountain toward Citan,
handling encounters normally. Do not reload the older unrelated test saves or
use Escape's test-only battle bypass. The full goal remains active.

## Mountain continuation: five normal victories

The preserved live run won encounters 2 through 5 using normal battle controls.
Each victory returned to field15. The Items screen opened and cancelled normally;
a read-only debugger inspection found all 150 live item quantities zero. The
Abilities screen displayed Guided Shot and cancelled normally. Fei is level2,
11/52 HP, 10/11 EP; healing through normal gameplay is needed before further
encounters. No inventory or health was injected. A mistaken File selection
exercised the explicit unsupported message; dismissal returned to the menu.

Navigation at the central cliff was investigated with read-only walkmesh dumps.
A simple triangle adjacency route that ignored materials incorrectly proposed
climbing material2 cliff faces. Restricting that diagnostic to material1 revealed
the eastern climb, which was traversed normally to approximately (585,-475,1124),
but it ends at a cliff rather than the bridge route. No collision repair was
justified or applied. Layer1 geometry identifies the raised western trail;
continuation should use the western route rather than repeat the eastern detour.

Latest earned save: route.xgqs, with fifth-victory.xgqs preserved separately.
Player is at field15 approximately (584,-14,-505), facing after two camera
rotation experiments (do not assume the original screen/world direction map).
Game PID1767877 remains owned and live on :95; all input scripts are finished.
Health management is the next gameplay decision. Normal battle Escape via its
command menu is allowed; the keyboard Escape test bypass remains forbidden.
The goal is still incomplete: Citan, burning Lahan, aftermath/world departure,
and Blackmoon Forest have not been reached on this fresh run.

## Process interruption and earned-checkpoint continuation

After the fifth victory, ordinary walking returned through Lahan (field1),
Fei's main floor (field13), and his basement room (field14). The bed interaction
displayed "Took your hidden money." (`bed4.png`). Rest and restored HP were not
yet observed. The session was then interrupted; on the next live check neither
the game nor Xvfb existed. The log has no recorded fatal error at its end. Cause
of process termination is unverified; do not classify it as a reproduced game
crash. No save was made after leaving the fifth-victory checkpoint.

The original `runtime.log` and `run.json` are preserved. A new process was
launched with the identical binary SHA256 (c62d685ea6e834b284e1af93782a706d7f4a7bd8e10771fc845232be6537f4f8),
same isolated checkpoint path, normal boot, and HD-2D enabled. Its log is
`runtime-resume1.log`, with pins in `resume1.json`. Normal New Game was selected
because loading checkpoints requires free field control. The continuation is
intended to load only this run's earned `route.xgqs`; load acceptance and recovery
remain pending until observed. This is not an uninterrupted playthrough.

## Title checkpoint recovery fix

The first restart replayed the opening (using 5x for the already-covered segment,
restored to 1x before F8) and loaded the earned fifth-victory checkpoint. A random
encounter interrupted the return home. Fei entered with 11/52 HP and was defeated
after normal Escape attempts. The later checkpoint is preserved; continuation
uses a separate byte-identical copy of this run's first-victory checkpoint in
`recovery.xgqs` (SHA256 a9b6a8a559f4cc99e14d309aa2657753a49441080315d19b3eb3a5b9bf8922bd).
This explicitly rolls back the later four victories for the ongoing route.

Recovery exposed a native checkpoint gap: title field490 has no controllable
player and its nested menu loop prevents FieldMain polling. F8 stayed pending.
The fix permits loads in the initialized title field, rejects title saves, polls
in the port title-menu body and carries the prepared load through ordinary menu
cleanup to FieldMain's existing teardown. Other gameplay control gates remain.
The first live attempt with only the field gate fixed stayed pending, motivating
the title-loop regression and handoff fix. This supports native checkpoints;
it does not implement retail memory-card Continue or certify that interface.

Validation:
- Real checkpoint gate regression failed before the fix at title loading; O0,
  O2 and UBSan now pass. Tests cover locked gameplay, inactive title, missing and
  corrupt files, preserved save contents, and the two-stage menu/field handoff.
- Production-linked title-chain tests pass O0/O2/UBSan, including checkpoint
  exit/cleanup without selecting New Game, Continue or attract mode. Existing
  five negative-control mutants still fail as expected.
- Existing file, request, collision-restore and integration regressions pass.
- Full native build passes: 78 generated function stubs, 96 adopted leaves.
- Fresh before/after MIPS compilation of `src/menu/main/misc.c` gives identical
  whole `.text` bytes, SHA256
  `7ff8ccc25bf6ba12b25cd80e2e813799fb77cbf9c2a5271a32ced06230ee2504`.
  The title hook is exclusively in the XENO_PC_PORT branch. This proves no
  MIPS change from this patch, not whole-port byte identity.
- Live rebuilt game: title F7 rejected; F8 loaded field15 at (555,-40,-737).
  `title-load-pass2.png` shows LOADED and HD2D ON. The source checkpoint hash
  remained unchanged. A natural random encounter followed; Fei displayed49/50HP.

Current continuation is pinned by `resume3.json`, with log `runtime-resume3.log`.
The earlier PIDs above are historical. Full Blackmoon route acceptance remains
incomplete; no new retail movement/combat repair was justified in this segment.

The subsequent encounter was won normally and returned to field15. Directional
input then moved Fei from (578,-19,-610) to (380,-36,-851), verifying movement
after title load and battle return. Walking through Lahan, field13 and field14
reached the bed. The normal interaction awarded200G, showed the rest prompt,
and Yes restored Fei: `recovery-stats2.png` shows level1,50/50HP,10/10EP,300G.
The menu cancelled normally; F7 saved field14 at (232,0,-237).
`recovery.xgqs` now holds that earned rested state; `rested-at-home.xgqs` is a
preserved copy. The earlier first/fifth-victory and route checkpoints remain.
Live continuation at this entry: PID96416, Xvfb :95, window2097204,1x speed,
HD2D ON; all automation/input scripts have finished. Verify these live before
resuming. The map camera differs from the previous low-HP run: in the present
orientation Down approximately decreases x and z; Down+Right decreases z.
Next: return to mountain, follow the western ascent/running-jump route to Citan,
and continue required story progression. Bed recovery and checkpoint recovery
are observed; Blackmoon Forest and full retail fidelity are still unproven.

## Western mountain traversal and Citan arrival (ff29b755 runtime)

The preserved rested game continued without another load or state edit. Normal
walking left field14, passed field13 and Lahan, and re-entered mountain15. Five
more random battles were won with ordinary attacks; each returned to the field.
`runtime-resume3.log` now contains six battle returns including its earlier
post-title-load encounter. The five new instruction counts are56386678,41659908,
51608816,37247635,43006359. No test-only Escape key was used.

The western route was traversed naturally: lower slope near(-811,-191,-730),
ramp near(-338,-286,-330), jump onto the upper trail near(-129,-348,-330),
then the narrow ledge. One overshoot dropped to the lower path; walking back
up and jumping the ramp recovered normally. No collision code change was made.
A fresh earned save was made at(-675,-409,-766). Holding ordinary directional
run+jump crossed the gap to(-1307,-457,-707), captured in `gap-landing.png`.
Bridge entry was near(-1099,-540,-187); after another normal encounter and
return, walking crossed to(-1111,-560,1107). The northern trail and another
victory led to the north exit(-167,-727,2353) and naturally into field17.

Field17 initially owned control during entry, so F7 queued until control
released, then saved(26,175,-1706). `citan-path-earned.xgqs` is the verified
copy of that completed save (an earlier copy was refreshed after the actual
save log). The workshop door and return worked. The main house loaded field19;
walking around the table reached Yui. Her normal dialogue opened, completed,
and released control. Repeated confirms briefly reopened the same conversation;
the helper then stopped at observed free control.

Latest earned state: `recovery.xgqs` and `yui-house-earned.xgqs`, field19 at
(20,0,396), scenario7. Preserved earlier checkpoints include rested-at-home,
gap-takeoff-earned, and citan-path-earned. All are isolated from user saves.
Live game remains PID96416, display:95, window2097204, speed1x, HD2D ON, binary
from ff29b755; revalidate before input. All input helpers finished. The camera
was rotated four L1 taps inside field19 to see around the wall/table. Do not
reuse mountain direction mappings in this room.

Next is leaving the house and reaching Citan on the rooftop, then music-box,
dinner/night return, burning Lahan, Gear battle, aftermath/world departure and
Blackmoon Forest. Those gates remain unobserved on this route. This milestone
proves traversal and dialogue/control return, not full audiovisual parity;
room screenshots still require comparison against a retail capture. There were
no source changes or new builds/tests in this gameplay-only continuation.

## Correction: Alice is a required story prerequisite

The rooftop approach at scenario7 did not trigger the story event. Inspection
of the field scripts, followed by the retail scenario-handler assembly, explains
this without a port repair:
- Field12 script0,0x001e checks scenario<8; the event writes scenario8 at0x0038
  before returning to field11 at0x0047.
- Field19 Yui interaction checks scenario<8 at0x037d. At7 it gives the short
  greeting and exits through0x038f; the later branch writes scenario10 at0x03d0.
- Field17 rooftop event actor13 checks scenario==10 at0x0569; otherwise its
  initialization takes0x0584.
The LessThan/Equal handler branches were read against their checked-in retail
assembly in asm/field/matchings/game_logic/scenario_flags. Decoded raw scripts
are in scratchpad/boot-to-blackmoon-20260909/field{12,17,19}-decode.txt.
The earlier statement that Alice was optional was wrong. The brief Yui greeting
was valid at scenario7, but did not satisfy the progression prerequisite.

The attempted normal backtrack won one encounter, then Fei was defeated in the
next with lowHP before recovery. F8 successfully loaded a preserved copy of
rested-at-home.xgqs through the title recovery path. This rolls the active route
back to that earned state; Citan/bridge traversal remains observed as an earlier
attempt, not the active story state. Later earned checkpoints remain preserved.
No state was fabricated. The painting texture after this reload appears different
from the earlier room capture; audiovisual fidelity remains unverified and needs
a controlled comparison, rather than treating checkpoint success as that proof.

Normal walking reached Alice's entrance (field1 actor51 at474,29,-33). Speaking
to the woman from the front completed her dialogue and moved her aside. Entering
field11, walking up its stairs, and entering field12 started the Alice event: Fei
walked normally and the wedding-dress dialogue appeared (`alice-scene-start.png`).
The prior field12 walk stall has not recurred on this entrance. Dialogue and final
scenario transition are being observed; do not assume completion from the opening.

Alice completion is now observed on the same live ff29b755 binary. Ordinary
confirm inputs advanced the dialogue and staged walking/camera sequence. The
event returned naturally to field11 at(-115,0,274), scenario8,canRun1,owner0xff,
b21d0=0. F7 then saved successfully; `alice-complete-earned.xgqs` preserves that
file and `recovery.xgqs` is the active copy. Screenshot `alice-complete.png` and
the runtime log record the actual end state. This freshly exercises the field12
walk sequence through its scene exit; it did not stall.

Current handoff: PID96416 remains live on:95/window2097204,1x,HD2D ON; all input
helpers have finished. Fei is downstairs in Alice's house, with the required
scenario8 earned normally. Next: leave field11, consider ordinary shop healing
supplies for the mountain, repeat the known western route to Citan, talk to Yui
again to reach scenario10, and trigger the rooftop event. Do not load the older
scenario7 Citan checkpoint to skip the travel: that would lose this prerequisite.
No implementation change was made for the retail story gate. Full Blackmoon
route acceptance and audiovisual parity remain incomplete.

## Supply shop: natural crash, native layout repair, remaining stub gate

From the completed Alice checkpoint, normal doorway/movement input reached
Lahan's supply shop (field6, exterior actor55 around -531,1052). Talking across
the counter from approximately (-138,0,-153) completed the shopkeeper's wedding
pitch. Menu request4 then crashed process96416. `shop-crash-info.txt` and
`shop.core` preserve the crash in `func_80034FFC`, called with a null destination
by `SystemRenderStringEntry -> func_801C5CBC -> func_801C5EE8 -> ShopMenuMain`.
These and all screenshots/logs below are under the ignored route scratch folder.

The initializer wrote its allocation at retail SystemMenu offset0x558, whereas
the builder read the expanded native `unk4E0[0].pVramBuffer`. The builder also
mixed retail0x80 record strides with native MenuString accesses and omitted
retail801C5D6C..801C5DD8's paired upload rectangles. The repair uses native
members/array indexing under XENO_PC_PORT and restores the retail rectangle and
render-before-shape order. The PSX branch is unchanged.

The regression now invokes the production initializer and builder together,
using the same primitive-layout define as production. Its previous fixture
manually planted pointers at host offsets, hiding the producer/consumer mismatch.
The revised test failed before the fix (`render args`, null work buffer), then
passed92 checks at O0/O2/UBSan. String-ID, allocation-offset, stride, and rectangle
mutants are rejected. The nonzero-offset7 case is retained. The retail pin now
covers the entire0x1B0-byte builder, not just its first0xAC bytes. This is a
production-linked semantic test, not an executed MIPS differential.

Recompiled shop misc2 whole .text is byte-identical before/after:
`8fa779dd66a10c4ee6544fd67187f4e53b9477de6ad98755035898f3ae0842b7`.
This demonstrates no MIPS change; it is not a new claim that the complete
compiled overlay matches retail. Adjacent shop-string UV tests pass13,200 cases,
224,406 checks, six distinguished mutants, at O0/O2/UBSan. Full native build
passes with78 stubs and96 adopted leaves. Raw outputs: `shop-test-red.log`,
`shop-test-green.log`, `shop-uv-test.log`, `shop-build.log`, `shop-mips*.text`.

The fixed binary was launched as process294163 on replacement Xvfb294052,
DISPLAY95/window2097204; pins are in `resume4.json`, log `runtime-resume4.log`.
F8 loaded the earned Alice checkpoint; normal movement repeated the shop entry
and dialogue. Menu request4 no longer crashes, but produces an unusable dark
interface and logs stubs `func_801CCFF4` and `func_801CBCF0`.
**Purchasing remains UNVERIFIED and blocked by missing shop rendering.**
Cross/cancel returns normally to the shopkeeper's farewell, then the field;
F7 saved field6(-132,0,-144), scenario8. Preserved copy:
`shop-repro-earned.xgqs`; active `recovery.xgqs` is the same state.
Screenshots `shop-fixed-menu.png` and `shop-fixed-cancel.png` show that distinction.
No purchases, stats edits, warps, or scenario writes were performed. HD2D remains
on, speed1x. Next: implement the two retail-backed shop functions and test a
normal purchase/return, then continue scenario8 toward Citan and Blackmoon.

## Selected shop text: retail instruction differential

Implemented native `func_801CBCF0` in shop misc3, leaving its retail INCLUDE_ASM
branch intact. This is the first of the two stubs reached by the live purchase
attempt above. It clears the requested active flags in mode0, positions the
selected record using the shop tables, preserves retail's mode1 use of record0's
width, and updates the selected record's render-context/active bytes. Other
modes only update those bytes. Native MenuString indexing replaces guest0x80
strides; coordinates still narrow through the polygon's16-bit fields.

`run_shop_selected_text_retail_test.sh` executes the pinned disc's820-byte
801CBCF0..801CC024 body through the MIPS adapter and compares native output.
The called clear helper is bridged with its retail byte-count/zeroing contract;
the native helper call is observed with the same contract. All205 instruction
sites execute across14,336 cases. O0/O2/UBSan each pass638,160 checks, including
both render contexts, modes0/1/2/255, selected records0..7, table offsets0..6,
coordinate narrowing, record preservation, zero/nonzero/truncated clear counts,
and the complete active-byte array. Four negative controls (wrong mode1 width,
lost context, omitted clearing, wrong Y) are rejected.

Before/after compiled misc3 whole .text is identical, SHA256
`a276cd675d8e46d36002427bb3291741caa1fe912ec6ea05802b4d45fbc4de42`.
Full native build passes, now77 function stubs; the binary contains a strong
`func_801CBCF0` owner. The adopted-leaf check remains96 leaves, not an assertion
that this newly implemented shop path is fully stub-free. Raw artifacts:
`shop-selected-test.log`, `shop-selected-build.log`, `shop-misc3-{before,after}.text`.

Runtime acceptance of this function and purchase completion remain pending.
The still-live process294163 is intentionally the previous shop-layout build;
its earned field6 checkpoint is preserved. Next implement `func_801CCFF4`,
whose retail body dispatches portrait highlights, descriptions, price strings,
and list rows, then restart on the new binary and repeat the normal shop flow.
Blackmoon route completion remains unproven.

## Shop renderer and static tables: visible menu, remaining item-list gate

Implemented native `func_801CCFF4` from retail801CCFF4..801CD404. It emits
portrait highlights, explanation/portrait strings, paired list strings,
descriptions, price lines/strings, quantity rows, and the independently gated
final-price string in retail order. Native MenuShop now declares the embedded
MenuString records at retail3C30/4030/45B0 instead of treating them as padding;
retail declarations remain unchanged behind the preprocessor branch.

The production-linked MIPS differential compares ordered calls and normalized
pointers at AddPrim/ShopMenuRenderString/ShopMenuRenderPolygons boundaries. Across
768 cases it executes all260 instructions, tests all-off/all-on/non-boolean and
individual flags, both contexts, and the independent equality-to-one final-price
gate. O0/O2/UBSan each pass43,799 checks; four mutations (truthy price gate,
wrong buffer, missing last row, swapped paired draw) are rejected. Selected-text
and text-pair regression suites also pass after the native layout change.
Compiled misc6 whole .text is unchanged before/after, SHA256
`c61da91444b89743222cee61dd6138aac4274a16861a2281f23e32210c7907e6`.

The first live retry (process372160, runtime-resume5.log) stopped hitting both
prior stubs but was still blank. Read-only GDB found numTexts=numCursors=3 and
all D_801D1F54 texture IDs and D_801D2194 positions zero. All three choices
therefore used texture0 at160,150. Those symbols were generated zero data stubs,
not the retail layout values. Added native data_shop_menu.c using the existing
contiguous-data/assembler-alias pattern: all2,224 bytes in retailCF50..D800 and
all32 aliases match the disc and corresponding decompiled data assembly.
The test compiles the real data module and checks every byte and alias address;
changed-value and shifted-alias mutants are rejected. The writable contiguous
image preserves table adjacency and the overlay's scratch globals.

Full native build passes with76 function stubs,547 data symbols,96 adopted
leaves. The second live retry (process393389, runtime-resume6.log, resume6.json)
loaded the earned field6 checkpoint through F8. Normal conversation opened
visible Buy/Sell/Exit choices (`shop-retail-data-menu.png`). Confirming Buy
opened its windows, character portrait, stored-quantity label and300G balance
(`shop-buy-first.png`). The item list is still empty, and the log identifies
the next missing functions: `func_801CDD14` and `func_801CEB3C`.
**No purchase has been made; purchase completion and Blackmoon remain unproven.**
Two normal cancels returned to the shopkeeper farewell (`shop-render-exit.png`),
then confirm returned to field control. F7 preserved the earned field6 scene.
HD2D on, speed1x, scenario8. No state edits or forced outcomes.

Raw evidence in the route scratch folder: `shop-render-test.log`,
`shop-render-{selected,pair}-regression.log`, `shop-render-build.log`,
`shop-data-test.log`, `shop-data-build.log`, and `shop-misc6-*.text`.
Next: port the retail item-list builder and selected-item update, verify a normal
healing-item purchase, then resume travel to Citan with Alice's scenario8 intact.

## Retail item-list builder: names and prices visible

Implemented native `func_801CDD14`, retail801CDD14..801CE480, in shop misc7.
It builds eight rows using the shop's compacted weapon/accessory/item IDs,
reads their retail16-byte records for prices, formats the five-digit cost,
marks affordability, uploads the two text planes, and builds owned-quantity
polygons. Empty rows clear their active/count flags. The two uploads and the
duplicate first-string render-context store are intentionally preserved from
retail; the second record's context is not silently rewritten.

The differential executes the1,900-byte disc function against the native owner,
observing text/GPU helper calls and deterministic helper responses, string
inputs, upload rectangles, affordability, widths, contexts, and row/count
outputs.396 cases cover start indices0/1/40, both contexts, all three valid item
types, empty rows, prices0..65535, gold-1..65535, and quantities0/1/9/99.
O0/O2/UBSan each pass120,070 checks.471/475 instruction sites execute; the four
excluded instructions are the default-type jumps at801CDE2C/30/40/44.
ShopMenuInitializeShopData produces only types0..2; invalid-type behavior is
not certified. Four mutants (strict affordability comparison, missing last row,
wrong digit, rewriting the price record's context) are rejected.

Compiled misc7 whole .text before/after remains byte-identical, SHA256
`9e6dc7d65ff47961e31e7dcdfa8b424e5310fe0fcfa12a627e83f93b7e197930`.
Full native build passes:75 function stubs,547 data symbols,96 adopted leaves.
Evidence: `shop-list-test.log`, `shop-list-build.log`, `shop-misc7-*.text` in the
route scratch directory.

Process423403 ran this build (resume7.json/runtime-resume7.log), after normal
F8 recovery of the earned shop checkpoint and normal shopkeeper dialogue.
Entering Buy now visibly lists Aquasol20, Rosesol100, Omegasol50, SurvivalTent150
(`shop-list-live.png`). No list-builder stub is hit. `func_801CEB3C` is still a
stub: the description and transaction total are incomplete. **No purchase was
attempted/completed.** The first cancel instead aborted in ShopMenuBuyMenu
with stack corruption; the subsequent inputs failed because the window was
gone. The earlier return/save claim was incorrect. No new checkpoint was
written; the last valid checkpoint remains field6(-143,0,-154), scenario8,300G.
The builder writes eight flags into the caller's four-byte sp28 declaration.
Crash evidence: shop-list-cancel-crash.txt (coredumpctl, process423403).

Next implement selected-item update801CEB3C and its stored-quantity helper
801CE91C. The equipment-preview branch also calls still-unported801CE480;
that remains a separate fidelity gap and must not be silently substituted.
Then test a normal Aquasol purchase and continue scenario8 toward Citan.
Blackmoon completion remains unproven.

## Buy cancel stack overwrite repaired

The native-only Buy caller buffer now has MAX_ITEMS_IN_VIEW (eight) bytes.
Retail assembly passes sp+0x28 to the row builder and keeps its next stack
local at sp+0x30, leaving eight bytes; the decompiled four-byte C declaration
was unsafe under the host stack layout. Retail source configuration is retained.
The caller's annotated assembly matches all2,008 corresponding disc bytes,
SHA256 `af8c2523a3a67cf5fb0bdf0c8b17d92e2e38dc66afb4aaabab2916a20270d411`.
Compiled misc8 whole .text before/after is identical, SHA256
`91cef40c4eb8ff1f2d08e73d589a18a1d49269a9f49fa32123173976dd1ba200`.
These checks are scoped to this caller/source file, not whole-port parity.

`run_shop_buy_cancel_test.sh` links the production Buy caller and row builder
with isolated menu/text/GPU helpers. It fills all eight rows, verifies their
flags at the selected-item boundary, supplies ordinary Back, and checks return
and unchanged gold/quantities. O0/O2 with stack protection and ASan+UBSan pass.
Restoring the four-byte buffer is rejected by ASan with the original builder
write into the caller's stack redzone. Full native build passes.

Live process458481 (`resume8.json`, `runtime-resume8.log`) entered Buy through
normal dialogue, displayed four item rows and300G, canceled to Buy/Sell/Exit,
canceled to farewell, confirmed, and successfully saved field6(-133,0,-141).
Scenario8 is retained, HD2D is on, speed1x. No purchase was attempted.
Observed screenshots: `shop-cancel-buy.png`, `shop-cancel-back.png`,
`shop-cancel-farewell.png`, `shop-cancel-field.png`. Test/build logs:
`shop-buy-cancel-{test,build}.log`; retail byte captures: `shop-misc8-*.text`.
This supersedes the failed resume7 cancel result, not its item-list observation.
Selected-item pricing/description and stored quantity remain the next gaps;
Blackmoon route completion is still unproven.

## Stored inventory quantity helper ported

Native `func_801CE91C` now follows retail801CE91C..801CEB3C: select the
weapon/accessory/item inventory, call the existing first-match byte lookup,
store D_801D2260, render the two-character quantity, upload its VRAM rectangle,
configure the Stored string, enable its renderer, and free the work buffer.
The quantity is not clamped to99; retail's byte inventory value and its digit
formatting are retained. Native inventory and MenuString fields replace retail
layout arithmetic only under XENO_PC_PORT.

`run_shop_stored_retail_test.sh` executes the544-byte disc routine and its real
68-byte inventory lookup, comparing with both production native functions.
15,360 cases cover types0/1/2, contexts0/1, every quantity0..255, IDs0/1/255,
upper ID bits, first/middle/last/missing/duplicate matches. The harness compares
ordered helper calls and string/rectangle payloads, byte-truncated width,
render context/flag, stored quantity, and unchanged inventory. O0/O2/UBSan each
pass522,380 checks.132/136 main-routine instruction sites execute; four invalid
item-type default jumps at801CE950/954/964/968 are excluded. Shop initialization
provides types0..2; invalid types remain outside this certification. Four
mutants (skip last inventory slot, wrong leading blank, wrong texture flag,
disabled renderer) are rejected. The production Buy-cancel regression also
passes O0/O2/ASan+UBSan and rejects its undersized-buffer mutant.

All544 annotated assembly bytes match the disc, SHA256
`deb7098105f27cf3db7fb65c21932c1d3eeb91e92bea4395b5d4f4e2d0b9595a`.
Compiled misc8 whole .text remains unchanged, SHA256
`91cef40c4eb8ff1f2d08e73d589a18a1d49269a9f49fa32123173976dd1ba200`.
Full native build passes; func_801CE91C is defined in the executable.
Evidence: `shop-stored-{test,build,cancel-regression}.log` and
`shop-stored-misc8-*.text` in the route scratch directory.

No new live purchase/route claim: process458481 remains on the previous
Buy-cancel build at the earned scenario8 checkpoint. The selected-item caller
801CEB3C is still a stub, so this helper has not yet been reached naturally in
the shop. Port that full caller next, retaining the equipment-preview dependency
801CE480 as an explicit remaining gap, then verify normal purchase and travel.
Blackmoon completion remains unproven.

## Selected-item update restored; normal supply purchase observed

Native func_801CEB3C follows retail801CEB3C..801CF2A0 for all three valid item
types: description lookup/upload, returned price, equip eligibility highlights,
equipped markers, stat-preview digit/color calls, and the Stored quantity chain.
It uses the last three positions of the nine-digit buffer (retail menu+0x322),
not the first three. It preserves compact portrait indexing across unavailable
characters and unchanged stat contexts where the preview value is zero.
All implementation changes remain under XENO_PC_PORT.

`run_shop_selected_retail_test.sh` compares the native production selected-item,
stored-count and inventory-lookup chain with the disc MIPS. Graphics/text,
equipped-flag and stat-calculation helpers are observed with deterministic shared
responses.1,536 cases exercise all three types, both contexts, four row/scroll
positions, empty/sparse/nine-member availability patterns including character15,
zero/nonzero item IDs, price extremes, equip masks, both stat colors, zero and
1/9/10/99/100/999/1000 preview values. O0/O2/UBSan each pass5,067,229 checks.
All469 valid-type instruction sites execute; four invalid-type jumps are
excluded (801CEBC8/BCC/BDC/BE0). The stat helper801CE480 remains unported: these
tests verify its caller contract, not that helper's gameplay calculation.
Five mutants (zero item price, wrong digit positions, broken portrait indexing,
nonempty description for ID0, wrong stat color) are rejected. The Buy-cancel
regression now includes the actual selected-item and Stored routines; it passes
O0/O2/ASan+UBSan and still detects the four-byte caller buffer overflow.

All1,892 annotated selected-item assembly bytes match the disc, SHA256
`22046557ec7d501b7b6901b314d92b64eb2c3ce38c23880c7b098ce1e6b627ad`.
Compiled misc8 whole .text remains byte-identical, SHA256
`91cef40c4eb8ff1f2d08e73d589a18a1d49269a9f49fa32123173976dd1ba200`.
Full native build passes. Raw evidence: `shop-selected-{test,build,cancel-test}.log`
and `shop-selected-misc8-*.text` in the route scratch directory.

Live process508015 (`resume9.json`/`runtime-resume9.log`) restored the earned
checkpoint, used ordinary shopkeeper dialogue and Buy, then showed Aquasol's
"Restores HP (50)" description and Stored0. Normal quantity inputs selected
five Aquasols, one Rosesol and one Omegasol. The confirmation displayed250G;
Left selected Yes and confirm completed the purchase. Read-only before/after
inventory inspection shows gold300→50, IDs1/6/49 with quantities5/1/1 (initial
consumable inventory empty). Reopening Buy displays Stored5 for Aquasol and
unaffordable rows dimmed. No shop stub call was logged on this consumable path.
No quantity, gold, script or result was written through a debugger.

Normal cancel/cancel/farewell and F7 saved field6(-144,0,-152), scenario8,
HD2D on, speed1x. Process508015 remains alive there. The pre-purchase earned
checkpoint is backed up as `shop-prepurchase-earned.xgqs`; original user saves
remain untouched. Screenshots: `shop-selected-{buy,supplies,confirm,purchased,
stored-five,farewell,saved}.png`; inventory evidence:
`shop-{prepurchase,postpurchase}-inventory.txt`.

Next: travel naturally to Citan with these supplies and Alice's scenario8 intact.
Equipment-preview calculation801CE480 is still a fidelity gap; no equipment
purchase/stat-preview runtime claim is made. Blackmoon completion remains unproven.

## Supplied route to Citan and Yui scenario10 observed

Continued process508015 on the selected-item build without loading another save
or editing game state. Normal movement/talk exited shop6, crossed Lahan1, and
entered mountain15 with Alice's scenario8 intact. Three naturally triggered
encounters were won with ordinary Attack/confirm controls. Each returned to the
field and accepted movement; retail interpreter return instruction counts were
73,841,440;45,163,074;56,686,137. The bounded attack helpers stopped on the logged
battle return. No keyboard Escape bypass was used.

After the second victory, the field menu showed level2 Fei at40/60HP and10/11EP,
the purchased supplies, and a Hob-Jerky battle drop. Normal Items/Aquasol/Fei
selection consumed exactly one Aquasol (5→4) and restored HP40→60, confirmed
both visually and by read-only state inspection. Three cancels returned to the
field. The third encounter began at60HP and ended at57HP. No health, inventory,
position, script flag or result was injected.

Natural traversal passed the western ascent and first running-jump section,
walked the narrow upper ledge, and jumped from(-643,-405,-760) across the gap to
(-1306,-449,-745). The bridge was crossed to(-1099,-575,1318), followed by the
northern trail and exit into field17. No collision/gameplay change was needed.
The mountain guide dialogue and Citan-area entrance camera sequence completed;
the latter released control and fulfilled the queued F7 save at(26,175,-1706).
The resulting scenario8 checkpoint is `citan-scenario8-earned.xgqs`.

Normal door interaction entered Yui's house19. Walking around the doorway/table,
with four ordinary L1 camera taps for visibility, reached Midori and Yui. Both
conversations ran normally. Yui's required conversation advanced scenario8→10;
a follow-up backyard reminder was also completed and visibly dismissed.
F7 then saved field19(47,0,413), scenario10,57/60HP, four Aquasols remaining.
`yui-scenario10-earned.xgqs` preserves that actual save. Read-only final state
is recorded in `yui-scenario10-state.txt`. Current controls are rotated180degrees
inside this room: screen Down increases world z; screen Right decreases world x.
Recheck after leaving the room. The character is near Yui, with no dialogue open.

Evidence lives under the route scratch directory: `runtime-resume9.log`,
`inputs.log`, `fight-resume9-{1,2,3}.log`, `route-heal-{menu,items,target,result,
return}.png`, `route-heal-result.txt`, `route-gap-{takeoff,land}.png`,
`route-bridge-{entry,crossing}.png`, `route-citan-{arrival,entry-control}.png`,
`route-yui-{talk2,dialogue3,dialogue7,scenario10,closed}.png`.
Process508015 remains live on display95/window2097204; input helpers finished.
Speed1x and HD2D remain enabled. Original user saves are unchanged.

This continuation changed no engine source and required no rebuild. It proves
this supplied scenario8 mountain route, three battle returns, field healing,
and Yui's scenario10 gate. It does not prove full audiovisual retail parity.
Next: leave house19, approach field17's workshop-side rooftop trigger now that
scenario10 is set, then Citan/music box/dinner/night return/burning Lahan and
Blackmoon. Those later gates and equipment-preview helper801CE480 remain open.

## Rooftop, music box, dinner and nighttime departure completed

Continued live process508015 from the earned Yui scenario10 state, with normal
movement/dialogue controls and no load or game-state edits. Leaving house19
returned to field17. Walking around the house to the workshop automatically
triggered Fei's call to Citan at(-451,31,415). The rooftop camera sequence,
Citan/Land Crab dialogue, and return of control completed, advancing to scenario12.
This is the naturally satisfied rooftop gate that previously failed at scenario7.

Normal workshop-door interaction loaded field21. Approaching and examining the
music box started its complete visual/story sequence: opening panels, statue,
light/particle effects, Fei's reaction, Citan's entrance and explanation, Fei's
scripted exit, the statue's destruction and Citan's closing dialogue. The scene
then transitioned to field19's dinner sequence at scenario13. Dinner dialogue
and actor movements completed, followed by the nighttime exterior conversation
in field17. Control returned at scenario15, position(65,31,-1153).

F7 saved this free-control state. `night-scenario15-earned.xgqs` is a preserved
copy of that actual save; `recovery.xgqs` is the current working checkpoint.
Read-only `rt15-night-state.txt` confirms scenario15, Fei57/60HP and remaining
consumable quantities4/1/1/1. No new STUB/FATAL/ERROR log entries were found.
Process508015 remains live on display95/window2097204, speed1x, HD2D enabled;
all input helper processes have finished. Recheck camera-relative directions
before descending: the rooftop/night cutscenes changed camera orientation.

Raw evidence remains in the route scratch directory: `runtime-resume9.log`,
`inputs.log`, `rt10-roof-trigger.png`, `rt10-roof-dialogue{1,4,7,11,14}.png`,
`rt12-workshop-inside2.png`, `rt12-musicbox2.png`, `rt12-musicbox-wait.png`,
`rt12-musicbox-timed.png`, the `rt12-musicbox-dialogue*.png` sequence,
`rt12-closing-timed.png`, `rt12-after-musicbox.png`, `rt12-transition.png`,
`rt13-dinner-dialogue{5,11,17,23}.png`, and `rt13-night-control.png`.
`roof-approach.png` was reused during this continuation; the current trigger
capture has also been copied to the distinct `rt10-roof-trigger.png` name.

No engine source changed and no rebuild was needed for this gameplay pass.
The evidence establishes natural progression and observed scene/transition
behavior, not a complete retail audiovisual comparison or human audio audition.
Next: nighttime descent toward Lahan, the incoming-Gear event, burning-village
and Gear-battle sequence, aftermath/world departure, then Blackmoon Forest.
Those gates and equipment-preview helper801CE480 remain unfinished.

## Night descent, burning Lahan and scenario24 world departure completed

Continued process508015 from the earned scenario15 night exterior using normal
movement, jump, dialogue and battle inputs. Field17's south exit loaded field15.
The nighttime bridge trigger advanced scenario15→16; Fei's Giants dialogue and
Citan's arrival/conversation completed and returned control at(-1053,-540,-193).
F7 saved there; `bridge-scenario16-earned.xgqs` preserves that actual checkpoint.
The incoming-Gear cinematic itself was not fully captured visually: a black
transition frame and the resulting dialogue were observed, so do not claim its
complete audiovisual presentation was verified.

Descending the mountain naturally reached burning Lahan(field2). The return jump
from the western platform landed on the lower path at(-728,-202,-712); normal
walking then reached the south exit. The village's fire effects, Alice/Timothy
conversation, Citan's departure, Dan/Gear events and cockpit movie progressed.
The cockpit STR decoder exited at frame563/end563 and field dialogue resumed.
Normal confirms advanced the Gear startup tutorial; a heavy attack consumed
30fuel and defeated the first enemy, while enemy damage reduced Weltall's HP
from1800 to1704. Further normal attacks completed the playable encounter.
The bounded input helper stopped as soon as its return appeared in the log.

Three consecutive retail battle interpreter invocations returned successfully:
241201233 instructions for the playable/tutorial fight,97623088 for the
reinforcement story sequence,107654881 for the later scripted confrontation.
The latter two advanced through normal dialogue/timed events. No battle escape
shortcut, forced result, modified stats or debugger state writes were used.
The destruction sequence then transitioned to Fei waking near the survivors.
Dan's confrontation, Citan's intervention, Blackmoon/Aveh directions, departure
and intervening camera/flashback scenes all advanced to the world map.

A Down/Down+c movement attempt appeared stationary, so a possible stall was
investigated before editing. Read-only guest slot1 position samples and a Right
input proved actual world movement to the Mountain Path entrance: position
changed from0x074fb000/0x02b43000 to0x0767d880/0x02900100 (x/z fixed point).
The cause of the earlier stationary direction was not determined; no general
world-map stall is established by that observation. F7 on the world map was
explicitly rejected as not a gameplay field, leaving the bridge save intact.
Normal z interaction re-entered field15 at(585,101,-1700), scenario24, with
free control. F7 then saved successfully. `post-lahan-scenario24-earned.xgqs`
preserves this actual save, and `recovery.xgqs` is the current working copy.
Read-only `rt24-earned-state.txt` confirms scenario24, Fei73/73HP and consumable
quantities4/1/1/1 (IDs1/6/49/57). Original user saves remain unchanged.

Raw evidence is in the route scratch directory: `runtime-resume9.log`,
`inputs.log`, `rt15-*.png`, `rt16-*.png`, `rt17-*.png`, `rt18-*.png`,
`rt24-world-right.png`, `rt24-mountain-reentry.png`, `rt16-fight.log`,
`rt18-world-stall-state.txt`, `rt24-world-held.txt`, `rt24-earned-state.txt`.
Screenshot prefixes rt17/rt18 were chosen before reading the actual final
scenario24 and must not be treated as measured scenario values.

No engine source changed, so this pass required no rebuild or new MIPS byte
comparison. It establishes observed gameplay progression and battle/field/world
handoffs, not exhaustive retail audiovisual parity. Process508015 remains live
on display95/window2097204 at the saved field15 entrance, speed1x, HD2D enabled,
with no input helper left running. Next: leave Mountain Path normally, navigate
the world map to Blackmoon Forest, and test its story, traversal and encounters.
Full route completion and equipment-preview helper801CE480 remain unfinished.
