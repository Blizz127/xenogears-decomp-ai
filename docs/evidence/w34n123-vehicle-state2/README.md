# W34N123 — retail state-2 slice of the slot-7 vehicle controller `wm_8008E76C`

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `07ef4352` (worldmap: restore file-backend pointer-domain publishers)
- Retail boundary: state body `[0x8008EB64, 0x8008EE78)`, stop arm
  `[0x8008FAAC, 0x8008FAC4)`, shared tail `[0x80090620, 0x800906B4)`
- Retail state-body slice SHA-256 (0x314 bytes at 0x8008EB64):
  `1665b4445eee1dc34214f089ca4ee1422b12f75de877311495136c8124d4a4ce`
- Verdict: **REPAIRED_VERIFIED_DORMANT_ON_FOOT_ROUTE**

## What this rung lands

`wm_8008E76C` (slot-7 cb1, 2013 retail instructions) dispatches through a
65-entry jump table at `0x80070A50` on the signed slot state `slot+0x20`.
The shipped port body dispatched on the low 13 bits of the control halfword
`0x8006EE68` instead, and its pre-dispatch arms for `slot+0x04` values
1/5/6/7/8 do not exist in retail (retail has only the `== 4` lap arm).

This rung restores the state-2 arm as a bounded unit
(`pc_port/src/world_map_state2_8eb64.c`) and routes the parent to it when
`slot+0x20 == 2`.  Other states still fall through the legacy body and are
documented as unrestored in `world_map_callback_8e76c.h`.

## Divergence repaired relative to the stashed draft

The stashed draft (`w34n123-state2-unverified`) cleared six locations at the
end of both arms.  Retail differs per arm:

- transition arm `0x8008EC34..0x8008EC48`: clears the area byte
  `0x8009BD60`, the boundary byte `0x8009D738` and the trigger halfword
  `0x8009BD04`; the velocity words `slot+0x38/0x3C/0x40` survive.
- walking arm `0x8008EE68..0x8008EC48`: clears the three velocity words and
  the trigger halfword; the area/boundary bytes written by `wm_8008C040`
  survive for the later scheduler slots that read them.

The certificate now asserts each arm's exact clear set
("walking arm clears velocities and trigger only",
"transition arm keeps velocity words").

## Focused certificate

`pc_port/tests/run_w34n123_state2.sh` passes O0, O2, O2 nonrecovering UBSan,
strict `-Wall -Wextra -Wconversion -Wsign-conversion -Werror`, and detects
mutants M1-M10 (exit-code clear, transition state, walkability mode, angle
output address, movement ABI, copy-on-result-one, trigger list two, second
matrix, presence record selection, pose-ring publish states).

## Route neutrality

On the accepted natural on-foot route (Lahan exit 1, `XENO_FIELD_MAP=1`,
`XENO_FIELD_ENTRANCE=0`), the slot-7 initializer `wm_8008E190` reads
`0x8006EE68 & 0x1FFF == 0` and returns scheduler state 3, so slot 7 is
dormant and `wm_8008E76C` never runs:

```text
[worldmap-scheduler] slot=7 state=0 cb=0x8008e190 executed ret=3
```

The on-foot player is slot 1 (`wm_8008A72C`).  The state-2 slice therefore
changes nothing on the on-foot route; it becomes live only for vehicle
variants (`0x8006EE68 & 0x1FFF != 0`).

Normal build: `LINK OK -> pc_port/build_native/xeno-port`.
