# Retail directional picker at 80084854

Status: C picker implemented; bounded hybrid and actual-native-callee differential tested. Exact C
matching INCOMPLETE; native runtime adoption and natural gameplay acceptance
NOT IMPLEMENTED/NOT OBSERVED. Native ratan2 is tested on this picker's sampled
bounded inputs, not established for its entire public API input domain.

Latest exact gate: tkfSQk FAIL136/138, size228 matching retail. Only offsets
1C8 and1CC differ: second product uses mflo a2/add a0,a0,a2 instead of
retail mflo v0/add a0,a0,v0. Earlier82/138 results below are historical.

## Authority

Battle body: file offset14D64, extent228 hex (138 instructions), SHA-256
`1830b4ef1fe37129972fc310dfad534f8161d6c0b123e74254c3711334a3e291`.
Inspected direct caller80084B40 calls it at80084CD0.
The actual ratan2 callee is8004B32C; its table starts80057030. Both come from
SLUS_006.64, SHA-256
`dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119`.
The test loads that executable's49800-byte payload at80010000, then overlays
the isolated battle body at84854. It does not bridge or replace any callee.

## Selection rules

Actor and direction arguments use their low byte. Default result is the actor.
The routine scans exactly list slots0..10 at800C3E90..800C3E9A, not slot11.
It ignores the global list count. FF and self entries are skipped, not a
termination condition; subsequent entries are still visited.

Coordinates are unsigned halfwords at800C3EC0 and800C3EBE, stride28 bytes.
For each other candidate, ratan2 receives candidate-minus-actor deltas in that
order. Let `a` be its return value and `u16` denote low16-bit truncation:

| Direction byte | Candidate angle accepted when |
| --- | --- |
| 0 | u16(a+200) <400 |
| 1 | u16(a+600) <400 |
| 2 | u16(a+800) <200 OR u16(a-600) <=200 |
| 3 | u16(a-200) <400 |
| other | never |

All constants in this table are hexadecimal. The direction2 upper boundary
is inclusive in its second interval. Invalid directions still visit other
candidates and call ratan2; do not silently introduce an early return.

After angle acceptance, the routine reloads the current list entry and
coordinates. Squared differences and their sum retain only the low32 bits;
the sum is compared as signed against the current best, initially00FFFFFF.
Strictly smaller wins, so later ties do not replace the earlier winner.
Overflow can make a distant target's signed wrapped score negative: do not
replace this with an unbounded distance or undefined signed C overflow.
The best target is returned through its low byte. Only the guest stack is
written by the inspected body and real callee.

## Evidence and limitations

`run_battle_target_direction_retail_test.sh`, final evidenceqKdykB (supersedes
P4yb40/MGxpU8): O0/O2/UBSan
each8713 cases: 8712 grid cases with three actor words (0,3,255 with dirty upper bits), three
coordinate origins, eleven values per coordinate (including signed16 and
square-overflow boundaries), and eight direction words. Fixtures include
self, FF holes, equal-position later ties, and an excluded slot11 candidate.
The additional directed case requires slot10 to win with distance1 over an
earlier distance400 candidate, while the excluded slot11 holds distance0.
This closes the original grid's coverage gap where slot10 could only tie.

Assertions cover result, all eleven scanned slots, exactly three real ratan2
calls with their actual argument and return words, only eight stack word writes, and
s0-s7/SP/RA preservation. The expected angle uses an independent integer
expression over the same authoritative table; this is not an exhaustive
ratan2 implementation proof or an independent emulator implementation.
Coordinate and state coverage is bounded, not exhaustive. Guest stack/data
aliasing and call-time mutation of the list/coordinates are not covered.
Native differential tests and negative controls must precede native adoption.

Per build, asserted nonzero witness counts include858 negative-score winners,
522 retained equal-score ties, and321 direction2 angle+800 upper-boundary
hits. These are synthetic contract witnesses, not natural gameplay evidence.

## C picker and explicit hybrid boundary

The C body in main35 uses unsigned32 products and addition, then interprets
the wrapped bits as s32 on the supported PSX/Linux compilers. Coordinate
differences and their negations remain within[-65535,65535], so no signed
product overflow is needed. Direction uses a signed low-byte value, while the
list loop compares pointer-width signed integers as retail does on PSX32.
Row310's existing declaration was moved earlier and reused for both coordinate
arrays; the neighboring85310 body is unchanged.

The native test's ratan2 function executes the actual retail callee through
the MIPS adapter for each call, checking arguments and results. This is
native-picker/retail-atan differential evidence, NOT a full native trig path.
Initial WXti2n failed native link on the missing picker (test-first RED).
Earlier CP3gFx (superseded by NQlCgv below): O0/O2/UBSan and separately compiled expanded O2 baseline each
8713casesPASS. Complete native coordinate arrays, including opaque bytes,
and the list are compared against snapshots. The original value-only checks
let the padding_write mutant survive in eRbLgO; the final test rejects it.

Nine mutants are rejected: actor_mask, direction_mask, mode2_strict,
unsigned_distance, ties, last_slot, self_skip, signed_product, padding_write.
The signed-product control must report UBSan signed integer overflow; the
others must reach specified assertion failures, not merely crash.

## Exact gate remains open

`run_battle_target_direction_match.sh`, latest evidence0fymkx: FAIL82/138,
size218 versus retail228. The frame now matches retail30 bytes (hex); the
setup sequence matches through offset053. Remaining differences include
cached list/coordinate loads, distance operand reuse and instruction scheduling.
No generated instructions were edited and no compiler flags were changed.

Earlier8n1SQG was73/138,size23C,frame40. Its original compiler RTL/global
allocation dump identifies squared-result pseudos167/168 constrained around
multiply-result registers and spilled. A register declaration hint did not
change that output (6Gr3Uv). Computing each absolute difference before one
unsigned square removes the cross-branch product assignments and the spills.
Precomputing maskedactor*sizeof(Row310) as a byte offset reproduces retail
setup ordering; maximum offset7140 remains within the256-row fixture arrays.
Reverse-field subtraction experiment kgBRRt regressed76/138 and was reverted.

The primary isolated linker places .direction at80084854. The secondary
selection-only linker parks it at80088000 to avoid its relocated attack text;
that secondary placement is not evidence of directional-function matching.
The existing attack and selection comparisons still pass. Neither link proves
a full overlay or activates runtime dispatch. Exactness, native-callee,
guest-alias, and natural-gameplay gates remain open.

## Actual native atan path

The runner now also compiles the existing PsyCross LIBGTE.C body, renaming its
ratan2 symbol only inside the test object so a thin observer can validate
arguments and results. No vendor source is changed. All1025 entries of that
body's actual ratan_tbl are compared against the executable's retail table.
This path executes no emulator calls from the native picker; the separate
retail reference still executes the original picker and original ratan2.

PsyCross declares int ratan2(int,int), while retail PsyQ declares long arguments
and return on PSX32. The picker now selects the correct prototype for each
build, avoiding an LP64 cross-translation-unit type mismatch. This change is
scoped to main35, not a repository-wide API audit.

NQlCgv terminal0: both hybrid and actual-native paths pass8713 cases at
O0/O2/UBSan, and both have separately executed expanded O2 baselines. Each
of the nine existing mutants is rejected on both paths (18 control runs),
including UBSan signed-product overflow and complete-row write detection.
Earlier IK7drJ established native positive cases before the paired mutants.

Native source evidence is recorded per run in native-atan-sources.sha256.
Inspected vendor HEAD f4da486aa53565a4a8efa439c9671bd35dc94eda, selected body
and table clean. Recorded body SHA c177bf5cf7b1649de2d2c1b64ca03651bc9436eb5594e849d5aa5c1de261849f;
table-header SHA80db6b99e58a0d64bd3f1b132a14c5c7d17e937a456e9f5436e6c837afec3fd4.

The tested picker only supplies differences in[-65535,65535]. This does not
prove PsyCross behavior at INT_MIN or on other callers' unrestricted inputs.
Exact82/138 matching, general alias/access-order coverage, runtime identity
and dispatch admission, and gameplay acceptance remain separate open gates.

## Native atan quotient-boundary coverage

EHD9Zc completed with terminal0. The standalone
`battle_target_atan_retail_test.c` compares the actual native body directly
against retail ratan2 for303121 executions per O0/O2/UBSan build. It samples
the first numerator reaching each quotient0..1024 and its two neighbors,
using13 denominators from1 through65535, both axis orders and all signs,
plus(0,0). Executions include duplicates; this is not an exhaustive Cartesian
input-domain test. All1025 retail table entries are observed being read and
both axis branches execute. Retail writes are forbidden, preserved registers
are checked, and the native table must remain unchanged.

An isolated native-table last-entry corruption is rejected specifically by
the native-versus-retail result assertion, after initial table identity checks.
The retail oracle and vendor files are not modified. Existing8713-case picker
tests and all18 paired picker mutation controls also pass in this run.
This adds boundary evidence within the caller domain without changing source
matching, runtime dispatch, or the unrestricted-ratan2 acceptance boundary.

Review follow-up: the execution count is now asserted, not merely printed,
to fail closed if the sample set shrinks. Final SyjZy4 rerun passed all three
builds and all19 negative controls (one table plus18 picker controls).

## Branch-local distance accumulation

The accepted-distance C now assigns the first unsigned square in each sign
branch and adds the second unsigned square inside each second-axis branch.
Negative branches compute a separate signed reverse difference from the same
u16 fields. Those differences are bounded, while products and their sum stay
unsigned and retain low32 bits. This removes allocator spills without adding
volatile accesses, barriers, instruction patches, or compiler flag changes.
Retail's repeated candidate loads are restored by the resulting compilation.

S6XevL and final tkfSQk give136/138 with exact228 size and30-byte frame.
pB4dqk terminal0: both picker paths pass8713 cases at O0/O2/UBSan and expanded
O2; standalone atan passes303121 per build; all19 negative controls rejected.
Exact matching still fails, and no runtime-adoption claim follows from this.

### Remaining reload-register diagnosis

Original-compiler diagnostic dumps in tkfSQk (`main35.i.lreg`/`.greg`,
`diagnostic.s`) place second-axis product pseudos205/206 in LO (register65).
The reload pass requests a GR_REGS scratch for the add and inserts insns423/426,
moving LO into a2; final optimization merges the branch-tail copies. This
locates the remaining mismatch in reload allocation, not the square arithmetic.
No compiler options used by production or the exact gate were changed.

YBqnZp conditional-expression addition, yykDkE signed accumulator with explicit
unsigned wrapping, and ZbwUmA reversed add operands each remain136/138.
CjqIVV assigning square bits back to the input variables regressed111/138,
size234. All trials reverted. JCY5Xl terminal1 reconfirms136/138,size228;
its preprocessed source and direction binary are byte-identical to tkfSQk.
