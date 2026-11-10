import re,sys
# remove repeated guarded XM_T_/XM_S_ blocks (balanced #if nesting)
for f in sys.argv[1:]:
    L=open(f).read().split('\n'); out=[]; seen=set(); i=0; dropped=0
    while i<len(L):
        m=re.match(r'#ifndef (XM_[TS]_\w+)$',L[i])
        if m and i+1<len(L) and L[i+1]=='#define '+m.group(1):
            depth=0; j=i
            while j<len(L):
                if re.match(r'#\s*(if|ifdef|ifndef)\b',L[j]): depth+=1
                elif re.match(r'#\s*endif\b',L[j]):
                    depth-=1
                    if depth==0: break
                j+=1
            if m.group(1) in seen:
                i=j+1; dropped+=1; continue
            seen.add(m.group(1))
        out.append(L[i]); i+=1
    open(f,'w').write('\n'.join(out)); print(f,dropped)
