#!/usr/bin/env bash
# Reproducible bare field-test launcher for Map001.  Do not override its map
# selection through the environment: use run_map014.sh for Map014 instead.
set -euo pipefail

readonly map=1
readonly entrance=6
readonly binary="${XENO_PORT_BINARY:-pc_port/build_native/xeno-port}"

if [[ -n "${XENO_FIELD_MAP:-}" && "${XENO_FIELD_MAP}" != "${map}" ]]; then
    printf 'error: run_map001.sh requires XENO_FIELD_MAP=%s (got %s)\n' \
        "${map}" "${XENO_FIELD_MAP}" >&2
    exit 64
fi

export XENO_FIELD_TEST=1
export XENO_KERNEL_SEL=0
export XENO_FIELD_MAP="${map}"
export XENO_FIELD_ENTRANCE="${entrance}"
export SDL_VIDEODRIVER="${SDL_VIDEODRIVER:-x11}"
export DISPLAY="${DISPLAY:-:0}"

printf 'Launching Map001 (XENO_FIELD_MAP=%s, entrance=%s)\n' \
    "${XENO_FIELD_MAP}" "${XENO_FIELD_ENTRANCE}"

if [[ "${1:-}" == "--gdb" ]]; then
    [[ -n "${2:-}" && $# -eq 2 ]] || { echo 'usage: run_map001.sh --gdb <script.gdb>' >&2; exit 64; }
    exec distrobox enter xenogears-dev -- bash -lc \
        "cd '$PWD' && env XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 XENO_FIELD_MAP=${map} XENO_FIELD_ENTRANCE=${entrance} SDL_VIDEODRIVER=x11 DISPLAY='${DISPLAY}' gdb -q -batch -x '$2' '$binary'"
fi

if [[ "${1:-}" == "--opcode-sweep" ]]; then
    [[ -n "${2:-}" && -n "${3:-}" && $# -eq 3 && "${3}" =~ ^[1-9][0-9]*$ ]] || {
        echo 'usage: run_map001.sh --opcode-sweep <records.bin> <seconds>' >&2
        exit 64
    }
    printf 'Opcode-sweep records: %s (fd 3, %ss)\n' "$2" "$3"
    exec 3>"$2"
    set +e
    timeout --signal=TERM "${3}s" "${binary}"
    rc=$?
    set -e
    python3 scratchpad/opcode_sweep_records.py finalize "$2"
    exit "$rc"
fi

exec "${binary}" "$@"
