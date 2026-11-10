import subprocess,re,glob,sys,os
os.chdir("/home/blizz/dev/xenogears-decomp-public-harvest")
obj=sys.argv[1]; mod=sys.argv[2]
out=subprocess.run("mips-linux-gnu-objdump -dr %s"%obj,shell=True,capture_output=True,text=True,env=dict(os.environ,PATH=os.environ["PATH"]+":/home/blizz/dev/xenogears-decomp-public-harvest/tools/binutils")).stdout
cur=None; bad=0; tot=0
for l in out.split("\n"):
    m=re.match(r'^[0-9a-f]{8} <(func_\w+)>:',l)
    if m: cur=m.group(1); continue
    m=re.search(r'R_MIPS_HI16\s+(jtbl_\w+)',l)
    if m and cur:
        tot+=1
        fs=glob.glob('asm/%s/*matchings/**/%s.s'%(mod,cur),recursive=True)
        if not fs: print("noasm",cur); continue
        if "dlabel "+m.group(1)+"\n" not in open(fs[0]).read(): print("BAD",cur,m.group(1)); bad+=1
print(tot,"bad",bad)
