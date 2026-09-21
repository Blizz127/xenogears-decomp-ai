# Retail dynamic module `(0x20,0)/file 1` and `0x801E6FC0` caller inventory

Date: 2026-09-06 UTC  
Repository inspected: `/var/home/blizz/Projects/xenogears-decomp-ai`  
HEAD inspected: `3a3e7aac03a2f166fb924945a489e392d706f282`  
Status: **READ-ONLY INVENTORY COMPLETE; IMPLEMENTATION NOT PERFORMED**

Only this report and `/tmp/xeno-opening-dynamic-module-20260906` were written. The shared repository was already heavily dirty. In particular, the new battle config/source/assembly lane is untracked and owned by other work; none of it was changed.

## Finding

The retail instruction at `0x801E6FC0` is not a standalone function. It is the resident-constructor call inside a single dynamic-module function with exact extent **`0x801E6CE8..0x801E71D4`**, module offsets `0x1CE8..0x21D4`, size `0x4EC` (1,260 bytes), SHA-256 `1f11c9ac7117c0d702c6829cb3fb57806d73c6413eaec5f9fbf1ecca2b427a2e`. The function creates/configures a battle text window on first entry, selects a string, queues it, waits for/handles completion, tears down the window, restores five control halfwords, and returns a completion value.

The smallest source-faithful decompilation unit is therefore the whole `0x4EC` function. Splitting out `0x801E6F74..0x801E702C` as a C function would create a boundary absent from retail and obscure live register/stack dependencies from the earlier part of the function.

The module is a **battle-owned dynamic command/UI module**, rather than base `battle.bin` or the main executable. Byte evidence for that bounded classification is:

- Base battle loads and initializes it, then calls module entry points `0x801E5160`, `0x801E563C`, and `0x801E879C`.
- `0x801E5160` allocates the module's `0x828`-byte control block and `0x98`-byte window, installs their pointers in `0x800D3278` and `0x800D2DAC`, and loads a string table into `0x800D3340`.
- The raw module begins with dispatch tables at offsets `0x0000..0x015F`. Two opcode paths at `0x801E8BDC` and `0x801E8C00` call wrappers `0x801E71D4` and `0x801E7230`; both wrappers call `0x801E6CE8`.
- The inventoried function calls the ordinary resident window constructor, string-table lookup, string queue, window control, and destruction routines.

This establishes module role and ownership. It does not identify the queued string's human-readable text or prove visible glyph output.

## Retail extraction and load authority

The module was independently extracted into scratch as `archive20_file1.bin` using the repository algorithm in `tools/scripts/psx/overlay.py` and Mode2/Form1 framing in `tools/scripts/cdrom/cdxa.py`:

| Item | Retail value |
|---|---:|
| Disc | `disc/disc1.bin` |
| Disc SHA-256 | `39c547a9afc6da15d847ef81a2c6cea1a6516bdfa562cf13b0999b04e8598bda` |
| Archive directory | `0x20` |
| Header raw base / adjusted base | `3088 / 3087` |
| File | `1` |
| Zero-based table index | `3087` |
| Table byte offset / bytes | `0x5469 / 7f ec 03 3c 4c 00 00` |
| Starting sector | `0x3EC7F` |
| Payload size | `0x4C3C` (19,516) |
| Payload SHA-256 | `64668d85bf48dea46cf6d5f04b38e38ca887877cfef53961f0c26cd2d363b670` |
| Runtime destination | `0x801E5000` |

The loader chain is explicit in existing retail assembly:

1. `asm/battle/133C.s`: `0x80070E40` calls `func_8008AB4C`.
2. `asm/battle/nonmatchings/main/func_8008AB4C.s`: `0x8008AB54..58` calls `ArchiveSetIndex(0x20,0)`.
3. `asm/battle/133C.s`: `0x80070E74..8C` calls `ArchiveReadFileToBuffer(1,0x801E5000,0,0x80)`.
4. `0x80070E90` synchronizes, then `0x80070E98` calls module initializer `0x801E5160`.

The file is raw, already linked for `0x801E5000`, and is not decompressed or relocated by this path. Independent comparison with `pc_port/build_native/opening-callback-trace-5-orqx88rm/stop-ram.bin` confirms payload `[0,0x4C30)` equals RAM `0x801E5000..0x801E9C30` byte-for-byte, SHA-256 `72d9c7f2210e81391b13dee8441193e27039260c85c6023886517ec001297857`. Only five bytes differ in the final three writable words:

| Offset/address | Retail | Saved RAM | Writer |
|---|---|---|---|
| `0x4C30 / 0x801E9C30` | `00000000` | `20000000` | `0x801E6FBC` stores panel X-derived value |
| `0x4C34 / 0x801E9C34` | `00000000` | `94000000` | `0x801E6FA0` stores panel Y-derived value |
| `0x4C38 / 0x801E9C38` | `00000000` | `00b31a80` | initializer `0x801E51D0` stores allocation pointer |

These are ordinary mutable module globals. There is no observed relocation patching of code or address-bearing tables.

## Exact function boundaries and callers

| Function or region | Exact extent | Size/hash | Evidence |
|---|---|---|---|
| Previous internal helper | `0x801E6750..0x801E6CE8` | ends with `jr ra` at `0x801E6CE0` | next instruction is a fresh 0x50-byte stack frame |
| Constructor/text-queue controller | **`0x801E6CE8..0x801E71D4`** | `0x4EC`; SHA above | prologue saves `s0..s7/fp/ra`; epilogue restores all and returns at `0x801E71CC` |
| Wrapper A | `0x801E71D4..0x801E7230` | `0x5C`; SHA-256 `69505502c8346efa340b887851eb66f04153731850f12ea1300d99349d147325` | called at `0x801E8BDC`, calls controller at `0x801E720C` |
| Wrapper B | `0x801E7230..0x801E7278` | `0x48`; SHA-256 `0a79aabb4a3607be1e09f7b898eec90928b20eaaaef7fd0d1e34b1325b0b88b3` | called at `0x801E8C00`, calls controller at `0x801E7250` |
| Next function | starts `0x801E7278` | prior wrapper returns at `0x801E7270` | fresh stack frame |

Wrapper A assembles controller argument 1 from command bytes `+1/+2`, passes a control byte from module state, and passes command byte `+3`. Wrapper B assembles argument 1 from bytes `+2/+3`, then passes bytes `+1` and `+4`. These are byte-backed interface facts; semantic names for the command fields remain inferred.

A misleading overlap exists in the current base-battle generated symbol set: `linker/undefined_funcs_auto.battle.txt` declares `func_801E7210`, because base battle contains a JAL to that address. In **this file-1 payload**, `0x801E7210` is the delay-slot instruction of the wrapper-A call at `0x801E720C`, not a valid independent function prologue. Base battle can replace the `0x801E5000` window with other archive modules before calling other `0x801E...` entry points. Therefore one global address-only namespace cannot safely assign every `func_801E...` reference to file 1. Module identity and load state are required.

## Direct-call inventory for `0x801E6CE8..0x801E71D4`

| Call PC | Target | Bounded role/ownership |
|---|---|---|
| `801E6E80`, `801E6F20` | `8008F8F4` | base-battle helper, currently nonmatching asm via `src/battle/main.c` |
| `801E6EA4`, `801E6F30`, `801E7034`, `801E7134` | `800716D8` | base-battle helper, nonmatching asm |
| `801E6EEC` | `801E6750` | same file-1 module, immediately preceding function |
| **`801E6FC0`** | **`80032F54`** | resident window constructor; current C in `src/slus_006.64/system/system.c` |
| `801E6FF0` | `80034614` | resident window queue/control reset, current C in `system.c` |
| **`801E7004`** | **`80033728`** | resident `GetStringEntry`, current C in `system.c` |
| **`801E7014`** | **`80034714`** | resident window string enqueue, current C in `system.c` |
| `801E7098` | `801E5B00` | same module; local presentation helper using module global `801E9C1C` |
| `801E70CC` | `800345E0` | resident window wait/control clear, current C in `system.c` |
| `801E712C` | `800346D4` | resident window row/work destruction, current C in `system.c` |
| `801E7158` | `8008FA60` | base-battle allocation cleanup helper, nonmatching asm |

The complete direct-call list is preserved as `direct-calls.csv`. Indirect control flow inside the function was not relabeled as calls.

## Constructor and text-queue block

The relevant retail data flow is exact:

- Controller argument `a0` is saved as an unsigned 16-bit string index at caller stack `+0x20` (`0x801E6D28`).
- `0x801E6F60` sets UI byte `[*(0x800D2D28)+0xC8] = 1`.
- `0x801E6F74/78` set constructor `tpageX=0x380`, `tpageY=0x100`.
- `0x801E6F80` loads module control pointer `*(0x800D3278)`; `0x801E6F88` loads window pointer `*(0x800D2DAC)`.
- Stack argument 5 (Y) is control `+0x7F8 + 8`; argument 6 (width) is `3 * control[+0x7FA]`; argument 7 (rows) is unsigned halfword `control[+0x7FC]`.
- **`0x801E6FC0` calls resident `0x80032F54`; its delay slot at `0x801E6FC4` writes rows to original SP+0x18.** This is a retail seven-argument ABI.
- It writes window byte `+0x58=4`, sets flags bit `0x2`, and calls `0x80034614`.
- `0x801E6FFC` loads string table `*(0x800D3340)`, `0x801E7000` loads the saved unsigned string index, and `0x801E7004` calls `GetStringEntry`.
- `0x801E7010` loads the window; `0x801E7014` calls `0x80034714(window, returned_string)`.
- `0x801E7028` sets `[*(0x800D2D28)+0xC9] = 1`, enabling the base-battle window presentation path.

A byte slice `0x801E6F60..0x801E702C` is 0xCC bytes, SHA-256 `0461ba2f1fc2ba75bd546e2c0ef7014d5eb82f788a0c9844ff47f42fa09766f7`. This hash is for review navigation, not a proposed independent function.

The resident constructor ABI difference must remain explicit during decompilation: retail passes seven arguments and consumes the low 16 bits at original SP+0x18 as rows. Current native `system.c` intentionally has eight arguments with a dead seventh `mode` and eighth `height`, because existing native callers compensate for it. A file-1 exact-match translation should declare/call the retail seven-argument interface in its separate decomp target. Native-port integration must use the target-specific adapter already justified by runtime evidence; it must not silently compile this retail call against the eight-argument native declaration. Astra owns the resident constructor-body audit, which this inventory does not duplicate.

## Data dependencies of the controller

### Inputs and module state

- Arguments: saved `u16 a0`; low byte of `a1`; low 16 bits and flag bits `0x1/0x2/0x4/0x8/0x10` of `a2`.
- `0x800D3278`: pointer to the module's `0x828` control block. The controller reads/writes offsets `+0x7F6`, `+0x7F8`, `+0x7FA`, `+0x7FC`, `+0x7FE`, and `+0x802`.
- `0x800D2D28`: UI state pointer. It observes `+0xBF` and writes `+0xC8`, `+0xC9`, `+0xCF`, and `+0x9E`.
- `0x800D2DAC`: window pointer. It reads/writes flags and cursor fields and passes the object to all resident window functions.
- `0x800D3340`: loaded string-table pointer used by `GetStringEntry`.
- Absolute byte `0x800D3014`: compared to 4 before one close/reset path.

### File-1-local data

- `0x801E9C10..0x801E9C1A`: five halfwords `7FFF,7FFF,0010,0008,01F0`; copied back to control `+0x7F6..+0x7FE` during teardown.
- `0x801E9C1C`: initialized value 4, used as a cycling presentation-helper index.
- `0x801E9C30` and `0x801E9C34`: writable X/Y-derived values set immediately before construction and consumed at `0x801E706C/708C`.
- Same-module callees `0x801E6750` and `0x801E5B00` are required dependencies. They should initially remain module-local asm boundaries while the controller is translated.

The module layout itself has address-bearing tables at offsets `0x0000..0x015F`, code beginning `0x0160` (`0x801E5160`), code ending at `0x4B5C` (`0x801E9B5C`), and local data `0x4B5C..0x4C3C`. Any config must preserve those distinctions and the local absolute addresses.

## Current repository ownership

There is currently **no extraction target, config, asm tree, or C source for archive `(0x20,0)/file1`**. Searches for its defining entries and for `0x801E6CE8` found none.

The adjacent base battle overlay is a separate work product:

- `config/battle.yaml` targets decompressed `disc/battle.bin` at `0x8006FAF0` and has no segment at `0x801E5000`.
- `src/battle/main.c` and `asm/battle/**` own base battle only. Calls to selected dynamic addresses appear as undefined external symbols.
- `linker/undefined_funcs_auto.battle.txt` contains twelve address-only `func_801E...` externs, but no `0x801E6CE8`. These declarations do not decompile or own the dynamic payload.
- `config/battle.yaml`, `config/symbol_addrs.battle.txt`, and `src/battle/main.c` are currently untracked shared changes. They must not be repurposed for file 1.
- The native port's MIPS runtime can execute loaded retail instructions from this address range. That is runtime support for retail bytes, not decomp-owned C or exact-match ownership.

## Minimal implementation plan

1. **Create a separate module identity.** Add a dedicated config/output for archive directory `0x20`, file 1, using an ignored local extraction at the pinned hash and VMA `0x801E5000`. Do not append this payload to `battle.bin` or reuse its linker namespace. Record archive coordinates and raw/no-relocation status in the config.
2. **Preserve the actual binary layout.** Start with module rodata/pointer tables `0x0000..0x0160`, code `0x0160..0x4B5C`, and data `0x4B5C..0x4C3C`. Split the target function at offsets `0x1CE8..0x21D4`; retain the preceding function and wrappers as asm initially. Verify the complete module rebuild or at minimum byte-exact slice in its dedicated linker target.
3. **Name symbols by module, not address alone.** Give the controller, its two wrappers, and local globals a file-1/module namespace or keep local linkage. Do not bind base-battle `func_801E7210` to the instruction at this file's `0x801E7210`; it belongs to a different dynamic-load state.
4. **Translate the whole `0x4EC` function.** Keep its inferred three-argument interface and completion return, all flag branches, per-frame waits, construction/queue/close sequence, and five-halfword restoration. Treat `0x801E6750` and `0x801E5B00` as typed asm dependencies until separately owned.
5. **Use a module-specific retail ABI header.** Declare resident calls at their retail MIPS interfaces, especially the seven-argument constructor. Keep guest pointers as 32-bit target pointers in the exact-match target. Avoid including the native eight-argument constructor declaration in this build.
6. **Prove decomp ownership independently.** Build with the project's PSYQ/MIPS matching pipeline and compare the generated bytes for `0x801E6CE8..0x801E71D4` against SHA-256 `1f11c9...`. Require direct-call PCs, delay slots, stack size, local-global addresses, and wrapper boundaries to remain exact. A native smoke test is not this gate.
7. **Gate native adoption separately.** Until a later port-specific change explicitly maps this module function to compiled C and passes raw-MIPS differential cases, retain the existing guest interpreter as runtime authority. If compiled C is adopted, use the target-specific `0x80032F54` seven-to-eight argument adapter and guest-memory aliases; test constructor rows, selected string pointer, queue order, UI C8/C9 flags, close behavior, and returned completion against the raw retail function. Do not infer equivalence merely because the exact-match source compiles into the decomp target.

This plan decompiles the actual retail function without changing the resident constructor, base battle ownership, dynamic loader, or native runtime dispatch.

## Scratch evidence

- `archive20_file1.bin`: exact extracted payload.
- `message-controller.bin`: exact `0x801E6CE8..0x801E71D4` function slice.
- `caller-context-disassembly.txt`: adjacent functions and controller disassembly.
- `direct-calls.csv`: all 15 direct calls in the function.
- `entry-disassembly.txt` and `update-disassembly.txt`: initializer/teardown context supporting module-role classification.
- `extraction-metadata.json`: archive-table derivation and disc pin.
- `ram-comparison.txt`: independent loaded-RAM comparison.
