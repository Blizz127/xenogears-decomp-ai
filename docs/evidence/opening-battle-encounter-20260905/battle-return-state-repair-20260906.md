# Retail battle return state repair (2026-09-06 UTC)

The earlier painting-room observation passed through an unintended Kernel Menu.
A normal New Game diagnostic, `opening-result-trace-5-uq_vd1pg`, confirmed the
split immediately after battle: guest result `1`, native placeholder `0`, both
guards `0`, shared return mode `0`, map `14`, and computed next state `0`.
The debugger stopped before `ChangeGameState` executed. See the before JSON.

`func_8001B6C4` now reads the two overlay-owned result bytes from guest RAM in
the native build. Direct retail-byte review additionally corrected its return
branches: a nonzero guard selects state `6`; unrecognized results skip
`ChangeGameState`. These latter control-flow corrections also apply to the
matching C. No default field state is forced and no menu rendering is hidden.

Root and the test worker independently checked retail `8001B6C4..8001B844`.
The regression executes those actual instructions, with documented boundary
spies for loading, battle, party reset, state change, and MainLoop. It covers
all 256 result bytes and 128 branch cases, compares call order and outputs,
and poisons native placeholders independently of guest inputs. The normal
loader gate is held at `-1`: the separate debug FontLoadFont 10-vs-11 argument
mismatch is explicitly outside this fixture.

Production was RED in three modes. The candidate and root's installed-source
run are GREEN: 384 cases each at O0, O2 and Clang UBSan, plus nine rejected
semantic mutations. Final root evidence:
`/tmp/xeno-battle-return-test-20260906/run.eLGT2WGt`.
Native build: LINK OK, 646 unresolved symbols, 79 function stubs, 566 data
symbols; the two unused result placeholders disappeared. The isolated
`make check` remains exit 2 before and after, with the same undeclared
`stderr` errors in animation_scripts.c and temp1.c. No matching claim.

The subsequent diagnostic `opening-result-after-5-a3gljb50` captured the
unadapted dialogue constructor (see pilot-window audit) but remained at the
pilot panel. Root requested SIGTERM after collecting that evidence; it did
not reach the post-battle result probe. Direct field return after repair
therefore still awaits the next normal replay.

## Live replay verified (2026-09-06 UTC)

`opening-window-fixed-1-bsbals5x`, binary SHA-256
`e7dc594198cd049b1e4a275aee4635a6a15b6e64687bf8f438cc589751866361`,
shows readable Fei battle dialogue, including “Guh! Boy are you persistent!”
at capture7440. Guest constructor calls now deliver mode0 and the requested
4/8 rows. Battle returns guest result1, guard0, mode0, map14; the native
dispatcher selects state1 and directly enters FieldMain. Painting-room
capture9060 shows Fei's dialogue there. The unexpected-Kernel entry probe
did not fire. MainLoop resets g_CurGameState to0 after choosing its function
pointer, explaining the0 in the FieldMain-entry observation; it does not
represent KernelMenu dispatch.

This is a natural New Game path with sparse read-only GDB observations,
not a retail-emulator framebuffer comparison or a fully uninstrumented run.
See `window-and-return-runtime-20260906.json` for pins, captures, input events,
failed title attempts, and current process/recording ownership.
