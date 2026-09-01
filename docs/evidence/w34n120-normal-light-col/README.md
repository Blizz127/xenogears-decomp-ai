# W34N120 — retail `NormalLightCol`

## Result

`RETAIL_NORMAL_LIGHT_COL_RESTORED`.

Starting HEAD was `d04f53459ece09a453ea2b1f00eee2a4ebbb1d06` on
`experiment/worldmap-open-gates-20260823`, equal to origin.  The complete
entrance-record census in W34N119 left one pixel-affecting world-route SDK
stub: mode 12 reached `NormalLightCol` while building its object/matrix table.

Retail authority is `NormalLightCol = 0x8004A260` in
`config/symbol_addrs.slus_006.64.txt` and the six-instruction body in
`asm/slus_006.64/matchings/psyq/libgte/NormalLightCol.s`.  The excluded PsyQ
translation unit `src/slus_006.64/psyq/libgte.c` independently expresses the
same port operation.

## Natural argument-domain witness

A read-only breakpoint at the naturally reached mode-12 call from
`func_8002D530` recorded:

| argument | raw host value | domain | first 8 bytes |
|---|---:|---|---|
| normal | `0x7fffffffca80` | native stack | `3c050000200f0000` |
| input color | `0x628dc0` | native emulated-RAM view | `0000002cc056c048` |
| output color | `0x652d60` | native emulated-RAM view | `eeee0e00a0eeeeee` |

All three arguments are native pointers at this live seam.  The compatibility
function therefore preserves the native PsyCross calling convention and does
not add speculative guest-address rebasing.

The initial probe scripts used the historical GDB expression
`(uintptr_t)g_pGameState`.  The generated-data definition makes that expression
ambiguous to GDB; the robust witness reads the pointer value from
`&g_pGameState` and writes both the host and guest GameState entrance copies.
This is probe-only handling, not a production change.

## Implementation and certificate

`pc_port/src/psyq_normal_light_col.c` implements the retail sequence exactly:

1. load the two normal words into GTE data registers 0 and 1;
2. load the input color word into RGBC register 6;
3. execute `nccs` (`0x0108041B`);
4. read RGB2 register 22 and store the result.

The loads/stores use `memcpy`, preserving the retail byte layout while avoiding
host alignment undefined behavior.  No other GTE or renderer semantics changed.

`pc_port/tests/run_w34n120_normal_light_col.sh` passes under O0, O2, and
nonrecovering UBSan with strict warnings.  M1-M6 are killed by named assertions:
wrong VXY register, missing VZ load, wrong RGBC register, wrong opcode, wrong
output register, and corrupted output value.

An isolated staged-tree build at anonymous verification commit
`34947e96631c1feb4dca7ff965e81cf51e648194` ended with `LINK OK`.  Its generated
stub manifest contains no `NormalLightCol`; `nm` resolves the symbol to the new
port-owned body.

## Natural mode-12 acceptance

The detached mode-12 route completed 120 displayed frames and reached its
bounded exit.  It logged no `NormalLightCol` stub, no world-map callback stub,
no OT-adapter abort, and no setup/runtime error.  The only generic stub line was
the known field-bootstrap `func_80028B14`, before world-map initialization.

| frame | SHA-256 |
|---:|---|
| 60 | `13a52c764dfc39ca118e6f072545f4a3d6675a65a6e48fe1c3b92ca4f87fd8d6` |
| 120 | `78634ad19e9883566355a7499def871bd92bc86f8582a7ad1deb71f08c14199f` |

Both captures are coherent ocean/ship views.  They are byte-identical to the
pre-fix W34N87 captures: this call occurs during object setup and its corrected
output does not alter these two sampled presentation frames.  The acceptance
claim is therefore stub retirement plus runtime neutrality, not a claimed
visual delta.

Artifacts remain under `scratchpad/w34n120_mode12_capture/` and the focused
runtime log is `scratchpad/w34n120_mode12_run.log`.

## Next target

Modes 15 and 16 naturally reach the generated `SpuSetNoiseClock` stub.  Decode
and restore that bounded PsyQ compatibility function next; do not broaden into
the source-local world audio stubs without a separate live witness.
