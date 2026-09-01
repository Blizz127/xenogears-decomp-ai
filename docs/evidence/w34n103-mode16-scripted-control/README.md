# W34N103 — mode-16 scripted control

## Scope and retail anchor

- Starting HEAD: `68bdc7561313a85891551555a737ea35e9bb3427`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail image: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Disassembly: `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`
- jump table: 65 entries at `0x800702E4`
- initializer `[0x80081174,0x800811C0)`, 76-byte SHA-256:
  `864390670113403bf12e13be0047e28317ecfbac4c07f6af96e08ff8944f514c`
- update `[0x800811C0,0x800813E8)`, 552-byte SHA-256:
  `3028b49e6f82656e76562451f790b900c56c0217d1be9898426db7ebc87a3824`
- complete pair `[0x80081174,0x800813E8)`, 628-byte SHA-256:
  `ee425beae1da78b70b258628ecd08e3afce792fa842d91dd8e0d11208a58e984`

## Retail decode and production transcription

`wm_80081174` initializes slot-local sequence index `+0x50`, command `+0x20`,
and timer `+0x22` from retail tables `0x8009A6C0` and `0x8009A70C`, then advances
the sequence index from zero to one.

`wm_800811C0` dispatches the signed command through retail's 65-entry table:

- command 1 decrements the signed timer and advances only after it becomes
  negative;
- commands 2-7 publish values 1, 0, 2, 3, 4, and 5 to scheduler slot 6;
- command 8 plays bank-relative sound IDs `0x2E`, `0x2F`, and `0x30`;
- command 16 activates slots 3 and 4 with value 1;
- command 17 publishes value 2 to slot 3;
- command 63 fades the active audio manager over 240 steps, publishes
  `(slot 0, value 13)`, and writes `CCA4=2`, `D3CC=4`;
- command 64 clears `D554` and `D7CC` and leaves the controller inactive.

All handled nonterminal commands return to the timer command. Negative and
out-of-range commands are ignored exactly as retail's unsigned bound check
requires. The scheduler resolves both guest addresses symbolically; no guest
function pointer is cast to a host function pointer.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n103_mode16_scripted_control.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- initializer/update/full retail slice hashes: PASS
- initial command, timer, and post-read sequence index: PASS
- signed timer zero/negative boundary and next-entry selection: PASS
- exact command 2-7 claim mapping: PASS
- command 8 bank-relative sound IDs: PASS
- command 16/17 multi-slot claims: PASS
- command 63 fade, claim, and global state: PASS
- command 64 terminal state: PASS
- negative-command rejection: PASS
- symbolic scheduler initializer/update resolution: PASS
- M1 wrong initial timer table: detected by `init.script_seed`
- M2 skipped initial index advance: detected by `init.script_seed`
- M3 timer expiration at zero: detected by `timer.zero_is_live`
- M4 wrong command-3 claim: detected by `command3.claim`
- M5 wrong sound range: detected by `command8.sounds`
- M6 missing command-16 second claim: detected by `command16.claims`
- M7 wrong command-63 fade duration: detected by `command63.fade_state`
- M8 skipped command-64 exit: detected by `command64.exit`

All M1-M8 were killed by named assertions. The W34N102 lifecycle certificate
still passes, and the normal product build completes with `LINK OK`.

## Natural entrance-16 route

The detached entrance-16 route executed `0x80081174` once and `0x800811C0`
120 times. Neither address produced a stub hit. Both capture requests were
fulfilled in-frame, both upload pumps retained `unknowns=0`, and the bounded
loop returned normally.

- frame 60 SHA-256:
  `6e0771f9c0ea4951783d453b448a35882dc2e31ae0e4c6ea4decc4d89851d9f4`
- frame 120 SHA-256:
  `af9dfb802c212976135ee3b089d998828cb230e320383c8a87a6b78972c86860`

These are bit-identical to W34N102. That is consistent with this sequence's
natural 120-frame prefix: it waits 10 ticks, selects command 8, emits the
three sound commands on the following pass, and then remains in the retail
170-tick delay. It does not yet reach a visual claim or terminal command.

Five private mode-16 initializer addresses remain unresolved, each with 121
stub hits:

- `0x800813E8`
- `0x800817A0`
- `0x800819C8`
- `0x80081C3C`
- `0x80081FB4`

## Verdict and next target

`MODE16_SCRIPTED_CONTROL_RESTORED_NATURALLY_EXECUTED`

The next target is the second private pair `0x800813E8/0x80081470`. Its
initializer is 136 bytes and establishes camera/world position state used by
its update callback; it is the next unresolved slot in retail registration
order.
