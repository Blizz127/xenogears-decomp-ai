# W34N55 — post-world `SoundHandleError` boundary

## Result

`WORLD_TEARDOWN_EXONERATED_FIELD_WDS_ALLOC_FAILURE`.

Starting HEAD was `e5c696c9f19dcabbe9729fbd2195f48bc824a650` on
`experiment/worldmap-open-gates-20260823`, equal to origin.  This rung made no
production change.  A temporary `XENO_DIAG_W34N55` witness was removed before
the normal rebuild.

The generated `[stub] SoundHandleError` line seen after the natural world-map
exit does not originate in base-world slot-2 teardown.  Both SEDS removals on
the accepted field-to-world-to-field route find the requested list head,
unlink it, and pass retail validation:

```text
field owner:
  target=0x61f1b8 head=0x61f1b8 next=0 validate=0

world owner:
  target=0x6402b4 head=0x6402b4 next=0 validate=0

[worldmap-open-loop] natural state exit frames=601 D7CC=0
```

The first and only error after that boundary is:

```text
id=0x1f caller=0x482de8 flags=0xb901
```

`addr2line` resolves native `0x482de8` to `SoundLoadWdsFile` at
`src/slus_006.64/system/sound.c:800`, the `spuAddr == 0` arm.  The process has
already returned from the world session and entered `FieldMain` for map 15 at
that point.  Retail error `0x1f` therefore reports an SPU-memory allocation
failure in the next field's WDS load.  It is not a world teardown error and
must not be hidden by implementing the broad `SoundHandleError` fallback as a
world-map repair.

## Bound

This proves only the accepted base-world route and its immediate natural
return.  It does not certify the subsequent field audio lifecycle.  The base
world session now reaches no generated world-function stub before `D7CC=0`;
the only earlier generated call is the already-known field bootstrap helper
`func_80028B14`.

Artifacts are in `scratchpad/w34n55_error_diag_run.log` and
`scratchpad/w34n55_diag_world_run.log`.  The normal non-diagnostic build ended
with `LINK OK`, and the tracked production source returned to its starting
content.
