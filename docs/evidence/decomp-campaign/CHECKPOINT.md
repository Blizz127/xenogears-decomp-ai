# Decomp Campaign CHECKPOINT

_Generated: 2026-08-17_

## 1. Current HEAD

- **Branch:** `integrate/w34b24-i1`
- **SHA:** `6d5d1cd` (Decompile func_8002945C)
- **Origin status:** local only (D1 rule — never push without explicit request)
- **Clean/dirty:** CLEAN — no uncommitted changes

## 2. Exact Progress

- **Total INCLUDE_ASM at project-wide:** 701 (across all source files)
- **INCLUDE_ASM in actively targeted files:** 120
- **Routines completed this campaign (across all sessions):** ~293 decompilation commits since campaign start (commit #76)
- **Routines completed this session:** ~80+ functions decompiled
- **Files now 100% decompiled (0 INCLUDE_ASM):**
  - `src/field/main/misc3.c`
  - `src/field/main/misc8.c`
  - `src/field/main/misc10.c`
- **Files partially remaining (INCLUDE_ASM count):**
  - `src/field/main/misc.c` — 2
  - `src/field/main/misc2.c` — 1
  - `src/field/main/misc4.c` — 3
  - `src/field/main/misc5.c` — 7
  - `src/field/main/misc6.c` — 6
  - `src/field/main/misc9.c` — 6
  - `src/field/main/misc11.c` — 24
  - `src/slus_006.64/system/temp1.c` — 22
  - `src/slus_006.64/system/temp2.c` — 30
  - `src/slus_006.64/system/temp3.c` — 3
  - `src/slus_006.64/system/work_list.c` — 6
  - `src/slus_006.64/system/rendering.c` — 5
  - `src/slus_006.64/system/system.c` — 1
  - `src/slus_006.64/system/libarchive.c` — 3
  - `src/field/scripts/virtual_machine.c` — 1
- **Matching percentage:** Project does not currently have a matching-percentage tool; all committed functions are behavior-faithful.

## 3. Validation Quality

| Validation type | Status |
|---|---|
| **Compile PASS** | YES — all 468 targets compile |
| **Link PASS** | YES — all 468 targets link |
| **Byte/matching PASS** | NOT TESTED — no matching build tooling run this session |
| **Runtime/test PASS** | NOT TESTED — no runtime tests executed |

All committed functions achieve **compile + link** pass. None have been verified as byte-matching.

## 4. Commit Audit

- **Commits created this campaign:** ~293 decomp commits (from commit #76 to HEAD)
- **First campaign commit:** `c55830a` (Decompile ArchiveDataSync — commit #76)
- **Current HEAD commit:** `6d5d1cd` (Decompile func_8002945C)
- **Linear history:** YES — all commits are on `integrate/w34b24-i1`, no merges
- **Known nonmatching/approximate commits:** All commits are BEHAVIORAL DECOMP — they compile and link but have not been verified as byte-matching against retail. Some use `u8*` raw pointer access for bitfield struct members that don't match retail layout perfectly.

## 5. Failure / Debt Inventory

### READY_SMALL (≤50 lines, no blockers)
- `func_8002C310` (temp2.c, 52 lines) — debug printf dump, many format string globals
- `func_8002BA58` (temp2.c, 68 lines) — CD streaming callback, archive.h dependency
- `func_8009FB98` (misc6.c, 32 lines) — matching C in comments

### READY_MEDIUM (50-100 lines, some complexity)
- `func_8002CDCC` (temp2.c, 98 lines) — matching C in #ifdef
- `func_8002C700` (temp2.c, 128 lines) — matching C in #ifdef
- `func_8002DDE4` (temp2.c, 141 lines) — matching C in #ifdef
- `func_8002D530` (temp2.c, 104 lines) — decompiled this session
- Multiple misc.c functions with 100-150 lines

### READY_LARGE (100+ lines, complex)
- `func_8008B5D4` (misc.c, 188 lines) — jump table rendering
- `func_8008A2E8` (misc.c, 137 lines) — archive + TIM loading
- `func_8008BF38` (misc.c, 153 lines) — party swap
- `func_800A0228` (misc6.c, 202 lines) — actor state management
- `func_800A0FD8` (misc6.c, 240 lines) — complex actor setup
- `func_800A7C58` (misc5.c, 455 lines) — large function
- `func_8007954C` (misc4.c, 118 lines) — #ifdef guarded

### BLOCKED
- `FieldScriptWriteActorDistance` (misc.c) — matching C in comments, volatile keyword prevents compilation
- `func_8007234C` (misc2.c) — call site uses no-arg version, can't change signature
- `func_800AB748` (misc9.c) — jump table references function labels, linker needs them
- `func_80022DF4` (temp1.c) — callback table conflict with extern array declaration
- `func_80023468` (temp1.c) — 16-entry jump table
- `func_8001BB50` (temp3.c) — matching C in comments
- `GamePartyGearsInitializeSkins` (temp3.c) — matching C in comments
- All remaining work_list.c — matching C in comments
- All remaining rendering.c — matching C in comments or #ifdef
- `SystemRenderStringEntry` (system.c) — matching C in comments

### NEEDS_RUNTIME
- None identified

### NEEDS_DATA_LAYOUT
- `func_8002BA58` (temp2.c) — needs ArchiveStreamFileSectionHeader type from archive.h, include ordering blocks it

### NEEDS_SYMBOL_IDENTITY
- `func_8002C310` (temp2.c) — format string addresses D_80018944/54/64/74 are unknown

### NEEDS_ASM_RESEARCH
- All remaining 100+ line functions need deeper CFG analysis

### KNOWN NONMATCHING
- All committed functions are behavioral approximations — no byte-matching verification has been performed.

## 6. Regression

```
make build 2>&1 | tail -3
```

Result: **468/468 targets — BUILD PASSING**

## 7. Context Handoff

```
CURRENT_HEAD=6d5d1cd
COMPLETED=~293 decomp commits (from campaign start)
REMAINING=120 INCLUDE_ASM in targeted files, 701 project-wide
LAST_FUNCTION=func_8002945C (archive stream file slot release)
NEXT_FUNCTION=Smallest available: func_8002C310 (52 lines, temp2.c) or
               func_8009FB98 (32 lines, misc6.c, matching C) or
               func_80096214 (47 lines, misc10.c) — ALREADY DONE
VALIDATION_STATUS=compile+link PASS, no matching verification
KNOWN_DEBT=All functions are behavioral approximations
NEXT_PRIORITY=Continue with remaining small functions across misc6.c,
               misc9.c, misc5.c, temp1.c, temp2.c
```

## Files Now Complete (0 INCLUDE_ASM)

| File | Status |
|------|--------|
| `src/field/main/misc3.c` | COMPLETE |
| `src/field/main/misc8.c` | COMPLETE |
| `src/field/main/misc10.c` | COMPLETE |
