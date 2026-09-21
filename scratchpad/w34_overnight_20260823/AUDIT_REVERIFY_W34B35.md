# W34B35 sibling audit and natural proof

## Retail confirmation

The fresh decode from the W34B34 audit was independently checked against
`WM_80075104_LISTING.txt` and the direct Capstone extraction:
`[0x80075104,0x80075228)`, 292 bytes / 73 instructions, one
`LoadImage` call at `0x800751E4`, signed count gate at `0x80075128`, and
record-array root `D7D0` selected by count `CD64`.

The sibling shares the exact 12-byte record timer/index state machine:
timer `+0xA` decrements with u16 wrap, zero triggers index `+0x8` advance,
the signed-indexed halfword table at descriptor `+0x0C` refreshes the timer,
and a negative refresh resets index to zero and reloads table entry zero.

It is not an alias of W34B34’s transfer formula. Retail reads descriptor
halfwords `+6` and `+4`, multiplies them as signed 32-bit values, reads the
signed indexed table factor, doubles the area, multiplies by that factor,
and adds the resulting wrapped u32 offset to record field `+0` before
`LoadImage`.

## Implementation and boundary

Implemented `world_map_upload_pump_75104.c` as a separate sibling family.
All record, descriptor, table, RECT, source, and final data pointers use the
same known-PSX/KSEG1 map-or-log rule as W34B34. The production frame driver
now calls it at retail `0x8007198C` and stops before the immediate
`SetGeomOffset` call at `0x80071994`.

The focused certificate covers signed count gates, timer nonzero/zero paths,
asymmetric 12-byte records, signed index wrap, dimension/factor scaling,
known mapped LoadImage arguments, and unknown source handling. Five executed
mutants were all detected: wrong stride, wrong trigger, unsigned index,
missing factor scale-by-two, and missing unknown guard.

## Natural proof

The re-anchored GDB diagnostic completed with `rc=0` at frame 916:

```text
upload-pump entry count=2 array=0x800f2c58
upload-pump exit transfers=2 unknowns=0
upload-pump-b entry count=3 array=0x800f2c78
upload-pump-b exit transfers=3 unknowns=0
fp_cut_pc=0x80071994
sched_entry=2 sched_callbacks_executed=29 sched_missing_hits=0
placeholder entered cleanly
```

The natural frontier advanced from `0x80075104` to `0x80071994`, exactly
before `SetGeomOffset`. The D554/backedge tail was not entered.
