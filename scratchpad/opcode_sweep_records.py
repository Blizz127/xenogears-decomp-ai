#!/usr/bin/env python3
"""Finalize and report fixed-size XENO_DIAG_OPCODE_SWEEP records."""

import collections
import struct
import sys
from pathlib import Path

EVENT = struct.Struct("<IIIhbB")
SENTINEL = struct.Struct("<4sIII")
EVENT_TAG = int.from_bytes(b"OPSW", "little")
TARGETS = {0x85, 0x8E, 0x98, 0xBE, 0xC8, 0xD4, 0xE2, 0xFA}


def read_events(path):
    data = path.read_bytes()
    if len(data) % EVENT.size:
        raise SystemExit(f"error: {path} has partial record ({len(data)} bytes)")

    sentinel = None
    if len(data) >= SENTINEL.size and data[-SENTINEL.size : -12] == b"END!":
        sentinel = SENTINEL.unpack_from(data, len(data) - SENTINEL.size)
        data = data[:-SENTINEL.size]

    events = []
    for offset in range(0, len(data), EVENT.size):
        event = EVENT.unpack_from(data, offset)
        if event[0] != EVENT_TAG:
            raise SystemExit(
                f"error: {path} record {offset // EVENT.size} has tag 0x{event[0]:08x}"
            )
        events.append(event)
    return events, sentinel


def counts(events):
    invalid_actor = sum(event[3] < 0 for event in events)
    invalid_script = sum(event[4] < 0 for event in events)
    return len(events), invalid_actor, invalid_script


def finalize(path):
    events, sentinel = read_events(path)
    if sentinel is not None:
        raise SystemExit(f"error: {path} is already finalized")
    total, invalid_actor, invalid_script = counts(events)
    with path.open("ab") as stream:
        stream.write(SENTINEL.pack(b"END!", total, invalid_actor, invalid_script))
    print(
        f"OPCODE_SWEEP_SENTINEL total={total} "
        f"invalid_actor={invalid_actor} invalid_script={invalid_script}"
    )


def report(path):
    events, sentinel = read_events(path)
    if sentinel is None:
        raise SystemExit(f"error: {path} has no END! sentinel")
    expected = counts(events)
    actual = sentinel[1:]
    if actual != expected:
        raise SystemExit(f"error: {path} sentinel {actual} != observed {expected}")

    print(
        f"SENTINEL total={actual[0]} invalid_actor={actual[1]} "
        f"invalid_script={actual[2]}"
    )
    observed = collections.Counter((event[5], event[3], event[4]) for event in events)
    for (opcode, actor, script), count in sorted(observed.items()):
        if opcode in TARGETS:
            print(
                f"TARGET opcode=0x{opcode:02X} count={count} "
                f"actor={actor} script={script}"
            )
    print("OPCODES " + " ".join(f"{opcode:02X}" for opcode in sorted({e[5] for e in events})))


def main():
    if len(sys.argv) != 3 or sys.argv[1] not in {"finalize", "report"}:
        raise SystemExit(f"usage: {sys.argv[0]} finalize|report records.bin")
    path = Path(sys.argv[2])
    if sys.argv[1] == "finalize":
        finalize(path)
    else:
        report(path)


if __name__ == "__main__":
    main()
