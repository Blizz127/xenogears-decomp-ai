# W34N46 — retail overlay renderer `0x800740B8`

## Result

`RESTORED_VERIFIED`

Starting HEAD was `ffbd626435895b0189b89e03093901d1dbc3e118`, equal to
`origin/experiment/worldmap-open-gates-20260823`.  The naturally reached
96-line sketch has been replaced with the complete bounded retail renderer
`[0x800740B8,0x80074594)` (311 instructions / 1244 bytes).

## Authority and natural reach

Retail authority is `disc/world_map.bin`, freshly decoded with:

```text
mips-linux-gnu-objdump -D -b binary -m mips:3000 -EL \
  --adjust-vma=0x8006faf0 \
  --start-address=0x800740b8 --stop-address=0x80074594 \
  disc/world_map.bin
```

The direct base-world call site is retail `0x80071B7C`.  A state-only census
at the first natural invocation established:

```text
D7F0 buffer index = 0
D_8006EE76        = 0 (call enabled)
D_8006F160 mask   = 0x00000000
draw record       = 0x8009BBC8
OT root           = 0x800A2228
```

The old body changed guest links by zero.  The restored body changes the
first-call domain counters from `guest=17` to `guest=22`, with native and
rejected counters unchanged: exactly four projected packets plus the final
buffer packet, as retail and the zero icon mask predict.

## Retail behavior restored

The function now:

1. builds the heading rotation at scratch `0xB8 -> 0xF0`;
2. computes the three retail translation words from `0x8009D55C`,
   `0x8009BCDC`, and `0x8009BE0C`, including the two signed magic-division
   sequences;
3. projects four triples from `0x8009A340` (24-byte source stride) into four
   28-byte packets at `0x8009C664 + buffer*112`;
4. publishes those four packets to the active world guest OT;
5. walks the 32 bits at `0x8006F160`, linking the mode packet and a 16-byte
   icon packet for each active bit;
6. reproduces the distinct default and special-coordinate calculations for
   icon indices 24, 25, and 26;
7. publishes the final packet at `0x8009C5C0 + buffer*40`.

All packet publication uses the established domain-aware primitive linker;
the destination OT and packet pools are both guest memory.

## Focused certificate

Runner: `pc_port/tests/run_w34n46_740b8.sh`

```text
CERTIFICATE O0/O2/UBSan PASS; strict warnings clean
M1  projected packet stride   DETECTED
M2  source vertex stride      DETECTED
M3  projection vertex order   DETECTED
M4  translation offset        DETECTED
M5  projected links omitted   DETECTED
M6  mask shifts left          DETECTED
M7  index-26 special source   DETECTED
M8  default X offset          DETECTED
M9  mode packet omitted       DETECTED
M10 final packet omitted      DETECTED
W34N46 0x800740B8 FULL CERTIFICATE PASS; M1-M10 DETECTED
```

The normal PC port also reports `LINK OK`.

## Natural 601-frame acceptance

The final normal binary ran through the accepted detached route with scripted
input and the natural state-exit schedule:

```text
natural state exit: frames=601, D7CC=0
capture 60:  request and fulfillment frame 60
capture 120: request and fulfillment frame 120
capture 600: request and fulfillment frame 600
adapter aborts/boundaries: none
upload unknowns: 0
```

New hashes (the old visual baseline is intentionally retired because retail
overlay packets are now present):

```text
frame 60  cdc95854e30d798f5634defcdd138ac9f5963afa4de8872981343e77068a46e8
frame 120 d5483c023f2fdecb030c6d335774048d98bc4684e1c524d7a7c3622e175f26bd
frame 600 6e861aecdf5e804bcbf6bb46c002f872308bfe831763ca663e8f2254294c3b1e
```

Visual inspection of all three captures confirms that the newly enabled
packets form the expected top-right world-map/minimap overlay and location
label.  They are not random triangles or an OT corruption artifact.  The
already-known coarse/malformed terrain appearance remains a separate issue.

The process reached the known post-world `SoundHandleError` stub after the
natural state-exit marker and was terminated only then; no whole-process
`rc=0` is claimed.
