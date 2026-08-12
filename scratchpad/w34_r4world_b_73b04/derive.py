#!/usr/bin/env python3
"""W34-R4WORLD-B fresh retail derivation for 0x80073B04.

Independent of the 71A58 audit: boundary is re-derived from raw bytes,
not inherited.
"""
import struct, hashlib, sys, csv, os
from capstone import *
from capstone.mips import *

ROM  = '/home/blizz/dev/xenogears-decomp/disc/world_map.bin'
BASE = 0x8006FAF0
DATA = open(ROM, 'rb').read()
END  = BASE + len(DATA)
OUT  = os.path.dirname(os.path.abspath(__file__))

def off(va):
    o = va - BASE
    if o < 0 or o >= len(DATA):
        raise ValueError('VA 0x%08X outside image [0x%08X,0x%08X)' % (va, BASE, END))
    return o

def w32(va):
    return struct.unpack_from('<I', DATA, off(va))[0]

def sl(a, b):
    return DATA[off(a):off(b)]

def sha(b):
    return hashlib.sha256(b).hexdigest()

md = Cs(CS_ARCH_MIPS, CS_MODE_MIPS32 + CS_MODE_LITTLE_ENDIAN)
md.detail = True

# ---------------------------------------------------------------- boundary
# Entry proof: scan backwards from 0x80073B04 for the previous `jr $ra` +
# delay slot; forwards for the terminating `jr $ra` + delay slot.
ENTRY = 0x80073B04

def is_jr_ra(va):
    return w32(va) == 0x03E00008

def find_prev_return(va):
    p = va - 4
    while p > BASE:
        if is_jr_ra(p):
            return p
        p -= 4
    return None

def find_next_return(va):
    p = va
    while p < END - 4:
        if is_jr_ra(p):
            return p
        p += 4
    return None

prev_ret = find_prev_return(ENTRY)
next_ret = find_next_return(ENTRY)
END_VA   = next_ret + 8            # jr $ra + delay slot

body = sl(ENTRY, END_VA)
NINS = len(body) // 4

# ---------------------------------------------------------------- listing
LIBGTE = {
    0x8004A92C: 'RotMatrixYXZ',
    0x8004931C: 'CompMatrix',
    0x80049EFC: 'SetRotMatrix',
    0x80049F8C: 'SetTransMatrix',
    0x8004A73C: 'RotTransPers4',
}

insns = list(md.disasm(body, ENTRY))
assert len(insns) == NINS, 'capstone decoded %d of %d' % (len(insns), NINS)

def note(i):
    if i.id in (MIPS_INS_JAL,):
        t = i.operands[0].imm
        return '  ; %s' % LIBGTE.get(t, 'sub_%08X' % t)
    return ''

lines = []
lines.append('# FRESH DERIVATION 0x%08X .. 0x%08X  (%d insns)' % (ENTRY, END_VA, NINS))
lines.append('# slice sha256 = %s' % sha(body))
lines.append('# image: %s size=%d sha256=%s' % (os.path.basename(ROM), len(DATA), sha(DATA)))
lines.append('# prev `jr $ra` (end of preceding fn) = 0x%08X' % prev_ret)
for i in insns:
    lines.append('%08X: %08x  %-10s %s%s' % (
        i.address, w32(i.address), i.mnemonic, i.op_str, note(i)))
open(os.path.join(OUT, '73B04_LISTING.txt'), 'w').write('\n'.join(lines) + '\n')

# ---------------------------------------------------------------- calls
calls = []
for i in insns:
    if i.id == MIPS_INS_JAL:
        t = i.operands[0].imm
        calls.append((i.address, 'JAL', t, LIBGTE.get(t, ''),
                      'in image' if BASE <= t < END else 'outside image (SLUS)'))
    if i.id in (MIPS_INS_JALR,):
        calls.append((i.address, 'JALR', 0, i.op_str, 'indirect'))

with open(os.path.join(OUT, 'CALL_TARGETS.csv'), 'w', newline='') as f:
    w = csv.writer(f)
    w.writerow(['site_va', 'kind', 'target_va', 'symbol', 'location'])
    for a, k, t, s, loc in calls:
        w.writerow(['0x%08X' % a, k, '0x%08X' % t if t else '', s, loc])

# ---------------------------------------------------------------- branches
branches = []
for i in insns:
    grp = i.groups
    if MIPS_GRP_JUMP in grp or MIPS_GRP_BRANCH_RELATIVE in grp:
        if i.id in (MIPS_INS_JAL, MIPS_INS_JALR, MIPS_INS_JR):
            continue
        tgt = None
        for op in i.operands:
            if op.type == MIPS_OP_IMM:
                tgt = op.imm
        if tgt is None:
            continue
        direction = 'backward (LOOP)' if tgt <= i.address else 'forward'
        inside = ENTRY <= tgt < END_VA
        branches.append((i.address, i.mnemonic, i.op_str, tgt, direction, inside))

# ---------------------------------------------------------------- globals
# gather lui/addiu and lui/lw pairs per register
def global_audit():
    hi = {}
    rows = []
    for i in insns:
        if i.id == MIPS_INS_LUI:
            hi[i.operands[0].reg] = (i.operands[1].imm, i.address)
        elif i.id == MIPS_INS_ADDIU and len(i.operands) == 3:
            src = i.operands[1].reg
            if src in hi and i.operands[2].type == MIPS_OP_IMM:
                h, ha = hi[src]
                va = ((h << 16) + i.operands[2].imm) & 0xFFFFFFFF
                rows.append((i.address, 'addr-form', va, i.mnemonic + ' ' + i.op_str, ha))
                hi[i.operands[0].reg] = (va >> 16, ha) if False else hi.get(i.operands[0].reg, (h, ha))
        elif i.id == MIPS_INS_ORI and len(i.operands) == 3:
            src = i.operands[1].reg
            if src in hi and i.operands[2].type == MIPS_OP_IMM:
                h, ha = hi[src]
                va = ((h << 16) | i.operands[2].imm) & 0xFFFFFFFF
                rows.append((i.address, 'addr-form', va, i.mnemonic + ' ' + i.op_str, ha))
        else:
            for op in i.operands:
                if op.type == MIPS_OP_MEM and op.mem.base in hi:
                    h, ha = hi[op.mem.base]
                    va = ((h << 16) + op.mem.disp) & 0xFFFFFFFF
                    kind = 'load' if i.id in (MIPS_INS_LW, MIPS_INS_LHU, MIPS_INS_LH,
                                              MIPS_INS_LBU, MIPS_INS_LB) else 'store'
                    rows.append((i.address, kind, va, i.mnemonic + ' ' + i.op_str, ha))
    return rows

grows = global_audit()
with open(os.path.join(OUT, 'GLOBAL_ADDRESS_AUDIT.csv'), 'w', newline='') as f:
    w = csv.writer(f)
    w.writerow(['site_va', 'kind', 'address', 'insn', 'lui_site'])
    for a, k, va, ins, ha in grows:
        w.writerow(['0x%08X' % a, k, '0x%08X' % va, ins, '0x%08X' % ha])

# ---------------------------------------------------------------- write set
STORES = {MIPS_INS_SW: 4, MIPS_INS_SH: 2, MIPS_INS_SB: 1, MIPS_INS_SWC1: 4}
wrows = []
for i in insns:
    if i.id in STORES:
        m = [op for op in i.operands if op.type == MIPS_OP_MEM][0]
        wrows.append((i.address, i.mnemonic, STORES[i.id], i.op_str,
                      i.reg_name(m.mem.base), m.mem.disp))
with open(os.path.join(OUT, 'WRITE_SET.csv'), 'w', newline='') as f:
    w = csv.writer(f)
    w.writerow(['site_va', 'insn', 'width_bytes', 'operands', 'base_reg', 'disp'])
    for r in wrows:
        w.writerow(['0x%08X' % r[0], r[1], r[2], r[3], r[4], r[5]])

# ---------------------------------------------------------------- COP2/GTE
cop2 = [i for i in insns if i.mnemonic.startswith(('mtc2', 'mfc2', 'ctc2', 'cfc2',
                                                   'lwc2', 'swc2')) or i.bytes[3] >> 2 == 0x12]

# ---------------------------------------------------------------- report
print('ENTRY      0x%08X' % ENTRY)
print('END        0x%08X (exclusive)' % END_VA)
print('prev jr $ra 0x%08X  (preceding function ends at 0x%08X)' % (prev_ret, prev_ret + 8))
print('bytes      %d' % len(body))
print('insns      %d' % NINS)
print('slice sha  %s' % sha(body))
print()
print('--- CALLS (%d) ---' % len(calls))
for a, k, t, s, loc in calls:
    print('  0x%08X %s 0x%08X %-16s %s' % (a, k, t, s, loc))
print()
print('--- BRANCHES (%d) ---' % len(branches))
for a, mn, ops, t, d, ins in branches:
    print('  0x%08X %-6s %-28s -> 0x%08X  %s  %s' % (a, mn, ops, t, d,
          'internal' if ins else 'EXTERNAL'))
print()
print('--- COP2/GTE direct insns: %d ---' % len(cop2))
for i in cop2:
    print('  0x%08X %s %s' % (i.address, i.mnemonic, i.op_str))
print()
print('--- STORES (%d) ---' % len(wrows))
for r in wrows:
    print('  0x%08X %-4s w=%d  %s' % (r[0], r[1], r[2], r[3]))
print()
print('--- RETURN PATHS ---')
for i in insns:
    if i.id == MIPS_INS_JR:
        print('  0x%08X jr %s   (delay slot 0x%08X = %08x)' % (
            i.address, i.op_str, i.address + 4, w32(i.address + 4)))
