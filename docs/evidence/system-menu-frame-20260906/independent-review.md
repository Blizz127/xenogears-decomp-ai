# Read-only review: `func_8001C074` frame repair

Date: 2026-09-06. Scope was the current `src/slus_006.64/system/menu.c`
function and its new retail differential test. No repository files were edited
by this review.

## Validation observed

Running the runner through `bash` (the file is mode `0644`) produced:

```
SYSTEM MENU FRAME PASS cases=300 checks=6020 instructions=77
SYSTEM MENU FRAME PASS cases=300 checks=6020 instructions=77
SYSTEM MENU FRAME PASS cases=300 checks=6020 instructions=77
SYSTEM MENU FRAME control rejected: stuck-buffer
SYSTEM MENU FRAME control rejected: wrong-submit-entry
SYSTEM MENU FRAME control rejected: wrong-page-toggle
SYSTEM MENU FRAME control rejected: skip-debug-recheck
```

The pinned full retail `SLUS_006.64` hash is checked by the runner. The
standalone/full-TU retail slice evidence remains byte-exact:

- `/tmp/xeno-system-menu-retail-20260906/candidate.bin` and
  `retail-frame.bin`: 308 bytes, SHA-256
  `0be4caf007322b055a6a33c46421b6e5228b73e54ff0244eaf7213b57a1f2ecf`.
- `/tmp/xeno-system-menu-retail-20260906/full-frame.bin` has the same hash.

Current input hashes at review time:

- `menu.c`: `a323d96071b9004f603085f3740667b5678a3f8cdfbd20a436751fa25eb134bb`
- `menu.h`: `7b7eca9f0769ff5bdd89bb8cd3f18f26382aa2c6ae95d76f023cace1f26a5543`
- test C: `0d43c106922d90eb12cc49f0c782784cbfef9f7d574acdea857d39a3c20480d5`
- runner: `acd295c4c886a23bbed7b6b74df104246d444818b5640758b1f71de16537daea`

## Findings

1. **Packaging gap: runner is not executable.**
   `pc_port/tests/run_system_menu_frame_retail_test.sh` is mode `0644`; direct
   invocation (`./pc_port/tests/run_system_menu_frame_retail_test.sh`) fails
   with `Permission denied`. `bash <runner>` passes. Set the executable bit
   before treating the test as a durable installed test.

2. **The known qualifier change needs a post-change exactness check.**
   The current header makes `SystemMenu.unk1E95` ordinary `u8` and the input
   routine uses explicit volatile lvalue accesses. That is the sound native
   ownership model: the frame routine reads ordinary RAM, while only the input
   RMW sequence is volatile. However, the pinned exact full-TU object was built
   before this header/source transition. The frame runner proves native
   behavior and the isolated old candidate proves the prior 308-byte slice;
   neither alone proves that the new header declaration preserves the exact
   MIPS `MenuProcessControllerInput` emission. Re-run the exact isolated
   compiler comparison for that input routine and the affected shop/member
   callers after the header change.

3. **Frame test scope is correctly bounded but does not test input RMW.**
   `MenuProcessControllerInput` is replaced by a spy, so this test covers the
   frame routine's post-input reload/rebind behavior and all seven-segment
   helper sequence, but it cannot validate L1/L2 decrement/increment semantics.
   That should remain a separate input test or existing controller gate.

4. **No functional defect found in the frame fixture.**
   The 300 cases cover three initial environment states, five page values,
   three debug values, both `unk1E94` flags, and four rebinding mutations,
   with the invalid null-target mutation constrained before helper calls.
   The native whole-struct write guard, helper trace comparison, 77/77 retail
   instruction coverage, and four negative controls cover the observable
   frame behavior. No additional source correction is justified by this
   review.
