# W34N11 — existing frame seams

## Result

`PARTIAL_VERIFIED`: four of the five active local shadows in
`world_map_frame_driver_712d0.c` were replaced by already-certified linked
implementations. Retail `0x8001D468` was deliberately held behind its local
shadow after a natural-run discriminator proved that the generic body is not
compatible with the guest-OT world route.

Starting HEAD: `bda938b19279f2e1868a66c1666851dd3aa0a19e`.

## Source binding

| Retail call | Final binding | Result |
|---|---|---|
| `0x80071478 -> 0x800250E0` | linked `func_800250E0(int)` | retired local shadow |
| `0x80071480 -> 0x8001D468` | local address-tagged shadow | held; see discriminator |
| `0x8007197C -> 0x80025044` | shared `wm_80025044_guest_safe` | retired local shadow |
| `0x80071984 -> 0x80074F2C` | certified `wm_80074F2C` | retired local shadow |
| `0x8007198C -> 0x80075104` | certified `wm_80075104` | retired local shadow |

The guest-safe `0x80025044` body was moved out of the legacy bounded driver
into `world_map_image_transfer_25044.c`; both frame drivers now call that one
implementation. This is necessary because the compiled generic
`func_80025044` treats its 32-bit guest links as native pointers.

## `0x8001D468` incompatibility discriminator

An initial build wired all five real bodies. The accepted 120-frame route then
reported adapter range aborts on 119 of 120 frames. The first rejected link was
a native low address (`0x0064...`) installed in a guest OT root; frame 60 also
diverged from the standing visual baseline.

Two diagnostic-only three-frame builds isolated the cause:

- skip `func_8001D468`, retain real `func_800250E0`: zero adapter aborts;
- retain `func_8001D468`, skip real `func_800250E0`: native `0x006ff6e8` and
  `0x006ff968` OT links were rejected on frames 2 and 3.

Therefore the first incompatible seam is the generic work-list drain. Its
downstream `func_8001DAE8` path publishes packets from native work-buffer
storage into an OT consumed as guest links. Activating it requires a bounded
guest-safe packet-publication binding; merely removing the local shadow is
not valid. The diagnostic preprocessor guards were removed before the final
build.

## Certificates

- `run_w34b30_8007197c_image_transfer_prod_test.sh`: 7/7 under O0, O2,
  nonrecovering UBSan; strict warnings clean. The certificate now links the
  shared production unit directly rather than dragging in the legacy driver.
- `run_w34b34_80074f2c_prod_test.sh`: 7/7 under O0, O2, nonrecovering UBSan;
  4/4 mutants detected.
- `run_w34b35_80075104_prod_test.sh`: 7/7 under O0, O2, nonrecovering UBSan;
  5/5 mutants detected.
- `run_w34c1_scheduler_cadence.sh`: O0/O2/nonrecovering UBSan PASS. It now
  proves the four linked seams execute exactly once per displayed frame in
  retail order; M10-M14 detect each omitted call and the swapped pump order.
  The full cadence set is M1-M14 detected.
- Normal port build: `LINK OK`.

## Natural 120-frame acceptance

Environment was the reduced W34N10 route: normal field bootstrap,
`XENO_WORLD_INIT=1`, `XENO_WORLD_OPEN_LOOP=1`, frame limit 120, scripted input
`0:0x2000,600:0x4000,916:0x2000,976:0`, and debugger detach before
`PcPort_WorldMapInitMain`.

- bounded exit: 120/120;
- frame 60:
  `26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3`;
- frame 120:
  `ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc`;
- both digests are byte-identical to W34N10;
- adapter aborts: 0;
- image-transfer unknowns: 0;
- upload-pump unknowns: 0;
- retired stub messages (`250E0`, `25044`, `74F2C`, `75104`): 0;
- intentionally held `8001D468` shadow messages: 120.

## Next exact target

Trace the `func_8001D468 -> func_8001DAE8` packet-publication path and provide
a guest-safe binding for the native work-buffer packet/OT links. Acceptance is
the same 120-frame route with the `8001D468` shadow removed, zero adapter
aborts, and an evidence-backed interpretation of any legitimate visual hash
change.
