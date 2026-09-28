/* xg_plat/audio.h -- audio interface: user-side output controls.  The
 * game's SPU/sequencer path stays on PsyCross's libspu emulation (OpenAL);
 * the game itself owns the PSX master volume (SpuSetCommonAttr), so a user
 * volume must be a separate gain stage after it.  Backend:
 * src/plat/xg_plat_psycross.c. */
#ifndef XG_PLAT_AUDIO_H
#define XG_PLAT_AUDIO_H

#ifdef __cplusplus
extern "C" {
#endif

/* User output volume, 0 (mute) .. 100.  Returns 1 when the backend applied
 * it, 0 when it has no user gain stage (the PsyCross backend does not yet). */
int xg_plat_audio_set_user_volume(int percent);

#ifdef __cplusplus
}
#endif

#endif
