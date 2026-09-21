# W34N111 — mode-17 command stream

## Scope and retail anchor

- Starting HEAD: `9d89bed19bc3ad6e3601becf0cd43a16004f0e36`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail image: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Disassembly: `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`
- initializer `[0x800827C8, 0x800827EC)`, 36-byte SHA-256:
  `97dc80eafdca7f8ab4ff87f912647188fee1e76de91b8c775e2e44117223dbbf`
- updater `[0x80076B34, 0x80076BC4)`, 144-byte SHA-256:
  `69ec010d238d8012c84864a98d0691d538fdb131eeae068ec9ab931e04a8a3df`
- handlers `[0x80076BC4, 0x80076DA4)`, 480-byte SHA-256:
  `f0470ef0465fbe7a9c1ee0b608a83b7159ac8fd7155c0b75a242d5836195659d`
- 12-entry retail table `[0x8009A3C0, 0x8009A3F0)`, 48-byte SHA-256:
  `c131683500b8887634be850125d2c76fe3ae98e6ac8908c4fdb90d25d7912c3e`

## Production transcription

`wm_800827C8` restores the initializer that publishes script cursor
`0x8009A758` into scheduler-record offset `+0x50`.

`wm_80076B34` restores retail's halfword-counted command interpreter. It
advances the cursor by twice the preceding handler return, signed-decodes the
three arguments, dispatches through the exact 12-command ordering, and keeps
executing commands in one scheduler pass until a handler returns zero.

The complete direct handler table is transcribed with no hidden callback
stubs:

| Opcode | Retail address | Effect | Advance (halfwords) |
| --- | --- | --- | --- |
| 0 | `0x80076BC4` | clear `D554` and `D7CC`; stop | 0 |
| 1 | `0x80076BDC` | seed/decrement record timer `+0x22` | 0 or 2 |
| 2 | `0x80076C18` | `wm_80097770(a1, a2)` | 4 |
| 3 | `0x80076C3C` | publish fixed-point world position | 4 |
| 4 | `0x80076C68` | publish three scratch angle halfwords | 4 |
| 5 | `0x80076C88` | create/update marker from scratch angles | 2 |
| 6 | `0x80076CB4` | clear marker record flags | 2 |
| 7 | `0x80076CD4` | clear marker group | 2 |
| 8 | `0x80076CF4` | audio-manager level transition | 4 |
| 9 | `0x80076D1C` | play bank-qualified sound | 2 |
| 10 | `0x80076D50` | control bank-qualified sound | 4 |
| 11 | `0x80076D8C` | publish `CCA4` / `D3CC` state | 4 |

The scheduler resolves both the initializer and updater symbolically.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n111_mode17_command.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- all four retail code/data hashes: PASS
- initializer cursor and no-side-effect contract: PASS
- timer seed, hold, expiry, and same-pass continuation: PASS
- exact command cursor scale and all 12 dispatch mappings: PASS
- fixed-point position and scratch-angle writes: PASS
- marker create/clear operations: PASS
- audio manager, bank-qualified sound, and sound-control arguments: PASS
- state publication and terminal exit: PASS
- scheduler initializer/updater resolution: PASS
- M1–M10: all detected by named assertions

The W34N110 transition/drain regression certificate passed unchanged. The
normal product build completed with `LINK OK`.

## Natural entrance-17 route

The detached entrance-17 route completed the exact 120-frame bounded loop:

- `0x800827C8`: one successful initializer execution
- `0x80076B34`: 120 successful recurring updater executions
- unresolved stub hits for either address: zero
- `0x800834D8`: 120 successful W34N110 updater executions
- upload-pump unknowns: zero
- frame-60 and frame-120 capture requests fulfilled in their requested frame
- bounded loop returned normally

The remaining private callback census is:

- `0x800827EC`: 120 unresolved initializer hits
- `0x80083214`: 605 unresolved initializer hits (five records)

Captures:

- frame 60:
  `/home/blizz/dev/xenogears-decomp/scratchpad/w34n111_mode17_capture/world-frame-000060.bmp`
- frame 60 SHA-256:
  `5851ca7ca04f8f77b7e8eb1b9a22a4510ad97205109f0d34bdf462733b66d2d0`
- frame 120:
  `/home/blizz/dev/xenogears-decomp/scratchpad/w34n111_mode17_capture/world-frame-000120.bmp`
- frame 120 SHA-256:
  `6a0f814c9a83160ea45b66338701e99bac400eb3e7b0b5196b2bd79907561b6a`

The hashes remain bit-identical to W34N109/W34N110. The script is executing,
but its visible dependencies remain masked by the two unresolved private
families; unchanged images are not claimed as visual acceptance.

## Verdict and next target

`MODE17_COMMAND_STREAM_RESTORED`

Only two private callback families remain. The five-instance
`0x80083214 / 0x80083264` pair is the smaller direct pair (700 retail bytes)
than the `0x800827EC / 0x800828DC` state machine (about 1.9 KiB before its
direct helpers), so it is the next implementation target.
