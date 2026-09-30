#!/usr/bin/env bash
# Dev menu controller / input-capture test (pc_port/tests/dev_menu_test.cpp):
# SDL virtual pads through the patched PsyX_pad.cpp and port_dev_menu.c.
# Needs the patched PsyCross tree (pc_port/build_port.sh); run in xenogears-dev.
set -euo pipefail
cd "$(dirname "$0")/../.."
PSX=pc_port/extern/PsyCross
grep -q 'PcPort_DevMenuPad' "$PSX/src/pad/PsyX_pad.cpp" || {
    echo "FAIL: $PSX/src/pad/PsyX_pad.cpp lacks psycross_pad_select.patch (run pc_port/build_port.sh)"; exit 1; }
# The toolbar strip is gone: nothing reserves or subtracts window pixels.
if grep -rq 'PC_PORT_HOST_TOOLBAR_HEIGHT' pc_port/src pc_port/patches; then
    echo "FAIL: PC_PORT_HOST_TOOLBAR_HEIGHT is still referenced"; exit 1; fi
grep -q 'extern "C" int PsyX_HostToolbarReservedHeight(void) { return 0; }' pc_port/src/psycross_host_toolbar.inl
if grep -q 'ANALOG: ON' "$PSX/src/pad/PsyX_pad.cpp"; then
    echo "FAIL: PsyCross Select+Start analog toggle still present"; exit 1; fi

OUT=$(mktemp -d "${TMPDIR:-/tmp}/dev-menu.XXXXXX")
trap 'rm -rf "$OUT"' EXIT
SDL_CFLAGS=$(pkg-config --cflags sdl2)
INC=(-include pc_port/src/port_compat.h -I"$PSX/include" -I"$PSX/include/psx" -I"$PSX/src" -Ipc_port/src)
g++ -std=gnu++17 -O1 -g -Wall -Wno-narrowing -fpermissive "${INC[@]}" $SDL_CFLAGS \
    -c "$PSX/src/pad/PsyX_pad.cpp" -o "$OUT/PsyX_pad.o"
g++ -std=gnu++17 -O1 -g -Wall "${INC[@]}" $SDL_CFLAGS -c pc_port/tests/dev_menu_test.cpp -o "$OUT/test.o"
gcc -std=gnu17 -O1 -g -Wall -Wextra -Ipc_port/src -c pc_port/src/port_dev_menu.c -o "$OUT/menu.o"
gcc -std=gnu17 -O1 -g -Wall -Wextra -Ipc_port/src -c pc_port/src/port_dev_menu_warps.c -o "$OUT/warps.o"
gcc -std=gnu17 -O1 -g -Wall -Wextra -Ipc_port/src -c pc_port/src/port_dev_menu_steps.c -o "$OUT/steps.o"
g++ "$OUT/test.o" "$OUT/PsyX_pad.o" "$OUT/menu.o" "$OUT/warps.o" "$OUT/steps.o" $(pkg-config --libs sdl2) -o "$OUT/dev_menu_test"
SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy timeout 60s "$OUT/dev_menu_test"
