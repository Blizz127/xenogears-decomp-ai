# Known Addresses and Globals

Human-readable table of important addresses and globals **already documented** in committed handoff notes. Status reflects handoff as of `8db4fb2`.

| Symbol | Meaning | Status | Evidence source |
|--------|---------|--------|-----------------|
| `D_8006F94E` | Field map selector; copied to `g_GameSceneMapNum` | Harness + decoded | Handoff `XENO_FIELD_MAP`; `port_main.c` |
| `D_8006F954` | Spawn entrance index → field script var 2 | Harness + decoded | Handoff `XENO_FIELD_ENTRANCE`; `port_main.c` |
| `D_800AFE9C` | Synthetic/real d-pad direction injection word | Verified | Handoff direction map; movement proofs |
| `D_800ADC18` | Field fade-in counter; gates `FieldAddPrimitives` | Decoded / trusted | `misc3.c:299` init; `misc4.c:21` decrement; `misc2.c:1508` gate |
| `D_800ADBEC` | Field transition / reload state | Observed | Exit transition handoff; `-1` at idle |
| `D_800B00C0` | VM stall flag | Observed | Trigger zone 5 proof |
| `D_800ADBDC` / `D_800ADBE4` | Encounter gate flags | Observed | Encounter trigger proof |
| `g_PlayerActorIndex` | Current player actor index | Verified | Fei = 1 on Map1 milestone |
| `g_FieldActors[]` | Field actor table | Decoded | Slot-init / op7 diagnostics |
| `g_FieldScriptVMHandlers` | Primary VM opcode table (0–255) | Decoded | `virtual_machine.c`; `data_field.c` |
| `g_FieldScriptVMHandlers2` | Extended VM table (256–482) | Decoded | Handoff VM encoding note |
| `g_Scene` / `FieldScene` | Scene state incl. `worldToScreenMatrix` | Fixed layout | `main.h` `unk7C` fix; gdb offset proof |
| `g_Scene.sceneScrZ` | Scene projection Z (`+0x68`) | Committed default | `func_8007254C` fix → `0x200` |
| `g_Scene.unk48` | Scene flags; encounter enable sets `0xC000` | Observed | Zone 5 trigger proof |
| `g_CamInterpolation` | Camera interpolation state | Decoded | Camera corruption investigation |
| `D_8004FBB8` | Matrix copy destination for projection | Observed | Pre/post `FieldScene` layout gdb |
| `D_800AFB54` | Actor count during field load | Suspected | Map1 stack-smash context (`func_80080A74`) |
| `D_800AFD1C` | Current script-move actor index | Observed | `func_80098CAC` fix context |
| `D_800B2290`–`D_800B229C` | Encounter cooldown / selection state | Classified | `func_80079288` diagnostic |
| `D_800595D8` | Sound channel records | Classified | `func_800855C8` / `func_8003A20C` asm |
| `D_80059404` | Sound control state | Classified | `func_80039F9C` asm |
| `g_GameSceneMapNum` | Current field map number | Observed | Map-load opcode family |
| `g_FieldControl` | Field control gate for player opcode | Observed | `OP_UPDATE_CHARACTER` runtime |
| `g_FieldScriptMemory` | Field script variable storage | Decoded | Var `0x0462`, `0x0408`, `0x0418` traces |
| Script var `0x0462` | Local exit latch (actor 48 routine 1) | Decoded | `4856738`, `e13c0e3` |
| Script var `0x0408` | Shared door state (actor 20 writes, actor 18 reads) | **Active frontier** | `0545ef3`, `8db4fb2` |
| Script var `0x0418` | Actor 18 local rotation counter | Decoded | Routine 1 state-2 branch |
| Script vars `1116`–`1122` | Exit zone "used" flags | Observed | Exit system handoff (`== 0` = armed) |
| `func_8009F5F4` | Opcode 0xA7 `OP_UPDATE_CHARACTER` | **Committed** | `e7b0101` |
| `func_8009EB78` | Opcode 7 actor-script starter | Decoded / fixed | `9e1b667` |
| `func_80080A74` | Actor script slot initialization | **Committed fix** | `9e1b667` |
| `func_80098CAC` | Script-move sprite pointer path | **Committed fix** | `9e1b667` |
| `func_800855C8` | Field sound cue wrapper | Shim (no-op) | `ae8c753` |
| `func_80079288` | Random encounter management | Stub / classified | Post-op54 diagnostic |
| `func_80093B10` | Enable random encounters subroutine | Proven live | Zone 5 trigger |
| `func_800A5C40` | Field load helper (INCLUDE_ASM) | Future blocker | Exit transition handoff |
| `func_8001B6C4` | Battle state main | Stubbed | `XENO_KERNEL_SEL=1` |
| `func_801C62A8` | Menu overlay entry | Overlay address | `XENO_KERNEL_SEL=4` harness gap |

## Actor indices (Map1, documented)

| Actor | Role |
|-------|------|
| 1 | Fei (player) |
| 18 | Exit transition target actor (op7 routine 4) |
| 20 | Owner of shared `var0x0408` door state |
| 48 | Exit zone poll routine (var `0x0462` latch) |

## Flag words (actor script slots)

Documented from op7 slot diagnostic:

- Idle/free: `0x003cffff` → priority `0xF`, busy `0`
- Buggy init: `0xffff0000` → priority `0xF`, busy `1` (blocked op7 before fix)
- Active scheduler: `0xffdf0000` → priority `7`, busy `1`

## PSX vs host type pitfalls

| PSX type | Host pitfall | Example |
|----------|--------------|---------|
| `u_long` (4 bytes) | Host `u_long` often 8 bytes | `FieldScene.unk7C` layout drift |
| `rand()` | PSX `0..32767` vs glibc large range | Actor 20 `var0x0408` chooser |
