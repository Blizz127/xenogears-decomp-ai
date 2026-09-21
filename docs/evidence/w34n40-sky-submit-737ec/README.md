# W34N40 — complete retail `wm_800737EC` sky-quad submitter

Verdict: **FULL_BODY_TRANSCRIBED_AND_CERTIFIED**.

## Anchor and authority

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `06de1b3f19408c9f134014558f2bf872bc1dd3dc`
- Retail authority:
  `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`, function
  `[0x800737EC,0x800739B8)`
- Date: 2026-08-30

The retail boundary is 115 instructions / 460 bytes. No instruction remains
unimplemented in this function.

## Retired approximation

The previous body was not structurally related to retail's packet walk. It
placed heading in Z rather than Y, built the first matrix at the wrong scratch
address, composed in place, derived a fake loop count from vertex bytes,
advanced by the wrong source stride, never addressed retail packet storage,
and never published a primitive to the active OT.

## Retail sequence now implemented

The completed helper:

1. writes `(0, heading, 0)` at scratch `0x1F800000`, calls `RotMatrixYXZ`
   with destination scratch `+0x28`, and clears that matrix's translation;
2. computes `CompMatrix(0x8009C808, scratch+0x28, scratch+0x08)` and installs
   the composed rotation and translation in the GTE;
3. walks exactly four source records from `0x8009A280` at `0x20` stride;
4. addresses the four destination packets as
   `0x8009D194 + D_8009D7F0*0x24 + index*0x48`, preserving retail's
   interleaved double-buffer layout;
5. projects the four vertices at offsets `0/8/0x10/0x18`, writes all four XY
   results before testing signed GTE FLAG, and rejects only bit-31 failures;
6. applies MIPS-SRAV semantics to OTZ using the established host shift
   authority `wm_73b04_ot_shift()`, then publishes through
   `PcPort_AddPrimDomainAware` into the active draw record's OT.

The last step shares both depth-shift authority and guest-link semantics with
the already-certified world horizon path rather than creating a second OT
interpretation.

## Focused certificate

`pc_port/tests/run_w34n40_737ec.sh` passes O0, O2, and nonrecovering UBSan
with focused warnings clean. It proves:

- heading axis and exact scratch matrix addresses;
- zero intermediate translation and camera-left composition;
- installation of the composed matrix;
- the fixed four-quad source walk and all four vertex addresses;
- the `0x24` buffer / `0x48` packet interleave;
- XY publication even for a rejected quad;
- bit-31 FLAG rejection while a positive `0x1000` FLAG remains accepted;
- SRAV depth shift, exact OT bucket addresses, and domain-aware 24-bit links;
- source vertices and camera matrix remain read-only.

Twelve named mutants are detected:

| mutant | failure mode |
|---|---|
| M1 | heading written to Z |
| M2 | first rotation written to composed-matrix scratch |
| M3 | reverse matrix composition |
| M4 | retain intermediate translation |
| M5 | install the intermediate rotation matrix |
| M6 | walk only three quads |
| M7 | use a `0x28` vertex stride |
| M8 | use a `0x48` buffer stride |
| M9 | use a `0x24` packet stride |
| M10 | omit signed-FLAG rejection |
| M11 | omit the depth shift |
| M12 | reject every nonzero FLAG |

The shared `run_guest_prim_link.sh` regression also passes O0/O2/UBSan with
M1-M4 detected.

## Normal build and natural frame-601 regression

The normal product build reports `LINK OK`. The accepted detached route used:

```text
XENO_TEST_INPUT=0:0x2000,600:0x4000,916:0x2000,976:0
XENO_WORLD_TEST_INPUT=0:0x2000,600:0x20,601:0
```

Its capture digests remain byte-identical to the standing accepted artifacts:

```text
frame 60  4310197d3881367bc7859ef174dcf4b9afa8759f0b7dc2ca3eea94b0f84f1521
frame 120 8039c2f96b88e09491eaf1cba049d0efd829f146aa943da744f56c93e85b1c16
frame 600 2b636ae21af4bf0003f60d5e53f8ffd9a042fc5a72457e4c096cff19814e1a38
```

The unseeded natural transition remained:

```text
[worldmap-open-loop] natural state exit frames=601 D7CC=0
```

Capture requests 60, 120, and 600 were fulfilled on the same numbered frame.
No primitive-link or OT-adapter abort was logged. The owned diagnostic process
was stopped only after natural world exit had selected and entered FieldMain.

## Bound

Runtime neutrality is expected on the accepted view and is observed. Active
positive/negative FLAG behavior and packet publication are proven by the
production-linked certificate; the natural route is not claimed to display a
new sky quad merely because the now-complete helper executes.
