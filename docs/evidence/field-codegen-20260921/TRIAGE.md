# Triage of the field "short by >200 bytes" cohort

Companion to `REPORT.md`, which established that field.bin's 11,420-byte
deficit is codegen rather than layout. That report flagged ~20 functions short
by more than 200 bytes and suggested they were **missing behaviour** rather
than register allocation. This document tests that suggestion.

**Result: the suggestion was wrong. 18 of 18 are innocent. Zero are missing
behaviour.** I was the one who raised the alarm in the previous dispatch; this
retracts it.

## The cohort

18 functions (not "roughly 20"), every field function short by >200 bytes.

## Method

Three structural tests plus source reading, then the differential harness.

**Test A — retail-only call targets.** If retail calls a function the C never
calls, logic is missing. Compared the multiset of `jal` targets in
`asm/field/matchings/<tu>/<func>.s` against `R_MIPS_26` relocations in the
built object.

**Test B — globals retail writes that the C body never names.** If retail
stores to a global the C body does not so much as mention, a side effect is
missing. Compared `%lo(SYM)` operands on retail store instructions against the
text of the corresponding C function body.

**Test C — control-flow volume.** Counted conditional branches, calls, stores
and loads on both sides. Missing behaviour must show up as *fewer branches and
fewer stores* with nothing to account for them.

**Test D — read the source** for every function where A-C left anything
unexplained.

## Headline results of Tests A and B

| test | functions failing |
|---|---|
| A — retail calls something the C never calls | **0 of 18** |
| B — retail writes a global the C body never names | **0 of 18** |

Test A's only near-miss: `func_800748E8` calls `CompMatrix` 8 times in retail
and emits 7 in the build. The C source contains **8** `CompMatrix` call sites
(misc2.c lines 1604, 1673, 1690, 1692, 1694, 1705, 1707, 1709); GCC
cross-jumped two identical call sequences into one. Logic present.

## The real mechanism, and why the "short = missing" heuristic failed

The deficit has three causes, none of which is missing logic. The first was
not in the list of innocent explanations I was asked to exclude, and it is the
single biggest contributor:

**1. Retail re-materializes global base addresses at every single use.** Each
access costs a `lui`+`addiu` pair. The C versions hoist the base into a local
pointer. Measured `%hi`/`%lo` reference counts in retail:

| function | symbol | retail refs | = instructions |
|---|---|---|---|
| func_8007E1C0 | `g_FieldTextBoxes` | 132 | 264 |
| FieldTextBoxInitializePrimitives | `g_FieldTextBoxes` | 78 | 156 |
| func_8007F8DC | `g_FieldTextBoxes` | 70 | 140 |
| FieldDistortionDraw | `g_FieldEffects` | 90 | 180 |
| func_800A3474 | `D_800AFC50` | 56 | 112 |
| func_80076AC0 | `g_FieldActors` | 58 | 116 |

Across the cohort the `lui` count alone falls from 543 (retail) to 313 (built).

**2. Retail inlines small callees that the C calls out to.** The tell is that
the C makes *more* calls than retail in 14 of 18 functions (call delta +1 to
+13). `func_8007E1C0` is the clearest: retail inlines ~16 copies of
`FieldTextBoxLinkPrim`.

**3. Retail duplicates shared tails and unrolled loop bodies.** Confirmed by
reading, see below.

## Per-function verdicts

`lui Δ` is instructions saved by hoisting (= 2 × the lui difference);
`call Δ` positive means the C calls *more* often than retail, i.e. retail inlined.

| # | function | deficit (B) | store Δ | branch Δ | call Δ | lui Δ (insn) | verdict |
|---|---|---|---|---|---|---|---|
| 1 | func_8007E1C0 | -1,192 | -13 | +1 | +3 | 126 | inlining + rematerialization (read) |
| 2 | func_800A84C0 | -1,100 | -16 | -7 | +6 | 118 | **duplication** (read) |
| 3 | func_800A28D4 | -576 | 0 | 0 | +4 | 30 | inlining + rematerialization |
| 4 | func_80076AC0 | -544 | -2 | 0 | +4 | 54 | inlining + rematerialization |
| 5 | func_8007C694 | -460 | -7 | +2 | +13 | 0 | inlining |
| 6 | FieldDistortionDraw | -448 | -7 | +1 | +7 | 52 | inlining + rematerialization |
| 7 | func_8007BEF4 | -444 | -2 | +2 | +12 | 0 | inlining |
| 8 | func_8007F8DC | -408 | -2 | -2 | +6 | 32 | inlining + rematerialization |
| 9 | func_8007CD80 | -400 | -8 | +4 | +10 | -4 | inlining |
| 10 | func_800748E8 | -384 | -2 | +1 | +5 | 12 | inlining (CompMatrix cross-jumped) |
| 11 | FieldTextBoxInitializePrimitives | -368 | -2 | 0 | 0 | 68 | rematerialization |
| 12 | func_8008004C | -272 | -3 | -1 | 0 | 48 | rematerialization |
| 13 | func_80082620 | -260 | -1 | 0 | +1 | 2 | inlining + rematerialization |
| 14 | func_80075B44 | -252 | **+12** | -7 | +4 | 20 | duplication (C stores *more*) |
| 15 | func_8007B1C4 | -228 | -2 | 0 | +4 | 0 | inlining (`NormalClip` ×3) |
| 16 | func_8008B5D4 | -224 | **-24** | +3 | 0 | 0 | **duplication** (read) |
| 17 | func_80077268 | -216 | +1 | -1 | 0 | 20 | rematerialization |
| 18 | func_800A3474 | -208 | -1 | 0 | +7 | 20 | inlining + rematerialization |

No row is MISSING, so the "named absent construct" column the dispatch asked
for is empty by construction. The three functions with the strongest apparent
signal were read in full:

### func_8008B5D4 (store Δ -24 — the strongest signal, and innocent)

Retail carries the unrolled primitive-offset loop body **twice**, selected by
`andi $v0, $v1, 0x8`: once at `.L8008B6C0` with stride 0x20 / 3 vertices, once
at `.L8008B778` with stride 0x28 / 4 vertices. Each copy performs 12 `sb`
stores, so retail does 24 that the C does not.

The C (`main/misc.c:1611`) computes `vertexCount` and `packetStride` from
`header & 8` and runs **one** shared loop. Identical behaviour, one copy.
Confirmed empirically: `run_field_script_vm_primitive_offset.sh` passes
against **retail slice 8008B5D4-8008B894** with its mutants killed.

### func_800A84C0 (-1,100, store Δ -16, branch Δ -7)

Retail duplicates the packet/OT-link tail into **all 8 arms** of the
`i`-dispatch chain — 8 copies × 2 stores = exactly the 16 missing stores, and
the 8 jumps to a shared exit explain the branch delta. The C
(`main/misc5.c`) runs the chain and then links once at the bottom of the loop.

### func_8007E1C0 (-1,192, the worst)

Read in full (`dialogue/text_box_render.c:176-370`). Every retail construct
has a counterpart, including the two the C annotates by retail address: the
`0x40A == 0` continue-arrow arm (retail 8007E4B8-8007E5F8) and the cursor
SPRT + draw-mode block (retail .L8007EA4C-.L8007EC28). The deficit is
264 instructions of `g_FieldTextBoxes` rematerialization plus ~16 inlined
copies of `FieldTextBoxLinkPrim`, plus an 8-iteration loop where retail
unrolled.

## Empirical confirmation

The existing differential harness, O0 / O2 / UBSan, all green, mutants killed:

```
run_field_script_vm_primitive_offset   FIELD_SCRIPT_VM_PRIMITIVE_OFFSET PASS retail_slice=8008B5D4-8008B894
                                       FIELD_SCRIPT_VM_PRIMITIVE_OFFSET MUTANTS PASS
run_field_overlay_render_retail_test   OVERLAY RENDER PASS 32 cases  (x3 regimes)
                                       OVERLAY RENDER negative controls PASS count/blink/angle/OT-tag/counter
run_actor_object_state_retail_test     OBJECT STATE PASS 7680 cases  (x3 regimes)
                                       OBJECT negative controls PASS
run_portrait_render_regression_test    Portrait render regression: PASS
run_field_overlay_init_retail_test     OVERLAY INIT PASS  (x3 regimes) + negative controls
run_field_party_remove_context_test    PARTY REMOVE CONTEXT PASS cases=195 (x3) + negative controls
run_field_script_vm_core_handlers      PASS retail_slices=80099F48-80099FC4,8008E1B4-8008E298 + MUTANTS PASS
```

Two of these cover cohort members directly (`func_8008B5D4` by retail slice,
`func_800A84C0` by the overlay-render suite, which even mutation-tests it).

`run_textbox_timing.sh` could not run: the host is missing
`/usr/lib64/libubsan.so.1.0.0`, so the UBSan link fails. Environmental, not a
code failure — `run_battle_child_billboard_retail_test.sh` documents the same
host limitation.

## The one real defect found: IsLiveHeapBlock

`src/field/main/misc3.c` defined `static int IsLiveHeapBlock()` — a port-only
defensive heap-free-list walk that retail has no equivalent of.

It is a **legitimate port-side helper**, as expected. Both call sites are
already inside `#ifdef XENO_PC_PORT`, and the matching build does not define
that macro, so nothing in the retail-derived path calls it: all 6 references to
the symbol in the built objects are its own internal branches. Nothing serious.

But the *definition* was unguarded, so GCC 2.7.2 emitted the uncalled body into
the matching build, making `main/misc3` the only TU in the whole field overlay
**longer** than retail (+112 B). Fixed by wrapping the definition in the same
`#ifdef XENO_PC_PORT` as its callers. This cannot change port behaviour: when
the macro is defined, function and callers are both present exactly as before.

`main/misc3` text: 7,400 B → 7,268 B against retail's 7,288 (GCC reclaimed 132
including alignment). misc3 is now 20 B short rather than 112 B long — a
smaller and more honest error, and it stops a phantom symbol from masking
misc3's real residual deficit.

## Measured effect on field.bin

| | before | after |
|---|---|---|
| `stat -c%s build/out/field.bin` | 249,442 | **249,310** |
| retail | 260,862 | 260,862 |
| `cmp -l ... \| wc -l` | 219,198 | **218,716** |
| sha256 | `e91869555a1b88d6…` | `8200777e926c7100…` |

Differing bytes improved by 482. The *length* moved 132 further from retail,
which is correct and expected: the removed bytes were dead code retail never
had, so deleting them makes the build more faithful while making the aggregate
length deficit larger. This is exactly the case where the thermometer and the
goal point in opposite directions.

`battle_command_file1.bin` re-verified unchanged at 19,516 B, still identical
to retail.

## Conclusion

No function bodies were changed, because none needed changing. The cohort
that motivated this dispatch is a codegen-style artifact — retail's compiler
re-materializes addresses, inlines aggressively, and duplicates tails; the C
reconstructions hoist, call and share. That is the *same* category as the 90
same-size/different-instruction functions the project has already decided not
to pay for.

**Recommendation: close the "field has missing behaviour" thread.** The
byte-parity work remaining in field is register allocation and codegen style
end to end, and PORT_GOAL_AND_PLAN.md says not to buy it.
