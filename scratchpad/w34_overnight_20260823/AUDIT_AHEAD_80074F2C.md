# Audit ahead — overlay helper `0x80074F2C`

## Region and behavior

Retail `0x80074F2C` is `[0x80074F2C, 0x8007502C)`, 64 instructions / 256
bytes. It decrements a per-entry halfword timer, advances a signed frame
index through a table, and calls `LoadImage` when an entry reaches its
transfer point. The helper loops over the count at `0x8009CC9C` and uses
the table rooted at `0x8009D780`; it reads/writes entry halfwords and the
image source base at `0x8009D780[0]`.

## Calls and boundaries

The only direct call is `0x80074FEC -> 0x80044894` (`LoadImage`). The
rectangle and image data are derived from guest table pointers and require
PSX address mapping before reaching the host API. The loop count is a
runtime global and may be zero, but the natural route has not yet captured
the helper's state because the call site is the current block.

No production-linked precise `wm_80074F2C` implementation exists. The old
`world_map_frame_driver_712d0.c` stub is not coverage. The helper is larger
than a leaf and its pointer-producing data path is host-boundary sensitive.

Classification: class (e), not an overnight implementation. Morning review
must approve the table layout, signed timer/index semantics, and mapped
LoadImage path before this helper is crossed.
