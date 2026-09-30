/* Dev-menu game-state actions, run from the cheat queue ("dev <command>") on
 * the game thread.  Each writes only the game's own state, bounded by the
 * game's own limits, and only at a safe field frame (DEV_MENU_SPEC.md §8). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "main/game.h"
#include "port_dev_menu.h"
#include "quick_checkpoint.h"

static int refuse(const char *command, const char *why)
{
    fprintf(stderr, "[CHEATS] refused '%s': %s\n", command, why);
    return -1;
}

extern void* g_FieldScriptMemory;
extern int g_GameSceneMapNum;
extern void FieldScriptMemoryWriteU16(int index, int value);
extern void func_800A30FC(void); /* the game's own script-memory -> g_GameState flush */

static unsigned scenario_now(void) { return ((unsigned short*)&g_FieldScriptMemory)[0]; }
static unsigned var_now(unsigned addr) { return ((unsigned short*)&g_FieldScriptMemory)[(addr >> 1) & 0x3FF]; }

/* Field-change request through the quick-checkpoint teardown path, then the
 * recorded script-variable values through the game's own writer and its own
 * flush (func_800A30FC) so the checkpoint copy of g_GameState carries them. */
static int warp_with(const char *command, int map, int entrance, const PcPortDevStepWrite *w, int nw, int scen)
{
    int i;
    if (!PcPort_QuickCheckpointRequestWarp(map, entrance))
        return -1;
    if (scen >= 0) FieldScriptMemoryWriteU16(0, scen);
    for (i = 0; i < nw; i++) FieldScriptMemoryWriteU16(w[i].addr, w[i].value);
    if (scen >= 0 || nw > 0) func_800A30FC();
    (void)command;
    return 0;
}

static int step_matches(const PcPortDevStep *st)
{
    if (!st->evidence) return 0;
    if (scenario_now() != st->detect_scen) return 0;
    if (st->detect_map >= 0 && (g_GameSceneMapNum & 0x3FFF) != st->detect_map) return 0;
    if (st->detect_var_addr != PC_DEV_NO_VALUE && var_now(st->detect_var_addr) != st->detect_var_value) return 0;
    return 1;
}

static int find_step(void)
{
    int i;
    for (i = 0; i < g_PcPortDevStepCount; i++)
        if (step_matches(&g_PcPortDevSteps[i])) return i;
    return -1;
}

/* Apply step `index` (detect already checked by the caller when needed). */
static int finish_step(const char *command, int index)
{
    const PcPortDevStep *st = &g_PcPortDevSteps[index];
    char toast[96];
    int i;
    if (warp_with(command, st->next_map, st->next_entrance, st->writes, st->nwrites, -1) != 0)
        return refuse(command, "a room change or checkpoint is already pending");
    fprintf(stderr, "[DEV_MENU] step finished: %s '%s' (area %s), %d writes:", st->id, st->name, st->area, st->nwrites);
    for (i = 0; i < st->nwrites; i++) fprintf(stderr, " var[0x%x]=%u", st->writes[i].addr, st->writes[i].value);
    fprintf(stderr, " -> map %d entrance %d\n", st->next_map, st->next_entrance);
    snprintf(toast, sizeof toast, "DONE %.40s", st->name);
    PcPort_DevMenuSetStatus(toast);
    return 1;
}

int PcPort_DevMenuAction(const char *command)
{
    int i, index, map, entrance, scen;
    if (!PcPort_DevMenuEnabled()) return refuse(command, "dev menu is off");
    if (!PcPort_QuickCheckpointFieldIsSafe()) {
        char why[160];
        PcPort_QuickCheckpointSafetyDescribe(why, sizeof why);
        return refuse(command, why);
    }
    if (!strcmp(command, "heal")) {
        int n = 0;
        for (i = 0; i < MAX_PARTY_MEMBERS; i++) {
            unsigned id = g_pGameState->partyMembers[i];
            if (id < MAX_GAME_CHARACTERS) {
                GameCharacter *c = &g_pGameState->characters[id];
                c->hp = c->maxHp; c->mp = c->maxMp; n++;
            }
        }
        fprintf(stderr, "[CHEATS] heal: %d party members refilled to their max HP/EP\n", n);
    } else if (!strcmp(command, "gold")) {
        unsigned gold = g_pGameState->gold;
        g_pGameState->gold = gold >= MAX_GOLD_AMOUNT - 10000u ? MAX_GOLD_AMOUNT : gold + 10000u;
        fprintf(stderr, "[CHEATS] gold: %u -> %u (cap %u)\n", gold, (unsigned)g_pGameState->gold,
                (unsigned)MAX_GOLD_AMOUNT);
    } else if (!strcmp(command, "items")) {
        int n = 0;
        for (i = 0; i < MAX_INVENTORY_ITEMS; i++) {
            unsigned q = g_pGameState->itemQuantities[i];
            if (q && g_pGameState->itemIDs[i]) {
                g_pGameState->itemQuantities[i] = q >= MAX_ITEM_QUANTITY - 10 ? MAX_ITEM_QUANTITY : q + 10;
                n++;
            }
        }
        fprintf(stderr, "[CHEATS] items: %d owned stacks +10 (cap %u)\n", n, (unsigned)MAX_ITEM_QUANTITY);
    } else if (sscanf(command, "warp %d", &index) == 1) {
        const PcPortDevWarp *w;
        if (index < 0 || index >= g_PcPortDevWarpCount || !g_PcPortDevWarps[index].evidence)
            return refuse(command, "not a verified warp");
        w = &g_PcPortDevWarps[index];
        if (warp_with(command, w->map, w->entrance, NULL, 0,
                      w->progression == PC_DEV_NO_SCENARIO ? -1 : (int)w->progression) != 0)
            return refuse(command, "a room change or checkpoint is already pending");
        fprintf(stderr, "[CHEATS] warp requested: %s / %s (map %d entrance %d scenario %d)\n",
                w->area, w->spot, w->map, w->entrance,
                w->progression == PC_DEV_NO_SCENARIO ? -1 : (int)w->progression);
    } else if (sscanf(command, "warpx %d %d %d", &map, &entrance, &scen) == 3) {
        /* Sweep harness only: an arbitrary candidate, never in the menu. */
        const char *u = getenv("XENO_DEV_MENU_UNVERIFIED");
        if (!u || u[0] != '1') return refuse(command, "warpx needs XENO_DEV_MENU_UNVERIFIED=1");
        if (warp_with(command, map, entrance, NULL, 0, scen) != 0)
            return refuse(command, "a room change or checkpoint is already pending");
        fprintf(stderr, "[CHEATS] warpx requested: map %d entrance %d scenario %d\n", map, entrance, scen);
    } else if (!strcmp(command, "step")) {
        index = find_step();
        if (index < 0) return refuse(command, "no verified step matches the current story state");
        return finish_step(command, index);
    } else if (sscanf(command, "stepx %d", &index) == 1) {
        const char *u = getenv("XENO_DEV_MENU_UNVERIFIED");
        if (!u || u[0] != '1') return refuse(command, "stepx needs XENO_DEV_MENU_UNVERIFIED=1");
        if (index < 0 || index >= g_PcPortDevStepCount) return refuse(command, "no such step");
        return finish_step(command, index);
    } else if (sscanf(command, "finish %d", &index) == 1) {
        return refuse(command, "no verified area finish");
    } else {
        return refuse(command, "unknown dev command");
    }
    return 1;
}
