# Archive-20/file-1 controller: safe native-adoption audit

Date: 2026-09-06 UTC  
Repository: `/var/home/blizz/Projects/xenogears-decomp-ai`  
HEAD inspected: `3a3e7aac03a2f166fb924945a489e392d706f282`  
Scope: read-only. Production/config/runtime/UI state was not changed. Source pins are in `/tmp/xeno-file1-native-adoption-20260906/source-pins.sha256`.

## Result

The reviewed `0x801E6CE8..0x801E71D4` controller can be adopted by the native port only as a **module-identity-gated bridge substitution** with explicit emulated-RAM bindings and guest-call reentry. It must not be added as a global native meaning for address `0x801E6CE8`.

The narrow dispatch behavior should be:

1. When the interpreter reaches `0x801E6CE8`, verify that the currently loaded module is exactly archive directory `0x20`, set-index entry `0`, file `1`, destination `0x801E5000`, size `0x4C3C`, full-load SHA-256 `64668d85...636b670`.
2. If identity matches, extract `u16(a0)`, `u8(a1)`, `u16(a2)`, run the compiled file-1 controller through port bindings, store its signed 32-bit result in guest `v0`, and return bridge-handled.
3. If identity is absent or differs, return bridge-unhandled so the interpreter executes the bytes currently loaded at that address. Do not abort and do not dispatch the file-1 C by address alone.

This placement is required because `PcPortMipsRun` asks the bridge before fetching each instruction and resumes a handled call at guest `ra` (`pc_port/src/battle_mips_adapter.c:409..440`). The current runtime deliberately returns unhandled for all guest targets before native symbol resolution (`battle_mips_runtime.c:646..662`). The file-1 interception therefore belongs immediately before that broad guest-code fallback and must include the identity test.

## Current executable path

### Module load and current identity gap

Retail base battle selects and loads the module through this exact sequence:

- `func_8008AB4C` calls `ArchiveSetIndex(0x20,0)` at `asm/battle/nonmatchings/main/func_8008AB4C.s:3..12`.
- `func_80070E2C` calls `ArchiveReadFileToBuffer(1,0x801E5000,0,0x80)`, synchronizes, then calls the loaded initializer `0x801E5160` (`asm/battle/133C.s:12..46`).
- `ArchiveSetIndex` stores the selected directory base in `g_CurArchiveOffset` (`src/slus_006.64/system/libarchive.c:75..85`); for this retail header it is 3087.
- The port's ordinary non-stream `ArchiveReadFile` completes the CD read synchronously and sets the archive state idle before returning (`pc_port/src/archive_port.c:551..595`). This makes the return from the bridged `ArchiveReadFileToBuffer` a valid point to validate and record the complete payload before the initializer mutates file-local data.

`BattleMipsRuntime` currently has no loaded-module identity field. `target_is_guest_code` accepts the entire `0x801D0000..0x802FFFFF` window (`battle_mips_runtime.c:374..380`), even though archive modules reuse those VMAs. The generated bridge map also contains unrelated `0x801E...` names, but runtime initialization intentionally skips every symbol at or above base battle (`battle_mips_runtime.c:196..233`). This protects the current interpreter path and is why adding a name to the map would not be a safe adoption mechanism.

The identity hook can stay local to `battle_mips_runtime.c`: observe successful bridged calls to `ArchiveSetIndex` and `ArchiveReadFileToBuffer`, invalidate the active module before every load overlapping the `0x801E5000` window, and set file-1 identity only after the exact tuple and full payload digest pass. Reset the identity at `func_80070F40` entry and clear it at exit (`battle_mips_runtime.c:875..908`). At dispatch, retain the load generation and validate the immutable tables/code span `[0x801E5000,0x801E9B5C)` if writes outside the archive bridge cannot be ruled out. Its SHA-256 is `8c071b7c6edffcb922b798ceddfb1a09167d22b402ae5d0d268d9252bf4a15c4`.

Digest constants are provenance metadata; the retail payload itself must remain outside Git.

### Existing guest/native call topology

The current runtime already provides both halves needed for nesting, but it lacks a multi-argument native-to-guest call interface:

- **Guest to native:** lower resident targets resolve through the generated retail symbol map or the exact `func_%08X` fallback, reject generated stubs, translate pointer-shaped arguments, call the host function, and place the result in `v0/v1` (`battle_mips_runtime.c:646..777`).
- **Native to guest callbacks:** packed work-list slots remain 32-bit. `WorkListInvokeCallback` routes KSEG callback identities through `PcPort_BattleMipsDispatchCallback`, aborting unresolved guest callbacks rather than casting them to host functions (`pc_port/src/work_list_port.c:21..39`; packed layout at lines 41..67).
- **Nested reentry:** `runtime_bridge` saves/restores `runtime->bridge_cpu` (`battle_mips_runtime.c:780..787`). `run_guest_callback` creates a fresh CPU and, when nested under a guest-to-native call, starts at the parent guest SP (`lines 829..850`). `PcPort_BattleMipsDispatchCallback` requires an active battle runtime and executes only recognized guest ranges (`lines 853..862`). Native sprite hooks use this path at `pc_port/src/game_overrides.c:2958..2976`.

`run_guest_callback` only supplies `a0` and has no return-value or stack-argument interface. The file-1 controller needs guest calls with 0, 1, 2, 5, and 7 arguments, so reusing it directly would lose arguments 2..7.

## Required controller bindings

The frozen scratch C cannot be linked unchanged into the native build. Its externs model an exact-target address space for the differential fixture; they are not the live native representation.

### Packed/global RAM binding

Runtime memory resolution sends battle and dynamic-module addresses to `PSX_ADDR`, while only symbols physically below `BATTLE_BASE` may bind to native shared globals (`battle_mips_runtime.c:291..323`). Therefore the native controller binding must use the runtime read/write resolver for:

| Retail location | Native interpretation |
|---|---|
| `0x800D3278` | load packed `u32` guest pointer, resolve to control block, minimum `0x828` bytes |
| `0x800D2D28` | load packed `u32` guest pointer, resolve to UI block through at least `+0xCF` |
| `0x800D2DAC` | load packed `u32` guest pointer, resolve to window through at least `+0x97` |
| `0x800D3340` | load packed `u32` guest pointer, resolve to string table before native `GetStringEntry` |
| `0x800D3014` | direct guest byte |
| `0x801E9C10..1A` | five file-1 default halfwords, read from the loaded module |
| `0x801E9C1C`, `0x801E9C30`, `0x801E9C34` | file-1 mutable signed words in the loaded module |

Each source-level reload must remain a fresh packed-global load. In particular, reload `D_800D3278` after setup/yield boundaries, reload `D_800D2DAC` after `GetStringEntry`, and reload UI/window pointers at the close and teardown sites. The reviewed candidate and its alias control explicitly require this order (`/tmp/xeno-file1-controller-review-20260906.md:31..64`). Caching host pointers for the full call would weaken the accepted behavior.

Use a checked resolver rather than applying the masking `PSX_ADDR` macro directly to an unvalidated packed word. The current macro masks to the 2-MB address window (`pc_port/src/psx_memory.h:20..30`); a corrupt or wrong-domain value could otherwise silently alias unrelated RAM.

### Resident calls that should stay native

The resident window/string bodies are already compiled native in `src/slus_006.64/system/system.c`:

- `func_80032F54`
- `func_80034614`
- `GetStringEntry`
- `func_80034714`
- `func_800345E0`
- `func_800346D4`

Pass them checked host pointers resolved from packed guest addresses. `GetStringEntry` returns a host pointer inside the resolved table (`system.c:236..238`), which can be passed directly to native `func_80034714`; reload the window global after lookup before queueing. The window routines operate on the provided host object (`system.c:786..851`).

The constructor needs a named typed adapter. The retail controller supplies seven arguments, with rows at original guest `SP+0x18`; the native constructor retains eight arguments with a dead `mode` before `height`. The existing guest bridge correctly reads the untranslated low 16 bits, sign-extends them, writes `args[6]=0`, and supplies height in `args[7]` (`battle_mips_runtime.c:727..739`). A native controller call bypasses that bridge, so it must call the same extracted adapter, for example:

```c
void PcPort_BattleWindowConstructRetail7(
    void *window, int32_t tx, int32_t ty, int32_t x, int32_t y,
    int32_t width, uint16_t rows);
```

Its only ABI conversion is `func_80032F54(window,tx,ty,x,y,width,0,(int16_t)rows)`. The current dedicated regression proves the observed tuple, signed-low-16 behavior, ignored eighth-slot poison, fail-closed stack read, and unrelated-target isolation (`pc_port/tests/battle_window_constructor_abi_test.c:34..177`). Extracting the conversion into one shared helper would prevent the guest bridge and native controller paths from drifting.

### Overlay calls that must remain guest

Do not bind these names to generated/native stubs:

| Target | Args | Reason |
|---|---:|---|
| `0x8008F8F4` | 7 | base-battle helper, still retail guest code |
| `0x800716D8` | 0 | base-battle yield/pump; itself calls other guest/dynamic code |
| `0x8008FA60` | 1 | base-battle cleanup helper |
| `0x801E6750` | 5 | local helper in the same validated file-1 payload |
| `0x801E5B00` | 2 | local presentation helper using file-1 state |

Add one internal typed service rather than five host-symbol aliases:

```c
int battle_call_guest(BattleMipsRuntime *rt, PcPortMipsCpu *native_caller,
                      uint32_t target, const uint32_t *args,
                      unsigned argc, uint32_t *result);
```

It should reject an inactive runtime, targets outside base battle or the currently validated dynamic module, and `argc > 7`; initialize a nested CPU with the current bus; reserve the controller's absent retail `0x50`-byte guest frame below `native_caller->gpr[29]`; put args 0..3 in `a0..a3`; put args 4..6 at nested `SP+0x10`; set `ra=BATTLE_HALT_PC`; run to halt; return `v0`; and propagate failure. This preserves the outgoing o32 stack slots that original controller calls used. While the nested CPU crosses a host boundary, the existing `runtime_bridge` save/restore makes that nested CPU the active `bridge_cpu`, so work-list callback reentry inherits the correct deeper SP.

The service must not reuse `PcPort_BattleMipsDispatchCallback`: that public callback interface intentionally models one `void *a0` only.

## Minimal production interfaces

A small port-only binding layer can preserve the reviewed state machine without duplicating it:

```c
typedef struct XenoBattleCommandFile1Port {
    BattleMipsRuntime *runtime;
    PcPortMipsCpu *caller;
    uint32_t guest_frame_sp;
} XenoBattleCommandFile1Port;

int32_t PcPort_BattleCommandFile1Controller(
    XenoBattleCommandFile1Port *port,
    uint16_t string_index, uint8_t command_byte, uint16_t flags);
```

Internally, give the controller narrow `load8/load16/load32/store8/store16/store32`, `load_pointer_global`, `call_guest`, and six typed resident-call bindings. Keep raw addresses private to this file-1 implementation. The decomp/matching target can retain its exact-address declarations; the native target should compile the same reviewed state-machine body against these port bindings. If macros or an included body are used to share it, rerun the full differential against the actual compiled production body so the binding refactor itself is covered.

The runtime-side handler is correspondingly small:

```c
static int bridge_file1_controller(BattleMipsRuntime *rt,
                                   PcPortMipsCpu *cpu,
                                   uint32_t target);
```

It returns 0 unless `target==0x801E6CE8` and the active module identity matches. On a match it builds the port context, calls the typed controller, stores `cpu->gpr[2]`, and returns 1. All nonmatching modules retain raw interpreter behavior.

Likely production touch set after review:

- `pc_port/src/battle_mips_runtime.c`: identity state, archive-call observation, typed guest-call service, identity-gated bridge hook.
- New port-only file/header for controller RAM/resident/helper bindings.
- The reviewed controller body from the module decomp lane, compiled once for the native target through those bindings.
- `pc_port/build_port.sh`: add only the port binding/controller sources; do not add a global `0x801E6CE8` symbol to the generic bridge map.

No archive implementation change is required if identity is observed at the existing guest/native bridge. If the loader later gains a general module-notification API, it can replace that observation without changing controller dispatch semantics.

## Required gates before enabling the substitution

1. **Identity/fallback test:** exact file-1 load tuple plus digest selects native; wrong directory, file, destination, size, digest, stale generation, and another module at `0x801E5000` all execute the loaded guest code instead.
2. **Adopted-path differential:** execute the raw controller and the actual bridge-selected native body from identical emulated RAM. Compare return, all control/UI/window bytes, module words, call order/scalars, and normalized pointer identities. Reuse the accepted 463-case extension and require the existing semantic mutants to fail by comparison, not compilation/crash.
3. **Guest-call integration:** cover all 0/1/2/5/7-argument calls, stack arguments at reserved `SP+0x10`, nested callback reentry, guest-call failure propagation, and restoration of `bridge_cpu` after success/failure.
4. **Constructor ABI:** keep `battle_window_constructor_abi_test` green through the shared seven-to-eight adapter.
5. **Packed-global controls:** reject host-symbol aliases, stale control/window/UI pointers across helper calls, missing module-local writes/restores, and invalid packed guest pointers.
6. **Interpreter fallback:** with adoption disabled or identity mismatched, demonstrate the existing raw `0x801E6CE8` execution remains byte-for-byte behaviorally unchanged.
7. **Natural runtime:** only after the above, replay the ordinary opening and confirm the same pilot panels, ordinary Circle progression, teardown, and battle return. This is runtime evidence, separate from the differential.

## Blockers and limits

- The accepted controller remains scratch-only. Production module ownership/config/extraction is being handled separately.
- Independent review reports the structural body GREEN, but the standalone PSYQ text is 1,256 bytes versus 1,260 retail bytes and differs from offset zero. Exact matching is **FAIL**, while native adoption is **NOT_RUN** (`/tmp/xeno-file1-controller-review-20260906.md:76..91`). A policy decision is still required on whether structural differential proof is sufficient for native adoption before exact match.
- No general multi-argument native-to-guest service exists. The current one-argument callback dispatcher is insufficient.
- No loaded dynamic-module identity/generation exists in `BattleMipsRuntime`.
- The scratch candidate's direct extern globals would bind the wrong storage domain in a native build; a checked emulated-RAM binding is mandatory.
- The real bodies of `0x801E6750`, `0x801E5B00`, `0x8008F8F4`, `0x800716D8`, and `0x8008FA60` are intentionally left in the interpreter. The controller differential models their boundaries and does not prove all helper alias effects; exact source reload order must therefore be retained.
- The immutable-span hash above is newly computed from the pinned local payload. Full payload and controller pins remain `64668d85...636b670` and `1f11c9ac...427a2e`, respectively.

## Evidence pins

See `/tmp/xeno-file1-native-adoption-20260906/source-pins.sha256`. Key current files:

- `pc_port/src/battle_mips_runtime.c`: `e671118805b557588947557075f77870e1ea4357b4a99f6d9b97ab6308d42097`
- `pc_port/src/battle_mips_adapter.c`: `4e0e896434e6d2f5452b8ac6f82799414ce8378bf9278fa7403cd083a4f2bf22`
- `pc_port/src/work_list_port.c`: `43e987a651805d662c7c976532982a0882e248e14a9e6f58816bcf6c91d8255c`
- `pc_port/src/archive_port.c`: `c39e542288fc5eb5f2e5778a9a1a02c9125de8d7a527deb96751ecf64d0493b9`
- `src/slus_006.64/system/system.c`: `b75fb09b069abd5462b73c0530b7365879cf2f5af5dfa8256a05bd807a4a67db`
- reviewed scratch controller: `a3085667773e2cc494e5c863a8a07bd92832e64abc2d5f3a9e716f5ca959fa4a`
- archive `(0x20,0)/file1`: `64668d85bf48dea46cf6d5f04b38e38ca887877cfef53961f0c26cd2d363b670`
