/* Verified dev-menu warp and area-finish tables (DEV_MENU_SPEC.md §6/§7).
 *
 * Warp > <area folder> > <named spot>.  Only entries with a verification
 * report (`evidence`, from the warp sweep: fresh New Game, arrival, idle,
 * walk-in where needed) are listed; an area folder with no verified spot is
 * hidden.  Fill rows here from the sweep; no code changes are needed.
 *
 *   { disc, chapter, area, spot, map, entrance, progression, evidence }
 *
 * No entry is verified yet (the unverified Lahan / Black Moon candidates of
 * the first draft were removed).  The trailing sentinel keeps the arrays
 * non-empty; the counts exclude it. */
#include <stddef.h>
#include "port_dev_menu.h"

const PcPortDevWarp g_PcPortDevWarps[] = {
    { 0, NULL, "", "", -1, 0, 0u, NULL } /* sentinel */
};
const int g_PcPortDevWarpCount = 0;

const PcPortDevFinish g_PcPortDevFinishes[] = {
    { 0, NULL, "", NULL } /* sentinel */
};
const int g_PcPortDevFinishCount = 0;
