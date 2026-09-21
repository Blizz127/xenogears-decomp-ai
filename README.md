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

Numbers below are from `tools/scripts/decomp_status.py` on this tree. The universe is every function with an `asm/<overlay>/{matchings,nonmatchings}` file. **Matched C** means a C body and no `INCLUDE_ASM`. **Coexistence** means the matching build still includes the retail assembly (so that build stays byte-exact) while the PC port has a real C body. **Unported** means a bare `INCLUDE_ASM` with no port body. Matched and coexistence together are done for the matching build.

Byte-exact matched C is the project convention (only a finished body is committed as plain C; near-matches stay on `INCLUDE_ASM`). This snapshot did not re-run objdiff. Regenerate with `python3 tools/scripts/decomp_status.py`. The in-repo gate `tools/scripts/test_decomp_complete.py` pins a universe of 3295; this split counted 3294.

| | Count |
|---|--:|
| Retail functions in the split | 3294 |
| Matched C bodies | 2557 (77.6%) |
| Coexistence (asm in the matching build, C in the port) | 737 (22.4%) |
| Unported | 0 |
| Done for the matching build | 3294 (100%) |
| Decomp C files under `src/` | 275 |
| Lines in `src/**/*.c` | 91,590 |
| PsyQ functions excluded from the port count | 348 |
| Functions the port classifies as real | 2946 |
| Oracle stubs (`pc_port/build_native/stubs.c`) | not in this tree (0 reported) |
| Port C files under `pc_port/src/` | 268 |
| Lines in `pc_port/src/**/*.c` | 79,689 |

From here the decompilation sources under `src/` are the baseline. Further commits are the PC port under `pc_port/`.

### By overlay

| Overlay | Functions | Matched C | Coexistence | Unported | Matched C share | Done |
|---|--:|--:|--:|--:|--:|--:|
| battle | 814 | 291 | 523 | 0 | 35.7% | 100% |
| field | 878 | 878 | 0 | 0 | 100% | 100% |
| member_change_menu | 67 | 67 | 0 | 0 | 100% | 100% |
| menu | 312 | 173 | 139 | 0 | 55.4% | 100% |
| shop_menu | 120 | 106 | 14 | 0 | 88.3% | 100% |
| slus_006.64 (main executable) | 1103 | 1042 | 61 | 0 | 94.5% | 100% |
| **Total** | **3294** | **2557** | **737** | **0** | **77.6%** | **100%** |

These configs exist beside that function universe and are mostly data or a few controller units, not rows in the table above: `battle_command_file1`, `battling`, `movie`, `world_map`.

### Every module

<details>
<summary>257 translation units</summary>

| Overlay / module | Functions | Matched C | Coexistence | Unported | Done |
|---|--:|--:|--:|--:|--:|
| `battle/main` | 8 | 3 | 5 | 0 | 100.0% |
| `battle/main10` | 3 | 1 | 2 | 0 | 100.0% |
| `battle/main101` | 4 | 1 | 3 | 0 | 100.0% |
| `battle/main103` | 2 | 1 | 1 | 0 | 100.0% |
| `battle/main105` | 1 | 1 | 0 | 0 | 100.0% |
| `battle/main106` | 3 | 1 | 2 | 0 | 100.0% |
| `battle/main11` | 2 | 2 | 0 | 0 | 100.0% |
| `battle/main111` | 8 | 1 | 7 | 0 | 100.0% |
| `battle/main119` | 1 | 1 | 0 | 0 | 100.0% |
| `battle/main12` | 9 | 1 | 8 | 0 | 100.0% |
| `battle/main121` | 1 | 1 | 0 | 0 | 100.0% |
| `battle/main125_q1` | 1 | 0 | 1 | 0 | 100.0% |
| `battle/main125_q2` | 1 | 1 | 0 | 0 | 100.0% |
| `battle/main128` | 3 | 3 | 0 | 0 | 100.0% |
| `battle/main13` | 7 | 3 | 4 | 0 | 100.0% |
| `battle/main135` | 3 | 0 | 3 | 0 | 100.0% |
| `battle/main14` | 7 | 7 | 0 | 0 | 100.0% |
| `battle/main16` | 1 | 1 | 0 | 0 | 100.0% |
| `battle/main17_q1` | 2 | 2 | 0 | 0 | 100.0% |
| `battle/main17_q2` | 5 | 5 | 0 | 0 | 100.0% |
| `battle/main19_q1` | 2 | 2 | 0 | 0 | 100.0% |
| `battle/main19_q2` | 10 | 10 | 0 | 0 | 100.0% |
| `battle/main2` | 33 | 3 | 30 | 0 | 100.0% |
| `battle/main20` | 5 | 5 | 0 | 0 | 100.0% |
| `battle/main21` | 9 | 3 | 6 | 0 | 100.0% |
| `battle/main22_q1` | 2 | 2 | 0 | 0 | 100.0% |
| `battle/main22_q2` | 2 | 2 | 0 | 0 | 100.0% |
| `battle/main23` | 4 | 4 | 0 | 0 | 100.0% |
| `battle/main24` | 11 | 4 | 7 | 0 | 100.0% |
| `battle/main26` | 2 | 1 | 1 | 0 | 100.0% |
| `battle/main27` | 2 | 2 | 0 | 0 | 100.0% |
| `battle/main28` | 13 | 1 | 12 | 0 | 100.0% |
| `battle/main3` | 4 | 1 | 3 | 0 | 100.0% |
| `battle/main30` | 29 | 28 | 1 | 0 | 100.0% |
| `battle/main31` | 5 | 1 | 4 | 0 | 100.0% |
| `battle/main32` | 2 | 2 | 0 | 0 | 100.0% |
| `battle/main33` | 4 | 1 | 3 | 0 | 100.0% |
| `battle/main34_q1` | 1 | 0 | 1 | 0 | 100.0% |
| `battle/main34_q2` | 3 | 3 | 0 | 0 | 100.0% |
| `battle/main35_p1` | 2 | 1 | 1 | 0 | 100.0% |
| `battle/main35_p2` | 22 | 2 | 20 | 0 | 100.0% |
| `battle/main35_p3_q1` | 1 | 0 | 1 | 0 | 100.0% |
| `battle/main35_p3_q2` | 1 | 1 | 0 | 0 | 100.0% |
| `battle/main35_p4` | 5 | 2 | 3 | 0 | 100.0% |
| `battle/main36_p2` | 3 | 2 | 1 | 0 | 100.0% |
| `battle/main37_q1` | 1 | 1 | 0 | 0 | 100.0% |
| `battle/main37_q2` | 1 | 1 | 0 | 0 | 100.0% |
| `battle/main38_p2` | 7 | 1 | 6 | 0 | 100.0% |
| `battle/main39` | 7 | 1 | 6 | 0 | 100.0% |
| `battle/main4` | 3 | 3 | 0 | 0 | 100.0% |
| `battle/main40_p2` | 10 | 1 | 9 | 0 | 100.0% |
| `battle/main41` | 7 | 6 | 1 | 0 | 100.0% |
| `battle/main42` | 3 | 1 | 2 | 0 | 100.0% |
| `battle/main43` | 5 | 3 | 2 | 0 | 100.0% |
| `battle/main44` | 5 | 5 | 0 | 0 | 100.0% |
| `battle/main45` | 4 | 2 | 2 | 0 | 100.0% |
| `battle/main46` | 5 | 2 | 3 | 0 | 100.0% |
| `battle/main47` | 4 | 2 | 2 | 0 | 100.0% |
| `battle/main48` | 4 | 2 | 2 | 0 | 100.0% |
| `battle/main49` | 12 | 1 | 11 | 0 | 100.0% |
| `battle/main5` | 5 | 5 | 0 | 0 | 100.0% |
| `battle/main50` | 3 | 1 | 2 | 0 | 100.0% |
| `battle/main51` | 47 | 1 | 46 | 0 | 100.0% |
| `battle/main52` | 2 | 1 | 1 | 0 | 100.0% |
| `battle/main53` | 11 | 1 | 10 | 0 | 100.0% |
| `battle/main54` | 5 | 1 | 4 | 0 | 100.0% |
| `battle/main55` | 5 | 2 | 3 | 0 | 100.0% |
| `battle/main56` | 4 | 1 | 3 | 0 | 100.0% |
| `battle/main57` | 2 | 1 | 1 | 0 | 100.0% |
| `battle/main58` | 9 | 1 | 8 | 0 | 100.0% |
| `battle/main59` | 6 | 1 | 5 | 0 | 100.0% |
| `battle/main6` | 9 | 2 | 7 | 0 | 100.0% |
| `battle/main60` | 2 | 1 | 1 | 0 | 100.0% |
| `battle/main61` | 6 | 1 | 5 | 0 | 100.0% |
| `battle/main62` | 4 | 1 | 3 | 0 | 100.0% |
| `battle/main63` | 3 | 1 | 2 | 0 | 100.0% |
| `battle/main64` | 9 | 1 | 8 | 0 | 100.0% |
| `battle/main65` | 10 | 3 | 7 | 0 | 100.0% |
| `battle/main66` | 9 | 2 | 7 | 0 | 100.0% |
| `battle/main67` | 6 | 1 | 5 | 0 | 100.0% |
| `battle/main68` | 18 | 2 | 16 | 0 | 100.0% |
| `battle/main69` | 7 | 1 | 6 | 0 | 100.0% |
| `battle/main7` | 2 | 1 | 1 | 0 | 100.0% |
| `battle/main70` | 27 | 3 | 24 | 0 | 100.0% |
| `battle/main71` | 2 | 2 | 0 | 0 | 100.0% |
| `battle/main72_p1` | 1 | 1 | 0 | 0 | 100.0% |
| `battle/main72_p2` | 6 | 0 | 6 | 0 | 100.0% |
| `battle/main72_p3` | 2 | 1 | 1 | 0 | 100.0% |
| `battle/main73_p1` | 5 | 0 | 5 | 0 | 100.0% |
| `battle/main73_p2` | 10 | 2 | 8 | 0 | 100.0% |
| `battle/main74` | 11 | 1 | 10 | 0 | 100.0% |
| `battle/main75` | 3 | 1 | 2 | 0 | 100.0% |
| `battle/main77` | 7 | 2 | 5 | 0 | 100.0% |
| `battle/main8` | 10 | 2 | 8 | 0 | 100.0% |
| `battle/main87` | 8 | 1 | 7 | 0 | 100.0% |
| `battle/main9` | 5 | 3 | 2 | 0 | 100.0% |
| `battle/main90` | 5 | 2 | 3 | 0 | 100.0% |
| `battle/main91` | 2 | 2 | 0 | 0 | 100.0% |
| `battle/main93` | 2 | 2 | 0 | 0 | 100.0% |
| `battle/main96` | 3 | 2 | 1 | 0 | 100.0% |
| `battle/mainc100` | 5 | 3 | 2 | 0 | 100.0% |
| `battle/mainc102` | 4 | 2 | 2 | 0 | 100.0% |
| `battle/mainc104` | 2 | 1 | 1 | 0 | 100.0% |
| `battle/mainc107` | 2 | 1 | 1 | 0 | 100.0% |
| `battle/mainc108` | 3 | 1 | 2 | 0 | 100.0% |
| `battle/mainc112` | 9 | 1 | 8 | 0 | 100.0% |
| `battle/mainc113` | 3 | 1 | 2 | 0 | 100.0% |
| `battle/mainc114_p1` | 1 | 0 | 1 | 0 | 100.0% |
| `battle/mainc114_p2` | 9 | 3 | 6 | 0 | 100.0% |
| `battle/mainc115` | 1 | 1 | 0 | 0 | 100.0% |
| `battle/mainc116` | 2 | 2 | 0 | 0 | 100.0% |
| `battle/mainc117` | 3 | 1 | 2 | 0 | 100.0% |
| `battle/mainc118` | 17 | 3 | 14 | 0 | 100.0% |
| `battle/mainc120` | 5 | 2 | 3 | 0 | 100.0% |
| `battle/mainc122` | 8 | 2 | 6 | 0 | 100.0% |
| `battle/mainc123` | 2 | 1 | 1 | 0 | 100.0% |
| `battle/mainc124` | 2 | 1 | 1 | 0 | 100.0% |
| `battle/mainc126` | 5 | 2 | 3 | 0 | 100.0% |
| `battle/mainc127` | 4 | 2 | 2 | 0 | 100.0% |
| `battle/mainc129` | 1 | 1 | 0 | 0 | 100.0% |
| `battle/mainc130` | 2 | 2 | 0 | 0 | 100.0% |
| `battle/mainc131` | 5 | 3 | 2 | 0 | 100.0% |
| `battle/mainc132` | 2 | 1 | 1 | 0 | 100.0% |
| `battle/mainc134` | 11 | 1 | 10 | 0 | 100.0% |
| `battle/mainc15` | 2 | 2 | 0 | 0 | 100.0% |
| `battle/mainc18_q1` | 2 | 2 | 0 | 0 | 100.0% |
| `battle/mainc18_q2` | 5 | 5 | 0 | 0 | 100.0% |
| `battle/mainc25_q1` | 8 | 0 | 8 | 0 | 100.0% |
| `battle/mainc25_q2` | 1 | 1 | 0 | 0 | 100.0% |
| `battle/mainc29_q1` | 2 | 2 | 0 | 0 | 100.0% |
| `battle/mainc29_q2` | 2 | 2 | 0 | 0 | 100.0% |
| `battle/mainc76` | 2 | 2 | 0 | 0 | 100.0% |
| `battle/mainc78` | 3 | 2 | 1 | 0 | 100.0% |
| `battle/mainc79` | 3 | 1 | 2 | 0 | 100.0% |
| `battle/mainc80` | 2 | 1 | 1 | 0 | 100.0% |
| `battle/mainc82` | 2 | 1 | 1 | 0 | 100.0% |
| `battle/mainc83` | 10 | 1 | 9 | 0 | 100.0% |
| `battle/mainc84` | 4 | 1 | 3 | 0 | 100.0% |
| `battle/mainc85` | 4 | 1 | 3 | 0 | 100.0% |
| `battle/mainc86` | 2 | 1 | 1 | 0 | 100.0% |
| `battle/mainc88` | 2 | 2 | 0 | 0 | 100.0% |
| `battle/mainc89` | 2 | 1 | 1 | 0 | 100.0% |
| `battle/mainc95` | 3 | 1 | 2 | 0 | 100.0% |
| `battle/mainc97` | 1 | 1 | 0 | 0 | 100.0% |
| `battle/mainc99` | 3 | 1 | 2 | 0 | 100.0% |
| `battle/mainl109` | 1 | 1 | 0 | 0 | 100.0% |
| `battle/mainl110` | 8 | 1 | 7 | 0 | 100.0% |
| `battle/mainl115` | 2 | 2 | 0 | 0 | 100.0% |
| `battle/mainl133` | 2 | 1 | 1 | 0 | 100.0% |
| `battle/mainl81` | 2 | 1 | 1 | 0 | 100.0% |
| `battle/mainl92` | 3 | 2 | 1 | 0 | 100.0% |
| `battle/mainl94` | 2 | 2 | 0 | 0 | 100.0% |
| `battle/mainl98` | 1 | 1 | 0 | 0 | 100.0% |
| `field/camera/camera_movement` | 30 | 30 | 0 | 0 | 100.0% |
| `field/dialogue/text_box` | 19 | 19 | 0 | 0 | 100.0% |
| `field/dialogue/text_box_render` | 17 | 17 | 0 | 0 | 100.0% |
| `field/effects/distortion` | 4 | 4 | 0 | 0 | 100.0% |
| `field/effects/fade` | 6 | 6 | 0 | 0 | 100.0% |
| `field/effects/fade_render` | 2 | 2 | 0 | 0 | 100.0% |
| `field/effects/particles` | 15 | 15 | 0 | 0 | 100.0% |
| `field/game_logic/gold` | 3 | 3 | 0 | 0 | 100.0% |
| `field/game_logic/scenario_flags` | 5 | 5 | 0 | 0 | 100.0% |
| `field/main/init` | 3 | 3 | 0 | 0 | 100.0% |
| `field/main/input_script_handlers` | 7 | 7 | 0 | 0 | 100.0% |
| `field/main/main` | 14 | 14 | 0 | 0 | 100.0% |
| `field/main/misc` | 198 | 198 | 0 | 0 | 100.0% |
| `field/main/misc10` | 9 | 9 | 0 | 0 | 100.0% |
| `field/main/misc11` | 102 | 102 | 0 | 0 | 100.0% |
| `field/main/misc2` | 44 | 44 | 0 | 0 | 100.0% |
| `field/main/misc3` | 11 | 11 | 0 | 0 | 100.0% |
| `field/main/misc4` | 42 | 42 | 0 | 0 | 100.0% |
| `field/main/misc5` | 36 | 36 | 0 | 0 | 100.0% |
| `field/main/misc6` | 63 | 63 | 0 | 0 | 100.0% |
| `field/main/misc7` | 91 | 91 | 0 | 0 | 100.0% |
| `field/main/misc8` | 58 | 58 | 0 | 0 | 100.0% |
| `field/main/misc9` | 27 | 27 | 0 | 0 | 100.0% |
| `field/party/stats` | 16 | 16 | 0 | 0 | 100.0% |
| `field/scripts/variable_handlers` | 27 | 27 | 0 | 0 | 100.0% |
| `field/scripts/virtual_machine` | 29 | 29 | 0 | 0 | 100.0% |
| `member_change_menu/main/misc` | 67 | 67 | 0 | 0 | 100.0% |
| `menu/main/misc` | 312 | 173 | 139 | 0 | 100.0% |
| `shop_menu/main/misc` | 18 | 18 | 0 | 0 | 100.0% |
| `shop_menu/main/misc2` | 66 | 65 | 1 | 0 | 100.0% |
| `shop_menu/main/misc3` | 2 | 1 | 1 | 0 | 100.0% |
| `shop_menu/main/misc4` | 2 | 1 | 1 | 0 | 100.0% |
| `shop_menu/main/misc5` | 5 | 4 | 1 | 0 | 100.0% |
| `shop_menu/main/misc6` | 7 | 6 | 1 | 0 | 100.0% |
| `shop_menu/main/misc7` | 3 | 1 | 2 | 0 | 100.0% |
| `shop_menu/main/misc8` | 5 | 3 | 2 | 0 | 100.0% |
| `shop_menu/main/misc9` | 12 | 7 | 5 | 0 | 100.0% |
| `slus_006.64/graphics/line_scroll` | 4 | 4 | 0 | 0 | 100.0% |
| `slus_006.64/main/main` | 1 | 1 | 0 | 0 | 100.0% |
| `slus_006.64/main/main_loop` | 9 | 9 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libapi` | 25 | 25 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libapi/WaitEvent` | 1 | 1 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libapi_2` | 8 | 8 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libapi_3` | 1 | 1 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libc` | 12 | 11 | 1 | 0 | 100.0% |
| `slus_006.64/psyq/libc2/puts` | 4 | 4 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libcard` | 7 | 7 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libcd/bios` | 14 | 14 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libcd/cdread` | 8 | 8 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libcd/event` | 4 | 4 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libcd/sys` | 23 | 23 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libetc/intr` | 16 | 16 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libetc/intr_dma` | 4 | 3 | 1 | 0 | 100.0% |
| `slus_006.64/psyq/libetc/intr_vsync` | 4 | 4 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libetc/video_mode` | 2 | 2 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libetc/vsync` | 2 | 2 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libgpu` | 66 | 66 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libgte` | 96 | 96 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libsn` | 8 | 8 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libspu/Spu` | 15 | 15 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libspu/SpuClearReverbWorkArea` | 1 | 1 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libspu/SpuDataCallback` | 1 | 1 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libspu/SpuGetReverbModeType` | 1 | 1 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libspu/SpuGetVoiceEnvelopeAttr` | 1 | 1 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libspu/SpuInit` | 1 | 1 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libspu/SpuInitMalloc` | 1 | 1 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libspu/SpuQuit` | 1 | 1 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libspu/SpuRead` | 1 | 1 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libspu/SpuReadDecodedData` | 1 | 1 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libspu/SpuSetCommonAttr` | 1 | 1 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libspu/SpuSetIRQ` | 1 | 1 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libspu/SpuSetIRQCallback` | 1 | 1 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libspu/SpuSetNoiseClock` | 1 | 1 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libspu/SpuSetReverb` | 1 | 1 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libspu/SpuSetReverbModeDelayTime` | 1 | 1 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libspu/SpuSetReverbModeDepth` | 1 | 1 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libspu/SpuSetReverbModeFeedback` | 1 | 1 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libspu/SpuSetReverbModeType` | 1 | 1 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libspu/SpuSetTransferCallback` | 1 | 1 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libspu/SpuSetTransferMode` | 1 | 1 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libspu/SpuSetTransferStartAddr` | 1 | 1 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libspu/SpuWrite` | 1 | 1 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libspu/_SpuCallback` | 1 | 1 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libspu/_SpuInit` | 2 | 2 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libspu/_SpuIsInAllocateArea` | 2 | 2 | 0 | 0 | 100.0% |
| `slus_006.64/psyq/libspu/_spu_setReverbAttr` | 1 | 1 | 0 | 0 | 100.0% |
| `slus_006.64/system/animation_scripts` | 34 | 34 | 0 | 0 | 100.0% |
| `slus_006.64/system/archive` | 11 | 11 | 0 | 0 | 100.0% |
| `slus_006.64/system/asset_loader` | 1 | 1 | 0 | 0 | 100.0% |
| `slus_006.64/system/controller` | 11 | 11 | 0 | 0 | 100.0% |
| `slus_006.64/system/font` | 28 | 28 | 0 | 0 | 100.0% |
| `slus_006.64/system/graphics` | 2 | 2 | 0 | 0 | 100.0% |
| `slus_006.64/system/heap_debug` | 3 | 3 | 0 | 0 | 100.0% |
| `slus_006.64/system/kernel_menu` | 4 | 4 | 0 | 0 | 100.0% |
| `slus_006.64/system/libarchive` | 27 | 27 | 0 | 0 | 100.0% |
| `slus_006.64/system/memory` | 38 | 38 | 0 | 0 | 100.0% |
| `slus_006.64/system/menu` | 8 | 8 | 0 | 0 | 100.0% |
| `slus_006.64/system/rendering` | 20 | 17 | 3 | 0 | 100.0% |
| `slus_006.64/system/sound` | 271 | 244 | 27 | 0 | 100.0% |
| `slus_006.64/system/system` | 54 | 53 | 1 | 0 | 100.0% |
| `slus_006.64/system/temp1` | 71 | 64 | 7 | 0 | 100.0% |
| `slus_006.64/system/temp2` | 102 | 81 | 21 | 0 | 100.0% |
| `slus_006.64/system/temp3` | 27 | 27 | 0 | 0 | 100.0% |
| `slus_006.64/system/work_list` | 29 | 29 | 0 | 0 | 100.0% |

</details>


***

<a href="https://github.com/ladysilverberg/xenogears-decomp/wiki"><img src="https://i.imgur.com/0tvDzYB.png" /></a>

How the starter project is organized is written up in [its wiki](https://github.com/ladysilverberg/xenogears-decomp/wiki).
