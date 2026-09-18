# Battle target-chain live-RAM adoption contract

Status: PROPOSED, NOT IMPLEMENTED. No native dispatch enabled by this document.

## Scope and authority

The eventual port must execute the production C target-selection chain against
the loaded battle overlay's actual RAM. The fixture globals are not a runtime
ownership model. Preserve the existing generic overlay rejection in
`initialize_runtime` and `runtime_bridge_call`; do not register all overlay
symbols or route them through generated native placeholders.

The initial boundary covers only `80083FF4`, `800841E0`, and `80084A7C`.
It does not include Attack input processing, audio, battle transitions, or a
new interpretation of eligibility. Use one source implementation per body,
shared between legacy/global and checked-RAM access forms. Do not hand-copy
the algorithm into an adapter that can drift independently.

Current evidence: eligibility 69/69 words; selection 49/49; list builder
115/218, still NOT MATCHING. All-real-chain fixture passes 122880 cases per
O0/O2/UBSan with seven mutations and a bounded independent review. None of
this proves live adoption, complete overlay matching, or natural gameplay.

Retail payload SHA-256:
`1830b4ef1fe37129972fc310dfad534f8161d6c0b123e74254c3711334a3e291`.
Loaded code authority must be checked independently of the native build:

| Entry | File offset | Bytes | SHA-256 |
| --- | --- | --- | --- |
| `80083FF4` | `14504` | `114` | `1cd53256b57c208c551a4d2b42855760ab72723916768831f980649e455313f8` |
| `800841E0` | `146F0` | `368` | `8591fbf353f29325387d719127537f0d8b7c2befaadb8c004841791279fdb73d` |
| `80084A7C` | `14F8C` | `C4` | `f76589bfaede7934be8547730b95c9af5492a4e500d3372512cba4c25f5e9df8` |

Offsets and sizes above are hexadecimal. Hashes identify retail bytes, not
permission to substitute nonmatching code or bypass the remaining exact gate.

## Memory interface

Use width-specific read/write operations or equivalent checked lvalues. Legacy
expansions must retain existing generated instruction matches. The RAM form
must operate on the actual `g_PsxRam` allocation, preserving overlapping views.

| State | Address calculation | Access |
| --- | --- | --- |
| Presence | `800D2DCC + target` | read byte |
| Block flag | `800C3EB7 + target*28` | read byte |
| Alternate flag | `800D32A1 + target*8` | read byte |
| Group | `800C3EB4 + actor_or_target*28` | read byte |
| Rank | `800CCD34 + target*170h` | read little-endian halfword |
| Normal/alternate status | `800CCD64 / 800CCE08 + target*170h` | read little-endian halfword |
| List | `800C3E90 + index` | read/write byte; retail clears 12 |
| Count | `800D3274` | read/write byte |
| Context owner | `800C3EAC` | read four-byte packed pointer |
| Actor target | `context + (actor & FFh)*40h + 3Ch` | read byte |
| Current selection | `context + 2E8h` | write byte |
| Matrix owner | `800D3364` | read four-byte packed pointer |
| Matrix value | `matrix + 140h + actor_group*64 + target_group*8` | read byte |

Rules:

1. Decode pointer slots as exactly four bytes; never read a host `void **`
   from guest storage. Validate KSEG0/KSEG1 RAM membership before converting
   aliases. Reject arbitrary high bits; do not mask invalid pointers into RAM.
2. Validate the actual derived access width and alignment, using checked
   addition before pointer construction. Do not require a fabricated whole
   context or maximum-sized matrix allocation when only one byte is touched.
3. Validate lazily, in the source's access path. Absent/blocked/alternate
   eligibility must not touch the matrix pointer; a null matrix on those
   paths is legal. Do not eagerly evaluate both sides of a short circuit.
4. Reload packed context owners at source access points. Do not cache a host
   context across calls or synchronize a shadow copy back into guest RAM.
5. Typed row declarations are addressing views, not proof of allocation
   extent or valid game actor count. Preserve the low-byte argument masks.
6. Guest stack storage is potentially observable RAM. The list builder's
   retail frame is `sp-50h`, with candidate bytes at frame `+10h` and matching
   bytes at `+20h`, plus saved registers. Selection has its own `28h` frame.
   A valid matrix/context pointer may alias those bytes: ordinary host-local
   arrays do not reproduce that behavior. Either reproduce the observable
   frames and scratch accesses, or prove a supported non-aliasing boundary
   before selection and decline to guest execution when it cannot be proved.
   Include indirect overlap through pointer slots and globals altered by
   earlier writes; a one-time snapshot of a matrix pointer is insufficient.
   Any declined valid case remains a native-port coverage gap, not completion.

## Dispatch and failure contract

Follow existing bridge outcomes: zero means not selected, one means handled,
negative means failed. Before selecting an entry, require the active runtime
and parent CPU to match, current loaded-code identity, and an explicit
allowlist. Do not remove the ordinary `target_is_guest_code` fallback guard.

Identity covers the transitive substituted call chain under one generation:
eligibility requires its own body; list requires list plus eligibility;
selection requires all three. An unchanged selected entry is not authority
for bypassing a modified descendant. Test each descendant independently.

Identity failure before selection leaves guest execution available and makes
no adapter writes. Once selected, an invalid access or identity/generation
change must stop execution through the bridge failure path. Preserve already
performed guest writes; never replay the original guest function afterward.
The existing file-1 adapter provides an example of this distinction, not a
drop-in ABI or lifecycle for the battle target chain.

Return values for eligibility/list are zero-extended scalar values in `v0`.
Selection is currently declared void: audit its retail call sites before
choosing adapter return-register handling. Preserve callee-saved CPU state;
do not assume file-1's outgoing stack-frame size or fabricate guest instruction
counts for native work. Record native calls separately from emulated work.

Current bounded call-site audit: the battle assembly has direct calls at
`800811C8` and `80082A58`. The first calls `80077698` immediately afterward;
the second overwrites `v0` with `19h` before its next use. Neither directly
consumes the selection result. This is not proof about indirect calls or
other overlays; do not silently promote it to a complete ABI census.

## Ordered implementation and acceptance gates

1. Extract/share the existing body logic through a minimal access interface.
   Re-run all three exact gates: eligibility and selection must remain exact;
   the list gate must continue reporting its real mismatch without weakening.
2. Build the checked-RAM access form in an isolated test, with runtime dispatch
   disabled. Repeat full-chain comparisons using one packed RAM image rather
   than separate fixture globals. Include overlapping field views and both
   cached/uncached aliases.
3. Prove rejection of bad segment, overflow, range and alignment; unused null
   matrix paths; byte-write canaries; context rebind behavior; and retained
   partial writes with no guest replay after a selected failure.
   Add matrix/context pointers into each active guest frame and scratch area;
   prove either identical observable frame behavior or a no-write decline
   before selection. Unsupported alias cases must still execute as guest.
4. Add a narrow bridge selection gate and test missing/wrong/modified code
   identity for every transitive callee, stale generation, foreign runtime/CPU,
   real ABI returns, and no side effects before selection. Keep ordinary live
   dispatch disabled until these tests, independent boundary review, and
   step 5's exact-match dependency gate pass.
5. Resolve the remaining list instruction-match gate before ordinary live
   dispatch of either list or selection; selection unconditionally calls list.
   Eligibility-only adoption still requires all its own RAM/ABI/identity
   gates. Any explicitly opt-in development experiment with a nonmatching
   dependency must remain labeled NOT MATCHING and cannot satisfy the full
   decompilation goal or enable default dispatch.
6. Build the complete native port and observe the natural battle route with
   this exact build, proving dispatch and gameplay separately. No teleport,
   forced eligibility, scripted victory flags, or fixture state as live proof.

Open design work: body-sharing mechanism, loaded battle identity lifecycle,
selection return-register use, and integration with existing build ownership.
No interface choice here authorizes publication or changing unrelated work.
