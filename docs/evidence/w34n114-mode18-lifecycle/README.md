# W34N114 — retail mode-18 lifecycle

## Anchors

- Starting HEAD: `2439d53d42dc926814daece415adc99516520c64`.
- Canonical image: `disc/world_map.bin`, load base `0x8006FAF0`.
- Setup `[0x8008355C,0x800837DC)`, SHA-256
  `28e89309b7fcbec07acd229f824fe06245d66d0ddda01fb219bb4a61012b8773`.
- Teardown `[0x800837DC,0x800838E8)`, SHA-256
  `4bb26cce644323bc0a25ffb78ab04ef755c79d1aca63299c87b2518ab80a3cbd`.
- Pair `[0x8008355C,0x800838E8)`, SHA-256
  `2710f69b48a2919dbe6896190ab7656d5ef98460b31682c848503241f01876a1`.

## Production result

`wm_8008355C` now owns the retail mode-table slot-1 setup. It performs the
framebuffer copy/transition, finishes the second archive wave, creates the
object pool, publishes the retail matrix/state and fixed position
`[0x01800000,0xFFF00000,0x01A00000]`, runs the exact asset/table/upload
sequence, links the loaded SEDS image, drains the CD work queue, and registers
the five retail callback pairs in order.

`wm_800837DC` now owns slot-2 teardown. It shuts down and frees SEDS state,
runs the retail teardown sequence, frees the five owned world allocations,
and publishes terminal state `(F94E,F954,BBC4,F950)=(617,4,1,BD3A)`.

Both mode-table addresses are resolved symbolically by
`world_map_main_loop_71034.c`. The second-wave finish seam reuses the already
accepted poll and `wm_80076954` bodies rather than duplicating them.

## Certificate

`pc_port/tests/run_w34n114_mode18_lifecycle.sh`:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- M1 wrong transition argument: DETECTED by `setup.transition_args`
- M2 wrong mode constant: DETECTED by `setup.state_values`
- M3 swapped object-matrix/GPU-A order: DETECTED by `setup.stage_order`
- M4 wrong private registration pair: DETECTED by
  `setup.registration_pairs`
- M5 missing SEDS link: DETECTED by `setup.sound_link`
- M6 missing palette tail: DETECTED by `setup.stage_order`
- M7 missing SEDS free: DETECTED by `teardown.seds_free`
- M8 wrong teardown state: DETECTED by `teardown.state_values`

The normal PC port build links successfully.

## Natural entrance-18 frontier

The accepted detached route was run with entrance 18 and the standard
scripted input. The lifecycle completes and the scheduler naturally reaches
all four mode-private initializers:

| slot | initializer | observed calls through attempted frame 60 |
|---:|---:|---:|
| 1 | `0x800838E8` | 61 |
| 2 | `0x8008390C` | 61 |
| 3 | `0x80083FE4` | 61 |
| 4 | `0x80088948` | 61 |

They are still unresolved and therefore return the scheduler's observable
neutral value instead of advancing to their update callbacks. Upload pumps
remain at `unknowns=0`, but frame 60 has no effective presentation and its
capture request is rejected as unfulfilled. No capture is claimed.

This is a truthful moved frontier, not mode-18 visual acceptance. The next
bounded target is the four live private callback pairs registered by
`wm_8008355C`; all four initializers are required on this natural route.
