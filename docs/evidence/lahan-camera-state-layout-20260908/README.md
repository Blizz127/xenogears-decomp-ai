# Camera state layout after Gear battle

Natural sprite-c5 run completed STR4 and won the first Gear battle. Reinforcements then returned to field2, whose actor0 waits at2D36 (EF03) indefinitely: both camera durations1, flags3, mode0. Repeated fieldmain stacks and black screenshots establish the stable return before actor reads. No state was changed through the debugger.

Retail800A37A8 and800A4284 restore/save0x1C8 bytes at800AF880. Retail8008FBA8 writes camera mode at800AF934 inside that span. Native camera mode and15other camera symbols instead resolved to independent generated stubs, outside the region. Initialization reset mode0; restoring the packed camera region could not restore that separate symbol. pc_port/src/data_field.c now aliases the16symbols into the existing retail-addressed block. No changes to script conditions, movement durations, fade flags or progression.

field_camera_state_layout_test.py checks27 linked symbol offsets against the retail camera address map. The old frozen executable fails16 checks; the rebuilt executable passes all27. Native build and neighboring MoveImage/actor layout regressions pass. This is an executable layout check, not a full save/restore behavioral oracle or exact-match claim. Only native data aliases changed; no matching PSX source change.73native function stubs remain.

Fresh natural New Game replay in scratchpad/lahan-natural-20260908-camera-state cleared the previous black-screen wait, as documented below. The earlier sprite-c5 run was left untouched. All changes are uncommitted; no push. Full Lahan retail 1to1 remains unproven.

## Natural replay validation

The frozen camera-state run naturally completed STR4, won the first Gear battle, and returned from the scripted reinforcements sequence to visible Citan dialogue in burning field 2. `live-field-return.png` and `live-field-return-camera.log` show the previous black wait cleared: mode 1, both movement durations 0, flags 0. See `live-validation.json` for the binary pin and limits. This does not establish full retail parity.
