#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main66", func_800A22E8);
#endif
#ifndef XENO_PC_PORT
/* A pool of 20-byte slots: base, first free index, slot count. */
typedef struct {
    u8* base;
    u16 firstFree;
    u16 count;
} Pool20;

/* Take the first free slot (NULL when full) and advance the first-free index. */
u8* func_800A2330(Pool20* pool) {
    u8* slot;

    if (pool->firstFree < pool->count) {
        slot = pool->base + pool->firstFree * 20;
        if (*slot != 0) {
            return NULL;
        }
        for (pool->firstFree++; pool->firstFree < pool->count; pool->firstFree++) {
            if (pool->base[pool->firstFree * 20] == 0) {
                break;
            }
        }
        return slot;
    }
    return NULL;
}

/* Release a 20-byte pool slot: clear it and lower the first-free index. */
s32 func_800A23E8(Pool20* pool, u8* slot) {
    s32 index;

    if (slot == NULL) {
        return -1;
    }
    index = (u32)(slot - pool->base) / 20;
    if (index < pool->firstFree) {
        pool->firstFree = index;
    }
    *slot = 0;
    return index;
}
#endif
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main66", func_800A2434);
INCLUDE_ASM("asm/battle/nonmatchings/main66", func_800A2704);
INCLUDE_ASM("asm/battle/nonmatchings/main66", func_800A2ACC);
typedef struct BattleNode7C {
    struct BattleNode7C* parent;
    u8 unk4[3];
    u8 pending;
    u8 unk8[2];
    u16 index;
    u8 unkC[0x64];
    u8* handle70;
    u8* handle74;
    u8* handle78;
} BattleNode7C;

void func_800A2BB8(s32 ctx, BattleNode7C* nodes, u8 id) {
    u8 pad[1];
    s32 i;
    s32 count = nodes->index;

    for (i = 0; i < count; i++, nodes++) {
        if (nodes->handle70 != NULL && nodes->handle70[3] == id) {
            func_800A23E8(ctx, nodes->handle70);
            nodes->handle70 = NULL;
        }
        if (nodes->handle74 != NULL && nodes->handle74[3] == id) {
            func_800A23E8(ctx, nodes->handle74);
            nodes->handle74 = NULL;
        }
        if (nodes->handle78 != NULL && nodes->handle78[3] == id) {
            func_800A23E8(ctx, nodes->handle78);
            nodes->handle78 = NULL;
        }
    }
}

#endif
extern void HeapChangeCurrentUser(u32 user, u32 arg);
extern void* HeapAlloc(u32 size, u32 flags);
extern void func_800A2D5C(u8* p);
/* func_800A2CA4.s: allocate (n + 1) 124-byte entries for the list at p
 * (count at +4, cursor +6 zeroed, buffer at +0) and initialise it with
 * func_800A2D5C; NULL when the allocation fails. */
u8* func_800A2CA4(u8* p, s32 n) {
    u8* r;

    HeapChangeCurrentUser(4, 0);
    *(s16*)(p + 4) = n;
    *(s16*)(p + 6) = 0;
    *(u8**)(p + 0) = HeapAlloc((n + 1) * 124, 0);
    if (*(u8**)(p + 0) == NULL) {
        r = NULL;
    } else {
        func_800A2D5C(p);
        r = p;
    }
    return r;
}


extern void HeapFree(u32 p);


/* func_800A2D1C.s */
void func_800A2D1C(u8* p) {
    u32 q = *(u32*)p;

    *(u16*)(p + 4) = 0;
    *(u16*)(p + 6) = 0;
    if (q != 0) {
        HeapFree(q);
    }
    *(u32*)p = 0;
}
