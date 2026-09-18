#!/usr/bin/env bash
# Differential prover for the adopted battle overlay host bodies.
#
# Runs every function in pc_port/src/battle_overlay_host_leaves.inc twice on the
# same guest RAM -- once as the retail MIPS bytes out of disc/battle.bin, once
# as the linked host C body -- and compares the result and the RAM that changed.
# See pc_port/tests/battle_overlay_host_differential_test.c for the contract and
# for what the comparison deliberately does not cover.
#
# Requires the port to have been built once (pc_port/build_native/stubs.c): the
# host bodies' callees and data come from the generated stub manifest, so this
# links the same definitions the shipped binary does.
set -euo pipefail
ulimit -c 0
cd "$(dirname "$0")/../.."

STUBS=pc_port/build_native/stubs.c
if [ ! -f "$STUBS" ]; then
    echo "BATTLE OVERLAY DIFFERENTIAL FAIL missing $STUBS" >&2
    echo "  Build the port first: ./pc_port/build_port.sh" >&2
    exit 1
fi

OUT=${BATTLE_OVERLAY_DIFFERENTIAL_OUT:-$(mktemp -d /tmp/xeno-overlay-diff.XXXXXXXX)}
mkdir -p "$OUT"
printf 'BATTLE OVERLAY DIFFERENTIAL artifacts=%s\n' "$OUT"

# The allowlist is the single source of truth for both the names and the TUs.
LEAVES=$(python3 tools/scripts/battle_overlay_host_tus.py --leaves)
TUS=$(python3 tools/scripts/battle_overlay_host_tus.py --verify)
python3 tools/scripts/battle_overlay_host_tus.py --leaf-types \
    > "$OUT/overlay_leaf_types.inc"

# A stale manifest that still stubs an adopted leaf would link two definitions
# of it; say so instead of failing with a wall of duplicate-symbol errors.
for leaf in $LEAVES; do
    if grep -q "\"$leaf\"" "$STUBS"; then
        echo "BATTLE OVERLAY DIFFERENTIAL FAIL $leaf is a generated stub in $STUBS" >&2
        echo "  The manifest is stale; re-run ./pc_port/build_port.sh" >&2
        exit 1
    fi
done

bridge_elf=()
if [ -f build/out/slus_006.64.elf ]; then
    bridge_elf+=(--elf build/out/slus_006.64.elf)
fi
python3 tools/scripts/gen_battle_bridge_map.py "${bridge_elf[@]}" \
    --symbols config/symbol_addrs.slus_006.64.txt \
    --symbols linker/undefined_funcs_auto.battle.txt \
    --symbols linker/undefined_syms_auto.battle.txt \
    --symbols config/symbol_addrs.battle.txt \
    --out "$OUT/battle_bridge_map.inc"

COMMON=(-std=gnu17 -fno-pie -DXENO_PC_PORT -DSKIP_ASM -D_LANGUAGE_C
    -DUSE_EXTENDED_PRIM_POINTERS=0 -include assert.h -ffunction-sections -fdata-sections
    -Ipc_port/include_shim -Iinclude -Ipc_port/src -I"$OUT"
    -Ipc_port/extern/PsyCross/include -Ipc_port/extern/PsyCross/include/psx)
# Must match pc_port/build_port.sh's GFLAGS: the body under test has to be the
# body the port ships.
HOST_FLAGS=(-std=gnu17 -fpermissive -DXENO_PC_PORT -DXENO_FIELD_OBJECT_OVERLAY
    -DXENO_BATTLE_OVERLAY_HOST_BODIES -DSKIP_ASM -D_LANGUAGE_C
    -DUSE_EXTENDED_PRIM_POINTERS=0 -include assert.h -w -O0 -g -m64 -fno-builtin)
HOST_INC=(-Ipc_port/include_shim -Iinclude -I"$OUT"
    -Ipc_port/extern/PsyCross/include -Ipc_port/extern/PsyCross/include/psx)

gcc -c "$STUBS" -O0 -g -o "$OUT/stubs.o"

# main12.c is the mutation target for the negative control: it owns
# func_80079934, whose whole body is `*pValue += 4`.
python3 - "$OUT" <<'PY'
from pathlib import Path
import sys
out = Path(sys.argv[1])
src = Path("src/battle/main12.c").read_text()
old = "*pValue += 4;"
assert src.count(old) == 1, src.count(old)
(out / "mutant_main12.c").write_text(src.replace(old, "*pValue += 5;", 1))
PY

mode_flags() {
    case "$1" in
        O0)    echo "-O0" ;;
        O2)    echo "-O2" ;;
        UBSan) echo "-O1 -fsanitize=undefined -fno-sanitize-recover=all -fno-sanitize=function" ;;
    esac
}

# build_link <mode> <host-object-dir> <main12 source>
build_link() {
    local mode="$1" hostdir="$2" main12="$3"
    local flags; flags=$(mode_flags "$mode")
    rm -rf "$hostdir"; mkdir -p "$hostdir"
    for tu in $TUS; do
        local src="$tu"
        [ "$tu" = "src/battle/main12.c" ] && src="$main12"
        gcc -c "$src" "${HOST_FLAGS[@]}" "${HOST_INC[@]}" \
            -o "$hostdir/$(basename "$tu" .c).o"
    done
    # shellcheck disable=SC2086
    clang "${COMMON[@]}" $flags -Wall -Wextra -Werror \
        -DBATTLE_RUNTIME_SOURCE="\"$PWD/pc_port/src/battle_mips_runtime.c\"" \
        -c pc_port/tests/battle_overlay_host_differential_test.c \
        -o "$OUT/$mode.test.o"
    clang "${COMMON[@]}" $flags -Wall -Wextra -Werror \
        -c pc_port/src/battle_mips_adapter.c -o "$OUT/$mode.cpu.o"
    # shellcheck disable=SC2086
    clang -no-pie -rdynamic $flags -Wl,--gc-sections \
        "$OUT/$mode.test.o" "$OUT/$mode.cpu.o" "$hostdir"/*.o "$OUT/stubs.o" \
        -ldl -o "$OUT/$mode$4"
}

# build_link <mode> <host-object-dir> <main12 source>
for mode in ${BATTLE_OVERLAY_DIFFERENTIAL_MODES:-O0 O2 UBSan}; do
    build_link "$mode" "$OUT/host-$mode" "src/battle/main12.c" ""
    status=0
    "$OUT/$mode" >"$OUT/$mode.log" 2>&1 || status=$?
    cat "$OUT/$mode.log"
    if [ "$status" -ne 0 ] || ! grep -q '^BATTLE OVERLAY DIFFERENTIAL PASS ' "$OUT/$mode.log"; then
        echo "BATTLE OVERLAY DIFFERENTIAL FAIL mode=$mode status=$status" >&2
        exit 1
    fi
done

# Negative control: a body that returns the wrong value must be rejected.
build_link O2 "$OUT/host-mutant" "$OUT/mutant_main12.c" "-mutant"
status=0
"$OUT/O2-mutant" >"$OUT/mutant.log" 2>&1 || status=$?
if [ "$status" -eq 0 ] || ! grep -q '^BATTLE OVERLAY DIFFERENTIAL FAIL ' "$OUT/mutant.log"; then
    cat "$OUT/mutant.log" >&2
    echo "BATTLE OVERLAY DIFFERENTIAL CONTROL FAILURE wrong-result body accepted" >&2
    exit 1
fi
grep -m1 '^BATTLE OVERLAY DIFFERENTIAL FAIL ' "$OUT/mutant.log" >&2

echo "BATTLE OVERLAY DIFFERENTIAL PASS (O0/O2/UBSan + wrong-result control rejected)"
