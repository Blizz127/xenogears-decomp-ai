#!/usr/bin/env python3
"""vtry.py NAME BLKFILE MODULE 'old1=>new1' ['old2=>new2' ...]   (each arg is a variant; use '&&' to chain several replacements in one variant)
Tries each variant via adopt.py --hand; stops at first byte-exact (leaves it applied)."""
import sys,subprocess,os,re
name,blk,mod=sys.argv[1:4]; vs=sys.argv[4:]
W=os.path.expanduser('~/scratch/xeno-mit-harvest'); R=os.path.expanduser('~/dev/xenogears-decomp-public-harvest')
t=open(blk).read()
t=re.sub(r'^#ifndef XENO_PC_PORT\n','',t,count=1); t=re.sub(r'#endif\s*$','',t.rstrip()+'\n')
for i,v in enumerate(vs):
    u=t
    for part in v.split('&&'):
        o,n=part.split('=>'); 
        if o not in u: print('variant',i,'pattern not found:',o); u=None; break
        u=u.replace(o,n)
    if u is None: continue
    open(W+'/vt.hand','w').write('//// %s\n%s'%(name,u))
    p=subprocess.run('cd %s; . ./.harvest_env.sh; nice -n 10 python3 %s/adopt.py %s --hand %s/vt.hand --pos --max-insns 400 --diag 2>&1 | tail -25'%(R,W,mod,W),shell=True,capture_output=True,text=True)
    out=p.stdout
    if "'ADOPTED'" in out: print('MATCH variant',i,v); sys.exit(0)
    d=[x for x in re.findall(r'^[+-][^+-].*',out,re.M) if not re.match(r'.[+-]?R_MIPS',x)]
    print('variant',i,'diff lines',len(d)); 
    for l in d[:6]: print('   ',l[:100])
print('no match')
