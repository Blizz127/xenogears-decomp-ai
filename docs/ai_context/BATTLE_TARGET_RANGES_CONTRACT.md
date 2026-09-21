# Retail target ranges at 80084548

Status: retail contract tested; C replacement and native adoption NOT IMPLEMENTED.
Authority: battle payload SHA-256
`1830b4ef1fe37129972fc310dfad534f8161d6c0b123e74254c3711334a3e291`,
file offset `14A58`, extent `208` hex (130 instructions).

## Defined setup in the inspected caller path

`80084DE4` calls this routine at `80084EC4`. Its inspected setup supplies
mode 0, 1, or 2. This is a bounded direct-call audit, not an indirect-call
or other-overlay census.

Additional pinned-image scan: all aligned words in battle.bin and SLUS_006.64
were checked for J/JAL encodings to84548 and literal addresses80084548,
A0084548 or00084548. The only match was battle offset153D4, JAL0C021152 at
80084EC4. No direct or literal-pointer match occurred in the executable.
This does not cover computed addresses, indirect transfers, or other overlays.

`battle_target_range_caller_test.c` executes the actual84DE4 instruction
prefix, stopping at the selected callee entry after the call delay slot.
It checks all65536 low-halfword flag values, complemented a0/a1 flag inputs,
control bytes0/1/2/3/255 with dirty upper bits, and fifth-argument bytes0/1/255.
That is983040 executions:327680 range calls and655360 actor-range calls.
Range arguments must match the independently computed mode, status bypass,
and order; all range modes are0..2. It also checks actor-byte masking, return
addresses, stack adjustment and bounded prefix stack writes. No callee is
bridged or executed, and no full caller-function behavior is claimed.

Dz1PDc terminal0: all983040 cases pass at O0/O2/UBSan. Range mode witnesses
are49152/49152/229376 for modes0/1/2 per build. An isolated instruction-copy
mutation changing mode2 setup to3 is rejected at the mode argument assertion.
The retail file itself is never modified. Independent read-only review found
no issues within this bounded caller-prefix contract.

All mode/flag/order arguments are interpreted through their low byte.

| Mode | First range | Second range |
| --- | --- | --- |
| 0 | 3 through 10 | none |
| 1 | 0 through 2 | none |
| 2, order byte zero | 3 through 10 | 0 through 2 |
| 2, order byte nonzero | 0 through 2 | 3 through 10 |

The routine clears list bytes `800C3E9B` down through `800C3E90` to FF,
then resets byte count `800D3274` and halfword accumulator `800C3D64`.
It visits each range in ascending index order. Each candidate is passed to
`80084108` with the low byte of the flag; the predicate result is also masked
to a byte. Accepted candidates are appended without rank sorting.

For each accepted candidate, list storage precedes the call to `80089C08`.
That helper reads a halfword from `800C3448 + index*2`; do not substitute a
computed bit shift for the table value. After the call, the routine reads
the current count and accumulator, writes their OR with the returned value
to the halfword accumulator, then writes the incremented byte count.
It returns list entry zero, including FF for an empty result.

## Out-of-domain register dependence

For mode bytes outside 0..2, retail does not initialize all range state.
With mode3, incoming a3=3, and all candidates present with status bypassed,
incoming s4=0 produces zero candidates, while s4=1 produces candidate3.
Both executions use the same explicit arguments and full retail bodies.

Do not silently initialize these inherited values in a C/native replacement.
Any native admission rule limited to modes0..2 must decline before writes
and preserve guest execution outside that domain. Such a rule remains a
coverage limitation until the actual caller domain is established. Likewise,
do not introduce uninitialized host C reads as a substitute for guest state.

## Evidence and remaining work

`run_battle_target_ranges_retail_test.sh`, evidence `MCpC03`: O0/O2/UBSan
each49152 supported cases, exact global write sequence/width/value checks,
both real retail callees, and mode3 incoming-register counterexamples.
This is an emulator-backed contract, not native differential or gameplay proof.

Before adoption: implement and match the body against all130 words; prove
native caller boundaries and callback ordering; test with real callees plus
negative controls; retain the existing overlay identity/dispatch guards.
The neighboring `800841E0` mismatch remains a separate unresolved gate.

## Actor-filtered alternate range at 80084750

Status: C replacement passes bounded native/retail differential tests;
instruction matching INCOMPLETE and native runtime adoption NOT IMPLEMENTED.
Same payload authority, file offset `14C60`, extent `104` hex (65 instructions).
The inspected `80084DE4` caller invokes it at `80084EB4` on the alternate
branch. Unlike the unsupported modes of 84548, its range setup is initialized
inside the function: exactly eight candidates, indices 3 through 10.

It uses the same descending twelve-byte list clear and count/mask reset order
as 84548, but calls `80083FF4(actor & FF, target)` instead of the status-only
predicate. Normal targets require a zero matrix entry at owner+140+
actor_group*64+target_group*8; alternate targets bypass that matrix check.
Both paths retain presence, block, and their selected status checks.
Accepted targets append in ascending order; real `80089C08` supplies each
table halfword. List storage precedes that call, and the mask halfword update
precedes the incremented count byte. Return is first list byte, FF if empty.

Expanded `run_battle_target_ranges_retail_test.sh`, evidence `12jMvf`, passes
O0/O2/UBSan: existing 49152 range cases and 10240 additional actor-range cases
per build. The latter cover five actor words (including 255 and dirty upper
bits), all256 candidate presence masks, and eight block/alternate/status
configurations with asymmetric actor/target matrix entries. All callees execute
their retail instructions. Assertions check list, count, arbitrary table-mask
OR, ordered global writes including widths, return, SP/RA, and s0 through s7.
These are synthetic non-aliasing fixtures, not a complete actor/state census,
stack-alias proof, or gameplay acceptance.

Native extension evidence `nmilv1`: the same10240 cases pass against the C
replacement with the real native83FF4 and main41's89C08, at O0/O2/UBSan plus
an independently executed expanded O2 baseline. All five mutants fail their
output assertions: actor_zero, range_zero, eligibility_zero, mask_replace,
clear_short. Initial native link `v2tZKC` failed on the missing84750 body.
The native fixture compares outputs; ordered writes and saved registers are
asserted on retail execution only. Native access ordering is not yet proved.

Exact gate `run_battle_target_actor_range_match.sh`, earlier source evidence
`cEY9CJ`: FAIL51/65, correct104-byte size. Differences were the clear-loop
pointer addressing/setup and register allocation; no generated instruction
patches or compiler-flag changes. The clear pointer starts one-past the
12-byte array, stores at clear[-1], and ends at the array start. A closer
postdecrement variant was rejected because it formed a pointer before the
host array. Do not restore it merely to improve a matching count.
Both isolated match linkers now place .actor_range at80084750, with external
89C08 at its retail address. This is not full-overlay linking or dispatch.

### Pointer-width cursor refinement

Current source uses a `uintptr_t` numeric cursor starting at the last list
byte. It converts only the twelve valid store addresses to pointers; the
final unused decrement operates on the integer. This follows the repository's
PSX32/Linux LP64 address model, not a claim about exotic pointer architectures.
The native translation unit explicitly includes stdint.h; the matching build
uses the existing types.h target typedef. The fill value and loop index are
initialized before the address cursor, reproducing retail setup more closely.

Earlier `WvPoDj`: FAIL55/65, correct104-byte size. All ten mismatches were a swap
of s1/s2 allocation, including corresponding saved-slot stores. No forced
registers, compiler flags, assembly patching, or before-array pointer retained.

`HydPdW` (supersedes OHX7qV): O0/O2/UBSan/PIE plus independently compiled expanded O2 and PIE
baselines pass the existing49152 retail range and10240 native/retail actor
cases each. The PIE fixture asserts its actual list address exceeds4GiB.
A cursor-narrowing mutant must produce SIGSEGV at exactly the truncated
address of the last list byte, while armed around the native call. The Linux
x86-64 control verifies REG_RIP is the pinned clear-loop instruction and
REG_ERR denotes a write, not an instruction fetch. Both disassembly and live
code-byte checks pin this test-only instruction at offset20 (67 C6 00 FF).
Only that fault yields exit79 and its expected marker; other faults return80.
Other faults or a surviving mutant fail the runner. All five existing
output mutants remain rejected. This closes the low-address-only fixture gap
identified by independent review; it does not establish general guest aliasing
or runtime adoption.

### Increment placement refinement

Current source increments the local candidate index after the accepted-target
block. The index is not exposed to either callee. `LfBw4O` matches62/65 words
with the correct104-byte size and saved-register allocation. The three
remaining differences are at offsets084,0CC,0D0: retail increments the index
in the eligibility branch delay slot; the compiled body puts it in the loop
backedge delay slot and reloads the actor argument inside the loop instead.
Exact matching remains FAIL, with no compiler/generated-instruction edits.

`aGBC1F` revalidates O0/O2/UBSan/PIE and expanded O2/PIE baselines, each49152
retail84548 cases plus10240 retail/native84750 cases. All five output mutants
and the instruction/address/write-checked cursor-narrowing control reject.
The existing native-ordering, guest-alias, and gameplay limitations remain.
