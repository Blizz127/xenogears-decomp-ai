#!/usr/bin/env bash
# Count SPU key-ons during the Id battle for each battle-BGM id.
# jtbl_80070A10 routes ids 1-4 to the real music loaders (0x801E range) and
# ids 0/5 to func_800B7870 (the no-special-music path); the warp inherits the
# field-load default of 5, which is why the fight was silent.
set -u
cd "$(dirname "$0")"
XA=/run/user/1000/xauth_FhtWoE
export DISPLAY=:0 XAUTHORITY=$XA XENO_DISPLAY=:0 XENO_XAUTHORITY=$XA
for bgm in "$@"; do
    log="bgm-sweep-$bgm.log"
    systemctl --user stop xeno-route23 >/dev/null 2>&1; sleep 2
    systemctl --user reset-failed xeno-route23 >/dev/null 2>&1
    cp map383-party3.xgqs warp-start.xgqs; : > warp.req
    env -u XENO_PS1_RAW_TEXTURE_IDENTITY systemd-run --user --unit=xeno-route23 \
        --collect --property=Type=simple \
        --property=WorkingDirectory=/var/home/blizz/Projects/xenogears-decomp-ai \
        --setenv=DISPLAY=:0 --setenv=XAUTHORITY=$XA --setenv=SDL_VIDEODRIVER=x11 \
        --setenv=XENO_FIELD_POS_DIAG=60 --setenv=XENO_SOUND_KON_TRACE=1 \
        --setenv=XENO_QUICKSAVE_PATH="$PWD/warp-start.xgqs" \
        --setenv=XENO_BATTLE_WARP_FILE="$PWD/warp.req" \
        "$PWD/route23-run.sh" "$PWD/$log" >/dev/null 2>&1
    sleep 12
    W=$(timeout 10 xdotool search --onlyvisible --name Xenogears | tail -1)
    xdotool windowactivate --sync "$W" 2>/dev/null
    for _ in 1 2 3 4; do xdotool key F8; sleep 6; done
    xdotool key z; sleep 28
    grep -q "FieldLoad begin field=383" "$log" || { echo "bgm=$bgm: no field"; continue; }
    pre=$(grep -c 'reg-flush] KON' "$log")
    echo "15:$bgm" > warp.req
    sleep 30
    post=$(grep -c 'reg-flush] KON' "$log")
    echo "bgm=$bgm  field_KON=$pre  battle_KON=$((post-pre))  $(grep -o 'bgm=[0-9]* [a-z]*' "$log" | tail -1)"
done
systemctl --user stop xeno-route23 >/dev/null 2>&1
