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
set environment XENO_WORLD_FRAME_REENTRY_TWICE 1
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

# Entry observations establish whether the clear-writer precondition reaches
# the wm_80090A84/wm_80090C68 branch at all.
break wm_8008A72C
commands
  silent
  printf "W34B50_A72_ENTRY frame=%d slot=%d resync=%u d554=0x%08x\n", $frame, slot_idx, *(unsigned char*)((unsigned char*)g_PsxRam + 0x6f8e5), *(unsigned int*)((unsigned char*)g_PsxRam + 0x9d554)
  continue
end

break wm_8008C844
commands
  silent
  printf "W34B50_C844_ENTRY frame=%d slot=%d resync=%u d554=0x%08x\n", $frame, slot_idx, *(unsigned char*)((unsigned char*)g_PsxRam + 0x6f8e5), *(unsigned int*)((unsigned char*)g_PsxRam + 0x9d554)
  continue
end

# Capture the helper return values directly; source-line breakpoints in the
# callback switch can be coalesced by the host compiler's line table.
break pc_port/src/world_map_helper_90a84.c:373
commands
  silent
  printf "W34B50_A72_HELPER_RETURN result=%d d554=0x%08x\n", result, *(unsigned int*)((unsigned char*)g_PsxRam + 0x9d554)
  continue
end

break pc_port/src/world_map_helper_90c68.c:100
commands
  silent
  printf "W34B50_C844_HELPER_RETURN result=3 d554=0x%08x\n", *(unsigned int*)((unsigned char*)g_PsxRam + 0x9d554)
  continue
end

break pc_port/src/world_map_helper_90c68.c:104
commands
  silent
  printf "W34B50_C844_HELPER_RETURN result=1 d554=0x%08x\n", *(unsigned int*)((unsigned char*)g_PsxRam + 0x9d554)
  continue
end

break pc_port/src/world_map_helper_90c68.c:122
commands
  silent
  printf "W34B50_C844_HELPER_RETURN result=0 d554=0x%08x\n", *(unsigned int*)((unsigned char*)g_PsxRam + 0x9d554)
  continue
end

# 0x8008A72C: class 1 is the only branch that clears D554.
break pc_port/src/world_map_callback_8a72c.c:351
commands
  silent
  printf "W34B50_A72_BRANCH frame=%d slot=%d cls=%d jt2=%u d554=0x%08x resync=%u\n", $frame, slot_idx, cls, jt2, *(unsigned int*)((unsigned char*)g_PsxRam + 0x9d554), *(unsigned char*)((unsigned char*)g_PsxRam + 0x6f8e5)
  continue
end

# 0x8008C844: ret 1 is the only branch that clears D554.
break pc_port/src/world_map_callback_8c844.c:150
commands
  silent
  printf "W34B50_C844_BRANCH frame=%d slot=%d ret=%d d554=0x%08x resync=%u\n", $frame, slot_idx, ret, *(unsigned int*)((unsigned char*)g_PsxRam + 0x9d554), *(unsigned char*)((unsigned char*)g_PsxRam + 0x6f8e5)
  continue
end

break wm_80071984_tail
commands
  silent
  if $frame == 916
    printf "W34B50_THIRD_TAIL frame=%d d554=0x%08x sched_entry=%d\n", $frame, *(unsigned int*)((unsigned char*)g_PsxRam + 0x9d554), wm_sched_get_entry()
  end
  continue
end

break PcPort_WorldMapPlaceholderMain
commands
  silent
  printf "W34B50_DONE frame=%d rc=0\n", $frame
  quit 0
end

run
