/* xg_plat configuration loader (see include/xg_plat/config.h). */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/xg_plat/config.h"
#include "../../include/xg_plat/audio.h"
#include "../../include/xg_plat/input.h"
#include "../../include/xg_plat/timing.h"

#define CFG_MAX 128

typedef struct { char key[64]; char value[192]; } CfgEntry;

static CfgEntry s_entries[CFG_MAX];
static int s_count;
static int s_loaded;
static char s_path[1024];

static char* trim(char* s)
{
    char* e;
    while (isspace((unsigned char)*s))
        s++;
    e = s + strlen(s);
    while (e > s && isspace((unsigned char)e[-1]))
        *--e = '\0';
    return s;
}

static void set_entry(const char* key, const char* value)
{
    int i;
    for (i = 0; i < s_count; i++) {
        if (strcmp(s_entries[i].key, key) == 0) {
            snprintf(s_entries[i].value, sizeof s_entries[i].value, "%s", value);
            return;
        }
    }
    if (s_count < CFG_MAX) {
        snprintf(s_entries[s_count].key, sizeof s_entries[s_count].key, "%s", key);
        snprintf(s_entries[s_count].value, sizeof s_entries[s_count].value, "%s", value);
        s_count++;
    } else {
        fprintf(stderr, "[xg_plat] config: more than %d keys, ignoring %s\n", CFG_MAX, key);
    }
}

int xg_plat_config_parse(const char* text)
{
    char section[64] = "";
    char line[512];
    const char* p = text;
    int n = 0;

    s_count = 0;
    while (*p) {
        size_t len = strcspn(p, "\n");
        char* s;
        char* eq;
        if (len >= sizeof line)
            len = sizeof line - 1;
        memcpy(line, p, len);
        line[len] = '\0';
        p += strcspn(p, "\n");
        if (*p == '\n')
            p++;
        s = trim(line);
        if (*s == '\0' || *s == '#' || *s == ';')
            continue;
        if (*s == '[') {
            char* close = strchr(s, ']');
            if (close) {
                *close = '\0';
                snprintf(section, sizeof section, "%s", trim(s + 1));
            }
            continue;
        }
        eq = strchr(s, '=');
        if (eq == NULL) {
            fprintf(stderr, "[xg_plat] config: ignoring line without '=': %s\n", s);
            continue;
        }
        *eq = '\0';
        {
            char key[64];
            char* k = trim(s);
            char* v = trim(eq + 1);
            char* hash = strchr(v, '#');
            if (hash) {
                *hash = '\0';
                v = trim(v);
            }
            if (section[0] && strchr(k, '.') == NULL)
                snprintf(key, sizeof key, "%s.%s", section, k);
            else
                snprintf(key, sizeof key, "%s", k);
            for (k = key; *k; k++)
                *k = (char)tolower((unsigned char)*k);
            set_entry(key, v);
            n++;
        }
    }
    return n;
}

int xg_plat_config_load(void)
{
    const char* env = getenv("XENO_CONFIG");
    const char* xdg = getenv("XDG_CONFIG_HOME");
    const char* home = getenv("HOME");
    char cand[3][1024];
    int ncand = 0, i, n;
    FILE* f = NULL;

    if (s_loaded)
        return s_count;
    s_loaded = 1;
    if (env && *env)
        snprintf(cand[ncand++], sizeof cand[0], "%s", env);
    snprintf(cand[ncand++], sizeof cand[0], "xenogears-port.cfg");
    if (xdg && *xdg)
        snprintf(cand[ncand++], sizeof cand[0], "%s/xenogears-port/xenogears-port.cfg", xdg);
    else if (home && *home)
        snprintf(cand[ncand++], sizeof cand[0], "%s/.config/xenogears-port/xenogears-port.cfg", home);
    for (i = 0; i < ncand && f == NULL; i++) {
        f = fopen(cand[i], "rb");
        if (f) {
            strncpy(s_path, cand[i], sizeof s_path - 1);
            s_path[sizeof s_path - 1] = '\0';
        }
    }
    if (f == NULL) {
        if (env && *env)
            fprintf(stderr, "[xg_plat] config: XENO_CONFIG=%s not readable; using defaults\n", env);
        return 0;
    }
    {
        char* text;
        long len;
        if (fseek(f, 0, SEEK_END) != 0 || (len = ftell(f)) < 0 || fseek(f, 0, SEEK_SET) != 0 ||
            (text = (char*)malloc((size_t)len + 1)) == NULL) {
            fclose(f);
            return -1;
        }
        len = (long)fread(text, 1, (size_t)len, f);
        text[len] = '\0';
        fclose(f);
        n = xg_plat_config_parse(text);
        free(text);
    }
    printf("[xg_plat] config: %d key(s) from %s\n", n, s_path);
    return n;
}

const char* xg_plat_config_path(void)
{
    return s_path[0] ? s_path : NULL;
}

static const char* env_override(const char* key)
{
    static const struct { const char* key; const char* env; } aliases[] = {
        { "game.speed", "XENO_SPEED" },
    };
    char name[96] = "XENO_";
    size_t i, j = 5;
    const char* v;
    for (i = 0; key[i] && j + 1 < sizeof name; i++)
        name[j++] = key[i] == '.' ? '_' : (char)toupper((unsigned char)key[i]);
    name[j] = '\0';
    v = getenv(name);
    if (v && *v)
        return v;
    for (i = 0; i < sizeof aliases / sizeof aliases[0]; i++) {
        if (strcmp(aliases[i].key, key) == 0 && (v = getenv(aliases[i].env)) && *v)
            return v;
    }
    return NULL;
}

const char* xg_plat_config_get(const char* key)
{
    const char* env = env_override(key);
    int i;
    if (env)
        return env;
    for (i = 0; i < s_count; i++)
        if (strcmp(s_entries[i].key, key) == 0)
            return s_entries[i].value;
    return NULL;
}

int xg_plat_config_int(const char* key, int fallback)
{
    const char* v = xg_plat_config_get(key);
    char* end;
    long n;
    if (v == NULL)
        return fallback;
    if (!strcmp(v, "on") || !strcmp(v, "true") || !strcmp(v, "yes"))
        return 1;
    if (!strcmp(v, "off") || !strcmp(v, "false") || !strcmp(v, "no"))
        return 0;
    n = strtol(v, &end, 0);
    if (end == v || *end != '\0') {
        fprintf(stderr, "[xg_plat] config: %s = '%s' is not a number; using %d\n", key, v, fallback);
        return fallback;
    }
    return (int)n;
}

void xg_plat_config_video(XgPlatVideoConfig* video)
{
    const char* fps = xg_plat_config_get("video.fps");
    video->window_width = xg_plat_config_int("video.width", video->window_width);
    video->window_height = xg_plat_config_int("video.height", video->window_height);
    video->fullscreen = xg_plat_config_int("video.fullscreen", video->fullscreen);
    video->vsync = xg_plat_config_int("video.vsync", video->vsync);
    video->bilinear = xg_plat_config_int("video.bilinear", video->bilinear);
    video->widescreen = xg_plat_config_int("video.widescreen", video->widescreen);
    if (fps && strcmp(fps, "native") != 0 && strcmp(fps, "30") != 0)
        fprintf(stderr, "[xg_plat] config: video.fps = %s needs frame interpolation, which "
                        "no renderer backend has yet; running at the game's native rate\n", fps);
}

void xg_plat_config_apply_runtime(void)
{
    const char* v;
    int b;
    if (xg_plat_config_get("game.speed"))
        xg_plat_timing_set_speed(xg_plat_config_int("game.speed", 1));
    if (xg_plat_config_get("audio.volume") &&
        !xg_plat_audio_set_user_volume(xg_plat_config_int("audio.volume", 100)))
        fprintf(stderr, "[xg_plat] config: audio.volume is not supported by this audio "
                        "backend yet; ignored\n");
    for (b = 0; b < XG_PLAT_BTN_COUNT; b++) {
        char key[32];
        snprintf(key, sizeof key, "input.%s", xg_plat_input_button_name((XgPlatButton)b));
        if ((v = xg_plat_config_get(key)) != NULL && !xg_plat_input_bind_key((XgPlatButton)b, v))
            fprintf(stderr, "[xg_plat] config: %s = '%s' is not a key name\n", key, v);
    }
}
