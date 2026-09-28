# Third-party code in the native PC port

The port (`pc_port/`) ships no Sony Psy-Q SDK code and no game data. Its PSX
hardware layer is PsyCross, plus the port's own `pc_port/src/psyq_compat.c`.

## PsyCross

| | |
|---|---|
| Upstream | https://github.com/OpenDriver2/PsyCross |
| Pinned commit | `e56e4cde1c2b8a15e0d4e38b26cdd9202e0d17e6` (2026-06-20, "relative cursor movement") |
| Machine-readable pin | [`psycross.lock`](psycross.lock) |
| Location | `pc_port/extern/PsyCross` (gitignored; not redistributed in this repo) |
| Local changes | `pc_port/patches/*.patch`, applied by `pc_port/build_port.sh` |
| License | MIT (full text below) |

`pc_port/build_port.sh` clones PsyCross at the pinned commit when
`pc_port/extern/PsyCross` is absent (`XENO_PSYCROSS_URL` overrides the clone
source, e.g. a local mirror). When the tree exists it must be a git checkout
whose history contains the pinned commit, with only local
`vendor baseline for patch apply` commits on top; otherwise the build stops.
Set `XENO_PSYCROSS_ALLOW_UNPINNED=1` to bypass that check while testing a new
pin, then update `psycross.lock` and this file together.

The patches in `pc_port/patches/` are modifications of PsyCross and are
distributed under the same MIT license.

### License text (from PsyCross `LICENSE` at the pinned commit)

```
MIT License

Copyright (c) 2020 REDRIVER2 Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
