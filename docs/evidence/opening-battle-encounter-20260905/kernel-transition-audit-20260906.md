# Xenogears opening transition Kernel Menu audit

Date: 2026-09-06
Scope: read-only audit; no source, UI, save, emulator, or worktree changes.

## Pins and observed run

- Repository: `/var/home/blizz/Projects/xenogears-decomp-ai`
- Worktree was already dirty and was preserved.
- Branch: `experiment/worldmap-open-gates-20260823`
- HEAD: `3a3e7aac03a2f166fb924945a489e392d706f282`
- Run: `pc_port/build_native/opening-type9-cleanup-5-s6ef07mm`
- Run scope: uninstrumented normal New Game after the retail A5/type9/line-scroll cleanup repairs; no gameplay-state writes.
- The parent-owned retail PCSX Redux process was left untouched.

The run log records the retail battle return at 368,858,285 instructions, followed by the field14 load path. The mislabeled `.png` captures are 640x480 BMPs. Frames `015360`, `015420`, `015480`, `015540`, and `015600` are the same navy/white Kernel Menu card (the first four have exactly 3 colors: `(0,0,32)`, white, and `(248,248,248)`). Frame `015660` is the field14 render. There are no native menu log lines in this interval; the menu implementation itself does not emit those lines.

## Identity of the transient screen

The screen is the retail `KernelMenuMain` state, not a field overlay. `src/slus_006.64/system/kernel_menu.c` defines:

```text
 XENOGEARS Kernel MENU
   %s %s MODE

     Field
     Battle
     Worldmap
     Battling
     Menu
     Movie
```

Those strings and the capture’s navy/white surface identify the rendered card. `pc_port/src/game_overrides.c` maps game state 0 to `KernelMenuMain`, state 1 to `FieldMain`, and state 2 to `func_8001B6C4`.

## Call and data path

1. The opening field4 exit reaches `src/field/main/misc4.c:func_8007954C`. For `exitCode == 0`, the retail path calls `ChangeGameState(2)` and then `MainLoop(0)`.
2. `src/slus_006.64/main/main_loop.c:MainLoop` selects `g_MainGameStates[g_CurGameState]`, resets controller state, calls that state’s main function, and recursively dispatches the next state.
3. State 2 is `src/slus_006.64/system/temp3.c:func_8001B6C4`. It calls `func_80070F40()` to run the retail battle overlay, then reads `D_800C48EA` and `D_800D3338`. For the normal return branch, the intended logic is:

   - `D_800C48EA` is `1` (normal battle return), or one of the explicit special values `0x40`/`0x21` enters the same decision block;
   - with `D_800D3338 == 0` and `D_8005947C == 0`, `D_8006F94E & 0x7ff` is compared with `0x400`;
   - map14 is below `0x400`, so the selected next state is `1` (`FieldMain`).

   The `0x81` value is an explicit special battle result that resets the map to `0x1ea`; it is not the ordinary opening return.
4. The retail battle assembly writes the result into guest memory at `D_800C48EA`. In particular, `asm/battle/nonmatchings/main/func_8007252C.s` writes `1` for the normal completion condition and `0x81` for its special condition; `func_80070F40.s` consumes and stores the battle status around its return.
5. The native bridge does not share this battle control state with the native state2 C. `pc_port/src/battle_mips_runtime.c` defines the battle overlay range beginning at `BATTLE_BASE 0x8006faf0`; its runtime initialization skips symbols at or above that address so overlay code/data remain in the loaded retail image. Its `resolve_memory` path returns `PSX_ADDR(canonical)` for those addresses. Therefore guest writes to `0x800c48ea` and `0x800d3338` land in emulated PSX RAM.
6. The native build still has generated host placeholders in `pc_port/build_native/stubs.c`: `D_800C48EA[32]` and `D_800D3338[32]`. `func_8001B6C4` reads those host arrays, which remain zero because the guest battle overlay did not write them. (`D_8005947C` is below the battle base and is bridged to shared host storage, so it is not the same split.)
7. With the host `D_800C48EA` value incorrectly left at zero, state2 computes `state == 0`, calls `ChangeGameState(0)`, and the next `MainLoop` dispatches state 0: `KernelMenuMain`. That produces the observed transient Kernel Menu. The subsequent field14 render does not make the state-0 dispatch intentional; it only shows that a later path advanced to FieldMain.

## Classification

This is a native control-state mismatch between the retail battle overlay’s guest RAM and the native state2 dispatcher’s generated host symbols. It is not an intentional retail story transition and should not be addressed by hiding the menu or suppressing its rendering. Retail’s expected ordinary opening return is battle result `D_800C48EA == 1`, followed by state 1 / `FieldMain` on map14.

## Concrete evidence required before any repair

The next bounded diagnostic should observe, immediately after `func_80070F40()` and immediately before `ChangeGameState(state)` in `func_8001B6C4`, all of:

```text
guest PSX_ADDR(0x800c48ea)[0]
native D_800C48EA[0]
guest PSX_ADDR(0x800d3338)[0]
native D_800D3338[0]
D_8005947C[0]
g_GameSceneMapNum
computed state
```

For the ordinary opening return, the expected observation is guest `D_800C48EA == 1`, native `D_800C48EA == 1` after the bridge is corrected, `D_800D3338 == 0`, `D_8005947C == 0`, map14, and computed state `1`. The suspected failure signature to confirm is a guest result of `1` with native host result `0`, followed by computed state `0` and KernelMenuMain; this audit did not instrument the live run to read the guest byte. If the guest value is `0x81` or another result, retain that as a retail condition to explain rather than forcing state 1.

No live rerun or instrumentation was performed during this audit. No source files were edited; this file is the only artifact created.
