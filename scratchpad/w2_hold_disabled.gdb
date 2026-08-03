# W2 closeout: natural Lahan exit with world init enabled, hold disabled.
# Required: arm near f916 → teardown → state 3 → overlay 0x0F → native init
#            cut 0x80071000 → placeholder → 120 Vsyncs.
set pagination off
set confirm off
set debuginfod enabled off
set breakpoint pending on
set print thread-events off
set environment XENO_FIELD_TEST 1
set environment XENO_KERNEL_SEL 0
set environment XENO_FIELD_MAP 1
set environment XENO_FIELD_ENTRANCE 0
set environment XENO_WORLD_INIT 1
unset environment XENO_FIELD_HOLD_TRANSITION
set environment SDL_AUDIODRIVER dummy
set environment LD_LIBRARY_PATH /home/blizz/dev/xenogears-assets/lib

python
import gdb
import struct

frame = 0
armed = False
state3_selected = False
in_init = False
in_placeholder = False
placeholder_vsyncs = 0
saw_overlay = False
saw_cut = False
teardown_started = False
teardown_seen = []
teardown_expected = [
    "func_800798BC",
    "func_800A91F0",
    "func_800A31E8",
    "FieldParticlesFreeAll",
    "func_800864F0",
    "func_8007FFE8",
    "DrawSync",
    "Vsync",
    "FieldFree",
    "FieldPartyFreeSkinDataBuffers",
    "func_80085988",
    "HeapFree(D_800ADB30)",
]

def inferior():
    return gdb.selected_inferior()

def as_int(expr):
    value = gdb.parse_and_eval(expr)
    if value.type.code == gdb.TYPE_CODE_PTR:
        value = value.cast(gdb.lookup_type("uintptr_t"))
    return int(value)

def read_bytes(addr, count):
    return bytes(inferior().read_memory(addr, count))

def symbol_addr(name):
    return as_int("&" + name)

def read_symbol(name, size=4, signed=True):
    return int.from_bytes(read_bytes(symbol_addr(name), size), "little", signed=signed)

def pointer_slot(name):
    return int.from_bytes(read_bytes(symbol_addr(name), 8), "little")

def tuple_values():
    game = pointer_slot("g_pGameState")
    return tuple(int.from_bytes(read_bytes(game + off, 2), "little")
                 for off in (0x231a, 0x231c, 0x231e, 0x2320))

def called_from_field_main():
    fr = gdb.newest_frame()
    caller = fr.older() if fr is not None else None
    return caller is not None and caller.name() == "FieldMain"

def record_teardown(name):
    global teardown_started
    if not armed or in_placeholder or in_init:
        return
    if not teardown_started:
        if name != teardown_expected[0]:
            return
        teardown_started = True
    index = len(teardown_seen)
    if index >= len(teardown_expected) or teardown_expected[index] != name:
        print("W2_ORDER_FAIL expected=%s got=%s seen=%s" %
              (teardown_expected[index] if index < len(teardown_expected) else "<end>",
               name, teardown_seen))
        gdb.execute("quit 2")
        return
    teardown_seen.append(name)
    print("W2_TEARDOWN n=%d op=%s" % (len(teardown_seen), name))

class FrameBreakpoint(gdb.Breakpoint):
    def stop(self):
        global frame
        frame += 1
        if frame in (814, 900, 916, 1020):
            try:
                print("W2_FRAME frame=%d D_800ADBE4=%d control=%d tuple=%s" %
                      (frame, read_symbol("D_800ADBE4"),
                       read_symbol("g_FieldControl", 2, True), tuple_values()))
            except Exception as e:
                print("W2_FRAME frame=%d (read err %s)" % (frame, e))
        if frame == 814:
            sel, hdg, a2, ent = tuple_values()
            if (sel, hdg, a2, ent) == (0x400, 0x0e00, 1, 1) and armed:
                print("W2_FAIL transition already armed at frame 814")
                gdb.execute("quit 2")
        if frame > 2000 and not in_placeholder:
            print("W2_FAIL timeout frame=%d armed=%d state3=%d" %
                  (frame, armed, state3_selected))
            gdb.execute("quit 2")
        return False

class InputBreakpoint(gdb.Breakpoint):
    def stop(self):
        # Approach then hold 0x4000 through 916, then neutral.
        if frame < 600:
            direction = 0x2000
        elif frame <= 916:
            direction = 0x4000
        else:
            direction = 0
        gdb.execute("set *(unsigned short*)&D_800AFE9C = %d" % direction,
                    to_string=True)
        return False

class ArmBreakpoint(gdb.Breakpoint):
    def stop(self):
        global armed
        armed = True
        selector, heading, transition_arg2, entrance = tuple_values()
        print("W2_ARM frame=%d tuple=(0x%04x,0x%04x,0x%04x,0x%04x) "
              "B02C8=%d D_800ADBE4=%d control=%d" %
              (frame, selector, heading, transition_arg2, entrance,
               read_symbol("D_800B02C8", 1, False),
               read_symbol("D_800ADBE4"),
               read_symbol("g_FieldControl", 2, True)))
        return False

class TeardownBreakpoint(gdb.Breakpoint):
    def __init__(self, symbol):
        super().__init__("*" + symbol, internal=True)
        self.label = symbol

    def stop(self):
        if called_from_field_main():
            record_teardown(self.label)
        return False

class HeapFreeBreakpoint(gdb.Breakpoint):
    def stop(self):
        if (called_from_field_main() and armed and not in_placeholder and
                not in_init and
                as_int("$rdi") == pointer_slot("D_800ADB30")):
            record_teardown("HeapFree(D_800ADB30)")
        return False

class DispatchBreakpoint(gdb.Breakpoint):
    def stop(self):
        exit_code = as_int("$rdi")
        print("W2_DISPATCH exit_code=%d teardown=%s tuple=%s" %
              (exit_code, teardown_seen, tuple_values()))
        if exit_code != 1 or teardown_seen != teardown_expected:
            print("W2_DISPATCH_FAIL")
            gdb.execute("quit 2")
        return False

class ChangeStateBreakpoint(gdb.Breakpoint):
    def stop(self):
        global state3_selected
        state = as_int("$rdi")
        if armed and state == 3:
            state3_selected = True
            print("W2_STATE_SELECT state=3 tuple=%s" % (tuple_values(),))
        return False

class OverlayBreakpoint(gdb.Breakpoint):
    def stop(self):
        global saw_overlay
        idx = as_int("$rdi")
        if state3_selected and idx == 3:
            saw_overlay = True
            print("W2_OVERLAY LoadGameStateOverlay(3) archive_path_ok")
        elif state3_selected and idx != 3:
            print("W2_OVERLAY_FAIL unexpected index=%d" % idx)
            gdb.execute("quit 2")
        return False

class InitBreakpoint(gdb.Breakpoint):
    def stop(self):
        global in_init
        in_init = True
        print("W2_INIT_ENTRY frame=%d" % frame)
        return False

class PlaceholderBreakpoint(gdb.Breakpoint):
    def stop(self):
        global in_placeholder
        in_placeholder = True
        selector, heading, transition_arg2, raw_entrance = tuple_values()
        world_index = (selector & 0x3fff) - 0x400
        entrance = raw_entrance & 0x7fff
        ram = symbol_addr("g_PsxRam")
        mem = as_int("g_MainGameStates[3].pMemStart") - ram
        heap = as_int("g_MainGameStates[3].pHeapStart") - ram
        overlay = as_int("g_MainGameStates[3].hasOverlay")
        # Entry fingerprint
        try:
            fp = int.from_bytes(read_bytes(ram + 0x70cfc, 4), "little")
        except Exception:
            fp = 0
        # World state after init
        try:
            wi = int.from_bytes(read_bytes(ram + 0x9bd0c, 4), "little", signed=True)
            we = int.from_bytes(read_bytes(ram + 0x9c5a8, 4), "little")
            wh = int.from_bytes(read_bytes(ram + 0x9c584, 4), "little")
            wa = int.from_bytes(read_bytes(ram + 0x9d3d4, 4), "little")
        except Exception as e:
            wi = we = wh = wa = -1
            print("W2_STATE_READ_ERR %s" % e)
        print("W2_PLACEHOLDER_ENTRY frame=%d selector=0x%04x world_index=%d "
              "entrance=%d transition_arg2=%d heading=0x%04x "
              "mem=0x%x heap=0x%x hasOverlay=%d fingerprint=0x%08x "
              "wm_state=(idx=%d ent=%d hdg=0x%x arg2=%d) saw_overlay=%d" %
              (frame, selector, world_index, entrance, transition_arg2,
               heading, mem, heap, overlay, fp, wi, we, wh, wa, int(saw_overlay)))
        if (not state3_selected or not in_init or not saw_overlay or
                (selector, heading, transition_arg2, raw_entrance) !=
                (0x400, 0x0e00, 1, 1) or world_index != 0 or entrance != 1 or
                mem != 0x9bbb0 or heap != 0x9d80c or overlay != 1 or
                fp != 0x27bdffd8 or wi != 0 or we != 1 or wh != 0x0e00 or wa != 1):
            print("W2_PLACEHOLDER_FAIL")
            gdb.execute("quit 2")
        return False

class VsyncBreakpoint(gdb.Breakpoint):
    def stop(self):
        global placeholder_vsyncs
        if in_placeholder:
            placeholder_vsyncs += 1
            if placeholder_vsyncs == 120:
                print("W2_DONE placeholder_vsyncs=120 stable=1 "
                      "teardown=%s arm_frame_ok=%d" %
                      (teardown_seen, int(armed)))
                gdb.execute("quit")
        elif armed and called_from_field_main() and not in_init:
            record_teardown("Vsync")
        return False

class StubBreakpoint(gdb.Breakpoint):
    def stop(self):
        name = inferior().read_memory(as_int("$rdi"), 96).tobytes().split(b"\0", 1)[0].decode()
        if name in ("func_8007954C", "func_800798BC", "func_8001BB50", "wm_800712D0"):
            print("W2_STUB_FAIL name=%s" % name)
            gdb.execute("quit 2")
        return False

FrameBreakpoint("func_8007554C", internal=True)
InputBreakpoint("func_8009F5F4", internal=True)
ArmBreakpoint("PcPort_FieldOpcode56TransitionIntercept", internal=True)
for name in teardown_expected:
    if name not in ("Vsync", "HeapFree(D_800ADB30)"):
        TeardownBreakpoint(name)
VsyncBreakpoint("Vsync", internal=True)
HeapFreeBreakpoint("HeapFree", internal=True)
DispatchBreakpoint("func_8007954C", internal=True)
ChangeStateBreakpoint("ChangeGameState", internal=True)
OverlayBreakpoint("LoadGameStateOverlay", internal=True)
InitBreakpoint("PcPort_WorldMapInitMain", internal=True)
PlaceholderBreakpoint("PcPort_WorldMapPlaceholderMain", internal=True)
StubBreakpoint("xeno_port_stub", internal=True)
end

catch signal SIGSEGV
commands
  printf "W2_SEGFAULT\n"
  bt 16
  quit 2
end

run
