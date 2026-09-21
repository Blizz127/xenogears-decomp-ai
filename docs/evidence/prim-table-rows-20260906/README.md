# D_8004FE50 model-prim dispatch table — absent rows filled from retail .sdata

Date: 2026-09-06 UTC. Scope: `pc_port/src/game_overrides.c` `D_8004FE50[17]` only.
No other file touched. No commit/stage/push.

## Problem class (previously diagnosed, see grind log 2026-09-02)

`temp2.c` skips a mesh group when `D_8004FE50[prim].proc[variant]` is NULL, but
the BUILD side (`temp2.c:749`, `buildProc`) has already linked that group's
packets into `ot3` with their tag lengths set. The walker then parses
uninitialized packets, reports "ptag length is not valid", and runs off the
chain. That is what produced the Map 2 (Lahan opening) SEGV / heap abort, fixed
for row 0x04 by commit `8349424a`.

A row that is **absent from the designated initializer entirely** is a strictly
worse instance of the same class: it is zero-filled, so `buildProc` is NULL and
all three strides are 0, not merely some `proc` slots.

## Retail table, read from the disc binary (evidence class: proven)

Read directly out of `disc/SLUS_006.64` `.sdata` at vaddr `0x8004FE50`, using the
project's established mapping `file offset = 0x800 + vaddr - 0x80010000`
(= `0x40650`). Row stride `0x28` = 6 `proc` pointers + `buildProc` + 3 strides.
Reproducer:

```bash
python3 - <<'EOF'
import struct
data = open('disc/SLUS_006.64','rb').read()
off = lambda v: 0x800 + v - 0x80010000
for row in range(17):
    va = 0x8004FE50 + row*0x28
    v = struct.unpack('<10I', data[off(va):off(va)+40])
    print("row 0x%02X @%08X proc=%s build=%08X strides=0x%X,0x%X,0x%X"
          % (row, va, " ".join("%08X"%p for p in v[0:6]), v[6], *v[7:10]))
EOF
```

Output (this is the authoritative table; row 0x04 reproduces the values already
recorded in the 2026-09-02 grind-log entry, which cross-validates the offset
math and the row layout):

```
row 0x00 @8004FE50  proc=8002E038 8002ED20 8002E470 8002E8DC 8002E038 8002E038  build=8002CDCC  strides=0x8,0x4,0x14
row 0x01 @8004FE78  proc=8002E04C 8002F2E0 8002E484 8002E8F0 8002EF0C 8002F0E4  build=8002D814  strides=0x8,0x8,0x20
row 0x02 @8004FEA0  proc=8002E024 8002F6B4 8002E45C 8002E8C8 8002E024 8002E024  build=8002D6AC  strides=0x8,0x4,0x1C
row 0x03 @8004FEC8  proc=8002E010 8002F4B4 8002E448 8002E8B4 8002E010 8002E010  build=8002DA14  strides=0x8,0x8,0x28
row 0x04 @8004FEF0  proc=8002E038 8002E038 8002E470 8002E8DC 8002E038 8002E038  build=8002CF34  strides=0x8,0x4,0x14
row 0x05 @8004FF18  proc=8002E04C 8002E04C 8002E484 8002E8F0 8002EF0C 8002F0E4  build=8002D984  strides=0x8,0x8,0x20
row 0x06 @8004FF40  proc=8002E024 8002E024 8002E45C 8002E8C8 8002E024 8002E024  build=8002D77C  strides=0x8,0x4,0x1C
row 0x07 @8004FF68  proc=8002E010 8002E010 8002E448 8002E8B4 8002E010 8002E010  build=8002DA14  strides=0x8,0x8,0x28
row 0x08 @8004FF90  proc=8002E254 8002F8D0 8002E674 8002EAE0 8002E254 8002E254  build=8002CF58  strides=0x8,0x4,0x18
row 0x09 @8004FFB8  proc=8002E268 8002FAE8 8002E688 8002EAF4 8002FCFC 8002FF0C  build=8002D530  strides=0x8,0xC,0x28
row 0x0A @8004FFE0  proc=8002E240 8002E240 8002E660 8002EACC 8002E240 8002E240  build=8002D180  strides=0x8,0x4,0x24
row 0x0B @80050008  proc=8002E22C 8002E22C 8002E64C 8002EAB8 8002E22C 8002E22C  build=8002D244  strides=0x8,0xC,0x34
row 0x0C @80050030  proc=8002E254 8002E254 8002E674 8002EAE0 8002E254 8002E254  build=8002D0C0  strides=0x8,0x4,0x18
row 0x0D @80050058  proc=8002E268 8002E268 8002E688 8002EAF4 8002FCFC 8002FF0C  build=8002D0E4  strides=0x8,0xC,0x28
row 0x0E @80050080  proc=8002E240 8002E240 8002E660 8002EACC 8002E240 8002E240  build=8002D180  strides=0x8,0x4,0x24
row 0x0F @800500A8  proc=8002E22C 8002E22C 8002E64C 8002EAB8 8002E22C 8002E22C  build=8002D244  strides=0x8,0xC,0x34
row 0x10 @800500D0  proc=80030750 x6                                            build=8002DAFC  strides=0x8,0x4,0x20
```

## Gap found (evidence class: proven)

Before this change the port initializer defined rows
`0x00,0x01,0x03,0x04,0x05,0x08,0x09,0x0A,0x0C,0x0D` — **seven of seventeen rows
(0x02, 0x06, 0x07, 0x0B, 0x0E, 0x0F, 0x10) were absent and therefore zero-filled.**

## Change made (evidence class: proven for the two rows filled)

Only the two rows whose retail contents are *exactly* covered by walkers already
ported were filled. Both are provable with no new walker code:

- **Row 0x0E** is byte-for-byte identical to retail row 0x0A (same six `proc`
  entries, same `buildProc` 0x8002D180, same strides 0x8/0x4/0x24). Row 0x0A is
  already ported, so 0x0E is filled identically: `ModelPrimQuadG4Variant0` for
  variants 0/1/4/5; `proc[2]`=0x8002E660 and `proc[3]`=0x8002EACC unported → NULL.
- **Row 0x07** differs from retail row 0x03 only at `proc[1]`: row 3 uses the
  separate lit GT3 path 0x8002F4B4, row 7 reuses the shared body 0x8002E010.
  `buildProc` (0x8002DA14) and all three strides (0x8/0x8/0x28) are identical.
  Filled with `ModelPrimTriGT3Variant0` for variants 0/1/4/5;
  `proc[2]`=0x8002E448 and `proc[3]`=0x8002E8B4 unported → NULL, exactly as row 0x03.

## Regression certificate (evidence class: proven)

`pc_port/tests/run_prim_table_retail_test.sh` +
`pc_port/tests/prim_table_retail_prod_test.c`. Links the real production
`pc_port/src/game_overrides.c` and compares its live `D_8004FE50` against the
retail table re-read from `disc/SLUS_006.64` at run time.

```
PRIM TABLE CASES 7 ASSERTIONS 112 FAILURES 0   -> PASS
PRIM TABLE CONTROLS REJECTED 4/4
  drop_row_07     correctly rejected (1 assertion failure)
  drop_row_0E     correctly rejected (5 assertion failures)
  bad_stride_07   correctly rejected (1 assertion failure)
  null_proc0_0E   correctly rejected (2 assertion failures)
```

What it pins: the parse itself (row 0x04 against the independently recorded
2026-09-02 values); the exact populated-row set, so filling or dropping a row is
always an explicit reviewable diff; all three strides against retail for every
populated row; `proc[0]` non-NULL; and the two structural identities this pass
relied on — that retail row 0x0E is byte-identical to row 0x0A, and that retail
row 0x07 differs from row 0x03 in exactly one slot (`proc[1]`, which equals its
own `proc[0]`). If either identity ever stops holding, the rows filled here stop
being justified and the test says so.

Two honest caveats about the harness:

- The `buildProc` slots cannot be checked at run time. They are not-yet-ported
  externs (`func_8002CDCC` and friends) and the test links with
  `--unresolved-symbols=ignore-all`, so every `buildProc` reads back as 0
  regardless of source. That check is therefore a **source-level** pin in the
  runner (every populated row must declare `.buildProc` and all three strides).
  An earlier draft of this test asserted it at run time and produced 12 false
  failures.
- Controls must fail at **run time**, not by failing to compile. Every mutation
  is still valid C, so a build failure means the environment broke. The runner
  treats a control build failure as a hard error for exactly this reason: on the
  first run all four controls "passed" only because `/tmp` was over quota, which
  would have made them blind. Reproduced green by redirecting both `TMPDIR` and
  `PRIM_TABLE_OUT` off the quota-limited tmpfs.

## Verification performed

- **build-verified only:** `gcc -fsyntax-only` over `pc_port/src/game_overrides.c`
  with the port's own flag set from `pc_port/build_port.sh` (`-std=gnu17
  -fpermissive -DXENO_PC_PORT -DXENO_FIELD_OBJECT_OVERLAY -DSKIP_ASM
  -D_LANGUAGE_C -DUSE_EXTENDED_PRIM_POINTERS=0 -include assert.h -w -m64
  -fno-builtin` plus the five include dirs) exits 0. Without `-fpermissive` the
  only diagnostic is a pre-existing implicit declaration of `Vsync` at
  `game_overrides.c:2430`, unrelated to this change and caused by the generated
  `pc_port/build_native` prototype header being absent in this checkout.
- **NOT_RUN / NOT_OBSERVED:** no native link, no runtime, no map render. The
  `xenogears-dev` container that `pc_port/build_port.sh` expects does not exist
  in this environment (`distrobox list` is empty, no docker), so neither of these
  rows has been observed dispatching a real mesh group. Filling them is argued
  from the retail table plus the identity with already-ported rows, **not** from
  an observed render.
- No claim is made that any specific map is fixed by this change. Both rows were
  latent — no map in the current survey was proven to reach them.

## Remaining (not addressed here)

### Second pass: rows 0x02 and 0x06 filled (evidence class: proven)

The inference below about 0x8002E024 is now **confirmed from retail assembly**.
Regenerating the assembly tree (`make check` runs `gears matching`; `asm/` is
gitignored and generated, which is why `asm/slus_006.64/` was absent earlier —
it was never missing data) shows `func_8002E010` is a **hand-written** function
with four alternate entry points, each loading three constants and jumping to
one shared body at `.L8002E058`
(`asm/slus_006.64/nonmatchings/system/temp2/func_8002E010.s`):

| entry | `$t9` | `$t8` (tag) | `$a3` (packet) | table rows |
|---|---|---|---|---|
| `8002E010` | 0xC | 0x09000000 | 0x28 | 0x03, 0x07 |
| `8002E024` | 0x8 | 0x06000000 | **0x1C** | 0x02, 0x06 |
| `8002E038` | 0x4 | 0x04000000 | 0x14 | 0x00, 0x04 |
| `8002E04C` | 0x8 | 0x07000000 | 0x20 | 0x01, 0x05 |

`$a3` is exactly the row's `outputStride` and `$t8` the packet tag, and both
agree with the retail table dumped above for **every** row — an independent
cross-check of the table parse. `$t9` is the per-vertex stride inside the
packet, which is why xy store offsets differ per entry.

Because `0x8002E024` shares `t9=0x8` with `0x8002E04C` it shares that body
exactly, differing ONLY in packet size and tag. The port's
`ModelPrimTriAverageVariant0` already encoded E04C's constants (`0x20` /
`0x07000000`) verbatim, so the body was parameterised into
`ModelPrimTriAverageShared(pCmd, count, packetStep, tagLen)` — mirroring
retail's own one-body/four-entries structure — with two thin wrappers:
`ModelPrimTriAverageVariant0` (0x20 / 0x07000000) and the new
`ModelPrimTriMediumAverageVariant0` (0x1C / 0x06000000). The `t9=0xC` and
`t9=0x4` entries have different xy layouts and keep their own bodies, untouched.

Rows 0x02 and 0x06 now match retail: the shared entry on variants 0/1/4/5 (row
0x02's `proc[1]` is the separate lit path 0x8002F6B4 and stays NULL, exactly as
rows 0x00/0x03 treat theirs), `proc[2]`/`proc[3]` (0x8002E45C / 0x8002E8C8)
unported → NULL, and buildProcs `func_8002D6AC` / `func_8002D77C` which were
already real C in `temp2.c`.

Verification: prim-table test **7 cases / 122 assertions / 0 failures**, 4/4
controls still rejected, populated set pinned at 14 rows. Native
`build_port.sh` **LINK OK** (function stubs 76 → 74). Container smoke of maps
16, 17 and 1: all `RC=124` (clean timeout), **zero** `missing D_8004FE50`
reports, zero `ptag length`/assert/SIGSEGV, model builds unchanged at 67/109,
90/132, 96/143 — no regression from the refactor.

## Remaining (not addressed here)

Rows **0x0B, 0x0F and 0x10** are still absent/zero-filled:

| row(s)      | missing walker | notes |
|-------------|----------------|-------|
| 0x0B, 0x0F  | `0x8002E22C` (+ `0x8002E64C` variant 2) | `0x8002E22C` is likewise an alternate entry inside `func_8002E010.s` (line 143, `ori $t9, $zero, 0xC`) — the quad-family counterpart, so the same treatment as E024 should apply. buildProc `func_8002D244` already real C in `temp2.c` |
| 0x10        | `0x80030750` | a genuine standalone function, `asm/slus_006.64/nonmatchings/system/temp2/func_80030750.s`; INCLUDE_ASM-only (`temp2.c:2064`) → auto-stubbed in the port |
