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
set $w34b53_dumped = 0
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
  printf "W34B50_A72_ENTRY frame=%d slot=%d resync=%u d554=0x%08x flags=0x%04x sel=%d byte=%u obj=0x%08x\n", $frame, slot_idx, *(unsigned char*)((unsigned char*)g_PsxRam + 0x6f8e5), *(unsigned int*)((unsigned char*)g_PsxRam + 0x9d554), *(unsigned short*)((unsigned char*)g_PsxRam + 0x9bd10), *(short*)((unsigned char*)g_PsxRam + 0x9bd24), *(unsigned char*)((unsigned char*)g_PsxRam + 0x9d738), *(unsigned int*)((unsigned char*)g_PsxRam + 0x9d7d8)
  continue
end

break wm_8008C844
commands
  silent
  printf "W34B50_C844_ENTRY frame=%d slot=%d resync=%u d554=0x%08x flags=0x%04x area=%d boundary=%u\n", $frame, slot_idx, *(unsigned char*)((unsigned char*)g_PsxRam + 0x6f8e5), *(unsigned int*)((unsigned char*)g_PsxRam + 0x9d554), *(unsigned short*)((unsigned char*)g_PsxRam + 0x9bd10), *(short*)((unsigned char*)g_PsxRam + 0x9bd24), *(unsigned char*)((unsigned char*)g_PsxRam + 0x9d738)
  continue
end

break wm_80094238
commands
  silent
  printf "W34B50_94238_ENTRY frame=%d pos=0x%08x idx=%u table=0x%08x\n", $frame, pos_vec, list_index, *(unsigned int*)((unsigned char*)g_PsxRam + 0x9bd00)
  if $frame == 916 && $w34b53_dumped == 0
    set $w34b53_dumped = 1
    printf "W34B53_94238_POS x=%d y=%d z=%d list0_r0=(x=%d z=%d w=%d h=%d sentinel=%d id=%u type=%d) list0_r1=(x=%d z=%d w=%d h=%d sentinel=%d id=%u type=%d)\n", *(int*)((unsigned char*)g_PsxRam + 0xd75e0), *(int*)((unsigned char*)g_PsxRam + 0xd75e4), *(int*)((unsigned char*)g_PsxRam + 0xd75e8), *(short*)((unsigned char*)g_PsxRam + 0xb670c), *(short*)((unsigned char*)g_PsxRam + 0xb670e), *(short*)((unsigned char*)g_PsxRam + 0xb6710), *(short*)((unsigned char*)g_PsxRam + 0xb6712), *(short*)((unsigned char*)g_PsxRam + 0xb6714), *(unsigned short*)((unsigned char*)g_PsxRam + 0xb6718), *(short*)((unsigned char*)g_PsxRam + 0xb671a), *(short*)((unsigned char*)g_PsxRam + 0xb671c), *(short*)((unsigned char*)g_PsxRam + 0xb671e), *(short*)((unsigned char*)g_PsxRam + 0xb6720), *(short*)((unsigned char*)g_PsxRam + 0xb6722), *(short*)((unsigned char*)g_PsxRam + 0xb6724), *(unsigned short*)((unsigned char*)g_PsxRam + 0xb6728), *(short*)((unsigned char*)g_PsxRam + 0xb672a)
    printf "W34B53_94238_TABLE guest=0x%08x entries:", *(unsigned int*)((unsigned char*)g_PsxRam + 0x9bd00)
    x/16wx (unsigned char*)g_PsxRam + 0xb66fc
    printf "W34B53_94238_DATA words_at_table_plus_0x40:\n"
    x/16wx (unsigned char*)g_PsxRam + 0xb673c
  end
  continue
end

break pc_port/src/world_map_helper_94238.c:143
commands
  silent
  printf "W34B50_94238_HIT frame=%d rec=0x%08x idA=%d idB=%d\n", $frame, *(unsigned int*)((unsigned char*)g_PsxRam + 0x9d7d8), *(short*)((unsigned char*)g_PsxRam + 0x9bd24), *(short*)((unsigned char*)g_PsxRam + 0x9ce68)
  continue
end

break pc_port/src/world_map_helper_94238.c:154
commands
  silent
  printf "W34B50_94238_MISS frame=%d rec=0x%08x idA=%d idB=%d\n", $frame, *(unsigned int*)((unsigned char*)g_PsxRam + 0x9d7d8), *(short*)((unsigned char*)g_PsxRam + 0x9bd24), *(short*)((unsigned char*)g_PsxRam + 0x9ce68)
  continue
end

# Capture the helper return values directly; source-line breakpoints in the
# callback switch can be coalesced by the host compiler's line table.
break pc_port/src/world_map_helper_90a84.c:373
commands
  silent
  printf "W34B50_A72_HELPER_RETURN result=%d d554=0x%08x flags=0x%04x sel=%d byte=%u\n", result, *(unsigned int*)((unsigned char*)g_PsxRam + 0x9d554), *(unsigned short*)((unsigned char*)g_PsxRam + 0x9bd10), *(short*)((unsigned char*)g_PsxRam + 0x9bd24), *(unsigned char*)((unsigned char*)g_PsxRam + 0x9d738)
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
