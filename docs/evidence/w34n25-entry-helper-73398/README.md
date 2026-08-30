# W34N25 — retail restore-entry helper `0x80073398`

Verdict: **RESTORE_ENTRY_HELPER_TRANSCRIBED_AND_CERTIFIED**.

## Anchor and scope

The rung started at
`1523cacb37e4385c410ec685acfd069805b92a26` on
`experiment/worldmap-open-gates-20260823`, equal to origin.  It implements
only retail helper `[0x80073398,0x80073448)` and its already-decoded selection
seam in base-world slot 1.  It does not alter the separate C894 restore path,
session cadence, WDS/SPU ownership, or slot-2 teardown.

The accepted route keeps EE6A zero, so natural-route equivalence is a
dormant-path gate.  The helper's live arm is instead covered by a focused
production certificate and by the slot-1 owner certificate with both C894
prelude states.

## Retail authority

Authority is `disc/world_map.bin`, loaded at `0x8006FAF0`:

- whole image SHA-256:
  `4c15fd32b3a03d7cd5ea4403dcaabc70abaf99b6aaca65d63d6866803edaac70`;
- helper interval `[0x80073398,0x80073448)`, file offset `0x38A8`,
  `0xB0` bytes / 44 instructions;
- helper slice SHA-256:
  `376357b7f2343061544eb3ff7666fd5e2a60cf6009efbf0ada9e7c5df6bc57d9`;
- seven-entry jump table at `0x8006FB24`, file offset `0x34`, 28 bytes,
  SHA-256
  `e3862fd70af72043fe9d9436548aaca63fc735694bfb69bf6517d9d65a8d8626`.

The table targets for modes 1 through 7 are:

```text
733D8 733D8 73438 73418 73418 73438 73418
```

Thus modes 1/2 select the direct-record arm, modes 4/5/7 select the vector
arm, and modes 3/6 do nothing after consuming the flag.

## Exact contract

The production transcription preserves these retail details:

1. load the full mode word from `0x8009BE10` before any write;
2. clear the halfword at `0x8006EE6A` unconditionally;
3. reject values outside unsigned range 1 through 7;
4. modes 1/2:
   - read unsigned halfwords `EE54`, `EE58`, then `EE56` in retail order;
   - write `C5AC = EE54 << 12`, `C584 = EE58`, and
     `C5B4 = EE56 << 12`;
   - leave `C5B0` untouched;
5. modes 4/5/7:
   - call the already-certified `wm_8008DFF4(0x8009C5AC)`;
   - copy unsigned halfword `EE66` to word `C584`;
6. modes 3/6 and out-of-range values change only EE6A.

All source and destination addresses are guest addresses.  No host-local
surrogate or raw native pointer is introduced.

## Caller integration

Retail slot 1 performs its C894-dependent audio/WDS ownership prelude before
testing EE6A.  The new owner path preserves that order, calls
`wm_80073398()` exactly once when EE6A is nonzero, skips both fresh and C894
restore placement, and rejoins at the following `ArchiveCdDataSync(0)`.
The live second C894 read remains intact in the EE6A-zero path.

The legacy forced initialization ladder now calls the same helper rather
than logging and skipping it.  Its separate C894 restore skip remains legacy
debt and was not broadened into this rung.

## Certificates

`pc_port/tests/run_w34n25_73398.sh` verifies the retail image, helper slice,
and jump table hashes, then tests every mode 0 through 8 plus `0xFFFFFFFF`.
It checks exact read/write/call order, high-bit unsigned halfword behavior,
arm grouping, C5B0 preservation, dependency destination, input preservation,
and the complete guest-RAM write set.

- O0: PASS;
- O2: PASS;
- nonrecovering UBSan: PASS;
- strict warnings: clean;
- M1 omitted flag clear: detected by `flag.cleared`;
- M2 clear-before-mode: detected by `order.mode_before_clear`;
- M3 wrong mode group: detected by `mode2.group.direct`;
- M4 signed direct input: detected by `direct.values.unsigned`;
- M5 swapped direct fields: detected by `direct.values.mapping`;
- M6 C5B0 overwrite: detected by `direct.c5b0.preserved`;
- M7 missing DFF4 call: detected by `dff4.called`.

The W34N9 slot-1 owner was extended and rerun at O0/O2/nonrecovering UBSan.
Its original M1-M11 still pass, and:

- M12 omitted helper: detected by `ee6a.helper.exactly_once`;
- M13 helper fallthrough: detected by `ee6a.exclusive.arm`.

The complete W34N22 restore runner re-gated W34N7 teardown, W34N9 slot 1,
and its own save/restore round trip.  W34C9 fresh-entry placement also passes
all regimes and M1-M5.  W34C1 cadence passes O0/O2/nonrecovering UBSan with
M1-M21 detected.  The final normal build reports `LINK OK` with the new
translation unit in the port source list.

## Detached native neutrality

The starting-head binary and final binary were each run for 120 displayed
frames with the accepted schedule
`0:0x2000,600:0x4000,916:0x2000,976:0`, with GDB detached before
`PcPort_WorldMapInitMain`.

Both runs fulfilled capture requests on the attributed frame, reached the
bounded return, and produced byte-identical pre/post artifacts:

- frame 60:
  `26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3`;
- frame 120:
  `ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc`.

This proves neutral behavior when the arm is dormant.  It does not pretend
that the port's accepted route naturally exercised EE6A.

## Residual

The bounded helper gap named by W34N22 is closed.  Natural EE6A arming and a
port-versus-retail state comparison at this exact seam remain useful dynamic
acceptance work; they are not prerequisites for trusting the finite helper
semantics already certified here.
