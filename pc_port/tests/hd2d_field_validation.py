#!/usr/bin/env python3
"""Field14 no-lane/OFF/ON evidence gate. Run only on the authorized headless runner.

Relinks completed build inputs (no full build). OFF requires matching loaded
checkpoint screenshots and render input-key hashes. ON cannot pass without
accepted production sprite parts and unchanged retail packets. Evidence is
private; no fabricated profile is injected. A BLOCKED ON exits 2.
"""
import argparse
import hashlib
import json
import os
import shlex
from pathlib import Path
import subprocess
import threading
import time

ROOT = Path(__file__).resolve().parents[2]
HOOKS = ['PcPort_Hd2dPostProcess', 'PcPort_Hd2dSpriteBegin', 'PcPort_FeiHd2dCapture', 'PcPort_Hd2dSpriteDraw']

def run(cmd, **kw):
    return subprocess.run(cmd, check=True, cwd=ROOT, **kw)


def drive(binary, qs, out, enabled, timeout, trace=False):
    out.mkdir()
    cap = out / 'cap'
    cap.mkdir()
    env = {k:v for k,v in os.environ.items() if not k.startswith(('XENO_', 'DISPLAY', 'WAYLAND_DISPLAY', 'XAUTHORITY'))}
    env.update(SDL_VIDEODRIVER='offscreen', SDL_AUDIODRIVER='dummy', ALSOFT_DRIVERS='null',
               XENO_CHEATS_CONSOLE='1', XENO_QUICKSAVE_PATH=str(qs),
               XENO_FIELD_CAPTURE_DIR=str(cap), XENO_FIELD_CAPTURE_EVERY='8',
               XENO_HD2D_BATCH_STATS='1', XENO_HD2D_TRACE_PARTS=str(int(trace)), XENO_HD2D_OCTOPATH=str(enabled), XENO_HD2D_OPENING_MAPS='14',
               XENO_FEI_HD2D='0', XENO_PRESENT_HASH_LOG=str(out/'hash.txt'),
               XENO_HD2D_FIELD_PROBE=str(out/'probe.txt'))
    lines=[]
    with (out/'run.log').open('w') as log:
        proc=subprocess.Popen([str(binary)], cwd=ROOT, env=env, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        def reader():
            for raw in proc.stdout:
                line=raw.decode('latin1'); lines.append(line); log.write(line); log.flush()
        thread=threading.Thread(target=reader); thread.start()
        start=time.monotonic(); sent=0; restored=None; restore_present=None; selected=[]
        try:
            while time.monotonic()-start < timeout and proc.poll() is None:
                text=''.join(lines)
                if restored is None and 'player position restored' in text:
                    restored=time.monotonic()
                    # The first capture boundary following observed restoration
                    # bounds the restore present conservatively by <=8 frames.
                    existing=sorted(cap.glob('field-frame-*.png'))
                    restore_present=int(existing[-1].stem.rsplit('-',1)[1])+8 if existing else 8
                if restored is None and 'console reading commands' in text and 'FieldLoad begin' in text and time.monotonic()-sent>3:
                    proc.stdin.write(b'load\n'); proc.stdin.flush(); sent=time.monotonic()
                if restored:
                    eligible=[p for p in sorted(cap.glob('field-frame-*.png'))
                              if int(p.stem.rsplit('-',1)[1]) >= restore_present+120]
                    if len(eligible)>=24:
                        selected=eligible[:24]
                        last=int(selected[-1].stem.rsplit('-',1)[1])-1
                        # Hash FILE is buffered. Continue presenting until the
                        # entire fixed measurement window is on disk.
                        h=out/'hash.txt'
                        raw=h.read_text() if h.exists() else ''
                        complete=raw[:raw.rfind('\n')].splitlines()
                        if complete and int(complete[-1].split()[0])>=last:
                            break
                time.sleep(.25)
            else:
                raise RuntimeError(f'{out.name}: checkpoint capture timeout/exit')
        finally:
            proc.terminate()
            try: proc.wait(10)
            except subprocess.TimeoutExpired: proc.kill(); proc.wait()
            thread.join()
    # Fixed count-based window, chosen before any pixel/hash comparison.
    (out/'window.json').write_text(json.dumps({'restore_present_upper_bound':restore_present,
        'settle_presents':120,'capture_count':24,'capture_stride':8},indent=2))
    if len(selected)<24: raise RuntimeError('insufficient field captures')
    manifest={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in selected}
    (out/'captures.json').write_text(json.dumps(manifest,indent=2))
    lo=int(selected[0].stem.rsplit('-',1)[1]); hi=int(selected[-1].stem.rsplit('-',1)[1])
    # Present hash index is zero based, field capture counter is one based.
    rows=(out/'hash.txt').read_text().splitlines()
    (out/'field-hash.txt').write_text('\n'.join(r for r in rows if lo<=int(r.split()[0])+1<=hi)+'\n')
    return manifest


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--out', type=Path, required=True)
    p.add_argument('--checkpoint', type=Path, default=Path.home()/'release-staging/xeno-work/r6-nogamedata/field14.xgqs')
    p.add_argument('--timeout',type=float,default=420)
    a=p.parse_args(); a.out=a.out.resolve(); a.out.mkdir(parents=True,exist_ok=False)
    build=ROOT/'pc_port/build_native'
    args=(build/'xeno-port.link-args').read_text().splitlines()
    flags=shlex.split((build/'port-source.cflags').read_text())
    run(['gcc','-c','pc_port/tests/hd2d_field_probe.c',*flags,'-o',str(a.out/'probe.o')])
    baseline=a.out/'no-lane'; lane=a.out/'lane'
    run(['gcc',*[x for x in args if '/obj/hd2d__' not in x],'-o',str(baseline)])
    run(['gcc',str(a.out/'probe.o'),*args,*['-Wl,--wrap='+h for h in HOOKS],'-o',str(lane)])
    for binary, expected in ((baseline,False),(lane,True)):
        symbols=run(['nm','--defined-only',str(binary)],capture_output=True,text=True).stdout
        for hook in HOOKS[:2]:
            strong=any(line.split()[-2:]==['T',hook] for line in symbols.splitlines())
            if strong!=expected: raise RuntimeError(f'wrong strong hook linkage: {binary} {hook}')
    manifests={}
    for name,binary,on in [('A',baseline,0),('B',lane,0),('ON',lane,1),('TRACE',lane,1)]:
        manifests[name]=drive(binary,a.checkpoint.resolve(),a.out/name,on,a.timeout,trace=name=='TRACE')
    result=subprocess.run(['python3','pc_port/tests/hd2d_field_sequence_compare.py',str(a.out/'A'),str(a.out/'B')],cwd=ROOT,capture_output=True,text=True)
    (a.out/'off-sequence.json').write_text(result.stdout)
    pairs=[(x,y) for x,h in manifests['A'].items() for y,k in manifests['B'].items() if h==k]
    offprobe=(a.out/'B/probe.txt').read_text()
    off_ok=result.returncode==0 and bool(pairs) and 'post ' in offprobe and 'begin ' in offprobe
    (a.out/'paired-captures.json').write_text(json.dumps(pairs,indent=2))
    on=(a.out/'ON/probe.txt').read_text().splitlines()
    facts={}
    trace=(a.out/'TRACE/probe.txt').read_text().splitlines()
    for line in trace:
        if not line.startswith('part '): continue
        row=dict(item.split('=',1) for item in line.split()[1:])
        if row['map']!='14' or row['actor']!=row['player']: continue
        key=tuple(row[k] for k in ('map','actor','fnv','frame','part','uv','tpage','clut'))
        facts[key]={'source_run':'TRACE', **{k:row[k] for k in ('map','actor','player','fnv','frame','part','uv','tpage','clut','quad','ot')}}
    (a.out/'observed-player-parts.json').write_text(json.dumps(list(facts.values()),indent=2))
    parts=[line for line in on if line.startswith('part ') and 'accepted=1' in line]
    if any('packet_exact=0' in line for line in parts): raise RuntimeError('ON changed retail packet geometry/UV/colour')
    verdict='BLOCKED: no production sprite parts accepted' if not parts else 'INCOMPLETE: accepted exact packets; OT/occlusion visual review still required'
    report={'off':'PASS' if off_ok else 'FAIL','off_hash':result.stdout,'paired_screenshots':len(pairs),'on':verdict,'accepted_parts':len(parts),'checkpoint_sha256':hashlib.sha256(a.checkpoint.read_bytes()).hexdigest()}
    (a.out/'summary.json').write_text(json.dumps(report,indent=2)); print(json.dumps(report,indent=2))
    return 2 if off_ok else 1

if __name__=='__main__':
    raise SystemExit(main())
