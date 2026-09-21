import re,collections
exec(open('scratch_fieldanalysis/triage.py').read().split("B=built_all()")[0])
B=built_all()
STORE={'sw','sh','sb','swl','swr','swc1','sdc1','swc2'}
LOAD={'lw','lh','lhu','lb','lbu','lwl','lwr','lwc1','ldc1','lwc2'}
BR=lambda o:{k:v for k,v in o.items() if k[0]=='b' and k!='break'}
hdr=f"{'FUNCTION':34}{'delta':>7} | {'calls':>11} {'stores':>11} {'branch':>11} {'loads':>11} {'lui':>11}"
print(hdr); print('-'*len(hdr))
rows=[]
for tu,fn,r,b in COHORT:
    rc,rs,ro=retail_info(tu,fn)
    bc,bs,bo,_=B[(tu,fn)]
    def pair(rv,bv): return f"{rv}->{bv}"
    rcall=sum(rc.values()); bcall=sum(bc.values())
    rst=sum(ro[k] for k in STORE); bst=sum(bo[k] for k in STORE)
    rld=sum(ro[k] for k in LOAD);  bld=sum(bo[k] for k in LOAD)
    rbr=sum(BR(ro).values()); bbr=sum(BR(bo).values())
    rlui=ro['lui']; blui=bo['lui']
    rows.append((tu,fn,int(b)-int(r),rcall,bcall,rst,bst,rbr,bbr,rld,bld,rlui,blui))
    print(f"{fn:34}{int(b)-int(r):7} | {pair(rcall,bcall):>11} {pair(rst,bst):>11} {pair(rbr,bbr):>11} {pair(rld,bld):>11} {pair(rlui,blui):>11}")
print()
print("Instruction deficit accounted for by lui/addiu address re-materialization alone:")
for tu,fn,d,rc_,bc_,rst,bst,rbr,bbr,rld,bld,rlui,blui in rows:
    insn_def=-d//4
    lui_def=rlui-blui
    print(f"  {fn:34} insn deficit {insn_def:5}   lui deficit {lui_def:5} (x2 for lui+addiu = {lui_def*2:5})  store delta {bst-rst:+4}  call delta {bc_-rc_:+3}  branch delta {bbr-rbr:+3}")
