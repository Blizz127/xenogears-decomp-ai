import sys, re, capstone
exe=open('/var/home/blizz/Projects/xenogears-decomp-ai/disc/SLUS_006.64','rb').read()
syms={}
for line in open('/var/home/blizz/Projects/xenogears-decomp-ai/config/symbol_addrs.slus_006.64.txt'):
    m=re.match(r'(\w+)\s*=\s*0x([0-9A-Fa-f]+)',line)
    if m: syms[int(m.group(2),16)]=m.group(1)
md=capstone.Cs(capstone.CS_ARCH_MIPS, capstone.CS_MODE_MIPS32|capstone.CS_MODE_LITTLE_ENDIAN); md.skipdata=True
start=int(sys.argv[1],16); end=int(sys.argv[2],16)
off=lambda a: 0x800 + a - 0x80010000
for i in md.disasm(exe[off(start):off(end)], start):
    s=i.op_str; m=re.search(r'0x([0-9a-f]+)$', s); extra=''
    if i.mnemonic in ('jal','j') and m:
        a=int(m.group(1),16); extra='   <'+syms.get(a,'?')+'>'
    print('%08x: %08x  %-8s %s%s'%(i.address,int.from_bytes(i.bytes,'little'),i.mnemonic,s,extra))
