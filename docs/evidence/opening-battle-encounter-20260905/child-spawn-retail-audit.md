# Child sprite spawn / heap-stop audit — 2026-09-06

Read-only bounded audit of child creation and transform backing sizes. No desktop/process interaction, no production edits, no callback-map re-audit. Root owns fixes and tests. This /tmp report is the only write.

## Outcome

No allocation-size, packed-pointer-width, or child-copy overrun mismatch was found in the audited native `800233A4 -> 80023A48 -> 80023B84` path. The observed heap stop occurs BEFORE the current child allocation returns, so none of the current child's copy or animation-binding writes have executed. The first bad heap writer remains UNRESOLVED; stop RAM does not contain the live native heap-head global or faulting local pointer.

One concrete source/retail mismatch was found in `func_80023538`: native code skips the retail transform recomputation after applying the default scale. This can leave a stale transform matrix; it is not established as the heap corruption cause and is not reached by the current failed allocation.

## Pins and source authority

Repository `/var/home/blizz/Projects/xenogears-decomp-ai`, branch `experiment/worldmap-open-gates-20260823`, HEAD `3a3e7aac03a2f166fb924945a489e392d706f282`, dirty concurrent work preserved.

- `disc/SLUS_006.64` SHA256 `dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119`.
- `pc_port/src/game_overrides.c` at audit read: `62088396db6b712eb2030eb650366e1cf7680c06c17bafcb052cc28bdd28d733`.
- `src/slus_006.64/system/temp1.c`: `4d7c5d28a4b4956f871d49f6f0d1d44aa33677ec27e0453b7105aafb95893524`.
- `src/slus_006.64/system/animation_scripts.c`: `e423b31fa6b7cb881f677a141b8dfa7c4d4ebdafdc3dc177639cb4316241bff7`.

Compared actual SLUS bytes using PS-X EXE file offset `guest - 80010000 + 800` and Capstone MIPS32 little endian disassembly. The native temp1 file contains dormant asm/matched-comment variants; the actual crash backtrace identifies production owners in `game_overrides.c`, which were audited rather than confusing those inactive alternatives with active code.

## Observed stop and temporal limit

`pc_port/build_native/opening-callback-trace-5-orqx88rm/run.log:1282-1302`:

- SIGSEGV `HeapConsolidate`, memory.c:342, evaluating `pOther[-1].userTag`.
- Caller `HeapAlloc(allocSize=236, allocFlags=0)`, memory.c:191.
- `func_800233A4(pOwner=0,dataSize=0)`, game_overrides.c:3147.
- `func_80023A48(type=4,mode=0,pAnimPackage=007C3B28,dataSize=0,pOwner=0)`, :3213.
- `func_80023B84(parent=007C3A18,script=007C35F8,package=007C3B28)`, :3494.
- Native opcode E0 handler via guest battle animation interpreter.

`HeapAlloc` performs pending consolidation at memory.c:190-191 before its allocation search. Consequently this invocation has not returned a new child and cannot yet have executed the current invocation's task initialization, transform copy, `23538`, or `24730`. `23B84` has already set parent+B0 bit11, and may have changed the native timer flag according to parent+B0 bit8; both writes match retail. Earlier invocations could still be relevant, but the saved backtrace cannot identify them as first writers.

## Allocation and backing-size comparison

| Mode | Native requested bytes, extraSize=0 | Retail evidence | Backing boundaries |
| --- | --- | --- | --- |
| 0 | `EC` | `80023AAC`, a1=0, then `800233B0` adds EC | two tasks at wrapper+0 and +1C, sprite at +38 occupies B4 bytes; end wrapper+EC |
| 1 | `EC + 58 + (N-1)*18 = 12C + N*18` | `80023AF8..80023B14` | transform at sprite+B4 = wrapper+EC; frame buffer at sprite+F4 = wrapper+12C, room for N entries of18 bytes |
| 2 | `EC + 54 = 140` | `80023B2C..80023B44` | transform at sprite+B4 = wrapper+EC, backing54 bytes |

`game_overrides.c:3145-3161` follows retail `800233A4..8002343C`: allocation flags read D_800591AF, timer/render registration, sprite initialization, packed task+4 sprite pointers, tick/free callback registration, same order. The maximum initialization word at sprite+B0 ends at sprite+B4, exactly within the smallest EC-byte wrapper.

`game_overrides.c:3193-3236` follows retail `80023A48..80023B80`: fifth argument is the owner from o32 caller stack+10; types5/6 substitute the ADDRESS of retail package globals, native declared byte arrays at game_overrides.c:2891-2892; frame count reads the packed first u32; allocation extras match; wrapper pointer stored at sprite+6C as u32; size halfword at sprite+86 is extra+EC (retail intentionally excludes caller dataSize); package stored at sprite+24 as u32.

`game_overrides.c:3166-3190` uses packed u32 pointer slots for both shape helpers. Mode1 frame storage starts immediately after its 0x40-byte transform header. Mode2 extra54 contains the initializer's +40 word, ending at +44 within54. No 64-bit pointer store is used in these active overrides.

Matrix backing is not widened by host `long`: the active shim includes PsyCross `pc_port/extern/PsyCross/include/psx/libgte.h`, whose MATRIX has nine shorts and three ints (32 bytes), VECTOR has four ints (16 bytes). `SpriteComputeTransformMatrix` in animation_scripts.c:1415 places MATRIX at base+C, so even a full32-byte matrix ends at base+2C, within both transform allocations. Mode1 frame storage does not begin until base+40. The dormant original game's long-based header is not the port header.

## Spawn-copy comparison

`game_overrides.c:3473-3584` matches the retail ordering `80023B84..80023FD4` for:

- Mark parent+B0 bit11, temporarily suppress D_800591AC when bit8 is set; derive type from child script (type3 inherits parent's type); derive mode; allocate with owner parent+6C and extra0.
- Mark child task bit29; set type/mode; copy selected flags, scale/frame/state fields using the corresponding u8/u16/u32 widths, retaining newly allocated transform pointer rather than copying parent+20.
- Share parent+7C only when parent does not own it (A8 bit0 clear); clear child A8 bit0; set child+70 parent backlink.
- Copy six position/movement words; for nonzero mode copy exactly six halfwords of transform rotation/scale (`80023F10..80023F84`) to freshly allocated backing.
- Bind script through `23538`, then post-init through `24730`, restore timer flag and return child sprite (`80023F88..80023FA4`).

The retail halfword-copy instructions reload the backing pointers for each component, while native caches them once. With the freshly allocated nonoverlapping child specified by this constructor, those slots are not written by any of the six copies; no practical mismatch was found on this valid construction path. No broad structural memcpy or native-width pointer copy exists here.

`func_80023538` uses packed u32 script pointers at +58/+64/+54, and its direct writes remain inside the B4-byte sprite. Transform writes are gated on non-null sprite+20. Mode1 direction reset requires a non-null base+34, which its fresh shape helper zeroes. The independent state clear at +7C requires A8 ownership bit0; `23B84` clears this bit before binding.

## Concrete transform-order mismatch

Native `temp1.c:495-501`:

```
if (!header_bit13) {
    if (D_800591AD) SpriteSetScale(sprite, D_800591A8);
} else if (D_800591AD) {
    SpriteComputeTransformMatrix(sprite);
}
```

Retail `800236C8` branches to `800236F4` when bit13 is set. When bit13 is CLEAR and D_800591AD is nonzero, it calls SpriteSetScale at `800236EC` and then FALLS THROUGH into `800236F4`, reloads D_800591AD and calls SpriteComputeTransformMatrix at `80023708`. Therefore both bit13 cases perform recomputation when the flag is set; the clear-bit case additionally applies default scale first. Native incorrectly makes recomputation exclusive to the set-bit case.

Small source-backed repair when root chooses to own this behavior: after the conditional scale block, perform the flag-gated recomputation independently, retaining the retail second read of D_800591AD. A meaningful focused check should observe Scale -> Compute order and final matrix for a nonzero scale differing from inherited scale, plus bit13 set and flag0 controls. This is a transform correctness repair, not a demonstrated heap repair; no edit or test made by this audit.

## Saved RAM supporting facts, not heap reconstruction

Saved RAM SHA256 `efcc6acf407ce421acd463f90b88d9e9be777f1c0c7067dfa88841b8c70b0972`. Pinned RAM base `00612E20`; parent offset `001B0BF8`.

- parent+20=`007C3ACC` (parent+B4), +24=`007C3B28`, +6C=0, +7C=`007C3B0C`.
- parent+3C=`40000000`, +40=`00108000`, +A8=`2001F800`, +AC=`0B008000`, +B0=`00000800`.
- Child script first halfword `0400` directly gives type4, mode0; operand bytes `8B FF` encode -117, and `007C366D - 117 = 007C35F8` agrees with the backtrace's requested script. This is a consistent E0 dispatch/size request.

These fields do not prove heap-head validity, native `pCurrent`/`pOther`, or allocation ownership boundaries; those were not captured. No scan result has been substituted for the live heap chain.

## Minimal next runtime observation

At the next naturally reached `HeapConsolidate` entry/failure, capture native `g_Heap`, `g_HeapNeedsConsolidation`, `pCurrent`, `pOther`, the sizeof/offset layout of HeapBlock, the current/other headers, and bounds of the mapped RAM. Record the chain walk read-only from the actual live g_Heap and stop at its first invalid link, identifying the predecessor/header whose pNext fails.

Once that exact damaged link is known, a reproducible normal replay can watch its pNext/header from allocation onward. If child construction remains implicated, record its allocation return bounds and dump headers before/after `23804`, six-halfword copy, `23538`, and `24730` for that allocation; include parent/child backing pointers and mode. This distinguishes a pre-existing bad chain from a child overrun without inventing a first writer or modifying guest state. Until then heap first writer = UNRESOLVED.
