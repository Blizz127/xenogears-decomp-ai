Correction, 2026-09-06: the original standalone comparison included two
leading alignment NOPs and placed the function at801E6CF0. It was not a valid
function-span comparison. With SUBALIGN(4), the same a3085667 C source starts
at801E6CE8 and emits1248 bytes against1260 retail bytes (still not exact).
See file1-matching-baseline-corrected-20260906.json. Historical numbers below
are retained as the original review record; do not use them as the current
matching baseline. A durable463-case regression now lives in pc_port/tests.

# File-1 controller independent review — 2026-09-06

## Verdict

**ACCEPT the frozen structural C candidate for its documented controller/helper boundary contract.** No mismatch was found in the complete `801E6CE8..801E71D4` source translation. This acceptance includes the final explicit `GetStringEntry` call followed by a fresh window-pointer load; it does not refer to the earlier nested-call form.

Three gates remain distinct:

| Gate | Result |
|---|---|
| Structural controller behavior against raw retail instructions under explicit helper stubs | **GREEN**, including independent review extensions below |
| Standalone resolved PSYQ text slice exact match | **FAIL**, superseding Sol's older NOT_RUN |
| Complete module reconstruction / native runtime adoption | **NOT_RUN**; not authorized or attempted by this review |

No source/config/runtime/UI or Sol artifact was edited. Only this report and `/tmp/xeno-file1-controller-review-20260906/` were written. Root independently owns constructor integration and runtime checks.

## Reviewed authority and pins

Source: `/tmp/xeno-opening-dynamic-module-20260906/candidate/xeno_battle_command_file1_controller.c`, SHA256 `a3085667773e2cc494e5c863a8a07bd92832e64abc2d5f3a9e716f5ca959fa4a`.

Final Sol fixture: `differential_test.c`, SHA256 `ee7bced3ab02f213e5f5add5984ff3e4f14ab108b36ca2f3ad4e13c398b86377`.

Raw archive `(0x20,0)/file1` payload, freshly rehashed: 19,516 bytes, SHA256 `64668d85bf48dea46cf6d5f04b38e38ca887877cfef53961f0c26cd2d363b670`. The exact function is payload `[1CE8,21D4)`, 1,260 bytes / 315 instructions, SHA256 `1f11c9ac7117c0d702c6829cb3fb57806d73c6413eaec5f9fbf1ecca2b427a2e`. Load address is `801E5000`; the prior inventory supplies disc-sector/archive-table/load-chain authority. The caller at801E6FC0 is inside this complete function, not an independent translation unit.

Review scratch `input-pins.json` pins the source, header, final fixture/runner/report, raw module, and actual interpreter source/header. The complete final branch census is in `coverage.json`; build/run commands and exit results are in `verification.json`. No native-derived expected-buffer formula was substituted for the raw controller.

## Full-function source audit

| Retail range | Review conclusion |
|---|---|
| 801E6CE8..6D50 | The saved u16 string index, low-byte command argument, low16 flags/fallback, initial unsigned X/Y/units, and `12*units+24` agree. Completion starts at0. |
| 801E6D54..6DC4 | Nonzero phase skips creation. Creation sets cycle4. Y sentinel7FFF selects16 or140 according to flag4. Rows below5 remain unchanged, including0; rows5 and above become4 for panel-layout calculation `13*n+20`. Actual constructor rows are not clamped. |
| 801E6DC8..6E44 | Sentinel centering uses `160-(low16(layout_width)>>1)`, and repeats after possible64-pixel growth. Flag8 skips setup. Alternate setup occurs exactly when flag2 is clear and command byte is notFF. Unsigned X subtraction preserves retail wrapping. |
| 801E6E48..6ECC | Standard setup passes seven correctly narrowed arguments, then polls current UI BF. Its jump bypasses the alternate-only C8/X adjustment. |
| 801E6ED0..6F70 | Alternate setup passes the five masked local-helper arguments, then the seven setup arguments, polls BF, sets C8, and adds64 to X only when flag1 is clear. **C8 is not unconditionally set by this function's creation path.** |
| 801E6F74..6FC4 | Control/window pointers reload after setup/yield boundaries. Constructor arguments are window,896,256,low16(X)+12,Y+8,3*lhu(units),lhu(rows), with the seventh value in the call delay slot. Saved module X/Y are full derived words. |
| 801E6FC8..7018 | Reload current window, write byte+58=4, OR flags2 before reset, reset, lookup using current table and u16 index, then reload window after lookup returns and queue the returned string. Final candidate preserves this sequencing explicitly. |
| 801E701C..7038 | Set current UI C9 and current control phase1 before yielding once; continue into update checks in the same invocation. |
| 801E703C..709C | Current window flag8 gates presentation update. Controller flag8 suppresses the local position helper. Cursor loads are signed16, multipliers4/14 cannot overflow s32, and additions to saved module words use unsigned modulo arithmetic matching ADDU/ADDIU. |
| 801E70A0..70F0 | Set UI CF. If absolute mode byte800D3014 equals4, call clear-wait with current window, then reload UI and clear CF/9E. |
| 801E70F4..715C | Reload window before testing bit4. Set bit4 returns0; clear bit4 clears C9, destroys current window, yields, reloads UI and clears C8, sets completion1, and calls cleanup(0) unless controller flag8 is set. |
| 801E7160..71D0 | Reload control after cleanup, clear phase, reload the control base once before the copy loop, restore exactly five halfwords from801E9C10 to+7F6..+7FE, return completion, and finish at the actual epilogue/delay-slot boundary. The source's cached base during this loop matches retail. |

The final candidate's ordinary C helper boundaries prevent reads from being moved across the separately sequenced lookup/reset/yield calls. No helper is nested inside an argument expression whose evaluation order decides which window is passed. The explicit table/index/string/window pointer identities are now recorded by the fixture. The stale-window negative control differs at queue event5 (`80052100` retail versus`80052000` mutant), which specifically validates the requested post-lookup reload.

The complete direct-call inventory remains15 calls at the original structural sites: setup2, polling/phase/teardown yields4, local6750 once, constructor once, reset once, string lookup once, queue once, local5B00 once, clear-wait once, destroy once, cleanup once. Helper implementations remain external boundaries. Their masked scalar inputs, stack arguments, order and normalized pointer identity are compared; their actual bodies are not certified here.

Width/overflow review found no remaining source error. Initial units are unsigned16, so `12*units+24+64` remains within u32; explicit low16 casts occur before centering and setup calls. Centered X may wrap and remains unsigned until retail's explicit low16 mask. The constructor intentionally receives low16(X)+12 and Y+8 without another low16 truncation, so65535 yields65547/65543 respectively. Constructor width3*65535 remains196605. Local helper coordinate accumulation uses unsigned modulo arithmetic, preserving signed cursor extremes and poisoned saved coordinates without C signed-overflow UB.

## Test review and independent extension

Sol's final suite reports458 cases at O0/O2/Clang UBSan and ten compile-successful semantic mutants. I checked all ten current mutant logs: each contains an explicit `DIFF FAIL`, including wrong constructor/window/rows, wrong string table/queue pointer, stale pre-lookup window, missing reset/destruction/default word/openC9/closeC9. They are not presently false rejections caused by compiler errors or crashes.

The original suite compares complete control/UI/window arrays, the alternate window, the selected window global, module cycle/X/Y words, return value and ordered helper events. All scalar arguments and relevant pointer identities are retained. The model's actual side effects are narrowly stated: yields set BF, constructor sets window flag4, lookup returns a fixed string and optionally rebinds the window, clear-wait clears bit8, and other helpers only log calls.

Independent review builds linked the **unchanged frozen candidate** against a derivative fixture in review-owned scratch. That fixture adds:

- Three yields before BF becomes ready, exercising repeated polling rather than only a single retry.
- Five cases for centered negative X, widths immediately around low16 wrap, Y=FFFF, zero fallback flags, phase255 and all window flag bits.
- Poisoned initial cycle word12345678 on both sides, checking both creation's reset to4 and update's retention of prior state. Sol's original initialization always started cycle4, so its existing tests alone could not detect omission of that assignment.
- Explicit rejection if either event trace overflows, plus measured event counts.
- Raw instruction-slot visitation and conditional-branch outcomes collected from the interpreter's instruction fetches before execution.

Results: **GCC O2 GREEN463 and all-Clang O1 UBSan GREEN463**, no sanitizer diagnostic. All315 instruction slots were visited; all21 conditional branches exercised both taken and not-taken outcomes, including the standard polling backedge801E6EC0. Maximum event count was10, below the32-event limit. An additional review-only missing-cycle-reset mutant failed explicitly in the first case, with all ordinary memory/trace comparisons equal and cycle state retail4 versus mutant12345678.

These are branch/statement coverage and observed differential results, not exhaustive combinations of every global/callee side effect. The complete state space, arbitrary pointer aliasing, helper-induced control/UI rebindings outside the tested lookup case, and nonterminating polls remain outside the model. The interpreter is shared infrastructure; its instruction semantics are not independently hardware-validated by this test.

## Fixture hygiene findings

These do not change the current frozen candidate's behavioral acceptance, but should be corrected if the script is promoted into a durable acceptance gate:

1. The final runner accepts any nonzero mutant exit. Current logs all prove actual differential failures; future runs should require exit1 plus an explicit DIFF FAIL and reject crashes/other errors separately.
2. Original trace overflow is only included in `memcmp`. Simultaneous overflow on both sides could compare equal after truncation. The current bounded model cannot reach the cap; review extension measured max10 and explicitly rejected either overflow. Make the durable gate fail closed on any overflow.
3. `ARCHIVE_FILE1` can override the shell's validated payload path, but the C fixture still opens the fixed default `/tmp/xeno-opening-dynamic-module-20260906/archive20_file1.bin`. With the current default both checked and executed files are the freshly pinned same payload. If the override is used, validation and execution could refer to different files; pass the validated path to the binary or remove the override. The runner also uses current interpreter source without enforcing its hash, so the separate review pin should be retained if claiming a pinned interpreter.

No changes to Sol's runner/fixture were made by this review.

## Exact-match and native-adoption boundaries

Root's `/tmp/xeno-file1-root-matching-20260906/comparison.json` was read directly. It pins this same source hash and reports a standalone resolved candidate text of1256 bytes, SHA256 `80f4dabf582b5f6c30a8921ea38c1cdf0c009ca9320b08ff1c7c8228d16bafaf`, against1260 retail bytes with the slice hash above. `byte_exact=false`; first mismatch is offset0. Therefore **exact match is FAIL**, not merely pending/NOT_RUN. No attempt to repair matching was made here, and no full-module reconstruction gate has run.

Native adoption is separately **NOT_RUN**. In particular, this source deliberately declares the resident constructor's seven-argument retail ABI. The existing native constructor has eight arguments with a dead seventh placeholder; direct compilation against that body without the appropriate adapter would recreate an ABI error. Module-local globals/callees also require file-1 identity and correct address/pointer binding. An address-only bridge in the reusable801E5000 module window is insufficient authority to dispatch this candidate for every payload loaded there.

Accepting this source for further decompilation does not authorize replacing the current retail interpreter path. Integration must retain the module identity, resolve the helper/pointer contracts, and pass an actual adopted-path differential and natural runtime check. No pilot-string content, framebuffer parity or complete opening acceptance follows from this controller review.
