# Retail field sprite binding arguments

Shared checkout HEAD 3a3e7aac03a2f166fb924945a489e392d706f282. No commit or push. Production change: argument roles in func_80076AC0, src/field/main/misc2.c.

## Observed failure and authority

The axis-delta-corrected run escaped its first mountain battle, then won the next encounter. Victory and Hob-Jerky reward screens were observed. On field15 reload it crashed at23:47:07 CDT in func_8002435C with a null package. The caller was restoring actor14 with skin selector3, sprite3. The saved field snapshot itself contains selector3; the three valid party buffers exist, but slot3 is outside their range. Core remains ignored locally at scratchpad/lahan-natural-20260907-axis/core.713407.

Retail field.bin SHA256 38a1ce829a6f094c505f67143d6ace2d328418c65425a7383991179467e1fdfc. At80076AC8, retail loads argument5 (incoming SP+10) into s6; at80076AF0 it loads argument6 (SP+14) into s0. At80076B58 s0 is stored to actorData+126 (skin selector). At80076B80 s6 supplies actorData+134 low4 bits (texture page), and s6 selects the allocator/texture position at80076C14 and80076CBC. C had these argument roles reversed. The restore caller800A28D4 already passes texture-page flags then saved selector, matching retail. Field-local initialization callers also pass their selector, including bit7, as argument6. The repair preserves that ABI and changes the callee's parameter roles, without a null guard or state substitution.

## Verification scope

The full production misc2 translation unit is compiled unchanged apart from test-only forward declarations for preexisting implicit declarations; an unreachable callback is weakened to an abort spy to permit linking. The oracle executes actual field.bin instructions from80076AC0 to the first sprite allocator call. Heap-user selection and prior sprite release are recorded; allocator arguments and every byte of the0x138 actorData fixture are compared. Allocation is an explicit terminal boundary, so subsequent binder behavior is not covered. Cases vary all256 selector bytes, sprite indices0..255, four modes, seven texture-page offsets and existing-sprite/initial-tick flags:14336 cases atO0,O2,UBSan. The pre-change source fails. Mutants replacing selector storage, texture-page flag storage, or allocator selection are all rejected. Native and PSX builds succeed. This is behavioral evidence for the tested prefix, not exact matching of the entire function or complete rendering proof.

## Fresh runtime check

Normal boot run scratchpad/lahan-natural-20260908-bind; main execution session38792, game795042, Xvfb795040, display:1. Frozen binary SHA256 0035eb0b9e42a7d641a5f575745622d48f013efd60795d7ccc1f623b4ed6c0ff. It is progressing through the opening; repeated battle return and later Lahan progression remain pending. Revalidate processes before control. No forced game state or transitions.

## Live result and next failure

Normal initialization of mountain actor14 passed selector131 (83hex), sprite3, page0, package00747088. After natural binder execution, actorData+126/+127 remained83/03 and page flags were0. observe-bind.log records this. At00:02:10 CDT combat aborted on previously unimplemented spriteBD (operand00, sprite00730200), before this run could test field return. The run, driver, and observer are terminal. Core preserved locally. BD's retail handler uses the table pointer at8006BE20, which the existing shared-data mapping places at native D_8006BE10+10hex. It reads an unsigned16 offset at table+2+2*operand and calls80023B84 with table+offset and sprite+24. BD is the next required implementation; no bypass was added.
