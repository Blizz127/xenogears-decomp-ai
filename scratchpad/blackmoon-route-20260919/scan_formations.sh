#!/usr/bin/env bash
# Capture one frame of each battle formation on a map, one game process per id.
#
# Winning each fight to get back to the field turned out to be the unreliable
# part: the attack ring needs AP the party may not have, and Cross opens the
# Deathblow list and traps a scripted driver there (observed on map 383
# formation 7).  A screenshot of the formation is all this needs, so it never
# fights -- it boots, warps, shoots, and kills the process.  Slower per id,
# but it cannot get stuck.
#
# usage: scan_formations.sh <save.xgqs> <first> <last> [outdir]
set -u
cd "$(dirname "$0")"
SAVE=$1; FIRST=$2; LAST=$3; OUT=${4:-battle-scan}
mkdir -p "$OUT"
export DISPLAY=:95 XENO_DISPLAY=:95

for id in $(seq "$FIRST" "$LAST"); do
    log="$OUT/scan-$id.log"
    systemctl --user stop xeno-route23 >/dev/null 2>&1
    sleep 2
    systemctl --user reset-failed xeno-route23 >/dev/null 2>&1
    cp "$SAVE" warp-start.xgqs
    : > warp.req
    env -u XENO_PS1_RAW_TEXTURE_IDENTITY systemd-run --user --unit=xeno-route23 \
        --collect --property=Type=simple \
        --property=WorkingDirectory=/var/home/blizz/Projects/xenogears-decomp-ai \
        --setenv=DISPLAY=:95 --setenv=SDL_VIDEODRIVER=x11 \
        --setenv=XENO_FIELD_POS_DIAG=60 --setenv=XENO_GOD_MODE=1 \
        --setenv=XENO_QUICKSAVE_PATH="$PWD/warp-start.xgqs" \
        --setenv=XENO_BATTLE_WARP_FILE="$PWD/warp.req" \
        "$PWD/route23-run.sh" "$PWD/$log" >/dev/null 2>&1

    # F8 must land before the title's idle timeout auto-confirms Continue into
    # the func_801D9F98 stub, which ends the run.
    for _ in 1 2 3 4; do sleep 6; python3 key.py F8 0.2 >/dev/null 2>&1; done
    sleep 4; python3 key.py z 0.2 >/dev/null 2>&1
    sleep 28

    if ! grep -q "FieldLoad begin field=" "$log" 2>/dev/null; then
        echo "id=$id: field never loaded"; continue
    fi
    echo "$id" > warp.req
    started=0
    for _ in $(seq 1 20); do
        sleep 2
        grep -q "battle-warp\] launching battle id=$id" "$log" && { started=1; break; }
    done
    if [ "$started" = 0 ]; then echo "id=$id: warp never fired"; continue; fi
    sleep 12
    timeout 20 import -window "$(timeout 10 xdotool search --onlyvisible --name Xenogears | tail -1)" \
        "$OUT/formation-$(printf %03d "$id").png" 2>/dev/null \
        && echo "id=$id: captured" || echo "id=$id: capture failed"
done
systemctl --user stop xeno-route23 >/dev/null 2>&1
echo done
