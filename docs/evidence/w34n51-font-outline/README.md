# W34N51 — retail system-font outline polarity

## Result

`RASTER_RESTORED / 4BPP_NIBBLE_ORDER_RESIDUAL`

Starting HEAD was `071ad59af3e547ebc77f47e4b80070f99da16c87` on
`experiment/worldmap-open-gates-20260823`, equal to origin.  This rung fixes
the CPU font rasterizer defect that turned the world-map location label into
solid colored blocks.  It does not claim that the label is fully readable on
screen: the corrected raster is still sampled with the four texel nibbles in
each 16-bit 4bpp word in reverse order by the port renderer.

## Retail authority and fault

Retail authority is `disc/slus_006.64`, with the system-font body decoded at
`func_80034FFC`.  In each of the three outline groups for both interleaved
texture pages, retail adds the outline shade when the corresponding glyph bit
is present.  Representative branch ranges are:

- row/page 0: `0x8003518C..0x80035198`;
- row/page 1: `0x80035474..0x80035480`.

The old C ternaries had the predicate reversed.  They added `0x2`/`0x8` to
empty texels and omitted it from present texels, filling the label rectangle
instead of outlining its glyphs.  All six copies of that transcription error
are corrected in `src/slus_006.64/system/system.c`.

The natural label source was independently checked before changing the
rasterizer:

```text
area          = 1
string table  = 0x800CB5F0
entry         = 0x800CB69A
decoded text  = Lahan Village
upload width  = 26 words
row count     = 1
stride        = 36 words
CLUTs         = 30739 / 30740
```

Retail PCSX-Redux at the equivalent seam renders the same string correctly.
The retail and port row geometry, sprite packet fields, palette selectors,
font glyph bytes, and active upload dimensions agree.

## Focused certificate

Runner: `pc_port/tests/run_w34n51_font_outline.sh`.

The certificate links the full production `system.c` body and verifies an
empty glyph does not fill either interleaved raster page, inactive page bits
remain unchanged, the glyph input is read-only, writes remain inside the
authorized buffer, padding is untouched, and a present glyph still renders.

```text
CERTIFICATE O0/O2/UBSan PASS
focused test warnings clean; legacy production TU warning-suppressed
M1 reversed row-0 outline polarity: DETECTED by row0.empty-glyph-does-not-fill-outline
M2 reversed row-1 outline polarity: DETECTED by row1.empty-glyph-does-not-fill-outline
W34N51 font outline certificate PASS; M1-M2 DETECTED
```

The normal PC port reports `LINK OK`.

## Retail/port raster comparison

Completed work buffers were captured at the same label-upload seam in the
port and in retail PCSX-Redux.  After masking unrelated allocator tail bytes,
the active 26-word by 13-row glyph plane compares exactly:

```text
active words compared = 338
mismatches            = 0
```

Rendering those words directly in PSX low-nibble-first order visibly spells
`Lahan Village`.  Rendering the same words with each four-nibble group
reversed reproduces the remaining port screenshot corruption.  This proves
the CPU raster and its source data are now correct and moves the next fault to
the 4bpp upload/sampling path.

## Natural regression

The final normal binary used the accepted detached route with scripted input
and reached the natural world-state exit:

```text
natural state exit: frames=601 D7CC=0
capture requests fulfilled on frames 60, 120, and 600
adapter abort/boundary diagnostics: none
upload unknowns: 0
```

Capture hashes are:

```text
frame 60  024259b1fa1b0b8b06a626a38b4cfee8bf6b7cdccddc7166a52c11c72b580f34
frame 120 0b5f7308e844157bac4fde8ff33093f99d27553d691d0d8f037bbb77b490581b
frame 600 0a14ff0e9fc9169eb4bd186445b9005ddb82a7017440361a3684e73ef26be4cf
```

The solid status blocks are gone.  At frames where the location label is
visible, each consecutive four-pixel group is still reversed horizontally.
The process reached the known post-world `SoundHandleError` stub after the
natural exit and was terminated there; no whole-process `rc=0` is claimed.

## Next exact boundary

`W34N52` must prove the actual 4bpp texel-index mapping at the renderer seam
with a deterministic asymmetric four-nibble canary, then repair only the
first point where the port differs from PSX low-nibble-first semantics.  A
global shader change is not authorized from this evidence alone because it
could affect every existing 4bpp texture.
