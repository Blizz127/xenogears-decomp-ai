# Image ownership: address 0x80093740

## Search result

The repository had no world-overlay implementation named `wm_80093740` and no
world source body matching the decoded function.  The generated native symbol
inventory contained `func_80093740`, but that symbol is owned by
`src/field/main/misc11.c` and is referenced by the field VM handler table in
`pc_port/src/data_field.c`.

The field body sets `D_800B00C0`, clears `D_800ADB64`, copies `D_800B236C` to
`D_80059171`, increments `D_8004F350`, and advances a field-script instruction
pointer.  It has no terrain inputs, scratchpad vectors, geometry calls, or
relationship to `disc/world_map.bin`.

The world image at the same virtual address begins with a 0x28-byte stack
frame, calls `0x80093660`, constructs two scratchpad vectors, calls
`OuterProduct0` and `VectorNormal`, and returns at `0x80093970`.  Overlays reuse
the virtual address at different times; the names are not interchangeable.

## Verdict

`func_80093740` is a field-overlay collision and remains untouched.
No existing WORLD implementation was found.  The new identity is explicitly
`wm_80093740` in a world terrain module.
