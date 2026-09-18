# Retail sprite E9/EA/EB axis deltas

Shared dirty checkout HEAD3a3e7aac03a2f166fb924945a489e392d706f282. No commit/push. Production scope is the E9/EA/EB block in src/slus_006.64/system/animation_scripts.c; new focused differential test and runner accompany it.

## Live progression and observed failure

The A7-corrected ordinary-boot run naturally reached fields14,13,1,15 and combat. Its first queued route correctly stopped when the painting dialogue had not closed; after observing that state, ordinary input finished the dialogue and retraced the stairs. No game-state writes or forced transitions occurred.

The recorded live A7 call had operand00, spriteAC=8004, delay9E=0, globaltimer=0. After naturally executing the repaired function, delay9E became1 and the globaltimer remained0, as retail requires. The next abort was unimplemented spriteE9 at23:18:44 CDT. Core: sprite0072FD58, operand pointer0072B6BE with bytes00 FF, model pointer0072FE0C, flags45. Core remains local at scratchpad/lahan-natural-20260907-a7/core.671427. That game and all associated observer/driver sessions are terminal.

## Authority and implementation

Retail jtbl_800183D8 entries E9/EA/EB target80021374/800213A8/800213DC. The handlers read a signed16-bit immediate, double it, and add modulo16 to model+6/+8/+A respectively. Model pointer is read from sprite+20. If null, no write occurs. Otherwise shared tail800215A4..800215B4 ORs10000000 into sprite+3C after the model write. The C implementation preserves these reads/writes, including overlapping model/flag and operand data. Doubling as unsigned before storing the low16 bits preserves the signed retail result without signed-shift undefined behavior. The observed FF00 operand corresponds to-512 after doubling.

## Verification and limits

Pre-change test reproduced the E9 assertion. Actual retail dispatcher instructions are executed with no substituted helper functions. All65536 operand patterns for allthree opcodes are compared across null/model/flag-halfword overlap/operand overlap fixtures, plus high opcode bits:987693 cases per O0/O2/UBSan pass. Complete fixture bytes are compared, not only the target halfword. Four mutants are rejected: omitted dirtyflag, missing double, wrongaxis and wide store. A7 regression and its controls pass.

Native and shared PSX builds succeed. Frozen native SHA256 ca8a36b2381dce2342b8b1a257e1fcb7414e4ffb763e97f352cb22d809c7dec7;74 function stubs remain. These tests are finite instruction-behavior evidence; full dispatcher exact compiled bytes, battle return, subsequent story and retail audiovisual acceptance remain unproven.

## Current run

Fresh normal boot at scratchpad/lahan-natural-20260907-axis. Execution session64225, game713407, Xvfb713405 display:1; driver PID in process.json. The driver stops automatic input at field14. Inspect/finish the painting dialogue before retracing; the earlier timed route is not deterministic enough to assume dialogue completion. Revalidate live processes before continuing.

## Live repaired E9 observation

The frozen axis binary naturally traversed fields14,13,1,15. The first encounter escaped via ordinary menu input and returned to field15; this does not prove victory. In the next encounter, ordinary Attack inputs reached E9 with operand00 FF, sprite0072FD58 and model0072FE0C. A read-only GDB breakpoint followed by natural function completion observed model+6 change2000 to1E00 and flags45 to10000045 (axis-live.log). The game continued past the previous E9 abort. Battle victory and the remaining story are still pending. No game-state writes or forced transitions were used.

## Victory and next failure

Ordinary further Square input completed the apparent stalled attack; it was not a full battle lock. Victory results and a Hob-Jerky reward were observed. The battle adapter returned after2310389471 instructions. At23:47:07 CDT the field15 reload crashed in func_8002435C because the restored actor14 requested party skin3 with a null animation package. crash-return.log records the core stack. No null guard or forced state was added; the restore/package data path requires diagnosis. Current run and driver are terminal, core retained locally. Field return, later story, and retail audiovisual accuracy remain unproven.
