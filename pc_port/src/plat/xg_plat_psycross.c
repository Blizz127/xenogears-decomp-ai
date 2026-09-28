/* PsyCross backend for the xg_plat renderer / audio / input / timing
 * interfaces (pc_port/include/xg_plat/).  Thin: each call maps to the
 * PsyCross (MIT, pinned in pc_port/psycross.lock) facility that already
 * implements it.  A different backend provides the same functions. */
#include <stdio.h>
#include <string.h>

#include "../../include/xg_plat/renderer.h"
#include "../../include/xg_plat/audio.h"
#include "../../include/xg_plat/input.h"
#include "../../include/xg_plat/timing.h"

#include "PsyX/PsyX_public.h"
#include "../host_speed.h"

extern int PsyX_Sys_GetVBlankCount(void);

/* --- renderer ------------------------------------------------------------ */
void xg_plat_renderer_default_config(XgPlatVideoConfig* cfg)
{
    cfg->window_width = 640;
    cfg->window_height = 480;
    cfg->fullscreen = 0;
    cfg->vsync = 1;
    cfg->bilinear = 0;
    cfg->widescreen = 0;
}

int xg_plat_renderer_supports(const char* option)
{
    /* PsyCross renders the PSX framebuffer 4:3; a widescreen view needs
     * game-side projection changes plus a backend that draws past 320px. */
    return strcmp(option, "widescreen") != 0;
}

int xg_plat_renderer_init(const char* title, const XgPlatVideoConfig* cfg)
{
    int ignored = 0;
    int w = cfg->window_width > 0 ? cfg->window_width : 640;
    int h = cfg->window_height > 0 ? cfg->window_height : 480;
    g_cfg_bilinearFiltering = cfg->bilinear ? 1 : 0;
    PsyX_Initialise((char*)title, w, h, cfg->fullscreen ? 1 : 0);
    if (!cfg->vsync)
        PsyX_EnableSwapInterval(0);
    if (cfg->widescreen && !xg_plat_renderer_supports("widescreen")) {
        fprintf(stderr, "[xg_plat] widescreen is not supported by the PsyCross "
                        "renderer backend yet; using 4:3\n");
        ignored++;
    }
    return ignored;
}

/* --- audio --------------------------------------------------------------- */
int xg_plat_audio_set_user_volume(int percent)
{
    /* PsyX_SPUAL_SetMasterVolume is the game's SpuSetCommonAttr mvol and is
     * overwritten by the game; PsyCross has no separate user gain yet. */
    (void)percent;
    return 0;
}

/* --- input --------------------------------------------------------------- */
static const char* const s_button_names[XG_PLAT_BTN_COUNT] = {
    "square", "circle", "triangle", "cross", "l1", "l2", "r1", "r2",
    "start", "select", "left", "right", "up", "down",
};

const char* xg_plat_input_button_name(XgPlatButton button)
{
    return (button >= 0 && button < XG_PLAT_BTN_COUNT) ? s_button_names[button] : NULL;
}

static int* key_slot(XgPlatButton button)
{
    PsyXKeyboardMapping* m = &g_cfg_keyboardMapping;
    switch (button) {
    case XG_PLAT_BTN_SQUARE: return &m->kc_square;
    case XG_PLAT_BTN_CIRCLE: return &m->kc_circle;
    case XG_PLAT_BTN_TRIANGLE: return &m->kc_triangle;
    case XG_PLAT_BTN_CROSS: return &m->kc_cross;
    case XG_PLAT_BTN_L1: return &m->kc_l1;
    case XG_PLAT_BTN_L2: return &m->kc_l2;
    case XG_PLAT_BTN_R1: return &m->kc_r1;
    case XG_PLAT_BTN_R2: return &m->kc_r2;
    case XG_PLAT_BTN_START: return &m->kc_start;
    case XG_PLAT_BTN_SELECT: return &m->kc_select;
    case XG_PLAT_BTN_LEFT: return &m->kc_dpad_left;
    case XG_PLAT_BTN_RIGHT: return &m->kc_dpad_right;
    case XG_PLAT_BTN_UP: return &m->kc_dpad_up;
    case XG_PLAT_BTN_DOWN: return &m->kc_dpad_down;
    default: return NULL;
    }
}

int xg_plat_input_bind_key(XgPlatButton button, const char* key_name)
{
    int* slot = key_slot(button);
    int code;
    if (slot == NULL || key_name == NULL)
        return 0;
    code = PsyX_LookupKeyboardMapping(key_name, -1);
    if (code < 0)
        return 0;
    *slot = code;
    return 1;
}

/* --- timing -------------------------------------------------------------- */
int xg_plat_timing_speed(void)
{
    return PsyX_GetSpeedMultiplier();
}

void xg_plat_timing_set_speed(int multiplier)
{
    if (multiplier < XG_PLAT_SPEED_MIN)
        multiplier = XG_PLAT_SPEED_MIN;
    if (multiplier > XG_PLAT_SPEED_MAX)
        multiplier = XG_PLAT_SPEED_MAX;
    PsyX_SetSpeedMultiplier(multiplier);
}

void xg_plat_timing_fast_forward(int held)
{
    PsyX_SetFastForwardHeld(held);
}

void xg_plat_timing_set_fast_forward_speed(int multiplier)
{
    PsyX_SetFastForwardSpeed(multiplier);
}

int xg_plat_timing_vblank_count(void)
{
    return PsyX_Sys_GetVBlankCount();
}
