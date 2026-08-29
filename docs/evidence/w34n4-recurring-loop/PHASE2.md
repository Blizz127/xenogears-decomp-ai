# W34N4 Phase 2 — retail mode callbacks

## Authority and dispatch mechanism

This phase uses the same authoritative `disc/world_map.bin` image and direct
decode recorded in Phase 1. The static callback records are resident in that
image at `0x8009A058`. Retail `WorldMapMain` reads mode index `0x8009C5A8`,
computes `mode * 12`, and uses three words per record:

| record word | retail use |
|---|---|
| `+0x00` (`0x8009A058`) | pre-session callback, null-checked at `0x80071020` |
| `+0x04` (`0x8009A05C`) | slot-1/session-entry callback, unconditional `jalr` at `0x8007105C` |
| `+0x08` (`0x8009A060`) | slot-2/session-exit callback, unconditional `jalr` at `0x800710C4` |

This is a fixed overlay data table, not runtime callback registration and not
the 64-slot `0x80097800` scheduler pool.

The retail image contains 19 contiguous pointer-shaped records, indices 0
through 18:

| mode | pre-session `+0` | slot 1 `+4` | slot 2 `+8` | full target present in port? |
|---:|---|---|---|---|
| 0 | `0x80071CDC` | `0x80072238` | `0x8007299C` | pre-session transcribed; slot 1 partial slices only; slot 2 absent |
| 1 | `0x80071CDC` | `0x80072238` | `0x8007299C` | same |
| 2 | `0x80071CDC` | `0x80072238` | `0x8007299C` | same |
| 3 | `0x80071CDC` | `0x80072238` | `0x8007299C` | same |
| 4 | `0x80071CDC` | `0x80072238` | `0x8007299C` | same |
| 5 | `0x80071CDC` | `0x80072238` | `0x8007299C` | same |
| 6 | `0x80071CDC` | `0x80072238` | `0x8007299C` | same |
| 7 | `0x80071CDC` | `0x80072238` | `0x8007299C` | same |
| 8 | `0x80071EF0` | `0x80077214` | `0x80077480` | pre-session transcribed; slot targets absent |
| 9 | `0x80071EF0` | `0x80077A64` | `0x80077CC0` | pre-session transcribed; slot targets absent |
| 10 | `0x80071EF0` | `0x80078A60` | `0x80078D24` | pre-session transcribed; slot targets absent |
| 11 | `0x80071EF0` | `0x80077214` | `0x80077480` | pre-session transcribed; slot targets absent |
| 12 | `0x80071EF0` | `0x8007BF50` | `0x8007C260` | pre-session transcribed; slot targets absent |
| 13 | `0x80071EF0` | `0x8007FF70` | `0x80080218` | pre-session transcribed; slot targets absent |
| 14 | `0x80071EF0` | `0x8007A5DC` | `0x8007A8AC` | pre-session transcribed; slot targets absent |
| 15 | `0x80071EF0` | `0x8007D918` | `0x8007DCE0` | pre-session transcribed; slot targets absent |
| 16 | `0x80071EF0` | `0x80080D00` | `0x8008106C` | pre-session transcribed; slot targets absent |
| 17 | `0x80071EF0` | `0x80082324` | `0x800826B4` | pre-session transcribed; slot targets absent |
| 18 | `0x80071EF0` | `0x8008355C` | `0x800837DC` | pre-session transcribed; slot targets absent |

The next 12 bytes do not form KSEG function pointers, establishing the end of
the contiguous pointer records in the image. No symbol declaring the array
length was found, so the formal range check that guarantees `C5A8 <= 18` is
**undetermined**. It would require the writer/range proof for `0x8009C5A8`.
The accepted route uses base-world mode 0.

## `0x80072238`: base-world slot 1

### Retail

Retail boundary is exactly `[0x80072238,0x8007299C)`, 0x764 bytes / 473
instructions. It is a session-entry/setup callback, not the displayed-frame
loop. The first direct call is framebuffer/GTE initializer `0x80072BB0`; the
body then performs the setup sequence whose pieces W34N3 currently exposes as
gates:

1. framebuffer/display initialization and initial move/sync;
2. second archive wave, readiness wait/fixup, and object-pool creation;
3. constant matrix/state publication, cross-product and mode setup;
4. GPU assets, object matrix, third archive wave, BSS constants, primitive
   templates, CLUT/work buffers, FT4 pools, tables, upload records, draw
   packets, and `0x80088F64`;
5. archive synchronization, first WDS consumer, and `ArchiveSetIndex(36,0)`;
6. terrain position, CD-work drain, ready-buffer consumption, and mode audio;
7. convergence table passes, callback-pool registration, and common tail
   through palette transfer; then return at `0x80072994`.

This ordering is established from retail calls, not inferred from W34N3. It
also explains why the W34N3 gate ladder matches this function so closely: the
accepted port path has been invoking bounded slices of this callback outside
the retail table dispatch.

### Port

No complete function with entry `0x80072238` exists at HEAD. Exact-address
search finds only the forbidden-path witness and comments. Many bounded
subranges are transcribed under the W34N3 item-1/item-2 functions, but they are
not integrated as a callable slot-1 body.

`world_map_main_loop_71034.c:57-73` substitutes
`ml_guest_stub(..., default_return=0)`. It additionally skips the slot-1 call
for `session == 1`, on the explicit assumption that gated initialization has
already completed it externally. A later session would log the stub and do no
setup.

### State consumed and produced

The callback consumes state produced before it:

- mode `C5A8`, world tuple/entrance-derived `C894`, and initial world globals:
  W34N3 item 1 `INIT` / normal selector and pre-loop setup;
- the pre-session `0x80071CDC` archive request state for modes 0..7:
  item 1 `MODE_INIT`;
- archive request/loaded-buffer pointers used by its second/third waves:
  item 1 `SECOND_WAVE` and `THIRD_WAVE` data lifecycle;
- static overlay tables/matrices loaded into guest RAM: loader/static-data
  substrate already established before W34N3.

It does **not** merely consume W34N3 item 2. Retail `0x80072238` is the owner
that produces the framebuffer, terrain, convergence, common-tail, and initial
scheduler state represented by item 2. The current gates split that producer
apart.

## `0x8007299C`: base-world slot 2

### Retail

Retail boundary is exactly `[0x8007299C,0x80072BB0)`, 0x214 bytes / 133
instructions. It is post-session teardown, called only after `0x800712D0`
returns. It:

- conditionally fades/stops the active audio manager based on `D7CC`;
- stops sound/SEDS state and frees its buffer;
- walks all 64 scheduler records and frees each non-null `slot+0x4C` object;
- conditionally performs the `D7CC==1` transition helper;
- calls world teardown helpers at `0x80092DD0`, `0x800931B0`, `0x80084818`,
  `0x80086124`, `0x80086568`, `0x800866C8`, `0x8007474C`, `0x80074F04`,
  `0x800750DC`, `0x80088FF4`, `0x80089128`, and `0x80097D64`;
- frees graphics/work/pool allocations from world globals;
- finishes with `0x800976A0` and `0x800960BC`, then returns.

### Port

No complete function with entry `0x8007299C` exists under any name. The
`COMMON_TAIL_P5` body returns `0x8007299C` as an overlay-local boundary; it is
not a transcription of the function starting there. Exact-address search
finds no alternate body.

The open-loop dispatcher substitutes the same observable default stub. More
importantly, a 120-frame bounded exit breaks before slot 2, so the accepted
route does not even execute that substitute. Natural session teardown is
therefore absent, not merely stubbed.

### State consumed

The teardown consumes:

- `D7CC`, produced/updated by the retail frame driver and scheduler callbacks;
- active audio manager and SEDS/WDS ownership (`0x80062528`, `0x8006259C`),
  produced by W34N3 item 1 `FIRST_WDS_CONSUMER` and item 2
  `READY_BUFFER_CONSUME` / `MODE_AUDIO_SETUP`;
- scheduler pool pointer `0x8009BE24` and its 64 `+0x4C` objects, produced by
  item 1 `OBJECT_POOL` plus item 2 convergence registration/scheduler work;
- graphics, upload, FT4, heap-table, and callback-package allocations created
  across item 1 `GFX_WORK_BUFFERS` through `88F64` and item 2 common tail.

It is strictly downstream of both W34N3 backlog items for natural execution.

## Other mode callbacks

All mode-8..18 slot-1 and slot-2 addresses in the table were searched by
retail address. None has a production body in `pc_port/src` or compiled
upstream `src`; the generic open-loop dispatcher sends every one to
`ml_guest_stub`. Their internal state dependencies are **undetermined** in this
rung because their bodies were not decoded. Establishing those dependencies
would require a separate decode of each address range and a proof of which
non-base modes are reachable from the current product route.

This unknown does not block a base-world-mode transcription: modes 0..7 all
resolve to the same `0x80072238`/`0x8007299C` pair. It does block claiming a
general retail `WorldMapMain` implementation for all 19 visible mode records.
