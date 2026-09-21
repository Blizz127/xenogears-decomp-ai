# W34N16 — retail menu archive request helper `0x80071FEC`

## Result

`PASS`. `wm_80071FEC` is now a compiled production function matching retail
`[0x80071FEC,0x80072090)`.  It is a prerequisite of the still-stubbed menu-
entry helper `0x800758C0`; this rung does not activate that larger lifecycle.

## Anchor

- Starting HEAD: `f0ebcf87af614bcaa998f46ed09f75b00732cf8f`
- Branch: `experiment/worldmap-open-gates-20260823`
- Local and origin matched before the change.
- Retail image: `disc/world_map.bin`, loaded at `0x8006FAF0`.
- Whole-image SHA-256:
  `4c15fd32b3a03d7cd5ea4403dcaabc70abaf99b6aaca65d63d6866803edaac70`.
- Slice: file offset `0x24FC`, length `0xA4` (41 instructions), SHA-256
  `49db0531db51dc7f74ac38402f18ec3877ac6bce69ee745a64f33fa06ec8cc90`.

## Exact behavior

The helper executes the retail sequence:

1. decode archive entry `0x26`, allocate its aligned size with flag 1, and
   publish the resulting host pointer to native menu authority `D_8005945C`;
2. decode archive entry `0x25`, allocate with flag 1, and publish its guest
   KSEG address to `0x8009D528` and request entry 0 at `0x8009D3FC`;
3. construct the two-entry, 8-byte-stride guest request list at `0x8009D3F8`:
   `(0x25, second)`, `(0x26, first)`, then a zero terminator;
4. submit `func_80029AFC(PSX_ADDR(0x8009D3F8),0,0)`.

The pointer split is deliberate.  Compiled menu consumers dereference
`D_8005945C` as a native pointer.  The guest request record and world mirrors
must contain guest KSEG values because the archive queue and later world code
rebase them through emulated RAM.

Retail writes only the index halfword and data word in each 8-byte queue
entry.  The focused test seeds and proves that the three padding halfwords are
preserved.

## Certificate

`pc_port/tests/run_w34n16_71fec.sh`:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict focused warnings: clean
- M1 wrong decode order: detected
- M2 wrong second allocation flag: detected
- M3 guest value substituted for native menu authority: detected
- M4 raw native second pointer published into guest state: detected
- M5 request payloads swapped: detected
- M6 terminator omitted: detected
- M7 queue submitted from the wrong base: detected

Every mutant is killed by its named semantic assertion.  The normal PC port
reports `LINK OK`.

## Dormant-route neutrality

The accepted detached route fulfilled frame 60 and frame 120 and logged
`bounded exit frames=120 limit=120`.  Captures remain byte-identical to the
standing baseline:

- frame 60:
  `26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3`
- frame 120:
  `ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc`

## Boundary and next target

`wm_80071FEC` is compiled but not yet called on the accepted route.  With this
helper and W34N15's paired-free leaves available, the next code-producing rung
is the retail menu lifecycle pair `0x800758C0` / `0x80075B58`.  That pair must
be integrated together: `758C0` owns teardown/readback before `MenuMain`, while
`75B58` reloads and rebuilds the world state afterward.  The existing
`0x8008440C` body also needs its harness-only process-wide one-shot restriction
removed because retail reloads `BD20` before calling it again.
