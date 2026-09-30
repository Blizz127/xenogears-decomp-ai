/* Opt-in host dev menu: model, navigation, pad combo and input capture.
 * Input only changes menu state or queues a request; every game mutation
 * runs through the cheat queue on the game thread (port_dev_menu_actions.c)
 * at a safe field frame.  No game headers here, so the pad/menu logic is
 * unit-tested with SDL virtual pads (pc_port/tests/run_dev_menu_test.sh). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "port_dev_menu.h"
#include "cheat_console.h"
#include "../include/xg_plat/config.h"

#define COMBO_WINDOW_MS 150u   /* Select/Start held back while the combo may form */
#define TAP_REPLAY_POLLS 2     /* a held-back tap is replayed for two samples */

enum { PAGE_ROOT, PAGE_WARP, PAGE_FINISH, PAGE_CHEATS, PAGE_OPTIONS, PAGE_ENHANCE, PAGE_WARP_AREA };
enum { COMBO_IDLE, COMBO_PENDING, COMBO_OWNED, COMBO_PASS };

static int s_enabled, s_open, s_drain, s_page, s_cursor, s_area;
static int s_stackPage[8], s_stackCursor[8], s_stackArea[8], s_depth;
static char s_status[96] = "";
static const PcPortDevMenuHost *s_host;

/* Pad state */
static unsigned s_padPrev;
static int s_padInstance = -1;
static int s_combo = COMBO_IDLE;
static unsigned s_comboStart, s_comboSeen;
/* The post-close drain is bounded: a pad whose mapping reports a bit that
 * never releases must not lock the game's input (looks like a freeze). */
#define DRAIN_MAX_MS 1000u
static unsigned s_drainStart;
static int s_drainArmed;
static int s_replay;
static unsigned s_replayBits;

/* ---- rows ------------------------------------------------------------- */
#define MAX_ROWS 24
typedef struct { char label[48]; int selectable; int action; int arg; } Row;
enum {
    ACT_NONE, ACT_PAGE, ACT_AREA, ACT_WARP, ACT_FINISH, ACT_STEP,
    ACT_GOD, ACT_BATTLES, ACT_HEAL, ACT_GOLD, ACT_ITEMS,
    ACT_FF, ACT_SPEED, ACT_RECORD, ACT_SHOT, ACT_QSAVE, ACT_QLOAD, ACT_HD2D
};
static Row s_rows[MAX_ROWS];
static int s_rowCount;

static void add_row(int selectable, int action, int arg, const char *fmt, const char *value)
{
    Row *r;
    if (s_rowCount >= MAX_ROWS) return;
    r = &s_rows[s_rowCount++];
    r->selectable = selectable;
    r->action = action;
    r->arg = arg;
    if (value) snprintf(r->label, sizeof r->label, "%s: %s", fmt, value);
    else snprintf(r->label, sizeof r->label, "%s", fmt);
}

static int area_seen_before(int i, const char *area)
{
    int j;
    for (j = 0; j < i; j++)
        if (g_PcPortDevWarps[j].evidence && !strcmp(g_PcPortDevWarps[j].area, area)) return 1;
    return 0;
}

static const char *on_off(int (*get)(void)) { return get && get() ? "ON" : "OFF"; }

static void build_rows(void)
{
    int i;
    char speed[8];
    s_rowCount = 0;
    switch (s_page) {
    case PAGE_ROOT:
        add_row(1, ACT_PAGE, PAGE_WARP, "WARP >", NULL);
        add_row(1, ACT_PAGE, PAGE_FINISH, "FINISH AREA >", NULL);
        add_row(1, ACT_PAGE, PAGE_CHEATS, "CHEATS >", NULL);
        add_row(1, ACT_PAGE, PAGE_OPTIONS, "OPTIONS >", NULL);
        break;
    case PAGE_WARP:
        for (i = 0; i < g_PcPortDevWarpCount; i++)
            if (g_PcPortDevWarps[i].evidence && !area_seen_before(i, g_PcPortDevWarps[i].area)) {
                char label[48];
                snprintf(label, sizeof label, "%s >", g_PcPortDevWarps[i].area);
                add_row(1, ACT_AREA, i, label, NULL);
            }
        if (!s_rowCount) add_row(0, ACT_NONE, 0, "(NO VERIFIED WARPS YET)", NULL);
        break;
    case PAGE_WARP_AREA:
        for (i = 0; i < g_PcPortDevWarpCount; i++)
            if (g_PcPortDevWarps[i].evidence && !strcmp(g_PcPortDevWarps[i].area, g_PcPortDevWarps[s_area].area))
                add_row(1, ACT_WARP, i, g_PcPortDevWarps[i].spot, NULL);
        break;
    case PAGE_FINISH:
        for (i = 0; i < g_PcPortDevStepCount; i++)
            if (g_PcPortDevSteps[i].evidence) {
                add_row(1, ACT_STEP, 0, "FINISH CURRENT STEP", NULL);
                break;
            }
        for (i = 0; i < g_PcPortDevFinishCount; i++)
            if (g_PcPortDevFinishes[i].evidence) {
                char label[48];
                snprintf(label, sizeof label, "COMPLETE %s", g_PcPortDevFinishes[i].area);
                add_row(1, ACT_FINISH, i, label, NULL);
            }
        if (!s_rowCount) add_row(0, ACT_NONE, 0, "(NO VERIFIED AREAS YET)", NULL);
        break;
    case PAGE_CHEATS:
        add_row(1, ACT_GOD, 0, "GOD MODE", on_off(s_host ? s_host->god_on : NULL));
        add_row(1, ACT_BATTLES, 0, "RANDOM BATTLES", s_host && s_host->random_battles_on
                ? (s_host->random_battles_on() ? "ON" : "OFF") : "ON");
        /* Refill / gold / items are listed only when verified (DEV_MENU_SPEC
         * §8): the owner's playtest froze after Refill (r6.3), and their
         * game-state tests went away with the draft.  Hidden until they go
         * through the game's own routines and pass a field+menu test.
         * XENO_DEV_MENU_UNVERIFIED=1 shows them for testing. */
        if (getenv("XENO_DEV_MENU_UNVERIFIED") && getenv("XENO_DEV_MENU_UNVERIFIED")[0] == '1') {
            add_row(1, ACT_HEAL, 0, "REFILL PARTY HP AND EP", NULL);
            add_row(1, ACT_GOLD, 0, "ADD 10000 GOLD", NULL);
            add_row(1, ACT_ITEMS, 0, "TOP UP OWNED ITEMS", NULL);
        }
        break;
    case PAGE_OPTIONS:
        add_row(1, ACT_FF, 0, "FAST-FORWARD", on_off(s_host ? s_host->fast_forward_on : NULL));
        snprintf(speed, sizeof speed, "%dX", s_host && s_host->speed ? s_host->speed() : 1);
        add_row(1, ACT_SPEED, 0, "GAME SPEED", speed);
        add_row(1, ACT_RECORD, 0, "RECORD VIDEO", on_off(s_host ? s_host->recording : NULL));
        add_row(1, ACT_SHOT, 0, "SCREENSHOT", NULL);
        add_row(1, ACT_QSAVE, 0, "QUICK SAVE", NULL);
        add_row(1, ACT_QLOAD, 0, "QUICK LOAD", NULL);
        add_row(1, ACT_PAGE, PAGE_ENHANCE, "ENHANCEMENTS >", NULL);
        break;
    case PAGE_ENHANCE:
        if (s_host && s_host->hd2d_available && s_host->hd2d_available())
            add_row(1, ACT_HD2D, 0, "HD-2D FEI", on_off(s_host->hd2d_on));
        if (!s_rowCount) add_row(0, ACT_NONE, 0, "(NONE AVAILABLE)", NULL);
        break;
    }
}

static int first_selectable(void)
{
    int i;
    for (i = 0; i < s_rowCount; i++) if (s_rows[i].selectable) return i;
    return -1;
}

/* ---- public model ----------------------------------------------------- */
void PcPort_DevMenuInit(void)
{
    const char *cheats = getenv("XENO_CHEATS");
    const char *env = getenv("XENO_DEV_MENU");
    int on = xg_plat_config_int("cheats.dev_menu", 0) != 0;
    if (env && *env) on = strcmp(env, "0") != 0;
    if (cheats && strcmp(cheats, "0") == 0) {
        if (on) fprintf(stderr, "[DEV_MENU] XENO_CHEATS=0: dev menu and cheats hard-disabled\n");
        on = 0;
    }
    s_enabled = on;
    s_open = s_drain = s_depth = 0;
    s_combo = COMBO_IDLE;
    s_padPrev = 0;
    s_padInstance = -1;
    s_replay = 0;
    if (on)
        fprintf(stderr, "[DEV_MENU] enabled: Select+Start on one pad or F8 opens it\n");
}
int PcPort_DevMenuEnabled(void) { return s_enabled; }
int PcPort_DevMenuOpen(void) { return s_enabled && s_open; }
int PcPort_DevMenuCursor(void) { return s_cursor; }
const char *PcPort_DevMenuStatus(void) { return s_status; }
void PcPort_DevMenuSetStatus(const char *text) { snprintf(s_status, sizeof s_status, "%s", text); }
void PcPort_DevMenuSetHost(const PcPortDevMenuHost *host) { s_host = host; }
int PcPort_DevMenuRows(void) { build_rows(); return s_rowCount; }
const char *PcPort_DevMenuRow(int row) { build_rows(); return row >= 0 && row < s_rowCount ? s_rows[row].label : ""; }
int PcPort_DevMenuRowSelectable(int row) { build_rows(); return row >= 0 && row < s_rowCount && s_rows[row].selectable; }
int PcPort_DevMenuCheatsActive(void)
{
    if (!s_enabled || !s_host) return 0;
    return (s_host->god_on && s_host->god_on()) ||
           (s_host->random_battles_on && !s_host->random_battles_on());
}
const char *PcPort_DevMenuTitle(void)
{
    switch (s_page) {
    case PAGE_WARP: return "WARP";
    case PAGE_WARP_AREA: return g_PcPortDevWarps[s_area].area;
    case PAGE_FINISH: return "FINISH AREA";
    case PAGE_CHEATS: return "CHEATS";
    case PAGE_OPTIONS: return "OPTIONS";
    case PAGE_ENHANCE: return "ENHANCEMENTS";
    default: return "DEV MENU";
    }
}

static void enter_page(int page, int area)
{
    if (s_depth < 8) {
        s_stackPage[s_depth] = s_page;
        s_stackCursor[s_depth] = s_cursor;
        s_stackArea[s_depth] = s_area;
        s_depth++;
    }
    s_page = page;
    s_area = area;
    build_rows();
    s_cursor = first_selectable();
}

static void open_menu(const char *how)
{
    s_open = 1;
    s_page = PAGE_ROOT;
    s_depth = 0;
    s_area = 0;
    build_rows();
    s_cursor = first_selectable();
    snprintf(s_status, sizeof s_status, "SIM KEEPS RUNNING");
    fprintf(stderr, "[DEV_MENU] open (%s)\n", how);
}

static void close_menu(const char *how)
{
    s_open = 0;
    s_drain = 1;
    fprintf(stderr, "[DEV_MENU] close (%s)\n", how);
}

static void go_back(void)
{
    if (s_depth == 0) { close_menu("back at root"); return; }
    s_depth--;
    s_page = s_stackPage[s_depth];
    s_area = s_stackArea[s_depth];
    build_rows();
    s_cursor = s_stackCursor[s_depth];
    if (s_cursor >= s_rowCount || (s_cursor >= 0 && !s_rows[s_cursor].selectable)) s_cursor = first_selectable();
}

static void move_cursor(int dir)
{
    int i, n;
    build_rows();
    n = s_rowCount;
    if (s_cursor < 0 || n == 0) return;
    for (i = 1; i <= n; i++) {
        int c = (s_cursor + dir * i + n * 2) % n;
        if (s_rows[c].selectable) { s_cursor = c; return; }
    }
}

static void queue(const char *command, const char *status)
{
    PcPort_CheatQueue(command);
    snprintf(s_status, sizeof s_status, "%s", status);
    fprintf(stderr, "[CHEATS] queued '%s'\n", command);
}

static void activate(void)
{
    Row r;
    char cmd[64];
    build_rows();
    if (s_cursor < 0 || s_cursor >= s_rowCount || !s_rows[s_cursor].selectable) return;
    r = s_rows[s_cursor];
    switch (r.action) {
    case ACT_PAGE: enter_page(r.arg, 0); break;
    case ACT_AREA: enter_page(PAGE_WARP_AREA, r.arg); break;
    case ACT_WARP:
        snprintf(cmd, sizeof cmd, "dev warp %d", r.arg);
        queue(cmd, "WARP REQUESTED");
        close_menu("warp");
        break;
    case ACT_FINISH:
        snprintf(cmd, sizeof cmd, "dev finish %d", r.arg);
        queue(cmd, "AREA FINISH REQUESTED");
        close_menu("finish area");
        break;
    case ACT_STEP:
        queue("dev step", "STEP FINISH REQUESTED");
        close_menu("finish current step");
        break;
    case ACT_GOD: queue("god", "GOD MODE TOGGLED"); break;
    case ACT_BATTLES: queue("encounters", "RANDOM BATTLES TOGGLED"); break;
    case ACT_HEAL: queue("dev heal", "REFILL QUEUED"); break;
    case ACT_GOLD: queue("dev gold", "GOLD QUEUED"); break;
    case ACT_ITEMS: queue("dev items", "ITEMS QUEUED"); break;
    case ACT_QSAVE: queue("save", "QUICK SAVE REQUESTED"); close_menu("quick save"); break;
    case ACT_QLOAD: queue("load", "QUICK LOAD REQUESTED"); close_menu("quick load"); break;
    case ACT_FF:
        if (s_host && s_host->fast_forward_toggle) s_host->fast_forward_toggle();
        fprintf(stderr, "[DEV_MENU] fast-forward toggled\n");
        break;
    case ACT_SPEED:
        if (s_host && s_host->speed_cycle) s_host->speed_cycle();
        fprintf(stderr, "[DEV_MENU] game speed cycled\n");
        break;
    case ACT_RECORD:
        if (s_host && s_host->record_toggle) s_host->record_toggle();
        fprintf(stderr, "[DEV_MENU] record toggled\n");
        break;
    case ACT_SHOT:
        close_menu("screenshot");
        if (s_host && s_host->screenshot) s_host->screenshot();
        break;
    case ACT_HD2D:
        if (s_host && s_host->hd2d_toggle) s_host->hd2d_toggle();
        fprintf(stderr, "[DEV_MENU] HD-2D Fei toggled\n");
        break;
    }
}

int PcPort_DevMenuKey(int key, int pressed)
{
    if (!s_enabled) return 0;
    if (key == PC_DEV_OPEN) {
        if (pressed) { if (s_open) close_menu("F8"); else open_menu("F8"); }
        return 1;
    }
    if (!s_open) return 0;
    if (!pressed) return 1;
    if (key == PC_DEV_UP) move_cursor(-1);
    else if (key == PC_DEV_DOWN) move_cursor(1);
    else if (key == PC_DEV_BACK) go_back();
    else if (key == PC_DEV_ACCEPT) activate();
    return 1;
}

/* ---- pad: combo, navigation, capture ---------------------------------- */
unsigned PcPort_DevMenuPad(unsigned bits, int instance, int held_any, unsigned now_ms)
{
    const unsigned combo = PC_DEV_PAD_BACK | PC_DEV_PAD_START;
    unsigned pressed, result = PC_DEV_PASS;
    if (!s_enabled) return PC_DEV_PASS;

    /* A pad that appears (or takes port 1) with buttons held must go neutral
     * before it can operate the menu. */
    if (instance != s_padInstance) {
        s_padInstance = instance;
        s_padPrev = bits;
        s_combo = (bits & combo) ? COMBO_PASS : COMBO_IDLE;
    }
    pressed = bits & ~s_padPrev;
    s_padPrev = bits;

    if (s_replay > 0) {
        s_replay--;
        if (s_replayBits & PC_DEV_PAD_BACK) result |= PC_DEV_FORCE_SELECT;
        if (s_replayBits & PC_DEV_PAD_START) result |= PC_DEV_FORCE_START;
    }

    /* Select+Start on this pad. */
    switch (s_combo) {
    case COMBO_IDLE:
        if ((bits & combo) == combo && (pressed & combo)) {
            if (s_open) close_menu("Select+Start"); else open_menu("Select+Start");
            s_combo = COMBO_OWNED;
        } else if (pressed & combo) {
            s_combo = COMBO_PENDING;
            s_comboStart = now_ms;
            s_comboSeen = bits & combo;
        }
        break;
    case COMBO_PENDING:
        s_comboSeen |= bits & combo;
        if ((bits & combo) == combo) {
            if (s_open) close_menu("Select+Start"); else open_menu("Select+Start");
            s_combo = COMBO_OWNED;
        } else if (!(bits & combo)) {
            /* Released before the combo formed: give the game its tap. */
            if (!s_open) { s_replay = TAP_REPLAY_POLLS; s_replayBits = s_comboSeen; }
            s_combo = COMBO_IDLE;
            if (!s_open) {
                if (s_replayBits & PC_DEV_PAD_BACK) result |= PC_DEV_FORCE_SELECT;
                if (s_replayBits & PC_DEV_PAD_START) result |= PC_DEV_FORCE_START;
                s_replay--;
            }
        } else if (now_ms - s_comboStart >= COMBO_WINDOW_MS) {
            s_combo = COMBO_PASS;   /* a plain Select or Start hold */
        }
        break;
    case COMBO_OWNED:
        if (!(bits & combo)) s_combo = COMBO_IDLE;
        break;
    case COMBO_PASS:
        if (!(bits & combo)) s_combo = COMBO_IDLE;
        break;
    }
    if (s_combo == COMBO_PENDING || s_combo == COMBO_OWNED)
        result |= PC_DEV_BLOCK_SELECT | PC_DEV_BLOCK_START;

    if (s_open) {
        if (pressed & PC_DEV_PAD_UP) move_cursor(-1);
        if (pressed & PC_DEV_PAD_DOWN) move_cursor(1);
        if (pressed & PC_DEV_PAD_B) go_back();
        else if (pressed & PC_DEV_PAD_A) activate();
        return result | PC_DEV_BLOCK_ALL;
    }
    if (s_drain) {
        /* After closing: the game sees a neutral pad until every button
         * (pad and keyboard) has been released. */
        if (!s_drainArmed) { s_drainArmed = 1; s_drainStart = now_ms; }
        if (!bits && !held_any) s_drain = 0;
        else if (now_ms - s_drainStart >= DRAIN_MAX_MS) {
            s_drain = 0;
            fprintf(stderr, "[DEV_MENU] input drain timed out after %u ms (a pad bit stayed held); game input restored\n",
                    DRAIN_MAX_MS);
        }
        if (!s_drain) s_drainArmed = 0;
        return result | PC_DEV_BLOCK_ALL;
    }
    return result;
}
