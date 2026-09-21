# Archive 0x20/file 1 decompilation foundation

The battle dialogue controller now has a module-specific extraction/configuration
boundary and a durable retail-instruction regression. This payload is distinct
from base battle and from other modules loaded at the same801E5000 address.
Native adoption remains unimplemented; the current port still interprets retail
controller instructions.

## Files and reproducible checks

- `tools/scripts/extract_battle_command_file1.py` verifies the complete retail
  disc, directory/header entry, exact19516-byte payload and1260-byte controller
  slice before writing an ignored local output. It refuses differing existing
  output. Default output is `disc/battle_command_file1.bin`.
- `config/battle_command_file1.yaml` separates prefix tables, assembly before
  the controller, controller C boundary, following assembly and module data.
  The symbol map has module-specific names and keeps reused addresses distinct.
- `src/battle_command_file1/message_controller.c` defaults to INCLUDE_ASM.
  `XENO_FILE1_USE_C` selects `message_controller_impl.inc` for its separate C
  comparison. The native build's SKIP_ASM guard leaves the default inert.
- `python3 pc_port/tests/run_battle_command_file1_controller_retail_test.py`
  tests the source `.inc` directly against the pinned retail slice. Each fresh
  output directory preserves source/tool/payload pins and logs.

The durable suite covers463 cases at O0, O2 and Clang UBSan. All11 semantic
mutants must compile successfully, then exit1 with a differential failure.
Both trace overflow fields fail comparison. The validated payload is passed
by argv to the executable. Helper functions are boundary spies; this suite
is not full helper execution, native integration or framebuffer parity.

Root independently passed the initially packaged suite at
`/tmp/xeno-file1-controller-retail-cqhai0if`. After defaulting to the actual
`.inc`, Luna passed it at `/tmp/xeno-file1-controller-retail-eoyt_1yy`.
Both used the same a3085667 baseline source bytes. The improved c84a38eb
source is now installed in the `.inc`; Luna passed it at
`/tmp/xeno-file1-controller-retail-u80gd8tf`, then root independently passed
it at `/tmp/xeno-file1-controller-retail-q_jw41cf` with all463 cases per mode
and all11 semantic controls. See `file1-foundation-20260906.json`.

Final coverage correction: the original packaged fixture defaulted to one
yield, which left one wait branch without both outcomes. The runner now
compiles with `AUDIT_YIELDS=3` and requires all315 retail instruction slots
visited and all21 branch sites to report both outcomes. Luna passed the final
runner at `/tmp/xeno-file1-controller-retail-erl2rshc`; root independently
passed at `/tmp/xeno-file1-controller-retail-h5bfxx02`, at O0/O2/ClangUBSan,
with all463 cases per mode and all11 semantic controls. Earlier output roots
above are historical checkpoints; this is the final durable acceptance run.

Astra's full matching report is `file1-controller-c-matching-20260906.md`;
its remaining13 instruction words are also preserved in JSON.

The improved C uses retail-width local variables, reloads and argument/store
scheduling. It emits the correct1260-byte function; it is still nonmatching.

## Matching baseline correction

The earlier standalone1256-byte comparison had two leading alignment NOPs:
its function actually began at801E6CF0. With SUBALIGN(4), the a3085667 draft
begins correctly at801E6CE8 and emits1248bytes against1260retail bytes.
It still fails byte matching. The corrected baseline is preserved in
`file1-matching-baseline-corrected-20260906.json`; historical comparison files
are retained with correction notices. A successful assembly reconstruction
must never be reported as successful C matching.

## Independent full-module reconstruction

`python3 tools/scripts/check_battle_command_file1.py` extracts and rebuilds
all 19,516 bytes from generated assembly in a retained isolated directory.
`--with-c` adds a separate C-controller and full-module comparison. It exits
nonzero if that C comparison fails, even when the assembly gate passes.
The checker pins the disc/payload, configuration, symbol maps, source files,
compiler/assembler inputs and container identity. It mounts the repository
read-only for compilation and leaves its generated files outside the checkout.

Root output `/tmp/xeno-file1-build-check-xceyjpdv` proves:

- Full assembly module: exact 19,516 bytes, SHA256 `64668d85bf48dea46cf6d5f04b38e38ca887877cfef53961f0c26cd2d363b670`.
- C build and symbol placement: PASS, entry801E6CE8, size1260, SUBALIGN(4).
- C function and C-containing module: FAIL;302 of315 instruction words match.
  Thirteen words differ, confined to the sentinel reload and constructor
  scheduling regions. First difference is function offset124.
- The C-containing module also has the correct19,516-byte length, but its
  hash differs. Equal length and behavior tests do not establish exactness.

The manifest is copied to `file1-module-build-check-20260906.json`. The command
returned1 because C byte matching is incomplete. No assembly or binary patch
was substituted for the compiled C to improve its comparison.

## Build and runtime boundaries

The native build passes with646 unresolved symbols,79 function stubs,
566 data stubs and18 checked port overrides. New binary SHA256:
`59028f92d36d02ecdf7888727e51b75e138e312defe748054164ec081610537d`.
Log: `/tmp/xeno-file1-foundation-native-build-20260906.log`.
There was no new runtime replay after this scaffold-only rebuild. The previous
constructor replay remains pinned to its ed9987ed binary and proved visible
Fei dialogue plus return to the painting room on an isolated virtual display.

The fresh isolated global `make check` still fails at the known resident
jump-table labels (`/tmp/xeno-file1-baseline-make-check-20260906.log`). The
new module is deliberately separate from that global overlay list.

Sol's read-only `file1-native-adoption-audit-20260906.md` identifies the next
integration requirements: loaded-payload identity, emulated-RAM bindings with
retail reload timing, shared retail7-to-native8 constructor conversion and
multiargument guest reentry for remaining assembly helpers. No address-only
native dispatch has been added. Full decompilation, exact matching and retail
pixel parity remain open.
