# Audit ahead — overlay helper `0x80075104`

## Region and behavior

Retail `0x80075104` is `[0x80075104, 0x80075228)`, 73 instructions / 292
bytes. It loops over the count at `0x8009CD64`, advances per-entry timers
and signed indices in the table rooted at `0x8009D7D0`, computes a transfer
offset from entry dimensions and the indexed width/height table, then calls
`LoadImage` at `0x800751E4`.

## Calls and state

The direct call is `0x800751E4 -> 0x80044894` (`LoadImage`). The source
buffer comes from the guest pointer at the table head (`0x8009D7D0[0]`),
and the computed destination image pointer is guest data. The helper
touches `CD64`, `D7D0`, entry halfwords at offsets 0/2/4/6, and the indexed
table at each entry's `+0x0C` base. It contains a runtime loop and signed
boundary logic; no precise production port exists.

Classification: class (e). It exceeds the leaf allowance, has an unresolved
host image-transfer boundary, and depends on the same table authority as
`0x80074F2C`. Do not implement overnight.
