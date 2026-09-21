# Opening battle pilot-panel dialogue audit (read-only, 2026-09-05)

## Result

The observed pilot-panel presentation does not use the main `FontDrawLetters` path. It is loaded as a paired battle animation archive: one file supplies animation/package records and bytecode, and the adjacent file streams indexed image data into VRAM. The active frame-385 type-5 sprite maps to presentation ID `0x17`, hence archive `(0x0C, 2)` files `0x50` and `0x51`.

The first completed post-E5 runs stopped on native animation opcode `0xAC`, in the same type-5 sprite and the same file-`0x50` bytecode stream. After the retail-backed AC implementation passed its differential suite, fresh normal replay `opening-ac-5-khm2410x` advanced exactly one command and naturally stopped on opcode `0xC4`, again in sprite `0x007DFEF8`. Its latest frame-5640 capture still shows the Gear battle without the pilot text. Therefore `C4` is now the first **observed** unsupported native boundary, while the text root cause remains **UNRESOLVED**; this observation does not establish that C4 generates or selects glyphs.

## Exact data and execution path

1. The battle script interpreter reaches retail `func_800AAD54` at `0x800ADE38..0x800ADE44`, loads the signed presentation ID from `D_800D39E4`, and calls `func_800B8054`. `func_800B8054` latches it in `D_800591B4`.
2. The battle frame pump `func_800BE790` reads that latch at `0x800BEA8C` and calls `func_800B8068` at `0x800BEA9C`, then clears the latch. `func_800B8068` calls `func_800B7C34` and `func_800B7E94`.
3. Retail `func_800B7C34` (`0x800B7C34..0x800B7E94`; `asm/battle/nonmatchings/main/func_800B7C34.s`) selects archive `(0x0C,2)` at `0x800B7CF0..0x800B7CF8`. For ID `n`, it derives file `2*n+0x22` at `0x800B7CFC..0x800B7D04` and file `2*n+0x23` at `0x800B7D18`. The first is allocated/read into `D_800594F0` at `0x800B7D1C..0x800B7D48`; the second is streamed through `func_80029EB0` at `0x800B7DFC..0x800B7E48` using `D_800594BC`.
4. Retail `func_800B7E94` (`0x800B7E94..0x800B8048`) creates/binds the special animation from `D_800594F0`: `func_80022224` at `0x800B7F08..0x800B7F2C`, child clone or `SpriteSetSpecialAnimFile`, and package parse `func_800C0FAC` at `0x800B7FD4..0x800B7FE0`. The parsed object is stored at sprite offset `+0x50` at `0x800B7FF0..0x800B7FF4`.
5. `func_800C0FAC` (`0x800C0FAC..0x800C1140`) walks the first file's six-section package. Non-`wds `/`seds` sections are instantiated by `func_80022224`; the sound sections are handled separately. This is package parsing, not ordinary font decoding.
6. `func_800248D4` (`src/slus_006.64/system/temp1.c:850`) sees battle mode `D_800591AD` and invokes the actual battle overlay interpreter `func_800C11CC`. In the port, `pc_port/src/battle_mips_runtime.c:852` dispatches retail guest address `0x800C11CC`; it does not substitute the field interpreter.
7. For the observed type-5 children, `func_80025224` binds the retail type table entry to `func_80025258` (`pc_port/src/game_overrides.c:3065`; type 5 is populated). The render path is `func_80025258` -> `func_8001E298` -> `func_8001E3D8` (`src/slus_006.64/system/temp1.c:1727`, `src/slus_006.64/system/rendering.c:545`, `:582`). `func_8001E3D8` turns the selected first-file frame records into `POLY_FT4` packets, including screen bounds, UV, tpage, CLUT, color, and OT link.
8. The adjacent second file reaches `func_8002BF38` through the port's synchronous `func_80029EB0` drain. Retail `func_8002BF38` is `0x8002BF38..0x8002C30C` (`src/slus_006.64/system/temp2.c:77`): it decodes `0x1200/0x1201` section headers and calls `LoadImage` for each strip. Thus the candidate text/panel pixels are sourced from this special-animation VRAM stream, while the first file controls which rectangles are drawn.

The generic-font branch in `func_800BE790` tests `D_80010000` at `0x800BE86C` and calls `FontDrawLetters` at `0x800BE880` only when it is not `-1`. The port's `D_80010000 == -1` is the unused retail-ROM mode here and should not be forced.

## Retail-byte identification

Authority: `disc/SLUS_006.64` SHA-256 `dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119`; battle archive bytes were extracted from `disc/disc1.bin` as raw 2352-byte sectors with each 2048-byte data payload beginning at sector offset `+24`.

The frame-385 sprite (`0x007DFEF8`) reported operands `0x0079EDD3`. Its RAM bytes uniquely matched the archive bytecode at file `0x50`, operand offset `0x55F`. Because `0x50 = 2*n + 0x22`, `n = 0x17`. The concrete pair is:

- `(0x0C,2)/0x50`: absolute archive-table entry `0xD83`, sector `0x4166C`, size `0x10714`, SHA-256 `84aa5786eb8ea686d41e76e0964fba2ae2c844cf99a4106dddd5ae7603cf4e3d`. Header: section count `6`, offsets `0x20, 0x5174, 0x6674, 0x6698, 0x6838, 0x6A54`, end `0x10714`.
- `(0x0C,2)/0x51`: absolute archive-table entry `0xD84`, sector `0x4168D`, size `0x11000`, SHA-256 `7515f9e3dcf5c39fc7e4fecbcfdc3c8bc38bfc687675dfed5ee84f95abeebf38`. Its three decoded uploads are `(x=0,y=465,w=256,h=1)`, `(x=0,y=464,w=48,h=1)`, and `(x=896,y=256,w=124,h=232)`; the last comprises 29 strips of height 8.

Exact file-`0x50` bytes around the observed control path:

```text
0550 b4 04 e0 37 fb 31 e4 fc ff 8e bb ea b8 01 e5 00
0560 12 c8 a0 00 c1 14 ac ff c4 ff b5 10 ba 02 91 f1
```

Opcode E5 is at file offset `0x55E`, with operands `00 12`. It is the RNG destination command implemented from SLUS `0x80020C20..0x80020C4C` (file offset `0x11420`), not `ScaleMatrix`. The first subsequent unsupported event in both `opening-ntsc-c1-5-jw5zz2qe/run.log:944` and the preserved `interactive-ntsc-speed-1-k02ujy2z/run.log:607` was opcode `0xAC`, operands host `0x0079EDDB`: file-`0x50` opcode offset `0x566`, operand `FF` at `0x567`. The authoritative handler is SLUS `0x80021644..0x80021698` (file offset `0x11E44`, handler SHA-256 `dc54f9d50528ab17f3a9cc0c8813fb1743ced8ceac92d383d2044ff8448e1620`). Current source has the new handler at `src/slus_006.64/system/animation_scripts.c:267`; bounded suite `sprite_dispatch_ac_retail_test.Vpp9ymX0` reports `PASS 4616193 cases` under O0, O2, and UBSan.

Fresh normal replay `opening-ac-5-khm2410x` used binary SHA-256 `b373a556c304a9136986cb940b95386f140dca90be0534907f30310b755490b4` and animation source SHA-256 `a6fe7059334e6ac2fc68829cc6ff8d4023de47822536551d01c47e9c0d7743b4`. At `run.log:947` it naturally aborted on raw opcode `196` (`0xC4`), dispatch index `58`, sprite `0x007DFEF8`, operands `0x0079EDDD`. This is two bytes after AC's operands, and maps directly to the adjacent retail bytes: C4 at file-`0x50` offset `0x568`, operand `FF` at `0x569`. Capture `captures/field-frame-005640.png` immediately before the stop shows the Gear battle with no pilot text. C4 is consequently the next observed native boundary, not a proven text cause.

## What the registration evidence rules out

`opening-battle-host-speed-39dbja_6/panel-register.jsonl` records type 10 and 11 at frame 12, type 4 at frame 101, and three type-5 registrations at frames 383/383/384. `panel-draw.jsonl` reaches the last type-5 sprite at frame 385. No type 8, 9, or 15 callback was requested before that stop. The unported 8/9/15 table entries are real gaps, but there is no evidence that they caused this missing line at the observed stop. Types 4, 10, and 11 are NULL in retail as well; type 5 is wired to the native billboard renderer.

The `panel-draw` probe establishes callback entry, not that a particular emitted quad is Fei's letters. Likewise, mapping the active bytecode to files `0x50/0x51` identifies the active presentation package, but does not yet prove which package subframe contains the wording.

## Concrete next read-only runtime observation

After the independently owned C4 boundary is resolved, replay without planted state and record four correlated facts for presentation ID `0x17`:

1. At `func_800B7C34`, log the requested ID and hashes/sizes of files `0x50/0x51`.
2. At `func_8002BF38`/`LoadImage`, log the three upload rectangles above and hash a VRAM readback of each destination after `DrawSync`.
3. For every type-5 call through `func_80025258 -> func_8001E3D8`, log sprite, current bytecode pointer/file-`0x50` offset, selected frame-record offset, screen quad, UV bounds, tpage, CLUT, and whether `AddPrim` links it into the live OT.
4. Hash/capture the corresponding final framebuffer region and compare the same story beat against retail execution.

This separates the remaining hypotheses without assuming a cause:

- Correct file and VRAM uploads, but no text-atlas quad emitted: animation control/frame selection remains wrong or stops later.
- Emitted quad references the expected `0x51` region, but VRAM readback differs: archive streaming/upload is wrong.
- Correct file, VRAM pixels, and linked packet, but no final pixels: renderer texture addressing, CLUT/tpage, blend, OT, or presentation is wrong.
- Retail selects a different ID/subframe: package selection or script state is wrong.

No current evidence supports forcing the generic font path, blaming callback types 8/9/15, or claiming that E5/AC themselves generate glyphs.
