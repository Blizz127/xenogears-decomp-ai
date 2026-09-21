# Natural Lahan to mountain battle: observed corruption

HEAD 3a3e7aac03a2f166fb924945a489e392d706f282, shared dirty checkout. Frozen native executable SHA256 87165a34833fd47912d4d9eb79621d028508169e42ded179481dcb531a18b49c.

The existing ordinary-input run walked from field1 across its positive-Z boundary and entered field15 (entrance1). Continued walking naturally started a Jackal encounter. The retail battle interpreter rendered the battle and accepted one Circle input. At 2026-09-07 22:32:12 CDT the process aborted; this was not a driver timeout. No quickload, debug boot, RAM/flag/pose modification, or forced encounter was used.

The live field1 script bytes (12056) and trigger logical bytes (288) matched disc archive BA; metadata is in field1-retail-provenance.json. Trigger decompression comparison covers only map-declared logical size, not six trailing stream bytes. Actor50's routine at1808 checks trigger5 then calls182A; that routine contains opcode98 at183B requesting field15 entrance1. Trigger5 has X -1247..907 and Z2241..2983. Native progression is observed, not retail audiovisual parity.

## Concrete failure

The final runtime line reports callback 800BAB0C failing on SPECIAL funct16, instruction00320096 at800BAB10. The core confirms SIGABRT from PcPort_BattleMipsDispatchCallback through WorkListUpdate. Retail battle.bin loaded at8006FAF0 contains instruction90423664 at800BAB10. The core instead contains00320096. Around800BA960..800BAB17, overwritten words exhibit POLY_FT4 layout at40-byte intervals: tag length09, opcode2D, screen coordinates and UV/CLUT fields interleaved with untouched retail instruction bytes. This proves code corruption before the callback; it does not yet identify the writer or its upstream bad pointer.

Do not add SPECIAL funct16 support or skip this callback to bypass the crash. Next diagnosis is to catch the first write into this region during a fresh natural encounter and trace the actual writer/arguments to retail behavior. No game source was changed in this observation pass.

## Preserved state and limits

Core and full emulated RAM remain local and ignored under scratchpad/lahan-natural-20260907-arithmetic/core.471575 and crash-psx-ram.bin; do not commit those payloads. The frozen executable, input log, field dumps, screenshots and full logs remain alongside them. The game PID471575 is terminal (zombie when first checked). Driver471572 was paused and Xvfb471573 still alive at capture; they are no longer evidence of a live game.

The battle did not complete or return to the field. The mountain events, return to Lahan, remaining story, exact compiled matching and retail audiovisual acceptance remain unfinished.

After preservation, the driver was resumed to reap the game. Its final screenshot attempted a missing window, so the owned driver and Xvfb were terminated. Session36992 is terminal.
