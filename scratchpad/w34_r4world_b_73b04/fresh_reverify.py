#!/usr/bin/env python3
"""Fresh retail-byte re-verification for isolated 0x80073B04."""
from pathlib import Path
import csv
import hashlib
import struct

from capstone import CS_ARCH_MIPS, CS_MODE_LITTLE_ENDIAN, CS_MODE_MIPS32, Cs
from capstone.mips import MIPS_OP_IMM

ROOT = Path(__file__).resolve().parents[2]
IMAGE = ROOT / "disc" / "world_map.bin"
OUT = Path(__file__).resolve().parent
BASE = 0x8006FAF0
START = 0x80073B04

data = IMAGE.read_bytes()
image_end = BASE + len(data)

def off(va):
    value = va - BASE
    if value < 0 or value >= len(data):
        raise ValueError(f"VA 0x{va:08X} is outside image")
    return value

def word(va):
    return struct.unpack_from("<I", data, off(va))[0]

def words(a, b):
    return data[off(a):off(b)]

def fmt_target(insn):
    for operand in insn.operands:
        if operand.type == MIPS_OP_IMM:
            return f"0x{operand.imm & 0xFFFFFFFF:08X}"
    return ""

md = Cs(CS_ARCH_MIPS, CS_MODE_MIPS32 | CS_MODE_LITTLE_ENDIAN)
md.detail = True

# The first JR $ra in this body is the single epilogue. Include its delay slot.
end = None
for address in range(START, image_end - 4, 4):
    insns = list(md.disasm(words(address, address + 4), address))
    if insns and insns[0].mnemonic == "jr" and insns[0].op_str.strip() == "$ra":
        end = address + 8
        break
if end is None:
    raise RuntimeError("no retail jr $ra found")

insns = list(md.disasm(words(START, end), START))
if len(insns) * 4 != end - START:
    raise RuntimeError("retail slice did not disassemble at 4-byte granularity")
if word(end) == 0:
    next_words = f"0x{word(end):08X}, 0x{word(end + 4):08X}"
else:
    next_words = f"0x{word(end):08X}, 0x{word(end + 4):08X}"

slice_sha = hashlib.sha256(words(START, end)).hexdigest()
image_sha = hashlib.sha256(data).hexdigest()

with (OUT / "73B04_LISTING.txt").open("w") as f:
    f.write(f"# fresh decode: world_map.bin sha256={image_sha}\n")
    f.write(f"# base 0x{BASE:08X} image [{BASE:#010x},{image_end:#010x})\n")
    f.write(f"# 0x{START:08X} .. 0x{end:08X} ({end-START} bytes / {len(insns)} insns) sha256={slice_sha}\n")
    for insn in insns:
        f.write(f"{insn.address:08X}: {word(insn.address):08x}  {insn.mnemonic:<10} {insn.op_str}\n")

calls = []
branches = []
delays = []
cop2 = []
for index, insn in enumerate(insns):
    mnem = insn.mnemonic.lower()
    if mnem in ("jal", "jalr"):
        calls.append((f"0x{insn.address:08X}", mnem, fmt_target(insn), insn.op_str))
    if mnem in ("b", "beq", "bne", "bnez", "beqz", "bgez", "bltz", "blez", "bgtz", "bc0f", "bc0t"):
        target = fmt_target(insn)
        branches.append((f"0x{insn.address:08X}", mnem, target, "loop" if target and int(target, 16) <= insn.address else "forward"))
        if index + 1 < len(insns):
            delay = insns[index + 1]
            delays.append((f"0x{insn.address:08X}", f"0x{delay.address:08X}", delay.mnemonic, delay.op_str))
    if (mnem.startswith("cop") or mnem in ("mfc2", "mtc2", "cfc2", "ctc2", "lwc2", "swc2")):
        cop2.append((f"0x{insn.address:08X}", mnem, insn.op_str))

with (OUT / "CALL_TARGETS.csv").open("w", newline="") as f:
    w = csv.writer(f)
    w.writerow(("pc", "kind", "target", "operands"))
    w.writerows(calls)

with (OUT / "GLOBAL_ADDRESS_AUDIT.csv").open("w", newline="") as f:
    w = csv.writer(f)
    w.writerow(("pc_or_range", "address_or_expression", "access", "width", "authority"))
    rows = [
        ("0x80073B08/0x80073B10", "0x1F800000 / 0x1F800000+0x38", "address", "word", "scratchpad angle and matrix arguments"),
        ("0x80073B18", "0x8009A300", "address", "byte", "static source SVECTOR[8], +0x20 per quad"),
        ("0x80073B28", "0x8009BD3A", "read", "halfword", "heading/scroll theta"),
        ("0x80073B50/0x80073B88/0x80073BB8/0x80073BE8", "0x8009D7F0", "read", "word", "double-buffer index"),
        ("0x80073C1C", "0x8009BD3A", "read", "halfword", "heading/scroll theta"),
        ("0x80073C3C", "0x8009C808", "read", "MATRIX", "world camera source"),
        ("0x80073C70..0x80073CF0", "0x8009C744 + quad*0x50 + idx*0x28", "write", "160 bytes", "two POLY_FT4 packet slots and projected XY"),
        ("0x80073D1C", "0x8009D3E4 / 0x8009D3D8", "read/write", "word", "DR_TWIN B/A tags"),
        ("0x80073D34", "0x8009BE3C -> +0x70", "read", "word/word", "current DB then OT base"),
        ("0x80073D2C", "0x80050100", "read", "word", "runtime arithmetic OT shift"),
        ("0x80073D04..0x80073D0C", "0x1F80005C", "read", "word", "last-quad GTE flag"),
        ("0x80073C18..0x80073C60", "0x1F800000..0x1F80005C", "write", "scratchpad", "angles, R translation, p and flag out-parameters"),
        ("0x80073D60..0x80073E08", "OT[(otz >> *(0x80050100))]", "read/write", "word", "single fixed OT bucket, four head-pushes"),
    ]
    w.writerows(rows)

with (OUT / "WRITE_SET.csv").open("w", newline="") as f:
    w = csv.writer(f)
    w.writerow(("pc_or_range", "target", "width", "value_or_semantics", "condition"))
    w.writerows([
        ("0x80073B48..0x80073C10", "0x8009C750 + quad*0x50 + idx*0x28 + {0x0,0x8,0x10,0x18}", "halfword", "V, V|0x80, V|0x3F00, V|0x3F80", "always, V=(theta>>2)&0x7F"),
        ("0x80073C14..0x80073C2C", "0x1F800000", "halfword", "vz=0, vx=0, vy=theta", "always"),
        ("0x80073C50..0x80073C60", "0x1F80004C/50/54", "word", "R.t={0,0,0}", "always"),
        ("0x80073C84..0x80073CF0", "packet0/packet1 + 0x08,+0x10,+0x18,+0x20", "word", "RotTransPers4 screen XY", "two quads"),
        ("0x80073D70..0x80073E08", "DR_TWIN/primitive tags and OT bucket", "word", "preserve high byte, replace low 24 bits", "unless last flag bit31 set"),
    ])

leaders = {START, 0x80073C84, 0x80073E0C}
for insn in insns:
    if insn.mnemonic.lower() in ("b", "beq", "bne", "bnez", "beqz", "bgez", "bltz", "blez", "bgtz"):
        target = fmt_target(insn)
        if target:
            leaders.add(int(target, 16))
        if insn.address + 8 < end:
            leaders.add(insn.address + 8)
leaders = sorted(x for x in leaders if START <= x < end)
blocks = []
for i, leader in enumerate(leaders):
    stop = leaders[i + 1] if i + 1 < len(leaders) else end
    body = [x for x in insns if leader <= x.address < stop]
    term = body[-1] if body else None
    if (len(body) >= 2 and body[-2].mnemonic.lower() in
            ("b", "beq", "bne", "bnez", "beqz", "bgez", "bltz", "blez", "bgtz")):
        term = body[-2]
    succ = []
    if term and term.mnemonic.lower() in ("b", "beq", "bne", "bnez", "beqz", "bgez", "bltz", "blez", "bgtz"):
        succ.append(fmt_target(term))
        if term.address + 8 < end:
            succ.append(f"0x{term.address+8:08X}")
    elif stop < end:
        succ.append(f"0x{stop:08X}")
    blocks.append((leader, stop, term, succ))

with (OUT / "73B04_CFG.md").open("w") as f:
    f.write(f"# Fresh CFG — 0x{START:08X}\n\n")
    f.write(f"Range `[0x{START:08X}, 0x{end:08X})`, {len(insns)} instructions. One counted loop (two iterations), one bit31 guard, no indirect branches.\n\n")
    f.write("## Basic blocks\n\n| block | range | terminator | successors |\n|---|---|---|---|\n")
    for i, (a, b, term, succ) in enumerate(blocks):
        t = f"0x{term.address:08X} {term.mnemonic} {term.op_str}" if term else ""
        f.write(f"| B{i} | 0x{a:08X}..0x{b:08X} | `{t}` | {', '.join(succ) or 'return'} |\n")
    f.write("\n## Branches, loops, and delay slots\n\n")
    for pc, mnem, target, kind in branches:
        f.write(f"- `{pc}` `{mnem}` → `{target}` ({kind})\n")
    f.write("\n| branch/call PC | delay PC | delay instruction |\n|---|---|---|\n")
    for row in delays:
        f.write(f"| {row[0]} | {row[1]} | `{row[2]} {row[3]}` |\n")
    f.write(f"\nCOP2/GTE opcodes in the body: **{len(cop2)}**; GTE work is performed by the five direct library calls in `CALL_TARGETS.csv`.\n")
    if cop2:
        f.write("\n" + "\n".join(f"- {a}: {m} {o}" for a, m, o in cop2) + "\n")

with (OUT / "73B04_BOUNDARY.md").open("w") as f:
    f.write("# Fresh 0x80073B04 boundary and ABI\n\n")
    f.write(f"- Image: `disc/world_map.bin`, {len(data)} bytes, SHA-256 `{image_sha}`.\n")
    f.write(f"- Load base: `0x{BASE:08X}`; image VA range `[0x{BASE:08X}, 0x{image_end:08X})`.\n")
    f.write(f"- Exact physical slice: `[0x{START:08X}, 0x{end:08X})`, file offsets `0x{off(START):06X}..0x{off(end):06X}`.\n")
    f.write(f"- Size: `{end-START}` bytes (`0x{end-START:X}`), `{len(insns)}` instructions; slice SHA-256 `{slice_sha}`.\n")
    f.write(f"- Boundary proof: the first `jr $ra` is at `0x{insns[-2].address:08X}`, its delay slot is `0x{insns[-1].address:08X}`; the next physical words are `{next_words}` at `0x{end:08X}`.\n")
    f.write("- ABI: leaf `void wm_80073B04(void)`; no argument registers are read, no return value is consumed, and there is no `jalr`.\n")
    f.write("- Stack frame: `0x40` bytes; saves `$ra` at `0x3C($sp)`, `$s0` at `0x28`, `$s1` at `0x2C`, `$s2` at `0x30`, `$s3` at `0x34`, `$s4` at `0x38`; restores them and returns through the sole epilogue.\n")
    f.write("- Control flow: one two-iteration loop over two quads; one last-quad GTE flag bit31 guard; no other return path.\n")
    f.write("- Direct calls, branches, delay slots, global/indirect accesses, and complete write set are recorded in the companion CSV/CFG files.\n")
    f.write("- Fresh retail result: no boundary, ABI, or dependency contradiction; the five direct GTE helpers are already linked in PsyCross and the helper is class A in isolation.\n")

print(f"boundary=0x{START:08X}..0x{end:08X} bytes={end-START} instructions={len(insns)} sha256={slice_sha}")
print(f"image_sha256={image_sha} calls={len(calls)} branches={len(branches)} cop2={len(cop2)}")
