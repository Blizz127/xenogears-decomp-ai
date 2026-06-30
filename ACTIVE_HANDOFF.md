# Active Handoff — PC Port

_Last updated: 2026-06-30. Canonical roadmap lives in the `pc-port-phases` memory; this is the short cross-session frontier note._

## Where we are
- **Phase A** (boot → interactive KernelMenu): DONE.
- **Menu.2** (disc/archive overlay loading): DONE + verified. Disc image mounts, `ArchiveInit` loads the index, synchronous archive reads work; selecting Field loads + LZSS-decompresses the field overlay from disc.
- **Phase C (Field)**: STARTED — `FieldMain` decompiled and entering.

## What was just done (FieldMain gateway)
- `src/field/main/main.c`: `FieldMain` decompiled (port-first functional C, control flow 1:1 with asm, all calls preserved, logging under `#ifdef XENO_PC_PORT`). It now **enters and runs its full setup**.
- `pc_port/src/port_main.c`:
  - `*(int*)D_80010000 = -1` — restores the real static rodata value so `g_FieldSystemMode = SYSTEM_MODE_CD_ROM(1)` and the mode-0 `break 1` trap is skipped (the original control flow decides the mode; not a forced value).
  - `ArchiveInit(...,0)` kept (NOT D_80010000's -1): with -1 ArchiveInit skips the CD table/header read and expects an EXE-baked table the port doesn't migrate.

Reproduce: `cd pc_port/build_native && XENO_KERNEL_SEL=0 ./xeno-port`

## The frontier (start here next)
`FieldMain → GamePartySyncSkinData → GamePartyCharactersInitializeSkins (temp3.c:76) → HeapAlloc(~6 MB) FAILS → GameHandleError(130) hangs.`

Root cause: **`g_GameState` is a 16-byte stub, but the struct is 0x2300** and `partyMembers` is at offset `0x1D34`. `g_pGameState->partyMembers[]` reads OOB garbage → garbage party IDs → `ArchiveDecodeAlignedSize(garbage+5)` ≈ 6 MB.

Why 16 bytes: `tools/scripts/gen_port_stubs.py` sizes data stubs `max(elf_sym_size, 16)`; `g_GameState`'s ELF size is ≤16. The real `size:0x2300` is only in `config/symbol_addrs.slus_006.64.txt`.

### Next steps (in order)
1. **Size data stubs from `symbol_addrs` `size:` annotations** in `gen_port_stubs.py` (fixes `g_GameState`=0x2300 and other undersized structs; eliminates OOB reads). Port-infra fix; low risk; helps broadly.
2. **Initialize a valid game state / party** (new-game-style setup) so `partyMembers[]` hold real character IDs before entering the field — otherwise a zeroed `g_GameState` still yields member 0 / a wrong skin archive index.
3. Then oracle-iterate FieldMain's remaining ~33 stub callees in the order it reveals; bring up the field map/actor/render pipeline.

## Rules in effect
One reversible change at a time; stop at each frontier; no fake field load; no permanent hardcoded mode without proof.
