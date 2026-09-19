#!/usr/bin/env python3
"""Annotate [vm-trace] lines with the handler symbol each opcode dispatches to.

The port's tracer (src/field/scripts/virtual_machine.c, XENO_VM_TRACE) prints
raw opcode bytes, which are unreadable without cross-referencing the two
dispatch tables by hand.  Those tables are declared as ordered
FIELD_VM_HANDLER(name) lines in pc_port/src/data_field.c, so the mapping can be
recovered offline -- no change to the port and nothing to keep in sync at
runtime.

    usage: vm_trace_annotate.py <log> [--loop] [--actor N]

--loop additionally reports, per actor, the repeating instruction cycle: the
tail of the trace collapsed into its distinct ip sequence with repeat counts,
which is what identifies a stalled script's wait condition.
"""
import argparse
import re
from collections import Counter, OrderedDict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DATA_FIELD = ROOT / "pc_port/src/data_field.c"
LINE_RE = re.compile(
    r"\[vm-trace\] (?:#(?P<seq>\d+) )?actor=(?P<actor>\d+) ip=(?P<ip>\d+) "
    r"op=(?P<op>FE[0-9A-Fa-f]{2}|[0-9A-Fa-f]{2}) args=(?P<args>.*)"
)


def load_tables():
    """Return (handlers, handlers2) as index -> symbol name."""
    text = DATA_FIELD.read_text()
    tables = {}
    current = None
    for line in text.splitlines():
        m = re.search(r'"(g_FieldScriptVMHandlers2?):', line)
        if m:
            current = m.group(1)
            tables[current] = []
            continue
        m = re.match(r"FIELD_VM_HANDLER\(([^)]+)\)", line.strip())
        if m and current is not None:
            tables[current].append(m.group(1))
    return tables.get("g_FieldScriptVMHandlers", []), tables.get(
        "g_FieldScriptVMHandlers2", []
    )


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("log")
    ap.add_argument("--loop", action="store_true")
    ap.add_argument("--actor", type=int, default=None)
    ap.add_argument("--tail", type=int, default=400)
    args = ap.parse_args()

    t1, t2 = load_tables()

    def name_for(op):
        if op.upper().startswith("FE"):
            idx = int(op[2:], 16)
            return f"FE{idx:02X} {t2[idx] if idx < len(t2) else '<oob>'}"
        idx = int(op, 16)
        return f"{idx:02X} {t1[idx] if idx < len(t1) else '<oob>'}"

    per_actor = OrderedDict()
    out = []
    for raw in Path(args.log).read_text(errors="replace").splitlines():
        m = LINE_RE.search(raw)
        if not m:
            continue
        actor = int(m.group("actor"))
        if args.actor is not None and actor != args.actor:
            continue
        ip = int(m.group("ip"))
        label = name_for(m.group("op"))
        seq = m.group("seq") or "-"
        out.append(f"#{seq:>7} actor={actor:<3} ip={ip:<6} {label:<40} args={m.group('args')}")
        per_actor.setdefault(actor, []).append((ip, label))

    print("\n".join(out))

    if args.loop:
        for actor, steps in per_actor.items():
            print(f"\n=== actor {actor}: {len(steps)} dispatches ===")
            counts = Counter(ip for ip, _ in steps)
            print("hottest ips:")
            for ip, n in counts.most_common(12):
                label = next(lbl for i, lbl in steps if i == ip)
                print(f"  ip={ip:<6} x{n:<6} {label}")
            # Collapse the tail into its run-length-encoded ip sequence: a
            # stalled script shows up as a short cycle repeating forever.
            tail = steps[-args.tail:]
            rle = []
            for ip, label in tail:
                if rle and rle[-1][0] == ip:
                    rle[-1][2] += 1
                else:
                    rle.append([ip, label, 1])
            print(f"tail cycle ({len(tail)} dispatches, run-length encoded):")
            for ip, label, n in rle:
                suffix = f" x{n}" if n > 1 else ""
                print(f"  ip={ip:<6} {label}{suffix}")


if __name__ == "__main__":
    main()
