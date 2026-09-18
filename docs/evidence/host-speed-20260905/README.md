# Host speed control, 2026-09-05

Status: the NTSC startup correction passes adapter tests and normal-opening
wall-clock verification. Field 4 measured **29.990 frames/s at 1x and 149.091
at 5x (4.971x)**. The earlier music-only / below-target user reports are
preserved below; the final uninstrumented native build also passes.

## Controls and scope

The host toolbar has a `SPEED 1X` button which cycles through 1x–5x. F11
cycles, Shift+F11 returns to 1x, and holding Backspace selects 5x until released
(or focus is lost). `XENO_SPEED=1` through `5` selects the startup rate; invalid
values select 1x. The toolbar stays outside game captures and recordings.

The implementation scales host vblank and sound-counter periods, streamed CD
sector pacing and OpenAL playback pitch together. It retains emulated voice
pitch registers and all script/game updates. Vblank queries no longer mutate
the clock as the old unlimited Backspace path did. These are host testing
controls, not changes to retail gameplay logic. Audio at accelerated speeds
is intentionally faster and higher pitched.

## Evidence so far

- Native build `host-speed-port-build-20260905.log` passes; executable
  `cb0f5ea1a4384ae358a40bb65f300dc3e2f71b9ec6f896d95ded78a4273961a1`.
- `host_speed_test.UlMzyUTT`: O0/O2/UBSan pass for actual extracted timer,
  CD-pacing and audio-refresh functions. Deterministic wall-clock and AL leaves
  cover all five rates in NTSC/PAL, 240-Hz sound scaling, 75/150-sector CD
  scaling, unchanged counters on queries, hold/release, invalid settings,
  toolbar hit boundaries and unchanged emulated audio registers. Five mutants
  reject unscaled clocks, query-driven ticks and audio-register corruption.
  This proves adapter behavior, not faster gameplay.
- Existing toolbar hit-zone and host-chrome ordering checks pass. The vendor
  patch replays forward byte-identically against the saved pre-change vendor
  files and passes reverse-check against the edited tree.
- `opening-battle-host-speed-39dbja_6` shows the speed button and logs actual
  rate changes. The automated click/measurement helper stopped on a missing
  expected acknowledgement amid concurrent user inputs; its full 1x–5x UI
  acceptance sequence did not pass. The user reported faster music only.
- `opening-speed-profile-jfyspim_` records scoped read-only GDB timings during
  normal New Game. Median `GR_StoreFrameBuffer`: 15.016 ms at 1x (181 samples),
  23.071 ms at 5x (273). Median complete field-frame function: 53.315/42.312 ms.
  Game-clock ticks per field frame increase from 2.73 to 10.43, while update
  throughput changes much less. GDB overhead limits these as benchmarks.

`GR_StoreFrameBuffer` unconditionally downloads the framebuffer texture,
converts 71,680 pixels and performs two allocations per 320x224 frame.
Deferring CPU mirroring requires preserving pending pages, overlapping CPU
reads and writes, StoreImage, MoveImage and immediate DR_MOVE ordering. The
diagnosis alone is not evidence that a proposed lazy-copy change is correct.

Ignored test/runtime artifacts are under `pc_port/build_native/`; the exact
before files, standalone launch/input helpers and vendor-patch replay are under
`/tmp/xeno-gear-resume-20260905-l12d_42r/`. No commit or push.

The separate [opening battle report](../opening-battle-encounter-20260905/README.md)
tracks retail script commands and missing dialogue text. User-confirmed Gear
and pilot-panel progress does not establish complete battle or dialogue parity.

## Deferred-cache experiment rejected (22:20 UTC)

The two-slot draft failed its sustained-capture regression: once both slots
filled, each further capture downloaded an old page even when superseded.
Review also found a GPU encoding defect independent of that performance bug:
`g_vramTexture` holds the low/high bytes of RGB555 words in RG channels, while
the capture blit copies display RGBA. The eager CPU conversion and subsequent
full upload currently restore the encoding before texture consumers. Removing
that conversion is unsafe without a corresponding GPU encoding/upload design.

The exact pre-experiment renderer was restored (SHA-256
`fab65768d65e5da211d983ceba1f09ec47c0d800832f73204b1d6cdeafa87288`).
The rejected draft and its RED fixture are preserved outside production under
`/tmp/framebuffer-speed-before.rXzmOR/`. No deferred-cache patch is installed.
Existing real-OpenGL staging, color, orientation and placement tests pass at
O0/O2/UBSan, and all three negative controls reject. Logs:
`/tmp/xeno-framebuffer-staging-{build,runtime}-20260905.log`.

Next measurement is a low-overhead scoped renderer profile and an isolated
renderer-only compiler optimization; no game/decomp optimization is implied.

## Root cause and measured correction (22:44 UTC)

The native bootstrap never called `SetVideoMode`. PsyCross initializes
`g_vmode=-1`, and its timing ternary treats every value except NTSC (0) as
PAL. A low-overhead native probe measured 300 ticks in approximately 1.2003
seconds at 5x, or 249.84 Hz. The rate was precisely the PAL target, rather
than the expected NTSC 300 Hz.

The pinned US executable initializes `g_VideoMode` at `80058990` to zero.
Its header load address is `80010000`, yielding file offset `49190` for that
word. `port_main.c` now calls `SetVideoMode(0)` immediately before
`PsyX_Initialise`, publishing the retail NTSC setting before thread creation.
The independent startup test executes the actual main prefix and both video
mode setter functions. RED `host_video_mode_boot_test.zY0KfnGw` observes the
unset mode at core startup. GREEN `host_video_mode_boot_test.wEOsH6ah` passes
O0/O2/UBSan and rejects omitted, PAL, and after-startup calls; retail/source
pins are in its provenance file.

Two normal New Game runs use the same native binary and field-only Circle
schedule, with no debugger or game-state writes. Temporary clock probes log
only once per 300 ticks. Capture/log chronology excludes speed and field
transitions from the field-4 frame-rate intervals:

| Setting | Vblank ticks/s | Field 4 frames/s | Intervals |
| --- | ---: | ---: | ---: |
| 1x | 59.999 | 29.990 | 24 |
| 5x | 299.926 | 149.091 | 55 |

The field-rate ratio is **4.971x**. Runs are
`host-render-profile-1-vpk5f9yb` and `host-render-profile-5-58bdvk27`, under
`pc_port/build_native/`. Raw analysis is
`/tmp/xeno-ntsc-clock-profile/comparison.json`. The temporary probes were
removed after measurement. `PsyX_main.cpp` is restored to host-speed SHA
`a943b7d33c426c661801ce405cd0f53c20ce7bd02868be33154af613ef04b065`.

The isolated renderer O2 experiment reduced mean conversion time from about
0.266 to 0.126 ms but did not materially improve field throughput. It was
removed. The original renderer, CMake options, and build link flags are
restored; no deferred cache or broad optimization ships. Earlier GDB timings
substantially overstated the renderer's normal-run costs.

Final normal executable: SHA-256
`8827e71ceda0adad0802b3b96d81ec2c7cd2703b791000b534afda84c2f1f40f`,
log `pc_port/build_native/ntsc-speed-c1-port-build-20260905.log`.
Its opening replay reaches the separately tracked unsupported AC battle
command; full battle completion and the user's final visual acceptance
remain separate from the measured speed result.
