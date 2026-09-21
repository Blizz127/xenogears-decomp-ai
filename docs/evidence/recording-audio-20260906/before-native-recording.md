# Native recording real-audio reproduction

## Result

**REPRODUCED: the captured AAC stream exists and is full-duration, but its decoded PCM is overwhelmingly silent and discontinuous.**

The isolated native run recorded 14.983 seconds of ordinary boot/movie/title presentation. The 14.976-second AAC track decoded successfully at 48 kHz stereo, but only 10 of 150 consecutive 100 ms windows contained measurable samples. The other 140 windows were exact digital zero. This is a real PulseAudio/PipeWire run; `ALSOFT_DRIVERS` was absent and `XDG_RUNTIME_DIR=/run/user/1000` was supplied.

The retained video visibly spans the boot flare into the Xenogears title logo. This reproduces the reported symptom more precisely as intermittent audio bursts separated by long digital-silence intervals, rather than a missing audio track.

## Run and ownership

- Run: `/tmp/xeno-recording-audio-repro-20260906/run-atzvj_59`
- Driver: `/tmp/xeno-recording-audio-repro-20260906/reproduce.py`
- Isolated display: `:1`
- Owned Xvfb PID: `83062`
- Owned native PID/window: `83350` / `2097204`
- Recorder FFmpeg PID observed through Pulse: `83931`
- Copied native binary SHA-256: `1738d43da2e7d901416cbc4376e2045298d0fcd26b0de418a0b75f764e992cd3`
- Driver SHA-256: `f3faaec960eadf35708e92025a8cb10bcfb1cfda5336bdb13ae9223cf5e89d21`
- Recording started at `2026-09-06T05:35:30.866682Z` with ordinary F9 and stopped after 15 seconds with ordinary F9.
- The final MP4 is `recordings/xenogears-20260906-003530.mp4`, 1,839,583 bytes, SHA-256 `2ed10ebecb170d5632718684bce923379a65ab47b14ffc19421185559a352344`.

The game did not respond to the isolated window's ordinary Alt+F4 within 15 seconds, so the driver used its owned-process SIGTERM fallback. The native process handled it and returned 0. Xvfb also returned 0. PIDs `83062`, `83350`, and `83931` are absent.

## Live routing

The default sink remained `alsa_output.pci-0000_80_1f.3.analog-stereo` throughout.

Before and during recording, the game's OpenAL stream appeared as Pulse sink-input index `38028`:

- `device.description=xeno-port`
- `node.name=xeno-port`
- `media.role=game`
- `media.name=Playback Stream`
- 44.1 kHz node rate
- stream mute `false`, corked `false`, left/right volume 100%
- current sink id `83`, named `easyeffects_sink`
- PipeWire property `target.object=12976`, the hardware sink object

The recording FFmpeg appeared as source-output index `38076`:

- process `83931`, binary `ffmpeg`
- source id `12976`
- `target.object=alsa_output.pci-0000_80_1f.3.analog-stereo`
- `stream.capture.sink=true`
- mute `false`, corked `false`, left/right volume 100%
- its X11 context was the owned display `:1`

The application's own log agrees that it selected `alsa_output.pci-0000_80_1f.3.analog-stereo.monitor`.

The sink objects themselves reported global mute `true` during this run: `easyeffects_sink` at 40% and the hardware sink at 76%. I did not change those global settings. That prevents this run from claiming speaker-audible output, but it does not explain the encoded pattern as a wholly absent source: the AAC contains three high-energy bursts amid exact inserted silence. The live capture therefore establishes recorder discontinuity under the current user audio graph; it does not by itself decide which timestamp/buffering operation causes it.

## Decoded audio evidence

`ffprobe.json` records:

- H.264 video duration: 14.983333 s
- AAC audio duration: 14.976000 s
- AAC sample rate/channels: 48,000 Hz / stereo

The AAC was decoded to raw float32 stereo as `audio.f32le`:

- 718,848 stereo frames, 14.976 s
- SHA-256 `e52a34429e49d82647a20ea26db80934288c8283640a817872e26e7b5f3e08e5`
- whole-track RMS `0.0476078405`
- peak `0.828017712`
- 67,840 nonzero samples of 1,437,696 samples
- 10 active 100 ms bins of 150 total; 140 bins are exact zero

The only active windows begin at 0.0–0.1 s, 10.3–10.6 s, and 14.6–14.9 s. On a one-second partition, 12 of 15 windows are exact zero. `pcm-analysis.json` contains every one-second and 100 ms measurement; SHA-256 `55001fc4aee277511ce4081af8fb7a533335aaaa53b770bb7c72dcbd91e88b4d`.

AAC packet timestamps themselves span the recording continuously in 21.333 ms steps: 703 packets from -0.021333 through 14.954667 seconds. However, 667 packets are 30 bytes or smaller while only 36 exceed 100 bytes. The packet inventory is in `audio-packets.csv` SHA-256 `7fdf6e2319e50d09c56656a31af25bef85ead757da2420df9229184676c79c79`.

## Visual and preservation evidence

- Pre-recording boot flare: `before-recording.png`, SHA-256 `2f01eece56210dceb78bb981179e982f25eefd0105a3af85f1bce6e6c11322b8`.
- Active recording at Xenogears title logo, toolbar showing STOP: `during-recording.png`, SHA-256 `4f65590e449a0aaa568f2b7b5924ce9cc05a42e59395a8ad64c00e3b0dd348fb`.
- Full routing snapshots are retained as `pulse-before-launch.json`, `pulse-game-before-recording.json`, `pulse-recording-2s.json`, `pulse-recording-8s.json`, `pulse-recording-15s.json`, and `pulse-after-recording.json`.
- `actions.jsonl` SHA-256: `36d5134f0bc93517650ff5500736d67c281a3cbb7452ae18ce3cc6c1349b951f`.
- The repository `recordings/` file/size/hash manifest is identical before and after the test.
- The pre-run root application log was restored byte-for-byte at SHA-256 `4abc6faa0b9549ca84bb28e303614a62b1f85ebee7d5d1a3cfe6c09280c8edae`. The owned log is retained separately as `owned-app.log`.
- No global sink, source, volume, mute, routing, or EasyEffects setting was changed. No microphone source was selected or recorded intentionally. No production file was edited.
