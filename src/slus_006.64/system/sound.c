#include "common.h"
#include "system/sound.h"
#include "psyq/kernel.h"
#include "psyq/libspu.h"

//----------------------------------------------------------------------------------------------------------------------
// SPU DECLARATIONS
//----------------------------------------------------------------------------------------------------------------------
typedef struct {
    /* 0x0 */ SpuVolume volume;
    /* 0x4 */ u16 pitch;
    /* 0x6 */ u16 addr;
    /* 0x8 */ u16 adsr[2];
    /* 0xC */ u16 volumex;
    /* 0xE */ u16 loop_addr;
} SPU_VOICE_REG;

#define SPU_VOICE_REG_VOLUME_L    0
#define SPU_VOICE_REG_VOLUME_R    1
#define SPU_VOICE_REG_PITCH       2
#define SPU_VOICE_REG_ADDR        3
#define SPU_VOICE_REG_ADSR1       4
#define SPU_VOICE_REG_ADSR2       5
#define SPU_VOICE_REG_VOLUMEX     6
#define SPU_VOICE_REG_LOOP_ADDR   7
#define SPU_VOICE_REG_SIZE        8
#define NUM_VOICES               24

typedef struct {
    // APF Displacement registers (1F801DC0h - 1F801DC2h)
    u16 m_dAPF1;
    u16 m_dAPF2;

    // Volume registers (1F801DC4h - 1F801DD2h)
    s16 m_vIIR;
    s16 m_vCOMB1;
    s16 m_vCOMB2;
    s16 m_vCOMB3;
    s16 m_vCOMB4;
    s16 m_vWALL;
    s16 m_vAPF1;
    s16 m_vAPF2;

    // Same Side Reflection Address registers (1F801DD4h - 1F801DD6h)
    u16 m_mLSAME;
    u16 m_mRSAME;

    // Comb Address registers (1F801DD8h - 1F801DDEh)
    u16 m_mLCOMB1;
    u16 m_mRCOMB1;
    u16 m_mLCOMB2;
    u16 m_mRCOMB2;

    // Same Side Reflection Address 2 registers (1F801DE0h - 1F801DE2h)
    u16 m_dLSAME;
    u16 m_dRSAME;

    // Different Side Reflection Address registers (1F801DE4h - 1F801DE6h)
    u16 m_mLDIFF;
    u16 m_mRDIFF;

    // Comb Address registers 3-4 (1F801DE8h - 1F801DEEh)
    u16 m_mLCOMB3;
    u16 m_mRCOMB3;
    u16 m_mLCOMB4;
    u16 m_mRCOMB4;

    // Different Side Reflection Address 2 registers (1F801DF0h - 1F801DF2h)
    u16 m_dLDIFF;
    u16 m_dRDIFF;

    // APF Address registers (1F801DF4h - 1F801DFAh)
    u16 m_mLAPF1;
    u16 m_mRAPF1;
    u16 m_mLAPF2;
    u16 m_mRAPF2;

    // Input Volume registers (1F801DFCh - 1F801DFEh)
    s16 m_vLIN;
    s16 m_vRIN;
} ReverbRegisters;

typedef struct {
    SPU_VOICE_REG voice[NUM_VOICES];
    // Volumes
    SpuVolume main_vol; // 1-bit for Volume Mode, 15-bits for Volume
    SpuVolume rev_vol; // Full 16 bits for volume

    // Voice Flags
    u16 key_on[2];
    u16 key_off[2];
    u16 chan_fm[2];
    u16 noise_mode[2];
    u16 rev_mode[2];
    u32 m_EndxFlags;

    u16 unk;

    // Memory
    u16 rev_work_addr;
    u16 irq_addr;
    u16 trans_addr;
    u16 trans_fifo;

    // Control
    u16 spucnt;
    u16 data_trans;
    u16 spustat;

    // Aux volumes
    SpuVolume cd_vol;
    SpuVolume ex_vol;

    SpuVolume main_volx;

    u32 unk2;

    ReverbRegisters m_Reverb;
} SPU_RXX;

// Voice Registers (0x00 - 0xBF)
#define SPU_RXX_VOICE_BASE              0x00    // Voice registers start (24 voices × 8 regs each)
#define SPU_RXX_VOICE_SIZE              8       // Registers per voice
#define SPU_RXX_VOICE_END               0xBF    // Last voice register
// Global Volume Registers (0xC0 - 0xC3)
#define SPU_RXX_MAIN_VOL_L              0xC0    // Main volume left
#define SPU_RXX_MAIN_VOL_R              0xC1    // Main volume right
#define SPU_RXX_REV_VOL_L               0xC2    // Reverb volume left
#define SPU_RXX_REV_VOL_R               0xC3    // Reverb volume right
// Voice Flag Registers (0xC4 - 0xCF) - 32-bit values stored as pairs
#define SPU_RXX_KEY_ON_LOW              0xC4    // Key on flags (bits 0-15)
#define SPU_RXX_KEY_ON_HIGH             0xC5    // Key on flags (bits 16-31)
#define SPU_RXX_KEY_OFF_LOW             0xC6    // Key off flags (bits 0-15)
#define SPU_RXX_KEY_OFF_HIGH            0xC7    // Key off flags (bits 16-31)
#define SPU_RXX_PITCH_MOD_LOW           0xC8    // Pitch modulation flags (bits 0-15)
#define SPU_RXX_PITCH_MOD_HIGH          0xC9    // Pitch modulation flags (bits 16-31)
#define SPU_RXX_NOISE_LOW               0xCA    // Noise flags (bits 0-15)
#define SPU_RXX_NOISE_HIGH              0xCB    // Noise flags (bits 16-31)
#define SPU_RXX_REVERB_LOW              0xCC    // Reverb flags (bits 0-15)
#define SPU_RXX_REVERB_HIGH             0xCD    // Reverb flags (bits 16-31)
#define SPU_RXX_ENDX_LOW                0xCE    // End flags (bits 0-15)
#define SPU_RXX_ENDX_HIGH               0xCF    // End flags (bits 16-31)
// Memory Address Registers (0xD0 - 0xD4)
#define SPU_RXX_UNKNOWN_D0              0xD0    // Unknown register
#define SPU_RXX_REV_WA_START_ADDR       0xD1    // Reverb work area start address
#define SPU_RXX_IRQ_ADDR                0xD2    // IRQ address
#define SPU_RXX_TRANS_ADDR              0xD3    // Transfer address
#define SPU_RXX_TRANS_FIFO              0xD4    // Transfer FIFO
// Control Registers (0xD5 - 0xD7)
#define SPU_RXX_SPUCNT                  0xD5    // SPU control register
#define SPU_RXX_TRANS_CTRL              0xD6    // Transfer control
#define SPU_RXX_SPUSTAT                 0xD7    // SPU status register
// Audio Input Volume Registers (0xD8 - 0xDB)
#define SPU_RXX_CD_VOL_L                0xD8    // CD input volume left
#define SPU_RXX_CD_VOL_R                0xD9    // CD input volume right
#define SPU_RXX_EXT_VOL_L               0xDA    // External input volume left
#define SPU_RXX_EXT_VOL_R               0xDB    // External input volume right
// Current Volume Registers (0xDC - 0xDD)
#define SPU_RXX_CURR_MAIN_VOL_L         0xDC    // Current main volume left
#define SPU_RXX_CURR_MAIN_VOL_R         0xDD    // Current main volume right
// Unknown Register (0xDE - 0xDF)
#define SPU_RXX_UNKNOWN2_LOW            0xDE    // Unknown register (bits 0-15)
#define SPU_RXX_UNKNOWN2_HIGH           0xDF    // Unknown register (bits 16-31)
// Reverb Registers start at 0xE0
#define SPU_RXX_REVERB_BASE             0xE0    // Reverb parameter registers start

// SPU Register volume modes
#define SPU_VOL_MODE_DIRECT     0x0000
#define SPU_VOL_MODE_LINEARIncN 0x8000
#define SPU_VOL_MODE_LINEARIncR 0x9000
#define SPU_VOL_MODE_LINEARDecN 0xA000
#define SPU_VOL_MODE_LINEARDecR 0xB000
#define SPU_VOL_MODE_EXPIncN    0xC000
#define SPU_VOL_MODE_EXPIncR    0xD000
#define SPU_VOL_MODE_EXPDec     0xE000

#define SPU_VOL_MODE_MASK (1 << 15)
#define SPU_VOL_MAX 0x7F

// SPU Control Register (SPUCNT) bit masks
#define SPU_CTRL_MASK_CD_AUDIO_ENABLE        (1 <<  0)              // 0
#define SPU_CTRL_MASK_EXT_AUDIO_ENABLE       (1 <<  1)              // 1
#define SPU_CTRL_MASK_CD_AUDIO_REVERB        (1 <<  2)              // 2
#define SPU_CTRL_MASK_EXT_AUDIO_REVERB       (1 <<  3)              // 3
#define SPU_CTRL_MASK_SRAM_TRANSFER_MODE    ((1 <<  4) | (1 << 5))  // 4-5
#define SPU_CTRL_MASK_IRQ9_ENABLE            (1 <<  6)              // 6
#define SPU_CTRL_MASK_REVERB_MASTER_ENABLE   (1 <<  7)              // 7
#define SPU_CTRL_MASK_NOISE_FREQ_STEP       ((1 <<  8) | (1 << 9))  // 8-9
#define SPU_CTRL_MASK_NOISE_FREQ_SHIFT      ((1 << 10) | (1 << 11) | (1 << 12) | (1 << 13))  // 10-13
#define SPU_CTRL_MASK_MUTE_SPU               (1 << 14)              // 14
#define SPU_CTRL_MASK_SPU_ENABLE             (1 << 15)              // 15

// SPU Control Register shift amounts for multi-bit fields
#define SPU_CTRL_SRAM_TRANSFER_SHIFT     4
#define SPU_CTRL_NOISE_FREQ_STEP_SHIFT   8
#define SPU_CTRL_NOISE_FREQ_SHIFT_SHIFT 10

#define SPU_CTRL_TRANSFER_MODE_STOP         ( 0 << SPU_CTRL_SRAM_TRANSFER_SHIFT ) // 0x00
#define SPU_CTRL_TRANSFER_MODE_MANUAL_WRITE ( 1 << SPU_CTRL_SRAM_TRANSFER_SHIFT ) // 0x10
#define SPU_CTRL_TRANSFER_MODE_DMA_WRITE    ( 2 << SPU_CTRL_SRAM_TRANSFER_SHIFT ) // 0x20
#define SPU_CTRL_TRANSFER_MODE_DMA_READ     ( 3 << SPU_CTRL_SRAM_TRANSFER_SHIFT ) // 0x30

// SPU Status Register (SPUSTAT) bit masks
#define SPU_STAT_MASK_CURRENT_SPU_MODE      ((1 <<  0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 <<  4) | (1 <<  5))  // 0-5
#define SPU_STAT_MASK_IRQ9_FLAG              (1 <<  6)              // 6
#define SPU_STAT_MASK_DMA_READ_WRITE_REQUEST (1 <<  7)              // 7
#define SPU_STAT_MASK_DMA_WRITE_REQUEST      (1 <<  8)              // 8
#define SPU_STAT_MASK_DMA_READ_REQUEST       (1 <<  9)              // 9
#define SPU_STAT_MASK_DATA_TRANSFER_BUSY     (1 << 10)              // 10
#define SPU_STAT_MASK_CAPTURE_BUFFER_HALF    (1 << 11)              // 11
#define SPU_STAT_MASK_UNKNOWN_UNUSED        ((1 << 12) | (1 << 13) | (1 << 14) | (1 << 15))  // 12-15

// SPU Status Register shift amounts for multi-bit fields
#define SPU_STAT_CURRENT_SPU_MODE_SHIFT      0
#define SPU_STAT_UNKNOWN_UNUSED_SHIFT        12

// SPU Status Register values
#define SPU_STAT_CAPTURE_FIRSTHALF           (0 << 11)  // Writing to first half
#define SPU_STAT_CAPTURE_SECONDHALF          (1 << 11)  // Writing to second half
#define SPU_STAT_TRANSFER_READY              (0 << 10)  // Transfer ready
#define SPU_STAT_TRANSFER_BUSY               (1 << 10)  // Transfer busy

typedef union {
    SPU_RXX _rxx;
    volatile SPU_RXX rxx;
    u16 _raw[0x100];
    volatile u16 raw[0x100];
} SpuUnion;

extern SpuUnion* g_pSoundSpuRegisters;

#ifdef XENO_PC_PORT
/* Retail g_pSoundSpuRegisters is an .sdata pointer to the memory-mapped SPU
 * register block at 0x1F801C00. The port has no mapped SPU hardware and the
 * auto-stub left the pointer NULL -- safe only while the tick body was
 * stubbed. Back it with a static block so the real tick's direct register
 * writes (func_8003E900/EB5C key on/off, ADSR, pitch) land in real memory.
 * Register state is faithfully maintained but not yet wired to the OpenAL
 * backend (that wiring is the WDS/B5 leg). */
static SpuUnion s_SpuRegisterBackingPage;
SpuUnion* g_pSoundSpuRegisters = &s_SpuRegisterBackingPage;
#endif

/* Note/pitch tables (sdata). D_80050B78: per-octave table (low nibble = pitch
 * table page, high nibble = octave shift). D_80050BF0: per-note SPU pitch. */
extern u8 D_80050B78[];
extern s16 D_80050BF0[];
/* Note-on velocity/articulation tables (sdata), indexed by the note byte. */
extern u8 D_80050A94[];
extern u8 D_800509B0[];
/* Script-opcode operand lengths (sdata), indexed by opcode-0x80. */
extern u8 D_80050824[];
/* Sequence-command dispatch table (sdata), indexed by opcode-0x80. Retail
 * data holds 128 handler addresses; the port's real handler routing is the
 * step-3 pass (unreached until sequence data loads -- no active elements). */
extern u8* (*g_SoundScriptHandlers[])(u8* pScript, AudioManager* pAudioManager,
                                      AudioElement* pAudioElements);
/* Tick bookkeeping: SPU-IRQ re-enable request flag, cumulative tick duration
 * in RCnt2 ticks, and tick count (the 240Hz profiler pair). */
extern u16 D_8005955C;
extern s32 D_800595C4;
extern s32 D_80059540;
/* Per-voice envelope object: 4 per element at element+0xD8, 0x20 bytes each.
 * The first word is the envelope HANDLER METHOD POINTER (retail: function
 * address; kept at ABI width as a SoundPsxAddress on LP64 -- this is the
 * runtime-trace-required jalr set from the tick-leg scoping; its setters are
 * script handlers (step 3), so in the port the active flag stays clear and
 * the call site is not reached yet). */
typedef struct {
    /* 0x00 */ SoundPsxAddress pfnHandler;
    /* 0x04 */ u8 unk4[0x10];
    /* 0x14 */ u16 delay;
    /* 0x16 */ u8 unk16[2];
    /* 0x18 */ s16 step;
    /* 0x1A */ u16 stepAdd;
    /* 0x1C */ u8 state;
    /* 0x1D */ u8 unk1D;
    /* 0x1E */ u16 flags;
} SoundEnvelope;

/* Handler-subtree helpers (unported; port auto-stubs). */
extern void func_8003A14C();
extern void func_80039F18();
extern void func_8003E3E0();
extern s32 func_8003E290();
/* Envelope handler-method table (sdata): retail PSX addresses consumed by
 * func_8003E180; the EFE4 jalr host routing lands with the envelope pass. */
extern u32 D_800508A4[];
//----------------------------------------------------------------------------------------------------------------------

void SoundInitialize(s32 arg0) {
    if (g_SoundControlFlags < 0) {
        SoundHandleError(0x28);
        return;
    }
    g_SoundControlFlags = arg0 | 0xB801;
    SpuInitMalloc(4, (char*)g_SoundSpuMemoryTableStart);
    SoundHeapInitialize(D_80065B0C, 0x6300);
    SoundSpuMemoryInitialize();
    g_SoundTransferQueue = SoundHeapAllocate(0xA0);
    SoundClearVoiceDataPointers();
    D_800594E4 = 0x12345678;
    g_SoundAudioManagerListHead = NULL;
    D_800595D8 = 0;
    g_SoundSedsLinkedList = NULL;
    g_SoundWdsLinkedList = NULL;
    D_80059518 = NULL;
    g_SoundKeyOnFlags = 0;
    g_SoundKeyOffFlags = 0;
    g_unk_VoicesNeedingProcessing = 0;
    g_SoundVolumeController.commonAttr.mvolmode.left = 0;
    g_SoundVolumeController.commonAttr.mvolmode.right = 0;
    g_SoundVolumeController.commonAttr.mask = 0xC;
    EnterCriticalSection();
    g_unk_SoundEvent = OpenEvent(0xF2000002, 2, 0x1000, func_8003C020);
    SetRCnt(0xF2000002, 0x44E8, 0x1000);
    StartRCnt(0xF2000002);
    SpuSetTransferCallback(SoundOnTransferCallback);
    SpuSetIRQCallback(SoundSpuIRQHandler);
    SpuSetIRQ(0);
    D_80059504 = 0;
    g_SoundSpuIRQCount = 0;
    ExitCriticalSection();
    SoundSpuMemoryAllocateBlockAtAddress(0x2000, 0x10000, 4);
    func_800386C4(1);
    SoundSetCdAttr(0, 1);
    SoundSetMasterVolumeWithFade(0x3FFF, 0);
    SoundSetCdVolumeWithFade(0x7FFF, 0);
    if (g_SoundControlFlags & 0x4000) {
        SoundSetupCdMix(0x80);
    }
    D_800595D8 = SOUND_PTR_TO_PSX(func_8003B148(0x10));
    D_80059544 = 8;
    g_SoundReverbMemoryHandle = -1;
    g_SoundUploadDestBuffer = 0;
    g_SoundReverbType = 0xFF;
    SoundSetReverbModeWithAllocation(4, 0, 0, 0);
    SpuSetReverb(1);
    g_SoundSpuErrorId = 0;
}

void SoundReset(void) {
    int i;

    if (g_SoundControlFlags == 0x0) {
        SoundHandleError(0x29);
        return;
    }
    
    EnterCriticalSection();
    g_SoundControlFlags = 0;
    SpuSetIRQ(SPU_OFF);
    SpuSetTransferCallback(NULL);
    SpuSetIRQCallback(NULL);
    StopRCnt(RCntCNT2);
    CloseEvent(g_unk_SoundEvent);
    ExitCriticalSection();
    for (i = 0; i < NUM_VOICES; i++) {
        SoundSetVoiceAdsrReleaseShiftAndMode(i, 6, 3);
    }
    SoundSetVoiceKeyOff(0xFFFFFF); // Release all voices
    SpuSetReverbModeDepth(0, 0);
    SpuSetReverbModeType(SPU_REV_MODE_OFF);
    g_SoundSpuErrorId = 0;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", SoundEnableAllSpuChannels);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", SoundMuteAllSpuChannels);

void func_80037F44(void) {
    if (!(g_SoundControlFlags & 1)) {
        g_SoundControlFlags |= 1;
        EnableEvent(g_unk_SoundEvent);
    }
}

void func_80037F88(void) {
    if (g_SoundControlFlags & 1) {
        DisableEvent(g_unk_SoundEvent);
        g_SoundControlFlags &= ~1;
    }
}



INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", SoundLoadWdsFile);

// Loads part of a WDS file, basically a sized SoundLoadWdsFile?
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_800380D0);

void SoundSpuMemoryAllocateWDS(SoundWDSEntry* pWdsFile, int mode) {
    if (mode == SOUND_WDS_ALLOCATE_AT_ADDRESS) {
        mode = pWdsFile->spuMemoryAddress;
    } else if (mode == SOUND_WDS_ALLOCATE_AUTOMATIC) {
        mode = 0;
    }
    
    if (mode == 0) {
        SoundSpuMemoryAllocateBlock(pWdsFile->adpcmDataSize, pWdsFile->unk1E);
        return;
    }
    
    SoundSpuMemoryAllocateBlockAtAddress(pWdsFile->adpcmDataSize, pWdsFile->spuMemoryAddress, pWdsFile->unk1E);
}

void SoundWdsSetTransferParamters(int transferAddress, int numBytesToTransfer) {
    g_SoundWdsCurSpuAddress = transferAddress;
    g_SoundWdsRemainingBytes = numBytesToTransfer;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", SoundTransferWdsPart);

void SoundFreeWdsEntry(SoundWDSEntry* pTargetEntry) {
    SoundWDSEntry* pPrev;
    SoundWDSEntry* pCurrent;

    pPrev = NULL;
    for (pCurrent = g_SoundWdsLinkedList; pCurrent != NULL;
         pCurrent = SOUND_PSX_TO_PTR(SoundWDSEntry, pCurrent->pNext)) {
        if (pCurrent == pTargetEntry) 
            break;
        
        pPrev = pCurrent;
    }

    if (pCurrent == NULL) {
        SoundHandleError(0x11U);
        return;
    }

    DisableEvent(g_unk_SoundEvent);
    if (pPrev != NULL)
        pPrev->pNext = SOUND_PTR_TO_PSX(
            SOUND_PSX_TO_PTR(SoundWDSEntry, pTargetEntry->pNext));
    else 
        g_SoundWdsLinkedList = SOUND_PSX_TO_PTR(SoundWDSEntry, pTargetEntry->pNext);
    EnableEvent(g_unk_SoundEvent);
    
    if (pTargetEntry->spuMemoryAddress != SoundSpuMemoryFreeBlock(pTargetEntry->spuMemoryAddress)) {
        SoundHandleError(0x24U);
    }
    
    SoundHeapFree(pTargetEntry);
}

SoundWDSEntry* SoundFindWdsEntry(int targetID) {
    SoundWDSEntry* pCurrent;

    for (pCurrent = g_SoundWdsLinkedList; pCurrent != NULL;
         pCurrent = SOUND_PSX_TO_PTR(SoundWDSEntry, pCurrent->pNext)) {
        if (pCurrent->id == targetID) {
            return pCurrent;
        }
    }
    return pCurrent;
}


void SoundAddSedsEntry(SoundFile* pSoundFile) {
    SoundFile* pEntry;
    short nSedsStatus;
    SoundPsxAddress* pList;
    SoundFile* pSoundFileToVerify = pSoundFile;

    // Ensure that an entry with the same SED ID does not exists in the linked list already
    if (!(g_SoundControlFlags & 0x80)) {
        for (pEntry = g_SoundSedsLinkedList; pEntry != NULL;
             pEntry = SOUND_PSX_TO_PTR(SoundFile, pEntry->pNext)) {
            if (pSoundFile->sedId == pEntry->sedId) {
                SoundHandleError(SOUND_ERR_ENTRY_ALREADY_EXISTS);
                return;
            }
        }
    }

    // Validate SEDS File
    nSedsStatus = SoundValidateFile(pSoundFileToVerify, FILE_SIGNATURE('s','e','d','s'), 0x101);
    if (nSedsStatus != SOUND_STATUS_OK) {
        SoundHandleError(nSedsStatus);
        return;
    }

    // Add the SED Entry to the linked list
    DisableEvent(g_unk_SoundEvent);
    pList = (SoundPsxAddress*)&g_SoundSedsLinkedList;
    while (SOUND_PSX_TO_PTR(SoundFile, *pList) != NULL)
        pList = &SOUND_PSX_TO_PTR(SoundFile, *pList)->pNext;
    *pList = SOUND_PTR_TO_PSX(pSoundFile);
    pSoundFile->pNext = SOUND_PTR_TO_PSX(NULL);
    EnableEvent(g_unk_SoundEvent);
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003852C);

//----------------------------------------------------------------------------------------------------------------------
void func_80038624(void) {
    func_80039FF8();
    g_SoundSedsLinkedList = 0;
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003864C);

//----------------------------------------------------------------------------------------------------------------------
void func_8003869C(void) {
    func_80039CC4();
    func_80039FF8();
}

//----------------------------------------------------------------------------------------------------------------------
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; the matching build
 * keeps INCLUDE_ASM (byte-exact) below. Semantics traced 1:1 against the asm
 * (reverb-mode flag machine: clear control bits 8-10, then OR in 0x100/0x300/
 * 0x500 for arg 1/2/3; re-apply volume; flag active voices; optional CD mix;
 * the D_80059518 echo-controller leg is dead at cold init -- that global is
 * only ever set to 0). Residual vs {}: gcc-2.7.2 lowers the mode switch to a
 * range-split decision tree (slti a0,3) whereas retail emits a linear
 * beq-to-body chain with a tail-merged store; neither a switch, a flat nor a
 * nested if-else, nor a shared-store switch reproduces retail's expand_case
 * shape. Not claimed as {}. */
void func_800386C4(s32 arg0) {
    AudioManager* manager;
    u8* p;
    s32 value;
    s16 flags;

    flags = g_SoundControlFlags & 0xF8FF;
    g_SoundControlFlags = flags;
    switch (arg0) {
    case 1:
        g_SoundControlFlags = flags | 0x100;
        break;
    case 2:
        g_SoundControlFlags = flags | 0x300;
        break;
    case 3:
        g_SoundControlFlags = flags | 0x500;
        break;
    }
    SoundApplyVolumeSettings();
    SpuSetReverbModeDepth(g_SoundReverbDepth.left, g_SoundReverbDepth.right);
    manager = g_SoundAudioManagerListHead;
    if (manager != NULL) {
        do {
            unk_SoundSetFlagsOnActiveVoices(0x100, manager);
            manager = SOUND_PSX_TO_PTR(AudioManager, manager->next);
        } while (manager != NULL);
    }
    if (g_SoundControlFlags & 0x4000) {
        SoundSetupCdMix(g_SoundVolumeController.unk_field2);
    }
    p = (u8*)D_80059518;
    if (p != NULL && (*(u16*)(p + 0x0) & 1)) {
        value = *(u16*)(p + 0x12);
        if (func_80038824() != 0) {
            value <<= 7;
            *(u16*)(p + 0x38) = value;
            *(u16*)(p + 0x3A) = 0;
            *(u16*)(p + 0x64) = 0;
        } else {
            value <<= 6;
            *(u16*)(p + 0x38) = value;
            *(u16*)(p + 0x3A) = value;
            *(u16*)(p + 0x64) = value;
        }
        *(u16*)(p + 0x66) = value;
        *(u16*)(p + 0x36) = 1;
        *(u16*)(p + 0x62) = 1;
    }
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_800386C4);
#endif

//----------------------------------------------------------------------------------------------------------------------
s32 func_80038824(void) {
    s32 out;

    if (g_SoundControlFlags & ((1 << 8) | (1 << 9) | (1 << 10))) {
        out = 1;
        if (g_SoundControlFlags & ((1 << 9) | (1 << 10))) {
            out = 2;
        }
    } else {
        out = 0;
    }

    return out;
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", SoundSetupCdMix);

//----------------------------------------------------------------------------------------------------------------------
void func_800388D4(s32 arg0) {

    if (arg0 != 0) {
        g_SoundControlFlags |= (1 << 12);
    } else {
        g_SoundControlFlags &= ~(1 << 12);
    }
}

//----------------------------------------------------------------------------------------------------------------------
void func_8003890C(AudioManager* manager, s32 bIn) {
    if (bIn != 0) {
        manager->unk_Flags &= ~(1 << 0);
    } else {
        manager->unk_Flags |= (1 << 0);
    }
}

//----------------------------------------------------------------------------------------------------------------------
void SoundSetReverbModeWithAllocation(s32 reverbType, s32 reverbDepth, s32 reverbDelay, s32 reverbFeedback) {
    SpuReverbAttr reverbAttr;
    s32 currentReverbType;
    s32 workAreaSize;
    s32 workAreaStartAddr;
    s32 allocationSize;
    s32 memoryHandle;
    b32 bAllocated;

    bAllocated = false;

    if (reverbType == -2) {
        return;
    }

    if (reverbType == SPU_REV_MODE_OFF) {
        reverbFeedback = 0;
        reverbDelay = 0;
        reverbDepth = 0;
    } else if (reverbType == SPU_REV_MODE_CHECK) {
        reverbType = g_SoundReverbType;
    }

    SpuGetReverbModeType(&currentReverbType);
    if (currentReverbType != reverbType || reverbType == SPU_REV_MODE_OFF) {

        if (g_SoundReverbMemoryHandle != -1) {
            SoundSpuMemoryFreeBlock(g_SoundReverbMemoryHandle);
        }

        workAreaSize = g_ReverbWorkAreaSizes[reverbType];
        workAreaStartAddr = workAreaSize;
        allocationSize = 0x80000 - workAreaSize;

        memoryHandle = SoundSpuMemoryAllocateBlockAtAddress(workAreaStartAddr, allocationSize, 5);
        g_SoundReverbMemoryHandle = memoryHandle;

        if (memoryHandle == 0) {
            SoundHandleError(0x20);
            reverbType = 0;
            reverbFeedback = 0;
            reverbDelay = 0;
            reverbDepth = 0;
        }
        bAllocated = true;

    }

    g_SoundReverbType = reverbType;
    g_SoundVolumeController.currentReverbDepth = reverbDepth;
    g_SoundReverbDelay = reverbDelay;
    g_SoundReverbFeedback = reverbFeedback;

    SoundApplyVolumeSettings();

    if (bAllocated) {
        // New reverb type - initialize with zero depth, set type, clear work area (probably)
        SpuSetReverbModeDepth(0, 0);
        SpuSetReverbModeType(reverbType);
        SoundInitiateReverbWorkAreaTransfer(allocationSize, workAreaStartAddr);
    } else {
        // Existing reverb type - apply current settings
        SpuSetReverbModeDepth(g_SoundReverbDepth.left, g_SoundReverbDepth.right);
        SpuSetReverbModeDelayTime(reverbDelay);
        SpuSetReverbModeFeedback(reverbFeedback);
    }
}

//----------------------------------------------------------------------------------------------------------------------
void SoundInitiateReverbWorkAreaTransfer(s32 addr, s32 size) {
    s32 temp_v0;

    g_SoundUploadSourceAddr = addr;
    g_SoundUploadBytesRemaining = size;
    if (g_SoundUploadDestBuffer == 0) {
        temp_v0 = SoundHeapAllocate(0x840);
        g_SoundUploadDestBuffer = temp_v0;
        if (temp_v0 == 0) {
            SoundHandleError(0x1E);
        }
    }
    g_SoundControlFlags |= (1 << 5);
    SoundExecuteReverbWorkAreaTransfer();
}

//----------------------------------------------------------------------------------------------------------------------
void SoundExecuteReverbWorkAreaTransfer(void) {
    s32 bytesRemaining;
    s32 chunkSize;
    s32 currentSourceAddr;

    bytesRemaining = g_SoundUploadBytesRemaining;

    if (bytesRemaining == 0) {
        SoundHeapFree(g_SoundUploadDestBuffer);
        g_SoundUploadDestBuffer = 0;

        SpuSetReverbModeDepth(g_SoundReverbDepth.left, g_SoundReverbDepth.right);
        SpuSetReverbModeDelayTime(g_SoundReverbDelay);
        SpuSetReverbModeFeedback(g_SoundReverbFeedback);

        g_SoundControlFlags &= ~(1 << 5);

    } else {
        chunkSize = (bytesRemaining <= 0x840) ? bytesRemaining : 0x800;

        currentSourceAddr = g_SoundUploadSourceAddr;

        g_SoundUploadBytesRemaining = bytesRemaining - chunkSize;
        g_SoundUploadSourceAddr = currentSourceAddr + chunkSize;

        SoundQueueSpuWriteCommand(currentSourceAddr, g_SoundUploadDestBuffer, chunkSize, SoundExecuteReverbWorkAreaTransfer);

        if ((g_SoundControlFlags & (1 << 4)) == 0) {
            SoundQueueSpuWriteCommand(currentSourceAddr, g_SoundUploadDestBuffer, chunkSize, NULL);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
void SoundSetMasterVolumeWithFade(s32 volume, s32 frames)
{
    g_SoundVolumeController.masterInterpolator.targetValue = (s16)volume;
    if (frames == 0) {
        g_SoundVolumeController.masterInterpolator.currentValue = volume << 0x10;
        g_SoundVolumeController.masterInterpolator.counter = 0;
        g_SoundVolumeController.currentMasterVolume = g_SoundVolumeController.masterInterpolator.targetValue;
        SoundSetVolumeWithPhase(g_SoundVolumeController.masterInterpolator.targetValue, &g_SoundVolumeController.commonAttr.mvol, 0);
        g_SoundVolumeController.commonAttr.mask |= SPU_COMMON_MVOLL | SPU_COMMON_MVOLR;
    } else {
        s32 volumeDifference = (volume << 8) - (g_SoundVolumeController.masterInterpolator.currentValue >> 8);
        if (volumeDifference != 0) {
            g_SoundVolumeController.masterInterpolator.stepIncrement = volumeDifference / frames << 8;
            g_SoundVolumeController.masterInterpolator.counter = (s16)frames;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
void SoundSetCdVolumeWithFade(s32 targetVolume, s32 fadeFrames) {
    s16 volumeValue;

    g_SoundVolumeController.cdInterpolator.targetValue = targetVolume;

    if (fadeFrames == 0) {
        g_SoundVolumeController.cdInterpolator.currentValue = targetVolume << 16;
        volumeValue = targetVolume;
        g_SoundVolumeController.cdInterpolator.counter = 0;
        g_SoundVolumeController.currentCdVolume = volumeValue;
        g_SoundVolumeController.commonAttr.cd.volume.right = volumeValue;
        g_SoundVolumeController.commonAttr.cd.volume.left = volumeValue;
        g_SoundVolumeController.commonAttr.mask |= SPU_COMMON_CDVOLL | SPU_COMMON_CDVOLR;

    } else {
        s32 currentVolume = g_SoundVolumeController.cdInterpolator.currentValue >> 8;
        s32 volumeDifference = (targetVolume << 8) - currentVolume;

        if (volumeDifference != 0) {
            s32 stepPerFrame = volumeDifference / fadeFrames;
            g_SoundVolumeController.cdInterpolator.counter = fadeFrames;
            g_SoundVolumeController.cdInterpolator.stepIncrement = stepPerFrame << 8;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
void SoundSetCdAttr(long reverb, long mix) {
    g_SoundVolumeController.commonAttr.cd.reverb = reverb;
    g_SoundVolumeController.commonAttr.cd.mix = mix;
    g_SoundVolumeController.commonAttr.mask = g_SoundVolumeController.commonAttr.mask | SPU_COMMON_CDREV | SPU_COMMON_CDMIX;
    SpuSetCommonAttr(&g_SoundVolumeController.commonAttr);
}

//----------------------------------------------------------------------------------------------------------------------
void SoundApplyVolumeSettings(void) {
    SoundSetVolumeWithPhase(g_SoundVolumeController.currentMasterVolume, &g_SoundVolumeController.commonAttr.mvol, 0);
    g_SoundVolumeController.commonAttr.cd.volume.right = g_SoundVolumeController.currentCdVolume;
    g_SoundVolumeController.commonAttr.cd.volume.left = g_SoundVolumeController.currentCdVolume;;
    SoundSetVolumeWithPhase(g_SoundVolumeController.currentReverbDepth, &g_SoundReverbDepth, 1);
    g_SoundVolumeController.commonAttr.mask |= SPU_COMMON_MVOLL | SPU_COMMON_MVOLR | SPU_COMMON_CDVOLL | SPU_COMMON_CDVOLR;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundSetVolumeWithPhase(s32 volume, SpuVolume* pVolume, s32 channelSelect)
{
    pVolume->right = volume;
    pVolume->left = volume;

    if ((g_SoundControlFlags & ((1 << 9) | (1 << 10))) != 0) {
        channelSelect &= 0xFF;
        if (!(g_SoundControlFlags & (1 << 9))) {
            if ((channelSelect ^ 1) != 0) {
                pVolume->left = -volume;
            } else {
                pVolume->right = -volume;
            }
        } else {
            if (channelSelect != CHANNEL_RIGHT) {
                pVolume->left = -volume;
            } else {
                pVolume->right = -volume;
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
void SoundHeapInitialize(void* startAddress, unsigned int size) {
    SoundHeapBlockHeader* pHeapBlock;
    unsigned int nAlignedSize;

    // Align start address and size
    pHeapBlock = (SoundHeapBlockHeader*) startAddress;
    nAlignedSize = size & ~0xF;
    if ((u32)pHeapBlock & 0xF) {
        nAlignedSize -= 0x10;
        pHeapBlock = ((u32)pHeapBlock + 0xF) & ~0xF;
    }
    
    g_SoundHeapEnd = (u32)pHeapBlock + nAlignedSize;
    pHeapBlock->unk0 = 0x8000;
    g_SoundHeapHead = pHeapBlock;
    g_SoundHeapSize = nAlignedSize;
    pHeapBlock->unk2 = 0;
    pHeapBlock->unk4 = 0;
    pHeapBlock->pPrev = SOUND_PTR_TO_PSX(pHeapBlock + 1);
    pHeapBlock->pNext = SOUND_PTR_TO_PSX(NULL);
}

void* SoundHeapAllocate(u32 allocSize) {
    void* pMemory;
    SoundHeapBlockHeader* pNewBlock;
    unsigned int nTotalSize;
    unsigned int nSize;
    SoundHeapBlockHeader* pNext;
    SoundHeapBlockHeader* pHeapBlock;

    DisableEvent(g_unk_SoundEvent);
    nTotalSize = ((allocSize + 0xF) & ~0xF) + sizeof(SoundHeapBlockHeader);
    
    for (pHeapBlock = g_SoundHeapHead;
         SOUND_PSX_TO_PTR(SoundHeapBlockHeader, pHeapBlock->pNext) != NULL;
         pHeapBlock = SOUND_PSX_TO_PTR(SoundHeapBlockHeader, pHeapBlock->pNext)) {
        pNext = SOUND_PSX_TO_PTR(SoundHeapBlockHeader, pHeapBlock->pNext);
        nSize = (u32)SOUND_PSX_TO_PTR(SoundHeapBlockHeader, pHeapBlock->pNext) -
                (u32)SOUND_PSX_TO_PTR(SoundHeapBlockHeader, pHeapBlock->pPrev);
        if (nSize >= nTotalSize) {
            goto alloc_new_block;
        }
    }
    
    pNext = (SoundHeapBlockHeader*)g_SoundHeapEnd;
    if ((u32)pNext - (u32)SOUND_PSX_TO_PTR(SoundHeapBlockHeader, pHeapBlock->pPrev) >= nTotalSize) {
    alloc_new_block:
        pNewBlock = (SoundHeapBlockHeader*)
            (((u32)SOUND_PSX_TO_PTR(SoundHeapBlockHeader, pHeapBlock->pPrev) + 0xF) & ~0xF);
        pMemory = pNewBlock + 1;
        pNewBlock->pPrev = SOUND_PTR_TO_PSX((u8*)pMemory + allocSize);
        pNewBlock->pNext = SOUND_PTR_TO_PSX(NULL);
        pNewBlock->unk4 = 0;
        pNewBlock->unk0 = 2;
        pNewBlock->unk2 = 0;
        pNewBlock->pNext = pHeapBlock->pNext;
        pHeapBlock->pNext = SOUND_PTR_TO_PSX(pNewBlock);
        EnableEvent(g_unk_SoundEvent);
        SoundHeapClearBlockMemory(pMemory, allocSize);
        return pMemory;
    }
    
    return NULL;
}

// SoundHeapAllocate, but slightly different
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_80039024);

void SoundHeapFree(void* pMemory) {
    SoundHeapBlockHeader* pBlock;
    SoundHeapBlockHeader* pPrevious;
    SoundHeapBlockHeader* pTarget;

    pTarget = (SoundHeapBlockHeader*)((u8*)pMemory - sizeof(SoundHeapBlockHeader));
    pBlock = g_SoundHeapHead;
    pPrevious = NULL;

    DisableEvent(g_unk_SoundEvent);
    while (pBlock != pTarget) {
        pPrevious = pBlock;
        pBlock = SOUND_PSX_TO_PTR(SoundHeapBlockHeader, pPrevious->pNext);
    }

    if (pPrevious != NULL) {
        pPrevious->pNext = pTarget->pNext;
    }
    EnableEvent(g_unk_SoundEvent);
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_800391CC);

void SoundHeapSetBlockMemory(void* pBlockMemory, void* pSrc, int size) {
    u32* pCurSrc;
    u32* pCurDst;
    u8* pCurSrcByte;
    u8* pCurDstByte;
    u32* pNextSrc;
    u32* pNextDst;
    u32 v0,v1,v2,v3;

    unsigned int nCount;
    pCurDst = pBlockMemory;
    pCurSrc = pSrc;
    
    for (nCount = size >> 4; nCount != 0; nCount--) {
        v0 = pCurSrc[0];
        v1 = pCurSrc[1];
        v2 = pCurSrc[2];
        v3 = pCurSrc[3];
        
        pCurDst[0] = v0;
        pCurDst[1] = v1;
        pCurDst[2] = v2;
        pCurDst[3] = v3;

        pCurSrc += 4;
        pCurDst += 4;
    }

    for (nCount = (size >> 2) & 3; nCount != 0; nCount--) {
        *pCurDst++ = *pCurSrc++;
    }

    pCurSrcByte = (u8*)pCurSrc;
    pCurDstByte = (u8*)pCurDst;
    for (nCount = size & 3; nCount != 0; nCount--) {
        *pCurDstByte++ = *pCurSrcByte++;
    }
}

void SoundHeapClearBlockMemory(void* pMemory, s32 size) {
    unsigned int nCount;
    u32* pDword;
    u8* pByte;

    pDword = pMemory;

    for (nCount = size >> 4; nCount != 0; nCount--) {
        pDword[3] = 0;
        pDword[2] = 0;
        pDword[1] = 0;
        pDword[0] = 0;
        pDword += 4;
    }
    
    for (nCount = (size >> 2) & 3; nCount != 0; nCount--) {
        *pDword++ = 0;
    }
    
    pByte = (u8*) pDword;
    for (nCount = size & 3; nCount != 0; nCount--) {
        *pByte++ = 0;
    }
}

void SoundSpuMemoryInitialize(void) {
    int i;
    
    for (i = MAX_SPU_MEMORY_BLOCKS - 1; i >= 0; i--) {
        g_SoundSpuMemoryBlocks[i].flags = SPU_MEMORY_FREE;
    }
    
    g_SoundSpuMemoryBlocks[0].flags = SPU_MEMORY_RESERVED | SPU_MEMORY_IN_USE;
    g_SoundSpuMemoryBlocks[0].unk1 = 5;
    g_SoundSpuMemoryBlocks[0].spuAddress = 0;
    g_SoundSpuMemoryBlocks[0].size = 0x1010;
    g_SoundSpuMemoryBlocks[0].nextBlockIndex = 0;
}


INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", SoundSpuMemoryAllocateBlock);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_800394B8);

#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; the matching build
 * keeps INCLUDE_ASM (byte-exact) below. Semantics traced 1:1 against the asm
 * (block-search over g_SoundSpuMemoryBlocks for a free gap containing
 * [addr, addr+size); insert a new in-use block). Residual vs {}: gcc-2.7.2
 * colors `addr` into s0 and the walking pointer into s1 (retail is the reverse)
 * and reloads the array base instead of caching it in t0 -- a register-coloring
 * inversion driven by `addr` being live across the SoundSpuMemoryGetFreeBlock
 * call; the instruction stream is otherwise identical. Not claimed as {}. */
u32 SoundSpuMemoryAllocateBlockAtAddress(s32 size, s32 addr, s32 arg2) {
    SoundSpuMemoryBlock* pBlock;
    SoundSpuMemoryBlock* pNext;
    s32 freeIdx;
    s32 gap;
    s32 regionEnd;
    s32 blockEnd;
    s32 nextIdx;

    pBlock = g_SoundSpuMemoryBlocks;
    gap = 0;
    regionEnd = addr + size;
    blockEnd = pBlock->spuAddress + pBlock->size;

    if (pBlock->spuAddress < addr) {
        for (;;) {
            nextIdx = pBlock->nextBlockIndex;
            if (nextIdx == 0) {
                gap = 0x80000 - blockEnd;
                break;
            }
            pNext = &g_SoundSpuMemoryBlocks[nextIdx];
            if (pNext->spuAddress >= regionEnd) {
                gap = pNext->spuAddress - blockEnd;
                break;
            }
            blockEnd = pNext->spuAddress + pNext->size;
            if (pNext->spuAddress >= addr) {
                break;
            }
            pBlock = pNext;
        }
    }

    if (gap < size) {
        return 0;
    }
    if (addr < blockEnd) {
        return 0;
    }
    freeIdx = SoundSpuMemoryGetFreeBlock();
    if (freeIdx < 0) {
        return 0;
    }
    pNext = &g_SoundSpuMemoryBlocks[freeIdx];
    pNext->flags = 0x80;
    pNext->unk1 = 0;
    pNext->spuAddress = addr;
    pNext->size = size;
    pNext->nextBlockIndex = pBlock->nextBlockIndex;
    pBlock->nextBlockIndex = freeIdx;
    return addr;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", SoundSpuMemoryAllocateBlockAtAddress);
#endif

int SoundSpuMemoryFreeBlock(int targetAddress) {
    SoundSpuMemoryBlock* pCurBlock;
    SoundSpuMemoryBlock* pPrevBlock;
    short nNextIndex;

    pCurBlock = g_SoundSpuMemoryBlocks;
    pPrevBlock = NULL;

    while (1) {
        if (pCurBlock->spuAddress == targetAddress) {
            pPrevBlock->nextBlockIndex = pCurBlock->nextBlockIndex;
            pCurBlock->flags = SPU_MEMORY_FREE;
            pCurBlock->unk1 = 0;
            pCurBlock->spuAddress = 0x0;
            pCurBlock->nextBlockIndex = 0;
            return targetAddress;
        }
        
        nNextIndex = pCurBlock->nextBlockIndex;
        pPrevBlock = pCurBlock;
        
        if (nNextIndex != 0) {
            pCurBlock = &g_SoundSpuMemoryBlocks[nNextIndex];
            continue;
        }
        
        return NULL;
    }
}

void func_80039748(int targetAddress, u_char value) {
    SoundSpuMemoryBlock *pBlock;

    pBlock = SoundSpuMemoryFindBlock(targetAddress);
    if (pBlock != NULL) {
        pBlock->unk1 = value;
    }
}

s32 func_8003977C(void) { return 0; }

int SoundSpuMemoryGetFreeBlock() {
    int i = 0;

    while (i < MAX_SPU_MEMORY_BLOCKS) {
        if (g_SoundSpuMemoryBlocks[i].flags == SPU_MEMORY_FREE)
            return i;
        i++;
    }

    return 0;
}

// Possibly misleading name
SoundSpuMemoryBlock* SoundSpuMemoryFindBlock(s32 targetAddress) {
    SoundSpuMemoryBlock* pCurrent;
    SoundSpuMemoryBlock* pRes;
    
    pCurrent = g_SoundSpuMemoryBlocks;
    pRes = g_SoundSpuMemoryBlocks;
    
    while (1) {
        if (pCurrent->spuAddress != targetAddress) {
            if (pCurrent->nextBlockIndex != 0) {
                return NULL;
            }
            pCurrent = pRes;
            continue;
        }
        return pCurrent;
    }
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_800397FC);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_80039850);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_80039910);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_800399D4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_80039A80);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_80039B68);

//----------------------------------------------------------------------------------------------------------------------
void func_80039C4C(AudioManager* manager) {
    if (manager == NULL) {
        SoundHandleError(5);
        return;
    }
    manager->unk_Flags &= ~(1 << 15);
    SoundReleaseAllVoices();
}

//----------------------------------------------------------------------------------------------------------------------
void func_80039C8C(AudioManager* manager, s32 arg1) {
    if (manager == NULL) {
        SoundHandleError(5);
        return;
    }
    func_8003A89C(manager, 0, arg1);
}

//----------------------------------------------------------------------------------------------------------------------
void func_80039CC4(void) {
    AudioManager* pManager;

    pManager = g_SoundAudioManagerListHead;
    if (pManager != NULL) {
        do {
            if (pManager->unk_Flags & 1) {
                pManager->unk_Flags &= ~(1 << 15);
                SoundReleaseAllVoices(pManager);
            }
            pManager = SOUND_PSX_TO_PTR(AudioManager, pManager->next);
        } while (pManager != NULL);
    }
}

//----------------------------------------------------------------------------------------------------------------------
void func_80039D24(void) {}

//----------------------------------------------------------------------------------------------------------------------
void func_80039D2C(s32 bIn) {
    if (bIn != 0) {
        g_SoundControlFlags |= (1 << 11);
    } else {
        func_80039FF8();
        g_SoundControlFlags &= ~(1 << 11);
    }
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_80039D78);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_80039DB8);

//----------------------------------------------------------------------------------------------------------------------
// Wowoweewa flag central
void func_80039E18(s32 arg0) {
    if (g_SoundControlFlags & (1 << 11)) {
        D_80059404 = 2;
        func_8003B644((1 << 2) | (1 << 3) | (1 << 13) | (1 << 14), arg0, (1 << 13) | ( 1 << 14), (1 << 14));
    }
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_80039E60);

//----------------------------------------------------------------------------------------------------------------------
void func_80039EC4(s32 arg0, s32 arg1) {
    if (g_SoundControlFlags & (1 << 11)) {
        D_80059404 = 2;
        func_8003B644(
            ((arg1 & 0xFE) ^ (1 << 3)) | (1 << 13), // wtf is this... we really need to figure out some of these macros
            arg0,
            (1 << 13) | (1 << 14),
            (1 << 14)
        );
    }
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_80039F18);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_80039F9C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_80039FF8);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003A094);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003A14C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003A20C);

void func_8003A2D4(void) {}

void func_8003A2DC(void) {}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003A2E4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003A344);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003A3B8);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003A450);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003A4FC);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003A55C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003A5D0);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003A65C);

s32 func_8003A82C(u16* arg0) {
    return arg0[8] >> 15;
}

// Program the manager's master-volume interpolator: immediate set (steps==0)
// or a stepped fade to `target` over `steps` ticks.
void func_8003A838(AudioManager* manager, s32 target, s32 steps) {
    s32 diff;

    if (target == 0) {
        target = 0x100;
    }
    manager->unk_Interpolator_0x64.targetValue = target;
    if (steps == 0) {
        manager->unk_Interpolator_0x64.counter = 0;
        manager->unk_Interpolator_0x64.currentValue = target << 16;
        manager->unk_0x54 = ((s16*)&manager->unk_0x58)[1] * target;
    } else {
        diff = (target << 16) - manager->unk_Interpolator_0x64.currentValue;
        if (diff != 0) {
            manager->unk_Interpolator_0x64.stepIncrement = diff / steps;
            manager->unk_Interpolator_0x64.counter = steps;
        }
    }
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003A89C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003A948);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003A9BC);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003AA30);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003AAC4);

void func_8003ABE8(u8* arg0, u8 arg1) {
    arg0[0x1B] = arg1;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003ABF0);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003AC58);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003ACC8);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003AD20);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003AD98);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003ADCC);

// Manager lifecycle restart: when the manager has a source image (unk_0x4)
// and the restart flag (0x10), re-seed the manager from it. Runs ON the tick
// path and re-enters the DisableEvent/EnableEvent bracket -- the reason the
// port's g_SoundTickMutex must be recursive (see psycross_sound_gate.patch).
void func_8003AE84(AudioManager* manager) {
    s32 savedTicks;

    if (manager->unk_Manager_0x4 != 0 && (manager->unk_Flags & 0x10)) {
        DisableEvent(g_unk_SoundEvent);
        SoundReleaseAllVoices(manager);
        savedTicks = manager->unk_0x24;
        SoundCopyAudioManagerData(manager,
                                  SOUND_PSX_TO_PTR(AudioManager, manager->unk_Manager_0x4));
        manager->unk_0x2c = savedTicks;
        SoundSetFlagsOnActiveVoices(manager, 0xFFFF);
        func_8003AFA0(manager);
        EnableEvent(g_unk_SoundEvent);
    }
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003AF24);

//----------------------------------------------------------------------------------------------------------------------
void func_8003AFA0(AudioManager* manager) {
    AudioElement* pElement;
    u32 cnt;

    cnt = manager->elementCount;
    pElement = &manager->elements[0];

    do {
        // What's going on here? Are the first two fields just a u32?
        if ((*(u32*)pElement & 0x101) == 0x101) {
            // I'm beginning to think that this isn't just a flag for activity considering this mask
            if ((pElement->active_flag & 0x30) == 0) {
                pElement->status_flags |= 0x1;
            }
        }
        pElement++;
        cnt--;
    } while (cnt);
}

//----------------------------------------------------------------------------------------------------------------------
void SoundAbortAllVoices(AudioManager* manager) {
    AudioElement* pElement;
    u32 cnt;

    cnt = manager->elementCount;
    pElement = &manager->elements[0];

    do {

        if (pElement->active_flag) {
            SoundAbortVoiceOnChannel(&pElement->voice_data, pElement->voice_number);
        }
        pElement++;
        cnt--;
    } while (cnt);
}

//----------------------------------------------------------------------------------------------------------------------
void SoundReleaseAllVoices(AudioManager* manager) {
    AudioElement* pElement;
    u32 cnt;

    cnt = manager->elementCount;
    pElement = &manager->elements[0];

    do {
        SoundReleaseVoiceFromChannel(&pElement->voice_data, pElement->voice_number);
        pElement++;
        cnt--;
    } while (cnt);
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003B0AC);

AudioManager* func_8003B148(s32 arg0) {
    s32 count;
    AudioManager* manager;
    AudioElement* elem;
    s32 i;
    s32 voice;
    s32 n;

    count = arg0 & ~1;
    D_80059478 = count;
    manager = SoundHeapAllocate(SoundCalculateAudioManagerSize(count));
    if (manager == NULL) {
        SoundHandleError(0x1E);
        return NULL;
    }
    func_8003B32C(manager);
    elem = manager->elements;
    voice = 0x18 - count;
    n = count;
    i = 0;
    do {
        elem->active_flag = 0;
        elem->unk_0x06[0] = i;
        elem->voice_number = voice;
        i++;
        elem++;
        voice++;
    } while (--n);
    SoundAddAudioManagerToList(manager);
    return manager;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003B1FC);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003B22C);

void func_8003B32C(AudioManager* manager) {
    manager->unk_Flags = 2;
    *(u16*)&manager->unk2[0] = 0x7FFF;
    *(u16*)&manager->unk_0x15[1] = 0;
    manager->unk_0x18 = 0x7F;
    manager->elementCount = D_80059478;
    SoundInitializeAudioManager(manager);
}

//----------------------------------------------------------------------------------------------------------------------
// TODO(jperos): Boy, we really need some names of this stuff
void SoundInitializeAudioManager(AudioManager* manager) {
    func_8003B930(manager);

    manager->unk_0x32 = 1;
    manager->unk_0x1a = 0;
    manager->unk_0x1b = 0;
    manager->unk_0x30 = 0;
    manager->unk_0x34 = 0;
    manager->unk_0x38 = 4;
    manager->unk_0x36 = 1;
    manager->unk_0x3a = 0x30;
    manager->unk_0x3c = 4;
    manager->unk_0x3e = 4;

    manager->unk_Interpolator_0x64.currentValue = 0x01000000;
    manager->unk_Interpolator_0x70.currentValue = 0x7F000000;

    manager->unk_0x58 = 0x00660000;
    manager->unk_0x54 = 0x6600;
    manager->unk_0x28 = 0;
    manager->unk_0x24 = 0;
    manager->unk_0x20 = 0;
    manager->unk_0x48 = 0;
    manager->unk_Interpolator_0x7c.currentValue = 0;
    manager->unk_Interpolator_0x88.currentValue = 0;
    manager->unk_Interpolator_0x64.counter = 0;
    manager->unk_Interpolator_0x70.counter = 0;
    manager->unk_Interpolator_0x7c.counter = 0;
    manager->unk_Interpolator_0x88.counter = 0;
    manager->unk_0x5c = 0;
    manager->unk_0x60 = 0;
    manager->unk_0x50 = 0x00010000;
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003B424);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003B644);

void func_8003B930(AudioManager* manager) {
    AudioManager* pEntry;
    AudioManager* pNext;

    pEntry = SOUND_PSX_TO_PTR(AudioManager, manager->unk_Manager_0x4);
    if (pEntry != NULL) {
        manager->unk_Manager_0x4 = SOUND_PTR_TO_PSX(NULL);
        do {
            pNext = SOUND_PSX_TO_PTR(AudioManager, pEntry->unk_Manager_0x4);
            SoundHeapFree(pEntry);
            pEntry = pNext;
        } while (pEntry != NULL);
    }
}

//----------------------------------------------------------------------------------------------------------------------
void SoundCopyAudioManagerData(AudioManager* pDest, AudioManager* pSrc) {
    AudioManager* savedNext;
    AudioManager* savedUnk;

    savedNext = SOUND_PSX_TO_PTR(AudioManager, pDest->next);
    savedUnk = SOUND_PSX_TO_PTR(AudioManager, pDest->unk_Manager_0x4);
    SoundHeapSetBlockMemory(pDest, pSrc, SoundCalculateAudioManagerSize(pDest->elementCount));
    pDest->next = SOUND_PTR_TO_PSX(savedNext);
    pDest->unk_Manager_0x4 = SOUND_PTR_TO_PSX(savedUnk);
}

//----------------------------------------------------------------------------------------------------------------------
void SoundAddAudioManagerToList(AudioManager* manager)
{
    AudioManager* temp;

    DisableEvent(g_unk_SoundEvent);
    temp = manager;
    manager->next = SOUND_PTR_TO_PSX(g_SoundAudioManagerListHead);
    g_SoundAudioManagerListHead = temp;
    EnableEvent(g_unk_SoundEvent);
}

//----------------------------------------------------------------------------------------------------------------------
s32 SoundRemoveAudioManagerFromList(AudioManager* manager) {
    AudioManager* current = g_SoundAudioManagerListHead;
    AudioManager* previous = NULL;

    // Search for target AudioManager in linked list
    while (current != NULL) {
        if (current == manager) {
            break;
        }
        previous = current;
        current = SOUND_PSX_TO_PTR(AudioManager, current->next);
    }

    // If not found, return error
    if (current == NULL) {
        SoundHandleError(SOUND_ERR_MANAGER_NOT_IN_LIST);
        return -1;
    }

    // Handle cleanup if needed (0x8000 flag set)
    if (manager->unk_Flags & 0x8000) {
        if (manager == NULL) {
            SoundHandleError(5);  // Invalid cleanup state
        } else {
            // Clear cleanup flag and release resources
            manager->unk_Flags &= ~(1 << 15);  // Clear bit 15
            SoundReleaseAllVoices(manager);
        }
    }

    // Remove from linked list
    if (previous != NULL) {
        // Removing head node
        previous->next = manager->next;
    } else {
        // Removing middle/end node
        g_SoundAudioManagerListHead = SOUND_PSX_TO_PTR(AudioManager, manager->next);
    }

    return 0;
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003BB08);

//----------------------------------------------------------------------------------------------------------------------
s32 SoundCalculateAudioManagerSize(s32 elementCount) {
    return (elementCount * sizeof(AudioElement)) + offsetof(AudioManager, elements);
}

//----------------------------------------------------------------------------------------------------------------------
void SoundOnTransferCallback(void) {
    SoundCommandCallback_t pCallback;

#ifdef XENO_PC_PORT
    pCallback = SoundTransferCallbackResolve(
        g_SoundTransferQueue[g_SoundTransferQueueReadIndex].callbackToken);
#else
    pCallback = (&g_SoundTransferQueue[g_SoundTransferQueueReadIndex])->pCallbackFn;
#endif
    g_SoundControlFlags |= 4;
    if (pCallback) {
        pCallback();
    }
    
    g_SoundControlFlags &= 0xFFEF;
    if (g_SoundTransferQueueReadIndex != g_SoundTransferQueueWriteIndex) {
        SoundProcessTransferCommand();
    }
    g_SoundControlFlags &= 0xFFFB;
}

void SoundQueueSpuWriteCommand(u32 transferAddress, void* pData, u_long dataSize, SoundCommandCallback_t pCallback) {
    SoundQueueTransferCommand(transferAddress, pData, dataSize, pCallback, SOUND_SPU_COMMAND_WRITE);
}

void SoundQueueSpuReadCommand(u32 transferAddress, void* pData, u_long dataSize, SoundCommandCallback_t pCallback) {
    SoundQueueTransferCommand(transferAddress, pData, dataSize, pCallback, SOUND_SPU_COMMAND_READ);
}

// Queue SPU ReadDecodedData SPU_ALL Command
void func_8003BC58(u32 transferAddress, void* pData, u_long dataSize, SoundCommandCallback_t pCallback) {
    SoundQueueTransferCommand(transferAddress, pData, dataSize, pCallback, 3);
}

// Queue SPU ReadDecodedData SPU_CDONLY Command
void func_8003BC7C(u32 transferAddress, void* pData, u_long dataSize, SoundCommandCallback_t pCallback) {
    SoundQueueTransferCommand(transferAddress, pData, dataSize, pCallback, 4);
}

void SoundQueueTransferCommand(u32 transferAddress, void* pData, u_long dataSize, SoundCommandCallback_t pCallback, unsigned short commandType) {
    SoundTransferCommand* pCmd;
    unsigned short nControlFlags;
    unsigned short nNextIndex;

    nControlFlags = g_SoundControlFlags;
    if (!(nControlFlags & 4)) {
        while (SoundTransferQueueSync());
        EnterCriticalSection();
    }

    nNextIndex = g_SoundTransferQueueWriteIndex + 1;
    if (nNextIndex >= SOUND_TRANSFER_QUEUE_SIZE) {
        nNextIndex = 0;
    }
    g_SoundTransferQueueWriteIndex = nNextIndex;
    
#ifdef XENO_PC_PORT
    pCmd = &g_SoundTransferQueue[nNextIndex];
#else
    // TODO: pCmd = &g_SoundTransferQueue[nNextIndex]; doesn't match, but there should be a cleaner line here
    pCmd = nNextIndex * sizeof(SoundTransferCommand) + (u32)g_SoundTransferQueue;
#endif
    pCmd->commandType = commandType & 0xF;
    pCmd->unk2 = 0;
#ifdef XENO_PC_PORT
    pCmd->pSpuData = SOUND_PTR_TO_PSX(pData);
    pCmd->pTransferAddress = (SoundPsxAddress)(transferAddress & 0x7FFF8);
    pCmd->dataSize = (u32)dataSize;
    pCmd->callbackToken = SoundTransferCallbackStore(nNextIndex, pCallback);
#else
    pCmd->pSpuData = pData;
    pCmd->pTransferAddress = transferAddress & 0x7FFF8;
    pCmd->dataSize = dataSize;
    pCmd->pCallbackFn = pCallback;
#endif
    
    if (!(g_SoundControlFlags & 0x10)) {
        SoundProcessTransferCommand();
    }
    
    if (!(nControlFlags & 4)) {
        ExitCriticalSection();
    }
}

int SoundTransferQueueSync() {
    unsigned short nWriteIndex = g_SoundTransferQueueWriteIndex;
    if (nWriteIndex < g_SoundTransferQueueReadIndex) {
        nWriteIndex += SOUND_TRANSFER_QUEUE_SIZE;
    }
    return (nWriteIndex - g_SoundTransferQueueReadIndex < 6) ^ 1;
}

void func_8003BDF4(void) {}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003BDFC);

//----------------------------------------------------------------------------------------------------------------------
void SoundProcessTransferCommand(void) {
    SpuTransferCallbackProc pPrevCallback;
    unsigned short nNextIndex;
    SoundTransferCommand* pCmd;

    nNextIndex = g_SoundTransferQueueReadIndex + 1;
    if (nNextIndex >= SOUND_TRANSFER_QUEUE_SIZE) {
        nNextIndex = 0;
    }
    g_SoundTransferQueueReadIndex = nNextIndex;
    
    g_SoundControlFlags |= 0x10;

#ifdef XENO_PC_PORT
    pCmd = &g_SoundTransferQueue[nNextIndex];
#else
    // TODO: pCmd = &g_SoundTransferQueue[nNextIndex]; doesn't match, but there should be a cleaner line here
    pCmd = nNextIndex * sizeof(SoundTransferCommand) + (u32)g_SoundTransferQueue;
#endif
    
    pPrevCallback = SpuSetTransferCallback(&SoundOnTransferCallback);
    SpuSetTransferMode(SPU_TRANSFER_BY_DMA);
    SpuSetTransferStartAddr(pCmd->pTransferAddress);
    switch (pCmd->commandType) {
        case 0:
            break;
        case SOUND_SPU_COMMAND_WRITE:
#ifdef XENO_PC_PORT
            SpuWrite(SOUND_PSX_TO_PTR(void, pCmd->pSpuData), pCmd->dataSize);
#else
            SpuWrite(pCmd->pSpuData, pCmd->dataSize);
#endif
            break;
        case SOUND_SPU_COMMAND_READ:
#ifdef XENO_PC_PORT
            SpuRead(SOUND_PSX_TO_PTR(void, pCmd->pSpuData), pCmd->dataSize);
#else
            SpuRead(pCmd->pSpuData, pCmd->dataSize);
#endif
            break;
        case 3:
#ifdef XENO_PC_PORT
            D_80059548 = SpuReadDecodedData(
                SOUND_PSX_TO_PTR(void, pCmd->pSpuData), SPU_ALL);
#else
            D_80059548 = SpuReadDecodedData(pCmd->pSpuData, SPU_ALL);
#endif
            break;
        case 4:
#ifdef XENO_PC_PORT
            D_80059548 = SpuReadDecodedData(
                SOUND_PSX_TO_PTR(void, pCmd->pSpuData), SPU_CDONLY);
#else
            D_80059548 = SpuReadDecodedData(pCmd->pSpuData, SPU_CDONLY);
#endif
            break;
    }
    
    if (pPrevCallback != &SoundOnTransferCallback) {
        SoundHandleError(SOUND_ERR_UNEXPECTED_CALLBACK);
    }
}

//----------------------------------------------------------------------------------------------------------------------
void SoundSpuIRQHandler(void) {
    g_SoundControlFlags |= SOUND_CTL_FLAG_IRQ_HANDLER;
    g_SoundSpuIRQCount++;
    if (g_SoundSpuIrqCallbackFn) {
        g_SoundSpuIrqCallbackFn();
    }
    g_SoundControlFlags &= ~SOUND_CTL_FLAG_IRQ_HANDLER;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundSetSpuIrqCallback(u32 func) {
    g_SoundSpuIrqCallbackFn = func;
}

//----------------------------------------------------------------------------------------------------------------------
// The 240Hz sound tick (RCnt2 counter-2 event handler, registered by
// SoundInitialize). Every other tick: master/CD volume fades + pending
// SpuSetCommonAttr flush. Every tick: flush dirty voice registers
// (func_8003E900); for each active manager run the lifecycle restart check
// (func_8003AE84 -- re-enters the event bracket), interpolator ticks, and
// the sequencer step loop (func_8003C4C4 + func_8003C6E8) while the step
// accumulator is in debt; second pass runs the envelope (func_8003EFE4) and
// voice-apply (func_8003EBF0) passes; then key-off flush (func_8003EB5C),
// deferred SPU-IRQ re-enable, and tick-duration bookkeeping.
// On the port this body runs on the interrupt thread inside g_SoundTickMutex
// (psycross_sound_gate.patch) -- the gate validated before this body landed.
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; the matching
 * build keeps INCLUDE_ASM (byte-exact) below. Residual vs {}: this pipeline's
 * cc1 strength-reduces the element cursor family onto a different anchor and
 * fills load-latency slack the retail object leaves unfilled -- same
 * operations at the same absolute element offsets, rebased registers/
 * displacements. Semantics traced 1:1 against the split asm. Not claimed
 * as {}. */
long func_8003C020(void) {
    u32 start;
    AudioManager* manager;

    if (g_SoundControlFlags & 0x40) {
        return 0;
    }
    start = GetRCnt(0xF2000002);
    {
        s32 phase = D_80059504;
        D_80059504 = phase + 1;
        if (phase & 0x1) {
            if (g_SoundVolumeController.masterInterpolator.counter != 0) {
                s16 vol;
                SoundTickInterpolator(&g_SoundVolumeController.masterInterpolator);
                vol = ((s16*)&g_SoundVolumeController.masterInterpolator.currentValue)[1];
                g_SoundVolumeController.currentMasterVolume = vol;
                SoundSetVolumeWithPhase(vol, &g_SoundVolumeController.commonAttr.mvol, 0);
                g_SoundVolumeController.commonAttr.mask |= 0x3;
            }
            /* Retail reads this via the alias symbol g_SoundCdFadeFramesRemaining
             * (== &g_SoundVolumeController.cdInterpolator.counter, 0x8005A404);
             * the struct field keeps one storage location on the port. */
            if (g_SoundVolumeController.cdInterpolator.counter != 0) {
                u16 vol;
                SoundTickInterpolator(&g_SoundVolumeController.cdInterpolator);
                vol = ((u16*)&g_SoundVolumeController.cdInterpolator.currentValue)[1];
                g_SoundVolumeController.currentCdVolume = vol;
                g_SoundVolumeController.commonAttr.cd.volume.right = vol;
                g_SoundVolumeController.commonAttr.cd.volume.left = vol;
                g_SoundVolumeController.commonAttr.mask |= 0xC0;
            }
            if (g_SoundVolumeController.commonAttr.mask != 0) {
                SpuSetCommonAttr(&g_SoundVolumeController.commonAttr);
                g_SoundVolumeController.commonAttr.mask = 0;
            }
        }
    }
    func_8003E900();
    manager = g_SoundAudioManagerListHead;
    if (manager != NULL) {
        do {
            if (manager->unk_Flags < 0) {
                if (manager->unk_0x2c != 0 &&
                    (u32)manager->unk_0x24 >= (u32)manager->unk_0x2c) {
                    func_8003AE84(manager);
                }
                if (manager->unk_Interpolator_0x64.counter != 0) {
                    SoundTickInterpolator(&manager->unk_Interpolator_0x64);
                    manager->unk_0x54 =
                        ((s16*)&manager->unk_0x58)[1] *
                        ((s16*)&manager->unk_Interpolator_0x64.currentValue)[1];
                }
                if (manager->unk_Interpolator_0x70.counter != 0) {
                    SoundTickInterpolator(&manager->unk_Interpolator_0x70);
                    unk_SoundSetFlagsOnActiveVoices(0x100, manager);
                }
                if (manager->unk_Interpolator_0x7c.counter != 0) {
                    SoundTickInterpolator(&manager->unk_Interpolator_0x7c);
                    unk_SoundSetFlagsOnActiveVoices(0x200, manager);
                }
                if (manager->unk_Interpolator_0x88.counter != 0) {
                    SoundTickInterpolator(&manager->unk_Interpolator_0x88);
                    unk_SoundSetFlagsOnActiveVoices(0x100, manager);
                }
                manager->unk_0x20++;
                manager->unk_0x28 += ((s16*)&manager->unk_Interpolator_0x64.currentValue)[1];
                manager->unk_0x50 -= manager->unk_0x54;
                if (manager->unk_0x50 < 0) {
                    do {
                        manager->unk_0x36--;
                        manager->unk_0x50 += 0x10000;
                        if (manager->unk_0x36 == 0) {
                            manager->unk_0x36 = manager->unk_0x3a;
                            manager->unk_0x34++;
                            if (manager->unk_0x38 < manager->unk_0x34) {
                                manager->unk_0x34 = 1;
                                manager->unk_0x32++;
                            }
                        }
                        {
                            u32 cnt = manager->elementCount;
                            if (cnt != 0) {
                                func_8003C4C4(manager, &manager->elements[0], cnt);
                                func_8003C6E8(manager, &manager->elements[0], cnt);
                            }
                        }
                        if (manager->unk_0x48 == 0) {
                            manager->unk_Flags &= 0x7FFF;
                            break;
                        }
                        manager->unk_0x24++;
                        if (manager->unk_Interpolator_0x70.currentValue == 0) {
                            func_80039C4C(manager);
                            manager->unk_Flags |= 0x100;
                        }
                        if (manager->unk_0x32 == *(u16*)&manager->unk_0x1c[2]) {
                            manager->unk_Flags &= 0xFFDF;
                            func_8003A838(manager, 0, 0);
                            *(u16*)&manager->unk_0x1c[2] = 0;
                        }
                    } while (manager->unk_0x50 < 0);
                }
            }
            manager = SOUND_PSX_TO_PTR(AudioManager, manager->next);
        } while (manager != NULL);
        manager = g_SoundAudioManagerListHead;
    }
    while (manager != NULL) {
        if (manager->unk_Flags < 0) {
            u32 cnt = manager->elementCount;
            if (cnt != 0) {
                func_8003EFE4(manager, &manager->elements[0], cnt);
                func_8003EBF0(manager, &manager->elements[0], cnt);
            }
        }
        manager = SOUND_PSX_TO_PTR(AudioManager, manager->next);
    }
    func_8003EB5C();
    {
        u16 irqFlags = D_8005955C;
        if (irqFlags & 0x1) {
            D_8005955C = irqFlags & 0xFFFE;
            SpuSetIRQ(0x1);
        }
    }
    {
        u32 end = GetRCnt(0xF2000002);
        if (end >= start) {
            D_800595C4 += end - start;
            D_80059540++;
        }
    }
    return 0;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003C020);
#endif

//----------------------------------------------------------------------------------------------------------------------
void SoundTickInterpolator(AudioInterpolator* interpolator) {
    interpolator->counter--;

    if (interpolator->counter != 0) {
        interpolator->currentValue += interpolator->stepIncrement;
    } else {
        interpolator->currentValue = (s32)(interpolator->targetValue << 16);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Per-step envelope/counter tick, run once per sequencer step for every
// active element: steps the manager's inline master-volume interpolator
// (unk_0x58 block), then per element decrements the pitch/vibrato/volume/pan
// envelope counters (packed u16 pairs), advances their accumulators, and
// retires notes whose duration ran out. The element is addressed through the
// status_flags-anchored cursor (`st`, halfword units) -- the retail second
// induction pointer.
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; the matching
 * build keeps INCLUDE_ASM (byte-exact) below. Residual vs {}: this pipeline's
 * cc1 strength-reduces the element cursor family onto a different anchor and
 * fills load-latency slack the retail object leaves unfilled -- same
 * operations at the same absolute element offsets, rebased registers/
 * displacements. Semantics traced 1:1 against the split asm. Not claimed
 * as {}. */
void func_8003C4C4(AudioManager* manager, AudioElement* pAudioElements, s32 count) {
    u32 minusOne;
    u16* st;
    u16 steps;

    steps = manager->unk_0x60;
    if (steps != 0) {
        steps--;
        if (steps != 0) {
            manager->unk_0x58 = manager->unk_0x58 + manager->unk_0x5c;
        } else {
            manager->unk_0x58 = *(u16*)&manager->unk_0x62 << 16;
        }
        manager->unk_0x60 = steps;
        manager->unk_0x54 = ((s16*)&manager->unk_0x58)[1] *
                            ((s16*)&manager->unk_Interpolator_0x64.currentValue)[1];
    }
    minusOne = 0xFFFF;
    st = &pAudioElements->status_flags;
    do {
        u16 act = pAudioElements->active_flag;
        if (act) {
            u32 pair;
            u32 lo;
            u32 hi;
            u16 status;
            pair = *(s32*)&st[0x2D];
            status = st[0];
            lo = pair & 0xFFFF;
            hi = pair >> 16;
            if (lo != 0) {
                u16 instFlags = st[1];
                u16 cnt;
                u16 v;
                if (instFlags & 0x8) {
                    cnt = st[0x4A] + minusOne;
                    st[0x4A] = cnt;
                    status |= 0x100;
                    if (cnt == 0) {
                        instFlags &= 0xFFF7;
                    }
                    *(s32*)&st[0x3B] = *(s32*)&st[0x3B] + *(s32*)&st[0x43];
                }
                if (instFlags & 0x1) {
                    status |= 0x200;
                    if (!(instFlags & 0x2)) {
                        cnt = st[0x49] + minusOne;
                        st[0x49] = cnt;
                        if (cnt == 0) {
                            instFlags &= 0xFFFE;
                        }
                    }
                    *(s32*)&st[0x33] = *(s32*)&st[0x33] + *(s32*)&st[0x41];
                }
                if (instFlags & 0x10) {
                    cnt = st[0x4B] + minusOne;
                    st[0x4B] = cnt;
                    if (cnt == 0) {
                        v = st[0x48];
                        instFlags &= 0xFFEF;
                    } else {
                        v = st[0x39] + st[0x47];
                    }
                    st[0x39] = v;
                    status |= 0x100;
                }
                if (instFlags & 0x20) {
                    cnt = st[0x4C] + minusOne;
                    st[0x4C] = cnt;
                    if (cnt == 0) {
                        v = st[0x46];
                        instFlags &= 0xFFDF;
                    } else {
                        v = st[0x3A] + st[0x45];
                    }
                    st[0x3A] = v;
                    status |= 0x100;
                }
                st[1] = instFlags;
                lo--;
                hi--;
                if (lo == 1 && (act & 0x1000)) {
                    *(u8*)&st[0x2C] = 0x6;
                    st[0x1A] |= 0x80;
                }
                if (hi == 0) {
                    status |= 0x2;
                    pAudioElements->active_flag |= 0x400;
                }
                *(s32*)&st[0x2D] = lo + (hi << 16);
            }
            st[0] = status;
        }
        st += 0xAC;
        count--;
        pAudioElements++;
    } while ((s16)count != 0);
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003C4C4);
#endif

// Sequencer step: for every active element whose duration counter reached
// zero, interpret script bytes -- notes (<0x80: velocity/articulation lookup,
// instrument/percussion prime) and command opcodes (>=0x80: dispatch through
// g_SoundScriptHandlers) -- until a status bit (0x100 note / 0x400 rest)
// stops the stream. Then scan ahead for ties (0x80/0x81/0x90/0x99/0x9A/0xB0)
// to decide the note-off, derive the duration/gate pair, and arm portamento,
// vibrato and envelope objects for the new note.
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; the matching
 * build keeps INCLUDE_ASM (byte-exact) below. Residual vs {}: this pipeline's
 * cc1 strength-reduces the element cursor family onto a different anchor and
 * fills load-latency slack the retail object leaves unfilled -- same
 * operations at the same absolute element offsets, rebased registers/
 * displacements. Semantics traced 1:1 against the split asm. Not claimed
 * as {}. */
void func_8003C6E8(AudioManager* manager, AudioElement* pAudioElements, s32 count) {
    u16* st;

    st = &pAudioElements->status_flags;
    do {
        if (pAudioElements->active_flag != 0 && st[0x2D] == 0) {
            u32 hasNote;
            u16 flagsIn;
            u8* ip;
            u32 b;

            hasNote = 0;
            flagsIn = pAudioElements->active_flag;
            ip = SOUND_PSX_TO_PTR(u8, *(SoundPsxAddress*)&st[0x9]);
            pAudioElements->active_flag = flagsIn & 0xF8FF;
            for (;;) {
                b = *ip++;
                if (b < 0x80) {
                    if (!(pAudioElements->active_flag & 0x8)) {
                        st[0x39] = b << 8;
                    }
                    st[0] |= 0x100;
                    {
                        u32 vel = *ip;
                        u32 tmp = *(u8*)&st[0x32] + D_80050A94[vel];
                        u32 dur;
                        ip++;
                        ((u8*)&st[0x31])[1] = tmp;
                        dur = D_800509B0[vel];
                        b = tmp & 0xFF;
                        if (dur == 0) {
                            dur = *ip++;
                        }
                        st[0x2D] = dur;
                    }
                    *(u8*)&st[0x2C] = *(u8*)&st[0x13];
                    st[0x1A] |= 0x80;
                    if (pAudioElements->active_flag & 0x10) {
                        func_8003CC84(manager, pAudioElements, b);
                    } else {
                        *(s32*)&st[0x33] =
                            (u32)((b << 8) + *(s16*)&st[0x36] + *(s16*)&st[0x35]) << 16;
                    }
                    st[0] |= 0x200;
                    pAudioElements->active_flag |= 0x180;
                    hasNote = 1;
                    if (flagsIn & 0x400) {
                        st[0] |= 0x1;
                    }
                    {
                        u16 act = pAudioElements->active_flag;
                        if (act & 0x8000) {
                            pAudioElements->active_flag = act & 0x7FFF;
                            st[0x1A] = 0xFFFF;
                            st[0] |= 0x300;
                        }
                    }
                } else {
                    ip = g_SoundScriptHandlers[(s16)(b - 0x80)](ip, manager, pAudioElements);
                    if (pAudioElements->active_flag == 0) {
                        manager->unk_0x48 &= ~(1 << *(u8*)&st[0x2]);
                        goto store_ip;
                    }
                }
                if (pAudioElements->active_flag & 0x500) {
                    break;
                }
            }
        store_ip:
            *(SoundPsxAddress*)&st[0x9] = SOUND_PTR_TO_PSX(ip);
            if (pAudioElements->active_flag != 0) {
                u16 act;
                u8* rec;
                s32 rel;
                s32 dur2;
                u32 gate;

                act = pAudioElements->active_flag;
                if (act & 0x800) {
                    pAudioElements->active_flag = act | 0x200;
                }
                rec = (u8*)pAudioElements + (st[0x38] * 12) + 0x9C;
                b = *ip;
                while (b >= 0x80) {
                    s32 sb = (s16)b;
                    if (sb == 0x90) {
                        SoundPsxAddress saved = *(SoundPsxAddress*)&st[0xB];
                        if (saved == 0) {
                            break;
                        }
                        ip = SOUND_PSX_TO_PTR(u8, saved);
                    } else if (sb == 0x80) {
                        pAudioElements->active_flag &= 0xFDFF;
                        break;
                    } else if (sb == 0x81) {
                        pAudioElements->active_flag |= 0x200;
                        break;
                    } else if ((u32)(sb - 0xB0) < 2) {
                        pAudioElements->active_flag &= 0xFDFF;
                        break;
                    } else {
                        if (sb == 0x99) {
                            if (*rec != 0) {
                                ip = SOUND_PSX_TO_PTR(u8, *(u32*)(rec + 4));
                                goto scan_next;
                            }
                            rec -= 0xC;
                        }
                        if (b == 0x9A && *rec == 0) {
                            ip = SOUND_PSX_TO_PTR(u8, *(u32*)(rec + 8));
                            rec -= 0xC;
                            goto scan_next;
                        }
                        ip += D_80050824[(s16)(b - 0x80)];
                    }
                scan_next:
                    b = *ip;
                }
                if (b < 0x80) {
                    pAudioElements->active_flag |= 0x1000;
                } else {
                    pAudioElements->active_flag &= 0xEFFF;
                }
                rel = (s8)*(u8*)&st[0x2F] + st[0x2D];
                if ((s16)rel > 0) {
                    dur2 = rel;
                } else {
                    dur2 = st[0x2D] + rel;
                    *(u8*)&st[0x2F] = *(u8*)&st[0x2F] + *(u8*)&st[0x2D];
                }
                if (!(pAudioElements->active_flag & 0x600)) {
                    gate = st[0x30];
                    if (gate == 0xF) {
                        gate = dur2 - 1;
                    } else if (gate == 0x10) {
                        gate = dur2;
                    } else {
                        gate = ((s16)dur2 * gate) >> 4;
                    }
                    if ((u16)gate == 0) {
                        gate = 1;
                    }
                } else {
                    gate = 0x7FFF;
                }
                *(s32*)&st[0x2D] = (s16)dur2 + (gate << 16);
                if (hasNote != 0) {
                    if (st[1] & 0x4) {
                        u32 diff = ((u8*)&st[0x31])[1] - *(u8*)&st[0x31];
                        if ((s8)diff != 0) {
                            s32 slide = (s32)(diff << 24) / st[0x37];
                            st[1] |= 0x1;
                            st[0x49] = st[0x37];
                            *(s32*)&st[0x33] =
                                (u32)((*(u8*)&st[0x31] << 8) + *(s16*)&st[0x36] +
                                      *(s16*)&st[0x35]) << 16;
                            *(s32*)&st[0x41] = slide;
                        }
                    }
                    {
                        u16 instFlags2 = st[1];
                        *(u8*)&st[0x31] = ((u8*)&st[0x31])[1];
                        if (instFlags2 & 0x100) {
                            st[0x4A] = st[0x3F];
                            *(s32*)&st[0x43] = *(s32*)&st[0x3D];
                            *(s32*)&st[0x3B] = st[0x40] << 16;
                            st[1] = instFlags2 | 0x8;
                        }
                    }
                    {
                        s32 slot = 4;
                        u16* pEnvFlags = (u16*)((u8*)pAudioElements + 0xF6);
                        do {
                            u16 envFlags = pEnvFlags[0];
                            if ((envFlags & 0x3) == 0x3) {
                                *(s32*)&pEnvFlags[-0xD] = 0;
                                pEnvFlags[-7] = 1;
                                pEnvFlags[-5] = pEnvFlags[-4];
                                pEnvFlags[-3] = pEnvFlags[-2];
                                st[0] |= 0x100;
                                pEnvFlags[0] = envFlags & 0xFFF3;
                            }
                            slot--;
                            pEnvFlags += 0x10;
                        } while (slot != 0);
                    }
                }
            }
        }
        st += 0xAC;
        count--;
        pAudioElements++;
    } while ((s16)count != 0);
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003C6E8);
#endif

// Percussion note-on: load the percussion map entry for `note` (instrument,
// transpose, pan), program the element's instrument via func_8003E5BC, and
// prime the note pitch + VOLUME_CHANGE status.
void func_8003CC84(AudioManager* manager, AudioElement* pAudioElements, s32 note) {
    u8* pPerc;

    pPerc = SOUND_PSX_TO_PTR(u8, ((note & 0xFF) << 2) + manager->unk_0xc);
    func_8003E5BC(pPerc[0], pAudioElements);
    pAudioElements->unk68 =
        (u32)((pPerc[1] << 8) + pAudioElements->unk_0x6E + pAudioElements->unk_0x6C) << 16;
    {
        u16 status = pAudioElements->status_flags;
        u32 pan = pPerc[3];
        pAudioElements->status_flags = status | 0x100;
        pAudioElements->unk_0x74 = pan << 8;
    }
}

u8* SoundScriptDefaultHandler(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    return pScript;
}

// Rest note handler?
// Seq cmd: rest -- set duration, flag REST status + active bit.
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; matching build
 * keeps INCLUDE_ASM below. Residual: load-scheduling cluster placement (same
 * ops, same offsets). */
u8* func_8003CD08(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u16 status;
    u16 active;
    status = pAudioElements->status_flags;
    pAudioElements->fermataDuration = pScript[0];
    active = pAudioElements->active_flag;
    pAudioElements->status_flags = status | 0x2;
    pAudioElements->active_flag = active | 0x400;
    return pScript + 1;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003CD08);
#endif

// Fermata / Hold note
u8* SoundScriptFermata(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8 fermataDuration = *pScript;
    pAudioElements->active_flag |= SOUND_CHANNEL_HOLD_NOTE;
    pAudioElements->fermataDuration = fermataDuration;
    return pScript + 1;
}

u8* SoundScriptNop3(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    return pScript;
}

// Seq cmd: loop-return marker -- when the marker byte matches the manager's
// current marker, save the resume IP + octave.
u8* func_8003CD54(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8 marker = *pScript++;
    if (marker == pAudioManager->unk_0x1b) {
        pAudioElements->savedScriptIP = SOUND_PTR_TO_PSX(pScript);
        pAudioElements->savedOctave = pAudioElements->octave;
    }
    return pScript;
}

u8* SoundScriptDefaultSkip3(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    return pScript + 3;
}

u8* SoundScriptNop4(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    return pScript;
}

// Seq cmd 0x90: return to the saved IP (dal segno) with octave restore; with
// no saved IP, release the voice and deactivate the element (track end).
u8* func_8003CD8C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8* ip = pScript;
    if (pAudioElements->savedScriptIP != 0) {
        ip = SOUND_PSX_TO_PTR(u8, pAudioElements->savedScriptIP);
        *(u16*)&pAudioElements->unk_0x1C[4] += 1;
        pAudioElements->octave = pAudioElements->savedOctave;
    } else {
        pAudioElements->status_flags &= 0xFFFC;
        SoundReleaseVoiceFromChannel(&pAudioElements->voice_data,
                                     pAudioElements->voice_number);
        pAudioElements->active_flag = 0;
    }
    return ip;
}

u8* SoundScriptSaveOctaveAndIP(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    pAudioElements->savedScriptIP = SOUND_PTR_TO_PSX(pScript);
    pAudioElements->savedOctave = pAudioElements->octave;
    return pScript;
}

u8* SoundScriptSetOctave(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    pAudioElements->octave = *pScript * NUM_NOTES_PER_OCTAVE;
    return pScript + 1;
}

u8* SoundScriptRaiseOctave(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    pAudioElements->octave += NUM_NOTES_PER_OCTAVE;
    return pScript;
}

u8* SoundScriptLowerOctave(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    pAudioElements->octave -= NUM_NOTES_PER_OCTAVE;
    return pScript;
}

// Time signature handler?
// Seq cmd: time signature -- beats/measure + beat length (0xC0/denominator).
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; matching build
 * keeps INCLUDE_ASM below. Residual: load-scheduling cluster placement (same
 * ops, same offsets). */
u8* func_8003CE68(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u32 denom = pScript[1];
    pAudioManager->unk_0x3a = 0xC0 / denom;
    pAudioManager->unk_0x3c = denom;
    pAudioManager->unk_0x38 = pScript[0];
    pAudioManager->unk_0x3e = pScript[0];
    pAudioManager->unk_0x36 = pAudioManager->unk_0x3a;
    return pScript + 2;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003CE68);
#endif

// Seq cmd: jump-count / measure sync -- latch measure number + beat.
u8* func_8003CE9C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u16 beat;
    u8 measure;
    pAudioManager->unk_0x32 = pScript[0];
    beat = pAudioManager->unk_0x3a;
    measure = pScript[1];
    pAudioManager->unk_0x36 = beat;
    pAudioManager->unk_0x34 = measure;
    return pScript + 2;
}

u8* SoundScriptSetManagerUnk1a(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    pAudioManager->unk_0x1a = *pScript;
    return pScript + 1;
}

u8* SoundScriptAddManagerUnk1a(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    pAudioManager->unk_0x1a += *pScript;
    return pScript + 1;
}

// Loop start handler?
// Seq cmd: loop start -- push a loop-stack record (count-1, resume IP,
// octave) at the element's current selector.
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; matching build
 * keeps INCLUDE_ASM below. Residual: non-semantic codegen shape (load-reload
 * elision / register-copy / arg-setup scheduling); ops+offsets audited 1:1
 * against the split asm and exercised via the synthetic stream. */
u8* func_8003CEF0(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8* rec;
    u16 sel;
    *(u16*)&pAudioElements->unk_0x70[2] += 1;
    sel = *(u16*)&pAudioElements->unk_0x70[2];
    rec = (u8*)pAudioElements + (sel * 12 + 0x9C);
    rec[0] = *pScript++ + 0xFF;
    *(u32*)(rec + 4) = SOUND_PTR_TO_PSX(pScript);
    rec[2] = pAudioElements->octave;
    return pScript;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003CEF0);
#endif

// Loop end handler?
// Seq cmd 0x99: loop continue -- decrement the loop-stack counter; while it
// hasn't wrapped, jump back to the loop start (saving the fall-through IP +
// octave in the record); when exhausted, pop the selector.
u8* func_8003CF38(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8* rec = (u8*)pAudioElements + (*(u16*)&pAudioElements->unk_0x70[2] * 12 + 0x9C);
    u8 cnt = rec[0] - 1;
    rec[0] = cnt;
    if (cnt != 0xFF) {
        *(u32*)(rec + 8) = SOUND_PTR_TO_PSX(pScript);
        rec[3] = pAudioElements->octave;
        pScript = SOUND_PSX_TO_PTR(u8, *(u32*)(rec + 4));
        pAudioElements->octave = rec[2];
    } else {
        *(u16*)&pAudioElements->unk_0x70[2] -= 1;
    }
    return pScript;
}

// Seq cmd: loop end (exhausted) -- when the record's counter hit zero, pop
// the loop stack: resume IP from the record, restore octave.
u8* func_8003CFA4(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8* rec = (u8*)pAudioElements + (*(u16*)&pAudioElements->unk_0x70[2] * 12 + 0x9C);
    if (rec[0] == 0) {
        u16 sel;
        pScript = SOUND_PSX_TO_PTR(u8, *(u32*)(rec + 8));
        sel = *(u16*)&pAudioElements->unk_0x70[2] - 1;
        pAudioElements->octave = rec[3];
        *(u16*)&pAudioElements->unk_0x70[2] = sel;
    }
    return pScript;
}

// Seq cmd: manager event with 16-bit parameter (fixed velocity/flag args).
u8* func_8003CFF0(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    func_80039F18(pScript[0] | (pScript[1] << 8), 0x7F, 0x40);
    return pScript + 3;
}

// Seq cmd: manager event -- forward a 16-bit parameter to func_8003A14C.
u8* func_8003D034(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    func_8003A14C(pScript[0] | (pScript[1] << 8));
    return pScript + 2;
}

// Seq cmd 0x9E: external jump into a SED file -- find the SED by the
// element's id (or take the list head), then jump via its 16-bit offset
// table. Not-found leaves the IP unadvanced (retail behavior).
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; matching build
 * keeps INCLUDE_ASM below. Residual: non-semantic codegen micro-shape
 * (commutative operand order / delay-slot copy placement / register reuse);
 * ops+offsets audited 1:1 against the split asm. */
u8* func_8003D070(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    SoundFile* e = g_SoundSedsLinkedList;
    u32 pair = pScript[0] | (pScript[1] << 8);
    s16 id = *(s16*)&pAudioElements->unk_0x06[4];
    u8 q = pScript[2];
    if (id != 0) {
        while (*(u16*)((u8*)e + 0x14) != id) {
            e = SOUND_PSX_TO_PTR(SoundFile, *(u32*)((u8*)e + 0x1C));
            if (e == NULL) {
                return pScript;
            }
        }
    }
    {
        u16 off = *(u16*)((u8*)e + ((q + ((s16)pair << 1)) << 1) + 0x20);
        pScript = (u8*)e + off;
    }
    return pScript + 3;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003D070);
#endif

// Set tempo handler?
// Seq cmd: set manager volume (immediate).
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; matching build
 * keeps INCLUDE_ASM below. Residual: load-scheduling cluster placement (same
 * ops, same offsets). */
u8* func_8003D0E8(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8 raw = pScript[0];
    s32 vol = raw & 0xFF;
    pAudioManager->unk_0x54 = vol * ((s16*)&pAudioManager->unk_Interpolator_0x64.currentValue)[1];
    pAudioManager->unk_0x58 = vol << 16;
    return pScript + 1;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003D0E8);
#endif

// Seq cmd: nudge manager volume by a signed step.
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; matching build
 * keeps INCLUDE_ASM below. Residual: load-scheduling cluster placement (same
 * ops, same offsets). */
u8* func_8003D110(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    s32 step = (s8)pScript[0] << 16;
    s32 cur = pAudioManager->unk_0x58;
    pAudioManager->unk_0x54 = 0;
    pAudioManager->unk_0x58 = cur + step;
    return pScript + 1;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003D110);
#endif

// Seq cmd: master volume fade -- target + step count for the inline
// interpolator at manager+0x58.
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; matching build
 * keeps INCLUDE_ASM below. Residual: load-scheduling cluster placement (same
 * ops, same offsets). */
u8* func_8003D13C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    s32 target = pScript[1];
    s32 steps = pScript[0];
    s32 diff;
    *(u16*)&pAudioManager->unk_0x62 = target;
    diff = (target << 16) - pAudioManager->unk_0x58;
    if (steps != 0 && diff != 0) {
        pAudioManager->unk_0x5c = diff / steps;
        pAudioManager->unk_0x60 = steps;
    }
    return pScript + 2;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003D13C);
#endif

// Seq cmd: channel level immediate -- set interp70 and flag volume change on
// active voices.
u8* func_8003D17C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8 level = *pScript++;
    pAudioManager->unk_Interpolator_0x70.currentValue = level << 24;
    unk_SoundSetFlagsOnActiveVoices(0x100, pAudioManager);
    return pScript;
}

// Seq cmd: channel level fade -- program the manager's interp70 toward
// target<<24 over n*32 steps.
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; matching build
 * keeps INCLUDE_ASM below. Residual: non-semantic codegen shape (load-reload
 * elision / register-copy / arg-setup scheduling); ops+offsets audited 1:1
 * against the split asm and exercised via the synthetic stream. */
u8* func_8003D1BC(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8* p = pScript;
    s32 steps = p[0] << 5;
    s32 diff = (p[1] << 24) - pAudioManager->unk_Interpolator_0x70.currentValue;
    if (steps != 0 && diff != 0) {
        pAudioManager->unk_Interpolator_0x70.counter = steps;
        pAudioManager->unk_Interpolator_0x70.targetValue = p[1] << 8;
        pAudioManager->unk_Interpolator_0x70.stepIncrement = diff / steps;
    }
    return p + 2;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003D1BC);
#endif

u8* SoundScriptSetUnk62(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    pAudioElements->unk_0x62 = *pScript;
    return pScript + 1;
}

// Seq cmd 0xAA: move the element to another voice (release + assign-stopped).
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; matching build
 * keeps INCLUDE_ASM below. Residual: non-semantic codegen micro-shape
 * (commutative operand order / delay-slot copy placement / register reuse);
 * ops+offsets audited 1:1 against the split asm. */
u8* func_8003D21C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8 n = *pScript++;
    if (n < 0x19) {
        SoundVoiceData* vd = &pAudioElements->voice_data;
        SoundReleaseVoiceFromChannel(vd, pAudioElements->voice_number);
        pAudioElements->voice_number = n;
        SoundAssignVoiceToChannelAndStop(vd, n);
    }
    return pScript;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003D21C);
#endif

u8* func_8003D298(u8* a0, s32 a1, s32 a2) {
    func_8003E5BC(*a0++, a2);
    return a0;
}

u8* func_8003D2D0(u8* a0, s32 a1, u8* a2) {
    u8 b = *a0++;
    if (b != 0) {
        a2[0x60] += b;
    } else {
        a2[0x60] = 0;
    }
    return a0;
}

// Percussion On handler?
s32 func_8003D300(s32 a0, s32* a1, u16* a2) {
    if (a1[3] != 0) {
        a2[0] |= 0x10;
    }
    return a0;
}

u8* SoundScriptPercussionOff(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    pAudioElements->active_flag &= ~SOUND_CHANNEL_PERCUSSION_ACTIVE;
    return pScript;
}

u8* SoundScriptSetActiveFlag800(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    pAudioElements->active_flag |= 0x800;
    return pScript;
}

u8* SoundScriptClearActiveFlag800(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    pAudioElements->active_flag &= ~0x800;
    return pScript;
}

// Seq cmd: noise mode on (odd voices only).
u8* func_8003D370(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    if (pAudioElements->voice_number & 0x1) {
        pAudioElements->voice_data.flags |= 0x1000;
        pAudioElements->voice_data.modeFlags |= 0x10;
    }
    return pScript;
}

// Seq cmd: noise mode off (odd voices only).
u8* func_8003D3A4(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    if (pAudioElements->voice_number & 0x1) {
        pAudioElements->voice_data.flags |= 0x1000;
        pAudioElements->voice_data.modeFlags &= 0xFFEF;
    }
    return pScript;
}

// Seq cmd 0xB4: set the noise clock; flag the voice for noise mode.
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; matching build
 * keeps INCLUDE_ASM below. Residual: non-semantic codegen micro-shape
 * (commutative operand order / delay-slot copy placement / register reuse);
 * ops+offsets audited 1:1 against the split asm. */
u8* func_8003D3D8(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    AudioElement* el = pAudioElements;
    *(u16*)&pAudioManager->unk_0x1c[0] = pScript[0];
    SpuSetNoiseClock(*(u16*)&pAudioManager->unk_0x1c[0]);
    pScript++;
    el->voice_data.flags |= 0x2000;
    el->voice_data.modeFlags |= 0x20;
    return pScript;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003D3D8);
#endif

// Seq cmd 0xB5: nudge the noise clock (mod 64); flag the voice for noise.
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; matching build
 * keeps INCLUDE_ASM below. Residual: non-semantic codegen micro-shape
 * (commutative operand order / delay-slot copy placement / register reuse);
 * ops+offsets audited 1:1 against the split asm. */
u8* func_8003D438(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    AudioElement* el = pAudioElements;
    *(u16*)&pAudioManager->unk_0x1c[0] =
        (pScript[0] + *(u16*)&pAudioManager->unk_0x1c[0]) & 0x3F;
    SpuSetNoiseClock(*(u16*)&pAudioManager->unk_0x1c[0]);
    pScript++;
    el->voice_data.flags |= 0x2000;
    el->voice_data.modeFlags |= 0x20;
    return pScript;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003D438);
#endif

u8* SoundScriptSetVoiceFlags2000AndMode(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    pAudioElements->voice_data.flags |= 0x2000;
    pAudioElements->voice_data.modeFlags |= 0x20;
    return pScript;
}

u8* SoundScriptSetVoiceFlags2000ClearMode(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    pAudioElements->voice_data.flags |= 0x2000;
    pAudioElements->voice_data.modeFlags &= ~0x20;
    return pScript;
}

// Seq cmd: reverb mode + depths -- stash in the manager, then apply via
// SoundSetReverbModeWithAllocation (auto work-area).
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; matching build
 * keeps INCLUDE_ASM below. Residual: non-semantic codegen shape (load-reload
 * elision / register-copy / arg-setup scheduling); ops+offsets audited 1:1
 * against the split asm and exercised via the synthetic stream. */
u8* func_8003D4E4(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8 mode = pScript[0];
    s8 depthL;
    s8 depthR;
    *(u16*)&pAudioManager->unk_0x40[4] = mode << 8;
    depthL = pScript[1];
    pAudioManager->unk_0x40[2] = depthL;
    depthR = pScript[2];
    pAudioManager->unk_0x40[3] = depthR;
    SoundSetReverbModeWithAllocation(-1, (s8)mode << 8, depthL, depthR);
    return pScript + 3;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003D4E4);
#endif

// Seq cmd 0xBA: conditional reverb-voice enable -- always when the manager
// isn't SFX-class; for SFX only when globally allowed and not flagged off.
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; matching build
 * keeps INCLUDE_ASM below. Residual: non-semantic codegen micro-shape
 * (commutative operand order / delay-slot copy placement / register reuse);
 * ops+offsets audited 1:1 against the split asm. */
u8* func_8003D53C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8* ip = pScript;
    if (!(pAudioManager->unk_Flags & 0x6) ||
        ((g_SoundControlFlags & 0x2000) && !(pAudioElements->active_flag & 0x2))) {
        pAudioElements->voice_data.flags |= 0x4000;
        pAudioElements->voice_data.modeFlags |= 0x40;
    }
    return ip;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003D53C);
#endif

u8* SoundScriptSetVoiceFlags4000ClearMode(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    pAudioElements->voice_data.flags |= 0x4000;
    pAudioElements->voice_data.modeFlags &= ~0x40;
    return pScript;
}

int SoundScriptSkip3(int arg0) {
    return arg0 + 3;
}

s32 SoundScriptNop(s32 arg0) {
    return arg0;
}

s32 SoundScriptNop2(s32 arg0) {
    return arg0;
}

// ADSR / Envelope Reset Handler?
u8* SoundScriptCallE5BC(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    func_8003E5BC(pAudioElements->unk_0x26, pAudioElements);
    return pScript;
}

// Seq cmd: set raw ADSR attack/decay/sustain fields.
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; matching build
 * keeps INCLUDE_ASM below. Residual: load-scheduling cluster placement (same
 * ops, same offsets). */
u8* func_8003D60C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    pAudioElements->voice_data.unkAdsr1 = pScript[0];
    pAudioElements->voice_data.unkAdsr2 = pScript[1];
    pAudioElements->voice_data.flags |= 0x1F0;
    pAudioElements->voice_data.unkAdsr3 = pScript[2];
    return pScript + 3;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003D60C);
#endif

u8* SoundScriptSetAttackTime(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8 attackTime = *pScript;
    pAudioElements->voice_data.flags |= 0x10;
    pAudioElements->voice_data.unkAdsr4 = attackTime;
    return pScript + 1;
}

u8* SoundScriptSetDecayTime(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8 decayTime = *pScript;
    pAudioElements->voice_data.flags |= 0x20;
    pAudioElements->voice_data.adsrDR = decayTime;
    return pScript + 1;
}

u8* SoundScriptSetSustain(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8 temp = *pScript;
    pAudioElements->voice_data.flags |= 0x40;
    pAudioElements->voice_data.unkAdsr5 = temp;
    return pScript + 1;
}

u8* SoundScriptSetRelease(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8 temp = *pScript;
    pAudioElements->voice_data.flags |= 0x80;
    pAudioElements->unk_0x28 = temp;
    pAudioElements->voice_data.unkAdsr6 = temp;
    return pScript + 1;
}

u8* SoundScriptSetSustainLevel(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8 temp = *pScript;
    pAudioElements->voice_data.flags |= 0x100;
    pAudioElements->voice_data.adsrSR = temp;
    return pScript + 1;
}

// Set decay and sustain?
u8* SoundScriptSetAdsrDRAndSR(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    pAudioElements->voice_data.adsrDR = *pScript;
    pAudioElements->voice_data.adsrSR = *(pScript + 1);
    pAudioElements->voice_data.flags |= 0x120;
    return pScript + 2;
}

u8* SoundScriptSetAttackMode(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8 attackMode = *pScript;
    pAudioElements->voice_data.flags |= 0x10;
    pAudioElements->voice_data.unkAdsr1 = attackMode;
    return pScript + 1;
}

u8* SoundScriptSetSustainMode(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8 sustainMode = *pScript;
    pAudioElements->voice_data.flags |= 0x40;
    pAudioElements->voice_data.unkAdsr2 = sustainMode;
    return pScript + 1;
}

u8* SoundScriptSetReleaseMode(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8 releaseMode = *pScript;
    pAudioElements->voice_data.flags |= 0x80;
    pAudioElements->voice_data.unkAdsr3 = releaseMode;
    return pScript + 1;
}

u8* SoundScriptSetUnk6E(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    s16 temp = (s8)*pScript << 5;
    pAudioElements->unk_0x6E = temp;
    pAudioElements->status_flags |= 0x200;
    return pScript + 1;
}

u8* SoundScriptAddUnk6E(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    pAudioElements->unk_0x6E += (s8)*pScript << 5;
    pAudioElements->status_flags |= 0x200;
    return pScript + 1;
}

u8* func_8003D79C(u8* a0, s32 a1, void* a2) {
    *(u16*)((u8*)a2 + 0x6E) += (s8)a0[0] << 3;
    *(u16*)((u8*)a2 + 0x2) |= 0x200;
    return a0 + 1;
}

// Seq cmd: detune -- add a signed 8.8 offset to the element pitch base.
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; matching build
 * keeps INCLUDE_ASM below. Residual: load-scheduling cluster placement (same
 * ops, same offsets). */
u8* func_8003D7C8(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    s32 offset = ((s16)(pScript[0] << 8)) + pScript[1];
    pAudioElements->status_flags |= 0x200;
    *(u16*)&pAudioElements->unk_0x6E = *(u16*)&pAudioElements->unk_0x6E + offset;
    return pScript + 2;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003D7C8);
#endif

// Seq cmd: pitch slide -- explicit step target over n steps (or disable).
u8* func_8003D7FC(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u16 n = pScript[0];
    s32 step = (s8)pScript[1] << 24;
    if (n != 0 && step != 0) {
        u16 f4;
        f4 = pAudioElements->unk_0x04;
        *(u16*)&pAudioElements->unk_0x76[0x1E] = n;
        pAudioElements->unk_0x04 = f4 | 0x1;
        *(s32*)&pAudioElements->unk_0x76[0xE] = step / n;
    } else {
        pAudioElements->unk_0x04 &= 0xFFFE;
    }
    return pScript + 2;
}

u8* SoundScriptToggleUnk04Bit2(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    pAudioElements->unk_0x04 ^= 0x2;
    return pScript;
}

u8* SoundScriptClearUnk04Bit1(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    pAudioElements->unk_0x04 &= ~0x1;
    return pScript;
}

// Seq cmd: portamento divisor -- nonzero enables portamento, zero disables.
u8* func_8003D884(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8 divisor = *pScript++;
    *(u16*)&pAudioElements->unk_0x70[0] = divisor;
    if (divisor != 0) {
        pAudioElements->unk_0x04 |= 0x4;
    } else {
        pAudioElements->unk_0x04 &= 0xFFFB;
    }
    return pScript;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003D8B8);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003D9A4);

// Seq cmd: envelope 0 rate -- step = 0x400 / ((n+1)*4).
u8* func_8003DAB0(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8 n = *pScript++ + 1;
    if (n != 0) {
        u16 step = 0x400 / (n << 2);
        *(u16*)&pAudioElements->unk_0xD0[0x22] = step;
        *(u16*)&pAudioElements->unk_0xD0[0x20] = step;
    }
    return pScript;
}

u8* SoundScriptSetUnkCEAndF6(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    pAudioElements->unk_0xCE |= 0x1;
    pAudioElements->unk_0xF6 |= 0x1;
    return pScript;
}

u8* SoundScriptClearUnkCEAndF6(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    pAudioElements->unk_0xCE &= ~0x1;
    pAudioElements->unk_0xF6 &= ~0x1;
    return pScript;
}

// Set volume handler / Dynamic?
// Seq cmd: set vibrato accumulator (byte << 24), drop pitch-env bits, mark
// volume change.
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; matching build
 * keeps INCLUDE_ASM below. Residual: load-scheduling cluster placement (same
 * ops, same offsets). */
u8* func_8003DB2C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u16 f4;
    u16 status;
    f4 = pAudioElements->unk_0x04;
    *(s32*)&pAudioElements->unk_0x76[2] = pScript[0] << 24;
    status = pAudioElements->status_flags;
    pAudioElements->unk_0x04 = f4 & 0xFEF7;
    pAudioElements->status_flags = status | 0x100;
    return pScript + 1;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003DB2C);
#endif

// Crescendo?
// Seq cmd: nudge the vibrato accumulator by a signed step (clamped positive),
// mark volume change, drop pitch-env bits.
u8* func_8003DB58(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u16 status;
    u16 f4;
    *(s32*)&pAudioElements->unk_0x76[2] =
        (*(s32*)&pAudioElements->unk_0x76[2] + ((s8)pScript[0] << 24)) & 0x7FFFFFFF;
    status = pAudioElements->status_flags;
    f4 = pAudioElements->unk_0x04;
    pAudioElements->status_flags = status | 0x100;
    pAudioElements->unk_0x04 = f4 & 0xFEF7;
    return pScript + 1;
}

// Seq cmd: vibrato depth fade -- step the accumulator toward target<<24
// over n steps.
u8* func_8003DB98(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u16 n = pScript[0];
    s32 diff = ((s8)pScript[1] << 24) - *(s32*)&pAudioElements->unk_0x76[2];
    if (n != 0 && diff != 0) {
        u16 f4;
        f4 = pAudioElements->unk_0x04;
        *(u16*)&pAudioElements->unk_0x76[0x20] = n;
        pAudioElements->unk_0x04 = (f4 | 0x8) & 0xFEFF;
        *(s32*)&pAudioElements->unk_0x76[0x12] = diff / n;
    }
    return pScript + 2;
}

// Seq cmd 0xF8: vibrato ramp -- from/to (signed<<24) over n steps.
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; matching build
 * keeps INCLUDE_ASM below. Residual: non-semantic codegen micro-shape
 * (commutative operand order / delay-slot copy placement / register reuse);
 * ops+offsets audited 1:1 against the split asm. */
u8* func_8003DBE4(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8* p = pScript;
    s32 from = p[0] << 24;
    s32 diff = (p[2] << 24) - from;
    s32 n = p[1];
    if (diff != 0 && n != 0) {
        u16 f4;
        f4 = pAudioElements->unk_0x04;
        *(u16*)&pAudioElements->unk_0x76[0xC] = from >> 16;
        *(u16*)&pAudioElements->unk_0x76[0xA] = n;
        pAudioElements->unk_0x04 = (f4 | 0x100) & 0xFFF7;
        *(s32*)&pAudioElements->unk_0x76[0x6] = diff / n;
    } else {
        pAudioElements->unk_0x04 &= 0xFEFF;
    }
    return p + 3;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003DBE4);
#endif

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003DC50);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003DD24);

// Seq cmd: envelope 1 rate -- step = 0x400 / ((n+1)*4).
u8* func_8003DE18(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8 n = *pScript++ + 1;
    if (n != 0) {
        u16 step = 0x400 / (n << 2);
        *(u16*)&pAudioElements->unk_0xF8[0x1A] = step;
        *(u16*)&pAudioElements->unk_0xF8[0x18] = step;
    }
    return pScript;
}

u8* SoundScriptSetUnkCEAndUnk116(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    pAudioElements->unk_0xCE |= 0x2;
    pAudioElements->unk_0x116 |= 0x1;
    return pScript;
}

u8* SoundScriptClearUnkCEAndUnk116(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    pAudioElements->unk_0xCE &= ~0x2;
    pAudioElements->unk_0x116 &= ~0x1;
    return pScript;
}

// Pan / Balance handler?
u8* SoundScriptSetUnk74(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    pAudioElements->unk_0x74 = *pScript << 8;
    pAudioElements->status_flags |= 0x100;
    return pScript + 1;
}

// Seq cmd: nudge pan by a signed step (clamped to 15 bits).
u8* func_8003DEB4(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    pAudioElements->unk_0x74 = (pAudioElements->unk_0x74 + ((s8)pScript[0] << 8)) & 0x7FFF;
    pAudioElements->status_flags |= 0x100;
    return pScript + 1;
}

// Seq cmd: pan fade -- step pan toward the signed target over n steps.
u8* func_8003DEE4(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u16 n = pScript[0];
    s32 diff = (s8)pScript[1] - (((s16)pAudioElements->unk_0x74) >> 8);
    if (n != 0 && diff != 0) {
        u16 f4;
        s32 scaled = diff << 8;
        f4 = pAudioElements->unk_0x04;
        *(u16*)&pAudioElements->unk_0x76[0x1C] = scaled;
        *(u16*)&pAudioElements->unk_0x76[0x22] = n;
        pAudioElements->unk_0x04 = f4 | 0x10;
        *(u16*)&pAudioElements->unk_0x76[0x1A] = scaled / n;
    }
    return pScript + 2;
}

// Seq cmd: envelope 2 rate -- step = 0x400 / ((n+1)*4).
u8* func_8003DF3C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8 n = *pScript++ + 1;
    if (n != 0) {
        u16 step = 0x400 / (n << 2);
        *(u16*)&pAudioElements->unk_0x118[0x1A] = step;
        *(u16*)&pAudioElements->unk_0x118[0x18] = step;
    }
    return pScript;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003DF78);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003E04C);

u8* SoundScriptSetUnkCEAndUnk136(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    pAudioElements->unk_0xCE |= 0x4;
    pAudioElements->unk_0x136 |= 0x1;
    return pScript;
}

s32 func_8003E160(s32 a0, s32 a1, u16* a2) {
    a2[0x67] &= 0xFFFB;
    a2[0x9B] &= 0xFFFE;
    return a0;
}

// Seq cmd 0xF0: define envelope object -- select slot, bind the handler
// method from the D_800508A4 table (retail PSX address; the port's host
// routing for these lands with the envelope pass), set mode/state.
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; matching build
 * keeps INCLUDE_ASM below. Residual: non-semantic codegen micro-shape
 * (commutative operand order / delay-slot copy placement / register reuse);
 * ops+offsets audited 1:1 against the split asm. */
u8* func_8003E180(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    SoundEnvelope* env;
    *(u16*)&pAudioElements->unk_0x76[0x56] = pScript[0];
    env = (SoundEnvelope*)((u8*)pAudioElements +
                           ((*(u16*)&pAudioElements->unk_0x76[0x56] << 5) + 0xD8));
    {
        u8 m = pScript[1];
        env->unk1D = m & 0xF;
        env->pfnHandler = D_800508A4[env->unk1D];
        if (!(m & 0x10)) {
            env->flags = 0x2;
        } else {
            env->flags = 0;
        }
    }
    env->stepAdd = 0x400;
    *(u16*)&env->unk16[0] = 0;
    env->state = pScript[2];
    return pScript + 3;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003E180);
#endif

// Seq cmd 0xF1: envelope target/rate -- rate = n + n*n/64; packed target via
// func_8003E290 keyed by the envelope's method index.
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; matching build
 * keeps INCLUDE_ASM below. Residual: non-semantic codegen micro-shape
 * (commutative operand order / delay-slot copy placement / register reuse);
 * ops+offsets audited 1:1 against the split asm. */
u8* func_8003E1F8(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    SoundEnvelope* env = (SoundEnvelope*)((u8*)pAudioElements +
                         ((*(u16*)&pAudioElements->unk_0x76[0x56] << 5) + 0xD8));
    s32 sq = pScript[0] * pScript[0];
    u16 rate = pScript[0] + sq / 64;
    *(s32*)&env->unk4[0x8] = func_8003E290(((s8)pScript[1] << 24) | (pScript[2] << 16),
                                           env->unk1D);
    *(u16*)&env->unk4[0xE] = rate;
    return pScript + 3;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003E1F8);
#endif

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003E290);

// Seq cmd: envelope-object rate config (selected slot): delay = p0*4,
// step/stepAdd = 0x400 / ((p1+1)*4).
u8* func_8003E308(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    SoundEnvelope* env;
    u8 n = pScript[1] + 1;
    env = (SoundEnvelope*)((u8*)pAudioElements +
                           ((*(u16*)&pAudioElements->unk_0x76[0x56] << 5) + 0xD8));
    if (n != 0) {
        u16 rate = 0x400 / (n << 2);
        *(u16*)&env->unk16[0] = pScript[0] << 2;
        env->stepAdd = rate;
        env->step = rate;
    }
    return pScript + 2;
}

u8* SoundScriptNop5(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    return pScript;
}

// Seq cmd 0xF6: arm envelope object N (prime via func_8003E3E0, set run flag
// + the element's active-envelope bit).
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; matching build
 * keeps INCLUDE_ASM below. Residual: non-semantic codegen micro-shape
 * (commutative operand order / delay-slot copy placement / register reuse);
 * ops+offsets audited 1:1 against the split asm. */
u8* func_8003E360(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8 n = pScript[0];
    SoundEnvelope* env = (SoundEnvelope*)((u8*)pAudioElements + ((n << 5) + 0xD8));
    func_8003E3E0(env);
    pScript++;
    env->flags |= 0x1;
    pAudioElements->unk_0xCE |= 1 << n;
    return pScript;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003E360);
#endif

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003E3E0);

// Seq cmd: disarm envelope object N -- clear its run flag + its bit in the
// element's active-envelope mask.
u8* func_8003E40C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u32 n = pScript[0];
    *(u16*)((u8*)pAudioElements + (n << 5) + 0xF6) &= 0xFFFE;
    pAudioElements->unk_0xCE &= ~(1 << n);
    return pScript + 1;
}

// Seq cmd 0xFC: select sample bank + load instrument in one command.
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; matching build
 * keeps INCLUDE_ASM below. Residual: non-semantic codegen micro-shape
 * (commutative operand order / delay-slot copy placement / register reuse);
 * ops+offsets audited 1:1 against the split asm. */
u8* func_8003E44C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8* p = pScript;
    AudioElement* el = pAudioElements;
    SoundWDSEntry* e;
    u8 inst = p[1];
    ((u8*)&el->unk_0x24)[1] = p[0];
    e = SoundFindWdsEntry(p[0]);
    if (e == NULL) {
        e = g_SoundWdsLinkedList;
    }
    el->unk2C = SOUND_PTR_TO_PSX(e);
    func_8003E5BC(inst, el);
    return p + 2;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003E44C);
#endif

// Seq cmd: master level (manager interpolator immediate).
u8* func_8003E4BC(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u32 level = *pScript++;
    if (level != 0) {
        pAudioManager->unk_0x54 = ((s16*)&pAudioManager->unk_0x58)[1] * (level << 8);
        pAudioManager->unk_Interpolator_0x64.currentValue = level << 24;
    }
    return pScript;
}

// Seq cmd: select sample bank -- find the WDS entry by id (fall back to the
// list head) and point the element's bank pointer at it.
u8* func_8003E4F0(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    SoundWDSEntry* e;
    u32 id = *pScript;
    pScript++;
    ((u8*)&pAudioElements->unk_0x24)[1] = id;
    e = SoundFindWdsEntry(id);
    if (e == NULL) {
        e = g_SoundWdsLinkedList;
    }
    pAudioElements->unk2C = SOUND_PTR_TO_PSX(e);
    return pScript;
}

// Seq cmd 0xFF: release the voice if its envelope has fully decayed.
u8* func_8003E54C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    long keyStat;
    short envx;
    SpuGetVoiceEnvelopeAttr(pAudioElements->voice_number, &keyStat, &envx);
    if (envx == 0) {
        pAudioElements->status_flags &= 0xFFFC;
        SoundReleaseVoiceFromChannel(&pAudioElements->voice_data,
                                     pAudioElements->voice_number);
        pAudioElements->active_flag = 0;
    }
    return pScript;
}

// Load instrument #instrument from the element's bank (unk2C) into its voice
// data: sample/loop SPU addresses, unpacked ADSR fields, program number.
// Marks the element CHANGE_INSTRUMENT and latches the instrument pitch base.
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; the matching build
 * keeps INCLUDE_ASM (byte-exact) below. Semantics traced 1:1 against the asm.
 * Residual vs {}: retail leaves the load-delay slot after the second ADSR-word
 * load unfilled (nop) and keeps the active_flag RMW at the statement tail;
 * this pipeline's cc1 lifts the RMW cluster into that latency hole (same ops,
 * same registers, 3 instructions placed ~14 slots earlier). Six source-shape
 * variants (RMW split, statement reorder, shared-temp) all canonicalize to the
 * hoisted schedule. Not claimed as {}. */
void func_8003E5BC(s32 instrument, AudioElement* pAudioElements) {
    u8* pBank;
    u8* pInstr;
    u32 adsr;
    u32 num;
    s32 base;
    u16 activeFlag;

    pAudioElements->unk_0x26 = instrument;
    pBank = SOUND_PSX_TO_PTR(u8, pAudioElements->unk2C);
    pInstr = pBank + (((s16)instrument << 4) + 0x30);
    base = *(s32*)pInstr << 3;
    pAudioElements->voice_data.startAddress = base + *(u32*)(pBank + 0x28);
    pAudioElements->voice_data.loopAddress = base + (*(u16*)(pInstr + 4) << 3);
    adsr = *(u16*)(pInstr + 0xC);
    pAudioElements->voice_data.unkAdsr1 = adsr & 0x7;
    pAudioElements->voice_data.unkAdsr2 = (adsr >> 4) & 0x7;
    pAudioElements->voice_data.unkAdsr3 = (adsr >> 8) & 0x7;
    adsr = *(u32*)(pInstr + 8);
    num = adsr >> 24;
    pAudioElements->voice_data.unkAdsr4 = adsr & 0x7F;
    pAudioElements->voice_data.adsrDR = (adsr >> 8) & 0xF;
    pAudioElements->voice_data.unkAdsr5 = (adsr >> 16) & 0x7F;
    pAudioElements->voice_data.adsrSR = (adsr >> 12) & 0xF;
    activeFlag = pAudioElements->active_flag;
    num &= 0x1F;
    pAudioElements->unk_0x28 = num;
    pAudioElements->voice_data.unkAdsr6 = num;
    pAudioElements->active_flag = activeFlag | 0x8000;
    pAudioElements->unk_0x6C = *(u16*)(pInstr + 6);
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003E5BC);
#endif

//----------------------------------------------------------------------------------------------------------------------
void unk_SoundSetFlagsOnActiveVoices(u16 flags, AudioManager* manager) {
    AudioElement* pElement;
    u32 cnt;

    pElement = &manager->elements[0];
    cnt = manager->elementCount;

    do {
        if (pElement->active_flag) {
            pElement->status_flags = flags | pElement->status_flags;
        }
        pElement++;
        cnt--;
    } while (cnt);
}

//----------------------------------------------------------------------------------------------------------------------
void SoundSetFlagsOnActiveVoices(AudioManager* manager, s32 flags) {
    AudioElement* pElement;
    u32 cnt;

    pElement = &manager->elements[0];
    cnt = manager->elementCount;

    do {

        if (pElement->active_flag) {
            // this doesn't feel right, considering SoundVoiceData::flags is a 16-bit flag
            // I know that SoundVoiceData::volume is 16-bits from SoundVoiceData::flags from func_8003E900
            // but I get a reg swap if flags is 16 bits
            pElement->voice_data.flags |= (u16)flags;
        }

        pElement++;
        cnt--;
    } while (cnt);
}

//----------------------------------------------------------------------------------------------------------------------
void SoundClearVoiceDataPointers(void) {
    s32 offset = sizeof(SoundVoiceData*) * (NUM_VOICES - 1);
    while( offset >= 0 ) {
        *(u32*)((u8*)&g_SoundChannels + offset) = NULL;
        offset -= sizeof(SoundVoiceData*);
    }
}

//----------------------------------------------------------------------------------------------------------------------
void SoundAssignVoiceToChannelAndStop(SoundVoiceData* voiceData, u32 channelIndex) {

    SoundVoiceData* currentVoice;
    SoundVoiceData** pChannel;

    pChannel = &g_SoundChannels[channelIndex];
    if (channelIndex < NUM_VOICES) {
        currentVoice = *pChannel;

        // Mark current voice as needing update
        if (currentVoice == voiceData) {
            g_unk_VoicesNeedingProcessing = (1 << channelIndex) | g_unk_VoicesNeedingProcessing;
            return;
        }

        // Do not steal a higher priority voices
        if (currentVoice && currentVoice->priority > voiceData->priority) {
            return;
        }

        // Assign voice to channel
        voiceData->flags = 0xFFFF;
        voiceData->assignedVoice = channelIndex;
        g_SoundChannels[channelIndex] = voiceData;

        // Mark for voice processing
        g_unk_VoicesNeedingProcessing = (1 << channelIndex) | g_unk_VoicesNeedingProcessing;

        // Stop any current key ons for this channel
        g_SoundKeyOnFlags = ~(1 << channelIndex) & g_SoundKeyOnFlags;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void SoundAssignVoiceToChannel(SoundVoiceData* voiceData, u32 channelIndex) {
    SoundVoiceData* currentVoice;
    SoundVoiceData** channelPtr;

    channelPtr = &g_SoundChannels[channelIndex];
    if (channelIndex < NUM_VOICES) {
        currentVoice = *channelPtr;

        if (currentVoice == voiceData || (currentVoice && currentVoice->priority > voiceData->priority)) {
            return;
        }

        voiceData->assignedVoice = channelIndex;
        *channelPtr = voiceData;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void SoundReleaseVoiceFromChannel(SoundVoiceData* voiceData, uint channelIndex)
{
    SoundVoiceData** channelPtr;
    uint channelBitMask;

    channelPtr = &g_SoundChannels[channelIndex];
    if (channelIndex < NUM_VOICES) {
        if (*channelPtr == voiceData) {
            *channelPtr = NULL;
            channelBitMask = 1 << channelIndex;
            g_unk_VoicesNeedingProcessing = channelBitMask | g_unk_VoicesNeedingProcessing;

            // Clear key-on flag to prevent playback
            g_SoundKeyOnFlags = ~channelBitMask & g_SoundKeyOnFlags;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
void SoundAbortVoiceOnChannel(SoundVoiceData* voiceData, u32 channelIndex) {

    if ((channelIndex < NUM_VOICES) && (g_SoundChannels[channelIndex] == voiceData)) {
        g_unk_VoicesNeedingProcessing = (1 << channelIndex) | g_unk_VoicesNeedingProcessing;
        g_SoundKeyOnFlags = ~(1 << channelIndex) & g_SoundKeyOnFlags;
    }
}

// Every-tick voice register flush: each assigned channel's dirty-flag word
// (SoundVoiceData::flags) selects which SPU voice registers to rewrite
// (volume/pitch/addresses/ADSR nibbles). Mode-bit masks (FM/noise/reverb) are
// accumulated over all voices and written when a voice requested them; pending
// key-ons are committed last.
void func_8003E900(void) {
    SoundVoiceData** ppChannel;
    u32 revMask;
    u32 noiseMask;
    u32 fmMask;
    u16 accFlags;
    SPU_VOICE_REG* pReg;
    volatile u16* pAdsr;
    u32 keyOn;
    s32 i;

    ppChannel = &g_SoundChannels[0];
    revMask = 0;
    noiseMask = 0;
    fmMask = 0;
    accFlags = 0;
    /* Hardware view: the ADSR-anchored cursor walks the volatile register
     * image (no CSE/reordering of the RMWs); volume.left rides the plain
     * voice-struct cursor. Both consume the loaded register pointer, so the
     * post-loop flag writes reload g_pSoundSpuRegisters. */
    pReg = &g_pSoundSpuRegisters->_rxx.voice[0];
    i = 0;
    pAdsr = &g_pSoundSpuRegisters->rxx.voice[0].adsr[0];
    for (; i < NUM_VOICES; pAdsr += 8, pReg++, i++, ppChannel++) {
        SoundVoiceData* pVoice = *ppChannel;
        if (pVoice != NULL) {
            u16 flags = pVoice->flags;
            if (flags != 0) {
                u16 val;
                if (flags & 0x1) {
                    pReg->volume.left = pVoice->volume.left;
                    pAdsr[-3] = pVoice->volume.right;
                }
                if (flags & 0x4) {
                    pAdsr[-2] = pVoice->pitch;
                }
                if (flags & 0x8) {
                    pAdsr[-1] = pVoice->startAddress >> 3;
                    pAdsr[3] = pVoice->loopAddress >> 3;
                }
                if (flags & 0x10) {
                    val = *(volatile u8*)pAdsr;
                    val = val + (pVoice->unkAdsr4 << 8) + ((pVoice->unkAdsr1 >> 2) << 15);
                    pAdsr[0] = val;
                }
                if (flags & 0x20) {
                    val = pAdsr[0];
                    val = (val & 0xFF0F) + (pVoice->adsrDR << 4);
                    pAdsr[0] = val;
                }
                if (flags & 0x40) {
                    val = pAdsr[1];
                    val = (val & 0x3F) + (pVoice->unkAdsr5 << 6) +
                          ((pVoice->unkAdsr2 >> 1) << 14);
                    pAdsr[1] = val;
                }
                if (flags & 0x80) {
                    val = pAdsr[1];
                    val = (val & 0xFFC0) +
                          (pVoice->unkAdsr6 + ((pVoice->unkAdsr3 >> 2) << 5));
                    pAdsr[1] = val;
                }
                if (flags & 0x100) {
                    val = pAdsr[0];
                    val = pVoice->adsrSR + (val & 0xFFF0);
                    pAdsr[0] = val;
                }
                accFlags |= flags & 0x7000;
                pVoice->flags = 0;
            }
            fmMask |= ((pVoice->modeFlags >> 4) & 1) << i;
            noiseMask |= ((pVoice->modeFlags >> 5) & 1) << i;
            revMask |= ((pVoice->modeFlags >> 6) & 1) << i;
        }
    }
    /* The cursor register is re-pointed at the union base for the tail
     * writes (retail reuses the same variable for both roles). */
    pReg = &g_pSoundSpuRegisters->_rxx.voice[0];
    if (accFlags) {
        if (accFlags & 0x1000) {
            ((SpuUnion*)pReg)->_rxx.chan_fm[0] = fmMask;
            ((SpuUnion*)pReg)->_rxx.chan_fm[1] = fmMask >> 16;
        }
        if (accFlags & 0x2000) {
            ((SpuUnion*)pReg)->_rxx.noise_mode[0] = noiseMask;
            ((SpuUnion*)pReg)->_rxx.noise_mode[1] = noiseMask >> 16;
        }
        if (accFlags & 0x4000) {
            ((SpuUnion*)pReg)->_rxx.rev_mode[0] = revMask;
            ((SpuUnion*)pReg)->_rxx.rev_mode[1] = revMask >> 16;
        }
    }
    keyOn = g_SoundKeyOnFlags;
    if (keyOn != 0) {
        ((SpuUnion*)pReg)->_rxx.key_on[0] = keyOn;
        ((SpuUnion*)pReg)->_rxx.key_on[1] = keyOn >> 16;
        g_SoundKeyOnFlags = 0;
    }
}

// Every-tick voice release/key-off flush: newly reassigned voices
// (g_unk_VoicesNeedingProcessing) get their release rate forced, then both
// pending masks are written to the SPU KOFF registers and cleared.
void func_8003EB5C(void) {
    u32 pending;
    SpuUnion* spu;
    s32 i;

    pending = g_unk_VoicesNeedingProcessing;
    spu = g_pSoundSpuRegisters;
    if (pending) {
        u32 bit;
        u16* pAdsr1;
        i = 0;
        bit = 1;
        pAdsr1 = &spu->_rxx.voice[0].adsr[1];
        for (; i < NUM_VOICES; i++, pAdsr1 += 8) {
            if (pending & (bit << i)) {
                *pAdsr1 = (*pAdsr1 & 0xFFC0) | 0x6;
            }
        }
    }
    pending = g_SoundKeyOffFlags | g_unk_VoicesNeedingProcessing;
    if (pending) {
        spu->_rxx.key_off[0] = pending;
        spu->_rxx.key_off[1] = pending >> 16;
        g_unk_VoicesNeedingProcessing = 0;
        g_SoundKeyOffFlags = 0;
    }
}

// Voice-apply pass (final tick pass, per element): fold the master/envelope
// volumes and the pan law into the SPU volume pair, convert the accumulated
// note (+envelope pitch bend +manager bend) into an SPU pitch via
// func_8003EEA0, then issue the pending key-on/key-off. Skipped entirely when
// the manager is muted (flag 0x20).
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; the matching
 * build keeps INCLUDE_ASM (byte-exact) below. Residual vs {}: this pipeline's
 * cc1 strength-reduces the element cursor family onto a different anchor and
 * fills load-latency slack the retail object leaves unfilled -- same
 * operations at the same absolute element offsets, rebased registers/
 * displacements. Semantics traced 1:1 against the split asm. Not claimed
 * as {}. */
void func_8003EBF0(AudioManager* manager, AudioElement* pAudioElements, s32 count) {
    u16* st;

    if (!(manager->unk_Flags & 0x20)) {
        st = &pAudioElements->status_flags;
        do {
            if (pAudioElements->active_flag) {
                u16 status = st[0];
                if (status & 0x100) {
                    s32 vol;
                    s32 pan;
                    s32 left;
                    s32 right;
                    vol = *(s16*)&st[0x3C] -
                          (((*(s16*)&st[0x3C]) * (*(s16*)&st[0x68])) >> 15);
                    if (vol > 0x7FFF) {
                        vol = 0x7FFF;
                    }
                    if (vol < 0) {
                        vol = 0;
                    }
                    vol = ((*(s16*)&st[0x3A]) * vol) >> 15;
                    vol = (((s16*)&manager->unk_Interpolator_0x70.currentValue)[1] * vol) >> 16;
                    pan = *(s16*)&st[0x39] + *(s16*)&st[0x69] +
                          ((s16*)&manager->unk_Interpolator_0x88.currentValue)[1];
                    if (pan > 0x7F00) {
                        pan = 0x7F00;
                    }
                    if (pan < 0) {
                        pan = 0;
                    }
                    if (g_SoundControlFlags & 0x100) {
                        if (pan < 0x4000) {
                            right = (pan * 0x5A00) >> 14;
                            left = 0x7F00 - ((pan * 0x2500) >> 14);
                        } else {
                            pan = 0x8000 - pan;
                            left = (pan * 0x5A00) >> 14;
                            right = 0x7F00 - ((pan * 0x2500) >> 14);
                        }
                        left = (left * vol) >> 15;
                        right = (right * vol) >> 15;
                    } else {
                        right = (vol * 0x5A00) >> 15;
                        left = right;
                    }
                    st[0x1A] |= 0x1;
                    st[0x1B] = left;
                    st[0x1C] = right;
                }
                if (status & 0x200) {
                    s16 pitch = func_8003EEA0(
                        (s16)(*(s16*)&st[0x34] + *(s16*)&st[0x67] +
                              ((s16*)&manager->unk_Interpolator_0x7c.currentValue)[1]));
                    st[0x21] = pitch & 0x3FFF;
                    st[0x1A] |= 0x4;
                }
                if ((status & 0x1) && !(pAudioElements->active_flag & 0x20)) {
                    SoundAssignVoiceToChannelAndPlay(&pAudioElements->voice_data,
                                                     ((u8*)&st[0x12])[1]);
                }
                if (status & 0x2) {
                    SoundStopVoiceOnChannel(&pAudioElements->voice_data, ((u8*)&st[0x12])[1]);
                }
                st[0] = 0;
            }
            st += 0xAC;
            count--;
            pAudioElements++;
        } while ((s16)count != 0);
    }
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003EBF0);
#endif

// Convert a note number (semitone in the high byte-ish 15-bit form) to an SPU
// pitch: octave table lookup + per-note pitch table + octave shift.
s16 func_8003EEA0(u32 note) {
    s32 entry;
    s32 pitch;
    s32 shift;

    entry = D_80050B78[(note & 0x7FFF) >> 8];
    pitch = D_80050BF0[(note & 0xFF) + ((entry & 0xF) << 8)];
    shift = 6 - (entry >> 4);
    if (shift < 0) {
        pitch = pitch << -shift;
    } else {
        pitch = pitch >> shift;
    }
    return pitch;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundAssignVoiceToChannelAndPlay(SoundVoiceData* voiceData, u32 channelIndex)
{
    SoundVoiceData* currentVoice;
    SoundVoiceData** pChannel;

    pChannel = &g_SoundChannels[channelIndex];
    if (channelIndex < NUM_VOICES) {
        currentVoice = *pChannel;

        // Skip assignment if voice already assigned
        if (currentVoice != voiceData) {
            if (currentVoice && currentVoice->priority > voiceData->priority) {
                return;
            }

            // Assign voice to channel
            voiceData->flags = 0xFFFF;
            voiceData->assignedVoice = channelIndex;
            g_SoundChannels[channelIndex] = voiceData;

            // Mark for voice processing
            g_unk_VoicesNeedingProcessing = (1 << channelIndex) | g_unk_VoicesNeedingProcessing;
        }

        // Always trigger playback on this channel
        g_SoundKeyOnFlags = (1 << channelIndex) | g_SoundKeyOnFlags;
    }
}

void SoundStopVoiceOnChannel(SoundVoiceData* voiceData, u32 channelIndex) {

    if( channelIndex < NUM_VOICES && g_SoundChannels[channelIndex] == voiceData ) {

        g_SoundKeyOffFlags = (1 << channelIndex) | g_SoundKeyOffFlags;
    }
}



// Envelope pass (second tick pass, per element): clear the per-axis envelope
// accumulators (element+0xD0/D2/D4), then for each of the 4 envelope objects
// whose flag bit 0 is set either count down its delay or invoke its handler
// method and accumulate the scaled result into the pitch (state 0) or
// volume/pan (states 1/2) accumulator, marking the matching status bit.
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; the matching
 * build keeps INCLUDE_ASM (byte-exact) below. Residual vs {}: this pipeline's
 * cc1 strength-reduces the element cursor family onto a different anchor and
 * fills load-latency slack the retail object leaves unfilled -- same
 * operations at the same absolute element offsets, rebased registers/
 * displacements. Semantics traced 1:1 against the split asm. Not claimed
 * as {}. */
void func_8003EFE4(AudioManager* manager, AudioElement* pAudioElements, s32 count) {
    u16* st;

    st = &pAudioElements->status_flags;
    do {
        if (pAudioElements->active_flag) {
            u16 nActive = st[0x66];
            st[0x69] = 0;
            st[0x68] = 0;
            st[0x67] = 0;
            if (nActive != 0) {
                s32 slot;
                SoundEnvelope* env;
                u16 status;
                u16* pSt;
                slot = 4;
                env = (SoundEnvelope*)((u8*)pAudioElements + 0xD8);
                status = st[0];
                pSt = (u16*)((u8*)pAudioElements + 0xF4);
                do {
                    if (pSt[1] & 0x1) {
                        if (pSt[-4] != 0) {
                            pSt[-4] = pSt[-4] - 1;
                        } else {
                            s32 value;
                            s32 step;
                            value = ((s32 (*)())SOUND_PSX_TO_PTR(void, env->pfnHandler))(env);
                            step = *(s16*)&pSt[-2];
                            if (step < 0x400) {
                                s32 scaled = (value >> 10) * step;
                                pSt[-2] = step + pSt[-1];
                                value = scaled;
                            }
                            value >>= 16;
                            switch (*(u8*)pSt) {
                                case 0:
                                    st[0x67] += value;
                                    status |= 0x200;
                                    break;
                                case 1:
                                    st[0x68] += value;
                                    status |= 0x100;
                                    break;
                                case 2:
                                    st[0x69] += value;
                                    status |= 0x100;
                                    break;
                            }
                        }
                    }
                    pSt += 0x10;
                    slot--;
                    env++;
                } while (slot != 0);
                st[0] = status;
            }
        }
        st += 0xAC;
        count--;
        pAudioElements++;
    } while ((s16)count != 0);
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003EFE4);
#endif

void func_8003F190(u16* arg0) {
    arg0[0xF] &= 0xFFFE;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003F1A4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003F1EC);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003F240);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003F2A0);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003F308);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003F354);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003F3C0);

extern s32 D_800594E4;

void func_8003F42C(s32 arg0) {
    D_800594E4 = arg0;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003F43C);

//----------------------------------------------------------------------------------------------------------------------
void SoundSetVoiceKeyOn(u32 voiceFlags) {
    g_pSoundSpuRegisters->_rxx.key_on[0] = voiceFlags;
    g_pSoundSpuRegisters->_rxx.key_on[1] = (u16)(voiceFlags >> 0x10);
}

//----------------------------------------------------------------------------------------------------------------------
void SoundSetVoiceKeyOff(u32 voiceFlags) {
    g_pSoundSpuRegisters->_rxx.key_off[0] = voiceFlags;
    g_pSoundSpuRegisters->_rxx.key_off[1] = (u16)(voiceFlags >> 0x10);
}

//----------------------------------------------------------------------------------------------------------------------
void SoundSetReverbVoices(u32 voiceFlags) {
    g_pSoundSpuRegisters->_rxx.rev_mode[0] = voiceFlags;
    g_pSoundSpuRegisters->_rxx.rev_mode[1] = (u16)(voiceFlags >> 0x10);
}

//----------------------------------------------------------------------------------------------------------------------
void SoundUnkDebugNoReturn_8003F4BC(void) {}

//----------------------------------------------------------------------------------------------------------------------
void SoundSetVoiceStartAddress(s32 voiceIndex, s32 addr) {
    SPU_VOICE_REG* voice = &g_pSoundSpuRegisters->_rxx.voice[voiceIndex];
    voice->addr = addr >> 3;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundSetVoiceLoopAddress(s32 voiceIndex, s32 addr) {
    SPU_VOICE_REG* voice = &g_pSoundSpuRegisters->_rxx.voice[voiceIndex];
    voice->loop_addr = addr >> 3;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundSetVoiceVolume(s32 voiceIndex, s32 volL, s32 volR) {
    SPU_VOICE_REG* voice = &g_pSoundSpuRegisters->_rxx.voice[voiceIndex];
    voice->volume.left = volL;
    voice->volume.right = volR;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundSetVoicePitch(s32 voiceIndex, s32 pitch) {
    SPU_VOICE_REG* voice = &g_pSoundSpuRegisters->_rxx.voice[voiceIndex];
    voice->pitch = pitch;
}

//----------------------------------------------------------------------------------------------------------------------
// ADSR Functions
void SoundSetVoiceAdsrAttackModeAndRate(s32 voiceIndex, s32 attackRate, s32 attackModeBit2) {
    SPU_VOICE_REG* voice = &g_pSoundSpuRegisters->_rxx.voice[voiceIndex];
    voice->adsr[0] = (voice->adsr[0] & 0x00FF) +
        (attackRate << 8) +
        ((attackModeBit2 >> 2) << 15);
}

//----------------------------------------------------------------------------------------------------------------------
void SoundSetVoiceAdsrDecayShift(s32 voiceIndex, s32 decayShift) {
    SPU_VOICE_REG* voice = &g_pSoundSpuRegisters->_rxx.voice[voiceIndex];
    voice->adsr[0] = (voice->adsr[0] & 0xFF0F) + (decayShift << 4);
}

//----------------------------------------------------------------------------------------------------------------------
void SoundSetVoiceAdsrSustainRateAndDirection(s32 voiceIndex, s32 sustainRate, s32 sustainDirBit1) {
    SPU_VOICE_REG* voice = &g_pSoundSpuRegisters->_rxx.voice[voiceIndex];
    voice->adsr[1] = (voice->adsr[1] & 0x003F) + (sustainRate << 6) + ((sustainDirBit1 >> 1) << 14);
}

//----------------------------------------------------------------------------------------------------------------------
void SoundSetVoiceAdsrReleaseShiftAndMode(s32 voiceIndex, s32 releaseRate, s32 releaseModeBit2) {
    SPU_VOICE_REG* voice = &g_pSoundSpuRegisters->_rxx.voice[voiceIndex];
    s32 combinedValue = releaseRate + ((releaseModeBit2 >> 2) << 5);
    voice->adsr[1] = (voice->adsr[1] & 0xFFC0) + combinedValue;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundSetVoiceAdsrSustainLevel(s32 voiceIndex, s32 sustainLevel) {
    SPU_VOICE_REG* voice = &g_pSoundSpuRegisters->_rxx.voice[voiceIndex];
    voice->adsr[0] = (voice->adsr[0] & 0xFFF0) + sustainLevel;
}
// End ADSR functions

//----------------------------------------------------------------------------------------------------------------------
int SoundValidateFile(SoundFile* pSoundFile, u32 magicBytes, u16 targetValue) {
    unsigned char bIsError;
    
    if (pSoundFile->magic != magicBytes) {
        return SOUND_ERR_INVALID_SIGNATURE;
    }
    
    if (SoundFileComputeChecksum(pSoundFile) == 0) {
        // Version check?
        bIsError = (pSoundFile->unkC != targetValue);
        return bIsError * SOUND_ERR_UNK_0X4;
    }
    
    return SOUND_ERR_INVALID_CHECKSUM;
}

//----------------------------------------------------------------------------------------------------------------------
s32 SoundUnkDebug0(void* p) {
#if 0
    // Secrets of the universe
#endif

    return 0;
}

//----------------------------------------------------------------------------------------------------------------------
int SoundFileComputeChecksum(SoundFile* pSoundFile) {
    int nResult;
    int* pCurrent;
    unsigned int nCount;

    pCurrent = pSoundFile;
    nCount = (pSoundFile->unk8 + 3) / 4; // Align to 4-byte boundary
    nResult = 0;
    do {
        nResult += *pCurrent++;
    } while (--nCount);
    
    return nResult;
}


INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", SoundHandleError);

#ifdef XENO_PC_PORT
/* Host sequence-command dispatch table (tick-leg step 3). Retail's
 * g_SoundScriptHandlers is 128 PSX addresses in .sdata (3F290.sdata.s);
 * on LP64 the port needs host-width function pointers, so the table is
 * rebuilt here with the same 128 entries in the same slot order (the
 * game_overrides.c runtime-dispatch-table pattern). The matching build
 * keeps the .sdata original. func_8003C6E8 dispatches through this for
 * every opcode >= 0x80. Entry casts: a few legacy handlers use loose
 * parameter types; the call convention is identical. */
typedef u8* (*SoundScriptHandlerFn)(u8* pScript, AudioManager* pAudioManager,
                                    AudioElement* pAudioElements);
extern u8* func_8003CD08(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003CD54(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003CD8C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003CE68(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003CE9C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003CEF0(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003CF38(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003CFA4(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003CFF0(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003D034(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003D070(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003D0E8(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003D110(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003D13C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003D17C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003D1BC(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003D21C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003D370(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003D3A4(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003D3D8(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003D438(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003D4E4(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003D53C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003D60C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003D7C8(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003D7FC(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003D884(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003D8B8(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003D9A4(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003DAB0(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003DB2C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003DB58(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003DB98(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003DBE4(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003DC50(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003DD24(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003DE18(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003DEB4(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003DEE4(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003DF3C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003DF78(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003E04C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003E180(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003E1F8(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003E308(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003E360(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003E40C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003E44C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003E4BC(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003E4F0(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
extern u8* func_8003E54C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements);
SoundScriptHandlerFn g_SoundScriptHandlers[128] = {
    /* 0x80 */ (SoundScriptHandlerFn)func_8003CD08,
    /* 0x81 */ (SoundScriptHandlerFn)SoundScriptFermata,
    /* 0x82 */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0x83 */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0x84 */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0x85 */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0x86 */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0x87 */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0x88 */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0x89 */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0x8A */ (SoundScriptHandlerFn)SoundScriptNop3,
    /* 0x8B */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0x8C */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0x8D */ (SoundScriptHandlerFn)func_8003CD54,
    /* 0x8E */ (SoundScriptHandlerFn)SoundScriptDefaultSkip3,
    /* 0x8F */ (SoundScriptHandlerFn)SoundScriptNop4,
    /* 0x90 */ (SoundScriptHandlerFn)func_8003CD8C,
    /* 0x91 */ (SoundScriptHandlerFn)SoundScriptSaveOctaveAndIP,
    /* 0x92 */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0x93 */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0x94 */ (SoundScriptHandlerFn)SoundScriptSetOctave,
    /* 0x95 */ (SoundScriptHandlerFn)SoundScriptRaiseOctave,
    /* 0x96 */ (SoundScriptHandlerFn)SoundScriptLowerOctave,
    /* 0x97 */ (SoundScriptHandlerFn)func_8003CE68,
    /* 0x98 */ (SoundScriptHandlerFn)func_8003CEF0,
    /* 0x99 */ (SoundScriptHandlerFn)func_8003CF38,
    /* 0x9A */ (SoundScriptHandlerFn)func_8003CFA4,
    /* 0x9B */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0x9C */ (SoundScriptHandlerFn)func_8003CFF0,
    /* 0x9D */ (SoundScriptHandlerFn)func_8003D034,
    /* 0x9E */ (SoundScriptHandlerFn)func_8003D070,
    /* 0x9F */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0xA0 */ (SoundScriptHandlerFn)func_8003D0E8,
    /* 0xA1 */ (SoundScriptHandlerFn)func_8003D110,
    /* 0xA2 */ (SoundScriptHandlerFn)func_8003D13C,
    /* 0xA3 */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0xA4 */ (SoundScriptHandlerFn)SoundScriptSetManagerUnk1a,
    /* 0xA5 */ (SoundScriptHandlerFn)SoundScriptAddManagerUnk1a,
    /* 0xA6 */ (SoundScriptHandlerFn)func_8003D17C,
    /* 0xA7 */ (SoundScriptHandlerFn)func_8003D1BC,
    /* 0xA8 */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0xA9 */ (SoundScriptHandlerFn)SoundScriptSetUnk62,
    /* 0xAA */ (SoundScriptHandlerFn)func_8003D21C,
    /* 0xAB */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0xAC */ (SoundScriptHandlerFn)func_8003D298,
    /* 0xAD */ (SoundScriptHandlerFn)func_8003D2D0,
    /* 0xAE */ (SoundScriptHandlerFn)func_8003D300,
    /* 0xAF */ (SoundScriptHandlerFn)SoundScriptPercussionOff,
    /* 0xB0 */ (SoundScriptHandlerFn)SoundScriptSetActiveFlag800,
    /* 0xB1 */ (SoundScriptHandlerFn)SoundScriptClearActiveFlag800,
    /* 0xB2 */ (SoundScriptHandlerFn)func_8003D370,
    /* 0xB3 */ (SoundScriptHandlerFn)func_8003D3A4,
    /* 0xB4 */ (SoundScriptHandlerFn)func_8003D3D8,
    /* 0xB5 */ (SoundScriptHandlerFn)func_8003D438,
    /* 0xB6 */ (SoundScriptHandlerFn)SoundScriptSetVoiceFlags2000AndMode,
    /* 0xB7 */ (SoundScriptHandlerFn)SoundScriptSetVoiceFlags2000ClearMode,
    /* 0xB8 */ (SoundScriptHandlerFn)func_8003D4E4,
    /* 0xB9 */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0xBA */ (SoundScriptHandlerFn)func_8003D53C,
    /* 0xBB */ (SoundScriptHandlerFn)SoundScriptSetVoiceFlags4000ClearMode,
    /* 0xBC */ (SoundScriptHandlerFn)SoundScriptSkip3,
    /* 0xBD */ (SoundScriptHandlerFn)SoundScriptNop,
    /* 0xBE */ (SoundScriptHandlerFn)SoundScriptNop2,
    /* 0xBF */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0xC0 */ (SoundScriptHandlerFn)SoundScriptCallE5BC,
    /* 0xC1 */ (SoundScriptHandlerFn)func_8003D60C,
    /* 0xC2 */ (SoundScriptHandlerFn)SoundScriptSetAttackTime,
    /* 0xC3 */ (SoundScriptHandlerFn)SoundScriptSetDecayTime,
    /* 0xC4 */ (SoundScriptHandlerFn)SoundScriptSetSustain,
    /* 0xC5 */ (SoundScriptHandlerFn)SoundScriptSetRelease,
    /* 0xC6 */ (SoundScriptHandlerFn)SoundScriptSetSustainLevel,
    /* 0xC7 */ (SoundScriptHandlerFn)SoundScriptSetAdsrDRAndSR,
    /* 0xC8 */ (SoundScriptHandlerFn)SoundScriptSetAttackMode,
    /* 0xC9 */ (SoundScriptHandlerFn)SoundScriptSetSustainMode,
    /* 0xCA */ (SoundScriptHandlerFn)SoundScriptSetReleaseMode,
    /* 0xCB */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0xCC */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0xCD */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0xCE */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0xCF */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0xD0 */ (SoundScriptHandlerFn)SoundScriptSetUnk6E,
    /* 0xD1 */ (SoundScriptHandlerFn)SoundScriptAddUnk6E,
    /* 0xD2 */ (SoundScriptHandlerFn)func_8003D79C,
    /* 0xD3 */ (SoundScriptHandlerFn)func_8003D7C8,
    /* 0xD4 */ (SoundScriptHandlerFn)func_8003D7FC,
    /* 0xD5 */ (SoundScriptHandlerFn)SoundScriptToggleUnk04Bit2,
    /* 0xD6 */ (SoundScriptHandlerFn)func_8003D884,
    /* 0xD7 */ (SoundScriptHandlerFn)func_8003DAB0,
    /* 0xD8 */ (SoundScriptHandlerFn)func_8003D8B8,
    /* 0xD9 */ (SoundScriptHandlerFn)func_8003D9A4,
    /* 0xDA */ (SoundScriptHandlerFn)SoundScriptSetUnkCEAndF6,
    /* 0xDB */ (SoundScriptHandlerFn)SoundScriptClearUnkCEAndF6,
    /* 0xDC */ (SoundScriptHandlerFn)SoundScriptClearUnk04Bit1,
    /* 0xDD */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0xDE */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0xDF */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0xE0 */ (SoundScriptHandlerFn)func_8003DB2C,
    /* 0xE1 */ (SoundScriptHandlerFn)func_8003DB58,
    /* 0xE2 */ (SoundScriptHandlerFn)func_8003DB98,
    /* 0xE3 */ (SoundScriptHandlerFn)func_8003DE18,
    /* 0xE4 */ (SoundScriptHandlerFn)func_8003DC50,
    /* 0xE5 */ (SoundScriptHandlerFn)func_8003DD24,
    /* 0xE6 */ (SoundScriptHandlerFn)SoundScriptSetUnkCEAndUnk116,
    /* 0xE7 */ (SoundScriptHandlerFn)SoundScriptClearUnkCEAndUnk116,
    /* 0xE8 */ (SoundScriptHandlerFn)SoundScriptSetUnk74,
    /* 0xE9 */ (SoundScriptHandlerFn)func_8003DEB4,
    /* 0xEA */ (SoundScriptHandlerFn)func_8003DEE4,
    /* 0xEB */ (SoundScriptHandlerFn)func_8003DF3C,
    /* 0xEC */ (SoundScriptHandlerFn)func_8003DF78,
    /* 0xED */ (SoundScriptHandlerFn)func_8003E04C,
    /* 0xEE */ (SoundScriptHandlerFn)SoundScriptSetUnkCEAndUnk136,
    /* 0xEF */ (SoundScriptHandlerFn)func_8003E160,
    /* 0xF0 */ (SoundScriptHandlerFn)func_8003E180,
    /* 0xF1 */ (SoundScriptHandlerFn)func_8003E1F8,
    /* 0xF2 */ (SoundScriptHandlerFn)func_8003E308,
    /* 0xF3 */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0xF4 */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0xF5 */ (SoundScriptHandlerFn)SoundScriptNop5,
    /* 0xF6 */ (SoundScriptHandlerFn)func_8003E360,
    /* 0xF7 */ (SoundScriptHandlerFn)func_8003E40C,
    /* 0xF8 */ (SoundScriptHandlerFn)func_8003DBE4,
    /* 0xF9 */ (SoundScriptHandlerFn)func_8003CE9C,
    /* 0xFA */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0xFB */ (SoundScriptHandlerFn)SoundScriptDefaultHandler,
    /* 0xFC */ (SoundScriptHandlerFn)func_8003E44C,
    /* 0xFD */ (SoundScriptHandlerFn)func_8003E4BC,
    /* 0xFE */ (SoundScriptHandlerFn)func_8003E4F0,
    /* 0xFF */ (SoundScriptHandlerFn)func_8003E54C,
};
#endif /* XENO_PC_PORT: host g_SoundScriptHandlers */
