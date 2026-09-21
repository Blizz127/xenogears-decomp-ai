#!/usr/bin/env bash
# Boot every field map with the REAL party and record what breaks.
#
# The 2026-09-08 Lahan survey could only cover maps 0-20 and had to use the
# XENO_FIELD_TEST harness, which has no real party -- so anything gated on party
# state read as a failure that was really a harness artifact.  The checkpoint
# map warp (xgqs_map.py) removes that limit: it drops the actual save onto any
# map id the loader accepts (< 0x400), so every map can be surveyed under the
# same conditions a player would see.
#
# One game process serves many maps: PcPort_QuickCheckpointRestore re-reads the
# checkpoint file on every F8, so rewriting the file and pressing F8 again is a
# map change, not a restart.  That is ~25s per map instead of ~90s.  The process
# is restarted only when it dies (an assert or a fault), which is itself one of
# the results being recorded.
#
# usage: map_survey.sh <base.xgqs> <first> <last> [outdir]
set -u
cd "$(dirname "$0")"
BASE=$1; FIRST=$2; LAST=$3; OUT=${4:-survey}
mkdir -p "$OUT/shots"
XA=/run/user/1000/xauth_FhtWoE
export DISPLAY=:95 XENO_DISPLAY=:95
RESULTS="$OUT/results.tsv"
[ -f "$RESULTS" ] || printf 'map\tstatus\tactors\tdefects\n' > "$RESULTS"
LOG="$PWD/$OUT/run.log"

start_game() {
    systemctl --user stop xeno-route23 >/dev/null 2>&1; sleep 2
    systemctl --user reset-failed xeno-route23 >/dev/null 2>&1
    : > "$LOG"
    env -u XENO_PS1_RAW_TEXTURE_IDENTITY systemd-run --user --unit=xeno-route23 \
        --collect --property=Type=simple \
        --property=WorkingDirectory=/var/home/blizz/Projects/xenogears-decomp-ai \
        --setenv=DISPLAY=:95 --setenv=SDL_VIDEODRIVER=x11 \
        --setenv=XENO_FIELD_POS_DIAG=120 --setenv=XENO_GOD_MODE=1 \
        --setenv=XENO_FEI_HD2D=0 \
        --setenv=XENO_QUICKSAVE_PATH="$PWD/warp-start.xgqs" \
        "$PWD/route23-run.sh" "$LOG" >/dev/null 2>&1
    # F8 must beat the title's idle timeout or the menu auto-confirms Continue
    # into the func_801D9F98 stub and the run is dead.  Poll rather than sleep
    # fixed amounts: almost every map in this survey needs a restart (most
    # maps' scenes never free the player, so the in-process F8 path rarely
    # works), which made fixed sleeps the single biggest cost in the run.
    local want=${1:-}
    for _ in $(seq 1 10); do
        sleep 3
        grep -q "quick\] load queued" "$LOG" 2>/dev/null && break
        python3 key.py F8 0.2 >/dev/null 2>&1
    done
    python3 key.py z 0.2 >/dev/null 2>&1
    for _ in $(seq 1 20); do
        sleep 2
        alive || break
        if [ -n "$want" ]; then
            grep -q "FieldLoad begin field=$want " "$LOG" 2>/dev/null && break
        else
            grep -q "FieldLoad begin field=" "$LOG" 2>/dev/null && break
        fi
    done
}

alive() { systemctl --user is-active xeno-route23 >/dev/null 2>&1; }

python3 xgqs_map.py "$BASE" warp-start.xgqs "$FIRST" >/dev/null
start_game

for map in $(seq "$FIRST" "$LAST"); do
    alive || { echo "restarting game before map $map" >&2; \
               python3 xgqs_map.py "$BASE" warp-start.xgqs "$map" >/dev/null; start_game; }
    mark=$(wc -l < "$LOG" 2>/dev/null || echo 0)
    python3 xgqs_map.py "$BASE" warp-start.xgqs "$map" >/dev/null
    python3 key.py F8 0.2 >/dev/null 2>&1
    loaded=0
    # Short window, then fall back to a restart.  A map whose scene never frees
    # the player (map 0 is one) leaves the queued load "waiting for free field
    # control" forever, and every later F8 in that process is ignored too -- so
    # a long wait here does not help and silently mislabels working maps as
    # NOLOAD.  A restart boots straight into the target map instead of queueing.
    for _ in $(seq 1 5); do
        sleep 2
        tail -n +"$mark" "$LOG" 2>/dev/null | grep -q "FieldLoad begin field=$map " && { loaded=1; break; }
        alive || break
    done
    if [ "$loaded" = 0 ]; then
        start_game "$map"
        mark=1
        grep -q "FieldLoad begin field=$map " "$LOG" 2>/dev/null && loaded=1
    fi
    sleep 5
    seg=$(tail -n +"$mark" "$LOG" 2>/dev/null)
    if ! alive; then
        status=DIED
    elif [ "$loaded" = 1 ]; then
        status=LOADED
    else
        status=NOLOAD
    fi
    actors=$(printf '%s' "$seg" | grep -oE "model-build count=[0-9]+ / actors=[0-9]+" | tail -1)
    defects=$(printf '%s' "$seg" | grep -oE "sprite_animation_unimplemented|Assertion|unresolved native call target=0x[0-9a-f]+|\[stub\] [A-Za-z_0-9]+|write[0-9]+ fault|SEGV" | sort -u | tr '\n' ',' )
    printf '%s\t%s\t%s\t%s\n' "$map" "$status" "${actors:--}" "${defects:--}" >> "$RESULTS"
    echo "map $map: $status ${defects:+[$defects]}"
    if [ "$status" = LOADED ]; then
        timeout 20 import -window "$(timeout 10 xdotool search --onlyvisible --name Xenogears | tail -1)" \
            "$OUT/shots/map-$(printf %04d "$map").png" >/dev/null 2>&1
    fi
    if [ "$status" = DIED ]; then
        cp "$LOG" "$OUT/died-$map.log" 2>/dev/null
    fi
done
systemctl --user stop xeno-route23 >/dev/null 2>&1
echo survey-done
