import re,collections
exec(open('scratch_fieldanalysis/triage.py').read().split("B=built_all()")[0])
B=built_all()
STORE=('sw','sh','sb','swl','swr','swc1','swc2')
def retail_stored(tu,fn):
    s=collections.Counter()
    for l in open(f'asm/field/matchings/{tu}/{fn}.s'):
        m=re.match(r'    /\* \S+ \S+ \S+ \*/\s+(\S+)\s+(.*)',l)
        if m and m.group(1) in STORE:
            for sym in re.findall(r'%lo\(([A-Za-z_]\w*)',m.group(2)): s[sym]+=1
    return s
def body(tu,fn):
    src=open(f'src/field/{tu}.c').read().split('\n')
    for i,l in enumerate(src):
        if re.match(r'^[A-Za-z_].*\b'+fn+r'\s*\(',l) and not l.rstrip().endswith(';'): s=i;break
    for j in range(s,len(src)):
        if src[j]=='}': return '\n'.join(src[s:j+1])
    return ''
COHORT=[l.split() for l in open('scratch_fieldanalysis/cohort.txt') if l.strip()]
print("globals retail STORES to, that this function's C BODY never even names:")
bad=0
for tu,fn,r,b in COHORT:
    bd=body(tu,fn); rw=retail_stored(tu,fn)
    miss=sorted(k for k in rw if k not in bd)
    if miss: bad+=1; print(f"  {fn:34} {miss}")
print(f"  -> {bad} of {len(COHORT)} functions have any such symbol")
