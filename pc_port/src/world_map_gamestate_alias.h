/*
 * Retail GameState channel-control aliases used by world callbacks.
 *
 * On PSX, GameState offset 0x1D34 is absolute address 0x8006F368, so the
 * three channel IDs and the callback-visible bytes are the same storage.
 * The PC port keeps authoritative GameState in a separate host allocation;
 * this bridge restores only those three retail aliases before world package
 * creation and scheduler execution.
 */
#ifndef WORLD_MAP_GAMESTATE_ALIAS_H
#define WORLD_MAP_GAMESTATE_ALIAS_H

#include "common.h"

#define WM_GAMESTATE_CHANNEL_HOST_OFFSET 0x1D34u
#define WM_GAMESTATE_CHANNEL_GUEST_BASE  0x8006F368u
#define WM_GAMESTATE_CHANNEL_COUNT       3u

void wm_sync_gamestate_channel_aliases(const u8* host_gamestate);

#endif /* WORLD_MAP_GAMESTATE_ALIAS_H */
