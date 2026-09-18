# Opening callback audit — 2026-09-06

Read-only production audit; only this /tmp note written. Root owns diagnosis rerun and production changes. No desktop operation, no game process started, no changes to dirty concurrent work.

## Pins

- Repository `/var/home/blizz/Projects/xenogears-decomp-ai`, branch `experiment/worldmap-open-gates-20260823`, HEAD `3a3e7aac03a2f166fb924945a489e392d706f282`; heavily dirty as expected.
- Pinned executable `pc_port/build_native/opening-aa-5-0rmj97mi/xeno-port` SHA256 `a2b0826171ab50f36ea4e3c0af6fedd951601a4975e654d96beab8d2232066ce` (verified).
- `src/slus_006.64/system/animation_scripts.c`: `e423b31fa6b7cb881f677a141b8dfa7c4d4ebdafdc3dc177639cb4316241bff7`.
- `pc_port/src/battle_mips_runtime.c`: `b46535084fe718fe7fd966d0ca8d27047891a8b56a81dfaa773c171f967e85d3`.
- `disc/battle.bin`: `1830b4ef1fe37129972fc310dfad534f8161d6c0b123e74254c3711334a3e291`.

## Finding

The failing target `0x007fc208` is a host pointer INTO EMULATED RAM, not native executable text. `nm -n` of the pinned executable puts `g_PsxRam` at `0x00612e20`; subtraction gives RAM offset `0x001e93e8`, or guest `0x801e93e8`. This classification is exact but does not by itself prove the bytes there are valid executable retail code.

The callback slot is authoritative: `asm/battle/nonmatchings/main/func_800C11CC.s:845-850` loads `lw v0,0x68(s2)` at retail PC `800C1E5C`, null-checks, and calls `jalr v0` at `800C1E6C` with `a0=s2`. Native `func_80021BF8` at `src/slus_006.64/system/animation_scripts.c:1236-1238` stores its second argument verbatim as a 32-bit value at sprite+0x68. `func_80023804` initializes the slot to zero (`temp1.c:566`).

The bridge has a concrete ABI omission: `is_callback_argument` at `battle_mips_runtime.c:360-370` lists work-list callback setters but omits `func_80021BF8`. Therefore calls into this setter follow `translate_argument` (`348-357`, selected at `711-724`), translating guest code addresses to host RAM pointers as though they were data arguments. An incoming callback `801e93e8` becomes exactly `007fc208` in this binary before being stored by the native setter.

The generated map registers `{0x80021bf8, function, "func_80021BF8"}` (`pc_port/build_native/battle_bridge_map.inc:90`). Known base-overlay setter calls demonstrate the argument semantics independent of names: `func_800BF600.s:25-27` passes callback `800BF5E8`; `func_800B9F78.s:28-30` and `func_800B89FC.s:180-182` pass `800B9B30`; multiple calls explicitly clear with a1=0. Seventeen setter calls exist in the base battle asm. The specific dynamic callback creation that produced `801e93e8` is NOT YET OBSERVED by this audit; root owns that rerun.

## Why not broaden callback acceptance

`target_is_guest_code` (`373-379`) already accepts guest `801d0000..80300000`, including `801e93e8`; it does not accept its host RAM alias. `runtime_bridge_call` (`658-690`) allows registered retail addresses, exact registered native symbol host addresses, or a resolved `func_%08X` symbol. The diagnostic is the intended fail-closed outcome after the malformed callback slot is read. The target cannot be repaired by registering an arbitrary host address or treating all RAM pointers as executable. Even fetching MIPS from a host alias would give guest J/JAL incorrect top-address bits in the adapter, besides bypassing code provenance.

## Smallest next observation and candidate repair

At native-boundary target `80021BF8`, record caller RA-8, raw guest a0/a1, translated a0/a1, and sprite+68 before/after the native store. At the unresolved `007fc208` stop record s2 and sprite+68 plus a bounded dump at RAM offset `1e93e8`; establish the loaded retail module/source bytes for that callback. Expected causal chain: incoming a1 `801e93e8` -> translated a1 `007fc208` -> sprite+68 `007fc208` -> exact failing JALR.

If confirmed, the smallest correct production repair is to add `func_80021BF8` argument index 1 to the existing callback-argument classification. Preserve its raw guest address; retain data translation for a0. No changes to accepted call ranges and no unknown-pointer fallback. A focused bridge regression should pass a retail callback through the real setter, assert the packed slot remains its guest address, dispatch the actual guest callback, and use omission of that one classification as a negative control. Confirm native host callbacks and zero still store correctly, and data argument translation remains intact. Then repeat normal opening to observe beyond AA and this callback.

Status: ABI omission and address arithmetic VERIFIED; exact runtime setter provenance and dynamic callback retail-byte identity PENDING root observation. No production fix made.


## Follow-up: dynamic callback retail-byte authority VERIFIED

Root-owned natural GDB replay `pc_port/build_native/opening-callback-trace-5-orqx88rm` stopped in a different `HeapConsolidate` SIGSEGV. Its setter trace has 50 direct native setters and no guest bridge setter rows. Therefore the exact setter sequence causing the prior `007fc208` failure remains NOT_OBSERVED. This does not prevent byte provenance from being verified against the saved RAM.

- Saved `stop-ram.bin`: 2,097,152 bytes, SHA256 `efcc6acf407ce421acd463f90b88d9e9be777f1c0c7067dfa88841b8c70b0972`.
- Retail disc `disc/disc1.bin` SHA256 `39c547a9afc6da15d847ef81a2c6cea1a6516bdfa562cf13b0999b04e8598bda`.
- Exact source: archive header directory `0x20`, file `1`, table index 3087 (zero-based), starting Mode2/Form1 sector `0x3EC7F`, payload length `0x4C3C` (19,516 bytes). Read 2048-byte user payloads starting at raw sector byte offset +24. Payload SHA256 `64668d85bf48dea46cf6d5f04b38e38ca887877cfef53961f0c26cd2d363b670`.
- Source extraction uses the repository's `tools/scripts/cdrom/cdxa.py` and the archive-header/table algorithm in `tools/scripts/psx/overlay.py`, cross-checked with native `ArchiveSetIndex` (`libarchive.c:75`) and `ArchiveDecodeSector` (`306`). No payload extraction files were written by this audit.
- Authoritative load: retail `80070E40` calls `func_8008AB4C`; that helper (`asm/battle/nonmatchings/main/func_8008AB4C.s:6-8`) selects `ArchiveSetIndex(0x20,0)`. `asm/battle/133C.s:30-38` loads file1 directly to `801E5000` with `ArchiveReadFileToBuffer` at `80070E88`. It synchronizes then calls loaded `801E5160` at `80070E98`. This module is raw, not compressed, and already linked at its runtime address. No relocation allowance is needed.

`stop-ram.bin[0x1E5000:0x1E9C30]` equals disc payload `[0:0x4C30]` byte-for-byte (SHA256 `72d9c7f2210e81391b13dee8441193e27039260c85c6023886517ec001297857`). Across the full module, the only differences are five bytes in its final three writable data words:

| Module offset | Guest address | Retail word | RAM word | Authoritative writer |
| --- | --- | --- | --- | --- |
| `4C30` | `801E9C30` | `00000000` | `00000020` | `801E6FBC sw a3,-63D0(at)` after `lui at,801F` |
| `4C34` | `801E9C34` | `00000000` | `00000094` | `801E6FA0 sw v0,-63CC(at)` after `lui at,801F` |
| `4C38` | `801E9C38` | `00000000` | `801AB300` | `801E51D0 sw v0,-63C8(at)` after `lui at,801F` |

These are mutable module globals, not code relocations. The callback itself has an unambiguous complete leaf-with-call extent `801E93E8..801E9430` (exclusive end), module offset `43E8..4430`, length `0x48` (72 bytes). The preceding function returns at `801E93E0/93E4`; this callback starts a fresh stack frame at `93E8`, returns at `9428/942C`, and the next function starts at `9430`. All 72 bytes exactly match the saved RAM and retail source, SHA256 `df0c3521a9f2dcfab542d62a8b63e03b627c4dacfdcc933e96868324311bcb5e`.

Callback semantics: read sprite+A8 bits30..31 and sprite+AC bits0..1; combine them as `((AC & 3) << 2) | (A8 >> 30)`; read the table owner from guest global `800D3278`; clear its byte at `owner+0x804+index`; call `func_80021BF8(sprite,0)` to unregister this same sprite callback; return. Retail does the clear in the setter JAL delay slot at `801E941C`.

Three exact setters in this raw module establish that `801E93E8` is an intentional callback address, independently of any host-runtime inference:

- `801E94D0`: JAL `80021BF8`, with `lui a1,801F` at `94CC` and delay-slot `addiu a1,a1,-6C18` at `94D4`.
- `801E9678`: same callback, `a0=s0` at `9670`, `lui a1,801F` at `9674`, delay-slot low half at `967C`.
- `801E96E4`: same callback, `a0=s0` at `96DC`, `lui a1,801F` at `96E0`, delay-slot low half at `96E8`.

Final follow-up classification: valid loaded retail guest callback VERIFIED; host text relocation REJECTED by pinned address map; arbitrary corrupted data as original callback intent REJECTED by three retail setter sites and exact code bytes. Exact original runtime setter-to-slot causality remains NOT_OBSERVED because this trace reached a separate heap failure. The scoped argument-classification repair is source-backed without needing any broadened call-target acceptance.
