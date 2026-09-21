# W34B37 targeted OT capture: arm only after the natural first tail.
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
set $armed = 0

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
    continue
  end

break wm_80071984_tail
  commands
    silent
    if $frame == 916
      set $armed = 1
      printf "W34B37_TARGET_ARM frame=%d\n", $frame
    end
    continue
  end

break DrawOTag
  commands
    silent
    if $armed == 1
      set $armed = 2
      printf "W34B37_TARGET_DRAWOTAG frame=%d p=%p psxram=%p delta=0x%lx root_delta=0x%lx\n", $frame, $rdi, &g_PsxRam, (unsigned long)((char*)$rdi - (char*)&g_PsxRam), (unsigned long)((char*)$rdi - (char*)&g_PsxRam - 0xffc)
      x/40wx $rdi-0x20
      x/20gx $rdi-0x20
      x/40wx $rdi-0xffc
      x/20gx $rdi-0xffc
    end
    continue
  end

catch signal SIGSEGV
  commands
    silent
    printf "W34B37_TARGET_SIGSEGV frame=%d pc=%p\n", $frame, $pc
    bt 8
    quit 0
  end

break PcPort_WorldMapPlaceholderMain
  commands
    silent
    printf "W34B37_TARGET_DONE frame=%d\n", $frame
    quit 0
  end

run
