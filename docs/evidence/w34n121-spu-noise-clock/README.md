# W34N121 — retail `SpuSetNoiseClock`

## Result

`RETAIL_SPU_NOISE_CLOCK_RESTORED`.

Starting HEAD was `2ccb2359af23957f26a06d0beebdbd6f3ec962cc` on
`experiment/worldmap-open-gates-20260823`, equal to origin.  The W34N119
route-wide census showed that modes 15 and 16 each naturally reached the
generated `SpuSetNoiseClock` stub.

Retail authority is `SpuSetNoiseClock = 0x8004D364` in
`config/symbol_addrs.slus_006.64.txt`, the exact body in
`asm/slus_006.64/matchings/psyq/libspu/SpuSetNoiseClock/SpuSetNoiseClock.s`,
and its matching C transcription in
`src/slus_006.64/psyq/libspu/SpuSetNoiseClock.c`.

## Implementation

The port now performs the complete retail operation:

- clamp the signed input to `[0, 63]`;
- preserve SPUCNT bits selected by `0xC0FF`;
- replace SPUCNT bits 13 through 8 with the six-bit noise clock;
- store at the retail SPU register offset `0x1AA`;
- return the clamped value.

The destination is `g_pSoundSpuRegisters`, the native backing page already
owned by the port's real sound translation unit.  Loads and stores use
`memcpy`, avoiding host alignment undefined behavior without changing the
two-byte register layout.

This restores the game's SPU register-state semantics.  PsyCross does not yet
synthesize the PlayStation noise source, so the rung does not claim audible
noise parity; that is a separate backend capability, not a reason to leave the
retail SDK call stubbed.

## Certificate and build

`pc_port/tests/run_w34n121_spu_noise_clock.sh` passes under O0, O2, and
nonrecovering UBSan with strict warnings.  It covers negative and high clamps,
both legal boundaries, exact SPUCNT placement, preservation of unrelated bits,
and the authorized two-byte write set.  M1-M6 are detected by named assertions:
missing low clamp, missing high clamp, wrong preserve mask, wrong shift, wrong
register offset, and returning the unclamped input.

An isolated staged-tree build at anonymous verification commit
`e7723466244abb366f5212eea018ca4844eaa5cf` ended with `LINK OK`.  Its generated
stub manifest contains no `SpuSetNoiseClock`; the final executable resolves the
symbol to the new port-owned body.

## Natural modes 15/16 acceptance

Pre- and post-fix binaries were run through the identical detached 120-frame
route.  The pre-fix logs each contain one `[stub] SpuSetNoiseClock`; the post-fix
logs contain none.  Both post-fix runs reached their bounded exits with no
world-map callback stub, no OT-adapter abort, and no setup/runtime error.  The
known field-bootstrap `func_80028B14` remains the sole generic stub line before
world-map initialization.

The repair is presentation-bit-neutral in both naturally affected modes:

| mode | frame | pre/post SHA-256 |
|---:|---:|---|
| 15 | 60 | `731fbfdb250dc2728d74b9a9d9bf114e2b2a119e43409f15060aacbbbc6751cd` |
| 15 | 120 | `e5ee91b40048e9bef67bcbd33e6d74ed254869de6b10f22ce4da3d9882cef84c` |
| 16 | 60 | `02270d073614fcdc4c5cc610f898ee2179f853c762375b5e0427faf7285e61ec` |
| 16 | 120 | `1bc0ae6f8b0378b66e053dfffc22e913433ddbcb3f6afa2022fe686a6efcb159` |

Each corresponding pre/post BMP pair compares byte-for-byte identical.  Mode
15's late particle/transition presentation is therefore existing mode behavior,
not a side effect of the SPU change.  Mode 16's captures remain coherent ocean
and coastline views.

Artifacts remain under `scratchpad/w34n121_mode15_capture/`,
`scratchpad/w34n121_mode16_capture/`, and their `w34n121_pre_*` controls.

## Next target

Repeat the route-wide stub census on the exact post-W34N121 binary.  Do not
promote source-local placeholder functions merely because they exist; select
the next implementation only from a natural world-route witness.
