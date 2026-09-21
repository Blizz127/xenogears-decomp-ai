# Read-only pilot rendering-path audit, 2026-09-06 UTC

## Result

**Pilot text ownership remains UNRESOLVED.** There is now concrete evidence that the previously identified presentation 0x17 is not the pilot panel/text asset: the independent atlas lane decoded its images as attack pieces and a red slash. Thus `func_800B7C34 -> files50/51 -> type5 -> func_8001E3D8` is a real retail attack path, but it does not establish the visible pilot panel path. Continuing to observe only that sprite would miss the requested text owner.

**The SDK FontDrawLetters path is excluded by actual retail bytes.** `disc/SLUS_006.64` contains `ffffffff` at executable data address 0x80010000. Battle `func_800BE790` tests this word at 0x800BE86C and skips `FontDrawLetters` at 0x800BE880 when it is -1. The same mode guard earlier skips a development path containing `break 1` at 0x800BE830. All three battle FontPrintf calls are in `func_800792F8`, which prints literal `Language Error` and `Actor%X No%x` strings; these are not pilot dialogue. No forced font flag is justified.

## Scope and authority

Repository `/var/home/blizz/Projects/xenogears-decomp-ai`; branch `experiment/worldmap-open-gates-20260823`; HEAD `3a3e7aac03a2f166fb924945a489e392d706f282`; already heavily dirty shared tree. No repository files, processes, runtime state, or assets were modified by this audit. Only this report was written under `/tmp`. No atlas work was duplicated.

Retail battle SHA-256: `1830b4ef1fe37129972fc310dfad534f8161d6c0b123e74254c3711334a3e291`.

Direct comparison of all instruction words in the following generated asm bodies to the offsets in `disc/battle.bin` found **1281 instruction words, zero mismatches**: `800BE790` (221), `800792F8` (62), `80074F70` (39), `800764EC` (22), `80071964` (41), `80071B94` (439), `80086C88` (196), `800B7C34` (152), `800B7E94` (109). These functions' addresses and branches below are retail byte verified, not guessed from function names. Base for battle binary file offsets is 0x8006FAF0.

The normal contact sheet `/tmp/xeno-pilot-runtime-20260906/normal-battle-contact.png` was viewed. It visibly contains the upper-left bordered panel, face on right, Gear/background on left, with no letters. It cannot by itself attribute any packet or uploaded image to those pixels.

## Other actual retail battle text path

There are two separate resident system-text APIs that the previous audit did not distinguish from the SDK debug font:

1. **Window update/draw `func_80034888`:** battle `func_800764EC` calls `func_80074F70` at 0x80076514. `func_80074F70` reads a UI state pointer at 0x800D2D28 and checks state byte +0xC9 at 0x80074FCC. If nonzero, the call at 0x80074FF4 is exactly:
   - a0 = 32-bit pointer stored at 0x800D2DAC;
   - a1 = pointer stored at 0x800CCB04, plus 4;
   - a2 = word at 0x800CCB34, render context.
   A separate +0xC8 gate adds a prebuilt FT4 from `*(0x800D3278)+0x7A4+40*byte(+0x7F4)` at 0x80074FB8.
   **Caution:** no direct write/reference to 0x800D2DAC other than this read and its BSS declaration was found in battle asm. Neither +0xC8 nor +0xC9 has another literal access in battle asm. This makes the existence of the function insufficient to call it the opening pilot owner. Log the natural gate value before following it.
2. **Single-string bitmap rasterizer `SystemRenderStringEntry` at 0x80034EAC:** this decodes text into a work buffer through resident `func_80033DF0`. Callers then upload the bitmap and draw ordinary UI primitives. It is independent of `D_80010000`/FontDrawLetters. Current native implementation is `src/slus_006.64/system/system.c:1060` with arguments `(pString,pWork,height,flag)`; name `height` is inherited and not a proven semantic for every caller.

The battle contains 19 calls to SystemRenderStringEntry (not 19 distinct functions). Native breakpoint at this API is a compact census of all ordinary battle strings. Record the guest return PC to assign ownership:

| Guest call PC | Owning battle function |
|---|---|
| 800719C4 | 80071964 |
| 80071D68 | 80071B94 |
| 80076F0C, 80076F38 | 80076EA4 |
| 800786C8 | 80078658 |
| 800860CC | 80086028 |
| 80086D58 | 80086C88 |
| 8008FFEC, 8009001C | 8008FE18 |
| 8009076C | 8009070C |
| 80091354 | 80091064 |
| 80091C1C, 80091C44 | 80091B38 |
| 800928D4, 80092914 | 80092784 |
| 80093220 | 800930AC |
| 800938B0, 800938D8 | 8009382C |
| 80093DB4 | 80093B08 |

Examples of exact data chains (not established pilot owners):

- 80071964: string table pointer at 800D39F0, index byte at 800D2CAF; GetStringEntry at 800719A8; bitmap destination pointer at 800D39C0; parameter2=0x39, flag=1; returned width byte at 800D39C6; LoadImage at 800719D8 uses RECT 800D39B8 and that bitmap.
- 80071B94: table pointer 800C3DDC; index selected by byte `800C3E3D + byte(*(800C3EAC)+0x2D3)`; GetStringEntry at 80071D4C; work pointer 800D3720; parameter2=0x39, flag=0; width byte 800D3726; upload helper800769E8 at80071D7C uses RECT800D3718.
- 80086C88: func800339C8 selects a system string; rasterizer at80086D58; LoadImage at80086D90. This is a menu-style row pipeline and should be attributed from its actual string, not merely assumed to be the pilot.

## Exact native/guest observations to collect next

No execution state needs modification for any of these observations.

- At native `runtime_bridge_call(opaque,cpu,target)`, record native target, raw guest registers and `cpu->gpr[31]-8` before pointer translation. Filter targets 0x80034EAC (string rasterizer), 0x80034888 (window), 0x80044894 (LoadImage), 0x80043B48 (AddPrim). Verify symbol-map spelling/address for any implementation-dependent native breakpoint. Generated map is `pc_port/build_native/battle_bridge_map.inc`.
- At native `SystemRenderStringEntry`, log string bytes before rasterization, work buffer, arguments, result width, and work bytes after return. Correlate the same pointer with following LoadImage rectangle and data. A zero-call census during the panel would exclude this API for that exact interval, not for all battle dialogue.
- Before interpreting window state, read guest 0x800D2D28, then byte +0xC9 and pointer 0x800D2DAC from `PSX_ADDR`. Battle overlay globals reside in g_PsxRam; they are not independently linked host symbols. If naturally active, log window offsets +0x10 flags, +0x1C string pointer, +0x28 rows, +0x2C bitmap work, +0x82 queued strings, +0x84/+0x86/+0x88 timing, +0x8C queue. Row stride is0x60; +0x58 width; +0x50 upload rectangle; +0x5E CLUT. Native func80034888 calls func80033DF0 then LoadImage and links row sprites through func80031798. Current source uses width!=0 as row-draw gate.
- At **every** native `func_8001E3D8`, not only type5/presentation0x17, log sprite and selected frame. Sprite +0x20 points to render base; base +0x30 points to 0x18-byte frame records. Frame count is `(sprite_word40>>2)&0x3f`. Each record has signed x/y at0/2, u/v/width/height at4/5/6/7, tpage at0xA, CLUT at0xC, RGB/code at0x10, direction/flags at0x14. Record sprite flags3C/40/AC and base direction transforms+0x34. Direction visibility can independently discard a frame.
- At the linked FT4, capture screen xy, tpage, CLUT, UV bounds and packet/OT pointer. Classify quads intersecting the visible panel rectangle from those coordinates; then trace their frame-record bytes to an archive package. Do not classify an owner from timing alone. `func_800BAB0C` can call func8001E298 directly, so callback type table alone is not exhaustive.
- If no matching billboard packet appears, record guest AddPrim call PC for packets intersecting the panel. Many battle UI packets are assembled directly in overlay asm and bypass func8001E3D8. The AddPrim hook should cover these while the source macro in resident rendering needs its own post-link observation.

For VRAM decoding use tpage depth; the same upload rectangle may contain adjacent4bpp and8bpp pages. Sol's0x17 result is a concrete warning against interpreting all of file51 as a single4bpp atlas.

## Independent atlas evidence used

The atlas lane reported the file51 split as4bpp page at(896,256),256x232 with CLUTs(0/16/32,464), and8bpp page at(960,256),120x232 used with CLUT(0,465). File50 frames1-18 select small flesh/mechanical/claw pieces; frame19 selects a red slash. No Latin glyphs, words, Fei portrait, or bordered pilot panel were visible. Artifact: `/tmp/xeno-pilot-atlas-20260906/subpackage-frames-contact-sheet.png`. This is delegated evidence; see that lane's report/metadata for hashes and decoder provenance.

## Acceptance boundary

No particular alternate function has yet been established to draw `Hiyaaaaa!`, nor has a retail framebuffer established the glyph placement. There is no proven production mismatch to repair in this lane. There is a **proven evidence-association mismatch**: presentation0x17's active type5 sprite was treated as a candidate pilot-text owner before its atlas was decoded. The bounded next step is actual panel-packet attribution followed by its data/font path, not another animation-opcode fix justified as text repair.
