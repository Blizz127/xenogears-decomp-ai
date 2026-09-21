import sys, re, capstone
base=0x801DC000
blob=open('/var/tmp/xeno-m16/ovl/arc6b9.bin','rb').read()
syms={}
for f in ['/var/home/blizz/Projects/xenogears-decomp-ai/config/symbol_addrs.slus_006.64.txt','/var/home/blizz/Projects/xenogears-decomp-ai/config/symbol_addrs.field.txt']:
    for line in open(f):
        m=re.match(r'(\w+)\s*=\s*0x([0-9A-Fa-f]+)',line)
        if m: syms[int(m.group(2),16)]=m.group(1)
md=capstone.Cs(capstone.CS_ARCH_MIPS, capstone.CS_MODE_MIPS32|capstone.CS_MODE_LITTLE_ENDIAN)
md.skipdata=True
start=int(sys.argv[1],16); end=int(sys.argv[2],16)
for i in md.disasm(blob[start-base:end-base], start):
    s=i.op_str
    m=re.search(r'0x([0-9a-f]+)$', s)
    extra=''
    if i.mnemonic in ('jal','j') and m:
        a=int(m.group(1),16); extra='   <'+syms.get(a, 'ovl_%08x'%a)+'>'
    print('%08x: %08x  %-8s %s%s'%(i.address, int.from_bytes(i.bytes,'little'), i.mnemonic, s, extra))
