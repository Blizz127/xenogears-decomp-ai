#include "common.h"
#include "system/sound.h"
#include "psyq/kernel.h"
#include "psyq/libspu.h"
#ifdef XENO_PC_PORT
/* Retail data loaded from the user's disc before main(). */
#include "../../../pc_port/src/retail_data.h"
#endif

#ifndef XENO_PC_PORT
s32 func_8003E290();
void *func_8003E1F8(void *arg0, s32 arg1, void *arg2);
#endif

#ifdef XENO_PC_PORT
/* Host GNU C has no implicit int(): prototypes must precede first use.
 * Matching build keeps the original implicit-decl order (gcc 2.7.2). */
SoundWDSEntry* SoundFindWdsEntry(int targetID);
AudioManager* func_80039850(SoundFile* pFile);
#endif

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


#ifdef XENO_PC_PORT
/* Retail sound sdata tables (3F290.sdata.s). The matching build links the
 * .sdata; the port has zero storage filled with the same bytes from the
 * user's disc/SLUS_006.64 before main() (retail_data.h) -- zeros would make
 * note pitch/velocity/operand lookups return 0 (silent notes, broken scan
 * skips). Loaded here rather than in a pc_port data TU so the tests that
 * build this TU on its own still link. */
u8 D_80050B78[120];
s16 D_80050BF0[3072];
u8 D_80050A94[228];
u8 D_800509B0[228];
u8 D_80050824[128];
XENO_RETAIL_DATA_BEGIN(sound_sdata)
    XENO_RD(D_80050B78, XENO_RD_SLUS, XENO_RD_SLUS_OFF(0x80050B78u), sizeof(D_80050B78)),
    XENO_RD(D_80050BF0, XENO_RD_SLUS, XENO_RD_SLUS_OFF(0x80050BF0u), sizeof(D_80050BF0)),
    XENO_RD(D_80050A94, XENO_RD_SLUS, XENO_RD_SLUS_OFF(0x80050A94u), sizeof(D_80050A94)),
    XENO_RD(D_800509B0, XENO_RD_SLUS, XENO_RD_SLUS_OFF(0x800509B0u), sizeof(D_800509B0)),
    XENO_RD(D_80050824, XENO_RD_SLUS, XENO_RD_SLUS_OFF(0x80050824u), sizeof(D_80050824)),
XENO_RETAIL_DATA_END(sound_sdata)
#endif /* XENO_PC_PORT sound sdata tables */

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

/* SFX chain (real as of S1). */
extern void func_8003A14C();
extern void func_80039F18();
extern s32 func_8003A65C();
extern void func_8003B644();
extern void* func_80039024(s32 size);
extern u32 SoundSpuMemoryAllocateWDS(SoundWDSEntry* pWdsFile, int mode);
extern void func_8003E3E0();
extern s32 func_8003E290();
#ifdef XENO_PC_PORT
extern void PsyX_Sys_SoundGateEnterCritical(void);
extern void PsyX_Sys_SoundGateExitCritical(void);
#endif
/* Envelope handler methods (real as of B5.2; bound directly by 0xD8/0xE4/
 * 0xEC and via the D_800508A4 table). */
extern s32 func_8003F240();
extern s32 func_8003F2A0();
extern s32 func_8003F43C();
/* Envelope handler-method table (retail sdata 3F290: 16 function words;
 * slots 8-15 are the no-op method). Port side: the same functions as host
 * addresses narrowed to the retail-width u32 slots -- the no-pie build
 * round-trips them through SoundPsxAddress (pfnHandler stores these,
 * func_8003EFE4 casts back and calls). Matching build uses the .sdata. */
#ifdef XENO_PC_PORT
extern void func_8003F190();
extern s32 func_8003F1A4();
extern s32 func_8003F1EC();
extern s32 func_8003F308();
extern s32 func_8003F354();
extern s32 func_8003F3C0();
SoundPsxAddress D_800508A4[16];
/* Filled at SoundInitialize (a 32-bit truncating cast of a host function
 * address is not a valid static initializer). */
static void PcPort_InitEnvelopeMethodTable(void) {
    static void* const methods[16] = {
        (void*)func_8003F1A4, (void*)func_8003F1EC,
        (void*)func_8003F240, (void*)func_8003F2A0,
        (void*)func_8003F308, (void*)func_8003F308,
        (void*)func_8003F354, (void*)func_8003F3C0,
        (void*)func_8003F190, (void*)func_8003F190,
        (void*)func_8003F190, (void*)func_8003F190,
        (void*)func_8003F190, (void*)func_8003F190,
        (void*)func_8003F190, (void*)func_8003F190,
    };
    int i;
    for (i = 0; i < 16; i++) {
        D_800508A4[i] = SOUND_PTR_TO_PSX(methods[i]);
    }
}
#else
extern u32 D_800508A4[];
#endif
//----------------------------------------------------------------------------------------------------------------------

void SoundInitialize(s32 arg0) {
    if (g_SoundControlFlags < 0) {
        SoundHandleError(0x28);
        return;
    }
#ifdef XENO_PC_PORT
    PcPort_InitEnvelopeMethodTable();
#endif
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

void SoundEnableAllSpuChannels(void) {
    SoundVoiceData** ppChannel = &g_SoundChannels[0];
    int i = 0;
    do {
        SoundVoiceData* pChannel = *ppChannel;
        i++;
        if (pChannel != NULL) {
            pChannel->flags |= 0x1F5;
        }
        ppChannel++;
    } while (i < 24);
    g_SoundControlFlags &= ~0x40;
}

typedef struct {
    u16 volL;
    u16 volR;
    u16 pitch;
    u16 addr;
    u16 adsr1;
    u16 adsr2;
    u16 adsrVol;
    u16 loopAddr;
} SoundSpuVoiceRegs;

/* Transcribed from
 * asm/slus_006.64/nonmatchings/system/sound/SoundMuteAllSpuChannels.s
 * (0x80037EE4-0x80037F44, 24 instructions). Sets bit 6 of
 * g_SoundControlFlags, then walks all 24 SPU voices (0x10 bytes each) zeroing
 * the two volume words and the ADSR-low word, writing 0x1FDF into the voice's
 * +0x0A word, and forcing the high byte of the +8 word to 0x7F while keeping its
 * low byte. The port keeps its own owner in pc_port/src/world_map_pause.c, so
 * this definition is weak under XENO_PC_PORT. */
#ifndef XENO_PC_PORT
/* Matching-build statement order: it reproduces retail's code exactly (the
 * stores still come out as +0,+2,+4,+A,+8). The port keeps its body. */
void SoundMuteAllSpuChannels(void) {
    u8* pVoice = (u8*)g_pSoundSpuRegisters;
    int i;

    g_SoundControlFlags |= 0x40;
#ifdef SOUND_MUTE_MUTANT_12_VOICES
    for (i = 0; i < 0x12; i++) {
#else
    for (i = 0; i < 0x18; i++) {
#endif
        ((SoundSpuVoiceRegs*)pVoice)->volL = 0;
        ((SoundSpuVoiceRegs*)pVoice)->adsr1 = (u16)(*(u8*)&((SoundSpuVoiceRegs*)pVoice)->adsr1 + 0x7F00);
        ((SoundSpuVoiceRegs*)pVoice)->volR = 0;
        ((SoundSpuVoiceRegs*)pVoice)->pitch = 0;
        ((SoundSpuVoiceRegs*)pVoice)->adsr2 = 0x1FDF;
        pVoice += sizeof(SoundSpuVoiceRegs);
    }
}
#else
__attribute__((weak))
void SoundMuteAllSpuChannels(void) {
    u8* pVoice = (u8*)g_pSoundSpuRegisters;
    int i;

    g_SoundControlFlags |= 0x40;
#ifdef SOUND_MUTE_MUTANT_12_VOICES
    for (i = 0; i < 0x12; i++) {
#else
    for (i = 0; i < 0x18; i++) {
#endif
        *(u16*)(pVoice + 0x00) = 0;
        *(u16*)(pVoice + 0x02) = 0;
        *(u16*)(pVoice + 0x04) = 0;
        *(u16*)(pVoice + 0x0A) = 0x1FDF;
        *(u16*)(pVoice + 0x08) = (u16)(*(u8*)(pVoice + 0x08) + 0x7F00);
        pVoice += 0x10;
    }
}
#endif


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



// Load a WDS sample-bank file: allocate SPU-RAM for the ADPCM data, queue
// its upload (transfer queue -> SpuWrite), copy the header into a sound-heap
// block, and append the entry to the WDS linked list under the tick bracket.
SoundWDSEntry* SoundLoadWdsFile(SoundWDSEntry* pWdsFile, s32 mode) {
    u32 spuAddr;
    SoundWDSEntry* pEntry;

    spuAddr = SoundSpuMemoryAllocateWDS(pWdsFile, mode);
    if (spuAddr == 0) {
        SoundHandleError(0x1F);
        return NULL;
    }
    SoundQueueSpuWriteCommand(spuAddr, (u8*)pWdsFile + pWdsFile->adpcmDataOffset,
                              pWdsFile->adpcmDataSize, NULL);
    pEntry = func_80039024(pWdsFile->headerSizeMby);
    if (pEntry == NULL) {
        SoundSpuMemoryFreeBlock(spuAddr);
        SoundHandleError(0x1E);
        return NULL;
    }
    SoundHeapSetBlockMemory(pEntry, pWdsFile, pWdsFile->headerSizeMby);
    pEntry->spuMemoryAddress = spuAddr;
    DisableEvent(g_unk_SoundEvent);
    {
        SoundPsxAddress* pList = (SoundPsxAddress*)&g_SoundWdsLinkedList;
        while (SOUND_PSX_TO_PTR(SoundWDSEntry, *pList) != NULL) {
            pList = &SOUND_PSX_TO_PTR(SoundWDSEntry, *pList)->pNext;
        }
        *pList = SOUND_PTR_TO_PSX(pEntry);
    }
    pEntry->pNext = 0;
    EnableEvent(g_unk_SoundEvent);
    return pEntry;
}

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): sound_start_wave_bank_stream (src/decomp/src/resident/console_and_sound_driver.c:949). */
#ifndef XM_T_SoundInstrument
#define XM_T_SoundInstrument
typedef struct {
    u32 start;         
    u16 loop;          
    s16 note;          
    u32 envelope;      
    u16 modes;         
    u8 unkE[2];
} SoundInstrument;
#endif
#ifndef XM_T_SoundSequence
#define XM_T_SoundSequence
typedef struct SoundSequence {
    u8 unk0[0x10];
    s32 header_size;   
    s32 size;          
    s32 offset;        
    u8 unk1C[2];
    u16 volume;
    u16 key;
    u8 unk22[6];
    s32 address;       
    struct SoundSequence *next;
    SoundInstrument instrument[1];
} SoundSequence;
#endif
#ifndef XM_S_sound_start_wave_bank_stream
#define XM_S_sound_start_wave_bank_stream
SoundSequence *xm_sound_start_wave_bank_stream(SoundSequence *bank, s32 size, s32 mode) __asm__("func_800380D0");
#endif
#ifndef XM_S_sound_find_wave_bank
#define XM_S_sound_find_wave_bank
SoundSequence *xm_sound_find_wave_bank(s32 key) __asm__("SoundFindWdsEntry");
#endif
#ifndef XM_S_sound_report_error
#define XM_S_sound_report_error
void xm_sound_report_error(s32 error) __asm__("SoundHandleError");
#endif
#ifndef XM_S_sound_alloc_wave_bank_spu_memory
#define XM_S_sound_alloc_wave_bank_spu_memory
s32 xm_sound_alloc_wave_bank_spu_memory(SoundSequence *bank, s32 mode) __asm__("SoundSpuMemoryAllocateWDS");
#endif
#ifndef XM_S_sound_set_wave_bank_stream_target
#define XM_S_sound_set_wave_bank_stream_target
void xm_sound_set_wave_bank_stream_target(s32 address, s32 size) __asm__("SoundWdsSetTransferParamters");
#endif
#ifndef XM_S_sound_transfer_wave_bank_part
#define XM_S_sound_transfer_wave_bank_part
s32 xm_sound_transfer_wave_bank_part(u8 *data, s32 size) __asm__("SoundTransferWdsPart");
#endif
#ifndef XM_S_sound_alloc_memory_high
#define XM_S_sound_alloc_memory_high
void *xm_sound_alloc_memory_high(s32 size) __asm__("func_80039024");
#endif
#ifndef XM_S_sound_free_spu_memory
#define XM_S_sound_free_spu_memory
u32 xm_sound_free_spu_memory(u32 address) __asm__("SoundSpuMemoryFreeBlock");
#endif
#ifndef XM_S_sound_copy_memory
#define XM_S_sound_copy_memory
void xm_sound_copy_memory(void *dst, void *src, s32 size) __asm__("SoundHeapSetBlockMemory");
#endif
#ifndef XM_S_DisableEvent
#define XM_S_DisableEvent
long xm_DisableEvent(long event) __asm__("DisableEvent");
#endif
#ifndef XM_S_sound_tick_event
#define XM_S_sound_tick_event
extern s32 xm_sound_tick_event __asm__("g_unk_SoundEvent");
#endif
#ifndef XM_S_sound_wave_bank_list
#define XM_S_sound_wave_bank_list
extern SoundSequence *xm_sound_wave_bank_list __asm__("g_SoundWdsLinkedList");
#endif
#ifndef XM_S_EnableEvent
#define XM_S_EnableEvent
long xm_EnableEvent(long event) __asm__("EnableEvent");
#endif
SoundSequence *func_800380D0(SoundSequence *bank, s32 size, s32 mode) {
    s32 address;
    SoundSequence *copy;
    SoundSequence **link;

    if (xm_sound_find_wave_bank(bank->key) != NULL) {
        xm_sound_report_error(0x16);
        return NULL;
    }
    address = xm_sound_alloc_wave_bank_spu_memory(bank, mode);
    if (address == 0) {
        xm_sound_report_error(0x1F);
        return NULL;
    }
    xm_sound_set_wave_bank_stream_target(address, bank->size);
    xm_sound_transfer_wave_bank_part((u8 *)bank + bank->offset, size - bank->header_size);
    copy = xm_sound_alloc_memory_high(bank->header_size);
    if (copy == NULL) {
        xm_sound_free_spu_memory(address);
        xm_sound_report_error(0x1E);
        return NULL;
    }
    xm_sound_copy_memory(copy, bank, bank->header_size);
    copy->address = address;
    xm_DisableEvent(xm_sound_tick_event);
    link = &xm_sound_wave_bank_list;
    if (xm_sound_wave_bank_list != NULL) {
        do {
            link = &(*link)->next;
        } while (*link != NULL);
    }
    *link = copy;
    copy->next = NULL;
    xm_EnableEvent(xm_sound_tick_event);
    return copy;
}
#endif
#else
// Loads part of a WDS file, basically a sized SoundLoadWdsFile?
void* func_800380D0(void* pWds, s32 arg1, s32 arg2) {
    void* pEntry;
    void* pSpu;
    void* pManager;
    if (SoundFindWdsEntry(*(u16*)((u8*)pWds + 0x20)) != NULL) {
        SoundHandleError(0x16);
        return NULL;
    }
    pSpu = SoundSpuMemoryAllocateWDS(pWds, arg2);
    if (pSpu == NULL) {
        SoundHandleError(0x1F);
        return NULL;
    }
    SoundWdsSetTransferParamters(pSpu, *(s32*)((u8*)pWds + 0x14), 0x1F);
    SoundTransferWdsPart((u8*)pWds + *(s32*)((u8*)pWds + 0x18), arg1 - *(s32*)((u8*)pWds + 0x10));
    pManager = func_80039024(*(s32*)((u8*)pWds + 0x10));
    if (pManager == NULL) {
        SoundSpuMemoryFreeBlock(pSpu);
        SoundHandleError(0x1E);
        return NULL;
    }
    SoundHeapSetBlockMemory(pManager, pWds, *(s32*)((u8*)pWds + 0x10));
    DisableEvent(g_unk_SoundEvent);
    *(void**)((u8*)pManager + 0x28) = pSpu;
    {
        void** ppList = &g_SoundWdsLinkedList;
        while (*ppList != NULL) {
            ppList = (void**)((u8*)*ppList + 0x2C);
        }
        *ppList = pManager;
    }
    EnableEvent(g_unk_SoundEvent);
    *(u32*)((u8*)pManager + 0x2C) = 0;
    return pManager;
}
#endif

#ifdef XENO_PC_PORT
/* Port variant of SoundLoadWdsFile for the field music-bank stream
 * substitute (func_80085C90): identical registration flow -- SPU alloc,
 * sound-heap header copy, list append under the tick bracket -- but the
 * ADPCM payload goes to the backend synchronously via SpuWrite instead of
 * the transfer queue. Retail STREAMS this file (16KB CD windows chunk-pumped
 * to SPU); it is never resident in main RAM, and the field heap cannot fit
 * it, so the port stages it in HOST memory -- which the transfer queue
 * cannot take (SoundQueueTransferCommand narrows source pointers to PSX
 * addresses). Synchronous SpuWrite from the caller's thread is safe: the
 * PsyX backend copies into its SPU image under its own mutex, and the tick
 * only reads SPU regions its voices already point at. */
SoundWDSEntry* SoundLoadWdsFileHostStaged(SoundWDSEntry* pWdsFile) {
    u32 spuAddr;
    SoundWDSEntry* pEntry;

    spuAddr = SoundSpuMemoryAllocateWDS(pWdsFile, 0);
    if (spuAddr == 0) {
        SoundHandleError(0x1F);
        return NULL;
    }
    SpuSetTransferStartAddr(spuAddr);
    SpuWrite((u8*)pWdsFile + pWdsFile->adpcmDataOffset, pWdsFile->adpcmDataSize);
    pEntry = func_80039024(pWdsFile->headerSizeMby);
    if (pEntry == NULL) {
        SoundSpuMemoryFreeBlock(spuAddr);
        SoundHandleError(0x1E);
        return NULL;
    }
    SoundHeapSetBlockMemory(pEntry, pWdsFile, pWdsFile->headerSizeMby);
    pEntry->spuMemoryAddress = spuAddr;
    DisableEvent(g_unk_SoundEvent);
    {
        SoundPsxAddress* pList = (SoundPsxAddress*)&g_SoundWdsLinkedList;
        while (SOUND_PSX_TO_PTR(SoundWDSEntry, *pList) != NULL) {
            pList = &SOUND_PSX_TO_PTR(SoundWDSEntry, *pList)->pNext;
        }
        *pList = SOUND_PTR_TO_PSX(pEntry);
    }
    pEntry->pNext = 0;
    EnableEvent(g_unk_SoundEvent);
    return pEntry;
}
#endif

#ifdef XENO_PC_PORT
/* Port body: explicit returns. Retail's bare-return v0-passthrough (kept
 * below for the matching build) is undefined behavior on the host -- the
 * native x86 build happened to keep the allocator's result in the return
 * register, but TSan's function-exit instrumentation clobbers it (caught as
 * spuAddr=garbage under XENO_TSAN=1). */
u32 SoundSpuMemoryAllocateWDS(SoundWDSEntry* pWdsFile, int mode) {
    if (mode == SOUND_WDS_ALLOCATE_AT_ADDRESS) {
        mode = pWdsFile->spuMemoryAddress;
    } else if (mode == SOUND_WDS_ALLOCATE_AUTOMATIC) {
        mode = 0;
    }

    if (mode == 0) {
        return SoundSpuMemoryAllocateBlock(pWdsFile->adpcmDataSize, pWdsFile->unk1E);
    }

    return SoundSpuMemoryAllocateBlockAtAddress(pWdsFile->adpcmDataSize, pWdsFile->spuMemoryAddress, pWdsFile->unk1E);
}
#else
u32 SoundSpuMemoryAllocateWDS(SoundWDSEntry* pWdsFile, int mode) {
    if (mode == SOUND_WDS_ALLOCATE_AT_ADDRESS) {
        mode = pWdsFile->spuMemoryAddress;
    } else if (mode == SOUND_WDS_ALLOCATE_AUTOMATIC) {
        mode = 0;
    }

    if (mode == 0) {
        SoundSpuMemoryAllocateBlock(pWdsFile->adpcmDataSize, pWdsFile->unk1E);
        return;   /* retail: result rides v0 from the allocator (bare return in a u32 fn) */
    }

    SoundSpuMemoryAllocateBlockAtAddress(pWdsFile->adpcmDataSize, pWdsFile->spuMemoryAddress, pWdsFile->unk1E);
}
#endif

void SoundWdsSetTransferParamters(int transferAddress, int numBytesToTransfer) {
    g_SoundWdsCurSpuAddress = transferAddress;
    g_SoundWdsRemainingBytes = numBytesToTransfer;
}

#ifdef XENO_PC_PORT
/* Retail 0x8003827C-0x80038310.  Feed at most the remaining WDS bytes to
 * the ordinary SPU transfer queue, then advance the retail destination and
 * remaining-byte cursors.  The queue/SpuWrite layer is the hardware adapter;
 * file selection, chunking, and cursor behavior remain game-owned. */
void SoundTransferWdsPart(u8* pData, s32 size) {
    s32 transferSize;

    if (g_SoundWdsRemainingBytes == 0) {
        return;
    }
    transferSize = size;
    if (g_SoundWdsRemainingBytes < transferSize) {
        transferSize = g_SoundWdsRemainingBytes;
    }
    SoundQueueSpuWriteCommand(g_SoundWdsCurSpuAddress, pData,
                              transferSize, NULL);
    g_SoundWdsCurSpuAddress += transferSize;
    g_SoundWdsRemainingBytes -= transferSize;
}
#else
s32 SoundTransferWdsPart__ret(u8 *, s32) asm("SoundTransferWdsPart");

s32 SoundTransferWdsPart__ret(u8 *arg0, s32 arg1) {
    s32 temp_s0;
    s32 temp_s2;
    s32 temp_v0;
    s32 var_v1;

    temp_s2 = g_SoundWdsRemainingBytes;
    if (temp_s2 == 0) {
        return 0;
    }
    var_v1 = temp_s2;
    if (arg1 < temp_s2) {
        var_v1 = arg1;
    }
    arg1 = var_v1;
    temp_s0 = g_SoundWdsCurSpuAddress;
    SoundQueueSpuWriteCommand(temp_s0, arg0, arg1, 0);
    temp_v0 = temp_s2 - arg1;
    g_SoundWdsCurSpuAddress = temp_s0 + arg1;
    g_SoundWdsRemainingBytes = temp_v0;
    return temp_v0;
}

#endif

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

#ifdef XENO_PC_PORT
extern void func_8003A094(SoundFile*);
#else
extern void func_8003A094(void*);
#endif
extern void SoundHandleError(s32);

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): sound_remove_effect_bank (src/decomp/src/resident/console_and_sound_driver.c:1104). */
#ifndef XM_T_SoundBank
#define XM_T_SoundBank
typedef struct SoundBank {
    u8 unk0[0x10];
    u16 flags;
    u8 unk12[2];
    u16 id;
    u16 unk16;         
    u16 volumes;       
    u8 unk1A[2];
    struct SoundBank *next;
    u16 effect[1];     
} SoundBank;
#endif
#ifndef XM_S_sound_remove_effect_bank
#define XM_S_sound_remove_effect_bank
void xm_sound_remove_effect_bank(SoundBank *bank) __asm__("func_8003852C");
#endif
#ifndef XM_S_sound_effect_bank_list
#define XM_S_sound_effect_bank_list
extern SoundBank *xm_sound_effect_bank_list __asm__("g_SoundSedsLinkedList");
#endif
#ifndef XM_S_sound_stop_bank_effects
#define XM_S_sound_stop_bank_effects
void xm_sound_stop_bank_effects(SoundBank *bank) __asm__("func_8003A094");
#endif
#ifndef XM_S_sound_check_file
#define XM_S_sound_check_file
s32 xm_sound_check_file(u32 *data, u32 magic, s32 id) __asm__("SoundValidateFile");
#endif
void func_8003852C(SoundBank *bank) {
    SoundBank *entry;
    SoundBank *prev = NULL;
    SoundBank *target = bank;
    s16 error;

    for (entry = xm_sound_effect_bank_list; entry != NULL; entry = entry->next) {
        if (entry == target) {
            break;
        }
        prev = entry;
    }
    if (entry == NULL) {
        xm_sound_report_error(0x10);
        return;
    }
    xm_sound_stop_bank_effects(bank);
    xm_DisableEvent(xm_sound_tick_event);
    if (prev != NULL) {
        prev->next = target->next;
    } else {
        xm_sound_effect_bank_list = target->next;
    }
    target->next = NULL;
    error = xm_sound_check_file((u32 *)bank, 0x73646573, 0x101);
    if (error != 0) {
        xm_sound_report_error(0xB);
        return;
    }
    xm_EnableEvent(xm_sound_tick_event);
}
#endif
#else
void func_8003852C(void* pSed) {
    void* pPrev = NULL;
    void* pCur = g_SoundSedsLinkedList;
    /* The +0x1C link is a 32-bit SoundPsxAddress (see SoundFile.pNext). */
    while (pCur != NULL && pCur != pSed) {
        pPrev = pCur;
        pCur = SOUND_PSX_TO_PTR(void, *(SoundPsxAddress*)((u8*)pCur + 0x1C));
    }
    if (pCur == NULL) {
        SoundHandleError(0x10);
        return;
    }
    func_8003A094(pSed);
    DisableEvent(g_unk_SoundEvent);
    if (pPrev != NULL) {
        *(SoundPsxAddress*)((u8*)pPrev + 0x1C) = *(SoundPsxAddress*)((u8*)pSed + 0x1C);
    } else {
        g_SoundSedsLinkedList = SOUND_PSX_TO_PTR(SoundFile, *(SoundPsxAddress*)((u8*)pSed + 0x1C));
    }
    *(SoundPsxAddress*)((u8*)pSed + 0x1C) = SOUND_PTR_TO_PSX(NULL);
    if (SoundValidateFile(pSed, 0x73646573, 0x101) != 0) {
        SoundHandleError(0xB);
    } else {
        EnableEvent(g_unk_SoundEvent);
    }
}
#endif

//----------------------------------------------------------------------------------------------------------------------
void func_80038624(void) {
    func_80039FF8();
    g_SoundSedsLinkedList = 0;
}

//----------------------------------------------------------------------------------------------------------------------
/* Finds the SED list entry with pData's id (+0x14), or with targetId when
 * pData is NULL. The +0x1C link is a 32-bit SoundPsxAddress on both builds. */
void* func_8003864C(void* pData, s16 targetId) {
    void* pEntry;

    if (pData != NULL) {
        targetId = *(u16*)((u8*)pData + 0x14);
    }
    for (pEntry = g_SoundSedsLinkedList; pEntry != NULL;
         pEntry = SOUND_PSX_TO_PTR(void, *(SoundPsxAddress*)((u8*)pEntry + 0x1C))) {
        if (targetId == *(u16*)((u8*)pEntry + 0x14)) {
            break;
        }
    }
    return pEntry;
}

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
#ifdef XENO_PC_PORT
/* Retail 0x8003885C-0x800388D4.  Store the requested CD level, derive the
 * four-byte CD-ROM attenuation matrix, and submit it through libcd. */
void SoundSetupCdMix(s32 level) {
    s32 mixedLevel;

    g_SoundVolumeController.unk_field2 = (s16)level;
    if (g_SoundControlFlags & 0x700) {
        mixedLevel = 0;
    } else {
        mixedLevel = level >> 1;
    }
    g_SoundCdRomAttenuation.val0 = (u8)mixedLevel;
    g_SoundCdRomAttenuation.val1 = (u8)mixedLevel;
    g_SoundCdRomAttenuation.val2 = (u8)mixedLevel;
    g_SoundCdRomAttenuation.val3 = (u8)mixedLevel;
    CdMix(&g_SoundCdRomAttenuation);
}
#else
void SoundSetupCdMix(s32 level) {
    register u8 left asm("$2");
    g_SoundVolumeController.unk_field2 = level;
    if (g_SoundControlFlags & 0x700) {
        left = level;
        level = 0;
    } else {
        level >>= 1;
        left = level;
    }
    g_SoundCdRomAttenuation.val0 = g_SoundCdRomAttenuation.val2 = left;
    g_SoundCdRomAttenuation.val1 = g_SoundCdRomAttenuation.val3 = level;
    CdMix(&g_SoundCdRomAttenuation);
}

#endif

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

#ifdef XENO_PC_PORT
        /* The port's SPU backend completes SpuWrite synchronously, so the call
         * above already ran the whole chunk chain to its bytesRemaining==0
         * tail, which frees g_SoundUploadDestBuffer and zeroes it. Retail's
         * DMA transfer callback fires from the SPU interrupt, i.e. after this
         * duplicate write has been queued, so its pointer is still live here.
         * The buffer is zeroed by SoundHeapAllocate and the duplicate writes
         * the same chunk to the same SPU address, so once the synchronous
         * completion has released it there is nothing left that retail would
         * observe: skip the duplicate instead of handing PsyX_SPUAL_Write a
         * host NULL (SIGSEGV in the reverb clear path). */
        if ((g_SoundControlFlags & (1 << 4)) == 0 && g_SoundUploadDestBuffer != 0) {
#else
        if ((g_SoundControlFlags & (1 << 4)) == 0) {
#endif
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
// Sound-heap allocate-from-top: find the LAST gap that fits (keeping WDS
// headers away from the low heap), link a block header in, zero the memory.
// NOTE (retail-faithful): the allocation-failure path returns WITHOUT
// re-enabling the tick event -- the bracket leaks on heap-full (unhit in
// practice; the caller error-paths out).
void* func_80039024(s32 size) {
    SoundHeapBlockHeader* found;
    u32 end;
    s32 aligned;
    void* pMem;

    DisableEvent(g_unk_SoundEvent);
    aligned = ((size + 0xF) & -0x10) + 0x10;
    found = NULL;
    end = 0;
    {
        SoundHeapBlockHeader* blk = g_SoundHeapHead;
        u32 heapEnd = g_SoundHeapEnd;
        for (;;) {
            SoundPsxAddress next = blk->pNext;
            if (next == 0) {
                if ((s32)(heapEnd - blk->pPrev) >= aligned) {
                    found = blk;
                    end = heapEnd;
                }
                break;
            }
            if ((s32)(next - blk->pPrev) >= aligned) {
                found = blk;
                end = next;
            }
            blk = SOUND_PSX_TO_PTR(SoundHeapBlockHeader, next);
        }
    }
    if (found == NULL) {
        return NULL;
    }
    {
        SoundHeapBlockHeader* hdr;
        end = end - aligned;
        hdr = SOUND_PSX_TO_PTR(SoundHeapBlockHeader, (end + 0xF) & -0x10);
        pMem = (u8*)hdr + 0x10;
        hdr->pPrev = SOUND_PTR_TO_PSX((u8*)pMem + size);
        hdr->pNext = 0;
        hdr->unk4 = 0;
        hdr->unk0 = 0x2;
        hdr->unk2 = 0;
        hdr->pNext = found->pNext;
        found->pNext = SOUND_PTR_TO_PSX(hdr);
        EnableEvent(g_unk_SoundEvent);
        SoundHeapClearBlockMemory(pMem, size);
        return pMem;
    }
}


void SoundHeapFree(void* pMemory) {
#ifdef XENO_PC_PORT
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
#else
    void *temp_s0;
    register void *temp_s1 asm("$17");
    register void *var_v0 asm("$2");
    void *var_v1;

    temp_s0 = g_SoundHeapHead;
    temp_s1 = pMemory;
    DisableEvent(g_unk_SoundEvent);
    temp_s1 -= 0x10;
    var_v0 = temp_s0;
    var_v1 = NULL;
    if (temp_s0 != temp_s1) {
        do {
            var_v1 = var_v0;
            var_v0 = *(void **)((s8*)(var_v1) + 0xC);
        } while (var_v0 != temp_s1);
    }
    if (var_v1 != NULL) {
        *(s32 *)((s8*)(var_v1) + 0xC) = (s32) *(s32 *)((s8*)(temp_s1) + 0xC);
    }
    EnableEvent(g_unk_SoundEvent);
#endif
}

s32 func_800391CC(void) {
    void* pBlock = g_SoundHeapHead;
    s32 maxFree = 0;
    void* pLast;
    while (*(void**)((u8*)pBlock + 0xC) != NULL) {
        void* pNext = *(void**)((u8*)pBlock + 0xC);
        s32 free = (u8*)pNext - *(u8**)((u8*)pBlock + 8);
        if (free > maxFree) maxFree = free;
        pLast = pNext;
        pBlock = pNext;
    }
    {
        s32 tailFree = (u8*)g_SoundHeapEnd - *(u8**)((u8*)pBlock + 8);
        if (tailFree > maxFree) maxFree = tailFree;
    }
    return maxFree & ~0xF;
}

void SoundHeapSetBlockMemory(void* pBlockMemory, void* pSrc, int size) {
    u32* pCurSrc;
    u32* pCurDst;
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

    /* retail steps the same two pointers bytewise */
    for (nCount = size & 3; nCount != 0; nCount--) {
        *(u8*)pCurDst = *(u8*)pCurSrc;
        pCurDst = (u32*)((u8*)pCurDst + 1);
        pCurSrc = (u32*)((u8*)pCurSrc + 1);
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

extern s32 SoundSpuMemoryGetFreeBlock(void);

#ifdef XENO_PC_PORT
/* Retail 0x800393B8-0x800394B8.  First-fit allocation in the twelve-entry
 * SPU-RAM descriptor list.  Retail does not consume arg1 in this routine. */
u32 SoundSpuMemoryAllocateBlock(s32 size, s32 arg1) {
    SoundSpuMemoryBlock* pBlock;
    SoundSpuMemoryBlock* pNew;
    s32 address;
    s32 freeIndex;

    (void)arg1;
    pBlock = &g_SoundSpuMemoryBlocks[0];
    address = pBlock->spuAddress + pBlock->size;

    while (pBlock->nextBlockIndex != 0) {
        SoundSpuMemoryBlock* pNext =
            &g_SoundSpuMemoryBlocks[pBlock->nextBlockIndex];
        if (pNext->spuAddress - address >= size) {
            break;
        }
        pBlock = pNext;
        address = pBlock->spuAddress + pBlock->size;
    }

    if (pBlock->nextBlockIndex == 0 && 0x80000 - address < size) {
        return 0;
    }

    freeIndex = SoundSpuMemoryGetFreeBlock();
    if (freeIndex < 0) {
        return 0;
    }
    pNew = &g_SoundSpuMemoryBlocks[freeIndex];
    pNew->flags = SPU_MEMORY_IN_USE;
    pNew->unk1 = 0;
    pNew->spuAddress = address;
    pNew->size = size;
    pNew->nextBlockIndex = pBlock->nextBlockIndex;
    pBlock->nextBlockIndex = (s16)freeIndex;
    return (u32)address;
}
#else
#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): allocateSpuRam (src/main/sound.c). */
#ifndef XM_T_SpuBlock
#define XM_T_SpuBlock
typedef struct SpuBlock {
     u8 m0_flags;
     u8 m1_tag;
     s16 m2_next;
     s32 m4_addr;
     s32 m8_size;
     s32 mC;
} SpuBlock;
#endif
#ifndef XM_S_allocateSpuRam
#define XM_S_allocateSpuRam
s32 xm_allocateSpuRam(s32 size, s32 tag) __asm__("SoundSpuMemoryAllocateBlock");
#endif
#ifndef XM_S_spuAllocatedBlocks
#define XM_S_spuAllocatedBlocks
extern SpuBlock xm_spuAllocatedBlocks[12] __asm__("g_SoundSpuMemoryBlocks");
#endif
#ifndef XM_S_findFreeSlotInLoadedSamples
#define XM_S_findFreeSlotInLoadedSamples
s32 xm_findFreeSlotInLoadedSamples(void) __asm__("SoundSpuMemoryGetFreeBlock");
#endif
s32 xm_allocateSpuRam(s32 size, s32 tag) __asm__("SoundSpuMemoryAllocateBlock");
s32 xm_allocateSpuRam(s32 size, s32 tag) {
    SpuBlock* p;
    SpuBlock* next;
    s32 addr;
    s32 i;

    p = xm_spuAllocatedBlocks;
    addr = xm_spuAllocatedBlocks[0].m4_addr + xm_spuAllocatedBlocks[0].m8_size;
    if (p->m2_next == 0) {
        goto tail;
    }
loop:
    next = &xm_spuAllocatedBlocks[p->m2_next];
    if (next->m4_addr - addr >= size) {
        goto found;
    }
    p = next;
    addr = next->m4_addr + next->m8_size;
    if (p->m2_next != 0) {
        goto loop;
    }
tail:
    if (0x80000 - addr < size) {
        return 0;
    }
found:
    i = xm_findFreeSlotInLoadedSamples();
    if (i < 0) {
        return 0;
    }
    next = &xm_spuAllocatedBlocks[i];
    next->m0_flags = 0x80;
    next->m1_tag = 0;
    next->m4_addr = addr;
    next->m8_size = size;
    next->m2_next = p->m2_next;
    p->m2_next = i;
    return addr;
}
#endif
#endif

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): sound_alloc_spu_memory_high (src/decomp/src/resident/console_and_sound_driver.c:1712). */
#ifndef XM_T_SpuMemBlock
#define XM_T_SpuMemBlock
typedef struct SpuMemBlock {
    u8 flags;          
    u8 unk1;
    s16 next;          
    u32 address;       
    u32 size;
    u32 unkC;
} SpuMemBlock;
#endif
#ifndef XM_S_sound_alloc_spu_memory_high
#define XM_S_sound_alloc_spu_memory_high
s32 xm_sound_alloc_spu_memory_high(s32 size) __asm__("func_800394B8");
#endif
#ifndef XM_S_sound_spu_memory_map
#define XM_S_sound_spu_memory_map
extern SpuMemBlock xm_sound_spu_memory_map[12] __asm__("g_SoundSpuMemoryBlocks");
#endif
#ifndef XM_S_sound_find_free_spu_block
#define XM_S_sound_find_free_spu_block
s32 xm_sound_find_free_spu_block(void) __asm__("SoundSpuMemoryGetFreeBlock");
#endif
s32 func_800394B8(s32 size) {
    SpuMemBlock *entry = xm_sound_spu_memory_map;
    SpuMemBlock *found = NULL;
    SpuMemBlock *block;
    u32 end;
    u32 address;
    s32 i;

    for (;;) {
        end = entry->address + entry->size;
        if (entry->next == 0) {
            if ((s32)(0x80000 - end) >= size) {
                found = entry;
                address = 0x80000 - size;
            }
            break;
        }
        block = &xm_sound_spu_memory_map[entry->next];
        if ((s32)(block->address - end) >= size) {
            found = entry;
            address = block->address - size;
        }
        entry = block;
    }
    if (found == NULL) {
        return 0;
    }
    i = xm_sound_find_free_spu_block();
    if (i < 0) {
        return 0;
    }
    block = &xm_sound_spu_memory_map[i];
    block->flags = 0x80;
    block->unk1 = 0;
    block->address = address;
    block->size = size;
    block->next = entry->next;
    entry->next = i;
    return address;
}
#endif
#else
void* func_800394B8(s32 size) {
    /* retail 0x800394B8: walk the whole next-chain; every gap (and the tail
     * up to 0x80000) that holds `size` becomes the candidate, the LAST one
     * wins and the new block is placed at the END of that gap.  Quirk kept
     * from retail: the new descriptor is spliced in after the FINAL block of
     * the chain (the one the walk ended on), not after the block whose gap
     * was chosen. */
    SoundSpuMemoryBlock* pBlock = &g_SoundSpuMemoryBlocks[0];
    SoundSpuMemoryBlock* pNew;
    s32 bestAddr = 0;
    s32 haveBest = 0;
    s32 freeIdx;

    for (;;) {
        s32 end = (s32)((u32)pBlock->spuAddress + (u32)pBlock->size);
        s16 nextIdx = pBlock->nextBlockIndex;
        SoundSpuMemoryBlock* pNext;
        s32 gapEnd;

        if (nextIdx == 0) {
            if (!((s32)(0x80000u - (u32)end) < size)) {
                haveBest = 1;
                bestAddr = (s32)(0x80000u - (u32)size);
            }
            break;
        }
        pNext = (SoundSpuMemoryBlock*)((u8*)g_SoundSpuMemoryBlocks + (s32)((u32)(s32)nextIdx << 4));
        gapEnd = pNext->spuAddress;
        if (!((s32)((u32)gapEnd - (u32)end) < size)) {
            haveBest = 1;
            bestAddr = (s32)((u32)gapEnd - (u32)size);
        }
        pBlock = pNext;
    }
    if (!haveBest) {
        return NULL;
    }
    freeIdx = SoundSpuMemoryGetFreeBlock();
    if (freeIdx < 0) {
        return NULL;
    }
    pNew = &g_SoundSpuMemoryBlocks[freeIdx];
    pNew->flags = SPU_MEMORY_IN_USE;
    pNew->unk1 = 0;
    pNew->spuAddress = bestAddr;
    pNew->size = size;
    pNew->nextBlockIndex = pBlock->nextBlockIndex;
    pBlock->nextBlockIndex = (s16)freeIdx;
    return (void*)(uintptr_t)(u32)bestAddr;
}
#endif

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
#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): allocateSPUSpaceForSamples (src/main/sound.c). */
#ifndef XM_S_allocateSPUSpaceForSamples
#define XM_S_allocateSPUSpaceForSamples
s32 xm_allocateSPUSpaceForSamples(s32 size, s32 addr, s32 tag) __asm__("SoundSpuMemoryAllocateBlockAtAddress");
#endif
s32 xm_allocateSPUSpaceForSamples(s32 size, s32 addr, s32 tag) __asm__("SoundSpuMemoryAllocateBlockAtAddress");
s32 xm_allocateSPUSpaceForSamples(s32 size, s32 addr, s32 tag) {
    SpuBlock* p;
    SpuBlock* next;
    s32 gap;
    s32 end;
    s32 a;
    s32 i;
    s32 lim;

    p = xm_spuAllocatedBlocks;
    gap = 0;
    lim = addr + size;
    end = xm_spuAllocatedBlocks[0].m4_addr + xm_spuAllocatedBlocks[0].m8_size;
    if (xm_spuAllocatedBlocks[0].m4_addr < addr) {
        do {
            

            do {
                if (p->m2_next == 0) {
                    goto top;
                }
            } while (0);
            next = &xm_spuAllocatedBlocks[p->m2_next];
            a = next->m4_addr;
            if (a >= lim) {
                goto mid;
            }
            end = a + next->m8_size;
            p = next;
        } while (a < addr);
    }
check:
    if (gap < size) {
        return 0;
    }
    if (addr >= end) {
        goto alloc;
    }
    return 0;
mid:
    gap = a - end;
    goto check;
top:
    gap = 0x80000 - end;
    goto check;
alloc:
    i = xm_findFreeSlotInLoadedSamples();
    if (i < 0) {
        return 0;
    }
    next = &xm_spuAllocatedBlocks[i];
    next->m0_flags = 0x80;
    next->m1_tag = 0;
    next->m4_addr = addr;
    next->m8_size = size;
    next->m2_next = p->m2_next;
    p->m2_next = i;
    return addr;
}
#endif
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

void* func_800397FC(s32 arg0, s32 arg1, s32 arg2) {
    void* result = func_80039850(arg0);
    func_80039A80(result, arg1, arg2);
    return result;
}

// Song-start: create an AudioManager from a loaded song file (music twin of
// func_8003B148). Sized by the file's element count (+0x180 tail block when
// the file carries override records), bound to the file at mgr+0x8, then
// header-init + element-arm + list registration.
AudioManager* func_80039850(SoundFile* pFile) {
    SoundFile* pF;
    s16 res;
    s32 size;

    res = SoundUnkDebug0(pFile);
    /* Retail merges live ranges: the parameter register is reused for the
     * manager while pF keeps the file; the copy schedules into the error
     * branch's delay slot. */
    pF = pFile;
    if (res != 0) {
        SoundHandleError(res);
        return NULL;
    }
    size = SoundCalculateAudioManagerSize(((u8*)pFile)[0x14]);
    if (((u8*)pFile)[0x15] != 0) {
        size += 0x180;
    }
    pFile = SoundHeapAllocate(size);
    if (pFile == NULL) {
        SoundHandleError(0x1E);
        return NULL;
    }
    ((AudioManager*)pFile)->unk_0x8 = SOUND_PTR_TO_PSX(pF);
    if (((u8*)pF)[0x15] != 0) {
        func_8003B0AC((AudioManager*)pFile, pF);
    }
    func_8003B22C((AudioManager*)pFile);
    func_8003B424((AudioManager*)pFile);
    *(u32*)&((AudioManager*)pFile)->unk_0x4c[0] = 0;
    SoundAddAudioManagerToList((AudioManager*)pFile);
    return (AudioManager*)pFile;
}

// Song-start: rebind an EXISTING manager allocation to a new song file
// (song switch without realloc) -- clear, rebind, re-init, re-arm,
// re-register; the 0x4000 flag marks the block as not-heap-owned so
// func_800399D4 will not free it.
AudioManager* func_80039910(SoundFile* pFile, AudioManager* manager) {
    s16 res;
    s32 size;

    res = SoundUnkDebug0(pFile);
    if (res != 0) {
        SoundHandleError(res);
        return NULL;
    }
    size = SoundCalculateAudioManagerSize(((u8*)pFile)[0x14]);
    if (((u8*)pFile)[0x15] != 0) {
        size += 0x180;
    }
    SoundHeapClearBlockMemory(manager, size);
    manager->unk_0x8 = SOUND_PTR_TO_PSX(pFile);
    if (((u8*)pFile)[0x15] != 0) {
        func_8003B0AC(manager, pFile);
    }
    func_8003B22C(manager);
    func_8003B424(manager);
    *(u32*)&manager->unk_0x4c[0] = 0;
    SoundAddAudioManagerToList(manager);
    *(u16*)&manager->unk_Flags |= 0x4000;
    return manager;
}

// Song-start: destroy a song manager -- stop if running, unregister from the
// manager list, release voices, and free the block unless 0x4000 marks it
// caller-owned (func_80039910 rebinds).
void func_800399D4(AudioManager* manager) {
#ifdef XENO_PC_PORT
    /* Same port-only guard as func_80039C4C / func_80039C8C / func_8003A89C:
     * the song-manager global (D_80062528) can be NULL in the port because the
     * music start path stores whatever func_80039850 returned (misc8.c:3053)
     * and still raises D_8004F35C, so the teardown chain reaches here with
     * NULL (SIGSEGV observed at map 23 field teardown, backtrace
     * func_800399D4(manager=0x0) <- func_8001B5E8 <- func_8001B66C <-
     * func_80078D44).  Retail's global is never NULL on this path. */
    if (manager == NULL || ((unsigned long)manager >> 47) != 0) {
        return;
    }
#endif
    if (*(s16*)&manager->unk_Flags & 0x8000) {
        func_80039C4C(manager);
    }
    if ((s16)SoundUnkDebug0(SOUND_PSX_TO_PTR(SoundFile, manager->unk_0x8)) != 0) {
        SoundHandleError(0xA);
        return;
    }
    if (SoundRemoveAudioManagerFromList(manager) != 0) {
        SoundHandleError(5);
        return;
    }
    func_8003B930(manager);
    if (!(*(u16*)&manager->unk_Flags & 0x4000)) {
        SoundHeapFree(manager);
    }
}

// Song-start: (re)start the bound song from the top -- re-init the manager
// from its file, re-arm the elements, zero the channel level then set it
// (func_8003A89C), and raise the running flag; all under the tick bracket.
void func_80039A80(AudioManager* manager, s32 level, s32 steps) {
    if (manager == NULL) {
        SoundHandleError(5);
        return;
    }
    *(u16*)&manager->unk_Flags &= 0x7FFF;
    if ((s16)SoundUnkDebug0(SOUND_PSX_TO_PTR(SoundFile, manager->unk_0x8)) != 0) {
        SoundHandleError(0xA);
        return;
    }
    if (*(s16*)&manager->unk_Flags & 0x8000) {
        func_80039C4C(manager);
    }
    DisableEvent(g_unk_SoundEvent);
    func_8003B22C(manager);
    func_8003B424(manager);
    manager->unk_Interpolator_0x70.currentValue = 0;
    func_8003A89C(manager, level, steps);
    {
        u16 flags = *(u16*)&manager->unk_Flags;
        u32 ev = g_unk_SoundEvent;
        *(u16*)&manager->unk_Flags = flags | 0x8000;
        EnableEvent(ev);
    }
}

// Song-start: WDS-rebind resume -- after a sample-bank swap, re-derive every
// element's SPU start/loop addresses from its instrument record in the
// (re)loaded bank, mark all voice registers dirty, then fade the level back
// in; 0x100 arms the resume path inside func_8003A89C.
void func_80039B68(AudioManager* manager, s32 level, s32 steps) {
    AudioElement* el;
    s32 count;

    if (manager == NULL) {
        SoundHandleError(5);
        return;
    }
    count = manager->elementCount;
    el = &manager->elements[0];
    /* The flags store sits LAST so the loop's induction family anchors on
     * voice_data.flags (el+0x36), matching retail; the scheduler then hoists
     * the halfword store into the load-delay slack above it. */
    do {
        u8* pBank;
        u8* pInstr;
        s32 base;

        pBank = (u8*)SoundFindWdsEntry(((u8*)&el->unk_0x24)[1]);
        count--;
        pInstr = pBank + ((el->unk_0x26 << 4) + 0x30);
        el->unk2C = SOUND_PTR_TO_PSX(pBank);
        base = *(s32*)pInstr << 3;
        el->voice_data.startAddress = base + *(u32*)(pBank + 0x28);
        el->voice_data.loopAddress = base + (*(u16*)(pInstr + 4) << 3);
        el->voice_data.flags = 0xFFFF;
        el++;
    } while (count != 0);
    manager->unk_Interpolator_0x70.currentValue = 0;
    *(u16*)&manager->unk_Flags |= 0x100;
    func_8003A89C(manager, level, steps);
}

//----------------------------------------------------------------------------------------------------------------------
void func_80039C4C(AudioManager* manager) {
#ifdef XENO_PC_PORT
    /* D_80062528 is a 32-byte data stub; loading it as AudioManager* can
     * produce a non-canonical host pointer (title/Map0 FieldMain crash). */
    if (manager == NULL || ((unsigned long)manager >> 47) != 0) {
        return;
    }
#else
    if (manager == NULL) {
        SoundHandleError(5);
        return;
    }
#endif
    manager->unk_Flags &= ~(1 << 15);
    SoundReleaseAllVoices(manager);
}

//----------------------------------------------------------------------------------------------------------------------
void func_80039C8C(AudioManager* manager, s32 arg1) {
#ifdef XENO_PC_PORT
    if (manager == NULL || ((unsigned long)manager >> 47) != 0) {
        return;
    }
#else
    if (manager == NULL) {
        SoundHandleError(5);
        return;
    }
#endif
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

extern s32 D_80059544;

s32 func_80039D78(s32 numVoices) {
    if (numVoices != 0) {
        s32 val;
        if (numVoices >= 0x11) numVoices = 0x10;
        if (numVoices < 4) numVoices = 4;
        val = numVoices & 0xFE;
        D_80059544 = val;
    }
    return D_80059544;
}

// SFX API: play a packed (sedId<<16|entry) effect on the fixed top slot
// (rotator-2) at default volume/pan; priority byte 0x80 (not stealable).
void func_80039DB8(s32 packedId) {
    if (g_SoundControlFlags & 0x800) {
        s32 slot;
        D_80059404 = 2;
        slot = D_80059478 - 2;
        func_8003B644((s16)(slot | -0x8000), packedId, 0x6000, 0x4000);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Wowoweewa flag central
void func_80039E18(s32 arg0) {
    if (g_SoundControlFlags & (1 << 11)) {
        D_80059404 = 2;
        func_8003B644((1 << 2) | (1 << 3) | (1 << 13) | (1 << 14), arg0, (1 << 13) | ( 1 << 14), (1 << 14));
    }
}

//----------------------------------------------------------------------------------------------------------------------
// SFX API: play a packed effect on an allocated slot (func_8003A65C dedupes
// and picks/steals a pair) at default volume/pan.
void func_80039E60(s32 packedId) {
    if (g_SoundControlFlags & 0x800) {
        s32 slot = func_8003A65C(packedId, 2);
        D_80059404 = 2;
        func_8003B644((s16)(slot | 0x2000), packedId, 0x6000, 0x4000);
    }
}

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
// SFX API (also reached from sequence handler CFF0 -- script-spawned
// effects): play a packed effect on an allocated slot with signed 8-bit
// volume/pan scaled <<8.
void func_80039F18(s32 packedId, s32 volume, s32 pan) {
    if (g_SoundControlFlags & 0x800) {
        s32 slot = func_8003A65C(packedId, 2);
        D_80059404 = 2;
        func_8003B644((s16)(slot | 0x2000), packedId, (s8)volume << 8,
                      (s8)pan << 8);
    }
}

// SFX API (field cues): play a packed effect on an explicit field slot
// (mapped onto the element pairs via ^8) with signed 8-bit volume/pan.
void func_80039F9C(s32 packedId, s32 slot, s32 volume, s32 pan) {
    if (g_SoundControlFlags & 0x800) {
        D_80059404 = 2;
        func_8003B644((s16)(((slot & 0xFE) ^ 8) | 0x2000), packedId,
                      (s8)volume << 8, (s8)pan << 8);
    }
}

// SFX: stop ALL effect elements (the D_80059478 SFX region of the main
// manager) under the tick bracket and clear the busy mask.
void func_80039FF8(void) {
    s32 count = D_80059478;
    AudioManager* manager = SOUND_PSX_TO_PTR(AudioManager, D_800595D8);
    AudioElement* el;
    SoundVoiceData* vd;

    el = &manager->elements[0];
    DisableEvent(g_unk_SoundEvent);
    vd = &manager->elements[0].voice_data;
    do {
        if (el->active_flag & 0x1) {
            el->active_flag = 0;
            SoundReleaseVoiceFromChannel(vd, *((u8*)vd - 0x9));
        }
        vd = (SoundVoiceData*)((u8*)vd + 0x158);
        count--;
        el++;
    } while (count != 0);
    manager->unk_0x48 = 0;
    EnableEvent(g_unk_SoundEvent);
}

#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; the matching
 * build keeps INCLUDE_ASM (byte-exact) below. Residual vs {}: cc1 re-anchors the element/voice cursor family (role permutation of the two loop cursors); same operations at the same element offsets.
 * Not claimed as {}. */
// SFX: stop every active effect belonging to a SED file (element+0x0A holds
// the sedId half of the packed id) and release its voice.
void func_8003A094(SoundFile* pFile) {
    AudioManager* manager = SOUND_PSX_TO_PTR(AudioManager, D_800595D8);
    s32 count = D_80059478;
    s16 sedId = *(s16*)((u8*)pFile + 0x14);
    u8* q = (u8*)manager + 0xBB;

    do {
        AudioElement* el = (AudioElement*)(q - 0x27);
        if (el->active_flag & 0x1) {
            if (*(s16*)(q - 0x1D) == sedId) {
                el->active_flag = 0;
                manager->unk_0x48 &= ~(1 << *(q - 0x21));
                SoundReleaseVoiceFromChannel(&el->voice_data, q[0]);
            }
        }
        q += 0x158;
        count--;
    } while (count != 0);
}
#else
void func_8003A094(void *arg0) {
    s16 temp_s4;
    s32 var_s3;
    u16 *var_s1;
    void *temp_s2;
    register void *var_s0 asm("$16");

    var_s3 = D_80059478;
    temp_s2 = D_800595D8;
    temp_s4 = *(s16 *)((s8*)(arg0) + 0x14);
    var_s1 = temp_s2 + 0x94;
    var_s0 = temp_s2 + 0xBB;
    do {
        var_s3 -= 1;
        if ((*var_s1 & 1) && (*(s16 *)((s8*)(var_s0) + -0x1D) == temp_s4)) {
            *var_s1 = 0;
            *(s32 *)((s8*)(temp_s2) + 0x48) = (s32) (~(1 << *(u8 *)((s8*)(var_s0) + -0x21)) & *(s32 *)((s8*)(temp_s2) + 0x48));
            SoundReleaseVoiceFromChannel((u8 *)var_s1 + 0x30, *(u8 *)((s8*)(var_s0) + 0));
        }
        var_s0 += 0x158;
        var_s1 = (u16 *)((u8 *)var_s1 + 0x158);
    } while (var_s3 != 0);
}

#endif

#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; the matching
 * build keeps INCLUDE_ASM (byte-exact) below. Residual vs {}: cc1 re-anchors the element/voice cursor family (role permutation); same operations at the same element offsets.
 * Not claimed as {}. */
// SFX (also reached from a sequence handler): stop every active effect
// matching a full packed (sedId<<16|entry) id.
void func_8003A14C(s32 packedId) {
    AudioManager* manager = SOUND_PSX_TO_PTR(AudioManager, D_800595D8);
    s32 count = D_80059478;
    u8* q = (u8*)manager + 0xBB;

    do {
        AudioElement* el = (AudioElement*)(q - 0x27);
        if (el->active_flag & 0x1) {
            if (*(s32*)(q - 0x1F) == packedId) {
                el->active_flag = 0;
                manager->unk_0x48 &= ~(1 << *(q - 0x21));
                SoundReleaseVoiceFromChannel(&el->voice_data, q[0]);
            }
        }
        q += 0x158;
        count--;
    } while (count != 0);
}
#else
void func_8003A14C(s32 packedId) {
    AudioManager* manager = SOUND_PSX_TO_PTR(AudioManager, D_800595D8);
    s32 count = D_80059478;
    register AudioElement* el asm("$17") = (AudioElement*)((u8*)manager + 0x94);
    register u8* q asm("$16") = (u8*)manager + 0xBB;

    do {
        __asm__("" : : "r"(el), "r"(q));
        if (el->active_flag & 0x1) {
            if (*(s32*)(q - 0x1F) == packedId) {
                el->active_flag = 0;
                manager->unk_0x48 &= ~(1 << *(q - 0x21));
                SoundReleaseVoiceFromChannel(&el->voice_data, q[0]);
            }
        }
        q += 0x158;
        el++;
        count--;
    } while (count != 0);
}
#endif

// SFX: stop the effect pair on a field slot and release its voices.
void func_8003A20C(s32 slot) {
    AudioManager* manager;
    AudioElement* el;
    u8* q;
    s32 count;

    slot &= 0xFE;
    slot ^= 8;
    count = 2;
    manager = SOUND_PSX_TO_PTR(AudioManager, D_800595D8);
    el = (AudioElement*)((slot * 0x158 + 0x94) + (u8*)manager);
    q = (u8*)el + 0x27;
    do {
        if (el->active_flag & 0x1) {
            el->active_flag = 0;
            manager->unk_0x48 &= ~(1 << *(q - 0x21));
            SoundReleaseVoiceFromChannel(&el->voice_data, q[0]);
        }
        q += 0x158;
        count--;
        el = (AudioElement*)((u8*)el + 0x158);
    } while (count != 0);
}

void func_8003A2D4(void) {}

void func_8003A2DC(void) {}

extern s32 D_80059478;

void func_8003A2E4(s32 pData, s32 volume) {
#ifdef XENO_PC_PORT
    u8* pBase = (u8*)(uintptr_t)D_800595D8;
    u8* pEntry = pBase + 0x96;
    s32 count = D_80059478;
    u16 volWord = (u16)(volume << 8);
    while (count-- != 0) {
        u16 flags = *(u16*)(pEntry - 2);
        if ((flags & 1) && *(s32*)(pEntry + 6) == pData) {
            *(u16*)(pEntry + 0x74) = volWord;
            *(u16*)(pEntry + 0) = 0x100;
        }
        pEntry += 0x158;
    }
#else
    AudioElement *el;
    register u8 *base asm("$2");
    register u16 *st asm("$3");
    s32 count;
    count = D_80059478;
    volume <<= 8;
    base = (u8 *)D_800595D8;
    el = (AudioElement *)(base + 0x94);
    st = (u16 *)(base + 0x96);
    do {
        if ((el->active_flag & 1) && *(s32 *)(st + 3) == pData) {
            st[0x3A] = volume;
            st[0] = 0x100;
        }
        el = (AudioElement *)((u8 *)el + 0x158);
        count--;
        st += 0xAC;
    } while (count != 0);
#endif
}

// SFX: set the expression volume (el+0x76) of an active field-slot pair;
// status = 0x100 (volume-dirty).
void func_8003A344(s32 slot, s32 volume) {
    AudioElement* el;
    u16* st;
    s32 count;

    slot &= 0xFE;
    slot ^= 8;
    count = 2;
    volume <<= 8;
    el = (AudioElement*)((slot * 0x158 + 0x94) +
                         (u8*)SOUND_PSX_TO_PTR(AudioManager, D_800595D8));
    st = &el->status_flags;
    do {
        if (el->active_flag & 0x1) {
            st[0x3A] = volume;
            st[0] = 0x100;
        }
        el = (AudioElement*)((u8*)el + 0x158);
        count--;
        st += 0xAC;
    } while (count != 0);
}


void func_8003A3B8(s32 pData, s32 pitch, s32 steps) {
#ifdef XENO_PC_PORT
    u8* pBase = (u8*)(uintptr_t)D_800595D8;
    u8* pEntry = pBase + 0x98;
    s32 count = D_80059478;
    s32 pitchWord = (s32)((u32)pitch << 8);
    while (count-- != 0) {
        u16 flags = *(u16*)(pEntry - 4);
        if ((flags & 1) && *(s32*)(pEntry + 4) == pData) {
            s16 curPitch = *(s16*)(pEntry + 0x72);
            s32 delta = pitchWord - curPitch;
            if (delta != 0) {
                s32 step;
                if (steps == 0) steps = 1;
                step = delta / steps;
                *(u16*)(pEntry + 0x8A) = pitchWord;
                *(u16*)(pEntry + 0x96) = (u16)steps;
                *(u16*)(pEntry + 0) |= 0x20;
                *(u16*)(pEntry + 0x88) = (u16)step;
            }
        }
        pEntry += 0x158;
    }
#else
    s32 temp_a1;
    s32 var_a2;
    s32 temp_v0;
    s32 quotient;
    register u8 *base asm("$2");
    s32 var_t1;
    u16 *var_t0;
    register void *var_a3 asm("$7");

    var_a2 = steps;
    temp_a1 = (u32)pitch << 8;
    base = (u8*)D_800595D8;
    var_t1 = D_80059478;
    var_t0 = (u16*)(base + 0x94);
    var_a3 = base + 0x98;
    do {
        if ((*var_t0 & 1) && (*(s32 *)((s8*)(var_a3) + 4) == pData)) {
            temp_v0 = temp_a1 - *(s16 *)((s8*)(var_a3) + 0x72);
            if (temp_v0 != 0) {
                if (var_a2 == 0) {
                    var_a2 = 1;
                }
                quotient = temp_v0 / var_a2;
                *(s16 *)((s8*)(var_a3) + 0x8A) = temp_a1;
                *(s16 *)((s8*)(var_a3) + 0x96) = var_a2;
                *(u16 *)((s8*)(var_a3) + 0) = (u16) (*(u16 *)((s8*)(var_a3) + 0) | 0x20);
                *(s16 *)((s8*)(var_a3) + 0x88) = (s16) quotient;
            }
        }
        var_a3 += 0x158;
        var_t1 -= 1;
        var_t0 += 0xAC;
    } while (var_t1 != 0);
#endif
}


void func_8003A450(s32 channel, s32 pitch, s32 steps) {
#ifdef XENO_PC_PORT
    s32 idx = (channel & 0xFE) ^ 8;
    u8* pBase = (u8*)(uintptr_t)D_800595D8;
    u8* pFlags = pBase + 0x94 + idx * 0x158;
    u8* pEntry = pFlags + 4;
    s32 count = 2;
    s32 pitchWord = (s32)((u32)pitch << 8);
    while (count-- != 0) {
        if (*(u16*)pFlags & 1) {
            s16 curPitch = *(s16*)(pEntry + 0x72);
            s32 delta = pitchWord - curPitch;
            if (delta != 0) {
                s32 step;
                if (steps == 0) steps = 1;
                step = delta / steps;
                *(u16*)(pEntry + 0x8A) = pitchWord;
                *(u16*)(pEntry + 0x96) = (u16)steps;
                *(u16*)pEntry |= 0x20;
                *(u16*)(pEntry + 0x88) = (u16)step;
            }
        }
        pFlags += 0x158;
        pEntry += 0x158;
    }
#else
    s32 temp_a1;
    s32 var_a2;
    s32 temp_v0;
    s32 quotient;
    register u32 offset asm("$2");
    s32 var_t0;
    u16 *var_a3;
    register void *var_a0 asm("$4");

    var_a2 = steps;
    var_a0 = (void*)((channel & 0xFE) ^ 8);
    var_t0 = 2;
    temp_a1 = (u32)pitch << 8;
    offset = (u32)var_a0 * 0x158 + 0x94;
    var_a3 = (u16*)(offset + (u32)D_800595D8);
    var_a0 = (u8*)var_a3 + 4;
    do {
        if (*var_a3 & 1) {
            temp_v0 = temp_a1 - *(s16 *)((s8*)(var_a0) + 0x72);
            if (temp_v0 != 0) {
                if (var_a2 == 0) {
                    var_a2 = 1;
                }
                quotient = temp_v0 / var_a2;
                *(s16 *)((s8*)(var_a0) + 0x8A) = temp_a1;
                *(s16 *)((s8*)(var_a0) + 0x96) = var_a2;
                *(u16 *)((s8*)(var_a0) + 0) = (u16) (*(u16 *)((s8*)(var_a0) + 0) | 0x20);
                *(s16 *)((s8*)(var_a0) + 0x88) = (s16) quotient;
            }
        }
        var_a0 += 0x158;
        var_t0 -= 1;
        var_a3 += 0xAC;
    } while (var_t0 != 0);
#endif
}

void func_8003A4FC(s32 pData, s32 pan) {
#ifdef XENO_PC_PORT
    u8* pBase = (u8*)(uintptr_t)D_800595D8;
    u8* pEntry = pBase + 0x96;
    s32 count = D_80059478;
    u16 panWord = (u16)(pan << 8);
    while (count-- != 0) {
        u16 flags = *(u16*)(pEntry - 2);
        if ((flags & 1) && *(s32*)(pEntry + 6) == pData) {
            *(u16*)(pEntry + 0x72) = panWord;
            *(u16*)(pEntry + 0) = 0x100;
        }
        pEntry += 0x158;
    }
#else
    AudioElement *el;
    register u8 *base asm("$2");
    register u16 *st asm("$3");
    s32 count;
    count = D_80059478;
    pan <<= 8;
    base = (u8 *)D_800595D8;
    el = (AudioElement *)(base + 0x94);
    st = (u16 *)(base + 0x96);
    do {
        if ((el->active_flag & 1) && *(s32 *)(st + 3) == pData) {
            st[0x39] = pan;
            st[0] = 0x100;
        }
        el = (AudioElement *)((u8 *)el + 0x158);
        count--;
        st += 0xAC;
    } while (count != 0);
#endif
}

// SFX: set the pan (el+0x74) of an active field-slot pair; status = 0x100.
void func_8003A55C(s32 slot, s32 pan) {
    AudioElement* el;
    u16* st;
    s32 count;

    slot &= 0xFE;
    slot ^= 8;
    count = 2;
    pan <<= 8;
    el = (AudioElement*)((slot * 0x158 + 0x94) +
                         (u8*)SOUND_PSX_TO_PTR(AudioManager, D_800595D8));
    st = &el->status_flags;
    do {
        if (el->active_flag & 0x1) {
            st[0x39] = pan;
            st[0] = 0x100;
        }
        el = (AudioElement*)((u8*)el + 0x158);
        count--;
        st += 0xAC;
    } while (count != 0);
}

/* Bitmask of active entries (optionally only those whose +8 word is pData).
 * The -1 loop advances the pointer before testing the flags it just read,
 * as retail does. */
u32 func_8003A5D0(s32 pData) {
    u32 bit = 1;
    s32 count = D_80059478;
    u8* pEntry = (u8*)(uintptr_t)D_800595D8 + 0x94;
    u32 mask = 0;
    if (pData == -1) {
        do {
            u16 flags = *(u16*)pEntry;
            pEntry += 0x158;
            if (flags & 1) mask |= bit;
            count--;
            bit <<= 1;
        } while (count != 0);
    } else {
        do {
            count--;
            if ((*(u16*)pEntry & 1) && *(s32*)(pEntry + 8) == pData) {
                mask |= bit;
            }
            pEntry += 0x158;
            bit <<= 1;
        } while (count != 0);
    }
    return mask;
}

#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; the matching
 * build keeps INCLUDE_ASM (byte-exact) below. Residual vs {}: register-allocation permutation across the steal-scan (window/mask/serial temporaries); control flow and offsets verified 1:1.
 * Not claimed as {}. */
// SFX slot allocator: first stop any element already playing this packed id
// (retrigger dedupe), then scan pair-aligned slots downward from the SFX
// rotator for a free window in the busy mask; if none, steal the OLDEST
// (smallest start serial) slot whose priority byte is <= 0x20.
s32 func_8003A65C(s32 packedId, s32 pairSize) {
    AudioManager* manager = SOUND_PSX_TO_PTR(AudioManager, D_800595D8);
    s32 count = D_80059478;
    u8* q = (u8*)manager + 0xBB;
    AudioElement* el;
    u32 exclude = 0;
    s32 bestSerial = -1;
    s32 best;
    s32 start;
    s32 lower;
    u32 mask;
    u32 window;
    u32 busy;

#ifdef XENO_PC_PORT
    /* Retail leaves `best` uninitialized ($s6) when every candidate has
     * priority > 0x20 -- on PSX the garbage index wraps harmlessly; on the
     * host it would index wild. Default to the scan start. */
    best = D_80059478 - (pairSize + 2);
#endif
    do {
        el = (AudioElement*)(q - 0x27);
        if (el->active_flag & 0x1) {
            if (*(s32*)(q - 0x1F) == packedId) {
                el->active_flag = 0;
                manager->unk_0x48 &= ~(1 << *(q - 0x21));
                SoundReleaseVoiceFromChannel(&el->voice_data, q[0]);
            }
        }
        q += 0x158;
        count--;
    } while (count != 0);

    start = D_80059478 - (pairSize + 2);
    lower = manager->elementCount - D_80059544;
    mask = 0xFFFFFFFFu >> (0x20 - pairSize);
    window = mask << start;
    manager = SOUND_PSX_TO_PTR(AudioManager, D_800595D8);
    el = (AudioElement*)((u8*)manager + start * 0x158 + 0x94);
    busy = ~exclude & manager->unk_0x48;
    if ((busy & window) != 0) {
        s32 stride = pairSize * 0x158;
        for (;;) {
            s32 serial = *(s32*)((u8*)el + 0xC);
            if ((u32)serial < (u32)bestSerial) {
                if (*((u8*)el + 0x7) < 0x21) {
                    bestSerial = serial;
                    best = start;
                }
            }
            window >>= pairSize;
            if (window < mask) {
                start = best;
                break;
            }
            if (!((u32)lower < (u32)start)) {
                start = best;
                break;
            }
            el = (AudioElement*)((u8*)el - stride);
            start -= pairSize;
            if ((busy & window) == 0) {
                break;
            }
        }
    }
    return start;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003A65C);
#endif

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

// Song-start: set the manager channel level (interp70) -- immediate when
// steps==0 (marks every active element volume-dirty), else programs a fade;
// a nonzero level while flag 0x100 (bank-rebind resume) is set completes the
// resume via func_8003AA30. This is field opcode FE 0E's target.
void func_8003A89C(AudioManager* manager, s32 level, s32 steps) {
#ifdef XENO_PC_PORT
    /* Field teardown calls this directly with the song-manager global
     * (src/field/main/main.c:578, `func_8003A89C(D_80062528, 0x7F, 0)`), and a
     * map entered without a started song leaves that global NULL -- the
     * music-manager chain only writes it once a WDS song lands (misc8.c:3053).
     * The sibling entry points func_80039C4C / func_80039C8C / func_800399D4
     * already take the same NULL guard; without it here the first
     * `manager->...` store faults (SIGSEGV observed on map 23 at the first
     * field teardown after a map reload). Retail's global is never NULL on
     * this path, so the guard only covers port-only state. */
    if (manager == NULL || ((unsigned long)manager >> 47) != 0) {
        return;
    }
#endif
    manager->unk_Interpolator_0x70.targetValue = level << 8;
    if (steps == 0) {
        manager->unk_Interpolator_0x70.currentValue = level << 24;
        manager->unk_Interpolator_0x70.counter = 0;
        unk_SoundSetFlagsOnActiveVoices(0x100, manager);
    } else {
        s32 diff = (level << 16) - (manager->unk_Interpolator_0x70.currentValue >> 8);
        if (diff == 0) {
            return;
        }
        manager->unk_Interpolator_0x70.counter = steps;
        manager->unk_Interpolator_0x70.stepIncrement = (diff / steps) << 8;
    }
    if ((*(u16*)&manager->unk_Flags & 0x100) && level != 0) {
        func_8003AA30(manager);
    }
}

void func_8003A948(void* pManager, s32 target, s32 steps) {
    *(u16*)((u8*)pManager + 0x86) = (u16)(target << 8);
    if (steps == 0) {
        *(s32*)((u8*)pManager + 0x7C) = target << 24;
        *(u16*)((u8*)pManager + 0x84) = 0;
        unk_SoundSetFlagsOnActiveVoices(0x200, pManager);
    } else {
        s32 current = *(s32*)((u8*)pManager + 0x7C) >> 8;
        s32 delta = (target << 16) - current;
        if (delta != 0) {
            s32 step = delta / steps;
            *(u16*)((u8*)pManager + 0x84) = (u16)steps;
            *(s32*)((u8*)pManager + 0x80) = step << 8;
        }
    }
}

void func_8003A9BC(void* pManager, s32 target, s32 steps) {
    *(u16*)((u8*)pManager + 0x92) = (u16)(target << 8);
    if (steps == 0) {
        *(s32*)((u8*)pManager + 0x88) = target << 24;
        *(u16*)((u8*)pManager + 0x90) = 0;
        unk_SoundSetFlagsOnActiveVoices(0x100, pManager);
    } else {
        s32 current = *(s32*)((u8*)pManager + 0x88) >> 8;
        s32 delta = (target << 16) - current;
        if (delta != 0) {
            s32 step = delta / steps;
            *(u16*)((u8*)pManager + 0x90) = (u16)steps;
            *(s32*)((u8*)pManager + 0x8C) = step << 8;
        }
    }
}

// Song-start: resume a muted/rebound manager -- reapply its reverb program,
// mark every voice register dirty (full SPU rewrite on the next tick),
// resync, clear the resume flag and raise running; under the tick bracket.
void func_8003AA30(AudioManager* manager) {
    DisableEvent(g_unk_SoundEvent);
    if (g_SoundControlFlags & 0x1000) {
        SoundSetReverbModeWithAllocation(manager->unk_0x40[1],
                                         *(s16*)&manager->unk_0x40[4],
                                         manager->unk_0x40[2],
                                         manager->unk_0x40[3]);
    }
    SoundSetFlagsOnActiveVoices(manager, 0xFFFF);
    func_8003AFA0(manager);
    {
        u16 flags = *(u16*)&manager->unk_Flags;
        u32 ev = g_unk_SoundEvent;
        *(u16*)&manager->unk_Flags = (flags & 0xFEFF) | 0x8000;
        EnableEvent(ev);
    }
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003AAC4);
#else
void func_8003AAC4(void* pManager, u32 mask) {
    u8* pFlags;
    u8* pVoiceData;
    u8* pChannelData;
    s32 count;
    if (pManager == NULL) return;
    pFlags = (u8*)pManager + 0x94;
    pVoiceData = (u8*)pManager + 0xBB;
    count = *(u8*)((u8*)pManager + 0x14);
    pChannelData = (u8*)pManager + 0xC4;
    *(u32*)((u8*)pManager + 0x4C) = mask;
    while (count-- != 0) {
        u16 flags = *(u16*)pFlags;
        if (flags != 0) {
            if (mask & 1) {
                if (!(flags & 0x20)) {
                    *(u16*)pFlags = flags | 0x20;
                    if (*(s16*)((u8*)pManager + 0x10) & 0x8000) {
                        SoundStopVoiceOnChannel(pChannelData, *pVoiceData);
                    }
                }
            } else {
                if (flags & 0x20) {
                    *(u16*)pFlags = flags & 0xFFDF;
                    if ((*(u32*)pFlags & 0x110) == 0x100) {
                        if (*(s16*)((u8*)pManager + 0x10) & 0x8000) {
                            SoundAssignVoiceToChannelAndPlay(pChannelData, *pVoiceData);
                        }
                    }
                }
            }
        }
        pVoiceData += 0x158;
        pChannelData += 0x158;
        pFlags += 0x158;
        mask >>= 1;
    }
}
#endif

void func_8003ABE8(u8* arg0, u8 arg1) {
    arg0[0x1B] = arg1;
}

void func_8003ABF0(void* source, void* output) {
    u32 value = *(u32*)((u8*)source + 0x28) >> 8;
    u32 units = value / 240;
    u32 largeUnits;

    *(u32*)output = *(u32*)((u8*)source + 0x24);
    *(u16*)((u8*)output + 4) = value - units * 240;
    largeUnits = units / 60;
    *(u16*)((u8*)output + 6) = units - largeUnits * 60;
    *(u16*)((u8*)output + 8) = largeUnits;
}

u16* func_8003AC58(void* pManager) {
#ifdef XENO_PC_PORT
    u8* pFlags = (u8*)pManager + 0x94;
    u16 count = *(u8*)((u8*)pManager + 0x14);
    u16* pOutput = (u16*)((u8*)pManager + 0x30);
    u16 minVal = 0xFFFF;
    u8* pVal = (u8*)pManager + 0xB4;
    while (count-- != 0) {
        if (*(u16*)pFlags != 0) {
            u16 val = *(u16*)pVal;
            if (val < minVal) minVal = val;
        }
        pFlags += 0x158;
        pVal += 0x158;
    }
    if (minVal == 0xFFFF) minVal = 0;
    *pOutput = minVal;
    return pOutput;
#else
    u16 *var_a0;
    u16 *out;
    u16 *var_a2;
    u16 var_a1;
    u16 var_v1;
    u16 flags;
    register u32 step asm("$8");

    var_a2 = pManager + 0x94;
    var_v1 = *(u8 *)((s8*)(pManager) + 0x14);
    out = pManager + 0x30;
    var_a1 = 0xFFFF;
    step = 0xFFFF;
    var_a0 = pManager + 0xB4;
    do {
        flags = *var_a2;
        var_a2 = (u16 *)((u8 *)var_a2 + 0x158);
        if ((flags != 0) && ((u16) *var_a0 < var_a1)) {
            var_a1 = *var_a0;
        }
        var_v1 += step;
        var_a0 = (u16 *)((u8 *)var_a0 + 0x158);
    } while (var_v1 & 0xFFFF);
    if ((var_a1 & 0xFFFF) == 0xFFFF) {
        var_a1 = 0;
    }
    *out = var_a1;
    return out;
#endif
}

/* Returns the data block of the audio manager pTarget (or of the list head
 * when pTarget is NULL: retail skips the search then), or NULL when pTarget
 * is not in the list. */
void* func_8003ACC8(void* pTarget) {
    void* pEntry = g_SoundAudioManagerListHead;
    void* pData;

    if (pTarget != 0) {
        while (pEntry != 0 && pEntry != pTarget) {
            pEntry = *(void**)((u8*)pEntry + 0);
        }
    }
    if (pEntry == 0) {
        return 0;
    }
    pData = *(void**)((u8*)pEntry + 8);
    return (void*)(*(u16*)((u8*)pData + 0x1E) + (uintptr_t)pData);
}

void func_8003AD20(void* pManager, s32 mode) {
    switch (mode) {
        case 0: func_8003AD98(pManager); break;
        case 1: func_8003ADCC(pManager); break;
        case 2: func_8003AE84(pManager); break;
    }
}

void func_8003AD98(void* pManager) {
    u16 flags = *(u16*)((u8*)pManager + 0x10);
    if (flags & 0x10) {
        *(u16*)((u8*)pManager + 0x10) = flags & 0xFFEF;
        func_8003B930(pManager);
    }
}

extern s32 SoundCalculateAudioManagerSize(s32);
extern void SoundHeapSetBlockMemory(void*, void*, s32);

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): sound_take_seq_snapshot (src/decomp/src/resident/sound.c:1074). */
#ifndef XM_T_SoundSeqHeader
#define XM_T_SoundSeqHeader
typedef struct SoundSeqHeader {
    u8 unk0[0x10];
    u16 unk10;
    u8 unk12[2];
    u8 channels;
    u8 entries;        
    u16 unk16;
    u16 unk18;
    u8 reverb_type;
    u8 reverb_depth;   
    u8 reverb_delay;
    u8 reverb_feedback;
    u16 unk1E;
    u16 table;         
    u16 channel[1];    
} SoundSeqHeader;
#endif
#ifndef XM_T_Fixed
#define XM_T_Fixed
typedef union {
    s32 value;
    struct {
        u16 fraction;
        s16 whole;
    } part;
} Fixed;
#endif
#ifndef XM_T_SoundEffectId
#define XM_T_SoundEffectId
typedef union {
    s32 full;
    struct {
        u16 effect;
        s16 bank;
    } part;
} SoundEffectId;
#endif
#ifndef XM_T_SoundChannel
#define XM_T_SoundChannel
typedef struct SoundChannel {
    u16 voice;         
    u16 mode;          
    s16 priority;
    u16 flags;         


    s16 volume_left;
    s16 volume_right;
    u16 unkC;
    u16 unkE;
    u8 unk10[4];
    u16 pitch;
    u8 unk16[6];
    u32 sample_start;  
    u32 sample_loop;
    SoundEnvelope envelope;
} SoundChannel;
#endif
#ifndef XM_T_SoundLoop
#define XM_T_SoundLoop
typedef struct {
    u8 count;          
    u8 unk1;
    u8 transpose;      
    u8 exit_transpose; 
    u8 *start;
    u8 *end;
} SoundLoop;
#endif
#ifndef XM_T_SoundModulator
#define XM_T_SoundModulator
typedef struct SoundModulator {
    s32 (*wave)(struct SoundModulator *modulator);
    s32 phase;         
    s32 slope;
    s32 step;
    u16 count;         
    u16 rate;          
    u16 delay_count;
    s16 delay;
    s16 period_count;
    s16 period;
    u8 target;         
    u8 shape;
    u16 flags;         
} SoundModulator;
#endif
#ifndef XM_T_SoundSeqChannel
#define XM_T_SoundSeqChannel
typedef struct {
    u16 flags;         
    u16 flags2;        
    u16 flags3;        
    u8 voice_bit;      
    u8 priority;
    SoundEffectId id;
    u32 stamp;         
    u8 *start;         
    u8 *position;      
    u8 *loop;          
    s32 unk1C;
    u16 unk20;
    u8 unk22;
    u8 unk23;
    u8 unk24;
    u8 unk25;
    u8 instrument;
    u8 voice;          
    u8 unk28;
    u8 unk29[3];
    struct SoundSequence *instruments;
    SoundChannel state;
    s16 unk5C;         
    u16 unk5E;         
    u8 duration_adjust; 
    u8 unk61;
    u16 gate_fraction; 
    u8 previous_note;
    u8 current_note;
    s16 transpose;     
    Fixed note;   
    s16 unk6C;
    s16 detune;
    u16 unk70;
    u16 loop_depth;    
    s16 pan;           
    s16 volume;
    Fixed level;  
    s32 unk7C;
    s16 unk80;
    s16 unk82;
    s32 unk84;
    s32 unk88;
    s16 volume_step;
    s16 volume_target;
    s16 pan_step;
    s16 pan_target;
    u16 unk94;         
    u16 unk96;         
    u16 pan_frames;
    u16 volume_frames;
    SoundLoop loops[4];
    u16 modulator_index; 
    u16 modulators;    
    s16 pitch_mod;     
    s16 level_mod;
    s16 pan_mod;
    u8 unkD6[2];
    SoundModulator modulator[4];
} SoundSeqChannel;
#endif
#ifndef XM_T_SoundSeq
#define XM_T_SoundSeq
typedef struct SoundSeq {
    struct SoundSeq *next;
    struct SoundSeq *snapshot; 
    struct SoundSeqHeader *header; 
    u32 *table;        
    u16 flags;         

    u16 unk12;
    u8 channels;
    u8 unk15;
    s16 unk16;         
    u16 unk18;
    u8 unk1A;
    u8 unk1B;
    u16 noise_clock;
    u16 unk1E;
    s32 unk20;
    u32 unk24;
    u32 ticks;
    u32 unk2C;
    u16 unk30;
    u16 unk32;
    u16 unk34;
    u16 unk36;
    u16 unk38;
    u16 unk3A;
    s16 unk3C;
    s16 unk3E;
    u8 unk40;
    u8 reverb_type;
    u8 reverb_delay;
    u8 reverb_feedback;
    s16 reverb_depth;
    u8 unk46[2];
    u32 voices;        
    u32 muted;         
    s32 unk50;
    s32 tick_step;     
    Fixed rate;   
    s32 rate_step;
    u16 rate_frames;
    u16 rate_target;
    Fixed tempo;  
    s32 tempo_step;
    s16 tempo_frames;
    s16 tempo_target;
    Fixed fade;   
    s32 fade_step;
    s16 fade_frames;
    s16 fade_target;
    Fixed pitch;  
    s32 pitch_step;
    s16 pitch_frames;
    s16 pitch_target;
    Fixed pan;    
    s32 pan_step;
    s16 pan_frames;
    s16 pan_target;
    SoundSeqChannel channel[1];
} SoundSeq;
#endif
#ifndef XM_S_sound_take_seq_snapshot
#define XM_S_sound_take_seq_snapshot
void xm_sound_take_seq_snapshot(SoundSeq *seq) __asm__("func_8003ADCC");
#endif
#ifndef XM_S_sound_get_seq_size
#define XM_S_sound_get_seq_size
s32 xm_sound_get_seq_size(s32 index) __asm__("SoundCalculateAudioManagerSize");
#endif
#ifndef XM_S_sound_alloc_memory_low
#define XM_S_sound_alloc_memory_low
void *xm_sound_alloc_memory_low(s32 size) __asm__("SoundHeapAllocate");
#endif
void func_8003ADCC(SoundSeq *seq) {
    s32 size;
    SoundSeq *snapshot;

    xm_DisableEvent(xm_sound_tick_event);
    size = xm_sound_get_seq_size(seq->channels);
    if (seq->snapshot == NULL) {
        snapshot = xm_sound_alloc_memory_low(size);
    }
    if (snapshot == NULL) {
        xm_EnableEvent(xm_sound_tick_event);
        return;
    }
    seq->snapshot = snapshot;
    seq->flags |= 0x10;
    xm_sound_copy_memory(snapshot, seq, size);
    snapshot->next = NULL;
    snapshot->snapshot = NULL;
    seq->unk2C = 0;
    xm_EnableEvent(xm_sound_tick_event);
}
#endif
#else
void func_8003ADCC(void* pManager) {
    void* pAlloc;
    s32 size;
    DisableEvent(g_unk_SoundEvent);
    size = SoundCalculateAudioManagerSize(*(u8*)((u8*)pManager + 0x14));
    if (*(void**)((u8*)pManager + 4) == NULL) {
        pAlloc = SoundHeapAllocate(size);
    } else {
        pAlloc = *(void**)((u8*)pManager + 4);
    }
    if (pAlloc == NULL) {
        EnableEvent(g_unk_SoundEvent);
        return;
    }
    *(void**)((u8*)pManager + 4) = pAlloc;
    *(u16*)((u8*)pManager + 0x10) |= 0x10;
    SoundHeapSetBlockMemory(pAlloc, pManager, size);
    *(u32*)pAlloc = 0;
    *(u32*)((u8*)pAlloc + 4) = 0;
    *(u32*)((u8*)pManager + 0x2C) = 0;
    EnableEvent(g_unk_SoundEvent);
}
#endif

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


void func_8003AF24(void* pManager, s32 arg1) {
#ifdef XENO_PC_PORT
    u16 flags;
    s16 vol;
    DisableEvent(g_unk_SoundEvent);
    *(u16*)((u8*)pManager + 0x1E) = (u16)arg1;
    flags = *(u16*)((u8*)pManager + 0x10);
    *(u16*)((u8*)pManager + 0x10) = flags | 0x20;
    SoundReleaseAllVoices(pManager);
    EnableEvent(g_unk_SoundEvent);
    vol = *(s16*)((u8*)pManager + 0x5A);
    *(u16*)((u8*)pManager + 0x6E) = 0x7F00;
    *(u16*)((u8*)pManager + 0x6C) = 0;
    *(s32*)((u8*)pManager + 0x64) = 0x7F000000;
    *(s32*)((u8*)pManager + 0x54) = vol * 0x7F00;
#else
    s32 event = g_unk_SoundEvent;
    register s32 scale asm("$2");
    s32 volume;
    *(s16 *)((s8*)(pManager) + 0x1E) = arg1;
    *(u16 *)((s8*)(pManager) + 0x10) = (u16) (*(u16 *)((s8*)(pManager) + 0x10) | 0x20);
    DisableEvent(event);
    SoundReleaseAllVoices(pManager);
    EnableEvent(g_unk_SoundEvent);
    volume = *(s16 *)((u8 *)pManager + 0x5A);
    scale = 0x7F00;
    *(s16 *)((s8*)(pManager) + 0x6E) = scale;
    *(s16 *)((s8*)(pManager) + 0x6C) = 0;
    *(s32 *)((s8*)(pManager) + 0x64) = 0x7F000000;
    *(s32 *)((s8*)(pManager) + 0x54) = volume * scale;
#endif
}

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

// Song-start: load the song file's override records into the manager's tail
// block (the +0x180 region func_80039850 allocates when file[0x15] != 0):
// file[0x15] 5-byte records {slot, valueLE32} at file + *(u16*)(file+0x20),
// stored word-wise at tailBlock[slot].
void func_8003B0AC(AudioManager* manager, SoundFile* pFile) {
    u32* block;
    u8* rec;
    s32 n;

    block = (u32*)((u8*)manager +
                   SoundCalculateAudioManagerSize(((u8*)pFile)[0x14]));
    manager->unk_0xc = SOUND_PTR_TO_PSX(block);
    n = ((u8*)pFile)[0x15];
    rec = (u8*)pFile + *(u16*)((u8*)pFile + 0x20);
    do {
        u32 val = rec[1] | (rec[2] << 8) | (rec[3] << 16) |
                  ((u32)rec[4] << 24);
        *(u32*)((u8*)block + (rec[0] << 2)) = val;
        rec += 5;
        n--;
    } while (n != 0);
}

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

extern void SoundHeapFree(void*);

void func_8003B1FC(void* pManager) {
    SoundRemoveAudioManagerFromList(pManager);
    SoundHeapFree(pManager);
}

// Song-start: init the manager from its bound song file's header -- copy id/
// element count/WDS id/tempo base/reverb program (file 0x10-0x1D), flag the
// manager as a song manager (bit 0), apply reverb when globally enabled,
// then the common manager init.
void func_8003B22C(AudioManager* manager) {
    SoundFile* pFile;

    pFile = SOUND_PSX_TO_PTR(SoundFile, manager->unk_0x8);
    if ((s16)SoundUnkDebug0(pFile) != 0) {
        SoundHandleError(0xA);
        return;
    }
    *(u16*)&manager->unk_Flags |= 1;
    *(u16*)&manager->unk2[0] = *(u16*)((u8*)pFile + 0x10);
    manager->elementCount = ((u8*)pFile)[0x14];
    *(u16*)&manager->unk_0x15[1] = *(u16*)((u8*)pFile + 0x16);
    *(u16*)&manager->unk_0x18 = *(u16*)((u8*)pFile + 0x18);
    manager->unk_0x40[1] = ((u8*)pFile)[0x1A];
    *(u16*)&manager->unk_0x40[4] = ((u8*)pFile)[0x1B] << 8;
    manager->unk_0x40[2] = ((u8*)pFile)[0x1C];
    manager->unk_0x40[3] = ((u8*)pFile)[0x1D];
    if (g_SoundControlFlags & 0x1000) {
        SoundSetReverbModeWithAllocation(*(s8*)&manager->unk_0x40[1],
                                         *(s16*)&manager->unk_0x40[4],
                                         manager->unk_0x40[2],
                                         manager->unk_0x40[3]);
    }
    SoundInitializeAudioManager(manager);
}

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): sound_init_effect_channels (src/decomp/src/resident/sound.c:1262). */
#ifndef XM_S_sound_init_effect_channels
#define XM_S_sound_init_effect_channels
void xm_sound_init_effect_channels(SoundSeq *seq) __asm__("func_8003B32C");
#endif
#ifndef XM_S_sound_effect_channel_count
#define XM_S_sound_effect_channel_count
extern s32 xm_sound_effect_channel_count __asm__("D_80059478");
#endif
#ifndef XM_S_sound_reset_seq_playback
#define XM_S_sound_reset_seq_playback
void xm_sound_reset_seq_playback(SoundSeq *seq) __asm__("SoundInitializeAudioManager");
#endif
void func_8003B32C(SoundSeq *seq) {
    seq->flags = 2;
    seq->unk12 = 0x7FFF;
    seq->unk16 = 0;
    seq->unk18 = 0x7F;
    seq->channels = xm_sound_effect_channel_count;
    xm_sound_reset_seq_playback(seq);
}
#endif
#else
void func_8003B32C(AudioManager* manager) {
    manager->unk_Flags = 2;
    *(u16*)&manager->unk2[0] = 0x7FFF;
    *(u16*)&manager->unk_0x15[1] = 0;
    manager->unk_0x18 = 0x7F;
    manager->elementCount = D_80059478;
    SoundInitializeAudioManager(manager);
}
#endif

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
// Song-start: the music element-arm loop (twin of the SFX starter's loop,
// driven by the manager's bound song file instead of a SED entry). Per
// channel script offset (file+0x22 halfwords): arm the element -- REST state
// (0x401; +0x20 if its bit persists in mgr+0x4C, +0x4 under manager flag
// 0x4), status 0x170, script IP = file + offset (base + current), volume
// accumulator 0x7F<<24, expression 0x6000, pan 0x4000, envelope run flags
// cleared, WDS bank bound + instrument 0 primed, voice parked stopped. A
// zero offset deactivates the slot. NOTE (retail-faithful): voice numbers
// lag the element index by one -- element 0 (the conductor track) gets
// voice 0xFF, which every voice-indexed path bounds-checks away.
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; the matching
 * build keeps INCLUDE_ASM (byte-exact) below. Residual vs {}: the same cc1
 * anchor-rebase family as C6E8/EBF0 -- retail keeps the element-field
 * cluster on the voice_data cursor (mgr+0xC4) while this pipeline's cc1
 * rebases the induction family onto a different anchor (last-use / derived-
 * giv), register-permuting the loop. Same operations at the same absolute
 * element offsets; semantics audited field-by-field against the split asm
 * (offset map in the body comments). Not claimed as {}. */
void func_8003B424(AudioManager* manager) {
    AudioElement* el;
    SoundVoiceData* vd;
    u16* pOffset;
    SoundFile* pFile;
    SoundWDSEntry* pBank;
    u32 mask;
    s32 index;
    s32 voice;
    s32 count;

    count = manager->elementCount;
    el = &manager->elements[0];
    if (count == 0) {
        return;
    }
    index = 0;
    voice = -1;                         /* element 0 = conductor, no voice */
    mask = 0;
    pFile = SOUND_PSX_TO_PTR(SoundFile, manager->unk_0x8);
    pOffset = (u16*)((u8*)pFile + 0x22);
    pBank = SoundFindWdsEntry(*(s16*)&manager->unk_0x15[1]);
    if (pBank == NULL) {
        pBank = g_SoundWdsLinkedList;
    }
    do {
        vd = &el->voice_data;
        if (*pOffset != 0) {
            u32 bit = 1 << index;
            u8* ip;

            mask |= bit;
            if (bit & *(u32*)&manager->unk_0x4c[0]) {
                el->active_flag = 0x421;            /* asm: sh 0x421, el+0x00 */
            } else {
                el->active_flag = 0x401;            /* asm: sh 0x401, el+0x00 */
            }
            if (*(u16*)&manager->unk_Flags & 0x4) {
                el->active_flag |= 0x4;             /* asm: lhu/ori 4/sh el+0x00 */
            }
            el->status_flags = 0x170;               /* asm: sh 0x170, el+0x02 */
            el->unk_0x04 = 0;                       /* asm: sh 0,     el+0x04 */
            el->unk_0x06[1] = 0x10;                 /* asm: sb 0x10,  el+0x07 */
            el->unk_0x06[0] = index;                /* asm: sb s5,    el+0x06 */
            *(u32*)&el->unk_0x06[2] =               /* asm: sw,       el+0x08 */
                *(u16*)((u8*)pFile + 0x10);         /*   = song id (file+0x10) */
            el->octave = 0x3C;                      /* asm: sh 0x3C,  el+0x66 */
            el->unk_0x62 = 0xF;                     /* asm: sh 0xF,   el+0x62 */
            *(u16*)&el->unk_0x70[2] = 0xFFFF;       /* asm: sh,       el+0x72 */
            *(u16*)&el->unk_0x76[0] = 0x6000;       /* asm: sh,       el+0x76 (expression) */
            *(s32*)&el->unk_0x76[2] = 0x7F000000;   /* asm: sw,       el+0x78 (vol accum) */
            el->savedScriptIP = 0;                  /* asm: sw 0,     el+0x18 */
            *(u32*)&el->unk_0x1C[0] = 0;            /* asm: sw 0,     el+0x1C */
            *(u16*)&el->unk_0x1C[4] = 0;            /* asm: sh 0,     el+0x20 */
            el->unk_0x1C[6] = 0;                    /* asm: sb 0,     el+0x22 */
            el->fermataDuration = 0;                /* asm: sh 0,     el+0x5C */
            el->unk_0x5E[2] = 0;                    /* asm: sb 0,     el+0x60 */
            el->unk_0x6E = 0;                       /* asm: sh 0,     el+0x6E */
            el->unk_0x64[0] = 0;                    /* asm: sb 0,     el+0x64 */
            el->unk_0x74 = 0x4000;                  /* asm: sh 0x4000,el+0x74 (pan) */
            *(u16*)&el->unk_0x70[0] = 0;            /* asm: sh 0,     el+0x70 */
            *(u16*)&el->unk_0xD0[0] = 0;            /* asm: sh 0,     el+0xD0 */
            *(u16*)&el->unk_0xD0[2] = 0;            /* asm: sh 0,     el+0xD2 */
            *(u16*)&el->unk_0xD0[4] = 0;            /* asm: sh 0,     el+0xD4 */
            *(u16*)&vd->unkC[0] = 0;                /* asm: sh 0,     el+0x3C */
            *(u16*)&vd->unkC[2] = 0;                /* asm: sh 0,     el+0x3E */
            el->unk_0xCE = 0;                       /* asm: sh 0,     el+0xCE */
            ip = (u8*)pFile + *pOffset;
            *(SoundPsxAddress*)&el->unk_0x06[0xA] = SOUND_PTR_TO_PSX(ip); /* el+0x10 */
            el->unk14 = SOUND_PTR_TO_PSX(ip);       /* asm: sw,       el+0x14 (tick IP) */
            {
                /* Clear the four envelope run-flag halfwords: el+0x156,
                 * 0x136, 0x116, 0xF6 (asm walks down from el+0x60+0xF6). */
                u8* q = (u8*)el + 0x60;
                s32 i = 3;
                do {
                    *(u16*)(q + 0xF6) = 0;
                    q -= 0x20;
                } while (--i >= 0);
            }
            el->unk2C = SOUND_PTR_TO_PSX(pBank);    /* asm: sw,       el+0x2C */
            ((u8*)&el->unk_0x24)[1] = manager->unk_0x15[1]; /* sb mgr+0x16 -> el+0x25 */
            if (pBank != NULL) {
                func_8003E5BC(0, el);
            }
            el->voice_number = voice;               /* asm: sb s4,    el+0x27 */
            vd->modeFlags = 0;                      /* asm: sh 0,     el+0x32 */
            vd->priority = 0x100;                   /* asm: sh 0x100, el+0x34 */
            SoundAssignVoiceToChannelAndStop(vd, voice);
        } else {
            el->active_flag = 0;
        }
        pOffset++;
        el++;
        index++;
        voice++;
        count--;
    } while (count != 0);
    manager->unk_0x48 = mask;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003B424);
#endif

#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; the matching
 * build keeps INCLUDE_ASM (byte-exact) below. Residual vs {}: the B424/C6E8/EBF0 cc1 anchor-rebase residual on the element-arm loop (expected: B424 is its twin); every store audited against the split asm offsets.
 * Not claimed as {}. */
// SFX element-arm (the twin of the music arm loop func_8003B424, driven by
// a SED entry instead of a song file): find the SED by id, bind its WDS
// bank, scale the requested volume by the entry's authored volume byte
// (clamped 0x7FFF), then arm D_80059404 consecutive elements from the
// spec's low byte -- each SED entry carries one script offset halfword per
// element (base sed+0x20). A zero offset releases that element instead.
// Element fields ride the q (= &voice_number) cursor, matching retail's
// anchor; el carries active_flag/envelope base/the E5BC argument and vd the
// voice-call argument.
void func_8003B644(s32 elementSpec, s32 packedId, s32 volume, s32 pan) {
    SoundFile* sed;
    AudioManager* manager;
    AudioElement* el;
    SoundVoiceData* vd;
    u8* q;
    u8* bank;
    u16* pOffset;
    s32 count;
    s32 vol;
    u8 prio;

    sed = g_SoundSedsLinkedList;
    manager = SOUND_PSX_TO_PTR(AudioManager, D_800595D8);
    while (sed->sedId != (s16)(packedId >> 16)) {
        sed = SOUND_PSX_TO_PTR(SoundFile, sed->pNext);
        if (sed == NULL) {
            return;
        }
    }
    bank = (u8*)SoundFindWdsEntry(sed->sndId);
    if (bank == NULL) {
        bank = (u8*)g_SoundWdsLinkedList;
    }
    vol = ((s16)volume *
           *((u8*)sed + *(u16*)((u8*)sed + 0x18) + (packedId & 0xFFFF))) >> 7;
    if ((vol >> 15) & 0x1) {
        vol = 0x7FFF;
    }
    prio = (u32)elementSpec >> 8;
    pOffset = (u16*)((u8*)sed + ((packedId & 0xFFFF) << 2) + 0x20);
    el = (AudioElement*)((u8*)manager + (elementSpec & 0xFF) * 0x158 + 0x94);
    q = (u8*)el + 0x27;
    vd = &el->voice_data;
    count = D_80059404;
    DisableEvent(g_unk_SoundEvent);
    do {
        *(u32*)(q - 0x1F) = packedId;                 /* el+0x08 id */
        *(u32*)(q - 0x1B) = D_80059504;               /* el+0x0C start serial */
        *(q - 0x20) = prio;                           /* el+0x07 priority */
        if (*pOffset != 0) {
            u8* ip;

            manager->unk_0x48 |= 1 << *(q - 0x21);
            el->active_flag = 0x409;
            if (*(u16*)((u8*)sed + 0x10) & 0x1) {
                el->active_flag = 0x40B;
            }
            *(u16*)(q - 0x25) = 0x170;                /* el+0x02 status */
            *(u16*)(q - 0x23) = 0;                    /* el+0x04 */
            *(u16*)(q + 0x3F) = 0x3C;                 /* el+0x66 octave */
            *(u16*)(q + 0x3B) = 0xF;                  /* el+0x62 */
            *(u16*)(q + 0x4B) = 0xFFFF;               /* el+0x72 */
            *(u32*)(q - 0xF) = 0;                     /* el+0x18 */
            *(u32*)(q - 0xB) = 0;                     /* el+0x1C */
            *(u16*)(q - 0x7) = 0;                     /* el+0x20 */
            *(q - 0x5) = 0;                           /* el+0x22 */
            *(u16*)(q + 0x35) = 0;                    /* el+0x5C */
            *(q + 0x39) = 0;                          /* el+0x60 */
            *(u16*)(q + 0x47) = 0;                    /* el+0x6E */
            *(q + 0x3D) = 0;                          /* el+0x64 */
            *(u16*)(q + 0x4F) = vol;                  /* el+0x76 expression */
            *(s32*)(q + 0x51) = 0x7F000000;           /* el+0x78 vol accum */
            *(u16*)(q + 0x15) = 0;                    /* el+0x3C */
            *(u16*)(q + 0x17) = 0;                    /* el+0x3E */
            *(u16*)(q + 0xA9) = 0;                    /* el+0xD0 */
            *(u16*)(q + 0xAB) = 0;                    /* el+0xD2 */
            *(u16*)(q + 0xAD) = 0;                    /* el+0xD4 */
            *(u16*)(q + 0xA7) = 0;                    /* el+0xCE */
            *(u16*)(q + 0x4D) = pan;                  /* el+0x74 */
            ip = (u8*)sed + *pOffset;
            *(u32*)(q - 0x17) = SOUND_PTR_TO_PSX(ip); /* el+0x10 */
            *(u32*)(q - 0x13) = SOUND_PTR_TO_PSX(ip); /* el+0x14 tick IP */
            {
                u8* p = (u8*)el + 0x60;
                s32 i = 3;
                do {
                    *(u16*)(p + 0xF6) = 0;
                    p -= 0x20;
                } while (--i >= 0);
            }
            *(u32*)(q + 0x5) = SOUND_PTR_TO_PSX(bank);/* el+0x2C */
            *(q - 0x2) = *((u8*)sed + 0x16);          /* el+0x25 bank id */
            if (bank != NULL) {
                func_8003E5BC(0, el);
            }
            *(u16*)(q + 0xB) = 0;                     /* vd modeFlags */
            *(u16*)(q + 0xD) = 0x200;                 /* vd priority */
            SoundAssignVoiceToChannelAndStop(vd, q[0]);
        } else {
            manager->unk_0x48 &= ~(1 << *(q - 0x21));
            el->active_flag = 0;
            SoundReleaseVoiceFromChannel(vd, q[0]);
        }
        pOffset++;
        vd = (SoundVoiceData*)((u8*)vd + 0x158);
        q += 0x158;
        count--;
        el = (AudioElement*)((u8*)el + 0x158);
    } while (count != 0);
    *(u16*)&manager->unk_Flags |= 0x8000;
    EnableEvent(g_unk_SoundEvent);
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003B644);
#endif

/* Frees the chain hanging off manager->unk_Manager_0x4. Retail reuses the
 * manager register as the next-entry holder, hence the one variable. */
void func_8003B930(AudioManager* manager) {
    AudioManager* pEntry;

    pEntry = SOUND_PSX_TO_PTR(AudioManager, manager->unk_Manager_0x4);
    if (pEntry != NULL) {
        manager->unk_Manager_0x4 = SOUND_PTR_TO_PSX(NULL);
        do {
            manager = SOUND_PSX_TO_PTR(AudioManager, pEntry->unk_Manager_0x4);
            SoundHeapFree(pEntry);
            pEntry = manager;
        } while (manager != NULL);
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
void func_8003BB08(u16 mask, void* pData) {
#ifdef XENO_PC_PORT
    u16* pEntry = (u16*)((u8*)pData + 0x94);
    s32 count = *(u8*)((u8*)pData + 0x14);
    while (count-- != 0) {
        if (*pEntry != 0) {
            *pEntry |= mask;
        }
        pEntry += 0xAC; /* stride 0x158 / 2 */
    }
#else
    u8 *p = (u8 *)pData + 0x94;
    s32 count = *(u8 *)((u8 *)pData + 0x14);
    do {
        count--;
        if (*(u16 *)p != 0) {
            *(u16 *)p = mask | *(u16 *)p;
        }
        p += 0x158;
    } while (count != 0);
#endif
}

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
#ifdef XENO_PC_PORT
        /* Retail's interrupt mask kept the 240Hz tick out of the queue-index
         * writes below; the port's EnterCriticalSection is a global no-op, so
         * take the tick gate here (caught as a real TSan race when the field
         * music path went live). */
        PsyX_Sys_SoundGateEnterCritical();
#endif
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
#ifdef XENO_PC_PORT
        PsyX_Sys_SoundGateExitCritical();
#endif
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

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): sound_sync_transfer (src/decomp/src/resident/sound.c:1664). */
#ifndef XM_T_SoundTransfer
#define XM_T_SoundTransfer
typedef struct SoundTransfer {
    u16 type;          
    u16 unk2;
    u8 *data;
    u32 address;       
    s32 size;
    void (*callback)(void);
} SoundTransfer;
#endif
#ifndef XM_S_sound_sync_transfer
#define XM_S_sound_sync_transfer
s32 xm_sound_sync_transfer(s32 wait) __asm__("func_8003BDFC");
#endif
#ifndef XM_S_sound_driver_flags
#define XM_S_sound_driver_flags
extern s16 xm_sound_driver_flags __asm__("g_SoundControlFlags");
#endif
#ifndef XM_S_sound_transfer_ring
#define XM_S_sound_transfer_ring
extern SoundTransfer *xm_sound_transfer_ring __asm__("g_SoundTransferQueue");
#endif
#ifndef XM_S_sound_transfer_ring_read_index
#define XM_S_sound_transfer_ring_read_index
extern u16 xm_sound_transfer_ring_read_index __asm__("g_SoundTransferQueueReadIndex");
#endif
s32 func_8003BDFC(s32 wait) {
    if (wait & 0x10) {
    busy:
        if (xm_sound_driver_flags & 0x10) {
            goto busy;
        }
    }
    if (xm_sound_driver_flags & 0x10) {
        return (s16)xm_sound_transfer_ring[xm_sound_transfer_ring_read_index].type;
    }
    return 0;
}
#endif
#else
s16 func_8003BDFC(s32 flags) {
    if (flags & 0x10) {
        while (g_SoundControlFlags & 0x10) {}
    }
    if (g_SoundControlFlags & 0x10) {
        u8* queue = (u8*)g_SoundTransferQueue;
        s32 idx = g_SoundTransferQueueReadIndex;
        return *(s16*)(queue + idx * 20);
    }
    return 0;
}
#endif

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
u8* func_8003CD08(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    pAudioElements->fermataDuration = pScript[0];
    pAudioElements->active_flag |= 0x400;
    pAudioElements->status_flags |= 0x2;
    return pScript + 1;
}

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
u8* func_8003CE68(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
#ifdef XENO_PC_PORT
    s32 denom = pScript[1];
#else
    register s32 denom asm("$6") = pScript[1];
#endif
    s32 beats = pScript[0];
    pAudioManager->unk_0x3a = 0xC0 / denom;
    pAudioManager->unk_0x3c = denom;
    pAudioManager->unk_0x38 = beats;
    pAudioManager->unk_0x3e = beats;
    pAudioManager->unk_0x36 = pAudioManager->unk_0x3a;
    return pScript + 2;
}

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
/* MATCHED C, byte-exact with this TU's pinned gcc-2.6.0 + maspsx.
 * The selector must be re-read from memory (`lhu $a1, 0x72($a2)` at
 * 0x8003D700) right after the increment store (`sh $v1, 0x72($a2)` at
 * 0x8003D6FC) instead of being cached in a local; the sibling func_8003CF38
 * below uses the same idiom. Verified instruction-for-instruction against the
 * retail slice at file offset 0x2D6F0 (72 B);
 * pc_port/tests/run_sound_voice_record_cef0_retail_test.sh re-checks it. */
u8* func_8003CEF0(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8* rec;
    *(u16*)&pAudioElements->unk_0x70[2] += 1;
    rec = (u8*)pAudioElements + (*(u16*)&pAudioElements->unk_0x70[2] * 12 + 0x9C);
    rec[0] = *pScript++ + 0xFF;
    *(u32*)(rec + 4) = SOUND_PTR_TO_PSX(pScript);
    rec[2] = pAudioElements->octave;
    return pScript;
}

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
        u16 off = *(u16*)(((q + ((s16)pair << 1)) << 1) + (u32)e + 0x20);
        pScript = (u8*)e + off;
    }
    return pScript + 3;
}

// Set tempo handler?
// Seq cmd: set manager volume (immediate).
/* MATCHED C, byte-exact with this TU's pinned gcc-2.6.0 + maspsx.
 * Retail (file offset 0x2D8E8, 40 B) masks the loaded volume byte with a
 * redundant `andi $v0,$v0,0xFF` before `mult`; under gcc-2.6.0 a plain
 * `pScript[0]` folds the lbu+zero-extend into a single lbu, so the byte is
 * read through a *non-combinable* (volatile) lvalue to keep the lbu and its
 * zero-extension as separate instructions, reproducing retail exactly.
 * Verified instruction-for-instruction against the retail slice; see
 * pc_port/tests/run_sound_voice_volume_d0e8_retail_test.sh. */
u8* func_8003D0E8(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    s32 vol = *(volatile u8*)pScript;
    pAudioManager->unk_0x54 = vol * ((s16*)&pAudioManager->unk_Interpolator_0x64.currentValue)[1];
    pAudioManager->unk_0x58 = vol << 16;
    return pScript + 1;
}

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
// interpolator at manager+0x58. steps is an s16: widening it for the
// division is retail's `move a2,a3`.
u8* func_8003D13C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    s32 target = pScript[1];
    s16 steps = pScript[0];
    s32 diff;

    *(u16*)&pAudioManager->unk_0x62 = target;
    diff = (target << 16) - pAudioManager->unk_0x58;
    if (steps != 0 && diff != 0) {
        pAudioManager->unk_0x5c = diff / steps;
        pAudioManager->unk_0x60 = steps;
    }
    return pScript + 2;
}

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
void *func_8003D1BC(void *arg0, void *arg1) {
    s16 temp_a0_2;
    s32 temp_v0;
    u8 temp_a0;
    u8 temp_a3;

    temp_a0 = *(u8 *)((s8*)(arg0) + 0);
    temp_a3 = *(u8 *)((s8*)(arg0) + 1);
    temp_a0_2 = temp_a0 << 5;
    temp_v0 = (temp_a3 << 0x18) - *(s32 *)((s8*)(arg1) + 0x70);
    if ((temp_a0_2 != 0) && (temp_v0 != 0)) {
        *(s16 *)((s8*)(arg1) + 0x78) = (s16) (temp_a0 << 5);
        *(s16 *)((s8*)(arg1) + 0x7A) = (s16) (temp_a3 << 8);
        *(s32 *)((s8*)(arg1) + 0x74) = (s32) (temp_v0 / temp_a0_2);
    }
    return arg0 + 2;
}

#endif

u8* SoundScriptSetUnk62(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    pAudioElements->unk_0x62 = *pScript;
    return pScript + 1;
}

// Seq cmd 0xAA: move the element to another voice (release + assign-stopped).
u8* func_8003D21C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    s32 n = *pScript++;
    if (n < 0x19) {
        SoundVoiceData* vd = &pAudioElements->voice_data;
        SoundReleaseVoiceFromChannel(vd, pAudioElements->voice_number);
        SoundAssignVoiceToChannelAndStop(vd, pAudioElements->voice_number = n);
    }
    return pScript;
}

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
u8* func_8003D3D8(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    AudioElement* el = pAudioElements;
    *(u16*)&pAudioManager->unk_0x1c[0] = pScript[0];
    SpuSetNoiseClock((pScript++, *(u16*)&pAudioManager->unk_0x1c[0]));
    el->voice_data.flags |= 0x2000;
    el->voice_data.modeFlags |= 0x20;
    return pScript;
}

// Seq cmd 0xB5: nudge the noise clock (mod 64); flag the voice for noise.
u8* func_8003D438(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    AudioElement* el = pAudioElements;
    *(u16*)&pAudioManager->unk_0x1c[0] =
        (pScript[0] + *(u16*)&pAudioManager->unk_0x1c[0]) & 0x3F;
    SpuSetNoiseClock((pScript++, *(u16*)&pAudioManager->unk_0x1c[0]));
    el->voice_data.flags |= 0x2000;
    el->voice_data.modeFlags |= 0x20;
    return pScript;
}

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
u8* func_8003D4E4(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8 mode = pScript[0];
#ifdef XENO_PC_PORT
    s32 depthL;
    s32 depthR;
#else
    register s32 depthL asm("$6");
    register s32 depthR asm("$7");
#endif
    *(u16*)&pAudioManager->unk_0x40[4] = mode << 8;
    depthL = ((s8*)pScript)[1];
    pAudioManager->unk_0x40[2] = depthL;
    depthR = ((s8*)pScript)[2];
    pAudioManager->unk_0x40[3] = depthR;
    SoundSetReverbModeWithAllocation(-1, (s8)mode << 8, depthL, depthR);
    return pScript + 3;
}

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
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): sound_seq_reverb_on (src/decomp/src/resident/sound.c:2572). */
#ifndef XM_S_sound_seq_reverb_on
#define XM_S_sound_seq_reverb_on
u8 *xm_sound_seq_reverb_on(u8 *data, SoundSeq *seq, SoundSeqChannel *channel) __asm__("func_8003D53C");
#endif
u8 *func_8003D53C(u8 *data, SoundSeq *seq, SoundSeqChannel *channel) {
    if ((seq->flags & 6) && (!(xm_sound_driver_flags & 0x2000) || (channel->flags & 2))) {
        return data;
    }
    channel->state.flags |= 0x4000;
    channel->state.mode |= 0x40;
    return data;
}
#endif
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
u8* func_8003D60C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8 adsr3;
    pAudioElements->voice_data.unkAdsr1 = pScript[0];
    pAudioElements->voice_data.unkAdsr2 = pScript[1];
    adsr3 = pScript[2];
    pAudioElements->voice_data.flags |= 0x1F0;
    pAudioElements->voice_data.unkAdsr3 = adsr3;
    return pScript + 3;
}

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
u8* func_8003D7C8(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    s32 high = *(volatile u8*)pScript;
    s32 low = *(volatile u8*)(pScript + 1);
    u16 status = *(volatile u16*)&pAudioElements->status_flags;
    s32 offset = ((s16)(high << 8)) + low;
    u16 pitch;

    __asm__ volatile("" : : "r"(offset) : "memory");
    pitch = *(volatile u16*)&pAudioElements->unk_0x6E;

    status |= 0x200;
    pitch += offset;
    pAudioElements->status_flags = status;
    *(u16*)&pAudioElements->unk_0x6E = pitch;
    return pScript + 2;
}

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

// Seq cmd 0xD8: prime envelope object 0 (direct method, signed-square
// target): arg = sign-preserving target^2 << 14.
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; matching build
 * keeps INCLUDE_ASM below. Residual: register-pressure frame shape (one
 * extra callee-saved reg vs retail); ops+offsets audited 1:1 against the
 * split asm (envelope base/field offsets cross-checked across the sibling
 * pair variants). {}-upgrade candidate for a fresh matching session. */
u8* func_8003D8B8(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8* p = pScript;
    u16 n = p[0];
    AudioElement* el = pAudioElements;
    if ((s8)p[1] != 0 && n != 0) {
        SoundEnvelope* env = (SoundEnvelope*)((u8*)el + 0xD8);
        s32 arg;
        s32 sq;
        {
            s32 t = (s8)p[1];
            if (t < 0) {
                arg = t * -t;
            } else {
                arg = t * t;
            }
        }
        sq = (s16)n * (s16)n;
        n = n + sq / 64;
        *(s32*)&env->unk4[8] = func_8003E290(arg << 14, (s16)n, 3);
        env->stepAdd = 0x400;
        *(u16*)&env->unk4[0xE] = n;
        env->pfnHandler = SOUND_PTR_TO_PSX(func_8003F2A0);
        env->unk1D = 3;
        env->flags = 3;
        env->state = 0;
        *(u16*)&env->unk16[0] = p[2] << 2;
        el->unk_0xCE |= 0x1;
        func_8003E3E0(env);
    }
    return p + 3;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003D8B8);
#endif

// Seq cmd 0xD9: prime envelope object 0 (table method, signed-square target).
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; matching build
 * keeps INCLUDE_ASM below. Residual: register-pressure frame shape (one
 * extra callee-saved reg vs retail); ops+offsets audited 1:1 against the
 * split asm (envelope base/field offsets cross-checked across the sibling
 * pair variants). {}-upgrade candidate for a fresh matching session. */
u8* func_8003D9A4(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8* p = pScript;
    u16 n = p[0];
    AudioElement* el = pAudioElements;
    if ((s8)p[1] != 0 && n != 0) {
        SoundEnvelope* env = (SoundEnvelope*)((u8*)el + 0xD8);
        u16 mode;
        u8 m;
        s32 arg;
        s32 sq;
        {
            s32 t = (s8)p[1];
            if (t < 0) {
                arg = t * -t;
            } else {
                arg = t * t;
            }
        }
        sq = (s16)n * (s16)n;
        n = n + sq / 64;
        mode = ((p[2] & 0x10) == 0) << 1;
        m = p[2] & 0xF;
        *(s32*)&env->unk4[8] = func_8003E290(arg << 14, (s16)n, m);
        env->stepAdd = 0x400;
        *(u16*)&env->unk4[0xE] = n;
        *(u16*)&env->unk16[0] = 0;
        env->unk1D = m;
        env->state = 0;
        env->flags = mode + 1;
        env->pfnHandler = D_800508A4[m];
        el->unk_0xCE |= 0x1;
        func_8003E3E0(env);
    }
    return p + 3;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003D9A4);
#endif

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
u8* func_8003DB2C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u16 f4;
    u16 status;
    f4 = pAudioElements->unk_0x04;
    *(s32*)&pAudioElements->unk_0x76[2] = pScript[0] << 24;
    status = pAudioElements->status_flags;
    pAudioElements->status_flags = status | 0x100;
    pAudioElements->unk_0x04 = f4 & 0xFEF7;
    return pScript + 1;
}

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
/* Coexistence: logic-verified port C body; matching build keeps INCLUDE_ASM
 * below because cc1 schedules the third script byte differently from retail. */
u8* func_8003DBE4(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8* p = pScript;
    s32 from = (s32)((u32)p[0] << 24);
    s32 diff = (s32)((u32)((u32)p[2] << 24) - (u32)from);
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
void *func_8003DBE4(void *arg0, s32 arg1, void *arg2) {
    s32 temp_v0;
    s16 temp_t0;
    s32 temp_v1;

    temp_v1 = *(u8 *)((s8*)(arg0) + 0) << 24;
    temp_t0 = *(u8 *)((s8*)(arg0) + 1);
    temp_v0 = (*(u8 *)((s8*)(arg0) + 2) << 0x18) - temp_v1;
    if ((temp_v0 != 0) && (temp_t0 != 0)) {
        *(s16 *)((s8*)(arg2) + 0x82) = (s16) (temp_v1 >> 16);
        *(s16 *)((s8*)(arg2) + 0x80) = (s16) temp_t0;
        *(u16 *)((s8*)(arg2) + 4) = (u16) ((*(u16 *)((s8*)(arg2) + 4) | 0x100) & 0xFFF7);
        *(s32 *)((s8*)(arg2) + 0x7C) = (s32) (temp_v0 / (s32) temp_t0);
    } else {
        *(u16 *)((s8*)(arg2) + 4) = (u16) (*(u16 *)((s8*)(arg2) + 4) & 0xFEFF);
    }
    return arg0 + 3;
}

#endif

// Seq cmd 0xE4: prime envelope object 1 (direct method) -- rate =
// n + n*n/64, packed target via func_8003E290, arm + notify func_8003E3E0.
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; matching build
 * keeps INCLUDE_ASM below. Residual: register-pressure frame shape (one
 * extra callee-saved reg vs retail); ops+offsets audited 1:1 against the
 * split asm (envelope base/field offsets cross-checked across the sibling
 * pair variants). {}-upgrade candidate for a fresh matching session. */
u8* func_8003DC50(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8* p = pScript;
    u16 n = p[0];
    AudioElement* el = pAudioElements;
    if ((s8)p[1] != 0 && n != 0) {
        SoundEnvelope* env = (SoundEnvelope*)((u8*)el + 0xF8);
        s32 sq = n * n;
        n = n + sq / 64;
        *(s32*)&env->unk4[8] = func_8003E290((s8)p[1] << 24, (s16)n, 2);
        env->stepAdd = 0x400;
        *(u16*)&env->unk4[0xE] = n;
        env->pfnHandler = SOUND_PTR_TO_PSX(func_8003F240);
        env->unk1D = 2;
        env->state = 1;
        env->flags = 3;
        *(u16*)&env->unk16[0] = p[2] << 2;
        el->unk_0xCE |= 0x2;
        func_8003E3E0(env);
    }
    return p + 3;
}
#else
void *func_8003DC50(void *arg0, s32 arg1, void *arg2) {
    s32 byte2;
    s32 var_a1;
    s8 temp_a0;
    s16 temp_s1;

    temp_a0 = *(s8 *)((s8*)(arg0) + 1);
    temp_s1 = *(u8 *)((s8*)(arg0) + 0);
    if ((temp_a0 != 0) && (temp_s1 != 0)) {
        var_a1 = temp_s1 * temp_s1;
        if (var_a1 < 0) {
            var_a1 += 0x3F;
        }
        temp_s1 = temp_s1 + (var_a1 >> 6);
        *(s32 *)((s8*)(arg2) + 0x104) = func_8003E290(temp_a0 << 0x18, temp_s1, 2);
        *(s16 *)((s8*)(arg2) + 0x10A) = temp_s1;
        byte2 = *(u8 *)((u8 *)arg0 + 2);
        *(s16 *)((s8*)(arg2) + 0x112) = 0x400;
        *(s32 **)((s8*)(arg2) + 0xF8) = &func_8003F240;
        *(s8 *)((s8*)(arg2) + 0x115) = 2;
        *(s8 *)((s8*)(arg2) + 0x114) = 1;
        *(s16 *)((s8*)(arg2) + 0x116) = 3;
        *(s16 *)((s8*)(arg2) + 0x10E) = (s16) (byte2 * 4);
        *(u16 *)((s8*)(arg2) + 0xCE) = (u16) (*(u16 *)((s8*)(arg2) + 0xCE) | 2);
        func_8003E3E0(arg2 + 0xF8);
    }
    return arg0 + 3;
}

#endif

// Seq cmd 0xE5: prime envelope object 1 (method from the D_800508A4
// table, low nibble of p2 selects; bit4 of p2 selects one-shot vs looping).
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; matching build
 * keeps INCLUDE_ASM below. Residual: register-pressure frame shape (one
 * extra callee-saved reg vs retail); ops+offsets audited 1:1 against the
 * split asm (envelope base/field offsets cross-checked across the sibling
 * pair variants). {}-upgrade candidate for a fresh matching session. */
u8* func_8003DD24(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8* p = pScript;
    u16 n = p[0];
    AudioElement* el = pAudioElements;
    if ((s8)p[1] != 0 && n != 0) {
        SoundEnvelope* env = (SoundEnvelope*)((u8*)el + 0xF8);
        u16 mode;
        u8 m;
        s32 sq = n * n;
        n = n + sq / 64;
        mode = ((p[2] & 0x10) == 0) << 1;
        m = p[2] & 0xF;
        *(s32*)&env->unk4[8] = func_8003E290((s8)p[1] << 24, (s16)n, m);
        env->stepAdd = 0x400;
        *(u16*)&env->unk4[0xE] = n;
        *(u16*)&env->unk16[0] = 0;
        env->state = 1;
        env->unk1D = m;
        env->flags = mode + 1;
        env->pfnHandler = D_800508A4[m];
        el->unk_0xCE |= 0x2;
        func_8003E3E0(env);
    }
    return p + 3;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003DD24);
#endif

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

// Seq cmd 0xEC: prime envelope object 2 (direct method) -- rate =
// n + n*n/64, packed target via func_8003E290, arm + notify func_8003E3E0.
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; matching build
 * keeps INCLUDE_ASM below. Residual: register-pressure frame shape (one
 * extra callee-saved reg vs retail); ops+offsets audited 1:1 against the
 * split asm (envelope base/field offsets cross-checked across the sibling
 * pair variants). {}-upgrade candidate for a fresh matching session. */
u8* func_8003DF78(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8* p = pScript;
    u16 n = p[0];
    AudioElement* el = pAudioElements;
    if ((s8)p[1] != 0 && n != 0) {
        SoundEnvelope* env = (SoundEnvelope*)((u8*)el + 0x118);
        s32 sq = n * n;
        n = n + sq / 64;
        *(s32*)&env->unk4[8] = func_8003E290((s8)p[1] << 24, (s16)n, 3);
        env->stepAdd = 0x400;
        *(u16*)&env->unk4[0xE] = n;
        env->pfnHandler = SOUND_PTR_TO_PSX(func_8003F2A0);
        env->unk1D = 3;
        env->state = 2;
        env->flags = 3;
        *(u16*)&env->unk16[0] = p[2] << 2;
        el->unk_0xCE |= 0x4;
        func_8003E3E0(env);
    }
    return p + 3;
}
#else
void *func_8003DF78(void *arg0, s32 arg1, void *arg2) {
    s32 byte2;
    s32 var_a1;
    s8 temp_a0;
    s16 temp_s1;

    temp_a0 = *(s8 *)((s8*)(arg0) + 1);
    temp_s1 = *(u8 *)((s8*)(arg0) + 0);
    if ((temp_a0 != 0) && (temp_s1 != 0)) {
        var_a1 = temp_s1 * temp_s1;
        if (var_a1 < 0) {
            var_a1 += 0x3F;
        }
        temp_s1 = temp_s1 + (var_a1 >> 6);
        *(s32 *)((s8*)(arg2) + 0x124) = func_8003E290(temp_a0 << 0x18, temp_s1, 3);
        *(s16 *)((s8*)(arg2) + 0x12A) = temp_s1;
        byte2 = *(u8 *)((u8 *)arg0 + 2);
        *(s16 *)((s8*)(arg2) + 0x132) = 0x400;
        *(s32 **)((s8*)(arg2) + 0x118) = &func_8003F2A0;
        *(s8 *)((s8*)(arg2) + 0x135) = 3;
        *(s8 *)((s8*)(arg2) + 0x134) = 2;
        *(s16 *)((s8*)(arg2) + 0x136) = 3;
        *(s16 *)((s8*)(arg2) + 0x12E) = (s16) (byte2 * 4);
        *(u16 *)((s8*)(arg2) + 0xCE) = (u16) (*(u16 *)((s8*)(arg2) + 0xCE) | 4);
        func_8003E3E0(arg2 + 0x118);
    }
    return arg0 + 3;
}

#endif

// Seq cmd 0xED: prime envelope object 2 (method from the D_800508A4
// table, low nibble of p2 selects; bit4 of p2 selects one-shot vs looping).
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body; matching build
 * keeps INCLUDE_ASM below. Residual: register-pressure frame shape (one
 * extra callee-saved reg vs retail); ops+offsets audited 1:1 against the
 * split asm (envelope base/field offsets cross-checked across the sibling
 * pair variants). {}-upgrade candidate for a fresh matching session. */
u8* func_8003E04C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u8* p = pScript;
    u16 n = p[0];
    AudioElement* el = pAudioElements;
    if ((s8)p[1] != 0 && n != 0) {
        SoundEnvelope* env = (SoundEnvelope*)((u8*)el + 0x118);
        u16 mode;
        u8 m;
        s32 sq = n * n;
        n = n + sq / 64;
        mode = ((p[2] & 0x10) == 0) << 1;
        m = p[2] & 0xF;
        *(s32*)&env->unk4[8] = func_8003E290((s8)p[1] << 24, (s16)n, m);
        env->stepAdd = 0x400;
        *(u16*)&env->unk4[0xE] = n;
        *(u16*)&env->unk16[0] = 0;
        env->state = 2;
        env->unk1D = m;
        env->flags = mode + 1;
        env->pfnHandler = D_800508A4[m];
        el->unk_0xCE |= 0x4;
        func_8003E3E0(env);
    }
    return p + 3;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/sound", func_8003E04C);
#endif

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
/* Coexistence (d88f13c pattern): logic-verified port C body; matching build
 * keeps INCLUDE_ASM below. Residual: non-semantic codegen micro-shape
 * (commutative operand order / delay-slot copy placement / register reuse);
 * ops+offsets audited 1:1 against the split asm. */
u8* func_8003E180(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    SoundEnvelope* env;
    u8 state;
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
    state = pScript[2];
    env->stepAdd = 0x400;
    *(u16*)&env->unk16[0] = 0;
    env->state = state;
    return pScript + 3;
}

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
                                           rate, env->unk1D);
    *(u16*)&env->unk4[0xE] = rate;
    return pScript + 3;
}
#else

#ifndef XENO_PC_PORT
void *func_8003E1F8(void *arg0, s32 arg1, void *arg2) {
    s16 temp_a1;
    s32 var_v1;
    u8 temp_v0;
    void *temp_s2;

    temp_v0 = *(u8 *)((s8*)(arg0) + 0);
    var_v1 = temp_v0 * temp_v0;
    temp_s2 = arg2 + ((*(u16 *)((s8*)(arg2) + 0xCC) << 5) + 0xD8);
    if (var_v1 < 0) {
        var_v1 += 0x3F;
    }
    temp_a1 = *(u8 *)((s8*)(arg0) + 0) + (var_v1 >> 6);
    *(s32 *)((s8*)(temp_s2) + 0xC) = func_8003E290((*(s8 *)((s8*)(arg0) + 1) << 0x18) | (*(u8 *)((s8*)(arg0) + 2) << 0x10), temp_a1, *(u8 *)((s8*)(temp_s2) + 0x1D));
    *(s16 *)((s8*)(temp_s2) + 0x12) = temp_a1;
    return arg0 + 3;
}
#endif

#endif

// Envelope target packing: scale the packed target by the step count for
// ramp-style methods (2/3: per-step delta; 4: ping-pong delta over rate-1
// steps); methods 0/1 and out-of-range leave the target absolute.
s32 func_8003E290(s32 target, s32 rate, s32 method) {
    if (target != 0) {
        s16 r = rate;
        if (r != 0) {
            s16 m = method;
            switch (m) {
            case 2:
            case 3:
                target = target / r;
                break;
            case 4:
                if (r != 1) {
                    target = target / (r - 1);
                }
                break;
            }
        }
    }
    return target;
}

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

// Envelope arm/reset: first tick fires immediately (counter 1), direction/
// phase flags cleared, delay reloaded from its programmed value, current
// value zeroed, step reset to the base step.
void func_8003E3E0(SoundEnvelope* env) {
    u16 flags;
    u16 d;
    u16 add;

    *(u16*)&env->unk4[0xC] = 1;
    flags = env->flags;
    d = *(u16*)&env->unk16[0];
    add = env->stepAdd;
    *(s32*)&env->unk4[0] = 0;
    env->flags = flags & 0xFFF3;
    env->delay = d;
    env->step = add;
}

// Seq cmd: disarm envelope object N -- clear its run flag + its bit in the
// element's active-envelope mask.
u8* func_8003E40C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
    u32 n = pScript[0];
    *(u16*)((u8*)pAudioElements + (n << 5) + 0xF6) &= 0xFFFE;
    pAudioElements->unk_0xCE &= ~(1 << n);
    return pScript + 1;
}

// Seq cmd 0xFC: select sample bank + load instrument in one command.
/* Coexistence (d88f13c pattern): logic-verified port C body; matching build
 * keeps INCLUDE_ASM below. Residual: non-semantic codegen micro-shape
 * (commutative operand order / delay-slot copy placement / register reuse);
 * ops+offsets audited 1:1 against the split asm. */
u8* func_8003E44C(u8* pScript, AudioManager* pAudioManager, AudioElement* pAudioElements) {
#ifdef XENO_PC_PORT
    AudioElement* el = pAudioElements;
    u8* p = pScript;
    SoundWDSEntry* e;
    u8 bank = p[0];
    u8 inst = p[1];
#else
    register AudioElement* el asm("$16") = pAudioElements;
    register u8* p asm("$17") = pScript;
    SoundWDSEntry* e;
    u8 bank = p[0];
    register u8 inst asm("$18") = p[1];
#endif
    ((u8*)&el->unk_0x24)[1] = bank;
    e = SoundFindWdsEntry(bank);
    if (e == NULL) {
        e = g_SoundWdsLinkedList;
    }
#ifdef XENO_PC_PORT
    /* New-game template (func_8001B970) can fire MAP14 seq cmd 0xFC
     * before any WDS bank is resident. Retail's mirrored RAM lets a NULL
     * fallback read as zeroed instruments; the host faults. */
    if (e == NULL) {
        return p + 2;
    }
#endif
    el->unk2C = SOUND_PTR_TO_PSX(e);
    func_8003E5BC(inst, el);
    return p + 2;
}

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
#ifdef XENO_PC_PORT
    if (pBank == NULL || (s16)instrument < 0 || (s16)instrument > 0x7F) {
        return;
    }
#endif
    pInstr = pBank + (((s16)instrument << 4) + 0x30);
#ifdef XENO_PC_PORT
    if ((u32)(uintptr_t)pInstr < 0x10000u) {
        return;
    }
#endif
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

// Envelope method 1: gate/square -- on each period toggle the value between
// the target and zero.
s32 func_8003F1A4(SoundEnvelope* env) {
    u16 cnt = *(u16*)&env->unk4[0xC] - 1;
    *(u16*)&env->unk4[0xC] = cnt;
    if (cnt == 0) {
        s32 v = 0;
        *(u16*)&env->unk4[0xC] = *(u16*)&env->unk4[0xE];
        if (*(s32*)&env->unk4[0] == 0) {
            v = *(s32*)&env->unk4[8];
        }
        *(s32*)&env->unk4[0] = v;
    }
    return *(s32*)&env->unk4[0];
}

// Envelope method 2: alternating set -- on each period set the value to the
// target with alternating sign.
s32 func_8003F1EC(SoundEnvelope* env) {
    u16 cnt = *(u16*)&env->unk4[0xC] - 1;
    *(u16*)&env->unk4[0xC] = cnt;
    if (cnt == 0) {
        s32 v = *(s32*)&env->unk4[8];
        *(u16*)&env->unk4[0xC] = *(u16*)&env->unk4[0xE];
        if (env->flags & 0x8) {
            v = -v;
        }
        *(s32*)&env->unk4[0] = v;
        env->flags ^= 0x8;
    }
    return *(s32*)&env->unk4[0];
}

// Envelope method 3: triangle ramp -- accumulate the per-step delta every
// tick, flipping the delta's sign at each period boundary.
s32 func_8003F240(SoundEnvelope* env) {
    u16 cnt = *(u16*)&env->unk4[0xC] - 1;
    s32 v;

    *(u16*)&env->unk4[0xC] = cnt;
    if (cnt == 0) {
        s32 stp = *(s32*)&env->unk4[8];
        *(u16*)&env->unk4[0xC] = *(u16*)&env->unk4[0xE];
        if (env->flags & 0x8) {
            stp = -stp;
        }
        *(s32*)&env->unk4[4] = stp;
        env->flags ^= 0x8;
    }
    v = *(s32*)&env->unk4[0] + *(s32*)&env->unk4[4];
    *(s32*)&env->unk4[0] = v;
    return v;
}

// Envelope method 4: ping-pong ramp -- like the triangle, but after the
// first (attack) leg the period doubles (flag 0x4), so the value swings
// symmetrically about the peak.
s32 func_8003F2A0(SoundEnvelope* env) {
    s32 cnt = *(u16*)&env->unk4[0xC];
    s32 v;

    cnt = cnt - 1;
    if (cnt == 0) {
        u16 flags = env->flags;
        s32 stp;
        /* the reload reuses the counter variable (retail merges the ranges) */
        cnt = *(u16*)&env->unk4[0xE];
        if (flags & 0x4) {
            cnt <<= 1;
        }
        stp = *(s32*)&env->unk4[8];
        *(s32*)&env->unk4[4] = stp;
        if (flags & 0x8) {
            *(s32*)&env->unk4[4] = -stp;
        }
        flags = (flags | 0x4) ^ 0x8;
        env->flags = flags;
    }
    v = *(s32*)&env->unk4[0] + *(s32*)&env->unk4[4];
    *(u16*)&env->unk4[0xC] = cnt;
    *(s32*)&env->unk4[0] = v;
    return v;
}

// Envelope method 5 (slots 4 and 5): sawtooth -- accumulate the per-step
// delta, snapping back to zero at each period boundary.
s32 func_8003F308(SoundEnvelope* env) {
    u16 cnt = *(u16*)&env->unk4[0xC] - 1;
    *(u16*)&env->unk4[0xC] = cnt;
    if (cnt == 0) {
        *(s32*)&env->unk4[0] = 0;
        *(u16*)&env->unk4[0xC] = *(u16*)&env->unk4[0xE];
    } else {
        *(s32*)&env->unk4[0] = *(s32*)&env->unk4[0] + *(s32*)&env->unk4[8];
    }
    return *(s32*)&env->unk4[0];
}

// Envelope method 6: random level -- each period, sample the xorshift RNG
// and scale the target by it (advancing the RNG every tick keeps the
// sequence hot, as retail does).
s32 func_8003F354(SoundEnvelope* env) {
    u16 cnt;

    func_8003F43C();
    cnt = *(u16*)&env->unk4[0xC] - 1;
    *(u16*)&env->unk4[0xC] = cnt;
    if (cnt == 0) {
        s32 r;
        *(u16*)&env->unk4[0xC] = *(u16*)&env->unk4[0xE];
        r = func_8003F43C();
        *(s32*)&env->unk4[0] = (*(s32*)&env->unk4[8] >> 15) * r;
    }
    return *(s32*)&env->unk4[0];
}

// Envelope method 7: random bipolar -- each period, a random value scaled
// by the target and re-centered to swing about zero.
s32 func_8003F3C0(SoundEnvelope* env) {
    u16 cnt = *(u16*)&env->unk4[0xC] - 1;
    *(u16*)&env->unk4[0xC] = cnt;
    if (cnt == 0) {
        s32 r;
        s32 t;
        *(u16*)&env->unk4[0xC] = *(u16*)&env->unk4[0xE];
        r = func_8003F43C();
        t = *(s32*)&env->unk4[8];
        *(s32*)&env->unk4[0] = (t >> 14) * r - t;
    }
    return *(s32*)&env->unk4[0];
}

extern s32 D_800594E4;

void func_8003F42C(s32 arg0) {
    D_800594E4 = arg0;
}

// Sound xorshift RNG (seed D_800594E4, set 0x12345678 at SoundInitialize):
// v ^= v<<17; v ^= v>>15 (arithmetic); 15-bit result.
s32 func_8003F43C(void) {
    s32 v = D_800594E4;
    v ^= v << 17;
    v ^= v >> 15;
    D_800594E4 = v;
    return v & 0x7FFF;
}

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


extern SoundFile D_80050910;
extern u16 D_80050924[];
extern SoundWDSEntry D_80050940;

/* MATCHED C for the matching build; the same body now serves the PC port.
 * It previously sat inside `#ifndef XENO_PC_PORT`, so the port stubbed the
 * symbol and the live field path logged `[stub] SoundHandleError`. That stub
 * was load-bearing: it fired because SoundLoadWdsFile could not allocate the
 * field's common WDS bank (a duplicate load -- see misc3.c, fixed by asserting
 * retail's D_8004F364=1 invariant now that port_main replays the retail
 * func_80019578 boot bank closure). With the duplicate gone the handler is no
 * longer reached on the live path, and all five callees plus the three data
 * symbols resolve in the port link (func_8003BDFC is the port-owned
 * transfer-polling override). */
void SoundHandleError(s32 errorId)
{
    if ((g_SoundControlFlags & 0x88) == 0) {
        g_SoundControlFlags |= 8;
        g_SoundSpuErrorId = errorId;
        SoundSpuMemoryFreeBlock(0x10000);
        SoundLoadWdsFile(&D_80050940, 0);
        SoundAddSedsEntry(&D_80050910);
        func_8003BDFC(0x10);
        func_80039E60((D_80050924[0] << 16) | 1);
    }
}

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
