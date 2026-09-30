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
HOOKS = ['PcPort_Hd2dPostProcess', 'PcPort_Hd2dSpriteBegin', 'PcPort_FeiHd2dCapture', 'PcPort_Hd2dSpriteDraw', 'PcPort_Hd2dSpriteTextureOverride']

def run(cmd, **kw):
    return subprocess.run(cmd, check=True, cwd=ROOT, **kw)


def drive(binary, qs, out, enabled, timeout, trace=False, settle=480, ready_marker='player position restored', readiness_kind='checkpoint restore', capture_every=8, capture_count=24, test_input=None):
    out.mkdir()
    cap = out / 'cap'
    cap.mkdir()
    env = {k:v for k,v in os.environ.items() if not k.startswith(('XENO_', 'DISPLAY', 'WAYLAND_DISPLAY', 'XAUTHORITY'))}
    env.update(SDL_VIDEODRIVER='offscreen', SDL_AUDIODRIVER='dummy', ALSOFT_DRIVERS='null',
               XENO_CHEATS_CONSOLE='1', XENO_QUICKSAVE_PATH=str(qs),
               XENO_FIELD_CAPTURE_DIR=str(cap), XENO_FIELD_CAPTURE_EVERY=str(capture_every),
               XENO_HD2D_FIELD_TRANSFORM_FIXTURE=os.environ.get('XENO_HD2D_FIELD_TRANSFORM_FIXTURE','0'),
               XENO_HD2D_FIELD_POSE_FIXTURE=os.environ.get('XENO_HD2D_FIELD_POSE_FIXTURE','0'), XENO_HD2D_SPRITE_DIAG='1', XENO_HD2D_BATCH_STATS='1', XENO_HD2D_TRACE_PARTS=str(int(trace)), XENO_HD2D_OCTOPATH=str(enabled), XENO_HD2D_OPENING_MAPS='14',
               XENO_FEI_HD2D='0', XENO_PRESENT_HASH_LOG=str(out/'hash.txt'),
               XENO_HD2D_FIELD_PROBE=str(out/'probe.txt'))
    if test_input is not None: env['XENO_FIELD_TEST_INPUT']=test_input
    lines=[]
    with (out/'run.log').open('w') as log:
        proc=subprocess.Popen([str(binary)], cwd=ROOT, env=env, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        def reader():
            for raw in proc.stdout:
                line=raw.decode('latin1'); lines.append(line); log.write(line); log.flush()
        thread=threading.Thread(target=reader); thread.start()
        start=time.monotonic(); sent=0; restored=None; restore_present=None; restore_existing=set(); selected=[]
        try:
            while time.monotonic()-start < timeout and proc.poll() is None:
                text=''.join(lines)
                if restored is None and ready_marker in text:
                    restored=time.monotonic()
                    # Field captures can pause throughout a transition. An
                    # existing filename may precede restoration by hundreds of
                    # presents; wait for a NEW capture to bound it from above.
                    restore_existing=set(cap.glob('field-frame-*.png'))
                if restored and restore_present is None:
                    newer=sorted(set(cap.glob('field-frame-*.png'))-restore_existing)
                    if newer: restore_present=int(newer[0].stem.rsplit('-',1)[1])
                if restored is None and '[xeno-port][quick] loading' not in text and 'console reading commands' in text and 'FieldLoad begin' in text and time.monotonic()-sent>3:
                    proc.stdin.write(b'load\n'); proc.stdin.flush(); sent=time.monotonic()
                if restore_present is not None:
                    eligible=[p for p in sorted(cap.glob('field-frame-*.png'))
                              if int(p.stem.rsplit('-',1)[1]) >= restore_present+settle]
                    if len(eligible)>=capture_count:
                        selected=eligible[:capture_count]
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
                raise RuntimeError(f'{out.name}: readiness/capture failed elapsed={time.monotonic()-start:.1f}s process_rc={proc.poll()} marker={ready_marker!r}')
        finally:
            exited_before_stop=proc.poll()
            (out/'process.json').write_text(json.dumps({'elapsed_seconds':time.monotonic()-start,
                'returncode_before_stop':exited_before_stop,'readiness_seen':restored is not None,
                'ready_marker':ready_marker},indent=2))
            proc.terminate()
            try: proc.wait(10)
            except subprocess.TimeoutExpired: proc.kill(); proc.wait()
            thread.join()
    # Fixed count-based window, chosen before any pixel/hash comparison.
    (out/'window.json').write_text(json.dumps({'restore_present_upper_bound':restore_present,'readiness_kind':readiness_kind,'ready_marker':ready_marker,
        'settle_presents':settle,'capture_count':capture_count,'capture_stride':capture_every},indent=2))
    if len(selected)<capture_count: raise RuntimeError('insufficient field captures')
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
    p.add_argument('--settle',type=int,default=480,help='fixed presents after checkpoint restore before measurement')
    p.add_argument('--pose-fixture',action='store_true',help='same observed actor10 pose in both binaries; separately labelled from organic gameplay')
    p.add_argument('--transform-fixture',action='store_true',help='same verified render inputs in both binaries; requires pose fixture')
    p.add_argument('--skip-trace',action='store_true',help='use accepted production ON records; diagnostic TRACE rejects active profiles')
    a=p.parse_args()
    if a.transform_fixture and not a.pose_fixture: p.error('transform fixture requires pose fixture')
    if a.settle<120 or a.settle>3600: p.error('settle must be between120 and3600 presents')
    a.out=a.out.resolve(); a.out.mkdir(parents=True,exist_ok=False)
    build=ROOT/'pc_port/build_native'
    args=(build/'xeno-port.link-args').read_text().splitlines()
    flags=shlex.split((build/'port-source.cflags').read_text())
    run(['gcc','-c','pc_port/tests/hd2d_field_probe.c',*flags,'-o',str(a.out/'probe.o')])
    baseline=a.out/'no-lane'; lane=a.out/'lane'
    shared=[]
    if a.pose_fixture:
        os.environ['XENO_HD2D_FIELD_POSE_FIXTURE']='1'
        run(['gcc','-c','pc_port/tests/hd2d_field_pose_fixture.c',*flags,'-o',str(a.out/'pose.o')])
        run(['gcc','-c','pc_port/tests/hd2d_field_pose_observer.c',*flags,'-o',str(a.out/'pose-observer.o')])
        shared=[str(a.out/'pose.o'),str(a.out/'pose-observer.o'),'-Wl,--wrap=func_8001D2B0','-Wl,--wrap=func_8001DAE8','-Wl,--wrap=PcPort_FeiHd2dBegin']
    if a.transform_fixture:
        os.environ['XENO_HD2D_FIELD_TRANSFORM_FIXTURE']='1'
        run(['gcc','-c','pc_port/tests/hd2d_field_transform_fixture.c',*flags,'-o',str(a.out/'transform.o')])
        shared.extend([str(a.out/'transform.o'),'-Wl,--wrap=func_8001E298'])
    run(['gcc',*shared,*[x for x in args if '/obj/hd2d__' not in x],'-o',str(baseline)])
    run(['gcc',*shared,str(a.out/'probe.o'),*args,*['-Wl,--wrap='+h for h in HOOKS],'-o',str(lane)])
    for binary, expected in ((baseline,False),(lane,True)):
        symbols=run(['nm','--defined-only',str(binary)],capture_output=True,text=True).stdout
        for hook in HOOKS[:2]:
            strong=any(line.split()[-2:]==['T',hook] for line in symbols.splitlines())
            if strong!=expected: raise RuntimeError(f'wrong strong hook linkage: {binary} {hook}')
    manifests={}
    runs=[('A',baseline,0),('B',lane,0),('ON',lane,1)]
    if not a.skip_trace: runs.append(('TRACE',lane,1))
    for name,binary,on in runs:
        manifests[name]=drive(binary,a.checkpoint.resolve(),a.out/name,on,a.timeout,trace=name=='TRACE',settle=a.settle)
        if a.transform_fixture:
            text=(a.out/name/'run.log').read_text()
            lo=min(int(k.rsplit('-',1)[1].split('.')[0]) for k in manifests[name])
            hi=max(int(k.rsplit('-',1)[1].split('.')[0]) for k in manifests[name])
            applied=set()
            for line in text.splitlines():
                if not line.startswith('[hd2d-transform-fixture] '): continue
                present=int(line.split('present=')[1].split()[0])
                if lo<=present<=hi:
                    if ' unapplied ' in line: raise RuntimeError('transform fixture declined inside measured window')
                    if ' applied ' in line: applied.add(present)
            if not set(range(lo,hi+1)).issubset(applied): raise RuntimeError('transform fixture missing inside measured window')
    result=subprocess.run(['python3','pc_port/tests/hd2d_field_sequence_compare.py',str(a.out/'A'),str(a.out/'B')],cwd=ROOT,capture_output=True,text=True)
    (a.out/'off-sequence.json').write_text(result.stdout)
    pairs=[(x,y) for x,h in manifests['A'].items() for y,k in manifests['B'].items() if h==k]
    offprobe=(a.out/'B/probe.txt').read_text()
    off_ok=result.returncode==0 and bool(pairs) and 'post ' in offprobe and 'begin ' in offprobe
    (a.out/'paired-captures.json').write_text(json.dumps(pairs,indent=2))
    on=(a.out/'ON/probe.txt').read_text().splitlines()
    facts={}
    source='ON' if a.skip_trace else 'TRACE'
    trace=(a.out/source/'probe.txt').read_text().splitlines()
    for line in trace:
        if not line.startswith('part '): continue
        row=dict(item.split('=',1) for item in line.split()[1:])
        if row['map']!='14' or row['actor']!=row['player'] or (a.skip_trace and row['accepted']!='1'): continue
        key=tuple(row[k] for k in ('map','actor','fnv','frame','part','uv','tpage','clut'))
        facts[key]={'source_run':source, **{k:row[k] for k in ('map','actor','player','fnv','frame','part','uv','tpage','clut','quad','ot')}}
    (a.out/'observed-player-parts.json').write_text(json.dumps(list(facts.values()),indent=2))
    parts=[line for line in on if line.startswith('part ') and 'accepted=1' in line]
    if any('packet_exact=0' in line for line in parts): raise RuntimeError('ON changed retail packet geometry/UV/colour')
    gpu=[line for line in on if line.startswith('gpu ') and 'accepted=1' in line]
    verdict=('BLOCKED: no production sprite parts accepted' if not parts else
             'BLOCKED: registered parts were not consumed as replacement textures' if not gpu else
             'INCOMPLETE: GPU consumed exact packets; footprint/occlusion review still required')
    scope='fixed-render-input replay' if a.transform_fixture else 'same-pose fixture' if a.pose_fixture else 'organic gameplay'
    report={'identity_scope':scope, 'gpu_accepted_parts':len(gpu), 'off':'PASS' if off_ok else 'FAIL','off_hash':result.stdout,'paired_screenshots':len(pairs),'on':verdict,'accepted_parts':len(parts),'checkpoint_sha256':hashlib.sha256(a.checkpoint.read_bytes()).hexdigest()}
    (a.out/'summary.json').write_text(json.dumps(report,indent=2)); print(json.dumps(report,indent=2))
    return 2 if off_ok else 1

if __name__=='__main__':
    raise SystemExit(main())
