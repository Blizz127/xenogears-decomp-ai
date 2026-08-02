# OVH Xenogears development

The tracked container is `dev/xenogears/`. Build it with
`tools/ovh/container-up`; it is Ubuntu 24.04, mounts only this repository at
`/home/blizz/Projects/xenogears-decomp`, and uses the invoking user's UID/GID.

Commands are fixed wrappers: `container-up`, `container-down`,
`matching-build`, `rom-check`, `native-build`, `native-smoke`, and
`retail-validation`. They accept no arbitrary emulator or shell arguments.

Retail validation uses the OpenBIOS-compatible retail BIOS at
`disc/scph5500.bin`, `disc/disc1.bin`, and the PCSX-Redux build 293 AppImage at
`/home/blizz/apps/pcsx-redux/build293-3e10093a/`. Its required SHA-256 is
`b27a564e6c32333453433c950ac26e652f40af32d5e236441f36e8123c9c651b`.

Disc images, BIOS, the AppImage, PsyCross, scratchpad files, captures, and
other research artifacts are local assets and are not committed. Rebuild
from source with `container-up`, then run the matching and native wrappers.
