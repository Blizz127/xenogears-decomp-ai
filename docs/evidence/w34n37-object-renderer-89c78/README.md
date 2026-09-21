# W34N37 — complete retail `wm_80089C78` object renderer

Verdict: **FULL_BODY_TRANSCRIBED_AND_CERTIFIED**.

## Anchor and retail authority

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `b563913fb9cfecaa5627298b5d36a9a2da4f8b8f`
- Static authority:
  `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`, retail function
  `[0x80089C78,0x8008A2C8)`
- Date: 2026-08-30

W34N36 repaired and certified the dynamic-pool/matrix/wrap prefix through
retail PC `0x80089F38`.  This rung transcribes the remaining
`0x80089F40..0x8008A2C8` projection, culling, packet-construction, and OT
publication body.

## Retail body

For each nonzero-state record in the 256 × `0x4c` pool at `D_8009BDF4`, the
completed helper now performs the retail sequence:

1. Store the 32-bit scale VECTOR at scratch `0x1F800098` and scale the
   per-record model matrix.  This corrects W34N36's temporary host-stack
   scale VECTOR.
2. Copy four 8-byte vertices from signed model index
   `0x8009B040 + index*0x20` to scratch `+0x00..+0x1f`.
3. Wrap camera-relative X/Z at scratch `+0x88/+0x90`.  Y is not part of the
   wrap helper's VECTOR and is not written at `+0x8c`; it is loaded directly
   from record `+0xc` when the projection SVECTOR is packed.
4. Rotate `(wrapped_x, y, -wrapped_z)` by camera R, add camera T, and install
   that translation into the scaled model matrix.
5. Project all four vertices, reject GTE bit-31 failure, reject when no signed
   X is below 320 or no signed Y is below 216, and reject `SZ3 >= 0x0c00`.
6. Compact accepted 40-byte `POLY_FT4` packets in the active pool selected by
   `D_8009D7F0` from `D_8009BE1C/D_8009BE20`.
7. Write final XY, record RGB, record tpage, and the four signed-index UV
   halfwords from `0x8009AFF0 + index*8`.
8. Link the packet into `draw_record->OT[SZ3 >> 4]` through the port's
   domain-aware guest primitive linker, preserving retail tag high bytes.

The function argument remains unused, matching the retail body.

## Focused certificate

The production-linked certificate passes O0, O2, and nonrecovering UBSan with
strict warnings.  Its synthetic route projects five live records: two pass,
while one each fails the flag, screen, and depth gates.  It proves exact
compaction around the rejected records, negative model-index handling, matrix
order and translation, packet fields, guest OT addresses, and pool
read-only behavior.

Thirteen named mutants are detected:

| mutant | failure mode |
|---|---|
| M1 | fixed `0x8009B040` record pool |
| M2 | inverted zero/nonzero state guard |
| M3 | wrap object storage instead of scratch |
| M4 | wrong record position fields |
| M5 | missing camera subtraction |
| M6 | host-stack scale VECTOR |
| M7 | unsigned model-index table arithmetic |
| M8 | skipped projection FLAG gate |
| M9 | conventional point-in-screen gate instead of retail upper-edge test |
| M10 | inverted depth gate |
| M11 | next UV record |
| M12 | raw host-derived OT link |
| M13 | noncompacted packet cursor |

The runner now deletes each mutant's old executable/stdout/stderr before
compilation.  This prevents a compile failure from being misclassified by a
stale named assertion; that integrity issue was found and corrected during
this rung.

## Native evidence

### State-only diagnostic

A temporary detached diagnostic (removed before the final build) reported:

```text
frame 1:   accepted object FT4s=0, adapter packets=390,   aborts=0/0/0/0
frame 2:   accepted object FT4s=0, adapter packets=603,   aborts=0/0/0/0
frame 60:  accepted object FT4s=0, adapter packets=15946, aborts=0/0/0/0
frame 120: accepted object FT4s=0, adapter packets=32151, aborts=0/0/0/0
```

The packet counter is cumulative in this diagnostic.  The current natural
state therefore exercises every transformation/gate but does not naturally
publish one of this helper's object quads.  Publication is proven by the
focused certificate, not claimed from this route.

### Determinism discriminator

Two detached 120-frame runs without `XENO_WORLD_TEST_INPUT` produced identical
new capture digests:

```text
frame 60  e237397ff0436c38f6f6c3c867a086bd08db5c00baca59753f6f8ddc64bb59a0
frame 120 0154437ad8d2d11fdd5019b183a32111315f305f7019714c44de8466e304b368
```

The full retail tail changes downstream scratch/GTE state even when its own
quad is rejected, so this no-world-input substrate legitimately differs from
the old prefix-only binary.  The result reproduced byte-for-byte.

### Accepted frame-601 lifecycle regression

The final normal binary then reran W34N36's exact accepted substrate,
including world-relative schedule
`0:0x2000,600:0x20,601:0`.  It retained the W34N36 captures exactly:

```text
frame 60  4310197d3881367bc7859ef174dcf4b9afa8759f0b7dc2ca3eea94b0f84f1521
frame 120 8039c2f96b88e09491eaf1cba049d0efd829f146aa943da744f56c93e85b1c16
frame 600 2b636ae21af4bf0003f60d5e53f8ffd9a042fc5a72457e4c096cff19814e1a38
```

At frame 601 the unseeded `0x0020` rising edge again produced natural
`D554=0`, completed base slot-2 teardown, and selected the D7CC-zero terminal
lane.  No primitive-link or OT-adapter abort was logged.

## Final hygiene and bound

- focused full-body certificate: PASS O0/O2/UBSan, M1-M13 detected;
- normal product build: `LINK OK`;
- all `XENO_DIAG_W34N37` source removed;
- `git diff --check`: clean.

The remaining runtime bound is explicit: no object quad from this helper is
visible in the current accepted state because all are naturally culled.  A
future route that makes one visible should retain a bounded packet witness,
but no unimplemented instruction remains in retail `wm_80089C78`.
