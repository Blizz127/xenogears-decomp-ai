import re,glob,os,subprocess,sys
os.chdir("/home/blizz/dev/xenogears-decomp-public-harvest")
mod,tu,srcf=sys.argv[1:4]; names=sys.argv[4:]
mp=open('build/out/%s.map'%mod,errors='ignore').read().split('\n')
base=None
for i,l in enumerate(mp):
    m=re.match(r'^ \.rodata\s+0x([0-9a-f]+)\s+0x([0-9a-f]+)\s+(\S+)$',l)
    if not m and re.match(r'^ \.rodata\s*$',l) and i+1<len(mp): m=re.match(r'^\s+0x([0-9a-f]+)\s+0x([0-9a-f]+)\s+(\S+)$',mp[i+1])
    if m and m.group(3)=='build/%s.o'%tu: base=int(m.group(1),16); break
print("base",hex(base))
L=open(srcf).read().split("\n")
for nm in names:
    t=open(glob.glob("asm/%s/*matchings/**/%s.s"%(mod,nm),recursive=True)[0]).read()
    tabs=sorted((int(m.group(1),16),4*len(re.findall(r"\.word",m.group(2)))) for m in re.finditer(r"dlabel jtbl_([0-9A-Fa-f]{8})\n(.*?)\.size jtbl_",t,re.S))
    mk=['__asm__(".xeno_jt %s %d jtbl_%08X");'%('4' if (a-base)%8==4 else '8',n,a) for a,n in tabs]
    # find definition line: a line starting with a type and containing nm( without ';' and with '{'
    idx=[i for i,l in enumerate(L) if re.search(r'\b%s\s*\('%nm,l) and l.rstrip().endswith('{') and not l.startswith(' ')]
    if len(idx)!=1: print("def?",nm,idx); continue
    i=idx[0]
    # skip if markers already precede
    j=i-1
    while j>=0 and '.xeno_jt' in L[j]: j-=1
    if j<i-1: print("has markers",nm); continue
    L[i:i]=mk; print(nm,mk)
open(srcf,"w").write("\n".join(L))
