# W34B22-I4B — private world collision-probe family

## Outcome

Implemented and certified all four retail-private helpers as one internal
axis/direction core with four explicit exported wrappers:

- `wm_8009443C`: X positive
- `wm_800945C8`: X negative
- `wm_80094750`: Z positive
- `wm_800948D8`: Z negative

`0x80094A5C` was not implemented or modified.

## Canonical and retail authority

```
canonical_base=112c2a2371c20fd2214a59b2eac7a2d08ec9503f
canonical_subject=Implement world helper 0x8008C040
world_map.bin_size=180422
world_map.bin_sha256=4c15fd32b3a03d7cd5ea4403dcaabc70abaf99b6aaca65d63d6866803edaac70
```

| Helper | Boundary | Bytes/insns | SHA-256 |
|---|---|---:|---|
| `9443C` | `[0x8009443C,0x800945C8)` | 396/99 | `8dc70738291fd311ea69e6b2b0879455514ad30287649a130e90a52be4df8ad8` |
| `945C8` | `[0x800945C8,0x80094750)` | 392/98 | `d7fba57ff23eb8a75f0b912ac7e3fd9faf581262dc74e56f0ca0bff9a82d448c` |
| `94750` | `[0x80094750,0x800948D8)` | 392/98 | `9111d4ca10fc4affddcc75f5e3bfe8d18d27534c365fea1713fd1972b93ec015` |
| `948D8` | `[0x800948D8,0x80094A5C)` | 388/97 | `f69aae62b8d5c8aba328f9871313d45e41528fd73d351f650e8dd71c2d8ef7ad` |

The hashes were independently recomputed before implementation. A raw retail
JAL-word census independently reproduced 20 family calls, split 7/3/5/5.

## Implementation contract

`world_map_private_collision.c` uses a private `WmProbeAxis` and
`WmProbeDirection` core. The four retail symbols are real wrapper functions,
not aliases.

The core preserves:

- 32-bit wrapping left shift before signed division;
- explicit divisor-zero and `INT_MIN/-1` traps without C division UB;
- signed multiply, low-32-bit product truncation, and explicit arithmetic
  shift by 12;
- the `0xFFF80000` mask and direction-specific edge source/`-1` placement;
- all 32-bit workspace loads/stores, including failure-path edge residue at
  workspace `+0` (X) or `+8` (Z);
- exact canonical call order `93354,93F18,94060` for each candidate;
- signed-low-halfword mode/code arguments and low-halfword interpretation of
  the `94060` result;
- candidate-0 four-word copy and short circuit;
- returns `{0,1,3}` for X and `{0,1,2}` for Z.

No direct global/table access, hardcoded scratchpad address, clamp, defensive
bounds repair, GTE operation, or `0x80094A5C` behavior was added.

## Focused family certificate

One shared 2x2 oracle exercises all wrappers and all three logical outcomes.
It verifies candidate vectors, edge residue, four-word copy, call order/count,
candidate pointers, signed arguments, low-halfword results, grid boundaries,
negative ratios, multiply wrap beyond 2^32, and both division traps.

| Gate | Result |
|---|---|
| O0 | PASS, 20 cases / 4 wrappers |
| O2 | PASS |
| UBSan | PASS, stderr empty |
| normalized comparison | O0/O2/UBSan identical |
| mutants | 21/21 independent fault classes killed |
| scratchpad canonical-chain integration | PASS |

The production-chain integration links the actual canonical `93354`, `93E8C`,
`93F18`, and `94060`. With `a2=0x1F800000`, current `PSX_ADDR` maps the
workspace to `g_PsxRam+0`, candidate 0 to `+0x20`, and candidate 1 to `+0x30`.
All four wrappers call through the canonical chain successfully; `93F18` trace
events observe candidate X/Z loads at `0x1F800020/28`. The separate
`g_PsxScratchpad` buffer remains untouched. This proves internal consistency
with the accepted dependency model and introduces no helper-local address hack.

## Noninterference / PACK

All 12 existing current-canonical world-helper runners pass:

`73B04`, `90A84`, `85760`, `74794`, `894C8`, `93534`, `7528C`, `93354`,
`93E8C`, `93F18`, `94060`, and `8C040`.

PACK and natural noninterference checks embedded in those runners pass. There
is no separate standalone PACK runner on this canonical.

## Link and ownership

Both isolated links pass:

| Build | Game TUs | Port TUs | Skipped | Function stubs | Data stubs | Result |
|---|---:|---:|---:|---:|---:|---|
| canonical baseline | 47 | 58 | 0 | 240 | 524 | PASS |
| candidate clean | 47 | 59 | 0 | 240 | 524 | LINK OK |
| candidate incremental | 47 | 59 | 0 | 240 | 524 | LINK OK |

The one-port-TU increase is exactly `world_map_private_collision.c`. `nm`
reports exactly one strong `T` for every family wrapper and for canonical
`wm_80093354`, `wm_80093E8C`, `wm_80093F18`, and `wm_80094060`.
Generated `stubs.c` contains none of those eight symbols.

## Warning

Current canonical `PSX_ADDR` aliases `0x1F800000` into the emulated main-RAM
backing rather than `g_PsxScratchpad`. This is pre-existing accepted behavior
shared by `93354`, `93E8C`, and `93F18`; the new family is proven consistent
with it. A future `94A5C` implementation must use the same coherent model or
perform a separate, whole-chain scratchpad migration. This rung deliberately
does neither.
