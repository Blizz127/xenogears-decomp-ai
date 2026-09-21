from pathlib import Path
import struct,hashlib,json,subprocess,re
out=Path('scratchpad/astra-combat-20260919')
b=(out/'mips/main43.text').read_bytes();disc=Path('disc/battle.bin').read_bytes()
objdump=subprocess.check_output(['mips-linux-gnu-objdump','-dr',str(out/'mips/main43.o')],text=True)
symbols={'D_8005919C':0x8005919c,'D_800D366C':0x800d366c,'func_80039DB8':0x80039db8,'func_8008AA40':0x8008aa40}
rows=[]
for name,address,size in [('func_8008AA40',0x8008aa40,52),('func_8008AA74',0x8008aa74,44)]:
 start=int(re.search(r'^([0-9a-f]+) <'+name+r'>:',objdump,re.M)[1],16)
 code=bytearray(b[start:start+size])
 for m in re.finditer(r'^\s*([0-9a-f]+): R_MIPS_(HI16|LO16|26)\s+(\w+)$',objdump,re.M):
  off=int(m[1],16)-start
  if not 0<=off<size:continue
  value=symbols[m[3]];word=struct.unpack_from('<I',code,off)[0]
  if m[2]=='26':word=(word&0xfc000000)|((value>>2)&0x3ffffff)
  elif m[2]=='HI16':word=(word&0xffff0000)|(((value+0x8000)>>16)&0xffff)
  else:word=(word&0xffff0000)|(value&0xffff)
  struct.pack_into('<I',code,off,word)
 retail=disc[address-0x8006faf0:address-0x8006faf0+size]
 assert code==retail,name
 mutant=bytearray(code);mutant[0]^=1;assert mutant!=retail
 rows.append(dict(function=name,retail_address=hex(address),bytes=size,status='BYTE_IDENTICAL',sha256=hashlib.sha256(code).hexdigest()))
(out/'sound-bytes.json').write_text(json.dumps(rows,indent=2)+'\n');print(json.dumps(rows,indent=2))
