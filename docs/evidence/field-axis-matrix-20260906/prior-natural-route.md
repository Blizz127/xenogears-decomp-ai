# Natural forest-route continuation — bounded observation

Date: 2026-09-06 (UTC)  
Verdict: **BOUNDARY REACHED: FIELD 4 OPENING TEXT. FIRST BATTLE, FIELD 14/13, VILLAGE, WORLD, LATER BATTLE, AND A9 NOT OBSERVED.**

## Identity and ownership

- Repository: `/var/home/blizz/Projects/xenogears-decomp-ai`
- Copied native executable: `/tmp/xeno-forest-route-20260906/run-rgg6p30b/xeno-port`
- Executable SHA-256: `1c9e2c73427c0ed20177b5db49cdd62ec488cc03b717269c68f57ac3b3e5b215`
- Build manifest: `docs/evidence/member-change-menu-20260906/final-build-pins.json`
- Manifest SHA-256: `81bb087eadd817231b7bb7641ac64dffbe5ae4b209a00b3d735a5ee82af774dc`
- Run directory: `/tmp/xeno-forest-route-20260906/run-rgg6p30b`
- Owned display/processes during the run: display `:2`, Xvfb `402539`, GDB `402618`, native `402637`, SDL window `6291508`.
- The run ended at `2026-09-06T07:01:14.943809+00:00`; all three owned PIDs are absent. Unrelated Xvfb `:91` PID `173769` and `:92` PID `1142781` remain live and were not touched.

No production source/configuration was edited. The environment did not contain `XENO_FIELD_TEST_INPUT`, a forced field/map/story value, or any auto-confirm schedule. The only game inputs were ordinary focused keyboard events. Read-only GDB probes covered `FieldMain`, battle entry/return, `KernelMenuMain`, signals, and the exact A9 case at current native address `0x4caf2d` (`animation_scripts.c:325`).

## Observed path

1. `FieldMain` entered field `490` at `06:46:34.550854Z`.
2. The apparent Xenogears logo was an internal movie frame, not the title menu. A capture two seconds after the first unfocused Return showed later spaceship footage. Subsequent captures showed the horizon, fetus graphic, wreckage, and hex-grid movie frames.
3. The manual helper initially sent keys without explicitly focusing the isolated SDL window. Those early Return/Circle events therefore do not certify receipt. After adding `windowfocus --sync` and sending ordinary `z`/Circle at `06:58:22.661910Z`, the native log immediately entered `func_801C58EC` with `choice=1`.
4. The run driver then sent ordinary `Up` followed by `z`; the log recorded `title confirm choice=2 keep=0`, which is the source-documented New Game choice.
5. The movie image continued until a second focused ordinary `z` at `06:59:56.138023Z`. `FieldMain` then entered field `4` at `06:59:57.469520Z`.
6. The driver changed the host speed from 1x to 5x through four F11 presses. Its 900-second local deadline expired while field 4 displayed the opening text, before battle entry.

The final converted periodic framebuffer is `run-rgg6p30b/final-periodic-converted.png`, SHA-256 `afa288ea3ad99b9225d40315df0d8cf18b655277c68bf84126bc7cfe20dbe656`. It visibly reads the opening narration beginning “The continent of Ignas...” and is therefore field-4 presentation evidence, not battle evidence. `field-events.jsonl` contains exactly field 490 and field 4. No `battle-events.jsonl` or `a9.jsonl` was produced.

## Timing and trace overhead

The long pre-title interval was mainly a control/focus error in this driver: it waited for the title-loop log before discovering and focusing the window, but the title loop was reached only after a focused Circle skipped the movie presentation. Once focus was explicit, Circle produced the title-loop marker immediately. The run also used 110 seconds of held Backspace fast-forward, recorded in `navigation-actions.jsonl`.

The GDB trace did not stop every frame. `FieldMain` fired twice, while the battle and A9 probes never fired. The periodic capture setting did write a 921,654-byte BMP every 300 frames (despite the `.png` extension), growing the run directory to 237 MiB; that avoidable I/O may have added overhead. It should be raised to at least 3000 frames or disabled during traversal. The A9 breakpoint itself remained sparse and unhit.

## Checkpoint result

An ordinary F5 tap was attempted at `07:00:55.461992Z`, but it did **not** create a checkpoint. There is no `[xeno-port][quick] save queued` or `saved` marker in the log. The repository's existing `quicksaves/quick.xgqs` remains the older file dated `2026-09-03 17:31:02 -0500`, size 9,096, SHA-256 `54143af4f7736907ac8a914b8ce2fdd216c73c31cdea6c3510468df26e9a35f9`; it is unrelated and was not modified. The current native exposes Quick Save through the toolbar dispatcher, while `quick_checkpoint.c`'s messages refer to F7/F8. No earned checkpoint exists from this run.

## Preservation and next run correction

- The application log was restored byte-for-byte: both the pre-run backup and current `Xenogears (PC port).log` hash to `4abc6faa0b9549ca84bb28e303614a62b1f85ebee7d5d1a3cfe6c09280c8edae`.
- Scratch driver SHA-256: `c46c495877f22683a067c9a95c8b135055c8d4e0ce2db918c1d8f777f73cd2b0`.
- Expanded run trace SHA-256: `edd3e3175b3e60843e6b1b6e075e9b8c2d06b4e303a8aeccfbcf151c96cf872a`.
- Current control helper SHA-256 after the focus correction: `155cff1a091439f76238d43f5d6f42a3c0b6a4a8d929ed2f2d07cf68d6a61e8f`.

A follow-up driver should discover and validate the SDL window as soon as the copied native process appears, focus it, and use the now-observed focused Circle sequence instead of waiting circularly for the title-loop log. It should support a live extension file, isolate checkpoints with `XENO_QUICKSAVE_PATH`, and invoke the visible Quick Save toolbar control only after free field control is observed. A second bootstrap was not launched in this turn.
