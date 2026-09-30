/* Dev-menu game-state actions, run from the cheat queue ("dev <command>") on
 * the game thread.  Each writes only the game's own state, bounded by the
 * game's own limits, and only at a safe field frame (DEV_MENU_SPEC.md §8). */
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "main/game.h"
#include "port_dev_menu.h"
#include "quick_checkpoint.h"

/* The game's own party-member recovery routines (src/field/party/stats.c):
 * clamp-to-max HP / EP increases, indexed by party slot, as the inn and the
 * script opcodes use them.  Plain C, no script VM state involved. */
extern void FieldPartyMemberIncreaseHp(int partyIndex, int amount);
extern void FieldPartyMemberIncreaseMp(int partyIndex, int amount);
extern void PcPort_PadTestInputHold(unsigned held);
extern unsigned PcPort_FieldLoopCount(void);
extern int g_GameSceneMapNum;

static int refuse(const char *command, const char *why)
{
    fprintf(stderr, "[CHEATS] refused '%s': %s\n", command, why);
    return -1;
}

static void status(const char *text) { PcPort_DevMenuSetStatus(text); }

/* Harness/diagnostic read-out: one line, always available while the dev menu
 * is enabled (no field-safety gate; it only reads). */
static void print_state(void)
{
    int i, pos[3] = {0, 0, 0}, have = PcPort_QuickCheckpointPlayerPosition(pos);
    unsigned ms = 0, slots = 0, total = 0;
    int input = PcPort_DevMenuInputState(&ms);
    fprintf(stderr, "[CHEATS] state: loop=%u map=%d safe=%d input=%s pos=", PcPort_FieldLoopCount(),
            (int)(g_GameSceneMapNum & 0x3FFF), PcPort_QuickCheckpointFieldIsSafe(),
            input == 0 ? "game" : input == 1 ? "menu" : "drain");
    if (have) fprintf(stderr, "(%d,%d,%d)", pos[0], pos[1], pos[2]); else fprintf(stderr, "none");
    fprintf(stderr, " party=");
    for (i = 0; i < MAX_PARTY_MEMBERS; i++) {
        int id = g_GamePartyMembers[i];
        if (id >= 0 && id < MAX_GAME_CHARACTERS) {
            const GameCharacter *c = &g_pGameState->characters[id];
            fprintf(stderr, "%s%d:%u/%u,%u/%u", i ? ";" : "", id, (unsigned)c->hp, (unsigned)c->maxHp,
                    (unsigned)c->mp, (unsigned)c->maxMp);
        }
    }
    for (i = 0; i < MAX_INVENTORY_ITEMS; i++)
        if (g_pGameState->itemIDs[i] && g_pGameState->itemQuantities[i]) {
            slots++; total += g_pGameState->itemQuantities[i];
        }
    fprintf(stderr, " gold=%u itemSlots=%u itemTotal=%u\n", (unsigned)g_pGameState->gold, slots, total);
}

int PcPort_DevMenuAction(const char *command)
{
    int i, index;
    unsigned hold;
    if (!PcPort_DevMenuEnabled()) return refuse(command, "dev menu is off");
    /* Read-only / harness commands: no field gate. */
    if (!strcmp(command, "state")) { print_state(); return 1; }
    if (sscanf(command, "pad %x", &hold) == 1) {
        PcPort_PadTestInputHold(hold);
        fprintf(stderr, "[CHEATS] pad: harness hold 0x%04x\n", hold);
        return 1;
    }
    if (!strcmp(command, "hurt")) {   /* harness: put the party at 1 HP / 0 EP, like a fight would */
        for (i = 0; i < MAX_PARTY_MEMBERS; i++) {
            int id = g_GamePartyMembers[i];
            if (id >= 0 && id < MAX_GAME_CHARACTERS) {
                g_pGameState->characters[id].hp = 1;
                g_pGameState->characters[id].mp = 0;
            }
        }
        fprintf(stderr, "[CHEATS] hurt: party set to 1 HP / 0 EP (test harness)\n");
        return 1;
    }
    if (sscanf(command, "setgold %u", &hold) == 1) {   /* harness: set gold (capped) */
        g_pGameState->gold = hold > MAX_GOLD_AMOUNT ? MAX_GOLD_AMOUNT : hold;
        fprintf(stderr, "[CHEATS] setgold: %u (test harness)\n", (unsigned)g_pGameState->gold);
        return 1;
    }
    if (sscanf(command, "seed %d:%u", &index, &hold) == 2 && index > 0 && index < 256) {
        /* harness: give an item the way the shop does (ShopMenuHandleBoughtItems,
         * src/shop_menu/main/misc8.c:358-380): add to an owned stack or take the first empty slot. */
        for (i = 0; i < MAX_INVENTORY_ITEMS; i++)
            if (g_pGameState->itemIDs[i] == index) { hold += g_pGameState->itemQuantities[i]; break; }
        if (i == MAX_INVENTORY_ITEMS)
            for (i = 0; i < MAX_INVENTORY_ITEMS && g_pGameState->itemIDs[i]; i++) ;
        if (i == MAX_INVENTORY_ITEMS) return refuse(command, "inventory full");
        g_pGameState->itemIDs[i] = (u_char)index;
        g_pGameState->itemQuantities[i] = (u_char)(hold > MAX_ITEM_QUANTITY ? MAX_ITEM_QUANTITY : hold);
        fprintf(stderr, "[CHEATS] seed: item %d x%u in slot %d (test harness)\n", index,
                (unsigned)g_pGameState->itemQuantities[i], i);
        return 1;
    }
    if (!strncmp(command, "key ", 4)) {
        static const struct { const char *n; int k; } keys[] = {
            {"f8", PC_DEV_OPEN}, {"up", PC_DEV_UP}, {"down", PC_DEV_DOWN}, {"enter", PC_DEV_ACCEPT},
            {"esc", PC_DEV_BACK}};
        for (i = 0; i < 5; i++)
            if (!strcmp(command + 4, keys[i].n)) {
                PcPort_DevMenuKey(keys[i].k, 1);
                fprintf(stderr, "[CHEATS] key: %s\n", keys[i].n);
                return 1;
            }
        return refuse(command, "unknown key");
    }
    if (!PcPort_QuickCheckpointFieldIsSafe()) {
        char why[160];
        PcPort_QuickCheckpointSafetyDescribe(why, sizeof why);
        status("NOT ON A FREE FIELD - NO CHANGE");
        return refuse(command, why);
    }
    if (!strcmp(command, "heal")) {
        /* Full recovery through the game's own party routines.  Both clamp
         * to max (stats.c:7-12 / :26-31), so passing the missing amount lands
         * exactly on max and nothing else is written.  Party slots only
         * (g_GamePartyMembers, 0xFF = empty), never the benched roster. */
        int n = 0;
        for (i = 0; i < MAX_PARTY_MEMBERS; i++) {
            int id = g_GamePartyMembers[i];
            if (id >= 0 && id < MAX_GAME_CHARACTERS) {
                GameCharacter *c = &g_pGameState->characters[id];
                FieldPartyMemberIncreaseHp(i, c->maxHp > c->hp ? c->maxHp - c->hp : 0);
                FieldPartyMemberIncreaseMp(i, c->maxMp > c->mp ? c->maxMp - c->mp : 0);
                n++;
            }
        }
        fprintf(stderr, "[CHEATS] heal: %d party members refilled to their max HP/EP\n", n);
        status("PARTY REFILLED");
    } else if (!strcmp(command, "gold")) {
        /* Same write as the field script opcode FieldScriptIncreaseGold
         * (src/field/game_logic/gold.c:26-30) and the shop
         * (src/shop_menu/main/misc8.c:299-302): add, clamp to MAX_GOLD_AMOUNT. */
        unsigned gold = g_pGameState->gold;
        unsigned next = gold + 10000u;
        if (next > MAX_GOLD_AMOUNT) next = MAX_GOLD_AMOUNT;
        g_pGameState->gold = next;
        fprintf(stderr, "[CHEATS] gold: %u -> %u (cap %u)\n", gold, (unsigned)g_pGameState->gold,
                (unsigned)MAX_GOLD_AMOUNT);
        status("GOLD ADDED");
    } else if (!strcmp(command, "items")) {
        /* Same write as the shop's buy (ShopMenuHandleBoughtItems,
         * src/shop_menu/main/misc8.c:361-368): an owned stack gains quantity,
         * clamped to MAX_ITEM_QUANTITY.  No new slots are created. */
        int n = 0;
        for (i = 0; i < MAX_INVENTORY_ITEMS; i++) {
            unsigned q = g_pGameState->itemQuantities[i];
            if (q && g_pGameState->itemIDs[i]) {
                q += 10;
                if (q > MAX_ITEM_QUANTITY) q = MAX_ITEM_QUANTITY;
                g_pGameState->itemQuantities[i] = (u_char)q;
                n++;
            }
        }
        fprintf(stderr, "[CHEATS] items: %d owned stacks +10 (cap %u)\n", n, (unsigned)MAX_ITEM_QUANTITY);
        status("ITEMS TOPPED UP");
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
