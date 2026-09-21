# W34N15 — retail paired world-pool frees

## Result

`PASS`. Three absent retail teardown leaves are now shared production
functions:

- `wm_80086124`: frees `D_8009D7EC`, then `D_8009D7E8`;
- `wm_800866C8`: frees `D_8009D7FC`, then `D_8009D7F8`;
- `wm_80089128`: frees `D_8009BE1C`, then `D_8009BE20`.

The already-transcribed base-session teardown now calls these exact leaves
instead of carrying three private inlined copies.  This makes the same retail
bodies reusable by the still-absent menu-entry lifecycle at `0x800758C0`.

## Anchor

- Starting HEAD: `a0915989339c4f564bb4d38a1ac88bcf409bc801`
- Branch: `experiment/worldmap-open-gates-20260823`
- Local and origin matched before the change.
- Retail image: `disc/world_map.bin`, load address `0x8006FAF0`.
- Whole-image SHA-256:
  `4c15fd32b3a03d7cd5ea4403dcaabc70abaf99b6aaca65d63d6866803edaac70`.

Retail slices, each 0x38 bytes / 14 instructions:

| Function | File offset | Slice SHA-256 |
|---|---:|---|
| `0x80086124..0x8008615C` | `0x16634` | `261e718bf13923cbb71d4c2941da2d403383b85751a254e7411f778aa8c3e207` |
| `0x800866C8..0x80086700` | `0x16BD8` | `8946a60aea0b5e5d4c349592835c8d75dc3cc080832884c051f0aada8c0ac061` |
| `0x80089128..0x80089160` | `0x19638` | `66d3f7cde11d67c9a569b3bb514dd14121dcd19ba196a667e2155b3210a973b9` |

## Port pointer-domain rule

The port's world allocators publish heap pointers in guest KSEG form.  Each
leaf therefore loads the retail global word, maps `0x80000000..0x801FFFFF`
through `PSX_ADDR`, and passes the resulting host pointer to `HeapFree`.
The established low-native compatibility case remains for allocations owned
by older compiled system translation units.  As in retail, both values are
forwarded unconditionally, including null.

## Certificate

`pc_port/tests/run_w34n15_86124.sh`:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict focused warnings: clean
- M1/M2: swapped or duplicated `86124` globals detected by
  `86124.frees.D7EC.then.D7E8`
- M3/M4: swapped or duplicated `866C8` globals detected by
  `866C8.frees.D7FC.then.D7F8`
- M5/M6: swapped or duplicated `89128` globals detected by
  `89128.frees.BE1C.then.BE20`

The W34N7 natural slot-2 teardown certificate also passes O0/O2/UBSan and
M1-M6 after integration.  The normal PC port reports `LINK OK`.

## Dormant-route neutrality

The accepted detached route reached its bounded 120-frame exit and fulfilled
both capture requests:

- frame 60:
  `26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3`
- frame 120:
  `ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc`

Both match the standing baseline byte-for-byte.  After the bounded world loop
returned, the port's outer test process began another boot cycle; the external
watchdog terminated that later re-entry.  The termination occurred after the
two captures and the logged `bounded exit frames=120 limit=120` and is not a
failure of the certified route.

## Boundary and next target

This rung does not activate `0x800758C0` or `0x80075B58`; both remain frame-
driver stubs.  Static dependency closure showed that activating the return
half alone would reallocate shared buffers without running the retail menu-
entry lifecycle.  The next bounded prerequisite is `0x80071FEC`, the 41-
instruction archive-buffer allocator/request helper called by `0x800758C0`.
