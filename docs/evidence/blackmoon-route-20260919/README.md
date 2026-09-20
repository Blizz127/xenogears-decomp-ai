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
