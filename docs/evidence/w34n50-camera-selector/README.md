# W34N50 — retail camera candidate selection and FT4 packet phase

## Result

`RESTORED_VERIFIED`

Starting HEAD was `ddb54d5ed7e69b1922541ea290271d95fe5ee3a8`, equal to
`origin/experiment/worldmap-open-gates-20260823`.

This rung restores the naturally reached retail camera selector
`wm_80091FF8`, completes the coupled state behavior in its caller
`wm_80091C18`, and corrects a pre-existing packet-initializer phase error in
`wm_800865A0` that the newly moving camera exposed.

## Retail authority

The authoritative image is `disc/world_map.bin`, decoded as raw little-endian
MIPS R3000 code at overlay base `0x8006FAF0`:

```text
mips-linux-gnu-objdump -Dz -b binary -m mips:3000 -EL \
  --adjust-vma=0x8006faf0 --start-address=0x80091ff8 \
  --stop-address=0x80092234 disc/world_map.bin

mips-linux-gnu-objdump -Dz -b binary -m mips:3000 -EL \
  --adjust-vma=0x8006faf0 --start-address=0x80091c18 \
  --stop-address=0x80091ff8 disc/world_map.bin

mips-linux-gnu-objdump -Dz -b binary -m mips:3000 -EL \
  --adjust-vma=0x8006faf0 --start-address=0x800865a0 \
  --stop-address=0x800866c8 disc/world_map.bin
```

Line-by-line working listings are:

- `scratchpad/w34n50_91ff8_full.objdump`
- `scratchpad/w34n50_91c18_full.objdump`
- `scratchpad/w34n50_865a0_full.objdump`

## Restored behavior

### `wm_80091FF8`

The old body explicitly simplified the retail candidate loop.  The restored
body now performs all three candidate passes, builds each candidate matrix
through `wm_80096F18`, applies the camera-world X/Z bases, samples the complete
6x6 terrain neighborhood, preserves retail's zero-floor minimum, compares the
candidate-specific height threshold, and applies the final proximity snap to
the requested candidate count.

### `wm_80091C18`

The full retail caller comparison found four additional drifts:

- subcommand 14 clears `D_8009D144`, not camera height `D_8009D3F0`;
- positive zoom convergence uses `0x8000` when the difference exceeds
  `0x40000`, otherwise `difference >> 3`;
- positive heading convergence uses fixed step `0x0F32` for candidate zero and
  `0x0799` otherwise, while the negative arm retains its arithmetic step;
- state 2 updates its angle fields and does not rebuild the camera matrix.

### `wm_800865A0`

The crash discriminator exposed a separate older transcription error.  Retail
keeps `s0 = packet + 14`, but its header stores use `-11..-7(s0)` and therefore
land at packet offsets `+3..+7`; `SetSemiTrans` receives the packet start in
`s1`.  The port instead added 14 and then wrote positive `+3..+7`, shifting the
length/RGB/opcode header into offsets `+17..+21`.

The corrected 288-record initializer writes:

```text
packet + 3       DMA length 9
packet + 4..6    RGB 0x26/0x26/0x26
packet + 7       POLY_FT4 code 0x2C, then semitrans bit -> 0x2E
packet + 14      CLUT
packet + 22      TPage
```

The whole 0x2D00-byte initialized pool is then copied to the second buffer as
retail does.

## Natural failure localization

With the exact selector/caller but the old packet phase, plain native execution
reproducibly crashed at displayed frame 68.  A post-init GDB attachment caught:

```text
wm_ot_draw_otag_guest
  cur       = 0x801C91B4
  tag       = 0x091C918C
  len       = 9
  word1     = 0xAEBBDE02
  opcode    = 0xAE (unknown)
ParsePrimitive -> LoadImage -> GR_CopyVRAM
  rect      = (0,-19,28720,32659)
```

A hardware watchpoint proved `wm_86798_publish` linked that packet naturally.
The tag was written, but `word1` retained the exact pre-allocation overlay byte
pattern.  Thus this was not a camera crash, OT phase mismatch, adapter decode
error, or GPU synchronization fault: the FT4 opcode had never been initialized
at the address consumed by `wm_80086798`.

This supersedes the implicit W34N47 assumption that a parseable first natural
packet proved the entire `D7F8/D7FC` pool header phase.  That first packet was
parseable only because residual byte 7 happened to be a supported opcode; the
new 288-record certificate proves the actual retail phase.

## Focused certificates

```text
W34N50 0x80091FF8
  O0/O2/nonrecovering UBSan PASS; strict warnings clean
  M1-M5 DETECTED

W34N50 0x80091C18
  O0/O2/nonrecovering UBSan PASS; strict warnings clean
  M6-M9 DETECTED

W34N50 0x800865A0 packet phase
  O0/O2/nonrecovering UBSan PASS; strict warnings clean
  M1 shifted-header phase DETECTED
```

Runners:

- `pc_port/tests/run_w34n50_91ff8.sh`
- `pc_port/tests/run_w34n50_91c18.sh`
- `pc_port/tests/run_w34n50_865a0_packet_phase.sh`

The normal PC port also links successfully with port-owned address verification.

## Natural acceptance

Two independent detached runs of the final normal build used the accepted
route with:

```text
XENO_TEST_INPUT=0:0x2000,600:0x4000,916:0x2000,976:0
XENO_WORLD_TEST_INPUT=0:0x2000,600:0x20,601:0
```

Both runs reached:

```text
natural state exit frames=601 D7CC=0
capture request/fulfillment at frames 60, 120, and 600
adapter aborts: 0/0/0/0
upload unknowns: 0
```

The two runs produced identical hashes:

```text
frame 60   024259b1fa1b0b8b06a626a38b4cfee8bf6b7cdccddc7166a52c11c72b580f34
frame 120  781d49b8d5d6e1435766e650b8cc8134b4fe6724c6b8967cb2dc86f690091a02
frame 600  7115c84be8e1f990870ab14342690bfb85684acabc89deed7708e92c0867719a
```

Primary captures are in `scratchpad/w34n50_capture/`; the repeat is in
`scratchpad/w34n50_capture_repeat/`.

These hashes intentionally retire the W34N49 hashes.  The camera now changes
candidate and view state, so visual neutrality would have been evidence that
the restoration was ineffective.  Inspection shows the world surface and
minimap remain coherent while the view evolves.  The garbled status-text strip
visible on later frames predates W34N50 and remains a separate rendering/UI
defect; it is not classified as fixed here.

After the accepted natural state-exit marker, each process reached the known
`SoundHandleError` stub and was terminated.  No whole-process `rc=0` is claimed.

All temporary `XENO_DIAG_W34N50` instrumentation was removed before the final
normal build.
