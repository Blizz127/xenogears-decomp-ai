#include "common.h"
#include "system/archive.h"
#include "system/memory.h"

extern int ArchiveDecodeSizeAbsolute(int index);
extern int ArchiveDecodeAlignedSize(int index);
extern int func_80029AFC(StreamDataQueueEntry* entries, int arg1, int arg2);
extern u8* D_800658C8;

#ifdef XENO_PC_PORT
#include "../../../pc_port/src/psx_memory.h"
/* Keep the queue in its retail RAM location and eight-byte layout. The native
 * archive adapter already accepts guest-RAM queues, translates their pointers
 * and consumes them synchronously. A host pointer must not occupy two words. */
typedef struct {
    s16 archiveIndex;
    u16 pad;
    u32 pData;
} AssetQueueEntry;
_Static_assert(sizeof(AssetQueueEntry) == 8, "retail archive queue stride");
#define ASSET_QUEUE ((AssetQueueEntry*)PSX_ADDR(0x8005A1DCu))
#define QUEUE_ADDRESS(p) PsxMemory_GuestAddr(p)
#define QUEUE_BUFFER(p) ((u8*)PSX_ADDR(p))
#else
extern StreamDataQueueEntry D_8005A1DC[3];
#define ASSET_QUEUE D_8005A1DC
#define QUEUE_ADDRESS(p) (p)
#define QUEUE_BUFFER(p) ((u8*)(p))
#endif

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): mode_load_battle_stage (src/decomp/src/resident/console_and_sound_driver.c:726). */
#ifndef XM_T_FileRequest
#define XM_T_FileRequest
typedef struct FileRequest {
    u16 file;
    void *destination;
} FileRequest;
#endif
#ifndef XM_S_mode_load_battle_stage
#define XM_S_mode_load_battle_stage
s32 xm_mode_load_battle_stage(s32 stage, s32 variant, u8 **stage_file, s32 *unused, u8 **scene) __asm__("func_800379D8");
#endif
#ifndef XM_S_cd_get_selected_directory
#define XM_S_cd_get_selected_directory
s32 xm_cd_get_selected_directory(s32 *group, s32 *index) __asm__("ArchiveGetArchiveOffsetIndices");
#endif
#ifndef XM_S_cd_select_directory
#define XM_S_cd_select_directory
s32 xm_cd_select_directory(s32 group, s32 index) __asm__("ArchiveSetIndex");
#endif
#ifndef XM_S_heap_select_owner_tag
#define XM_S_heap_select_owner_tag
void xm_heap_select_owner_tag(s32 tag, char **names) __asm__("HeapChangeCurrentUser");
#endif
#ifndef XM_S_cd_get_directory_file_count
#define XM_S_cd_get_directory_file_count
s16 xm_cd_get_directory_file_count(s32 file) __asm__("ArchiveDecodeSizeAbsolute");
#endif
#ifndef XM_S_heap_alloc
#define XM_S_heap_alloc
void *xm_heap_alloc(s32 size, s32 mode) __asm__("HeapAlloc");
#endif
#ifndef XM_S_cd_get_aligned_file_size
#define XM_S_cd_get_aligned_file_size
s32 xm_cd_get_aligned_file_size(s32 file) __asm__("ArchiveDecodeAlignedSize");
#endif
#ifndef XM_S_heap_protect_block
#define XM_S_heap_protect_block
void xm_heap_protect_block(void *data) __asm__("HeapPinBlock");
#endif
#ifndef XM_S_mode_battle_stage_file_list
#define XM_S_mode_battle_stage_file_list
extern FileRequest xm_mode_battle_stage_file_list[4] __asm__("D_8005A1DC");
#endif
#ifndef XM_S_cd_read_file_list
#define XM_S_cd_read_file_list
s32 xm_cd_read_file_list(FileRequest *list, s32 mode, s32 unused) __asm__("func_80029AFC");
#endif
#ifndef XM_S_mode_battle_scene_data
#define XM_S_mode_battle_scene_data
extern u8 *xm_mode_battle_scene_data __asm__("D_800658C8");
#endif
s32 func_800379D8(s32 stage, s32 variant, u8 **stage_file, s32 *unused, u8 **scene) {
    s32 group;
    s32 index;
    s32 result = 0;
    u8 *scene_file;
    u8 *data;
    s32 file;
    s32 base;

    xm_cd_get_selected_directory(&group, &index);
    xm_cd_select_directory(12, 3);
    xm_heap_select_owner_tag(4, 0);
    if (stage >= xm_cd_get_directory_file_count(5) / 2) {
        result = -1;
        *stage_file = NULL;
        *unused = 0;
        *scene = NULL;
    } else {
        base = stage * 2;
        scene_file = xm_heap_alloc(xm_cd_get_aligned_file_size(base + 7 + variant), 1);
        xm_heap_protect_block(scene_file);
        file = base + 6;
        data = xm_heap_alloc(xm_cd_get_aligned_file_size(file), 1);
        xm_heap_protect_block(data);
        xm_mode_battle_stage_file_list[0].file = file;
        xm_mode_battle_stage_file_list[0].destination = data;
        base += variant + 7;
        xm_mode_battle_stage_file_list[1].file = base;
        xm_mode_battle_stage_file_list[1].destination = scene_file;
        xm_mode_battle_stage_file_list[2].file = 0;
        xm_mode_battle_stage_file_list[2].destination = NULL;
        xm_cd_read_file_list(xm_mode_battle_stage_file_list, 0, 0);
        *stage_file = data;
        *unused = 0;
        *scene = (u8 *)xm_mode_battle_stage_file_list[1].destination + 4;
        xm_mode_battle_scene_data = (u8 *)xm_mode_battle_stage_file_list[1].destination + 4;
    }
    xm_cd_select_directory(group, index);
    return result;
}
#endif
#else
/* Retail 800379D8..80037B88. Loads the selected pair from archive 0xC/3,
 * retaining both pinned allocations and exposing the second file past its
 * four-byte header. No replacement data or success-on-failure path is added. */
s32 func_800379D8(s32 index, s32 variant,
                  u8** first, u8** second, u8** third)
{
    s32 savedDirectory, savedEntry;
    s32 result = 0;
    s32 count;
    u8* firstData;
    u8* secondData;

    ArchiveGetArchiveOffsetIndices(&savedDirectory, &savedEntry);
    ArchiveSetIndex(0xC, 3);
    HeapChangeCurrentUser(4, NULL);
    count = (s16)ArchiveDecodeSizeAbsolute(5) / 2;
    if (index >= count) {
        result = -1;
        *first = NULL;
        *second = NULL;
        *third = NULL;
    } else {
        /* SLL/ADDU/ADDIU are modulo 32 bits, including signed input cases. */
        s32 firstFile = (s32)((u32)index * 2u + 6u);
        s32 secondFile = (s32)((u32)index * 2u + 7u + (u32)variant);
        secondData = HeapAlloc(ArchiveDecodeAlignedSize(secondFile), 1);
        HeapPinBlock((HeapBlock*)secondData);
        firstData = HeapAlloc(ArchiveDecodeAlignedSize(firstFile), 1);
        HeapPinBlock((HeapBlock*)firstData);

        ASSET_QUEUE[0].archiveIndex = (s16)firstFile;
        ASSET_QUEUE[0].pData = QUEUE_ADDRESS(firstData);
        ASSET_QUEUE[1].archiveIndex = (s16)secondFile;
        ASSET_QUEUE[1].pData = QUEUE_ADDRESS(secondData);
        ASSET_QUEUE[2].archiveIndex = 0;
        ASSET_QUEUE[2].pData = 0;
        func_80029AFC((StreamDataQueueEntry*)ASSET_QUEUE, 0, 0);

        *first = firstData;
        *second = NULL;
        /* Retail reloads queue[1] after the archive call can sort the queue. */
        *third = QUEUE_BUFFER(ASSET_QUEUE[1].pData) + 4;
        D_800658C8 = QUEUE_BUFFER(ASSET_QUEUE[1].pData) + 4;
    }
    ArchiveSetIndex(savedDirectory, savedEntry);
    return result;
}
#endif
