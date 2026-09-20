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

## Repeated world-entry guard lifetime repaired

Leaving the earned scenario24 Mountain Path checkpoint in process508015
reproduced a fatal second field→world transition. The runtime log ends with
`gpu-asset-b ERROR: already ran this process`, slot1 setup failure and process
termination. This was a native guard failure, not a retail script gate.
`MainLoop` clears/reloads/decompresses the selected overlay on every state
change (`src/slus_006.64/main/main_loop.c`). The native static stage ownership
flags instead survived the previous world visit. GPU asset B was the first
rejecting stage; subsequent asset/record/upload/WDS stages had the same lifetime
mismatch, as did the work-buffer dispatch count and ready-consumer entry count.

`PcPort_WorldMapInitMain` now resets those native guards at the entry boundary.
Duplicate-stage checks remain active within each visit. No guest flags, stats,
coordinates, retail source logic or MIPS code changed. Diagnostic wording now
identifies the world entry rather than the process as the guard lifetime.

`run_world_entry_guard_test.py` extracts the actual production entry prefix,
seeds prior-visit native guards and verifies three fresh entries. It failed on
the original code, passes atO0/O2/UBSan, and rejects18 omitted-reset mutations.
Its subsystem-reset calls are stubs; this focused test proves guard lifetime,
not resource reloads or runtime behavior. The existing slot1-owner suite passes
O0/O2/UBSan with13 negative controls detected. The container lacked rg, so its
simple `rg -q` checks used an exported grep wrapper for that run; assertions
were unchanged. Full native builds pass (75function stubs,96adopted leaves).
The annotated retail setup span80072238..80072620 matches all1000 disc bytes,
SHA256 f5df46873f4b7463ba67f850b62ee144dc0e8b089639c232803739a881a09903.
This is a disc/assembly comparison, not a claim of whole-port byte identity.

Fresh process724853 loaded the earned scenario24 save via F8, exited Mountain
Path to the world, walked to its entrance, entered withz, then exited again.
Both world entries completed asset setup and rendered normally in the same
process; the original failing lifecycle is now observed passing. Live binary
pins are in `resume10.json`, log `runtime-resume10.log`. The final rebuild after
this runtime check changes only diagnostic wording relative to that live binary.
No gameplay state was forced; the original user saves and HD2D setting persist.

World navigation remains in progress. Left/Right inputs produce travel. From
clear ground near the forest, a one-second Down sample left slot1 x/y/z at
0x07408000/0xffedb000/0x029b0000. This is evidence for further collision/input
investigation, not enough to attribute a root cause or certify navigation.
Process724853 remains live on display95/window2097204 at that world position.
The latest working checkpoint is still the earned scenario24 field15 entrance;
no new world save was attempted. No input helper remains running.

Evidence: `bm01` attempt failed because the original process had exited;
`bm03-world-first.png`, `bm04-mountain-target.png`, `bm05-world-second.png`,
`bm06-world-west.png`, `bm10-clear-ground.png`, `bm10-{before,after}-down.txt`,
`world-entry-{guard-test,owner-tests,build,final-build}.log` under route scratch.
Blackmoon entry/traversal/encounters and the equipment-preview helper remain
unverified/unfinished. The full goal is still active.

## World direction delay-slot transcription corrected

Read-only breakpoint observation in process724853 found that Down never supplied
nonzero velocity to95414; a subsequent Right input reached it with(4096,0,-4096).
`bm11-down-resolver.txt` therefore records the subsequent Right call, despite
its initial filename. Retail80090B6C calls rcos;80090B7C stores its result in the
jal-rsin delay slot BEFORE rsin executes. The native90A84 helper incorrectly
stored sine to+0x38 and negative sine to+0x40, making two cardinal directions
stationary. The old focused test encoded the same wrong sine expectation.

Corrected the native store and call/store order, with the existing test changed
first to the disc-backed cosine expectation (observed RED). Added execution of
the actual56-byte disc sequence80090B50..80090B88 via the MIPS adapter; distinct
trig doubles are shared with native and the rsin boundary asserts that cosine
has already been stored. All16 existing mutations are detected, including the
former sine-store bug, with O0/O2/UBSan passing. The runner drops the C-inapplicable
fpermissive flag and uses grep for its simple assertion check. All476 annotated
bytes80090A84..80090C60 match disc/world_map.bin, SHA256
3d44f71483da56bb24aa5df0fc0ff0f32462a8e2c051e7a948db94284aa86fa5.
Only native code/tests changed; the retail compiled source is untouched.

Build passed. New process790479 (`resume11.json`, `runtime-resume11.log`) loaded
the earned scenario24 checkpoint. A mountain encounter occurred before exit;
the heavy-only helper timed out, then normal c/x inputs completed the fight,
and reward confirms returned through the pending exit to the world. No battle
bypass or state edits were used. Down then changed x07580000→073a0000 with
z02900000 unchanged (`bm19-{before,after}-down.txt`), proving the repaired cardinal
movement. Left followed by Down reached the visible Blackmoon Forest label
(`bm21-forest-trigger.png`). Normalz entered the forest loader, but process790479
terminated during field initialization. This new crash remains unresolved;
forest playability is NOT established. The failed bm22 capture was not created.
The earned scenario24 checkpoint is intact. No game process remains running.

User additionally requested a toggle for invulnerability in on-foot and Gear
combat. Implementation is pending; default is planned OFF and retail acceptance
must continue with it OFF. This does not change the natural-route goal.

## Optional combat HP protection toggle

Added a native toolbar `GOD OFF` / `GOD ON` button. Default is OFF on each
process launch; clicking it toggles party HP damage protection in both foot and
Gear battles. ON is shown in red. The setting is not written to game saves.
It does not revive defeated actors or guarantee protection against status
changes, scripted defeat or other non-HP mechanics. EP and fuel remain retail.

The native battle interpreter intercepts entry to retail `func_80085618` and,
only while enabled, zeros positive queued HP damage for active party slots
0..2 (damage kinds 0/5/7/8). It preserves enemy slots 3..10, healing and other
action kinds. Retail damage/death handling then executes unchanged. No retail
source or disc instruction is patched. This feature is explicitly a testing
option; route acceptance continues with it OFF.

Validation: the test runner checks all 1,196 annotated instruction bytes at
80085618..80085AC0 against `disc/battle.bin` (SHA-256
`05fc6de8ea8fb22b5c59d3ca757c1334ada092ae9262dcae95f47f0ec5c67b89`).
It executes that actual disc routine using the port interpreter, with external
slot-mask/death-notification callees replaced by fixtures. Each O0/O2/UBSan run
passes 2,116 cases covering both modes, all 11 actor slots, all 12 action kinds,
two action rows, lethal/nonlethal damage, healing, signed-foot versus unsigned-
Gear damage and switching protection back OFF. Toolbar hit-zone tests pass.
These controlled combat fixtures are not natural-route acceptance evidence.

Native build passes. Live toolbar clicks emitted ON then OFF; initial visual
inspection found the missing G glyph and insufficient label width, both fixed
before final validation. Evidence logs/images are local under
`scratchpad/blackmoon-route-20260919/god-mode-*`.

Final UI validation: fresh process 869720 (`runtime-resume13.log`) launched
with protection OFF. Normal mouse clicks toggled ON then OFF, confirmed by
logs and `god-mode-final-on.png` / `god-mode-final-off.png`; the complete
labels are visible. Final native build and toolbar boundary tests pass.
The game is left running at title, HD2D ON, God mode OFF. The earned route
checkpoint is unchanged; Blackmoon field initialization remains unresolved.

## Blackmoon entry collision-flag pointer repair

Reproduced the entry crash on commit 04f1dc28 from the unchanged scenario24
checkpoint, with HD2D ON and God mode OFF. Normal controls: exit Mountain Path
with Down+C, then world Down 1s, Left 2s, Down 1s; visible Blackmoon Forest
label; Z to enter. GDB captured `func_800924D4(index=152, component=0,
value=216)` dereferencing `0x0076bcf00077339c`. The table held two independent
words `0077313c 0076bcf0`; the port's `extern u32*` loaded both into a 64-bit
pointer. Retail's LW at 8009251C loads only the first word, as does the field
loader's existing packed-u32 table representation.

Native getter and setter now explicitly widen table word zero. The retail
build keeps its original declaration and expressions. The native high-byte
setter uses an unsigned shift, resolving the UBSan failure for value216 <<24.
No bounds clamp, skipped script or substituted collision behavior was added.

`run_field_collision_flags_test.sh` compiles the production TU and compares
getter/setter behavior against the actual disc MIPS through the interpreter:
all256 indices, all4 components, 5 byte values, plus unchanged-neighbor checks.
5120 cases pass at O0, O2 and UBSan. The identical runner against the pre-fix
production TU segfaults (exit139), demonstrating sensitivity to this bug.
Annotated getter/setter instructions match disc bytes (176 and204 bytes):
- getter SHA256 b6ec77629f5f3aadac9084334163fb0377430ce8d33ea3a38d53999f819df367
- setter SHA256 c125f26e7774584bd3b9287e5b45c278f3c5f3d978de82ed30f925e580d25d2d

Raw evidence: `forest-repro.gdb`, `forest-flags-tests.log`,
`forest-flags-negative.log`, and `forest-flags-build.log` in the local route
scratch directory. Live fixed-build acceptance is pending below.

The legacy MIPS compiler successfully rebuilt the production misc11 object.
Its `.text` is byte-identical to the pre-edit object; SHA256
`d24bf8c558d42731d4dc8d8391e36a89ebf34c13dfbd9db403d9e403c8baac03`.
Host ninja emitted its three commands for execution inside the toolchain
container (which does not contain ninja). No compiled-retail behavior changed.

On the first fixed-build retry (resume14), a normal world-map encounter occurred
before entry. The battle returned through retail, then world reload aborted in
`PsyX_SPUAL_Write`'s size/address assertion. Its core stack includes
`SoundLoadWdsFile -> SoundHandleError -> SoundLoadWdsFile -> SpuWrite` during
`wm_first_wds_consumer`. This separate audio-bank lifecycle failure is OPEN;
it must not be hidden by relaxing the SPU assertion. Resume14's core is retained
by systemd (PID926167). A fresh retry uses the same untouched checkpoint.

Fixed-build live acceptance (resume15, PID934514): normal world travel reached
the Blackmoon label, Z entered field22 successfully. Fei started at
(1949,0,1561), scenario24, control enabled. Holding Up for0.5s moved him to
(1949,0,1486), visible in before/after screenshots. HD2D stayed ON, God mode
OFF throughout. F7 saved the earned forest checkpoint to the scratch recovery
path; the previous mountain checkpoint was backed up first as
`pre-forest-scenario24-earned.xgqs`. Original user quicksaves are untouched.
Evidence: `resume15.json`, `runtime-resume15.log`, `resume15-label.png`,
`resume15-forest-entered.png`, `resume15-moved.png`. This proves entry and initial
movement, not completion through the forest or resolution of the separate
world battle-return SPU failure. Game remains live at the forest entrance.

## Forest traversal, combat and healing follow-up

Continued live PID934514 on 88b6b2e9 with God mode OFF / HD2D ON. Two normal
forest encounters completed and returned to map22, with movement and the menu
working afterward. The second victory screen showed level5, EXP229, HP38/73,
EP14/14. Used one existing Aquasol through Items and the normal Fei target
prompt: HP38->73, quantity4->3, then closed the menu and continued walking.
Screenshots `forest-battle01..03.png`, `forest-menu02..05.png`, and
`forest-healed.png` preserve the UI observations. The 45-input battle helper
expired on the second victory/reward screen; manual Z confirmations completed
the return, so that helper timeout was not an engine stall.

Traversal has not yet left map22. Normal walking, camera rotation and a jump
input were tested around trees and ledges; `forest-walk01..20.png` preserve
views. Latest F7 checkpoint is map22 (-1098,0,141), scenario24, after healing.
`forest-entrance-earned.xgqs` backs up the original forest entrance checkpoint.
No teleports, flags, stat writes or encounter skips were used. The live game
remains running. Full forest/story completion remains OPEN.

Further read-only analysis of resume14's world battle-return core identified
size=0 at PsyX_SPUAL_Write, called for the zeroed native D_80050940 error bank.
The preceding error is SoundSpuErrorId31 (allocation failure) while loading
bank46 (196608 bytes). Bank46 is already resident at SPU229376 and remains the
g_GameCurLoadedWDS owner. C894 is zero in the dump. Retail branches around the
world WDS load at800724C4 when C894!=0; the native slot1 owner contains that
branch too. Next diagnosis must trace why the resumed world takes the fresh
session path / retains that ownership; do not change allocation behavior or
relax the SPU assertion to hide it. Evidence: `world-battle-return.gdb`,
`world-audio-banks.gdb`, `world-audio-owner.gdb`, and the preserved systemd core
(PID926167). No audio repair has been made yet.

## World battle-return entrance ownership repair

The resume14 core proves a split transition flag: native D_8006F954=0001,
guest RAM at8006F954=8000. Native world initialization reads the game-state
blob (the native D_8006F954 alias at offset2320). Native slot2 teardown had
instead read/written the guest mirror for retail80072A68..74. Consequently the
next entry missed the resume bit and attempted to allocate resident bank46
again. The resulting allocation error entered an unpopulated error-bank path,
which attempted a zero-size SPU upload. This is upstream of the SPU assertion.

The native-only teardown now ORs8000 into the native entrance owner. Retail
logic, sound allocation and the SPU assertion are unchanged. The strengthened
production test seeds native0123 versus guest0456, requires native8123 and an
unchanged guest value on the battle branch, and requires both unchanged on
state0. The pre-fix production code failed both ownership assertions. O0/O2/
UBSan plus ten mutation checks pass, including the old guest-write behavior.
The 532 retail teardown bytes at8007299C..80072BAC match disc/world_map.bin;
SHA25646d838ed3e2342dd1af4e5701f7b16c686704cdf81bb51eea516b32124be2c9b.
Native build passes. No MIPS-build source was changed.

Natural validation uses a copy of the earlier scenario24 Mountain Path save
(`world-return-checkpoint.xgqs`); the newer forest recovery save is preserved
and hashed in `forest-save-before-world-test.sha256`. PID934514 was stopped
normally after its earned checkpoint had been saved. Fixed-build resume16
launches with HD2D ON and God mode OFF; live validation follows below.

Resume16 natural test: world walking triggered an encounter, retail battle
returned, and native entrance8001 was observed during battle. Reload reached
C894=1, skipped the duplicate WDS load and passed audio setup. It then crashed
at wm_native_animation_value(sprite_bits=0), called by slot1's update callback.
The resume16 core and `world-return-restore.gdb` preserve this subsequent fault.
No successful complete return is claimed for this intermediate fix.

Diagnosis found the resume convergence branch80072784..80072908 still only
represented as a cut PC. The slot1 owner discarded that result and went to the
common tail. Retail instead uses800976FC to reset twelve saved slots to init
state and install their initialization callbacks (plus selector-dependent
slots15/16/17/19). Snapshot teardown had correctly freed and cleared the old
sprites; skipping initialization ran update with a null sprite.

Added that native resume branch and routed the owner to it on the existing
FLAG1_ARC result. A test executes the actual disc branch AND helper in the MIPS
interpreter and compares the complete8192-byte pool against production for
selectors0..14 andUINT_MAX: all branches pass O0/O2/UBSan. Byte checks cover
392 bytes of branch and28 bytes of helper. The owner test now models the actual
C894-dependent convergence result and requires the resume call in sequence;
its new skip-branch mutant fails, all14 owner mutants detected. No null-sprite
bypass or forced runtime state was introduced. Logs: world-resume-callback-tests,
world-resume-owner-tests and world-return-build2.

Resume17 reached a second natural world encounter, completed retail battle,
and entered the resumed world without the prior SPU assertion or null-sprite
update. It stopped at a newly exposed porting gap: scheduler reports
`guest=0x8008a52c kind=invalid_callback slot=1 state=0`. The required retail
resume initializer is not registered/implemented in the native scheduler.
Sent TERM to PID1034174 because it was repeatedly logging this known
frontier; a later process check found it still alive, so KILL was required; no successful full world battle return is claimed. Forest recovery
hash remains unchanged. Runtime/launch evidence is resume17.json and
runtime-resume17.log; screenshot world-return17-restored.png is black and is
NOT visual acceptance.

Next required work: implement and register the retail resume initializers
selected by80072784. First is8008A52C..8008A5B8 (140 bytes, exclusive end): same sprite creation,
animation0, scale1800 and flag-bit2 clear as the first part of cold initializer
8008A2C8, but crucially preserves saved movement/state. Do not substitute the
cold initializer because it resets the saved position. Inspect the remaining
selected callbacks for missing scheduler entries too; avoid only satisfying
slot1 and leaving the next slot unresolved. Keep branch/MIPS differential
coverage and retry a complete natural encounter after callback completion.


## World battle resume initializers and live return (resume18)

Implemented the twelve missing initializers selected by the retail resume branch:
8008A52C, 8008B498, 8008BD1C, 8008C6EC, 8008D520, 8008DE9C, 8008E4F4,
800907C4, 80087F60, 8008868C, 800879E0, and 80088C90. The native-only module
recreates released sprites without resetting their saved pose, restores channel
objects and model visibility/matrices, and reinstates context links and textured
packet state. The last operation includes the previously missing 80087904 helper.
Registered all twelve through the production scheduler's weak bindings/adapters.
No retail MIPS source changed.

`run_world_resume_initializers_test.sh` verifies every annotated instruction byte
of these thirteen routines against disc/world_map.bin, then executes their actual
retail instructions in the MIPS interpreter. The production native callbacks are
called through the production scheduler resolver/adapters. External dependencies
use matching test bridges; 80087904 itself executes as retail MIPS. Comparisons
cover return values, ordered helper arguments, full RAM below the guest stack,
complete scratchpad, all 64 slot indices, absent/present party members, modes
including signed extremes, both EE68 result branches, and zero/multiple packets.
Native SpriteData pointers are normalized to a guest object address for comparison.
All 1,152 cases pass independently at O0, O2, and O2+UBSan. Seven deliberate
regressions are rejected: lost saved position, wrong sprite scale, missing sprite
flag clear, absent-party creation, channel mode boundary, packet transparency,
and omitted scheduler binding. Evidence: world-resume-initializers-tests.log.

Native build passed with 75 generated function stubs and 96 adopted leaves checked;
log world-return-build3.log. Resume18 ran that binary (launch JSON records hash)
from the isolated earned world-return-checkpoint. A normal encounter completed
with ordinary battle inputs and returned after 29,546,194 retail instructions.
The resumed scheduler executed the new initializers, and world update passes
continued. Visually inspected world-return18-restored.png (terrain, Fei, map)
and world-return18-moved.png after Left 0.6 seconds: movement and terrain scrolling
work. Both show SPEED 1X, FEI HD2D ON, GOD OFF. No teleport, forced combat outcome,
or altered player stats used. Forest recovery hash still matches its pre-test
snapshot. Full Blackmoon Forest story completion remains pending.

## Aquasol F2 packet-address crash (resume19/20)

Resume19 continued normal map22 traversal and encounters. Using Aquasol during
a four-Hobgob battle crashed in native `func_800B2AEC`, called by animation
opcode F2. The core shows a native model pointer (`0x781454`) alongside guest
packet-buffer addresses (`0x8013d010`, `0x8013e410`), with RGB deltas -7.
Dereferencing the latter as host pointers caused SIGSEGV. Evidence:
`forest-aquasol.core`, `forest-aquasol.gdb`, and `runtime-resume19.log`.

The native primitive-color leaf now independently translates KSEG0/KSEG1 RAM
arguments through `PSX_ADDR`, preserving native pointers and the existing
packet/color logic. Translation is guarded by `XENO_PC_PORT`; the matching
retail build still uses its existing assembly body. This native leaf is not
claimed to compile byte-identically to retail.

The regression runner pins the actual 2,140-byte battle routine and 44-byte
SLUS clamp and executes those instructions for the comparison. Added 192 cases
covering every independent native/guest argument combination, both cached and
uncached aliases, twelve supported packet shapes, two records, and guards.
All 4,422 cases pass at O0, O2, and UBSan. Removing the address translation
reproduces SIGSEGV; the seven existing arithmetic/layout mutants are also
rejected. The native build passes (75 generated function stubs, 96 adopted
leaves checked). Logs: `aquasol-pointer-tests.log`, `aquasol-pointer-build.log`.

Resume20 loaded the earned map22 checkpoint with Aquasol count3, HP66/73,
GOD OFF, HD2D ON and 1X speed. Normal Aquasol use in a two-Armor-Grub encounter
survived the former crash point; HP69/73 after the next enemy hit and subsequent
turns were visually observed (`aquasol20-result.png`, `aquasol20-nextturn.png`).
The battle was won normally, with HP27/73 and Bizfruit1 awarded
(`aquasol20-aftercombo.png`, `aquasol20-return.png`). The field resumed,
ordinary Left input changed the player position, and the field menu opened.
`aquasol20-inventory.png` visibly confirms Aquasol2: exactly one consumed.
Field return/menu functionality is observed, but return-position fidelity needs
investigation: the last pre-battle position log is (613,0,1021), while the first
stable post-battle position is (96,37,299), with the view obscured by a tree.
No conclusion about the cause or retail-correct return positioning is drawn.
The earned recovery checkpoint is not overwritten with this return state.

Rendering remains incomplete: both runs report unbound animation render table
index15 (retail `func_800257F0`). Its decompiled body still has packed-pointer
loads unsafe for the native build, and its `func_800B1F6C` dependency remains a
generated stub. The crash fix does not establish healing-effect visual parity.

Navigation finding: the map22 route climbs the southwest root staircase before
following the raised western path north; the exit log cannot be reached by
jumping directly from the ground below it. Read-only decoding of guide actor22's
retail jump script gives these (x,y,z) landings: (-847,-42,-1030),
(-928,-90,-925), (-999,-173,-933), (-1097,-193,-899), (-1219,-252,-964),
then (-1218,-158,-1060). These are navigation evidence, not injected poses.
The next attempt must turn south as well as west at the fifth landing.
No movement defect has been established from the failed ground approaches.
The earned recovery checkpoint predates the Aquasol crash; the backup
`pre-aquasol-crash-earned.xgqs` preserves it. User saves remain untouched.

## Traced field return and root-staircase progress (resume20 continued)

Loaded the preserved earned checkpoint and triggered another encounter using
ordinary movement. Read-only GDB tracing captured the field snapshot at
`func_8007954C` and again at entry to `func_800A3474` after a normal victory.
The complete 65,536-byte snapshot is identical before and after battle:
SHA-256 `d39119d6c4ea5e10bf37faa4ad4335bbc1b451c166dc11f62656fb7380fe61ab`.
Player actor4 was saved at (-697,0,777), and settled at (-696,0,762) after
restoration. The restore path is used and saved data was not damaged in this
repro. The earlier comparison used a periodic pre-battle movement sample rather
than the actual saved position; its large discrepancy does not establish a
return-position bug. A small post-restore displacement remains unclassified.
No code change was made from that inconclusive observation. Evidence:
`field-return-trace.gdb`, `field-return-trace.log`, `field-save-before.bin`,
`field-save-after.bin`, and `field-return-battle-end2.png`. GDB detached normally.

Normal traversal then cleared the southwest root staircase. Observed landings:
(-834,-38,-1026), (-918,-85,-943), (-988,-171,-934), (-1086,-193,-900),
(-1202,-258,-968), and (-1234,-162,-1044). A jump from the lower slope to the
fifth landing needed vertical clearance before lateral movement: X for .15s,
then Right+Up+C for .12s while X remained held. This is ordinary controller
input, not a position/velocity write. A stationary jump measurement also showed
the expected approximately119-unit rise; collision changes are not justified.
Screenshots `forest56.png` through `forest65.png` show the ascent and western
ledge. The save-point interaction at (-1626,-143,-981) opened the field menu
(`forest66.png`). Exited normally and saved that earned location to the isolated
recovery checkpoint using F7. User memory cards and quicksave remain untouched.
HD2D ON, GOD OFF, SPEED1X throughout. The next natural encounter occurred while
following the raised western path toward the log/boulder event. Elly, the boss,
and the forest exit remain pending.

## Deferred graphics-free guest pointer (log-side encounter)

Continued along the raised western path, won the next two-Armor-Grub encounter,
and returned at the pre-battle position (-1698,-143,-39). Normal movement around
the stumps reached the log/boulder area. F7 saved the earned checkpoint at
(-1071,-189,953), HP48/73. Approaching the log's Hobgob and interacting normally
started the four-Hobgob encounter (`forest87.png`). Aquasol use then produced a
new SIGSEGV. Resume20 is terminated; its core and stack are preserved as
`log-hobgob-aquasol.core` and `log-hobgob-aquasol.gdb`.

The color-update fault fixed by 0da7e4ae was passed. The new fault was in
`HeapFree(0x8013d010)`, called by native graphics-context reset `func_800250E0`.
The deferred list still contained guest packet pointers. Its consumer now
translates the list head, payload and next link independently using the existing
graphics address helper, solely under `XENO_PC_PORT`. The free order, reading
the next link after each free, context selection and list clearing are unchanged.
No retail-source branch or heap algorithm is changed.

`run_gfx_deferred_free_retail_test.sh` compares every annotated instruction byte
of 800250E0..80025180 with the disc (160 bytes, SHA-256
`305e3a5c2f2ea2284536e0f47b7760caaa1827edd657d98faf06ab2c27834ac3`).
The actual disc routine executes in the MIPS interpreter; HeapFree is bridged
to an ordered-call observer in this fixture. The production native function is
linked from the complete temp1 translation unit. Each O0/O2/UBSan run passes
1,458 cases: both contexts, zero/one/two nodes, independent native/KSEG0/KSEG1
head/link/payload domains, untouched other-context list, graphics cursor/end
state and node guards. Removing each of the three conversions independently
is detected. The original code fails the new suite with SIGSEGV. This is finite
boundary coverage, not a whole-allocator or visual parity certificate.

The native build passes (75 generated function stubs, 96 adopted leaves checked).
Logs: `gfx-deferred-free-red.log`, `gfx-deferred-free-tests.log`,
`gfx-deferred-free-build.log`. Resume21's launch record pins the rebuilt binary
and the isolated checkpoint path. The same normal log encounter and Aquasol
use now restore Fei to 73/73 without the cleanup crash (`resume21-aquasol.png`).
GOD OFF, HD2D ON, SPEED1X. Render callback15 remains unbound; full healing-effect
visual fidelity is still pending. Battle victory/return is checked separately.

Resume21 subsequently won the four-Hobgob battle normally and returned to
map22. The follow-on log event moved Fei under script control from
(-1122,-196,1008) to (-22,-177,1008), then unlocked control. Visually inspected
`resume21-log-event.png`; saved that earned state with F7. Normal Right+C
movement along the log reached zone3 at (-1654,-210,1008) and started retail
STR6, frames0..300. `resume21-log-cross.png` visibly shows the Fei/Elly anime
sequence, and the log confirms the first STR frame was delivered. This advances
the route beyond the root staircase and log encounter; movie completion and
the subsequent Elly/boss/forest-exit gates remain to be observed.

STR6 then exited at frame300 and returned to the forest. Elly's initial
conversation is visible with her portrait and text in `resume21-elly1.png`.
No movie skip, forced flag or field override was used.

## Elly/Wels frontier: missing timer callback lookup

Normal confirmations advanced Elly's conversation and its camera/animation
changes (`resume21-elly2.png` through `resume21-elly5.png`) into the scripted
two-Wels battle (`resume21-elly-battle.png`). Fei entered at HP43/73. Attempting
normal Aquasol use during that encounter exposed a different missing boundary:
`unresolved native call target=0x8001d164 guest-pc=0x800b4d50`, then callback
800C11CC aborted. This was SIGABRT, not either previously fixed SIGSEGV.
Resume21 is terminated. Preserved `wels-unresolved.core`,
`wels-unresolved-core.log`, `wels-unresolved.gdb`, and its runtime log.
No Wels victory or subsequent forest completion is claimed.

The native binary has no `func_8001D164` export, although the generated battle
bridge map already lists it. `src/slus_006.64/system/work_list.c` contains a
decompiled body, but the port uses `pc_port/src/work_list_port.c`, which lacks
this routine. The retail assembly at 8001D164..8001D19C searches the timer list
for a matching callback at +8, follows +18, and returns the first match or NULL.
Its branch-delay slot clears v0 at exhaustion. The decompiled body's return of
the previous/last node on a miss is therefore not authoritative; do not copy
that behavior into the port. Preserve the callback's address as an identity:
`is_callback_argument` currently lacks the index0 case for this function.
Existing timer-work-list retail and battle-graphics ABI suites can cover the
native lookup and its interpreter boundary respectively. No repair for this
new frontier has been made yet.

The latest earned recovery checkpoint is after winning the log battle, at
(-22,-177,1008), before entering zone3 and the Elly movie. Replay from it uses
normal Right+C along the log to zone3, then ordinary dialogue confirmations.
GOD OFF and HD2D ON throughout the successful log-battle/movie segment. All
input helpers and the debugger have stopped; Xvfb95 remains available.

## Timer callback lookup repair (2026-09-20)

Added the native-only `func_8001D164` owner to `work_list_port.c`, returning the
first callback match or NULL on exhaustion. The interpreter now preserves its
argument0 as a callback identity. No retail source or assembly was changed.
All 56 annotated assembly bytes compare equal to the disc; SHA-256
`13bdca2e97bc7696a87d543c4551f7e79f51c8f46961caed7f59f6abe2652262`.
The test executes that retail slice and compares native return values, list
heads, and node guards for 480 empty/miss/first/duplicate/address-identity cases
at O0/O2/UBSan. Existing 576 timer allocation/list and 30,721 owner/callback
lookup cases also pass. A deliberately wrong last-node-on-miss implementation
is rejected. The production interpreter bridge test covers five callback
identities and rejects treating argument0 as a data pointer. Both suites pass;
logs are `timer-callback-lookup-tests.log` and `timer-callback-abi-tests.log`.
The ABI runner now links the actual God-mode hook (default OFF), which its
production runtime include requires.

Native build passes (`timer-callback-build.log`, 75 stubs checked, 96 adopted
leaves); `nm -D` confirms an exported `func_8001D164`. Resume22 launch metadata
pins the rebuilt binary for the natural Wels retry from the earned checkpoint.
These machine checks alone do not establish Wels victory or forest completion.

Resume22 reproduced the scripted two-Wels encounter with Fei at43/73
(`resume22-wels.png`). Normal Aquasol selection no longer aborts; the next
battle-menu observation is58/73 after enemy actions (`resume22-wels-healed.png`).
Ordinary attacks won the encounter (`resume22-wels-fight.png`, victory screen,
level6/HP58/75). The retail battle returned to map22 and the next story event
ran: Fei holds Elly and says "Are you alright!? Hang in there!"
(`resume22-post-wels.png`). This is live evidence beyond the original
8001D164 crash, not merely a test result. God mode stayed OFF, HD2D ON, speed1X.
The unbound animation-render callback15 warning remains; no complete
healing-effect fidelity claim is made. Rankar/Citan/forest exit remain pending.

The post-Wels story naturally moved to map25's campfire conversation
(`resume22-camp.png`, `resume22-camp2.png`). Its camera transition subsequently
started retail STR7, frames0..1059, and delivered the first frame; the flashback
is visible in `resume22-camp4.png`. No story flag or field position override
was used.

STR7 completed at frame1059. Normal dialogue confirmations then returned field
control on map23, scenario27 (`resume22-post-flashback.png`). F7 saved the earned
state at(29,0,1270) to the isolated `recovery.xgqs`; the preceding log/Wels
checkpoint was retained as `pre-wels-earned.xgqs`. Ordinary walking changed the
position to(29,0,1191), then onward through the map. The next route frontier is
map23 navigation toward the remaining forest events; no Rankar/Citan/forest
exit acceptance is claimed yet.

At the end of this observation the live resume22 process is PID2376929, map23,
scenario27, position(-396,0,-1511), layer0/triangle239, canRun1, held0. No input
helper or debugger remains active. The saved checkpoint remains at the start
of this map, not this exploration position. Several camera angles occlude Fei
behind trees/raised terrain; shoulder-button rotation changes the view. Walk
and jump inputs have been tried around the central tree/log boundary, without
establishing a new movement bug or changing collision logic. Zone2 bounds are
X[-978,-606], Z[-1793,-1314]; zone0 is centered(-1853,-2439), zone1(2536,1412).
Do not treat those geometric zones alone as proof of their story destinations.

## Zone-2 destination decoded, and three live defects repaired (2026-09-20)

### Destination: trigger zone index 2 (not 0 or 1)

Read-only decoding of `map23-script.bin` (the live `g_FieldCurScriptFile`, bytecode
base +0x6C4) found the map's only scenario-27 check, in actor 1 routine 1 at code
0x1ED, and the zone test that follows it at code 0x1F6 (`C9 02 1E 02` =
`CheckTriggerZone2D` index byte `0x02`). Live cross-check (non-pausing
`/proc/<pid>/mem`): `scenario=27`, `g_FieldScriptMemory+0x404=1` (the arm flag set
by code 0x1F2), actor 1 `ip=0x21E` (the miss/yield target), and
`g_pFieldTriggerZones[2]` = x[-978,-606], z[-1793,-1314], centre(-792,-1553),
floor y~-144. Zone 1 (2536,1412 east) and zone 0 (-1853,-2439 far SW) are never
checked by the scenario-27 path. The earlier "zone 2 unreachable" result was an
artefact of `map23-route.py`'s 0.6 slope filter dropping the real ramp triangles
tri231/tri232; at 0.4 the mesh connects start -> zone 2 (616 triangles, goal
reached). The headless walker therefore aims at (-700,-1500). Full writeup:
`analysis/map23-route-decode.md`.

### Defect 1 (fixed): main-executable work-list callback abort, 0x80025A88

A normal map-23 battle aborted with
`[xeno-port][work-list] unresolved guest callback 0x80025a88` (core preserved for
PID 3811934 at ~01:11; the work-list fix path `abort()`s by design). Cause: the
interpreted battle overlay's `func_800B6438` (`src/battle/mainc88.c:50`) does
`WorkListSetTaskCallback(task, 0x80025A88)` — the retail address of the
main-executable effect renderer `func_80025A88` — and
`PcPort_BattleMipsDispatchCallback` only dispatched battle-overlay guest code, so
the packed callback slot could never be invoked.

Fix (`pc_port/src/battle_mips_runtime.c`): the dispatcher now, for a callback that
is not overlay guest code, calls a new `run_main_exe_callback`, which resolves the
address with the same two lookups `runtime_bridge_call` uses (`find_function`
bridge entry, then `dlsym(RTLD_DEFAULT, "func_%08X")`) and calls the host owner
with the already-native argument, refusing generated stubs. Overlay dispatch is
unchanged.

Regression test `run_battle_main_exe_callback_dispatch_test.sh`: 23 checks at
O0/O2/UBSan covering bridge-table dispatch (called exactly once, identical
argument, returns 1, wins over dlsym), the dlsym fallback, generated-stub refusal
on both paths, unknown-address returns 0, inactive-runtime returns 0, and a
guard-byte canary. 5/5 source-level mutants rejected (`old-main-exe-early-return`,
`host-wrong-argument`, `drop-bridge-stub-refusal`, `drop-dlsym-stub-refusal`,
`success-without-call`).

Live retest: the same battle now wins and returns to map 23; the walk continued
from (-201,-860) through (-437,-1025), (-437,-1252), (-499,-1277).

### Defect 2 (fixed): animation-render callback 15 and its renderer

`D_8004FD40[15]` was NULL, so the healing-effect render was skipped and every use
logged an unbound callback. `src/battle/anim_render_index15.inc` (new) now
implements, natively and with guest->host address translation, `func_800257F0`
(callback 15), its sibling `func_80025A88`, and the battle-overlay indexed
primitive batch renderer `func_800B1F6C` + `func_800B1F0C`. `game_overrides.c`
binds slot 15; `port_owned_overrides.txt` carries `func_800257F0` and
`func_80025A88`. Retail pins: `800257F0` 166, `80025A88` 95, `800B1F6C` 736,
`800B1F0C` 24 annotated instructions all match disc.

Regression test `run_anim_render_index15_retail_test.sh`: 19+11 callback cases
and 40 renderer cases (all 16 descriptor keys, 8 shapes, counts, tpage, depth
clamp/reject, shift mask, domains, work-buffer capacity, full GTE state) at
O0/O2/UBSan, with 17/17 mutants rejected. `battle_child_billboard_retail_test.c`
was strengthened to assert the now-bound slot 15. This is a behaviour-preserving
reimplementation, not a byte-match transcription.

### Defect 3 (fixed): 7 KB of retail .sdata was never loaded, 64 zero stubs

`pc_port/src/psx_memory.h` ended `.sdata` at 0x800576E4; `config/slus_006.64.yaml`
places `.sdata` at [0x8004EA90,0x800592BC), a 4-byte `.data` at
[0x800592BC,0x800592C0) and `.sbss` at 0x800592C0. The loader therefore dropped
7128 bytes of real initialized data, and 64 generated data symbols whose retail
bytes are non-zero were linked as all-zero arrays (60 of them read by native
code). The boundary is now 0x800592C0, and `pc_port/src/data_slus_sdata.c` (new)
defines 29 of those symbols with exact retail bytes (26 byte-exact arrays incl.
the `D_8004FBB8`/`D_8004FDA0` matrices, the controller stick LUTs, the font CLUT,
the reverb sizes and the sound tables; 3 pointer tables stored as host pointers
into `g_PsxRam` because native readers dereference them on LP64).

Test `run_data_slus_sdata_retail_test.sh`: 107 assertions at O0/O2 (per-symbol
retail memcmp, section boundaries, last non-zero image byte 0x800592BB, the
previously-dropped tail non-empty) with 3/3 mutants rejected (`zero`, `truncate`,
`sdataend`). `w34c2_static_data_prod_test.c` now canaries `.sbss` with 0xA5 so its
M4 mutant is still detected; `docs/evidence/w34c2-static-data-load/README.md`
carries the correction.

### Regressions the .sdata fix exposed, both real port defects (fixed)

The reverb-work-area and music-manager paths had never run because their tables
were zero. With real bytes they do, and both faulted:

1. `SoundInitiateReverbWorkAreaTransfer` -> `SoundExecuteReverbWorkAreaTransfer`
   (sound.c). PsyCross completes `SpuWrite` synchronously, so the first
   `SoundQueueSpuWriteCommand` already ran the chunk chain to its
   `bytesRemaining==0` tail, which frees `g_SoundUploadDestBuffer`; the guarded
   duplicate write then handed `PsyX_SPUAL_Write` a host NULL and SIGSEGV'd at
   boot. The port now skips the duplicate when the synchronous completion has
   already released the buffer (the buffer is zeroed and the duplicate writes the
   same zeros to the same SPU address, so nothing retail would observe is lost).
2. `func_8003A89C(NULL, 0x7F, 0)` from field teardown (`src/field/main/main.c:578`)
   once a map is torn down before its song manager was created (the manager global
   is only written when a WDS song lands, `misc8.c:3053`). The sibling entry
   points `func_80039C4C`/`func_80039C8C`/`func_800399D4` already took a NULL
   guard; `func_8003A89C` now does too. Live evidence: SIGSEGV in
   `func_8003A89C(manager=0x0, level=127, steps=0)` at `sound.c:2645` with a full
   gdb backtrace on the map-23 teardown after a map reload.

Both fixes are `XENO_PC_PORT`-guarded so the matching C is unchanged.

### Next live frontier: opcode 0xBC sub-command 0x22

After those repairs the walk continued further and the port then failed loudly, by
design:

```
{"event":"sprite_animation_bc_unimplemented","sub":34,"sprite":"0x79a4e8"}
src/slus_006.64/system/animation_scripts.c:972: func_8001FBE4: Assertion
`0 && "func_8001FBE4 opcode 0xBC sub-command is not implemented"' failed.
```

`sub` 34 = 0x22 is in the player/party-relative group that reads `D_800C3E1C`
(player sprite) and `D_800D363C` (party list), whose retail writer is not in the
port yet. This is being implemented next from
`asm/slus_006.64/matchings/system/animation_scripts/func_8001FBE4.s`; it is a real
gap, not a walk error.

### Build and test environment notes

- The host has no SDL2 development files, so `pc_port/build_port.sh` must run in a
  container. The working invocation on 2026-09-20 was
  `podman run --rm --userns=keep-id --security-opt label=disable -v <repo>:/workspace:rw -w /workspace -e TMPDIR=/var/tmp localhost/xenogears-dev-toolchain:krom-20260913 bash pc_port/build_port.sh`
  (rootless podman maps container uid 0 to the host user, so `--userns=keep-id` is
  required for the bind mount to be writable).
- This host's GCC cannot link UBSan (`/usr/lib64/libubsan.so.1.0.0` and the 64-bit
  `libubsan.a` are both missing). Every new runner probes for it and runs the
  UBSan regime under `clang -fsanitize=undefined` instead, printing which compiler
  ran; none silently skips.
- Two pre-existing runners were broken during this work and are now repaired and
  green (test-harness changes only; no production source changed for them):
  `run_battle_child_billboard_retail_test.sh` failed because its `fn()` extractor
  matched the `SpriteRenderAddress` forward declaration at `temp1.c:1931` instead
  of the definition at 2109 (the extractor now requires the definition's `{`
  before the next `;`), and it also needed the `PcPort_GodModeBeforeGuest` stub;
  it is EXIT=0 at O0/O2/UBSan with 7/7 mutants rejected.
  `run_battle_guest_call_test.sh` needed the same stub, and its "legacy callback
  nonguest unhandled" assertion had been made stale by the dispatcher change
  above; that assertion was kept (now with an empty bridge table so the callback
  is genuinely unresolvable) and coverage for the new registered-non-guest
  dispatch path was added - 204 checks at O0/O2/UBSan with 6/6 controls rejected,
  no assertion deleted.
- Integrated build at the end of this session: LINK OK, 74 function stubs,
  `# 96 adopted leaves reach no generated stub`, `xeno-port` SHA-256
  `695e784d95107de874169d905f2c66fc501494c0a47e1b5387c7cad6c61eba97`.
  No documented completion below claims audiovisual identity against a retail
  hardware capture.

## Opcode 0xBC sub-command 0x22, .rodata data restoration, and the last teardown guard

### `func_8001FBE4` opcode 0xBC sub 0x22 (the fail-loud frontier above) - fixed

Decoded from `jtbl_800185A8` (39 entries, index `op0 & 0x3F`): index 0x22 ->
`0x8002049C` (`lui/lw $s0,%lo(D_800C3E1C)`; `j .L80020550`), i.e. the shared
sub-0x14 body with the *player/leader* sprite as its source instead of the
target: `vec.vx/vy/vz = *(s16*)(player + 0x2/0x6/0xA)` and
`vec.vy -= *(u16*)(player + 0x38)` (the full height; the half variant is 0x23),
with the camera-relative flag `sprite+0x3F bit 0` preserved into the existing
shared tail. Neighbours decoded: 0x20/0x21/0x23 = the 0x12/0x13/0x15 bodies with
the player source, 0x01 = player position, 0x19-0x1F = the player anchor table,
0x03 = player/self midpoint, 0x02/0x04 = averages over the `D_800D363C` chain,
0x05 genuinely indeterminate.

`D_800C3E1C` has exactly two writers in the whole game, both battle:
`func_800BF85C` (mainc131) and `func_800B8048` (mainc100). The port already has
both at runtime - `func_800B8048` as a native adopted leaf and `func_800BF85C`
executed by the interpreter, both writing the same guest RAM slot - so no new
writer was needed; the helper deliberately does not NULL-check, matching retail's
dereference of RAM offset 2. `src/slus_006.64/system/animation_scripts.c` now
implements case 0x22 and leaves the other player/party subs fail-loud.

Test `run_sprite_dispatch_bc22_retail_test.sh`: four disc SHA-256 slice pins
(`jtbl_800185A8`, the sub-22 entry, the body, the whole 0xBC handler), 19
annotated instruction bytes matched, **336 cases at O0/O2/UBSan** (player XYZ x
height 0/1/2/3/0xFFFF/0x8000/0x7FFF x camera flag x op0 bit 6 x guest/host
pointer encoding) and 7/7 mutants rejected (`height_off`, `half`, `add_height`,
`x_off`, `y_off`, `camera_flip`, `host_ptr`). One bit-identical extra: the shared
0xBC word-destination tail now writes `(u32)(s32)v << 16` instead of
`(s32)v << 16`, removing UBSan signed-shift UB and matching the idiom used
elsewhere in the file.

Still fail-loud by design: 0xBC subs 0x01-0x04, 0x19-0x1F, 0x20, 0x21, 0x23 and
the indeterminate 0x05. Field-slot caveat: the port never clears the field BSS
region (`0x800AF5E4..0x800C426C`) on map load, so after a battle the field can
observe a stale `D_800C3E1C` where retail sees 0.

### 27 `.rodata` mismatches restored, and the generator's function/data bug

`pc_port/src/data_slus_rodata.c` (new) defines all 27 `.rodata` symbols the data
audit found retail-non-zero/host-zero, with exact retail bytes and splat
`.size` from the per-symbol `.rodata.s` listings: `D_80010000[4]=0xFFFFFFFF`,
`D_80010004[0x8000]`, `D_80018004[0x80]` (the real size is 0x80, not the
0x1520 in `port_buffers`), and 24 debug/format strings `D_800180FC..D_80018974`;
33498 bytes total. The audit's remaining `.sdata` entries are not defects
(`D_8004FD40` split-brain, `D_80050200`/`D_800591B0` write-only, `D_800591B1` is
byte 1 of `D_800591B0`, and `D_80056414` is a false positive whose own word is
zero). `port_main.c`'s ad-hoc `*(int*)D_80010000 = -1` is now subsumed and was
removed; `ArchiveInit(...,0)` was kept because passing retail's `-1` would switch
the archive to the baked-table path, an unverified behaviour change.

`firstfile`/`nextfile` are FUNCTIONS in the main-exe `.text`, but the overlay ELFs
carry them as ABS symbols so the generator classified them as data and emitted
`unsigned char firstfile[32]`, which the menu call sites then bound to. The
generator now honours splat's `type:func` annotation (`parse_symbol_types`), the
two names are annotated in `config/symbol_addrs.slus_006.64.txt`, and a control
run shows the classification diff is exactly those two symbols (74/518 ->
76/516). `D_800308D0` is a mid-function GTE code label inside `func_80030750`,
not data; its consumer's last access needs 0x8E bytes, so a port-only
`size:0x8E` annotation stops the 32-byte stub from being overflowed (content
stays zero; nothing natively reads it).

Tests: `run_data_slus_rodata_retail_test.sh` (125 assertions, 5/5 mutants
rejected) and `run_gen_port_stubs_classify_test.sh` (3/3 mutants). A pre-existing
generator unit test, `tools/tests/test_gen_port_stubs.py::
test_unknown_function_name_is_not_authority`, also fails on HEAD and is unrelated
to this change.

### Last teardown guard: `func_800399D4`

The same NULL song-manager state crashed a second teardown entry point -
`func_800399D4(manager=0x0)` at `sound.c`, reached via `func_8001B5E8` <-
`func_8001B66C` <- `func_80078D44` on the map-23 teardown after a map reload. It
now takes the same `XENO_PC_PORT` guard as `func_80039C4C`/`func_80039C8C`/
`func_8003A89C`. The underlying cause is open and NOT claimed fixed: the port's
song-start path stores whatever `func_80039850` returned and still sets
`D_8004F35C`, so a failed/failed-to-run manager creation leaves a NULL manager
while the "song loaded" flag is set. No music-manager allocation failure has been
observed or ruled out yet.

### Final integrated state

Container build: LINK OK, **76 function stubs, 489 data symbols, 96 adopted
leaves across 61 translation units**, `xeno-port` SHA-256
`072eece28bae8cc79163ae409d8c1aa852b613e7d81c989312e8fc43a488bc6b`. All eight
affected suites PASS on that binary: `run_sprite_dispatch_bc22_retail_test`,
`run_data_slus_rodata_retail_test`, `run_gen_port_stubs_classify_test`,
`run_data_slus_sdata_retail_test`, `run_battle_main_exe_callback_dispatch_test`,
`run_battle_child_billboard_retail_test`, `run_battle_guest_call_test`,
`run_anim_render_index15_retail_test`.

Live map-23 progress with the fixes: the checkpointed walk from (29,0,1270)
reached (-383,0,-1530), i.e. z is already inside trigger zone 2's range but x
(-383) has not yet crossed the zone's -606 edge; the destination is a short
walk further west. Zone 2 entry and the scripted forest-exit cutscene
(particles off, field resources streamed, camera yaw, Fei/Elly routines) are
**not yet observed**, so no completion is claimed for the route.

## Round notes: the ramp, battle controls, and a new effect-render defect (2026-09-20 later)

### Walkmesh route into zone 2 (the direct line does not exist)

`scratchpad/blackmoon-route-20260919/plan_zone2_path.py` Dijkstras over the
extracted map23 layer-0 walkmesh with a 0.4 walkable-slope filter (the 0.6 filter
in `map23-route.py` drops the ramp). From the ramp triangle tri232
(-426,-96,-1509) the only route into zone 2 is
tri232 -> tri219 (-496,-1643) -> tri221 (-623,-1641): go -z along the ramp first,
then -x across the basin floor (y~-144). Approaching zone 2 directly from the
east is blocked by the cliff; a re-plan from (-415,-1430) gives
(-423,-1468) -> (-425,-1336) -> (-521,-1258) -> (-591,-1235) -> (-714,-1452).
The fixed-mapping walker reached (-415,-1430), i.e. standing on the ramp, before
the party was wiped.

### Battle controls, learned by live experiment

- The command ring rotates with Up/Down; the entry that Circle (Z) opens is the
  one drawn as the bottom label.  `resume23i-wheel-item-selected.png` shows
  `Escape | Combo` above `Item` and Circle opens the item list;
  `resume23i-wheel-attack-selected.png` shows `Chi` above `Defense | Attack`.
  Getting a specific command therefore means rotating until it is the bottom
  label, not reading the highlighted one.
- In the item list Circle selects, a target cursor appears, and Circle confirms.
  Cross (C) cancels back out one level.  `resume23i-aquasol-heal-effect.png`
  shows Aquasol restoring Fei 58 -> 70/75 **with the blue healing ring drawn**,
  which is a live confirmation that the newly ported index-15 effect renderer
  (`func_800257F0` / `func_800B1F6C`) is emitting the retail effect primitives.
- Items resolve but the *description panel* for later items renders empty
  (Zetasol/Rosesol list with a blank top panel), and Circle then does not
  advance from that list.  That is a real UI gap worth a look, captured in the
  round's screenshots.

### New defect on the live path: corrupt full-screen effect primitives

`resume23i-corrupt-effect.png` shows a full-screen field of vertical
rainbow-striped triangles during a map-23 battle (the attack/effect between an
enemy action and the next turn).  It is not the healing ring and does not look
like any authored effect.  The prime suspect is the newly ported overlay
renderer `func_800B1F6C` producing garbage for a descriptor key family the
healing path does not exercise (its 40-case differential covers all 16 keys, but
only against the *retail interpreter*, and the live caller passes shared-sprite
state the test fixture does not model), or the shared global `D_80050100`/OT
pointer being fed a guest address.  Not yet diagnosed; recorded here so it is not
lost.  It is a rendering regression risk introduced by the effect-render port and
must be triaged before any effect-parity claim.

### Party survival is the route bottleneck

The map-23 random encounters wipe the checkpoint party (Fei LV6 75 HP, Elly LV4
40 HP) after roughly four to six fights, and a wipe returns to the title, so each
attempt costs the full walk plus the fights.  The checkpoint is intact.  The next
attempt should either get an F7 save on the ramp (an earned mid-forest
checkpoint, which the route already uses) before pushing into the basin, or find
the Escape command reliably (escape is explicitly allowed) to conserve HP.
`resume23i-low-hp-battle.png` records the state at which this attempt was lost.

## Battle bridge: unresolved-call cache and LoadImage pointer translation (2026-09-20 round 2)

Two defects found by driving map 23 after the opcode fix. Both are in
`pc_port/src/battle_mips_runtime.c`; neither changes a resolution outcome.

### 1. Unresolved guest calls were re-resolved through `dlsym` on every call

An attack stalled for minutes while the screen barely changed. Sampling the
live process showed the stack repeatedly inside the dynamic linker
(`do_lookup_x` / `_dl_lookup_symbol_x` for `func_800806CC`, `func_800BEB24`,
...) and inside `runtime_bridge_call`'s O(n) host-pointer scan
(`battle_mips_runtime.c:847`). Every call to a guest routine that is not in the
bridge table fell back to `dlsym(RTLD_DEFAULT, "func_%08X")` and a linear scan,
per call, forever.

Fix: a 512-slot, 4-way-probing cache (`BridgeCallCacheSlot` in the runtime
struct) keyed by retail target address, storing either the resolved
`ResolvedFunction` or the "no host owner, run the retail bytes" verdict. It is
cleared when the table is (re)built and stores its key (the first cut forgot
`slot->target = target`, which made it a no-op cache - worth remembering).
Behaviour is unchanged: the same entries are chosen, only their cost changes.
The existing `run_battle_guest_call_test` (204 checks), the
main-executable-callback test (23 checks) and the anim-render suite all still
pass.

### 2. `LoadImage` received an untranslated low physical-RAM pointer

With the battle running at speed the guest reached `LoadImage(RECT*, u_long*)`
(`0x80044894`) at `0x800b78d8` and passed a *low physical* RAM address `0x1000`.
`translate_argument` deliberately only rewrites KSEG0/KSEG1/scratchpad values
because it is shared by scalar arguments, so `0x1000` reached PsyCross unchanged
and `GR_CopyVRAM` memmoved from host address `0x1000` (SIGSEGV; core backtrace
`LoadImage -> GR_CopyVRAM(src=0x1000, w=19088, h=12)`).

Fix: a `LoadImage` bridge (`bridge_load_image`), the same shape as the existing
`ReadTIM` bridge. It resolves the RECT pointer as a guest pointer (OR-ing the
KSEG0 bit for low addresses, exactly like the CompMatrix bridge already does)
and maps a low source address to `g_PsxRam + address`, which is what the PS1
physical alias means; anything else goes through `translate_argument`.

Live after both fixes: a fresh checkpoint load survived a normal encounter and
the walk continued to (-201,-725) with zero `unresolved native call` lines,
where the same segment previously aborted or stalled.

**Regression test (added):** `pc_port/tests/battle_load_image_cache_test.c` +
`run_battle_load_image_cache_test.sh`. 16 checks at O0/O2/UBSan (gcc, gcc,
clang - the UBSan regime probes and prints its compiler): KSEG0 RECT + low
source, bare low RECT + low source, KSEG1 source, and two cache proofs - a hit
must keep using the entry resolved on the first call even after the bridge
table's host is swapped, and the slot must actually store its target key. The
runner mutates the whole production file and rejects 5/5 controls:
`loadimage-no-low-map`, `loadimage-no-rect-kseg0`, `loadimage-rect-not-guest`,
`cache-key-dropped` (the exact bug hit while writing the fix) and
`cache-ignored`.

Build: LINK OK, 76 stubs, 489 data symbols, 96 adopted leaves,
`xeno-port` SHA-256 `8e467514067d02ce32b67a7ee1d858001e46b670b5bba09cecd99ac398f8599e`.


## New route blocker: a garbage RECT reaches `LoadImage` in the battle overlay (2026-09-20 round 3)

After the `LoadImage` pointer fix the SIGSEGV stopped (the source pointer is now
correctly `g_PsxRam + 0x1000`), but the battle still never finishes: the guest
loops in the overlay's texture upload, and the stack repeatedly lands in

```
bridge_load_image -> LoadImage(rect=g_PsxRam+0xC4A90, p=g_PsxRam+0x1000)
                  -> GR_CopyVRAM(src=..., x=0, y=0, w=19088, h=12, dst_x=396, dst_y=5)
```

`w=0x4A90`, `h=0xC` come straight from the RECT the guest passes.  Reading it
live gives `{0x698c, 0x0005, 0x4a90, 0x000c}` - four halfwords that are not a
rectangle, while PsyCross's `GR_CopyVRAM` does `for i<h: memcpy(dst, src, w*2)`
with no bounds check, so each row copies 19088 pixels and scribbles far past the
VRAM row.  That both hangs the upload and is the most likely source of the
full-screen corrupt-effect frame recorded earlier.

Where the RECT comes from: the overlay's own `LoadImage` call site in
`func_800B7870` (`asm/battle/nonmatchings/mainc99/func_800B7870.s:49`) builds a
*stack* RECT (`sp+0x10` = 0x2C0 x 0x100, 0x140 x 0xE0 = 320x224) - that is not
the call being sampled.  The sampled argument 0x800C4A90 is `D_800C3EB0+0xBE0`,
i.e. inside the battle overlay's own structure area, and it has no symbol.  That
address is **past the loaded battle image** (`disc/battle.bin` is 0x53F80 bytes
from base 0x8006FAF0, ending at 0x800C3A70), so it is overlay BSS: retail zeroes
it on load and the overlay fills it at runtime, while the port loads the image
and never clears the region beyond it.  Stale bytes there would explain a
rectangle nobody wrote.

**Not yet diagnosed or fixed.**  Next: identify the overlay routine that fills
`D_800C3EB0+0xBE0` and check whether it ran; and decide whether the port must
clear the battle overlay's BSS range on load (the route notes already record
that the field's `0x800AF5E4..0x800C426C` region is never cleared either).  Do
not paper over it by clamping `GR_CopyVRAM`: the invalid RECT is guest state.


## Fixed: the bridge call cache handed out another target's entry on eviction (round 4)

The `LoadImage` loop above was not guest state at all - it was a bug in the
resolved-call cache added the previous round.  `bridge_cache_slot` probes four
ways of one hash base and, when all four belong to other targets, returned the
primary slot **without clearing its state**.  `runtime_bridge_call` then saw
`state == 1` and treated the victim's verdict as a hit, so the address being
looked up was dispatched to whatever host function happened to live in that
slot.  The bridge is invoked once per guest instruction with `cpu->pc`, so the
cache thrashes and the mis-dispatch was frequent: the trace
(`XENO_BATTLE_MIPS_TRACE=1`) showed

```
call LoadImage target=0x800b798c a0=800c4a90 a1=00001000 a2=000000e0 a3=00000140
```

repeated forever - `0x800b798c` is an overlay PC, not `LoadImage` (0x80044894),
and the "RECT" it passed is an unrelated overlay table.  That also explains why
the first battle after the cache landed survived (no collision on the paths it
took) and later ones did not.

Fix: mark the victim empty (`victim->state = 0`) before returning it, so the
caller re-resolves.  `battle_load_image_cache_test.c` gained a case that finds
four non-guest targets sharing a cache base, fills all four ways, and then looks
up an unresolved *guest* target with the same base - it must be interpreted, not
dispatched.  The runner now rejects 6/6 controls, the new one being
`cache-evict-stale` (return the live slot instead of clearing it).  25 checks at
O0/O2/UBSan.

Live after the rebuild: the same encounter that looped forever now renders and
runs a normal attack sequence (`resume23q` battle, Fei 75/75, combo counter
active) instead of spinning inside `GR_CopyVRAM`.

Build: LINK OK, `xeno-port` SHA-256
`fa33ec66bfadc721d29e660d076c43ec2aa3573d0b21eb918973419642761e1e`.


## Earned mid-forest checkpoint on the ramp (round 5)

The corridor walk with escape-first battles reached the ramp and, after the
routine F7 refusal while a script lock was still up (moving one step cleared
it), a normal F7 wrote an **earned mid-forest checkpoint**:

- `recovery.xgqs` sha256 `04122f1e3032a1cc00cc29d35e2a9a0cfacda23ba11ac0a2f2f22c2511fa6492`,
  map 23, (-393,0,-1247), scenario 27.
- The previous map-23 start checkpoint is preserved as
  `route23-start-earned.xgqs` (sha256
  `bfe7bd2985dadf0cae56ddf7ad0f4e6ae0a18005838430fb942bac0c1f51030c`).

This is the route's first checkpoint past the corridor, so a wipe no longer
costs the whole walk: F8 at the title reloads the ramp directly (verified -
`pos=(-393,0,-1247)` was restored). From there the walkmesh route to trigger
zone 2 is only ~550 units: (-414,-1212) -> (-521,-1258) -> (-591,-1235) ->
(-714,-1452).

Two attempts from it were lost to ordinary attrition (the mapped encounters
come faster than the escapes succeed: escape attempts failed 2-3 times per
fight, and the checkpoint party is Fei LV6 75 HP / Elly LV4 40 HP). The walker
did reach (-495,-1279), i.e. inside the zone-2 approach, before the wipe.
Zone 2 entry and the forest-exit cutscene are still **not observed**.

No new port defect was established this round: every battle entered ran,
rendered and returned (or was escaped) on build `fa33ec66`, so the round-4
cache-eviction fix holds up under repeated play.


## Field-menu healing, verified (round 6)

The ramp checkpoint saves a *worn* party - Fei 53/75, Elly 1/40 (the status
screen after loading shows exactly that) - which is why every push from it died
in the first two encounters.  The field menu can repair that, and the sequence
is now verified end to end:

```
V (open menu)            cursor lands on Status
Down, Down               -> Items
Z                        -> item list, first item selected, description shown
Z                        -> target panel (Fei / Elly with their HP)
Z                        -> uses the item on the highlighted character
Down x4, Z               -> Exit
```

Confirmed with screenshots: Aquasol took Fei from 53/75 to 75/75 on the status
screen and left the list.  Target selection is a panel listing both characters
with a red cursor, so a specific character can be chosen with Up/Down before the
final Z.

Two related live facts:
- Battle input only lands during the party's own turn; presses issued while an
  enemy acts are discarded.  That is why escape attempts often fail - the
  Left/Circle pair has to fall inside the command window.
- Elly at 1/40 is KO'd by the first hit and stays down, so the party effectively
  fights one character.  Healing her before the first encounter, or reviving her
  with Zetasol ("Removes KO status"), is the lever for the last ~550 units.

Zone 2 entry and the forest-exit cutscene are still not observed.


## Resume point advanced to the basin approach (round 7)

A normal F7 at the ramp/basin boundary wrote a further-forward earned
checkpoint:

- `recovery.xgqs` sha256 `22e61c2d4407578f98beb610f9f83c2c3464ec39713b66536893794a5039ec57`,
  map 23, **(-417,0,-1428)**, scenario 27.
- The earlier ramp checkpoint `04122f1e...` at (-393,0,-1247) is superseded; the
  map-23 start backup `route23-start-earned.xgqs` (`bfe7bd29...`) is unchanged.

This is the closest resume point yet to trigger zone 2 (x[-978,-606],
z[-1793,-1314]). From here the walkmesh route is the ramp descent
tri232 (-426,-1509) -> tri219 (-496,-1643) -> tri221 (-623,-1641), so it needs
mostly -z then -x; the walker reached (-315,-1521) before the encounter that
ended the attempt.

### Menu observations from this round

- Item descriptions do render, but not for every entry: `Bizfruit` showed
  "Restores EP (10) Non-battle" while `Omegasol`, `Rosesol` and `Hob-Jerky`
  showed a blank panel. `Bizfruit` is an EP item, so it is not the HP heal the
  party needs; the HP items are `Rosesol`/`Hob-Jerky`/`Aquasol` (Aquasol's
  "Restores HP (50)" was confirmed earlier). Whether the blank panel is a data
  gap or just a slow description load is not established.
- A queued F7 save shows `WAIT` in the toolbar and, while it is pending, the
  field menu stopped responding to Circle as a cancel (the item window stayed
  open across several presses). The save did complete later, from that same
  state. Recorded as an observation only - no defect is claimed, and a menu
  cancel regression test is not yet written.

The party still wipes on the way in (Fei alone after Elly is KO'd), so zone 2
and the forest-exit cutscene remain unobserved.


## Suspected defect: a map-23 encounter never grants the party a turn (round 8)

From the basin-approach checkpoint (-417,-1428) a normal encounter started at
map 23 (-392,-1514) and then **stalled**: for well over 90 seconds the screen
kept animating but neither character ever acted.

Observed in `runtime-resume23t.log` and the screenshots
`resume23t-stalled-battle-start.png` (Fei 24/84), `-ap0.png` and `-late.png`
(Fei 16/84):

- Both the AP and Time gauges stay pinned at **0**; they never fill.
- Fei's HP dropped 24 -> 16 early and then did not change again, so the enemies
  were not acting either - the battle is a deadlock, not a beating.
- The command ring stays in its default `Defense | Attack` state and does not
  respond: repeated Left/Circle pairs (six attempts, 0.7 s apart, 3 s between)
  never rotated it to `Escape | Combo`, so there is no turn in which to escape
  or fight.
- `retail battle returned` never appears; the interpreter is not looping in the
  bridge (no unresolved-call or stub messages), it simply never advances the
  turn scheduler far enough for a command window.

Earlier encounters in the same session did grant turns (several were won), so
this is encounter-specific rather than a global battle break.  **Not diagnosed
yet**: the next step is to instrument the battle face/ATB state
(`func_8008A3EC` pause path, the AP fill, and the turn-order list) and compare
against the retail update that advances them, then decide whether the port
mis-initialises a per-battle value for these enemies.

Until this is understood, the basin approach cannot be pushed through on foot:
the encounter that blocks the last ~200 units into trigger zone 2 is the one
that deadlocks.  Zone 2 and the forest-exit cutscene remain unobserved.


## Suspected defect: the field menu can wedge while holding the input owner (round 9)

While trying to heal at the (-417,-1428) checkpoint the **field menu stopped
responding while still displayed**, and nothing could move it back:

1. `V` opened the menu normally - `resume23v-menu-open.png`, Fei 24/84 / Elly
   1/40, cursor on Status.
2. `Down, Down, Z` opened the Items list (`resume23v-items-open.png`).
3. From then on the item window stayed drawn but stopped taking input.  Arrow
   presses began moving the *main menu* cursor behind it, the description panel
   stayed blank, and eventually no key changed anything:
   `resume23v-menu-wedged.png` and `-wedged-late.png` are byte-different frames
   taken ~25 s apart with the play clock still advancing (000:02:41 -> 000:03:27).
   Circle, Triangle and the D-pad were all tried.

The field itself is alive throughout - `POSDIAG` keeps printing with
`canRun=1` and the position is unchanged - but its `owner` byte is **0x80**
(the menu) instead of 0xFF (none):

```
POSDIAG map=23 pos=(-417,0,-1428) inZones=[] scenario=27 ...
        held=0x0010 newpress=0x0000 canRun=1 owner=0x80 b21d0=0
```

So the menu holds the field's input ownership while it no longer consumes input.
This is the same wedge seen in round 1 (where it was mis-attributed to stuck
keys) and it is the practical blocker for the heal-then-push plan: with the menu
wedged, the player cannot heal, save or walk.

**Not diagnosed.**  Next: find the menu state machine that sets `D_800ADB64` to
0x80 and check which transition can leave it set with the menu's own input poll
no longer running - the candidates are the Items list hand-off and the pending
F7 save observed earlier (a queued save shows `WAIT` in the toolbar and the menu
stopped cancelling at the same time).  A regression test needs the production
menu/field input-owner hand-off, not just the checkpoint gate.

Route state: zone 2 is still not entered.  The run that did get a normal
encounter this round won it (Fei 15/84, Elly KO'd) - so the deadlock recorded
last round did **not** reproduce, and the remaining obstacle is party condition
plus this menu wedge.


### Follow-up diagnosis of the menu wedge (same round)

The menu code path is now pinned, and it explains the symptom exactly:

- `func_801C55A0` (`src/menu/main/misc.c:241`, port body) is the field system
  menu's input/render loop.  It draws each frame through `func_801C7BF4()` and
  then does `input = g_Menu->input`.  It **only leaves the loop** when
  `input == 5` (cancel) or when the confirm path sets `s1 = 0`.
- `g_Menu->input` is produced by `func_801C7D78` (`src/menu/main/misc.c:1343`).
  Its mapping is `released & 0x20` (Circle) -> `input = 4` (confirm) and
  `released & 0x40` (**Cross**) -> `input = 5` (cancel), i.e. the menu expects
  Cross to be *released* while it owns input.
- The port's own diagnostics agree with the observations: the log shows
  `func_800799D4 request=128 -> MenuMain`, then
  `func_801C55A0 input loop shouldDrawMenu=1`, and **no further menu line** - the
  loop is running (the on-screen clock keeps advancing) but never sees
  `input == 5`.

So the wedge is not a hung renderer and not the field: `func_801C55A0` is
spinning with a `g_Menu->input` that never reaches cancel, because the Cross
*release* edge is not reaching `func_801C7D78` while the menu owns input
(`owner=0x80`).  The pad queue is reset between the field and menu phases (see
the harness note on `g_XenoMenuNavReaderTicks` in the same file), which is the
obvious suspect for a press/release pair being split across the hand-off.

Next step (not done): a focused test that drives a Cross press+release across
the field -> menu ownership hand-off and asserts `func_801C55A0` leaves its
loop, with a mutant that drops the release edge.  That is a much narrower target
than the whole menu.


### Wedge narrowed to the pad-queue release edge (same round, live measurements)

The stack during the wedge is in the **Items screen**, not the top-level menu:
`func_801C7BF4` (menu draw) -> `func_801DBE54` (`src/menu/main/misc.c:7560`,
the port's Nav-N2a Items lifecycle).  That loop runs `while (running)` and only
clears `running` on `MENU_INPUT_BACK` (`0x5`) with nothing selected:

```c
        case MENU_INPUT_BACK:
            if (selected == -1) { running = 0; } else { selected = -1; }
```

`MENU_INPUT_BACK` comes from `func_801C7D78`
(`src/menu/main/misc.c:1343`), which sets `input = 5` only on
`g_C1ButtonStateReleased & 0x40` (Cross release).

Live measurements taken from the wedged process:

| probe | value | meaning |
|---|---|---|
| `'controller_vblank_service.c'::enabled` | `1` | vblank delivery is not masked |
| `serviced_count` vs `PsyX_Sys_GetVBlankCount()` | `7488` vs `7488` | the tick service is keeping up |
| `g_ControllerIsStateStackFull` | `0` | the queue is not overflowing/resetting |
| `g_ControllerNumStates` | `0..1` | states are being pushed and popped |
| `g_XenoMenuNavReaderTicks` | climbing (1652 -> 1908) | the reader runs every frame |
| `g_C1ButtonState` while held | z `0x20`, c `0x40`, v `0x10`, x `0x80` | per-key mapping is correct |
| `g_Menu->input` | `8` (idle) and `0` seen, never `5` | cancel never arrives |

So the mapping is right, the press path works (direction inputs are read from
`pressed` and did move the cursor), the queue is live, and the tick service is
healthy - what never happens is the **Cross release edge** reaching the reader as
`g_Menu->input = 5`.

Why the existing coverage misses it: the port's headless menu harness
(`psyq_compat.c` `XENO_MENU_NAV_TEST`, around line 1358) injects the edge
directly - `g_C1ButtonStateReleased |= 0x40` - after `g_XenoMenuNavReaderTicks`
reaches a threshold.  It therefore bypasses the `ControllerPushState` ->
`ControllerPopState` -> `g_C1ButtonStateReleased` path entirely, which is exactly
where this wedge lives.

Next step: a scoped test that pushes a Cross press **and** release through the
real queue (two `ControllerPushState` snapshots with different
`g_C1ButtonStateReleased`), pops them through `func_801C7D78`, and asserts
`g_Menu->input == MENU_INPUT_BACK`; plus a mutant that drops the release field in
the push snapshot.  That is a much smaller target than the whole menu and is
directly on the live path that fails.


## CORRECTION: the "field-menu wedge" was not a port defect (round 11)

The wedge recorded in rounds 9-10 does **not** reproduce as a defect, and the
earlier conclusion is withdrawn.  Re-testing on a clean load:

1. Loaded the (-417,-1428) checkpoint - `g_C1ButtonState`,
   `g_C1ButtonStateReleased` and `g_C1PrevButtonState` all read `0`, so no stuck
   key was involved.
2. Opened the menu and the Items screen; the port logged
   `Nav N2a: Items windows 3/4 settled open`.
3. A hardware **watchpoint on `g_Menu->input`** caught the reader writing
   `Old value = 8 (idle)` -> `New value = 5 (MENU_INPUT_BACK)`: the Cross
   cancel edge does reach the Items loop.
4. Pressing Cross (c) with slower spacing (0.6 s hold, 2.5 s apart) then closed
   the menu: `POSDIAG ... owner=0xff` returned and the field was playable again.

So the menu, the pad queue and the cancel path all work.  What failed was my
*input timing*: the menu consumes one queued pad state per frame, and the rapid
repeated taps used earlier could land entirely between reads, which looked like
a wedged loop.  The round-9/10 entries above are kept as the record of what was
observed, but no port defect should be inferred from them, and no fix is owed.

### The same timing rule fixes item use

The Items screen needs **three** confirms, not one: the first sets `selected`,
the second opens the use prompt, the third picks the target.  With 0.5 s holds
and ~2 s spacing this worked end to end:

- **Omegasol** ("Restores HP and EP to FULL") used on Elly: 1/40 -> **40/40**,
  and the item left the list.
- **Hob-Jerky** ("Restores HP (50) Non-battle") used on Fei, count 4 -> 3.
- The description panel only renders once the list settles; an empty panel just
  means the cursor is on an empty slot, not a data gap.

### Healed checkpoint

A normal F7 then wrote `recovery.xgqs` sha256
`53a26c8c7333f28d6d9caf2dc47f50600a738484e4b55116f23247da7e3fe825` at map 23,
(-417,0,-1428) - the same resume point as `22e61c2d...`, but with the party
healed (Fei ~74/84, Elly 40/40).  Future pushes into zone 2 now start from a
healthy party instead of 53/75 and 1/40.


## Battle driving recipe, and where the route still stands (round 12)

Battles on this route are winnable with pure command input, and the sequence is
now known and scripted (`scratchpad/blackmoon-route-20260919/fight.py`):

```
Cross (c)                reset the ring to Defense | Attack
Circle (z)               select Attack
Triangle (v), Triangle   spend two combo points (the menu shows
                         "1 point" Triangle / "2 points" Square /
                         "3 points" Cross / "cancel-end" Circle)
Circle (z)               execute
```

Every press is held ~0.5 s and the steps are ~2 s apart, for the same reason the
menu needs it: the command ring only accepts input on the party's own turn and
consumes one queued pad state per frame.  With that, three encounters in a row
were won with no losses, and both characters levelled (Fei LV7, Elly LV4 -> LV5
with 48 max HP); the drops also topped the bag up (Hob-Jerky 3 -> 7,
Zetasol 2 -> 4).

### Navigation is the only remaining blocker

The party is pinned at the ramp foot.  Movement probes from (-396,-1495) show
`+z` (Up) and `+x,+z` (Right) open while `-x` (Left) is blocked, and every
attempt to cross to the ramp proper at x ~ -426 stops there.  The walkmesh route
to trigger zone 2 needs that crossing followed by the descent (the ramp drops
from y=0 to y=-48/-96 at tri231/tri232, then the basin floor is y=-144), and the
zone itself starts at x=-606.  Encounters fire every few seconds in this pocket,
so each attempt spends most of its time in battles.

### Checkpoint left untouched

The party was at Elly 1/48 when the round ran out, so the healed checkpoint
(`53a26c8c...`, Fei ~74/84, Elly 40/40 at (-417,0,-1428)) was deliberately **not**
overwritten.  The levelled state (Elly LV5) is live in the running session but
not persisted.

No new port defect was established this round: every battle entered ran,
rendered and returned normally, and the two menu interactions exercised behaved
correctly.  Zone 2 and the forest-exit cutscene remain unobserved.


## FIXED: selecting "File" in the field menu hung the game (round 13)

The wedge chased since round 9 is **this**, and it is a real port defect - not an
input-timing artefact as round 11 concluded.  The trigger is the field menu's
**File** entry:

`src/menu/main/misc.c` calls `PcPort_NotifyUnsupportedFileMenu()` when File is
chosen, and `pc_port/src/psycross_host_toolbar.inl` implemented it by calling
`SDL_ShowSimpleMessageBox` **on the game thread**.  That call is modal - it runs
the native dialog and does not return until the user dismisses it - so when the
dialog cannot be surfaced (no window manager to map the zenity window onto the
game's display) the game thread never returns.  Observed state:

- `POSDIAG` frozen at map 23 (-496,0,-1276) with `owner=0x80`, `held=0x0010`;
- two screenshots 25 s apart were **byte-identical**, i.e. the frame was not even
  being redrawn;
- `gdb` showed the stack parked in `SDL_SYS_DelayNS` ->
  `SDL_Zenity_ShowMessageBox`, and `ps` showed an orphan
  `zenity --question ... --title "File menu not implemented"`.

Round 9 was the same hang; the round-11 retest simply never selected File.

### Fix

`PcPort_NotifyUnsupportedFileMenu` now dispatches the notice through the weak
`PcPort_FileMenuNoticeDialog` seam on a **detached thread**
(`SDL_CreateThread` + `SDL_DetachThread`), so the field stays live whether or not
the dialog can be shown.  Allocation and thread-creation failures log and return
instead of crashing.

### Verified live after the rebuild

Same script as the failure - open the menu, move the cursor to **File**, press
Circle:

- a dialog still spawned (the notice is still surfaced where possible);
- the game stayed responsive: two screenshots 6 s apart differed, and pressing
  Cross returned `POSDIAG ... owner=0xff`, so the menu closed normally and the
  field became playable.

### Regression test (added in round 14)

The policy was extracted into `pc_port/src/file_menu_notice.c` (SDL-free) with the
platform hooks as installable function pointers
(`PcPort_FileMenuNoticeAlloc/Free/Log/StartThread/Dialog`); the host renderer
installs the SDL-backed ones.  That makes the "never run the modal dialog on the
calling thread" rule directly testable:

- `pc_port/tests/file_menu_notice_test.c` + `run_file_menu_notice_test.sh` -
  **23 checks at O0/O2/UBSan** (gcc, gcc, clang, with the UBSan compiler probed
  and printed).  They pin that the request allocates once, hands the notice to
  the thread hook, never calls the dialog inline, dispatches exactly once, runs
  the dialog and frees from the thread body, returns in under 50 ms even though
  the dialog is blocking, cleans up when the dispatch is refused, does not free
  on an allocation failure, and keeps the advertised title/text.
- The runner mutates the production source and rejects **5/5** controls:
  `dialog-inline` (the original defect), `dispatch-skipped`, `oom-frees`,
  `failure-leaks` and `title-drift`.
- Provenance is pinned too: the call site must stay on the `menu1Choice == 1`
  (File) branch of `src/menu/main/misc.c`, and the request entry must not call
  `PcPort_FileMenuNoticeDialog` directly.

Re-verified live after the refactor: selecting File leaves the menu open
(`owner=0x80`) with the game running, and Cross returns it to the field
(`owner=0xff`) - no hang.


### Route probing after the fix (round 14)

With the hang understood, the basin approach was re-probed and the wall is
**z-banded**, not absolute:

- from (-417,-1428): Up -> (-417,-1338) -> (-426,-1042), then **Left to
  (-553,-1003)** - westward movement is open up there;
- pushing south from (-553,-1003) drifts south-east: (-543,-1047) ->
  (-448,-1369) -> (-374,-1546), where Left is blocked again at (-395,-1512).

So the route needs to go west while north (z ~ -1000), reach x < -606, and only
then descend into the zone's z range [-1793,-1314].  An encounter interrupted the
last attempt at (-395,-1252).  Trigger zone 2 is still not entered.


## Basin approach mapped, descent not yet threaded (round 15)

Long route session from the healed checkpoint (Fei 74/84, Elly 40/40).  Five
encounters were fought and won with the Attack sequence, then the party wiped
and the game returned to the title; `recovery.xgqs` (`53a26c8c...`) is untouched.

What the walk showed about the terrain (all y=0, map 23):

| from | move | to | note |
|---|---|---|---|
| (-395,-1212) | Up, Up | (-426,-828) | north is open |
| (-426,-828) | Left x2 | **(-846,-808)**, **(-988,-852)** | far west, past the zone's x edge |
| (-979,-870) | Down x10 | unchanged | **stuck**: only Up moved afterwards |
| (-441,-868) | Left | (-546,-988) | west+south |
| (-516,-1143) | Down | **(-420,-1421)** | **z inside the zone band**, x 190 too far east |
| (-420,-1421) | Left x2 | (-499,-1276) | blocked at x=-499 |
| (-553,-1005) | Left | blocked | west is closed here |
| (-553,-1005) | Up, Left | (-553,-1005) | no progress |

Two facts that shape the next attempt:

1. **Westward movement is z-banded.** It works around z=-808 (reaching x=-988)
   but is closed around z=-1005..-1276 (blocked at x~-500).  So the route must
   go west *before* descending south, not after.
2. **The zone's z band is reachable at x=-420** (east of the zone), and the
   walkmesh planner's descent is the ramp at tri128 (-554,-96,-1215) ->
   tri193 (-594,-144,-1348) -> tri210 (-714,-144,-1452, inside the zone).  The
   party stood at (-526,-1103), i.e. right beside that ramp mouth, several times
   without stepping onto the descent - pressing Down there drifted south-east
   instead.

Recommended next attempt: drive the planner's waypoint chain
(-646,-841) -> (-512,-822) -> (-491,-943) -> (-500,-1079) -> (-554,-1215) ->
(-594,-1348) -> (-714,-1452) with a walker that **scores y first** (a y drop is
worth more than any flat progress), so it takes the ramp instead of drifting past
it, and that fights through the encounters rather than trying to escape them.


## The descent edge is adjacent but not enterable from the reachable side (round 16)

Ran the planner from the checkpoint itself and it returns an 11-step path whose
**first node is the ramp** - tri231 at (-423,-48,-1468), 40 units from the
start - then tri240 (-425,-1336) back at y=0, tri61 (-521,-48,-1258), tri190
(-591,-144,-1235) on the basin floor, and finally tri210 (-714,-144,-1452)
inside trigger zone 2.

A new walker (`route_walk.py`) follows that chain, scoring a y drop at 12x its
distance value so it prefers the descent, and fighting encounters instead of
escaping.  It walked the plateau fine (reaching (-490,-1273), i.e. ~30 units
from tri61) and then reported BLOCKED, and manual probing agrees:

| at | move | result |
|---|---|---|
| (-490,-1273) | Up | (-490,-1078) north |
| (-490,-1273) | Down | (-490,-1273) back |
| (-490,-1273) | Left / Right | blocked |
| (-490,-1273) | Up+Left, Down+Left, Up+Right | blocked, position pinned |
| (-396,-1512) | Left | blocked; Down drifts south-east to (-284,-1660) |

So the walkmesh route exists but its first traversable edge is not reachable from
the positions the player can stand on: around x=-490 the terrain only allows
north-south movement, and the westward band that *is* open (z ~ -808, reaching
x=-988) does not connect south into the basin.  Five-plus encounters were fought
during the attempt and won; the party is healthy at the checkpoint.

**Concrete next step:** stop guessing coordinates.  Extend the existing
`XENO_FIELD_POS_DIAG` telemetry (`pc_port/src/field_pos_diag.c`) to print the
**current walkmesh triangle index**, so the driver can steer by triangle identity
("be on tri231") - the planner already speaks that language - and so a blocked
edge is reported as "triA has no traversable neighbour towards triB" instead of a
coordinate guess.  That is a test-only diagnostic, guarded by the same env var as
the rest of POSDIAG.


## Walkmesh triangle telemetry, verified against the extracted mesh (round 17)

`XENO_FIELD_POS_DIAG` now also prints the player's current walkmesh triangle, read
exactly as the engine's own collision routine does
(`src/field/main/misc4.c:1789-1794`: the layer is `*(s16*)(actorData + 0x10)`, the
triangle index for that layer is `*(s16*)(actorData + layer*2 + 0x08)`, and the
per-layer bases are `D_800AFB24[]`/`D_800AFB34[]`):

```
POSDIAG map=23 pos=(-417,0,-1428) ... tri=240 layer=0 triV=161,160,168
```

**The runtime index matches the extracted map23 walkmesh exactly**: extracted
tri240 is `verts=(161, 160, 168)`.  So the planner's triangle language and the
live game now agree, which is what the coordinate attempts could never confirm -
standing 30 units from the descent triangle looked identical to standing on it.

### The descent chain, in triangle identity

`tri_path.py` (new) Dijkstras the extracted mesh from a *triangle index* and emits
each hop with the **midpoint of the shared edge** as the steering target, which is
what the driver needs (the neighbour's centroid is often outside the current
triangle).  From the spawn triangle:

```
tri240(y=0) -> tri241(y=0) -> tri61(y=-48) -> tri128(y=-96) -> tri190(y=-144)
            -> tri192(y=-144) -> tri193(y=-144) -> tri210(y=-144, zone 2)
  tri240 -> tri241 via [-433.5, 0.0, -1279.5]
  tri241 -> tri61  via [-496.5, 0.0, -1251.5]
  tri61  -> tri128 via [-530.0, -72.0, -1246.0]
  tri128 -> tri190 via [-585.5, -144.0, -1211.0]
  tri190 -> tri192 via [-585.0, -144.0, -1277.0]
  tri192 -> tri193 via [-594.5, -144.0, -1310.0]
  tri193 -> tri210 via [-606.0, -144.0, -1387.5]
```

Eight triangles, three of them the actual descent (y steps 0 -> -48 -> -96 ->
-144), and the final hop crosses x=-606 at z=-1387.5, i.e. **into the zone's x
range inside its z band**.  This is a much shorter route than the round-15
plateau loop: from the checkpoint the descent is almost directly south-west.

### Walker status

`tri_walk.py` (new) confirms hops by triangle identity and did follow
tri240 -> tri241 live, but it drifts afterwards: it probes all four directions and
*undoes* each probe, and the undo does not return to the same position, so the
next leg starts from a different triangle (observed drifting
tri249 -> tri253 -> tri242 -> tri247).  It also needs the menu guard now added
(the battle driver's Cross presses can leave the field menu open, and while it
owns input every probe looks blocked).

**Next step:** make it closed-loop - step once in the chosen direction, then
**re-plan from the resulting triangle** instead of undoing, so drift cannot
accumulate.  `tri_path.py` already takes an arbitrary start triangle, so a
re-plan per step is cheap.


## Closed-loop walking works structurally; the camera-relative input mapping is the real blocker (round 18)

`closed_loop.py` (new) removes the round-17 drift by construction: it never
undoes a probe.  Each step reads the player's triangle, **re-plans from that
triangle** (Dijkstra over the extracted mesh, `tri_path.py`'s hop logic), aims at
the shared-edge midpoint, and steps once.  Live it walked the plateau cleanly -
`tri249 -> tri247`, `tri248 -> tri247`, and up to **tri241, 28 units from the
descent triangle tri61** - confirming hops by triangle identity as it went.

What defeats it is not the path but the **input mapping**:

- the field's direction keys are **camera-relative**, and the camera rotates as
  the player moves, so a direction's world delta measured at one spot does not
  hold at the next;
- the first version calibrated once and every 8 steps and went stale - it drifted
  east through `tri40 -> tri237 -> tri234`;
- adding diagonals (`Up+Left`, ...) and accepting any triangle change helped but
  still relied on a stale table;
- making the table **adaptive** (each committed step records the delta it
  produced) removed the separate calibration pass, but with an empty/short table
  the walker has nothing to rank with, so it deadlocked at `tri234`
  ("order=[]") and the party, after five more won encounters on the way, was
  finally wiped.  `recovery.xgqs` (`53a26c8c...`) is untouched.

The important part is that this is a *driver* problem, not a port problem: the
triangle telemetry from round 17 makes the game state fully observable, and the
walkmesh route (tri240 -> tri241 -> tri61 -> tri128 -> tri190 -> tri192 ->
tri193 -> tri210) is known and short.

**Concrete next step:** print the **field camera yaw** in the same POSDIAG line.
With the yaw, the camera-relative key mapping is a pure rotation, so the driver
can convert "I need to move -x,+z" into the correct key analytically instead of
learning it - which is exactly the trick that made the triangle index work.
`src/field/main/misc11.c`/`misc8.c` carry the field camera state; the render
context already exposes the rotation used for the view transform.


## Camera telemetry, and the analytic walker reaching the descent edge (round 19)

POSDIAG now prints the field camera as well (`eye=`/`at=`, 16.16 world units):

```
POSDIAG map=23 pos=(-417,0,-1428) ... tri=240 layer=0 triV=161,160,168
        eye=(-417,-814,-2782) at=(-417,-32,-1428)
```

That is the missing piece for input: the direction keys are camera-relative, so
`forward = normalize(at - eye)` and `right = (forward.z, -forward.x)` turn "I need
to move -x,+z" into a key combination analytically, with one probe to fix the
global handedness sign.

`cam_walk.py` does that - recomputing the basis every step, re-planning from the
player's actual triangle every step, and scaling the key hold to the distance
left.  Live it walked the plateau and arrived **7 units from the tri241/tri61
descent edge** (`fwd=0 sid=-8`), the closest any attempt has come.  Two driver
defects showed up and are fixed:

- the step size was a fixed 0.6 s (~90 units), so a 7-unit approach overshot the
  neighbour triangle; it is now `clamp(want/140, 0.12, 0.75)`;
- the battle routine pressed Triangle twice per round, and when a battle ended
  mid-round those presses landed in the **field**, where Triangle opens the
  status menu - which then owned input (`owner=0x80`) and made every subsequent
  move look blocked.  Every press is now gated on the battle still being active.

Both are harness bugs, not port bugs.  With them fixed the descent chain
(tri240 -> tri241 -> tri61 -> tri128 -> tri190 -> tri192 -> tri193 -> tri210) is
within reach of the analytic walker.


## The analytic walker works; four harness fixes it needed (round 20)

`cam_walk.py` now walks the extracted walkmesh correctly.  Getting there needed
four harness fixes, all of them in the driver rather than the port:

1. **Fine steps.** A fixed 0.6 s press moves ~90 units and hopped clean over a
   7-unit approach to the descent edge; the hold is now
   `clamp(want/140, 0.12, 0.75)`.
2. **Battle key leakage.** The fight routine pressed Triangle twice per round,
   and when a battle ended mid-round those presses landed in the FIELD, where
   Triangle opens the status menu - which then owned input (`owner=0x80`) and
   made every move look blocked.  Every press is now gated on the battle still
   being active.
3. **Stale telemetry.** POSDIAG prints once per 60 frames, so measuring a press
   with the last sample read gave `(0,0)` for every direction.  The walker now
   waits for a NEW POSDIAG line before measuring.
4. **Combination choice.** Picking the best vertical and best horizontal
   independently chose `Down+Right` for a mostly-southward target (Left and Right
   are near-perpendicular there, so noise decided) and pushed the player east
   every step.  All eight key combinations are now ranked by the cosine between
   the move they would produce and the direction wanted.

Measured live, the mapping at the checkpoint was `Up=(0,70)`, `Down=(0,-70)`,
`Right=(65,-65)`, `Left=(-65,65)` - i.e. the diagonal keys move at 45 degrees, so
the eight-way choice really is the whole input space.

With all four fixed the walker followed the chain hop by hop (`tri245 -> tri246`
confirmed).  The attempt then died the way the others did - the party, after the
encounters the plateau throws at it, was wiped and the run returned to the title
- and a **map guard** was added so the walker stops instead of planning against
the title map's walkmesh (`tri18`), which it did before the fix.

The route itself is unchanged and short: from the checkpoint triangle tri240,
`tri240 -> tri241 -> tri61 -> tri128 -> tri190 -> tri192 -> tri193 -> tri210`
into trigger zone 2.  Next attempt should load the checkpoint and run
`cam_walk.py` immediately, before any manual wandering moves the player off
tri240.


### Aim-past-the-edge, and where the attempt ended (round 20)

One more driver change came out of the clean run: the eight key combinations move
at 45 degrees, so a steering target sitting exactly **on** a shared edge gets
oscillated around rather than crossed (observed: tri240 -> tri241 -> tri243 back
to tri240).  `next_hop` now aims at a point up to 60 units **beyond** the edge,
towards the neighbour's centroid, so an overshoot lands inside the neighbour.

The clean attempt from the checkpoint then ran into the usual tax: an encounter
started within the first couple of legs, and the fight routine - attacking with
the verified combo sequence - was still working through it when the round's
budget ran out.  At that point the party was healthy (Fei 60/84, Elly 28/40) and
the field was on tri240 at (-415,0,-1306).  `recovery.xgqs` (`53a26c8c...`) is
untouched.

So: the tools are now correct and verified (triangle identity, camera telemetry,
per-step key measurement, cosine-combination choice, aim-past-edge, battle
gating, map guard), the route is 8 triangles long, and what remains is **execution
time on the route**: the walker needs to survive the plateau encounters long
enough to make the ~7 hops.


## The descent edge is blocked in-game, and it is the ONLY planner route (round 21)

Walked the descent edge by hand with the triangle telemetry guiding it, and got
right up against it:

- tri241 and tri61 share the edge between vertices 159 (-491,0,-1222) and
  160 (-502,0,-1281) - a north-south line at x ~ -496 spanning 59 units of z;
- stepping west with short taps reached **(-493,0,-1278)**, i.e. ~8 units from
  that edge at that z, and there the player **stops**: further Left presses do
  nothing, while Down steps back to tri240 and Up/Left shuffle along tri241.

So the walkmesh edge to the descent triangle cannot be traversed.  And the
descent triangle is not optional: re-running the planner from tri240 with
stricter slope filters gives

```
threshold 0.40 -> NO PATH? no: tri240 -> tri241 -> tri61 -> tri128 -> tri190 ... (573 cost)
threshold 0.70 -> NO PATH from tri240
threshold 0.85 -> NO PATH from tri240
```

i.e. **every** route into zone 2 crosses tri61 (and tri128), a slope whose
vertices span y=0 to y=-144 over ~80 horizontal units (~60 degrees).  The 0.4
threshold the planner has been using is more permissive than the game's own
collision is.

This reframes the blocker.  Either:

1. the intended route to zone 2 does not start from this plateau at all (the
   forest exit may be reached from a different part of map 23, or the zone is
   entered from a scripted move), or
2. the port's translation of the field collision (`func_8007BEF4`,
   `src/field/main/misc4.c:1789`, which contains the walkmesh edge search and
   slope handling) refuses a slope the retail game allows - which would be a real
   port defect and exactly the kind this objective is for.

The next step is therefore a **retail-backed check of the slope rule**: find the
criterion `func_8007BEF4` uses to reject an edge (the threshold is in the
triangle flags/normal test it computes), compare it against the triangle's own
normal, and decide (1) or (2) from evidence rather than from the planner's
assumption.  The player is sitting at (-493,0,-1278) on tri241, one edge from the
descent, so the case is reproducible in seconds.


## RESOLVED: why the basin is not walkable-to, with the collision rule to prove it (round 21)

The blocked descent edge now has a complete, evidence-backed explanation, and it
is **not** a port defect - the game's own rule refuses both descent routes for the
actor state the party is in.

**1. The planner's "walkable" filter was wrong.**  `plan_zone2_path.py`/
`tri_path.py` used an arbitrary slope threshold of 0.4.  Re-planning with the
game's actual rule instead (drop triangles whose material flags carry the ledge
bit) gives a *short* route the slope filter had been excluding:

```
tri240(y=0) -> tri191(y=-48) -> tri758(y=-96) -> tri757(y=-144)
            -> tri193(y=-144) -> tri210(y=-144, ZONE 2)
```

**2. Both descents are refused by `func_8007BEF4`** (`src/field/main/misc4.c`,
the field's walkmesh edge search).  The extracted material flags for the two
descent routes are:

| triangle | y span | slope | material flags |
|---|---|---|---|
| tri240 (plateau) | 0,0,0 | 0 deg | 0x18 |
| tri61 / tri128 | -144..0 | ~60 deg | **0x400022** |
| tri191 / tri758 | -144..0 | 70-73 deg | **0x800000** |
| tri757 / tri193 / tri210 (basin, zone) | -144 | 0 deg | 0x4 / 0x4 / 0x0 |

and the code refuses exactly those flags for this actor:

```c
} else if ((flags & 0x00800000) != 0 && *(s16*)(actorData + 0x10) == 0) {
    triIndex = -1;                      /* refused: actor layer 0 */
} else if ((flags & 0x00400000) != 0) {
    if (!forceEdgeSearch) {
        func_8007B07C(...);             /* surface height at the target point */
        if (outPoint[1] < *(s16*)((u8*)base + 0x06)) {
            triIndex = -1;              /* refused: stepping DOWN */
        }
    }
}
```

`POSDIAG` reports `layer=0` for the player throughout, and the live attempt
confirms it: short westward taps reach (-493,0,-1278), ~8 units from the tri241|
tri61 edge, and stop there.

**3. So the basin is not reached by walking down from this plateau.**  Both ways
in (the 0x400000 slope down the south side, the 0x800000 slope at tri191) are
refused for a layer-0 actor, and no other non-ledge route exists from tri240.
The zone-2 trigger must therefore be reached either by a **scripted move** (the
story taking the party down) or from **another field map** whose walkmesh
connects to the basin floor - not by free walking from the camp plateau.

This is where the "walk the ramp" line of attack ends.  The next step is to look
for the scripted/other-map entrance to the basin (the forest is multi-map; the
MAPJUMP/exit list for map 23 and the neighbouring maps' load points are the
place to look), and to drive that instead - which is normal retail play, since it
is what the game does.


## CORRECTION: the descent DOES work; round 21's conclusion was too strong (round 22)

Round 21 concluded the basin was "not walkable-to" because the descent triangles
carry ledge-flagged materials.  That was wrong on two counts.

**1. The collision checks are conditional, not absolute.**  In `func_8007BEF4`
the flags are masked before use:

```c
    if (((*(u32*)(actorData + 0x04) >> (state + 3)) & 1) == 0) {
        collisionMask = (D_800B21CC == 0) ? -1 : 0;
    }
    ...
    flags = ((u32*)(uintptr_t)D_800AFB20[0])[*((u8*)tri + 0x0C)] & collisionMask;
```

so with the actor's collision bit set (or `D_800B21CC != 0`) the masked flags are
0 and *no* rejection runs.  And for tri61 the `0x00400000` branch is skipped
anyway, because `forceEdgeSearch` is itself set by that flag
(`forceEdgeSearch = ((triFlags & 0x00400000) != 0) || mode == 0x80;`).

**2. There is direct evidence the party has descended here before.**  Older
POSDIAG samples in this evidence set show the player well below the plateau on
map 23 at the ramp:

```
pos=(-399,-57,-1520)
pos=(-391,-92,-1514)
pos=(-423,-119,-1525)
```

i.e. y = -57, -92, -119 around x -400..-423, z -1514..-1525 - the tri191/tri231
ramp.  So walking down works.

**3. Re-planning with the game's ledge rule instead of an arbitrary slope
threshold** (0.4) gives a short route the slope filter had wrongly excluded:

```
tri240(y=0) -> tri191(y=-48) -> tri758(y=-96) -> tri757(y=-144)
            -> tri193(y=-144) -> tri210(y=-144, ZONE 2)
```

Zone-2 triangles are reachable this way (8 of them), and so are zone-0's.

So the basin is reachable on foot, and the blocker is entirely in the driver:
`cam_walk.py` was re-planned onto this chain, but the camera-relative key mapping
shifts as the player moves, and the ramp entry window is narrow - the attempts
this round kept being steered past it (south-east drift) or interrupted by
encounters.  The player was taken to (-396,0,-1509), essentially the ramp mouth,
and further probing drifted east before the round's budget ran out.

Next step: enter the ramp while standing on the right line.  The measured mapping
at (-374,-1469) was `Left = pure -x`, so from (-396,-1509) a Left press puts the
player at x ~ -421 on the ramp line; the following step has to be **south along
that line**, which the current key set only reaches as a diagonal.


## Ramp geometry pinned, and why the driver keeps missing it (round 23)

Point-in-triangle tests on the extracted mesh locate the historical descents
exactly:

| point | triangle | surface height |
|---|---|---|
| (-417,-1428) | tri240 | 0 |
| (-396,-1509) | tri239 | 0 |
| **(-399,-1520)** | **tri233** | **-15.7** |
| **(-423,-1525)** | **tri232** | **-104.5** |
| (-426,-1509) | tri232 | -96 |

So the south ramp is **tri232/tri233**, entered from tri239 at z ~ -1515, and its
mouth is only about 30 units wide in z at x ~ -400..-425.  The other ramp is
tri191/tri758 (centroid (-458,-48,-1391)), which is what the ledge-rule planner
routes through from tri240.

Two driver facts make that 30-unit window hard to hit:

- **The key mapping rotates as the player moves.**  At (-399,-1491) `Up` was
  (0,+60) - pure +z - and a moment later, two steps away, `Down` measured
  (+34,-44).  So a mapping learned (or even measured) at one spot is wrong at the
  next, which is exactly why the table-based walkers drifted and why the
  analytic walker re-measures every step.
- **The eight moves are 45-degree diagonals**, so a "south, slightly west" step
  to enter the ramp has to be built from two presses, and a single coarse press
  (+33,-50 at 0.6 s) jumps clean over the 30-unit window into tri237.

`cam_walk.py` now runs with the game's ledge rule and re-plans from the player's
triangle every step; live it walked tri236 -> tri237 -> tri239 -> tri240
correctly, then met an encounter and was fighting through it when the round's
budget ran out.  The party is being worn down by the encounter rate on the
plateau (Fei 50/84, Elly KO'd in that fight), which is what ends most attempts.


## The ramp top edge is directly south of the checkpoint (round 24)

Point-in-triangle tests along the line due south of the spawn settle the approach:

| point | triangle | height |
|---|---|---|
| (-417,-1428) spawn | tri240 | 0 |
| (-417,-1498) | **tri232** | **-51.7** |
| (-417,-1510) | tri232 | -65.9 |
| (-430,-1510) | tri232 | -111.1 |
| (-440,-1520) | tri217 | -144.0 (basin floor) |
| (-399,-1520) | tri233 | -15.7 |

So the ramp (tri232/233) starts at z ~ -1490, i.e. **70 units due south of the
checkpoint**, and it is wide open there - the earlier "30-unit mouth at x -400..
-425" is only the part reachable *after* drifting east.

The catch is the input, not the terrain: measured live, `Down` at the spawn was
`(+18,-62)` - not pure south - so one press from (-417,-1428) lands at
(-399,-1490) on **tri239 at y=0**, one triangle east of the ramp, and the next
press carries further east to tri237/tri236.  A press of `Left` before the
descent puts the player back on the ramp line.

`goto.py` (new) is a targeted version of the analytic walker: instead of planning
a chain it drives to one fixed point, measuring the four key deltas every step,
ranking all eight key combinations by direction cosine, and re-reading the
position after probing.  Verified live: it took tri236 -> tri238 in one step at
(-394,-1509).  Attempts this round were again dominated by encounters - the
driver spent its budget inside the battle routine - so the descent was not
completed.

Next step is small and specific: from the spawn press `Down` (one step to tri239
at ~(-399,-1490)), then `Left` to return to x ~ -417, then `Down` again, which
lands on tri232 and drops y to ~ -50.  `goto.py <log> -417 -1495 15 20` does
exactly that as one target if the encounters allow it through.


## The descent entry edge, in exact coordinates (round 25)

Reading the ramp's adjacency out of the extracted mesh names the whole descent:

```
tri239 (plateau, y=0)      neighbours 238, 231, 40
tri231 (ramp,  y -144..0)  neighbours 191, 239, 232
tri232 (ramp,  y -144..0)  neighbours 217, 231, 233
tri233 (ramp,  y -144..0)  neighbours 232, 237, 127
tri217 (basin, y -144)     <- floor
```

so the way down is **tri239 -> tri231 -> tri232 -> tri217**, and the edge to cross
is the one tri239 and tri231 share, between vertices 225 and 161:

| vertex | world position |
|---|---|
| 225 | **(-397, 0, -1513)** |
| 161 | **(-409, 0, -1450)** |
| edge midpoint | **(-403, 0, -1482)** |

That is the exact target: stand at about (-403,-1490) and step north-west across
that 63-unit segment.  Everything the driver was doing - pushing south toward
(-399,-1520) - was aiming at tri233, which is **not** a neighbour of the plateau
tri239 at all (its neighbours are 232, 237, 127), which is why the southward taps
always ended on tri237 at y=0.

Live this round the player got to (-396,-1507) - within a few units of the edge's
southern end - and a pair of `Up+Left` presses moved it to (-399,-1493) and then
jumped to (-412,-1441) without entering tri231, so the crossing still needs a
shorter step at the right moment.  Encounters took most of the round's budget
again (one fight, one menu close, then repositioning).

Next: `goto.py <log> -403 -1487 10 20` from a fresh load - it aims at the edge
midpoint with per-step measurement; then a single short `Up+Left` tap should cross
onto tri231 and drop y to about -48.


## PROVEN: the party cannot walk down into the basin (round 28)

Rounds 21-27 assumed the descent was a driver-precision problem.  It is not: the
retail collision code refuses both descent edges for the actor state the party is
in, and the caller settles the one remaining doubt.

The player's own movement calls the walkmesh search with **`mode = -1`**
(`src/field/main/misc4.c:1679`, `func_8007BEF4(move, ..., -1, &flags)`), so

```c
    forceEdgeSearch = ((triFlags & 0x00400000) != 0) || mode == 0x80;
```

evaluates `triFlags` from the **current** triangle.  The party stands on the
plateau - tri237/tri239/tri240, which are unflagged (`0x0`, `0x0`, `0x18`) - so
`forceEdgeSearch` is **false** and the step-down test runs:

```c
    } else if ((flags & 0x00400000) != 0) {
        if (!forceEdgeSearch) {
            func_8007B07C(...);                 /* surface height at the target */
            if (outPoint[1] < *(s16*)((u8*)base + 0x06)) {
                triIndex = -1;                  /* refused: stepping DOWN */
            }
        }
    }
```

Every ramp triangle carries `0x400022`, and stepping onto one from the plateau
means moving to a surface *below* the current one - refused.  The other descent,
tri191/tri758, carries `0x800000`, refused outright for a layer-0 actor:

```c
    } else if ((flags & 0x00800000) != 0 && *(s16*)(actorData + 0x10) == 0) {
        triIndex = -1;
    }
```

and `POSDIAG` reports `layer=0` throughout.

Live this is exactly what happens, and the two exact edges were driven to this
round:

| descent | shared edge | midpoint | result |
|---|---|---|---|
| tri237 -> tri233 | verts 162-225 | (-378,0,-1545) | player walks the edge line, never crosses |
| tri240 -> tri191 | verts 161-160 | (-456,0,-1366) | player reached **(-454,-1363)**, 3 units from the edge, and every south/south-west tap slid east along the plateau |

`goto.py` put the player within 3-4 units of both edges - the targeting is now
exact - and the crossing still does not happen, which is the rule and not the
input.

**So the descent is a scripted move, not a walk.**  The historical y=-57/-92/-119
samples on the ramp came from a state the walker cannot reach on foot (the party
is placed on the ramp), which also explains why the basin is only reachable
through itself: the ledge-rule graph connects tri239 to the basin, but every
connection is one of these two refused edges.

Next step: find the story event that performs the descent.  The search is over
map 23's field scripts and the neighbouring maps' load points for the opcode that
places the party on the ramp (or moves them to the basin map), and then drive that
trigger in normal play - which is what a player does.


## CORRECTION + the map's real walkable descent is in the NORTH-EAST (round 29)

Round 28 concluded the party could not walk down anywhere.  That is wrong: it is
true only of the two ramps next to the checkpoint.  Filtering the mesh by the
game's own two refusal rules (`0x400000` step-down, `0x800000` layer-0) and
running Dijkstra from tri239 shows **296 reachable triangles spanning heights 0
down to -300** - so a walkable descent exists, just elsewhere:

```
tri239 -> tri40 -> tri240 -> tri241 -> tri242 -> tri247 -> tri248 -> tri249
       -> ... -> tri314 -> tri504(-15) -> tri507(-35) -> tri508(-58)
       -> tri509(-74) -> tri510(-94) -> tri511(-110) -> tri512(-127)
       -> tri514(-135)
```

The descent is an unflagged staircase of triangles **tri504..tri514** in the
**north-east**, landing at tri514 (712,-135,452) - 42 hops and ~3100 cost from the
checkpoint.  Every earlier attempt was fighting over tri231/tri233/tri191, which
the collision rule refuses and which the map does not need.

Note also what this implies about the trigger zones: with both refusal rules
applied, **neither zone 0 nor zone 2 is reachable from tri239**, so the forest's
exit is not one of map 23's three trigger zones as far as walking is concerned -
it is either the north-east low ground or a map transition out there.

`cam_walk.py` gained a `XENO_WALK_GOAL_TRI` override (walk to one triangle rather
than the zone rectangle) plus the corrected two-flag walkability rule, and is
walking the 42-hop route live (`tri240 -> tri241` confirmed) as the round ends.


## New harness switches: mass damage in GOD mode, and random battles on/off

Route testing was spending nearly all of its wall clock in random encounters, and
each attempt ended when the party was worn down rather than when the route was
explored.  Two host-side switches fix that.  Both default to retail behaviour and
neither writes a guest word for the encounter switch, so the matching build is
untouched (the two call-site skips are `#ifdef XENO_PC_PORT`).

**GOD mode now also deals mass damage.**  `PcPort_GodModeBeforeGuest` already
hooks retail `0x80085618`, which consumes an action row (HP damage kinds 0/5/7/8,
amounts at `0xC3FE8 + action*72 + slot*2`, slots 0..2 party and 3..10 enemies).
It used to only zero the party's incoming damage; it now walks every slot and
forces enemy damage to **9999** (keeping the Gear bit, never touching an absorb),
so ordinary encounters end in one action while a boss with more HP still takes
the hit and the row-consume path stays honest.  Log:

```
[xeno-port][god-mode] ON (party HP damage blocked, enemy damage forced to 9999, foot and Gear)
[xeno-port][god-mode] mass damage 24 -> 9999 slot=3 gear=0
```

**Random encounters can be switched off.**  Retail gates encounters on
`g_FieldControl.isRandomEncountersEnabled == 0` and rolls for one on every held
direction (`func_80079288`) plus the per-frame check (`func_8008399C`,
`src/field/main/misc8.c:495`, `src/field/main/misc6.c:747`).  Both call sites now
consult `PcPort_RandomBattlesEnabled()`.

Controls: **F10** toggles, and the toolbar gained a **BATTLES** button (green =
encounters live, red = suppressed).  The F10 binding lives in
`pc_port/src/psycross_host_toolbar.inl` (tracked) rather than in PsyCross's
`PsyX_main.cpp`, because `pc_port/extern/` is git-ignored - a binding there would
vanish on a clean checkout.  That handler sees every SDL event first and consumes
the key, so there is exactly one toggle per press (verified: one log line).  The right-hand buttons were re-laid out to
fit: `HD2D` 400-468, `BATTLES` 472-570, `GOD` 578-656.

Verified live: with BATTLES off, eight movement presses produced **0 encounters**
(the party walked from (-417,-1428) to (-237,-1641) freely), and clicking GOD on
logged the mass-damage mode.  Build `cc14798dd96c…`, LINK OK.


## FOREST EXIT REACHED: map 23 -> map 22 (round 31)

With the encounter switch off (F10, the harness feature added this session) the
walk was finally uninterrupted and the exit was found and crossed.

**The route**, from the earned checkpoint at (-417,0,-1428) on tri239:

1. **North-east staircase, not the checkpoint ramps.**  `cam_walk.py` planned with
   both of retail's refusal rules applied (`0x400000` step-down, `0x800000`
   layer-0) and walked the 42-hop chain
   `tri239 -> tri40 -> tri240 -> tri241 -> ... -> tri504(-15) -> tri507(-35) ->
   tri508(-58) -> tri509(-74) -> tri510(-94) -> tri511(-110) -> tri512(-127) ->
   tri514(-135)`.  It arrived at **tri514, (727,-140,511)**, i.e. down on the
   lower forest floor, and the walker logged `IN ZONE at (727,-140,511)`.
2. **Continue north-east.**  A second plan to tri185 crossed 60+ more hops and
   reached **(1903,-376,1410)** - the map descends further out there.
3. **Walk east.**  From (2267,-351,1470) pressing east crossed the map boundary:
   the last map-23 sample is `pos=(2267,-351,1470)`, the first map-22 sample is
   `pos=(2327,-348,1470)`, and the loader log shows

```
[field-diag] FieldLoad begin field=23 mapBuf=0x821d14
[field-diag] FieldLoad begin field=22 mapBuf=0x81b45c
```

So **the map-23 exit is the eastern boundary at x ~ 2300, z = 1470**, and the
party arrives on **map 22 at (-1356,-204,1000)** - the lower forest, with a log
bridge visible on screen.

This closes the navigation question completely: the checkpoint ramps the earlier
rounds fought over (tri191/tri231/tri232/tri233) are refused by retail's own
collision rules and are **not** the route; the route is the north-east staircase,
then east off the map edge.

**Caveat for acceptance:** this run had random encounters suppressed by the F10
switch, so no battle was fought between the checkpoint and the exit.  The route
is proven walkable; re-running it with encounters live (battles on) is the
retail-play acceptance step, and the checkpoint plus the triangle chain above
make that a repeatable scripted run.


## The intermittent stalled battle, localised: the guest spins in Vsync (round 32)

The retail-play acceptance pass (encounters live, GOD off) reached leg 14 of the
north-east chain and then hit the same stalled battle seen in rounds 8 and 30:
the party's **AP and Time gauges stay pinned at 0**, the scene keeps animating,
no turn ever resolves and no input helps (Circle, Cross, Escape and the attack
combo were all tried for several minutes).  `retail battle returned` never
appears, so the run cannot continue.

Sampling the live guest during the stall now names the loop:

```
$1 = 0x8004b54c          <- guest PC
$2 = 520246042           <- instructions executed
```

`0x8004B54C` is `Vsync` - `asm/slus_006.64/matchings/psyq/libetc/vsync/Vsync.s:2`
and `linker/undefined_funcs_auto.menu.txt:105 Vsync = 0x8004B54C`.  The round-8
stall sampled the *same* address (`0x8004b54c`, 130 million steps), so this is one
recurring defect and not three different ones: during these encounters the battle
code sits in a **`Vsync`-driven wait loop whose exit condition never becomes true**
in the port, which is also why the AP/Time gauges never fill.

That reframes it usefully: `Vsync` itself is not stuck (the port's shim advances
frames - the scene animates), so the missing piece is whatever the battle expects
to change between frames: the AP/time update, or a work-list timer task that the
port does not service while the battle overlay runs.

**Next step:** find the battle routine that calls `Vsync` in a loop, identify the
flag it tests, and check whether the port advances it.  The stall is intermittent
(roughly one encounter in three on this map), so it is reproducible but not
deterministic; `recovery.xgqs` (`53a26c8c...`) is intact and the acceptance run can
be retried.


## Candidate cause of the stall: `Vsync` returns the wrong value (round 33)

Sampling the stalled battle puts the guest inside a **nested guest callback**:
`run_guest_callback(callback=0x800BA974)` -> `PcPortMipsRun`, i.e. the battle
overlay invoked a guest callback (`func_800BA8F4`,
`asm/battle/matchings/mainl110/func_800BA8F4.s`, which is called per actor and
returns immediately), and the outer interpreter is parked in `Vsync`
(`0x8004B54C`).  So the battle's frame loop is running - the scene animates - but
it never satisfies whatever ends the fight.

Reading retail `Vsync` next to the port's shows a concrete parity gap in its
**return value**:

```
retail (asm/slus_006.64/matchings/psyq/libetc/vsync/Vsync.s)
    $s1 = (g_pTMR_HRETRACE_VAL - g_HsyncInterruptCount) & 0xFFFF
    a0 < 0   -> return g_VsyncInterruptCount
    a0 == 1  -> return $s1                      # hretrace delta
    a0 == 0  -> v_wait(...); return $s1         # hretrace delta
    a0  > 1  -> v_wait(...); return $s1

port (pc_port/src/psyq_compat.c:1700)
    mode == 1 or mode < 0 -> VSync(mode)  (PsyCross: PsyX_Sys_GetVBlankCount())
    otherwise             -> VSync(mode); return PsyX_Sys_GetVBlankCount()
```

So for `mode >= 0` retail returns a 16-bit **hretrace delta**, while the port
returns a **monotonically increasing vblank count**.  The guest consumes that
value as a per-frame timing quantity:

- `g_FrameDeltaTime = Vsync(1);` (`src/field/main/main.c:131`, `FieldUpdateDeltaTime`)
- `D_800ADB9C = Vsync(1);` (`src/field/main/main.c:308`, `src/field/main/misc2.c:2133, 2207`)

and the field's frame function confirms the store in asm
(`asm/field/matchings/main/misc2/func_8007554C.s`):

```asm
    jal   Vsync            # a0 = 1
    lui   $at, %hi(D_800ADB9C)
    sw    $v0, %lo(D_800ADB9C)($at)   # Vsync(1)'s value is kept per frame
    jal   Vsync            # a0 = -1  -> the vblank count, which the port gets right
```

A value that grows without bound instead of measuring the frame is exactly the
kind of thing that makes a `Vsync`-paced battle loop never terminate.  `Vsync(-1)`
(the `while (Vsync(-1) < target)` waits in `src/field/main/misc2.c:2297` and the
libcd timeouts) is the one form the port gets right.

The port has `g_VsyncInterruptCount` but **no** hsync/hretrace counters, so the
faithful value is not available as-is.  **Next step:** implement a port-owned
per-call delta (frames elapsed since the previous `Vsync(1)` query) as the
`mode >= 0` return, with a regression test that pins "the value tracks elapsed
frames rather than the absolute count" and rejects a mutant returning the raw
count; then re-run the acceptance pass and see whether the stall clears.


## CORRECTION: the Vsync return gap is real parity debt, but NOT the stall's cause (round 34)

Round 33 flagged `Vsync`'s return value as a candidate cause of the stalled
battle.  Checking every access to the global that receives it settles the
question: **`D_800ADB9C` is written and never read.**

```
asm/field/matchings/main/misc2/func_8007554C.s:9    sw $v0, %lo(D_800ADB9C)($at)
asm/field/matchings/main/misc2/func_8007554C.s:167  sw $v0, %lo(D_800ADB9C)($at)
asm/field/matchings/main/main/func_80077DAC.s:7     sw $v0, %lo(D_800ADB9C)($at)
```

Three stores, no loads - and `g_FrameDeltaTime` (`main.c:131`) has no readers
either.  So the field records `Vsync(1)`'s value and never branches on it, and the
divergence cannot be what ends - or fails to end - a battle.

It is still a genuine parity gap worth fixing for fidelity: retail returns a 16-bit
hretrace delta for `mode >= 0` (`(g_pTMR_HRETRACE_VAL - g_HsyncInterruptCount) &
0xFFFF`) where the port returns the absolute vblank count.  It is recorded here as
**known parity debt with no observed live effect**, not as the stall's cause; no
fix is claimed for it, and the earlier "candidate cause" wording is withdrawn.

The stall therefore still needs its own investigation.  The next step is a
differential call trace: run one battle with `XENO_BATTLE_MIPS_TRACE=1` to
completion and one to the stall, and diff the bridge call sequences to see which
call the stalled one stops making.


## Harness note: the title lands in the Continue stub and refuses the load (round 34)

The traced-battle investigation could not start: every fresh boot this round ended
up on the title's **Continue** screen, which calls the stubbed `func_801D9F98`,
and the checkpoint load was then refused:

```
[xeno-port][quick] F8 ignored: not in a field      (repeated)
```

The working run earlier in the session has no such line - its log goes straight
from boot to `load queued` -> `FieldLoad begin field=23` - so the difference is
that the title entered the Continue screen before the load.  `key.py` sends F8 via
`xdotool keydown/keyup` after focusing the window, and F8 does reach the handler
(the "ignored" line is the handler's own message), so the title screen itself is
what consumed the state.

Nothing about the route or the port changed; this is purely a harness state issue
and it needs to be cleared before the differential trace.  Candidate recovery:
confirm the title menu's cursor position before the load (or clear the Continue
selection) and load while the field is still active.


## Harness recovered, and the stall is now consistently reproducible (round 35)

**Why the load kept failing.**  With *no input at all* a fresh boot still ends up on
the title's Continue screen and then the stubbed save screen
(`[stub] func_801D9F98`).  The cause is the port's boot menu: on timeout it
confirms whatever the cursor is on, and the cursor defaults to Continue
(`pc_port/src/boot_menu.c:179`):

```c
        if ((released & CTRL_BTN_CIRCLE) ||
            (pressedOnce & (CTRL_BTN_START | CTRL_BTN_CROSS)) ||
            ui->phaseFrames >= delay) {
            int timedOut = ui->phaseFrames >= delay;
            ...
            if (ui->menuChoice != 0) {              /* Continue */
                if (PcPort_BootHasSave()) {
                    return PC_PORT_BOOT_TICK_CONTINUE;   /* -> save screen (stub) */
```

So a timed-out title silently enters an unimplemented screen and the field stops
being active, after which `F8` is refused with "not in a field".
`XENO_BOOT_DELAY` did not avoid it because the retail title loop, not only the
port's boot UI, is what advances here.

**Recovery that works:** press F8 repeatedly from ~10 s after boot; the press that
lands before the title's timeout queues the load and the checkpoint comes back at
(-417,0,-1428) on tri240.  That is what the run this round used.

Worth fixing at some point (port wart, not on the critical path): a timed-out
title menu should not confirm an unimplemented entry - it should stay on the menu.

**The stall now reproduces every time.**  With the checkpoint loaded and
encounters live, the acceptance walk reached leg 19 and its first battle stalled in
exactly the known way (AP and Time pinned at 0, no turn, inputs dead).  Unlike
earlier rounds this is now consistent, which makes it a good fix candidate.
Sampling the live guest during the stall:

```
pc=0x80089f20  ra=0x80089e7c  sp=0x801ffd98
pc=0x800aaf10  ra=0x800aaeec  sp=0x801ffb80
```

Both are battle-overlay addresses and the stack pointer is in the guest stack, so
the overlay is executing a *variety* of code rather than spinning in one place -
the fight simply never advances a turn.  (Guest memory is inside `g_PsxRam`, so
`gdb` cannot dump the stack directly; the port would need a small guest-stack
readout for that.)

Next step: capture the battle's per-frame bridge call sequence for one stalled
fight (`XENO_BATTLE_MIPS_TRACE=1`) and look for the update the battle expects
between frames but never gets - the AP/time advance is the obvious candidate.


## The battle frame loop, and what the stall is NOT (round 36)

The traced-battle run could not be driven to a fight this round (the harness ended
up with a pending quick-save showing `WAIT` in the toolbar and the field menu open
and unresponsive - the round-7 observation again - so `owner=0x80` blocked every
move), but reading the battle's frame loop narrows the stall usefully.

`func_800BE790` (`asm/battle/nonmatchings/mainc122/func_800BE790.s`) is the battle's
per-frame driver, and it does this every frame:

```asm
    jal  func_8001D468
    jal  WorkListUpdate
    jal  TimerWorkListUpdate
    lh   $s0, D_80059494        # frame counter
    addiu $s0, $s0, -1
    beq  $s0, -1, .L800BE904    # done when it reaches -1
.L800BE8EC:
    jal  func_800BBAB8
    addiu $s0, $s0, -1
    jal  TimerWorkListUpdate
    bne  $s0, $1, .L800BE8EC
```

so the battle paces itself on `D_80059494`, and retail's `TimerWorkListUpdate`
(`src/slus_006.64/system/work_list.c:86`) is what moves it:

```c
    if (g_WorkListCurTimer) {
        g_WorkListCurTimer--;
        if (g_WorkListCurTimer == 0) {
            D_80059494 = 0;
        }
        return;
    }
    ... walk g_TimerWorkList, calling each onTriggerCallback ...
```

**The port's `TimerWorkListUpdate` (`pc_port/src/work_list_port.c:106`) is
byte-for-byte the same logic, including `D_80059494 = 0`, and its callback
dispatch (`WorkListInvokeCallback`) either runs the callback or aborts loudly with
"unresolved guest callback".**  So the stall is *not* a silently dropped timer
callback, and it is not the work-list plumbing: if a battle timer callback were
unresolvable the port would abort instead of hanging quietly.

That leaves the battle waiting on something the work-list does not drive - the
attack/animation task it started (`func_800BBAB8` and friends), an expected input
edge, or the AP gauge update.  Continuing from here needs the bridge call trace of
one stalled fight, which the harness must first be able to reach.


## The real harness blocker: a quick-load leaves a stale menu owner (round 37)

Round 34-36 blamed the title's auto-continue for the load trouble.  The load does
work (with F8 pressed early enough), but **every quick-load leaves the field menu
owning input**, and that is why movement never starts afterwards.

Measured with **no input at all** immediately after a load:

```
g_C1ButtonState     = 0x0      no pad button held
g_C1PrevButtonState = 0x0
D_800C3900          = 0x0      no field press edge
D_800ADB64          = 0x80     <- the field system menu owns input
POSDIAG ... owner=0x80
[xeno-port][menu] func_800799D4 request=128 -> MenuMain
```

So the menu is open with zero input: the owner is stale, not pressed.  The field
only clears it in `src/field/main/main.c:671-675`, and that guard (render context
0 and no actor script lock) is evidently not met at that moment, so `0x80` sticks
and every movement press is swallowed.

A fix was attempted in `PcPort_QuickCheckpointRestore` - drop the queued pad
states (`ControllerResetState`), clear the field's derived masks (`D_800C3900`,
`D_800C2694`, `D_800AFE9C`) and set `D_800ADB64 = 0xFF` - and **it did not work**:
`owner` came back as `0x80` on the next sample, so something re-sets the owner
after the restore runs (only `src/field/main/main.c:678` assigns `0x80`, guarded by
`D_800C3900 & 0x10`, which needs a press edge).  Rather than commit an unproven
change the edit was reverted and the build restored to `589fb02e...`.

**Next step:** find what sets the owner after the restore - the prime suspect is the
load being *prepared* by the title menu (see the comment in
`pc_port/src/quick_checkpoint.c` about "FieldMain's next poll after the menu's own
cleanup") so the field's first frame runs the title menu's pending request.  Trace
`D_800ADB64` writes across one load (a watchpoint on it) and close the path that
sets `0x80`.


## BATTLES button verified, and the menu-owner write pinned (round 38)

**The random-battle switch works from both controls.**  Clicking the toolbar
**BATTLES** button (now x 472-570, y 4-30 after the right-hand re-layout) toggles
it and the log confirms each press; **F10** does the same:

```
[xeno-port][random-battles] OFF (route testing; field encounter rolls suppressed while off)
[xeno-port][random-battles] ON  (route testing; field encounter rolls suppressed while off)
```

The button reads **green** while encounters are live and **red** while they are
suppressed (both captured this round).  So the switch the user asked for behaves
correctly from the toolbar and the keyboard.

**The stale menu owner is a real write in the field.**  A conditioned hardware
watchpoint on `D_800ADB64` across one quick-load caught it:

```
Hardware watchpoint 1: D_800ADB64
Old value = 255
New value = 128
#0  FieldMain () at src/field/main/main.c:679
#1  MainLoop (errorCode=0) at src/slus_006.64/main/main_loop.c:149
#2  FieldMain () at src/field/main/main.c:708
```

so it is the field's own menu-open at `main.c:678-679`, firing because
`D_800C3900 & 0x10` (a Triangle press edge) is set on that frame - the loader's
own clear to 0xFF happens first (`func_800705DC` during `FieldLoad`), and a press
edge from the title-menu phase is still in the pad queue afterwards.

A guard was implemented (arm on restore, suppress the menu-open for N frames) and
tested at both 1 frame and 30 frames: **the menu still opened**, so the edge is not
confined to the frames after the restore, and the change was reverted rather than
committed unproven.  The build is back to `589fb02e...`.

**Next step:** the edge survives longer than a fixed window, so the fix belongs
where the edge is consumed - either drop the field's `D_800C3900` bit at the point
`FieldMain` reads it when the field has just been restored, or find which title-menu
path leaves the pad state queued.  A watchpoint on `D_800C3900` across the load
would name the producer.


## ROOT CAUSE: the port's function keys leak into the pad state (round 38)

The stale menu owner after a quick-load is now explained end to end, and the cause
is not the menu at all: **pressing a function key also presses a pad button.**

Measured live by sampling `g_C1ButtonState` while each key was held:

| key | `g_C1ButtonState` |
|---|---|
| `q` (unmapped) | `0x0` |
| `F1` | `0x0` |
| **`F8`** (quick-load) | **`0x20` = Circle** |
| **`F9`** (recording) | **`0x20` = Circle** |
| **`F11`** (speed cycle) | **`0x10` = Triangle** |
| `z` (mapped Circle) | `0x20` (as expected) |

So a quick-load both loads the checkpoint **and presses Circle**, which:

- explains why the title menu kept auto-selecting or confirming entries while the
  load was being prepared, and
- is the press edge that the restored field sees - `D_800C3900` picks it up and
  `FieldMain` (`main.c:678-679`, watchpoint-proven) opens the system menu, whose
  `0x80` owner then swallows every movement press.

F11 leaking Triangle is the same defect and is user-visible too: cycling the speed
also presses Triangle, which opens the field menu.

`PcPort_ForcedKernelSelect` (`pc_port/src/psyq_compat.c:505`) is the only synthetic
Circle injection in the tree and it is inert here (`sel` starts at -1 and returns
early), so the leak is in the keyboard-to-pad path - PsyCross's `PAD_Update` / the
port's `g_cfg_keyboardMapping` overrides (`port_main.c:1234` sets `kc_circle = 29`,
`kc_triangle = 25`) or in how the port's function-key handler interacts with the
pad state that frame.

**Next step:** fix the leak - consume the function keys before the pad update, or
exclude them from the pad mapping - then the quick-load needs no menu guard at all.

A menu-open guard was implemented and tested along the way (arm on restore, allow
once the input settles, hard cap).  Its certificate passed 63 checks x 3 regimes
with 5/5 mutants rejected, but the settle rule **failed live** (a 30-frame window
let the menu open a frame later, and even a settle-based rule released before the
edge arrived; only a long fixed window kept it shut).  Since it mitigates a cause
now identified, the whole change was reverted and the build restored to
`589fb02e...` rather than ship an unproven guard plus a certificate for it.


## Input leaks and a stale-driver confound (round 39)

Two things were measured this round, and one of them had been corrupting every
attempt:

**1. Stale walker processes.**  Four `cam_walk.py` instances from earlier rounds
were still alive and kept pressing keys, so input arrived from several drivers at
once and the field menu kept being re-opened.  After killing every driver
(`cam_walk`, `goto`, `fight`, `tri_walk`) and closing the menu once with Cross
presses, a single walker ran cleanly - which also means several earlier "the menu
re-opened by itself" observations were partly this, not only the F8 leak.

**2. Host input leaks into the pad.**  Sampling `g_C1ButtonState` while holding
keys or clicking shows host UI actions reaching the game as pad buttons:

| action | `g_C1ButtonState` |
|---|---|
| `q` (unmapped key) | `0x0` |
| `F1` | `0x0` |
| `F8` (quick-load) | `0x20` Circle |
| `F9` (recording) | `0x20` Circle |
| `F11` (speed) | `0x10` Triangle |
| click on the GOD button (617,17) | `0x20` Circle |
| click in the game area (300,300) | `0x10` Triangle |
| click bottom-right (640,500) | `0x0` |

So the toolbar's own buttons - including LOAD, which is the documented way to
restore a checkpoint - also press Circle in the game.  `PcPort_ForcedKernelSelect`
is the only synthetic Circle injection and is inert here, and `PsyX_pad.cpp`'s
keyboard mapping reads `g_sdlKeyboardState[mapping.kc_circle]` correctly, so the
leak is elsewhere in the host-input path and still needs pinning.  Recorded as
**known input-routing debt**, with the practical workaround that the menu must be
closed with Cross presses after a load before walking.

**3. The acceptance walk now runs.**  With one driver and the menu closed, the
retail-play walk (encounters **live**, GOD off) reached **leg 29, tri314
(117,0,174)**, heading for the tri504 descent staircase, having entered 2
encounters - i.e. the north-east route is being walked under normal play.

### Round 39 addendum: the walk completed 3/3 encounters

With a single driver and the menu closed, the retail-play walk (encounters live,
GOD off) reached **leg 71 at tri318 (340,0,469)** heading for the tri504 descent
staircase and **completed all three encounters it met (3 entered / 3 returned)**.

That matters because the "intermittent stalled battle" chased since round 8 did not
occur once the stray drivers were gone.  Several of those stalls were probably the
same confound - more than one `cam_walk.py`/`fight.py` instance alive at once,
pressing battle keys and leaving the battle waiting for input that never arrives in
the order it expects.  The stall is still recorded as *unexplained* rather than
solved, but any future attempt should treat "exactly one driver" as a precondition
before blaming the port.

## Round 40 (goal's last round): where the route stands, and the blocker

The retail-play acceptance walk (encounters live, GOD off, one driver) reached
**leg 80 - tri511 at (552,-115,494), one hop from the tri512/tri514 descent goal** -
and then hit the **stalled battle** again: the party's AP and Time gauges sit at 0,
the scene keeps animating ("total damage", combo 1/4, Fei 74/84, Elly 40/44 with
unchanged HP), no turn ever resolves, and five Left+Circle escape attempts were
ignored.  `battles=4/3` in the log: the fourth encounter never returns.

### What the goal covered, and what it did not

**Achieved and committed on the covered path**

- The forest route itself is **proven**: checkpoint (-417,0,-1428) -> the north-east
  staircase `tri240 -> ... -> tri504..tri514` (y 0 -> -140) -> north-east to
  (1903,-376,1410) -> east off the map boundary at **x ~ 2300, z = 1470**, where the
  loader reports `FieldLoad field=23 -> field=22` and the party arrives on **map 22
  at (-1356,-204,1000)**.  That run had encounters suppressed by the F10/BATTLES
  switch (a harness feature added this session).
- With encounters **live** the same route was walked to leg 80 and the descent
  staircase at y=-115 before the stall - i.e. the route works under normal play as
  far as the stall allows.
- Port defects found on the live path and fixed with guarded changes plus
  mutant-rejecting certificates: the battle bridge's resolved-call cache eviction
  (`cdd91e4f`; a stale entry was served as a hit and mis-dispatched overlay PCs) and
  the File-menu notice blocking the game thread on a modal dialog (`44e14a9e`,
  `b69a8cd3`).
- Harness features the user asked for: mass damage in GOD mode and the
  random-battle switch (F10 + the BATTLES button), both verified live
  (`d2293840`), and walkmesh triangle + camera telemetry for driving the route
  (`383e3daa`, `4ede4378`).
- Documented, unfixed debt found on the live path: `Vsync`'s return value differs
  from retail for `mode >= 0` (write-only in practice, `bb743235`); host input
  leaks into the pad (F8/F9 -> Circle, F11 -> Triangle, toolbar clicks -> Circle),
  which is what leaves the field menu owning input after a quick-load; and the
  round-8..40 **battle stall**.

**Not achieved:** the route was never driven end-to-end with encounters live, zone 2
was never entered, and no story progression past the forest exit was observed.

### Concrete blocker

An **intermittent battle that never grants a turn**: AP and Time stay pinned at 0,
the fight animates but never resolves, and no input (attack combo, Cancel, or
Escape) is accepted, so the run cannot continue and must be reloaded.  It has now
blocked acceptance attempts in rounds 8, 30, 32, 36 and 40, twice at the last hop
of the descent; the party is healthy when it happens and the guest is executing
(`pc` samples across the battle overlay), so it is not a crash and not an
out-of-resources state.
