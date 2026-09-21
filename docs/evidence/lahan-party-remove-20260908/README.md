# Lahan departure party-removal context repair

The natural motion-matching replay reached map 3, completed the aftermath conversation, and faded out when Fei left. The game remained in FieldMain. Its loaded 5,340-byte script exactly matches retail map 3. At script 0x451 the controller hides Fei, then FE19 02 removes Citan. A hidden Fei still holding movement opcode 4A is not by itself the cause: native and retail walkmesh resolver replays agree on the captured candidate step.

Retail function 8008C180 (436 bytes, verified assembly against field.bin) saves the caller FieldActor and ActorData separately, publishes the target context, calls 80080A74, reloads the actor slot, calls 80076AC0, and restores the caller context. It updates the target ActorData retained after the constructor and clears the party actor slot. The former native body omitted teardown and target publication, restored ActorData from a FieldActor pointer, wrote the saved IP through that wrong pointer, and omitted the actor-slot reset.

The repair follows those retail operations, including the unusual saved-IP store into the target at 8008C2C4. It does not bypass the field script, change its target, or force the live game forward.

Validation: the former body fails the callback-order check. The repaired full production translation unit passes 195 cases each at O0, O2, and UBSan. Cases cover all three party positions, four target actor positions, distinct/aliased callers, a constructor that changes the target owner, four initial byte patterns, and absent actors. Five negative controls reject the missing teardown, wrong restored context, wrong IP owner, missing slot reset, and missing target context. Tests assert the complete ActorData arrays plus callback order, arguments, context, and party-slot effects.

The original 436-byte MIPS routine also passes the same 195-case callback/context contract in the instruction interpreter. Its two external calls use the same fixture callbacks, with retail global addresses mapped to the fixture state.

Limits: this is a bounded shared-contract comparison, not an exhaustive full-routine byte match. Constructor and teardown internals are isolated fixtures. The earlier PID 155721 observation used a frozen binary without this repair. A fresh repaired run has now naturally reached the world map; see [runtime observation](runtime-observation.md). Full Lahan retail 1:1 completion is not established. No commit or push.

Both native build and full `make build` completed successfully; logs preserved here. Byte-for-byte matching of the repaired routine remains unverified.

Compiled PSX size measured: 424 bytes versus retail 436; exact matching FAIL. A separate frozen natural replay started with SHA e04219abd6adb04855034836273b8429f71086e107f9c068ab3b8ae2f7b154b4, driver445594/game445617/Xvfb445595/display3. It includes this repair and the decompiled motion owners. That earlier run predates the grounding repair. The subsequent frozen slope-ground run cleared departure; see [runtime observation](runtime-observation.md).
