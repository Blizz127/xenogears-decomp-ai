# Generic host-call ABI mismatch — isolation and proposed fix (for review)

Status: PROPOSAL ONLY. No production behaviour changed. Requested by the
coordinator (Root msg d45fb128ab5d4f3d) as non-destructive source/test work.

## The defect

`runtime_bridge_call` (pc_port/src/battle_mips_runtime.c) resolves a guest call
target to a host symbol with `dlsym` and, unless a typed special case matches
first (trig, CompMatrix, effect callbacks, 8008ABB8, the GTE/GPU helpers),
calls it through

    typedef uintptr_t (*GenericHostFunction)(uintptr_t x 12);

regardless of the callee's real prototype. That is undefined behaviour in C.
On SysV x86-64 it works for integer/pointer arguments of at most 8 bytes, which
is why every gcc regime passes. Concrete exposures:

1. clang `-fsanitize=function` rejects it (first hit: HeapChangeCurrentUser,
   `void(int, void *)`, in Codex's `heap-wrapper-before-clang.log`, which
   predates the 8008ABB8 candidate). gcc has no function sanitizer, so the
   project's O0/O2/UBSan bar cannot see this class.
2. Return width: production stores `gpr[3] = result >> 32`. For an `int` or
   narrower callee the upper half of RAX is unspecified by the ABI, so v1 is
   compiler-dependent garbage. `void` callees leave RAX unspecified, so v0 is
   as well. Retail callers that ignore v0/v1 are unaffected. A caller reading
   them would diverge silently.
3. Narrow parameters (`int16_t`, `uint8_t`) depend on the callee extending
   them, and the generic path passes whatever upper bits translate_argument
   produced. Most typed special cases exist because of this (e.g. effect
   callbacks, trig angle masking).
4. Non-Windows-portable: the Win64 ABI and any future ARM64 port differ in the
   variadic-free twelve-argument spill layout. The current Windows build works
   only because the same integer-register argument passing holds.

## Isolation (this commit)

`pc_port/tests/run_generic_host_call_abi_test.sh`: copies the production
typedef out of the source (fails if its shape or the single cast site
changes), then calls a `void(int, void *)` and an `int(int)` host function
through it. Expected and observed: gcc O0/O2/UBSan run it undetected, clang
`-fsanitize=function` rejects it, and the correctly typed control passes. It
reports "KNOWN LIMIT REPRODUCED". It is not a boundary pass, and it fails when
the behaviour changes, so it has to be updated together with a fix.

## Proposed minimal fix (staged, no behaviour change per step)

1. Measure: an opt-in `XENO_BATTLE_GENERIC_CALL_LOG` (or the existing
   `trace_calls`) set of distinct `resolved->name` that reach the generic
   call over the R3 battle window and the field14 opening battle. Likely a few
   dozen names, not the whole bridge map.
2. Generate typed thunks for exactly those names, in a new generated
   `battle_bridge_thunks.inc` built from their real C declarations (headers
   under include/ and pc_port/src; overlay leaves already have
   `battle_overlay_host_tus.py --leaf-types` metadata):

       static uintptr_t thunk_HeapChangeCurrentUser(const uintptr_t *a)
       { HeapChangeCurrentUser((int)a[0], (void *)a[1]); return 0; }

   Returns are widened explicitly (signed for signed types, pointer via the
   existing GuestAddr rule only where it is today), so v1 becomes 0 or a
   defined sign extension instead of RAX garbage.
3. In the bridge, use the thunk when present, otherwise the current generic
   call, unchanged. Add a clang `-fsanitize=function` regime to the heap and
   leaf bridge tests as an additional regime. gcc O0/O2/UBSan stay as they are;
   nothing is weakened.
4. Once measurement shows full coverage of reached names, make the generic
   fallback a loud refusal under the interpreter-disabled mode (never in
   Original/hybrid production) so a new unthunked name is visible.

Risk to retail parity: step 2 changes v0/v1 only where they were unspecified
before. The battle_state_hash identity run and the overlay differential prover
must pass before adoption. Effort: one generator plus a few dozen
declarations. No new dependencies.

## Step 1, static part — census (2026-10-03)

`tools/scripts/battle_generic_call_census.py` (read-only, no build) classifies
every direct `jal` in disc/battle.bin (SHA 1830b4ef…) by target. Receipt:
`generic-call-census.json` in the private evidence root (SHA c1032081…).

- 1005 distinct targets: 350 reach the generic call (213 main-executable
  functions, 137 allowlisted overlay leaves, 1836 call sites), 13 typed
  special cases, 638 interpreted guest targets, and 4 data words that decode
  as jal.
- Grades for the 350 generic callees, using source definitions where they
  exist: 130 void return (v0 and v1 unspecified), 45 32-bit return (v1
  unspecified), 36 pointer or 64-bit, 104 narrow parameter or return,
  27 conflicting definitions, 6 declaration-only conflicts, 1 unknown
  (func_80025D4C), 1 variadic.
- Highest-risk class: narrow returns through the generic path. Bits above
  8/16 in v0 depend on the callee. func_80089C08 `u16(u8)` (77 sites),
  func_8001BD40 `u8(u8,u8)` (24), func_8007A280 `u16(u8,u8,u16,u8)` (12),
  func_80079ED8, func_80089BEC, func_80089B50, func_800885D0, func_8009C050.
  Production compiles with gcc -O0 (build_port.sh GFLAGS), which in practice
  extends inside the callee, so today's binary works by toolchain habit, not
  by ABI guarantee. A clang or optimized build could change guest v0.
- `FontPrintf(char *, ...)` (3 sites) is variadic, and the generic
  non-variadic call leaves %al unset under SysV. It needs a typed thunk or a
  refusal.
- 27 names have more than one non-static definition in the sources
  (e.g. WorkListAddTask, Square0, ReadGeomScreen, func_8003BDFC `int` vs
  `s16`). Which one dlsym binds depends on what the port links; the thunk
  generator must take the linked definition, not a header guess.
- Not covered: 48 indirect jalr sites, the file1/intro/results modules,
  dlsym exportability, and the dynamic subset actually reached (the
  runtime-trace part of step 1, which needs a run).

Suggested first thunk batch for review: the 8 narrow-return callees plus
FontPrintf, then HeapFree (98 sites), func_80021AD8 (86) and AddPrim (52).

Not done here: runtime part of step 1, steps 2–4, any runtime build, the differential prover.
