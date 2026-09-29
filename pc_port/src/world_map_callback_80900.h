/* Retail mode-13 marker scheduler callback pair. */
#ifndef WORLD_MAP_CALLBACK_80900_H
#define WORLD_MAP_CALLBACK_80900_H

#include "common.h"

/* 0x80080900 (init) runs the matched C: func_80080900 in src/world_map/main.c. */
s32 func_80080900(s32 arg0);
s32 wm_80080944(s32 slot_index);

#endif /* WORLD_MAP_CALLBACK_80900_H */
