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

/* Guest-pointer domain seams for world-map bodies the port compiles. Guest
 * RAM stores 32-bit PSX addresses: WM_GPTR turns a loaded guest word into a
 * pointer, WM_GADDR turns a pointer back into the guest address to store.
 * The port gets PSX_ADDR / PsxMemory_GuestAddr from psx_memory.h (pulled in
 * by world_map_port_data.h before any use); retail is a plain cast, so the
 * matching build emits the same lw/sw. */
#ifdef XENO_PC_PORT
#define WM_GPTR(g)  ((void*)PSX_ADDR((u32)(g)))
#define WM_GADDR(p) (PsxMemory_GuestAddr((const void*)(p)))
#else
#define WM_GPTR(g)  ((void*)(g))
#define WM_GADDR(p) ((u32)(p))
#endif

#endif /* WORLD_MAP_WM_SPAD_H */
