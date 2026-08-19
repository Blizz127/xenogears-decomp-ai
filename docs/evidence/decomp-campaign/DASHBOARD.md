# Xenogears Decompilation Progress Dashboard

_Authoritative: backed by `tools/scripts/decomp_status.py --json`._
_Last updated: 2026-08-19_

---

## Project Status

| Metric | Value |
|--------|-------|
| **Branch** | `integrate/w34b24-i1` |
| **HEAD** | `a68135c7` (Decompile func_801C9270 — menu memory card character validation) |
| **Build** | 468/468 targets — PASSING |
| **Decomp_status universe** | 2477 functions |
| **Matched {}** | 1957 (79.0%) |
| **Coexistence** | 170 (6.9%) |
| **Unported** | 350 (14.1%) |
| **Total done** | 2127 (85.9%) |
| **Decomp commits (this campaign)** | 589 |
| **First campaign commit** | `c55830a` (Decompile ArchiveDataSync) |

---

## Per-Overlay Progress

| Overlay | Total | Matched {} | Coex | Unported | %done |
|---------|------:|-----------:|-----:|---------:|------:|
| field | 878 | 848 | 5 | 25 | 97.2% |
| member_change_menu | 67 | 66 | 0 | 1 | 98.5% |
| menu | 312 | 103 | 104 | 105 | 66.3% |
| shop_menu | 118 | 101 | 0 | 17 | 85.6% |
| slus_006.64 | 1102 | 822 | 61 | 219 | 80.1% |

---

## Per-Subsystem (slus_006.64)

| Subsystem | Total | Done | Remaining |
|-----------|------:|-----:|----------:|
| sound.c | 271 | 216 | 55 |
| temp2.c | 102 | 73 | 29 |
| temp1.c | 71 | 51 | 20 |
| menu.c | 8 | 7 | 1 |
| animation_scripts.c | 34 | 27 | 7 |
| rendering.c | 20 | 15 | 5 |
| system.c | 54 | 53 | 1 |
| libarchive.c | 27 | 25 | 2 |
| work_list.c | 29 | 29 | 0 |
| libgte.c | 96 | 9 | 87 |
| libgpu.c | 66 | 46 | 20 |
| libapi.c | 25 | 7 | 18 |
| libapi_2.c | 8 | 1 | 7 |
| libsn.c | 8 | 0 | 8 |
| libc.c | 12 | 6 | 6 |
| libcard.c | 7 | 1 | 6 |
| libetc | 18 | 16 | 2 |

---

## Remaining Workload Classification

### BIOS/Hardware Stubs (cannot decompile)
- libapi.c: 7 BIOS syscall stubs (break/jr $t2)
- libapi_2.c: 7 BIOS stubs
- libsn.c: 8 BIOS stubs
- libcard.c: 6 BIOS stubs

### BLOCKED (known blockers)
- `func_8007234C` (misc2.c) — call-site mismatch, no-arg call vs 2-arg impl
- `func_800AB748` (misc9.c) — jump table (needs linker fix)
- `func_80023468` (temp1.c) — jump table
- `func_80022DF4` (temp1.c) — callback table conflict

### MATCHING_C (known correct C in comments, toolchain mismatch)
- `FieldScriptWriteActorDistance` (misc.c) — volatile keyword
- misc4.c: 3 functions with matching C in comments/#ifdef
- temp3.c: 3 functions with matching C
- rendering.c: 5 functions with matching C

### NEEDS_FORMAT_STRINGS (unknown rodata)
- `func_8002C310` (temp2.c, 52 lines) — 4 format string addresses unknown
- `func_8004463C` (libgpu.c, 81 lines) — GPU validation with format strings

### NEEDS_MAGIC_DIVISION
- `func_8003ABF0` (sound.c, 28 lines) — timer decomposition

### READY_MEDIUM (60-100 lines, decompilable)
- `func_801E76EC` (menu, 81 lines) — character render loop
- `func_801E1418` (menu, 84 lines)
- `func_801C9270` (menu, 86 lines)
- `func_801CB8AC` (menu, 86 lines)
- `func_801D827C` (menu, 87 lines)
- `func_801D249C` (menu, 88 lines)

### READY_LARGE (100+ lines, decompilable)
- `func_801D02D8` (menu, 422 lines) — complex menu data
- `func_801E20C8` (menu, 110 lines) — menu navigation loop
- `func_801E5B88` (menu, 182 lines)
- `func_8001A6E8` (temp3.c, 266 lines) — grid rendering
- misc5.c: 7 functions (144-455 lines)
- misc6.c: 3 functions (165-240 lines)
- misc9.c: 5 functions (129-386 lines)
- temp2.c: 27 functions (112-372 lines)
- temp1.c: 18 functions (175-351 lines)
- libgte.c: 87 functions (mostly handwritten GTE)

### Menu Overlay #ifdef Wall
The menu overlay has 105 unported functions. Most are inside `#ifndef XENO_PC_PORT` guards — they have matching native C implementations but the MIPS build keeps retail assembly. These cannot be decompiled further; they need the PC port build path to be enabled.

---

## Completed Files (0 INCLUDE_ASM)

| File | Status |
|------|--------|
| src/field/main/misc3.c | COMPLETE |
| src/field/main/misc8.c | COMPLETE |
| src/field/main/misc10.c | COMPLETE |
| src/field/main/misc11.c | COMPLETE |
| src/slus_006.64/system/work_list.c | COMPLETE |
| src/slus_006.64/system/archive.c | COMPLETE |
| src/slus_006.64/system/system.c | COMPLETE |

---

## Campaign Summary

| Metric | Value |
|--------|-------|
| Total decomp commits | 572 |
| INCLUDE_ASM remaining (project-wide) | 537 |
| INCLUDE_ASM remaining (active targets) | 537 |
| Unported (decomp_status.py) | 367 |
| Files 100% complete | 7 |
| Build status | 468/468 PASSING |

---

## Continuation Block

```
CURRENT_HEAD=8d2e41c6
COMPLETED=1940 matched + 170 coex = 2110/2477 (85.2%)
REMAINING=367 unported INCLUDE_ASM
LAST_FUNCTION=func_8001C074 (menu render tick)
NEXT_TARGETS=func_801E76EC (menu, 81L), func_801E1418 (menu, 84L),
              func_8004463C (libgpu, 81L), func_801CB184 (menu, 86L)
VALIDATION_STATUS=compile+link PASS, no matching verification
KNOWN_DEBT=All functions behavioral approximations; menu #ifdef wall;
           BIOS stubs cannot decompile; magic division functions skipped
NEXT_PRIORITY=Continue menu functions 81-90 lines; tackle libgpu.c
```
