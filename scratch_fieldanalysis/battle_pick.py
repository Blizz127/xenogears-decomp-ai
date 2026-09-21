import re,os,collections,subprocess,sys
ROOT='.'
ASM=[f'{ROOT}/asm/battle/nonmatchings',f'{ROOT}/asm/battle/matchings']
JAL=re.compile(r'\bjal\s+([A-Za-z]\w+)')
callers=collections.defaultdict(set)   # callee -> set(caller)
calls=collections.defaultdict(set)     # caller -> set(callee)
for base in ASM:
    if not os.path.isdir(base): continue
    for d,_,fs in os.walk(base):
        for f in fs:
            if not f.endswith('.s'): continue
            name=f[:-2]
            txt=open(os.path.join(d,f),errors='ignore').read()
            for t in JAL.findall(txt):
                calls[name].add(t); callers[t].add(name)
adopted=set(re.findall(r'"(func_\w+)"',open('pc_port/src/battle_overlay_host_leaves.inc').read()))
elig=[l.strip() for l in open(os.environ['TMPDIR']+'/eligible.txt') if l.strip()]
print(f"adopted={len(adopted)} eligible={len(elig)}\n")
print(f"{'CANDIDATE':18} {'#callers':>8} {'#interp callers':>15}  {'insn':>5}  callers (interpreted ones marked *)")
rows=[]
for c in elig:
    cs=sorted(callers.get(c,()))
    interp=[x for x in cs if x not in adopted]
    # retail size
    n=0
    for base in ASM:
        p=f'{base}/'
        for d,_,fs in os.walk(base):
            if c+'.s' in fs:
                n=sum(1 for l in open(os.path.join(d,c+'.s'),errors='ignore') if l.startswith('    /* ')); break
        if n: break
    rows.append((c,len(cs),len(interp),n,cs,interp))
for c,nc,ni,n,cs,interp in sorted(rows,key=lambda r:(-r[2],-r[1])):
    mark=' '.join((x+'*' if x in interp else x) for x in cs[:6])
    print(f"{c:18} {nc:8} {ni:15}  {n:5}  {mark}{' ...' if len(cs)>6 else ''}")
