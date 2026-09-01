# W34N115 — retail mode-18 private initializers

## Anchors

- Starting HEAD: `629df1a36585686be55d6a4af56db3cb58f1d0c5`.
- Canonical image: `disc/world_map.bin`, load base `0x8006FAF0`.
- `wm_800838E8 [0x800838E8,0x8008390C)`, SHA-256
  `f50927db5ce9246f63827545fa12f5c021b977e2f80f67309df70ca054e0c2e7`.
- `wm_8008390C [0x8008390C,0x80083A00)`, SHA-256
  `8bbdad2839313cbef0510f1695a8dcd3cf127107fd747ade4555ae7cc09832c8`.
- `wm_80083FE4 [0x80083FE4,0x80084068)`, SHA-256
  `b25fedbe29dc220a7607e784ff19e8afcc4e54a6474991699d2d6f1a97b4ba78`.

## Production result

The three private initializer callbacks registered by the accepted mode-18
lifecycle are now exact bounded native transcriptions and resolve symbolically
through the world scheduler.

- `wm_800838E8` publishes the retail static record `0x8009AC60` through the
  slot's `+0x50` field.
- `wm_8008390C` initializes the camera slot, fans the reset position out to
  the live position/target records, publishes the retail angles and movement
  constants, and initializes the camera/view-height globals.
- `wm_80083FE4` initializes the mode object slot and its paired object
  position/enable fields.

## Certificate

`pc_port/tests/run_w34n115_mode18_initializers.sh`:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- M1 wrong static-record pointer: DETECTED by
  `init_static.record_pointer`
- M2 wrong scale: DETECTED by `camera_init.constants`
- M3 missing position fanout: DETECTED by
  `camera_init.position_fanout`
- M4 wrong angles: DETECTED by `camera_init.angles`
- M5 wrong camera height: DETECTED by `camera_init.height`
- M6 wrong object Z: DETECTED by `object_init.slot_position`
- M7 missing object enables: DETECTED by `object_init.enable`
- M8 missing object translation: DETECTED by
  `object_init.translation`

The normal PC port build links successfully.

## Natural entrance-18 acceptance

The detached 120-frame entrance-18 route used the standard scripted input and
completed its bounded loop. Both capture requests were fulfilled on the same
numbered frame, and both upload pumps retained `unknowns=0`.

The initializers now resolve and advance their scheduler slots. The remaining
private frontier is exactly two per-frame update callbacks:

| slot | unresolved update | calls in 120 displayed frames | slot state |
|---:|---:|---:|---:|
| 2 | `0x80083A00` | 60 | 1 |
| 3 | `0x80084068` | 60 | 1 |

No other mode-18 callback stub was observed.

Capture digests:

- frame 60:
  `8926b5828a06b83d85c7cab34518806a5469cacd8c1124380aed19b81b8aed55`
- frame 120:
  `ce06918d6d085fe3f959e0a245b375e519fa07f5811ea9ff97b9762fa34911ab`

Both captures show a coherent blue sky/cloud layer and distant dark terrain,
which is a material improvement over W34N114A's vertical-band corruption.
They still contain a large flat white foreground and a black lower strip, so
this is not mode-18 visual acceptance.

## Next exact frontier

Transcribe the two live update callbacks independently, beginning with
`wm_80083A00 [0x80083A00,0x80083FE4)` and then
`wm_80084068 [0x80084068,0x8008440C)`. Each is reached 60 times on the
accepted route; neither should be replaced with forced state or a fabricated
default.
