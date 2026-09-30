#!/usr/bin/env python3
"""Relink strong lane for existing b84 retail/HD/hidden footprint cycle.

No synthetic profile and no forced acceptance. Production must accept exactly
six active field14 player parts for FNV db7d0105/frame190 in HD blocks. Disable presentation effects only
through a copied test config. Raw lane/legacy logs are retained separately.
Runtime retail VRAM alpha bounds supply the analyzer calibration box. PASS also
requires measured GPU texture acceptance and changed Fei pixels.
"""
import argparse
import json
import os
from pathlib import Path
import shlex
import subprocess
import sys

ROOT=Path(__file__).resolve().parents[2]

def replacement_evidence_pass(strict_rc, gpu_count, changed_pixels, per_part):
    return strict_rc==0 and gpu_count>0 and changed_pixels>0 and per_part


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--out',required=True,type=Path)
    p.add_argument('--checkpoint',type=Path,default=Path.home()/'release-staging/xeno-work/r6-nogamedata/field14.xgqs')
    p.add_argument('--timeout',default=420,type=int)
    p.add_argument('--head-crop-trials',action='store_true',help='test-only predetermined UV trials; never production evidence')
    from PIL import Image  # Fail before launching the game if tooling is missing.
    a=p.parse_args(); out=a.out.resolve(); out.mkdir(parents=True,exist_ok=False)
    env={k:v for k,v in os.environ.items() if not k.startswith(('XENO_','DISPLAY','WAYLAND_DISPLAY','XAUTHORITY'))}
    env.update(SDL_VIDEODRIVER='offscreen',XENO_FEI_HD2D='0',XENO_HD2D_BATCH_STATS='1',
               XENO_HD2D_OPENING_MAPS='14',XENO_HD2D_FOOTPRINT_PART_LOG=str(out/'lane-parts.log'))
    build=ROOT/'pc_port/build_native'; binary=out/'lane-footprint'
    def run(cmd,**kw): return subprocess.run(cmd,cwd=ROOT,env=env,**kw)
    run(['gcc','-c','pc_port/tests/hd2d_footprint_probe.c',*shlex.split((build/'port-source.cflags').read_text()),'-o',str(out/'probe.o')],check=True)
    hooks=['PcPort_Hd2dSpriteBegin','PcPort_Hd2dSpriteDraw','PcPort_Hd2dSpriteEnd','PcPort_Hd2dPostProcess','XgHd2d_Present','PcPort_Hd2dSpriteTextureOverride']
    objects=[str(out/'probe.o')]
    if a.head_crop_trials:
        run(['gcc','-c','pc_port/tests/hd2d_head_crop_probe.c',*shlex.split((build/'port-source.cflags').read_text()),'-o',str(out/'crop-probe.o')],check=True)
        objects.append(str(out/'crop-probe.o'))
        hooks.append('PcPort_Hd2dSpriteSubmitPart')
        env.update(XENO_HD2D_HEAD_CROP_VARIANTS='1',XENO_HD2D_HEAD_CROP_VARIANT_LOG=str(out/'head-variants.log'))
    run(['gcc',*objects,*(build/'xeno-port.link-args').read_text().splitlines(),*['-Wl,--wrap='+h for h in hooks],'-o',str(binary)],check=True)
    with (out/'drive.txt').open('w') as f:
        drive=run([sys.executable,'pc_port/tests/fei_hd2d_footprint_drive.py',str(binary),str(a.checkpoint.resolve()),str(out),'120' if a.head_crop_trials else '24',str(a.timeout)],stdout=f,stderr=subprocess.STDOUT)
    if drive.returncode: print('HD2D FOOTPRINT FAIL capture/production acceptance; inspect run.log'); return 1
    runtime=(out/'run.log').read_text()
    if '[hd2d-footprint] FAIL ' in runtime:
        print('HD2D FOOTPRINT FAIL runtime invariant; inspect run.log'); return 1
    lane_path=out/'lane-parts.log'
    lane=lane_path.read_text().splitlines() if lane_path.exists() else []
    legacy=(out/'parts.log').read_text().splitlines()
    # Match exactly the b84 analyzer's build window: block start through
    # captured present at offset6. Early loading/fallback outside that fixed
    # measurement window is retained as raw evidence, never counted as HD.
    captures=sorted(int(f.stem.rsplit('-',1)[1]) for f in (out/'cap').glob('fei-frame-*.png'))
    measured={present for c in captures if c//8%3==1 for present in range(c//8*8,c+1)}
    def present(line): return int(line.split('present=')[1].split()[0])
    contamination=[line for line in legacy if 'mode=1 ' in line and present(line) in measured]
    observed=[line for line in lane if 'part=' in line and present(line) in measured]
    gpu=[line for line in lane if 'gpu_accepted=1' in line and present(line) in measured]
    boxes=[line for line in lane if 'source=runtime_retail_vram' in line and present(line) in measured]
    if not observed or contamination:
        print('HD2D FOOTPRINT FAIL missing measured lane parts or measured legacy HD contamination'); return 1
    merged=sorted(legacy+lane,key=lambda l:int(l.split('present=')[1].split()[0]))
    (out/'merged-parts.log').write_text('\n'.join(merged)+'\n')
    if a.head_crop_trials:
        return run([sys.executable,'pc_port/tests/hd2d_head_crop_report.py',str(out),'--out',str(out/'variants')]).returncode
    env['FEI_FOOTPRINT_STRICT_HORIZONTAL']='1'
    with (out/'analysis.txt').open('w') as f:
        result=run([sys.executable,'pc_port/tests/fei_hd2d_footprint_analyze.py',str(out/'merged-parts.log'),str(out/'cap'),str(out),'--tol','1'],stdout=f,stderr=subprocess.STDOUT)
    # The b84 evidence images come from a comparable retail/HD/hidden triple.
    # CPU linkage alone cannot prove that the GPU sampled the replacement.
    changed=0
    if all((out/f'full_{name}.png').exists() for name in ('retail','hd2d','hidden')):
        retail=Image.open(out/'full_retail.png').convert('RGB')
        hd=Image.open(out/'full_hd2d.png').convert('RGB')
        hidden=Image.open(out/'full_hidden.png').convert('RGB')
        changed=sum(r!=h and (r!=x or h!=x) for r,h,x in zip(retail.getdata(),hd.getdata(),hidden.getdata()))
    analysis=(out/'analysis.txt').read_text()
    per_part=bool(boxes) and 'p0:' in analysis and 'p5:' in analysis
    passed=replacement_evidence_pass(result.returncode,len(gpu),changed,per_part)
    report={'production_lane_parts':len(lane),'measured_production_parts':len(observed),'measured_legacy_hd_parts':len(contamination),
            'strict_b84_analyzer':'PASS' if result.returncode==0 else 'FAIL',
            'measured_gpu_acceptances':len(gpu),'changed_fei_pixels':changed,
            'per_part_alpha':'MEASURED' if per_part else 'NOT_MEASURED',
            'overall':'PASS' if passed else 'FAIL'}
    (out/'summary.json').write_text(json.dumps(report,indent=2));print(json.dumps(report,indent=2))
    return 0 if passed else 1

if __name__=='__main__': raise SystemExit(main())
