#include "psx_memory.h"
#include "world_map_gamestate_alias.h"

void wm_sync_gamestate_channel_aliases(const u8* host_gamestate)
{
    u8* guest_channels =
        (u8*)PSX_ADDR(WM_GAMESTATE_CHANNEL_GUEST_BASE);
    u32 channel;

    for (channel = 0; channel < WM_GAMESTATE_CHANNEL_COUNT; channel++) {
        guest_channels[channel] =
            host_gamestate[WM_GAMESTATE_CHANNEL_HOST_OFFSET + channel];
    }
}
