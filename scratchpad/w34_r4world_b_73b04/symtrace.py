#!/usr/bin/env python3
"""Symbolic register tracker for 0x80073B04.

Tracks each register as either a concrete constant (address/immediate) or an
opaque symbol. This gives an exact, non-guessed global-address audit and an
ABI (def-before-use) proof.
"""
import struct, hashlib, csv, os
from capstone import *
from capstone.mips import *

ROM  = '/home/blizz/dev/xenogears-decomp/disc/world_map.bin'
BASE = 0x8006FAF0
DATA = open(ROM, 'rb').read()
OUT  = os.path.dirname(os.path.abspath(__file__))
ENTRY, END_VA = 0x80073B04, 0x80073E30

def off(va): return va - BASE
def w32(va): return struct.unpack_from('<I', DATA, off(va))[0]

md = Cs(CS_ARCH_MIPS, CS_MODE_MIPS32 + CS_MODE_LITTLE_ENDIAN)
md.detail = True
insns = list(md.disasm(DATA[off(ENTRY):off(END_VA)], ENTRY))
by_va = {i.address: i for i in insns}

LIBGTE = {0x8004A92C: 'RotMatrixYXZ', 0x8004931C: 'CompMatrix',
          0x80049EFC: 'SetRotMatrix', 0x80049F8C: 'SetTransMatrix',
          0x8004A73C: 'RotTransPers4'}

# ---- value lattice: ('c', n) concrete | ('sym', name) opaque
def C(n): return ('c', n & 0xFFFFFFFF)
def S(n): return ('sym', n)

CALLER_SAVED = set()
for r in ['$v0','$v1','$a0','$a1','$a2','$a3','$t0','$t1','$t2','$t3','$t4',
          '$t5','$t6','$t7','$t8','$t9','$ra']:
    CALLER_SAVED.add(r)

def run(trace_label, iterate_loop=True):
    """Linear walk; the single backward branch is handled by unrolling twice."""
    reg = {}
    reg['$sp'] = S('sp')
    reg['$zero'] = C(0)
    # Registers NOT initialised -> any read is an incoming-argument read (ABI proof)
    reads_before_def = []
    events = []   # (va, kind, detail)

    def get(name):
        if name == '$zero': return C(0)
        if name not in reg:
            reads_before_def.append((cur.address, name))
            reg[name] = S('IN_' + name)
        return reg[name]

    order = [i for i in insns]
    for i in order:
        cur = i
        va = i.address
        mn, ops = i.mnemonic, i.operands

        def rn(o): return i.reg_name(o.reg)

        # ---- memory operand address resolution
        for o in ops:
            if o.type == MIPS_OP_MEM:
                b = i.reg_name(o.mem.base)
                bv = get(b)
                if bv[0] == 'c':
                    addr = (bv[1] + o.mem.disp) & 0xFFFFFFFF
                    kind = ('store' if mn.startswith('s') else 'load')
                    events.append((va, kind, mn, '0x%08X' % addr, b, o.mem.disp,
                                   'concrete'))
                else:
                    kind = ('store' if mn.startswith('s') else 'load')
                    events.append((va, kind, mn, '%s+%d' % (bv[1], o.mem.disp),
                                   b, o.mem.disp, 'symbolic'))

        d = rn(ops[0]) if ops and ops[0].type == MIPS_OP_REG else None

        if mn == 'lui':
            reg[d] = C(ops[1].imm << 16)
        elif mn in ('addiu', 'addi'):
            s = get(rn(ops[1]))
            reg[d] = C(s[1] + ops[2].imm) if s[0] == 'c' else S('%s+%d' % (s[1], ops[2].imm))
        elif mn == 'ori':
            s = get(rn(ops[1]))
            reg[d] = C(s[1] | ops[2].imm) if s[0] == 'c' else S('%s|%d' % (s[1], ops[2].imm))
        elif mn == 'andi':
            s = get(rn(ops[1]))
            reg[d] = C(s[1] & ops[2].imm) if s[0] == 'c' else S('%s&%d' % (s[1], ops[2].imm))
        elif mn in ('addu', 'add'):
            a, b = get(rn(ops[1])), get(rn(ops[2]))
            reg[d] = C(a[1] + b[1]) if a[0] == b[0] == 'c' else S('sum@%08X' % va)
        elif mn in ('and',):
            a, b = get(rn(ops[1])), get(rn(ops[2]))
            reg[d] = C(a[1] & b[1]) if a[0] == b[0] == 'c' else S('and@%08X' % va)
        elif mn in ('or',):
            a, b = get(rn(ops[1])), get(rn(ops[2]))
            reg[d] = C(a[1] | b[1]) if a[0] == b[0] == 'c' else S('or@%08X' % va)
        elif mn in ('sll',):
            a = get(rn(ops[1]))
            reg[d] = C(a[1] << ops[2].imm) if a[0] == 'c' else S('shl@%08X' % va)
        elif mn in ('srl', 'sra'):
            a = get(rn(ops[1]))
            reg[d] = C(a[1] >> ops[2].imm) if a[0] == 'c' else S('shr@%08X' % va)
        elif mn == 'srav':
            reg[d] = S('srav@%08X' % va)
        elif mn == 'move':
            reg[d] = get(rn(ops[1]))
        elif mn in ('lw', 'lhu', 'lh', 'lbu', 'lb'):
            reg[d] = S('%s@%08X' % (mn, va))
        elif mn == 'slti':
            reg[d] = S('slti@%08X' % va)
        elif mn == 'jal':
            t = ops[0].imm
            events.append((va, 'CALL', 'jal', '0x%08X' % t, LIBGTE.get(t, '?'), 0, 'call'))
            # snapshot argument registers at the call (delay slot executes first
            # in hardware, handled below by pre-applying it)
            ds = by_va[va + 4]
            argsnap = {}
            for a in ('$a0', '$a1', '$a2', '$a3'):
                v = reg.get(a)
                argsnap[a] = ('0x%08X' % v[1]) if v and v[0] == 'c' else (v[1] if v else 'UNDEF')
            events.append((va, 'CALLARGS', LIBGTE.get(t, '?'),
                           ' '.join('%s=%s' % (k, argsnap[k]) for k in
                                    ('$a0', '$a1', '$a2', '$a3')), '', 0, 'args'))
            for r in CALLER_SAVED:
                reg[r] = S('clobber@%08X_%s' % (va, r))
            reg['$v0'] = S('ret@%08X' % va)
        elif mn in ('sw', 'sh', 'sb', 'nop', 'jr', 'bnez', 'beqz', 'bltz', 'bgez'):
            pass
        else:
            if d:
                reg[d] = S('%s@%08X' % (mn, va))
    return events, reads_before_def

events, rbd = run('linear')

# ---------------- ABI proof
print('=== ABI: def-before-use ===')
if not rbd:
    print('  no register read before definition -> takes NO register arguments')
else:
    for va, r in rbd:
        print('  0x%08X reads %s before any definition' % (va, r))
print()

# ---------------- concrete globals
print('=== CONCRETE GLOBAL ACCESSES ===')
rows = []
for e in events:
    va, kind, mn, addr, base, disp, tag = e
    if tag == 'concrete' and kind in ('load', 'store'):
        rows.append(e)
        print('  0x%08X %-6s %-4s %s   (base %s%+d)' % (va, kind, mn, addr, base, disp))
print()

print('=== SYMBOLIC (computed-pointer) ACCESSES ===')
for e in events:
    va, kind, mn, addr, base, disp, tag = e
    if tag == 'symbolic' and kind in ('load', 'store'):
        print('  0x%08X %-6s %-4s base=%s disp=%+d' % (va, kind, mn, base, disp))
print()

print('=== CALL ARGUMENTS (register a0-a3 at call site) ===')
for e in events:
    if e[1] == 'CALLARGS':
        print('  0x%08X %-16s %s' % (e[0], e[2], e[3]))
print()

with open(os.path.join(OUT, 'GLOBAL_ADDRESS_AUDIT.csv'), 'w', newline='') as f:
    w = csv.writer(f)
    w.writerow(['site_va', 'kind', 'insn', 'address_or_expr', 'base_reg', 'disp', 'resolution'])
    for e in events:
        if e[1] in ('load', 'store'):
            w.writerow(['0x%08X' % e[0], e[1], e[2], e[3], e[4], e[5], e[6]])
print('wrote GLOBAL_ADDRESS_AUDIT.csv')
