#!/usr/bin/env python3
"""Generate the address/name table used by the retail battle MIPS adapter.

Addresses come from splat symbol_addrs files (the retail link map).  ELF input
is used only to classify symbol names as code or data; its current section
layout is deliberately not used as address authority.
"""

import argparse
import re
import subprocess


ASSIGN_RE = re.compile(
    r"^\s*([A-Za-z_.$][\w.$]*)\s*=\s*(0x[0-9A-Fa-f]+)\s*;"
    r"(?:.*?\bsize:\s*(0x[0-9A-Fa-f]+|\d+))?"
)


def elf_function_names(paths):
    names = set()
    for path in paths:
        output = subprocess.run(
            ["readelf", "-sW", path], check=True, text=True,
            capture_output=True
        ).stdout
        for line in output.splitlines():
            fields = line.split()
            if len(fields) >= 8 and fields[0].endswith(":"):
                if fields[3] == "FUNC" and fields[6] != "UND":
                    names.add(fields[7])
    return names


def parse_symbols(paths, functions):
    by_key = {}
    for path in paths:
        with open(path, encoding="utf-8") as source:
            for line in source:
                match = ASSIGN_RE.match(line)
                if not match:
                    continue
                name = match.group(1)
                if name.startswith(".L") or name.startswith("L8"):
                    continue
                address = int(match.group(2), 0)
                size = int(match.group(3), 0) if match.group(3) else 0
                is_function = (
                    name in functions or name.startswith("func_") or
                    name in {
                        "bzero", "memcpy", "memmove", "memset", "printf",
                        "rand", "ratan2", "rcos", "rsin",
                    }
                )
                key = (address, name)
                old = by_key.get(key)
                if old is None or size > old[0]:
                    by_key[key] = (size, is_function)
    return sorted(
        ((address, name, size, is_function)
         for (address, name), (size, is_function) in by_key.items()),
        key=lambda row: (row[0], not row[3], row[1]),
    )


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--elf", action="append", default=[])
    parser.add_argument("--symbols", action="append", required=True)
    parser.add_argument("--out", required=True)
    args = parser.parse_args()

    functions = elf_function_names(args.elf)
    symbols = parse_symbols(args.symbols, functions)
    with open(args.out, "w", encoding="utf-8") as out:
        out.write("/* Generated from retail symbol maps; do not edit. */\n")
        out.write("static const PcPortBattleSymbol g_BattleBridgeSymbols[] = {\n")
        for address, name, size, is_function in symbols:
            escaped = name.replace("\\", "\\\\").replace('"', '\\"')
            out.write(
                f'    {{ 0x{address:08x}u, {size}u, '
                f'{1 if is_function else 0}, "{escaped}" }},\n'
            )
        out.write("};\n")
        out.write(
            "static const size_t g_BattleBridgeSymbolCount = "
            "sizeof(g_BattleBridgeSymbols) / sizeof(g_BattleBridgeSymbols[0]);\n"
        )


if __name__ == "__main__":
    main()
