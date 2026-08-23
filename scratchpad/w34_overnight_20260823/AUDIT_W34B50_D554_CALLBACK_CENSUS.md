# W34B50 — live D554 callback branch census

## Frontier and scope

The control frontier remains `0x800719C8`, with the bounded diagnostic route
executing three clean frame-tail passes. This slice is read-only evidence
work; it does not clear D554, enable the legacy driver, or alter a tripwire.

## Retail predicates under test

- `0x8008A72C` (slot 1, `cb1`) enters its state `0/1` path only when the
  resync byte at `0x8006F8E5` is zero. It calls `0x80090A84`; only helper
  class 1 (`result == 1`, `jt2 == 0`) stores `D554=0` and `D7CC=0`.
- `0x8008C844` (slot 4, `cb1`) reaches `0x80090C68` only when the same
  resync byte is exactly one. Its `result == 1` area-mismatch branch stores
  `D554=0` and `D7CC=0`.

These are the only two live current-port callback bodies identified by the
W34B46 writer census as direct D554 clear candidates.

## Natural branch observations

The debugger captured both callback entries on each of the three bounded
re-entries. Every observation was:

```text
A72C: slot=1, resync=0, wm_80090A84 result=0
C844: slot=4, resync=0; wm_80090C68 clear-capable path not entered
```

Therefore A72C took neither its class-1 clear lane nor a resync lane, and
C844 was gated before its helper. The source-line breakpoints on the switch
arms were zero-hit because the host compiler coalesced their line table;
direct helper return breakpoints supplied the result evidence. The callback
entry and helper records are banked in `slice_23_d554_callback_census.log`.

At each tail the production log still reported `D554=1`, `held=1`:

```text
0x800719C8 ... d554=0x00000001 held=1 hit=1
0x800719C8 ... d554=0x00000001 held=1 hit=2
0x800719C8 ... d554=0x00000001 held=1 hit=3
```

The debugger's tail breakpoint prints D554 before the tail reload; the
production tail record is the authoritative post-reload value.

## Classification and decision

This census does not expose a bounded class-(a/b) implementation. It rules
out the two live callback predicates as the cause of the persistent D554=1
in the natural fixture, but it does not justify forcing either callback or
implementing the 183-instruction frame-local clear region. The unresolved
frame-local region and thirteen external retail clear sites remain
`BLOCKED-NEEDS-REVIEW` class (e), as documented by W34B46.

The next implementation decision requires review of the full D554 state
machine and its unresolved callees. The bounded frame re-entry remains a
diagnostic guard, not a milestone claim.

## Natural result

- rc `0`; LINK unchanged and production source unchanged.
- Scheduler entry `4`; `53/53` executed, missing `0`, invalid `0`.
- Three tail hits; D554 remains `1` at all three tails.
- Mode loop `0x80072238`, renderer `0x8007299C`, and world DrawOTag remain
  zero-hit naturally.
- No framebuffer PNG; Milestones 1, 2, and 3 remain unreached.
