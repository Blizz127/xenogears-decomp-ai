/* field_warp_diag.c -- TEST TOOLING: one-shot debug teleport.
 *
 * Enabled only by XENO_FIELD_WARP=<map>:<x>:<z>; disabled and inert otherwise.
 * When that map is loaded and the player actor exists, the player's X/Z are
 * set once and the warp disarms.
 *
 * Why this exists: field 14's only trigger zone (the map 14 -> 13 door at
 * roughly (335,-26)) has never been reached by a walk from the post-battle
 * spawn at (115,-455), and two different search strategies stalled well short
 * of it.  That leaves two very different explanations -- the player cannot
 * PATH there, or the zone/transition itself is broken -- and no amount of
 * further walking distinguishes them.  Placing the player in the zone does:
 * if the field changes, the exit machinery is sound and the problem is
 * navigation; if it does not, the problem is the zone or its script.
 *
 * This DOES write game state, unlike PcPort_FieldPosDiag in the neighbouring
 * file, which is why it is a separate translation unit with its own switch:
 * any run that sets XENO_FIELD_WARP is a diagnostic run and its output must
 * not be quoted as evidence of ordinary play.
 *
 * REMOVAL: delete this file, its build_port.sh entry, and the
 * PcPort_FieldWarpDiag() call in pc_port/src/psyq_compat.c's Vsync shim.
 */
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "field/actor.h"
#include "field/main.h"
#include "system/math.h"

#include "quick_checkpoint.h"

extern s32 g_PlayerActorIndex;
extern int g_GameSceneMapNum;

void PcPort_FieldWarpDiag(void)
{
    static int parsed = 0;
    static int armed = 0;
    static int wantMap = -1;
    static int wantX = 0;
    static int wantZ = 0;
    /* A single shot at the first field-active frame lands DURING the room's
     * setup scene, which then repositions the player and erases it (measured:
     * fired at (-151,1774), player ended at (112,-458)).  So wait `delay`
     * field-active frames, then re-apply `repeats` times spaced `period`
     * apart, which outlasts the scene's own placement. */
    static int delay = 600;
    static int repeats = 5;
    static int period = 120;
    static int ticks = 0;
    static int fired = 0;
    ActorData* actor;

    if (!parsed) {
        const char* e = getenv("XENO_FIELD_WARP");

        parsed = 1;
        if (e != NULL && *e != '\0' &&
            sscanf(e, "%d:%d:%d:%d:%d", &wantMap, &wantX, &wantZ, &delay,
                   &repeats) >= 3) {
            armed = 1;
            printf("[xeno-port][test] WARP armed map=%d -> (%d,%d) "
                   "delay=%d repeats=%d\n",
                   wantMap, wantX, wantZ, delay, repeats);
            fflush(stdout);
        }
    }
    if (!armed)
        return;
    if ((g_GameSceneMapNum & 0xFFF) != wantMap)
        return;
    if (!PcPort_QuickCheckpointFieldIsActive())
        return;
    if (g_FieldActors == NULL)
        return;
    actor = (ActorData*)(uintptr_t)g_FieldActors[g_PlayerActorIndex].pActorData;
    if (actor == NULL)
        return;

    if (ticks++ < delay)
        return;
    if (((ticks - delay) % period) != 0)
        return;

    printf("[xeno-port][test] WARP firing %d/%d: (%d,%d) -> (%d,%d)\n",
           fired + 1, repeats, (int)CONV_TO_GTE(actor->position.vx),
           (int)CONV_TO_GTE(actor->position.vz), wantX, wantZ);
    fflush(stdout);
    /* position is stored pre-CONV_TO_GTE; invert the diag's view. */
    actor->position.vx = CONV_FROM_GTE(wantX);
    actor->position.vz = CONV_FROM_GTE(wantZ);
    if (++fired >= repeats)
        armed = 0;
}
