# W34B53 — 0x80094238 BD00 data-plane audit

## Scope

This is a read-only audit of the live wm_80094238 miss observed during the
held 0x800719C8 backedge. It does not alter the second-frame loop, seed
selection, D554, or any tripwire.

## Retail/port mechanism

wm_80094238(pos_vec, list_index) loads the pointer-table root from
0x8009BD00, selects one 32-bit list pointer with list_index << 2, then
scans 16-byte records. The record scan terminates when signed halfword
record + 0x08 equals -1; otherwise it compares the fixed-point X/Z
coordinates against the record rectangle and writes the record/id globals on
hit, or 0xFFFFFFFF/-1/-1 on miss.

The current second-wave fixup computes:

    decompressed base  = 0x800B6674
    0x8009BD00         = 0x800B66FC

The first four relocated list entries at 0x800B66FC are:

    0: 0x800B670C
    1: 0x800B673C
    2: 0x800B674C
    3: 0x800B675C

This is consistent with the retail relative-pointer rewrite in
wm_80073530_fixup; no host pointer or unrelocated relative value is present
at the live root.

## Natural state and data

At the first live helper entry:

    pos_vec = 0x800D75E0
    position = (x=0, y=-16384, z=0)
    list_index = 0

List 0 is populated. Its first two records are:

    record 0: x=30095 z=10342 width=253 height=169, field+8=15, id=2, type=0
    record 1: x=29792 z=10572 width=365 height=354, field+8=1,  id=1, type=0
    terminator: field+8=-1

The live X/Z point (0,0) is outside both rectangles. The helper therefore
misses naturally and stores recptr=0xFFFFFFFF, idA=-1, and idB=-1 on all
three bounded re-entries.

## Classification

Not a fixable writer or relocation defect. This is valid populated trigger
data with a natural position outside list 0. No implementation slice is
justified. Do not seed a selection, alter the position, or force a hit to
make the D554 predicate fire.

## Natural proof

The corrected GDB run exits rc=0 and reports:

- three bounded frame tails, each with D554=1 and held=1;
- scheduler pass 4 ending 53 dispatched / 53 executed / 0 missing;
- wm_80094238 miss 0x800D75E0/index 0 on each tail;
- no mode-loop, renderer, or DrawOTag tripwire firing.

Evidence is in slice_24_94238_data_plane.log; the probe is retained in
w34b50_d554_callback_census.gdb.

## Next boundary

Return to the class-(e) frame-local state machine
0x8007169C..0x80071978. The next productive task is a retail audit of its
unresolved callees and D554 clear predicates; manual D554 clearing and an
unbounded backedge remain prohibited.
