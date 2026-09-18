# Retail sprite BD indexed child script

Shared checkout HEAD3a3e7aac03a2f166fb924945a489e392d706f282. No commit/push. Production scope is the BD dispatcher branch in animation_scripts.c.

The binder-corrected natural boot reached field15 and combat, then aborted on BD at00:02:10 CDT. Sprite00730200, operand00, package00730310. Core remains ignored locally at scratchpad/lahan-natural-20260908-bind/core.795042. The source handler is retail80021410..8002143C: read an unsigned byte index, load table pointer8006BE20, read unsigned16 offset at table+2+2*index, read package pointer at sprite+24, call80023B84(sprite,table+offset,package), discard return. The existing native shared-data bridge owns8006BE20 inside D_8006BE10+10hex; the crash core has table007447B0 there. Reading the guest RAM slot directly would incorrectly yield zero.

The new C branch follows that sequence using the existing packed shared block and child helper. Test runs the real retail dispatcher and records the child helper boundary on both sides, including a helper-side sprite write; compares all fixture bytes and exact arguments/count. Exhaustive unsigned16 offsets at selected indices, all256 indices with high-bit offsets, opcode high bits and selected table/operand/sprite overlap cases produce262656 cases perO0/O2/UBSan. The pre-change assertion reproduces; four mutants are rejected (index stride, table header, signed offset, wrong package). Test-only weak shared block supports borrowed dependency guards. Actual child creation/rendering is outside this boundary test, as are full dispatcher byte matching and retail audiovisual accuracy.

Native build passes. Fresh normal boot scratchpad/lahan-natural-20260908-bd, session78214, game832994, Xvfb832992 display:1. Frozen binary SHA2562bc5def4788a269124651a089b97fba4f9d21bdaebb3990da2d781ce3ec2dcfc. Revalidate processes before control. PSX build and prior axis regression were started; live BD/child completion and repeated field return remain pending.

PSX build and axis regression finished successfully (allthree builds plus four axis mutants rejected). Fresh game remains live in opening field4; do not restart based on this turn ending. Driver stops input atfield14; observe/close painting dialogue before retrace.

## First return and instrumentation failure

The natural run won its first mountain battle and returned to field15 (first-field-return.png). A second encounter began. The long-lived GDB observer then terminated with143; the game subsequently trapped at dispatcher entry+39, where core memory retainsCC (INT3), with current opcodeB4. This is instrumentation failure, not evidence of a BD or B4 production defect. No live BD completion or second return was proved. Core preserved locally; that run and observer are terminal. A fresh same-binary run will use no long-lived breakpoint.

Same-binary fresh run scratchpad/lahan-natural-20260908-bd-clean, main session63226, game876907, Xvfb876905, display:1. No debugger attached. Revalidate live state before continuing; stop/close painting dialogue before retrace.

## Uninstrumented repeat and sprite93

The clean run escaped its first encounter and returned to field15. A brief read-only attach/detach (no breakpoint) observed actor14 selector/sprite83/03 after reload, confirming selector preservation through this return. During the second battle, at00:29:59 CDT, opcode93 asserted on sprite0074F558 with parent0074408C. This is a production unimplemented path with no debugger attached. Repeated field return remains pending. Core and logs preserved; the run is terminal. Retail93 includes a parent-direction copy and an allocation helper8001D4E8, whose C is still commented in rendering.c and therefore requires its own implementation/verification. No93 repair or bypass has been added yet.
