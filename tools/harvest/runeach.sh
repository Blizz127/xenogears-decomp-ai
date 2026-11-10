#!/bin/bash
# runeach.sh <xem|ea> <module> [names...]  -- adopt each candidate on its own (address order), accumulating in the tree.
# With no names, uses the 'mismatch' entries of the module's last adopt8_<src>_<module>.json.
cd ~/dev/xenogears-decomp-public-harvest; . ./.harvest_env.sh
W=~/scratch/xeno-mit-harvest; src=$1; mod=$2; shift 2
names="$@"
if [ -z "$names" ]; then names=$(python3 -c "
import json
d=json.load(open('$W/adopt8_${src}_$mod.json'))
print(' '.join(r[0] for r in sorted(d,key=lambda r:r[1]) if r[2] in ('mismatch',)))"); fi
for n in $names; do
  out=$(nice -n 10 python3 $W/adopt.py $mod --source $src --max-insns 400 --pos --only $n 2>&1 | grep "^{")
  echo "$n $out" | tee -a $W/runeach.log
done
echo EACHDONE >> $W/runeach.log
