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
    /* Script memory words that gate story-driven map entry.  Index 0 is
     * SCRIPT_VAR_SCENARIO_FLAG (include/field/script_vm.h); byte address
     * 0x20 is the variable map 15's actor-22 block tests before its
     * CHANGE_FIELD to map 16 (Blackmoon Forest).  Same u16 view as
     * FieldScriptVMGetVariableValue / FieldScriptMemoryWriteU16. */
    printf("[xeno-port][test] POSDIAG map=%d pos=(%d,%d,%d) inZones=[%s] "
           "scenario=%u var20=%u\n",
           g_GameSceneMapNum & 0xFFF, (int)CONV_TO_GTE(actor->position.vx),
           (int)CONV_TO_GTE(actor->position.vy),
           (int)CONV_TO_GTE(actor->position.vz), zones,
           (unsigned)((u16*)&g_FieldScriptMemory)[SCRIPT_VAR_SCENARIO_FLAG >> 1],
           (unsigned)((u16*)&g_FieldScriptMemory)[0x20 >> 1]);
    fflush(stdout);
}
