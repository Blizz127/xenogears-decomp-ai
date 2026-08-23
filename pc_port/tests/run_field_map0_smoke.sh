#!/usr/bin/env bash
set -euo pipefail

binary="${XENO_PORT_BINARY:-pc_port/build_native/xeno-port}"
log="$(mktemp /tmp/xeno-map0-smoke.XXXXXX.log)"
trap 'rm -f "$log"' EXIT

set +e
env XENO_FIELD_TEST=1 \
    XENO_KERNEL_SEL=0 \
    XENO_FIELD_MAP=0 \
    XENO_FIELD_ENTRANCE=0 \
    XENO_FIELD_DIAG=1 \
    SDL_VIDEODRIVER="${SDL_VIDEODRIVER:-x11}" \
    DISPLAY="${DISPLAY:-:0}" \
    timeout --signal=TERM --kill-after=3 12s "$binary" >"$log" 2>&1
rc=$?
set -e

case "$rc" in
    0|124|137|143) ;;
    *)
        sed -n '1,220p' "$log"
        echo "Map0 smoke: FAIL (runtime rc=$rc)" >&2
        exit 1
        ;;
esac

# The field loop intentionally remains alive under the smoke harness. Require
# a real OT submission after the four-frame fade gate, plus at least one actor
# entering the sprite renderer. The second assertion catches a boot-global
# guard that can leave the background path alive while hiding every actor.
if ! rg -q '\[field-diag\] frame=[0-9]+ .*primSubmits=[1-9][0-9]* DrawOTag=1' "$log"; then
    sed -n '1,220p' "$log"
    echo "Map0 smoke: FAIL (no nonzero primitive submission)" >&2
    exit 1
fi

if ! rg -q '\[field-diag\] func_80075B44 frame=[0-9]+ active=[1-9][0-9]* plain=[1-9][0-9]* .*globalSkip=0' "$log"; then
    sed -n '1,220p' "$log"
    echo "Map0 smoke: FAIL (actor renderer globally skipped)" >&2
    exit 1
fi

echo "Map0 smoke: PASS (runtime rc=$rc; nonzero OT submission and actor draw observed)"
