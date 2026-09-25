#!/usr/bin/env python3
"""World-map retail -> port C coverage table.

Retail side: asm/world_map/120C.s (splat/spimdisasm output of disc/world_map.bin).
  - functions  = glabel entries (start address, size from .size / next glabel)
  - edges      = jal targets, %hi/%lo(func_X) materialisations in code,
                 and data tables: code that references a data label D_X whose
                 table (up to the next dlabel) holds `.word func_Y` gets an
                 edge to every such func_Y.
Port side: every C *definition* in pc_port/src/**/*.c whose name embeds a
world-map address (8 hex digits in [0x8006FAF0, 0x80099E8C)).  Test-scaffold
names (_should_not_run, _get_exec_count, ...) and stub_func_ names are
recorded as such, not as implementations.  A definition only counts as
LINKED when its file is in pc_port/build_port.sh's source list.

Usage: python3 .fanout/wmap_coverage.py [--md OUT.md] [--csv OUT.csv]
"""
import re, sys, pathlib, collections

root = pathlib.Path(__file__).resolve().parents[1]
ASM = root / "asm/world_map/120C.s"
DATA = [root / "asm/world_map/data/0.rodata.s", root / "asm/world_map/data/2A39C.data.s"]
LO, HI = 0x8006FAF0, 0x80099E8C

# ---------------------------------------------------------------- retail
funcs = collections.OrderedDict()   # name -> dict
cur = None
line_re = re.compile(r'/\* [0-9A-F]+ ([0-9A-F]{8}) [0-9A-F]{8} \*/\s+(\S+)\s*(.*)')
for ln in ASM.read_text().splitlines():
    m = re.match(r'glabel (\w+)', ln)
    if m:
        cur = {"name": m.group(1), "start": None, "end": None, "insns": 0,
               "jal": set(), "ptr": set(), "dref": set(), "jalr": 0, "ext": set()}
        funcs[cur["name"]] = cur
        continue
    if cur is None:
        continue
    m = line_re.search(ln)
    if not m:
        continue
    addr = int(m.group(1), 16)
    if cur["start"] is None:
        cur["start"] = addr
    cur["end"] = addr + 4
    cur["insns"] += 1
    op, args = m.group(2), m.group(3)
    if op == "jal":
        t = args.strip()
        cur["jal"].add(t)
    elif op == "jalr":
        cur["jalr"] += 1
    for f in re.findall(r'%(?:hi|lo)\((func_[0-9A-F]{8}|WorldMapMain)\)', args):
        cur["ptr"].add(f)
    for d in re.findall(r'%(?:hi|lo)\((D_[0-9A-F]{8})\)', args):
        cur["dref"].add(d)

addr2name = {f["start"]: n for n, f in funcs.items()}
name2addr = {n: f["start"] for n, f in funcs.items()}

# data tables
tables = collections.defaultdict(set)
cur_d = None
for p in DATA:
    for ln in p.read_text().splitlines():
        m = re.match(r'dlabel (\w+)', ln)
        if m:
            cur_d = m.group(1)
            continue
        m = re.search(r'\.word\s+(func_[0-9A-F]{8}|WorldMapMain|D_[0-9A-F]{8})\b', ln)
        if m and cur_d:
            tables[cur_d].add(m.group(1))
# tables of tables (.word D_X): flatten transitively to the functions reached
def _flat(lbl, seen):
    out = set()
    for s in tables.get(lbl, ()):
        if s.startswith("D_"):
            if s not in seen:
                seen.add(s)
                out |= _flat(s, seen)
        else:
            out.add(s)
    return out
tables = {k: _flat(k, {k}) for k in list(tables)}
data_ptr_targets = set().union(*tables.values()) if tables else set()

edges = collections.defaultdict(set)
callers = collections.defaultdict(set)
ext_calls = collections.Counter()
for n, f in funcs.items():
    tg = set()
    for t in f["jal"]:
        if t in funcs:
            tg.add(t)
        else:
            f["ext"].add(t)
            ext_calls[t] += 1
    tg |= {t for t in f["ptr"] if t in funcs}
    for d in f["dref"]:
        tg |= tables.get(d, set())
    edges[n] = tg
    for t in tg:
        callers[t].add(n)

reach = set()
stack = ["WorldMapMain"]
while stack:
    n = stack.pop()
    if n in reach:
        continue
    reach.add(n)
    stack.extend(edges[n] - reach)

# ---------------------------------------------------------------- port
bp = (root / "pc_port/build_port.sh").read_text()
linked_files = set(re.findall(r'pc_port/src/([\w./-]+\.c)', bp))
defn = re.compile(r'^(?!static\s+inline)([A-Za-z_][\w \t\*]*?)\b(\w*?([0-9A-Fa-f]{8})\w*)\s*\(([^;{)]*)\)\s*\{', re.M)
SCAFFOLD = re.compile(r'(should_not_run|should_reenter|exec_count|_test_|set_trace|set_probe|_instrumented|run_bounded)', re.I)
port = collections.defaultdict(list)   # addr -> [(kind, file, fname, linked)]
for p in sorted((root / "pc_port/src").rglob("*.c")):
    rel = str(p.relative_to(root / "pc_port/src"))
    t = p.read_text(errors="ignore")
    for m in defn.finditer(t):
        fname, hexs = m.group(2), m.group(3)
        a = int(hexs, 16)
        if not (LO <= a < HI):
            continue
        if fname.startswith("stub_"):
            kind = "stub"
        elif SCAFFOLD.search(fname):
            kind = "scaffold"
        elif fname.startswith("wm_sched_builtin_"):
            kind = "sched-thunk"
        else:
            kind = "impl" if re.fullmatch(r'(wm_|func_)[0-9A-Fa-f]{8}', fname) else "variant"
        port[a].append((kind, rel, fname, rel in linked_files))


# second pass: any definition (static or not, any name) whose immediately
# preceding comment block names a retail function start ("Retail 0x800831D8").
anydef = re.compile(r'^(?:static\s+)?(?!return|else|if|while|for|switch)[A-Za-z_][\w \t\*]*?\b(\w+)\s*\([^;{)]*\)\s*\{', re.M)
starts = {f["start"] for f in funcs.values()}
for p in sorted((root / "pc_port/src").rglob("*.c")):
    rel = str(p.relative_to(root / "pc_port/src"))
    t = p.read_text(errors="ignore")
    for m in anydef.finditer(t):
        fname = m.group(1)
        if re.search(r'[0-9A-Fa-f]{8}', fname) and re.fullmatch(r'(wm_|func_)[0-9A-Fa-f]{8}', fname):
            continue
        head = t[max(0, m.start() - 1500):m.start()].rstrip()
        if not head.endswith("*/"):
            continue
        cstart = head.rfind("/*")
        block = head[cstart:]
        found = [int(x, 16) for x in re.findall(r'(?:0x|\b)(8[0-9A-Fa-f]{7})\b', block)]
        found = [a for a in found if a in starts]
        if not found or SCAFFOLD.search(fname) or not re.search(r'etail|transcri', block):
            continue
        a = found[0]
        if any(e[2] == fname for e in port[a]):
            continue
        port[a].append(("variant", rel, fname + "[comment]", rel in linked_files))


# third pass: hand-verified implementations the name/comment heuristics cannot
# see (static helpers with no address in the name, bodies inlined into the
# caller).  Each entry was checked against the asm by reading both sides.
MANUAL = {
    0x80076BC4: ("world_map_callback_827c8.c", "m17c_command_0 [table D_8009A3C0[0], manual]"),
    0x80076BDC: ("world_map_callback_827c8.c", "m17c_command_1 [table[1], manual]"),
    0x80076C18: ("world_map_callback_827c8.c", "m17c_command_2 [table[2], manual]"),
    0x80076C3C: ("world_map_callback_827c8.c", "m17c_command_3 [table[3], manual]"),
    0x80076C68: ("world_map_callback_827c8.c", "m17c_command_4 [table[4], manual]"),
    0x80076C88: ("world_map_callback_827c8.c", "m17c_command_5 [table[5], manual]"),
    0x80076CB4: ("world_map_callback_827c8.c", "m17c_command_6 [table[6], manual]"),
    0x80076CD4: ("world_map_callback_827c8.c", "m17c_command_7 [table[7], manual]"),
    0x80076CF4: ("world_map_callback_827c8.c", "m17c_command_8 [table[8], manual]"),
    0x80076D1C: ("world_map_callback_827c8.c", "m17c_command_9 [table[9], manual]"),
    0x80076D50: ("world_map_callback_827c8.c", "m17c_command_10 [table[10], manual]"),
    0x80076D8C: ("world_map_callback_827c8.c", "m17c_command_11 [table[11], manual]"),
    0x8007474C: ("world_map_teardown_7299c.c", "inlined in wm_8007299C (3x td_free_value) [manual]"),
    0x80092DD0: ("world_map_teardown_7299c.c", "inlined in wm_8007299C (func_800346D4(D_8009D498)) [manual]"),
    0x800931B0: ("world_map_teardown_7299c.c", "inlined in wm_8007299C (func_800346D4(D_8009BD64)) [manual]"),
}
for a, (rel, fname) in MANUAL.items():
    port[a].append(("variant", rel, fname, rel in linked_files))


def owner(a):
    """retail function containing address a (for port entries not at a glabel)."""
    best = None
    for n, f in funcs.items():
        if f["start"] <= a < f["end"]:
            return n
    return best


def status(n):
    a = name2addr[n]
    ents = port.get(a, [])
    impl = [e for e in ents if e[0] in ("impl", "variant")]
    if any(e[3] for e in impl):
        return "PORTED", impl
    if impl:
        return "UNLINKED", impl
    if any(e[0] == "stub" for e in ents):
        return "STUB", ents
    return "MISSING", ents


rows = []
for n, f in funcs.items():
    st, ents = status(n)
    rows.append((f["start"], n, (f["end"] - f["start"]), sorted(callers[n]), st, ents,
                 n in reach, n in data_ptr_targets or any(n in funcs[c]["ptr"] for c in funcs)))

# port entries that are not at a splat function start
orphans = []
for a, ents in sorted(port.items()):
    if a in addr2name:
        continue
    impl = [e for e in ents if e[0] in ("impl", "variant")]
    if impl:
        orphans.append((a, owner(a), impl))


def fmt_ents(ents):
    out = []
    for kind, rel, fname, linked in ents:
        out.append(f"`{rel}`:{fname}" + ("" if linked else " (NOT LINKED)"))
    return "; ".join(out[:3]) + (f" (+{len(out)-3})" if len(out) > 3 else "")


def emit_md(path):
    cnt = collections.Counter(r[4] for r in rows)
    rc = collections.Counter(r[4] for r in rows if r[6])
    by = lambda st, rch: sum(r[2] for r in rows if r[4] == st and (r[6] or not rch))
    L = []
    L.append("# World-map (world_map.bin) retail -> port C coverage\n")
    L.append("Generated by `python3 .fanout/wmap_coverage.py --md .fanout/world_map_coverage.md`.\n")
    L.append("Retail: `asm/world_map/120C.s` (386 splat functions, 0x80070CFC..0x80099E8C). "
             "Port: C definitions in `pc_port/src/**/*.c` whose name embeds the function's address "
             "(`wm_XXXXXXXX` / `func_XXXXXXXX`), counted as PORTED only if the file is in "
             "`pc_port/build_port.sh`. Reachability = transitive closure from `WorldMapMain` over "
             "jal, `%hi/%lo(func_X)` materialisations and data-table (`.word func_X`) references.\n")
    L.append("## Summary\n")
    L.append("| status | all funcs | bytes | reachable from WorldMapMain | reachable bytes |")
    L.append("|---|---|---|---|---|")
    for st in ("PORTED", "UNLINKED", "STUB", "MISSING"):
        L.append(f"| {st} | {cnt[st]} | {by(st, False)} | {rc[st]} | {by(st, True)} |")
    L.append(f"| total | {len(rows)} | {sum(r[2] for r in rows)} | {len(reach)} | "
             f"{sum(r[2] for r in rows if r[6])} |\n")
    miss_r = [r for r in rows if r[6] and r[4] != "PORTED"]
    L.append(f"## Reachable functions with no linked port C ({len(miss_r)})\n")
    L.append("| addr | name | size | status | callers | notes |")
    L.append("|---|---|---|---|---|---|")
    for r in miss_r:
        cs = ", ".join(c.replace("func_", "") for c in r[3][:6]) + (" ..." if len(r[3]) > 6 else "")
        L.append(f"| {r[0]:08X} | {r[1]} | 0x{r[2]:X} | {r[4]} | {cs} | "
                 f"{'fn-ptr target; ' if r[7] else ''}{fmt_ents(r[5])} |")
    L.append("")
    if orphans:
        L.append(f"## Port bodies at addresses that are not splat function starts ({len(orphans)})\n")
        L.append("These are secondary entry points / mis-split starts; the owning splat function is listed.\n")
        L.append("| addr | inside retail func | port |")
        L.append("|---|---|---|")
        for a, o, ents in orphans:
            L.append(f"| {a:08X} | {o} | {fmt_ents(ents)} |")
        L.append("")
    L.append("## Full table\n")
    L.append("| addr | name | size | reach | status | callers | port C |")
    L.append("|---|---|---|---|---|---|---|")
    for r in rows:
        cs = ", ".join(c.replace("func_", "") for c in r[3][:5]) + (f" (+{len(r[3])-5})" if len(r[3]) > 5 else "")
        L.append(f"| {r[0]:08X} | {r[1]} | 0x{r[2]:X} | {'Y' if r[6] else ''}{'*' if r[7] else ''} | "
                 f"{r[4]} | {cs} | {fmt_ents(r[5])} |")
    L.append("\n`*` = function-pointer target (address materialised in code or stored in a data table).\n")
    L.append("## External (slus/kernel) callees from world_map code, by call-site count\n")
    L.append(", ".join(f"{k}({v})" for k, v in ext_calls.most_common()))
    pathlib.Path(path).write_text("\n".join(L) + "\n")


if __name__ == "__main__":
    args = sys.argv[1:]
    cnt = collections.Counter(r[4] for r in rows)
    rc = collections.Counter(r[4] for r in rows if r[6])
    print("funcs", len(rows), "reachable", len(reach), "status", dict(cnt), "reachable-status", dict(rc),
          "orphan-port-entries", len(orphans))
    if "--md" in args:
        emit_md(args[args.index("--md") + 1])
    if "--list" in args:
        for r in rows:
            if r[6] and r[4] != "PORTED":
                print(f"{r[0]:08X} {r[1]} size=0x{r[2]:X} {r[4]} callers={','.join(r[3])}")
