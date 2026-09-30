#!/usr/bin/env python3
"""Diagnostic retail trace reached through the native safe-field warp API.

Load real map14 checkpoint; queue a native warp; await distinct destination-safe
marker; settle and capture. Empty private profile means no replacement art.
--prepare-only compiles/relinks without launching a graphics process.
"""
import argparse
import hashlib
import json
from pathlib import Path
import shlex
import subprocess
from hd2d_field_validation import ROOT,HOOKS,drive
from hd2d_opening_evidence import checkpoint,probe


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--out',type=Path,required=True)
    p.add_argument('--checkpoint',type=Path,default=Path.home()/'release-staging/xeno-work/warps/ckpt14.xgqs')
    p.add_argument('--map',dest='map_id',type=int,default=12)
    p.add_argument('--entrance',type=int,default=0)
    p.add_argument('--settle',type=int,default=120)
    p.add_argument('--timeout',type=float,default=420)
    p.add_argument('--prepare-only',action='store_true')
    a=p.parse_args()
    if not(0<=a.map_id<1024 and a.map_id!=14 and 0<=a.entrance<65536 and a.settle>=0):p.error('invalid destination or settle')
    qs=a.checkpoint.resolve(); facts=checkpoint(qs)
    if facts['map']!=14:p.error('source checkpoint must actually be map14')
    out=a.out.resolve();out.mkdir(parents=True,exist_ok=False)
    private=out/'private-include';private.mkdir()
    (private/'hd2d_sprite_profile.h').write_text('#include "hd2d_sprite_parts.h"\nstatic const PcPortHd2dPartProfile hd2d_sprite_profile={0};\n')
    build=ROOT/'pc_port/build_native';flags=shlex.split((build/'port-source.cflags').read_text())
    def run(cmd):subprocess.run(cmd,cwd=ROOT,check=True)
    objects=[]
    for source,name,extra in [('pc_port/tests/hd2d_field_probe.c','probe',[]),
        ('pc_port/tests/hd2d_warp_teardown_probe.c','teardown',[]),
        ('pc_port/tests/hd2d_capture_warp.c','warp',[f'-DHD2D_CAPTURE_MAP={a.map_id}',f'-DHD2D_CAPTURE_ENTRANCE={a.entrance}'])]:
        obj=out/(name+'.o');run(['gcc','-c',source,*extra,*flags,'-o',str(obj)]);objects.append(str(obj))
    parts=out/'empty-parts.o'
    run(['gcc','-I'+str(private),'-c','pc_port/src/hd2d/hd2d_sprite_parts.c',*flags,'-o',str(parts)])
    args=(build/'xeno-port.link-args').read_text().splitlines()
    matches=[i for i,v in enumerate(args) if v.endswith('/hd2d__hd2d_sprite_parts.c.o')]
    if len(matches)!=1:raise RuntimeError('expected exactly one parts object')
    args[matches[0]]=str(parts)
    teardown_hooks=['func_800798BC','func_800A91F0','func_800A31E8','FieldParticlesFreeAll','func_800864F0','func_8007FFE8','FieldFree','FieldPartyFreeSkinDataBuffers','func_80085988']
    binary=out/'warp-trace';run(['gcc',*objects,*args,*['-Wl,--wrap='+h for h in HOOKS+teardown_hooks],'-o',str(binary)])
    marker=f'[capture-warp] destination-active map={a.map_id} entrance={a.entrance} '
    metadata={'source_checkpoint':facts,'destination_map':a.map_id,'entrance':a.entrance,
              'profile':'private empty diagnostic profile','readiness_marker':marker,'settle_presents':a.settle}
    (out/'fixture.json').write_text(json.dumps(metadata,indent=2))
    if a.prepare_only:print('HD2D WARP TRACE PREPARED '+str(binary));return 0
    drive(binary,qs,out/'TRACE',1,a.timeout,trace=True,settle=a.settle,
          ready_marker=marker,readiness_kind='native warp destination safe')
    log=(out/'TRACE/probe.txt').read_text()
    if any('accepted=1' in line for line in log.splitlines() if line.startswith(('begin ','part ','gpu '))):
        raise RuntimeError('diagnostic empty-profile trace unexpectedly accepted replacement')
    rows=[r for r in probe(out/'TRACE/probe.txt') if r['map']==str(a.map_id)]
    if not rows:raise RuntimeError('destination has no observed sprite parts')
    (out/'observed-destination-keys.json').write_text(json.dumps(rows,indent=2))
    print(f'HD2D WARP TRACE captured map={a.map_id} keys={len(rows)} art_acceptance=0')
    return 0
if __name__=='__main__':raise SystemExit(main())
