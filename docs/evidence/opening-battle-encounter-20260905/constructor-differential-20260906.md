# Constructor retail differential and candidate

Final scope: scratch only. `system.candidate.c` is a full copy of the currently pinned `system.c` with changes confined to `func_80032F54`. No repository source was edited and no game/UI/replay was launched. The native eight-argument ABI is preserved, including the unused mode argument. The existing bridge repair is untouched.

## Result

| Build | Original production body | Candidate |
|---|---|---|
| GCC O0 | RED: 112 differing completed cases | GREEN: all 124 cases |
| GCC O2 | RED: 112 differing completed cases | GREEN: all 124 cases |
| Clang O1 + UBSan, nonrecovering | RED: negative signed shift in actual production body | GREEN: all 124 cases, no sanitizer report |

There are 31 input tuples under four retained-memory patterns: **112 full constructor executions** and **12 allocator-boundary-only cases**. All six observed opening tuples are included and checked against the pinned runtime JSON by `prepare.py`. Each comparison examines the entire 655,360-byte arena, including the complete 0x90-byte window, all allocated rows, the entire raster request, redzones, and unused storage. The raw-retail runs execute 120,800 MIPS instructions per matrix, including the three GPU helpers and their internal leaves. There is no native-derived expected-buffer implementation.

Seven separately compiled semantic controls at O2 produce explicit `CONSTRUCTOR_RETAIL_FAIL` results:

| Candidate mutation | Differing cases |
|---|---:|
| Restore raster memset | 108 |
| Swap the two CLUT globals | 108 |
| Change large-width subtraction 240 to 256 | 8 |
| Omit second allocation | 120 |
| Mask signed row x to 16 bits | 12 |
| Reuse untruncated height | 8 |
| Zero-extend the left width for its OR | 12 |

The runner requires the explicit FAIL summary for controls, not only exit code 1. Candidate success requires PASS and no sanitizer error. Production UBSan RED is classified separately and requires a diagnostic in `constructor.production.c`; fixture/interpreter faults cannot satisfy that gate.

Targeted Clang UBSan subprocesses also establish distinct original-source errors: width -1 shifts a negative value; textureY=INT_MAX overflows when adding13; high coordinate bits make the original signed y shift unrepresentable. The candidate passes those same cases in the full matrix.

## What changed and why

- Remove the raster memset. Retail never writes through the work pointer during construction; the successful allocator boundary does not initialize its returned payload. In opening9/pattern33, the original differs in782 raster bytes while all nine ordered helper events agree. Thus this is a constructor-state mismatch on opening input, independently of its visible downstream effect.
- Normalize x/y/height to their signed low halfwords, matching the retail stores/reloads. Normalizing texture coordinates is equivalent for their stored low-bit UV/RECT values and final masked tpage helpers; it also avoids demonstrated signed overflow for high input bits. These conversions retain the supported target's signed16 representation.
- Use unsigned shifts for packed coordinates and the low16 pixel-width calculation. Preserve the whole sign-extended x operand in each retail OR; do not introduce conventional coordinate masking.
- Preserve sign extension of the left-width word and the complete background-width OR operand. Width8192 produces retail FFFF8004 for the left-size word; width16384 makes the retail background word's high half differ from the old masked expression. Negative width -1 tests the signed shift and left-width behavior with a safe 56-byte work request.

Allocation order, tags, flags, sizes on normalized inputs, row layouts/clones, palette selection, primitive opcodes, right-width subtraction240, and GPU helper sequence remain unchanged. No guard substitutes a different behavior for negative rows or negative stride: the fixture stops those cases at its explicit allocation boundary.

## Oracle and boundary model

`prepare.py` freshly validates the retail SLUS hash and constructor/helper slices from the parent audit pins. It extracts the actual production constructor verbatim, and extracts the actual PsyCross GetTPage/SetSemiTrans/SetDrawMode bodies, changing only their symbol names so the fixture can log calls. `fixture.c` uses the existing `pc_port/src/battle_mips_adapter.c` instruction interpreter and executes the original constructor plus actual retail GPU instruction bytes, including branch/load delay semantics implemented by that interpreter. This oracle is independent of the native constructor implementation; it is not independent hardware validation of the interpreter itself.

Only `HeapSetCurrentContentType` and `HeapAlloc` are serviced as stubs. The tag setter is modeled as a halfword assignment. The allocator records request size, flag, current tag, order and returned pointer, then resets the tag to20. Its success model provides distinct fixed buffers without clearing them. It does **not** test real best-fit selection, fragmentation, consolidation, headers, failure policy or allocation metadata. Row requests over32768 bytes and raster requests over524288 bytes stop before returning storage. Negative rows, negative stride, and signed-stride wrap are compared only up to those boundaries. The high-bit height cases complete in retail/candidate but the original native body reaches the bounded oversized-request stop, exposing the differing request.

Guest and native use separate storage initialized identically; the native test process exclusively maps its scratch arena with `MAP_FIXED_NOREPLACE`. Both domains deliberately use identical numerical addresses for window/row/work pointers, eliminating pointer-normalization exceptions from byte comparisons. The guest write bus accepts only window bytes, requested payload bytes and the bounded stack; unexpected guest redzone writes fault. Native arena redzones and all unused bytes are compared too. The surrounding mapped arena gives guards around each allocation, including zero-sized allocations. Payloads use four deterministic nonzero-rich patterns rather than zero-fill; the two CLUT globals have distinct patterned values. All native functions are ordinary compiled calls into the extracted actual bodies.

The normal PSX GPU mode byte is zero for these cases. The retail alternate-chip draw-mode mask is not claimed equivalent for arbitrary tpages. No GPU rendering, VRAM upload, decoder, queue, text timing, retail heap lifecycle or whole-opening behavior is certified here.

## Reproduce and review

Run from any directory:

```sh
python3 /tmp/xeno-window-constructor-audit-20260906/differential/run.py
```

This creates only scratch outputs and requires the pinned original production source. It intentionally fails closed if that source has already changed; after integration, use a deliberately adapted production acceptance test rather than silently replacing the baseline pin. It requires the existing repository interpreter/headers, GCC, and `/home/linuxbrew/.linuxbrew/bin/clang`. O0/O2 compile and link with GCC; **every UBSan object and the final executable compile/link with Clang**. The temporary `UBSan` argument reruns only that gate when addressing a toolchain issue.

Primary review files: `system.candidate.c`, `candidate.patch`, `fixture.c`, `prepare.py`, `run.py`, `results.json`, `manifest.json`, and `final-pins.json`. Per-variant logs retain all differing cases and the first byte/event disagreement. The retail `.bin` slices are private scratch authority inputs and must not be committed.

During fixture development, the installed GCC UBSan runtime path was unavailable. Switching the entire sanitizer lane to Clang also exposed a fixture pointer-expression issue (`ram + address - base` formed an out-of-range intermediate pointer); it was corrected to subtract integer offsets before pointer addition. The final matrix uses the corrected fixture and an all-Clang sanitizer lane. Those development failures are not counted as production evidence.

Root still owns candidate integration, the actual production checks/build, decoder/clear-boundary review, and an unforced opening replay. The original ABI fixture and this constructor differential establish different contracts; neither substitutes for visible retail/opening acceptance.
