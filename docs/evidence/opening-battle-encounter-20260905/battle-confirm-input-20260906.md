# Battle pilot-panel confirmation input — read-only source map

Status: **RESOLVED; no input bug found.** The ordinary physical `z` pulse recorded at `2026-09-06T04:02:01.307344Z` generated the expected Circle confirmation and advanced the visible panel from `Hiyaaaaa` to `Huff,huff... That's one down!?`. The replay then reached its independent 300-second monitor limit before another confirmation was sent.

## Exact retail/base-battle writer

The base-battle input dispatcher is retail function `func_80089CCC` in `asm/battle/nonmatchings/main/func_80089CCC.s`.

- `0x80089E74` calls `ControllerPopState`, so the function consumes queued controller samples.
- `0x80089F40..0x80089F50` loads `g_C1ButtonStateReleased` (`0x8005948C` per `config/symbol_addrs.slus_006.64.txt`), masks `0x20`, and branches on nonzero to `.L80089DFC`.
- `.L80089DFC` assigns `s1 = 4` (`0x80089DFC..0x80089E00`).
- At the common tail, provided `D_800C3444 == 0`, `0x8008A114..0x8008A118` stores `s1` as a byte to `D_800D3014` (`0x800D3014`).
- For comparison, the immediately following `0x40` test branches to `.L80089E04`, which assigns `s1 = 5`. Thus Circle writes action `4`; Cross writes action `5`.
- `func_8008A274` calls this dispatcher as `func_80089CCC(0)` at `0x8008A2A4` when its low-byte argument is zero.

This identifies the normal controller-input writer relevant to the panel. Other battle routines also read or mutate `D_800D3014`; this note does not claim the dispatcher is the only writer in the whole overlay.

## Exact panel consumer

The exact retail dynamic-module disassembly for archive `(0x20,0)`, file 1, controller `0x801E6CE8..0x801E71D4`, checks the same action byte:

- `0x801E70B0..0x801E70BC` loads `D_800D3014`, compares it with `4`, and skips the close path when unequal.
- On equality, `0x801E70C4..0x801E70CC` loads the current window and calls `func_800345E0`.
- It then clears the panel state bytes at offsets `0xCF` and `0x9E` (`0x801E70D4..0x801E70F0`).

Therefore the pilot panel specifically consumes action `4`, the Circle action produced above. Cross/action `5` is not its confirm value.

## Native physical-input path

- `include/system/controller.h` defines `CTRL_BTN_CIRCLE = 0x20` and `CTRL_BTN_CROSS = 0x40`.
- `pc_port/src/port_main.c:1228..1235` changes the post-initialization keyboard binding to `kc_circle = 29`, documented there as `SDL_SCANCODE_Z`; `C` remains Cross.
- `pc_port/src/game_overrides.c:134..135` supplies the native identity remap table and retail masks `{0x20,0x40,0x10,0x80,0x04,0x01,0x08,0x02}`. Under this table Circle remains `0x20`; it is not swapped with Cross.
- `pc_port/src/psyq_compat.c:1266..1284` runs `PsyX_UpdateInput`, `ControllerPoll`, then `ControllerPushState` from the native VSync shim.
- `ControllerPoll` remaps the active controller state and computes `g_C1ButtonStateReleased = (current ^ previous) & current` (`src/slus_006.64/system/controller.c:103..133`). Despite the historical variable name, this is a newly-pressed/rising-edge mask.
- `ControllerPushState` queues that edge; `ControllerPopState` restores it for the battle dispatcher (`controller.c:191..252`).

A held key does not continuously create action `4`: it needs release followed by a new press to make another rising edge. One ordinary `z` press advances one panel; after the next panel becomes visible, send a separate ordinary `z` press/release.

## Runtime evidence and correction

`pc_port/build_native/opening-constructor-virtual-ysi05oci/actions.jsonl` records the ordinary `z` pulse at `04:02:01.307344Z`, held for 0.1 seconds, and the owned stop at `04:02:20.361519Z` for the 300-second run limit. Final-frame inspection by the runtime owner showed the subsequent `Huff,huff... That's one down!?` panel. This corrects the preliminary interpretation that the input had failed. No controller repair is indicated.

The source map explains the effective path; it does not separately instrument every host-to-guest storage alias boundary. The observed panel advance is the runtime confirmation of the endpoint.

## Evidence pins

- `asm/battle/nonmatchings/main/func_80089CCC.s`: `e5a40165b94b98e2237122ecc7b03aac354143fa28ce623bc780fdfd4bd52801`
- `asm/battle/nonmatchings/main/func_8008A274.s`: `d8ae4595a12ad4f8e435af5a3dd59210ae06d81b58f05af2d97229c09423473d`
- `include/system/controller.h`: `8d63c9faf13ff1cb25992f26949ba59b2ed1c92b90948c86a7e232132a7473bf`
- `src/slus_006.64/system/controller.c`: `f6e0db2575ab0d815e15aed4b5361675f24904a4c9abffd0e40958e03d198ebf`
- `pc_port/src/port_main.c`: `640309e5d6bb5b8b19bcf54b9cd69449f6f9ee03c88b6dfa1f629f3cece39f39`
- `pc_port/src/psyq_compat.c`: `55c0014d00b856ce82d07dce5f84fbc1a1772b11061fc3cca2589810819baf3a`
- `pc_port/src/game_overrides.c`: `fff3464fa60aa38f629358753927771fc5f8328cb59e07cf9a1ae066feb4583f`
- Exact dynamic-controller disassembly `/tmp/xeno-opening-dynamic-module-20260906/candidate/retail-801e6ce8-801e71d4.objdump.txt`: `ab05cf484025a28f475357197bab5297caa20bc40c66e53f895a39fad26b318b`
- Runtime action log `pc_port/build_native/opening-constructor-virtual-ysi05oci/actions.jsonl`: `112442b448cfa4e85dda981c1cff17a13c8abdd0a28d72a49f62f102f024df6c`

No repository files, runtime processes, emulator APIs, or UI state were modified.
