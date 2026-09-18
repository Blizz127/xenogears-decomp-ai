# Fresh natural traversal observation

The frozen native build `0ebb5b95379d474c9f2dc3859e323a6d93acd8ff8e811adccd54b43f31b802da` ran from a fresh New Game on 2026-09-08. It includes the grounding correction and the separately documented party-removal correction. The run directory is `scratchpad/lahan-natural-20260908-slope-ground`; `build-pins.json` records its source pins. `runtime-observation.json` pins the binary and selected local screenshots. Retail payloads and screenshots remain in the local run directory.

After reaching the mountain naturally, Fei moved through the region near the earlier frozen position. A read-only field observation placed him at (-852,-251,-144); ordinary downhill input then released him into the canyon. He subsequently returned to the upper route. This is evidence of recovery movement in the corrected build, not a controlled live A/B proof that the correction alone caused it.

Two gap attempts fell short. A later attempt from (-682,-409,-775), holding ordinary Left+run+jump through the landing, crossed successfully. Screenshots `gap-late-jump-1.png`, `gap-late-jump-3.png`, and `gap-late-landing.png` show the jump and west platform. The stable landing was (-1299,-449,-745). Ordinary movement then crossed the bridge (`bridge-cross.png`) and reached field17 at 22:00:42 CDT (`driver.log`, `citan-path-entry.png`). No positions, flags, inventory, or scene progress were directly set. One naturally earned Hob-Jerky was used through the menu, and two encounters were exited with the normal Escape command before field17 entry.

The gap failures were resolved by changing ordinary takeoff and hold timing. No additional jump-code change was made. The observation therefore does not establish another jump defect or prove retail jump parity.

One field-coordinate query was mistakenly issued during battle9 and returned (0,0,0). That result is invalid and excluded from the traversal evidence. Field screenshots must be inspected in a separate step before reading field actor state.

The existing offline comparison and exact-match limits remain unchanged: 96 selected variants pass at O0 and O2, with the shared-GTE and uncaptured-entry-argument limitations documented in the README. The function remains 2396 compiled versus 2704 retail bytes. Hardware audiovisual parity, complete retail behavior parity, and the Lahan departure sequence remain unproven. No commit or push was made.
