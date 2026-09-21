import re,sys
COHORT=[l.split() for l in open('scratch_fieldanalysis/cohort.txt') if l.strip()]
for tu,fn,r,b in COHORT:
    p=f'src/field/{tu}.c'
    src=open(p).read().split('\n')
    start=None
    for i,l in enumerate(src):
        if re.match(r'^[A-Za-z_].*\b'+fn+r'\s*\(', l) and not l.rstrip().endswith(';'):
            start=i; break
    if start is None: print(f"{fn}: NOT FOUND in {p}"); continue
    end=start
    for j in range(start,len(src)):
        if src[j]=='}': end=j; break
    body='\n'.join(src[start:end+1])
    print(f"{fn:34} {p:42} lines={end-start+1:4} retail={r} built={b}")
