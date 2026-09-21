# Battle-overlay host-leaf coverage push — 24 → 92 adopted leaves (2026-09-18)

Status: **PROVEN for every claim below** (host build + differential prover +
retail byte gate), build-verified only — the battle overlay has no runtime
consumer in the port yet, so nothing here has been observed executing.

## Result

`pc_port/src/battle_overlay_host_leaves.inc`: **24 → 92** adopted leaves.
Every one of them is proven by `pc_port/tests/run_battle_overlay_host_differential_test.sh`
against the retail MIPS in `disc/battle.bin`, in O0, O2 and Clang UBSan, with the
wrong-result mutant still rejected.

| gate | before | after |
|---|---|---|
| adopted leaves | 24 | 92 |
| prover checks (per mode) | 252 | 2247 |
| prover inconclusive | 53 | 386 |
| prover pointer-argument leaves | 8 (hand cases only) | 8 + automated pointer pass |
| `pc_port/build_port.sh` | LINK OK, 51 stubs | LINK OK, 82 stubs, 0 collisions |
| stub gate | 24 leaves reach no stub | 92 leaves reach no stub |
| `battle.bin` vs retail | byte-exact | byte-exact, `1830b4ef1fe37129972fc310dfad534f8161d6c0b123e74254c3711334a3e291` |
| `run_battle_overlay_leaf_bridge_test.sh` | PASS 40 | PASS 40 × O0/O2/UBSan + control |

The stub count rises with every batch because each newly compiled battle host TU
also defines (dormant, unreachable) placeholders for the battle bodies it
references. The gate that matters — an adopted leaf must not *reach* one — stays
green.

Candidate accounting: 68 leaves were added here. 61 come from the 189-entry
generated candidate list; 7 are matched *empty* bodies
(`func_8009795C func_8009F5B0 func_800B3348 func_800B3350 func_800B89F4
func_800BAF40 func_800BDD34`, retail `jr ra`) that
`gen_battle_overlay_guest_ram.py` skips and which had to be allowlisted by hand.
Raw per-batch rejection lists are in `rejected-batches.txt`.

## Harness upgrades (each was required to get past a plateau)

1. **`--pointer-globals` + seeded sweep patterns.**
   `tools/scripts/battle_overlay_host_tus.py --pointer-globals` emits the 17
   guest addresses whose alias is a pointer (parsed from the generated
   `battle_overlay_guest_ram.h`); the runner writes
   `overlay_pointer_globals.inc`; the prover seeds each word with a live RAM
   address in sweep patterns 3-4, and fills a 0x400-byte target window with the
   same pattern so a body reading through the pointer sees pattern data. Before
   this, a body that dereferences `D_800D2D28`-style globals faulted on the
   retail side in every case and was reported UNPROVEN. Patterns 0-2 still run
   unseeded, so null handling is reported *inconclusive* rather than hidden.

2. **SIGSEGV/SIGBUS handler around the host call.**
   The interpreter validates every address through its bus; the host C body does
   not. A body that walks off `g_PsxRam` used to abort the whole run and hide
   every other leaf's result. It is now reported as `host-fault` per case, i.e. a
   divergence (retail completed it), and the sweep continues.

3. **Closure-aware eligibility / stub gate** (landed by the preceding session,
   shipped here together with the rest): `--check-stubs` follows retail `jal`
   edges through undecompiled functions, and the build refuses to link an adopted
   leaf whose call graph reaches a generated stub.

4. **Pointer-argument sweep pass.** Leaves that declare a pointer parameter used
   to be skipped by the sweep entirely and depended on hand-written cases. The
   sweep now gives each pointer parameter `PTRT` (and each scalar parameter a
   small value), seeds the pointer globals, and writes a live nested target at
   the first word of every argument target so `u8**` bodies can run too. The
   eight baseline pointer-parameter leaves keep their hand cases *and* gain these
   cases; this is what made the 103 generator-excluded `u8**` bodies evaluable at
   all (see below).

## Retail-behaviour bugs the prover caught, and the source fixes

The generated guest-RAM alias picks *one* interpretation per `D_*` symbol, but
battle sources use several of them two ways, and host pointers have neither the
32-bit wrap nor the 2 MiB RAM mask of the guest address bus. Six bodies were
wrong (or would not even compile) because of that. Each fix is guarded with
`#ifdef XENO_PC_PORT`, so the matching build is textually unchanged — confirmed
by the byte-exact `battle.bin` above.

| file / function | symptom | fix |
|---|---|---|
| `src/battle/main128.c` `func_800BF720` | `D_800D2D68 != 0` where mainc118/mainc120 declare it an array → host test was pointer-nonnull, always true (retail 0, host 1 in all 18 cases) | read `D_800D2D68[0]` in port mode |
| `src/battle/mainc123.c` `func_800BED30` | `D_800C3610[0] = 0` stored *through* the pointer alias instead of into the word (retail ram 00, host 1e at `800c3612`) | store through `PSX_ADDR(0x800C3610)` |
| `src/battle/mainl81.c` `func_800B3B6C` | same class: null test always true, then dereferenced a host pointer (retail 1, host 0x83) | load the word, branch, `PSX_ADDR(p + 0x41)` |
| `src/battle/mainc108.c` `func_800B9B30` | `D_800C3610[0][0x48]` — the alias is already the pointer, so the second subscript subscripts a `u8`; **TU did not compile in port mode** | drop one index level in port mode |
| `src/battle/main59.c` `func_8009CA90` | port-only coexistence body assigned to the pointer alias (`D_800C3DFC = D_800C3DFC + 0x78`), which is not an lvalue; **TU did not compile in port mode** | advance the cursor word through `PSX_ADDR(0x800C3DFC)` |
| `src/battle/main90.c` `func_800B6930`, `func_800B6990` | `p += <16-bit field of p>`; retail's 32-bit arithmetic wraps and the bus masks to the RAM mirror, a host pointer does neither, so a wild offset field walked off `g_PsxRam` (`host-fault`) | rebase through `PSX_ADDR(PsxMemory_GuestAddr(p) + n)` |

`func_8009CA90` had never been compiled by the port: main59.c had no adopted
leaf, so its "coexistence" port body was dead text. Fixing it is what let
main59.c host-compile and `func_8009CB68` be adopted. `func_800B6930`/`6990`
were baseline leaves whose *hand cases* used well-formed data and therefore
missed the bug; the new pointer-argument pass found it (and the fix keeps both
leaves adopted).

## Evaluated and rejected (each measured, with the reason on record)

| candidate(s) | reason not adopted |
|---|---|
| `func_800800E8`, `func_8008AB4C`, `func_8008AB70`, `func_800BEDE8` | call `HeapFree` / `ArchiveSetIndex`, which the prover can only place as *unresolved callee* placeholders; every case is inconclusive |
| `func_800B7364` | unresolved call target `0x8001cb48` on the retail side |
| `func_8008AA40`, `func_8009E3C8`, `func_800B16F0` | the word is classified as a scalar/array but *holds* a guest address; with RAM zeroed retail dereferences null and faults, so no case completes |
| `func_800AA7DC`, `func_8009A7B8`, `func_800AA79C` | unmasked guest-address arithmetic: host indexes past the guest address, or faults outright |
| `func_800B9258` | real RAM divergence (retail 01, host 00 at `801e0034`) |
| `func_8008887C` | the interpreter resolves `func_8001BD40` through the runtime's registered adapter while the host side calls the prover's placeholder — a divergence the harness cannot adjudicate |
| `func_800B8840`, `func_800BB7F8`, `func_800BCAA4`, `func_800BCAD0`, `func_800BC2F0` | cascade into `func_800BC2F0`, which mainc114.c deliberately compiles out under `XENO_PC_PORT` (the port owns that path in `battle_target_setup.c`); the build's `--check-stubs` gate catches this |
| 103 bodies with two-level pointer dereferences (`*a0`, `(*a0)[i]`) | generator-excluded by `NESTED_RE`; the build gate accepts them, but with the pointer-argument pass every one that was tried host-faults: they dereference guest addresses the port does not translate at the second level. Raw list in `rejected-batches.txt` (batches `nest-*`). This is the measured reason the generator excludes them, not an accident. |

### Host-compile blockers still open (11 candidates)

| TU | error |
|---|---|
| `main32.c` | `D_800D367C = buf;` — the pointer-classified alias is not an lvalue |
| `main35.c`, `main39.c` | multi-declarator externs (`extern u8 D_800D3014, D_800D366C, D_800C3E29;`, `extern Row310 D_800C3EBE[], D_800C3EC0[];`) are mis-parsed by `gen_battle_overlay_guest_ram.py`; it uses the *last* name and a garbage type token, so `D_800D3014` has no alias and three aliases are syntactically invalid |
| `main55.c`, `main70.c`, `mainc115.c` | aliases carry a type token that is only visible in one TU (`BattleSetupShortVector`), so the header does not compile elsewhere |
| `main75.c`, `mainc130.c`, `mainc84.c` | found later, same header/alias family; each blocks the candidates in it |

These are generator defects, not body bugs: (1) one declarator per extern line,
(2) type tokens are emitted globally even when they are TU-local typedefs, (3) a
symbol used both as a word and as a pointer cannot be served by one alias, (4) a
pointer aliased at one level cannot serve a two-level dereference. Filed in
OPEN_ISSUES.md with a reproducer.

## Reproduce

```
# host build + all overlay gates (needs the SDL2 header workaround below)
PKG_CONFIG_PATH=/home/linuxbrew/.linuxbrew/lib/pkgconfig \
CPPFLAGS=-I/home/linuxbrew/.linuxbrew/opt/sdl2-compat/include/SDL2 \
CFLAGS="$CPPFLAGS" CXXFLAGS="$CPPFLAGS" TMPDIR=/var/home/blizz/.cache/xeno-tmp \
  ./pc_port/build_port.sh                     # LINK OK, 82 stubs, 92 leaves reach no stub

TMPDIR=/var/home/blizz/.cache/xeno-tmp \
  ./pc_port/tests/run_battle_overlay_host_differential_test.sh
# -> PASS checks=2247 inconclusive=386 hand-only=8 leaves=92 (O0/O2/UBSan), mutant rejected

TMPDIR=/var/home/blizz/.cache/xeno-tmp \
  ./pc_port/tests/run_battle_overlay_leaf_bridge_test.sh
# -> PASS checks=40 (O0/O2/UBSan + always-interpret control)

podman run --rm --security-opt label=disable --userns=keep-id \
  -v /var/home/blizz/Projects/xenogears-decomp-ai:/home/blizz/Projects/xenogears-decomp \
  -w /home/blizz/Projects/xenogears-decomp -e TMPDIR=/var/tmp \
  localhost/xenogears-dev-toolchain:current \
  bash -lc 'source /.venv/bin/activate && ninja build/out/battle.bin && cmp disc/battle.bin build/out/battle.bin'
# -> BYTE-EXACT
```

**Environment note:** on this host the Homebrew SDL2 prefix is broken
(`include/SDL2/SDL.h` exists but its siblings do not; `SDL_main.h` only lives in
the Cellar). `pc_port/build_port.sh` therefore needs the `-I…/opt/sdl2-compat/include/SDL2`
workaround above, plus the Homebrew `PKG_CONFIG_PATH`. The podman image has SDL2
but no OpenSSL pkg-config, so it cannot run `build_port.sh` as-is. Also: the
adoption loop mutates `battle_overlay_host_leaves.inc` and the shared build
objects, so never run two adoption cycles concurrently — one did here and
produced a spurious "retired override symbol" failure.

## Not claimed

No runtime observation: the PC port still does not execute the battle overlay, so
these bodies have been proven byte-compatible and behaviour-compatible but never
run in-game. `pc_port/tests/battle_overlay_bootstrap_test.sh` is red in this tree
because `config/battle.yaml` lacks `generate_asm_macros_files: False`; that file
is untouched by this work (pre-existing).
