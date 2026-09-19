# Current port / compiled MIPS audit — 2026-09-19

**The current tree is not verified as 100% decompiled or retail-equivalent.**
Every one of the 275 C translation units under `src/` was freshly compiled with
its actual matching-build Ninja flags. No compilation failed. The native port
also builds successfully. These are distinct checks: x86 port code cannot be
byte-compared directly with MIPS code.

Baseline: branch `experiment/worldmap-open-gates-20260823`, HEAD
`f1830448959de0bde3aaf3b524d06553896a5bd6`, plus the guarded
[field-12 repair](../field12-walk-stall-20260919/README.md). No other gameplay
logic was repaired by this audit. No commit or push was made.

## Fresh byte comparison

| Matching-build body | Byte-identical | Different size | Different bytes, same size | Unresolved relocation | No same-name retail listing | Unresolved assembly span |
|---|---:|---:|---:|---:|---:|---:|
| C | 1905 | 535 | 158 | 26 | 31 | 0 |
| Included assembly | 902 | 0 | 0 | 0 | 0 | 2 |

Counts are **compiled body occurrences**, not a completion percentage. There are
3559 occurrences and 3507 unique `(overlay,function)` names; alternative battle
TUs define 52 names more than once. Assembly matches do not count as decompiled C.
The 693 different C outputs are matching gaps, not proof of 693 gameplay bugs:
compiler shape, deliberate port coexistence, and semantic differences need
individual diagnosis. The 31 names without listings include added C helpers.

[functions.csv](functions.csv) records every body, its byte result, hashes when
resolved, and current native source ownership. [summary.json](summary.json)
contains counts and authority hashes. `audited-inputs.sha256` pins the audited
source/configuration snapshot (verify with `sha256sum -c` from repository root). All ten disc images in
`config/checksum.sha` were checked. Every instruction-byte comment used from the
retail listings was checked against the corresponding disc image.

ELF `STT_FUNC` symbols define compiled function boundaries, not objdump's internal
local-label headings. Embedded object symbols terminate code spans. Relocations
are resolved to retail addresses using symbol maps, address-bearing retail
names, known retail function entries, and the configured GP (`0x80059170`).
Resolved little-endian bytes are compared directly and both SHA-256 values are
recorded. This is a per-function comparison at retail placement, **not a claim
that an entire freshly linked image equals retail**, or that data sections match.
A separate symbolic instruction comparison is retained in the local JSON as a
diagnostic; its `EXACT` label must not replace the raw-byte result.

The resolver supports direct/local jumps, PC-relative branches, HI16/LO16 and
GP-relative relocations. Unresolved section-based relocations stay unresolved.
The two assembly span cases (`battle:func_800C11CC` and
`battling:func_80090F38`) involve padding or embedded data/boundary differences;
they are not asserted to be bad retail assembly. The byte comparator rejects
both an instruction-word mutation and a relocation-target mutation of independently
verified `func_8009E10C`.

## What the native port actually runs

The native link has **82 generated function stubs**, **577 generated data
symbols**, and **20 explicit port-owned overrides**. These are not equivalent
categories: host GPU/audio/pointer replacements may be necessary and correct;
a generated zero-return function is still a missing behavior if reached.

[stubs.csv](stubs.csv) lists all 82 current function stubs. Twelve have a C
candidate in matching source, demonstrating that more decompilation is not the
answer to every gap. Examples include `ArchiveCdDriveCommandHandler` and battle
functions `80076A10`, `80076B68`, `80076BF0`, `8007FDEC`, `800800E8`, `8008A274`,
`800B8354`, `800BC2F0`, `800BC460`, and `800BDCF8`. These need ownership/call-path
review before adoption; addresses can collide across overlays, and an existing
C body is not automatically safe for host pointers or native state.

The build also registers **253 runtime port C files**, including the entry
point, and five test-only files. [port-only-sources.csv](port-only-sources.csv)
lists them. Their host-specific code is outside the matching MIPS compilation;
this pass inventories them and checks the native build, but does **not** certify
all of their semantics. They require retail differential tests or observed
runtime evidence. Tests already in the tree were not all rerun by this audit.

Battle still links `battle_mips_runtime.c` and a host-adoption bridge. The fresh
build verifies 92 adopted leaves cannot reach a generated stub. That bounded
gate does not mean the rest of battle is native C. World-map retail code is
assembled from `asm/world_map/120C.s`; there is no `src/world_map/*.c`. Numerous
world-map implementations exist separately in `pc_port/src`, so this is a
matching-source coverage gap, not evidence that the port has no world-map code.

Native owner classification uses `nm` plus linked DWARF source locations;
compiled object presence alone is not ownership proof. A matching-source body
may be replaced by a strong port definition or an identically named function
from another overlay. The CSV preserves these distinctions. Matching MIPS
bytes for a shared source do not verify its `XENO_PC_PORT` branch.

[retail-labels-outside-c-tu-symbols.csv](retail-labels-outside-c-tu-symbols.csv)
records retail labels without a same-name function symbol among the fresh C-TU
objects. This is a review queue, not a count of missing implementations:
renames, assembly-only build inputs, exported data labels, and port replacements
must be reconciled before claiming a missing body.

## Recommended next work, based on these results

1. Follow actual executed stubs and missing battle/world-map dispatch targets.
   Check whether a correct C implementation already exists before decompiling.
2. Audit native divergences on the active path against full retail control flow.
   Field12 is the concrete example: an existing test encoded the same erroneous
   omission as the port. The repair restored a shared-tail store, not a gate bypass.
3. Use the function CSV to select matching gaps relevant to the path being
   repaired. Do not infer semantic correctness or missing functionality solely
   from compiled size, source coverage, or an `INCLUDE_ASM` match.
4. Extend differential tests to the port-only implementations and perform
   natural gameplay acceptance. Whole-game/native parity remains unverified.

`tools/scripts/decomp_status.py` explicitly infers `MATCHED` from plain C source
layout without an objdiff oracle. Its completeness number is therefore not an
independent byte-match result and must not override this measurement. Older
percentage claims in handoffs should be treated as dated scope-specific evidence.

## Reproduce

Run from the repository root. Existing generated Ninja configuration is used;
no retail payload or existing matching output is overwritten.

```sh
python3 docs/evidence/current-port-mips-audit-20260919/prepare.py
podman run --rm --userns=keep-id --security-opt label=disable \
  -v "$PWD:$PWD" -w "$PWD" localhost/xenogears-dev-toolchain:current \
  python3 docs/evidence/current-port-mips-audit-20260919/compile.py
python3 docs/evidence/current-port-mips-audit-20260919/compare.py
python3 docs/evidence/current-port-mips-audit-20260919/native_inventory.py
python3 docs/evidence/current-port-mips-audit-20260919/summarize.py
```

The host needs Ninja and `mips-linux-gnu-objdump`; the current container provides
the matching compilers. Native builds/tests instead use
`localhost/xenogears-dev-toolchain:krom-20260913` for OpenSSL3/KROM support.
Build the native executable first if regenerating its ownership inventory.
The audit scripts assume repository-root cwd and write isolated products to
`scratchpad/astra-port-audit-20260919/`. Local `jobs.json`, `compiled.json`,
`comparison.json`, per-object build logs, and `native-symbols.json` retain the
complete commands/results. CSVs and hashes are the reviewable evidence; no
retail binary payload is included here.
