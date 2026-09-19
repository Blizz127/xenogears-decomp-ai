from pathlib import Path
import struct,hashlib,json
p=Path('scratchpad/astra-field12-20260919/misc6.c.o');b=p.read_bytes();shoff=struct.unpack_from('<I',b,32)[0];esize,n,_=struct.unpack_from('<HHH',b,46)
secs=[struct.unpack_from('<10I',b,shoff+i*esize) for i in range(n)]
def data(s):return b[s[4]:s[4]+s[5]]
st=next(s for s in secs if s[1]==2);strings=data(secs[st[6]]);syms=[struct.unpack_from('<IIIBBH',data(st),i) for i in range(0,st[5],st[9])]
def name(s):return strings[s[0]:].split(b'\0')[0].decode()
s=next(s for s in syms if name(s)=='func_8009E10C');code=bytearray(data(secs[s[5]])[s[1]:s[1]+s[2]])
addrs={'FieldScriptVMGetArgument':0x800ACDEC,'g_FieldScriptVMCurActor':0x800B0078};rs=[]
for sec in secs:
 if sec[1]!=9 or sec[7]!=s[5]:continue
 for off in range(0,sec[5],sec[9]):
  loc,info=struct.unpack_from('<II',data(sec),off)
  if not s[1]<=loc<s[1]+s[2]:continue
  kind=info&255;sym=name(syms[info>>8]);addr=addrs[sym];pos=loc-s[1];word=struct.unpack_from('<I',code,pos)[0]
  if kind==4:
   assert word&0x03ffffff==0;word|=(addr>>2)&0x03ffffff
  elif kind in (5,6):
   assert word&0xffff==0;word|=((addr+0x8000)>>16)&0xffff if kind==5 else addr&0xffff
  else:raise AssertionError(kind)
  struct.pack_into('<I',code,pos,word);rs.append({'offset':pos,'kind':kind,'symbol':sym,'address':hex(addr)})
raw=Path('disc/field.bin').read_bytes();retail=raw[0x8009e10c-0x8006faf0:0x8009e1a0-0x8006faf0]
assert len(rs)==3 and code==retail
print(json.dumps({'function':'func_8009E10C','bytes':len(code),'relocations':rs,'byte_identical':True,'compiled_sha256':hashlib.sha256(code).hexdigest(),'retail_sha256':hashlib.sha256(retail).hexdigest()},indent=2))
