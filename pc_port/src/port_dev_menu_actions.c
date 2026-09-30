/* Dev-menu game-state actions, run from the cheat queue ("dev <command>") on
 * the game thread.  Each writes only the game's own state, bounded by the
 * game's own limits, and only at a safe field frame (DEV_MENU_SPEC.md §8). */
#include <stdio.h>
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

int PcPort_DevMenuAction(const char *command)
{
    int i, index;
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
        if (!PcPort_QuickCheckpointRequestWarp(w->map, w->entrance))
            return refuse(command, "a room change or checkpoint is already pending");
        fprintf(stderr, "[CHEATS] warp requested: %s / %s (map %d entrance %d)\n",
                w->area, w->spot, w->map, w->entrance);
    } else if (sscanf(command, "finish %d", &index) == 1) {
        return refuse(command, "no verified area finish");
    } else {
        return refuse(command, "unknown dev command");
    }
    return 1;
}
