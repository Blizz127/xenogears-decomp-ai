#!/usr/bin/env bash
# Fei HD-2D footprint and OT test.
#
# RULE: the HD-2D Fei replaces each retail part POLY_FT4 in place: the same
# screen quad (position, size, feet anchor) and the same OT slot, with more
# texel density inside it and the atlas art box aligned to the retail
# silhouette. This test renders one camera three ways and compares:
#
#   1. boots headless, loads a Lahan field quicksave (default: Fei behind the
#      easel in his house, field 14) through the cheat console `load`;
#   2. XENO_FEI_HD2D_TEST_CYCLE=1 draws the player sprite retail / HD-2D /
#      hidden on consecutive presents (present % 3), logging every part's
#      screen quad and OT slot (XENO_FEI_HD2D_PART_LOG) and capturing those
#      presents;
#   3. pc_port/tests/fei_hd2d_footprint_analyze.py: Fei's alpha mask in a
#      render = pixels differing from the hidden render; retail and HD masks
#      are compared per part and for the whole silhouette (within 1 px at
#      render scale), and the rows hidden behind the object must match.
#
# Evidence PNGs go to $XENO_FEI_FOOTPRINT_OUTDIR (default
# ~/release-staging/xeno-work/hd2d/claude-r6-pad/fei_footprint/<stamp>).
# Quicksave: XENO_FEI_FOOTPRINT_QS (default the field-14 one below). To make
# one: run the title/New Game smoke schedule, page Circle until field 14 after
# the opening battle, then `save` on the XENO_CHEATS_CONSOLE=1 console.
set -uo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT"
BIN="${XENO_PORT_BINARY:-$ROOT/pc_port/build_native/xeno-port}"
QS="${XENO_FEI_FOOTPRINT_QS:-$HOME/release-staging/xeno-work/r6-nogamedata/field14.xgqs}"
OUTDIR="${XENO_FEI_FOOTPRINT_OUTDIR:-$HOME/release-staging/xeno-work/hd2d/claude-r6-pad/fei_footprint/$(date +%Y%m%d-%H%M%S)}"
CAPTURES="${XENO_FEI_FOOTPRINT_CAPTURES:-24}"
TIMEOUT="${XENO_FEI_FOOTPRINT_TIMEOUT:-420}"
mkdir -p "$OUTDIR"
say() { echo "FEI FOOTPRINT $*" | tee -a "$OUTDIR/summary.txt"; }
for f in "$BIN" "$QS" disc/disc1.bin pc_port/assets/fei_hd2d/fei.rgba; do
    [ -e "$f" ] || { say "overall=NOT_RUN missing=$f"; exit 2; }
done
cp "$QS" "$OUTDIR/quicksave.xgqs"
say "binary=$BIN quicksave=$QS outdir=$OUTDIR"

# unit layer first: per-part replacement, OT slots, UV mapping
if ! bash pc_port/tests/run_fei_hd2d_test.sh > "$OUTDIR/unit.txt" 2>&1; then
    say "FAIL unit test (see $OUTDIR/unit.txt)"; exit 1
fi
say "unit: $(tail -1 "$OUTDIR/unit.txt")"

attempt=1
while :; do
    rm -rf "$OUTDIR/cap" "$OUTDIR/parts.log"
    python3 pc_port/tests/fei_hd2d_footprint_drive.py "$BIN" "$OUTDIR/quicksave.xgqs" "$OUTDIR" \
        "$CAPTURES" "$TIMEOUT" > "$OUTDIR/drive.txt" 2>&1
    drc=$?
    say "drive attempt=$attempt rc=$drc $(tail -1 "$OUTDIR/drive.txt")"
    [ "$drc" -eq 0 ] && break
    attempt=$((attempt + 1))
    [ "$attempt" -gt 2 ] && { say "overall=FAIL could not capture the quicksave scene"; exit 1; }
done

python3 pc_port/tests/fei_hd2d_footprint_analyze.py "$OUTDIR/parts.log" "$OUTDIR/cap" "$OUTDIR" --tol 1 \
    | tee "$OUTDIR/analysis.txt" | tee -a "$OUTDIR/summary.txt" >/dev/null
arc=${PIPESTATUS[0]}
grep -E "checked_triples|overall|part sets|triples with|horizontal" "$OUTDIR/analysis.txt"
say "evidence: $(ls "$OUTDIR"/*.png 2>/dev/null | tr '\n' ' ')"
exit "$arc"
