# W34N18 — Retail transition selector and frame lane

## Anchor

- Starting HEAD: `deb5a88a5fc9ef12a0fb930cb95eaacd3468d6e4`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail image: `disc/world_map.bin`
- Image SHA-256:
  `4c15fd32b3a03d7cd5ea4403dcaabc70abaf99b6aaca65d63d6866803edaac70`
- `wm_80075E7C`: `[0x80075E7C,0x80076098)`, file slice
  `[0x638C,0x65A8)`, `0x21C` bytes / 135 instructions, SHA-256
  `3a2e295fabc162d78e84c420a53b5795673f86d933bf8756e82f978a35b3f394`.
- `wm_80094028`: `[0x80094028,0x80094060)`, file slice
  `[0x24538,0x24570)`, `0x38` bytes / 14 instructions, SHA-256
  `135be64ab129d14a479be4074296de5125120704db22375c6a348616baef971b`.
- Sole retail caller: `0x800717FC` in `wm_800712D0`, with
  `a0=0x8009D55C` and `a1=lhu(0x8006EF64)`.

The prior local `wm_80075E7C` was a `void` address-tagged no-op and had no
callsite in the active bounded driver. Replacing that body alone would not
have restored retail behavior.

## Retail behavior restored

`wm_80094028` loads X/Z from the supplied guest vector, calls the existing
retail `wm_80093660` terrain-cell lookup, and returns bits 2..5 of cell byte
3.

`wm_80075E7C`:

1. obtains the cell selector through `wm_80094028` and area through the
   existing `wm_80093F18`;
2. remaps the selector through signed-halfword table `0x8009A3A0` only for
   area 4;
3. selects a threshold bucket from unsigned-halfword table `0x8009B57A`;
4. reads the corresponding 16 weights at
   `D73C[selector]+0x200+bucket*0x10`;
5. returns zero without advancing the PRNG or publishing state when their
   sum is zero;
6. otherwise chooses `rand()%sum` by the retail weighted ordering, copies
   exactly `0x200` bytes from `D73C[selector]` to `D_800658DC`, publishes the
   selected byte to `D_80059508`, and returns one.

Retail loads every weight byte twice (once for its stack copy and once for
the sum). The C body loads it once and reuses the byte; the read-only source,
sum, copied local weights, selection, writes, and PRNG cadence are
output-equivalent.

The active driver now executes the exact guarded transition lane:

- `C178==0`, `D804==0`, signed `BD24==-1`, signed `CE68==-1`, `D554!=0`,
  and `D80C!=0` are required for the selector call;
- result one clears `D554`, stores `D7CC=1`, clears native
  `D_8005954C`, and publishes the three party bytes;
- the converged tail always clears `D80C` and toggles `0x8006EE76` when
  `BD10 & 0x100` is set.

## Address and authority audit

The independent decode caught signed-low-half address errors before the
implementation was certified:

- `lui 0x8006; sb ...,0x954C` is `0x8005954C`, not `0x8006954C`;
- `lui 0x8007; sh ...,0xEE70/72/74` is
  `0x8006EE70/72/74`, not `0x8007EE70/72/74`;
- the caller's `lui 0x8007; lhu ...,0xEF64` is `0x8006EF64`.

`D73C` and its relocated resource pointers are guest addresses and are
rebased through `PSX_ADDR`. `D_800658DC`, `D_80059508`, and
`D_8005954C` are native main-executable BSS authorities read directly by
compiled field/battle consumers. The implementation writes those native
symbols rather than creating guest-only twins. `0x8006EE70..76` remains the
established guest-resident world handoff record.

## Certificate

`pc_port/tests/run_w34n18_transition_selector.sh` passes with strict warnings
under O0, O2, and nonrecovering UBSan. It verifies both selector paths,
threshold equality, leading-zero weighted selection boundaries, one PRNG
advance on success, zero-sum nonpublication, exact 512-byte copy/canaries,
native publication, and a byte-for-byte read-only guest snapshot.

Eight focused mutants are detected by named assertions:

- M1/M2: wrong terrain-cell byte or bit shift;
- M3: missing area-4 remap;
- M4: wrong threshold equality;
- M5: copy from the weight area rather than the record base;
- M6: unweighted selection;
- M7: short payload copy;
- M8: zero-sum publication.

The W34C1 cadence certificate passes O0/O2/nonrecovering UBSan and now kills
M1-M21. New M16-M21 cover the selector call, nonzero-D80C guard, session-exit
stores, party publication, unconditional D80C clear, and bit-0x100 toggle.

The normal port build reports `LINK OK`.

## Detached runtime acceptance

The accepted detached route reached both presentation captures and logged
`bounded exit frames=120 limit=120`:

- frame 60:
  `26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3`
- frame 120:
  `ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc`

Both match the standing accepted baseline. The current route does not arm
the transition selector; neutrality is therefore the correct natural-route
result, while armed semantics are established by the production-linked
certificates.

Artifacts:

- `scratchpad/w34n18_build.log`
- `scratchpad/w34n18_accept.log`
- `scratchpad/w34n18_capture/world-frame-000060.bmp`
- `scratchpad/w34n18_capture/world-frame-000120.bmp`

## Result

`REPAIRED_VERIFIED`. The final local address-tagged no-op in the active
frame driver is gone, and its retail callsite is restored. Remaining
frame-driver gaps are integration or absent-helper targets, not local
address-tagged stubs.
