# Xenogears Decompiation Progress Dashboard

_Authoritative: backed by `tools/scripts/decomp_status.py --json` and `objdiff report`._
_Last updated: 2026-08-18_

---

## Project Status

| Metric | Value |
|--------|-------|
| **Branch** | `integrate/w34b24-i1` |
| **HEAD** | `6979c7d` (Decompile func_800925A0 — field script actor motion speed set) |
| **Build** | 468/468 targets — PASSING |
| **Matching verification (objdiff)** | 1689/2292 funcs matched (73.7%), 60.4% byte-match |
| **Decomp_status universe** | 2477 functions, 1954 done (78.9%) |
| **Decomp commits (this campaign)** | 416 |
| **First campaign commit** | `c55830a` (Decompile ArchiveDataSync) |

---

## Universe Summary (decomp_status.py)

**Universe = 2477 functions** (asm `.s` union across 4 overlays: slus_006.64, field, member_change_menu, shop_menu)

| Category | Count | % |
|----------|------:|--:|
| **MATCHED {}** (C body, no INCLUDE_ASM, byte-exact inferred) | 1783 | 72.0% |
| **COEXISTENCE** (INCLUDE_ASM + port body, byte-exact via coexistence) | 171 | 6.9% |
| **UNPORTED** (unconditional INCLUDE_ASM) | 523 | 21.1% |
| **Done-for-matching** (MATCHED + COEX) | **1954** | **78.9%** |
| **Stubs in port** | 238 | — |

### Objdiff verification (authoritative matching)

| Metric | Value |
|--------|-------|
| **Total functions** | 2292 |
| **Matched (100%)** | 1689 (73.7%) |
| **Unmatched** | 603 (26.3%) |
| **Code bytes matched** | 331,576 / 549,068 (60.4%) |

### Per-overlay (objdiff)

| Overlay | Total | Matched | % |
|---------|------:|--------:|--:|
| Psy-Q SDK | 367 | 337 | 91.8% |
| Main Executable | 1223 | 907 | 74.2% |
| Field Overlay | 884 | 599 | 67.8% |
| Member Change Menu | 67 | 65 | 97.0% |
| Shop Menu | 118 | 118 | 100.0% |

---

## Per-Overlay Progress (decomp_status.py)

| Overlay | Total | Matched | Coex | Unported | %done |
|---------|------:|--------:|-----:|---------:|------:|
| field | 878 | 811 | 6 | 61 | 93.1% |
| member_change_menu | 67 | 66 | 0 | 1 | 98.5% |
| menu | 312 | 23 | 104 | 185 | 40.7% |
| shop_menu | 118 | 100 | 0 | 18 | 84.7% |
| slus_006.64 | 1102 | 783 | 61 | 258 | 76.6% |
| **TOTAL** | **2477** | **1783** | **171** | **523** | **78.9%** |

---

## Subsystem Progress

### Field (878 functions, 93.1% done)
| Module | Total | Done | Remaining |
|--------|------:|-----:|----------:|
| field/main/misc | 198 | 196 | 2 |
| field/main/misc2 | 44 | 43 | 1 |
| field/main/misc4 | 42 | 42 | 0 |
| field/main/misc5 | 36 | 29 | 7 |
| field/main/misc6 | 63 | 58 | 5 |
| field/main/misc7 | 91 | 73 | 18 |
| field/main/misc8 | — | — | 0 (complete) |
| field/main/misc9 | 27 | 21 | 6 |
| field/main/misc10 | — | — | 0 (complete) |
| field/main/misc11 | 102 | 84 | 18 |
| field/main/main | 14 | 13 | 1 |
| field/camera | 30 | 29 | 1 |
| field/party/stats | 16 | 15 | 1 |
| field/scripts/vm | 29 | 28 | 1 |

### Kernel/System (slus_006.64, 1102 functions, 76.6% done)
| Module | Total | Done | Remaining |
|--------|------:|-----:|----------:|
| system/sound | 271 | 240 | 31 |
| system/temp1 | 71 | 51 | 20 |
| system/temp2 | 102 | 76 | 26 |
| system/temp3 | 27 | 25 | 2 |
| system/rendering | 20 | 15 | 5 |
| system/work_list | 29 | 29 | 0 |
| system/libarchive | 27 | 24 | 3 |
| system/system | 54 | 54 | 0 |
| system/animation_scripts | 34 | 26 | 8 |
| system/menu | 8 | 6 | 2 |
| psyq/libgte | 96 | 11 | 85 |
| psyq/libgpu | 66 | 44 | 22 |
| psyq/libapi | 25 | 7 | 18 |
| psyq/libapi_2 | 8 | 1 | 7 |
| psyq/libcard | 7 | 1 | 6 |
| psyq/libc | 12 | 4 | 8 |
| psyq/libsn | 8 | 0 | 8 |

### Menu (312 functions, 40.7% done)
| Module | Total | Done | Remaining |
|--------|------:|-----:|----------:|
| menu/main/misc | 312 | 127 | 185 |

### Shop Menu (118 functions, 84.7% done)
| Module | Total | Done | Remaining |
|--------|------:|-----:|----------:|
| shop_menu/main/misc | 118 | 100 | 18 |

---

## Files Now Complete (0 unported INCLUDE_ASM)

| File | Functions |
|------|----------:|
| `src/field/main/misc3.c` | — |
| `src/field/main/misc8.c` | — |
| `src/field/main/misc10.c` | — |
| `src/field/main/misc4.c` | 42 |
| `src/slus_006.64/system/work_list.c` | 29 |
| `src/slus_006.64/system/archive.c` | 11 |
| `src/slus_006.64/system/system.c` | 54 |

---

## Validation Quality

| Check | Status |
|-------|--------|
| Compile (468/468) | **PASS** |
| Link (468/468) | **PASS** |
| rom-check (field.bin) | **FAIL** (known red — nonmatching decompilations) |
| objdiff function match | 73.7% (1689/2292) |
| objdiff byte match | 60.4% |

---

## Continuation Block

```
CURRENT_HEAD=6979c7d
COMPLETED=1954/2477 (78.9%) [decomp_status] or 1689/2292 (73.7%) [objdiff]
REMAINING=523 unported INCLUDE_ASM + 603 decompiled nonmatching
LAST_FUNCTION=func_800925A0 (field script actor motion speed set)
VALIDATION_STATUS=compile+link PASS, objdiff 73.7% func match
NEXT_PRIORITY=misc11.c remaining 18 INCLUDE_ASM, then misc7.c, misc5.c
```
