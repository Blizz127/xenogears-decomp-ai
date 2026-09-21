# W34N52 — presented sprite orientation discriminator

## Result

`PRESENTATION_CORRECT_CAPTURE_READBACK_INVERTED`

Starting HEAD was `2259cd173952fb240983ac8eb9b7001d204e03ca` on
`experiment/worldmap-open-gates-20260823`, equal to origin. The natural
world-map location label is an `SPRT` packet and its texture reaches the
renderer intact. The presented window is correct. The game-owned BMP capture
was the observer with inverted vertical row order.

This supersedes the preliminary interpretation that `MakeTexcoordRect` needed
its V endpoints reversed. That interpretation came exclusively from a
Y-flipped `glReadPixels` artifact.

## Packet and texture lineage

The natural label packet is a semi-transparent `SPRT` (`code=0x65`):

```text
xy       = (160,120)
tpage    = 0x001f
uv       = (0,128)
clut     = 0x7813
size     = 104 x 13 at full width
guest    = 0x8011c074 / 0x8011c09c by render context
OT roots = 0x800a2228 / 0x800a3230 by render context
```

The packet is linked into and decoded from the same OT and passes the expected
length check. The 36-word-by-13-row texture source at VRAM `(960,384)` and its
16-word CLUT at `(304,480)` are byte-identical between CPU VRAM and the active
GPU texture. PSX low-nibble-first 4bpp decoding produces an upright label
raster. Horizontal texel order is correct.

`MakeVertexRect` emits top-left, bottom-left, bottom-right, top-right. The
existing `MakeTexcoordRect` pairing is correspondingly:

```text
upper-left  = (u,     v)
lower-left  = (u,     v + h)
lower-right = (u + w, v + h)
upper-right = (u + w, v)
```

The tracked `psycross_sprite_v_orientation.patch` makes those endpoint names
explicit so the focused mutant certificate can guard the mapping, but it does
not change the normal build's output. Polygon UV paths remain independent.

## Decisive same-run observation

A normal current binary ran with the accepted field-to-world schedule and no
debugger. An external watcher froze the 640x480 SDL window immediately after
the port reported fulfillment of world frame 120.

```text
game BMP:
  scratchpad/w34n52_live_parity_capture/world-frame-000120.bmp
  SHA256 0b5f7308e844157bac4fde8ff33093f99d27553d691d0d8f037bbb77b490581b

same-run X11 window:
  scratchpad/w34n52_live_parity_window.png
```

The live window visibly reads `Mountain Path` upright. The game BMP displays
the entire framebuffer upside down, including terrain, minimap, and label.
Comparing its decoded rows with the X11 window gives 920,263 differing color
channels as stored versus 136,797 after reversing the BMP's row order; the
remaining difference is expected because the watcher freezes just after the
capture log is emitted rather than inside the presentation call.

An earlier diagnostic that reversed only the sprite V endpoints made the BMP
label look upright, but it compensated for the faulty observer and would
invert sprites in the actual window. It is rejected as a production fix.

## Focused sprite certificate

Runner: `pc_port/tests/run_w34n52_sprite_v.sh`.

The certificate compiles the real `MakeTexcoordRect` body. It checks the
observed 104x13 label mapping, 8-bit U/V page clamping, page/CLUT metadata,
brightness/dither state, read-only input, write canaries, polygon-marker
exclusion, raw-identity clearing, and bilinear metadata.

```text
W34N52 O0 PASS
W34N52 O2 PASS
W34N52 UBSan PASS
W34N52 M1 DETECTED assertion=label.vertical_direction
W34N52 M2 DETECTED assertion=label.bottom_v_uses_exclusive_height
W34N52 M3 DETECTED assertion=label.left_u_is_base
W34N52 SPRITE V CERTIFICATE PASS; O0/O2/nonrecovering UBSan;
focused warnings clean; M1-M3 DETECTED
```

M1 reverses V, M2 shortens the bottom endpoint by one texel, and M3 reverses
U. The normal PC port links.

## Runtime bound

The base-V build completed the accepted natural world session exit at frame
601 (`D7CC=0`) with captures at frames 60 through 600 and no upload-pump
unknowns. A second run reproduced frame 120 byte-for-byte before the watcher
stopped it. Representative pre-correction capture hashes are:

```text
frame 60  024259b1fa1b0b8b06a626a38b4cfee8bf6b7cdccddc7166a52c11c72b580f34
frame 120 0b5f7308e844157bac4fde8ff33093f99d27553d691d0d8f037bbb77b490581b
frame 600 0a14ff0e9fc9169eb4bd186445b9005ddb82a7017440361a3684e73ef26be4cf
```

These hashes deliberately record the inverted capture observer and are not a
visual acceptance oracle.

## Next exact target

Fix screenshot readback at `PsyX_TakeScreenshotPath`: OpenGL returns rows from
the lower-left origin, while the SDL surface treats the first supplied row as
the image top. The bounded correction is to reverse complete RGBA rows after
`glReadPixels`, leaving rendering and sprite UVs untouched.
