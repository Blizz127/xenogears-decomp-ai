# Fresh 0x80073B04 boundary and ABI

- Image: `disc/world_map.bin`, 180422 bytes, SHA-256 `4c15fd32b3a03d7cd5ea4403dcaabc70abaf99b6aaca65d63d6866803edaac70`.
- Load base: `0x8006FAF0`; image VA range `[0x8006FAF0, 0x8009BBB6)`.
- Exact physical slice: `[0x80073B04, 0x80073E30)`, file offsets `0x004014..0x004340`.
- Size: `812` bytes (`0x32C`), `203` instructions; slice SHA-256 `87f9ff21fffad640d8fb7ec9a96da4f5d29a71275c4b15452d49677b6ad23197`.
- Boundary proof: the first `jr $ra` is at `0x80073E28`, its delay slot is `0x80073E2C`; the next physical words are `0x27BDFFD0, 0x00002021` at `0x80073E30`.
- ABI: leaf `void wm_80073B04(void)`; no argument registers are read, no return value is consumed, and there is no `jalr`.
- Stack frame: `0x40` bytes; saves `$ra` at `0x3C($sp)`, `$s0` at `0x28`, `$s1` at `0x2C`, `$s2` at `0x30`, `$s3` at `0x34`, `$s4` at `0x38`; restores them and returns through the sole epilogue.
- Control flow: one two-iteration loop over two quads; one last-quad GTE flag bit31 guard; no other return path.
- Direct calls, branches, delay slots, global/indirect accesses, and complete write set are recorded in the companion CSV/CFG files.
- Fresh retail result: no boundary, ABI, or dependency contradiction; the five direct GTE helpers are already linked in PsyCross and the helper is class A in isolation.
