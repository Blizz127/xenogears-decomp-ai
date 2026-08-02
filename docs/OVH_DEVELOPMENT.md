# OVH Xenogears development

The tracked container is `dev/xenogears/`. Build it with
`tools/ovh/container-up`; it is Ubuntu 24.04, mounts only this repository at
`/home/blizz/Projects/xenogears-decomp`, and uses the invoking user's UID/GID.

Commands are fixed wrappers: `container-up`, `container-down`,
`matching-build`, `rom-check`, `native-build`, `native-smoke`, and
`retail-validation`. The native wrapper invokes the repository's complete
`pc_port/build_port.sh` driver, including game translation units, port-only
sources, and typed generated stubs; the standalone CMake target is only the
PsyCross scaffold and is not the native validation route. They accept no
arbitrary emulator or shell arguments.

Retail validation uses the OpenBIOS-compatible retail BIOS at
`disc/scph5500.bin`, `disc/disc1.bin`, and the PCSX-Redux build 293 AppImage at
`/home/blizz/apps/pcsx-redux/build293-3e10093a/`. Its required SHA-256 is
`b27a564e6c32333453433c950ac26e652f40af32d5e236441f36e8123c9c651b`.

Disc images, BIOS, the AppImage, PsyCross, scratchpad files, captures, and
other research artifacts are local assets and are not committed. Rebuild
from source with `container-up`, then run the matching and native wrappers.

## Native-link validation

`tools/ovh/native-build` uses `pc_port/build_port.sh`, the complete native
driver. The standalone `cmake -S pc_port` target is only a PsyCross scaffold
and omits the game translation units. The driver compiles 47 game units and
the port-only sources, generates typed stubs in the disposable native build
directory, and links `pc_port/build_native/xeno-port`.

The recovery validation produced `LINK OK` on clean and incremental runs.
The bounded native smoke and existing F14/F16 GDB proofs pass. Matching build
passes; ROM-check continues to report the four documented known-red retail
mismatches. Those are matching/decompilation drift, separate from the native
host link.
