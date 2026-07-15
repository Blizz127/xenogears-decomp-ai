#ifndef _XENO_SOUND_INPUT_H
#define _XENO_SOUND_INPUT_H

#include "psyq/libspu.h"
#include "psyq/libcd.h"

#ifdef XENO_PC_PORT
#include <stdint.h>
#endif

#define NUM_VOICES 24

#define NUM_NOTES_PER_OCTAVE 12

#define CHANNEL_RIGHT 0
#define CHANNEL_LEFT  1

typedef struct {
    s16 assignedVoice;
    u16 modeFlags;
    s16 priority;
    u16 flags;
    SpuVolume volume;
    u8 unkC[8];
    u16 pitch;
    u8 unk16[6];
    u32 startAddress;
    u32 loopAddress;
    u8 unkAdsr1;
    u8 unkAdsr2;
    u8 unkAdsr3;
    u8 unkAdsr4;
    u8 adsrDR;
    u8 unkAdsr5;
    u8 unkAdsr6;
    u8 adsrSR;
} SoundVoiceData;

#define SOUND_CTL_FLAG_IRQ_HANDLER (1 << 2)

#define SOUND_STATUS_OK 0x0
#define SOUND_ERR_INVALID_SIGNATURE 0x1
#define SOUND_ERR_INVALID_CHECKSUM 0x2 // Maybe
#define SOUND_ERR_UNK_0X4 0x4
#define SOUND_ERR_MANAGER_NOT_IN_LIST 0xF
#define SOUND_ERR_ENTRY_ALREADY_EXISTS 0x15
#define SOUND_ERR_UNEXPECTED_CALLBACK 0x26

#define SOUND_TRANSFER_QUEUE_SIZE 8

#define SOUND_SPU_COMMAND_WRITE 0x1
#define SOUND_SPU_COMMAND_READ 0x2

#define SOUND_WDS_ALLOCATE_AT_ADDRESS 0
#define SOUND_WDS_ALLOCATE_AUTOMATIC -1

#define FILE_SIGNATURE(a, b, c, d) (d<<24)+(c<<16)+(b<<8)+a

typedef void (*SoundCommandCallback_t)(void);

/*
 * Retail sound structures embed 32-bit PSX addresses.  Keep those slots at
 * their ABI width on LP64 hosts and convert explicitly at each use.  This is
 * the common accessor pattern for heap, file, WDS, element, and manager
 * structure pointers; native pointer fields must not be used for them.
 */
typedef u32 SoundPsxAddress;
#define SOUND_PSX_TO_PTR(type, address) ((type*)(uintptr_t)(address))
#define SOUND_PTR_TO_PSX(pointer) ((SoundPsxAddress)(uintptr_t)(pointer))

typedef struct {
    /* 0x0 */ undefined16 unk0; // Flags?
    /* 0x2 */ undefined16 unk2;
    /* 0x4 */ undefined32 unk4;
    /* 0x8 */ SoundPsxAddress pPrev;
    /* 0xC */ SoundPsxAddress pNext;
} SoundHeapBlockHeader;

#ifdef XENO_PC_PORT
_Static_assert(sizeof(SoundHeapBlockHeader) == 0x10,
               "SoundHeapBlockHeader must retain its retail size");
_Static_assert(__builtin_offsetof(SoundHeapBlockHeader, pPrev) == 0x8,
               "SoundHeapBlockHeader pPrev must retain its retail offset");
_Static_assert(__builtin_offsetof(SoundHeapBlockHeader, pNext) == 0xC,
               "SoundHeapBlockHeader pNext must retain its retail offset");
#endif


#define MAX_SPU_MEMORY_BLOCKS 0xC

#define SPU_MEMORY_FREE 0x0
#define SPU_MEMORY_RESERVED 0x1 // Or last block?
#define SPU_MEMORY_IN_USE 0x80

typedef struct {
    /* 0x0 */ u_char flags;
    /* 0x1 */ u_char unk1; // Type of memory/data?
    /* 0x2 */ short nextBlockIndex;
    /* 0x4 */ int spuAddress;
    /* 0x8 */ int size;
    /* 0xC */ u_int unkC; // Padding?
} SoundSpuMemoryBlock;

// Possible a more general queue to sending commands to the SPU,
// but all supported commands has to do with data transfer.
typedef struct {
    /* 0x0  */ u_short commandType;
    /* 0x2  */ short unk2;
    /* 0x4  */ void* pSpuData;
    /* 0x8  */ mem_addr pTransferAddress;
    /* 0xC  */ u_long dataSize;
    /* 0x10 */ SoundCommandCallback_t pCallbackFn;
} SoundTransferCommand;

// Possibly a struct which can either be a SMD (Background Music), SED (Sound Effect) or SND entry
struct SoundFile_t {
    /* 0x0  */ u_int magic;
    /* 0x4  */ undefined32 unk4;
    /* 0x8  */ undefined32 unk8; // Size?
    /* 0xC  */ undefined16 unkC; // File format version?
    /* 0xE  */ undefined16 unkE;
    /* 0x10 */ undefined32 unk10; // smdId?
    /* 0x14 */ u_short sedId;
    /* 0x16 */ u_short sndId;
    /* 0x18 */ undefined32 unk18;
    /* 0x1C */ SoundPsxAddress pNext;
    /* 0x20 */ // Variable payload: pairs of script offsets per instrument; one script per channel.
};
typedef struct SoundFile_t SoundFile;

#define SOUND_FILE_SCRIPT_OFFSETS_OFFSET 0x20
#define SOUND_FILE_SCRIPT_OFFSETS(soundFile) \
    ((u16*)((u8*)(soundFile) + SOUND_FILE_SCRIPT_OFFSETS_OFFSET))

#ifdef XENO_PC_PORT
_Static_assert(sizeof(SoundFile) == SOUND_FILE_SCRIPT_OFFSETS_OFFSET,
               "SoundFile header must end at the retail payload offset");
_Static_assert(__builtin_offsetof(SoundFile, pNext) == 0x1C,
               "SoundFile pNext must retain its retail offset");
#endif

struct SoundWDSEntry_t {
    /* 0x0  */ u_char _unk0[0x10];
    /* 0x10 */ u_int headerSizeMby;
    /* 0x14 */ u_int adpcmDataSize; // Sample size
    /* 0x18 */ u_int adpcmDataOffset; // Offset to data to write to SPU
    /* 0x1C */ u_short unk1C;
    /* 0x1E */ u_short unk1E;
    /* 0x20 */ u_short id;
    /* 0x22 */ u_short unk22;
    /* 0x24 */ u_int unk24;
    /* 0x28 */ int spuMemoryAddress; // Optional
    /* 0x2C */ SoundPsxAddress pNext;
};
typedef struct SoundWDSEntry_t SoundWDSEntry;

#ifdef XENO_PC_PORT
_Static_assert(sizeof(SoundWDSEntry) == 0x30,
               "SoundWDSEntry must retain its retail size");
_Static_assert(__builtin_offsetof(SoundWDSEntry, pNext) == 0x2C,
               "SoundWDSEntry pNext must retain its retail offset");
#endif

typedef struct {
    /* 0x0 */ int currentValue; // Current interpolated value
    /* 0x4 */ int stepIncrement; // Amount to add each step
    /* 0x8 */ short counter; // Steps remaining
    /* 0xA */ short targetValue; // Final value when counter reaches 0
} AudioInterpolator;

// AudioElement.active_flags
#define SOUND_CHANNEL_PERCUSSION_ACTIVE 0x10
#define SOUND_CHANNEL_NOTE_ACTIVE 0x80
#define SOUND_CHANNEL_HOLD_NOTE 0x100
#define SOUND_CHANNEL_REST_NOTE 0x400
#define SOUND_CHANNEL_CHANGE_INSTRUMENT 0x8000

// AudioElement.status_flags
#define SOUND_STATUS_REST_NOTE 0x2
#define SOUND_STATUS_VOLUME_CHANGE 0x100
#define SOUND_STATUS_PLAYING_NTOE 0x200

typedef struct {
    /* 0x0   */ u16 active_flag;
    /* 0x2   */ u16 status_flags;
    /* 0x4   */ u16 unk_0x04; // Instrument flags?
    /* 0x6   */ s8 unk_0x06[0xE];
    /* 0x14  */ u32 unk14;
    /* 0x18  */ SoundPsxAddress savedScriptIP;
    /* 0x1C  */ s8 unk_0x1C[0x07];
    /* 0x23  */ u8 savedOctave;
    /* 0x24  */ s16 unk_0x24; // loops?
    /* 0x26  */ u8 unk_0x26;
    /* 0x27  */ u8 voice_number;
    /* 0x28  */ u8 unk_0x28; // instrument number?
    /* 0x29  */ s8 unk29;
    /* 0x2A  */ s16 unk2A;
    /* 0x2C  */ u32 unk2C; // SoundWDSEntry* ?
    /* 0x30  */ SoundVoiceData voice_data;
    /* 0x5C  */ s16 fermataDuration;
    /* 0x5E  */ u8 unk_0x5E[0x04];
    /* 0x62  */ u16 unk_0x62;
    /* 0x64  */ u8 unk_0x64[0x02];
    /* 0x66  */ u16 octave;
    /* 0x68  */ u32 unk68; // Final note to play?
    /* 0x6C  */ u16 unk_0x6C;
    /* 0x6E  */ s16 unk_0x6E;
    /* 0x70  */ u8 unk_0x70[0x04];
    /* 0x74  */ u16 unk_0x74; // Pan?
    /* 0x76  */ u8 unk_0x76[0x58];
    /* 0xCE  */ u16 unk_0xCE;
    /* 0xD0  */ u8 unk_0xD0[0x26];
    /* 0xF6  */ u16 unk_0xF6;
    /* 0xF8  */ u8 unk_0xF8[0x1E];
    /* 0x116 */ u16 unk_0x116;
    /* 0x118 */ u8 unk_0x118[0x1E];
    /* 0x136 */ u16 unk_0x136;
    /* 0x138 */ u8 unk_0x138[0x20];
} AudioElement;

#ifdef XENO_PC_PORT
_Static_assert(sizeof(AudioElement) == 0x158,
               "AudioElement must retain its retail size");
_Static_assert(__builtin_offsetof(AudioElement, active_flag) == 0x0,
               "AudioElement active_flag must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, status_flags) == 0x2,
               "AudioElement status_flags must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, unk_0x04) == 0x4,
               "AudioElement unk_0x04 must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, unk_0x06) == 0x6,
               "AudioElement unk_0x06 must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, unk14) == 0x14,
               "AudioElement unk14 must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, savedScriptIP) == 0x18,
               "AudioElement savedScriptIP must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, unk_0x1C) == 0x1C,
               "AudioElement unk_0x1C must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, savedOctave) == 0x23,
               "AudioElement savedOctave must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, unk_0x24) == 0x24,
               "AudioElement unk_0x24 must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, unk_0x26) == 0x26,
               "AudioElement unk_0x26 must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, voice_number) == 0x27,
               "AudioElement voice_number must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, unk_0x28) == 0x28,
               "AudioElement unk_0x28 must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, unk29) == 0x29,
               "AudioElement unk29 must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, unk2A) == 0x2A,
               "AudioElement unk2A must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, unk2C) == 0x2C,
               "AudioElement unk2C must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, voice_data) == 0x30,
               "AudioElement voice_data must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, fermataDuration) == 0x5C,
               "AudioElement fermataDuration must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, unk_0x5E) == 0x5E,
               "AudioElement unk_0x5E must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, unk_0x62) == 0x62,
               "AudioElement unk_0x62 must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, unk_0x64) == 0x64,
               "AudioElement unk_0x64 must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, octave) == 0x66,
               "AudioElement octave must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, unk68) == 0x68,
               "AudioElement unk68 must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, unk_0x6C) == 0x6C,
               "AudioElement unk_0x6C must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, unk_0x6E) == 0x6E,
               "AudioElement unk_0x6E must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, unk_0x70) == 0x70,
               "AudioElement unk_0x70 must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, unk_0x74) == 0x74,
               "AudioElement unk_0x74 must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, unk_0x76) == 0x76,
               "AudioElement unk_0x76 must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, unk_0xCE) == 0xCE,
               "AudioElement unk_0xCE must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, unk_0xD0) == 0xD0,
               "AudioElement unk_0xD0 must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, unk_0xF6) == 0xF6,
               "AudioElement unk_0xF6 must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, unk_0xF8) == 0xF8,
               "AudioElement unk_0xF8 must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, unk_0x116) == 0x116,
               "AudioElement unk_0x116 must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, unk_0x118) == 0x118,
               "AudioElement unk_0x118 must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, unk_0x136) == 0x136,
               "AudioElement unk_0x136 must retain its retail offset");
_Static_assert(__builtin_offsetof(AudioElement, unk_0x138) == 0x138,
               "AudioElement unk_0x138 must retain its retail offset");
#endif

typedef struct AudioManager {
    /* 0x0  */ struct AudioManager* next;
    /* 0x4  */ struct AudioManager* unk_Manager_0x4;
    /* 0x8  */ s32 unk_0x8; // Seq file pointer?
    /* 0xC  */ s32 unk_0xc; // Pointer to percussion data?
    /* 0x10 */ s16 unk_Flags;
    /* 0x12 */ s8 unk2[2];
    /* 0x14 */ u8 elementCount;
    /* 0x15 */ u8 unk_0x15[3];
    /* 0x18 */ u8 unk_0x18;
    /* 0x19 */ u8 unk_0x19;
    /* 0x1A */ u8 unk_0x1a;
    /* 0x1B */ u8 unk_0x1b;
    /* 0x1C */ u8 unk_0x1c[4];
    /* 0x20 */ s32 unk_0x20;
    /* 0x24 */ s32 unk_0x24;
    /* 0x28 */ s32 unk_0x28;                  
    /* 0x2C */ s32 unk_0x2c;
    /* 0x30 */ s16 unk_0x30;
    /* 0x32 */ s16 unk_0x32;
    /* 0x34 */ s16 unk_0x34;
    /* 0x36 */ s16 unk_0x36;
    /* 0x38 */ s16 unk_0x38;
    /* 0x3A */ s16 unk_0x3a;
    /* 0x3C */ s16 unk_0x3c;
    /* 0x3E */ s16 unk_0x3e;
    /* 0x40 */ u8 unk_0x40[8];
    /* 0x48 */ s32 unk_0x48;
    /* 0x4C */ u8 unk_0x4c[4];
    /* 0x50 */ s32 unk_0x50;
    /* 0x54 */ s32 unk_0x54;
    /* 0x58 */ s32 unk_0x58;                          
    /* 0x5C */ s32 unk_0x5c;
    /* 0x60 */ s16 unk_0x60;
    /* 0x62 */ u8 unk_0x62[2];
    /* 0x64 */ AudioInterpolator unk_Interpolator_0x64;
    /* 0x70 */ AudioInterpolator unk_Interpolator_0x70;
    /* 0x7C */ AudioInterpolator unk_Interpolator_0x7c;
    /* 0x88 */ AudioInterpolator unk_Interpolator_0x88;
    /* 0x94 */ AudioElement elements[24];
} AudioManager;

typedef struct {
    /* 0x00 */ SpuCommonAttr commonAttr;

    // Volume state management
    /* 0x28 */ s16 currentMasterVolume;
    /* 0x2A */ s16 currentCdVolume;
    /* 0x2C */ s16 currentReverbDepth;
    /* 0x2E */ s16 unk_field2;

    /* 0x30 */ AudioInterpolator masterInterpolator;
    /* 0x3C */ AudioInterpolator cdInterpolator;
} SoundVolumeController;

#ifdef XENO_PC_PORT
_Static_assert(sizeof(SoundVolumeController) == 0x48,
               "SoundVolumeController must retain its retail size");
_Static_assert(__builtin_offsetof(SoundVolumeController, commonAttr) == 0x0,
               "SoundVolumeController commonAttr must retain its retail offset");
_Static_assert(__builtin_offsetof(SoundVolumeController, currentMasterVolume) == 0x28,
               "SoundVolumeController currentMasterVolume must retain its retail offset");
_Static_assert(__builtin_offsetof(SoundVolumeController, currentCdVolume) == 0x2A,
               "SoundVolumeController currentCdVolume must retain its retail offset");
_Static_assert(__builtin_offsetof(SoundVolumeController, currentReverbDepth) == 0x2C,
               "SoundVolumeController currentReverbDepth must retain its retail offset");
_Static_assert(__builtin_offsetof(SoundVolumeController, unk_field2) == 0x2E,
               "SoundVolumeController unk_field2 must retain its retail offset");
_Static_assert(__builtin_offsetof(SoundVolumeController, masterInterpolator) == 0x30,
               "SoundVolumeController masterInterpolator must retain its retail offset");
_Static_assert(__builtin_offsetof(SoundVolumeController, cdInterpolator) == 0x3C,
               "SoundVolumeController cdInterpolator must retain its retail offset");
#endif

extern SoundVolumeController g_SoundVolumeController;

extern s32 g_ReverbWorkAreaSizes[SPU_REV_MODE_MAX];
extern u8 g_SoundReverbType;
extern u8 g_SoundReverbDelay;
extern u8 g_SoundReverbFeedback;
extern s32 g_SoundReverbMemoryHandle;

extern s32 D_80059404;

// Heap
extern SoundHeapBlockHeader* g_SoundHeapHead;
extern u32 g_SoundHeapEnd;
extern u32 g_SoundHeapSize;

// SPU Transfer Command Queue
extern s16 D_80059548;
extern SoundTransferCommand* g_SoundTransferQueue;
extern u16 g_SoundTransferQueueReadIndex;
extern u16 g_SoundTransferQueueWriteIndex;

// SPU Memory Management
extern SoundSpuMemoryBlock g_SoundSpuMemoryBlocks[MAX_SPU_MEMORY_BLOCKS];

extern s32 g_SoundUploadDestBuffer;
extern s32 g_SoundUploadSourceAddr;
extern s32 g_SoundUploadBytesRemaining;
extern u_long g_unk_SoundEvent; // Event Descriptor

extern SoundFile* g_SoundSedsLinkedList;
extern SoundWDSEntry* g_SoundWdsLinkedList;

extern int g_SoundWdsCurSpuAddress;
extern int g_SoundWdsRemainingBytes;

extern short g_SoundSpuErrorId;
extern long g_unk_VoicesNeedingProcessing;
extern AudioManager* g_SoundAudioManagerListHead;
extern short g_SoundControlFlags;
extern SpuIRQCallbackProc g_SoundSpuIrqCallbackFn;
extern int g_SoundSpuIRQCount;

extern u32 g_SoundKeyOnFlags;
extern u32 g_SoundKeyOffFlags;
extern SoundVoiceData* g_SoundChannels[24];

extern SpuVolume g_SoundReverbDepth;

extern CdlATV g_SoundCdRomAttenuation;

// Heap / SPU-memory helpers. Their return and argument widths are established
// by retail sound.s (0x80038F18, 0x800397C0, and 0x8003F614 respectively).
void* SoundHeapAllocate(u32 allocSize);
void SoundHeapClearBlockMemory(void* pMemory, s32 size);
SoundSpuMemoryBlock* SoundSpuMemoryFindBlock(s32 targetAddress);
int SoundValidateFile(SoundFile* pSoundFile, u32 magicBytes, u16 targetValue);

extern s32 SoundCalculateAudioManagerSize(s32 elementCount);
extern void SoundSetVolumeWithPhase(s32, SpuVolume*, s32);

#endif
