#ifndef _XENO_SOUND_INPUT_H
#define _XENO_SOUND_INPUT_H

#include "psyq/libspu.h"
#include "psyq/libcd.h"

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

typedef struct {
    /* 0x0 */ undefined16 unk0; // Flags?
    /* 0x2 */ undefined16 unk2;
    /* 0x4 */ undefined32 unk4;
    /* 0x8 */ void* pPrev;
    /* 0xC */ void* pNext;
} SoundHeapBlockHeader;


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
    /* 0x1C */ struct SoundFile_t* pNext;
    /* 0x20 */ // starts of 0x2 size offsets to scripts. Pair of scripts for each instrument. 1 script for 1 channel.
};
typedef struct SoundFile_t SoundFile;

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
    /* 0x2C */ struct SoundWDSEntry_t* pNext;
};
typedef struct SoundWDSEntry_t SoundWDSEntry;

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
    /* 0x18  */ u8* savedScriptIP;
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
