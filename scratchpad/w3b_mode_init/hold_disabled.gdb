# W3B: natural Lahan exit with W2+W3B gates; expect mode-init then placeholder.
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
set environment XENO_WORLD_MODE_INIT 1
unset environment XENO_FIELD_HOLD_TRANSITION
set environment SDL_AUDIODRIVER dummy
set environment LD_LIBRARY_PATH /home/blizz/dev/xenogears-assets/lib

python
import gdb

frame = 0
armed = False
state3 = False
in_placeholder = False
saw_mode_init = False
placeholder_vsyncs = 0
hits = {"72238": 0, "712d0": 0, "7299c": 0, "main_loop": 0}

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

class FrameBreakpoint(gdb.Breakpoint):
    def stop(self):
        global frame
        frame += 1
        if frame in (814, 916):
            print("W3B_FRAME frame=%d tuple=%s D_800ADBE4=%d" %
                  (frame, tuple_values(), read_symbol("D_800ADBE4")))
        if frame > 2500 and not in_placeholder:
            print("W3B_FAIL timeout")
            gdb.execute("quit 2")
        return False

class InputBreakpoint(gdb.Breakpoint):
    def stop(self):
        if frame < 600:
            d = 0x2000
        elif frame <= 916:
            d = 0x4000
        else:
            d = 0
        gdb.execute("set *(unsigned short*)&D_800AFE9C = %d" % d, to_string=True)
        return False

class ArmBreakpoint(gdb.Breakpoint):
    def stop(self):
        global armed
        armed = True
        print("W3B_ARM frame=%d tuple=%s" % (frame, tuple_values()))
        return False

class ChangeStateBreakpoint(gdb.Breakpoint):
    def stop(self):
        global state3
        if as_int("$rdi") == 3:
            state3 = True
            print("W3B_STATE3")
        return False

class ModeInitBreakpoint(gdb.Breakpoint):
    def stop(self):
        global saw_mode_init
        # C function name for native body
        saw_mode_init = True
        print("W3B_MODE_INIT_HIT frame=%d" % frame)
        return False

class PlaceholderBreakpoint(gdb.Breakpoint):
    def stop(self):
        global in_placeholder
        in_placeholder = True
        print("W3B_PLACEHOLDER frame=%d mode_init=%d state3=%d hits=%s" %
              (frame, int(saw_mode_init), int(state3), hits))
        if not state3 or not saw_mode_init:
            print("W3B_PLACEHOLDER_FAIL")
            gdb.execute("quit 2")
        if hits["72238"] or hits["712d0"] or hits["7299c"] or hits["main_loop"]:
            print("W3B_FORBIDDEN_HIT %s" % hits)
            gdb.execute("quit 2")
        return False

class VsyncBreakpoint(gdb.Breakpoint):
    def stop(self):
        global placeholder_vsyncs
        if in_placeholder:
            placeholder_vsyncs += 1
            if placeholder_vsyncs == 120:
                print("W3B_DONE stable=120 mode_init=1 forbidden=%s" % hits)
                gdb.execute("quit")
        return False

FrameBreakpoint("func_8007554C", internal=True)
InputBreakpoint("func_8009F5F4", internal=True)
ArmBreakpoint("PcPort_FieldOpcode56TransitionIntercept", internal=True)
ChangeStateBreakpoint("ChangeGameState", internal=True)
ModeInitBreakpoint("wm_80071CDC_mode_init", internal=True)
PlaceholderBreakpoint("PcPort_WorldMapPlaceholderMain", internal=True)
VsyncBreakpoint("Vsync", internal=True)
end

run
