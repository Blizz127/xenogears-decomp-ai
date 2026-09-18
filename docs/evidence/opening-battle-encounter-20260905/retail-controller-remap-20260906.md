# Retail controller remap audit — 2026-09-06

## Result

The retail path intentionally remaps the first four face buttons before menus
read them.  The captured `pad-diagnosis-20260906/actions.jsonl` values are
little-endian byte dumps: `state:"2000"` means the `u16` game state
`0x0020`, and `state:"4000"` means `0x0040`.

The observed retail table is:

```
g_ControllerButtonMasks    @ 0x800501E8: 20 00 40 00 10 00 80 00 04 00 01 00 08 00 02 00
g_ControllerButtonMappings @ 0x80050238: 01 00 03 02 04 05 06 07
```

`ControllerRemapButtonState` tests the physical mask at index `i` and ORs the
mask at `g_ControllerButtonMappings[i]`.  Therefore the actual digital-pad
mapping is:

| physical packet/button | raw mask | retail game state |
| --- | ---: | ---: |
| Circle | `0x0020` | `0x0040` (Cross) |
| Cross | `0x0040` | `0x0020` (Circle) |
| Triangle | `0x0010` | `0x0080` (Square) |
| Square | `0x0080` | `0x0010` (Triangle) |
| L1/L2/R1/R2 | `0x0004/1/8/2` | unchanged |

D-pad and select/start/L3/R3 bits are passed through before this loop.  The
captured packets prove the two relevant cases directly:

* physical Cross: packet `0041ffbf...` has raw low-button mask `0x0040`, and
  the snapshot is `state:"2000"` = game `0x0020`;
* physical Circle: packet `0041ffdf...` has raw low-button mask `0x0020`, and
  the snapshot is `state:"4000"` = game `0x0040`.

Thus the prior “Circle confirms” interpretation missed
`ControllerRemapButtonState`: on this retail path physical Cross produces the
logical Circle bit tested by `KernelMenuUpdate`, while physical Circle produces
the logical Cross bit.  This is an input-state interpretation issue, not proof
of an intentional Circle-confirm retail path.

## Source and retail bytes

The source algorithm is `src/slus_006.64/system/controller.c:48-64`, called for
controller 1 by `ControllerPoll` at lines 103-106.  The retail symbols pin the
same functions to `ControllerRemapButtonState=0x800357C0`,
`ControllerPoll=0x800358BC`, and the tables to `0x800501E8`/`0x80050238`.

For `disc/SLUS_006.64` (SHA-256
`dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119`), using
the executable file mapping `file_offset = address - 0x8000F800`, the exact
retail slice `0x800357C0..0x8003582C` is 108 bytes, SHA-256
`683767715535a666ba6993ab9bf1f54a704ef33e8c781a1a4eaf04af5f76ba93`, and is:

```
2130800000ff8430211800000580053ce801a5242138a0000000a294000000002410c2000a004010000000000580013c21082300380222900000000040100200211047000000429400000000252082000100632408006228efff40140200a524001404000800e00303140200
```

The disassembly is the expected loop: preserve the high/pass-through bits,
load each 16-bit mask from `0x800501E8`, test the input, load one byte from
`0x80050238 + i`, scale it by two, fetch the destination mask, OR it, and
return the signed 16-bit result.

## Mapping initialization

`ControllerInit` is pinned at `0x80036288`; no C implementation is currently
present in `src/slus_006.64/system/controller.c`.  The retail bytes show the
initializer unambiguously.  It first fills `0x80050238..0x8005023F` with the
identity sequence `00 01 02 03 04 05 06 07`, then overwrites:

```
0x80050238 = 1
0x80050239 = 0
0x8005023A = 3
0x8005023B = 2
```

The exact retail initializer body `0x800362C8..0x8003632C` is 100 bytes,
SHA-256 `551d67b36a763a1b6ae64f2b55a9703fbaa6db375e148383013e93643c13e1c8`:

```
070003340580043c3f028424010002340580013c040220ac0680013c8c9322a00680013c909320ac000083a0ffff6324fdff6104ffff8424010002340580013c380222a0030002340580013c3a0222a0020002340580013c390220a00580013c3b0222a0
```

The corresponding instructions are `li v1,7; lui/addiu a0,0x8005023f;`
descending byte stores through `0x80050238`, followed by the four explicit
stores above.  This is enough to explain why the runtime snapshot differs from
the identity initializer comments in `include/system/controller.h:172-178`.

The current native override in `pc_port/src/game_overrides.c:125-135` still
declares identity mapping `{0,1,2,3,4,5,6,7}`.  That is a native-vs-retail
control-state mismatch and is relevant to any port-side confirmation probe;
this audit made no production edit.

## Evidence pins and scope

* Action log: `/tmp/xeno-retail-opening-reference-20260906-8mpinaz6/pad-diagnosis-20260906/actions.jsonl`, SHA-256
  `34aad1559de44aa764c7baade67846fa9d126fcd33f9caf5c78c20bf1c98ddd3`.
* Repository HEAD observed: `3a3e7aac03a2f166fb924945a489e392d706f282`.
* The repository was already dirty in `pc_port/src/game_overrides.c`; it was
  not modified by this audit.  No repository, emulator, process, API, or UI
  writes were performed.
