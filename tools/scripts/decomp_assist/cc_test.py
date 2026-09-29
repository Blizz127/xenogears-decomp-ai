#!/usr/bin/env python3
"""cc_test.py OV NAME FILE.c [cc1 ...]

Compile a hand-written C file standalone (repo cpp + the given cc1s, default
gcc-2.7.2-psx and gcc-2.6.3-psx, + maspsx) and compare function NAME with the
retail words; prints MATCH / diff / size per compiler.  Set
XENO_MASPSX_EXTRA=--expand-div for code with divide checks.  FILE.c should include
"common.h" and declare what it uses."""
import os
import subprocess
import sys

sys.path.insert(0, os.path.dirname(__file__))
from assist_common import CACHE, CC1_FLAGS, MASPSX_FLAGS, REPO, compare, func_words, load_table

ov, name, src = sys.argv[1], sys.argv[2], os.path.abspath(sys.argv[3])
ccs = sys.argv[4:] or ['gcc-2.7.2-psx', 'gcc-2.6.3-psx']
os.chdir(REPO)
ins = next(f['ins'] for fl in load_table(ov).values() for f in fl if f['name'] == name)
os.makedirs(CACHE, exist_ok=True)
i_file, s_file, o_file = (os.path.join(CACHE, '_cct' + e) for e in ('.i', '.s', '.o'))
p = subprocess.run(['tools/gcc-2.7.2-psx/cpp', '-Iinclude', '-D_LANGUAGE_C', '-DSKIP_ASM', '-P', '-undef',
                    '-lang-c', '-nostdinc', src, i_file], capture_output=True, text=True)
if p.returncode:
    sys.exit(p.stderr)
for cc in ccs:
    p = subprocess.run([f'tools/{cc}/cc1'] + CC1_FLAGS + ['-o', s_file, i_file], capture_output=True, text=True)
    if p.returncode:
        print(cc, 'cc1-fail:', p.stderr.strip().split('\n')[-1])
        continue
    if os.path.exists(o_file):
        os.remove(o_file)
    subprocess.run(['python3', 'tools/maspsx/maspsx.py'] + MASPSX_FLAGS + os.environ.get('XENO_MASPSX_EXTRA', '').split() + ['-o', o_file, s_file],
                   capture_output=True, stdin=subprocess.DEVNULL)
    got = func_words(o_file, name)
    if not got:
        print(cc, 'no symbol')
        continue
    bad = compare(ins, got[1])
    print(cc, 'MATCH' if bad == 0 else (f'diff {bad}/{len(ins)}' if bad > 0 else f'size {got[0]} vs {len(ins) * 4}'))
