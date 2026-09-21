import os,re,collections

# --- retail: per (tu, func) list of instruction words (hex, little-endian in .s comment)
retail={}
for root,_,files in os.walk('asm/field/matchings'):
    tu=os.path.relpath(root,'asm/field/matchings')
    for f in files:
        if not f.endswith('.s'): continue
        ws=[]
        for l in open(os.path.join(root,f)):
            m=re.match(r'    /\* [0-9A-Fa-f]+ [0-9A-Fa-f]+ ([0-9A-Fa-f]{8}) \*/',l)
            if m:
                b=m.group(1)  # bytes in memory order; convert to word
                ws.append((b[6:8]+b[4:6]+b[2:4]+b[0:2]).lower())
        retail[(tu,f[:-2])]=ws

# --- built: parse objdump
built={}; relocs=collections.defaultdict(set)
cur_tu=None; cur_fn=None; base=0
for line in open('build/field_disasm.txt'):
    m=re.match(r'###OBJ build/src/field/(.+)\.c\.o',line)
    if m: cur_tu=m.group(1); cur_fn=None; continue
    m=re.match(r'^([0-9a-f]{8}) <([\w.]+)>:',line)
    if m:
        cur_fn=(cur_tu,m.group(2)); base=int(m.group(1),16); built[cur_fn]=[]; continue
    m=re.match(r'^\s+([0-9a-f]+):\t([0-9a-f]{8}) ',line)
    if m and cur_fn:
        built[cur_fn].append(m.group(2)); continue
    m=re.match(r'^\t+\s*([0-9a-f]+): (R_MIPS_\S+)',line)
    if m and cur_fn:
        relocs[cur_fn].add((int(m.group(1),16)-base)//4)

exact=0; size_ok_byte_bad=0; size_bad=0; missing=0
bad=[]
for k,rw in retail.items():
    bw=built.get(k)
    if bw is None: missing+=1; continue
    if len(bw)!=len(rw):
        size_bad+=1; bad.append((k,'size',len(rw)*4,len(bw)*4,None)); continue
    rel=relocs[k]
    diff=[i for i in range(len(rw)) if i not in rel and rw[i]!=bw[i]]
    if not diff: exact+=1
    else:
        size_ok_byte_bad+=1
        bad.append((k,'bytes',len(rw)*4,len(bw)*4,len(diff)))

tot=len(retail)
print(f"retail functions in field C TUs : {tot}")
print(f"  byte-exact (mod relocations)  : {exact}")
print(f"  right size, wrong instructions: {size_ok_byte_bad}")
print(f"  wrong size                    : {size_bad}")
print(f"  not found in build            : {missing}")
print(f"  TOTAL DIVERGENT               : {size_ok_byte_bad+size_bad}  ({100*(size_ok_byte_bad+size_bad)/tot:.2f}%)")
print(f"  match rate by function        : {100*exact/tot:.2f}%")
bt=collections.Counter(k[0] for k,_,_,_,_ in bad)
print("\ndivergent functions per TU:")
for tu,c in sorted(bt.items(), key=lambda x:-x[1]):
    tot_tu=sum(1 for k in retail if k[0]==tu)
    print(f"  {tu:32} {c:4} / {tot_tu:4}")

rb=sum(len(v)*4 for v in retail.values())
ok=sum(len(retail[k])*4 for k in retail if k not in [b[0] for b in bad])
print(f"\nbyte-weighted: {ok} / {rb} retail text bytes in byte-exact functions = {100*ok/rb:.2f}%")
print(f"total retail text {rb}, total built text {sum(len(v)*4 for v in built.values())}, deficit {sum(len(v)*4 for v in built.values())-rb}")
with open('docs/evidence/field-codegen-20260921/divergent_functions.txt','w') as fh:
    fh.write("# field.bin: functions whose built code differs from retail\n")
    fh.write("# kind=size -> instruction count differs; kind=bytes -> same size, different instructions\n")
    fh.write("# relocated words are masked, so 'bytes' divergence is real codegen, not link-time fixups\n")
    fh.write(f"# {len(bad)} divergent of {len(retail)} total\n")
    fh.write(f"{'TU':28} {'FUNCTION':44} {'KIND':6} {'RETAIL':>7} {'BUILT':>7} {'BADWORDS':>9}\n")
    for (tu,fn),kind,r,b,nd in sorted(bad,key=lambda x:(x[0][0],x[0][1])):
        fh.write(f"{tu:28} {fn:44} {kind:6} {r:7} {b:7} {'' if nd is None else nd:>9}\n")
print("wrote docs/evidence/field-codegen-20260921/divergent_functions.txt")
