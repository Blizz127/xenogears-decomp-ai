# W34B39 audit — bucket `0x320` first writer

## Frontier and observed writer

W34B38's guest-native OT walk stops at bucket `0x320`:

- OT root: `0x800A2228`
- bad predecessor word: guest address `0x800A2EA8` (`root + 0xC80`)
- observed value: `0x00092EA4`
- expected reverse-clear link: `0x00A2EA4` (guest `0x800A2EA4`)

The W34B39 hardware watchpoint was armed at frame 916, scheduler entry 2,
after the frame driver's guest-native `ClearOTagR` and before scheduler body
execution.  It fired on the first write:

```
Old value = 0x000A2EA4
New value = 0x00092EA4
pc = __memcpy_avx_unaligned_erms
pt_sh(a=0x000A2EAA, v=9)
wm_80089580() at world_map_helper_89580.c:44
wm_80089748() at world_map_helper_89748.c:121
wm_80071A58(slot_idx=14) at world_map_r4world_71a58.c:55
wm_80097800() at world_map_scheduler.c:561
```

The two-byte store is the retail particle-cleanup decrement of
`slot_record + 0x0A`; its target overlaps the OT bucket because the helper
loaded the wrong slot-table base global.

## Retail decode and source comparison

Retail `0x80089580..0x80089748` is 114 instructions, has no direct callees,
and uses these globals:

- `0x8009BDF4`: particle table base (`0x80089580`)
- `0x8009BCC0`: slot table base (`0x800896FC..0x80089708`)

The retail sequence at `0x800896FC` is `lui 0x800A; lw -0x4340`, resolving to
`0x8009BCC0`.  The current source used `pt_lw(D_8009BDF4 - 0x14)`, i.e.
`0x8009BDE0`, despite a comment claiming `D_8009BCC0`.  That is the complete
cause of the first bad OT write.

## Classification and smallest slice

Class **(b)** bounded function/helper: 114 instructions, no unported callees.
Smallest implementation is to load the slot-table base from `0x8009BCC0`.
No OT adapter, renderer, callback routing, or second-frame/backedge change is
needed.  The correction is intentionally isolated in
`world_map_helper_89580.c`.

## Test plan

`run_w34b39_89580_prod_test.sh` links the production helper and executes:

- asymmetric BCC0/BDE0/BDF4 fixtures with the wrong-base target placed on the
  live OT bucket;
- inactive cleanup, active counter/position/UV updates, and canaries;
- O0, O2, and UBSan-O2 byte-identical output;
- three address mutants: wrong nearby global, particle global, and offset
  global, all required to fail.

Natural proof remains the required downstream test: rc=0, scheduler passes
complete, adapter reaches beyond bucket `0x320`, and no unsafe raw guest
pointer call is introduced.
