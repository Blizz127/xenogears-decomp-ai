import re,sys
pat=re.compile(r'^#ifndef (XM_[TS]_\w+)\n#define \1\n.*?\n#endif\n',re.M|re.S)
for f in sys.argv[1:]:
    t=open(f).read(); seen=set(); out=[]; last=0; dropped=0
    for m in pat.finditer(t):
        # only consider blocks after first adoption comment
        if m.group(1) in seen:
            out.append(t[last:m.start()]); last=m.end(); dropped+=1
        else: seen.add(m.group(1))
    out.append(t[last:]); open(f,'w').write(''.join(out)); print(f,dropped)
