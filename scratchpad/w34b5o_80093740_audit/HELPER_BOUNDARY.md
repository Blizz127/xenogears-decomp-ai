# World helper 0x80093740 boundary

- Authority: `disc/world_map.bin`, SHA-256 `4c15fd32b3a03d7cd5ea4403dcaabc70abaf99b6aaca65d63d6866803edaac70`.
- Load base: `0x8006FAF0`.
- File range: `[0x23C50, 0x23E88)`.
- Function range: `[0x80093740, 0x80093978)`.
- Prologue: `0x80093740 addiu sp,sp,-0x28`; saves `s0-s3` and `ra`.
- Sole return: `0x80093970 jr ra`.
- Return delay slot: `0x80093974 nop`.
- End exclusive / next prologue: `0x80093978 addiu sp,sp,-0x50`.
- Size: `0x238` bytes (568 decimal).
- Instruction count: 142.
- Direct calls: three. Indirect calls: zero. Loops: zero. Early returns: zero.
