# W34N56 — retail mode-8/11 scheduler callbacks

## Result

`REPAIRED_VERIFIED`.  The port now contains the two callbacks registered by
retail session setup `0x80077214` for world modes 8 and 11:

- `wm_8007756C`, retail `[0x8007756C,0x800776E0)`;
- `wm_800776E0`, retail `[0x800776E0,0x80077954)`.

The bounded scheduler resolves guest callback words `0x8007756C` and
`0x800776E0` to those native bodies.  This rung intentionally does not yet
integrate the owning mode-table setup/teardown pair (`0x80077214` /
`0x80077480`); that is the next code-producing unit.

## Retail authority

- image: `disc/world_map.bin`
- SHA-256: `4c15fd32b3a03d7cd5ea4403dcaabc70abaf99b6aaca65d63d6866803edaac70`
- load address: `0x8006FAF0`
- decoded listing used for the callback pair:
  `scratchpad/w34n56_mode8_callbacks.objdump`

The callbacks are passed to `wm_80097718` by retail setup sites
`0x800773EC..0x80077400`.  They are not inferred from the existing port.

## Restored behavior

`0x8007756C` initializes the camera slot and shared state:

- slot stride `0x80`, target `(-0x80000, 0x200000)`;
- angles `(-128, 512, 0)`, height `0x00400000`, geometry Y `120`;
- camera vector `C5AC/B0/B4 -> BE28/2C/30`;
- exact `wm_80096F18(BD40, BE28, D3F0, BD38)` call;
- retail eight-byte matrix-prefix exchange via guest scratch `0x1F8000A0`.

`0x800776E0` restores the recurring update:

- released bit `0x40` clears `D554` and `D7CC`;
- held-input masks update the two camera targets;
- the asymmetric retail X limit and one-eighth smoothing are preserved;
- smoothed values drive `BD38/BD3A`, followed by the same matrix exchange;
- `BD3A` receives the `+0x800 & 0x0FFF` half turn;
- `SetGeomScreen(*(u32*)0x8009BCDC)` runs last.

## Certificate

`pc_port/tests/run_w34n56_mode811_callbacks.sh` passed:

- O0;
- O2;
- nonrecovering UBSan;
- strict warnings;
- direct state/write-set assertions;
- scheduler guest-address resolution for cb0 then cb1;
- M1 wrong slot stride detected by `init.slot_target`;
- M2 missing matrix exchange detected by `init.matrix_block_swap`;
- M3 missing smoothing detected by `update.eighth_smoothing`;
- M4 wrong lower limit detected by `update.lower_limit_inclusive`;
- M5 missing half turn detected by `update.half_turn_wrap`.

The normal port build completed with `LINK OK` (`scratchpad/w34n56_build.log`).

## Base-route neutrality

The accepted detached base route ran to its natural session exit at displayed
frame 601 with `D7CC=0`; both upload pumps retained `unknowns=0`.  Because the
new callbacks are not registered by the base-mode table, exact neutrality was
required and obtained:

| frame | W34N56 SHA-256 | accepted W34N54 SHA-256 |
|---|---|---|
| 60 | `15fdbff279b0bb5a83d0e049ba6bce0ca3397a94f6239bdea51424a23b430d04` | same |
| 120 | `50512b12deef85444cc69c92b924fc88021261f46146887e95a553977ce009c5` | same |
| 600 | `894f9da798e4cdb01e093834fda64ea49a8730a068e5a3827aaea507a0d15c93` | same |

Artifacts:

- `scratchpad/w34n56_run.log`
- `scratchpad/w34n56_capture/`

The later field-side `SoundHandleError` remains the already bounded W34N55
post-world event and is not attributed to these callbacks.
