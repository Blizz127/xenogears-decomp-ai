# Xenogears Decompilation

Maintained by [Blizz127](https://github.com/Blizz127).
Published at [github.com/Blizz127/xenogears-decomp-ai](https://github.com/Blizz127/xenogears-decomp-ai).

This repository is a matching decompilation of Xenogears for the PlayStation, US release SLUS-00664, and the in-tree PC port that runs the recovered C.

<img src="https://i.imgur.com/FfAa7QA.png" />

## Original starter

This repository continues the original decompilation starter by ladysilverberg:

> This project is an in-progress matching decompilation of Xenogears for Playstation 1. The project currently targets the US release (SLUS 006.64), with the intention to target other releases as well down the road.
>
> — [ladysilverberg/xenogears-decomp](https://github.com/ladysilverberg/xenogears-decomp)

The splat layout, the US-disc target, and the thanks list below come from that starter. Matching work and the PC port after the fork are Blizz127's. The starter's build status and progress chart stay on that project: [actions](https://github.com/ladysilverberg/xenogears-decomp/actions/workflows/build.yaml), [decomp.dev](https://decomp.dev/ladysilverberg/xenogears-decomp). Its wiki is [here](https://github.com/ladysilverberg/xenogears-decomp/wiki).

## Thanks
This project leans on a lot of work done by others before.
-  [xenogears-decomp](https://github.com/ladysilverberg/xenogears-decomp) — the original starter this repository continues
-  [splat](https://github.com/ethteck/splat) - For splitting the binaries
-  [spimdiasm](https://github.com/Decompollaborate/spimdisasm) - For disassembling the binaries
-  [The silent hill decompilation](https://github.com/Vatuu/silent-hill-decomp/tree/master) - As a project structure template this project could be adapted from.
-  [MASPSX](https://github.com/mkst/maspsx) - For making parts of building less painful
-  [objdiff](https://github.com/encounter/objdiff) - For diffing
-  [decomp.dev](https://decomp.dev/) - For progress tracking
-  [decomp.me](https://decomp.me/) - For decompiling in collaboration

## Completion

There are two different numbers, and they measure different things.

**Accounted for: 100% of the in-scope targets.** `make rom-check` rebuilds from clean and checks every pinned
artifact against its retail SHA-256. All 10 pass (`rom-check: ALL PINNED ARTIFACTS MATCH RETAIL`):
the main executable `slus_006.64`, `field`, `menu`, `shop_menu`, `member_change_menu`, `battle`, `battling`,
`movie`, `battle_command_file1` and `world_map`. Every retail function is either byte-exact C or the
retail assembly (`INCLUDE_ASM`), and the linked result is byte-identical. The pins are never edited.

**Byte-exact matched C: 2,281 / 4,464 functions (51.1%), 323,920 / 1,383,684 code bytes (23.4%).**
This is the objdiff report (`make report`, then `tools/scripts/progress_dashboard.py`), built with
`-DSKIP_ASM`, so a function still carried as `INCLUDE_ASM` counts toward the total and not the matched
share. objdiff alone is not proof: it can score 100% on a function whose relocated stores are swapped,
so C only stays in the retail build when the whole binary still passes `make rom-check`.

| Target | Matched C functions | Matched C bytes | rom-check |
|---|--:|--:|:-:|
| Main executable (`slus_006.64`) | 883 / 1,223 | 100,508 / 218,072 | PASS |
| Field | 628 / 884 | 98,300 / 253,052 | PASS |
| Menu | 135 / 307 | 20,536 / 148,360 | PASS |
| Shop menu | 104 / 118 | 37,460 / 53,008 | PASS |
| Member-change menu | 67 / 67 | 24,936 / 24,936 | PASS |
| Battle | 292 / 857 | 27,672 / 332,316 | PASS |
| Battling | 126 / 483 | 9,116 / 139,692 | PASS |
| Battle command file 1 | 31 / 105 | 2,520 / 18,940 | PASS |
| Movie | 15 / 34 | 2,872 / 26,972 | PASS |
| World map | 0 / 386 | 0 / 168,336 | PASS |

Earlier versions of this section reported "2,557 matched C bodies (77.6%)" and "field 878/878". That
count treated any C body without `INCLUDE_ASM` as matched without checking its bytes, and several
hundred of those bodies did not match retail (field alone had 308). They are now `INCLUDE_ASM` in the
retail build, with the C kept for the port under `#ifdef XENO_PC_PORT`. The current campaign ledger,
with root causes, is in [docs/evidence/decomp-campaign/SCOPE_AND_LEDGER.md](docs/evidence/decomp-campaign/SCOPE_AND_LEDGER.md).

Longer notes on the game, the decompile, and the port are on the [wiki](https://github.com/Blizz127/xenogears-decomp-ai/wiki).

## Port status

The port compiles decompiled C with `-DXENO_PC_PORT -DSKIP_ASM` and links it to [PsyCross](https://github.com/OpenDriver2/PsyCross) (software GTE, LibGPU to OpenGL, LibSPU to OpenAL, LibCD to a disc image, controllers to SDL2). PsyCross itself is cloned locally and is not in this repository. Disc images and the BIOS are not either.

`pc_port/build_port.sh` is the native driver. On this tree it treats game translation units like this:

| | Count | What that means |
|---|--:|---|
| C files under `src/` | 275 | The decompilation |
| Psy-Q sources | 46 | Not linked. PsyCross plus `pc_port/src/psyq_compat.c` replace them |
| Intentionally excluded | 2 | `src/slus_006.64/system/archive.c` (see `archive_port.c`) and `src/battling/main.c` |
| Reference-only | 169 | Almost all of `src/battle`. Kept for the matching build. The port does not link these TUs |
| Known compile-failure allowlist | 0 | `KNOWN_BROKEN_GAME_TUS` is empty. Any other game-TU compile error aborts the build |
| Game TUs compiled into the port | 58 | Field, menus, shop, movie, member change, and the rest of the main executable |
| Port-only C files | 268 | `pc_port/src`, 79,689 lines. Checkpoints, movie player, battle adapter, boot, overrides |
| Port-owned symbol overrides | 22 | `pc_port/port_owned_overrides.txt`. Each row names the symbol and when it should retire |
| `INCLUDE_ASM` that also has a port C body | 129 | Menu 80, main executable 26, battle 17, shop 6 |
| `INCLUDE_ASM` omitted from the port compile | 608 | 506 of these are battle. The matching build still assembles them. The port has no body |
| `INCLUDE_ASM` in `src/battling` | 169 | Outside the 3,294-function split above |

Battle is the gap. The retail battle overlay is not linked as those 169 TUs. `pc_port/src/battle_mips_adapter.c` and `battle_overlay_host_leaves.inc` run some battle leaves as native C and leave the rest of `disc/battle.bin` to an interpreter. Field code is on the native side: matched C, walkmesh, script VM, and the routes exercised in this tree (including Blackmoon Forest). The world map is not that retail overlay.

A logging stub is generated at build time for a symbol the link still cannot find. Those stubs are not committed. They return without retail behavior. Byte-exact matching is not required for a port body. A function the port omits still needs a C body before that path can run.

### By overlay

Per-target matched-C numbers are in the Completion section above. The port-side view (which functions
the port compiles, overrides or omits) is regenerated by `python3 tools/scripts/decomp_status.py`; that
script counts C bodies, not verified matches.

The port architecture (platform interfaces, config file, mods and asset replacement, cheat console) is
described in [docs/port/ARCHITECTURE.md](docs/port/ARCHITECTURE.md). The port ships no game data: it reads
everything from your own disc image at runtime and checks it against known hashes on first run.
Known port-vs-retail behaviour differences are tracked in [docs/port/RETAIL_DIVERGENCES.md](docs/port/RETAIL_DIVERGENCES.md).


***

<a href="https://github.com/ladysilverberg/xenogears-decomp/wiki"><img src="https://i.imgur.com/0tvDzYB.png" /></a>

How the starter project is organized is written up in [its wiki](https://github.com/ladysilverberg/xenogears-decomp/wiki).
