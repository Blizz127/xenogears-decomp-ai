# Retail sprite93 parent direction inheritance

Shared dirty checkout HEAD3a3e7aac03a2f166fb924945a489e392d706f282. No commit or push. Production changes: opcode93 in animation_scripts.c and func_8001D4E8 in rendering.c. The latter replaces an INCLUDE_ASM and old commented C.

## Natural observation

The preceding debugger-free run escaped its first mountain battle and returned to field15. A read-only attach/detach observed actor14 selector/sprite83/03, confirming the prior binder repair through that return. In the second encounter, opcode93 asserted at00:29:59 CDT, sprite0074F558, parent0074408C, child model0074F60C, package0074419C. Core remains ignored locally at scratchpad/lahan-natural-20260908-bd-clean/core.876907. That run is terminal; second return remained unproved.

## Retail authority and implementation

SLUS SHA256 dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119. Opcode93 body80020E30..80020F34 gates on parent at+70 and sprite mode low2 bits. It calls8001EE68 on the package's first pointer; zero result sets sprite40 type bits to1C000 underFFFE1FFF. If child model exists and parent model has direction storage, it calls8001D4E8, then copies8 records of8 bytes with both words read before either is written and model/direction pointers reloaded each iteration. Parent model bytes3C/3D follow. It then calls8001D2B0 with the current frame halfword. No null-parent-model guard is invented where retail dereferences it.

Retail8001D4E8 allocates64 bytes only when model+34 is zero, reloads sprite+20 after HeapAlloc, stores the allocation at the reloaded model+34, and calls800234AC to initialize the eight records. Production C preserves that reload and allocation size.

## Verification and limits

Pre-change test reproduces the assertion. Actual retail dispatcher, format leaf8001EE68, allocator8001D4E8 and initializer800234AC execute in the MIPS oracle; native tests link their production TUs. HeapAlloc and frame selector8001D2B0 are explicit recorded boundaries. Auxiliary rendering/temp1 object symbols are weakened only to resolve unrelated duplicate dependency guards; guards for the covered functions are renamed so the real native bodies execute. Full fixture and boundary-call state compare across null parent, zero modes, missing child model or parent directions, existing/allocation paths, overlapping direction storage, all256 format bytes, all65536 frame halfwords, and model replacement during allocation.94208 cases pass perO0/O2/UBSan. Six mutants are rejected by state/argument failures, including omitted model reload. The initial stale-model mutant fixture crashed at a null direction pointer; the fixture was corrected to keep that alternative pointer valid, and the final control rejects the wrong writes directly.

BD regression and its four mutants pass. Native build succeeds. Tests do not prove the frame selector's behavior, full dispatcher exact bytes, rendering parity, or the remaining Lahan story.

## Subsequent runtime and matching checks

The frozen normal boot `scratchpad/lahan-natural-20260908-93` (SHA256 bd247e8ca828cad9e3c58b9b2cf8aceaf78a1cb5af67b0a00ff7c67120348325) is terminal. Game947324 aborted at00:49:32 CDT at missing opcode CD during the first mountain encounter, with no debugger attached. Its core is preserved. Opcode93 execution and repeated field return were not observed in this run.

PSX build and scoped whitespace checks passed. See matching-note.md for the later allocator C-expression adjustment, behavioral regression, and remaining assembler immediate-expansion mismatch. A scratch assembly option produces matching function bytes, but the normal build configuration still differs.
