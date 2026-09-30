#!/usr/bin/env bash
# Run a list of warp / step sweep targets, N at a time, HEADLESS ONLY.
#   warp_sweep_run.sh BIN OUTROOT CKPT TARGETS_TSV [PARALLEL]
# TARGETS_TSV columns (tab separated, '#' comments):
#   id  kind  map  entrance  scenario  walk  name
#   kind = warp | step ; for kind=step, `map`/`entrance` are the step's NEXT
#   location and `scenario` holds the step index (into g_PcPortDevSteps).
# Each target gets OUTROOT/<id>/{target.json,run.log,route.jsonl,final_frame.bmp}.
# Judge with: python3 pc_port/tests/warp_sweep_analyze.py OUTROOT
unset DISPLAY WAYLAND_DISPLAY XAUTHORITY
set -u
BIN=$(readlink -f "$1"); OUTROOT=$(mkdir -p "$2" && readlink -f "$2"); CKPT=$(readlink -f "$3"); TSV=$4; PAR=${5:-4}
HERE="$(cd "$(dirname "$0")" && pwd)"
export XENO_DATA_DIR=${XENO_DATA_DIR:?XENO_DATA_DIR must point at the disc files}
one() {
    local id kind map ent scen walk name rest
    IFS=$'\t' read -r id kind map ent scen walk name <<<"$1"
    local d="$OUTROOT/$id"
    mkdir -p "$d"
    printf '{"id":"%s","kind":"%s","map":%s,"entrance":%s,"scenario":%s,"walk":%s,"name":"%s"}\n' \
        "$id" "$kind" "$map" "$ent" "${scen:--1}" "$walk" "$name" >"$d/target.json"
    if [ "$kind" = warp ]; then
        bash "$HERE/warp_sweep_one.sh" "$BIN" "$d" "$CKPT" warp "$map" "$ent" "${scen:--1}"
    else
        bash "$HERE/warp_sweep_one.sh" "$BIN" "$d" "$CKPT" "${SWEEP_STEPMODE:-cont}" "$scen"
    fi
}
export -f one
export OUTROOT BIN CKPT HERE
grep -v '^#' "$TSV" | grep -v '^$' | xargs -P "$PAR" -d '\n' -I{} bash -c 'one "$1"' _ {}
