"""Compare generated atlas object with pinned retail, resolving calls and local jumps."""
from pathlib import Path
import hashlib, json, struct
obj_path=Path('build/src/slus_006.64/system/temp1.c.o')
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
sym=next(s for s in symbols if string(strings,s[0])=='func_800263E4')
start,size,section=sym[1],sym[2],sym[5]
out=bytearray(data(sections[section])[start:start+size]);relocs=[]
addresses={'SetPolyFT4':0x80043CB0,'SetSemiTrans':0x80043BFC,'SetShadeTex':0x80043C24,'GetTPage':0x80043A1C,'GetClut':0x80043A58}
for s in sections:
 if s[1]!=9 or s[7]!=section:continue
 for i in range(0,s[5],s[9]):
  off,info=struct.unpack_from('<II',data(s),i)
  if not start<=off<start+size:continue
  symbol=symbols[info>>8];name=string(strings,symbol[0]);assert info&255==4
  pos=off-start;word=struct.unpack_from('<I',out,pos)[0]
  if name in addresses:
   assert word==0x0c000000
   target=addresses[name]
  else:
   assert symbol[5]==section and symbol[3]&15==3
   offset=(word&0x3ffffff)*4+symbol[1]
   assert start<=offset<start+size
   target=0x800263e4+offset-start
  struct.pack_into('<I',out,pos,(word&0xfc000000)|(target>>2&0x3ffffff))
  relocs.append({'offset':pos,'symbol':name or '.text','retail_address':hex(target)})
assert sum(r["symbol"] in addresses for r in relocs)==5
retail=Path('disc/SLUS_006.64').read_bytes()
assert hashlib.sha256(retail).hexdigest()=='dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119'
expected=retail[0x263e4-0xf800:0x2675c-0xf800]
report={'function':'func_800263E4','size':size,'retail_start':'0x800263E4','object_sha256':hashlib.sha256(b).hexdigest(),'relocations':relocs,'retail_bytes_sha256':hashlib.sha256(expected).hexdigest(),'resolved_bytes_sha256':hashlib.sha256(out).hexdigest(),'match':out==expected,'scope':'Function object bytes with calls resolved to retail addresses; not a whole-ROM match.'}
print(json.dumps(report,indent=2))


assert out == expected, "compiled atlas bytes do not match retail"
