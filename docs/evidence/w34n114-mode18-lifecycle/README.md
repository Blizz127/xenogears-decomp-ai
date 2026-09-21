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

The first detached route was run with entrance 18 and the standard scripted
input. The lifecycle completed and the scheduler reached these four values:

| slot | initializer | observed calls through attempted frame 60 |
|---:|---:|---:|
| 1 | `0x800838E8` | 61 |
| 2 | `0x8008390C` | 61 |
| 3 | `0x80083FE4` | 61 |
| 4 | `0x80088948` | 61 |

That first run used an incorrectly decoded fifth registration pair:
`0x80088948/0x80088950`. Retail forms those values with signed `addiu`
immediates from `0x8008`, so the effective pair is the already port-owned
`0x80078948/0x80078950`. W34N114A corrects both production and certificate;
the table above is therefore a superseded frontier, not the final mode-18
stub census. Upload pumps remained at `unknowns=0`, but frame 60 had no
effective presentation and its capture request was rejected as unfulfilled.
No capture is claimed from that run.

The corrected W34N114A run proves the effective shared pair is
`0x80078948/0x80078950`: it resolves and presents normally. Exactly three
private initializers remain unresolved, each reached 121 times (one setup
scheduler pass plus 120 displayed frames):

| slot | unresolved initializer | calls |
|---:|---:|---:|
| 1 | `0x800838E8` | 121 |
| 2 | `0x8008390C` | 121 |
| 3 | `0x80083FE4` | 121 |

The corrected route reaches and fulfills both capture requests, completes its
bounded 120-frame loop, and keeps both upload pumps at `unknowns=0`:

- frame 60:
  `f52c10caa70d6ce0a18f6f4aec0a001d0afc936827372f80da2c248952ecaabd`
- frame 120:
  `95d2fec62f92d9d15f012532e751c2b15b980968094ba0dc4318bf4d21d2da0d`

Both captures contain a live but visibly incomplete/malformed mode-18 scene;
they are evidence of presentation, not visual acceptance.

This is a truthful moved frontier, not mode-18 visual acceptance. After the
W34N114A signed-immediate correction, the next bounded target is the three
genuinely private callback pairs registered by `wm_8008355C`.
