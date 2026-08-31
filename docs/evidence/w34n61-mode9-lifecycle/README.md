# W34N61 — retail mode-9 lifecycle

## Scope and anchors

- Starting HEAD: `90df400b8ee2e5678f9fe3c6d51417b116c3f20a`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail source: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Mode-9 setup/teardown boundary: `[0x80077A64, 0x80077DC8)`
- Mode-9 archive relocation helper: `[0x80076954, 0x80076A14)`
- Mode-9 SEDS archive loader: `[0x800721E4, 0x80072238)`

The focused runner pins all three exact retail slices before compiling:

| Retail slice | SHA-256 |
|---|---|
| `0x80077A64..0x80077DC8` | `279c30f95bbb352733c5ccfa20041685f8324e68935ec08e8ab5c1b242f4b045` |
| `0x80076954..0x80076A14` | `89e5625f0d853113669915f9cb69f2d1dfbb017162d777e77541104478c730cc` |
| `0x800721E4..0x80072238` | `1ae7931d6500b7844f35add116401df406500fc48076c21626ffcba05c5ba2d8` |

## Production behavior restored

`wm_80076954` now finishes the mode-9 second archive wave with the retail
LZSS decode, publishes the decoded allocation as a KSEG guest address, frees
the compressed allocation, and relocates all seven persistent table roots
from their exact header offsets.

`wm_800721E4` decodes the SEDS archive size, allocates the native sound
record, publishes it through the established native `D_8006259C` authority,
and submits the archive read.

`wm_80077A64` now owns the complete retail mode-9 setup sequence: transition
image preservation, second-wave completion, object pool, template and mode
state, cross products, SEDS loading, object position, object/GPU/table/upload
stages, sound registration, archive switch, terrain convergence, all four
scheduler registrations, and the three common render allocations.

`wm_80077CC0` now performs the matching retail teardown: sound shutdown and
SEDS free, render/upload/terrain cleanup, all persistent allocation frees,
pool teardown, and the retail field-transition state publication.

The main-loop dispatcher resolves `0x80077A64` and `0x80077CC0` symbolically;
it never calls a raw guest address.

## Focused certificate

`pc_port/tests/run_w34n61_mode9_lifecycle.sh`:

- archive helper O0/O2/nonrecovering UBSan: PASS
- lifecycle O0/O2/nonrecovering UBSan: PASS
- strict warnings: clean
- exact relocation destinations, KSEG publication, compressed free, setup
  order/arguments/state, scheduler pairs, sound link, teardown frees, and
  terminal state: PASS
- M1 missing KSEG publication: detected by `relocation.guest_publication`
- M2 swapped header offsets: detected by `relocation.header_offsets`
- M3 missing compressed free: detected by `relocation.compressed_free`
- M4 wrong transition argument: detected by `setup.transition_args`
- M5 wrong mode state: detected by `setup.state_values`
- M6 wrong scheduler pair: detected by `setup.registration_pairs`
- M7 missing SEDS link: detected by `setup.sound_link`
- M8 missing SEDS free: detected by `teardown.seds_free`
- M9 wrong teardown state: detected by `teardown.state_values`
- no host-stack pointer truncation in either production unit: PASS

## Natural detached acceptance

The normal native binary was bootstrapped through the accepted field route,
entrance 9 was selected at `PcPort_WorldMapInitMain`, and GDB detached before
world-map initialization. The run used the standing scripted inputs and the
retail recurring-frame cadence.

Observed naturally:

- mode-9 archives 143, 144, and 145 submitted and consumed;
- the 64-record object pool and all shared setup stages completed;
- scheduler pairs `923A8/925A0`, `77DC8/77E68`, `7828C/783E8`, and
  `78948/78950` registered and executed;
- each of the three recurring mode-specific callbacks executed 711 times in
  the natural-exit run;
- both upload pumps reported `unknowns=0` throughout;
- no `[worldmap-ot-adapter] ABORT` and no unresolved world-map callback stub
  appeared;
- a bounded 240-frame run produced and fulfilled captures at frames 60, 120,
  180, and 240;
- an independent longer run reproduced the frame-60 and frame-120 bytes and
  reached the retail terminal transition naturally at displayed frame 711,
  with `D7CC=0`; the main loop dispatches slot 2 before printing that state
  exit, so the mode-9 teardown completed on this path.

Capture digests:

| Frame | SHA-256 |
|---:|---|
| 60 | `d4b77003e72e3838093fd57aa7c287e3a470390d41c9fad037eb029bc69c1659` |
| 120 | `6f9844023470de58eb999810f0c7f9a468ffdffef0d7f00f7fac42ed1adac38b` |
| 180 | `aacd56dcbbdefbca93bc52ed7177e2f0689308bca9d05f11e3bd46d8d9e2ad1b` |
| 240 | `c5c4e2f7b36c7b6ff34f9cc5e0cf5c47d09798595722fdde64648350b26a6263` |
| 660 | `bf2833e4222c7b8b2127d0c6247ad6b3bc2bb654ff3e65ae1ef70169663999da` |

Visual inspection shows a coherent textured Yggdrasil and world background;
the changing hashes correspond to its retail path/camera sequence rather
than static or malformed output.

After the natural world-session exit, the wider program reaches the existing
post-world `SoundHandleError` stub and remains open. That downstream sound
fallback is outside this lifecycle slice; the process was terminated only
after mode-9 slot-2 teardown and the `D7CC=0` state exit were observed.

## Regression and build gates

- W34N59 mode-path math certificate: PASS
- W34N60 mode-9 callback certificate: PASS
- W34N57 mode-8/mode-11 lifecycle certificate: PASS
- W34N9 slot-1 integration certificate: PASS
- W34C1 scheduler cadence certificate, M1–M21: PASS
- W34N28 terminal-zero certificate: PASS
- W34N7 slot-2 teardown certificate: PASS
- canonical `./pc_port/build_port.sh`: LINK OK; port-owned addresses verified
- `git diff --check`: clean

## Verdict

`MODE9_LIFECYCLE=REPAIRED_VERIFIED`

Mode 9 now has a retail-anchored setup, recurring callback set, visible native
presentation, natural terminal transition, and matching teardown. The next
bounded lifecycle target is the next unresolved mode-table entry; it must not
be mixed into this commit.
