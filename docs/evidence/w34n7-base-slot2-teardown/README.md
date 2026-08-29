# W34N7 — retail base-world slot-2 teardown

Verdict: **TRANSCRIBED_AND_CERTIFIED**.

This is the first bounded lifecycle implementation from the W34N4 Lane-B
scope.  Retail base-mode callback `0x8007299C` is now compiled production C,
and the active mode-table dispatcher invokes it instead of the local default
stub.  The broader first-session setup callback `0x80072238` remains
unintegrated and this rung does not claim a natural all-session route.

## Anchor and authority

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `5ecb8db3813fac12914dc6f4c18289948ac7a564`
- Local matched origin before the change.
- Retail authority: `disc/world_map.bin`, loaded at `0x8006FAF0`, decoded with
  `mips-linux-gnu-objdump -D -b binary -m mips:3000 -EL
  --adjust-vma=0x8006faf0`.
- Previously banked structural authority:
  `docs/evidence/w34n4-recurring-loop/PHASE2.md`.
- Focused callback listing: `scratchpad/w34n5_slot2_7299c.objdump`.

The exact retail callback boundary is `[0x8007299C,0x80072BB0)`, 0x214 bytes
and 133 instructions.

## Implemented retail lifecycle

`pc_port/src/world_map_teardown_7299c.c` preserves the decoded order:

1. On `D7CC == 0`, fade the active audio manager to zero over 240 ticks.
2. Stop sound effects, unlink the SEDS/WDS allocation, and free it.
3. Walk all 64 scheduler records (0x80-byte stride), destroy each non-null
   native object at `slot+0x4C`, then clear that field.
4. On `D7CC == 1`, execute retail `0x80075460`: save the 0x2000-byte scheduler
   pool and its scalar/block state through destination `0x8005A4E4`, then OR
   `0x8000` into `0x8006F954`.
5. Tear down both window records, the variable-length object table, graphics
   work buffers, FT4/heap/upload/particle tables, all 256 asset slots, five
   direct world allocations, three primary/secondary channel pairs, and the
   scheduler pool in retail order.
6. Apply retail `0x800960BC`'s two archive-state polls to select `BE08` versus
   `D3C0`, then free `D7D4`.

The active dispatcher in `world_map_main_loop_71034.c` now resolves guest
target `0x8007299C` symbolically to `wm_8007299C()`.  Target `0x80072238`
remains an observable stub and was not widened in this rung.

## Pointer-domain decisions

The teardown is a mixed-domain boundary, so a blanket cast or blanket
`PSX_ADDR` conversion would be wrong:

- allocations published by the world setup stages are guest KSEG addresses;
  they are rebased before native `HeapFree`/model cleanup calls;
- scheduler `slot+0x4C` is the established native-pointer exception and is
  passed to `func_800230A8` without guest rebasing;
- `D_80062528`, `D_8006259C`, and `g_GfxWorkBuffers` are native host-pointer
  authorities and are consumed as native pointers.

The compiled port previously generated an empty `GfxFreeWorkBuffers` stub.
Its retail body is only `HeapFree(g_GfxWorkBuffers)` followed by the work-list
reset performed by `func_8001D2A4`; W34N7 supplies those exact semantics in
production C.  The final binary no longer lists this symbol in generated
`stubs.c` and resolves both it and `wm_8007299C` as text symbols.

## Focused certificate

Runner: `pc_port/tests/run_w34n7_slot2_teardown.sh`.

The certificate links the production teardown and an actual test-only call
through the production mode dispatcher.  It proves:

- `D7CC==0` versus `D7CC==1` audio behavior;
- exact major call/free event ordering;
- all 64 scheduler slots, including slot 63, and post-destroy clearing;
- guest/native pointer-domain behavior at each mixed seam;
- two object-table records and the final base free;
- sparse entries at both ends of the 256-entry asset table;
- primary/secondary three-pair conditional frees;
- the exact `0x80075460` pool, block, signed-scalar, and scalar layout;
- graphics work-buffer free plus work-list reset;
- the `BE08`/`D3C0` selection and unconditional `D7D4` free.

Results:

```text
O0 PASS
O2 PASS
nonrecovering UBSan PASS
strict focused warnings PASS
M1 fade outside D7CC==0: DETECTED by state1_no_fade
M2 63-slot walk: DETECTED by slot63_cleared
M3 omit slot clear: DETECTED by slot63_cleared
M4 omit D7CC==1 snapshot: DETECTED by snapshot_pool_copy
M5 swap window teardown: DETECTED by window_order
M6 omit secondary channel frees: DETECTED by secondary_pair_freed
```

The pre-existing cadence certificate was repaired only to provide the two
newly linked natural-epilogue symbols as test stubs, then rerun.  It remains
green under O0/O2/UBSan with M1-M8 detected and now explicitly asserts that a
bounded exit calls neither the queue barrier nor slot 2.

## Full build and bounded-route neutrality

Normal `./pc_port/build_port.sh` completed:

```text
LINK OK -> pc_port/build_native/xeno-port (port-owned addresses verified)
```

The final detached 120-frame route reached its bounded exit.  Since that seam
deliberately exits before natural slot 2, a correct integration must be
neutral there.  Both captures remained byte-identical to the standing
baseline:

- frame 60:
  `26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3`
- frame 120:
  `ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc`
- exit: `bounded exit frames=120 limit=120`

## Remaining acceptance boundary

This rung proves the callback body, its dispatch, and dormant-route
neutrality.  It does **not** prove the teardown against naturally armed world
state: the accepted bounded route does not set `D554=0`, and the retail slot-1
owner `0x80072238` is not yet the natural first-session setup path.  Final
lifecycle acceptance still requires an integrated natural session that
executes slot 1, reaches `D554==0`, invokes this slot 2 exactly once, and
matches the W34N5 retail event oracle through the signed `D7CC` decision.
