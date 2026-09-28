#!/usr/bin/env python3
"""m2c_insert.py [--inner] OV SEG names...

Replace `INCLUDE_ASM("asm/OV/nonmatchings/SEG", name);` lines (or the
`"../asm/..."` spelling) in src/OV/SEG.c
with the cleaned m2c_try.py drafts: M2C_FIELD becomes a raw-offset access,
M2C_UNK becomes s32, the drafts' externs/prototypes are merged at the top of
a retail-only block after the file's first #include run (skipping anything the file's
headers already declare, and using each inserted definition's own signature
as its prototype), and an extern whose type conflicts with one already in
scope is read through `(*(T*)&sym)`.  Every inserted body is wrapped in
#ifndef XENO_PC_PORT, so port-compiled TUs (e.g. battle host TUs) keep
their port view unchanged.  Rebuild and compare afterwards.

--inner: for a site shaped `#ifndef XENO_PC_PORT / INCLUDE_ASM / #else /
<port definition> / #endif`, fold the draft into that definition as its
retail arm (one signature, `#ifdef XENO_PC_PORT <port body> #else <draft
body> #endif`) instead of adding a second definition.  Twin definitions break
port tests that extract a function's source with awk.  A site whose port
signature differs from the draft's is skipped (left INCLUDE_ASM) with a note;
other sites fall back to the plain replacement."""
import sys,os,re,subprocess
sys.path.insert(0, os.path.dirname(__file__))
from assist_common import CACHE, REPO
os.chdir(REPO)
INNER='--inner' in sys.argv[1:]
argv=[a for a in sys.argv[1:] if a!='--inner']
OV,SEG=argv[0],argv[1]; names=argv[2:]
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
# declarations visible in this TU: the file itself plus any #include "x.c"
# (BATTLE_SUB/FIELD_SUB run wrappers include the TU that holds them)
decl_src=s
for inc in re.findall(r'^#include "([\w./]+\.c)"',s,re.M):
    ip=os.path.join(os.path.dirname(p),inc)
    if os.path.exists(ip): decl_src+='\n'+open(ip).read()
for m in re.finditer(r'^extern ([^;(]*?)\s*\b(\w+)(\[\])?;',decl_src,re.M):
    types[m[2]]=m[1]
    if m[3]: arrays.add(m[2])
seen=set(types)|set(re.findall(r'^extern [^;]*?\b(\w+)(?:\[\])?\s+asm\(',decl_src,re.M))|set(re.findall(r'^(?:extern\s+)?\w[\w\s\*]*?(\w+)\([^;{]*\);\s*$',decl_src,re.M))|set(re.findall(r'^\w[\w\s\*]*?\b(\w+)\([^;]*\)\s*\{',decl_src,re.M))
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
def fold_inner(s,old,fn,n):
    """--inner: merge draft fn into the site's existing port definition.
    Returns the new text, None when the site is not an #ifndef/#else port
    site, or 'skip' when the signatures differ."""
    site='#ifndef XENO_PC_PORT\n'+old+'#else\n'
    if s.count(site)!=1: return None
    a=s.index(site); L=s[a+len(site):].split('\n'); d=0
    for j,l in enumerate(L):
        t=l.strip()
        if re.match(r'#\s*if',t): d+=1
        elif re.match(r'#\s*endif',t):
            if d==0: break
            d-=1
    port=L[:j]; rest='\n'.join(L[j+1:])
    sig=re.compile(r'^\w[\w\s\*]*?\b'+n+r'\s*\([^;{]*\)\s*\{\s*$')
    k=[i for i,l in enumerate(port) if sig.match(l)]
    rl=fn.rstrip('\n').split('\n'); rk=[i for i,l in enumerate(rl) if sig.match(l)]
    if not k or not rk or port[-1]!='}' or rl[-1]!='}': return None
    norm=lambda x: re.sub(r'\s','',x)
    if norm(port[k[0]])!=norm(rl[rk[0]]):
        print(f'{n}: port signature differs from the draft; left INCLUDE_ASM'); return 'skip'
    body=port[:k[0]]+rl[:rk[0]]+[port[k[0]],'#ifdef XENO_PC_PORT']+port[k[0]+1:-1]+['#else']+rl[rk[0]+1:-1]+['#endif','}']
    return s[:a]+'\n'.join(body)+'\n'+rest
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
                if sym in hdrdecl and sym not in types:
                    if m[4]:
                        # header-declared scalar read as an array: alias it
                        alias=f'{sym}__a_'+re.sub(r'\W','',mt)
                        if alias not in seen:
                            externs.append(f'extern {mt} {alias}[] asm("{sym}");'); seen.add(alias)
                        aliases[sym]=alias
                    continue
                if sym not in seen:
                    externs.append(m[1]); seen.add(sym); types[sym]=mt
                    if m[4]: arrays.add(sym)
                    continue
                elif m[4] or sym in arrays:
                    if (sym in arrays)!=bool(m[4]) or types.get(sym,'').replace(' ','')!=mt.replace(' ',''):
                        alias=f'{sym}__{"a" if m[4] else "s"}_'+re.sub(r'\W','',mt)
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
    old=[x for x in (f'INCLUDE_ASM("asm/{OV}/nonmatchings/{SEG}", {n});\n',
                     f'INCLUDE_ASM("../asm/{OV}/nonmatchings/{SEG}", {n});\n') if s.count(x)==1]
    assert old,n
    old=old[0]
    folded=fold_inner(s,old,fn,n) if INNER else None
    if folded=='skip': continue
    s=folded if folded is not None else s.replace(old,'\n#ifndef XENO_PC_PORT\n'+fn+'#endif\n\n')
# externs/prototypes: a retail-only block after the file's first run of
# #include lines, so every BATTLE_SUB/FIELD_SUB run of the TU sees them.
lines=s.split('\n'); k=next((i for i,l in enumerate(lines) if l.startswith('#include')),0)
while k<len(lines) and (lines[k].startswith('#include') or not lines[k].strip()): k+=1
add=externs+protos
if add:
    lines[k:k]=['#ifndef XENO_PC_PORT']+add+['#endif','']
    s='\n'.join(lines)
if '#include "system/memory.h"' not in s:  # retail-only: never change the port's view
    s=s.replace('#include "common.h"\n','#include "common.h"\n#ifndef XENO_PC_PORT\n#include "system/memory.h"\n#endif\n',1)
open(p,'w').write(s); print(len(names),'inserted;',len(externs),'externs;',len(protos),'protos')
