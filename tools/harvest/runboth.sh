#!/bin/bash
cd ~/dev/xenogears-decomp-public-harvest
. ./.harvest_env.sh
W=~/scratch/xeno-mit-harvest
for src in xem ea; do for m in world_map field menu slus_006.64 shop_menu movie battling; do
  echo "== $src $m start $(date +%H:%M:%S)" >> $W/runboth7.log
  nice -n 10 python3 $W/adopt.py $m --source $src --max-insns 400 --out $W/adopt7_${src}_$m.json >> $W/runboth7.log 2>&1
done; done
echo ALLDONE >> $W/runboth7.log
