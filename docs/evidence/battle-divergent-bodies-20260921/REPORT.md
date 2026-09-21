# Six divergent battle bodies: five fixed and adopted, one refused

Target: the divergent-body list accumulated over the previous dispatches — six
existing C bodies the differential prover found genuinely wrong against the
retail bytes in `disc/battle.bin`.

**Result: 5 fixed and proven, 1 refused. Adopted leaves 103 → 108.**

Every fix is proven by the same prover that rejected the body: it runs the
function twice on identical guest RAM, once as retail MIPS bytes and once as
the linked host C, and compares the result and every byte of RAM that changed.

## Per-function verdicts

| function | TU | verdict | root cause |
|---|---|---|---|
| `func_800AA7DC` | main71.c | **FIXED** | unbounded walk ran off the end of host RAM |
| `func_800885D0` | main39.c | **FIXED** | 64-bit pointer truncated to `u32`; double-scaled index |
| `func_800AA79C` | main71.c | **FIXED** | unbounded index + guest address used as host pointer |
| `func_800AA760` | main70.c | **FIXED** | same pair as `func_800AA79C` |
| `func_800BEDE8` | mainc124.c | **FIXED** | pointer-typed alias: read/wrote the pointee, not the word |
| `func_8008887C` | main40.c | **REFUSED** | divergence is deterministic but **unexplained** |

The five fixes are four distinct root causes, all of them 32-bit-PSX
assumptions that are invisible until a body actually executes on a 64-bit
host. None was a logic error in the decompiled algorithm — every one of the
five had the retail control flow right.

### 1. Guest addresses dereferenced as host pointers

`func_800AA79C`, `func_800AA760`. `D_800D3368` is a table of **guest**
addresses. Retail dereferences a slot directly because a guest address is a
real pointer on the PSX; on the host `(u8*)D_800D3368[i]` is wild.

```c
*(u8*)PSX_ADDR(*pa + 0x34) = 0;     /* was: *(u8*)(D_800D3368[arg0] + 0x34) */
```

### 2. Unbounded indices running off the end of host RAM

Same two functions, plus `func_800AA7DC`. Retail's `lw`/`lbu` wrap inside the
2 MB of RAM and the interpreter masks every access the same way, but
`D_800D3368[arg0]` and `D_800C402F[off]` on the host index straight past the
buffer. `func_800AA7DC`'s loop is explicitly unbounded — it walks in steps of
0x48 until it finds a byte that is not 0xF7.

Addressing each access through `PSX_ADDR` restores the hardware's wrap:

```c
u32* pa = (u32*)PSX_ADDR(0x800D3368 + arg0 * 4);
v = *(u8*)PSX_ADDR(0x800C402F + off);
```

This is the fix that mattered most: my first attempt translated only the
*dereference* and the prover still rejected both functions, because the
**index** was the wild part. Worth recording — the obvious half of this bug
hides the other half.

### 3. A 64-bit host pointer truncated to `u32`

`func_800885D0` had two bugs in three lines:

```c
u32 t = (u32)D_800C3EB4[index * 28] + 0x18;
return *(u8*)((u32)D_800D301C + (t << 2));
```

`(u32)D_800D301C` truncates a 64-bit host pointer to 32 bits and then uses it
as an address — a plain 32-bit-host assumption. And `D_800C3EB4` is declared
`u8 D_800C3EB4[][28]` in main35.c, so the shared guest-RAM alias is
`u8 (*)[28]`, which made `D_800C3EB4[index * 28]` scale by 28 **twice**.

Retail is unambiguous: `lbu` at `D_800C3EB4 + 28*index`, then `lbu` at
`D_800D301C + (t<<2)` — byte loads relative to each symbol's address. Indexing
both through a plain byte pointer is correct in both builds, so this one
needed no `#ifdef`.

Note this second bug was *introduced into visibility* by my own previous
generator fix, which corrected `D_800C3EB4` from a malformed macro to
`u8 (*)[28]`. The prover caught it immediately, which is the system working.

### 4. A pointer-typed alias where the TU meant the word

`func_800BEDE8`. mainc124.c declares `extern u32 D_800C3610[]`, but another TU
declares it `u8*`, so the single shared alias is pointer-typed:

```c
#define D_800C3610 ((u8 *)PSX_ADDR(*(u32 *)PSX_ADDR(0x800C3610)))
```

So `D_800C3610[0]` read the **byte at the address held in the word**, and
`D_800C3610[0] = 0` wrote that byte — while retail does `lw`/`sw` at
`0x800C3610` itself. That is exactly why the prover saw guest `0x800C3610..13`
left unzeroed (`first at guest 800c3612 retail=00 host=1e`).

Fixed by going through the word directly, the idiom main59.c already uses for
`D_800C3DFC`:

```c
HeapFree(*(u32*)PSX_ADDR(0x800C3610));
*(u32*)PSX_ADDR(0x800C3610) = 0;
```

This is the same per-TU-conflicting-type hazard documented in
`docs/evidence/battle-guest-ram-generator-20260921/` — here it produced a
silently wrong body rather than a compile error.

## The refusal: func_8008887C

`ram 1 bytes differ, first at guest 800c3a9c retail=3e host=5c`, one failing
case in thirty.

I could not explain it, so I did not fix it. What I established:

- **It is deterministic.** Identical across three consecutive prover runs, so
  it is not jitter. (My first hypothesis was RNG nondeterminism, on the basis
  that the host byte had been `32` in the previous dispatch and `5c` now —
  that was wrong: the change came from my own generator fix between the two
  builds, and the three-run test disproved it.)
- **Neither value can have come from the call that writes that word.** The
  only writer of `D_800C3A9C` in this function is the result of
  `func_8001BD40`, which is the `rand()`-based pick in `[min,max]` defined at
  `src/slus_006.64/system/temp3.c:969`, called here with `(1, 8)`. Neither
  `0x3e` (62) nor `0x5c` (92) is in `[1,8]`. So in the failing case **neither
  side stored a result from that call**, and something outside this function's
  disassembly is writing that byte.
- Arguments and control flow check out against retail: `a0..a3` map to
  `x0,y0,x1,y1`; retail overwrites `a0`/`a1` with dx/dy but leaves `a2`/`a3`
  holding the original `x1,y1`; the `slt`/`div` branches match the C; and the
  store widths match the alias widths exactly (`sb` for `D_800C3A94`,
  `D_800C3A98`, `D_800C207C`; `sw` for the rest).

**A real defect was found and fixed here in passing, and it is not the cause.**
main40.c declared `extern u32 func_8001BD40(u32,u32,u32,u32)` for a function
actually defined as `u8 func_8001BD40(u8,u8)` — calling through a mismatched
prototype is undefined behaviour. Corrected to the true two-argument form,
which is also the faithful reading of retail (it sets only `$a0=1`, `$a1=8`;
`$a2`/`$a3` are this function's own leftovers that the two-parameter callee
ignores). The observed bytes did not change, confirming it is not the cause.

`func_8008887C` stays on the do-not-adopt list with that reason, now sharpened
from "1 byte differs" to "the byte cannot have come from the only call that
writes it — find the real writer first".

## Adoption: 103 → 108

All five fixed functions already satisfied the standing criteria (eligible,
owning TU compiles in host-bodies mode, at least one still-interpreted `jal`
caller), so all five were adopted once proven.

| metric | before | after |
|---|---|---|
| adopted leaves | 103 | **108** |
| differential checks | 2521 | **2643** |
| generated function stubs | 77 | 77 |

## Gates

```
# 108 adopted leaves reach no generated stub (77 stubs checked)
    LINK OK -> pc_port/build_native/xeno-port (port-owned addresses verified)
    overlay host allowlist verified against the linked binary

BATTLE OVERLAY DIFFERENTIAL PASS checks=2643 inconclusive=470 hand-only=8 leaves=108
BATTLE OVERLAY DIFFERENTIAL bridge-placeholders=26
BATTLE OVERLAY DIFFERENTIAL FAIL func_80079934 ...   <- the wrong-result control, correctly rejected
BATTLE OVERLAY DIFFERENTIAL PASS (O0/O2/UBSan + wrong-result control rejected)

BATTLE OVERLAY LEAF PASS checks=40   (x3: O0/O2/UBSan)
BATTLE OVERLAY LEAF PASS (O0/O2/UBSan + always-interpret control)
```

### Byte parity — proved, not assumed

This dispatch edits five `src/battle` files, so parity had to be re-proved:

```
battle.bin built=343936 retail=343936
battle.bin IDENTICAL to retail
124703d415f570f9cd8d7dad89c6a4eddfd7e6f0  build/out/battle.bin
124703d415f570f9cd8d7dad89c6a4eddfd7e6f0  disc/battle.bin
battle_command_file1 IDENTICAL
field.bin 249310 diffbytes=218716   (unchanged)
```

Splat tracebacks in the matching build: **0**. Every port-only change is
inside `#ifdef XENO_PC_PORT` with the retail-shaped body retained in the
`#else`, following the mainl81.c / main59.c precedent; the one change that is
correct in both builds (`func_800885D0`) needed no guard.

## Suites that could NOT run on this host — not passes

- `run_battle_guest_call_test.sh` — **NOT RUN**. Wants
  `asm/battle_command_file1/matchings/message_controller/xeno_battle_command_file1_controller.s`;
  `config/battle_command_file1.yaml` uses whole-TU `asm` subsegments and emits
  no per-function `matchings/` tree. Config is sibling-owned.
- `run_textbox_timing.sh` — **NOT RUN**. Host lacks
  `/usr/lib64/libubsan.so.1.0.0`, so the UBSan link fails.

## Left alone by instruction

`run_battle_graphics_abi_test.sh` was not touched. Its entry in
`OPEN_ISSUES.md` now also records that, because `set -e` kills the script at
the O0 regime, **that suite's UBSan regime has never executed on this host** —
routed to the harness-integrity worker along with the leaf-bridge and
native-callback-map gates. No `run_*.sh` was modified in this dispatch.
