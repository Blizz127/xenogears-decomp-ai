# DeepSeek continuation prompt (Xenogears matching + port)

Paste this as `--prompt`. Start a **new** DeepCode session in
`/var/home/blizz/Projects/xenogears-decomp-ai`. Do **not** `--last` / `--resume`
session `c102a918-709e-47ef-9e9d-c27df16823ba`: that session's last message is
stale, and it ended with context exhausted (~330k active tokens).

If using the wrapper: `MAX_ROUNDS=25 ./xeno-deepseek-loop.sh` (it now reads this
file). The wrapper still uses `--last`; for a clean restart, unset that by
running DeepCode directly as below.

```bash
cd /var/home/blizz/Projects/xenogears-decomp-ai
deepcode -p "$(cat docs/ai_context/DEEPSEEK_CONTINUE.md)"
# or one-shot:
# deepcode -x -p "$(sed -n '/^```prompt$/,/^```$/p' docs/ai_context/DEEPSEEK_CONTINUE.md | sed '1d;$d')"
```

The block to paste into DeepCode is everything between the `prompt` fences.

```prompt
Continue the Xenogears matching decompile and native pc_port in this checkout. This is a continuation, not a status summary. Do not touch the UE5 tree.

CURRENT TREE (verify before acting; do not trust the last DeepSeek chat turn):
- Session c102a918 ended 2026-09-10 23:29Z with tbl3 still null. That is STALE.
- src/field/main/misc4.c already has case 7 on func_8007BEF4, func_8007C694, func_8007CD80, and func_8007D3D4. Retail func_8007CD80.s DOES contain `sltiu $v0, $a1, 0x8` at 0x8007CF94/0x8007CF9C; $a1 is sideMask; index 7 is `addiu $a3, $zero, -1` = nextTri = -1. Do not re-add those cases.
- src/field/main/misc8.c func_80080A74 already dropped `s16 stateBuf[0x34] = { 0 }`. Keep that. The remaining gap is the function body ~1128 B vs retail 1232 B (−104). Do not re-add the zero template to fake field_RODATA_SIZE.
- Field has 0 INCLUDE_ASM( macros. slus has 74. battle ~614. menu 139. shop 14. battling 169. member_change/movie 0.
- Pins in config/checksum.sha: member_change 3b9e2b89… PASS, shop 7890e14b… PASS, battle 1830b4ef… PASS. slus dc0b2dd7… FAIL (built ~61b675d2… size 303068 vs retail 303104). field 38a1ce82… FAIL (built ~761df27a… size 249290 vs retail 260862). NEVER re-pin checksum.sha.
- Native ./pc_port/build_port.sh last LINK OK, 72 function stubs, xeno-port in pc_port/build_native/. Map0 smoke target is RUN_RC=124, primSubmits=2. Live path still hits [stub] gte_RotTransPers (PsyCross has RotTransPers; do not invent a game-logic stub body).
- Working tree is dirty and uncommitted on purpose. No commits, pushes, resets, cleans, or overwrites of unrelated work.
- Matching rebuilds: podman image localhost/xenogears-dev-toolchain:current, mount this repo at a path containing xenogears-decomp, TMPDIR=/var/tmp (not /tmp). splat 0.33.2 / spimdisasm 1.33.0. Battle CDK bodies use tools/gcc-2.7.2-cdk-psx + --dont-expand-li and must stay on the BattleCdk preset.

NEXT TASK QUEUE (do the first item that is still unfinished after you grep the tree; one function or one switch-case per pass):
1. func_80080A74 (−104 vs retail). Read asm/field/matchings/main/misc8/func_80080A74.s. Recover missing stores/control-flow from the listing. Do not zero-init stateBuf. Acceptance: function size moves toward 1232 B AND field_RODATA_SIZE stays 0x2FC AND member/shop/battle pins still PASS.
2. misc4 table-4 entry-order permutation. Built jtbl ranks were (0,2,6,1,5,3,4,7) vs retail (0,3,5,1,6,2,4,7). Fix case→body assignment from the retail .s; do not pad dummy cases.
3. Remaining slus host-compilable bodies, smallest first, from src/slus_006.64 (sound.c / temp2.c / temp1.c). Handoff names still open: func_8002BB50, func_8002AC24, func_80019578, func_8001A6E8, func_80023B84. Owed differentials: func_800257F0, func_80024730. Do not relitigate the 0x4CC slus rodata-layout drift in one pass.
4. Battle CDK leftovers only if (1)–(3) are blocked. Use tools/scripts/gen_battle_tus.py + BattleCdk. Array-declaration rule for lui-kept globals. Require battle.bin stays 1830b4ef…. Park bodies that still fold to $at after that rule.
5. Port stub retirement only with verified C on the live path. Do not replace a stub with a silent no-op. 72 stubs are mostly menu/shop/world-map 0x801E/0x8028 plus func_80028F30 stream decode.

GATES for every kept change:
- Cite retail .s range (and instruction) in the evidence log / ACTIVE_HANDOFF.md.
- Matching TU via the real gcc 2.6/2.7 + maspsx pipeline inside the toolchain image. Size-only or hash-churn without an asm-backed body is not progress.
- make rom-check / tools/scripts/check_rom_hashes.sh: green pins stay PASS; slus/field may stay FAIL only if the touched function moved toward retail.
- If you land a C body, add or extend pc_port/tests/run_*_retail_test.sh (O0/O2/UBSan + at least one mutant). Do not mock the unit under test.
- Native ./pc_port/build_port.sh still LINK OK. Prefer Map0 offscreen smoke twice (RUN_RC=124) when the launcher can start; if it cannot, record that and keep matching+unit tests as the bar. Do not fabricate screenshots.
- Update docs/ai_context/ACTIVE_HANDOFF.md with: change, evidence, remaining uncertainty, next executable step.

RULES:
- Transcribe from retail SLUS-006.64 asm or a live gdb trace that agrees with that asm. Unverified tails stay INCLUDE_ASM or a named fail-loud assert. No invented gap-fill.
- Matching C and port C stay the same function unless an evidence-backed XENO_PC_PORT / labeled shim is required. Do not force a shared macro that changes port behavior.
- PSX-embedded pointers stay u32. Do not dereference 0x1F800000 on the host.
- Function-size convergence and an empty call-divergence set do not prove byte-exact matching. Runtime fidelity needs runtime evidence, not merely LINK OK.
- Do not modify xeno-deepseek-loop.sh, its logs, or its stop file.

End every checkpoint with exactly one of:
RUNNER_STATUS=CONTINUE
RUNNER_STATUS=NEEDS_HUMAN
RUNNER_STATUS=COMPLETE_CANDIDATE
CONTINUE = ordinary checkpoint or a local blocker that has another safe work path.
NEEDS_HUMAN = concrete authorization/input/safety blocker that prevents other useful work.
COMPLETE_CANDIDATE = all acceptance gates demonstrably satisfied (operator will review; do not self-declare 100%).
Never fabricate a provider limit.

Begin the next unfinished implementation now.
```
