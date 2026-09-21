#!/usr/bin/env python3
"""Read or rewrite the map id in a quick-checkpoint (.xgqs).

The checkpoint carries the field map id, the entrance and the player's 16.16
world position; PcPort_QuickCheckpointRestore writes the map into game state
(quick_checkpoint.c:225) and only rejects ids >= 0x400, so retargeting a save
drops the SAME party and game state onto any field the game can load.  That is
the only way to reach a map the run cannot yet walk to -- the world map does
not move the party yet, so everything past Blackmoon Forest is unreachable on
foot.

Format from pc_port/src/quick_checkpoint_file.c: a 48-byte header
("XGQCKPT\0", version, header size, state size, checksum at +20, then map,
entrance, position, rotation) followed by 0x2358 bytes of game state.  The
checksum is FNV-1a over header[24:48] + the whole game state, so any edit has
to recompute it -- a raw byte poke is rejected with "load rejected: missing or
invalid".

usage: xgqs_map.py <file>                    -- show map/entrance/position
       xgqs_map.py <src> <dst> <map> [ent]   -- rewrite (position zeroed so the
                                               destination places the party at
                                               its own entrance)
"""
import struct
import sys

HEADER = 48
STATE = 0x2358
OFF_CHECKSUM = 20
OFF_MAP = 24
OFF_ENTRANCE = 26
OFF_POS = 28
MAGIC = b"XGQCKPT\0"


def checksum(buf):
    h = 2166136261
    for b in buf[OFF_MAP:HEADER] + buf[HEADER:HEADER + STATE]:
        h = ((h ^ b) * 16777619) & 0xFFFFFFFF
    return h


def show(path):
    b = open(path, "rb").read()
    m, e = struct.unpack_from("<HH", b, OFF_MAP)
    x, y, z = struct.unpack_from("<iii", b, OFF_POS)
    ok = b[:8] == MAGIC and struct.unpack_from("<I", b, OFF_CHECKSUM)[0] == checksum(b)
    print(f"{path}: map={m} entrance={e} pos=({x >> 16},{y >> 16},{z >> 16}) "
          f"checksum={'ok' if ok else 'BAD'}")


def rewrite(src, dst, mapid, entrance):
    b = bytearray(open(src, "rb").read())
    if not 0 <= mapid < 0x400:
        raise SystemExit(f"map {mapid} out of range (restore rejects >= 0x400)")
    struct.pack_into("<HH", b, OFF_MAP, mapid, entrance)
    struct.pack_into("<iii", b, OFF_POS, 0, 0, 0)
    struct.pack_into("<I", b, OFF_CHECKSUM, checksum(bytes(b)))
    open(dst, "wb").write(bytes(b))
    show(dst)


if __name__ == "__main__":
    if len(sys.argv) == 2:
        show(sys.argv[1])
    elif len(sys.argv) in (4, 5):
        rewrite(sys.argv[1], sys.argv[2], int(sys.argv[3]),
                int(sys.argv[4]) if len(sys.argv) == 5 else 0)
    else:
        raise SystemExit(__doc__)
