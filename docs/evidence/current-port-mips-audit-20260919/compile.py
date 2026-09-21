from pathlib import Path
from concurrent.futures import ThreadPoolExecutor,as_completed
import subprocess,json,time
out=Path('scratchpad/astra-port-audit-20260919');jobs=json.loads((out/'jobs.json').read_text())
def run(j):
 log=Path(j['object']+'.log')
 with log.open('w') as f: p=subprocess.run(['bash','-e','-c',j['commands']],stdout=f,stderr=subprocess.STDOUT)
 return dict(j,returncode=p.returncode)
results=[]
with ThreadPoolExecutor(max_workers=4) as pool:
 for future in as_completed([pool.submit(run,j) for j in jobs]):
  r=future.result();results.append(r)
  if r['returncode'] or len(results)%25==0: print(len(results),r['source'],'rc',r['returncode'],flush=True)
(out/'compiled.json').write_text(json.dumps(results,indent=2))
print('COMPILED',len(results),'FAILED',sum(r['returncode']!=0 for r in results),flush=True)
