#!/usr/bin/env python3
"""Classify jump tables per TU from the linker maps of a from-clean build:
 rel = (table addr - TU .rodata start) % 8.  rel==4 -> 'tight' (cannot be produced by a compiler-emitted .align 3 inside that TU)."""
import re,glob,os,collections
R=os.path.expanduser('~/dev/xenogears-decomp-public-harvest')
for mp in sorted(glob.glob(R+'/build/out/*.map')):
    mod=os.path.basename(mp)[:-4]
    if mod.startswith('battle') and mod!='battling': continue
    t=open(mp,errors='ignore').read().split('\n')
    cur=None; tus=[]  # (file,start,size)
    syms=[]
    i=0
    while i<len(t):
        l=t[i]
        m=re.match(r'^ \.rodata\s+0x([0-9a-f]+)\s+0x([0-9a-f]+)\s+(\S+)',l)
        if not m and re.match(r'^ \.rodata\s*$',l) and i+1<len(t):
            m=re.match(r'^\s+0x([0-9a-f]+)\s+0x([0-9a-f]+)\s+(\S+)',t[i+1]); i+=1 if m else 0
        if m and int(m.group(2),16)>0:
            tus.append((m.group(3),int(m.group(1),16),int(m.group(2),16))); cur=len(tus)-1
        else:
            m2=re.match(r'^\s+0x([0-9a-f]+)\s+(jtbl_[0-9A-F]{8})$',l)
            if m2 and cur is not None: syms.append((int(m2.group(1),16),m2.group(2),cur))
        i+=1
    c=collections.Counter()
    for a,n,k in syms:
        f,s,sz=tus[k]; c[(os.path.basename(f),(a-s)%8)]+=1
    print(mod,dict(c))
