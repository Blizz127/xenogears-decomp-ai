# W34N100 — mode-15 moving-object callback

## Scope and retail anchor

- Starting HEAD: `77c7fc361b03eaafe02024cf29921bcffe11a0ba`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail image: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Disassembly: `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`
- primitive helper `[0x8007EBBC,0x8007ECA4)`, 232-byte SHA-256:
  `30a467d29779dcc703194884efaa60f5cffec04d35b321655d5fcd26b223820d`
- initializer `[0x8007ECA4,0x8007EE34)`, 400-byte SHA-256:
  `7ddcfde63cd20a06e97c0053d0cb60fe35383d9068c85f73d21661576a159d91`
- update `[0x8007EE34,0x8007F8AC)`, 2,680-byte SHA-256:
  `4e618ec4fb4925b9a99a9277adf8d1446426cfb08f93a669bb67681a1abda850`
- complete unit `[0x8007EBBC,0x8007F8AC)`, 3,312-byte SHA-256:
  `a037a9c4198e9bddd4de4aeebc5911d6372b64bf929bdce98c45bfaaf5dcc7c0`

## Production transcription

`wm_8007EBBC` restores the retail FT4-like primitive-stream initialization,
including the texture-page and CLUT selection and the source-to-destination
stream copy. `wm_8007ECA4` restores the slot-3 object initializer: context
links, the three primitive streams, initial matrix/velocity state, target
table, and starting position.

`wm_8007EE34` implements all observed latches (`1-5`, `16`, and `24`),
velocity integration, the complete retail transition set (`0-3`, `16`,
`17`, `24`, `64`, and `65`), target claims and marker effects, context
publication, both fixed and velocity-aligned render-orientation paths, and
effect release. The scheduler resolves both initializer and update symbols.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n100_mode15_moving_object.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- all four retail slice hashes: PASS
- primitive stream initialization and texture arguments: PASS
- three-context linkage and target-table construction: PASS
- latch and velocity-integration behavior: PASS
- state transitions, target claims, and effect lifecycle: PASS
- fixed and velocity-aligned matrix paths: PASS
- symbolic scheduler initializer/update resolution: PASS
- M1 wrong tpage: detected by `primitive.tpage_args`
- M2 skipped third context link: detected by `init.context_links`
- M3 wrong target stride: detected by `init.target_table`
- M4 wrong initial distance: detected by `init.position_distance`
- M5 wrong state-1 limit: detected by `state1.strict_limit`
- M6 skipped target claims: detected by `state0.claim_range`
- M7 wrong fixed render matrix: detected by `render.fixed_matrix`
- M8 skipped effect release: detected by `state65.release_return`

All M1-M8 were killed by named assertions. The W34N99 focused regression
also passes. The normal product build completes with `LINK OK`.

## Natural route

The normal detached entrance-15 route executed `0x8007ECA4` once and
`0x8007EE34` on all 120 recurring scheduler passes. There were no remaining
`0x8007ECA4` stub hits. The run fulfilled the frame-60 and frame-120 capture
requests in their requested frames and returned from the bounded loop.

The slot-3 object first becomes visible late in the run: frame 60 is
byte-identical to W34N99, while frame 120 adds a large projected object over
the moving sky. Its incomplete appearance is not treated as a defect in this
unit because the five related slots remain unresolved and naturally switch
between their `0x8007F8AC` initializer and `0x8007F968` update callbacks.

- frame 60 SHA-256:
  `731fbfdb250dc2728d74b9a9d9bf114e2b2a119e43409f15060aacbbbc6751cd`
- frame 120 SHA-256:
  `b1781737a445333d9cf2dfec632534bd2e80bd30b482851dea43f29703ebf4b3`
- next unresolved natural callbacks:
  - `0x8007F8AC`: 600 initializer-state stub hits across slots 4-8
  - `0x8007F968`: 5 update-state stub hits across slots 4-8

Both upload pumps remained at `unknowns=0` in the acceptance log.

## Verdict and next target

`MODE15_MOVING_OBJECT_RESTORED_NATURALLY_EXECUTED`

Next exact target: decode and restore the five-slot mode-15 callback unit
beginning at `0x8007F8AC`, including the naturally reached update at
`0x8007F968`, then rerun this same detached route.
