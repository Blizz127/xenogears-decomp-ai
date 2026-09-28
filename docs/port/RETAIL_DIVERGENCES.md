# Retail vs port divergences

Found while matching menu.bin. In each case the retail build now compiles a
byte-matching body under `#ifndef XENO_PC_PORT`, and the port body under
`XENO_PC_PORT` is unchanged. Nothing listed here has been fixed in the port yet.

| Function (menu.bin) | Retail behaviour | Port behaviour | Suspected impact |
| --- | --- | --- | --- |
| `func_801D0E20` | `for (i = 7; i >= 0; i--) {} return i;` returns -1 | `for (i = 6; ...) return i + 1;` returns 0 | None found: both callers ignore the result (it's a spin loop). Worth checking again if a caller starts using it. |
| `func_801D0EBC` | same shape from 5, returns -1 | same shape from 4, returns 0 | Same as `func_801D0E20`. |
| `func_801D1258` | AddPrim to `pGfxEnv + 0x90`, i.e. `ot[8]` of the 4-byte OT | `&ot[4]` on the host's 8-byte `unsigned long` OT (same byte offset, different slot) | The dim quad and its DR_MODE go into OT slot 4 instead of 8, so the dim layer may sort at a different depth relative to other menu prims. |
| `func_801C6D90` | LoadImage RECT {x 0, y 0x1C0, w 0x10, h 1} | RECT {0, 0, 0x1C0, 1} | The 16-entry CLUT row lands at VRAM (0,0) instead of (0,448) and overwrites 448 pixels of framebuffer row 0. Palette at y=448 is never uploaded. |
| `func_801E64E0` | ClearImage RECT {0x140, 0xE0, 0x40, 0x20}, colour 0,0,0 | RECT {0, 0, 0x140, 0xE0}, colour 0x40,0x20,0x20 | The port clears the whole 320x224 screen to a dark red-brown instead of blacking a 64x32 VRAM block at (320,224). |
| `func_801E4258` | party record = `&g_GameState + 0x978 + idx * 0xA4` | `+ idx * 0x28` | For idx > 0 the port reads and writes the wrong character record (stats at +0x70/+0x72). |
| `func_801E2324` | passes `&D_801EA568[arg0]` (a pointer) to `func_801E8018` | passes the byte `D_801EA568[arg0]` | `func_801E8018` gets a small integer where retail passes a string/table pointer. Wrong text, or a bad dereference if the port callee uses it. |
| `func_801CB28C` | stores `*(s32*)arg0` into `D_80059488`; ignores `func_801E4D10`'s result | stores `*(s32*)` of the returned pointer | Same only if `func_801E4D10` returns `arg0`; otherwise the play-time word is taken from the wrong place. |

| Function (field.bin) | Retail behaviour | Port behaviour | Suspected impact |
| --- | --- | --- | --- |
| `func_800A708C` (misc5.c) | passes `D_800C3A36` zero-extended (lhu) to `func_801D3538` | sign-extended it (lh) | Differs only for values >= 0x8000. Port fixed to retail in 063a970a. |
| `func_800ACB90` (misc9.c) | `FieldLoadTIMWithClut(D_800AF784, 0x380, 0x100, 0, 0x1FF, 0, 0)` | passes y = 0 | The TIM is uploaded at VRAM (0x380, 0) instead of (0x380, 0x100). Open: the function is still INCLUDE_ASM. |

## Resolutions

| Function | Status | Suspected cause of the port divergence | Verification |
| --- | --- | --- | --- |
| `func_801C6D90` | fixed in bffc225f (one shared body) | RECT halfwords shifted one slot (y lost, w/h take y/w) | rom-check 10/10; no port test covers it |
| `func_801E64E0` | fixed in bffc225f | RECT fields and the ClearImage colour arguments each shifted by two halfwords | rom-check 10/10; no port test covers it |
| `func_801E4258` | fixed in bffc225f | struct misread: 0x28 is the snapshot-side record size, not the 0xA4 g_GameState record | rom-check 10/10; run_menu_misc_coexistence passes (it stubs this function) |
| `func_801E2324` | fixed in bffc225f | array element vs address confusion on an unprototyped call | rom-check 10/10; no port test covers it |
| `func_801CB28C` | fixed in bffc225f | assumed func_801E4D10 returns its buffer argument | rom-check 10/10; run_menu_misc_coexistence passes |
| `func_801D0E20` | fixed in bffc225f | loop bounds re-derived by hand (6/+1 instead of 7/return i) | rom-check 10/10 |
| `func_801D0EBC` | fixed in bffc225f | same as func_801D0E20 | rom-check 10/10 |
| `func_801D1258` | open | the port reinterpreted the ot[8] byte offset for an 8-byte host OT and ended up at slot 4; the retail C doesn't match yet, so no shared body | — |
