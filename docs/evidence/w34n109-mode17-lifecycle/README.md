# W34N109 — mode-17 lifecycle

## Scope and retail anchor

- Starting HEAD: `ff70f2220898f1d64a774e054d3b26f2667c3880`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail image: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Disassembly: `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`
- setup `[0x80082324,0x800826B4)`, 912-byte SHA-256:
  `cdc211e3b4dffd4108b2b9c70d9186efa0bc28c87d4b30805bfdfdc347ef3547`
- teardown `[0x800826B4,0x800827C8)`, 276-byte SHA-256:
  `09d655074bc9c63537a8bd823ff165c705b3bdea824a1682af91fa29b4108021`
- lifecycle `[0x80082324,0x800827C8)`, 1,188-byte SHA-256:
  `38110e35aabdad414d33198f1f298652c67123bfdb042ae8bd5971369a7a2f82`

## Production transcription

`wm_80082324` now owns retail mode-table slot 1 for mode 17. It restores the
exact framebuffer transition, second-wave relocation, object-pool and static
state setup, mode-17 position `(0x070A9000,0xFFEB8000,0x042AA000)`, graphics
and archive stages, WDS/song ownership transfer, ten scheduler registrations,
and common-tail order including `wm_80085FE0`.

The registration sequence is:

1. `0x800923A8 / 0x800925A0`
2. `0x800827C8 / 0x80076B34`
3. `0x800827EC / 0x800828DC`
4. five records of `0x80083214 / 0x80083264`
5. `0x800834D0 / 0x800834D8`
6. `0x80076A14 / 0x80076A1C`

`wm_800826B4` now owns mode-table slot 2. It restores audio/SEDS release,
world allocation teardown including `wm_80086124`, the five global frees,
and the mode-17 terminal tuple `F94E=617`, `F954=2`, `BBC4=1`, and
`F950=BD3A`.

The main-loop dispatcher resolves both lifecycle addresses symbolically.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n109_mode17_lifecycle.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- setup/teardown/full retail hashes: PASS
- transition arguments and exact stage order: PASS
- static state and mode-17 position: PASS
- WDS/song transfer and start: PASS
- all ten registration records, including five duplicates: PASS
- complete common-tail order: PASS
- SEDS/global teardown order and terminal state: PASS
- main-loop symbolic setup/teardown dispatch: PASS
- second-wave poll plus `wm_80076954` seam: PASS
- no host-pointer truncation in the lifecycle body: PASS
- M1–M9: all detected by named assertions

The normal product build completed with `LINK OK`.

## Natural entrance-17 route

The detached entrance-17 route reached the integrated lifecycle with no
mode-table slot-1 stub, registered all ten records, rendered and captured 120
frames, kept both upload-pump unknown counts at zero, fulfilled both capture
requests in-frame, and returned from the exact bounded loop normally.

Private callback census across the session-entry scheduler plus 120 recurring
passes:

- `0x800827C8`: 121 unresolved initializer hits
- `0x800827EC`: 121 unresolved initializer hits
- `0x80083214`: 605 unresolved initializer hits (five records)
- `0x800834D0`: 121 unresolved initializer hits
- shared `0x80076A1C`: 120 successful update executions

Captures:

- frame 60:
  `/home/blizz/dev/xenogears-decomp/scratchpad/w34n109_mode17_capture/world-frame-000060.bmp`
- frame 60 SHA-256:
  `5851ca7ca04f8f77b7e8eb1b9a22a4510ad97205109f0d34bdf462733b66d2d0`
- frame 120:
  `/home/blizz/dev/xenogears-decomp/scratchpad/w34n109_mode17_capture/world-frame-000120.bmp`
- frame 120 SHA-256:
  `6a0f814c9a83160ea45b66338701e99bac400eb3e7b0b5196b2bd79907561b6a`

The captures are deterministic products of the now-live lifecycle but are not
a visual acceptance gate: the scene is largely blank/inverted while the four
private callback families remain stubbed.

## Verdict and next target

`MODE17_LIFECYCLE_RESTORED_PRIVATE_FRONTIER_EXPOSED`

The smallest remaining private mode-17 pair is
`0x800834D0 / 0x800834D8`; it is the next exact target.
