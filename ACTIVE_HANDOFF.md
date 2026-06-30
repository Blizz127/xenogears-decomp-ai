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

## DONE since: stub sizing (commit ffe5b5a)
`gen_port_stubs.py` now sizes data stubs from `symbol_addrs` `size:` annotations
(`max(elf_size, symaddr_size, 16)`); `build_port.sh` passes the four symbol_addrs
files. 10 stubs resized, `g_GameState` → 0x2300, so `partyMembers` (offset 0x1D34)
no longer reads out of bounds. Necessary infra fix, broadly useful — but it did
not by itself unblock the field (the alloc size below was the same before/after,
because the OOB read happened to land on zeroed memory).

## The frontier (start here next): game-state / party + archive-context init
`FieldMain → GamePartySyncSkinData → GamePartyCharactersInitializeSkins (temp3.c:76)
→ HeapAlloc(6143680 ≈ 6 MB) FAILS → GameHandleError(130) hangs.`

The KernelMenu→Field debug path enters with a **zeroed g_GameState** and **no archive
directory set**, neither of which the normal new-game/save-load entry would leave:
- `partyMembers[]` are all 0 → every slot treated as character 0 (CHARACTER_ID_NONE=0xFF).
- `ArchiveDecodeAlignedSize(member0 + 5)` → `ArchiveDecodeSize(5)` reads
  `g_ArchiveTable[(5 + g_CurArchiveOffset − 1) * 7]`. `g_CurArchiveOffset` is not the
  party-skin directory (the normal flow sets it via `ArchiveSetIndex`), so it reads
  the wrong table entry → 6 MB → alloc fail.

### Next steps (do NOT fake party IDs — fix the underlying setup)
1. Find where the real game initializes the game state / party + selects the
   party-skin archive directory before the field runs (new-game path / the field
   debug entry's own setup). Port/invoke that so `g_GameState.partyMembers[]` and
   `g_CurArchiveOffset` are valid.
2. Then oracle-iterate FieldMain's remaining ~33 stub callees in the order it
   reveals; bring up the field map/actor/render pipeline.

## Rules in effect
One reversible change at a time; stop at each frontier; no fake field load; no permanent hardcoded mode without proof.
