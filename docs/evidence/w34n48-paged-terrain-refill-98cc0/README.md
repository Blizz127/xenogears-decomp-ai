# W34N48 — retail paged terrain refill `0x80098CC0`

## Result

`RESTORED_VERIFIED_SYNTHETIC / NATURAL_GATE_DORMANT`

Starting HEAD was `e97199155dd94b4b67ed3053121231cb0ef5c47c`, equal to
`origin/experiment/worldmap-open-gates-20260823`. The incomplete 101-line
asset-loader sketch has been replaced by the complete bounded retail helper
`[0x80098CC0, 0x8009932C)` (411 instructions / 1644 bytes).

## Retail authority

The authoritative input is `disc/world_map.bin`, decoded with zero runs
enabled:

```text
mips-linux-gnu-objdump -Dz -b binary -m mips:3000 -EL \
  --adjust-vma=0x8006faf0 \
  --start-address=0x80098cc0 --stop-address=0x8009932c \
  disc/world_map.bin
```

The full decode used for line-by-line transcription is
`scratchpad/w34n48_98cc0_full.objdump`. Fresh call-graph authority is
`scratchpad/world-callgraph-v2/FUNCTIONS.csv`. The port invokes the helper
conditionally from `wm_80071A58` after `wm_800980D4`, `wm_800981C8`, and the
queue barrier whenever paging bits `D_8009D558` are nonzero.

## Retail behavior restored

The function now performs the complete retail state machine:

1. walks all 81 entries of the **previous** window at `D_8009D318`; a tile
   with a non-null `C184[tile]` slot is freed only when it is absent from all
   81 entries of the new window at `D_8009D570`;
2. polls `func_8002C3D8` twice and selects the sector backend when
   `first == 0 || second == -1`, exactly matching `0x80098D7C..0x80098D98`;
3. fills the primary archive's two horizontal edge bands, window indices
   `1..7` then `73..79`;
4. fills the secondary archive's two vertical edge bands, indices
   `9,18,..63` then `17,26,..71`, using its transposed source index
   `(tile % width) * height + tile / width`;
5. examines the four `D_8009BBAC`-selected window entries and allocates any
   remaining holes. If the primary edge pass allocated anything, it is the
   source; otherwise a productive secondary pass supplies the transposed
   source. With neither flag, retail publishes the allocation but queues no
   transfer;
6. terminates and publishes the selected queue through
   `(wm_8009623C, wm_80096328)` for the decoded-sector backend or
   `(wm_800962B0, wm_800965A4)` for the file-path backend;
7. stores `HeapAlloc` results in `C184` as guest addresses and converts them
   back to host views for `HeapFree`, preserving the W34C5 domain contract.

The old body used `D570` as both eviction lists, ignored the readiness
predicate and four-entry closure, used fabricated contiguous loops, decoded
but discarded one archive result, and never implemented the file-path
backend.

## Focused certificate

Runner: `pc_port/tests/run_w34n48_98cc0.sh`

```text
CERTIFICATE O0/O2/UBSan PASS; strict warnings clean
M1  new window used as eviction authority     DETECTED
M2  readiness OR changed to AND               DETECTED
M3  second primary edge starts at 72          DETECTED
M4  secondary edge uses contiguous indices    DETECTED
M5  secondary archive is not transposed       DETECTED
M6  four-entry closure omitted                DETECTED
M7  secondary source wrongly wins priority    DETECTED
M8  file offset omits the sector shift        DETECTED
M9  raw host pointers published in C184       DETECTED
M10 queue-publication backends swapped        DETECTED
W34N48 0x80098CC0 FULL CERTIFICATE PASS; M1-M10 DETECTED
```

The production-linked certificate also proves both independent ready cases
(`first == 0` and `second == -1`), exact transfer ordering and arguments,
primary-only/secondary-only closure behavior, the retail allocation-without-
source case, guest-domain free/publication, input tables read-only, and the
backend-specific terminator/flush pair. O0, O2, and nonrecovering UBSan have
identical output under strict warnings.

## Natural 601-frame gate witness

A temporary `#ifdef` witness at the post-`wm_800980D4` branch was used with
the accepted detached route and then removed. Across all 601 naturally
displayed frames it recorded no nonzero `D_8009D558` event and therefore no
natural call to `wm_80098CC0`:

```text
input schedule = 0:0x2000,600:0x4000,916:0x2000,976:0
natural state exit: frames=601 D7CC=0
paging calls: 0
upload unknowns: 0
```

The normal captures are byte-identical to W34N47, as required for a dormant
replacement:

```text
frame 60  cdc95854e30d798f5634defcdd138ac9f5963afa4de8872981343e77068a46e8
frame 120 d5483c023f2fdecb030c6d335774048d98bc4684e1c524d7a7c3622e175f26bd
frame 600 6e861aecdf5e804bcbf6bb46c002f872308bfe831763ca663e8f2254294c3b1e
```

This is explicitly a dormant natural gate, not live paging acceptance. A
future route that crosses the `wm_800980D4` world boundary must confirm the
allocation/copy/free lifecycle under naturally nonzero `D558`. The bounded
certificate is the acceptance authority until such a route exists.

The diagnostic was removed, the normal port rebuilt, and the final build
reported `LINK OK`.
