#!/usr/bin/env bash
set -euo pipefail

binary="${XENO_PORT_BINARY:-pc_port/build_native/xeno-port}"
logs=()
cleanup() {
    if [ "${#logs[@]}" -gt 0 ]; then
        rm -f "${logs[@]}"
    fi
}
trap cleanup EXIT

run_case() {
    local name="$1"
    local map="$2"
    local entrance="$3"
    local seconds="$4"
    local log
    local rc
    local video="${SDL_VIDEODRIVER:-x11}"
    local -a runner=()

    log="$(mktemp "/tmp/xeno-${name,,}-smoke.XXXXXX.log")"
    logs+=("$log")

    # SDL's dummy driver is not sufficient for this OpenGL path. In a display-
    # less CI shell, provide a real X11 framebuffer through Xvfb by default;
    # callers that set SDL_VIDEODRIVER explicitly retain that choice.
    if [ -z "${DISPLAY:-}" ] && [ -z "${SDL_VIDEODRIVER:-}" ]; then
        if ! command -v xvfb-run >/dev/null 2>&1; then
            echo "$name smoke: FAIL (DISPLAY is unset and xvfb-run is unavailable)" >&2
            exit 1
        fi
        runner=(xvfb-run -a)
        video=x11
    fi

    set +e
    "${runner[@]}" env \
        XENO_FIELD_TEST=1 \
        XENO_KERNEL_SEL=0 \
        XENO_FIELD_MAP="$map" \
        XENO_FIELD_ENTRANCE="$entrance" \
        XENO_FIELD_DIAG=1 \
        SDL_VIDEODRIVER="$video" \
        timeout --signal=TERM --kill-after=3 "${seconds}s" "$binary" >"$log" 2>&1
    rc=$?
    set -e

    case "$rc" in
        0|124|137|143) ;;
        *)
            sed -n '1,260p' "$log"
            echo "$name smoke: FAIL (runtime rc=$rc)" >&2
            exit 1
            ;;
    esac

    # The field loop intentionally remains alive under the smoke harness.
    # Require a real OT submission after the four-frame fade gate, plus an
    # actor entering the sprite renderer. The second assertion catches a boot-
    # global guard that leaves the background path alive while hiding actors.
    if ! rg -q '\[field-diag\] frame=[0-9]+ .*primSubmits=[1-9][0-9]* DrawOTag=1' "$log"; then
        sed -n '1,260p' "$log"
        echo "$name smoke: FAIL (no nonzero primitive submission)" >&2
        exit 1
    fi

    if ! rg -q '\[field-diag\] func_80075B44 frame=[0-9]+ active=[1-9][0-9]* plain=[1-9][0-9]* .*globalSkip=0' "$log"; then
        sed -n '1,260p' "$log"
        echo "$name smoke: FAIL (actor renderer globally skipped)" >&2
        exit 1
    fi

    echo "$name smoke: PASS (runtime rc=$rc; nonzero OT submission and actor draw observed)"
}

run_case Map0 0 0 12
run_case Map1 1 6 25
