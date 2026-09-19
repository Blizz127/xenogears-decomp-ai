/* field_pos_diag.c -- TEST TOOLING: headless navigation telemetry.
 *
 * Enabled only by XENO_FIELD_POS_DIAG=<N>; prints every N Vsync frames the
 * current field map, the player actor's world position, and which of the
 * map's trigger zones currently contain the player.  Containment uses the
 * same packed-Z:X NormalClip quad test the VM's zone opcodes use
 * (FieldScriptCheckTriggerZone2D, src/field/main/misc11.c), so a printed
 * zone is a zone the script would consider entered.
 *
 * Without this a headless walk is blind: there is no other way to see where
 * the player is standing or which map boundary is underfoot, which is what
 * route-finding from one field map to a neighbour requires.
 *
 * Read-only -- writes no game state, and lives in port-owned code so no
 * matching translation unit is touched.  REMOVAL: delete this file, drop the
 * PcPort_FieldPosDiag() call in pc_port/src/psyq_compat.c's Vsync shim, and
 * delete the walk harness under scratchpad/boot-to-blackmoon-*.
 */
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "field/actor.h"
#include "field/main.h"
#include "system/math.h"
#include "field/script_vm.h"

#include "quick_checkpoint.h"

/* g_FieldActors / g_pFieldTriggerZones are declared by the field headers
 * above; only the two the headers do not carry are declared here. */
extern s32 g_PlayerActorIndex;
extern void* D_8005A4E0; /* loaded field header; +0x12C = triggers size */
extern int g_GameSceneMapNum;
extern s32 D_800ADBFC;   /* live field actor count */
extern u16 D_800AFE9C;   /* held-button field mask */
extern u16 D_800C2694;   /* newly pressed field-button mask */
/* The gates OP_UPDATE_CHARACTER (func_8009F5F4, src/field/main/misc6.c) tests
 * before it will move the player. Printed because field 14 shows held input
 * arriving at a player under free control that still does not move, and these
 * are the only remaining things that can be refusing it. */
extern s32 D_800ADB68;   /* playerCanRun */
extern s32 D_800ADB64;   /* active field/menu owner; 0xFF = none */
extern u8  D_800B21D0;   /* encounter/control block flag */

void PcPort_FieldPosDiag(void)
{
    static int every = -1;
    static unsigned int frame = 0;
    ActorData* actor;
    long here;
    int zone;
    int zoneCount;
    char zones[128];
    int used = 0;

    if (every < 0) {
        const char* e = getenv("XENO_FIELD_POS_DIAG");
        every = (e != NULL && *e != '\0') ? atoi(e) : 0;
        if (every < 0)
            every = 0;
    }
    if (every == 0)
        return;
    if ((frame++ % (unsigned int)every) != 0)
        return;
    /* Only a live field has valid actors.  The battle adapter keeps driving
     * Vsync while g_FieldActors[player].pActorData is stale, and reading it
     * there segfaults (observed: crash in the opening battle, walk3). */
    if (!PcPort_QuickCheckpointFieldIsActive())
        return;
    if (g_FieldActors == NULL || g_pFieldTriggerZones == NULL ||
        D_8005A4E0 == NULL)
        return;
    actor = (ActorData*)(uintptr_t)g_FieldActors[g_PlayerActorIndex].pActorData;
    if (actor == NULL)
        return;

    here = (CONV_TO_GTE(actor->position.vz) << 0x10) +
           CONV_TO_GTE(actor->position.vx);
    zones[0] = '\0';
    /* Zone count comes from the field header the loader itself reads: the
     * triggers section's decompressed size lives at D_8005A4E0+0x12C
     * (src/field/main/misc3.c:868), and each FieldTriggerZone is 0x18 bytes.
     * Scanning a fixed 32 instead read past the table and reported a
     * nonexistent "zone 17" on map 15 (which has 17 zones, 0..16). */
    zoneCount = (int)(*(u32*)((u8*)D_8005A4E0 + 0x12C) / sizeof(FieldTriggerZone));
    if (zoneCount < 0 || zoneCount > 256)
        return;
    /* One-time per field: dump every trigger zone's quad.  "inZones=[]" tells
     * a walk that it is not standing in a zone but not where any zone IS, so
     * route-finding degenerates to blind sweeping -- which is how field 14 got
     * mis-read as a script lock.  Printed once per loaded field, keyed on the
     * trigger table pointer so a re-entry re-dumps. */
    {
        static FieldTriggerZone* dumped = NULL;

        if (dumped != g_pFieldTriggerZones) {
            int i;

            dumped = g_pFieldTriggerZones;
            printf("[xeno-port][test] ZONEDUMP map=%d count=%d\n",
                   g_GameSceneMapNum & 0xFFF, zoneCount);
            for (i = 0; i < zoneCount; i++) {
                FieldTriggerZone* z = &g_pFieldTriggerZones[i];

                if (z->x0 == 0 && z->z0 == 0 && z->x1 == 0 && z->z1 == 0 &&
                    z->x2 == 0 && z->z2 == 0 && z->x3 == 0 && z->z3 == 0) {
                    continue;
                }
                printf("[xeno-port][test] ZONE %2d "
                       "(%d,%d) (%d,%d) (%d,%d) (%d,%d) center=(%d,%d)\n",
                       i, (int)z->x0, (int)z->z0, (int)z->x1, (int)z->z1,
                       (int)z->x2, (int)z->z2, (int)z->x3, (int)z->z3,
                       ((int)z->x0 + z->x1 + z->x2 + z->x3) / 4,
                       ((int)z->z0 + z->z1 + z->z2 + z->z3) / 4);
            }
            fflush(stdout);
        }
    }
    for (zone = 0; zone < zoneCount; zone++) {
        FieldTriggerZone* z = &g_pFieldTriggerZones[zone];
        long p0 = (z->z0 << 0x10) + z->x0;
        long p1 = (z->z1 << 0x10) + z->x1;
        long p2 = (z->z2 << 0x10) + z->x2;
        long p3 = (z->z3 << 0x10) + z->x3;

        if (p0 == 0 && p1 == 0 && p2 == 0 && p3 == 0)
            continue;
        if (NormalClip(p0, p1, here) >= 0 && NormalClip(p1, p2, here) >= 0 &&
            NormalClip(p2, p3, here) >= 0 && NormalClip(p3, p0, here) >= 0) {
            used += snprintf(zones + used, sizeof(zones) - (size_t)used,
                             "%s%d", used ? "," : "", zone);
            if (used >= (int)sizeof(zones) - 4)
                break;
        }
    }
    /* Field actors, every ACTORDIAG_EVERY position samples.  Field 14's only
     * trigger zone sits ~470 units outside the reachable floor, so the way out
     * of that room is an ACTOR (a door or an NPC answering Circle), not a zone
     * -- and there was previously no way to see where any actor stood.
     * Enabled with the same XENO_FIELD_POS_DIAG switch to keep the harness
     * surface small; read-only. */
    {
        static unsigned int samples = 0;
        enum { ACTORDIAG_EVERY = 20 };

        if ((samples++ % ACTORDIAG_EVERY) == 0) {
            int i;

            printf("[xeno-port][test] ACTORDUMP map=%d count=%d player=%d\n",
                   g_GameSceneMapNum & 0xFFF, (int)D_800ADBFC,
                   (int)g_PlayerActorIndex);
            for (i = 0; i < (int)D_800ADBFC && i < 64; i++) {
                ActorData* a =
                    (ActorData*)(uintptr_t)g_FieldActors[i].pActorData;

                if (a == NULL) {
                    continue;
                }
                printf("[xeno-port][test] ACTOR %2d pos=(%d,%d,%d) "
                       "status=0x%04x ip=%u\n",
                       i, (int)CONV_TO_GTE(a->position.vx),
                       (int)CONV_TO_GTE(a->position.vy),
                       (int)CONV_TO_GTE(a->position.vz),
                       (unsigned)(g_FieldActors[i].status & 0xFFFF),
                       (unsigned)a->scriptInstructionPointer);
            }
            fflush(stdout);
        }
    }

    /* Script memory words that gate story-driven map entry.  Index 0 is
     * SCRIPT_VAR_SCENARIO_FLAG (include/field/script_vm.h); byte address
     * 0x20 is the variable map 15's actor-22 block tests before its
     * CHANGE_FIELD to map 16 (Blackmoon Forest).  Same u16 view as
     * FieldScriptVMGetVariableValue / FieldScriptMemoryWriteU16. */
    /* held/newpress: the field's own button masks.  A headless walk cannot
     * otherwise tell "the game ignored my input" from "my input never got
     * there" -- and those need opposite fixes.  It matters here because
     * PsyCross reads HELD buttons from SDL_GetKeyboardState (which is empty
     * unless the window owns X input focus) but latches TAPS from key events,
     * so on a WM-less Xvfb confirms can work while direction holds do not. */
    printf("[xeno-port][test] POSDIAG map=%d pos=(%d,%d,%d) inZones=[%s] "
           "scenario=%u var20=%u held=0x%04x newpress=0x%04x "
           "canRun=%d owner=0x%02x b21d0=%u status=0x%04x\n",
           g_GameSceneMapNum & 0xFFF, (int)CONV_TO_GTE(actor->position.vx),
           (int)CONV_TO_GTE(actor->position.vy),
           (int)CONV_TO_GTE(actor->position.vz), zones,
           (unsigned)((u16*)&g_FieldScriptMemory)[SCRIPT_VAR_SCENARIO_FLAG >> 1],
           (unsigned)((u16*)&g_FieldScriptMemory)[0x20 >> 1],
           (unsigned)D_800AFE9C, (unsigned)D_800C2694,
           (int)D_800ADB68, (unsigned)(D_800ADB64 & 0xFF),
           (unsigned)D_800B21D0,
           (unsigned)(g_FieldActors[g_PlayerActorIndex].status & 0xFFFF));
    fflush(stdout);
}
