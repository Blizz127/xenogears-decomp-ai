# Xenogears VPS Desktop Development Workflow

## Overview

The Xenogears PC-port development environment runs on an OVH VPS with a
graphical XFCE4 desktop session accessed via xrdp. This document describes
how to build, launch, debug, and capture the native port alongside PCSX-Redux
for retail comparison.

**Key principle:** Docker is used for builds and CI only. The graphical
desktop session is used for visual debugging, retail comparison, and
development workflow.

## Graphical Session Requirements

- **Desktop:** XFCE4 via xrdp
- **Display:** `:10.0` (X11, not Wayland)
- **Remote access:** RDP client (any standard RDP client connects to xrdp)
- **OpenGL:** Mesa llvmpipe (software renderer) — OpenGL 4.5 Core Profile
- **No Xvfb:** Visual validation uses the real desktop session, never Xvfb

## Directory Layout

```
/home/blizz/dev/xenogears-decomp/          # Repository checkout
  pc_port/build_native/xeno-port           # Native binary (built via Docker)
  pc_port/build_port.sh                    # Authoritative build driver
  pc_port/extern/PsyCross/                 # PsyCross (git submodule)
  disc/                                    # Disc images, BIOS, extracted bins
  tools/vps-desktop/                       # Launch wrappers
  tools/vps-desktop/templates/             # Desktop launcher templates
  tools/ovh/                               # Docker container wrappers
  .env.local                               # Local paths (NOT committed)

/home/blizz/dev/xenogears-assets/          # External assets (NOT in repo)
  lib/                                     # Extracted SDL2/OpenAL runtime libs
  logs/                                    # Run logs
  captures/                                # Screenshots and videos

/home/blizz/apps/pcsx-redux/               # PCSX-Redux AppImage (NOT in repo)
```

## Dependencies

### Build (inside Docker container `xenogears-dev:24.04`)

The container provides: gcc, g++, cmake, ninja, pkg-config, python3,
libsdl2-dev, libopenal-dev, libgl1-mesa-dev, gdb, binutils.

### Runtime (on VPS host)

The VPS host does not have SDL2/OpenAL packages installed. Instead, the
required shared libraries are extracted from the Docker container into
`/home/blizz/dev/xenogears-assets/lib/` and loaded via `LD_LIBRARY_PATH`.

Extracted libraries:
- `libSDL2-2.0.so.0` (2.30.0)
- `libopenal.so.1` (1.23.1)
- `libdecor-0.so.0` (optional, Wayland decoration)
- `libsndio.so.7` (optional, sndio audio backend)

All other transitive dependencies (X11, PulseAudio, ALSA, etc.) are provided
by the VPS desktop environment.

## Build Command

```bash
# Build inside the Docker container
docker exec xenogears-dev bash -c \
  'cd /home/blizz/Projects/xenogears-decomp && bash pc_port/build_port.sh'

# Or use the OVH wrapper
tools/ovh/native-build
```

The build produces `pc_port/build_native/xeno-port` (ELF 64-bit, dynamically
linked). Verify with:
```bash
sha256sum pc_port/build_native/xeno-port
ldd pc_port/build_native/xeno-port
file pc_port/build_native/xeno-port
```

## Native Launch Command

```bash
# Default launch (all accuracy options off)
tools/vps-desktop/run-xeno-port

# With PS1 accuracy corrections (experimental)
tools/vps-desktop/run-xeno-ps1-accurate

# With individual switches
XENO_PS1_HALF_PIXEL_ORIGIN=1 tools/vps-desktop/run-xeno-port
XENO_PS1_TEXEL_CENTER=1 tools/vps-desktop/run-xeno-port
XENO_PS1_FIXED_UV_INTERPOLATION=1 tools/vps-desktop/run-xeno-port
```

## GDB Test Routes

### Lahan Field Walk
```bash
tools/vps-desktop/run-xeno-lahan
```
Uses `scratchpad/walk.gdb` — automated walk through Lahan with periodic GL
backbuffer captures at key frames.

### Map014 Visual Review
```bash
tools/vps-desktop/run-xeno-map014
```
Uses `scratchpad/map14_visual_review.gdb` — multi-frame capture of the Map014
painting sequence (DR_MOVE ordering, FT4 dispatch, CLUT data).

### Genuine Exit Test
```bash
tools/vps-desktop/run-xeno-exit
```
Runs until the exit tuple `(0x0400, 0x0E00, 1, 1)` is reached or times out
(60s default, configurable via `XENO_EXIT_TIMEOUT`).

## PCSX-Redux (Retail Comparison)

```bash
tools/vps-desktop/run-pcsx-redux
```

- AppImage: `/home/blizz/apps/pcsx-redux/build293-3e10093a/`
- SHA-256: `b27a564e6c32333453433c950ac26e652f40af32d5e236441f36e8123c9c651b`
- Disc: `disc/disc1.bin`
- BIOS: `disc/scph5500.bin` (optional — OpenBIOS fallback available)

Native and retail windows can run side-by-side in the same desktop session
for visual comparison.

## Screenshot Method

The VPS desktop provides `xfce4-screenshooter` for screenshots:
```bash
xfce4-screenshooter -f -s /path/to/screenshot.png
```

The existing capture script (`pc_port/tools/run_xeno_capture.sh`) also
supports: grim, spectacle, gnome-screenshot, scrot, import.

## Video Method

OBS Studio or ffmpeg X11 capture:
```bash
ffmpeg -f x11grab -video_size 640x480 -i :10.0+1733,1069 \
  -c:v libx264 -preset ultrafast capture.mp4
```

## Cursor Workflow

```bash
tools/vps-desktop/open-project
```

Cursor opens `/home/blizz/dev/xenogears-decomp`. The terminal inherits the
graphical environment, so launch scripts work directly from Cursor's
integrated terminal.

## External Asset Policy

The following are local assets and are **never** committed:
- Disc images (`disc/*.bin`, `disc/*.chd`)
- BIOS files (`disc/scph5500.bin`)
- PCSX-Redux AppImage
- PsyCross checkout (`pc_port/extern/PsyCross/`)
- Screenshots, captures, videos
- Scratchpad diagnostics
- `.env.local`
- Run logs
- Extracted runtime libraries

## Docker Role

Docker (`xenogears-dev:24.04`) is used for:
- **Builds:** Compiling the native binary with all required dev packages
- **CI:** Matching builds, ROM checks, reproducible validation
- **Not for:** Graphical display, visual debugging, retail comparison

## Graphical Desktop Role

The XFCE4 xrdp desktop session is used for:
- **Visual debugging:** Seeing the native port window with real OpenGL
- **Retail comparison:** Running PCSX-Redux alongside the native port
- **Development:** Cursor IDE, terminal, file manager, screenshot tools
- **Not for:** Headless CI or automated builds (those use Docker + Xvfb)
