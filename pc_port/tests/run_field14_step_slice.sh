#!/usr/bin/env bash
# Field14 movement slice, headless: legitimate earned fixture -> ordinary
# quick load at the title (XENO_QUICKLOAD_ON_TITLE, the F12 request) ->
# field 14 -> mask-preserving field-step pad input (XENO_FIELD_STEP_PAD_INPUT)
# -> stop.  Run once as the hybrid reference and once with the CPU interpreter
# disabled; field14_step_slice_summary.py compares them.
#
#   run_field14_step_slice.sh BINARY FIXTURE.xgqs OUTDIR reference|disabled
#
# No console, dev menu, warp, FIELD_TEST, cheat or raw vblank schedule.
# Headless: SDL offscreen, null audio (no audio evidence).
set -u
BINARY=$1; FIXTURE=$2; OUT=$3; MODE=$4
case "$MODE" in reference) DISABLED=0 ;; disabled) DISABLED=1 ;; *) exit 2 ;; esac
mkdir -p "$OUT"/home "$OUT"/config "$OUT"/state "$OUT"/memcards "$OUT"/captures
cp "$FIXTURE" "$OUT/quicksave.xgqs"
: > "$OUT/settings.ini"
# Steps count completed FieldPollControllers drains after arming in field 14:
# settle, Right, settle, Left, settle, Down, settle.
SCHEDULE="0:0,300:0x2000,360:0,480:0x8000,540:0,660:0x4000,720:0"
printf '%s\n' "$SCHEDULE" > "$OUT/schedule.txt"
env -i PATH="$PATH" HOME="$OUT/home" XDG_CONFIG_HOME="$OUT/config" XDG_STATE_HOME="$OUT/state" \
    SDL_VIDEODRIVER=offscreen EGL_PLATFORM=surfaceless ALSOFT_DRIVERS=null SDL_AUDIODRIVER=dummy \
    LIBGL_ALWAYS_SOFTWARE=1 XENO_NO_DIALOG=1 XENO_MODS_ENABLED=0 \
    XENO_CONFIG="$OUT/settings.ini" XENO_MEMCARD_DIR="$OUT/memcards" \
    XENO_DATA_DIR="$PWD/disc" XENO_DISC="$PWD/disc/disc1.bin" \
    XENO_QUICKSAVE_PATH="$OUT/quicksave.xgqs" XENO_QUICKLOAD_ON_TITLE=1 \
    XENO_CHEATS_CONSOLE=0 XENO_DEV_MENU=0 XENO_DEV_MENU_UNVERIFIED=0 \
    XENO_DISABLE_CPU_INTERPRETER=$DISABLED XENO_CPU_EXECUTION_STATS=1 XENO_CPU_FALLBACK_STATS=1 \
    XENO_FIELD_DIAG=1 XENO_FIELD_POS_DIAG=10 \
    XENO_FIELD_STEP_PAD_INPUT="$SCHEDULE" XENO_FIELD_STEP_PAD_MAP=14 \
    XENO_FIELD_CAPTURE_DIR="$OUT/captures" XENO_FIELD_CAPTURE_EVERY=600 \
    timeout --signal=INT --kill-after=20 "${FIELD14_SLICE_SECONDS:-600}s" "$BINARY" > "$OUT/run.log" 2>&1
echo "rc=$?" > "$OUT/rc.txt"
cat "$OUT/rc.txt"
