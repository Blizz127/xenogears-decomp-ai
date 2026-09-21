# W34B37 follow-up audit — PsyCross OT representation

Date: 2026-08-23  
Branch: `integrate/w34b24-i1`  
Frontier under test: `0x80071984 -> 0x800719C8` tail, first natural DrawOTag

## Finding

The W34B37 writer conversion is semantically correct at the guest boundary,
but it exposes a second, architectural boundary in the host GPU shim. Retail
allocates each OT as `0x1000` bytes and indexes it as `0x400` four-byte words.
The retail tail passes `ot + 0xFFC` to DrawOTag. The mapped natural call was:

```text
g_PsxRam     = 0x54ee40
OT root host = 0x5f1068  (g_PsxRam + 0xa2228)
DrawOTag p   = 0x5f2064  (g_PsxRam + 0xa3224) = root + 0xffc
```

The production binary is compiled with `USE_EXTENDED_PRIM_POINTERS=0`.
GDB's debug types report `sizeof(OT_TAG)=8`, `sizeof(P_TAG)=8`, and
`sizeof(unsigned long)=8`. The port's `_xeno_ot_pad` makes each non-extended
OT tag eight bytes so PsyCross's `ClearOTagR` can serve the port's other
host-side `u_long[]` arrays. Its implementation casts the caller's memory to
`OT_TAG*` and writes `ptag_list[i]` for `i=1..0x3ff`, therefore treating this
0x1000-byte retail allocation as a 0x2000-byte host OT. The non-extended
`setaddr` stores the low 24 bits of a host pointer for those cleared links,
while the decompiled world callbacks write retail four-byte tag words and
guest 24-bit packet links.

The target capture shows the mismatch directly. At the root, the host clear
operation is visible at eight-byte intervals, while the retail callback has
overwritten four-byte OT words. At `root+0xFFC`, the words consumed by
`ParsePrimitivesLinkedList` are not an OT tag; they begin with values such as
`0x00431025`, `0x005f2060`, and `0x807ffffe`. PsyCross consequently reports
invalid tag lengths (`99`, `136`) and malformed primitive lengths before the
SIGSEGV in `ParsePrimitivesLinkedList` line 906.

## Retail/current-port comparison

| Property | Retail world map | Current PsyCross path |
| --- | --- | --- |
| OT allocation | `HeapAlloc(0x1000, 0)` | host view of same emulated RAM |
| OT slot stride | 4 bytes | `sizeof(OT_TAG)=8` on x86-64 |
| link encoding | 24-bit guest address in 32-bit tag | low 24 bits of host link for PsyCross-cleared slots |
| clear range | 0x400 words / 0x1000 bytes | 0x400 `OT_TAG`s / 0x2000 bytes |
| DrawOTag input | `root + 0xFFC` | mapped host `root + 0xFFC`, which is not a PsyCross tag boundary |

`world_map_callback_925a0.c` and `world_map_helper_73b04.c` are retail-exact
at the guest data level: they mask and publish guest packet addresses into
four-byte tag words. Converting those stores alone to host pointers would
change the guest representation and would not repair the 8-byte stride or
the `+0xFFC` tail contract. Even after a stride repair, guest packet links
such as `0x0009d310` cannot be dereferenced as the host links expected by
`nextPrim`. Conversely, changing the global PsyCross
primitive ABI is outside the bounded W34B37 writer slice and risks unrelated
GPU paths.

## Classification and decision

The original BC38/BCB0 host-pointer truncation is class (a) and was fixed in
the working tree with an asymmetric O0/O2/UBSan certificate. The newly
exposed DrawOTag failure is a class-(e) host OT ABI/layout boundary: it needs
a reviewed adapter or a PsyCross ABI change, not a guessed local callback
patch. Per W34B37 instructions, do not implement it in this slice, do not
enter the second frame iteration, and do not weaken any tripwire.

## Evidence

- Targeted GDB script: `w34b37_ot_target.gdb`.
- Targeted natural capture: `slice_10_ot_representation.log`.
- Prior writer/census audit and evidence: `AUDIT_W34B37_BE3C_OT.md`,
  `slice_09_census.log`, and `slice_09_natural.log`.
- Natural target state: frame 916; scheduler pass 2 was 29 executed / 0
  missing; both upload pumps completed; DrawOTag entered once; no clean rc=0
  proof after the attempted production changes.

## Morning approval request

Review and approve one of two bounded directions before implementation:

1. a world-map-only host OT adapter that preserves retail four-byte OT memory
   and translates links during a controlled DrawOTag submission; or
2. a PsyCross-wide OT representation repair with regression coverage for all
   users of `ClearOTagR`, `DrawOTag`, `addPrim`, and `termPrim`.

Neither direction is safe to infer from this overnight slice.
