#!/usr/bin/env python3
"""m2c_insert.py OV SEG names...

Replace `INCLUDE_ASM("asm/OV/nonmatchings/SEG", name);` lines in src/OV/SEG.c
with the cleaned m2c_try.py drafts: M2C_FIELD becomes a raw-offset access,
M2C_UNK becomes s32, the drafts' externs/prototypes are merged at the top of
the file's first `#ifndef XENO_PC_PORT` block (skipping anything the file's
headers already declare, and using each inserted definition's own signature
as its prototype), and an extern whose type conflicts with one already in
scope is read through `(*(T*)&sym)`.  Rebuild and compare afterwards."""
import sys,os,re,subprocess
sys.path.insert(0, os.path.dirname(__file__))
from assist_common import CACHE, REPO
os.chdir(REPO)
OV,SEG=sys.argv[1],sys.argv[2]; names=sys.argv[3:]
D=os.path.join(CACHE,'m2c',OV,SEG)
os.makedirs(CACHE,exist_ok=True)
INC_C=os.path.join(CACHE,'_inc.c'); INC_I=os.path.join(CACHE,'_inc.i')
p=f'src/{OV}/{SEG}.c'; s=open(p).read()
# identifiers declared by the file's includes
incs=''.join(l+'\n' for l in s.split('\n') if l.startswith('#include'))
open(INC_C,'w').write(incs+'#include "system/memory.h"\n')
subprocess.run(['tools/gcc-2.7.2-psx/cpp','-Iinclude','-D_LANGUAGE_C','-DSKIP_ASM','-P','-undef','-lang-c','-nostdinc',INC_C,INC_I],capture_output=True)
hdrdecl=set(re.findall(r'\b(\w+)\s*\(',open(INC_I).read()))|set(re.findall(r'\b(\w+)\s*(?:\[[^\]]*\])*\s*;',open(INC_I).read()))
types={}; arrays=set()
for m in re.finditer(r'^extern ([^;(]*?)\s*\b(\w+)(\[\])?;',s,re.M):
    types[m[2]]=m[1]
    if m[3]: arrays.add(m[2])
seen=set(types)|set(re.findall(r'^\w[\w\s\*]*?(\w+)\([^;{]*\);\s*$',s,re.M))
externs=[]
def clean(c):
    while 'M2C_FIELD(' in c:
        i=c.rindex('M2C_FIELD(')
        j=i+len('M2C_FIELD('); d=1; args=[]; cur=''
        while d:
            ch=c[j]
            if ch=='(': d+=1
            elif ch==')':
                d-=1
                if d==0: break
            if ch==',' and d==1: args.append(cur); cur=''
            else: cur+=ch
            j+=1
        args.append(cur)
        e,t,o=[a.strip() for a in args]
        c=c[:i]+f'*({t})((s8*)({e}) + {o})'+c[j+1:]
    return c.replace('M2C_UNK','s32')
defs={}; bodies={}
for n in names:
    c=clean(open(f'{D}/{n}.c').read().split('#include "m2c_macros.h"\n',1)[1])
    m=re.search(r'^(\w[\w\s\*]*?\b'+n+r'\([^)]*\)) \{',c,re.M)
    defs[n]=m[1]; bodies[n]=c
protos=[defs[n]+';' for n in names if n not in hdrdecl]
seen|=set(names)
for n in names:
    c=bodies[n]; conflicts={}; aliases={}
    lines=c.split('\n'); body=[]; inbody=False
    for l in lines:
        if not inbody:
            m=re.match(r'^(extern (.*?)\s*\b(\w+)(\[\])?;)',l)
            m2=re.match(r'^(\w[\w\s\*]*?(\w+)\(.*\));\s*/\* extern \*/',l)
            if m:
                sym=m[3]; mt=m[2].strip()
                if sym in hdrdecl and sym not in types: continue
                if sym not in seen:
                    externs.append(m[1]); seen.add(sym); types[sym]=mt
                    if m[4]: arrays.add(sym)
                    continue
                elif m[4] or sym in arrays:
                    if (sym in arrays)!=bool(m[4]) or types.get(sym,'').replace(' ','')!=mt.replace(' ',''):
                        alias=f'{sym}__{"a" if m[4] else "s"}'
                        if alias not in seen:
                            externs.append(f'extern {mt} {alias}{"[]" if m[4] else ""} asm("{sym}");'); seen.add(alias)
                        aliases[sym]=alias
                elif types.get(sym) is not None and types[sym].replace(' ','')!=mt.replace(' ',''): conflicts[sym]=mt
                continue
            if m2:
                if m2[2] not in seen and m2[2] not in hdrdecl: externs.append(m2[1]+';'); seen.add(m2[2])
                continue
            if re.match(r'^\w.*\)\s*\{',l): inbody=True
            else: continue
        body.append(l)
    fn='\n'.join(body).strip()+'\n'
    for sym,t in conflicts.items(): fn=re.sub(r'\b'+sym+r'\b',f'(*({t}*)&{sym})',fn)
    for sym,al in aliases.items(): fn=re.sub(r'\b'+sym+r'\b',al,fn)
    old=f'INCLUDE_ASM("asm/{OV}/nonmatchings/{SEG}", {n});\n'
    assert s.count(old)==1,n
    s=s.replace(old,'\n'+fn+'\n')
i=s.index('#ifndef XENO_PC_PORT\n')+len('#ifndef XENO_PC_PORT\n')
add=externs+protos
if add: s=s[:i]+'\n'.join(add)+'\n\n'+s[i:]
if '#include "system/memory.h"' not in s: s=s.replace('#include "common.h"\n','#include "common.h"\n#include "system/memory.h"\n',1)
open(p,'w').write(s); print(len(names),'inserted;',len(externs),'externs;',len(protos),'protos')
