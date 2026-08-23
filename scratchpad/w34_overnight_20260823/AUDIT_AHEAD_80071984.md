# Audit ahead — continuation after `0x80071984`

## Region

`[0x80071984, 0x800719CC)` is 72 bytes / 18 instructions. It calls
`0x80074F2C`, `0x80075104`, `SetGeomOffset(0xA0, D_8009BE0C)`, and
`DrawOTag(*(BE3C+0x70)+0xFFC)`, then tests `D554` and branches back to
`0x8007130C` when nonzero. There is no loop inside the region itself, but
the backedge is a new frame/prologue pass and must not be crossed while the
callback inventory is unresolved.

## State and host boundaries

Touched state is `D_8009BE0C`, `BE3C`, `D554`, and the OT pointer at
`BE3C+0x70`. The `DrawOTag` argument is a guest pointer and needs the same
known-value map/log treatment as the `0x80025044` image list. The natural
fixture has `D554=1` from the frame head, so the retail branch would take
the backedge after the calls.

## Calls and classification

| PC | Target | Status |
|---|---|---|
| `0x80071984` | `0x80074F2C` | absent overlay helper; 63 instructions and a `LoadImage` call |
| `0x8007198C` | `0x80075104` | absent overlay helper; about 72 instructions and a `LoadImage` call |
| `0x8007199C` | `0x8004A12C` | host `SetGeomOffset`, bounded API shim available |
| `0x800719B4` | `0x80044BD0` | host `DrawOTag`, guest OT pointer requires validation/mapping |

Classification is class (e): unresolved callees exceed the leaf allowance,
and the unresolved frame backedge would create another scheduler pass.
Do not implement this region in the overnight branch.
