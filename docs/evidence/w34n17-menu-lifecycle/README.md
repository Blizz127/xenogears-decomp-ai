# W34N17 — retail world-map menu lifecycle

## Result

`PASS`. The frame driver now links the paired retail menu lifecycle instead of
local no-op stubs:

- `wm_800758C0`: world teardown, display preservation, and menu archive setup;
- `wm_80075B58`: world archive/GPU reload, display restoration, buffer rebuild,
  and party reconciliation after `MenuMain`.

The functions were integrated together. Activating only the return half would
have reallocated shared graphics pools without running retail's entry teardown.

## Anchor

- Starting HEAD: `5b1f65ba53404a0d61fc68fb05fe0d74bb94f609`
- Branch: `experiment/worldmap-open-gates-20260823`
- Local and origin matched before editing.
- Retail authority: `disc/world_map.bin`, load address `0x8006FAF0`.
- Whole-image SHA-256:
  `4c15fd32b3a03d7cd5ea4403dcaabc70abaf99b6aaca65d63d6866803edaac70`.

| Function | Retail range | File offset / length | Slice SHA-256 |
|---|---|---|---|
| menu entry | `0x800758C0..0x80075B58` | `0x5DD0 / 0x298` | `90555ee38c3a95ed3839b77a42081a79db4f1d2112b58944103d0e8fd2d864d1` |
| menu return | `0x80075B58..0x80075D4C` | `0x6068 / 0x1F4` | `a3d88489847d346d2544b8f1fc404685904d336f33ac62537037f403d49b1977` |

## Entry lifecycle

The entry half now performs the retail sequence, including:

- controller/presence snapshot and `D_80059179` save;
- terrain-selector result handling;
- queue barrier and the W34N15 paired graphics-pool frees;
- frees of `BC3C`/`BCB4`;
- retail heap-cursor scratch-size calculation;
- W34N16's `0x80071FEC` two-file menu archive request;
- two VRAM readback allocations and `StoreImage` calls;
- conditional display copy, fixed backup copy, sync and loading transition;
- `ArchiveDataSync` polling while the signed result is at least 2;
- decompression of `D528` into `C7E4`, source free, final move/sync.

All world-table heap words are resolved from guest KSEG before host APIs are
called. Newly allocated world buffers are published back as guest addresses.
The named menu resource `D_8005945C` remains a native pointer authority.

## Return lifecycle and `8440C`

The return half restores heap/archive ownership, reloads `BD20`, runs the
loading transition, restores both VRAM regions, clears the active 0x400-word
guest OT through `wm_ot_clear_r_guest`, and calls the real GPU asset loader.
It then rebuilds buffers in retail order:

`978FC -> 8901C -> 865A0 -> 85FE0`

followed by palette transfer, VSync, controller reset, saved flag restoration,
and `75D4C` reconciliation.

`wm_8008440C` is now a shared symbol. Its old harness-only process-wide
one-shot rejection was removed. Each invocation still requires a non-null
current `BD20` allocation and consumes/frees that allocation; retail `75B58`
reloads `BD20` immediately before reentry. GPU asset B retains the separate
requirement that asset A has completed at least once.

## Certificate

`pc_port/tests/run_w34n17_menu_lifecycle.sh`:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict focused warnings: clean
- M1 swapped entry free order: detected
- M2 wrong heap-cursor scratch size: detected
- M3 missing conditional display move: detected
- M4 wrong archive-poll boundary: detected
- M5 wrong guest OT length: detected
- M6 swapped rebuild order: detected
- M7 missing final state restoration: detected
- M8 missing `8440C` reentry: detected
- source gate confirms the former `8440C` process-wide guard is absent.

The W34C1 cadence certificate remains PASS under O0/O2/UBSan with M1-M15
detected. The normal PC port reports `LINK OK`.

## Detached runtime

The accepted route fulfilled both capture requests and logged
`bounded exit frames=120 limit=120`:

- frame 60:
  `26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3`
- frame 120:
  `ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc`

Both remain byte-identical to the standing baseline. This accepted route does
not naturally select menu modes 1..3, so neutrality proves that integration
does not perturb the normal frame path; the complete entry/return execution is
proved by the focused production-linked certificate rather than a natural menu
interaction run.

## Remaining frame-driver boundary

The source audit leaves exactly one local address-tagged no-op in the retail
frame driver: `wm_80075E7C`. It is the next exact code-producing target.
