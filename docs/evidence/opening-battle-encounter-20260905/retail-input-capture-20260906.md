# Xenogears retail opening input/capture report

Capture run date: 2026-09-06 UTC

## Verdict

**UNRESOLVED:** this bounded retail run naturally reached and saved the title, opened the retail `New Game / Continue / Sound` menu, and moved the cursor from Continue to New Game through the ordinary pad API. It did not enter New Game. Host-keyboard and Lua-override Circle events were not observed by the game: both a two-second hold and a fresh press after a three-second selection-redraw settle left the verified New Game framebuffer byte-identical through three seconds. The requested opening Gear pilot panel was not reached, so no retail framebuffer establishes the exact Fei text or its glyph placement.

The expected wording remains **`Hiyaaaaa!`** from `/tmp/xeno-retail-dialogue-reference-20260906.md`; this run does not promote that reference to retail-runtime proof.

## Scope and ownership

- Repository anchor, read only: `/var/home/blizz/Projects/xenogears-decomp-ai`
- Owned isolated PCSX profile and evidence: `/tmp/xeno-retail-opening-reference-20260906-8mpinaz6`
- No repository or ambient PCSX configuration was changed.
- No RAM, register, story, position, encounter, or presentation state was planted or edited.
- The only state load was the ordinary PCSX API restore of a savestate made at a naturally reached retail title screen. It was explicitly authorized to avoid replaying the movie.
- No native `xeno-port` process was touched.

## Retail process and provenance

- The handed-off owned process was PID `2706532`; `/proc/2706532/exe` resolved exactly to `/var/home/blizz/Applications/pcsx-redux-src/AppDir/usr/bin/pcsx-redux`. Its isolated profile and launch metadata were preserved before a normal `SIGTERM` as:
  - `pcsx.before-keyboard-restart.json`, SHA-256 `136062da1f57fba0ca6b10dffb7868653e16d4da3425ddd2bb5b9ca77a8114e5`
  - `run.before-keyboard-restart.json`, SHA-256 `ae28894af5b481c6668af1ca7f06a7370b9b1299c3d88261312d9c69dd2deb3c`
- Fresh PID `2822389` booted the same retail inputs at 100% scaler and naturally traversed the opening movie. It exited cleanly when its attached transient execution session ended; no useful state was lost.
- Persistent isolated service `codex-xeno-retail-opening-20260906.service` started PID `2890592` at 2026-09-06 00:56:46 UTC with the exact executable, isolated portable profile, interpreter CPU, retail BIOS/ISO, isolated memory cards, and `/tmp/.../retail-pad-api.lua`.
- After preserving the profile, helper, unit definition, and journal, PID `2890592` was normally stopped and relaunched as PID `3069504` with Circle handlers. A second preserved restart added Up/Down handlers and relaunched the same exact command as PID `3106417` at 2026-09-06 01:46:29 UTC.
- At the final check, `2026-09-06T02:03:17.908236521Z`, service invocation `1a2d35a9910c49e48694289df8e29bc9` remained `active/running` with main PID `3106417`; `/proc/3106417/exe` resolved to the exact expected PCSX executable. The emulator and isolated profile were intentionally preserved.
- PCSX executable SHA-256: `0621042a2e2d2b4df0f3fd863a11d51be16957303c12f32b44fec2e11e61fa78`
- Disc 1 raw BIN SHA-256: `39c547a9afc6da15d847ef81a2c6cea1a6516bdfa562cf13b0999b04e8598bda`
- BIOS SHA-256: `11052b6499e466bbf0a709b1f9cb6834a9418e66680387912451e971cf8a1fef`
- Retail executable `SLUS_006.64` SHA-256: `dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119`
- Retail executable `SLUS_006.64` SHA-1: `560bbdbeb9264c935294ecad5a3d4ab230a006a9`, matching `config/slus_006.64.yaml`
- Retail title/menu overlay `disc/menu.bin` SHA-256: `82f84a24ac1fd1979754e1cc9c861405fb59ce95dd78db00a76f00c8f1bd177d`
- Retail title/menu overlay `disc/menu.bin` SHA-1: `fd911d26b921d6eb0c73c3bbc83ea82dc459122f`, matching `config/menu.yaml`
- `/api/v1/cd/info` identified disc label `XENOGEARS` and ID `SLUS00664`.
- Final isolated `pcsx.json` SHA-256: `e38cb3dee0943feccbbe494835d71d13ae133be417243383cc6b50bca40a188b`
- Original `retail-pad-api.lua` SHA-256: `75d2a995ff4af18f7212e14f96646c8880280ef930c6823dcd1bc32b9a963f05`
- Final helper with Start, Cross, Circle, Up, and Down press/release handlers SHA-256: `c9d476df7ec403e2e177d596dced34e92ffd9cc0497c79c9d0c262b46515810a`

## Input configuration source proof

PCSX's JSON names are counterintuitive in this source. `/var/home/blizz/Applications/pcsx-redux-src/src/core/pad.cc:42-43` defines `InputType { Auto, Controller, Keyboard }` and emulated `PadType { Digital = 0, ... }`. At `pad.cc:119-121`, `SettingInputType` is serialized as JSON `"PadType"`, while `SettingDeviceType` is serialized as JSON `"DeviceType"`. The verified keyboard/digital configuration is therefore:

- JSON `pads[0].PadType = 2`: host input type `Keyboard`
- JSON `pads[0].DeviceType = 0`: emulated PSX `Digital` controller
- JSON `pads[0].Keyboard_PadCircle = 68`: GLFW `D`, confirmed by `pad.cc:89`

The isolated profile used these values. This mapping is the opposite of what the JSON key names suggest; the source lines above must remain with the evidence to avoid reversing them in a future attempt.

The local Lua normal-controller path is also source-backed. `pad.cc:811` polls `pad.buttonStatus & pad.overrides`; Lua `setOverride`/`clearOverride` therefore changes ordinary PSX button bits and does not edit game RAM. The helper initially defined Start and Cross endpoints; authorized isolated-profile restarts added Circle and Up/Down endpoints.

## Retail menu semantics

The local retail/decomp source at `src/menu/main/misc.c:1304-1321` drains queued controller states and maps the field named `g_C1ButtonStateReleased` as follows:

- Circle bit `0x20` -> menu input `4`, confirm
- Cross bit `0x40` -> menu input `5`, cancel

The field name is misleading. `src/slus_006.64/system/controller.c:131-133` computes `(current ^ previous) & current`, where `current` has one bits for pressed buttons. It therefore records the single 0-to-1 **press edge**, not the release edge. Holding a button cannot recreate that edge, and the later release is not checked by this title loop.

The title loop at `misc.c:397-408` acts on input `4`; at `misc.c:419-425`, `unk2D8 >= 0x259` exits to the opening movie through the disc-1 idle timeout. The title menu maps internal choice `0` to Sound/options, `1` to Continue, and `2` to New Game (`misc.c:356-363`), while menu initialization selects choice `1` (`misc.c:823-829`). `include/psyq/libetc.h:14-15` defines Up as bit `0x1000` and Down as `0x4000`; `misc.c:1308-1311` and `:382-396` show that Up maps to input `3` and increments choice `1` to `2`.

### Retail-byte authority check

The confirm-bit conclusion was checked directly against the local retail payloads; it does not rest only on the decompilation source.

- `disc/menu.bin` loads at `0x801C5000`. Raw little-endian MIPS disassembly of retail `func_801C7D78` confirms Up mask `0x1000` at `0x801C7EB0`, the load of `g_C1ButtonStateReleased` (`0x8005948C`) at `0x801C7EBC`, Circle mask `0x20` at `0x801C7EC8`, and Cross mask `0x40` at `0x801C7ED0`. The Circle branch at `0x801C7ECC` reaches the input-4 path at `0x801C7E28`.
- Raw retail title-loop body `func_801C58EC` confirms the input-4 comparison at `0x801C5980-0x801C5984`, confirm transition call `func_801C531C(7)` at `0x801C59C8`, input-3 choice increment around `0x801C5A14-0x801C5A40`, and the 601-tick idle threshold at `0x801C5B00`.
- `disc/SLUS_006.64` loads at `0x80010000` after its `0x800`-byte PS-X EXE header. Raw retail `ControllerGetButtonState` at `0x8003569C` reads packet bytes 3 and 2 at `0x80035708-0x8003570C`, inverts the byte-3 value at `0x80035710`, shifts and XORs byte 2 at `0x80035718-0x8003571C`, then combines them at `0x80035724`. Thus PCSX raw Circle bit 13 becomes game mask `0x20`, while raw Up bit 4 becomes game mask `0x1000`.
- Raw retail `ControllerPoll` at `0x800358BC` computes the press edge at `0x800359E8-0x800359EC` as `(current XOR previous) AND current` and stores it to `g_C1ButtonStateReleased` at `0x800359F8`. Raw retail `ControllerPushState` at `0x80035C0C` enforces the 16-entry limit at `0x80035C18-0x80035C20`; `ControllerPopState` starts at `0x80035CDC`.

These retail bodies agree with the cited decompilation. The PCSX override implementation is emulator source rather than game retail bytes; its behavior is independently supported here by Cross opening/dismissing the menu and Up visibly moving the retail cursor.

## Naturally reached title and bounded retry

1. The title detector observed two consecutive retail framebuffer matches at 2026-09-06 01:14:12 UTC (RMSE `0.127705`, grayscale correlation `0.765350` against the known logo).
2. PCSX saved the naturally reached state through `/api/v1/state/save?name=retail-title-natural`:
   - `SLUS00664/retail-title-natural.sstate`, 1,108,777 bytes
   - SHA-256 `4b879798e21fcf8e77dcda028fde11bccdc9caa695fbcff5c431bc91f1fa7f15`
3. `api-title-detected-011412.png` records the title, SHA-256 `626866bc6287f4861df0a4e6e1e2cb40436fabe3656070f29b34a7a1c77e20ba`.
4. A Lua Start press/release left the title unchanged. A Lua Cross press/release exposed the retail `New Game / Continue / Sound` menu with the cursor at Continue:
   - `api-after-cross-011412.png`, SHA-256 `b8aa831ce992a40817d36f9f29fdfba2113b5c19e631ed0a822529ead9d89db2`
5. A second Cross dismissed the menu, consistent with the source-backed cancel binding. This is retained as a positive API-controller-path control.
6. The natural title state was loaded through `/api/v1/state/load?name=retail-title-natural`, and Cross opened the menu again. An attempted Circle API call returned `URL Not found.` because the isolated helper had no Circle handler; no input reached the game in that attempt.
7. For the one authorized native retry, the title state was loaded again, Cross opened the menu, the exact retail PID/window were validated, and one ordinary compositor keyboard `D` event was injected through the isolated `/dev/uinput` `ydotoold`:
   - key down: 2026-09-06 01:19:11.780150383 UTC
   - key up: 2026-09-06 01:19:12.814558712 UTC
8. `retry2-menu-before-circle.png` and `retry2-after-circle-3s.png` are byte-identical, both SHA-256 `b8aa831ce992a40817d36f9f29fdfba2113b5c19e631ed0a822529ead9d89db2`. This proves the visible menu did not respond within three seconds.
9. `retry2-after-circle-11s.png` is black during the transition, SHA-256 `da57286ecf4f118c1d7e8fae5b9ea5098da9d0ebb3328e54a0e489a788838ede`. At 26 seconds, `retry2-after-circle-26s.png` shows the opening movie, SHA-256 `5c1a24af8be1a8271a58aecf0348a548c6836b3cbb3bc8c62dc31782bb198c6b`.
10. Dense API framebuffer capture followed the complete movie. At 2026-09-06 01:30:11 UTC, `dense-newgame/1265-013011.377030.png` returned to the same Xenogears title framebuffer and the same SHA-256 `626866bc...`. Subsequent frames showed the early opening movie again, including `DIGITAL QUANTITY / COUNTING` in `poll-current/10.png` and the bridge commander in `dense-newgame-2/0223-013246.565781.png`. This establishes an ordinary attract-loop restart, not New Game progress.

## Result and next source question

- New Game opening Gear-panel capture: **NOT OBSERVED / UNRESOLVED**
- Exact visible Fei text: **NOT OBSERVED**; `Hiyaaaaa!` remains expected reference wording only
- Naturally reached Gear-panel savestate: **NOT CREATED**
- Naturally reached title savestate: **CREATED AND PRESERVED**

The selection-redraw timing hypothesis is ruled out by the final control below: a fresh Circle edge sent after the New Game selection had remained stable for more than three seconds still produced no visible response. The remaining source/runtime question is whether PCSX raw override bit 13 reaches the emulated controller packet and whether retail `ControllerPoll` records/enqueues the resulting game-mask `0x20` press edge. A future attempt should observe that path without changing game RAM, using the preserved natural-title state and isolated process/profile. No further movie-loop attempt was made.

## Live endpoint investigation

The follow-up source audit found no API-only mechanism to register the missing handler in the running Lua VM:

- PCSX `src/core/web-server.cc:306-405` implements `/api/v1/lua/<name>` only by looking up an existing `PCSX.WebServer.Handlers[name]` function and calling it. It exposes no Lua evaluation, `dofile`, or handler-registration route.
- `src/gui/gui.cc:523-543` provides Lua evaluation through the interactive Lua console, and `src/gui/widgets/luaeditor.cc:50-92` provides it through the interactive editor. Both require desktop UI, which was relinquished to the concurrent native run.
- Editing `retail-pad-api.lua` alone cannot affect the already-running Lua table; command-line `--dofile` is processed only during startup at `src/main/main.cc:433-456`.

The restart was explicitly authorized and performed after preserving `service.before-circle-restart.txt`, both Circle-restart journals, `pcsx.before-circle-restart.json`, and `retail-pad-api.before-circle.lua`. The Circle-only helper launched as PID `3069504`. Because the visible default was Continue, a second authorized restart preserved both direction-restart journals, `pcsx.before-direction-restart.json`, and `retail-pad-api.before-direction.lua`, then added Up/Down and launched PID `3106417`.

## Verified New Game selection and Circle negative control

- `api-newgame-retry3-menu-continue.png`, SHA-256 `b8aa831ce992a40817d36f9f29fdfba2113b5c19e631ed0a822529ead9d89db2`, shows the default cursor beside Continue.
- One source-backed Up pulse changed the cursor to New Game. `api-newgame-retry3-menu-selected-paused.png`, SHA-256 `d69af166a229bb7f0217139014248eccfb0bee634a5470c93cf8e6cb8b4db40d`, is the exact paused verification frame.
- The final uninterrupted timing control loaded the natural title at 2026-09-06 01:55:58.123 UTC, pulsed Cross at `01:55:59.226-59.556`, pulsed Up at `01:56:02.087-02.419`, and held Circle from `01:56:03.237` to `01:56:05.267`.
- `api-hold-menu-newgame.png`, `api-hold-after-circle-1s.png`, and `api-hold-after-circle-3s.png` are byte-identical, all SHA-256 `d69af166a229bb7f0217139014248eccfb0bee634a5470c93cf8e6cb8b4db40d`.
- Exact actions are in `api-newgame-circle-hold.jsonl`; earlier timing diagnostics are in `api-circle-retry.jsonl`, `api-newgame-retry.jsonl`, and `api-newgame-timed.jsonl`.
- The strongest final control loaded the natural title at `02:00:15.996773106Z`, pulsed Cross at `02:00:17.098621965-02:00:17.428895426Z`, pulsed Up at `02:00:19.966039741-02:00:20.294247362Z`, waited `3.066` seconds with New Game visibly selected, then sent one fresh Circle press/release at `02:00:23.360661617-02:00:23.689205957Z`.
- `api-settled-menu-newgame-before-circle.png`, `api-settled-after-circle-1s.png`, and `api-settled-after-circle-3s.png` are byte-identical, all SHA-256 `d69af166a229bb7f0217139014248eccfb0bee634a5470c93cf8e6cb8b4db40d`.
- The four-record action log `api-newgame-settled-circle.jsonl` is valid JSONL and has SHA-256 `dc9320bf8453b5bc79de88d81afcd0fc8f0501cba53c8cd04579dc4fb85b166c`.

An earlier provisional status interpreted an `I am Alpha...` movie frame as New Game progress. Later dense capture showed that sequence loop back to the title, and source/visual review showed the first Circle was on Continue and later Circle attempts occurred after timeout. That provisional claim is explicitly retracted; the authoritative result is the byte-identical New Game selection negative control above.
