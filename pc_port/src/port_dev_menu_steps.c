/* Verified Finish Current Step table (DEV_MENU_SPEC.md 7a).  Rows come from
 * the recorded route (XENO_ROUTE_LOG -> tools/analysis/route_steps.py) and are
 * listed only with a passing sweep (pc_port/tests/sweep_analyze.py).  Step
 * names are our own words; ordering follows the public walkthrough
 * references. */
#include <stddef.h>
#include "port_dev_menu.h"

const PcPortDevStep g_PcPortDevSteps[] = {
    { NULL, 0, NULL, "", "", 0, -1, PC_DEV_NO_VALUE, 0, NULL, 0, -1, 0, 0, NULL } /* sentinel */
};
const int g_PcPortDevStepCount = 0;
