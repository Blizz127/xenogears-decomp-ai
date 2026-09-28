# Port architecture (moddable platform layer)

This follows the shape of the Parasite Eve port's `docs/ARCHITECTURE-PORT.md`
(owner decision, 2026-09-28: the ports on this dev box share one standard)
and adapts it to Xenogears. The goal is a port people can customise: better
graphics, mods, cheats.

## Layers

```
 game C (matched decomp TUs in src/ + hand ports in pc_port/src)
   │  still calls the PSX SDK interface (libgpu/libgte/libspu/libcd names),
   │  implemented by PsyCross + pc_port/src/psyq_compat*
   │  port-side code calls the xg_plat_* interfaces
 ┌─┴──────────────────────────────────────────────────────────────┐
 │ platform interfaces  pc_port/include/xg_plat/*.h                │
 │  disc/storage · renderer · audio · input · timing · config · mods│
 └─┬──────────────────────────────────────────────────────────────┘
   │  one backend per interface today; swappable at link time
 backends: PsyCross (MIT, pinned, fetched, not committed — pc_port/THIRD_PARTY.md)
           · in-house (retail_data.h, disc_check.c, src/plat/*)
```

Rules:

1. New port code reaches the host only through `xg_plat_*`. The game's own
   PSX SDK calls move behind the interfaces as each area is touched; until
   then they go to PsyCross and the clean-room `psyq_compat*` layer.
2. Interfaces are defined by meaning, not by PS1 registers (window size,
   key names, speed multiplier, "the user's retail file X"), so a modern
   backend can replace PsyCross without touching game code.
3. No Sony Psy-Q code or tables in the port (`tools/analysis/psyq_port_guard.py
   --strict`). No game data in git or in any shipped file: everything is read
   from the user's disc at run time and checked against the retail hashes
   (`tools/analysis/retail_data_guard.py`).
4. A backend change must keep the port building, both guards green and the
   title smoke (`pc_port/tests/run_title_newgame_smoke.sh`) no worse than the
   recorded baseline.

## Interfaces

| Interface | Covers | Backend now |
|---|---|---|
| `disc.h` | Disc 1 image, retail files (`SLUS_006.64`, overlay `.bin`s), BIOS, sector reads, Redump / checksum verification | stdio + `retail_data.h` + `disc_check.c`; weak, pulled in by the header so every TU/test links it; a strong backend overrides it |
| `renderer.h` | output window size (= internal resolution with PsyCross), fullscreen, vsync, filtering; capability query (`widescreen`) | PsyCross |
| `audio.h` | user output volume | none yet: PsyCross's master volume is the game's own `SpuSetCommonAttr` value |
| `input.h` | keyboard bindings by SDL key name | PsyCross keyboard mapping |
| `timing.h` | speed multiplier, fast-forward (hold) and its speed, vblank clock | PsyCross host speed |
| `config.h` | `config.ini`, `--config` / `--set`, `XENO_*` overrides | `src/plat/xg_plat_config.c` |
| `mods.h` | `mods/` folders, manifests, events, C plugins, asset replacement | `src/plat/xg_plat_mods.c` |

The storage interface came first because the run-off-the-disc loader
already centralised it; timing and input followed.

## Configuration

`config.ini` next to the `xeno-port` executable or in
`~/.config/xenogears-port/`, or `--config FILE`. Keys (see
`pc_port/config.example.ini`):

| Key | Meaning | PsyCross backend |
|---|---|---|
| `video.internal_resolution` | N × 320×240 | yes (renders at the window size) |
| `video.width`, `video.height`, `video.fullscreen`, `video.vsync`, `video.bilinear` | output | yes |
| `video.widescreen` | 16:9 view | reported unsupported, 4:3 used |
| `video.fps` | 30 (native) or 60 | 60 reported unsupported (needs interpolation) |
| `game.speed`, `game.fast_forward_speed` | 1..5 | yes |
| `video/audio/input.backend` | backend choice | `psycross` only |
| `input.<button>` | SDL key name | yes |
| `mods.enabled`, `mods.dir` | mod loading | yes |
| `cheats.console` | stdin cheat console | yes |

Precedence per key: `--set key=value` > `XENO_<SECTION>_<KEY>` environment
variables (the existing `XENO_*` names stay aliases, e.g. `XENO_SPEED`) >
the file. Options a backend cannot honour are reported once and ignored,
never faked.

## Mods

- Folders under `mods/` (or `mods.dir`) and
  `~/.local/share/xenogears-port/mods/`, each with a `mod.txt` manifest
  (`name`, `version`, `load_order`, `abi = 1`). Lower load order loads
  first; later mods override earlier ones.
- **Asset replacement** keyed by the SHA-256 of the original bytes the
  archive loader read from the user's disc: `assets/<sha256>.bin`. Served
  only from mod folders; replacements larger than the game's buffer are
  refused. `XENO_MODS_LOG_ASSETS=1` prints each loaded file's hash;
  `XENO_MODS_DUMP_DIR=dir` saves the originals (from your own disc) for
  editing. Mod folders are user content and are git-ignored.
- **Hooks** (named events): `boot`, `frame_tick` (after each presented
  frame), `asset_loaded`, `room_enter` (field map number). With no
  subscriber every emit is a no-op, so default play is unchanged.
- **C plugins**: optional `plugin.so` exporting
  `int xg_mod_init(const XgPlatModApi*)` (subscribe / unsubscribe / log),
  versioned by `XG_PLAT_MODS_ABI_VERSION`.

## Cheats

`pc_port/src/cheat_console.c` registers the cheats as named commands on the
hook layer: `god`, `encounters`, `save` / `load` (quick checkpoint),
`warp <map> <x> <z>`, `battle <id>`, `speed <n>`, `mods`, `help`. Commands
queue from any thread (host toolbar and hotkeys, or stdin with
`cheats.console = on`) and run on the game thread from `frame_tick`. The
per-frame cheat services (field warp, battle warp) are `frame_tick`
subscribers rather than calls in the VSync shim. Test-harness switches
(`XENO_GOD_MODE`, `XENO_FIELD_WARP`, `XENO_BATTLE_WARP_FILE`, …) keep
working and stay off by default.

## Migration order

1. Interfaces + PsyCross backend: disc/storage, then timing and input
   (done), renderer/audio setup (done).
2. Config file and CLI (done); mods loader, asset replacement, first hooks
   (done); cheat console on the hooks (done).
3. Move the game's own SDK calls (drawing, sound, CD) behind the
   renderer/audio/disc interfaces as those areas are worked on; extend asset
   replacement to streamed/movie data.
4. A modern renderer backend (widescreen, 60 fps interpolation, texture
   replacement) only needs the renderer interface.

Credit: structure and policy adapted from the Parasite Eve decompilation's
`docs/ARCHITECTURE-PORT.md`.
