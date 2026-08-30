# W34N20 — retail pause and controller-recovery lane

## Verdict

`RESTORED_VERIFIED`

The four generated no-ops on the recurring base-world frame path are now
bounded, port-owned implementations:

- `GraphicsDrawPauseLetters` `[0x8001FAB4,0x8001FB30)`
- `SoundMuteAllSpuChannels` `[0x80037EE4,0x80037F44)`
- `wm_8007634C` `[0x8007634C,0x80076594)`
- `wm_80076594` `[0x80076594,0x800767D4)`

The exact driver interval `[0x8007169C,0x80071774)` now dispatches the START
pause and disconnected-controller waits under the retail guards.  Both guards
remain dormant on the accepted scripted route, so the post-change frame 60 and
120 artifacts are bit-identical to W34N19.

Starting HEAD: `bf23f5dd4614c9fa975dd7566aa81250016b4965`

## Retail anchors

- `disc/world_map.bin`
  - SHA-256: `4c15fd32b3a03d7cd5ea4403dcaabc70abaf99b6aaca65d63d6866803edaac70`
  - `wm_8007634C` slice SHA-256:
    `36af52de9565fbd6ad1bc116fefdd09ca5f34c81f8d7e5516fb814e022666db7`
  - `wm_80076594` slice SHA-256:
    `40348010f5b647acef727c2b979eb4cada1eff5a29478755c50c8f77d7ddf79d`
- `disc/SLUS_006.64`
  - SHA-256: `dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119`
  - pause-letter slice SHA-256:
    `d9044bd10a0ca5c465ec94392064bb1bfaffb414380476d47111381facd614bf`
  - SPU-mute slice SHA-256:
    `6204c02cfa4a25dd7a973a5d62b2b38a4d2a7694ec98d6bf23abc984901e69c6`
- Listings:
  - `asm/slus_006.64/nonmatchings/system/rendering/GraphicsDrawPauseLetters.s`
  - `asm/slus_006.64/nonmatchings/system/sound/SoundMuteAllSpuChannels.s`

## Recovered behavior

`GraphicsDrawPauseLetters` rebases retail static data at guest `0x8004FBD8`,
decompresses it, uploads it with `(decoded,1,x,y,0,0,0)`, synchronizes, and
frees the decoded host allocation.  It intentionally preserves retail's lack
of a null-result guard.

`SoundMuteAllSpuChannels` sets the native sound-control bit `0x40` and walks
exactly 24 records in the native SPU backing page.  Per voice it preserves the
observable retail MMIO order: zero left volume, read the old ADSR1 low byte,
zero right volume and pitch, write ADSR2 `0x1FDF`, then write ADSR1
`0x7F00 + old_low`.  It does not key voices off and does not rebase the native
register page through guest RAM.

Both world waits preserve native `D_80059488`, perform the retail front/back
image copies and environment switches, mute sound, draw the pause letters,
clear and drain the six controller accumulators once per wait iteration,
re-enable sound, restore the active display/draw records using independent
`D7F0` reloads, and restore the timer.  `wm_8007634C` exits on released START
(`BD10 & 0x0800`); `wm_80076594` exits when controller type becomes nonzero.

The driver guards are:

```text
C178 == 0 && D804 == 0 && D554 != 0 && D80C == 0 &&
    (BD10 & 0x0800) != 0  -> wm_8007634C

C178 == 0 && D804 == 0 && D554 != 0 && D80C == 0 &&
    ControllerGetType(0) == 0 -> wm_80076594
```

Every guard is re-read for the second arm, as retail does.  The recurring
controller drain now also calls the real zero-argument `ControllerPopState`
API rather than passing a fabricated port argument.

## Certificate

`pc_port/tests/run_w34n20_pause.sh` compiles the production pause source and
driver seam directly:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean for pause and test TUs
- focused legacy warning policy: clean for the existing driver TU
- M1–M14: all DETECTED by named assertions

The certificate covers the guest static source and graphics call order, the
24-voice native SPU write set, both modal predicates, six-source input
clear/drain behavior, image copies, independent environment reloads, timer
restoration, sound resume, and both driver guards.  A certificate-only
liveness defect was found during independent rerun: M10's wrong START mask
could leave the driver fixture spinning.  The fixture now provides both mask
bits while the direct wait test distinguishes the retail bit; a fresh unique
build-directory run completes without external termination.

Dependent gates:

- W34C1 cadence O0/O2/nonrecovering UBSan: PASS; M1–M21 DETECTED
- normal port build: `LINK OK`
- `nm` shows all four functions as strong text symbols and none appears in
  generated `stubs.c`

## Detached native neutrality

Harness: `scratchpad/w34n20_detach.gdb`; debugger detached before
`PcPort_WorldMapInitMain`; scripted input
`0:0x2000,600:0x4000,916:0x2000,976:0`; bounded 120 displayed frames.

- frame 60 fulfilled at frame 60:
  `scratchpad/w34n20_capture/world-frame-000060.bmp`
  - SHA-256: `26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3`
- frame 120 fulfilled at frame 120:
  `scratchpad/w34n20_capture/world-frame-000120.bmp`
  - SHA-256: `ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc`
- bounded exit: `frames=120 limit=120`, returned to init
- both BMPs compare byte-for-byte equal to W34N19
- detached PID `1946054` was verified against the exact binary path and
  terminated only after the bounded loop returned

## Residual

`ControllerGetType(0)` has a known return-value representation split in the
port, but its connected/disconnected zero predicate matches retail at this
guard.  The two modal branches are synthetically certified but were not
naturally armed by the accepted route; a future live START/disconnect route
should provide behavioral confirmation without weakening this exact
implementation.
