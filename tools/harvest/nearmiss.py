import re,sys,glob,os
W=os.path.expanduser('~/scratch/xeno-mit-harvest')
rows=[]
for f in sorted(glob.glob(W+'/diag_*.log')):
    src,mod=re.match(r'diag_(xem|ea)_(.*)\.log',os.path.basename(f)).groups()
    t=open(f,errors='ignore').read()
    for m in re.finditer(r'=== DIAG (\w+)\n(.*?)(?=\n=== DIAG|\nSECDIFF|\n\{|\Z)',t,re.S):
        body=m.group(2).split('\n')
        d=[l for l in body if (l.startswith('+') or l.startswith('-')) and not l.startswith('+++') and not l.startswith('---')]
        ctx=[l for l in body if l.startswith(' ')]
        rows.append((len(d),len(ctx)+len(d),m.group(1),src,mod))
rows.sort()
seen=set()
for r in rows:
    if r[2] in seen: continue
    seen.add(r[2]); print(*r)
