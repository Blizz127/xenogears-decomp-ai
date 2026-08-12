#!/usr/bin/env python3
"""Control-flow-following symbolic interpreter for 0x80073B04.

Correctly models MIPS branch-delay slots and unrolls the single backward
loop, so call arguments and packet/vertex addresses are derived, not guessed.
"""
import struct, os, csv
from capstone import *
from capstone.mips import *

ROM  = '/home/blizz/dev/xenogears-decomp/disc/world_map.bin'
BASE = 0x8006FAF0
DATA = open(ROM, 'rb').read()
OUT  = os.path.dirname(os.path.abspath(__file__))
ENTRY, END_VA = 0x80073B04, 0x80073E30

def off(va): return va - BASE
md = Cs(CS_ARCH_MIPS, CS_MODE_MIPS32 + CS_MODE_LITTLE_ENDIAN)
md.detail = True
insns = list(md.disasm(DATA[off(ENTRY):off(END_VA)], ENTRY))
I = {i.address: i for i in insns}

LIBGTE = {0x8004A92C: 'RotMatrixYXZ', 0x8004931C: 'CompMatrix',
          0x80049EFC: 'SetRotMatrix', 0x80049F8C: 'SetTransMatrix',
          0x8004A73C: 'RotTransPers4'}

# concrete known globals so loop-controlling values resolve
KNOWN = {0x8009D7F0: None}   # dbidx: parameterised below

def interp(dbidx):
    """Run with a concrete double-buffer index; returns event log."""
    reg  = {'zero': ('C', 0), 'sp': ('S', 'sp')}
    stk  = {}
    ev   = []
    unknown = {}

    def U(tag):
        unknown.setdefault(tag, 'SYM(%s)' % tag)
        return ('S', tag)

    def g(r):
        if r == 'zero': return ('C', 0)
        v = reg.get(r)
        if v is None: return U('IN_' + r)
        return v

    def isC(v): return v[0] == 'C'

    def loadmem(addr_v, disp, mn, va):
        if isC(addr_v):
            a = (addr_v[1] + disp) & 0xFFFFFFFF
            if a == 0x8009D7F0:
                ev.append((va, 'load', mn, a, 'dbidx'))
                return ('C', dbidx)
            if a == 0x8009BD3A:
                ev.append((va, 'load', mn, a, 'theta'))
                return U('theta')
            if a == 0x80050100:
                ev.append((va, 'load', mn, a, 'ot_shift'))
                return U('ot_shift')
            if a == 0x8009BE3C:
                ev.append((va, 'load', mn, a, 'db_ptr'))
                return U('db_ptr')
            ev.append((va, 'load', mn, a, ''))
            return U('mem@%08X' % a)
        if addr_v[1].startswith('sp'):
            ev.append((va, 'load', mn, None, 'stack%+d' % disp))
            return stk.get(disp, U('stackload%+d' % disp))
        ev.append((va, 'load', mn, None, '%s%+d' % (addr_v[1], disp)))
        return U('ld@%08X' % va)

    pc = ENTRY
    steps = 0
    while pc < END_VA and steps < 5000:
        steps += 1
        i = I[pc]
        mn, ops = i.mnemonic, i.operands
        rn = lambda o: i.reg_name(o.reg)
        nxt = pc + 4

        # ---------------- branches / calls: execute delay slot FIRST
        if mn in ('bnez', 'beqz', 'bltz', 'bgez', 'blez', 'bgtz', 'jal', 'jr', 'j'):
            ds = I.get(pc + 4)
            take = None
            target = None
            if mn == 'jal':
                target = ops[0].imm
            elif mn == 'jr':
                target = None
            elif mn == 'j':
                target = ops[0].imm
            else:
                cond = g(rn(ops[0]))
                target = ops[-1].imm
                if isC(cond):
                    c = cond[1]
                    sc = c - (1 << 32) if c >> 31 else c
                    take = {'bnez': c != 0, 'beqz': c == 0, 'bltz': sc < 0,
                            'bgez': sc >= 0, 'blez': sc <= 0, 'bgtz': sc > 0}[mn]
                else:
                    take = ('UNKNOWN', mn)

            if ds is not None:
                exec_one(ds, reg, stk, ev, g, U, isC, loadmem, i.reg_name)

            if mn == 'jal':
                args = {a: g(a) for a in ('a0', 'a1', 'a2', 'a3')}
                sargs = {d: stk.get(d) for d in (0x10, 0x14, 0x18, 0x1c, 0x20, 0x24)}
                ev.append((pc, 'CALL', LIBGTE.get(target, 'sub_%08X' % target),
                           target, (args, sargs)))
                for r in ('v0','v1','a0','a1','a2','a3','t0','t1','t2','t3','t4',
                          't5','t6','t7','t8','t9','ra'):
                    reg[r] = U('clob@%08X_%s' % (pc, r))
                reg['v0'] = U('ret@%08X' % pc)
                pc += 8
                continue
            if mn == 'jr':
                ev.append((pc, 'RETURN', i.op_str, None, ''))
                break
            if take is True:
                pc = target; continue
            if take is False:
                pc += 8; continue
            ev.append((pc, 'BRANCH-UNRESOLVED', mn, target, ''))
            pc += 8
            continue

        exec_one(i, reg, stk, ev, g, U, isC, loadmem, i.reg_name)
        pc = nxt
    return ev


def exec_one(i, reg, stk, ev, g, U, isC, loadmem, regname):
    mn, ops, va = i.mnemonic, i.operands, i.address
    rn = lambda o: regname(o.reg)
    d = rn(ops[0]) if ops and ops[0].type == MIPS_OP_REG else None

    # stores
    if mn in ('sw', 'sh', 'sb'):
        m = [o for o in ops if o.type == MIPS_OP_MEM][0]
        b = regname(m.mem.base)
        bv = g(b)
        src = g(rn(ops[0]))
        if isC(bv):
            addr = (bv[1] + m.mem.disp) & 0xFFFFFFFF
            ev.append((va, 'store', mn, addr, src))
        elif bv[1].startswith('sp'):
            stk[m.mem.disp] = src
            ev.append((va, 'store', mn, None, ('stack%+d' % m.mem.disp, src)))
        else:
            ev.append((va, 'store', mn, None, ('%s%+d' % (bv[1], m.mem.disp), src)))
        return

    if mn in ('lw', 'lhu', 'lh', 'lbu', 'lb'):
        m = [o for o in ops if o.type == MIPS_OP_MEM][0]
        reg[d] = loadmem(g(regname(m.mem.base)), m.mem.disp, mn, va)
        return

    if mn == 'lui':      reg[d] = ('C', (ops[1].imm << 16) & 0xFFFFFFFF); return
    if mn == 'nop':      return

    def bin2(f, tag):
        a, b = g(rn(ops[1])), g(rn(ops[2]))
        reg[d] = ('C', f(a[1], b[1]) & 0xFFFFFFFF) if isC(a) and isC(b) else U('%s@%08X' % (tag, va))
    def imm2(f, tag):
        a = g(rn(ops[1]))
        reg[d] = ('C', f(a[1], ops[2].imm) & 0xFFFFFFFF) if isC(a) else U('%s@%08X' % (tag, va))

    if   mn in ('addiu', 'addi'): imm2(lambda x, y: x + y, 'add')
    elif mn == 'ori':             imm2(lambda x, y: x | y, 'or')
    elif mn == 'andi':            imm2(lambda x, y: x & y, 'and')
    elif mn == 'sll':             imm2(lambda x, y: x << y, 'shl')
    elif mn == 'srl':             imm2(lambda x, y: x >> y, 'shr')
    elif mn == 'sra':             imm2(lambda x, y: (x - (1 << 32) if x >> 31 else x) >> y, 'sra')
    elif mn in ('addu', 'add'):   bin2(lambda x, y: x + y, 'add')
    elif mn == 'and':             bin2(lambda x, y: x & y, 'and')
    elif mn == 'or':              bin2(lambda x, y: x | y, 'or')
    elif mn == 'subu':            bin2(lambda x, y: x - y, 'sub')
    elif mn == 'move':            reg[d] = g(rn(ops[1]))
    elif mn == 'srav':            reg[d] = U('srav@%08X' % va)
    elif mn == 'slti':
        a = g(rn(ops[1]))
        if isC(a):
            sa = a[1] - (1 << 32) if a[1] >> 31 else a[1]
            reg[d] = ('C', 1 if sa < ops[2].imm else 0)
        else:
            reg[d] = U('slti@%08X' % va)
    elif d:
        reg[d] = U('%s@%08X' % (mn, va))


def fmt(v):
    if v is None: return '<none>'
    return '0x%08X' % v[1] if v[0] == 'C' else v[1]

for dbidx in (0, 1):
    print('=' * 78)
    print('=== INTERPRETATION with *(0x8009D7F0) [double-buffer idx] = %d ===' % dbidx)
    print('=' * 78)
    ev = interp(dbidx)
    for e in ev:
        va, kind = e[0], e[1]
        if kind == 'store':
            addr, src = e[3], e[4]
            if addr is not None:
                print('  0x%08X STORE.%-2s -> 0x%08X   value=%s' % (va, e[2], addr, fmt(src)))
            else:
                print('  0x%08X STORE.%-2s -> %-22s value=%s' % (va, e[2], e[4][0], fmt(e[4][1])))
        elif kind == 'CALL':
            args, sargs = e[4]
            print('  0x%08X CALL %s (0x%08X)' % (va, e[2], e[3]))
            for a in ('a0', 'a1', 'a2', 'a3'):
                print('           %s = %s' % (a, fmt(args[a])))
            for so in (0x10, 0x14, 0x18, 0x1c, 0x20, 0x24):
                if sargs[so] is not None:
                    print('           sp+0x%02X = %s' % (so, fmt(sargs[so])))
        elif kind == 'load' and e[3] is not None:
            print('  0x%08X LOAD .%-2s <- 0x%08X   %s' % (va, e[2], e[3], e[4]))
        elif kind == 'RETURN':
            print('  0x%08X RETURN' % va)
        elif kind == 'BRANCH-UNRESOLVED':
            print('  0x%08X BRANCH %s -> 0x%08X  [data-dependent]' % (va, e[2], e[3]))
    print()
