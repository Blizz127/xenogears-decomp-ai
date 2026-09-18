# Field script VM and native field audit — 2026-09-04

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `3a3e7aac03a2f166fb924945a489e392d706f282`
- Audit scope: the current dirty working tree, preserved in place
- Verdict: **DISPATCH TABLE EXACT; RETAIL PARITY NOT YET PROVEN; FIELD RUNTIME INCOMPLETE**

This audit does not turn “the opening currently plays” into a retail-parity
claim. It inventories every implemented field-script dispatch slot, compares
the matching MIPS output with retail, and identifies the native field paths
that can still enter generated no-op stubs or executable `assert(0)` branches.
It also detects checked-in functions that call `xeno_port_stub`, so an authored
placeholder cannot disappear behind the generated-stub census. The unresolved
rows remain red by design.

## Authority and input pins

| input | size | SHA-256 |
|---|---:|---|
| `disc/field.bin` | 260862 | `38a1ce829a6f094c505f67143d6ace2d328418c65425a7383991179467e1fdfc` |
| `disc/SLUS_006.64` | 303104 | `dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119` |
| `disc/disc1.bin` | 718738272 | `39c547a9afc6da15d847ef81a2c6cea1a6516bdfa562cf13b0999b04e8598bda` |
| Disc 1 archive entry `0x860` / field-local `0x6B9`, sector-padded payload | 51200 | `14395a9f54c124fed016f07906cc882b2a9256d5ade2d10eef99e10fe9366523` |

`field.bin` is loaded at `0x8006FAF0`. The field-handler source-input
manifest (`src/field/**/*.c`, `pc_port/src/data_field.c`, `config/field.yaml`,
and `config/symbol_addrs.field.txt`, sorted as `sha256sum` records) hashes to
`44ed36d0d05abc74273e97b1c9ba72deaa49286163b3d6b2298fde1226f434d3`.
The isolated current/full-ASM field-object manifest hashes to
`abac626030895358732ab4219e7fd8731755c0ae74bf0b729bc0952fe59a3105`.
The isolated copy and working tree had no differences under `src/field` or in
the field configuration at comparison time.

The canonical native rebuild produced `pc_port/build_native/xeno-port` with
SHA-256
`a3c623f0c8dced436a39cee5042b4a4999607db09f369afe53b9abdc037ec817`.
Its generated manifest `pc_port/build_native/stubs.c` hashes to
`c4fdeb16e6aeb64bea6424b5bdf973771ffc26b6417a90db9eae69e6c406119b`.

These hashes establish internal consistency with the supplied retail images;
they are not independent authentication of the operator-owned disc dump.

## Method

[`audit_field_script_vm.py`](../../../pc_port/tools/audit_field_script_vm.py)
performs six independent checks:

1. Read both dispatch tables directly from `field.bin` and compare every slot
   with the symbol selected by `pc_port/src/data_field.c`.
2. Locate every handler's C definition, native conditional replacement, or
   assembly fallback and scan its body for explicit incomplete markers.
3. Compare each freshly built current MIPS function with the full retail-ASM
   object. Instruction operands covered by relocations are normalized, while
   relocation kind and external symbol target are compared symmetrically;
   section-local jumps retain and compare their function-relative target.
4. Read the generated native function/data manifest and inspect native
   relocations for direct edges from every dispatched handler and every
   compiled field TU. Missing native binary/object/manifest artifacts are a
   strict failure rather than an empty result.
5. Parse checked-in native bodies that call `xeno_port_stub`, ignoring comments
   and strings, then inspect field-object relocations to those authored stubs.
6. Inventory executable `assert(0...)` source sites and retain their
   preprocessor context.

This is static evidence. A direct edge is a conservative dependency frontier,
not proof that ordinary play reaches it. Conversely, absence of a direct edge
does not prove all transitive system, renderer, sound, menu, world, or battle
dependencies. Runtime and human-visible acceptance remain separate gates.

## Exhaustive dispatch result

| check | result |
|---|---:|
| Primary slots | 256 |
| Secondary `FE` slots | 227 |
| Total slots | 483 |
| Unique handlers | 481 |
| Retail table-address mismatches | 0 |
| Native binary/object/generated-manifest set present | yes |
| Handlers dispatched directly to a generated native stub | 0 |
| Handlers dispatched directly to an authored native stub | 0 |
| Handlers with a direct generated-stub dependency | 3 |
| Handlers with a direct authored-stub dependency | 0 |

Matching-object outcomes for all 481 unique handlers:

| comparison | handlers |
|---|---:|
| Same size, normalized instructions, and relocation-target signature | 346 |
| Size mismatch | 79 |
| Instruction mismatch | 53 |
| External relocation-target mismatch | 3 |

The audit performed 4084 relocation-target checks across fully matching
functions. The three relocation-only failures are conservative jump-table
ownership differences: opcode `02` (`jtbl_8006FD58`), `F8`
(`jtbl_8006FC88`), and `FA` (`jtbl_8006FCA8`) resolve to named retail tables
but section-local `.rodata` in the current object. Their instruction words and
sizes match, but the data linkage is not yet proven, so they remain unproven.

Final evidence classes:

| class | handlers | meaning |
|---|---:|---|
| `RETAIL_MIPS_MATCH` | 340 | Matching instruction stream and relocation-target signature, with no native override or direct generated-stub dependency |
| `C_NONMATCHING_UNPROVEN` | 129 | C exists, but this build does not reproduce the retail function exactly |
| `PORT_OVERRIDE_UNPROVEN` | 9 | Native-only code or a host hook changes the compiled port path |
| `INCOMPLETE_DIRECT_DEPENDENCY` | 3 | The handler directly calls a generated no-op |

“Unproven” does not mean “known wrong.” It means the current evidence cannot
support a retail-equivalence claim. Likewise, genuinely empty functions are
not automatically fakes: `func_8009CF70`, `func_8008A4E0`,
`func_8008A4E8`, `func_8008A4F0`, `func_8008A500`, `func_8008A508`,
`func_8008A510`, `func_8008A518`, and `func_800931F8` reproduce their retail
empty bodies.

The tool exposes two deliberately different fail-closed gates:

| gate | current result | rejects |
|---|---|---|
| `--strict-runtime` | FAIL | incomplete handler roots, generated or authored field dependencies, generated field data, and native-reachable `assert(0)` |
| `--strict-retail` | FAIL | every runtime blocker plus table drift and all 141 handlers that are not currently classified `RETAIL_MIPS_MATCH` |

This prevents “links and runs” from being mistaken for parity while still
keeping runtime-completeness and exact-retail evidence as separate questions.

## Direct handler blockers

| opcode | handler | generated dependency | retail dependency range | size | SHA-256 |
|---|---|---|---|---:|---|
| `FE1F` | `func_8009FDD4` | `func_800AD4D4` | `0x800AD4D4..0x800AD898` | `0x3C4` | `98fa2e22aeb3ccae9c04d4661febd4dd771e108639b87b8f5a2db1e205bf5f0f` |
| `FE20` | `func_8009FE4C` | `func_800ACFD0` | `0x800ACFD0..0x800AD4D4` | `0x504` | `4a0c2c56dd0051c0c7db399d66b66c82e304869bd3b20058dfa349b96c953ac5` |
| `FEB1` | `func_80087FD4` | `func_800A8BA4` | `0x800A8BA4..0x800A8EAC` | `0x308` | `0ad29bfc1b27aeafdff5fa8b850bc1485fe92167ed61bb93b380306c06fd69d3` |

These routines are dependency closures, not safe leaf substitutions.
`func_800A8BA4`, for example, activates `D_800AF278` and therefore exposes the
still-unimplemented active path in `func_800A84C0`; it also consumes retail
lookup data that the native link currently supplies as generated zero storage.
Implementing only the visible return value would be another fake and is
explicitly rejected.

## Native-only handler paths

| opcode | handler | current reason for native divergence | status |
|---|---|---|---|
| `4B` | `func_80098430` | Native C replacement for the assembly fallback | Unproven |
| `56` | `func_80093014` | Retail transition writes plus an opt-in transition-hold observer/interceptor | Unproven; normal retail path must not rely on the hold |
| `93` | `func_800A1364` | Native C object-registration replacement | Unproven |
| `99` | `func_8008FB98` | Host-compatible register declarations | Unproven native semantics |
| `9A` | `func_8008FC4C` | Host-compatible register declarations; matching build also differs in size | Unproven |
| `A7` | `func_8009F5F4` | Host input injection/edge latch precedes the retail-derived controller body | Behavior-tested, but not instruction-exact |
| `FE21` | `func_800A06E8` | Native C party/actor binding replacement | Unproven |
| `FE54` | `func_80093B10` | Native transition observation hook precedes retail writes | Unproven native semantics |
| `FEB0` | `func_8008AACC` | Host pointer-layout adapter in the WDS path | Unproven |

The transition hold is test instrumentation, not accepted gameplay behavior.
It is opt-in through `XENO_FIELD_HOLD_TRANSITION`; a normal boot must complete
the retail transition without it before that path can be called closed.

## Wider native field dependency frontier

The rebuilt native port contains 93 generated function stubs across all
modules. Direct relocation scanning of compiled `src/field` objects plus the
field-object overlay finds 20 field callers, 26 edges, and 18 unique generated
function targets:

| generated target | direct field callers |
|---|---|
| `RotMatrixZYX` | `FieldParticleUpdateAndRender` |
| `func_8007234C` | `func_800726E8` |
| `func_800A3C8C` | `func_800A2714` |
| `func_800A6C40` | `func_800A5924` |
| `func_800A8BA4` | `func_80087FD4` |
| `func_800ABA98` | `FieldMain` |
| `func_800ACE90` | `FieldMain` |
| `func_800ACFD0` | `func_8009FE4C`, `func_800AD978` |
| `func_800AD4D4` | `func_8009FDD4`, `func_800AD978` |
| `func_802811EC` | `FieldMain` |
| `func_8028125C` | `func_800705DC` |
| `func_802812A4` | `FieldLoad` |
| `func_80281400` | `func_8007554C` |
| `func_80281450` | `func_8007554C` |
| `func_802815B0` | `FieldMatrixCreateWorldToScreen` |
| `func_80281678` | `func_8008237C` |
| `func_80281B00` | `FieldParticlesTickAndRender`, `func_80077DAC`, `func_800739C0`, `func_8007520C`, `func_800752C8`, `func_8007554C`, `func_8008110C` |
| `func_80284EA4` | `func_800726E8` |

One additional field edge terminates in a checked-in authored no-op rather
than the generated manifest:

| field caller | authored target | evidence |
|---|---|---|
| `func_80079288` | `func_80281204` | `pc_port/src/game_overrides.c` calls `xeno_port_stub("func_80281204")` |

This is the random-encounter field-to-battle handoff. The encounter roll can
select and commit a retail formation, but the handoff itself is not
implemented. It is a strict runtime blocker and must not be described as a
working random battle.

The same link also generates 588 independent zero-storage data symbols.
Compiled field code directly references 302 of them through 1968 unique
caller/symbol edges from 695 field functions. Of those 302 symbols, 288 use
the generator's generic 32-byte allocation. Some are legitimate zero-initial
retail BSS, but the generator's minimum-size/doubling heuristic does not prove
their size, adjacency, overlap, or aliasing. Therefore none of those 302
owners is accepted as retail-accurate merely because the port links. The full
symbol-to-caller inventory is emitted in the JSON ledger under
`field_generated_data_targets` and `field_generated_data_edges`.

The highest-fan-out generated owners are:

| symbol | field callers | generated bytes |
|---|---:|---:|
| `g_FieldScriptVMCurActor` | 482 | 32 |
| `g_FieldScriptVMCurScriptData` | 162 | 32 |
| `g_FieldActors` | 150 | 32 |
| `D_800B00C0` | 89 | 32 |
| `g_pGameState` | 72 | 32 |
| `g_GamePartyMembers` | 30 | 32 |
| `g_FieldCurRenderContextIndex` | 28 | 32 |
| `g_FieldCurRenderContext` | 26 | 32 |
| `D_800ADB1C` | 22 | 32 |
| `D_800ADBFC` | 22 | 32 |

Many of these are scalar pointers or flags for which over-allocation can be
harmless. The risk is provenance and layout: their independent allocations do
not establish the retail address relationships consumed by raw-offset code.

Five executable `assert(0...)` source sites remain:

| source | function | branch |
|---|---|---|
| `src/field/main/misc2.c:1795` | `func_800748E8` | PC-HDD timing-marker path |
| `src/field/main/misc2.c:2453` | `func_80075B44` | Far-color fallback; this assertion is under the non-`XENO_PC_PORT` side |
| `src/field/main/misc2.c:2462` | `func_80075B44` | Actor double-render path |
| `src/field/main/misc2.c:2489` | `func_80075B44` | Rotated-actor path |
| `src/field/main/misc5.c:1363` | `func_800A84C0` | Active field-overlay path |

Four of those five sites compile into the native port; the far-color assertion
is replaced by the native fog adapter. The strict runtime audit remains failing
until all direct field generated/authored stub edges and native-reachable
assertion branches are retired from retail evidence.

## Explicit native exceptions found

These are source-visible and intentionally excluded from any retail-parity
claim:

- `func_80281204` is the authored field-to-battle no-op identified above.
- `PcPort_WorldMapPlaceholderMain` remains reachable when world-overlay load,
  initialization, or dispatch fails, and from the legacy diagnostic tail. It
  is an explicit host UI placeholder, not world-map behavior.
- `XENO_FIELD_NO_MODEL_BUILD=1` disables retail field model construction and
  rendering. It is diagnostic instrumentation only; evidence captured with it
  cannot certify gameplay parity.
- `XENO_FIELD_HOLD_TRANSITION` intercepts normal transition progress for
  observation. It is test instrumentation only.
- `func_80080968` retains two native null-owner returns around the relocated
  `D_800AFB24`/`D_800AFB20` tables. Retail assumes those owners are valid, so
  the guards remain behavior divergences even though they fail closed.
- `func_80077AB4` retains a native null-owner guard after
  `func_801E742C`. The archive-0x6B9 owner is now implemented, but the guard is
  still a host-only failure path.
- `OvlyStampTpageClut` rewrites object GPU packets at the host-renderer
  boundary, including forcing textured/raw sampling in specific cases. This is
  a transparent-adapter candidate, not instruction-exact field evidence, and
  remains unproven against PSX GPU output.
- The Status-menu `func_801E2BE4` safe-return bridge is outside this opcode
  ledger and remains fake Status behavior. Its existing regression encodes the
  shortcut and cannot be used as acceptance evidence.

The former native omission of FieldLoad's final per-script-actor direction
initialization is no longer on this list: production now compiles both retail
branches and the focused omission mutant proves the test would fail if that
block were compiled out again.

## Repairs completed during this audit

- Restored the retail semantics of
  `FieldScriptVMWritePartyLeaderCharacterID`, `FieldScriptWriteActorDistance`,
  `func_8009AEE0`, `func_8009B210`, `func_8009B398`, and `func_8008B5D4`,
  including the retail delay-slot side effect in `func_8009B210`.
- Restored the omitted interaction, talk, stuck-retry, and cooldown control
  flow in `func_8009F5F4`.
- Corrected `func_80099AC0`'s retail movement-mode mapping.
- Corrected `func_8007BEF4`'s retail compound walkmesh-side normalization. The
  former mutant is proven wrong; correlation to the reported Alice-stairs lock
  still requires a fresh human/runtime replay.
- Added source-backed leaf implementations for `func_8003633C`,
  `func_800379B4`, `func_800379C8`, and `func_80026F44`.
- Replaced generated field-object cleanup stubs with transparent host-owner
  adapters for `func_801E8030` and `func_801E7FD4`. Their retail slices are
  `0x801E8030..0x801E8330`
  (`e15a7ea45c5adf850542501ced1817d2f6bda71e2b355251dd2381cb3c3d95c7`)
  and `0x801E7FD4..0x801E8030`
  (`0fc1564d18b625500a97b6545bd868c46d1dd8aa93937d392b5e60fbbc2d1218`).
  The adapter frees only allocations created by the native overlay and refuses
  foreign/extended retail owners before performing a partial cleanup.
- Restored FieldLoad's final per-script-actor direction initialization in the
  native build. The implementation now calls `func_80021FE0` or
  `func_800223B0` under the same retail status/model flag split instead of
  compiling the entire block out under `XENO_PC_PORT`.

These are not all instruction-exact ports. The focused host tests establish
the stated behavior and ownership contracts; the exhaustive ledger continues
to classify nonmatching/native paths as unproven.

## Verification certificate

The following passed on this tree:

- `run_field_script_vm_audit.sh`: all 483 slots covered; table, instruction,
  relocation, source, authored-stub, and assertion scanner negative controls
  passed.
- `run_field_script_vm_core_handlers.sh`: O0/O2/UBSan agreement and two mutants.
- `run_field_script_vm_party_move.sh`: O0/O2/UBSan agreement, three pinned
  retail slices, and two mutants.
- `run_field_script_vm_primitive_offset.sh`: O0/O2/UBSan agreement, pinned
  retail slice, and two mutants.
- `run_field_script_vm_update_character.sh`: O0/O2/UBSan agreement, 40 checks,
  pinned retail slice, and three mutants.
- `run_field_walkmesh_compound_edge_test.sh`: O0/O2/UBSan agreement, 36 checks,
  pinned retail slice, and the former compound-side mutant detected.
- `run_retail_leaf_adapters_test.sh`: O0/O2/Clang UBSan agreement, 15 checks,
  and four pinned executable slices.
- `run_field_object_cleanup_adapter.sh`: O0/O2/Clang UBSan agreement, 37 checks,
  two pinned overlay slices, and three ownership mutants.
- `run_field_load_final_actor_init_test.sh`: O0/O2 production-object relocation
  proof, pinned `FieldLoad` retail slice `0x80070CC8..0x80071A64`, retail JAL
  targets at `0x80071A04`/`0x80071A1C`, and the former native-omission mutant
  detected. The `0xD9C`-byte slice hashes to
  `b07e933f5c019476dd472261b76e1200aae50eb4bae8627cad769a3f8f75ad2c`.
- Canonical native container build: 49 game TUs compiled, zero skipped, final
  link passed.
- Bounded native Map 2 diagnostic smoke on isolated Xvfb: `FieldLoad` allocated
  104 actors, built 65 model actors, completed its VM pass, and entered the
  field main loop without an assertion or crash before the expected 15-second
  timeout. The 68147-byte log hashes to
  `0551302a3c1cc6b76011aad1f2a2cf5e5b4051ca5e43063bd27ee3dd74e5b5ed`.
  This is production-path crash evidence, not retail boot or human-visible
  acceptance.

Both `--strict-runtime` and `--strict-retail` exit `1` on this tree as
intended. A green build is not a green retail-parity gate.

Regenerate the full JSON/CSV ledger with:

```sh
PATH="/tmp/xeno-cross-bin:$PATH" ninja -C "$MATCHING_ROOT" -j24 build/out/field.elf
python3 pc_port/tools/audit_field_script_vm.py \
  --matching-root "$MATCHING_ROOT" \
  --json /tmp/field-script-vm-audit.json \
  --csv /tmp/field-script-vm-audit.csv
python3 pc_port/tools/audit_field_script_vm.py \
  --matching-root "$MATCHING_ROOT" --strict-runtime
python3 pc_port/tools/audit_field_script_vm.py \
  --matching-root "$MATCHING_ROOT" --strict-retail
```

The generated JSON and CSV for this run hash to
`a0295a70b31562ed1a861cbb2dde03f915c6fac6db8084651f0e984948663fbe`
(603482 bytes) and
`4093dd2665e11016b8c9899ee97492b06dbd0a545f809afddc55e7f191be0b85`
(86336 bytes).

## Remaining boundary and continuation rule

The next field-script closure is the three direct dependencies above, starting
with their data owners rather than isolated function shells. After that, the
wider `FieldMain`/`FieldLoad`, rendering, particle, and overlay frontiers must
be retired. Each future closure must carry a retail byte/range anchor, a
production-path test, at least one negative control where practical, a fresh
native build, and a regenerated exhaustive ledger. Counts may improve; the
gate must not be weakened to make them green.

Outside this field-opcode scope, `func_801E2BE4` remains an explicitly labeled
Status-menu safe-return bridge. It is not retail Status behavior and must not
be counted as completed. Title KROM/art, world, battle, sound, and renderer
parity require their own authority-specific audits.
