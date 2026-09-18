# Retail atlas work-buffer restoration

This follows the naturally observed mountain Jackal battle abort recorded in ../lahan-mountain-battle-corruption-20260907/README.md. The callback failed after graphics packets overwrote its retail code.

## Source-backed defect and change

Retail SLUS 80026BA4..80026DCC takes table, index, X, Y and an ordering-table entry. At80026BDC..80026C10 it loads the global graphics cursor and end and compares cursor+40*count strictly below end. At80026C2C..80026C40 it takes each packet from that cursor and advances it. At80026D08 it loads the fifth argument and passes it directly to AddPrim. No sixth argument is read. The only direct battle call found is800BDD80; its delay slot supplies the ordering-table pointer at callerSP+10.

The prior C implementation instead accepted a sixth packet pointer, did not reserve graphics-buffer space, treated argument five as a short OT index, and confused texture-page coordinates with UVs. The replacement restores the five-argument API, global buffer allocation with strict capacity check, retail texture addressing and direct OT link. Source ownership is limited to func_80026BA4 in src/slus_006.64/system/temp1.c; unrelated existing edits are preserved.

## Verification

The new pc_port/tests/run_atlas_work_buffer_retail_test.sh extracts the actual production body. Its C fixture executes the pinned retail SLUS instructions including the real GetClut, GetTPage and AddPrim callees with the existing MIPS interpreter. It compares the complete atlas/packet/guard/OT fixture and the final graphics cursor. 3000 cases per O0, O2 and UBSan pass: zero through four records; exact fit, one byte short and one byte spare; signed randomized atlas fields and screen offsets. This is finite instruction-behavior equivalence, not compiled-byte identity or end-to-end Lahan proof.

The pre-change test failed with SIGSEGV. Four mutants are rejected: permitting exact fit, omitting cursor advance, incorrect TPage inputs and incorrect UV inputs. Native build links successfully with74 function stubs; fixed executable SHA256 daa4dd9b36ab001bf5530ba910a2440eda62b18483f92dafbf4f0412acea1b4a. Host UBSan could not link its installed runtime; all three configurations passed inside the existing toolchain container.

## Pending

A frozen unmodified normal-boot run is active under scratchpad/lahan-natural-20260907-watch, game586760, driver586757, execution session24362, GDB session27147, Xvfb586758 display:1. A breakpoint on the suspect function is armed to capture the actual caller and bad sixth argument. Revalidate these processes before reuse. Corrected-binary natural battle return, complete Lahan story, exact compiled matching and retail audiovisual comparison are not yet established.

The shared PSX `make build` also succeeds after the repair. The rebuilt object reports func_80026BA4 size544 bytes; retail spans552 bytes. This function is therefore still not a byte-matched decompilation despite the finite behavioral tests passing.

## Natural writer confirmation

The unmodified frozen run naturally reached field15 and a different ordinary enemy encounter. The breakpoint captured callback800BDD3C calling native80026BA4 from retail800BDD80, with table index129, X144, Y42. GuestSP was1F800390: its fifth word at+10 was the valid OT pointer006E4BC0; the unowned sixth word at+14 was800BA960. Generic bridge translation turned that code address into host006D6A20, which the old C accepted as pPrimBuffer. The actual graphics cursor was00726530, end0072B4E0. A hardware watchpoint on code800BA960 then stopped inside the old function as it wrote the packet length byte09. This directly confirms the writer and argument mismatch. GDB source line text was newer than the frozen executable; argument values, machine instructions, and addresses are the relevant evidence.

After recording the first write, the owned unmodified process was terminated in GDB; the driver and Xvfb exited. Sessions27147 and24362 are terminal. No live RAM repair or callback bypass was performed. The corrected binary still needs a fresh natural-battle run.

Corrected natural run launched under scratchpad/lahan-natural-20260907-atlas-fixed, session50891, game623679, Xvfb623677, display:1. process.json records the driver. Its frozen executable matches the fixed SHA above. This is the current live run; revalidate before continuing.

The fixture now includes full-width signed X/Y arguments, explicitly INT32_MIN, INT32_MAX, -1 and0. The same3000 cases per configuration and four rejected mutants pass; output is preserved in retail-tests.log. This checks the retail modulo32-bit additions before halfword stores.

## Corrected natural runtime result

The corrected frozen executable progressed normally through opening battle, fields14/13/1 and a normal field15 encounter. The one-shot GDB observation captured actual func_80026BA4 execution with table index129, X144, Y42 and valid OT006E4BC0. Its graphics cursor was00726530, end0072B4E0. The code at800BA960 was intact both at this call and in the later crash dump. The previous atlas-corruption failure is resolved on this observed route; the battle return is not yet proven.

At23:02:05 CDT, the run aborted on the existing unimplemented spriteA7 handler. The core is preserved locally in scratchpad/lahan-natural-20260907-atlas-fixed/core.623679. All game/driver/Xvfb processes and execution session50891 are terminal; observation session82797 detached normally and is terminal.

A7's operand pointer initially looked suspicious because it was inside SpriteData. Retail battle800C1FC8..800C1FE0 explains this: C8 calls8001FBA4 to resolve the operand variable, then calls8001FBE4 with the requested opcode. The core has s5=C8, s1=007ED19D and bytes A7 03 8E CC...; a2=00752E11 is the resolved variable, value00. This is a valid variable-indirect A7 invocation, not established pointer corruption.

Next repair authority: jtbl_800183D8[A7-8A] targets8001FDF0. Operand bit7 sets g_WorkListCurTimer=(operand&7F)+1. Otherwise the routine computes ((operand+1)*((spriteAC>>7)&FFF))/256, clamps a zero result to1, then adds it modulo16 bits to sprite9E. The retail body ends at8001FE64. No A7 source change has been made yet; add the retail differential fixture before implementing. Remaining story, exact byte matching and audiovisual acceptance are still open.
