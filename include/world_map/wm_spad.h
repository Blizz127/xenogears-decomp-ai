#ifndef WORLD_MAP_WM_SPAD_H
#define WORLD_MAP_WM_SPAD_H

/* PSX scratchpad (0x1F800000, 1 KiB) addresses as the world-map code writes
 * them. The retail build keeps the literal address; the port maps it onto its
 * emulated scratchpad buffer. Byte-neutral for the matching build. */
#ifdef XENO_PC_PORT
extern unsigned char g_PsxScratchpad[];
#define WM_SPAD(a) ((void*)(g_PsxScratchpad + ((unsigned int)(a) & 0x3FFu)))
#else
#define WM_SPAD(a) (a)
#endif

#endif /* WORLD_MAP_WM_SPAD_H */
