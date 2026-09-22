#!/usr/bin/env python3
"""Print objdiff's own per-category match percentages from build/progress.json.

Used as the independent cross-check on the byte-weighted exact-function figure
produced by analyze.py.  Regenerate the input with `make report`.
"""
import json

d = json.load(open("build/progress.json"))
m = d["measures"]
print(f"TOTAL  {m.get('matched_code_percent', 0):6.2f}%  "
      f"{m.get('matched_code', 0)}/{m.get('total_code', 0)} bytes  "
      f"funcs {m.get('matched_functions', 0)}/{m.get('total_functions', 0)}")
print()
for c in d.get("categories", []):
    m = c["measures"]
    print(f"{c['name']:<34} {m.get('matched_code_percent', 0):6.2f}%  "
          f"{m.get('matched_code', 0):>7}/{m.get('total_code', 0):<7} bytes  "
          f"funcs {m.get('matched_functions', 0)}/{m.get('total_functions', 0)}")
