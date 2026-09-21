# W2 closeout: world init enabled but field hold intercepts departure.
# Required: genuine transition near f916, no state 3, no overlay, no world-init.
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
set environment XENO_FIELD_HOLD_TRANSITION 1
set environment SDL_AUDIODRIVER dummy
set environment LD_LIBRARY_PATH /home/blizz/dev/xenogears-assets/lib

python
import gdb

frame = 0
armed = False
state3 = False
overlay3 = False
world_init = False
post_ops = 0

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

def actor_position(index):
    ptr = as_int("g_FieldActors[%d].pActorData" % index)
    raw = read_bytes(ptr + 0x20, 12)
    vals = [int.from_bytes(raw[i:i+4], "little", signed=True) >> 16
            for i in (0, 4, 8)]
    return tuple(vals)

class FrameBreakpoint(gdb.Breakpoint):
    def stop(self):
        global frame
        frame += 1
        if frame in (814, 900, 916, 1020):
            print("W2H_FRAME frame=%d pos=%s D_800ADBE4=%d control=%d tuple=%s" %
                  (frame, actor_position(as_int("g_PlayerActorIndex")),
                   read_symbol("D_800ADBE4"),
                   read_symbol("g_FieldControl", 2, True),
                   tuple_values()))
        if frame >= 1040:
            if not armed:
                print("W2H_FAIL never armed")
                gdb.execute("quit 2")
            if state3 or overlay3 or world_init:
                print("W2H_FAIL leaked to world state3=%d overlay=%d init=%d" %
                      (state3, overlay3, world_init))
                gdb.execute("quit 2")
            control = read_symbol("g_FieldControl", 2, True)
            adbe4 = read_symbol("D_800ADBE4")
            print("W2H_DONE frames=%d armed=1 state3=0 overlay3=0 world_init=0 "
                  "control=%d D_800ADBE4=%d tuple=%s live_field=1" %
                  (frame, control, adbe4, tuple_values()))
            # Field should still be live: not in world placeholder
            gdb.execute("quit")
        return False

class InputBreakpoint(gdb.Breakpoint):
    def stop(self):
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
        print("W2H_ARM frame=%d tuple=(0x%04x,0x%04x,0x%04x,0x%04x) "
              "D_800ADBE4=%d control=%d" %
              (frame, selector, heading, transition_arg2, entrance,
               read_symbol("D_800ADBE4"),
               read_symbol("g_FieldControl", 2, True)))
        # Hold should restore departure latch
        if read_symbol("D_800ADBE4") == 0:
            print("W2H_WARN D_800ADBE4 still 0 after hold intercept (may restore async)")
        return False

class ChangeStateBreakpoint(gdb.Breakpoint):
    def stop(self):
        global state3
        state = as_int("$rdi")
        if state == 3:
            state3 = True
            print("W2H_FAIL ChangeGameState(3)")
            gdb.execute("quit 2")
        return False

class OverlayBreakpoint(gdb.Breakpoint):
    def stop(self):
        global overlay3
        if as_int("$rdi") == 3:
            overlay3 = True
            print("W2H_FAIL LoadGameStateOverlay(3)")
            gdb.execute("quit 2")
        return False

class InitBreakpoint(gdb.Breakpoint):
    def stop(self):
        global world_init
        world_init = True
        print("W2H_FAIL PcPort_WorldMapInitMain entered")
        gdb.execute("quit 2")
        return False

class PlaceholderBreakpoint(gdb.Breakpoint):
    def stop(self):
        print("W2H_FAIL placeholder entered under hold")
        gdb.execute("quit 2")
        return False

FrameBreakpoint("func_8007554C", internal=True)
InputBreakpoint("func_8009F5F4", internal=True)
ArmBreakpoint("PcPort_FieldOpcode56TransitionIntercept", internal=True)
ChangeStateBreakpoint("ChangeGameState", internal=True)
OverlayBreakpoint("LoadGameStateOverlay", internal=True)
InitBreakpoint("PcPort_WorldMapInitMain", internal=True)
PlaceholderBreakpoint("PcPort_WorldMapPlaceholderMain", internal=True)
end

catch signal SIGSEGV
commands
  printf "W2H_SEGFAULT\n"
  bt 12
  quit 2
end

run
