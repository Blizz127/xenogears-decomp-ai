#!/usr/bin/env python3
"""Actual retail field walking trace with safe-field-gated existing pad schedule."""
import argparse,json,shlex,subprocess
from pathlib import Path
from hd2d_field_validation import ROOT,HOOKS,drive
from hd2d_opening_evidence import checkpoint,probe
SCHEDULE='0:0,16:0x2000,40:0,56:0x8000,80:0,96:0x4000,120:0,136:0x1000,160:0'
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--out',required=True,type=Path);p.add_argument('--prepare-only',action='store_true');a=p.parse_args()
 qs=Path.home()/'release-staging/xeno-work/npc/map1e0.xgqs';facts=checkpoint(qs);assert facts['map']==1
 out=a.out.resolve();out.mkdir(parents=True,exist_ok=False);inc=out/'private';inc.mkdir()
 (inc/'hd2d_sprite_profile.h').write_text('#include "hd2d_sprite_parts.h"\nstatic const PcPortHd2dPartProfile hd2d_sprite_profile={0};\n')
 build=ROOT/'pc_port/build_native';flags=shlex.split((build/'port-source.cflags').read_text())
 def run(args):subprocess.run(args,cwd=ROOT,check=True)
 objs=[]
 for name in ('hd2d_field_probe','hd2d_walk_input_probe'):
  obj=out/(name+'.o');run(['gcc','-c',f'pc_port/tests/{name}.c',*flags,'-o',str(obj)]);objs.append(str(obj))
 part=out/'empty-parts.o';run(['gcc','-I'+str(inc),'-c','pc_port/src/hd2d/hd2d_sprite_parts.c',*flags,'-o',str(part)])
 args=(build/'xeno-port.link-args').read_text().splitlines();matches=[i for i,v in enumerate(args) if v.endswith('/hd2d__hd2d_sprite_parts.c.o')];assert len(matches)==1;args[matches[0]]=str(part)
 binary=out/'walk-trace';run(['gcc',*objs,*args,*['-Wl,--wrap='+h for h in HOOKS+['PcPort_FieldTestInputMerge']],'-o',str(binary)])
 (out/'fixture.json').write_text(json.dumps({'checkpoint':facts,'schedule':SCHEDULE,'profile':'private empty','source_settle_presents':120},indent=2))
 if a.prepare_only:print('PREPARED',binary);return
 drive(binary,qs,out/'TRACE',1,240,trace=True,settle=0,ready_marker='[capture-walk] settled-active map=1 ',readiness_kind='safe settled map1 before actual pad schedule',capture_every=2,capture_count=128,test_input=SCHEDULE)
 (out/'observed-keys.json').write_text(json.dumps(probe(out/'TRACE/probe.txt'),indent=2))
 print('WALK TRACE COMPLETE',out)
if __name__=='__main__':main()
