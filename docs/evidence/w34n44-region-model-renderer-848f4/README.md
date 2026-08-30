# W34N44 — retail region-model renderer `0x800848F4`

## Anchors and scope

- Starting HEAD: `0b1ffd33ae7c04bfc17e69aff2e3c2cd680edecf`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail bytes: `disc/world_map.bin`
- Retail symbol map: `config/symbol_addrs.slus_006.64.txt`
- Correct retail boundary: `[0x800848F4, 0x80084D00)`, `0x40C` bytes / 259
  instructions.  `0x80084D00` is the already-separated region scanner; the
  old header's `0x80084DB8` end was wrong.

The old port body was not a bounded approximation of this function.  It used a
4-byte record stride, treated `record+0x20` as a pointer, copied data into
ever-advancing scratch locations, and never projected or dispatched a model.
Retail walks the `C620` table with a `0x54`-byte stride.

## Natural record census

The first natural call on the accepted route reported:

- `D7E0=18`, `C620=0x800D9540` (`GUEST_KSEG`)
- 17 active records (`state==0`), one inactive record
- records 2 and 3 have live `+0x50=0x800D9540` parent links; the link and its
  terminator are guest-domain values
- record model headers at `+0x40` are guest-domain values
- record double-buffer work pointers at `+0x48/+0x4C` are native low pointers

Thus the production seam must rebase model headers and the OT through
`PSX_ADDR`, while preserving the work pointer as a native pointer.

## Restored retail flow

`wm_800848F4` now performs the full retail sequence:

1. initializes the scratch scale/zero vector and live native renderer globals;
2. walks signed `D7E0` records at `0x54` bytes, reloading the count at the tail;
3. skips nonzero record states;
4. copies the record MATRIX and installs its position translation;
5. follows each guest `+0x50` parent, publishes that parent's translation,
   left-multiplies the child basis, and transforms the child position;
6. wraps camera-relative X/Z through `wm_80093534`;
7. applies the retail `0x800` scale and `CompMatrix(camera, record, output)`;
8. projects the zero vertex, rejects negative FLAG and `SZ3 >= 0xD80`;
9. selects `record+0x48 + 4*D7F0`, loads the signed variant from
   `0x8009AD2C`, and calls `func_8002C700`.

The old body also mis-decoded two signed MIPS addresses.  `LUI 0x8006` plus
signed immediates `0x95C0/0x9578` means native main-executable globals
`D_800595C0/D_80059578`, not guest addresses `0x800695C0/0x80069578`.
`D_80050104` is likewise the native renderer authority.

## Model-packet OT domain finding

The restored function immediately exposed a second real defect.  The model
primitive walkers wrote `(u32)(uintptr_t)out & 0xFFFFFF` into a guest world OT.
On the first 601-frame run this produced exactly 601 safe adapter range aborts,
with links such as `0x00631670` and `0x00632910`.  The adapter correctly refused
to dereference them.

All 13 port-mode model primitive walkers now publish through
`PcPort_LinkModelPrim`: the 12 walkers owned by `game_overrides.c`, plus
`func_8002E688` in the `XENO_PC_PORT` branch of `system/temp2.c`.  The helper
installs the retail tag length and delegates to `PcPort_AddPrimDomainAware`.
Guest OTs receive a guest packet address; native field OTs retain the old
native-link behavior.  A focused natural witness reset only the diagnostic
counters at `wm_800848F4` entry and observed:

```text
W34N44_848F4_LINKS model_emitted=13 guest=13 native=0 rejected=0
```

## Certificates

`pc_port/tests/run_w34n44_848f4.sh`:

- O0 PASS
- O2 PASS
- nonrecovering UBSan PASS
- strict warnings clean
- M1–M14 DETECTED: stride, state gate, matrix source, parent chain/rotation,
  camera subtraction, wrap, scale, composition order, FLAG/depth gates,
  buffer phase, signed variant lookup, and dispatch omission

`pc_port/tests/run_w34n44_model_prim_link.sh`:

- O0 PASS
- O2 PASS
- nonrecovering UBSan PASS
- strict warnings clean
- raw-native-link and missing-tag-length mutants DETECTED

Normal port build: `LINK OK`.

## Natural acceptance

Accepted route and input:

```text
XENO_TEST_INPUT=0:0x2000,600:0x4000,916:0x2000,976:0
XENO_WORLD_TEST_INPUT=0:0x2000,600:0x20,601:0
detach before PcPort_WorldMapInitMain
```

Post-repair result:

- natural world-state exit: `frames=601 D7CC=0`
- capture request/fulfillment frame equality at 60, 120, and 600
- OT adapter aborts: range/alignment/length/steps = `0/0/0/0`
- frame hashes returned to the established pre-abort baseline:
  - frame 60: `4310197d3881367bc7859ef174dcf4b9afa8759f0b7dc2ca3eea94b0f84f1521`
  - frame 120: `8039c2f96b88e09491eaf1cba049d0efd829f146aa943da744f56c93e85b1c16`
  - frame 600: `2b636ae21af4bf0003f60d5e53f8ffd9a042fc5a72457e4c096cff19814e1a38`

The detached process remains live after the natural world-state exit at the
already-known `SoundHandleError` stub boundary.  Two test PIDs were terminated
with `SIGTERM` only after their logs had recorded `frames=601 D7CC=0`; this rung
does not claim a whole-process `rc=0` beyond that external subsystem boundary.

The helper and the model-link repair are one atomic production change: enabling
the retail model dispatcher without fixing its packet publication creates a
known-invalid guest OT on every displayed frame.
