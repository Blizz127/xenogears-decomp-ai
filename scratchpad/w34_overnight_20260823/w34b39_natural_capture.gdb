# W34B39 natural capture, re-anchored after the guest OT walk returns.
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

# Line 112 is the first instruction after wm_ot_draw_otag_guest returned and
# before the held D554 backedge decision.
break pc_port/src/world_map_frame_tail_71984.c:112
  commands
    silent
    if $frame == 916
      printf "W34B39_NATURAL_TAIL frame=%d last_ot=0x%08x draw_calls=%d packets=%d steps=%d abort_range=%d abort_align=%d abort_len=%d abort_steps=%d multi=%d last_addr=0x%08x last_tag=0x%08x\n", $frame, wm_71984_tail_get_last_ot(), wm_71984_tail_get_draw_calls(), wm_ot_get_packets_submitted(), wm_ot_get_steps(), wm_ot_get_abort_range(), wm_ot_get_abort_align(), wm_ot_get_abort_len(), wm_ot_get_abort_steps(), wm_ot_get_multi_prim_packets(), wm_ot_get_last_addr(), wm_ot_get_last_tag()
    end
    continue
  end

break world_map_init.c:7861
  commands
    silent
    if $frame == 916
      printf "W34B39_NATURAL_SCHED frame=%d entry=%d executed=%d missing=%d invalid=%d\n", $frame, wm_sched_get_entry(), wm_sched_get_callbacks_executed(), wm_sched_get_missing_hits(), wm_sched_get_invalid_hits()
    end
    continue
  end

break PcPort_WorldMapPlaceholderMain
  commands
    silent
    printf "W34B39_NATURAL_PLACEHOLDER frame=%d rc=0\n", $frame
    quit 0
  end

run
printf "W34B39_NATURAL_STOP frame=%d pc=0x%lx\n", $frame, (unsigned long)$pc
quit
