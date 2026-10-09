#!/usr/bin/env bash
# Test-only dirty-room oracle job. Run on the admitted runner, not a game session.
# Export only the JSONL and provenance JSON to the isolated implementer.
set -euo pipefail
cd "$(dirname "$0")/../.."
out=${1:?usage: run_cleanroom_read_geom_vectors.sh OUTPUT_DIRECTORY}
mkdir -p "$out"
python3 - <<'PY'
from pathlib import Path
import hashlib
p = Path('disc/SLUS_006.64')
assert hashlib.sha256(p.read_bytes()).hexdigest() == 'dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119', 'wrong retail executable'
PY
scratch=$(mktemp -d /tmp/xeno-cleanroom-vectors.XXXXXX)
trap 'rm -rf "$scratch"' EXIT
cc -std=c11 -O2 -Wall -Wextra -Werror -Ipc_port/src \
    pc_port/tests/cleanroom_read_geom_vectors.c pc_port/src/battle_mips_adapter.c \
    -o "$scratch/read-geom-vectors"
# The interpreter is permitted only as this test oracle. Production settings
# and binaries are not changed.
XENO_DISABLE_CPU_INTERPRETER=0 "$scratch/read-geom-vectors" disc/SLUS_006.64 \
    > "$out/read-geom.jsonl" 2> "$out/read-geom.log"
python3 - "$out" <<'PY'
from pathlib import Path
import hashlib, json, subprocess, sys
out = Path(sys.argv[1])
vectors = out / 'read-geom.jsonl'
rows = [json.loads(line) for line in vectors.read_text().splitlines()]
assert len(rows) == 1430, len(rows)
assert {r['function'] for r in rows} == {'ReadGeomOffset', 'ReadGeomScreen'}
paths = ['pc_port/tests/cleanroom_read_geom_vectors.c', 'pc_port/src/battle_mips_adapter.c',
         'pc_port/src/battle_mips_adapter.h', 'disc/SLUS_006.64']
receipt = {
    'status': 'vectors generated; host replacements not yet tested',
    'source_commit': subprocess.check_output(['git', 'rev-parse', 'HEAD'], text=True).strip(),
    'vector_sha256': hashlib.sha256(vectors.read_bytes()).hexdigest(),
    'case_count': len(rows),
    'input_sha256': {p: hashlib.sha256(Path(p).read_bytes()).hexdigest() for p in paths},
    'oracle': 'existing test-only PcPortMipsRun; modeled CFC2 control reads from public psx-spx',
    'limits': ['not hardware execution', 'no accepted host implementation',
               'memory outside bounded outputs faults; unexpected GTE operations fault'],
}
(out / 'read-geom-provenance.json').write_text(json.dumps(receipt, indent=2) + '\n')
print(json.dumps(receipt, indent=2))
PY
