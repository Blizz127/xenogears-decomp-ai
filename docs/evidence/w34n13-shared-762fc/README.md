# W34N13 — shared retail helper 0x800762FC

## Result

`TRANSCRIBED_AND_CERTIFIED`. The active frame driver's local no-op for
`0x800762FC` is retired. Initialization and the dormant menu-mode branch now
bind one shared implementation of the retail GPU/cache synchronization helper.

Starting HEAD: `60cc7fc2730b1b3e753caa3a78a7c4f38339bf24`. Local matched
`origin/experiment/worldmap-open-gates-20260823` before the change.

## Retail anchor

- Authority: `disc/world_map.bin`, SHA-256
  `4c15fd32b3a03d7cd5ea4403dcaabc70abaf99b6aaca65d63d6866803edaac70`,
  loaded at `0x8006FAF0`.
- Decode:
  `mips-linux-gnu-objdump -D -b binary -m mips:3000 -EL --adjust-vma=0x8006faf0`.
- Exact boundary: `[0x800762FC, 0x8007634C)`.

The seven retail calls, in order, are:

1. `0x800445D0 DrawSync(0)`
2. `0x8004B54C Vsync(0)`
3. `0x800404D4 EnterCriticalSection()`
4. `0x800445D0 DrawSync(0)`
5. `0x8004B54C Vsync(0)`
6. `0x80040454 FlushCache()`
7. `0x800404E4 ExitCriticalSection()`

The address-to-symbol mapping is independently present in
`config/symbol_addrs.slus_006.64.txt`. The private implementation previously
inside `world_map_init.c` matches this retail sequence exactly; this rung
extracts it rather than reconstructing or extending it.

## Integration

- `world_map_helper_762fc.[ch]` now owns the single compiled body.
- `world_map_init.c` calls that shared body at the established pre-session
  GPU/geometry initialization seam.
- `world_map_frame_driver_712d0.c` calls it at both retail menu-mode sites
  (`0x8007190C` and `0x8007191C`) instead of an address-tagged no-op.
- The other four genuinely absent frame-driver helpers remain untouched.

The accepted 120-frame route never enters the menu-mode branch, so this rung
does not claim a natural witness for those two calls. It does naturally call
the same body during initialization.

## Focused certificate

Runner: `pc_port/tests/run_w34n13_762fc.sh`.

The production body passes O0, O2, and nonrecovering UBSan under strict
warnings. The certificate proves exact call order and zero arguments. Six
mutants are detected by named assertions:

- M1 missing first DrawSync — `ASSERT_EVENT_COUNT`;
- M2 wrong first Vsync argument — `ASSERT_SYNC_ARGUMENT`;
- M3 missing critical-section entry — `ASSERT_EVENT_COUNT`;
- M4 swapped second DrawSync/Vsync — `ASSERT_EVENT_SEQUENCE`;
- M5 missing FlushCache — `ASSERT_EVENT_COUNT`;
- M6 missing critical-section exit — `ASSERT_EVENT_COUNT`.

Normal port build: `LINK OK` with port-owned addresses verified.

The existing scheduler-cadence certificate now links this production helper
instead of relying on the driver's former private definition. It remains PASS
under O0/O2/nonrecovering UBSan with M1-M15 detected.

## Native neutrality gate

The detached accepted route used scripted input
`0:0x2000,600:0x4000,916:0x2000,976:0`, reached the bounded 120/120 exit, and
fulfilled both capture requests on their requested frames. There were no OT
adapter abort reports, upload-pump unknowns remained zero, and no
`[worldmap-stub] ... 800762FC` message occurred.

- frame 60:
  `26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3`
- frame 120:
  `ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc`

Both are byte-identical to the W34N12 accepted baseline, as required for an
extraction of an already-live initialization body plus activation of a dormant
branch.

## Remaining frame-driver frontier

The accepted 120-frame log contains no calls to any remaining local helper
stub. The unresolved set is `0x80075D4C`, `0x800758C0`, `0x80075B58`, and
`0x80075E7C`; each requires a bounded retail transcription and a route or
synthetic state that reaches its owning branch. No activity was invented to
rank dormant helpers by runtime frequency.
