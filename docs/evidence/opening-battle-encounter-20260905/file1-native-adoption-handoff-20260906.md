# File-1 native controller adoption — frozen implementation

Date: 2026-09-06. Repository: `/var/home/blizz/Projects/xenogears-decomp-ai`.
Ownership: battle runtime/internal header, private file-1 adapter, dedicated adoption tests, and the parent-authorized mechanical controller declarations split. No build script, controller function-body, docs handoff, native build, desktop, or live process changes from this lane.

## Result

Native substitution is implemented only for `0x801E6CE8` under proven archive-20/file-1 load identity. The actual production bridge/binding/service passes the 463-case retail-instruction corpus plus 10 additional cases at Clang O0, O2 and UBSan: 4,319 assertions per mode, all 315 retail instruction slots and both outcomes of all 21 branches. Ten compiled semantic controls fail with exit 1 and the explicit `FILE1 ADOPTION FAIL` marker.

Evidence: `frozen/{O0,O2,UBSan}.log`, `frozen/pins.json`, `frozen/negative-controls.json`, and per-control logs. Reproduce with:

```
python3 pc_port/tests/run_battle_file1_adoption_test.py --out /tmp/CHOOSE-NEW-DIRECTORY
```

Production source is frozen at these SHA-256 values:

| File | SHA-256 |
|---|---|
| `pc_port/src/battle_mips_runtime.c` | `a0ef5fc6c138d894f584b1c0ca17804952ec347771832cf2e3abcb2418970755` |
| `pc_port/src/battle_mips_runtime_internal.h` | `26de7c57e4c93a481b27084603a6240fd80af9a52052101bade169091dacf9d3` |
| `pc_port/src/battle_file1_controller.h` | `c3ed7df6a4680986db0b4532586fb6be9db242973883c0622f17a880096fde9b` |
| `pc_port/src/battle_file1_controller.inc` | `a4d8c85d71955d185418d7a6398a519d61d6955e7b0970cb0009caf803f8ddee` |
| `pc_port/tests/battle_file1_adoption_test.c` | `d039264cda724db4ad1c20b77ca5cb24dc63a16fded3ec2033f3b5e45bf863f4` |
| `pc_port/tests/run_battle_file1_adoption_test.py` | `09dd3edef7941092a239bd1404877ea62ca4fc27c1e2e034df29b67b1887cc9e` |
| `src/battle_command_file1/message_controller_impl.inc` | `c1d5eab2545fd028a9f45e22350c5f2da639b6027f6b6352c71eb49ee37bdf7b` |
| `src/battle_command_file1/message_controller_bindings.inc` | `deb85054c5d5dfebf2a8bf30a5f06f6b8b29c4ad6f3b3614cb5f97b4750d4041` |

The shared controller comment/function body is byte-identical to the pre-split exact source `a1dfab19...`: body SHA-256 `20c58e59ecf4c271a14920ba49c31becc7aa6613840dc1638fec60c0943be2d7`. See `body-split-verification.json`. Parent independently reports exact/full-module PASS at `/tmp/xeno-file1-build-check-f9ph734c` and original 463-case/11-control PASS at `/tmp/xeno-file1-controller-retail-j0hvqpyo`. Those checks were parent-owned.

## Identity and invalidation

The bridge observes `ArchiveSetIndex(0x20,0)` returning 3087 and the succeeding ordinary `ArchiveReadFileToBuffer(1,0x801E5000,0,0x80)` with current archive offset 3087 and decoded aligned size `0x4C3C`. It establishes authority only after successful native completion and full payload SHA-256:

`64668d85bf48dea46cf6d5f04b38e38ca887877cfef53961f0c26cd2d363b670`.

The native ordinary archive path is synchronous (`pc_port/src/archive_port.c:483`); the post-call hash precedes any guest initializer execution. This does not authorize debug-file or streaming variants. Before the read, the existing read-only `ArchiveDecodeAlignedSize` supplies its overlap extent. A missing decoder, nonpositive size or debug archive mode conservatively invalidates authority. This adds one read-only size query before the native routine's own size query.

Overlapping ordinary, sector, or direct bridged archive reads invalidate before invocation. Every observed write overlapping immutable module bytes also invalidates, even if writing the same value. Entry/exit of `func_80070F40` invalidate identity and clear the archive-selection observation. Generations change on invalidation. Unobserved native writes are caught by checking `[0x801E5000,0x801E9B5C)` at dispatch and helper boundaries against:

`8c071b7c6edffcb922b798ceddfb1a09167d22b402ae5d0d268d9252bf4a15c4`.

Mutable defaults/cycle/saved coordinates remain writable without discarding identity. A nonoverlapping resource read does not discard an otherwise valid module. Unproven identity returns bridge-unhandled and continues the raw interpreter. Once selected, pointer/helper/generation failure returns an error; it never retries raw instructions after partial native mutation.

## Native binding and ABI

The private adapter is included by `battle_mips_runtime.c`, so no additional native object or build source-list entry is required. The adapter defines checked bound names and includes the exact same controller body as the PSX compilation. `message_controller_bindings.inc` holds the prior declarations/record layout/access macros; its extern declarations are disabled only for this native binding.

Each source-level global access freshly reads packed RAM. Pointer validation precedes canonicalization and rejects invalid high bits, misalignment and physical-RAM overrun. Physical KSEG0/KSEG1 aliases and actual native pointers into the same 2-MB RAM are supported. Global control/UI/window records require their accessed extents; string lookup validates the selected halfword-entry extent before the existing resident lookup, then validates the returned pointer. The binding explicitly requires little endian, 16-bit/32-bit scalar widths and the retail typed-record offsets.

The five guest helper boundaries use the existing production `PcPort_BattleMipsCallGuest`, unchanged. The six resident boundaries use a nested CPU with the same `SP-0x50` outgoing frame, inherited GP/register environment, raw argument words and the existing production resident bridge. Consequently `0x80032F54` uses the existing single low-16 signed-height/seventh-to-eighth argument conversion. Resident return pointers retain both result words before checked RAM conversion. Bridge context is restored before error propagation, including nested adoption. Adapter errors unwind through a per-call context; performed RAM writes are not rolled back.

## Test scope and controls

The new runner extracts the existing oracle bus/helper stubs, case corpus and instruction-coverage counters mechanically; it does not regenerate/copy a C retail oracle. The pinned raw payload supplies all 315 controller instructions. Native execution goes through the production `runtime_bridge` identity selector, shared function body, RAM bindings, guest-call service and resident bridge.

Only external helper implementations are fixture boundaries: the five guest helpers are intercepted on the production service's real nested CPU, and the six resident helpers are exact-12-word bridge spies. The constructor spy specifically checks the retained native eighth-argument ABI. The test does not claim execution of the entire guest helper graphs or native window allocator/renderer bodies.

Every case compares result, ordered helper arguments, the entire physical RAM except the compiler-owned `0x50` stack frame, and caller saved registers/context. The excluded frame contains raw compiler locals/saved registers that native C does not produce; its argument placement is checked separately by boundary spies and the service regression. Additional cases exercise seven control/UI/window rebindings at yield, nested controller adoption, mutated in-RAM defaults and pointer-shaped guest scalar results. Generation invalidation during yield and selected helper/pointer errors assert restored caller CPU/context. The corpus uses distinct valid object allocations; it is not a proof for fabricated object aliases into module globals or every possible corrupt internal linked-list state inside resident helpers.

The ten controls are wrong archive, blind full-payload acceptance, stale authority after invalidation, skipped immutable hash, masking invalid pointers, skipped helper generation check, wrong resident frame, wrong constructor stack offset, constant defaults and stale window binding.

Unchanged guest service regression: `guest-final/` and `guest-final.log`, 202 assertions at O0/O2/all-Clang UBSan plus all six semantic controls. Existing constructor ABI regression: `window-regression/` and `window-regression.log`, all three modes plus missing-shift/wrong-slot/raw-scalar controls PASS. These remain ABI/service tests, not full constructor or glyph oracles.

## Remaining integration gates

Parent owns independent adoption-suite execution, native build, and natural opening/field-return replay. None was run from this lane. Runtime observation must show an actual hit on `file1_bound_controller` under a verified `runtime->file1` generation; a successful opening alone could otherwise still be raw fallback. Useful read-only probes are `file1_try_controller`, `file1_bound_controller`, `file1_after_archive`, `runtime->file1.{verified,generation,archive20_selected}`, the current bridge CPU SP and the packed globals.

No flags, waits, controller C semantics, archive payload, global target gate, or public callback behavior are forced or replaced. Each guest helper retains the service's existing instruction bound; controller waits retain their retail conditions, without a new host timeout. Full native gameplay/visible lettering and direct field return remain separate runtime acceptance gates.
