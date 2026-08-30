# W34N22 — retail base-world session snapshot restore

## Result

`RESTORE_REENTRY_TRANSCRIBED_AND_CERTIFIED`.

Starting HEAD was
`966e0466a58f30213ca4714cb46f1ef46d89ab39` on
`experiment/worldmap-open-gates-20260823`, equal to origin.  This rung restores
the C894-nonzero base-session entry which the slot-1 owner previously rejected.
It also corrects the matching slot-2 snapshot producer to use the port's real
native save-buffer authority.

The remaining `EE6A != 0` arm is still explicit and loud because its retail
helper `0x80073398` is not transcribed.  It is not silently routed through the
new C894 arm.

## Retail anchor

Authority is `disc/world_map.bin`, loaded at `0x8006FAF0`:

- whole image SHA-256:
  `4c15fd32b3a03d7cd5ea4403dcaabc70abaf99b6aaca65d63d6866803edaac70`;
- restore helper: `[0x8007565C,0x800758C0)`, file offset `0x5B6C`,
  size `0x264` / 153 instructions;
- helper slice SHA-256:
  `e535f68765934333b427762b0d22dfbdeeb06003c800956f999bcf6e33a1becd`;
- the helper is a `void(void)` leaf with no calls and two bounded copy-loop
  branches.

The caller anchor is retail `0x80072380..0x80072434`.  Its exact order is:

1. read C894;
2. C894 zero calls fresh WDS cleanup, while C894 nonzero stops/destroys the
   current audio owner, clears `D_8004F2FC`, then publishes that saved pointer
   to `D_80062528`;
3. check EE6A;
4. read C894 again;
5. choose fresh placement or `0x8007565C -> 0x80075D4C`.

The focused owner certificate covers both `(C894,EE6A)=(0,1)` and `(1,1)` so
the prelude-before-EE6A ordering is observable.  A test audio seam changes
C894 between the two reads, proving the second read is live rather than a
cached C value.

## Snapshot authority correction

Retail address `0x8005A4E4` is not the same authority as
`PSX_ADDR(0x8005A4E4)` in this port.  `pc_port/src/data_field.c` defines the
compiled native `D_8005A4E4[0x10000]`, and compiled field VM code consumes that
symbol directly.  The prior W34N7 producer wrote a decoy guest-RAM twin, while
the correct restore necessarily reads the native array.

W34N22 changes both halves:

- retail save helper `0x80075460`, inside the slot-2 teardown, now writes
  native `D_8005A4E4`;
- new `wm_8007565C` reads that native authority and writes the pool and fixed
  world globals through guest memory.

The save/restore certificate seeds the guest twin differently and proves it
remains untouched.  This correction is dormant on the accepted bounded route
because that route does not take natural D7CC==1 teardown.

## Exact restore contract

The restore copies:

- `+0x0000..+0x1FFF` to the pool at `lw(0x8009BE24)`;
- `+0x2000..+0x200F` to both `0x8009C5AC` and `0x8009D55C`;
- scalar words/halfwords at `+0x2010/+0x2014/+0x2018/+0x201C`;
- `+0x2020..+0x203F` to `0x8009C854`;
- `+0x2040..+0x22BF` to the `0x8009CEC4` ring;
- the angle, camera/world position, map, and projection fields from
  `+0x22C0..+0x22FB` in retail store order.

Retail save deliberately leaves `+0x200C`, `+0x22E0`, and `+0x22F8`
untouched, while restore consumes them into `C5B8/D568`, `BBC0`, and `BE34`.
The production code preserves this sparse carry-through.  The same-process
round-trip seeds all three holes with `0xCDCDCDCD`, runs the real save and real
restore, and proves those values survive at all four destinations.

## Review corrections before acceptance

Independent review found and caused correction of three pre-commit defects:

- the initial implementation checked EE6A before the C894-dependent prelude
  and cached C894 across retail's required reload;
- the inherited C snapshot stored the `+0x22D4` group before the `+0x22E4`
  group, opposite retail order;
- the first tests did not protect sparse-hole preservation with a real
  producer-to-consumer round trip.

After correction, the independent audit verified the full retail interval,
caller order, both C894 reads, native/guest domains, sparse holes, build
wiring, and tests and reported `PASS` with no remaining concrete defect.

## Certificates and build

`pc_port/tests/run_w34n22_7565c_restore.sh`:

- focused restore: O0 PASS, O2 PASS, nonrecovering UBSan PASS, strict warnings
  clean;
- M1 guest-twin source: detected by `source.native.authority`;
- M2 short pool copy: detected by `pool.full.0x2000`;
- M3 missing duplicate pose publication: detected by
  `runtime.record.dual.publish`;
- M4 missing timer block: detected by `timers.full.0x20`;
- M5 short ring copy: detected by `ring.full.0x280`;
- M6 missing camera-position block: detected by
  `camera.position.four.words`;
- M7 swapped final stores: detected by `restore.write.order`.

The same runner re-gates:

- W34N7 slot-2 teardown at O0/O2/nonrecovering UBSan with M1-M9 detected,
  including native authority, sparse holes, exact save order, and the real
  round trip;
- W34N9 slot-1 owner at O0/O2/nonrecovering UBSan with M1-M11 detected,
  including audio store order, restore-pair order, and the live C894 reload.

`pc_port/tests/run_w34c1_scheduler_cadence.sh` remains PASS at
O0/O2/nonrecovering UBSan with M1-M21 detected.  The final normal build reports
`LINK OK -> pc_port/build_native/xeno-port (port-owned addresses verified)`.

## Detached native neutrality

Final normal binary SHA-256:
`49e0f79465430e748c847c6a703874856714df94ee92d4f32a8f4d2a7a5d4a5f`.
The launched `/proc/2131960/exe` matched that hash and the exact binary path.
The debugger detached before `PcPort_WorldMapInitMain`; the accepted input
schedule and 120 displayed-frame bound were unchanged.

- frame 60 fulfilled at frame 60:
  `scratchpad/w34n22_final_capture/world-frame-000060.bmp`
  - SHA-256:
    `26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3`;
- frame 120 fulfilled at frame 120:
  `scratchpad/w34n22_final_capture/world-frame-000120.bmp`
  - SHA-256:
    `ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc`;
- both captures are byte-identical to W34N21;
- bounded exit reached `frames=120 limit=120` and returned to init.

PID 2131960 was terminated only after the bounded-return marker.  The ambient
unrelated PID 1345676 was not touched.

## Residual and next boundary

The C894 restore arm is behaviorally certified but has not yet been naturally
armed by the accepted route.  Natural round-trip acceptance still needs the
outer WorldMapMain terminal/session lifecycle to drive D7CC==1 through slot-2
save and a later slot-1 restore.

The independent EE6A branch remains blocked on bounded helper `0x80073398`.
The highest-value architectural target is the omitted retail terminal control
at `0x800710E4..0x800712CC`; `0x80073398` is the remaining smaller base-slot-1
restore leaf.
