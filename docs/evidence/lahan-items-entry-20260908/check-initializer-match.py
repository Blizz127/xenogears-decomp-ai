"""Compare generated allocator object with pinned retail, resolving its two calls."""
from pathlib import Path
import hashlib, json, struct
obj_path=Path('build/src/menu/main/misc.c.o')
b=obj_path.read_bytes()
assert b[:6]==b'\x7fELF\x01\x01'
shoff=struct.unpack_from('<I',b,32)[0]
entsize,count,names=struct.unpack_from('<HHH',b,46)
sections=[struct.unpack_from('<10I',b,shoff+i*entsize) for i in range(count)]
def data(s):return b[s[4]:s[4]+s[5]]
def string(s,i):return s[i:s.index(0,i)].decode()
section_names=data(sections[names])
si=next(i for i,s in enumerate(sections) if string(section_names,s[0])=='.symtab')
symsec=sections[si];strings=data(sections[symsec[6]])
symbols=[struct.unpack_from('<IIIBBH',data(symsec),i) for i in range(0,symsec[5],symsec[9])]
sym=next(s for s in symbols if string(strings,s[0])=='func_801DA4A8')
start,size,section=sym[1],sym[2],sym[5]
out=bytearray(data(sections[section])[start:start+size]);relocs=[]
addresses={'func_801D22F4':0x801D22F4,'func_801E8018':0x801E8018,'HeapAlloc':0x80031BDC,'bzero':0x8003F8E8,'func_801C72BC':0x801C72BC,'g_Menu':0x800625A0,'D_801EA548':0x801EA548}
for sec in sections:
 if sec[1]!=9 or sec[7]!=section:continue
 for i in range(0,sec[5],sec[9]):
  off,info=struct.unpack_from('<II',data(sec),i)
  if not start<=off<start+size:continue
  name=string(strings,symbols[info>>8][0]);kind=info&255;assert name in addresses
  pos=off-start;word=struct.unpack_from('<I',out,pos)[0];addr=addresses[name]
  if kind==4:
   assert word==0x0c000000;word|=(addr>>2)&0x3ffffff
  elif kind in (5,6):
   assert word&65535==0
   word|=((addr+0x8000)>>16)&65535 if kind==5 else addr&65535
  else:raise AssertionError(kind)
  struct.pack_into('<I',out,pos,word)
  relocs.append({'offset':pos,'type':kind,'symbol':name,'retail_address':hex(addr)})
assert len(relocs)==11
expected=Path('disc/menu.bin').read_bytes()[0x154a8:0x15518]
assert hashlib.sha256(expected).hexdigest()=='06aab1a7db7c225166b03ac059f45cdcfeec49fd9e7180c9918cdf6ffde33226'
differences=[{'offset':i,'compiled':out[i:i+4].hex(),'retail':expected[i:i+4].hex()} for i in range(0,max(len(out),len(expected)),4) if out[i:i+4]!=expected[i:i+4]]
print(json.dumps({'function':'func_801DA4A8','object_sha256':hashlib.sha256(b).hexdigest(),'compiled_bytes':size,'retail_bytes':len(expected),'relocations':relocs,'differences':differences,'match':out==expected,'scope':'Object function with zero-addend relocations resolved to retail addresses; not whole-ROM match'},indent=2))
assert out==expected
