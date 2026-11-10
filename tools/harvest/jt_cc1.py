#!/usr/bin/env python3
"""cc1 wrapper: gcc-X-psx cc1 plus a jump-table alignment hint pass (serves tools/gcc-X-psx-jt/cc1 for every X).

Retail objects were linked with 4-byte section alignment, so a compiler-emitted
`.align 3` jump table can sit at 4 mod 8 relative to the start of our (single, big)
TU rodata.  C marks the tables of a function with file-scope
    __asm__(".xeno_jt <4|8> <retail table bytes incl. trailing pad>");
(one per table, in table order) just before the function.  This wrapper runs the real
cc1, deletes the marker lines, downgrades the matching `.rdata` + `.align 3` to
`.align 2` for mode 4, and appends `.space` so the table occupies exactly the retail
byte count (retail pad words belong to the table's .s in the baseline).
A TU without markers gets byte-identical compiler output."""
import os,subprocess,sys
here=os.path.dirname(os.path.abspath(sys.argv[0]))
real=os.path.join(os.path.dirname(here),os.path.basename(here)[:-3],"cc1")
args=sys.argv[1:]
r=subprocess.run([real]+args)
if r.returncode!=0: sys.exit(r.returncode)
if "-o" in args:
    out=args[args.index("-o")+1]
    t=open(out).read()
    if ".xeno_jt" in t:
        L=t.split("\n"); res=[]; q=[]; i=0; cur=None; n=0; ren=[]
        while i<len(L):
            l=L[i]
            if l.strip().startswith(".xeno_jt"):
                p=l.split(); q.append((p[1],int(p[2]),p[3] if len(p)>3 else None)); i+=1; continue
            if cur is not None:
                if l.split()[:1]==[".word"]:
                    n+=1; res.append(l); i+=1; continue
                post=cur[1]-4*n
                if post>0: res.append("\t.space\t%d"%post)
                cur=None
            if q and l.strip()==".rdata" and i+1<len(L) and L[i+1].split()[:2]==[".align","3"]:
                cur=q.pop(0); n=0; res.append(l)
                res.append("\t.align\t2" if cur[0]=="4" else L[i+1]); i+=2
                while i<len(L) and L[i].startswith("$L") and L[i].rstrip().endswith(":"):
                    if cur[2]:
                        ren.append((L[i].rstrip()[:-1],cur[2])); res.append("\t.globl\t"+cur[2]); res.append(cur[2]+":")
                    else: res.append(L[i])
                    i+=1
                continue
            res.append(l); i+=1
        txt="\n".join(res)
        import re
        for a_,b_ in ren: txt=re.sub(re.escape(a_)+r"(?![0-9A-Za-z_$])",b_,txt)
        open(out,"w").write(txt)
sys.exit(0)
