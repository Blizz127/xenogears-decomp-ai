# W34C1 Rung 2 — deterministic in-process scripted input

Date: 2026-08-25

Starting production HEAD: `ee90010f57ce056081da94c10d094d8ff72628f9`

## Harness contract

`XENO_TEST_INPUT` accepts a strictly increasing declarative schedule such as:

`0:0x2000,600:0x4000,916:0`

The value at each boundary is held until the next boundary. The first boundary
must be frame zero; frames are decimal and values use C integer syntax up to
`0xFFFF`. Empty schedules, malformed pairs, trailing input, duplicate or
decreasing boundaries, nonzero first boundaries, and out-of-range values are
startup errors. When the variable is unset, no input word is written.

The shared test frame clock advances at `func_8007554C` during field bootstrap.
The requested field injection remains exactly at the `func_8009F5F4` boundary,
where it writes `D_800AFE9C`. Once control transfers into the nested world-map
loop and field frames stop, `wm_80071034` continues the same clock and injection
once per world frame. This continuation is required for a detached run to
change or release input after world-map entry; it does not create a second
schedule or reset frame numbering.

The parser runs from `port_main` immediately after PSX memory initialization
and before PsyCross/window startup. A real launch with `XENO_TEST_INPUT=bad`
returned rc 1 and reported:

`[test-input] invalid XENO_TEST_INPUT: invalid frame boundary`

## Focused certificate

`pc_port/tests/run_w34c1_scripted_input.sh` proves:

- unset environment is inert across repeated frame advances/injections;
- malformed schedules fail initialization rather than injecting zero;
- exact boundary values and hold semantics;
- an explicitly scheduled zero is still injected when the environment is set;
- startup, field clock, field injection, and world continuation sites exist;
- `XENO_TEST_INPUT_MUTANT_DROP_HOLD` is detected by `ASSERTION hold.frame1`.

Results:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- hold-dropping mutant: DETECTED by named assertion
- normal PC port: LINK OK

## Detached 120-frame acceptance

Permanent harness:

- `pc_port/tests/w34c1_scripted_input_detach.gdb`
- `pc_port/tests/run_w34c1_scripted_input_acceptance.sh`

GDB only waits for `PcPort_WorldMapInitMain` and detaches. It has no input
breakpoint and performs no state writes. Each native run must reach frames 60
and 120, fulfill capture requests on the same numbered presentation frame, and
return from the bounded loop.

Accepted run root:

`pc_port/build_native/w34c1_scripted_input_acceptance/run.rfF8OC`

Two runs using
`0:0x2000,600:0x4000,916:0x2000,1036:0` were byte-identical:

- frame 60: `cbd7be8acc0672aea6c9c3fade53e288f3146cc8c7779a3d7c7e558a6cb2f6d4`
- frame 120: `bcdfc279969de6a13d81d3a9756d5f2fdda02ff1977aefd948d82cceeae58acf`

The bootstrap-only control `0:0x2000,600:0x4000,916:0` differed at both gates:

- frame 60: `b1e78334b3366758826a99140acce833384c45e29b5a4ad84cfb3310c99fc47f`
- frame 120: `d560fb2a9c70c2b2f147467fcd20f9093a04789d705e1ad0532a1bc1d56578da`

This proves deterministic schedules and proves that post-handoff input reaches
the world map. All detached captures before W34C1 Rung 2 were no-input world
captures because their GDB input breakpoint was disabled before world entry.
