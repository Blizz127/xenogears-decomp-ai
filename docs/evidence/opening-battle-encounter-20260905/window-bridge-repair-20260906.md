# Opening dialogue constructor bridge repair (2026-09-06 UTC)

The natural opening calls retail `80032F54` from dynamically loaded module
`801E6FC0`, not from base battle.bin. Astra recovered the caller from retail
archive `(0x20,0)/file1`. It puts the row count in argument seven, then queues
a string and activates the window. See `pilot-window-audit-20260906.md` for
retail-byte and archive pins.

The native constructor deliberately retains an extra unused mode argument.
Native field/world callers compensate for it, but the guest bridge did not.
In diagnostic `opening-result-after-5-a3gljb50`, actual guest requests for four
and eight rows both arrived as native mode values, with height zero. The
constructor consequently allocated zero rows and the row renderer skipped them.
See the finalized `window-constructor-before-20260906.json`; the earlier
read-only audit inspected the capture while it was still growing.

The bridge now handles only resolved retail target `80032F54`: read the
untranslated halfword at guest SP+18, interpret it as signed16, insert mode0,
and deliver that row count as native argument eight. This follows retail's
LHU/store/LH chain and avoids treating a pointer-shaped scalar as a pointer.
An unreadable row-count argument returns failure without invoking the native
constructor. Existing native callers retain their established calling convention.

The actual production runtime bridge passes a typed constructor-ABI spy in
Clang O0/O2/UBSan; function-type instrumentation is excluded specifically for
the pre-existing generic native-call envelope. Cases include the observed
four-row call, signed boundaries, high-word aliases, eighth-slot poison,
unreadable stack data, and an unrelated target. This is a bridge ABI test,
not a complete constructor or pixel oracle. Root corrected the initial
pretranslated-scalar mutation to read args6 before clearing it; all three
semantic mutations are rejected. Final installed-source evidence:
`/tmp/xeno-window-abi-test-20260906.hiF8ugeX`.

Native build is LINK OK (646 unresolved symbols;79 function stubs;566 data).
No matching-build input changed in this bridge-only repair. The immediately
preceding return-state make-check before/after logs retain the same existing
stderr errors. Visible dialogue and direct field return are being checked in
`opening-window-fixed-5-_uibhzh0`; neither is asserted by this test result.

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

Later runtime end: after the verified opening/field14 and further world-map
progression, another battle stopped at unimplemented sprite opcode0xA9,
index31 (sprite00731F04, operands0071E4AE). This is preserved in final-run.log
and the runtime JSON. Native/debugger are no longer live; root did not stop
them. Recording finalization has not been asserted.
