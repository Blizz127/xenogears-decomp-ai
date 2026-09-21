"""Run from the repository root; prepare isolated MIPS builds using current Ninja flags."""
from pathlib import Path
import json, subprocess
out = Path('scratchpad/astra-port-audit-20260919')
out.mkdir(parents=True, exist_ok=True)
jobs = []
for source in sorted(Path('src').rglob('*.c')):
    target = 'build/' + str(source) + '.o'
    commands = subprocess.check_output(['ninja', '-t', 'commands', target], text=True)
    destination = out / 'mips' / source
    destination.parent.mkdir(parents=True, exist_ok=True)
    jobs.append(dict(source=str(source), object=str(destination)+'.o',
        commands=commands.replace('build/'+str(source), str(destination)),
        native_object='pc_port/build_native/obj/'+str(source).replace('/', '_')+'.o'))
(out/'jobs.json').write_text(json.dumps(jobs, indent=2)+'\n')
print('Prepared', len(jobs), 'translation units')
