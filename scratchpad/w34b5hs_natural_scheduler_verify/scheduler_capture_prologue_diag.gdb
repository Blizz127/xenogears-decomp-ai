# W34B5HS DIAGNOSTIC COPY (post-scheduler prologue probe). Not the proof script.
# Derived from scheduler_capture.gdb; adds XENO_WORLD_FRAME_PROLOGUE=1 and
# markers for the 0x8007106C -> 0x800712D0 continuation. Exits at placeholder.
# Pure GDB command driver (proven w34b5es/fs pattern).
# Enables scheduler; captures counters and slot state after bounded stop.

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

# ---- Frame counter + controller write (pure GDB commands) ----

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
    if $frame == 1 || $frame == 600 || $frame == 917
      printf "W34B5HS_SCH_FRAME %d\n", $frame
    end
    continue
  end


# ---- DIAG: scheduler dump site (line 7751) -- light print only ----
break world_map_init.c:7852
  commands
    silent
    printf "W34B5HS_DIAG_SCHED_DUMP frame=%d entry=%d executed=%d missing=%d frontier=0x%08x\n", $frame, wm_sched_get_entry(), wm_sched_get_callbacks_executed(), wm_sched_get_missing_hits(), wm_sched_get_frontier_pc()
    continue
  end

# ---- DIAG: post-scheduler continuation entered (DrawSync(0) at 0x8007106C site) ----
break world_map_init.c:7414
  commands
    silent
    printf "W34B5HS_DIAG_PROLOGUE_ENTER frame=%d sched_entry=%d (retail 0x8007106C continuation)\n", $frame, wm_sched_get_entry()
    continue
  end

# ---- DIAG: retail 0x800712D0 driver entered ----
break wm_800712D0_frame_prologue
  commands
    silent
    printf "W34B5HS_DIAG_712D0_ENTER frame=%d fp_entry_before=%d\n", $frame, wm_fp_get_entry()
    continue
  end

# ---- DIAG: hard cut inside the 0x800712D0 driver ----
# Re-anchored after W34B35: capture at the post-0x80075104 diagnostic
# fprintf, after both upload helpers have returned and s_fp_cut_pc is set.
break world_map_frame_driver.c:651
  commands
    silent
    printf "W34B5HS_DIAG_712D0_CUT cut_pc=0x%08x cd_work=%d vsync_retries=%d pad_iters=%d sched_calls=%d\n", wm_fp_get_cut_pc(), wm_fp_get_cd_work_calls(), wm_fp_get_vsync_retries(), wm_fp_get_pad_iters(), wm_fp_get_scheduler_calls()
    continue
  end

# ---- DIAG: prologue instrumentation dump site (line 7785) -- record results ----
break world_map_init.c:7886
  commands
    silent
    printf "W34B5HS_DIAG_PROLOGUE_DUMP frame=%d\n", $frame
    python
import gdb, os
OUT = "/home/blizz/dev/xenogears-decomp/scratchpad/w34b5hs_natural_scheduler_verify"
def as_int(e):
    v = gdb.parse_and_eval(e)
    if v.type.code == gdb.TYPE_CODE_PTR:
        v = v.cast(gdb.lookup_type("uintptr_t"))
    return int(v)
r = {}
for k, e in [("sched_entry","wm_sched_get_entry()"),("sched_callbacks_executed","wm_sched_get_callbacks_executed()"),
             ("sched_missing_hits","wm_sched_get_missing_hits()"),("sched_last_slot","wm_sched_get_last_slot()"),
             ("sched_outcome","wm_sched_get_outcome()"),("fp_entry","wm_fp_get_entry()"),
             ("fp_cd_work_calls","wm_fp_get_cd_work_calls()"),("fp_vsync_retries","wm_fp_get_vsync_retries()"),
             ("fp_pad_iters","wm_fp_get_pad_iters()"),("fp_scheduler_calls","wm_fp_get_scheduler_calls()")]:
    r[k] = as_int(e)
r["sched_frontier_pc"] = "0x%08x" % as_int("wm_sched_get_frontier_pc()")
r["sched_last_callback"] = "0x%08x" % as_int("wm_sched_get_last_callback()")
r["fp_cut_pc"] = "0x%08x" % as_int("wm_fp_get_cut_pc()")
for k in sorted(r): print("W34B5HS_DIAG %s=%s" % (k, r[k]))
with open(os.path.join(OUT, "scheduler_prologue_diag_results.txt"), "w") as f:
    for k in sorted(r): f.write("%s=%s\n" % (k, r[k]))
print("W34B5HS_DIAG_SAVED")
    end
    continue
  end

# ---- DIAG: placeholder idle begins -> record and exit ----
break PcPort_WorldMapPlaceholderMain
  commands
    silent
    printf "W34B5HS_DIAG_PLACEHOLDER_ENTER frame=%d (progress stops here: hollow UI idle)\n", $frame
    bt 6
    quit 0
  end

run
# DIAG: post-mortem on any signal (SIGSEGV observed 2026-08-22 inside g_PsxRam)
printf "W34B5HS_DIAG_STOP pc=0x%lx psxram_base=0x%lx psx_pc=0x%08lx\n", (unsigned long)$pc, (unsigned long)&g_PsxRam, (unsigned long)($pc - (unsigned long)&g_PsxRam + 0x80000000)
bt 12
info registers rip rsp rax rdx rsi rdi
x/8i $pc-16
quit
