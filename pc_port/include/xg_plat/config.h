/* xg_plat/config.h -- port configuration (moddable port layer).
 *
 * One `key = value` per line, `#` or `;` comments, optional `[section]`
 * headers that prefix keys ("[video]" + "width = 1280" == "video.width").
 * The first file found is used:
 *   $XENO_CONFIG, ./xenogears-port.cfg,
 *   ${XDG_CONFIG_HOME:-~/.config}/xenogears-port/xenogears-port.cfg
 * Every key can be overridden from the environment as XENO_<KEY> with dots
 * as underscores, upper-case ("video.width" -> XENO_VIDEO_WIDTH); a few
 * historical names are aliases (XENO_SPEED == game.speed).  Environment wins
 * over the file.  Keys: see pc_port/xenogears-port.example.cfg.
 */
#ifndef XG_PLAT_CONFIG_H
#define XG_PLAT_CONFIG_H

#include "renderer.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Load the config file (once).  Returns the number of keys read, 0 when no
 * file exists, -1 on a read error. */
int xg_plat_config_load(void);
/* Path of the loaded file, or NULL. */
const char* xg_plat_config_path(void);
/* Value for `key` (environment override first), or NULL. */
const char* xg_plat_config_get(const char* key);
int xg_plat_config_int(const char* key, int fallback);
/* Parse text directly (tests; replaces any loaded values). */
int xg_plat_config_parse(const char* text);

/* Apply the [video] keys to a renderer config before xg_plat_renderer_init. */
void xg_plat_config_video(XgPlatVideoConfig* video);
/* Apply game.speed, audio.volume and input.* after the backend is up. */
void xg_plat_config_apply_runtime(void);

#ifdef __cplusplus
}
#endif

#endif
