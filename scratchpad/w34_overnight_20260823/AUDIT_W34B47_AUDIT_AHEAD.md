# W34B47 — three-region audit-ahead after the D554 block

This is the prescribed D2 detour after W34B46 classified the D554 clear
writer as class-(e). No production source changed. The three next retail
regions are audited below from the retail listings and current resolver/
tripwire inventory.

## Region 1 — frame-exit continuation `0x800719D0..0x80071A4C`

Retail reaches this block only when the `0x800719C8` D554 branch is not
taken. It is therefore downstream of the held edge, not a way to get past
it. The block is 0x7C bytes (31 instructions) and has these direct calls:

| PC | target | role/status |
| --- | --- | --- |
| `0x800719D0` | `0x80044110` | `ResetGraph(1)`; host symbol exists |
| `0x80071A08` | `0x8004495C` | display-environment setup; host symbol exists |
| `0x80071A10` | `0x80096694` | world post-frame/CD cleanup; no production body, only a legacy stub |
| `0x80071A18` | `0x800445D0` | `DrawSync(0)`; existing shim |
| `0x80071A20` | `0x8004B54C` | `Vsync(0)`; existing shim |
| `0x80071A30` | `0x80044E9C` | `PutDispEnv`/presentation call; existing host path |

The branch at `0x800719E4` skips the small draw-environment setup when the
buffer-parity word is nonzero. The calls are not raw guest-pointer calls in
the retail shape, but `0x80096694` is an unresolved cleanup leaf and the
block is not naturally reachable while D554 remains 1. Classification:
**class-(e) in the current route**; do not implement as a speculative
post-tail shim. If D554 is later proven to clear naturally, re-audit this
as a possible class-(b) bounded continuation with a focused
`0x80096694` decision.

## Region 2 — mode/session initializer `0x80072238..0x80072998`

This is a one-shot session initializer, 0x760 bytes / 472 instructions,
with multiple loops and synchronization/resource lanes. Its direct-call
inventory from `W_72238_RAW.txt` is:

`0x80072BB0, 0x8004495C, 0x800445D0, 0x80072DB4, 0x80028A60,
0x80071EF0, 0x800286CC, 0x80073530, 0x8009766C, 0x80098044,
0x8001B66C, 0x80039CC4, 0x800399D4, 0x80073398, 0x80073448,
0x8007565C, 0x80075D4C, 0x8008440C, 0x800979C8, 0x80084580,
0x80072090, 0x800736DC, 0x80073E30, 0x80085F58, 0x80024F64,
0x80074594, 0x800863E0, 0x80074E58, 0x80075030, 0x800739B8,
0x80088F64, 0x80037FD8, 0x80028470, 0x80097BC0, 0x800967E4,
0x8004B54C, 0x80096668, 0x80097CB8, 0x80096694, 0x800320E8,
0x80038428, 0x800288EC, 0x8003F968, 0x80039850, 0x80039A80,
0x80039B68, 0x80097718, 0x800976FC, 0x80089160, 0x800978FC,
0x8008901C, 0x800865A0, 0x80085FE0, 0x80075228, 0x80033698`.

Several targets are already implemented host/PsyQ or bounded world pieces,
but the prerequisites at `0x80072BB0` and `0x80097BC0`, the archive/setup
fan-out, the repeated polling loops at `0x800722A0` and `0x80072514`, and
the guarded `0x800976FC` table population remain unresolved as one retail
function. Existing convergence P1/P2 and common-tail P0..P5 cover only the
later selected arms; they do not constitute the initializer body.

Globals touched include `A180 -> BE4C` configuration copies, `CCA4`,
`D3CC`, `D804`, `CEC0`, `C7E8`, `BD34`, `D144`, `C178`, `CD40`, the
`0x8006` audio/resource mirrors, and the dispatch-table/session records.
The function contains DrawSync/Vsync and CD polling waits, not a leaf
sync-shim route. Classification: **class-(e), BLOCKED-NEEDS-REVIEW**;
the `0x80072238` should-not-run tripwire stays intact.

## Region 3 — post-loop teardown `0x8007299C..0x80072BAC`

This address is a separate function entered indirectly from the session
loop only after `0x800712D0` returns and the D7CC session condition permits
teardown. It is 0x210 bytes / 132 instructions and contains 26 direct
calls:

`0x8003A89C, 0x80039FF8, 0x8003852C, 0x800320E8 (multiple),
0x800230A8, 0x80075460, 0x80092DD0, 0x800931B0, 0x80084818,
0x80086124, 0x80024FB8, 0x80086568, 0x800866C8, 0x8007474C,
0x80074F04, 0x800750DC, 0x80088FF4, 0x80089128, 0x80097D64,
0x800976A0, 0x800960BC`.

The 64-slot loop frees slot payloads at `+0x4C`; a second three-entry loop
frees `BC38/BCB0/BC3C/BCB4`-adjacent session resources. It also releases
graphics/work-list state and invokes several world resource helpers. It
does not render the first frame and cannot advance the held frame edge.
Classification: **class-(e), BLOCKED-NEEDS-REVIEW**. The
`0x8007299C` should-not-run tripwire remains intact.

## Detour conclusion

No class-(a)/(b) convergence gap appeared. The only small-looking next
block, `0x800719D0`, is unreachable until D554 clears and contains the
unresolved `0x80096694` cleanup leaf. The mode initializer and teardown are
large, stateful, and wait/resource-heavy. The morning review queue is now:

1. decide whether to audit/implement the D554-clearing frame/callback
   closure;
2. if that clears D554, separately audit `0x80096694` before the frame-exit
   continuation;
3. keep the mode and teardown tripwires unchanged until their prerequisites
   are explicitly approved.

Evidence is read-only: `W_712D0_TAIL.txt`, `W_72238_RAW.txt`,
`W_7299C_RAW.txt`, `POST_CONVERGENCE_CALL_GRAPH.md`, and the W34B45/W34B46
natural/census logs. No renderer or framebuffer milestone was reached.
