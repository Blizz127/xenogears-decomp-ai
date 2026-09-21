"""Size is a necessary exact-match gate, not a behavioral equivalence test."""
from pathlib import Path
import struct, json, hashlib
p=Path('build/src/field/dialogue/text_box_render.c.o');b=p.read_bytes()
assert b[:6]==b'\x7fELF\x01\x01'
off=struct.unpack_from('<I',b,32)[0];esz,n,_=struct.unpack_from('<HHH',b,46)
secs=[struct.unpack_from('<10I',b,off+i*esz) for i in range(n)]
def data(s): return b[s[4]:s[4]+s[5]]
def string(t,i): return t[i:t.index(0,i)].decode()
sy=next(s for s in secs if s[1]==2);strings=data(secs[sy[6]])
for i in range(0,sy[5],sy[9]):
 r=struct.unpack_from('<IIIBBH',data(sy),i)
 if string(strings,r[0])=='func_8007F8DC': break
else: raise AssertionError('function absent')
print(json.dumps({'object':str(p),'object_sha256':hashlib.sha256(b).hexdigest(),'compiled_bytes':r[2],'retail_bytes':1804,'exact_match':False if r[2]!=1804 else 'NOT_CHECKED','scope':'Size gate only; relocation-normalized byte comparison not run if sizes differ.'},indent=2))
assert r[2]==1804, 'compiled function size differs from retail'
