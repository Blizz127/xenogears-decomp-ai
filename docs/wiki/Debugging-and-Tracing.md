# Debugging and Tracing

Practical gdb/logging commands from documented handoff workflows. Logs are stored under `captures/render_diag/` (gitignored locally).

## Prerequisites

- Build with debug symbols: `./pc_port/build_port.sh` (uses `-g -O0`)
- Run inside `distrobox enter xenogears-dev`
- For field routes, always set **`XENO_KERNEL_SEL=0`** (otherwise process may not reach `FieldMain`)
- `rg` is **not** available in container — use `grep -E`

## Basic harness runs

### Map0 stability check

```bash
distrobox enter xenogears-dev -- bash -lc 'cd /home/blizz/Projects/xenogears-decomp && \
  XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 timeout -s KILL 45 ./pc_port/build_native/xeno-port'
```

### Map1 with diagnostics

```bash
distrobox enter xenogears-dev -- bash -lc 'cd /home/blizz/Projects/xenogears-decomp && \
  XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 XENO_FIELD_MAP=1 \
  XENO_FIELD_ENTRANCE=9 XENO_FIELD_0BB_VRAM_UPLOAD=1 \
  XENO_FIELD_DIAG=1 timeout -s KILL 90 ./pc_port/build_native/xeno-port'
```

### Filtered log (stub / frame / assert events)

```bash
distrobox enter xenogears-dev -- bash -lc 'cd /home/blizz/Projects/xenogears-decomp/pc_port && \
  XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 XENO_FIELD_MAP=1 XENO_FIELD_ENTRANCE=9 \
  XENO_FIELD_0BB_VRAM_UPLOAD=1 timeout -s KILL 90 build_native/xeno-port 2>&1 | \
  grep -E "RUN_RC|\[stub\]|func_8001E3D8|frame=|assert|SIG|Aborted|INSIDE3D|OP7_" | \
  tee ../captures/render_diag/map1_filtered_$(date +%Y%m%d_%H%M%S).log; \
  echo RUN_RC=${PIPESTATUS[0]}'
```

## Return code reference

| RC | Meaning |
|----|---------|
| 124 | `timeout` clean kill (success for harness) |
| 134 | SIGABRT (assert) |
| 137 | SIGKILL from `timeout -s KILL` |
| 139 | SIGSEGV |

## Gdb — field scene / projection

Documented probes from visual recovery investigation:

```gdb
# Break after FieldScene layout fix
break func_80024FF4
commands
  print &g_Scene.worldToScreenMatrix
  print (void*)((char*)&g_Scene + 0xD4)
  continue
end

# First actor quad projection
break RotTransPers4
commands
  print $C2_H
  print g_Scene.sceneScrZ
  continue
end

# Actor packet OT link
break ParsePrimitive
condition 1 polyTag->code == 0x2d
```

## Gdb — exit trigger zone 11 route

Documented synthetic input pattern:

1. Set env: `XENO_FIELD_MAP=1 XENO_FIELD_ENTRANCE=9 XENO_FIELD_0BB_VRAM_UPLOAD=1 XENO_KERNEL_SEL=0`
2. Inject `D_800AFE9C=0x1000` (+X) frames 1–105
3. Then `D_800AFE9C=0x2000` (+Z)
4. Break `misc11.c:992` for inside-3D trigger
5. Break `func_8009EB78` for opcode 7 slot allocation

Example gdb script location: `captures/render_diag/map1_exit_zone11_route_ent9_20260708.gdb`

## Gdb — opcode 7 slot state

Watch actor 18 slot flags after `func_80080A74`:

```gdb
break func_80080A74
commands
  bt
  continue
end

break func_8009EB78
commands
  printf "OP7_START_SCRIPT\n"
  continue
end
```

Expected after fix (`9e1b667`): slots initialize to `0x003cffff`; op7 advances IP.

## Gdb — var 0x0408 writer trace

Lightweight approach (heavy global watchpoints timed out in documented probes):

- Break `FieldScriptVMHandlerVariableAssign` / mul-with-rand handlers
- Log actor index + IP + value written
- Reference log: `captures/render_diag/map1_actor18_var0408_init_probe_20260708_150922.log`

## Stub oracle

Any `[stub] <symbol>` line in port output = next missing function on the live path.

```bash
distrobox enter xenogears-dev -- bash -lc 'cd /home/blizz/Projects/xenogears-decomp && \
  XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 timeout -s KILL 90 ./pc_port/build_native/xeno-port 2>&1 | \
  grep "\[stub\]" | sort -u'
```

Current stub count after recent fixes: **~250** generated function stubs (handoff `ae8c753`).

## What NOT to treat as signal

| Run config | Why invalid |
|------------|-------------|
| Host without distrobox / missing SDL | Toolchain/libs missing |
| `XENO_KERNEL_SEL=4` without overlay | Hits `func_801C62A8` stub — harness gap |
| `XENO_KERNEL_SEL=1` | Battle main stubbed |
| Dummy video without X11 for input milestone | Real keyboard milestone needs `SDL_VIDEODRIVER=x11` |
| Entrances 6 / 10 on Map1 | Harness-invalid; crash |

## Evidence file naming

Handoff convention: `captures/render_diag/<topic>_<YYYYMMDD>_<HHMMSS>.log`

Keep gdb scripts alongside: `captures/render_diag/<topic>_<YYYYMMDD>.gdb`
