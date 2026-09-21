# W34N26 — retail terminal typed-region helper `0x80094364`

Verdict: **HELPER_TRANSCRIBED_AND_CERTIFIED; TERMINAL_INTEGRATION_PENDING**.

## Anchor and scope

The rung started at
`cd6786101d9c32f7fa1a1064eda3028aec337950` on
`experiment/worldmap-open-gates-20260823`, equal to origin.  It transcribes
the last helper still genuinely absent from W34N4's base-world frame/terminal
list.  The earlier list is otherwise stale: N6, N14, N17, N18, N20, N21,
N22, and N25 already supplied its other named bodies.

This rung deliberately does not guess the containing D7CC==0 terminal lane.
The helper is compiled into the port and called by its focused production
certificate.  Its product call at retail `0x80071158` remains the immediately
next integration boundary.

## Retail authority and boundary

Authority is `disc/world_map.bin`, loaded at `0x8006FAF0`:

- whole image SHA-256:
  `4c15fd32b3a03d7cd5ea4403dcaabc70abaf99b6aaca65d63d6866803edaac70`;
- function interval `[0x80094364,0x80094434)`, file offset `0x24874`;
- size `0xD0` / 208 bytes / 52 instructions;
- slice SHA-256:
  `6fbc417654acf248c20c124e97b8bbc5e4cce2e617c8e12e82d4c6aa69262efd`.

The return at `0x8009442C` and its delay slot end this body.  The independent
empty leaf beginning at `0x80094434` is not included.

Retail's sole decoded call is at `0x80071158` in the D7CC==0 terminal lane:

```text
a0 = 0x8009D55C
a1 = 3
a2 = lhu(0x8006EF64)
jal 0x80094364
```

It occurs after overlay/state selection and only when BBC4 is zero and the
currently published D7D8 record has signed type 3.  The caller ignores the
return and continues by publishing fields from whichever D7D8 record remains
selected.

## Exact helper contract

Arguments are a guest 20.12 position vector, a pointer-table index, and a
signed requested record type.  The helper:

1. reads X and Z words at vector offsets 0 and 8;
2. applies logical shift right 12 and masks each result to 16 bits;
3. loads the guest pointer table through `lw(0x8009BD00)` and selects
   `table[list_index]` with a four-byte entry stride;
4. treats signed halfword `record+8 == -1` as the list terminator;
5. walks 16-byte records with signed halfword fields
   `{x0,z0,width,depth}` at offsets 0,2,4,6;
6. applies inclusive bounds on both axes;
7. requires signed halfword `record+0xE` to equal the full requested type;
8. on the first match only, writes that guest record address to
   `0x8009D7D8` and returns 1;
9. returns 0 on an empty list or miss without changing D7D8 or any other
   guest byte.

The coordinate and pointer-table domains match the neighboring certified
`wm_80094238`, but the state contract is intentionally different: `94364`
does not clear or publish the BD24/CE68 IDs and does not overwrite D7D8 on a
miss.

## Focused certificate

`pc_port/tests/run_w34n26_94364.sh` verifies the retail image and exact slice,
then runs the actual production translation unit under O0, O2, and
nonrecovering UBSan with strict warnings.  It covers:

- first and second record matches;
- requested-type filtering and signed type `-1`;
- inclusive lower and upper bounds;
- 16-byte record stride;
- four-byte pointer-table scale;
- logical-shift/mask behavior on `0x12345000`;
- empty-list and scanned misses;
- whole-guest-RAM write sets for hit and miss.

Seven mutants are killed by named assertions:

- M1 wrong table scale: `list.index.scale`;
- M2 missing coordinate mask: `coord.mask`;
- M3 exclusive lower bound: `bounds.inclusive`;
- M4 ignored requested type: `type.match.scan`;
- M5 unsigned record type: `type.signed`;
- M6 wrong record stride: `stride.0x10`;
- M7 publication on miss: `miss.no_publish`.

The neighboring W34B24-I6 dependency pack was rerun: O0/O2/nonrecovering
UBSan pass, normalized outputs agree, and all 25 existing mutants are killed.
The normal port build reports `LINK OK` with `world_map_helper_94364.c` in the
port source list.

The final normal detached 120-frame route completed and fulfilled both capture
requests in their requested frames.  Its artifacts are byte-identical to the
accepted pre-rung baseline:

- frame 60: `26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3`;
- frame 120: `ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc`.

## Native bound and next action

The accepted 120-frame route does not take a natural D7CC==0 terminal exit,
so it cannot exercise this helper.  Its captures remain the standing frame-60
and frame-120 neutrality artifacts; that is only a dormant-path check, not a
natural witness of the helper's production caller.

Before integrating the terminal lane, extend the executable W34N23 retail
trace with one disclosed D7CC==0 seed after slot 2.  Record the BBC4/D7D8/type
guard, whether `94364` is called, its before/after D7D8 state, the three
terminal publications, and the common `762FC -> MainLoop(0)` epilogue.  That
dynamic sequence, together with the existing static decode, is the acceptance
oracle for the next code-producing integration rung.
