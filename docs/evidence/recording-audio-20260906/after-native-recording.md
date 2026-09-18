# Fixed native recorder: paired real-audio verification

## Result

**PASS: the fixed F9 recorder captured sustained game audio, verified against a simultaneous direct capture from the same Pulse monitor.**

The 15.104-second AAC decode and the aligned direct-monitor reference each contain 77 active 100 ms bins out of the same 152-bin interval, with identical active/silent masks. Their aligned RMS values are `0.1111683` and `0.1147243`, a recorder/reference ratio of `0.9690`. The 10 ms amplitude-envelope correlation is `0.9250`; active-bin 100 ms RMS correlation is `0.9031`.

This is stronger than audio-track presence. The prior unfixed real-game recording had only 10 active bins out of 150, including 140 exact-zero bins. The fixed recording follows every natural music-active and music-silent interval seen by the independent monitor reference.

## Pins and ownership

- Run: `/tmp/xeno-recording-audio-fixed-repro-20260906/run-kudgiv1t`
- Capture driver: `/tmp/xeno-recording-audio-fixed-repro-20260906/capture.py`
- Driver SHA-256: `8954fe7d868eca1510a8fd5baaebe140d9664826ff7270d11cae198c554a2954`
- Copied fixed binary SHA-256: `bd9b23362e894ba56a4852671235f9bf016437bd9473e8ce2ca0e9968732e5ac`
- Recorder source SHA-256: `5d98c739232f16bbab678c9f1d2e915add5ac28377cdc51850557354b5b1e725`
- Fixed native build log SHA-256: `029829a1b4eb46b4ace39286d1b43058e38668c0fdcb62810a72c482d6d6bf4d`
- Isolated display: `:1`
- Owned Xvfb PID: `124759`
- Owned native PID/window: `124767` / isolated window
- Independent reference FFmpeg PID: `125790`
- Recorder FFmpeg PID: `125852`
- `XDG_RUNTIME_DIR=/run/user/1000`; `ALSOFT_DRIVERS` was absent.

The ordinary F9 interval began at `2026-09-06T05:44:01.258618Z` and ended at `05:44:16.421436Z`. The independent monitor capture began 0.803 seconds before F9 and stopped immediately after the recorder finalized.

## Exact audio route

The default sink was `alsa_output.pci-0000_80_1f.3.analog-stereo`; both capture clients selected its monitor. Before launch, Pulse had zero sink-inputs and zero source-outputs, excluding unrelated playback/capture streams from the paired interval.

During the recording:

- Game sink-input index `38477` was `xeno-port` / `Playback Stream`, mute `false`, corked `false`, stereo volume 100%, current sink `83` (`easyeffects_sink`), with PipeWire `target.object=12976`.
- Independent reference source-output index `38524`, PID `125790`, read source id `12976`, target `alsa_output.pci-0000_80_1f.3.analog-stereo`, mute `false`, corked `false`, stereo volume 100%.
- In-game recorder source-output index `38538`, PID `125852`, read the same source id `12976` with the same target, mute/cork state, and volume.
- The application log states `audio=alsa_output.pci-0000_80_1f.3.analog-stereo.monitor` and records a successful final save.

Snapshots at 2, 8, and 15 seconds retain both capture source-outputs and the single game playback stream. No microphone source was selected. No global sink, source, volume, mute, or EasyEffects setting was changed.

## Output and decoded measurements

The final file is `recordings/xenogears-20260906-004401.mp4`:

- size: 1,812,559 bytes
- SHA-256: `efb14c6d9c67fb53fd9024175eeb49334a2f22bdc49145a9038ad33db5d3dbe9`
- H.264 duration: 15.116667 s
- AAC duration: 15.104000 s
- AAC format: 48 kHz stereo

The recorder AAC decoded to `recorder.f32le`:

- 724,992 stereo frames / 15.104 s
- SHA-256: `514bb960fcf19c22231d32f67bc0283f267cd8ab1f0b2a8a0dc5d2c6b6b83880`
- RMS `0.1111683`, peak `0.8895828`
- 77 active 100 ms bins, 74 exact-zero bins, and one nonzero bin below the activity threshold (the final bin is a 192-frame partial)

The direct monitor capture is `direct-monitor.wav`:

- SHA-256: `162791ed59e7a1c6cc60b265f2b0c9557da8198648f17eb56f7a5c28a9f03f70`
- decoded `reference.f32le` SHA-256: `f244e69bfdffddc39e813e47b2f0b8e98b9a8252cec86386727ec8943ee86efa`
- full reference duration: 16.45 s; its extra lead/tail explain its unaligned 84/165 activity count

Alignment placed recorder time zero at reference frame 32,976, a 0.687-second reference lead. Over the resulting full 15.104-second paired interval:

- recorder activity: 77/152 bins
- reference activity: 77/152 bins
- active masks identical: yes
- recorder/reference RMS: `0.1111683` / `0.1147243`
- RMS ratio: `0.9690`
- 10 ms envelope correlation: `0.9250`
- active-bin 100 ms RMS correlation: `0.9031`

The single fixed-offset raw-sample waveform correlation is only `0.0564`. Local lag visibly drifts during the independently clocked Pulse/FFmpeg captures, while AAC encoding and resampling further prevent a sample-exact comparison. This run therefore claims successful game-audio activity capture and close amplitude tracking, not sample-exact PCM identity. The activity masks, paired RMS, and envelope correlations are the acceptance evidence.

Full metrics are in `paired-analysis.json`, SHA-256 `b75cb5688ead386507d5f16dd3224bd6982229169bb985ec95240f8a4acd508f`. Every aligned bin is retained in `paired-100ms.csv`, SHA-256 `644cf23b98eead2fdaa10707a2f997f287466073e4608f09e16436bce45e72d7`.

## Visual, cleanup, and preservation

The recording screenshot shows the Xenogears title logo and the toolbar's red STOP state: `during-recording.png`, SHA-256 `4f65590e449a0aaa568f2b7b5924ce9cc05a42e59395a8ad64c00e3b0dd348fb`.

The direct-reference FFmpeg returned 255 after the driver's intentional SIGINT finalization; its WAV finalized and decoded successfully. The game did not respond to Alt+F4 within 15 seconds, so the driver used its owned SIGTERM fallback; the native process returned 0. Xvfb returned 0. Owned PIDs `124759`, `124767`, `125790`, and `125852` are absent.

The repository recording manifest is byte-identical before and after this run. The original root application log was restored unchanged at SHA-256 `4abc6faa0b9549ca84bb28e303614a62b1f85ebee7d5d1a3cfe6c09280c8edae`; the owned application log is retained separately. No production file was edited by this verification task.
