# Decomp Campaign CHECKPOINT

_Generated: 2026-08-19_

## 1. Current HEAD

- **Branch:** `integrate/w34b24-i1`
- **SHA:** `8d2e41c6` (Decompile func_8001C074 — menu render tick)
- **Origin status:** local only (D1 rule — never push without explicit request)
- **Clean/dirty:** CLEAN — no uncommitted changes

## 2. Exact Progress

- **Universe:** 2477 functions (asm .s union across 4 overlays)
- **Matched {}:** 1940 (78.3%)
- **Coexistence:** 170 (6.9%)
- **Unported:** 367 (14.8%)
- **Total done:** 2110 (85.2%)
- **INCLUDE_ASM remaining (project-wide):** 537
- **Files 100% complete:** 7 (misc3, misc8, misc10, misc11, work_list, archive, system)
- **Decomp commits (this campaign):** 572

## 3. Validation Quality

| Check | Status |
|-------|--------|
| Compile (468/468) | **PASS** |
| Link (468/468) | **PASS** |
| Byte-matching | NOT VERIFIED |
| Runtime test | NOT VERIFIED |

## 4. Commit Audit

- **Total decomp commits:** 572
- **First campaign commit:** `c55830a` (Decompile ArchiveDataSync)
- **Current HEAD:** `8d2e41c6` (Decompile func_8001C074)
- **Linear history:** YES — all on `integrate/w34b24-i1`
- **Known nonmatching:** All functions are behavioral approximations

## 5. Remaining Workload

### BLOCKED (4)
- func_8007234C (misc2.c) — call-site mismatch
- func_800AB748 (misc9.c) — jump table
- func_80023468 (temp1.c) — jump table  
- func_80022DF4 (temp1.c) — callback table

### BIOS Stubs (28)
- libapi.c (7), libapi_2.c (7), libsn.c (8), libcard.c (6)

### Menu #ifdef Wall (~105)
- Most remaining menu functions are inside `#ifndef XENO_PC_PORT`

### READY_MEDIUM (6 functions, 81-90 lines)
- func_801E76EC, func_801E1418, func_801C9270, func_801CB8AC, func_801D827C, func_801D249C

### READY_LARGE (100+ lines)
- 200+ functions across misc5/6/9, temp1/2/3, libgte, menu, etc.

## 6. Regression

```
make build → 468/468 PASSING
```

## 7. Context Handoff

```
CURRENT_HEAD=8d2e41c6
COMPLETED=2110/2477 (85.2%)
REMAINING=367 unported
LAST_FUNCTION=func_8001C074 (menu render tick)
NEXT_TARGETS=func_801E76EC (menu, 81L), func_8004463C (libgpu, 81L)
VALIDATION_STATUS=compile+link PASS, no matching verification
KNOWN_DEBT=All behavioral approximations; menu #ifdef wall; BIOS stubs
NEXT_PRIORITY=Continue menu 81-90L functions; libgpu.c
```
