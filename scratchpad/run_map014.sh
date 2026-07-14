#!/usr/bin/env bash
# Reproducible bare field-test launcher for Map014.  Do not override its map
# selection through the environment: use run_map001.sh for Map001 instead.
set -euo pipefail

readonly map=14
readonly entrance=6
readonly binary="${XENO_PORT_BINARY:-pc_port/build_native/xeno-port}"

if [[ -n "${XENO_FIELD_MAP:-}" && "${XENO_FIELD_MAP}" != "${map}" ]]; then
    printf 'error: run_map014.sh requires XENO_FIELD_MAP=%s (got %s)\n' \
        "${map}" "${XENO_FIELD_MAP}" >&2
    exit 64
fi

export XENO_FIELD_TEST=1
export XENO_KERNEL_SEL=0
export XENO_FIELD_MAP="${map}"
export XENO_FIELD_ENTRANCE="${entrance}"
export SDL_VIDEODRIVER="${SDL_VIDEODRIVER:-x11}"
export DISPLAY="${DISPLAY:-:0}"

printf 'Launching Map014 (XENO_FIELD_MAP=%s, entrance=%s)\n' \
    "${XENO_FIELD_MAP}" "${XENO_FIELD_ENTRANCE}"
exec "${binary}" "$@"
