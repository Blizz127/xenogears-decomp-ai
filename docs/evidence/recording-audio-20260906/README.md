# Record button: silent audio between short bursts

The recorder's Pulse input was overriding audio packet timestamps with the
time ffmpeg read each packet. With buffered input, the resulting timeline
collapsed sound into short bursts and the asynchronous resampler filled the
gaps with silence. An AAC stream was present, so the old structural checks
did not detect the missing sound.

The correction removes that override from the Pulse input. Raw video retains
its wall-clock timestamps. Pulse's own sample timestamps now reach the audio
resampler. The framebuffer capture, default output-monitor selection, AAC
encoding and game audio code are otherwise unchanged.

## Controlled reproduction

Root compiled the actual recorder include into a small host harness with a
synthetic framebuffer and continuous 440 Hz audio on an owned private Pulse
null sink. A simultaneous direct capture established that the selected
monitor carried uninterrupted audio. Only the probe's own sink, source and
playback stream were configured; existing desktop audio settings were left
alone. Every probe process and private module was cleaned up.

| Recorded path | Active 100 ms windows | RMS |
| --- | ---: | ---: |
| Original recorder | 4 / 79 | 0.01672 |
| Original direct reference | 101 / 101 | 0.08818 |
| Corrected recorder | 79 / 79 | 0.08811 |
| Corrected direct reference | 101 / 101 | 0.08818 |

Original output: `/tmp/xeno-recording-routed-tone-c988s_db`.
Corrected output: `/tmp/xeno-recording-routed-tone-7ai4s_xz`.
Their analysis and metadata manifests are copied beside this report.
The candidate differs from installed source only by the explanatory comment.

An earlier candidate attempt had a silent direct reference as well as a
silent recording; it was rejected as an invalid experiment. The paired
results above verify actual stream routing and private sink/source levels.
The initial code-audit hypothesis of a wrong monitor is not required to
explain this reproduced defect.

## Native baseline and build

Sol reproduced the old recorder in a real boot/movie/title run using ordinary
F9 input and a copied binary. Its 14.976-second AAC track contained only
10 active 100 ms windows out of 150; 140 were exact digital silence.
See `before-native-recording.md` for routing, file hashes and process cleanup.

The corrected native build completed successfully:
`/tmp/xeno-recording-audio-fixed-native-build-20260906.log`.
Binary SHA-256:
`bd9b23362e894ba56a4852671235f9bf016437bd9473e8ce2ca0e9968732e5ac`.
Installed recorder SHA-256:
`5d98c739232f16bbab678c9f1d2e915add5ac28377cdc51850557354b5b1e725`.
Full source/build pins are in `installed-build-pins.json`.

## Corrected native recording

The rebuilt game was recorded through ordinary F9 input while an independent
capture read the same output monitor. Across the aligned 15.104-second
interval, both recordings have identical active/silent masks: 77 active
windows out of 152. The recorded/reference RMS ratio is 0.969 and the 10 ms
amplitude-envelope correlation is 0.925. Root independently recomputed the
aligned RMS values and activity masks from the decoded samples.

See `after-native-recording.md`, `after-native-paired-analysis.json` and
`after-native-root-check.json`. The test clip is
`/tmp/xeno-recording-audio-fixed-repro-20260906/run-kudgiv1t/recordings/xenogears-20260906-004401.mp4`,
SHA-256 `efb14c6d9c67fb53fd9024175eeb49334a2f22bdc49145a9038ad33db5d3dbe9`.
Its video/audio durations differ by about 13 ms. This establishes captured
game audio and close amplitude tracking, not sample-exact identity between
independently clocked captures and lossy AAC encoding.

All owned game, encoder, reference and Xvfb processes have exited. Existing
recordings and the root application log were preserved.

## Durable regression

Run `python3 pc_port/tests/run_recording_audio_test.py` from the repository.
It compiles the production recorder include, generates a continuous tone,
uses an owned private Pulse monitor, verifies both capture-client PIDs, and
compares decoded audio with its independent reference. It exercises 30 Hz
and 150 Hz presentation, verifies audio/video durations and source hashes,
then removes its private sink and processes. It requires the existing C++
compiler, ffmpeg/ffprobe and Pulse utilities; no microphone is captured.

Root's final run `/tmp/xeno-recording-audio-root-fixed-ong_ytap` passes:

- 30 Hz: 38/38 active interior windows; RMS ratio 0.99973; A/V duration
  difference 12.7 ms.
- 150 Hz: 39/39 active interior windows; RMS ratio 0.99988; A/V duration
  difference 18.0 ms. The recorder retains its host frame-rate cap.
- Actual old source, selected with `--source` and `--expect-failure`, fails
  the sustained-audio comparison at 3/38 windows with a valid reference.
  Output: `/tmp/xeno-recording-audio-root-old-control-1u4svgiq`.

Silent references, routing failures, changed source hashes and cleanup errors
cannot count as successful negative controls. The existing structural
`psycross_recording_regression_test.sh` also passes and now requires the
wall-clock override to occur only on the raw-video input. Root manifests
and summarized results are retained alongside this report.

This host recorder change does not alter retail game simulation or the
remaining PSX matching checksum failures.
