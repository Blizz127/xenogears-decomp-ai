# W34B54 — actual DrawOTag ABI boundary audit

## Requirement under audit

The W34B37 brief requires the actual PsyCross DrawOTag call to fire cleanly
after the BE3C+0x70 writer correction, unless the representation is an
architectural class-(b) boundary. This audit resolves that distinction from
the current sources and the banked natural crash.

## Retail representation

Retail 0x8007369C allocates two 0x1000-byte ordering tables. Each table has
0x400 four-byte entries. ClearOTagR writes reverse links as guest addresses
masked to 24 bits; the tail calls DrawOTag(root + 0xFFC). World packet tags
use the same 32-bit layout: high byte is payload length and low 24 bits are
guest links.

The W34B37 writer correction publishes those allocations as guest KUSEG
values in BE3C+0x70. The later W34B39 slot-table correction and W34B38 guest
adapter proof show that the guest OT itself is valid.

## Current PsyCross ABI

The production build compiles PsyCross and the port with
USE_EXTENDED_PRIM_POINTERS=0. In that mode:

- P_TAG remains the retail byte layout with a 24-bit address field;
- OT_TAG is padded to 8 bytes for the host u_long stride;
- PsyCross DrawOTag/ParsePrimitivesLinkedList follows OT_TAG links through
  nextPrim, treating the 24-bit field as a host pointer;
- the 64-bit host pointer to g_PsxRam cannot be represented in that field.

The first direct DrawOTag attempt reached PsyCross with a valid guest OT
mapped to g_PsxRam, then crashed while ParsePrimitivesLinkedList decoded the
guest low-24 link as a host pointer. This is banked in
AUDIT_W34B37_BE3C_OT.md and AUDIT_W34B38_ADAPTER_DESIGN.md.

## Classification

The BE3C+0x70 writer defect was class (a) and is fixed by the committed
W34B37 writer conversion. The remaining requirement to call the actual
PsyCross DrawOTag is class (b), architectural: the host renderer's ordering
table ABI is incompatible with the retail guest representation under the
current 64-bit/simple-primitive build.

The accepted W34B38 adapter is therefore the bounded safe boundary: it walks
guest 24-bit links, maps each packet with PSX_ADDR, calls DrawPrim for each
packet, and calls DrawAllSplits once. It does not pretend that PsyCross
DrawOTag executed. No fake OT, low-address allocation, global PsyCross ABI
change, or second backedge was attempted.

## Stop decision

This is the brief's class-(b) stop condition. Do not implement a one-line
call-site change. Human review must choose one architecture before the
actual DrawOTag milestone can be claimed:

1. extend PsyCross with an explicitly guest-OT-aware DrawOTag path; or
2. formally retain the world-only adapter and redefine the milestone around
   the equivalent renderer traversal.

The current natural route remains clean through the adapter: rc=0, valid
guest OT walk, zero adapter aborts. Actual PsyCross DrawOTag calls remain
zero by design.
