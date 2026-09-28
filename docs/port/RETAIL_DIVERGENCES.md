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
| `func_80098370` (misc7.c) | sets only the current script slot's `flags_0` (u16 at slot+4) to 0xFFFF | also wrote 0xFFFF to the slot's upper flag half (state, isInUse and the other bitfields) | The port marked the slot's state/in-use bits all set, which could change later script scheduling. Port fixed to retail in the func_80098370 match. |
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

## Found later (open)

| Function (menu.bin) | Retail behaviour | Port behaviour | Suspected cause | Suspected visible effect |
| --- | --- | --- | --- | --- |
| `func_801E41C0` | party record `&g_GameState + 0x978 + idx * 0xA4`; `rec[0x9F] = tbl[0x17]` always (stored in the branch delay slot) | stride 0x28; `rec[0x9F]` written only when `rec[0x64] < rec[0x60]` | same struct misread as func_801E4258; delay-slot store read as conditional | wrong character's gear stats for idx > 0; field 0x9F stale |
| `func_801E42AC` | stride 0xA4; `rec[0x3F] = tbl[0xE]` always (delay slot) | stride 0x28; `rec[0x3F]` only inside the `if` | same as func_801E41C0 | same as func_801E41C0 |
| `func_801E4998` | `*(u16*)(model + 0x24) = (val / 10) * 2 / 9`, then rounded down to a multiple of 10 | an extra `/ 2` before the store | reciprocal-multiply shifts miscounted | displayed value (fuel/speed class) half of retail |
| `func_801CB184` | copies each 0x14-byte g_GameState name entry into ONE buffer (`buf[i] = e[i]; buf[i+1] = e[i+1]` up to a 0,0 pair), then decodes `buf` with `func_80033B34(buf, out, i / 2)` | even bytes into `srcBuf`, odd bytes into a separate `dstBuf`, and decodes `srcBuf` into `dstBuf` | the two interleaved store pointers (sp+0x10 / sp+0x11, stepping 2) read as two buffers | decoded names corrupted (half the bytes missing, output overwrites the odd-byte buffer) |
| `func_801E35BC` | character/stat records at `g_GameState + 0x26C + idx * 0xA4` (base record via the byte at `+0x30C + char * 0xA4`, then `+0x978 + b * 0xA4`); slot table indexed `*(pCtx + charIdx * 4 + 0x20) + slotIdx * 0x28` | stride 0x28 for all three records; slot table indexed by `slotIdx * 4` | same 0xA4 stride misread as func_801E4258; wrong index register | wrong character's stat raised/capped for index > 0 |

These five still need a byte-matching C body before a shared body can fix
them (they're still INCLUDE_ASM in retail); the port bodies are unchanged.

## Resolutions (continued)

| Function | Status | Suspected cause of the port divergence | Verification |
| --- | --- | --- | --- |
| `func_801C6D90` | pinned in 2e901ce2 | (fixed in bffc225f) | run_menu_vram_rect_pins_test asserts RECT {0, 0x1C0, 0x10, 1}; the old port args are rejected |
| `func_801E64E0` | pinned in 2e901ce2 | (fixed in bffc225f) | run_menu_vram_rect_pins_test asserts RECT {0x140, 0xE0, 0x40, 0x20} and colour 0,0,0; the old port args are rejected |
| `func_801D83AC` | fixed in a00d7c50 (one shared body) | retail keeps the colour in a stack `u8 rgb[3]`: mode 0 = {0x80,0x40,0x40}, mode 1 = {0x40,0x40,0x80}, mode 2 = {0x40,0x40,0x40}; the port had mode 0 = {0x40,0x80,0x10}, i.e. the three stores were read as r/g/b locals in the wrong slots. Visible effect: the tint on the prim bank the function recolours | rom-check 10/10; no port test covers it |

## Found later (open, continued)

| Function (menu.bin) | Retail behaviour | Port behaviour | Suspected cause | Suspected visible effect |
| --- | --- | --- | --- | --- |
| `func_801CD710` | case 0 stores 7 to g_Menu+0x334; returns 0 when the called handler (func_801CD2AC / CC6D8 / CB304 / CBD90) returns nonzero | stores 1 in case 0; ignores the handler results and always returns 1 | immediate misread; handler return values dropped | wrong follow-up menu state after the case-0 handler; failures not reported to the caller |

## Resolutions (continued, 2)

| Function | Status | Verification |
| --- | --- | --- |
| `func_801E42AC` | fixed in 433db88e (one shared body, retail C matches) | rom-check 10/10; pinned by run_menu_party_record_pins_test (0xA4 stride, unconditional rec[0x3F]) in 0fd709e6 |
| `func_801E4258` | shared body's `(s32)&g_GameState` host-pointer truncation fixed in 433db88e | run_menu_party_record_pins_test |
| `func_801E41C0` | port fixed in 433db88e (retail C still asm) | run_menu_party_record_pins_test (0xA4 stride, unconditional rec[0x9F]) |
| `func_801E4998` | port fixed in 433db88e (retail C still asm) | run_menu_party_record_pins_test ((v/10)*2/9, rounded to x10; extra-halving mutant rejected); run_menu_gear_weapon passes |
| `func_801E35BC` | port fixed in 433db88e (retail C still asm) | run_menu_party_record_pins_test (0xA4 strides, charIdx slot index, reverse path) |
| `func_801CB184` | port fixed in 433db88e (retail C still asm) | run_menu_name_decode_pins_test (one contiguous pair buffer, pair count) |
| `func_801CD710` | port partly fixed in 433db88e (retail C still asm): mode 0 now stores 7. Still open: retail returns 0 when the handler returns nonzero, but func_801CD2AC / CC6D8 / CB304 / CBD90 have no port implementation, so there is no result to use yet | run_menu_card_state_pins_test (7/6/2/3) |

## Resolutions (continued, 3)

| Function | Status | Verification |
| --- | --- | --- |
| `MenuExecute` (slus `system/menu.c`, `g_MenuDebugEnabled` branch) | port fixed: the two heap reservations sized `(u32)host_ptr + 0x7FE24000` / `+ 0x7FE3B000` (garbage on the host) now use `PsxMemory_GuestAddr(ptr)`, and the object-overlay (archive 0x6B9) and menu-overlay loads go to `PSX_ADDR(0x801DC000)` / `PSX_ADDR(0x801C5000)` instead of raw host addresses; port-only `#ifdef`, retail bytes unchanged | rom-check 10/10; port LINK OK |
