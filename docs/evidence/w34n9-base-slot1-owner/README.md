# W34N9 — retail base-world slot-1 setup owner

Verdict: **FRESH_PATH_TRANSCRIBED_AND_CERTIFIED**.

Retail base-mode slot 1, `[0x80072238,0x8007299C)`, now has a compiled
production owner which calls the already-certified setup stages in retail
order.  The active mode-table dispatcher resolves guest target `0x80072238`
symbolically to this owner.  This rung deliberately does not claim that the
forced route is natural: its first synthetic session still skips slot 1 and
performs the same stages through the external gate ladder.

## Anchor and authority

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `7b138818e04bd5c186b1ab5e7fb19fee676332c2`
- Local matched origin before the change.
- Retail authority: `disc/world_map.bin`, loaded at `0x8006FAF0`, decoded with
  `mips-linux-gnu-objdump -D -b binary -m mips:3000 -EL
  --adjust-vma=0x8006faf0`.
- Structural authority: `docs/evidence/w34n4-recurring-loop/PHASE2.md` and
  `docs/evidence/w34n4-recurring-loop/README.md`.
- WDS lifecycle authority: `docs/evidence/w34n5-wds-lifecycle/README.md`.
- Mandatory transition helper authority:
  `docs/evidence/w34n8-transition-helper-72db4/README.md`.

W34N5 is important here: the world WDS lifecycle is already implemented and
proved non-null.  It is not a remaining slot-1 blocker.

## Production integration

`pc_port/src/world_map_session_setup_72238.c` owns the base fresh-session
sequence.  Thin `wm_72238_stage_*` wrappers in `world_map_init.c` expose the
existing certified stage bodies without duplicating their behavior or moving
their established one-shot guards.

The owner preserves this bounded retail order:

1. `0x80072BB0`, MoveImage `(0,0,320,216)` to `(704,256)`, DrawSync, and
   `wm_80072DB4(64,0,4,2)`.
2. Archive sync, second archive wave, object pool, state-template copy,
   mode-enter state, and the four cross products.
3. Fresh-session WDS cleanup and entry placement.
4. Archive sync, both GPU assets, object matrix, third archive wave, BSS
   constants, primitive templates, CLUT relocation, graphics work buffers,
   FT4 pools, heap table, both upload builders, draw packets, and `0x80088F64`.
5. Archive poll, conditional first WDS load, archive-index transition, terrain
   initialization, and the real CD-work drain loop with Vsync until circular
   distance is below two.
6. Ready-buffer consumption, mode audio setup, both convergence passes, and
   common-tail stages P0 through P5.

The dispatcher in `world_map_main_loop_71034.c` now calls `wm_80072238()` for
guest target `0x80072238` instead of the generated/default stub.

## Explicit restore-session boundary

Only retail's accepted fresh-session arm is transcribed in this rung.  If
`0x8006EE6A != 0` or `0x8009C894 != 0`, the owner emits an observable
`unsupported restore entry` diagnostic and returns `-2`; it never silently
substitutes fresh placement.  The missing restore seams remain:

- `0x80073398` on the EE6A arm;
- `0x8007565C` and `0x80075D4C` on the C894 restore arm.

This boundary does not affect the current first-session substrate, but it
prevents a later multi-session route from being called complete prematurely.

## Focused certificate

Runner: `pc_port/tests/run_w34n9_slot1_owner.sh`.

The certificate links the production owner against deterministic seams and
proves exact stage ordering, MoveImage source/destination, transition
arguments, repeated CD draining, convergence P2, all six common-tail stages,
and the loud unsupported-restore result.  The runner additionally checks that
the production dispatcher actually names `wm_80072238` at the slot-1 case.

```text
O0 PASS
O2 PASS
nonrecovering UBSan PASS
strict warnings PASS
M1 missing transition: DETECTED by ASSERTION retail_order
M2 template/mode ordering: DETECTED by ASSERTION retail_order
M3 missing WDS cleanup: DETECTED by ASSERTION retail_order
M4 single CD drain: DETECTED by ASSERTION cd_drain_repeats
M5 missing convergence P2: DETECTED by ASSERTION retail_order
M6 swapped common-tail P2/P3: DETECTED by ASSERTION retail_order
```

Regression certificates also passed for W34N5 WDS lifecycle, W34N8
transition helper, W34N7 slot-2 teardown, and W34C1 cadence, including all of
their named mutants.

## Full build and dormant-route neutrality

Normal `./pc_port/build_port.sh` completed:

```text
LINK OK -> pc_port/build_native/xeno-port (port-owned addresses verified)
```

The detached 120-frame forced route reached its bounded exit, fulfilled both
capture requests on their requested frame, and did not emit a slot-1 owner
diagnostic.  Because this substrate deliberately skips slot 1 on its first
session, integration must remain byte-neutral here.  It did:

- frame 60:
  `26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3`
- frame 120:
  `ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc`
- exit: `bounded exit frames=120 limit=120`

## Next exact implementation boundary

The fresh owner exists and compiles; the remaining first-session blocker is
ownership, not another decode.  The next production rung should make
`wm_80072238` the normal first-session setup path and retire the corresponding
external W34N3 gate ladder.  Acceptance must use the W34N5 retail event oracle
and structural callback/frame invariants, then establish new deterministic
visual hashes.  The old hashes above describe the deliberately forced route
and are not an oracle once slot 1 naturally owns setup.

Later-session acceptance remains separately blocked on the three restore
helpers named above and a natural slot-2 teardown run.
