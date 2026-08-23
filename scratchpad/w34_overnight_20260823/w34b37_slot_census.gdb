# W34B37 read-only census: capture scheduler slot state at the held tail.
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

break wm_80071984_tail
  commands
    silent
    printf "W34B37_CENSUS_ENTRY frame=%d sched_entry=%d\n", $frame, wm_sched_get_entry()
    python
import gdb, struct
inf = gdb.selected_inferior()
base = int(gdb.parse_and_eval("&g_PsxRam"))
def read(addr, size):
    return bytes(inf.read_memory(base + (addr & 0x1fffff), size))
def u16(addr):
    return struct.unpack("<H", read(addr, 2))[0]
def s16(addr):
    return struct.unpack("<h", read(addr, 2))[0]
def u32(addr):
    return struct.unpack("<I", read(addr, 4))[0]
pool = u32(0x8009BE24)
print("W34B37_CENSUS_POOL 0x%08x" % pool)
for i in range(16):
    slot = (pool + i * 0x80) & 0xffffffff
    print("W34B37_SLOT %02d state=%d state_raw=0x%04x timer=0x%04x flag=0x%04x cb0=0x%08x cb1=0x%08x payload=0x%08x" %
          (i, s16(slot), u16(slot), u16(slot + 2), u16(slot + 4),
           u32(slot + 0x18), u32(slot + 0x1c), u32(slot + 0x4c)))
    end
    printf "W34B37_CENSUS_BACKEDGE target=0x8007130C (retail 0x800719C8 bnez)\n"
    continue
  end

break PcPort_WorldMapPlaceholderMain
  commands
    silent
    printf "W34B37_CENSUS_DONE frame=%d rc=0\n", $frame
    quit 0
  end

run
