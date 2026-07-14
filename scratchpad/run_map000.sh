#!/usr/bin/env bash
# Reproducible bare field-test launcher for Map000.
set -euo pipefail

readonly map=0
# Map000's script spawn table has exactly two records (entrances 0 and 1).
# The ABR witness uses the first legal direct-entry record.
readonly entrance=0
readonly binary="${XENO_PORT_BINARY:-pc_port/build_native/xeno-port}"

if [[ -n "${XENO_FIELD_MAP:-}" && "${XENO_FIELD_MAP}" != "${map}" ]]; then
    printf 'error: run_map000.sh requires XENO_FIELD_MAP=%s (got %s)\n' \
        "${map}" "${XENO_FIELD_MAP}" >&2
    exit 64
fi

if [[ -n "${XENO_FIELD_ENTRANCE:-}" && "${XENO_FIELD_ENTRANCE}" != "${entrance}" ]]; then
    printf 'error: run_map000.sh requires XENO_FIELD_ENTRANCE=%s (got %s)\n' \
        "${entrance}" "${XENO_FIELD_ENTRANCE}" >&2
    exit 64
fi

export XENO_FIELD_TEST=1
export XENO_KERNEL_SEL=0
export XENO_FIELD_MAP="${map}"
export XENO_FIELD_ENTRANCE="${entrance}"
export SDL_VIDEODRIVER="${SDL_VIDEODRIVER:-x11}"
export DISPLAY="${DISPLAY:-:0}"

printf 'Launching Map000 (XENO_FIELD_MAP=%s, entrance=%s)\n' \
    "${XENO_FIELD_MAP}" "${XENO_FIELD_ENTRANCE}"

if [[ "${1:-}" == "--gdb" ]]; then
    [[ -n "${2:-}" && $# -eq 2 ]] || { echo 'usage: run_map000.sh --gdb <script.gdb>' >&2; exit 64; }
    exec distrobox enter xenogears-dev -- bash -lc \
        "cd '$PWD' && env XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 XENO_FIELD_MAP=${map} XENO_FIELD_ENTRANCE=${entrance} SDL_VIDEODRIVER=x11 DISPLAY='${DISPLAY}' gdb -q -batch -x '$2' '$binary'"
fi


exec "${binary}" "$@"
