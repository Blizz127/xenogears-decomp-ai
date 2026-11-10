#!/bin/bash
# trial.sh MODULE NAME BLKFILE  -> inserts block in place of INCLUDE_ASM, builds the TU object, leaves it in tree (git checkout src to revert)
cd ~/dev/xenogears-decomp-public-harvest; . ./.harvest_env.sh
mod=$1; name=$2; blk=$3
f=$(grep -rl "INCLUDE_ASM([^)]*, *$name *)" src/$mod | head -1)
python3 - "$f" "$name" "$blk" <<'P'
import sys,re
f,name,blk=sys.argv[1:4]
t=open(f).read(); b=open(blk).read().rstrip('\n')+'\n'
t,k=re.subn(r'^[ \t]*INCLUDE_ASM\(\s*[^,]+,\s*%s\s*\)\s*;[ \t]*$'%name,lambda m:b.rstrip('\n'),t,count=1,flags=re.M)
open(f,'w').write(t); print(f,k)
P
tu=build/${f}.o
nice -n 10 ninja -j4 $tu 2>&1 | tail -3
echo $tu
