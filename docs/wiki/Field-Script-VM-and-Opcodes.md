# Field Script VM and Opcodes

Decoded field script VM behavior from committed handoff notes and source references. Noah is cited only as readability corroboration — retail asm and runtime traces are authoritative.

## VM dispatch (decoded / trusted)

- Primary handler table: `g_FieldScriptVMHandlers` — opcodes 0–255
- Extended table: `g_FieldScriptVMHandlers2 = &g_FieldScriptVMHandlers[256]` — opcodes 256–482 (483 total handlers)
- Byte **`254`** (`0xFE`) = `FieldScriptVM2Run` — prefixes extended opcode
- Dispatch: `src/field/scripts/virtual_machine.c` (handoff cites line 250)
- Full handler name list in `pc_port/src/data_field.c`

## Key opcodes (documented)

| Opcode | Name / handler | Role | Status |
|--------|----------------|------|--------|
| `0xA7` (167) | `func_8009F5F4` — `OP_UPDATE_CHARACTER` | Per-frame player update; `scriptFlags & 0x4000` branches walk vs idle | **Committed** `e7b0101` |
| `7` | `func_8009EB78` | Start actor script routine; scans 8 slots for free `(priority==0xF, busy==0)` | **Decoded**; slot-init fix `9e1b667` |
| `54` | `FieldScriptVMHandlerVariableSetTrue` | Set script variable true | **Decoded** |
| `55` | `FieldScriptVMHandlerVariableSetFalse` | Set script variable false | **Decoded** |
| `116` (`0x74`) | `func_8008F668` | Set warp dest; calls sound cue path | **Reached** in exit trace |
| `203` | `FieldScriptCheckTriggerZone` | 3D height-aware trigger poll | **Proven** zones 8–11 |
| `254` + ext | `FieldScriptVM2Run` | Extended opcode dispatch | **Decoded** |
| `0xA8` (168) | `FieldScriptVMHandlerMulVariableWithRand` | `var = (rand() * (arg+1)) >> 15` | **Decoded**; PSX rand range issue on PC |
| `0x5F` (95) | `func_8009AD6C` | Actor direction handler | **Committed** `2627286` |
| `0x23` (35) | HideActor | Hides actor (actor 18 init script) | **Observed** |
| `179` / `180` | FadeOut / FadeIn | Transition fades | **Not reached** on current exit route |
| `152` / `71` / `234` | Map-load family via `func_80092894` | Writes `g_GameSceneMapNum`, entrance, `D_800ADBEC` | **Gated** behind exit path |

## Map1 trigger systems (proven)

### Encounter zones (0–7) — 2D

- Opcodes at IPs 6152–6176 poll zones 5, 2, 7, 1, 0, 4, 6
- Handler: `FieldScriptHandleTriggerZone2D` (`misc11.c:844`)
- Inside path: `misc11.c:873–874` → subroutine → **`func_80093B10`** enables random encounters
- **All 7 zones are encounter-region activators**, not map transitions
- Verified: zone 5 fires with Fei at documented position (`94d8cfc`)

### Exit zones (8–11) — 3D height-gated

- Opcode **203** polls zones 8/9/10/11 at IPs 5991/6018/6045/6072
- Inside path: **`misc11.c:992`** (`FieldScriptCheckTriggerZone`)
- Armed when script vars `1116/1118/1120/1122 == 0`
- Transition bytecode after inside: **op7** + **op116** + **op54** + jump + clear
- Full transition chain (if reached): map swap opcodes → `FieldMain` reload → `FieldLoad` → **`func_800A5C40`** (INCLUDE_ASM)

## Exit route script decode (actor 48 routine 1)

Zone 11 path (documented static decode):

```
0x17b8: trigger zone 11 → target 0x17d0
0x17bc: if var0x0462 == 0 → continue else jump 0x17cd
0x17c4: op7  (actor 18, routine 4)
0x17c7: op116 (sound cue)
0x17ca: op54 set var0x0462
0x17cd: jump 0x17d3
0x17d0: op55 clear var0x0462
0x17d3: stop
```

**Var `0x0462`** is a **local latch** preventing repeated op7 — not a fade consumer (`e13c0e3`).

## Actor 18 script state (decoded)

| Routine | Offset | Content |
|---------|--------|---------|
| 0 | `0x069f` | Sprite init (`0xBC`) + stop |
| 1 | `0x06a1` | State machine on **`var0x0408`** |
| 4 | `0x07cf` | Short Y-rotation door animation (no fade/map-load) |

Actor 18 routine 1 branches:

- `var0x0408 == 0` → `0x06af`
- `var0x0408 == 1` → sleep/stop at `0x0710` (**current path**)
- `var0x0408 == 2` → rotation branch `0x071e`
- else → fallback `0x0777`

## Actor 20 — owner of `var0x0408` (frontier)

- Routine 1 at `0x0888` uses `0xA8` random chooser (max 4) then dispatches states 0–4
- Fallback at `0x0997` assigns `var0x0408 = 1`
- **PC port bug:** host `rand()` range breaks chooser → always falls back to `1` (`8db4fb2`)

## Actor script slots (decoded / committed)

- 8 slots per actor; base `p + 0x8C + i*8`
- Idle/free flag word: `0x003cffff` (priority `0xF`, busy clear)
- **Bug:** `func_80080A74` used wrong slot base → busy bit set on idle slots
- **Fix:** `9e1b667` mirrors retail writes

## Opcode 7 decode example (zone 11)

After trigger, bytes at IP 6084: `07 12 24`

- Target actor selector `0x12` → actor **18**
- Routine byte `0x24` → script/routine **4**
- Priority **1**

## What is NOT decoded enough to implement blindly

- Full fade/map-load chain past op54 (still tracing script state)
- `func_800A5C40` field reload (INCLUDE_ASM)
- Battle overlay handoff (`func_80281204`, `LoadGameStateOverlay(2)`)
- Full encounter selection path in `func_80079288`
