# W34N126 — retail boot path: movie state 6, opening STR, hand-off to Map 0

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `6589917f` (docs: log W34N125)
- Commits: `6432959f` (split configs), `a177ca62` (movie.bin decomp),
  `f03b1747` (SLUS BSS symbols), `e7a4b4f6` (split boundary fixes),
  `7991642c` (PsyCross CD streaming + display present), `3d107600` (movie
  player module port), `847520e9` (boot wiring), `34219225` (overlays.yaml
  comment)
- Date: 2026-09-02
- Verdict: **OPENING_MOVIE_PLAYS_ON_RETAIL_PATH; TITLE_SCREEN_ON_MAP0_NOT_YET_VERIFIED**

## What retail does at boot (asm authority)

`func_80019578` (asm/slus_006.64/nonmatchings/main/main), after
`GameShowSplashScreen`:

```text
80019888: g_CurGameStateOverlayID = -1; g_CurGameStateOverlayBuffer = 0
800198A4: D_8004FE44 = 1     ; movie number
800198AC: D_8004FE46 = 1     ; game state to enter afterwards (Field)
800198B4: D_8004FE47 = 0     ; skipping allowed
800198B8: D_8004FE45 = ArchiveGetDiscNumber()
80019924: func_8001B6BC()    ; empty
8001992C: ChangeGameState(6)
80019934: MainLoop(0)
```

and, earlier in the same function (0x80019870), `func_8001BB50` →
`func_8001B970`: the new-game template (archive 0x10 file 3, 0x2358 bytes)
is copied over `g_GameState`.  0x2358 > sizeof(GameState) 0x2300, so the
copy also writes `D_8006F94E/50/52/54` (offsets 0x231A..0x2320) — the
field transition tuple that `FieldMain` reads.  The disc-1 template holds
map 0, entrance 0, camera 0.

State 6 is `movie.bin` (archive 0x12; table entry at SLUS `0x8001808C+0x60`:
main `0x800737EC`, BSS `0x80076F38`, heap `0x80077454`).  Its `MovieMain`
places the movie player module (archive directory 0x18 file 1, 0x16000
bytes) at `0x801D3000` by sizing a top-of-heap block from a 4-byte probe
(`HeapAlloc(4,1)` then `HeapAlloc((probe & 0xFFFFFF) - 0x1D3008, 1)`),
plays movie `D_8004FE45 + 2` of directory 0x18/1 (frames 1..233, channel
1, 320x240, 24-bit) and returns to `ChangeGameState(D_8004FE46)`.

There is no title/menu game state.  The title screen must therefore be the
Map 0 field script.

## What the port did before

`PcPort_BootMain` (a hand-written title → software STR → New Game/Continue
stand-in) as state 0, then `PcPort_ApplyNormalBootFieldDefaults` forcing
Map 14.  Both bypassed by this rung on the normal lane.

## What was ported

| piece | source | status |
|---|---|---|
| `movie.bin` | `src/movie/main.c` | shipping path + developer menu transcribed; CD-ROM CHECK / MONITOR / DISC CHANGE tools are labeled placeholders |
| movie player module, Square layer | `pc_port/src/movie_player.c` | transcribed (func_801D30C4 … func_801D43B0) |
| module cdstream ring | same | transcribed; sector source is PsyCross `CdGetSector` |
| module `DecDCTvlc` | same | handwritten asm transcribed register-for-register; tables read from the archive at first use |
| module MDEC (`DecDCTin/out`) | same | software (psx-spx RL/IDCT/YUV with the module's IQ/scale tables); completion delivered on the next pump |
| PsyCross CD | `patches/psycross_cd_stream_movie.patch` | CdlSetmode / CdlReadS(NULL) / CdlSetfilter / CdlPause, spooler paced at 75/150 sectors/s |
| PsyCross display | `patches/psycross_display_present.patch` + `psyq_compat.c` Vsync | present the DISPENV VRAM area (15/24-bit) at a blocking Vsync(0) when no primitives were drawn |
| boot | `port_main.c`, `game_overrides.c` | retail tail above; `func_8001BB50` after archive init; state 6 bound to `MovieMain`; state 0 = `KernelMenuMain` |

## Runtime evidence

`scratchpad/w34n126_boot_movie/run_movie_diag.log` (`XENO_MOVIE_DIAG=1`,
plain run, no gdb):

```text
[movie-diag] pumps=60   lastframe=5   shown=5   ... skips=0
[movie-diag] pumps=1200 lastframe=120 shown=120 ... skips=0
[movie-diag] pumps=2280 lastframe=228 shown=228 ... skips=0
[xeno-port][boot] retail boot: movie state 6 (movie 1, disc 1) -> state 1
[FieldMain] g_pGameState=0x979d18 g_GameSceneMapNum=0
```

6 frames per 60 pumps at 3 pumps/vblank ≈ 18 fps with the spooler paced at
150 sectors/s; 233 frames, zero skipped frames (`D_801E89D4`).

Captures (`XENO_MOVIE_CAPTURE_DIR`, `scratchpad/w34n126_boot_movie/`):

| frame | sha256 (BMP) |
|---:|---|
| 10 | `a2210e918f9e3052e677e29e9807c31265dc6f031e54756eff7cff4cc96861c5` |
| 40 | `2ff705efa3e8119c5efef69db2437f36b7e4e37a2e0f1c9f9cba3ae42aebf241` |
| 80 | `fdde2e56a1bf7a64736b8158812915f6c72c217b870e25cd8b75e8a31bf5c87a` |
| 120 | `2ecfaa1b1a431e915864addb614d45229e7faa650a55798f827b8f66f0166ea6` |
| 160 | `43f6e3dc3e0a91039b72e3400e1b3c432a858bc0f63bbf9be9216ebb4f5fc3c6` |
| 200 | `724540f19c8b8e54cc4092b5249e1fe9ce5cf7c3e1189733f841582039ad4ede` |
| 230 | `55e5ee31006d6347ebff71fa9d7884368daf19fee931dd4b695d462cc8d3b8e5` |

Frames 80/160/230 show the anime opening (storm clouds, Fei's face, Grahf
in red and Fei in the rain) in 24-bit colour.  Frame 80 shows faint
vertical striping in the dark sky; whether that is the source material's
rain, MDEC rounding or dithering is not established.

After the movie the port enters `FieldMain` map 0; `map0-last.png` shows
the map 0 script running (five sprite actors and a cow in a dark room,
"down / no" text) — the same scene the July Map 0 harness runs showed.

## Not established / open

- **Title screen.** Retail reaches map 0 the same way, yet the port's map 0
  looks like a debug room.  Whether retail's map 0 script shows the title
  menu (via the menu overlay, `src/menu/main/misc.c` case 9 is New Game)
  and the port diverges inside that script, or the title is elsewhere, is
  the next question.  No claim is made about the New Game / opening field
  scenes ("fire scene").
- **Audio.** XA-ADPCM movie audio is not decoded; the audio sectors are
  discarded by the 0x160 check exactly where the retail SF filter would
  drop them.
- **MDEC fidelity.** The software MDEC follows the psx-spx description, not
  a bit-exact hardware model.
- **Harness lane.** `XENO_FIELD_TEST=1` keeps the roster / func_8001ACA4
  stand-ins and does not run `func_8001BB50`; it deviates from retail and
  W34N124 depends on that (6/6 still passes).
- `PcPort_BootMain`, `boot_menu.c`, `boot_str.c`, `boot_assets.c` are
  unreferenced on the normal lane (still used by the boot certificate).
- `make check`: 4/4 FAILED with identical hashes throughout
  (`34e552c6… / af1e5dc3… / 242c5450… / 0f22fa9a…`); the only make-visible
  change was the two BSS symbol names (`f03b1747`).
