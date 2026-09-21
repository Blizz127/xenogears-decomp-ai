# Xenogears development container

This is the reproducible OVH development environment. It mounts only this
repository at `/home/blizz/Projects/xenogears-decomp` and runs as the host
user, so build outputs are not root-owned.

From the repository root:

```sh
tools/ovh/container-up
tools/ovh/matching-build
tools/ovh/native-build
```

The legal disc, BIOS, emulator, captures, and research artifacts remain
outside Git. The container does not mount the host home directory or Docker
socket.
