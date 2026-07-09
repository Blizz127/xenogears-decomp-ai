# Useful Commands

Copy-paste commands from documented handoff and `pc_port/` notes only.

## Build

```bash
# PC port (primary workflow)
distrobox enter xenogears-dev -- bash -lc 'cd /home/blizz/Projects/xenogears-decomp && ./pc_port/build_port.sh'

# Matching decomp (separate toolchain container)
make -B build
```

## Run — field test harness

```bash
# Map0 kernel0 field route (45s)
distrobox enter xenogears-dev -- bash -lc 'cd /home/blizz/Projects/xenogears-decomp && \
  XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 timeout -s KILL 45 ./pc_port/build_native/xeno-port'

# Map1 visible control milestone (real keyboard)
distrobox enter xenogears-dev -- bash -lc 'cd /home/blizz/Projects/xenogears-decomp && \
  SDL_VIDEODRIVER=x11 XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 \
  XENO_FIELD_MAP=1 XENO_FIELD_ENTRANCE=0 XENO_FIELD_0BB_VRAM_UPLOAD=1 \
  ./pc_port/build_native/xeno-port'

# Map1 entrance 9 (exit-route probes)
distrobox enter xenogears-dev -- bash -lc 'cd /home/blizz/Projects/xenogears-decomp && \
  SDL_VIDEODRIVER=x11 XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 \
  XENO_FIELD_MAP=1 XENO_FIELD_ENTRANCE=9 XENO_FIELD_0BB_VRAM_UPLOAD=1 \
  timeout -s KILL 90 ./pc_port/build_native/xeno-port'
```

## Run — with logging

```bash
# Tee full log
distrobox enter xenogears-dev -- bash -lc 'cd /home/blizz/Projects/xenogears-decomp/pc_port && \
  XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 XENO_FIELD_MAP=1 XENO_FIELD_ENTRANCE=9 \
  XENO_FIELD_0BB_VRAM_UPLOAD=1 timeout -s KILL 90 build_native/xeno-port 2>&1 | \
  tee ../captures/render_diag/run_$(date +%Y%m%d_%H%M%S).log; echo RUN_RC=${PIPESTATUS[0]}'

# Opt-in field diagnostics
XENO_FIELD_DIAG=1 ...

# Filter stubs only
... 2>&1 | grep "\[stub\]"
```

## Kernel routing probes

```bash
# Battle route (expect immediate func_8001B6C4 stub)
XENO_FIELD_TEST=1 XENO_KERNEL_SEL=1 timeout -s KILL 30 ./pc_port/build_native/xeno-port

# Menu route (expect func_801C62A8 stub — harness gap)
XENO_KERNEL_SEL=4 timeout -s KILL 30 ./pc_port/build_native/xeno-port
```

## Git (host only)

```bash
cd /home/blizz/Projects/xenogears-decomp
git status
git log --oneline -10
```

## Wiki sync preview

```bash
./scripts/preview_wiki_sync.sh ~/Projects/xenogears-decomp-ai.wiki
```

## Wiki manual sync

```bash
git clone git@github.com:Blizz127/xenogears-decomp-ai.wiki.git

rsync -av \
  ~/Projects/xenogears-decomp/docs/wiki/*.md \
  ~/Projects/xenogears-decomp-ai.wiki/

cd ~/Projects/xenogears-decomp-ai.wiki
git status
git add *.md
git commit -m "Update Xenogears project wiki"
git push
```

## Phase 1 slice (from pc_port/README.md)

```bash
# After matching ELFs exist
gcc -c src/field/game_logic/gold.c -DXENO_PC_PORT -DSKIP_ASM -D_LANGUAGE_C \
    -include assert.h -w \
    -I pc_port/include_shim -I include \
    -I pc_port/extern/PsyCross/include -I pc_port/extern/PsyCross/include/psx \
    -o gold.o
nm -u gold.o | awk '{print $2}' | sort -u > undef.txt
python3 tools/scripts/gen_port_stubs.py \
    --elf build/out/slus_006.64.elf --elf build/out/field.elf \
    --undefined undef.txt --out pc_port/generated/stubs.c
```

## Environment variable quick reference

```bash
export XENO_FIELD_TEST=1
export XENO_KERNEL_SEL=0          # 0=field, 1=battle, 4=menu
export XENO_FIELD_MAP=1
export XENO_FIELD_ENTRANCE=9      # 0, 8, 9 documented; NOT 6 or 10
export XENO_FIELD_0BB_VRAM_UPLOAD=1
export XENO_FIELD_DIAG=1          # optional verbose field prints
export SDL_VIDEODRIVER=x11        # real keyboard input
```

## Container notes

```bash
distrobox enter xenogears-dev     # toolchain + SDL/OpenAL
# git NOT on PATH in container — use host for git commands
# grep -E instead of rg in container
```
