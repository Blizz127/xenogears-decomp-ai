#!/bin/bash
# gate: from-clean make build, compare all hashes to baseline; port preprocess equality for changed files
cd ~/dev/xenogears-decomp-public-harvest
. ./.harvest_env.sh
W=~/scratch/xeno-mit-harvest
nice -n 10 make build NUMPROC=4 > $W/gate_build.log 2>&1 || { echo "GATE: BUILD FAILED"; tail -5 $W/gate_build.log; exit 1; }
sha256sum build/out/*.bin build/out/slus_006.64 > $W/gate.sha
if diff -q <(sort $W/baseline.sha) <(sort $W/gate.sha) >/dev/null; then echo "GATE: hashes identical to baseline"; else echo "GATE: HASH DIFF"; diff <(sort $W/baseline.sha) <(sort $W/gate.sha) | cut -c1-120; exit 2; fi
for f in $(git diff --name-only -- src); do
  a=$(git show HEAD:$f | cpp -P -DXENO_PC_PORT -Iinclude -Ipc_port/src -undef 2>/dev/null | grep -v '^\s*$' | md5sum)
  b=$(cpp -P -DXENO_PC_PORT -Iinclude -Ipc_port/src -undef $f 2>/dev/null | grep -v '^\s*$' | md5sum)
  [ "$a" == "$b" ] && echo "PORT-PP same: $f" || { echo "PORT-PP DIFF: $f"; exit 3; }
done
echo "GATE: OK"
