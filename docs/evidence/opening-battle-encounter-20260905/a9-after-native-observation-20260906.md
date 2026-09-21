# Native A9 later-battle replay — bounded result

## Result

**BOUNDARY REACHED: field 13. LATER BATTLE NOT REACHED. OPCODE 0xA9 NOT OBSERVED.**

The isolated native replay naturally completed the opening battle and returned to field 14. Ordinary movement then reached field 13 and advanced the mandatory Dan/Timothy dialogue through multiple visibly different lines until the textbox disappeared. The run did not reach field 1, the world map, or the later battle before its 900-second bound. Therefore this run supplies no runtime execution proof for the repaired sprite opcode `0xA9`.

This is native-port observation with null audio. It is not retail emulator or retail-pixel proof.

## Ownership and immutable pins

- Run: `/var/home/blizz/Projects/xenogears-decomp-ai/pc_port/build_native/a9-later-battle-virtual-y6iwd77a`
- Scratch driver: `/tmp/xeno-a9-later-battle-replay-20260906/virtual-replay.py`
- Isolated display: `:2`
- Owned Xvfb PID: `4152558`
- Owned GDB PID: `4152568`
- Owned game PID: `4152589`, window `6291508`
- Copied binary SHA-256: `f4dcc43c404d453861429e80a017a5c4bda2ff355f85829d359276c274743ec6`
- Compiled `system.c` SHA-256: `e1dbf43e5ecb9ca41c344e92e5109e92324ae3e4d7028c691e0575f3535f41bd`
- Compiled `animation_scripts.c` SHA-256: `8e5de18aa4c657cea9026626b4dd9a0d92f104788a4438024fa7a10c910bec5c`
- Archived build manifest SHA-256: `2f399d681112f13414cb73261f5b35f30bd2de769fc03f0c8bb72b5d82f19932`
- Driver SHA-256: `c05cdb5aed2e9cbc6db135ed33c9bf60c24448431caecee23e6d3bbf499d75f2`
- Run-specific GDB trace SHA-256: `c2c79ea6a843337104369fa8375b9052acbf357d2997b5bf0b9988accfe19bee`
- 700-pulse schedule SHA-256: `2a888551133333b637ff493412b43bf2794059e87cf4d802bdd1df8e67c65520`

## Natural milestones

- Started: `2026-09-06T05:14:18.213256Z`.
- Ordinary New Game input: `05:14:41.988366Z`.
- Field entries captured by the sparse debugger: field 490 at `05:14:37.900095Z`, field 4 at `05:14:44.583355Z`, and field 14 after the battle at `05:15:44.845843Z`.
- Opening battle entered at `05:15:20.583613Z` and returned at `05:15:44.833253Z` after ordinary `z`/Circle confirmations.
- Native log records `FieldLoad begin field=13` at `run.log:1959`. Field 13 was also visually captured. There is no field 1 load in the retained log.
- The exact `0xA9` case-only breakpoint at copied-binary address `0x4cac66` did not fire. `a9.jsonl` was not created.
- Owned stop at `05:29:18.348676Z`; `exit.json` records GDB return code 0, first battle returned true, and A9 return observed false.

## Ordinary route and input findings

- Five-times host speed was used only to shorten automatic presentation/dialogue periods; traversal was visually normalized to 1x.
- In the painting room, `Left+Up` followed by `x`/Square jump cleared the easel/furniture obstruction.
- The first upper route led to a storage/barrel dead end. Two ordinary `Shift_R`/R1 camera rotations exposed the route needed to backtrack and descend the stairs.
- Moving down from the stairs loaded field 13. Continuing toward the front exit triggered the required Dan/Timothy dialogue.
- Ordinary `z` advanced the dialogue: retained captures show different dialogue stages, and the final field image has no textbox. This run does not support a stuck-dialogue or confirm-mapping defect.
- Further `Down`, `Down+Left`, and `Left` movement remained in field 13 when the time bound expired. The filenames `nav-28-village-check.png`, `nav-36-village.png`, `nav-37-village-check.png`, and `nav-38-village-check.png` describe attempted destinations; they are not evidence that field 1 was reached.

All ordinary inputs and UTC timestamps are retained in `actions.jsonl` and `navigation-actions.jsonl`.

## Evidence

- `actions.jsonl` SHA-256: `62f65b3bc6f2fe4d6af98d299fd71fb95a6138ec262f8d195c7f8177ea0fdfab`
- `navigation-actions.jsonl` SHA-256: `3ba1e2a218ef0416b4392607f45bf8af8aa8f9f0dda2a09555c4c6ecedcbacd3`
- `battle-events.jsonl`: opening battle entry and natural return.
- `field-events.jsonl`: fields 490, 4, and 14; `run.log` supplies the later field 13 load.
- `run.log` SHA-256: `4e0d21b79f7b48018d6ffd10f0c2d8eb279459eb0dd6e118eecd90a164c25a8a`
- First field-13 visual, `nav-27-hall13-check.png`, SHA-256: `e91e3e88deb1f5b7a3440ecfea982916d5ff8c5d7ca9accd819969076d38e08e`
- Mandatory-dialogue visual, `nav-35-dialogue.png`, SHA-256: `093c2178b68fe028cd806847f3bf0c906de6d2e88a3419f0a1a054899257cff1`
- Last manual visual, `nav-38-village-check.png`, SHA-256: `cd7c56006d2055646e2590b9a3a8bf25bdd97b271081367490b2e2e392ee52dc`; despite its filename it still shows field 13.
- Last captured framebuffer converted from the driver's BMP payload to PNG, `last-field-frame-converted.png`, SHA-256: `709dc8462b78ab8187f8460f0d250101cd27c335379a1f2ec52266e92acd47eb`. It visibly shows the field-13 gathering area without a textbox.

## Termination and preservation

The driver reached its planned 900-second limit and sent SIGTERM only to the owned game process. `failure.jsonl` is the GDB signal-hook record of that planned termination at `Vsync`; it is not a spontaneous runtime failure. Its read-only RAM capture hashes to `c51221c84c3044c8d8df1ed1fc5c2e9fbfde3d4a26420e8cea0908a91fe86a8c`.

Owned PIDs `4152558`, `4152568`, and `4152589` are all absent. No unrelated display or process was touched. The pre-run root application log was retained as `previous-game.log` and restored unchanged at SHA-256 `4abc6faa0b9549ca84bb28e303614a62b1f85ebee7d5d1a3cfe6c09280c8edae`; the owned run log was preserved separately as `owned-game.log` SHA-256 `86d4c1cd6ff90fd5ae6ccdaeba5d9c2dcb864385d6d9d2ef13fe9dad6d8bf7b6`.

The next runtime attempt should resume the ordinary field-13 route with enough budget to locate the village exit, then traverse field 1 and the world map. The sparse exact-case A9 breakpoint can be reused unchanged; this attempt did not reach it.
