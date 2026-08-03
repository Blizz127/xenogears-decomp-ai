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
set environment XENO_FIELD_HOLD_TRANSITION 1
set environment SDL_AUDIODRIVER dummy
set environment LD_LIBRARY_PATH /home/blizz/dev/xenogears-assets/lib
python
import gdb
frame=0; armed=False
def as_int(e):
    v=gdb.parse_and_eval(e)
    if v.type.code==gdb.TYPE_CODE_PTR: v=v.cast(gdb.lookup_type('uintptr_t'))
    return int(v)
def rb(a,n): return bytes(gdb.selected_inferior().read_memory(a,n))
def sa(n): return as_int('&'+n)
def rs(n,s=4,signed=True): return int.from_bytes(rb(sa(n),s),'little',signed=signed)
def ps(n): return int.from_bytes(rb(sa(n),8),'little')
def tup():
    g=ps('g_pGameState')
    return tuple(int.from_bytes(rb(g+o,2),'little') for o in (0x231a,0x231c,0x231e,0x2320))
class F(gdb.Breakpoint):
    def stop(self):
        global frame
        frame+=1
        if frame>=1040:
            print('W3BH_DONE armed=%d frame=%d' % (armed, frame))
            gdb.execute('quit')
        return False
class I(gdb.Breakpoint):
    def stop(self):
        d=0x2000 if frame<600 else (0x4000 if frame<=916 else 0)
        gdb.execute('set *(unsigned short*)&D_800AFE9C = %d'%d,to_string=True)
        return False
class A(gdb.Breakpoint):
    def stop(self):
        global armed
        armed=True
        print('W3BH_ARM frame=%d tuple=%s'%(frame,tup()))
        return False
class C(gdb.Breakpoint):
    def stop(self):
        if as_int('$rdi')==3:
            print('W3BH_FAIL state3'); gdb.execute('quit 2')
        return False
class M(gdb.Breakpoint):
    def stop(self):
        print('W3BH_FAIL mode_init'); gdb.execute('quit 2')
        return False
class P(gdb.Breakpoint):
    def stop(self):
        print('W3BH_FAIL placeholder'); gdb.execute('quit 2')
        return False
F('func_8007554C',internal=True)
I('func_8009F5F4',internal=True)
A('PcPort_FieldOpcode56TransitionIntercept',internal=True)
C('ChangeGameState',internal=True)
M('wm_80071CDC_mode_init',internal=True)
P('PcPort_WorldMapPlaceholderMain',internal=True)
end
run
