# W34N14 — retail party-presence helper 0x80075D4C

## Result

`TRANSCRIBED_AND_CERTIFIED`. The smallest genuinely absent frame helper is now
compiled C, and the active frame driver calls it instead of a local no-op.

Starting HEAD: `2138a9366f5de6eb579e6d2d316ab70736b6f215`. Local matched
`origin/experiment/worldmap-open-gates-20260823` before the change.

## Retail anchor

- Authority: `disc/world_map.bin`, loaded at `0x8006FAF0`.
- Whole-image SHA-256:
  `4c15fd32b3a03d7cd5ea4403dcaabc70abaf99b6aaca65d63d6866803edaac70`.
- Exact function boundary: `[0x80075D4C, 0x80075E7C)`, file offset `0x625C`,
  0x130-byte slice SHA-256
  `6f86842c69fe4ea4d262436e341fbdc37fdb49792bb8a19d5a80a1326d0dcb99`.
- Decode command:
  `mips-linux-gnu-objdump -D -b binary -m mips:3000 -EL --adjust-vma=0x8006faf0`.

The production transcription preserves the two bounded three-channel loops:

1. Compare current bytes `0x8006F8E5+i` with saved bytes
   `0x8006EE70+i*2`.
2. On removal, restore four saved words at pool-slot offsets
   `+0x228/+0x22C/+0x230/+0x258` to active offsets
   `+0xA8/+0xAC/+0xB0/+0xD8`.
3. On addition, write resync value `0x0400` at `0x8006EF8E+i*6`, clear the
   slot halfword at `+0x224`, and save the same four active words to the
   backup offsets.
4. Count channels whose `0x8006F368+i != 0xFF` and current presence equals
   one.
5. Unless `0x8006EE68 & 0x4000` is set, publish mode 2 for a nonzero count or
   mode 1 otherwise at `0x8009BE10`.

The first focused execution caught and corrected a transcription-label error:
the add/remove direction is selected by the **current** `F8E5` byte after a
mismatch, not by the saved `EE70` byte. No runtime or integration claim was
made from that failed attempt.

## Integration boundary

- `world_map_frame_driver_712d0.c` now binds the retail call at `0x80071684`
  to `wm_80075D4C`.
- The same helper is the second half of the slot-1 C894 restore pair
  `0x8007565C -> 0x80075D4C`. The predecessor `0x8007565C` remains absent, so
  that restore arm remains explicitly unsupported and does not call this
  helper out of order.
- `0x80075B58` also calls this helper at retail `0x80075D2C`; that caller
  remains an independent absent transcription.

## Focused certificate

Runner: `pc_port/tests/run_w34n14_75d4c.sh` verifies the retail image and
function-slice hashes, then links the production helper with emulated guest
RAM.

- O0 PASS
- O2 PASS
- nonrecovering UBSan PASS
- strict warnings clean
- M1 wrong 0x80 pool stride: detected by
  `addition.saves.active.to.backup`
- M2 reversed removal copy: detected by
  `removal.restores.backup.to.active`
- M3 missing slot-control clear: detected by
  `addition.clears.slot.control`
- M4 missing resync timer: detected by
  `addition.arms.resync.timer`
- M5 wrong active-channel predicate: detected by
  `active.party.selects.mode2`
- M6 ignored input freeze: detected by `input.freeze.preserves.mode`

The existing scheduler-cadence certificate links both shared frame helpers and
remains PASS under O0/O2/nonrecovering UBSan with M1-M15 detected. Normal port
build: `LINK OK` with port-owned addresses verified.

## Native route

The accepted detached 120-frame route reached its bounded exit and fulfilled
captures at frames 60 and 120. It emitted no OT-adapter abort report. The
capture digests remain byte-identical to W34N12/W34N13:

- frame 60:
  `26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3`
- frame 120:
  `ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc`

The pre-W34N14 accepted log contained no `0x80075D4C` stub message, so this
route does not naturally witness the conditional frame call. The certificate
is the behavioral oracle; the driver integration is compiled but dormant
under this state.

## Next bounded frontier

The remaining frame-driver no-ops are `0x800758C0`, `0x80075B58`, and
`0x80075E7C`. Of these, `0x80075B58` is the next smallest complete retail body
and naturally composes with the helper completed here.
