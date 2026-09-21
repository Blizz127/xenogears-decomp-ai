# Native file-1 adoption opening replay — 2026-09-06

## Result

**PASS for the bounded native runtime gates.** The copied final binary entered
the opening battle naturally, executed the module-identity-gated native
`file1_bound_controller` 670 times, rendered readable Fei pilot text, completed
the opening battle after ordinary `z` confirms, and re-entered `FieldMain` with
field 14. The owned native/GDB/Xvfb processes were then stopped and are absent.

This is native-port runtime evidence. It is not retail emulator or retail-pixel
authority.

## Pins and ownership

- Run: `/var/home/blizz/Projects/xenogears-decomp-ai/pc_port/build_native/file1-adoption-opening-qqvyl5z1`
- Copied executable SHA-256:
  `f59f7f87cb2b611d40441e54e973c7a295c4978f839920f31c0455b9b360c205`
- Build manifest SHA-256:
  `9700bfdfd51e3aa5ff26ff3b1236fbd2f72fbd489c1cbd665ee8e47ded9687f7`
- Driver SHA-256:
  `aeda4e3af94705a4a92a5437784fba9061209d8673ad0e8fc2448111de055926`
- Instantiated trace SHA-256:
  `e1050c0b165686d84fdfcfaef19523d237c6a0db0189dc4774e56bbbe2e2a749`
- Isolated display/PIDs: Xvfb `325554`, GDB `325634`, native child
  `325683`, window `6291508`, display `:1`. All three PIDs were absent after
  exit.
- Existing `Xenogears (PC port).log` was restored byte-for-byte: current and
  preserved-before SHA-256 are both
  `4abc6faa0b9549ca84bb28e303614a62b1f85ebee7d5d1a3cfe6c09280c8edae`.
- No recording was made; the unique run recording directory is empty. Audio
  used the null backend for this non-audio test.

## Natural route and ordinary input

- The native title loop appeared, was captured, then received ordinary
  `Up` for 150 ms, a 500 ms settle, and `z` for 200 ms at
  `2026-09-06T06:26:12.564978Z`.
- Four ordinary F11 host speed steps selected speed 5 after field 4 loaded.
- During the first battle the driver sent ordinary 80 ms `z` pulses at 1.5 s
  intervals and captured the window before each pulse. No RAM, register, story,
  map, or position writes were used.
- Battle entry: `2026-09-06T06:26:48.181982Z`.
- Battle return: `2026-09-06T06:27:13.809331Z`.
- `FieldMain` field 14 entry: `2026-09-06T06:27:13.824733Z`.
- Owned stop was requested three seconds later, at
  `2026-09-06T06:27:17.212661Z`.

## Native adoption evidence

`file1-adoption.jsonl` has 670 paired entry/return records with indices
1 through 670 and no missing return. Every entry and return records:

- `verified = 1`;
- controller call generation `2`;
- runtime module generation `2`;
- immutable `[0x801E5000,0x801E9B5C)` SHA-256
  `8c071b7c6edffcb922b798ceddfb1a09167d22b402ae5d0d268d9252bf4a15c4`;
- native stack `file1_bound_controller -> file1_try_controller ->
  runtime_bridge_call -> runtime_bridge -> PcPortMipsRun -> func_80070F40`;
- guest PC/register arguments and the native three-argument entry values.

Observed native argument/count distribution:

- `(arg0,arg1,arg2)=(0,0,1)`: 83
- `(1,0,1)`: 92
- `(2,0,0)`: 208
- `(3,0,0)`: 100
- `(4,0,1)`: 129
- `(5,2,0)`: 58

The controller returned 0 on 664 calls and completion value 1 on six calls.
The full 0x4C3C module hash had 31 values during execution because the mutable
tail changed; the identity-critical 0x4B5C prefix remained exact on all 670
calls. The runtime's loader-identity latch remained verified throughout.

Condensed machine-readable counts are in `adoption-summary.json`, SHA-256
`fe13ca28a2ad3316392aefb5b2a2a39a2248dbc92911967973983eaf548058ef`.

## Pilot text evidence

The optional `file1_queue` probe resolved and retained six raw selected-string
and window records in `pilot-text-bytes.jsonl`, SHA-256
`3da1ff6c8cba78fcd89bc453344c0081d22c7d2d3cee2e0774f16a55bb0583c3`.

Visual inspection confirmed these readable rendered panels:

- `captures/004-opening-battle-before-confirm-004.png`, SHA-256
  `9d7a9f93120f2cbfaf0b87b42e5ed47cf25769ea5dab03bfe304e9118f83f041`:
  speaker `Fei`, text `"Hiyaaaaa!"`.
- `captures/006-opening-battle-before-confirm-006.png`, SHA-256
  `b5e37b6b97b332077c5503b08cb9fa7c9424214bbc961b1773f88c60450d48b2`:
  speaker `Fei`, text `"Huff, huff... That's one down!?"`.

The field-14 capture was taken during the dark entry fade; map identity comes
from the read-only `FieldMain` probe, not an inference from that image.

## Evidence hashes and terminal status

- `actions.jsonl`:
  `080cfe4e9c827297b6704bbf305e37cfd0931bd2cb0e079c5b02323a50187152`
- `battle-events.jsonl`:
  `cd3b46c46fb097bae520994b9adc825c16ac159b3561f5eb7eb63fdf6fc9f0e0`
- `field-events.jsonl`:
  `03b38a9f79bc1f3b75826d577f332785b453f1f7a273cf028bcc5b87e80418f9`
- `file1-adoption.jsonl`:
  `5ca36eb1838a859ba654beeca87974ec6883095143d02ba64a06abb8ebc3a1e7`
- `exit.json`:
  `fb87093232b454332b5baa55ad7a706b9f4ea6779cde5f718c76ea2381394010`

`exit.json` records GDB return code 0, first-battle return true, field-14 return
true, 670 native adoption entries, and 17 driver screenshots. The retained
`failure.jsonl` contains one `SIGTERM` record. That signal is the driver's
intentional owned stop after the successful field-14 gate, not a runtime crash;
it occurred in field rendering three seconds after the field-14 entry. No
unexpected kernel entry or adoption-stop diagnostic occurred.
