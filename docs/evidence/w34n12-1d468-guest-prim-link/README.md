# W34N12 — 0x8001D468 guest primitive linkage

## Result

`REPAIRED_VERIFIED`. The remaining active frame-driver shadow at retail
`0x8001D468` is removed. The linked work-list drain now runs once per displayed
frame without installing native addresses in the world-map guest OT.

Starting HEAD: `6ddad8432be8e49cddc7d63053c0b7e2002e7f6a`.

## First divergence and repair

W34N11 proved that enabling the compiled `func_8001D468` caused the guest OT
adapter to reject low native links beginning on frame 2. Source lineage is:

1. `func_8001D468` drains `D_80059190` and updates sprite frame data through
   `func_8001DAE8`;
2. the recurring world scheduler renders those objects through
   `wm_80085CDC -> func_8001E298 -> func_8001E3D8`;
3. `func_8001E3D8` allocates `POLY_FT4` packets from `g_GfxCurWorkBuffer` and
   uses generic `addPrim`;
4. both the packet and the destination OT are host pointers into `g_PsxRam`,
   but generic `addPrim` stores the packet's low native address (`0x0064...`)
   in the OT; the world adapter requires the packet's low 24-bit guest offset.

`PcPort_AddPrimDomainAware` now selects representation from the OT destination:

- OT inside `g_PsxRam`: require the packet inside `g_PsxRam`, translate it with
  `PsxMemory_GuestAddr`, preserve the primitive length byte and predecessor;
- native OT: preserve the existing native low-address behavior;
- guest OT plus native packet: reject and count instead of corrupting the OT.

Only the two `addPrim` sites in `func_8001E3D8` use this binding. Other render
paths are unchanged. A diagnostic-only three-frame run after the OT repair
exposed one adjacent representation: `func_800251C8` publishes raw native
pointers that still point inside `g_PsxRam`. The shared world image consumer
now resolves either KSEG guest addresses or raw-native-in-guest-RAM addresses;
invalid values remain rejected.

## Certificates

- `run_guest_prim_link.sh`: O0/O2/nonrecovering UBSan PASS, strict warnings
  clean; M1-M4 detect native-address encoding, missing predecessor, missing OT
  publication, and broken native fallback.
- `run_w34b30_8007197c_image_transfer_prod_test.sh`: 8/8 under O0/O2/
  nonrecovering UBSan. The eighth case proves a raw-native-in-guest-RAM image
  node and data pointer resolve to their exact host bytes.
- `run_w34c1_scheduler_cadence.sh`: O0/O2/nonrecovering UBSan PASS; M1-M15
  detected. The certificate proves `0x8001D468` and the other four restored
  frame seams execute once per displayed frame in retail order.
- Normal port build: `LINK OK`.

## Runtime witnesses

A paced, attached, state-only three-frame witness reported:

```
frames=3 guest_links=32 native_links=100921 rejects=0
adapter_packets=821 adapter_range_aborts=0
```

The native count includes field bootstrap before world-map entry; the 32 guest
links prove the new world branch executed naturally. It was not merely a
failure-avoidance path.

The accepted detached 120-frame route used the reduced W34N10 environment and
scripted input `0:0x2000,600:0x4000,916:0x2000,976:0`:

- bounded exit: 120/120;
- `0x8001D468` shadow messages: 0;
- adapter aborts: 0;
- image-transfer unknowns: 0;
- upload-pump unknowns: 0;
- frame 60:
  `26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3`;
- frame 120:
  `ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc`.

The visual digests remain byte-identical to W34N10/W34N11. This is acceptable
because the state witness independently proves real guest packets were linked,
while the capture route's two sampled presentations did not change.

## Status

All five active frame seams named by W34N4 are now real on the natural route:
`0x800250E0`, `0x8001D468`, `0x80025044`, `0x80074F2C`, and `0x80075104`.
The remaining local frame-driver stubs are genuinely absent retail helpers and
are not part of this repair.
