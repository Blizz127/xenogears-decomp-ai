# Retail boot audio regression closure — 2026-09-03

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `3a3e7aac03a2f166fb924945a489e392d706f282`
- Verdict: **TITLE_AND_FIELD_SAMPLE_OWNERSHIP_VERIFIED; HUMAN_AUDIO_PENDING**

## Diagnosed regressions

Three retail sound primitives were generated no-op stubs:

- `SoundSpuMemoryAllocateBlock` returned zero, so WDS banks could not reserve
  SPU RAM.
- `SoundTransferWdsPart` discarded streamed field-bank payloads.
- `SoundSetupCdMix` computed the retail matrix but had no production owner.

After those bodies were restored from `disc/SLUS_006.64`, the final call made
by `SoundSetupCdMix` still resolved to a generated `CdMix` stub. The native
boot replacement also omitted the four WDS loads performed by retail
`func_80019578` before title/opening execution. That omission produced title
voice key-ons at SPU address zero.

The first XA implementation also sent decoded 37.8 kHz movie audio directly
to OpenAL. That delegated the PS1 CD decoder's fixed 37.8 -> 44.1 kHz zigzag
interpolation to a host-selected resampler and was not retail-faithful.

## Retail and hardware boundaries

Pinned executable slices:

| owner | retail range | size | SHA-256 |
|---|---|---:|---|
| `SoundTransferWdsPart` | `0x8003827C..0x80038310` | `0x94` | `488e928c9eae2726cd8e7d359aafffc4432066fe811c09420b8003f53c2604d0` |
| `SoundSetupCdMix` | `0x8003885C..0x800388D4` | `0x78` | `045ca7300d7b5076a7c2b9b5c2d9800cd400441d9adffb912e036ed0b814a51e` |
| `SoundSpuMemoryAllocateBlock` | `0x800393B8..0x800394B8` | `0x100` | `cf6569f7fe8bae597637de59da0d4317cc6873b47102b3d9868ec10211c994a3` |
| `CdMix` | `0x8004138C..0x800413AC` | `0x20` | `344bf8a32638701db5bae1222a9e7c13d50b4d54fb0476ae8e9f63674c41c1a9` |
| boot WDS load/publish | `0x80019670..0x80019774` | `0x104` | `1894a377dcc4940bcc83fbd1c926acc94af3ae6b26c4574f1d08579400d2c410` |
| boot WDS drain/free | `0x80019840..0x80019868` | `0x28` | `0f1c263653017d52e9d36479814c20d09a3ccca8a2554f576adf1ca3fa2c2c78` |

`CdMix` is a transparent PsyQ adapter. It submits all four `CdlATV` routes to
the existing XA backend. The backend applies each unsigned 8-bit coefficient
with `sample * coefficient >> 7`, sums the two routes for each output and
saturates to signed 16-bit before the separate SPU common-CD-volume stage.
Defaults are the hardware identity matrix `80 00 00 80`.

Movie XA now uses the documented seven-phase, 29-tap PS1 CD zigzag filter:
every six decoded input frames produce seven 44.1 kHz output frames, with
ADPCM and interpolation history continuous across sectors. OpenAL receives
44.1 kHz PCM and no longer chooses the XA resampling algorithm. `CdlModeSF`
gates file/channel filtering, while the sector gate requires realtime audio
and rejects video/data submodes.

The boot closure selects archive `(0,1)`, allocates and reads files 2..5,
syncs once, calls `SoundLoadWdsFile` on each in order, publishes the second and
fourth returned handles, drains the transfer queue with `func_8003BDFC(0x10)`,
then frees the four source buffers. No replacement samples are introduced.

## Verification

- `run_sound_retail_primitives_test.sh`: O0/O2/UBSan PASS; native ownership
  and retail byte hashes PASS.
- `run_movie_xa_decode_test.sh`: XA decode, CD mix enable/common volume,
  four-route attenuator, saturation, `CdlModeRT`/`CdlModeSF` sector routing,
  and 37.8 -> 44.1 kHz zigzag resampling PASS. The continuity gate reads
  pinned Disc 1 LBAs 48 and 56, compares production PCM byte-for-byte with
  an independent Python hardware model, and pins the 18,816-byte result to
  SHA-256 `a9d1f2db85edf4148bad8d6613fb1a1d9f10906a9c6056dfeb495662cb33cf8b`.
- `run_boot_sound_banks_test.sh`: O0/O2/UBSan PASS; exact archive/load/sync/
  publish/free order PASS; retail bytes pinned.
- Canonical container build: 49 game TUs, 0 skipped, final link PASS.
- Durable PsyCross patch applies cleanly after its declared prerequisite
  patch sequence.

Runtime with the null OpenAL device proves routing, not audibility. Before the
repair, title voices 22/23 keyed at `addr=0x0`. After the repair, the title
uses nonzero bank addresses including `0xfe00`, `0xcf90`, `0x1050` and
`0x1ae0`. The opening field music uses `0x57a20`, `0x5cf20` and `0x62090`.

## Bound

This does not claim human-perceived parity. Movie XA sectors decode, resample
through the PS1 filter, and reach the CD hardware matrix, but final audible
comparison on the operator's real OpenAL device remains pending.
`SoundHandleError` remains fail-closed and was not enabled as a shortcut.
