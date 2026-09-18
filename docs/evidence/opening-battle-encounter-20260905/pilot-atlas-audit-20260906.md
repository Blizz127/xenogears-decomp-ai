# Xenogears retail presentation `0x17` pixel and frame audit

Audit date: 2026-09-06 UTC

## Result

**The retail `(0x0C,2)/0x50` and `/0x51` pair does not contain the expected pilot-dialogue letters.** Exhaustive lossless decoding shows:

- file `0x50`'s primary package contains nine frames of a small orange-haired character action/run sprite, visually consistent with Fei;
- file `0x50`'s secondary package selects file `0x51` pixels for eighteen flesh-colored mechanical claw/hand subframes and one large red three-streak energy/slash subframe;
- neither file contains a pilot portrait, a dialogue-panel border, Latin glyphs, a word image, or pixels spelling `Hiyaaaaa!`.

No UV, CLUT, or subframe in this pair selects letters because no letter pixels are present. Presentation `0x17` is therefore an action/attack presentation, not the owner of the missing pilot lettering. The earlier runtime association—an active type-5 sprite used `0x17` while a pilot-panel shot was on screen—established concurrent package use, but did not establish that this package supplied the panel or text.

The visual labels above are image interpretation. The stronger byte-backed statement is that every image-bearing record in both file-`0x50` packages and both indexed texture pages uploaded by file `0x51` has been decoded into the inspectable atlases listed below, and none visibly contains the expected lettering.

## Runtime context

The current repository milestone is the normal native replay `opening-type9-cleanup-5-s6ef07mm`: it returned from battle after `368,858,285` guest instructions, loaded field 14, and rendered Fei at the painting-room easel. Its binary SHA-256 is `36b934f2899d5514093d98a66753a555211914f95dbf1aa9182901eeb4886af1`; the preserved evidence image `painting-room-after-battle.png` has SHA-256 `636587bc62f8e061feec7967069fa2d5568cadca69a91497a3617bbe7398a33b`. That milestone proves battle return and does not prove the missing pilot lettering.

This audit launched or controlled no native process and made no PCSX, API, input, restart, or game-state call.

## Retail provenance

The source disc is `/var/home/blizz/Projects/xenogears-decomp-ai/disc/disc1.bin`, SHA-256 `39c547a9afc6da15d847ef81a2c6cea1a6516bdfa562cf13b0999b04e8598bda`. Extraction reads the 2048-byte Mode2/Form1 user payload at raw-sector offset `+24` from each 2352-byte sector.

| Archive item | Absolute table entry | Start sector | Exact size | SHA-256 |
|---|---:|---:|---:|---|
| `(0x0C,2)/0x50` | `0xD83` | `0x4166C` | `0x10714` | `84aa5786eb8ea686d41e76e0964fba2ae2c844cf99a4106dddd5ae7603cf4e3d` |
| `(0x0C,2)/0x51` | `0xD84` | `0x4168D` | `0x11000` | `7515f9e3dcf5c39fc7e4fecbcfdc3c8bc38bfc687675dfed5ee84f95abeebf38` |

The extracted copies are `retail-50.bin` and `retail-51.bin`. Their hashes reproduce the earlier archive audit exactly.

## File `0x51`: exact `func_8002BF38` stream decode

The decoder follows retail `func_8002BF38` at `[0x8002BF38,0x8002C30C)`: a `0x800`-byte header sector carries tag `0x1200` or `0x1201`, coordinate pairs at `+0x04..+0x0A`, width in VRAM words at `+0x0C`, total section count at `+0x14`, strip count at `+0x18`, and per-strip heights at `+0x1C`. Each following sector supplies `width * height` little-endian VRAM words to `LoadImage`, and the destination Y advances by the strip height.

The complete file parses without residue, bounds failure, or nonzero strip padding:

| Section | Header | Tag | Coordinate words | `LoadImage` rectangle | Strips |
|---:|---:|---:|---|---|---|
| 0 | `0x0000` | `0x1201` | `0,464,0,1` | `(x=0,y=465,w=256,h=1)` | one row |
| 1 | `0x1000` | `0x1201` | `0,464,0,0` | `(x=0,y=464,w=48,h=1)` | one row |
| 2 | `0x2000` | `0x1200` | `896,256,0,0` | `(x=896,y=256,w=124,h=232)` | 29 strips, each height 8 |

The frame records establish the pixel-depth split that the upload headers alone do not carry:

- VRAM `(896,256,64,232)` is one complete `256x232` 4-bit texture page. It uses three 16-color CLUTs at `(0,464)`, `(16,464)`, and `(32,464)`.
- VRAM `(960,256,60,232)` is the used `120x232` portion of the adjacent 8-bit texture page. It uses the 256-color CLUT at `(0,465)`.

Treating the whole 124-word upload as one 4-bit bitmap is wrong: it incorrectly splits each 8-bit index into two unrelated nibbles. The frame tile-header depth bits are the authority for the two-page interpretation.

Retail executable authority:

- `disc/SLUS_006.64` SHA-256: `dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119`
- raw retail `func_8002BF38` slice SHA-256: `31e7581dd65ae85b67dfc09ef5efde0e034fa81cb00e294a364a00339bd21080`
- local explanatory source: `src/slus_006.64/system/temp2.c:77-212`
- direct raw disassembly confirms `0x1200/0x1201` checks at `0x8002BFF8/0x8002C000`, coordinate loads at `0x8002C030..0x8002C158`, width/height state at `0x8002C178..0x8002C1F4`, and the `LoadImage` call at `0x8002C1FC`.

## File `0x50`: package and frame coverage

The six-section root has offsets `0x20, 0x5174, 0x6674, 0x6698, 0x6838, 0x6A54`, end `0x10714`. Retail `func_80022224` binds offsets `+4/+8/+0xC` as animation, frame, and palette data. Retail battle `func_800C0FAC` also binds the nested non-sound package at file offset `0x6698` with texture base `(896,256)` and fallback CLUT base `(0,500)`.

The primary frame table begins at `0x5174` with header `0x1409`. It has nine frames, ten embedded 4-bit tile parts per frame, and one 16-color palette at file offset `0x6678`, uploaded to runtime CLUT `(0,500)`. These 90 embedded tiles are decoded and composited in `primary-frames-contact-sheet.png`; they show the small Fei action/run cycle and no letters.

The nested package at `0x6698` has section offsets `0x14,0x24,0x19C`. Its frame table begins at file offset `0x66BC` with pre-backed header `0x8613` and nineteen frames. Retail `func_8001D53C` interprets these direct tile headers to select the tpage, depth, UV rectangle, and CLUT below.

| Frame | Record offset | Parts | Depth | Tpage origin | UV rectangle(s) | CLUT | Visible asset |
|---:|---:|---:|---:|---|---|---|---|
| 1 | `0x66E4` | 1 | 4 | `(896,256)` | `(1,201,30,30)` | `(0,464)` / `0x7400` | gray claw/impact stage |
| 2 | `0x66EE` | 1 | 4 | `(896,256)` | `(203,118,17,9)` | `(16,464)` / `0x7401` | small flesh/claw stage |
| 3 | `0x66F8` | 1 | 4 | `(896,256)` | `(201,134,17,17)` | `(16,464)` / `0x7401` | small flesh/claw stage |
| 4 | `0x6702` | 1 | 4 | `(896,256)` | `(1,6,20,19)` | `(16,464)` / `0x7401` | small flesh/claw stage |
| 5 | `0x670C` | 1 | 4 | `(896,256)` | `(25,7,20,19)` | `(16,464)` / `0x7401` | small flesh/claw stage |
| 6 | `0x6716` | 1 | 4 | `(896,256)` | `(48,4,22,28)` | `(16,464)` / `0x7401` | hand/claw stage |
| 7 | `0x6720` | 2 | 4 | `(896,256)` | `(163,2,19,28)`; `(76,6,26,20)` | `(16,464)` / `0x7401` | two-part hand/claw stage |
| 8 | `0x6732` | 1 | 4 | `(896,256)` | `(76,6,26,20)` | `(16,464)` / `0x7401` | hand/claw stage |
| 9 | `0x673C` | 1 | 4 | `(896,256)` | `(105,2,19,30)` | `(16,464)` / `0x7401` | hand/claw stage |
| 10 | `0x6746` | 2 | 4 | `(896,256)` | `(163,2,19,28)`; `(133,6,25,21)` | `(16,464)` / `0x7401` | two-part hand/claw stage |
| 11 | `0x6758` | 1 | 4 | `(896,256)` | `(133,6,25,21)` | `(16,464)` / `0x7401` | hand/claw stage |
| 12 | `0x6762` | 1 | 4 | `(896,256)` | `(1,96,35,20)` | `(16,464)` / `0x7401` | hand stage |
| 13 | `0x676C` | 1 | 4 | `(896,256)` | `(184,2,61,46)` | `(16,464)` / `0x7401` | larger hand/claw stage |
| 14 | `0x6776` | 1 | 4 | `(896,256)` | `(1,34,59,61)` | `(16,464)` / `0x7401` | larger hand/claw stage |
| 15 | `0x6780` | 1 | 4 | `(896,256)` | `(66,35,68,68)` | `(16,464)` / `0x7401` | larger hand/claw stage |
| 16 | `0x678A` | 1 | 4 | `(896,256)` | `(1,124,85,64)` | `(16,464)` / `0x7401` | extended claw stage |
| 17 | `0x6798` | 1 | 4 | `(896,256)` | `(137,48,84,59)` | `(16,464)` / `0x7401` | extended claw stage |
| 18 | `0x67A2` | 3 | 4 | `(896,256)` | `(89,119,50,28)`; `(1,96,35,20)`; `(150,114,47,22)` | `(16,464)` / `0x7401` | three-part extended claw stage |
| 19 | `0x67B6` | 1 | 8 | `(960,256)` | `(1,1,110,170)` | `(0,465)` / `0x7440` | red three-streak slash/energy image |

The third 4-bit CLUT `(32,464)` is visibly a high-contrast monochrome alternative, but none of these nineteen nested frame records selects it. Full tile record offsets, screen offsets, tile headers, and command bytes are in `decode-manifest.json`.

Retail-byte pins for the interpretation:

- `func_8001D53C [0x8001D53C,0x8001DAE8)`, SHA-256 `572485f0111f17626cc2debb40bd81482df1d71e5c2ba5192b7faa0d69a5b41a`, is the pre-backed frame decoder used by header bit `0x8000`.
- `func_8001DAE8 [0x8001DAE8,0x8001E148)`, SHA-256 `978ba2ac82bf8303091ad18c62ac1db65a1ff98fd7c9a3220f4f2e578325580f`, chooses the pre-backed path and otherwise uploads primary embedded tiles.
- `func_80022224 [0x80022224,0x800222BC)`, SHA-256 `8aae6b3ae2770208ac1def4251a6326db7132c34f954f20cbab4a695b8842d15`, binds each package's offsets and base coordinates.
- `disc/battle.bin` SHA-1 `124703d415f570f9cd8d7dad89c6a4eddfd7e6f0` matches `config/battle.yaml`. Retail `func_800C0FAC [0x800C0FAC,0x800C1140)` has SHA-256 `488fc4066deaa55d40789fc158188e243adf4cacf2257b9d8a59b02b9bf86f23`; raw instructions at `0x800C10BC..0x800C10FC` bind `(896,256)` and `(0,500)` through `func_80022224`.

## Inspectable artifacts

| Artifact | Purpose | SHA-256 |
|---|---|---|
| `primary-frames-contact-sheet.png` | all nine file-`0x50` embedded Fei frames | `44c7b213f97499605be7cfb3a69734939b4939015d9444318987cba0cf73d1a6` |
| `page-4bpp-cluts-contact-sheet.png` | complete 4-bit file-`0x51` page under all three CLUTs | `6ca68a0c1642b0b8afe4d605aebf1784b36648ec85f6e58400506065289cb844` |
| `page-8bpp-clut-x000-y465.png` | complete used 8-bit page with its 256-color CLUT | `7b6926b907867b75d412c58d271a4fa334c29d34402f4c45c12b28e8b5271cc9` |
| `subpackage-frames-contact-sheet.png` | all nineteen nested frames composited from exact records | `61e3f43a71240b70cba817f7745406727da3b06fd3029705f7402fdb61d3d8b8` |
| `decode-manifest.json` | extraction, sections, palettes, primary parts, and exact nested UV/CLUT records | `9c632335f06c5047244a9b6e0dceeb774754d6ab122e090e12b63607b04935af` |
| `decode_pilot_pair.py` | reproducible retail extraction and decoder | `df2789ee707ad09971fcd192d25a7e849dcfdc100797ff4f768aacc152aedca3` |

All PNGs are nearest-neighbor, lossless RGBA renderings. Checkerboard variants expose palette index zero as transparent. The index-map and per-frame PNGs remain alongside the contact sheets.

## Authority boundary and next question

This static retail-byte audit establishes the assets in this exact pair and rules them out as the source of `Hiyaaaaa!`. It does not identify the actual owner of the live pilot panel or dialogue, and it does not prove which `0x17` frame was displayed at a particular runtime tick. No current local retail framebuffer proves the expected spelling.

The next read-only runtime question should be reframed: at the no-bars pilot shot, identify every linked primitive in the panel's screen rectangle and trace each packet's tpage/CLUT or window owner back to its package/archive. An ordinary battle-window path may be adjacent, but current evidence does not establish it as this panel's owner. The generic `FontDrawLetters` release/debug branch remains excluded at this beat. Do not continue diagnosing text inside presentation `0x17` unless a new packet trace directly ties the missing pixels to this pair.

Repository `/var/home/blizz/Projects/xenogears-decomp-ai` was read only. All generated files are confined to `/tmp/xeno-pilot-atlas-20260906`.
