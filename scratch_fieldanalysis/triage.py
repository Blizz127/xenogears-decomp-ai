import os,re,collections,sys

COHORT=[l.split() for l in """
dialogue/text_box_render func_8007E1C0 3148 1956
main/misc5 func_800A84C0 1764 664
scripts/virtual_machine func_800A28D4 1772 1196
main/misc2 func_80076AC0 1776 1232
main/misc4 func_8007C694 1736 1276
effects/distortion FieldDistortionDraw 2060 1612
main/misc4 func_8007BEF4 1948 1504
dialogue/text_box_render func_8007F8DC 1804 1396
main/misc4 func_8007CD80 1652 1252
main/misc2 func_800748E8 2340 1956
dialogue/text_box_render FieldTextBoxInitializePrimitives 1952 1584
dialogue/text_box_render func_8008004C 1448 1176
main/misc8 func_80082620 1432 1172
main/misc2 func_80075B44 2416 2164
main/misc4 func_8007B1C4 692 464
main/misc func_8008B5D4 704 480
main/misc2 func_80077268 732 516
scripts/virtual_machine func_800A3474 2072 1864
""".strip().split("\n")]

def retail_info(tu,fn):
    p=f'asm/field/matchings/{tu}/{fn}.s'
    calls=collections.Counter(); syms=collections.Counter(); ops=collections.Counter()
    for l in open(p):
        m=re.match(r'    /\* \S+ \S+ \S+ \*/\s+(\S+)\s*(.*)',l)
        if not m: continue
        op,rest=m.group(1),m.group(2).strip()
        ops[op]+=1
        if op in ('jal','j'):
            t=rest.split()[0] if rest else ''
            if not t.startswith('.'): calls[t]+=1
        for s in re.findall(r'%(?:hi|lo|gp_rel)\(([A-Za-z_]\w*)',rest): syms[s]+=1
    return calls,syms,ops

def built_all():
    res={}
    cur_tu=cur=None
    for line in open('build/field_disasm.txt'):
        m=re.match(r'###OBJ build/src/field/(.+)\.c\.o',line)
        if m: cur_tu=m.group(1); cur=None; continue
        m=re.match(r'^([0-9a-f]{8}) <([\w.]+)>:',line)
        if m: cur=(cur_tu,m.group(2)); res[cur]=[collections.Counter(),collections.Counter(),collections.Counter(),None]; continue
        if cur is None: continue
        m=re.match(r'^\s+[0-9a-f]+:\t[0-9a-f]{8} \t(\S+)\s*(.*)',line)
        if m: res[cur][2][m.group(1)]+=1; res[cur][3]=m.group(1); continue
        m=re.match(r'^\t+\s*[0-9a-f]+: (R_MIPS_\S+)\t(\S+)',line)
        if m:
            k,t=m.group(1),m.group(2)
            if k=='R_MIPS_26': res[cur][0][t]+=1
            elif k in ('R_MIPS_HI16','R_MIPS_GPREL16'): res[cur][1][t]+=1
    return res

B=built_all()
for tu,fn,r,b in COHORT:
    rc,rs,ro=retail_info(tu,fn)
    bc,bs,bo,_=B[(tu,fn)]
    only_r={k:v for k,v in rc.items() if bc.get(k,0)<v}
    only_b={k:v for k,v in bc.items() if rc.get(k,0)<v}
    sr={k:v for k,v in rs.items() if bs.get(k,0)<v}
    sb={k:v for k,v in bs.items() if rs.get(k,0)<v}
    rbr=sum(v for k,v in ro.items() if k.startswith(('b','j')) )
    bbr=sum(v for k,v in bo.items() if k.startswith(('b','j')))
    print(f"\n=== {tu}/{fn}  retail={r} built={b} delta={int(b)-int(r)}")
    print(f"    branches/jumps retail={rbr} built={bbr}   loads retail={ro['lw']+ro['lh']+ro['lhu']+ro['lb']+ro['lbu']} built={bo['lw']+bo['lh']+bo['lhu']+bo['lb']+bo['lbu']}")
    print(f"    CALLS retail-only : {dict(sorted(only_r.items(),key=lambda x:-x[1]))}")
    print(f"    CALLS built-only  : {dict(sorted(only_b.items(),key=lambda x:-x[1]))}")
    print(f"    SYMS  retail-only : {dict(sorted(sr.items(),key=lambda x:-x[1])[:12])}")
    print(f"    SYMS  built-only  : {dict(sorted(sb.items(),key=lambda x:-x[1])[:12])}")
