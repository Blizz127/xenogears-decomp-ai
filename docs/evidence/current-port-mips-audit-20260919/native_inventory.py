"""Read current linked native ownership with nm and DWARF; run from repository root."""
from pathlib import Path
import subprocess, re, json, collections
out=Path('scratchpad/astra-port-audit-20260919')
stubs=re.findall(r'^long (\w+)\(void\) \{ xeno_port_stub',Path('pc_port/build_native/stubs.c').read_text(),re.M)
objects=collections.defaultdict(list)
for obj in Path('pc_port/build_native/obj').glob('*.o'):
    text=subprocess.check_output(['nm','--defined-only',str(obj)],text=True,stderr=subprocess.DEVNULL)
    for line in text.splitlines():
        parts=line.split()
        if len(parts)==3 and parts[1] in ('T','W','t'):
            objects[parts[2]].append(dict(object=obj.name,binding=parts[1]))
text=subprocess.check_output(['nm','--defined-only','pc_port/build_native/xeno-port'],text=True)
entries=[line.split() for line in text.splitlines() if len(line.split())==3 and line.split()[1] in ('T','W','t')]
locations=subprocess.run(['addr2line','-e','pc_port/build_native/xeno-port'],input='\n'.join(e[0] for e in entries)+'\n',text=True,capture_output=True,check=True).stdout.splitlines()
assert len(entries)==len(locations)
symbols={name:dict(binding=binding,candidate_objects=objects.get(name,[]),linked_source_location=location) for (address,binding,name),location in zip(entries,locations)}
# Preserve duplicate static names as well as the convenience name index.
all_entries=[dict(address=a,binding=b,function=n,linked_source_location=l) for (a,b,n),l in zip(entries,locations)]
(out/'native-symbols.json').write_text(json.dumps(dict(stubs=stubs,symbols=symbols,linked_entries=all_entries),indent=2)+'\n')
