/* xg_plat config parser/overrides (src/plat/xg_plat_config.c), with the
 * backend calls stubbed.  No game data involved. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/xg_plat/config.h"
#include "../include/xg_plat/audio.h"
#include "../include/xg_plat/input.h"
#include "../include/xg_plat/timing.h"

static int s_speed, s_volume_calls, s_binds;
static char s_bound[64];
void xg_plat_timing_set_speed(int m) { s_speed = m; }
int xg_plat_audio_set_user_volume(int p) { (void)p; s_volume_calls++; return 0; }
const char* xg_plat_input_button_name(XgPlatButton b)
{
    static const char* n[XG_PLAT_BTN_COUNT] = { "square", "circle", "triangle", "cross", "l1",
        "l2", "r1", "r2", "start", "select", "left", "right", "up", "down" };
    return n[b];
}
int xg_plat_input_bind_key(XgPlatButton b, const char* k)
{
    s_binds++;
    snprintf(s_bound, sizeof s_bound, "%s=%s", xg_plat_input_button_name(b), k);
    return strcmp(k, "NotAKey") != 0;
}

static int fails;
#define CHECK(c, what) do { if (!(c)) { fails++; printf("FAIL %s\n", what); } else printf("ok   %s\n", what); } while (0)

int main(void)
{
    XgPlatVideoConfig v = { 640, 480, 0, 1, 0, 0 };
    int n = xg_plat_config_parse(
        "# comment\n[video]\nwidth = 1280   # trailing comment\nheight=720\n"
        "fullscreen = on\n; other comment\nwidescreen = yes\nfps = native\n"
        "[game]\nspeed = 3\n[input]\ncircle = Z\n[audio]\nvolume = 50\n"
        "bogus line\ngame.speed = 4\n");
    CHECK(n == 9, "parsed key count (9 assignments)");
    CHECK(xg_plat_config_int("video.width", 0) == 1280, "section key video.width");
    CHECK(xg_plat_config_int("video.fullscreen", 0) == 1, "on -> 1");
    CHECK(xg_plat_config_int("game.speed", 0) == 4, "later dotted key overrides section key");
    CHECK(xg_plat_config_get("missing.key") == NULL, "missing key -> NULL");
    setenv("XENO_VIDEO_WIDTH", "1920", 1);
    CHECK(xg_plat_config_int("video.width", 0) == 1920, "XENO_VIDEO_WIDTH overrides the file");
    unsetenv("XENO_VIDEO_WIDTH");
    setenv("XENO_SPEED", "2", 1);
    CHECK(xg_plat_config_int("game.speed", 0) == 2, "XENO_SPEED alias overrides game.speed");
    unsetenv("XENO_SPEED");
    xg_plat_config_video(&v);
    CHECK(v.window_width == 1280 && v.window_height == 720 && v.fullscreen == 1 &&
          v.vsync == 1 && v.widescreen == 1, "video config applied, unset keys kept");
    xg_plat_config_apply_runtime();
    CHECK(s_speed == 4, "game.speed applied");
    CHECK(s_binds == 1 && strcmp(s_bound, "circle=Z") == 0, "input.circle bound");
    CHECK(s_volume_calls == 1, "audio.volume forwarded to the backend");
    xg_plat_config_parse("video.width = abc\n");
    CHECK(xg_plat_config_int("video.width", 77) == 77, "non-number falls back");
    printf("xg_plat config test: %s\n", fails ? "FAIL" : "PASS");
    return fails != 0;
}
