set pagination off
set confirm off
set debuginfod enabled off
set breakpoint pending on
set print thread-events off
set environment XENO_FIELD_TEST 1
set environment XENO_KERNEL_SEL 0
set environment XENO_FIELD_MAP 1
set environment XENO_FIELD_ENTRANCE 0
unset environment XENO_FIELD_HOLD_TRANSITION
set environment SDL_AUDIODRIVER dummy

python
import gdb

frame = 0
armed = False
state3_selected = False
in_placeholder = False
placeholder_vsyncs = 0
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
    frame = gdb.newest_frame()
    caller = frame.older() if frame is not None else None
    return caller is not None and caller.name() == "FieldMain"

def record_teardown(name):
    global teardown_started
    if not armed or in_placeholder:
        return
    if not teardown_started:
        if name != teardown_expected[0]:
            return
        teardown_started = True
    index = len(teardown_seen)
    if index >= len(teardown_expected) or teardown_expected[index] != name:
        print("F16_ORDER_FAIL expected=%s got=%s seen=%s" %
              (teardown_expected[index] if index < len(teardown_expected) else "<end>",
               name, teardown_seen))
        gdb.execute("quit 2")
        return
    teardown_seen.append(name)
    print("F16_TEARDOWN n=%d op=%s" % (len(teardown_seen), name))

class FrameBreakpoint(gdb.Breakpoint):
    def stop(self):
        global frame
        frame += 1
        return False

class InputBreakpoint(gdb.Breakpoint):
    def stop(self):
        if frame < 600:
            direction = 0x2000
        elif frame < 900:
            direction = 0x4000
        elif frame < 1020:
            direction = 0x8000
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
        print("F16_ARM frame=%d tuple=(0x%04x,0x%04x,0x%04x,0x%04x) "
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
                as_int("$rdi") == pointer_slot("D_800ADB30")):
            record_teardown("HeapFree(D_800ADB30)")
        return False

class DispatchBreakpoint(gdb.Breakpoint):
    def stop(self):
        exit_code = as_int("$rdi")
        print("F16_DISPATCH exit_code=%d teardown=%s tuple=%s" %
              (exit_code, teardown_seen, tuple_values()))
        if exit_code != 1 or teardown_seen != teardown_expected:
            print("F16_DISPATCH_FAIL")
            gdb.execute("quit 2")
        return False

class ChangeStateBreakpoint(gdb.Breakpoint):
    def stop(self):
        global state3_selected
        state = as_int("$rdi")
        if armed and state == 3:
            state3_selected = True
            print("F16_STATE_SELECT state=3 tuple=%s" % (tuple_values(),))
        return False

class OverlayBreakpoint(gdb.Breakpoint):
    def stop(self):
        if state3_selected:
            print("F16_OVERLAY_FAIL state3 attempted LoadGameStateOverlay")
            gdb.execute("quit 2")
        return False

class StubBreakpoint(gdb.Breakpoint):
    def stop(self):
        name = inferior().read_memory(as_int("$rdi"), 96).tobytes().split(b"\0", 1)[0].decode()
        if name in ("func_8007954C", "func_800798BC", "func_8001BB50"):
            print("F16_STUB_FAIL name=%s" % name)
            gdb.execute("quit 2")
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
        print("F16_PLACEHOLDER_ENTRY frame=%d selector=0x%04x world_index=%d "
              "entrance=%d transition_arg2=%d heading=0x%04x "
              "mem=0x%x heap=0x%x hasOverlay=%d" %
              (frame, selector, world_index, entrance, transition_arg2,
               heading, mem, heap, overlay))
        if (not state3_selected or (selector, heading, transition_arg2, raw_entrance) !=
                (0x400, 0x0e00, 1, 1) or world_index != 0 or entrance != 1 or
                mem != 0x9bbb0 or heap != 0x9d80c or overlay != 0):
            print("F16_PLACEHOLDER_FAIL")
            gdb.execute("quit 2")
        return False

class VsyncBreakpoint(gdb.Breakpoint):
    def stop(self):
        global placeholder_vsyncs
        if in_placeholder:
            placeholder_vsyncs += 1
            if placeholder_vsyncs == 120:
                print("F16_DONE placeholder_vsyncs=120 stable=1 teardown=%s tuple=%s" %
                      (teardown_seen, tuple_values()))
                gdb.execute("quit")
        elif armed and called_from_field_main():
            record_teardown("Vsync")
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
StubBreakpoint("xeno_port_stub", internal=True)
PlaceholderBreakpoint("PcPort_WorldMapPlaceholderMain", internal=True)
end

catch signal SIGABRT
commands
  printf "F16_ABORT\n"
  bt 12
  quit 2
end

catch signal SIGSEGV
commands
  printf "F16_SEGFAULT\n"
  bt 12
  quit 2
end

run
