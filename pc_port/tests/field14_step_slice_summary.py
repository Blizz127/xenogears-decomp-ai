#!/usr/bin/env python3
"""Summarise and compare run_field14_step_slice.sh runs.

  field14_step_slice_summary.py REFERENCE_DIR DISABLED_DIR OUT.json

Per run: fixture load, field-step arming, step transitions, empty drains,
interpreter refusals, CPU execution counters, and the stable map-14 position
at the end of each neutral phase (the POSDIAG sample just before each
transition, and the last one).  PASS needs both runs to load the fixture,
arm, complete all transitions, show identical checkpoint positions, and the
disabled run to report zero interpreted instructions and no refusal.  It is
movement-slice evidence only: no audio, no frame-exact timing.
"""
import json
import re
import sys
from pathlib import Path

POS = re.compile(r'POSDIAG map=(\d+) pos=\((-?\d+),(-?\d+),(-?\d+)\)')
STEP = re.compile(r'\[field-step-pad\] step=(\d+) held=(0x[0-9a-f]+)')


def run(path):
    text = Path(path, 'run.log').read_text(errors='replace').splitlines()
    out = {'dir': str(path), 'loaded': False, 'armed': False, 'transitions': [],
           'empty_drains': 0, 'refusals': 0, 'stubs': 0, 'checkpoints': [],
           'execution_stats': [], 'fallback_stats': []}
    last_pos = None
    for line in text:
        if '[xeno-port][quick] loading ' in line:
            out['loaded'] = True
        if '[field-step-pad] armed map=14' in line:
            out['armed'] = True
        if 'drained no vblank' in line:
            out['empty_drains'] += 1
        if 'CPU interpreter disabled: unconverted target' in line:
            out['refusals'] += 1
        if '[stub]' in line:
            out['stubs'] += 1
        if 'cpu-execution' in line:
            out['execution_stats'].append(line.strip())
        if '[cpu-fallback] battle=' in line:
            out['fallback_stats'].append(line.strip())
        m = POS.search(line)
        if m and m.group(1) == '14' and out['armed']:
            last_pos = [int(m.group(i)) for i in (2, 3, 4)]
        m = STEP.search(line)
        if m and out['armed']:
            out['transitions'].append([int(m.group(1)), m.group(2)])
            out['checkpoints'].append(last_pos)
    out['checkpoints'].append(last_pos)
    interp = [int(x) for l in out['execution_stats']
              for x in re.findall(r'interpreted_instructions=(\d+)', l)]
    out['interpreted_instructions'] = max(interp) if interp else None
    out['rc'] = Path(path, 'rc.txt').read_text().strip() if Path(path, 'rc.txt').exists() else None
    return out


def main():
    ref, dis = run(sys.argv[1]), run(sys.argv[2])
    want = [[300, '0x2000'], [360, '0x0000'], [480, '0x8000'], [540, '0x0000'],
            [660, '0x4000'], [720, '0x0000']]
    checks = {
        'both loaded the fixture': ref['loaded'] and dis['loaded'],
        'both armed in field 14': ref['armed'] and dis['armed'],
        'both completed every transition': ref['transitions'] == want and dis['transitions'] == want,
        'checkpoint positions identical': ref['checkpoints'] == dis['checkpoints'] and None not in ref['checkpoints'],
        'movement observed': len({tuple(c) for c in ref['checkpoints'] if c}) >= 3,
        'disabled: no interpreter refusal': dis['refusals'] == 0,
        'disabled: zero interpreted instructions counted': dis['interpreted_instructions'] == 0,
        'no stubs reached': ref['stubs'] == 0 and dis['stubs'] == 0,
    }
    result = {'pass': all(checks.values()), 'checks': checks, 'reference': ref, 'disabled': dis,
              'limits': 'movement slice only: headless null audio, no frame-exact timing, '
                        'checkpoints are quiescent positions, not per-step traces'}
    Path(sys.argv[3]).write_text(json.dumps(result, indent=2) + '\n')
    for k, v in checks.items():
        print(('PASS ' if v else 'FAIL ') + k)
    print('FIELD14 SLICE ' + ('PASS' if result['pass'] else 'FAIL'))
    return 0 if result['pass'] else 1


if __name__ == '__main__':
    sys.exit(main())
