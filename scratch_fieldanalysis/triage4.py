import re,collections
exec(open('scratch_fieldanalysis/triage.py').read().split("B=built_all()")[0])
B=built_all()
COHORT=[l.split() for l in open('scratch_fieldanalysis/cohort.txt') if l.strip()]
print("call-count shortfalls (retail calls it MORE often than the C does):")
any_=False
for tu,fn,r,b in COHORT:
    rc,rs,ro=retail_info(tu,fn); bc,bs,bo,_=B[(tu,fn)]
    short={k:(v,bc.get(k,0)) for k,v in rc.items() if bc.get(k,0)<v and not k.startswith('.L')}
    if short:
        any_=True
        print(f"  {fn:34} {', '.join(f'{k}: retail x{a} vs C x{c}' for k,(a,c) in short.items())}")
if not any_: print("  (none)")
