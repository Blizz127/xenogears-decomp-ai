# W34N53 — OpenGL capture readback orientation

## Result

`CAPTURE_MATCHES_PRESENTATION`

Starting HEAD was `75d9f493d483ba8c58bc8e22702be9fe78d888bf` on
`experiment/worldmap-open-gates-20260823`, equal to origin. W34N52 proved that
the SDL window presents world sprites and terrain upright while
`PsyX_TakeScreenshotPath` archived the whole framebuffer upside down.

## Root cause

`PsyX_TakeScreenshotPath` and the interactive `PsyX_TakeScreenshot` passed the
buffer returned by `glReadPixels` directly to an SDL surface. OpenGL places
the lower framebuffer row first; the SDL surface treats the first supplied
row as its image top. The mismatch inverted every archived frame vertically.

This was an observer defect, not a renderer defect. `MakeTexcoordRect`, terrain
geometry, OT traversal, texture upload, and presentation remain unchanged.

## Production correction

`PsyX_FlipCaptureRows` reverses complete four-byte pixel rows in place after
OpenGL/GL ES readback and before SDL serialization. It handles even and odd
heights, preserves an odd image's center row, and is a no-op for null or
degenerate inputs. Both named-path world captures and the interactive
screenshot key use the same helper.

The durable vendor change is
`pc_port/patches/psycross_capture_readback_orientation.patch`, applied by
`pc_port/build_port.sh` after the presentation-boundary capture hook. A fresh
vendor-baseline replay applies the patch cleanly. The repository-wide replay
comparison still reports the previously documented unrelated live-vendor
differences in `PsyX_main.cpp`, `LIBCD.C`, and `LIBGPU.C`.

## Focused certificate

Runner: `pc_port/tests/run_w34n53_capture_rows.sh`.

The certificate compiles the real `PsyX_main.cpp` production helper with
function/data sections and retains only the tested seam. It proves full rows
for even and odd heights, odd center-row preservation, four-byte pixel stride,
write canaries, degenerate no-write behavior, and the involution property.

```text
W34N53 O0 PASS
W34N53 O2 PASS
W34N53 UBSan PASS
W34N53 M1 DETECTED assertion=even.outer_rows
W34N53 M2 DETECTED assertion=even.interior_rows
W34N53 M3 DETECTED assertion=even.pixel_stride
W34N53 CAPTURE ROW CERTIFICATE PASS; O0/O2/nonrecovering UBSan;
focused warnings clean; M1-M3 DETECTED
```

M1 skips the flip, M2 omits the innermost row pair, and M3 uses a three-byte
instead of four-byte pixel stride. The normal PC port reports `LINK OK`.

## Same-run presentation proof

A plain native run used the accepted field-to-world and world-input schedules.
An external watcher stopped the process immediately after frame-120 capture
fulfillment and read the exact 640x480 SDL window through X11.

```text
capture: scratchpad/w34n53_live_capture/world-frame-000120.bmp
window:  scratchpad/w34n53_live_window.png
```

After decoding the BMP into display row order, all 480 rows and all RGB color
channels are identical to the live window:

```text
dimensions         = 640 x 480
differing channels = 0
exact rows          = 480
pixel exact         = YES
```

Both artifacts visibly show upright `Mountain Path`, terrain, and minimap.
The new decoded frame is exactly the vertical row reversal of W34N52's
pre-correction frame, with no other pixel transformation.

## Natural-session acceptance

The final normal binary reached all ten 60-frame capture boundaries and the
natural world-session exit at frame 601 (`D7CC=0`). It reported no capture
error, pending request, upload-pump unknown, or adapter-abort diagnostic.
Every corrected frame is exactly the vertical row reversal of the
corresponding W34N52 pre-correction frame. A second run reproduced frames 60
and 120 byte-for-byte.

```text
frame 60  15fdbff279b0bb5a83d0e049ba6bce0ca3397a94f6239bdea51424a23b430d04
frame 120 50512b12deef85444cc69c92b924fc88021261f46146887e95a553977ce009c5
frame 180 9631bc3446d7b2e9e153d6ead29b60bc4f4c0cf4073c4703060401375731444e
frame 240 c1b0e95730c85e5f3ab08e358ee72bea2b629f2ceb8ad1efeb79dc97625269df
frame 300 d7ace4cd669a7c7379e575c861ae2944ef2078c5da5c725042f4a476e205a0b2
frame 360 bc9db3123f4d56852e5544a6643d6b6497137b9a29e8995fb572182f51573d8e
frame 420 21607a21862f950e2e26b7ee6e9b1a2fd490f5d00740a10cad083f383ccd7834
frame 480 5a3f53beeb6028f7df8a6c944c38c41f7ced8bd7854e41c7e7108a8f2389bb0f
frame 540 4b98b746e5deeabc86490318c640d84845693073302a3dc38775a3a87fec8f1c
frame 600 894f9da798e4cdb01e093834fda64ea49a8730a068e5a3827aaea507a0d15c93
```

These replace all earlier vertically inverted capture baselines.
