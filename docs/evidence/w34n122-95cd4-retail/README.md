# W34N122 — retail `wm_80095CD4`

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD/origin: `6c68006f3d9bbca494976ae6e85d2ddbe9d25734`
- Retail boundary: `[0x80095CD4, 0x80095F78)` (676 bytes)
- Retail slice SHA-256: `743e266e712d1b5cb9cb4187a9693f961128b910444917a0b38d92e69ad0cb69`
- Verdict: **REPAIRED_VERIFIED**

## Retail divergences repaired

The prior body was not an exact transcription.  The retail listing and focused
certificate establish these corrections:

1. The ABI has five arguments.  The caller-supplied movement mode is forwarded
   unchanged to `wm_800951A8`; the old body fabricated that argument from the
   projected X word at `0x1F800060`.
2. `wm_80084D00` receives retail's stack attribute slot.  The port now uses the
   reserved dead guest-stack address `WM_95CD4_FRAME_ATTR=0x801FFDE0`, preserving
   the W34C18 pointer-domain repair without a native stack pointer.
3. Every `wm_80085418` iteration reads the same halfword attribute.  The old
   body advanced through a host array.
4. A rejected candidate writes `0xFFFF` to the candidate record beginning at
   `0x8009D718`, not to the attribute frame.
5. The candidate list is in main RAM at `0x8009D718`.  The old transcription's
   `0x1F800000 + 0xD718` expression selected unrelated memory.
6. Retail delegates to `wm_800951A8` when `rejected == count`; the old body
   reflected velocity on that condition.
7. Terrain offset/clamp and lower-bound crossing order now follow the retail
   branch sequence, and MIPS `mult` low-word plus arithmetic shifts are modeled
   without C signed-overflow undefined behavior.

The only current caller was updated to the five-argument ABI.  The helper is
still dormant on the accepted route until `wm_8008E76C` dispatches through its
retail `slot+0x20` state; that dispatcher is the next bounded target.

## Focused certificate

`pc_port/tests/run_w34n122_95cd4.sh` passes:

- O0
- O2
- O2 nonrecovering UBSan
- strict `-Wall -Wextra -Wconversion -Wsign-conversion -Werror`
- retail binary-slice hash

Nine mutants are detected by named assertions:

- M1 wrong forwarded mode
- M2 reversed all-rejected resolution
- M3 advancing attribute address
- M4 invalidating the attribute frame instead of the candidate record
- M5 missing terrain offset
- M6 missing lower crossing clamp
- M7 wrong fixed-point shift
- M8 full rather than half reflection
- M9 scratchpad rather than main-RAM candidate base

The certificate also checks both wrap-helper calls, exact vector scaling,
candidate-record stride, the fixed attribute value, input read-only behavior,
and the resolver ABI.

## Isolated full build and dormant-route neutrality

The staged delta was built in isolated worktree `/tmp/xeno-w34n122.rHBjpq` so
unrelated shared-worktree edits could not enter the result:

```text
LINK OK -> pc_port/build_native/xeno-port (port-owned addresses verified)
```

The exact isolated binary ran the accepted detached mode-0 route for 120
frames with scripted input
`0:0x2000,600:0x4000,916:0x2000,976:0`.  It reached the bounded exit, fulfilled
both capture requests in their requested frames, emitted no `[worldmap-stub]`
line, and showed no adapter-abort signature.

Because the repaired helper is dormant under the still-incorrect dispatcher,
neutrality is exact:

```text
frame 60  317a339ec91020f13d9e44c53e600e4b2266760b9750684f91bd15ee8cfd5332
frame 120 c94d9e9e17904cdd53c8a51b34359af82f0b8cc2b48927e454624edcf7dfc714
```

Both post-repair BMPs are byte-identical to their pre-repair W34N122 census
artifacts.  This proves neutral-when-dormant, while the next dispatcher rung
must prove the corrected semantics on the natural live state-2 route.
