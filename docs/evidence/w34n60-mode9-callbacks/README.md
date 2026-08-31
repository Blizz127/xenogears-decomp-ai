# W34N59/W34N60 — retail mode-9 scheduler callbacks

## Scope and anchors

- Starting HEAD: `cf6da6f492edd6db823d35460b835d4562ef83f2`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail source: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Mode-9 callback boundary: `[0x80077DC8, 0x80078948)`
- Dependency leaves:
  - `[0x80076858, 0x80076954)` — fixed-point three-vector blend
  - `[0x80097070, 0x80097244)` — matrix-to-Euler decomposition
  - `[0x80094154, 0x800941C4)` — planar X/Z distance

The focused runners pin the exact retail slices before compiling:

| Retail slice | SHA-256 |
|---|---|
| `0x80077DC8..0x80078948` | `c43efadac3b27b4f4d702a2e88f6ad57a9e1fe00f461002640460395b3e9e417` |
| `0x80076858..0x80076954` | `2e0f9e5ebbb78a734f16e8565b669194f7e8716346684578f6d7ccd40d17d5db` |
| `0x80097070..0x80097244` | `4d0fbc4c927d74b78faee039c3996b038668f7909c4e60974ad6340451ab85d8` |
| `0x80094154..0x800941C4` | `b36aa7c6085cff6f1e273998a83c96be927bfdfcee291f24e9706a05f66821f1` |

## Production behavior restored

The scheduler now resolves both mode-9 callback pairs directly:

- `0x80077DC8 / 0x80077E68` — path/camera sequence
- `0x8007828C / 0x800783E8` — world-context sequence

`wm_80077DC8` initializes the retail camera/path state, starts sound bank
entry `0xA4`, and installs the 24-tick initial delay. `wm_80077E68` samples
the retail path table, builds and publishes the camera matrix, executes all
seven path states, updates the sound source using signed X/Z halfwords, and
claims the terminal world transition.

`wm_8007828C` links context record zero to records 1 through 13 in exact
order and seeds the six angular motion fields. `wm_800783E8` advances the
position and angles, builds four retail rotation matrices, then copies their
full 32-byte records through the exact context chains.

The pre-certificate retail audit caught and corrected four implementation
hazards before they entered a passing build:

- the recurring Z decrement targets `0x8009BBBC`, not `0x8009BBB4`;
- the sound-distance vectors load signed halfwords, not packed words;
- the fourth rotation input explicitly clears its X/Y fields; and
- the C/D/A/B context matrix chains contain no self-copy or swapped source.

## Focused certificates

`pc_port/tests/run_w34n59_mode_path_math.sh`:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- fixed-point blend, zero guard, `ratan2` argument order, base-matrix use,
  final negation, input read-only behavior, and planar X/Z distance: PASS
- M1–M6: all detected by named assertions

`pc_port/tests/run_w34n60_mode9_callbacks.sh`:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- direct callback state, arguments, path transitions, sound vectors,
  13-link setup, four rotation inputs, all matrix chains, and scheduler
  guest-address resolution: PASS
- M1 wrong render-Z address: detected by `context.render_z`
- M2 packed-word sound-vector load: detected by `path.sound_vectors`
- M3 missing terminal exit: detected by `path.terminal_exit`
- M4 missing thirteenth context link: detected by `context.link_count`
- M5 wrong matrix source: detected by `context.matrix_d`

## Regression and build gates

- W34B68 second-scheduler certificate: PASS
- W34C1 scheduler cadence certificate, M1–M21: PASS
- W34N56 mode-8/mode-11 callback certificate: PASS
- W34N57 mode-8/mode-11 lifecycle certificate: PASS
- W34N58 shared-mode draw certificate: PASS
- canonical `./pc_port/build_port.sh`: LINK OK; port-owned addresses verified
- `git diff --check`: clean

## Runtime bound and next dependency

The accepted base and mode-8/mode-11 routes do not register these callbacks,
so this change is runtime-neutral on those paths. Mode 9 still lacks its
retail lifecycle setup/teardown integration at `0x80077A64` and `0x80077CC0`;
that is the next bounded production target. Natural mode-9 visual acceptance
belongs to that lifecycle slice rather than to synthetic callback dispatch.
