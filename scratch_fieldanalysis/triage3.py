import re,collections
exec(open('scratch_fieldanalysis/triage.py').read().split("B=built_all()")[0])
B=built_all()
STORE=('sw','sh','sb','swl','swr','swc1','swc2')
# retail: symbols appearing as the %lo operand of a STORE
def retail_stored_syms(tu,fn):
    s=collections.Counter()
    for l in open(f'asm/field/matchings/{tu}/{fn}.s'):
        m=re.match(r'    /\* \S+ \S+ \S+ \*/\s+(\S+)\s+(.*)',l)
        if not m: continue
        if m.group(1) not in STORE: continue
        for sym in re.findall(r'%lo\(([A-Za-z_]\w*)',m.group(2)): s[sym]+=1
    return s
# built: LO16 reloc sitting on a store instruction
def built_stored_syms(tu,fn):
    s=collections.Counter(); cur=None; inf=False; last_is_store=False
    for line in open('build/field_disasm.txt'):
        m=re.match(r'###OBJ build/src/field/(.+)\.c\.o',line)
        if m: inf=False; cur_tu=m.group(1); continue
        m=re.match(r'^([0-9a-f]{8}) <([\w.]+)>:',line)
        if m: inf=(cur_tu==tu and m.group(2)==fn); continue
        if not inf: continue
        m=re.match(r'^\s+[0-9a-f]+:\t[0-9a-f]{8} \t(\S+)',line)
        if m: last_is_store = m.group(1) in STORE; continue
        m=re.match(r'^\t+\s*[0-9a-f]+: R_MIPS_LO16\t(\S+)',line)
        if m and last_is_store: s[m.group(1).split('+')[0]]+=1
    return s
COHORT=[l.split() for l in open('scratch_fieldanalysis/cohort.txt') if l.strip()]
print(f"{'FUNCTION':34} {'retail-only CALL targets':38} {'globals retail WRITES but C never writes'}")
print('-'*130)
for tu,fn,r,b in COHORT:
    rc,rs,ro=retail_info(tu,fn); bc,bs,bo,_=B[(tu,fn)]
    only_calls=sorted(k for k,v in rc.items() if bc.get(k,0)==0 and not k.startswith('.L'))
    rw=retail_stored_syms(tu,fn); bw=built_stored_syms(tu,fn)
    src=open(f'src/field/{tu}.c').read()
    # a global is 'never written by C' only if the C source never mentions it at all
    missing_w=sorted(k for k in rw if k not in bw and k not in src)
    print(f"{fn:34} {str(only_calls) if only_calls else '-':38} {str(missing_w) if missing_w else '-'}")
