# W34N39 — complete retail `wm_80089748` particle spawner

Verdict: **FULL_BODY_TRANSCRIBED_AND_CERTIFIED**.

## Anchor and authority

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `ae0d1d6dbdd4a78f8378b02c10886bba7336bc34`
- Retail authority:
  `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`,
  function `[0x80089748,0x80089C78)`
- Date: 2026-08-30

W34N38 first completed the adjacent leaf `wm_80089580`. This rung then
transcribed the entire 332-instruction source-to-particle path that calls it.

## Retired approximation

The former C body was not a bounded representation of retail. In particular,
it:

- walked 256 records at `0x4c` stride, while retail walks 512 effect-source
  records at `0x54` stride;
- reduced the two-half timer state machine to an unrelated 16-bit countdown;
- treated the source table and dynamic particle pool as the same record type;
- used host-stack matrices/vectors and only one approximate transform;
- omitted both random normalized-vector constructions, their fixed/random
  amplitude rules, the second endpoint/direction construction, exact payload
  publication, source accounting, and most copied particle state.

## Retail sequence now implemented

For each active source record at `D_8009BCC0`, the completed helper:

1. applies the `+4` packed high/low timer stages and deactivates bit 7 only
   when both expire;
2. advances/reset the `+0x12/+0x10` spawn timer and applies retail's
   `+8 > 0`, `+0xa < +8`, and nonzero `+0xe` eligibility predicates;
3. searches all 256 dynamic `0x4c` particle records from `D_8009BDF4`,
   testing the packed owner halfword at particle `+6`;
4. constructs the source rotation matrix at guest scratch `+0xf0`, applies
   it to source vectors `+0x24` and `+0x2c`, and preserves retail random
   call order (Y when enabled, then X, then Z);
5. normalizes each random vector, selects fixed or modulo-random amplitudes
   from `+0x40/+0x42`, constructs the two endpoints, normalizes their
   direction, scales it by `+0x34`, and computes the heading with
   `ratan2`;
6. publishes the exact signed/copy fields through particle `+0x48`,
   increments source `+0xa`, stores the updated packed timers, and finally
   calls `wm_80089580` once.

The source flags byte at `+0x4f` is correctly treated as the high byte of the
word copied from source `+0x4c`.

## Focused certificate

`pc_port/tests/run_w34n39_89748.sh` passes O0, O2, and nonrecovering UBSan
with focused warnings clean. The synthetic route places the only spawnable
source in record 511 and makes particle record 0 occupied with a zero low
half, so the certificate simultaneously proves both table geometries and the
correct free test.

It also proves the two timer transitions, exact helper argument addresses,
random-call order, three VectorNormal inputs, both published vectors, heading,
signed halfword extension, copied payload/flags, source-count increment, and
the final tick call.

Twelve named mutants are detected:

| mutant | failure mode |
|---|---|
| M1 | only 256 source records |
| M2 | `0x4c` source stride |
| M3 | flags at source `+0x4b` |
| M4 | discard the packed high timer |
| M5 | free test against packed low half |
| M6 | first ApplyMatrix source at `+0x28` |
| M7 | draw random X before random Y |
| M8 | first amplitude from `+0x42` |
| M9 | omit the second ApplyMatrix |
| M10 | zero-extend signed copied halfwords |
| M11 | omit source spawn-count increment |
| M12 | omit the final particle tick |

## Normal build and frame-601 lifecycle regression

The normal product build reports `LINK OK`. The exact accepted detached
route was rerun with:

```text
XENO_TEST_INPUT=0:0x2000,600:0x4000,916:0x2000,976:0
XENO_WORLD_TEST_INPUT=0:0x2000,600:0x20,601:0
```

All capture digests remain byte-identical:

```text
frame 60  4310197d3881367bc7859ef174dcf4b9afa8759f0b7dc2ca3eea94b0f84f1521
frame 120 8039c2f96b88e09491eaf1cba049d0efd829f146aa943da744f56c93e85b1c16
frame 600 2b636ae21af4bf0003f60d5e53f8ffd9a042fc5a72457e4c096cff19814e1a38
```

Frame 601 again produced the unseeded natural state transition:

```text
[worldmap-open-loop] natural state exit frames=601 D7CC=0
```

No primitive-link or OT-adapter abort was logged. The owned post-exit process
was stopped after natural world teardown/terminal selection completed.

## Bound

The accepted route does not naturally arm an effect source, so runtime
neutrality is expected and observed. Active spawning is proven by the
production-linked certificate rather than claimed from that route. No
instruction remains unimplemented in retail `wm_80089748`.
