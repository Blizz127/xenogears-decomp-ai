# Lahan continuation: retail sprite variable arithmetic

Branch experiment/worldmap-open-gates-20260823, HEAD
3a3e7aac03a2f166fb924945a489e392d706f282; existing dirty work preserved.
No stage/commit/push. Scope is resident sprite VM arithmetic, not proof of
Lahan runtime reachability or complete decompilation.

## Authority

SLUS_006.64 SHA256 dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119.
The runner pins that entire image before oracle execution.
Retail dispatcher 8001FBE4, table 800183D8, variable resolver 8001FBA4.

| Opcodes | Retail address | Actual operation |
| --- | --- | --- |
| D0, D3, DD, DE | 80020D68 | Add two unsigned variable bytes |
| D1 | 80020BB0 | Multiply two unsigned variable bytes |
| D2, D5 | 80020BE8 | Unsigned variable-byte division |
| D6 | 80020C50 | Add immediate byte |
| D7 | 80020C74 | Multiply unsigned byte by signed immediate |
| D8 | 80020C9C | Signed division of unsigned byte by signed immediate |
| D9 | 80020CC4 | Shift byte left |
| DA | 80020CE8 | Arithmetic shift of signed byte right |
| DB | 80020D0C | Shift assembled little-endian word left |
| DC | 80020D34 | SRAV on zero-extended assembled word |

No operation was inferred from mnemonic/opcode adjacency: the table aliases
and each body were inspected. DIV/DIVU zero-divisor quotient is all ones for
these nonnegative numerators. Shifts mask the count to five bits. Stores retain
the byte/word width and ordering. Both variable addresses resolve before loads.

## Verification

Before implementation the new fixture stopped at the D0 unsupported assertion.
After: 1,089,536 cases per O0/O2/UBSan configuration pass, executing the actual
retail dispatcher and resolver with no helper substitutions. The whole fixture
is compared. Cases include all 256x256 byte pairs for every opcode, stack slots,
variable self-aliasing, immediate operand/destination overlap, zero divisors,
negative immediate values, and counts outside the ordinary shift range.
Six deliberately incorrect implementations are rejected by the durable runner:
zero-divisor substitution, unsigned immediate division, wrong shift mask,
unsigned byte shift, signed word shift, and invented subtraction for table aliases.
Full native build: LINK OK, 74 generated function stubs remain.

## Limits

This C transcription proves tested behavior, not compiled PSX byte equality.
No complete-overlay hash or end-to-end Lahan acceptance is claimed. The separate
normal-boot run in scratchpad/lahan-natural-20260907-e6 uses a frozen binary
from before this arithmetic change; it cannot validate this linked build.
The 100% goal remains active.

Post-change E6 and no-op regression suites, including their negative controls, also pass.

## Fresh normal-route observation on the rebuilt binary

The corrected, single-driver run in scratchpad/lahan-natural-20260907-arithmetic
uses the linked 87165a34... binary. At local 21:48:27 it confirmed New Game,
21:48:34 entered field4, 21:50:36 entered field2, then entered retail battle.bin
and returned to field14 at21:51:59. The painting framebuffer is captured here.
Input was ordinary Z/Circle presses through X11/SDL; no scene/party/position,
checkpoint, battle-result, or test-input state was set. The initial boot movie
was not bypassed by a configuration override. Runtime uses llvmpipe, not the
user desktop GPU. Automatic input stopped at field14. This validates only the
observed route segment on this build, not end-to-end Lahan or retail pixel parity.
The previous e6 run ended through session cleanup, not a game failure.

Input-driver correction is source-backed: port_main.c explicitly swaps the
vendor Circle and Triangle keys after PsyX initialization. Live Circle is Z;
V is Triangle. The observed mapping is not a build drift.

## Continuation point

Four Circle presses closed Fei's painting dialogue. Ordinary directional holds
moved him around the chest/easel to the stair passage. The field14 exit has NOT
been observed: Fei is currently occluded by the wall, with player1 at integer
(333,0,-120), walkmesh triangle171, from a read-only GDB snapshot. This is an
unresolved navigation/visibility boundary, not a diagnosed gameplay defect.
No game state was changed by GDB. Input history and state snapshot are retained.
The diagnostic game remained live when this note was written: PID471575,
Xvfb471573, execution session36992, isolated display:2. Check those handles
before reuse. The single driver has a 15-minute observation deadline starting
21:48:04 local and will terminate its owned game/display at that boundary.
Do not treat that expected diagnostic shutdown as a game crash.
