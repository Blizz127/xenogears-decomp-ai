# Sprite CE model dirty flag and defined arithmetic

The CD crash core contains `CD 05 00 CE 04 00 EA 80 00`. Inspection of the next opcode found the existing CE implementation always set sprite+3C bit28. Retail 0x8001FFC0..80020088 branches directly to the dispatcher return after indexed direction writes, and only model writes join the dirty-flag tail at 0x800215A4.

A pinned retail-oracle regression failed at operand0x0200 with a valid direction table. The production fix limits dirty-flag writes to index zero. It also uses packed byte reads/writes and removes signed left-shift undefined behavior while preserving the low16 operand value, mirror bit, add/assign behavior, and null gates.

The test compares the entire fixture after executing the retail dispatcher and production C. It enumerates every16-bit operand, both mirror states, three pointer states, and four normal/overlapping layouts, plus initial-value variation. Mutation controls cover the original dirty-flag bug, mirror behavior, direction offset, and assignment bit.

Run `bash pc_port/tests/run_sprite_dispatch_ce_retail_test.sh`. This is behavioral evidence; exact compiled-byte and rendered parity are not established. The ongoing frozen CD run does not include this subsequent CE repair. No live scene state was modified.
