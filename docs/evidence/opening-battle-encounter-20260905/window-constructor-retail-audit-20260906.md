# Full window-constructor retail audit, 2026-09-06

## Result and scope

The constructor is **not byte-for-byte state-equivalent to retail**, even for the opening's six captured battle tuples: native `system.c:75` clears the raster allocation; retail does not. Both retail and native HeapAlloc flag 2 select best fit and do not clear the returned payload. The extra clear executes for all six tuples. Its actual changed-byte count and visible consequence in the opening are **UNRESOLVED**, because the captures contain only post-constructor work bytes, not the pre-allocation payload on retail.

For these six tuples, static instruction-to-statement comparison found matching constructor-owned header scalars, allocation request sizes/order, background geometry, every row's geometry/UV/metadata and palette-selection rule, primitive clones, and the two draw-mode commands. This is **static agreement**, not a completed independent full-constructor execution oracle. Runtime JSON captures only the first row and first 128 work bytes, so it cannot certify the other rows or all work bytes. The already-repaired bridge argument adaptation is deliberately outside this audit.

Only this report and `/tmp/xeno-window-constructor-audit-20260906/` were written. No production source, shared prior scratch, runtime, UI, or replay was changed or launched.

## Authority and reproducibility

Repository: `/var/home/blizz/Projects/xenogears-decomp-ai`. Retail instructions were freshly decoded directly from `disc/SLUS_006.64` with Capstone MIPS32 little-endian; file offset = address - `80010000` + `800`. Source-faithfulness authority is the retail bytes, not the translated source.

| Artifact | SHA256 |
|---|---|
| Whole `disc/SLUS_006.64` | `dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119` |
| Retail constructor `[80032F54,8003342C)`, file offset `23754`, 1240 bytes | `e48e0f7d63d082ab40246cb43b6bf7e00bd53775a72e822c744f0d10ed0f6be5` |
| Native `src/slus_006.64/system/system.c` | `6e9b2c746304d38cdb72de0cb22f112a6c2809e90cbbd42ef02e43297a66bdc8` |
| Extracted constructor source text, lines 39–135 | `7a3374f3f83ab32f058ba90d9a3033043d4711b51b99716ceae0a8e36a311f36` |
| `docs/evidence/opening-battle-encounter-20260905/window-and-return-runtime-20260906.json` | `68dcd3b970b6a1cb5520037bcee895c3abfd77b16850f72d77fd322c1a417df2` |

Scratch `pins.json` additionally pins the native allocator, PsyCross GPU helper sources, and the complete retail helper ranges. `constructor.disasm.txt` covers all 310 constructor instructions including delay slots; `constructor-source.txt` preserves the exact reviewed function. Other disassembly files cover HeapAlloc `[80031BDC,80031F70)`, content-tag setter `[800324B8,800324C4)`, GetTPage `[80043A1C,80043A58)`, SetSemiTrans `[80043BFC,80043C24)`, SetDrawMode `[800454DC,80045534)`, its mode helper `[800459DC,80045A34)`, and texture-window helper `[80045C10,80045C94)`.

## Opening inputs and expected sizes

All six captured guest constructors use tpageX=896, tpageY=256, and caller `801E6FC0`. Define `W = (width low16)|1`, `P = low16(W*4)`, `S = low16(W+3)`, and `H = signed16(rows)`. The constructor stores W/P/S as halfwords. On these positive inputs all arithmetic below is ordinary integer arithmetic.

| Capture index | x,y,width,H | W / P / S | Row allocation | Raster allocation | Background x,y,w,h |
|---|---|---|---|---|---|
| 9 | 28,24,24,4 | 25 / 100 / 28 | 384 | 784 | 21,19,113,66 |
| 10 | 50,24,39,8 | 39 / 156 / 42 | 768 | 1176 | 43,19,169,122 |
| 11 | 32,148,48,8 | 49 / 196 / 52 | 768 | 1456 | 25,143,209,122 |
| 12 | 68,148,30,8 | 31 / 124 / 34 | 768 | 952 | 61,143,137,122 |
| 13 | 92,24,27,8 | 27 / 108 / 30 | 768 | 840 | 85,19,121,122 |
| 14 | 32,24,48,8 | 49 / 196 / 52 | 768 | 1456 | 25,19,209,122 |

The post-constructor headers and captured first-row words agree with these values. Both draw modes are length 2, command `E100001E` / `E100001F`, then zero. Their retained low-24 tag bits differ across captures, which is expected: the constructor does not initialize the link field. First-row metadata CLUT is `7C00` in the six captures; odd-row CLUT was not captured.

## Complete constructor-owned packed output

Offsets below are hexadecimal, relative to the window unless a row is named. All pointers occupy exactly four packed bytes. Do not replace these accesses with host-width pointers/struct copies.

| Output | Retail instructions | Matching native source |
|---|---|---|
| +04=x, +06=y, +0A=width then width OR 1, +0C=H, +0E=tpageY, all halfwords | 80032F7C, 80032FB0..80032FE0 | system.c:50,58,64–68 |
| +08=low16(W*4), +12=low16(W+3), +14=14 | 80032F90,80032FE4..80032FF8 | :55,69–70 |
| +10,+82,+84 halfword zero; +8C word zero | 80032F80..80032F8C | :51–54 |
| bytes +68,+69=1; +6A,+6B,+6C,+6D=0; +6E=FF | 80032FA0..80032FD0 | :56–63 |
| +28=row allocation; +2C=raster allocation | 8003301C,80033040 | :72,74 |
| Background at +48: retain tag low24; tag length byte +4B=3; word +4C=62000000 after semitrans; xy at +50; wh at +54 | 80033038..8003309C | :77–82, with general-input qualifications below |
| Copy all 16 bytes +48..+57 to +58..+67, including retained tag bytes | 800330A0..800330BC | :84–87 |
| Draw modes at +30 and +3C, each 12 bytes; retain tag low24, set length 2 and two command words | 800333A4..80033408 plus helper | :131–134 |

Untouched window bytes are part of the output contract too: +00..03, +16..27, +6F..81, +86..8B, plus retained link bytes in draw/background packets. The constructor does **not** reset current text/cursor pointers, queue timing +86/+88, or the entire window. Consequently old header/string values in later constructor snapshots are not evidence of a missing constructor assignment. No whole-window memset exists in retail.

Each row is 0x60 bytes. Let i be the nonnegative row index, k=floor(i/2), odd=i&1, T=tpageY+13*k, U=(tpageX&63)*4, V=T&255. In the opening, T for eight rows is 256,256,269,269,282,282,295,295; V is 0,0,13,13,26,26,39,39. Four rows use the first four values.

| Row offsets | Retail final state and ownership | Native statements |
|---|---|---|
| +03,+17 | primitive length 4 | :106,108 |
| +07,+1B | opcode 65; retail writes 64 then ORs bit 1 | :107,109 directly write final 65 |
| +08 / +1C | packed xy: x / x+256, and y+14*i | :100–101 |
| +0C / +20 | UV halfword U|(V<<8) | :104–105 |
| +10 | if signed16(P)<257, 000D0000 OR signed16(P); otherwise 000D0100 | :97,102; signed mismatch for negative P, below |
| +24 | if signed16(P)<257, 000D0000; otherwise 000D0000 OR (signed16(P)-240) | :98,103 |
| +28..3B / +3C..4F | full 20-byte copies from +00..13 / +14..27 | :111–120 |
| +50,+52,+54,+56 | halfwords tpageX,T,S,13 | :121–124 |
| +58 | halfword zero: initial populated glyph width | :125 |
| +5A,+5B,+5C | bytes odd,i,T | :127–129 |
| +5E | halfword Palette2 for odd, Palette1 for even | :126 |

Retail row geometry is at 800330EC..800331A8; UV at 800331AC..800331E8; primitive tag/opcode writes at 800331EC..80033260; clones at 80033264..800332D0; metadata at 800332D4..8003338C. Loop count rereads signed window+0C at 80033390..8003339C.

The odd/even CLUT sources are actual retail addresses `80059414` (Palette2; load 80033334) and `800595D4` (Palette1; load 80033344). Constructor row +0E/+22 CLUT fields, RGB bytes, low24 tags, and byte +5D are retained allocation bytes, and those packet bytes are copied into both contexts. The runtime's varying +5D bytes and packet junk are therefore expected retained state. No constructor code copies +5E into packet CLUT fields: the draw/update path does that later. Preserve this distinction in tests.

The opening widths all take the P<257 branch; the right packet is constructed with width zero. Retail's large-width branch subtracts **240**, not 256. Although later rendering overwrites populated widths, the constructor differential must retain this actual formula and test that otherwise-unexercised branch.

## Allocation and external helper boundaries

Call order is: tag29 at 80032FF4; HeapAlloc(96*H,2) at 8003300C; tag28 at 80033018, storing first pointer in its delay slot; HeapAlloc(28*signed16(S),2) at 80033030; SetSemiTrans(window+48,1) at 80033098; GetTPage(0,0,signed16(tpageX),signed16(tpageY)) at 800333C0; SetDrawMode(window+30,0,0,u16(tpage),NULL) at 800333D8; second GetTPage with signed16(tpageX)+64 at 800333EC; second SetDrawMode at 80033404. Native :71–74,82,131–134 has the same helper sequence and opening arguments, with the extra memset between the second allocation and SetSemiTrans.

Retail content setter writes the current halfword tag. HeapAlloc rounds requests to four bytes, chooses the smallest suitable free block for flag2 (80031D64..80031D84), and mutates allocator headers. Its normal allocation paths 80031CD8..80031D50 and 80031E68..80031F54 do not initialize returned payload bytes. The constructor requests zero-sized rows too when H=0, then skips rows but still allocates raster and writes the background/draw modes. Do not silently suppress either allocation in an oracle stub. Negative H would become a huge unsigned allocation request; allocator failure behavior is a separate boundary, not a sensible live replay case.

Native allocator source `src/slus_006.64/system/memory.c:162–304` likewise contains no payload clearing. Its port-specific caller-address debug metadata differs from MIPS by design and is outside this constructor-body comparison. This audit does not certify the entire allocator implementation, consolidation, or out-of-memory handler; a constructor oracle should model successful allocations deterministically and compare requested sizes, flags, order and packed pointer outputs separately from heap policy.

The native GPU boundary is PsyCross (`pc_port/extern/PsyCross/src/psx/LIBGPU.C:379,501,631`, with macros in its `include/psx/libgpu.h:224,230,246,254,283`). GetTPage masks coordinate bits and produces 001E/001F here. SetSemiTrans ORs mask02 into byte7, turning the background opcode 60 into 62. SetDrawMode's NULL texture window produces zero, and its mode helper produces E100001E/E100001F for these arguments. Retail has a GPU-type-dependent alternative mode mask at 800459DC..80045A30; both branches agree for these two tpages and dfe=dtd=0. Thus this constructor's opening helper results agree without claiming general PsyCross/retail GPU-helper parity.

## Concrete mismatches and applicability

1. **Raster memset: exercised by every opening constructor.** system.c:75 writes `28*S` zero bytes through +2C. Retail 80033030 returns from HeapAlloc, 80033040 stores its pointer, and execution continues directly to background construction; the remainder of the function never writes through +2C. The whole range has no corresponding clear helper or inline loop. Nonzero retained raster bytes produce different post-state. Captures establish native zeros in only the first 128 bytes, not what retail would have retained. No claim is made here that removing the memset alone preserves currently visible text; establish the natural decoder/upload initialization path and differential state first.

2. **Signed packing at low/negative x: not exercised by the six tuples.** At 8003306C..80033084 retail loads signed16(x), subtracts7, and ORs the entire result with `(signed16(y)-5)<<16`; native :79 masks x-7 to 16 bits. For x=0,y=24, retail background word is FFFFFFF9; native's intended 32-bit word is 0013FFF9. Retail row xy at 80033104..80033118 similarly ORs signed16(x) unmasked; native :100 masks x. With x=-1,y=24, retail is FFFFFFFF versus native intended 0018FFFF. The second row packet has the analogous distinction when signed16(x)+256 is negative. These are real packed-output differences, even though the retail behavior is surprising.

3. **Signed/overflowed width packing: not exercised by opening widths.** Retail left width uses LH at 80033168 and OR at 8003317C; native :97 uses a u16 for the OR operand. Example width=8192 gives W=8193,P=8004: retail left word FFFF8004, native 000D8004. Native background width :80 masks `signed16(W)*4+13` to 16 bits while retail 8003305C..80033094 ORs its complete 32-bit value. Width=-5 gives retail background word FFFFFFF9 versus native intended `(heightPixels<<16)|FFF9`. Native signed shifts of negative values also invoke C undefined behavior, whereas MIPS shifts have defined bit behavior. These should be fixed with explicit unsigned bit operations if general constructor parity is pursued, not by imposing conventional geometry on retail.

4. **Raw height rather than stored signed halfword: outside the captured, normalized input domain.** Native :72,:80,:94 reuse its full s32 height; retail allocation, geometry and loop use signed window+0C. For a direct native height=00010004, stored +0C is4 but allocation/loop are 65540 rows, unlike retail4. The current guest path supplies normalized signed16 height, so this does not reopen the already-fixed bridge issue. It remains relevant to a whole-function input-domain contract and independent constructor tests.

Native GetTPage calls pass raw coordinates whereas retail explicitly sign-extends them at 800333AC..800333BC. For the helper's masked coordinate formula these high-bit differences do not change returned tpages; do not label them a demonstrated output bug. Similarly, native's direct opcode65 writes have the same final packed output as retail's64-then-OR1 on the valid nonoverlapping allocation path. Retail reloads window fields/pointers during each row operation while native caches some; without aliasing/mutating helper effects, no extra opening mismatch is established from that implementation difference.

## Minimum independent differential-test scope

A typed bridge spy is insufficient. The minimum constructor gate should execute the **pinned retail constructor bytes** with MIPS delay-slot semantics and the **actual native function body**, against independently initialized, identical virtual memory. Use fixed, distinct low-address allocation mappings, capture actual native allocation calls, and normalize only mapped pointer identities when comparing guest/native output. An expected buffer reconstructed from these same native statements is not independent authority.

- Include all six captured tuples, all rows (0x60*H), all 0x90 window bytes, the entire 28*S work allocation, allocation redzones and retained bytes. Initialize window, row allocation and work allocation with different nonzero patterns; run another pattern so accidental shared zeros cannot hide missing/premature writes. Preserve exact prior contents for untouched fields and clones.
- Capture ordered helper calls and each allocation request/tag/flag. Use deterministically successful HeapAlloc stubs without implicit clearing. Execute retail GetTPage, SetSemiTrans, SetDrawMode and its two leaves, or independently test those helpers against their pinned retail bytes before using stubs. Include both palette globals with distinct values so an odd/even swap cannot pass.
- Opening exercises even and odd rows, even input widths rounded odd, and retained-state reconstruction. Add H=0 and H=1, width63 versus64 (P252 versus260 across the split), nonzero textureU, and tpageY near255 so row-pair UV wrapping is exercised. H0 must still make both allocation requests and leave row payload untouched.
- For a full-domain claim, add x0/x6/x7, negative x/y, width8192 and negative/overflowing width, and high-bit scalar variants. Bound or intercept impossible allocation requests so those cases cannot allocate/write huge regions. Use the retail helper-call trace as the expected outcome for pre-allocation edge cases; do not force a successful unsafe loop.
- Run actual native code at O0/O2 and with undefined-behavior checks for exercised safe cases; the current signed-negative shift cases must be explicitly addressed before making an edge-domain C-parity claim. The current native function should fail the nonzero-work fixture because of its extra clear. After correction, negative controls should independently restore the extra raster clear, swap palettes, change the right-width subtraction240→256, or suppress the second allocation, and each appropriate case must detect its defect.

After that constructor gate, decoder/raster/upload/packet execution and an unforced opening replay remain separate acceptance work. Constructor-byte agreement alone cannot certify text timing, glyph rasterization, palette contents, GPU sampling or the complete opening.
