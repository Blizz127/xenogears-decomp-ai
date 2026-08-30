# W34N21 — recurring CD status and synchronization lane

## Verdict

`RESTORED_VERIFIED`

The active recurring-frame path now owns the exact retail CD transfer-state
dispatcher and consumes its return value at the frame driver.  Status `3`
alone yields through `Vsync(0)` and retries; statuses `0`, `1`, and `2`
continue to `CdSync(1, 0x8009C588)`.  The accepted 120-frame route remains
bit-identical to W34N20 because its natural CD state is idle.

Starting HEAD: `b3545cf2175f1191f1445db227dadcafb2e8a5f4`

## Retail authority

Source image: `disc/world_map.bin`

- full image SHA-256:
  `4c15fd32b3a03d7cd5ea4403dcaabc70abaf99b6aaca65d63d6866803edaac70`
- `wm_800967E4 [0x800967E4,0x800968E0)`: 252 bytes / 63
  instructions, SHA-256
  `0d16c4f020e76808390b2ad94cdff89aa6c28b60bcd4beb2bd0890af938b1530`
- `wm_800968E0 [0x800968E0,0x8009699C)`: 188 bytes / 47
  instructions, SHA-256
  `27221b582c93408a01c5a64c4b05aa1fa6cb1d6a4eef07311f16237a28963c93`
- driver lane `[0x800713FC,0x8007142C)`: 48 bytes / 12 instructions,
  SHA-256
  `8040e026cfb4b6e2e095d58e91338840da68feba74ad2945759cdb974401c77b`
- jump table `[0x80070CA0,0x80070CB8)`:
  `96910,96950,96950,96950,96918,96958`

## Recovered behavior

`wm_800968E0` is `u32(void)`, not the previously fabricated two-argument
zero-fill helper:

- state 0 returns 0;
- states 1, 2, and 3 return 1;
- state 4 decrements the unsigned `BD2C` word, stores it, and, only when it
  reaches zero, reloads live `CD44`, increments it, and returns 1;
- state 5 stores `CD44=0`, clears `D788[old BCB8]`, advances
  `BCB8=(old+1)&15`, and returns 2;
- every unsigned state at least 6 returns 3.

The `BD2C=0` case deliberately underflows to `UINT32_MAX` and does not advance
the state.  Guest word access uses production-effective volatile bytewise
loads/stores, so the live state-4 reload and state-5 store order cannot be
elided or supplied merely by certificate hooks.

`wm_800967E4` retains the exact ready predicate formed from two
`func_8002C3D8` calls.  On the D788 path it propagates every nonzero dispatcher
status and starts `wm_8009699C` only at state 0.  On the C624 path it calls
`wm_800966CC`, then reloads `BCB8` before clearing the selected C624 entry and
advancing the tail; caching the pre-callback tail is not retail-equivalent.

The driver interval is exposed as `wm_712d0_run_cd_sync_lane` and executes:

```text
wm_800967E4
  result == 3 -> Vsync(0), retry
  result != 3 -> CdSync(1, PSX_ADDR(0x8009C588))
```

The linked API declarations now match their real ABI: `int Vsync(int)` and
`int CdSync(int, u_char *)`.

The former `world_map_helper_9623c.c::wm_800968E0(addr,count)` had zero source
callers, zero native call references, and no retail counterpart at that PC.
It was introduced by misidentifying the 47-instruction dispatcher and is
deleted rather than renamed speculatively.

## Certificate

`pc_port/tests/run_w34n21_cd_status.sh` compiles the actual helper and driver
seam directly:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- production O2 helper without test hooks: PASS
- helper/test strict warnings: clean
- existing driver TU under focused warning policy: clean
- M1–M9: all DETECTED by named assertions

Coverage includes every state class, state-4 underflow and live reload,
state-5 destination/wrap/store order, both D788 ready predicates, the C624
post-callback tail reload, return propagation, two consecutive status-3
retries with event order `WVWVWC`, and the exact `CdSync` mode/result pointer.

Independent review found and caused correction of three issues before this
evidence was accepted: cached C624 tail, cached state-4 `CD44`, and mismatched
Vsync/CdSync declarations.  A second review after volatile guest accessors
reported PASS with no remaining blocker.

Dependent gates on the final source:

- W34C11 CD loader: PASS; M1–M9 detected
- W34N6 queue barrier: PASS; M1–M4 detected
- W34N9 slot-1 owner: O0/O2/nonrecovering UBSan PASS; M1–M6 detected
- W34C1 controller source: O0/O2/nonrecovering UBSan PASS; M1–M3 detected
- W34C1 cadence: O0/O2/nonrecovering UBSan PASS; M1–M21 detected
- W34B18C legacy duplicate authority: O0/O2/nonrecovering UBSan 97/97;
  three loader mutants and fourteen frame mutants detected
- normal port build: `LINK OK`

The controller and W34B18C test fixtures received test-only ownership/ABI
maintenance needed to keep their historical assertions linked against the
current production seams; no behavior was added to production for those
fixtures.

## Detached native neutrality

Harness: `scratchpad/w34n21_detach.gdb`; debugger detached before
`PcPort_WorldMapInitMain`; scripted input
`0:0x2000,600:0x4000,916:0x2000,976:0`; bounded 120 displayed frames.

Stable binary identity:

- path: `pc_port/build_native/xeno-port`
- SHA-256:
  `a7909e455cb2f85a93891c7314775c5478321b54bcd4608134328b5b713a54e7`

Artifacts:

- frame 60 fulfilled at frame 60:
  `scratchpad/w34n21_capture/world-frame-000060.bmp`
  - SHA-256:
    `26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3`
- frame 120 fulfilled at frame 120:
  `scratchpad/w34n21_capture/world-frame-000120.bmp`
  - SHA-256:
    `ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc`
- bounded exit: `frames=120 limit=120`, returned to init
- both BMPs are byte-for-byte identical to W34N20

## Residual

`world_map_init.c` still contains the older, differently named
`wm_800968E0_dispatch_partial` and `wm_800967E4_dispatch_cd_work` authority
used by legacy/gated code.  W34B18C continues to certify that duplicate, but
its result is not treated as coverage of the natural owner implemented here.
Retiring or wrapping that duplicate is a separate integration task.
