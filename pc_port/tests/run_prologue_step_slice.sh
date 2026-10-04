#!/usr/bin/env bash
# Prologue narration slice, headless: retail boot -> title (map 490) -> New
# Game -> prologue narration (map 4), with no fixture and no save loaded.
# The boot and title are driven by the raw pad schedule (XENO_PAD_TEST_INPUT,
# clocked by Vsync-shim calls, as run_title_newgame_smoke.sh does). It
# retires when map 4 loads (XENO_PAD_TEST_STOP_FIELD=4) and hands over to
# the drain-clocked field-step probe armed on map 4, which pages the
# narration. Run once as the hybrid reference and once with the CPU
# interpreter disabled.
#
#   run_prologue_step_slice.sh BINARY OUTDIR reference|disabled
#
# No console, dev menu, warp, FIELD_TEST, cheat or save. Headless: SDL
# offscreen, null audio device. Evidence per run: text-box show/hide log with
# step numbers, checkpoint frames (captures/step-*.png), the per-vblank SPU
# register-page hash log (spu.log) and the field-diag map transitions. The
# run ends by itself at PROLOGUE_EXIT_AT (a field step), which must lie
# before the opening battle: battle code still runs on the interpreter.
set -u
BINARY=$1; OUT=$2; MODE=$3
case "$MODE" in reference) DISABLED=0 ;; disabled) DISABLED=1 ;; *) exit 2 ;; esac
mkdir -p "$OUT"/home "$OUT"/config "$OUT"/state "$OUT"/memcards "$OUT"/captures
: > "$OUT/settings.ini"
# Vsync-shim frames (title smoke defaults): skip the opening STR, interrupt the
# title attract, open the title menu, Up to New Game, confirm. Values are game
# buttons (Circle 0x20, Cross 0x40, Up 0x1000).
UP_HOLD="${PROLOGUE_UP_HOLD:-2}"
BOOT="${PROLOGUE_BOOT_SCHEDULE:-0:0,300:0x40,320:0,3700:0x20,3702:0,4500:0x20,4502:0,4900:0x1000,$((4900 + UP_HOLD)):0,5000:0x20,5002:0}"
# Field steps in map 4: Circle for 2 steps every 16 from step 60.
if [ -z "${PROLOGUE_SCHEDULE:-}" ]; then
    PROLOGUE_SCHEDULE="0:0"
    s=60
    while [ "$s" -lt "${PROLOGUE_PAGE_UNTIL:-4000}" ]; do
        PROLOGUE_SCHEDULE+=",$s:0x20,$((s + 2)):0"
        s=$((s + ${PROLOGUE_PAGE_EVERY:-16}))
    done
fi
CHECKPOINTS="${PROLOGUE_CHECKPOINTS:-100,300,500,700,900,1100,1300,1500,1700,1900}"
printf '%s\n' "$BOOT" > "$OUT/boot-schedule.txt"
printf '%s\n' "$PROLOGUE_SCHEDULE" > "$OUT/schedule.txt"
printf '%s\n' "$CHECKPOINTS" > "$OUT/checkpoints.txt"
env -i PATH="$PATH" HOME="$OUT/home" XDG_CONFIG_HOME="$OUT/config" XDG_STATE_HOME="$OUT/state" \
    SDL_VIDEODRIVER=offscreen EGL_PLATFORM=surfaceless ALSOFT_DRIVERS=null SDL_AUDIODRIVER=dummy \
    LIBGL_ALWAYS_SOFTWARE=1 XENO_NO_DIALOG=1 XENO_MODS_ENABLED=0 \
    XENO_CONFIG="$OUT/settings.ini" XENO_MEMCARD_DIR="$OUT/memcards" \
    XENO_DATA_DIR="$PWD/disc" XENO_DISC="$PWD/disc/disc1.bin" \
    XENO_QUICKSAVE_PATH="$OUT/quicksave.xgqs" \
    XENO_CHEATS_CONSOLE=0 XENO_DEV_MENU=0 XENO_DEV_MENU_UNVERIFIED=0 \
    XENO_SOUND_TICK_LOCKSTEP="${XENO_SOUND_TICK_LOCKSTEP:-0}" \
    XENO_DETERMINISTIC_VBLANK="${XENO_DETERMINISTIC_VBLANK:-0}" \
    XENO_FIELD_STEP_EXIT_AT="${PROLOGUE_EXIT_AT:-2000}" \
    XENO_FIELD_TEXTBOX_LOG=1 \
    XENO_DISABLE_CPU_INTERPRETER=$DISABLED XENO_CPU_EXECUTION_STATS=1 XENO_CPU_FALLBACK_STATS=1 \
    XENO_FIELD_DIAG=1 XENO_FIELD_POS_DIAG=10 \
    XENO_PAD_TEST_INPUT="$BOOT" XENO_PAD_TEST_STOP_FIELD=4 \
    XENO_FIELD_STEP_PAD_INPUT="$PROLOGUE_SCHEDULE" XENO_FIELD_STEP_PAD_MAP=4 \
    XENO_FIELD_STEP_CHECKPOINTS="$CHECKPOINTS" XENO_FIELD_STEP_SPU_LOG="$OUT/spu.log" \
    XENO_FIELD_STEP_RNG_LOG="$([ "${PROLOGUE_RNG_LOG:-0}" = 1 ] && echo "$OUT/rng.log")" \
    XENO_RAND_TRACE="$([ "${PROLOGUE_RAND_TRACE:-0}" = 1 ] && echo "$OUT/rand-trace.log")" \
    XENO_FIELD_CAPTURE_DIR="$OUT/captures" XENO_FIELD_CAPTURE_EVERY=100000 \
    timeout --signal=INT --kill-after=20 "${PROLOGUE_SLICE_SECONDS:-900}s" "$BINARY" > "$OUT/run.log" 2>&1
echo "rc=$?" > "$OUT/rc.txt"
cat "$OUT/rc.txt"
