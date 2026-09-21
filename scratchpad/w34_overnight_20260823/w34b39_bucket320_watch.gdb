# W34B39 read-only diagnosis: identify the first writer of retail OT bucket
# 0x800A2EA8 (bucket 0x320) during scheduler pass 2 at frame 916.
# The watchpoint is armed at wm_80097800 entry, after the frame driver's
# guest-native ClearOTagR and immediately before scheduler callback stores.
# No production code or second-frame/backedge behavior is changed.

set pagination off
set confirm off
set breakpoint pending on
set print thread-events off
set debuginfod enabled off

set environment XENO_FIELD_TEST 1
set environment XENO_KERNEL_SEL 0
set environment XENO_FIELD_MAP 1
set environment XENO_FIELD_ENTRANCE 0
set environment XENO_WORLD_ARCHIVE_SET_INDEX 1
set environment XENO_WORLD_967E4_ROUTE 1
set environment XENO_WORLD_READY_BUFFER_CONSUME 1
set environment XENO_WORLD_MODE_AUDIO_SETUP 1
set environment XENO_WORLD_CONVERGENCE_P1 1
set environment XENO_WORLD_CONVERGENCE_P2 1
set environment XENO_WORLD_FRAMEBUFFER_GTE_INIT 1
set environment XENO_WORLD_TERRAIN_POSITION_INIT 1
set environment XENO_WORLD_COMMON_TAIL_P0 1
set environment XENO_WORLD_COMMON_TAIL_P1 1
set environment XENO_WORLD_COMMON_TAIL_P2 1
set environment XENO_WORLD_COMMON_TAIL_P3 1
set environment XENO_WORLD_COMMON_TAIL_P4 1
set environment XENO_WORLD_COMMON_TAIL_P5 1
set environment XENO_WORLD_SCHEDULER_97800 1
set environment XENO_WORLD_FRAME_PROLOGUE 1
set environment SDL_AUDIODRIVER dummy
set environment LD_LIBRARY_PATH /home/blizz/dev/xenogears-assets/lib

set $frame = 0
set $scheduler_entry = 0

break func_8007554C
  commands
    silent
    set $frame = $frame + 1
    if $frame < 600
      set *(unsigned short*)&D_800AFE9C = 0x2000
    end
    if $frame >= 600 && $frame <= 916
      set *(unsigned short*)&D_800AFE9C = 0x4000
    end
    if $frame > 916
      set *(unsigned short*)&D_800AFE9C = 0
    end
    if $frame == 916
      printf "W34B39_FRAME %d\n", $frame
    end
    continue
  end

break wm_80097800
  commands
    silent
    set $scheduler_entry = $scheduler_entry + 1
    if $frame == 916 && $scheduler_entry == 2
      printf "W34B39_ARM frame=%d scheduler_entry=%d bucket=0x800A2EA8 host_base=0x%lx\n", $frame, $scheduler_entry, (unsigned long)&g_PsxRam
      set $bucket_host = (char *)&g_PsxRam + 0xA2EA8
      watch -location *(unsigned int*)$bucket_host
      commands
        printf "W34B39_FIRST_WRITE frame=%d scheduler_entry=%d pc=0x%lx value=0x%08x\n", $frame, $scheduler_entry, (unsigned long)$pc, *(unsigned int*)$bucket_host
        printf "W34B39_BUCKET_HOST 0x%lx\n", (unsigned long)$bucket_host
        x/4wx $bucket_host-8
        bt 24
        info registers rip rax rbx rcx rdx rsi rdi rbp rsp
        quit 0
      end
    end
    continue
  end

break PcPort_WorldMapPlaceholderMain
  commands
    silent
    printf "W34B39_NO_WRITE frame=%d scheduler_entry=%d placeholder=1\n", $frame, $scheduler_entry
    quit 0
  end

run
printf "W34B39_STOP pc=0x%lx frame=%d scheduler_entry=%d\n", (unsigned long)$pc, $frame, $scheduler_entry
bt 12
quit
