#include "common.h"
#include "field/actor.h"
#include "field/main.h"
#include "field/text_box.h"
#include "field/effects.h"
#include "system/memory.h"
#include "system/archive.h"
#include "system/sound.h"
#include "system/math.h"
#include "psyq/libetc.h"
#include "psyq/libcd.h"
/* Retail inlines the libgte macro form of NormalClip (6 inlined NCLIP in
 * func_8007CD80, zero `jal NormalClip`). Use the same PSY-Q inline macros in
 * the matching build; the port resolves these to PsyCross' equivalents. */
#ifdef XENO_PC_PORT
#include <psx/inline_c.h>
#include <psx/gtemac.h>
/* PsyCross gte_ldsxy3 dereferences its arguments; PSY-Q mtc2's packed SXY
 * values. Route the matching-shape macro onto NormalClip() so the live
 * path does not SEGV. Matching still uses the PSY-Q inline form. */
#undef gte_NormalClip
#define gte_NormalClip(r1, r2, r3, r4) \
    do { *(r4) = NormalClip((int)(r1), (int)(r2), (int)(r3)); } while (0)
#else
#include "psyq/inline_c.h"
#include "psyq/gtemac.h"
#endif
#ifdef XENO_PC_PORT
#include <stdint.h> /* uintptr_t for LP64 pointer widening below */
#include <stdio.h>
#include <stdlib.h> /* getenv for the env-gated party-pointer diagnostic */
#endif

/* Per-frame helper: rand seed, map-load check, angle-step timer. */
extern s32 D_8004F308, D_8004F324, D_800ADC18;
extern int func_80085C90(int);
#ifndef XENO_PC_PORT
/* func_80078B5C: the unfilled beqz delay slot before the epilogue means v0 is
 * live at exit: retail defined this as returning int (no return statement).
 * The TU declares it void, so the definition goes through an asm alias. */
s32 func_80078B5C__s(void) asm("func_80078B5C");

s32 func_80078B5C__s(void) {
    rand();
    if (D_8004F308 == -1) {
        D_8004F308 = func_80085C90(D_8004F324);
    }
    if (D_800ADC18 != 0) {
        D_800ADC18 -= 1;
    }
}
#else
void func_80078B5C(void) {
    rand();
    if (D_8004F308 == -1) {
        D_8004F308 = func_80085C90(D_8004F324);
    }
    if (D_800ADC18 != 0) {
        D_800ADC18--;
    }
}
#endif /* XENO_PC_PORT */

extern s32 D_800ADB2C;
extern s32 D_800ADB90;
extern s32 D_800ADB34;
extern s32 D_800ADBC4;

#ifndef XENO_PC_PORT
/* func_80078BC8: after the ArchiveDataSync call retail sets v0 = -1 once (in the
 * first bnez delay slot) and runs the remaining tests through v1, i.e. the result
 * variable lives in v0 throughout.  Plain C allocates it to v1 (the short test
 * temporaries win v0), so the result is pinned with register asm("$2"), the same
 * register-pin idiom the tree already uses. */
#endif
s32 func_80078BC8(void) {
#ifdef XENO_PC_PORT
    if (D_800ADB2C != 0) {
        return -1;
    }
    if (ArchiveDataSync() != 0) {
        return -1;
    }
    if (D_8004F308 != 0) {
        return -1;
    }
    if (D_800ADB90 != 0) {
        return -1;
    }
    if (D_800ADB34 != 0) {
        return -1;
    }
    return D_800ADBC4 != 0xFF ? -1 : 0;
#else
    s32 sync;
    register s32 ret asm("$2");

    if (D_800ADB2C != 0) {
        return -1;
    }
    sync = ArchiveDataSync();
    ret = -1;
    if (sync == 0 && D_8004F308 == 0 && D_800ADB90 == 0 && D_800ADB34 == 0) {
        ret = -(D_800ADBC4 != 0xFF);
    }
    return ret;
#endif
}

extern s16 D_800B2344;
void func_80078C5C(void) {
    RECT rect;
    int i;
    u_int* pImage;

    if (D_800B2344 != 0) {
        g_FieldRenderContexts[0].drawEnvs[0].dtd = 0;
        g_FieldRenderContexts[1].drawEnvs[0].dtd = 0;
        pImage = HeapAlloc(0x4000, 0x0);
        rect.y = 0x1E0;
        rect.w = 0x100;
        rect.x = 0;
        rect.h = 0x20;
        StoreImage(&rect, pImage);
        DrawSync(0);

        // Process image before storing it back in VRAM
        for (i = 0; i < 0x1000; i++) {
            if (pImage[i] & 0xFFFF) {
                pImage[i] |= 0xC63;
            }
            if (pImage[i] & 0xFFFF0000) {
                pImage[i] |= 0x0C630000;
            }
        }
        
        LoadImage(&rect, pImage);
        DrawSync(0);
        HeapFree(pImage);
    }
}

/* ---- func_80078D44: field-init / initial map-load orchestrator --------------
 * Functional decompile (port-first; control flow mirrors the asm). FieldMain's
 * setup calls this: it loads the map file (func_800777DC -> func_8001B484), brings
 * up the render contexts + VRAM, sets g_FieldCurRenderContextIndex=1, then parses
 * the map via FieldLoad (which allocates g_FieldActors). The fade/zoom loops play a
 * transition; in the port their effect calls stub out and ArchiveDataSync returns
 * 0, so the loops just iterate their fixed counts. */
extern s32 D_8004F2F8, D_8004F304, D_8004F308, D_8004F310, D_8004F324, D_8004F338;
extern s32 D_8004F2FC, D_8004F348, D_800ADB60, D_800C2684, D_800B2264, D_800AFD04;
extern s32 g_GamePartySkinsInitialized;
extern u8 D_800594D0, D_8005942C;
extern void *D_800ADC14, *D_80062528, *g_GameCurLoadedWDS;
extern void func_80077544(), func_800A915C(), func_800777DC(), func_800A4748();
extern void func_800A476C(), func_800A5884(), func_80070488();
extern void func_801E7378(s32 value);
extern void func_800A5600(), func_80039C4C(), func_800399D4(), func_800A24C4();
extern void func_8001B66C(), func_80085B20(), func_80085EEC(), func_800A31E8();
extern void func_800A91F0(), func_80077DAC(), func_80078B5C(), func_8007554C();

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_enter_map (src/decomp/src/field/field.c:3208). */
#ifndef XM_T_FadeChannel
#define XM_T_FadeChannel
typedef struct {
    DR_MODE modes[2]; 
    TILE tiles[2];   
    s32 level[3];
    s32 step[3];
    u16 abr;
    s16 active;
    s16 steps;
    u16 pad;
} FadeChannel;
#endif
#ifndef XM_T_FieldWork
#define XM_T_FieldWork
typedef struct FieldWork {
    s16 unk2078;               
    s16 unk207A;               
    s16 effect_steps;          
    u8 unk207E[2];
    s32 effect_value[6];       
    s32 effect_step[6];        
    s16 unk20B0[2];            
    void *effect_buffers[4];   
    FadeChannel fades[2];      
    u16 open_windows;          

    s16 encounter_inhibition;  
    s16 terrain_angle;         
    u16 input_mask;            
    s32 unk217C;               
    s32 unk2180;               
    SVECTOR sprite_angles;     
    s16 scale;                 
    s16 sprite_gate;           
    u8 fog_color[4];           
    u8 far_color[4];           
    s16 fog_range[2];          
    u8 clear_color[3];         
    u8 party_bits;             
    s16 unk21A0[6];            
    s16 emitter_range;         
    s16 piece_drift[3];        
    s16 unk21B4;               
    u8 unk21B6[2];
    s32 last_sound_effect;     
    s32 unk21BC[3];            
    u8 unk21C8[0x21CC - 0x21C8];
    u8 party_processing_mode;  
    u8 camera_floor_fixed;     
    u8 preserve_nonplayer_motion; 
    u8 forced_position;        
    u8 script_control[2];      
    u8 piece_drift_mode;       
    u8 unk21D3;
    s16 unk21D4;               
    s16 text_speed;            
    s32 camera_counter;        
    s16 unk21DC[4];            
    s16 unk21E4[4];            
    SVECTOR layer_positions[4]; 
    s32 layer_scales[4];       
    s16 unk221C[5][3];         
    u8 unk223A[2];
    s16 unk223C[3][3];         
    u8 unk224E[0x225C - 0x224E];
    u8 unk225C[3];             
    u8 unk225F[0x2264 - 0x225F]; 
    s32 unk2264;               
    s32 unk2268;               
    s32 controlled;            
    u16 encounter_music[16];   
    s16 battle_music;          
    u8 unk2292[2];
    s32 unk2294;               
    s32 unk2298;               
    s32 unk229C;               
    u16 unk22A0[32];           
    s16 unk22E0;               
    s16 emitter_descriptor[3]; 
    s16 unk22E8[3][4];         
    s16 unk2300[3][4];         
    s32 unk2318[3];            
    s16 emitter_position[3][4]; 
    u16 effects_kept;          
    u16 unk233E;               
    s16 repeat_delay;          
    s16 repeat_remaining;      
    s16 jump_mode;             
    s16 animation_mode;        
    u16 unk2348;               
    s16 unk234A;               
    s16 gear_riding_lock_override; 
    s16 followers_idle;        
    u32 unk2350;               
    u8 unk2354;                
    u8 unk2355;                
    u8 unk2356;                
    u8 unk2357;                
    u8 unk2358;                
    u8 unk2359[0x235C - 0x2359];
} FieldWork;
#endif
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
#ifndef XM_T_SoundEnvelope
#define XM_T_SoundEnvelope
typedef struct {
    u8 attack_mode;
    u8 sustain_mode;
    u8 release_mode;
    u8 attack_rate;
    u8 decay_rate;
    u8 sustain_rate;
    u8 release_rate;
    u8 sustain_level;
} SoundEnvelope;
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
#ifndef XM_S_field_enter_map
#define XM_S_field_enter_map
void xm_field_enter_map(void) __asm__("func_80078D44");
#endif
#ifndef XM_S_field_load_text_palette
#define XM_S_field_load_text_palette
void xm_field_load_text_palette(void) __asm__("func_80077544");
#endif
#ifndef XM_S_field_vram_column_save
#define XM_S_field_vram_column_save
void xm_field_vram_column_save(void) __asm__("func_800A915C");
#endif
#ifndef XM_S_cd_select_directory
#define XM_S_cd_select_directory
s32 xm_cd_select_directory(s32 group, s32 index) __asm__("ArchiveSetIndex");
#endif
#ifndef XM_S_field_wait_map_read_ahead
#define XM_S_field_wait_map_read_ahead
void xm_field_wait_map_read_ahead(void) __asm__("func_800777DC");
#endif
#ifndef XM_S_field_sync_draw_and_vsync
#define XM_S_field_sync_draw_and_vsync
void xm_field_sync_draw_and_vsync(void) __asm__("FieldRenderSync");
#endif
#ifndef XM_S_mode_field_entered_once
#define XM_S_mode_field_entered_once
extern s32 xm_mode_field_entered_once __asm__("D_8004F2F8");
#endif
#ifndef XM_S_field_screen_convert_24bit_to_15bit
#define XM_S_field_screen_convert_24bit_to_15bit
void xm_field_screen_convert_24bit_to_15bit(s32 unused) __asm__("FieldImageConvert24BitTo15Bit");
#endif
#ifndef XM_S_field_draw_init_display
#define XM_S_field_draw_init_display
void xm_field_draw_init_display(void) __asm__("FieldInitializeRenderContexts");
#endif
#ifndef XM_S_MoveImage
#define XM_S_MoveImage
int xm_MoveImage(RECT *rect, int x, int y) __asm__("MoveImage");
#endif
#ifndef XM_S_field_draw_buffer_index
#define XM_S_field_draw_buffer_index
extern s32 xm_field_draw_buffer_index __asm__("g_FieldCurRenderContextIndex");
#endif
#ifndef XM_S_field_screen_save_copy
#define XM_S_field_screen_save_copy
void xm_field_screen_save_copy(void) __asm__("func_800A4748");
#endif
#ifndef XM_S_field_screen_copy_to
#define XM_S_field_screen_copy_to
void xm_field_screen_copy_to(s32 x, s32 y) __asm__("func_800A476C");
#endif
#ifndef XM_S_DrawSync
#define XM_S_DrawSync
int xm_DrawSync(int mode) __asm__("DrawSync");
#endif
#ifndef XM_S_field_draw_swap_and_clear_ots
#define XM_S_field_draw_swap_and_clear_ots
void xm_field_draw_swap_and_clear_ots(void) __asm__("FieldClearAndSwapOTag");
#endif
#ifndef XM_S_mode_result_code
#define XM_S_mode_result_code
extern u8 xm_mode_result_code __asm__("D_800594D0");
#endif
#ifndef XM_S_mode_battle_return_fade
#define XM_S_mode_battle_return_fade
extern u8 xm_mode_battle_return_fade __asm__("D_8005942C");
#endif
#ifndef XM_S_field_screen_pieces_show
#define XM_S_field_screen_pieces_show
void xm_field_screen_pieces_show() __asm__("func_800A5884");
#endif
#ifndef XM_S_cd_sync_reads
#define XM_S_cd_sync_reads
s32 xm_cd_sync_reads(s32 mode) __asm__("ArchiveCdDataSync");
#endif
#ifndef XM_S_field_load_from_bundle
#define XM_S_field_load_from_bundle
void xm_field_load_from_bundle(void) __asm__("FieldLoad");
#endif
#ifndef XM_S_field_map_stream_start
#define XM_S_field_map_stream_start
void xm_field_map_stream_start(void) __asm__("func_80070488");
#endif
#ifndef XM_S_field_dialogue_open_blocked
#define XM_S_field_dialogue_open_blocked
extern s32 xm_field_dialogue_open_blocked __asm__("D_800AFD04");
#endif
#ifndef XM_S_field_work
#define XM_S_field_work
extern FieldWork xm_field_work __asm__("g_FieldEffects");
#endif
#ifndef XM_S_gear_model_set_instant_keyframes
#define XM_S_gear_model_set_instant_keyframes
void xm_gear_model_set_instant_keyframes(s32 value) __asm__("func_801E7378");
#endif
#ifndef XM_S_field_map_stream_running
#define XM_S_field_map_stream_running
extern s32 xm_field_map_stream_running __asm__("D_800ADB60");
#endif
#ifndef XM_S_field_screen_pieces_draw
#define XM_S_field_screen_pieces_draw
void xm_field_screen_pieces_draw(void) __asm__("FieldZoomFadeEffectUpdate");
#endif
#ifndef XM_S_field_draw_present_overlay
#define XM_S_field_draw_present_overlay
void xm_field_draw_present_overlay(void) __asm__("FieldDisplay");
#endif
#ifndef XM_S_field_screen_pieces_scale
#define XM_S_field_screen_pieces_scale
extern s32 xm_field_screen_pieces_scale __asm__("D_800C2684");
#endif
#ifndef XM_S_field_screen_pieces_set_shade
#define XM_S_field_screen_pieces_set_shade
void xm_field_screen_pieces_set_shade(s32 shade) __asm__("func_800A5600");
#endif
#ifndef XM_S_cd_get_pending_read_count
#define XM_S_cd_get_pending_read_count
s32 xm_cd_get_pending_read_count(void) __asm__("ArchiveDataSync");
#endif
#ifndef XM_S_heap_free
#define XM_S_heap_free
s32 xm_heap_free(void *data) __asm__("HeapFree");
#endif
#ifndef XM_S_field_map_stream_ring
#define XM_S_field_map_stream_ring
extern void *xm_field_map_stream_ring __asm__("D_800ADC14");
#endif
#ifndef XM_S_field_brighten_text_strip
#define XM_S_field_brighten_text_strip
void xm_field_brighten_text_strip(void) __asm__("func_80078C5C");
#endif
#ifndef XM_S_mode_worldmap_area_load_count
#define XM_S_mode_worldmap_area_load_count
extern s32 xm_mode_worldmap_area_load_count __asm__("D_8004F304");
#endif
#ifndef XM_S_sound_stop_seq
#define XM_S_sound_stop_seq
void xm_sound_stop_seq(SoundSeq *seq) __asm__("func_80039C4C");
#endif
#ifndef XM_S_mode_music_seq
#define XM_S_mode_music_seq
extern s32 xm_mode_music_seq __asm__("D_80062528");
#endif
#ifndef XM_S_sound_release_seq
#define XM_S_sound_release_seq
void xm_sound_release_seq(SoundSeq *seq) __asm__("func_800399D4");
#endif
#ifndef XM_S_sound_release_wave_bank
#define XM_S_sound_release_wave_bank
void xm_sound_release_wave_bank(SoundSequence *bank) __asm__("SoundFreeWdsEntry");
#endif
#ifndef XM_S_mode_music_wave_bank
#define XM_S_mode_music_wave_bank
extern struct SoundSequence *xm_mode_music_wave_bank __asm__("g_GameCurLoadedWDS");
#endif
#ifndef XM_S_field_finish_return_to_field
#define XM_S_field_finish_return_to_field
void xm_field_finish_return_to_field(void) __asm__("func_800A24C4");
#endif
#ifndef XM_S_mode_unread_arena_departure_count
#define XM_S_mode_unread_arena_departure_count
extern s32 xm_mode_unread_arena_departure_count __asm__("D_8004F310");
#endif
#ifndef XM_S_mode_field_return_pending
#define XM_S_mode_field_return_pending
extern s32 xm_mode_field_return_pending __asm__("g_GamePartySkinsInitialized");
#endif
#ifndef XM_S_pad_clear_queue
#define XM_S_pad_clear_queue
void xm_pad_clear_queue(void) __asm__("ControllerResetState");
#endif
#ifndef XM_S_mode_music_load_pending
#define XM_S_mode_music_load_pending
extern s32 xm_mode_music_load_pending __asm__("D_8004F308");
#endif
#ifndef XM_S_mode_music_selected_track
#define XM_S_mode_music_selected_track
extern s32 xm_mode_music_selected_track __asm__("D_8004F324");
#endif
#ifndef XM_S_field_music_release_cached_seq
#define XM_S_field_music_release_cached_seq
void xm_field_music_release_cached_seq(void) __asm__("func_80085EEC");
#endif
#ifndef XM_S_mode_music_loaded_track
#define XM_S_mode_music_loaded_track
extern s32 xm_mode_music_loaded_track __asm__("D_8004F338");
#endif
#ifndef XM_S_mode_stop_music
#define XM_S_mode_stop_music
void xm_mode_stop_music(void) __asm__("func_8001B66C");
#endif
#ifndef XM_S_mode_music_cached_seq
#define XM_S_mode_music_cached_seq
extern s32 xm_mode_music_cached_seq __asm__("D_8004F2FC");
#endif
#ifndef XM_S_mode_music_reuse_seq
#define XM_S_mode_music_reuse_seq
extern s32 xm_mode_music_reuse_seq __asm__("D_8004F348");
#endif
#ifndef XM_S_field_music_change_track
#define XM_S_field_music_change_track
void xm_field_music_change_track(s32 music, s32 unused) __asm__("func_80085B20");
#endif
#ifndef XM_S_field_update_play_record
#define XM_S_field_update_play_record
void xm_field_update_play_record(void) __asm__("func_800A31E8");
#endif
#ifndef XM_S_field_fade_in
#define XM_S_field_fade_in
void xm_field_fade_in(s32 steps) __asm__("FieldFadeToBlack");
#endif
#ifndef XM_S_field_run_pre_frame
#define XM_S_field_run_pre_frame
void xm_field_run_pre_frame(void) __asm__("func_80077DAC");
#endif
#ifndef XM_S_field_run_frame
#define XM_S_field_run_frame
void xm_field_run_frame(void) __asm__("func_8007554C");
#endif
#ifndef XM_S_field_run_post_frame
#define XM_S_field_run_post_frame
s32 xm_field_run_post_frame(void) __asm__("func_80078B5C");
#endif
#ifndef XM_S_field_vram_column_restore
#define XM_S_field_vram_column_restore
void xm_field_vram_column_restore(void) __asm__("func_800A91F0");
#endif
#ifndef XM_S_heap_coalesce
#define XM_S_heap_coalesce
void xm_heap_coalesce(void) __asm__("HeapConsolidate");
#endif
#ifndef XM_S_console_close
#define XM_S_console_close
void xm_console_close(void) __asm__("FontFree");
#endif
void func_80078D44(void) {
    RECT rect;
    s32 grow;
    s32 shade;
    s32 i;

    xm_field_load_text_palette();
    xm_field_vram_column_save();
    xm_cd_select_directory(4, 0);
    xm_field_wait_map_read_ahead();
    xm_field_sync_draw_and_vsync();
    if (xm_mode_field_entered_once == 0) {
        xm_field_screen_convert_24bit_to_15bit(0);
    }
    xm_field_draw_init_display();
    if (xm_mode_field_entered_once == 0) {
        rect.y = 0x100;
        rect.w = 0x140;
        rect.x = 0;
        rect.h = 0xE0;
        xm_MoveImage(&rect, 0, 0);
    }
    xm_field_draw_buffer_index = 1;
    xm_field_screen_save_copy();
    xm_field_screen_copy_to(0, 0x100);
    xm_DrawSync(0);
    xm_field_draw_swap_and_clear_ots();
    xm_field_sync_draw_and_vsync();
    if (xm_mode_result_code == 1 || xm_mode_battle_return_fade == 1) {
        xm_field_screen_pieces_show(0, 0);
    } else {
        xm_field_screen_pieces_show(1, 1);
    }
    xm_mode_field_entered_once = 1;
    xm_cd_sync_reads(0);
    xm_cd_select_directory(4, 0);
    xm_field_load_from_bundle();
    xm_field_map_stream_start();
    xm_field_dialogue_open_blocked = 1;
    if (xm_field_work.unk2264 != 0) {
        xm_gear_model_set_instant_keyframes(1);
    }
    if (xm_mode_result_code == 1 || xm_mode_battle_return_fade == 1) {
        grow = 0;
    } else {
        grow = 0x20;
    }
    shade = 0x800000;
    if (xm_field_map_stream_running == 1) {
    stream:
            xm_field_draw_swap_and_clear_ots();
            xm_field_screen_pieces_draw();
            xm_field_draw_present_overlay();
            if (xm_mode_battle_return_fade != 1) {
                if (xm_field_screen_pieces_scale < 0x22C0) {
                    xm_field_screen_pieces_scale += grow;
                }
            } else {
                xm_field_screen_pieces_set_shade(shade >> 16);
                shade -= 0x40000;
                if (shade < 0) {
                    shade = 0;
                }
            }
        if (xm_cd_get_pending_read_count() != 0) {
            goto stream;
        }
        xm_DrawSync(0);
        xm_heap_free(xm_field_map_stream_ring);
        xm_field_map_stream_running = 0;
        xm_field_brighten_text_strip();
    }
    if (xm_mode_battle_return_fade == 1) {
        do {
            xm_field_draw_swap_and_clear_ots();
            xm_field_screen_pieces_draw();
            xm_field_draw_present_overlay();
            xm_field_screen_pieces_set_shade(shade >> 16);
            shade -= 0x40000;
        } while (shade >= 0);
    }
    if (xm_mode_result_code == 1) {
        rect.w = 0x140;
        rect.y = 0;
        rect.x = 0;
        rect.h = 0xE0;
        xm_MoveImage(&rect, 0x200, 0);
    }
    if (xm_mode_worldmap_area_load_count != 0) {
        xm_sound_stop_seq((SoundSeq *)xm_mode_music_seq);
        xm_sound_release_seq((SoundSeq *)xm_mode_music_seq);
        xm_sound_release_wave_bank(xm_mode_music_wave_bank);
        xm_mode_worldmap_area_load_count = 0;
    }
    xm_field_finish_return_to_field();
    xm_mode_unread_arena_departure_count = 0;
    xm_mode_field_return_pending = 0;
    xm_field_sync_draw_and_vsync();
    xm_pad_clear_queue();
    xm_mode_music_load_pending = 0;
    if (xm_mode_result_code == 1) {
        xm_mode_music_selected_track = 0xE;
        xm_field_music_release_cached_seq();
    }
    if (xm_mode_music_loaded_track != xm_mode_music_selected_track) {
        xm_mode_stop_music();
        xm_mode_music_load_pending = -1;
        if (xm_mode_music_cached_seq != 0) {
            xm_mode_music_reuse_seq = 1;
        }
        xm_field_music_change_track(xm_mode_music_selected_track, 1);
    } else {
        xm_field_music_release_cached_seq();
    }
    xm_field_update_play_record();
    if (xm_mode_result_code != 1) {
        if (xm_mode_battle_return_fade != 1) {
            xm_field_fade_in(0x20);
            shade = 0x800000;
            for (i = 0; i < 0x20; i++) {
                xm_field_run_pre_frame();
                xm_field_screen_pieces_draw();
                xm_field_run_frame();
                xm_field_run_post_frame();
                if (xm_mode_result_code != 1) {
                    xm_field_screen_pieces_set_shade(shade >> 16);
                    shade -= 0x40000;
                    if (shade < 0) {
                        shade = 0;
                    }
                    if (xm_field_screen_pieces_scale < 0x22C0) {
                        xm_field_screen_pieces_scale += grow;
                    }
                }
            }
        } else {
            xm_field_fade_in(0x20);
        }
    } else {
        for (i = 0; i < 8; i++) {
            xm_field_run_pre_frame();
            xm_field_screen_pieces_draw();
            xm_field_run_frame();
            xm_field_run_post_frame();
        }
    }
    if (xm_field_work.unk2264 != 0) {
        xm_gear_model_set_instant_keyframes(0);
    }
    xm_field_vram_column_restore();
    xm_heap_coalesce();
    xm_console_close();
    xm_field_load_text_palette();
    xm_field_dialogue_open_blocked = 0;
}
#endif
#else
void func_80078D44(void) {
    RECT rect;
    s32 s0, s1, s2;

    func_80077544();
    func_800A915C();
    ArchiveSetIndex(4, 0);
    func_800777DC();              /* load the map file into D_8005A4E0 */
    FieldRenderSync();
    if (D_8004F2F8 == 0) {
        FieldImageConvert24BitTo15Bit(0);
    }
    FieldInitializeRenderContexts();
    s0 = 1;
    if (D_8004F2F8 == 0) {
        rect.x = 0; rect.y = 0x100; rect.w = 0x140; rect.h = 0xE0;
        MoveImage(&rect, 0, 0);
        s0 = 1;
    }
    g_FieldCurRenderContextIndex = s0;
    func_800A4748();
    func_800A476C(0, 0x100);
    DrawSync(0);
    FieldClearAndSwapOTag();
    FieldRenderSync();
    if (D_800594D0 == 1) {
        func_800A5884(0, 0);
    } else if (D_8005942C != 1) {
        func_800A5884(1, 1);
    } else {
        func_800A5884(0, 0);
    }
    D_8004F2F8 = 1;
    ArchiveCdDataSync(0);
    ArchiveSetIndex(4, 0);
    FieldLoad();                  /* parse map -> allocates g_FieldActors */
    func_80070488();
    D_800AFD04 = 1;
    if (D_800B2264 != 0) {
        func_801E7378(1);
    }
    if (D_800594D0 == 1) {
        s2 = 0;
    } else if (D_8005942C != 1) {
        s2 = 0x20;
    } else {
        s2 = 0;
    }

    s0 = 0x800000;
    if (D_800ADB60 == 1) {
        do {
            FieldClearAndSwapOTag();
            FieldZoomFadeEffectUpdate();
            FieldDisplay();
            if (D_8005942C == 1) {
                func_800A5600(s0 >> 16);
                s0 -= 0x40000;
                if (s0 < 0) s0 = 0;
            } else if (D_800C2684 < 0x22C0) {
                D_800C2684 += s2;
            }
        } while (ArchiveDataSync() != 0);
        DrawSync(0);
        HeapFree(D_800ADC14);
        D_800ADB60 = 0;
        func_80078C5C();
    }

    if (D_8005942C == 1) {
        do {
            FieldClearAndSwapOTag();
            FieldZoomFadeEffectUpdate();
            FieldDisplay();
            func_800A5600(s0 >> 16);
            s0 -= 0x40000;
        } while (s0 >= 0);
    }

    if (D_800594D0 == 1) {
        rect.x = 0; rect.y = 0; rect.w = 0x140; rect.h = 0xE0;
        MoveImage(&rect, 0x200, 0);
    }
    if (D_8004F304 != 0) {
        func_80039C4C(D_80062528);
        func_800399D4(D_80062528);
        SoundFreeWdsEntry(g_GameCurLoadedWDS);
        D_8004F304 = 0;
    }
    func_800A24C4();
    D_8004F310 = 0;
    g_GamePartySkinsInitialized = 0;
    FieldRenderSync();
    ControllerResetState();
    D_8004F308 = 0;
    if (D_800594D0 == 1) {
        D_8004F324 = 0xE;
        func_80085EEC();
    }
    if (D_8004F338 != D_8004F324) {
        func_8001B66C();
        D_8004F308 = -1;
        if (D_8004F2FC != 0) {
            D_8004F348 = 1;
        }
        func_80085B20(D_8004F324, 1);
    } else {
        func_80085EEC();
    }
    func_800A31E8();

    if (D_800594D0 == 1) {
        s1 = 0;
        do {
            func_80077DAC();
            s1++;
            FieldZoomFadeEffectUpdate();
            func_8007554C();
            func_80078B5C();
        } while (s1 < 8);
    } else if (D_8005942C == 1) {
        FieldFadeToBlack(0x20);
    } else {
        FieldFadeToBlack(0x20);
        s0 = 0x800000;
        s1 = 0;
        do {
            func_80077DAC();
            FieldZoomFadeEffectUpdate();
            func_8007554C();
            func_80078B5C();
            if (D_800594D0 != 1) {
                func_800A5600(s0 >> 16);
                s0 -= 0x40000;
                if (s0 < 0) s0 = 0;
                if (D_800C2684 < 0x22C0) {
                    D_800C2684 += s2;
                }
            }
            s1++;
        } while (s1 < 0x20);
    }

    if (D_800B2264 != 0) {
        func_801E7378(0);
    }
    func_800A91F0();
    HeapConsolidate();
    FontFree();
    func_80077544();
    D_800AFD04 = 0;
}
#endif /* XENO_PC_PORT */

/* func_80079288 -- per-step random-encounter roll. Decompiled from
 * asm/field/nonmatchings/main/misc4/func_80079288.s (was INCLUDE_ASM ->
 * stubbed, so encounters never rolled). Called each field step a direction is
 * held (misc6.c:659). Guards -> step-counter decrement (refresh via
 * func_8008E718 at 0) -> age cooldown timers -> on an expired timer, weighted
 * roll over the 16 formation weights (D_80065ADC) -> commit + battle handoff.
 * See docs/ai_context/ACTIVE_HANDOFF.md. */
extern s32 D_8004F308, D_8004F370;
extern s32 D_800ADBDC, D_800ADBE4, D_800ADBEC, D_800ADB2C, D_800ADBD0;
extern u8  D_800ADB04;
extern s32 D_800B2294, D_800B2298, D_800B229C;
extern s16 D_800B22A0[];
extern s16 D_800B2270[];
extern s16 D_800B2290;
extern u8  D_80065ADC[16];        /* per-formation encounter weights (main-exe BSS) */
extern u8  D_80059508, D_800594F8;
extern void  func_8008E718(void);
extern void  func_80281204(s32);
extern void *LoadGameStateOverlay(unsigned int);

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_encounter_count_down (src/decomp/src/field/field.c:3370). */
#ifndef XM_S_field_encounter_count_down
#define XM_S_field_encounter_count_down
s32 xm_field_encounter_count_down(void) __asm__("func_80079288");
#endif
#ifndef XM_S_field_battle_not_requested
#define XM_S_field_battle_not_requested
extern s32 xm_field_battle_not_requested __asm__("D_800ADBDC");
#endif
#ifndef XM_S_field_worldmap_exit_not_requested
#define XM_S_field_worldmap_exit_not_requested
extern s32 xm_field_worldmap_exit_not_requested __asm__("D_800ADBE4");
#endif
#ifndef XM_S_field_map_change_not_requested
#define XM_S_field_map_change_not_requested
extern s32 xm_field_map_change_not_requested __asm__("D_800ADBEC");
#endif
#ifndef XM_S_field_music_stream_running
#define XM_S_field_music_stream_running
extern s32 xm_field_music_stream_running __asm__("D_800ADB2C");
#endif
#ifndef XM_S_field_encounters_enabled
#define XM_S_field_encounters_enabled
extern u8 xm_field_encounters_enabled __asm__("D_800ADB04");
#endif
#ifndef XM_S_field_encounter_draw_steps
#define XM_S_field_encounter_draw_steps
void xm_field_encounter_draw_steps(void) __asm__("func_8008E718");
#endif
#ifndef XM_S_formation_encounter_weights
#define XM_S_formation_encounter_weights
extern u8 xm_formation_encounter_weights[16] __asm__("D_80065ADC");
#endif
#ifndef XM_S_rand
#define XM_S_rand
int xm_rand(void) __asm__("rand");
#endif
#ifndef XM_S_formation_selected_index
#define XM_S_formation_selected_index
extern u8 xm_formation_selected_index __asm__("D_80059508");
#endif
#ifndef XM_S_mode_battle_standalone
#define XM_S_mode_battle_standalone
extern u8 xm_mode_battle_standalone __asm__("D_800594F8");
#endif
#ifndef XM_S_mode_field_standalone
#define XM_S_mode_field_standalone
extern s32 xm_mode_field_standalone __asm__("D_8004F370");
#endif
#ifndef XM_S_mode_load_overlay_block
#define XM_S_mode_load_overlay_block
void *xm_mode_load_overlay_block(s32 mode) __asm__("LoadGameStateOverlay");
#endif
#ifndef XM_S_field_encounter_battle_pending
#define XM_S_field_encounter_battle_pending
extern s32 xm_field_encounter_battle_pending __asm__("D_800ADBD0");
#endif
#ifndef XM_S_field_monitor_absent
#define XM_S_field_monitor_absent
extern s32 xm_field_monitor_absent __asm__("g_FieldSystemMode");
#endif
#ifndef XM_S_field_debug_count_encounter
#define XM_S_field_debug_count_encounter
void xm_field_debug_count_encounter(s32 formation) __asm__("func_80281204");
#endif
s32 func_80079288(void) {
    s32 start[16];
    u8 *weights;
    s32 total;
    s32 sum;
    s32 found;
    s32 i;

    if (xm_field_battle_not_requested == 0 || xm_field_worldmap_exit_not_requested == 0 || xm_field_map_change_not_requested == 0 || xm_mode_music_load_pending == -1 || xm_field_work.unk2298 == 0
        || xm_field_work.encounter_inhibition == -1 || xm_field_music_stream_running == 1 || xm_field_encounters_enabled == 0) {
        return;
    }
    if (--xm_field_work.unk2294 == 0) {
        xm_field_encounter_draw_steps();
    }
    for (i = 0; i < xm_field_work.unk229C; i++) {
        if (xm_field_work.unk22A0[i] != 0xFFFF) {
            xm_field_work.unk22A0[i]--;
        }
    }
    for (i = 0; i < xm_field_work.unk229C; i++) {
        if (xm_field_work.unk22A0[i] == 0) {
            xm_field_work.unk22A0[i] = 0xFFFF;
            goto draw;
        }
    }
    return;
draw:
    weights = xm_formation_encounter_weights;
    total = 0;
    for (i = 0; i < 16; i++) {
        total += weights[i];
    }
    sum = 0;
    for (i = 0; i < 16; i++) {
        start[i] = sum;
        sum += weights[i];
    }
    total = (xm_rand() * (total + 1)) >> 15;
    found = 0;
    for (i = 15; i >= 0; i--) {
        if (weights[i] != 0 && start[i] < total) {
            found++;
            break;
        }
    }
    if (found != 0) {
        xm_formation_selected_index = i;
        xm_mode_battle_standalone = 0;
        xm_field_work.battle_music = xm_field_work.encounter_music[i];
        if (xm_mode_field_standalone == 0) {
            xm_mode_load_overlay_block(2);
        }
        xm_field_battle_not_requested = 0;
        xm_field_encounter_battle_pending = 1;
        if (xm_field_monitor_absent == 0) {
            xm_field_debug_count_encounter(i);
        }
    }
}
#endif
#else
void func_80079288(void) {
    u16 *timers = (u16 *)D_800B22A0;
    s32  cumulative[16];
    s32  i, sum, acc, roll, hit, selected;

    if (D_800ADBDC == 0) return;
    if (D_800ADBE4 == 0) return;
    if (D_800ADBEC == 0) return;
    if (D_8004F308 == -1) return;
    if (D_800B2298 == 0) return;
    if (*(s16 *)&g_FieldControl == -1) return;
    if (D_800ADB2C == 1) return;
    if (D_800ADB04 == 0) return;

    D_800B2294 -= 1;
    if (D_800B2294 == 0) {
        func_8008E718();
    }

    if (D_800B229C > 0) {
        for (i = 0; i < D_800B229C; i++) {
            if (timers[i] != 0xFFFF) {
                timers[i] = (u16)(timers[i] - 1);
            }
        }
    }

    if (D_800B229C <= 0) return;
    hit = -1;
    for (i = 0; i < D_800B229C; i++) {
        if (timers[i] == 0) { hit = i; break; }
    }
    if (hit < 0) return;
    timers[hit] = 0xFFFF;

    sum = 0;
    for (i = 0; i < 16; i++) {
        sum += D_80065ADC[i];
    }
    acc = 0;
    for (i = 0; i < 16; i++) {
        cumulative[i] = acc;
        acc += D_80065ADC[i];
    }

    roll = (rand() * (sum + 1)) >> 15;

    selected = -1;
    for (i = 15; i >= 0; i--) {
        if (D_80065ADC[i] != 0 && cumulative[i] < roll) {
            selected = i;
            break;
        }
    }
    if (selected < 0) return;

    D_80059508 = (u8)selected;
    D_800594F8 = 0;
    D_800B2290 = D_800B2270[selected];
    if (D_8004F370 == 0) {
        LoadGameStateOverlay(2);
    }
    D_800ADBDC = 0;
    D_800ADBD0 = 1;
    if (g_FieldSystemMode == 0) {
        func_80281204(selected);
    }
}
#endif /* XENO_PC_PORT */

/* Field-state exit dispatcher. */
extern s16 D_8004F384;
extern s32 D_800AFC78, D_800B0064;
extern void* g_pGameState;
extern void func_80085FB8(void);
extern int func_80085F30(void);
extern void func_8001BB50(void);
extern void ChangeGameState(unsigned int state);
extern void MainLoop(int errorCode) __attribute__((noreturn));

void func_8007954C(s32 exitCode) {
    D_8005942C = 0;

    switch (exitCode) {
    case 0: {
        void* gameState;
        s32 skin;
        s32 pending;
        u16 priorSkin;

        func_800A30FC();
        gameState = g_pGameState;
        skin = D_800AFC78;
        pending = D_8004F370;
        priorSkin = *(u16*)((u8*)gameState + 0x1932);
        D_8004F324 = skin;
        *(u16*)((u8*)gameState + 0x2322) = (u16)skin;
        *(u16*)((u8*)gameState + 0x2320) = priorSkin;
        if (pending != 0) {
            return;
        }
        ChangeGameState(2);
        break;
    }

    case 1:
        if (D_8004F384 == 1) {
            func_8001B66C();
            func_80085FB8();
            ArchiveCdDataSync(0);
            func_80085F30();
            func_8001B66C();
        }
        if (D_8004F370 != 0) {
            return;
        }
        ChangeGameState(3);
        break;

    case 2: {
        void* gameState = g_pGameState;
#ifdef XENO_PC_PORT
        s32 skin = D_8004F324;
#else
        register s32 skin __asm__("$3") = D_8004F324;
#endif
        s32 pending = D_8004F370;
        u16 priorSkin = *(u16*)((u8*)gameState + 0x1932);

        *(u16*)((u8*)gameState + 0x2322) = (u16)skin;
        *(u16*)((u8*)gameState + 0x2320) = priorSkin;
        if (pending != 0) {
            return;
        }
        ChangeGameState(4);
        g_GamePartySkinsInitialized++;
        break;
    }

    case 3:
        D_8004F310 = 0;
        g_GamePartySkinsInitialized = 0;
        if (D_8004F370 != 0) {
            return;
        }
        if (D_800B0064 & 0x80) {
            func_8001BB50();
        }
        ChangeGameState(D_800B0064 & 0x7F);
        break;

    default:
        break;
    }

    MainLoop(0);
}
void func_800796F4(void) {}

void FieldSwapRenderContext(void) {
    g_FieldCurRenderContextIndex = (g_FieldCurRenderContextIndex + 1) % 2;
    g_FieldCurRenderContext = &g_FieldRenderContexts[g_FieldCurRenderContextIndex];
    PutDispEnv(&g_FieldCurRenderContext->dispEnv);
    PutDrawEnv(&g_FieldCurRenderContext->drawEnvs[0]);
}

extern RECT D_800AFE4C;
extern TILE D_800AFE54[];
extern DR_MODE D_800AFE24[];
	
void func_80079784(int color) {
    FieldClearAndSwapOTag();
    setRGB(D_800AFE54[g_FieldCurRenderContextIndex], color * 4);
    addPrim(g_FieldCurRenderContext->ot1, &D_800AFE54[g_FieldCurRenderContextIndex]);
    addPrim(g_FieldCurRenderContext->ot1, &D_800AFE24[g_FieldCurRenderContextIndex]);
    FieldRenderSync();
    MoveImage(&D_800AFE4C, 0, g_FieldCurRenderContextIndex * 0x100);
    PutDispEnv(&g_FieldCurRenderContext->dispEnv);
    PutDrawEnv(&g_FieldCurRenderContext->drawEnvs[0]);
    DrawOTag(g_FieldCurRenderContext->ot1 + 1);
}


/* Resolve the field movement gate from the active player actor's +0x14 flags,
 * then apply the signed-halfword script override when it is not 0x00FF.
 * Transcribed from asm/field/nonmatchings/main/misc4/func_800798BC.s
 * (0x800798BC-0x80079958). Retail control flow: beqz D_800B2268 -> store 1;
 * else andi flags+0x14,0xC0 / bnez store 1 else store 0; then lh D_800B234C
 * and sb override when != 0xFF. */
extern s32 D_800B2268;
extern s16 D_800B234C;
extern u8 D_80059179;
extern s32 g_PlayerActorIndex;

void func_800798BC(void) {
    if (D_800B2268 != 0) {
        u32 flags = *(u32*)((u8*)(uintptr_t)g_FieldActors[g_PlayerActorIndex].pActorData
                            + 0x14);
#ifdef FIELD_MOVEGATE_MUTANT_C0_MASK
        if (flags & 0x40) {
#else
        if (flags & 0xC0) {
#endif
            D_80059179 = 1;
        } else {
            D_80059179 = 0;
        }
    } else {
        D_80059179 = 1;
    }

    if (D_800B234C != 0xFF) {
        D_80059179 = D_800B234C;
    }
}

void func_8007995C(short w, short h, short x, short y, int destX, int destY) {
    RECT rect;

    rect.w = w;
    rect.h = h;
    rect.x = x;
    rect.y = y;
    MoveImage(&rect, destX, destY);
    DrawSync(0);
}

void FieldRenderSyncAndFlush(void) {
    FieldRenderSync();
    EnterCriticalSection();
    FlushCache();
    ExitCriticalSection();
}

/* VRAM coordinate pairs for the party-sprite save/restore blit tables. */
typedef struct {
    u16 x;
    u16 y;
} FieldVramCoord;

extern FieldVramCoord D_800ADCB0[]; /* field-side sprite VRAM rects */
extern FieldVramCoord D_800ADCC8[]; /* menu-backup VRAM rects */
extern s16 D_8006BE2C[];            /* party-member-present flags snapshot */
extern TILE D_800AFE64;             /* fade tile, render context 1 */
extern void* D_8005945C;            /* menu shared-resource buffer */
extern void* D_800ADB20;            /* saved 0x6B9 archive buffer */
extern void* D_800ADB30;            /* menu-arg source buffer */
extern void* D_8005A4AC;            /* &g_FieldRenderContexts + 0xCC */
extern void* D_8005A4B0;            /* &g_FieldRenderContexts + 0x81C0 */
extern u8 D_80059460;               /* menuToEnter = request & 0x7F */
extern u8 D_800B02C8;
extern u8 D_800ADB05;
extern s32 D_800AFE84, D_800ADB50, D_800ADB60, D_800ADBEC;
extern s32 D_80050100, D_8004F320, D_8004F31C, D_8004F350;
extern s32 D_800ADB64;
extern s32 g_GameSceneMapNum;
extern s32 g_GamePartyMemberSkins[];
extern RenderContext g_FieldRenderContexts[2];
extern void* g_pGameState;
extern u8 D_800B21D0[];
extern u8 g_MenuDebugEnabled;
extern void* g_PartyDataBuffers[];

extern void func_800798BC(void);
extern void func_800A2488(void);
extern void func_80070488(void);
extern void func_80070508(void);
extern void func_80077544(void);
extern void FieldPartyFreeSkinDataBuffers(void);
extern void FieldPartyAllocateSkinDataBuffers(void);
extern void GamePartySyncSkinData(void);
extern void GamePartySyncStreamedData(void);
extern void FontFree(void);
extern void FieldSwapRenderContext(void);
extern void MenuMain(void);
extern void FieldScriptMemoryWriteU16(int index, int value);
extern int func_80029AFC(StreamDataQueueEntry* pEntries, int arg1, int arg2);
extern u32 LZSSDecompress(void* src, void* dst);

#ifdef XENO_PC_PORT
/* DIAGNOSTIC / TEST TOOLING (XENO_MENU_PARTY_DIAG=1).  The field menu opener
 * frees and reallocates the party skin buffers around MenuMain.  On PSX the
 * heap is deterministic, so the reallocation lands on the same addresses and
 * any pointer an actor still holds into them keeps working; on the host it need
 * not.  Dump g_PartyDataBuffers[], g_GamePartyMemberSkins[], and every field
 * actor's pSpriteData (+0x4) / pActorData (+0x4C) at each seam so a retained
 * pre-free pointer is visible rather than inferred.  Inert unless armed. */
extern s32 D_800ADBFC;      /* live actor count (party + scripted actors) */
extern s32 g_FieldNumActors;
extern s32 g_PlayerActorIndex;

static int PcPort_MenuPartyDiagArmed(void)
{
    static int armed = -1;
    if (armed < 0) {
        const char* e = getenv("XENO_MENU_PARTY_DIAG");
        armed = (e && e[0] == '1') ? 1 : 0;
    }
    return armed;
}

/* DIAGNOSTIC / TEST TOOLING (XENO_MENU_PARTY_DIAG=1).  Second half of the same
 * question: the opener blits the 6 party-sprite VRAM rects out to the backup
 * table before MenuMain and back afterwards.  The C is symmetric, but that only
 * proves the *calls* pair up -- if the host MoveImage loses the pixels, the
 * party textures come back as 0x0000 texels, which the PSX blender treats as
 * fully transparent, i.e. the sprite draws its quads and shows nothing.  Digest
 * both rect tables straight out of the VRAM mirror so that case is measured
 * rather than assumed. */
static void PcPort_MenuVramDigest(const char* tag)
{
    extern unsigned short vram[];
    enum { VRAM_W = 1024, RECT_W = 0x40, RECT_H = 0x20 };
    s32 r;

    if (!PcPort_MenuPartyDiagArmed())
        return;
    /* Retail party-page coordinates spelled out, INDEPENDENT of the migrated
     * D_800ADCB0 (asm/field/data/3DF78.data.s @ 0x3E1C0).  The table-driven
     * digest below reports whatever the build actually blits; this probe always
     * reports the real party texture pages, so a build whose table is stubbed
     * to zero can still be measured at the same place as a build whose table is
     * correct.  That is what makes the before/after comparison an A/B. */
    {
        static const FieldVramCoord kRetailPartyPages[6] = {
            { 0x0000, 0x00E0 }, { 0x0040, 0x00E0 }, { 0x0080, 0x00E0 },
            { 0x00C0, 0x00E0 }, { 0x0100, 0x00E0 }, { 0x0100, 0x01E0 },
        };

        for (r = 0; r < 6; r++) {
            unsigned int h = 2166136261u;
            int nonZero = 0;
            s32 y;

            for (y = 0; y < RECT_H; y++) {
                s32 x;
                for (x = 0; x < RECT_W; x++) {
                    unsigned short p =
                        vram[(kRetailPartyPages[r].y + y) * VRAM_W +
                             kRetailPartyPages[r].x + x];
                    h ^= p;
                    h *= 16777619u;
                    if (p != 0)
                        nonZero++;
                }
            }
            printf("[xeno-port][vram-probe] %-11s page[%d] (%u,%u) fnv=%08x "
                   "nonzero=%d/%d\n",
                   tag, (int)r, (unsigned)kRetailPartyPages[r].x,
                   (unsigned)kRetailPartyPages[r].y, h, nonZero,
                   RECT_W * RECT_H);
        }
    }

    for (r = 0; r < 6; r++) {
        const FieldVramCoord* pRects[2];
        const char* names[2];
        s32 t;

        pRects[0] = &D_800ADCB0[r];
        pRects[1] = &D_800ADCC8[r];
        names[0] = "field";
        names[1] = "backup";
        for (t = 0; t < 2; t++) {
            unsigned int h = 2166136261u;
            int nonZero = 0;
            s32 y;

            for (y = 0; y < RECT_H; y++) {
                s32 x;
                for (x = 0; x < RECT_W; x++) {
                    unsigned short p =
                        vram[(pRects[t]->y + y) * VRAM_W + pRects[t]->x + x];
                    h ^= p;
                    h *= 16777619u;
                    if (p != 0)
                        nonZero++;
                }
            }
            printf("[xeno-port][vram-diag] %-11s %s[%d] (%u,%u) fnv=%08x "
                   "nonzero=%d/%d\n",
                   tag, names[t], (int)r, (unsigned)pRects[t]->x,
                   (unsigned)pRects[t]->y, h, nonZero, RECT_W * RECT_H);
        }
    }
    fflush(stdout);
}

static void PcPort_MenuPartyDiag(const char* tag)
{
    s32 i;
    s32 count;

    if (!PcPort_MenuPartyDiagArmed())
        return;
    printf("[xeno-port][party-diag] %-10s buffers=%p/%p/%p skins=%d/%d/%d "
           "numActors=%d liveActors=%d player=%d\n",
           tag, g_PartyDataBuffers[0], g_PartyDataBuffers[1],
           g_PartyDataBuffers[2], (int)g_GamePartyMemberSkins[0],
           (int)g_GamePartyMemberSkins[1], (int)g_GamePartyMemberSkins[2],
           (int)g_FieldNumActors, (int)D_800ADBFC, (int)g_PlayerActorIndex);
    count = D_800ADBFC;
    if (count > 8)
        count = 8;
    for (i = 0; i < count; i++) {
        u8* actor = (u8*)g_FieldActors + i * 0x5C;
        u32 sprite = *(u32*)(actor + 0x4);
        u32 data = *(u32*)(actor + 0x4C);
        u32 skinId = 0xFFFFFFFFu;
        if (data != 0)
            skinId = *(u8*)((u8*)(uintptr_t)data + 0x126);
        printf("[xeno-port][party-diag] %-10s actor[%d] pSpriteData=%08x "
               "pActorData=%08x skinId=%02x status=%04x\n",
               tag, (int)i, (unsigned)sprite, (unsigned)data,
               (unsigned)skinId, (unsigned)*(u16*)(actor + 0x58));
    }
    fflush(stdout);
}
#endif

/* Transcribed from asm/field/nonmatchings/main/misc4/func_800799D4.s
 * (0x800799D4-0x8007A448). Early-out if request==0x80 and D_800B21D0!=0.
 * Overlay HeapAlloc uses ArchiveDecodeAlignedSize((id+5)&0x7F) when
 * D_8004F370==1, else (D_800ADB30&0xFFFFFF)+0xFFE3AFF8. Restore 0x6B9 uses
 * the same D_800ADB30 word +0xFFE23FF8 when D_8004F370==0. */
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_800799D4);
#else
void func_800799D4(void) {
    FieldVramCoord* pFieldRects;
    FieldVramCoord* pBackupRects;
    void* pMenuArchiveBackup = NULL;
    void* pOverlayBuf;
    void* pSaveTop;
    void* pSaveBottom;
    StreamDataQueueEntry menuLoadCommands[4];
    void* savedMenuArgSrc;
    RECT rect;
    s32 i;
    s32 menuId;
    s16* pPresentFlags;

    if (D_800ADB64 == 0x80 && D_800B21D0[0] != 0) {
        return;
    }

    /* Full-screen semi-transparent black fade tile, duplicated per context. */
    setTile(&D_800AFE54[0]);
    SetSemiTrans(&D_800AFE54[0], 1);
    D_800AFE54[0].w = 0x140;
    D_800AFE54[0].b0 = 0;
    D_800AFE54[0].g0 = 0;
    D_800AFE54[0].r0 = 0;
    D_800AFE54[0].y0 = 0;
    D_800AFE54[0].x0 = 0;
    D_800AFE54[0].h = 0xE0;
    D_800AFE64 = D_800AFE54[0];

    SetDrawMode(&D_800AFE24[0], 0, 0, GetTPage(0, 2, 0, 0), 0);
    SetDrawMode(&D_800AFE24[1], 0, 0, GetTPage(0, 2, 0, 0), 0);
#ifdef XENO_PC_PORT
    PcPort_MenuPartyDiag("pre-free");
#endif
    FieldPartyFreeSkinDataBuffers();
#ifdef XENO_PC_PORT
    PcPort_MenuPartyDiag("post-free");
#endif

    /* Preserve the streamed 0x6B9 field archive across the overlay load. */
    if (D_800B2264 != 0) {
        ArchiveSetIndex(0x4, 0);
        pMenuArchiveBackup = HeapAlloc(ArchiveDecodeAlignedSize(0x6B9), 0);
        memcpy(pMenuArchiveBackup, D_800ADB20, ArchiveDecodeAlignedSize(0x6B9));
        HeapFree(D_800ADB20);
    }

    savedMenuArgSrc = D_800ADB30;
    ArchiveSetIndex(0x10, 0);
#ifdef XENO_PC_PORT
    /* Retail's else-branch sizes this buffer as the fixed-map gap
     * [0x801C5008, D_800ADB30): the menu overlay must land at 0x801C5000 on
     * PSX, and D_800ADB30 is the next allocation above it.  On the host heap
     * D_800ADB30 is a host pointer, so that subtraction is meaningless (it
     * produced a ~5MB bogus size -> HeapAlloc failure -> GameHandleError(130)
     * in the map005 repro).  Size the buffer by what is actually loaded into
     * it: menuLoadCommands[1] streams archive file (menu id + 5) here, so use
     * that file's decoded size -- the same derivation retail's own
     * D_8004F370 branch and func_80077884's TOC branch (main.c:171) use.
     * (void)savedMenuArgSrc: keep the retail read; only the sizing differs. */
    (void)savedMenuArgSrc;
    pOverlayBuf = HeapAlloc(ArchiveDecodeAlignedSize((D_800ADB64 + 5) & 0x7F), 1);
#else
    if (D_8004F370 == 1) {
        pOverlayBuf = HeapAlloc(ArchiveDecodeAlignedSize((D_800ADB64 + 5) & 0x7F), 1);
    } else {
        pOverlayBuf = HeapAlloc(
            ((u32)savedMenuArgSrc & 0xFFFFFF) + 0xFFE3AFF8, 1);
    }
#endif

    menuLoadCommands[0].archiveIndex = 1;
    menuLoadCommands[2].archiveIndex = 0;
    menuLoadCommands[2].pData = NULL;
    menuLoadCommands[3].archiveIndex = 0;
    menuLoadCommands[3].pData = NULL;
    menuLoadCommands[0].pData = HeapAlloc(ArchiveDecodeAlignedSize(1), 1);
    D_8005945C = menuLoadCommands[0].pData;
    menuLoadCommands[1].pData = pOverlayBuf;
    menuId = D_800ADB64 & 0x7F;
    menuLoadCommands[1].archiveIndex = menuId + 5;
    if (menuId == 5 && D_8004F370 == 0) {
        menuLoadCommands[2].archiveIndex = 0xC;
        menuLoadCommands[2].pData = (void*)0x1DC000;
    }

    ArchiveCdDataSync(0);
    func_80029AFC(menuLoadCommands, 0, 0);
    ArchiveSetIndex(0x4, 0);

    /* Back up the field party-sprite VRAM rects, then two fixed screen rects. */
#ifdef XENO_PC_PORT
    PcPort_MenuVramDigest("pre-backup");
#endif
    pFieldRects = D_800ADCB0;
    pBackupRects = D_800ADCC8;
    rect.w = 0x40;
    rect.h = 0x20;
    for (i = 0; i < 6; i++) {
        rect.x = pFieldRects->x;
        rect.y = pFieldRects->y;
        MoveImage(&rect, pBackupRects->x, pBackupRects->y);
        DrawSync(0);
        pFieldRects++;
        pBackupRects++;
    }
#ifdef XENO_PC_PORT
    PcPort_MenuVramDigest("post-backup");
#endif
    func_8007995C(0x40, 0x100, 0x3C0, 0x100, 0x300, 0);
    func_8007995C(0x40, 0x100, 0x2C0, 0x100, 0x280, 0);

    D_800AFE4C.x = 0x2C0;
    D_800AFE4C.y = 0x100;
    D_800AFE4C.w = 0x140;
    D_800AFE4C.h = 0xE0;
    func_800A4748();
    MoveImage(&D_800AFE4C, 0, 0x100);
    DrawSync(0);

    for (i = 0; i < 0x20; i++) {
        func_80079784(i);
    }
    FieldRenderSync();
    D_800AFE4C.x = 0;
    D_800AFE4C.y = 0;
    FieldSwapRenderContext();
    FontFree();
    MoveImage(&D_800AFE4C, 0, 0xE0);
    FieldRenderSync();
    ArchiveCdDataSync(0);

    D_800594D0 = 0;
    g_MenuDebugEnabled = 0;
#ifndef MENU_MUTANT_NEVER_REQUEST
    D_80059460 = D_800ADB64 & 0x7F;
#endif
    pPresentFlags = D_8006BE2C;
    for (i = 0; i < 3; i++) {
        pPresentFlags[i] = ((u8*)g_pGameState)[0x22B1 + i];
    }
    func_800798BC();
    D_8005A4AC = (u8*)g_FieldRenderContexts + 0xCC;
    D_8005A4B0 = (u8*)g_FieldRenderContexts + 0x81C0;
    FieldRenderSyncAndFlush();

#ifdef XENO_PC_PORT
    printf("[xeno-port][menu] func_800799D4 request=%d D_80059460=%d -> MenuMain\n",
           (int)D_800ADB64, (int)D_80059460);
    fflush(stdout);
    /* TEST TOOLING: only the field system menu (ADB64=0x80). Title/other
     * requests must not arm PadOnControl Equip nav. */
    if (D_800ADB64 == 0x80) {
        extern int g_PcPortFieldMenuOpened;
        g_PcPortFieldMenuOpened = 1;
    }
#endif
#ifndef MENU_MUTANT_SKIP_MENUMAIN
    MenuMain();
#endif
#ifdef XENO_PC_PORT
    printf("[xeno-port][menu] MenuMain returned D_800594D0=%d\n", (int)D_800594D0);
    fflush(stdout);
#endif

    FieldRenderSyncAndFlush();
    D_80050100 = 2;

    /* Post-menu state fixups keyed on the menu return state D_800594D0. */
    if (D_800594D0 == 0 && (D_800ADB64 & 0x7F) == 2) {
        D_800B02C8 = 1;
        FieldScriptMemoryWriteU16(0x46, 0);
        FieldScriptMemoryWriteU16(4, 4);
        g_GameSceneMapNum = 4;
        *(s16*)((u8*)g_pGameState + 0x2320) = 0;
        *(s16*)((u8*)g_pGameState + 0x1932) = 0;
        *(s16*)((u8*)g_pGameState + 0x231A) = 4;
    }
    if (D_800594D0 == 2) {
        D_800B02C8 = 1;
        FieldScriptMemoryWriteU16(0x46, 2);
        FieldScriptMemoryWriteU16(4, *(u16*)((u8*)g_pGameState + 0x231A) & 0x3FFF);
        if ((*(u16*)((u8*)g_pGameState + 0x231A) & 0x3FFF) < 0x400) {
            *(s16*)((u8*)g_pGameState + 0x2320) =
                *(u16*)((u8*)g_pGameState + 0x1984);
        }
    }

    /* Copy the menu's last frame (0,0xE0) to (0x140,0), then from there
     * into both field frame buffers (0,0) and (0,0x100).  Retail 80079FCC
     * passes a1=0x140 (ori a1,zero,0x140 at 80079FA8): the first copy lands
     * at x=0x140, not x=0.  With dest (0,0) the next two copies spread the
     * texture pages at (0x140,0) over both frame buffers, which showed as a
     * few frames of VRAM garbage on every menu close (and so right before
     * the New Game FMV). */
    FieldRenderSync();
    D_800AFE4C.x = 0;
    D_800AFE4C.y = 0xE0;
    D_800AFE4C.w = 0x140;
    D_800AFE4C.h = 0xE0;
#ifdef FIELD_799D4_MUTANT_CLOSE_COPY_TO_ORIGIN
    MoveImage(&D_800AFE4C, 0, 0);
#else
    MoveImage(&D_800AFE4C, 0x140, 0);
#endif
    FieldRenderSync();
    D_800AFE4C.x = 0x140;
    D_800AFE4C.y = 0;
    MoveImage(&D_800AFE4C, 0, 0);
    MoveImage(&D_800AFE4C, 0, 0x100);
    FieldRenderSync();

    PutDispEnv(&g_FieldCurRenderContext->dispEnv);
    PutDrawEnv(&g_FieldCurRenderContext->drawEnvs[0]);
    D_800AFE4C.x = 0x2C0;
    D_800AFE4C.y = 0x100;
    rect.x = 0x300;
    rect.y = 0;
    rect.w = 0x40;
    rect.h = 0x100;
    pSaveTop = HeapAlloc(0x8000, 1);
    StoreImage(&rect, (u_long*)pSaveTop);
    DrawSync(0);
    rect.x = 0x280;
    rect.y = 0;
    rect.w = 0x40;
    rect.h = 0x100;
    pSaveBottom = HeapAlloc(0x8000, 1);
    StoreImage(&rect, (u_long*)pSaveBottom);
    DrawSync(0);

#ifdef XENO_PC_PORT
    PcPort_MenuVramDigest("pre-restore");
#endif
    pFieldRects = D_800ADCC8;
    pBackupRects = D_800ADCB0;
    for (i = 0; i < 6; i++) {
        rect.w = 0x40;
        rect.h = 0x20;
        rect.x = pFieldRects->x;
        rect.y = pFieldRects->y;
        MoveImage(&rect, pBackupRects->x, pBackupRects->y);
        DrawSync(0);
        pFieldRects++;
        pBackupRects++;
    }
#ifdef XENO_PC_PORT
    PcPort_MenuVramDigest("post-restore");
#endif

    ArchiveSetIndex(0x4, 0);
    HeapChangeCurrentUser(0x8, NULL);
    D_800ADB60 = 0;
    func_80070488();

    if (D_800AFE84 != 0) {
        for (i = 0x20; i < 0x3F; i++) {
            func_80079784(i);
        }
        D_800ADB50 = 1;
    } else {
        for (i = 0x1F; i >= 0; i--) {
            func_80079784(i);
        }
        func_80079784(0);
        D_800ADB50 = 0;
    }
    func_80070508();

    FieldRenderSync();
    rect.x = 0x2C0;
    /* Retail 8007A1FC restores the saved field texture strips at y=0x100. */
    rect.y = 0x100;
    rect.w = 0x40;
    rect.h = 0x100;
    LoadImage(&rect, (u_long*)pSaveBottom);
    DrawSync(0);
    rect.x = 0x3C0;
    LoadImage(&rect, (u_long*)pSaveTop);
    DrawSync(0);
    HeapFree(pSaveBottom);
    HeapFree(pSaveTop);
    HeapFree(pOverlayBuf);

    ArchiveSetIndex(0x4, 0);
    if (D_800B2264 != 0) {
#ifdef XENO_PC_PORT
        /* Same fixed-map idiom as the overlay alloc above: retail sizes the
         * restored 0x6B9 buffer as [0x801DC008, pMenuArchiveBackup), invalid
         * against a host pointer.  The buffer holds the decoded 0x6B9 archive
         * (memcpy'd back below), so use its decoded size -- identical to
         * retail's own D_8004F370 branch and main.c:171. Latent at cold boot
         * (D_800B2264 == 0 in the repro) but the same crash class once maps
         * with streamed archives open a menu. */
        D_800ADB20 = HeapAlloc(ArchiveDecodeAlignedSize(0x6B9), 1);
#else
        if (D_8004F370 == 0) {
            D_800ADB20 = HeapAlloc(
                ((u32)savedMenuArgSrc & 0xFFFFFF) + 0xFFE23FF8, 1);
        } else {
            D_800ADB20 = HeapAlloc(ArchiveDecodeAlignedSize(0x6B9), 1);
        }
#endif
        memcpy(D_800ADB20, pMenuArchiveBackup, ArchiveDecodeAlignedSize(0x6B9));
        HeapFree(pMenuArchiveBackup);
    }

    SetGeomOffset(0xA0, 0x70);
    SetGeomScreen(g_Scene.sceneScrZ);
    FieldPartyAllocateSkinDataBuffers();
#ifdef XENO_PC_PORT
    PcPort_MenuPartyDiag("post-alloc");
#endif

    if (D_800ADB64 == 1) {
        g_GamePartyMemberSkins[0] = 0xFF;
        g_GamePartyMemberSkins[1] = 0xFF;
        g_GamePartyMemberSkins[2] = 0xFF;
        D_8004F320 = 0;
        D_8004F31C = 0;
        GamePartySyncSkinData();
        GamePartySyncStreamedData();
        FieldRenderSync();
        D_800ADBEC = 0;
        D_800ADB05 = 1;
    } else {
        for (i = 0; i < 3; i++) {
            s32 skin = g_GamePartyMemberSkins[i];
            if (skin != 0xFF) {
                void* pBuf = HeapAlloc(ArchiveDecodeAlignedSize(skin + 5), 1);
                ArchiveReadFileToBuffer(skin + 5, pBuf, 0, 0x80);
                ArchiveCdDataSync(0);
                LZSSDecompress(pBuf, g_PartyDataBuffers[i]);
                HeapFree(pBuf);
            }
        }
#ifdef XENO_PC_PORT
        PcPort_MenuPartyDiag("post-lzss");
#endif
        func_800A2488();
#ifdef XENO_PC_PORT
        PcPort_MenuPartyDiag("post-2488");
#endif
        FieldRenderSync();
    }

    D_800ADB64 = 0xFF;
    func_80077544();
#ifndef MENU_MUTANT_SKIP_WAIT_CLEAR
    D_8004F350 = 0;
#endif
#ifdef XENO_PC_PORT
    printf("[xeno-port][menu] WAIT_MENU D_8004F350=%d\n", (int)D_8004F350);
    fflush(stdout);
#endif
}
#endif /* XENO_PC_PORT */


// Quad rendering
// --------------------
void FieldClampPolyFT4UVs(POLY_FT4* poly, short u0, short v0, short u1, short v1, short u2, short v2, short u3, short v3) {
    if (u0 < 0) u0 = 0;
    if (u1 < 0) u1 = 0;
    if (u2 < 0) u2 = 0;
    if (u3 < 0) u3 = 0;
    if (v0 < 0) v0 = 0;
    if (v1 < 0) v1 = 0;
    if (v2 < 0) v2 = 0;
    if (v3 < 0) v3 = 0;
    
    if (u0 >= 0x100) u0 = 0xFF;
    if (u1 >= 0x100) u1 = 0xFF;
    if (u2 >= 0x100) u2 = 0xFF;
    if (u3 >= 0x100) u3 = 0xFF;
    if (v0 >= 0x100) v0 = 0xFF;
    if (v1 >= 0x100) v1 = 0xFF;
    if (v2 >= 0x100) v2 = 0xFF;
    if (v3 >= 0x100) v3 = 0xFF;

    poly->u0 = u0;
    poly->v0 = v0;
    poly->u1 = u1;
    poly->v1 = v1;
    poly->u2 = u2;
    poly->v2 = v2;
    poly->u3 = u3;
    poly->v3 = v3;
}

extern u8 D_800B0FEC[];
extern s16 D_800ADE30[][8];
extern u8 D_800ADE70[][8];

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_compass_build_quadrant_quads (src/decomp/src/field/field_motion.c:240). */
#ifndef XM_T_FieldMarker
#define XM_T_FieldMarker
typedef struct FieldMarker {
    SVECTOR v[4];
    POLY_FT4 poly[2];
} FieldMarker;
#endif
#ifndef XM_S_field_compass_build_quadrant_quads
#define XM_S_field_compass_build_quadrant_quads
void xm_field_compass_build_quadrant_quads(void) __asm__("func_8007A5C4");
#endif
#ifndef XM_S_field_compass_markers
#define XM_S_field_compass_markers
extern FieldMarker xm_field_compass_markers[25] __asm__("D_800B06BC");
#endif
#ifndef XM_S_SetPolyFT4
#define XM_S_SetPolyFT4
void xm_SetPolyFT4(POLY_FT4 *p) __asm__("SetPolyFT4");
#endif
#ifndef XM_S_field_compass_quadrant_corners
#define XM_S_field_compass_quadrant_corners
extern s16 xm_field_compass_quadrant_corners[32] __asm__("D_800ADE30");
#endif
#ifndef XM_S_field_set_quad_uvs_clamped
#define XM_S_field_set_quad_uvs_clamped
void xm_field_set_quad_uvs_clamped(POLY_FT4 *poly, s16 u0, s16 v0, s16 u1, s16 v1, s16 u2, s16 v2, s16 u3, s16 v3) __asm__("FieldClampPolyFT4UVs");
#endif
#ifndef XM_S_field_compass_quadrant_uvs
#define XM_S_field_compass_quadrant_uvs
extern u8 xm_field_compass_quadrant_uvs[32] __asm__("D_800ADE70");
#endif
#ifndef XM_S_SetSemiTrans
#define XM_S_SetSemiTrans
void xm_SetSemiTrans(void *p, int abe) __asm__("SetSemiTrans");
#endif
#ifndef XM_S_GetTPage
#define XM_S_GetTPage
u_short xm_GetTPage(int tp, int abr, int x, int y) __asm__("GetTPage");
#endif
#ifndef XM_S_GetClut
#define XM_S_GetClut
u_short xm_GetClut(int x, int y) __asm__("GetClut");
#endif
void func_8007A5C4(void) {
    FieldMarker *record;
    POLY_FT4 *quad;
    POLY_FT4 *copy;
    s32 i;

    for (i = 0; i < 4; i++) {
        record = &xm_field_compass_markers[i + 21];
        copy = &xm_field_compass_markers[i + 21].poly[1];
        quad = &xm_field_compass_markers[i + 21].poly[0];
        xm_SetPolyFT4(quad);
        record->v[0].vx = xm_field_compass_quadrant_corners[i * 8];
        record->v[0].vy = 0;
        record->v[0].vz = xm_field_compass_quadrant_corners[i * 8 + 1];
        record->v[1].vx = xm_field_compass_quadrant_corners[i * 8 + 2];
        record->v[1].vy = 0;
        record->v[1].vz = xm_field_compass_quadrant_corners[i * 8 + 3];
        record->v[2].vx = xm_field_compass_quadrant_corners[i * 8 + 4];
        record->v[2].vy = 0;
        record->v[2].vz = xm_field_compass_quadrant_corners[i * 8 + 5];
        record->v[3].vx = xm_field_compass_quadrant_corners[i * 8 + 6];
        record->v[3].vy = 0;
        record->v[3].vz = xm_field_compass_quadrant_corners[i * 8 + 7];
        quad->r0 = 0x80;
        quad->g0 = 0x80;
        quad->b0 = 0x80;
        xm_field_set_quad_uvs_clamped(quad, xm_field_compass_quadrant_uvs[i * 8], xm_field_compass_quadrant_uvs[i * 8 + 1] + 0xC0, xm_field_compass_quadrant_uvs[i * 8 + 2],
                      xm_field_compass_quadrant_uvs[i * 8 + 3] + 0xC0, xm_field_compass_quadrant_uvs[i * 8 + 4], xm_field_compass_quadrant_uvs[i * 8 + 5] + 0xC0, xm_field_compass_quadrant_uvs[i * 8 + 6],
                      xm_field_compass_quadrant_uvs[i * 8 + 7] + 0xC0);
        xm_SetSemiTrans(quad, 1);
        quad->tpage = xm_GetTPage(0, 2, 0x280, 0x1C0);
        quad->clut = xm_GetClut(0x100, 0xF2);
        *copy = *quad;
    }
}
#endif
#else
void func_8007A5C4(void) {
    s32 i;

    for (i = 0; i < 4; i++) {
        Quad* pPart = (Quad*)(D_800B0FEC + (i * 0x70));
        POLY_FT4* pPoly = &pPart->polys[0];

        SetPolyFT4(pPoly);

        pPart->vertices[0].vx = D_800ADE30[i][0];
        pPart->vertices[0].vy = 0;
        pPart->vertices[0].vz = D_800ADE30[i][1];
        pPart->vertices[1].vx = D_800ADE30[i][2];
        pPart->vertices[1].vy = 0;
        pPart->vertices[1].vz = D_800ADE30[i][3];
        pPart->vertices[2].vx = D_800ADE30[i][4];
        pPart->vertices[2].vy = 0;
        pPart->vertices[2].vz = D_800ADE30[i][5];
        pPart->vertices[3].vx = D_800ADE30[i][6];
        pPart->vertices[3].vy = 0;
        pPart->vertices[3].vz = D_800ADE30[i][7];

        setRGB0(pPoly, 0x80, 0x80, 0x80);
        FieldClampPolyFT4UVs(pPoly,
                             D_800ADE70[i][0], D_800ADE70[i][1] + 0xC0,
                             D_800ADE70[i][2], D_800ADE70[i][3] + 0xC0,
                             D_800ADE70[i][4], D_800ADE70[i][5] + 0xC0,
                             D_800ADE70[i][6], D_800ADE70[i][7] + 0xC0);
        SetSemiTrans(pPoly, 1);
        pPoly->tpage = GetTPage(0, 2, 0x280, 0x1C0);
        pPoly->clut = GetClut(0x100, 0xF2);
        pPart->polys[1] = *pPoly;
    }
}
#endif /* XENO_PC_PORT */

extern SVec4 D_800ADCE0[]; // X0 -> X3 positions
extern SVec4 D_800ADD28[]; // Y0 -> Y3 positions
extern SVec4 D_800ADD70[]; // U0 -> U3s
extern SVec4 D_800ADDB8[]; // V0 -> V3s

// Texture / Clut stuff
extern short D_800ADE00[];
extern short D_800ADE02[];
extern short D_800ADE04[];
extern short D_800ADE06[];
extern short D_800ADE08[];
extern short D_800ADE0A[];

// Compass primitive initialization?
void func_8007A7F4(Quad* pPart, int x, int y, int tex) {
    int nIndexTex;
    POLY_FT4* pNextPoly;
    char _pad[0x88];

    pNextPoly = &pPart->polys[1]; 
    SetPolyFT4(&pPart->polys[0]);
    
    nIndexTex = tex * 0x6,

    // Set Position 0, 1, 2, and 3
    pPart->vertices[0].vx = D_800ADCE0[x].x;
    pPart->vertices[0].vy = 0x0;
    pPart->vertices[0].vz = D_800ADD28[y].x;

    pPart->vertices[1].vx = D_800ADCE0[x].y;
    pPart->vertices[1].vy = 0;
    pPart->vertices[1].vz = D_800ADD28[y].y;

    pPart->vertices[2].vx = D_800ADCE0[x].z;
    pPart->vertices[2].vy = 0;
    pPart->vertices[2].vz = D_800ADD28[y].z;

    pPart->vertices[3].vx = D_800ADCE0[x].w;
    pPart->vertices[3].vy = 0;
    pPart->vertices[3].vz = D_800ADD28[y].w;
    
    setRGB0(&pPart->polys[0], 0x80, 0x80, 0x80);
    
    pPart->polys[0].tpage = GetTPage(
        D_800ADE00[nIndexTex], 
        D_800ADE02[nIndexTex], 
        D_800ADE04[nIndexTex], 
        D_800ADE06[nIndexTex]
    );
    
    pPart->polys[0].clut = GetClut(D_800ADE08[nIndexTex], D_800ADE0A[nIndexTex]);
    
    FieldClampPolyFT4UVs(&pPart->polys[0], 
        D_800ADD70[x].x, D_800ADDB8[y].x, 
        D_800ADD70[x].y, D_800ADDB8[y].y, 
        D_800ADD70[x].z, D_800ADDB8[y].z, 
        D_800ADD70[x].w, D_800ADDB8[y].w
    );

    *pNextPoly = pPart->polys[0];
}

// Actor primitive initialization
void func_8007AA44(Quad* pQuad) {
    POLY_FT4* pPoly;
    POLY_FT4* pPoly2;

    pPoly = &pQuad->polys[0];
    pPoly2 = &pQuad->polys[1];
    
    SetPolyFT4(pPoly);
    pQuad->vertices[3].vx = -0x18;
    pQuad->vertices[3].vz = -0x18;
    pQuad->vertices[3].vy = 0;

    pQuad->vertices[2].vx = 0x18;
    pQuad->vertices[2].vy = 0;
    pQuad->vertices[2].vz = -0x18;

    pQuad->vertices[1].vx = -0x18;
    pQuad->vertices[1].vy = 0;
    pQuad->vertices[1].vz = 0x18;

    pQuad->vertices[0].vx = 0x18;
    pQuad->vertices[0].vy = 0;
    pQuad->vertices[0].vz = 0x18;

    setRGB0(pPoly, 0x80, 0x80, 0x80);
    pPoly->tpage = GetTPage(0, 2, 0x280, 0x1E0);
    pPoly->clut = GetClut(0x100, 0xF3);
    SetSemiTrans(pPoly, 1);

    pPoly->v0 = 0xE0;
    pPoly->v1 = 0xE0;
    pPoly->u0 = 0;
    pPoly->u1 = 0xF;
    pPoly->u2 = 0;
    pPoly->v2 = 0xEF;
    pPoly->u3 = 0xF;
    pPoly->v3 = 0xEF;
    
    *pPoly2 = *pPoly;
}

void FieldRenderQuad(u_long* ot, Quad* pQuad, MATRIX* pMatTransform, int renderContext) {
    long nInterpolation;
    long nFlag;
    POLY_FT4* pPoly;

    pPoly = &pQuad->polys[renderContext];
    PushMatrix();

    // Set our GTE transform matrix (RTM and TRV)
    SetRotMatrix(pMatTransform);
    SetTransMatrix(pMatTransform);

    // Transform our vertices to screen coordinates
    RotAverage4(
        &pQuad->vertices[0], &pQuad->vertices[1], &pQuad->vertices[2], &pQuad->vertices[3], 
        (long*)&pPoly->x0, (long*)&pPoly->x1, (long*)&pPoly->x2, (long*)&pPoly->x3, 
        &nInterpolation, &nFlag
    );

    // Queue our primitive for rendering
    addPrim(ot + 1, pPoly);

    PopMatrix();
}

void func_8007AC58(u_long* ot, Quad* pQuad, MATRIX* pMatTransform, int renderContext) {
    long nInterpolation;
    long nFlag;
    int nPosY;
    int nPosX1;
    POLY_FT4* pPoly;
    int nPosX2;

    pPoly = &pQuad->polys[renderContext];
    PushMatrix();

    // Set our GTE transform matrix (RTM and TRV)
    SetRotMatrix(pMatTransform);
    SetTransMatrix(pMatTransform);

    // Transform our vertices to screen coordinates
    RotAverage4(
        &pQuad->vertices[0], &pQuad->vertices[1], &pQuad->vertices[2], &pQuad->vertices[3], 
        (long*)&pPoly->x0, (long*)&pPoly->x1, (long*)&pPoly->x2, (long*)&pPoly->x3, 
        &nInterpolation, &nFlag
    );
    
    nPosX1 = (pPoly->x3 + pPoly->x2) / 2;
    nPosY = pPoly->y3;

    nPosX2 = nPosX1 + 8;
    nPosX1 = nPosX1 - 8;
    setXY4(pPoly,
        nPosX1, nPosY - 10,
        nPosX2, nPosY - 10,
        nPosX1, nPosY,
        nPosX2, nPosY
    );
    
    // Queue our primitive for rendering
    addPrim(ot + 1, pPoly);

    PopMatrix();
}


// Controller stuff
// --------------------
extern s32 g_pFieldControllerBuffer1;
extern s32 g_pFieldControllerBuffer2;

extern u_short g_FieldMouseSpeedX;
extern u_short g_FieldMouseSpeedY;
extern s32 D_800C3A44;
extern s32 D_800C3A4C;
extern s32 D_800C3A50;
extern s32 D_800C3A54;
extern int g_FieldMousePositionsX[2];
extern int g_FieldMousePositionsY[2];

void FieldSetControllerBuffers(void* controllerBuffer1, void* controllerBuffer2) {
    g_pFieldControllerBuffer1 = controllerBuffer1;
    g_pFieldControllerBuffer2 = controllerBuffer2;
}

void func_8007ADA4(int arg0, int arg1, int arg2, int arg3) {
    D_800C3A44 = arg0 * g_FieldMouseSpeedX;
    D_800C3A50 = arg1 * g_FieldMouseSpeedX;
    D_800C3A4C = arg2 * g_FieldMouseSpeedY;
    D_800C3A54 = arg3 * g_FieldMouseSpeedY;
}

void FieldSetMouseSpeed(u_short xSpeed, u_short ySpeed) {
    g_FieldMouseSpeedX = xSpeed;
    g_FieldMouseSpeedY = ySpeed;
}

void FieldSetMousePosition(int mouseIndex, int xMovement, int yMovement) {
    g_FieldMousePositionsX[mouseIndex] = xMovement * g_FieldMouseSpeedX;
    g_FieldMousePositionsY[mouseIndex] = yMovement * g_FieldMouseSpeedY;
}

extern void func_8007AF74(s32 mouseIndex);

#ifndef XENO_PC_PORT
/* func_8007AE78: returns int in retail (v0 live at the epilogue: the bnez delay
 * slot stays empty); misc4.c has no earlier declaration, misc2.c declares it void
 * (harmless).  Controller pointer via ((s8**)&g_pFieldControllerBuffer1)[i]. */

s32 func_8007AE78__ret(s32 mouseIndex, s32* pOut) asm("func_8007AE78");

s32 func_8007AE78__ret(s32 mouseIndex, s32* pOut) {
    s8* pController;

    func_8007AF74(mouseIndex);
    pOut[0] = g_FieldMousePositionsX[mouseIndex] / g_FieldMouseSpeedX;
    pOut[1] = g_FieldMousePositionsY[mouseIndex] / g_FieldMouseSpeedY;
    pOut[2] = -0x100;
    pOut[3] = ((s8**)&g_pFieldControllerBuffer1)[mouseIndex][4];
    pOut[4] = ((s8**)&g_pFieldControllerBuffer1)[mouseIndex][5];
    pController = ((s8**)&g_pFieldControllerBuffer1)[mouseIndex];
    if (pController[0] == 0 && pController[1] == 0x12) {
        pOut[2] = ~pController[3] & 0xC;
    }
}
#else
void func_8007AE78(s32 mouseIndex, void* out) {
    s32* pOut = out;
    u8* pController;

    func_8007AF74(mouseIndex);

    pOut[0] = g_FieldMousePositionsX[mouseIndex] / g_FieldMouseSpeedX;
    pOut[1] = g_FieldMousePositionsY[mouseIndex] / g_FieldMouseSpeedY;
    pOut[2] = -0x100;

    pController = (u8*)(uintptr_t)(mouseIndex ? g_pFieldControllerBuffer2 : g_pFieldControllerBuffer1);
    pOut[3] = (s8)pController[4];
    pOut[4] = (s8)pController[5];

    if ((s8)pController[0] == 0 && (s8)pController[1] == 0x12) {
        pOut[2] = (u8)(~pController[3]) & 0x0C;
    }
}
#endif /* XENO_PC_PORT */

#ifndef XENO_PC_PORT
/* func_8007AF74: the controller pointer is ((s8**)&g_pFieldControllerBuffer1)[i]
 * (Buffer2 follows Buffer1), re-read from the slot for each use, with s8 reads
 * (lb even for the != 0 test); the X and Y clamps use separate position temps. */
#endif
void func_8007AF74(s32 mouseIndex) {
#ifdef XENO_PC_PORT
    u8* pController = (u8*)(uintptr_t)(mouseIndex ? g_pFieldControllerBuffer2 : g_pFieldControllerBuffer1);
    s32 pos;

    if ((s8)pController[0] != 0 || (s8)pController[1] != 0x12) {
        return;
    }

    g_FieldMousePositionsX[mouseIndex] += (s8)pController[4];
    g_FieldMousePositionsY[mouseIndex] += (s8)pController[5];

    pos = g_FieldMousePositionsX[mouseIndex];
    if (pos > D_800C3A50) {
        g_FieldMousePositionsX[mouseIndex] = D_800C3A50;
    } else if (pos < D_800C3A44) {
        g_FieldMousePositionsX[mouseIndex] = D_800C3A44;
    }

    pos = g_FieldMousePositionsY[mouseIndex];
    if (pos > D_800C3A54) {
        g_FieldMousePositionsY[mouseIndex] = D_800C3A54;
    } else if (pos < D_800C3A4C) {
        g_FieldMousePositionsY[mouseIndex] = D_800C3A4C;
    }
#else
    s32 posX;
    s32 posY;

    if (((s8**)&g_pFieldControllerBuffer1)[mouseIndex][0] != 0 || ((s8**)&g_pFieldControllerBuffer1)[mouseIndex][1] != 0x12) {
        return;
    }

    g_FieldMousePositionsX[mouseIndex] += ((s8**)&g_pFieldControllerBuffer1)[mouseIndex][4];
    g_FieldMousePositionsY[mouseIndex] += ((s8**)&g_pFieldControllerBuffer1)[mouseIndex][5];

    posX = g_FieldMousePositionsX[mouseIndex];
    if (posX > D_800C3A50) {
        g_FieldMousePositionsX[mouseIndex] = D_800C3A50;
    } else if (posX < D_800C3A44) {
        g_FieldMousePositionsX[mouseIndex] = D_800C3A44;
    }

    posY = g_FieldMousePositionsY[mouseIndex];
    if (posY > D_800C3A54) {
        g_FieldMousePositionsY[mouseIndex] = D_800C3A54;
    } else if (posY < D_800C3A4C) {
        g_FieldMousePositionsY[mouseIndex] = D_800C3A4C;
    }
#endif
}

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_collision_compute_plane_height (src/decomp/src/field/field_motion.c:444). */
#ifndef XM_S_field_collision_compute_plane_height
#define XM_S_field_collision_compute_plane_height
void xm_field_collision_compute_plane_height(SVECTOR *a, SVECTOR *b, SVECTOR *c, SVECTOR *p, VECTOR *normal) __asm__("func_8007B07C");
#endif
#ifndef XM_S_VectorNormal
#define XM_S_VectorNormal
long xm_VectorNormal(VECTOR *v0, VECTOR *v1) __asm__("VectorNormal");
#endif
#ifndef XM_S_OuterProduct12
#define XM_S_OuterProduct12
void xm_OuterProduct12(VECTOR *v0, VECTOR *v1, VECTOR *v2) __asm__("OuterProduct12");
#endif
void func_8007B07C(SVECTOR *a, SVECTOR *b, SVECTOR *c, SVECTOR *p, VECTOR *normal) {
    VECTOR edge_b;
    VECTOR edge_c;
    VECTOR d;

    d.vx = b->vx - a->vx;
    d.vy = b->vy - a->vy;
    d.vz = b->vz - a->vz;
    xm_VectorNormal(&d, &edge_b);
    d.vx = c->vx - a->vx;
    d.vy = c->vy - a->vy;
    d.vz = c->vz - a->vz;
    xm_VectorNormal(&d, &edge_c);
    xm_OuterProduct12(&edge_b, &edge_c, normal);
    if (normal->vy == 0) {
        p->vy = 0;
        return;
    }
    p->vy = a->vy + (-(normal->vx * (p->vx - a->vx)) - normal->vz * (p->vz - a->vz)) / normal->vy;
}
#endif
#else
void func_8007B07C(s16* arg0, s16* arg1, s16* arg2, s16* arg3, VECTOR* arg4) {
    VECTOR vec0;
    VECTOR normal0;
    VECTOR normal1;

    vec0.vx = arg1[0] - arg0[0];
    vec0.vy = arg1[1] - arg0[1];
    vec0.vz = arg1[2] - arg0[2];
    VectorNormal(&vec0, &normal0);

    vec0.vx = arg2[0] - arg0[0];
    vec0.vy = arg2[1] - arg0[1];
    vec0.vz = arg2[2] - arg0[2];
    VectorNormal(&vec0, &normal1);

    OuterProduct12(&normal0, &normal1, arg4);

    if (arg4->vy == 0) {
        arg3[1] = 0;
        return;
    }

    arg3[1] = arg0[1] +
              ((-(arg4->vx * (arg3[0] - arg0[0])) -
                (arg4->vz * (arg3[2] - arg0[2]))) /
               arg4->vy);
}
#endif /* XENO_PC_PORT */

/* ---- func_8007B1C4: walkmesh point-to-triangle locator ---------------------
 * Faithful port of the handwritten GTE asm (asm/.../misc4/func_8007B1C4.s,
 * 181 insns). For layer `idx`, walks its D_800AFB44[idx] triangles and finds
 * the one that contains the XZ point (posX, posZ) via three NCLIP
 * (cross-product) sign tests (all >= 0 => inside). On a hit it calls
 * func_8007B07C to interpolate the surface Y at the point (into pOut) and the
 * face normal (into pState), then returns the triangle index. If no triangle
 * contains the point (or the layer has no triangles), it zeroes both outputs
 * and returns 0 -- exactly the asm's fall-through behavior. Used at actor
 * spawn (func_8009E574 / func_80080A74) to seed walkmeshTriIds[], and for the
 * camera path (func_8007B478). Requires D_800AFB44 to be populated by the
 * walkmesh loader (FieldLoad); with it zero the loop is skipped. */
extern s32 D_800AFB24[];
extern s32 D_800AFB34[];
extern s32 D_800AFB44[];
extern u32 D_800AFB20[];
extern u8 D_800B21CC;
extern s16 D_800AFB54;

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_8007B1C4);
#else
s16 func_8007B1C4(s16 posX, s16 posZ, s32 idx, s16* pOut, s32* pState) {
    u8* triBase = (u8*)(uintptr_t)(u32)D_800AFB24[idx];
    s32 count = D_800AFB44[idx];
    u8* vertBase = (u8*)(uintptr_t)(u32)D_800AFB34[idx];
    s32 packedPos = ((s32)posX << 16) + posZ;
    s16 point[3];
    s32 i;

    point[0] = posX;
    point[1] = 0;
    point[2] = posZ;

    for (i = 0; i < count; i++) {
        s16* tri = (s16*)(triBase + i * 14);
        s16* v0 = (s16*)(vertBase + tri[0] * 8);
        s16* v1 = (s16*)(vertBase + tri[1] * 8);
        s16* v2 = (s16*)(vertBase + tri[2] * 8);
        s32 p0 = ((s32)v0[0] << 16) + v0[2];
        s32 p1 = ((s32)v1[0] << 16) + v1[2];
        s32 p2 = ((s32)v2[0] << 16) + v2[2];

        /* NOTE: retail inlines NCLIP here too, but this function's overall
         * loop structure is still a faithful rewrite (see 8007B1C4 header), so
         * the macro form measures 12 bytes WORSE than the call form until the
         * rest of the body is matched. Keep the call form for now. */
        if (NormalClip(p0, p1, packedPos) < 0) {
            continue;
        }
        if (NormalClip(p1, p2, packedPos) < 0) {
            continue;
        }
        if (NormalClip(p2, p0, packedPos) < 0) {
            continue;
        }

        /* Point is inside triangle i: interpolate Y + normal, return index. */
        func_8007B07C(v0, v1, v2, point, (VECTOR*)pState);
        pOut[0] = point[0];
        pOut[1] = point[1];
        pOut[2] = point[2];
        return (s16)i;
    }

    /* No containing triangle: zero outputs, return 0. */
    pOut[0] = 0;
    pOut[1] = 0;
    pOut[2] = 0;
    pState[0] = 0;
    pState[1] = 0;
    pState[2] = 0;
    return 0;
}
#endif /* XENO_PC_PORT */

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_collision_is_outside_triangle (src/decomp/src/field/field_motion.c:542). */
#ifndef XM_S_field_collision_is_outside_triangle
#define XM_S_field_collision_is_outside_triangle
s32 xm_field_collision_is_outside_triangle(SVECTOR *a, SVECTOR *b, SVECTOR *c, SVECTOR *p) __asm__("func_8007B478");
#endif
#ifndef XM_S_OuterProduct0
#define XM_S_OuterProduct0
void xm_OuterProduct0(VECTOR *v0, VECTOR *v1, VECTOR *v2) __asm__("OuterProduct0");
#endif
s32 func_8007B478(SVECTOR *a, SVECTOR *b, SVECTOR *c, SVECTOR *p) {
    VECTOR edge;
    VECTOR to_p;
    VECTOR cross;

    edge.vx = b->vx - a->vx;
    edge.vy = 0;
    edge.vz = b->vz - a->vz;
    to_p.vx = p->vx - a->vx;
    to_p.vy = 0;
    to_p.vz = p->vz - a->vz;
    xm_OuterProduct0(&edge, &to_p, &cross);
    if (cross.vy < 0) {
        return -1;
    }
    edge.vx = c->vx - b->vx;
    edge.vy = 0;
    edge.vz = c->vz - b->vz;
    to_p.vx = p->vx - b->vx;
    to_p.vy = 0;
    to_p.vz = p->vz - b->vz;
    xm_OuterProduct0(&edge, &to_p, &cross);
    if (cross.vy < 0) {
        return -1;
    }
    edge.vx = a->vx - c->vx;
    edge.vy = 0;
    edge.vz = a->vz - c->vz;
    to_p.vx = p->vx - c->vx;
    to_p.vy = 0;
    to_p.vz = p->vz - c->vz;
    xm_OuterProduct0(&edge, &to_p, &cross);
    if (cross.vy < 0) {
        return -1;
    }
    return 0;
}
#endif
#else
s32 func_8007B478(SVECTOR* p0, SVECTOR* p1, SVECTOR* p2, SVECTOR* pTest) {
    VECTOR edge1, edge2, cross;
    /* Edge1 = p1 - p0, Edge2 = p2 - p0 */
    edge1.vx = p1->vx - p0->vx;
    edge1.vy = p1->vy - p0->vy;
    edge1.vz = 0;
    edge2.vx = p2->vx - p0->vx;
    edge2.vy = p2->vy - p0->vy;
    edge2.vz = 0;
    OuterProduct0(&edge1, &edge2, &cross);
    if (cross.vz < 0) return -1;

    /* Edge1 = pTest - p0, Edge2 = p2 - p0 */
    edge1.vx = pTest->vx - p0->vx;
    edge1.vy = pTest->vy - p0->vy;
    edge1.vz = 0;
    edge2.vx = p2->vx - p0->vx;
    edge2.vy = p2->vy - p0->vy;
    edge2.vz = 0;
    OuterProduct0(&edge1, &edge2, &cross);
    if (cross.vz < 0) return -1;

    /* Edge1 = p1 - pTest, Edge2 = p2 - pTest */
    edge1.vx = p1->vx - pTest->vx;
    edge1.vy = p1->vy - pTest->vy;
    edge1.vz = 0;
    edge2.vx = p2->vx - pTest->vx;
    edge2.vy = p2->vy - pTest->vy;
    edge2.vz = 0;
    OuterProduct0(&edge1, &edge2, &cross);
    return cross.vz >> 31;
}
#endif /* XENO_PC_PORT */

extern s16 D_800B218C;

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_compute_offset_at_angle (src/decomp/src/field/field_motion.c:581). */
#ifndef XM_S_field_compute_offset_at_angle
#define XM_S_field_compute_offset_at_angle
void xm_field_compute_offset_at_angle(VECTOR *out, s32 distance, s32 angle) __asm__("func_8007B614");
#endif
#ifndef XM_S_gpu_get_cos
#define XM_S_gpu_get_cos
s32 xm_gpu_get_cos(s32 angle) __asm__("rsin");
#endif
#ifndef XM_S_gpu_get_sin
#define XM_S_gpu_get_sin
s32 xm_gpu_get_sin(s32 angle) __asm__("rcos");
#endif
void func_8007B614(VECTOR *out, s32 distance, s32 angle) {
    s32 heading;

    distance *= 16;
    distance = (distance * xm_field_work.scale) >> 12;
    heading = angle & 0xFFF;
    out->vx = xm_gpu_get_cos(heading) * distance;
    out->vz = -(xm_gpu_get_sin(heading) * distance);
    out->vy = 0;
}
#endif
#else
void func_8007B614(s32* pOut, s16 scale, s16 angle) {
    s32 magnitude = (scale << 4) * D_800B218C;

    magnitude >>= 12;
    angle &= 0xFFF;

    pOut[0] = rsin(angle) * magnitude;
    pOut[1] = 0;
    pOut[2] = -(rcos(angle) * magnitude);
}
#endif /* XENO_PC_PORT */

s32 func_8007B694(s32* arg0) {
    return -ratan2(arg0[2], arg0[0]) & 0xFFF;
}

extern long FieldGetVec2Magnitude(long x, long y);

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_collision_slide_along_edge (src/decomp/src/field/field_motion.c:602). */
#ifndef XM_S_field_collision_slide_along_edge
#define XM_S_field_collision_slide_along_edge
s32 xm_field_collision_slide_along_edge(s16 heading, SVECTOR *edge, VECTOR *velocity, s32 unused) __asm__("func_8007B6C4");
#endif
#ifndef XM_S_ratan2
#define XM_S_ratan2
long xm_ratan2(long y, long x) __asm__("ratan2");
#endif
#ifndef XM_S_field_compute_planar_length
#define XM_S_field_compute_planar_length
s32 xm_field_compute_planar_length(s32 dx, s32 dz) __asm__("FieldGetVec2Magnitude");
#endif
s32 func_8007B6C4(s16 heading, SVECTOR *edge, VECTOR *velocity, s32 unused) {
    VECTOR d;
    VECTOR n;
    s16 angle;
    s32 relative;
    s16 result;
    s32 speed;

    relative = (0xC00 - heading) & 0xFFF;
    angle = -xm_ratan2(edge[1].vz - edge[0].vz, edge[1].vx - edge[0].vx) & 0xFFF;
    relative = (relative + angle) & 0xFFF;
    result = angle;
    if (relative - 0x80 > 0xF00U) {
        velocity->vx = 0;
        velocity->vy = 0;
        velocity->vz = 0;
        return angle;
    }
    if (relative < 0x800) {
        d.vx = edge[0].vx - edge[1].vx;
        d.vy = 0;
        d.vz = edge[0].vz - edge[1].vz;
        result = (angle + 0x800) & 0xFFF;
    } else {
        d.vx = edge[1].vx - edge[0].vx;
        d.vy = 0;
        d.vz = edge[1].vz - edge[0].vz;
    }
    xm_VectorNormal(&d, &n);
    speed = xm_field_compute_planar_length(velocity->vx >> 12, velocity->vz >> 12);
    velocity->vx = n.vx * speed;
    velocity->vy = 0;
    velocity->vz = n.vz * speed;
    return result;
}
#endif
#else
s32 func_8007B6C4(s16 angle, s16* edge, s32* vec) {
    VECTOR side;
    VECTOR normal;
    s32 edgeAngle;
    s32 deltaAngle;
    s32 magnitude;

    deltaAngle = (0xC00 - angle) & 0xFFF;
    edgeAngle = -ratan2(edge[6] - edge[2], edge[4] - edge[0]) & 0xFFF;
    deltaAngle = (deltaAngle + edgeAngle) & 0xFFF;

    if ((u32)(deltaAngle - 0x80) >= 0xF01) {
        vec[0] = 0;
        vec[1] = 0;
        vec[2] = 0;
        return edgeAngle;
    }

    if (deltaAngle < 0x800) {
        side.vx = edge[0] - edge[4];
        side.vy = 0;
        side.vz = edge[2] - edge[6];
        edgeAngle = (edgeAngle + 0x800) & 0xFFF;
    } else {
        side.vx = edge[4] - edge[0];
        side.vy = 0;
        side.vz = edge[6] - edge[2];
    }

    VectorNormal(&side, &normal);
    magnitude = FieldGetVec2Magnitude(vec[0] >> 12, vec[2] >> 12);
    vec[0] = normal.vx * magnitude;
    vec[1] = 0;
    vec[2] = normal.vz * magnitude;
    return edgeAngle;
}
#endif /* XENO_PC_PORT */

extern s32 D_800ADB98;
extern s32 func_8007C694(s32* move, s32* base, void* pActorData,
                         s16* outEdge, s16* outPoint, s32 mode);

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_collision_sweep (src/decomp/src/field/field_motion.c:648). */
#ifndef XM_T_xm_Fixed
#define XM_T_xm_Fixed
typedef union {
    s32 value;
    struct {
        u16 fraction;
        s16 whole;
    } part;
} xm_Fixed;
#endif
#ifndef XM_T_xm_ScriptSlot
#define XM_T_xm_ScriptSlot
typedef struct xm_ScriptSlot {
    u16 resume_pc;      
    u8 countdown;       
    u8 tag;             
    u32 value : 16;     
    u32 unk16 : 2;
    u32 priority : 4;   
    u32 unk22 : 1;
    u32 move_mode : 2;  
    u32 unk25 : 7;
} xm_ScriptSlot;
#endif
#ifndef XM_T_xm_PlatformLink
#define XM_T_xm_PlatformLink
typedef struct xm_PlatformLink {
    SVECTOR rotation; 
    s16 radius;       
    s16 unkA;
} xm_PlatformLink;
#endif
#ifndef XM_T_xm_ActorState
#define XM_T_xm_ActorState
typedef struct {
    u32 mode : 2;   
    u32 unk2 : 3;   
    u32 unk5 : 1;   
    u32 depth : 3;  
    u32 octant : 3; 
    u32 unk12 : 1;  
    u32 layer : 3;  
    u32 unk16 : 2;  
    u32 unk18 : 10; 
    u32 unk28 : 4;
} xm_ActorState;
#endif
#ifndef XM_T_xm_FieldActor
#define XM_T_xm_FieldActor
typedef struct xm_FieldActor {
    u32 flags;           
    u32 layer_flags;     
    s16 triangle[4];     
    s16 layer;           
    u8 unk012[2];
    u32 floor_attribute; 
    s16 unk18;           
    s16 height;          
    xm_Fixed gravity;       
    s32 position[3];     
    u8 unk02C[4];
    s32 unk030[3];       
    u8 unk03C[4];
    s32 unk40[3];        
    u8 unk04C[4];
    s32 unk50[3];        
    u8 unk05C[4];
    s16 unk60;           
    s16 unk62;           
    s16 unk64;           
    u8 unk066[2];
    s16 last_position[3]; 
    u16 stuck;           
    s16 unk70;           
    s16 unk72;           
    u8 ridden_actor;     
    u8 unk075;           
    s16 unk76;           
    u16 call_stack[4];   
    u8 character;        
    u8 unk081;
    u8 unk82;            
    u8 unk83;            
    s32 unk84;           
    s16 unk88;           
    s16 unk8A;           
    xm_ScriptSlot slots[8]; 
    u16 pc;              
    u8 slot;             
    u8 unk0CF;
    s32 target[3];       
    u8 unk0DC[0xE2 - 0xDC];
    u8 unkE2;            
    u8 unk0E3;
    s16 unkE4;           
    s16 unkE6;           
    s16 unkE8;           
    s16 unk0EA;          
    s16 unkEC;           
    s16 unkEE;           
    s32 unkF0;           
    s16 scale[3];        
    u8 unk0FA[2];
    u8 color0[3];        
    u8 color1[3];        
    u16 unk102;          
    s16 heading;         
    s16 heading_goal;    
    s16 unk108;          
    u16 sound;           
    u8 sound_volume;     
    u8 sound_mode;       
    u8 unk10E[0x110 - 0x10E];
    struct xm_PlatformLink *link; 
    void *unk114;        
    s32 *list;           
    u16 unk11C;          
    s16 unk11E;          
    void *unk120;        
    s16 unk124;          
    u8 unk126;           
    u8 unk127;           
    u16 unk128;          
    u8 unk12A[2];
    union {
        u32 word;
        xm_ActorState bits;
    } state;             
    u32 unk130 : 9;      
    u32 unk130_9 : 10;   
    u32 unk130_19 : 9;   
    u32 sprite_kind : 2; 
    u32 unk130_30 : 2;
    u32 unk134;          
} xm_FieldActor;
#endif
#ifndef XM_S_field_collision_sweep
#define XM_S_field_collision_sweep
s32 xm_field_collision_sweep(VECTOR *delta, xm_FieldActor *actor, SVECTOR *edge, s16 heading) __asm__("func_8007B814");
#endif
#ifndef XM_S_field_collision_walk
#define XM_S_field_collision_walk
s32 xm_field_collision_walk(VECTOR *probe, s32 *position, xm_FieldActor *actor, SVECTOR *edge, SVECTOR *floor, s32 mode) __asm__("func_8007C694");
#endif
#ifndef XM_S_field_ground_override_enabled
#define XM_S_field_ground_override_enabled
extern s32 xm_field_ground_override_enabled __asm__("D_800ADB98");
#endif
s32 func_8007B814(VECTOR *delta, xm_FieldActor *actor, SVECTOR *edge, s16 heading) {
    VECTOR probe;
    SVECTOR floor;
    u8 unused[0x20]; 
    s32 angle;

    angle = heading & 0xFFF;
    probe.vx = delta->vx + (xm_gpu_get_cos(angle) << 6);
    probe.vz = delta->vz - (xm_gpu_get_sin(angle) << 6);
    if (xm_field_collision_walk(&probe, actor->position, actor, edge, &floor, -1) == -1) {
        probe.vx = delta->vx;
        probe.vy = delta->vy;
        probe.vz = delta->vz;
        xm_field_collision_slide_along_edge(heading, edge, &probe, 0);
    } else {
        angle = heading - 0x100;
        angle &= 0xFFF;
        probe.vx = delta->vx + (xm_gpu_get_cos(angle) << 6);
        probe.vz = delta->vz - (xm_gpu_get_sin(angle) << 6);
        if (xm_field_collision_walk(&probe, actor->position, actor, edge, &floor, -1) == -1) {
            probe.vx = delta->vx;
            probe.vy = delta->vy;
            probe.vz = delta->vz;
            xm_field_collision_slide_along_edge(heading, edge, &probe, 0);
        } else {
            angle = heading + 0x100;
            angle &= 0xFFF;
            probe.vx = delta->vx + (xm_gpu_get_cos(angle) << 6);
            probe.vz = delta->vz - (xm_gpu_get_sin(angle) << 6);
            if (xm_field_collision_walk(&probe, actor->position, actor, edge, &floor, -1) == -1) {
                probe.vx = delta->vx;
                probe.vy = delta->vy;
                probe.vz = delta->vz;
                xm_field_collision_slide_along_edge(heading, edge, &probe, 0);
            } else {
                probe.vx = delta->vx;
                probe.vy = delta->vy;
                probe.vz = delta->vz;
            }
        }
    }
    if (xm_field_collision_walk(&probe, actor->position, actor, edge, &floor, 0) == -1) {
        return -1;
    }
    if (!(actor->flags & 0x40000)) {
        if ((floor.vy << 16) < actor->position[1] && xm_field_ground_override_enabled == 0) {
            return -1;
        }
    } else {
        floor.vy = actor->unkEC;
    }
    probe.vy = (floor.vy << 16) - actor->position[1];
    delta->vx = probe.vx;
    delta->vy = probe.vy;
    delta->vz = probe.vz;
    actor->unk72 = (actor->position[1] + delta->vy) >> 16;
    return 0;
}
#endif
#else
void* func_8007B814(s32* pVec, void* pActorData, s32* pOut, s16 angle) {
    u8* actorData = pActorData;
    s16 resolvedAngle = angle;
    s16 point[16];
    s32 move[3];
    s32 result;
    s32 candidateAngle;

    candidateAngle = angle & 0x0FFF;
    move[0] = pVec[0] + (rsin(candidateAngle) << 6);
    move[1] = 0;
    move[2] = pVec[2] - (rcos(candidateAngle) << 6);
    result = func_8007C694(move, (s32*)(actorData + 0x20), actorData,
                           (s16*)pOut, point, -1);

    if (result != -1) {
        resolvedAngle = result >> 16;

        candidateAngle = (resolvedAngle - 0x100) & 0x0FFF;
        move[0] = pVec[0] + (rsin(candidateAngle) << 6);
        move[1] = 0;
        move[2] = pVec[2] - (rcos(candidateAngle) << 6);
        result = func_8007C694(move, (s32*)(actorData + 0x20), actorData,
                               (s16*)pOut, point, -1);

        if (result != -1) {
            candidateAngle = (resolvedAngle + 0x100) & 0x0FFF;
            move[0] = pVec[0] + (rsin(candidateAngle) << 6);
            move[1] = 0;
            move[2] = pVec[2] - (rcos(candidateAngle) << 6);
            result = func_8007C694(move, (s32*)(actorData + 0x20), actorData,
                                   (s16*)pOut, point, -1);

            if (result != -1) {
                move[0] = pVec[0];
                move[1] = pVec[1];
                move[2] = pVec[2];
            } else {
                move[0] = pVec[0];
                move[1] = pVec[1];
                move[2] = pVec[2];
                func_8007B6C4(resolvedAngle, (s16*)pOut, move);
            }
        } else {
            move[0] = pVec[0];
            move[1] = pVec[1];
            move[2] = pVec[2];
            func_8007B6C4(resolvedAngle, (s16*)pOut, move);
        }
    } else {
        move[0] = pVec[0];
        move[1] = pVec[1];
        move[2] = pVec[2];
        func_8007B6C4(resolvedAngle, (s16*)pOut, move);
    }

    if (func_8007C694(move, (s32*)(actorData + 0x20), actorData,
                      (s16*)pOut, point, 0) == -1) {
        return (void*)-1;
    }

    if ((*(u32*)actorData & 0x00040000) != 0) {
        point[1] = *(u16*)(actorData + 0xEC);
    } else if (((s32)point[1] << 16) < *(s32*)(actorData + 0x24) &&
               D_800ADB98 == 0) {
        return (void*)-1;
    }

    move[1] = ((s32)point[1] << 16) - *(s32*)(actorData + 0x24);
    pVec[0] = move[0];
    pVec[1] = move[1];
    pVec[2] = move[2];
    *(s16*)(actorData + 0x72) = (*(s32*)(actorData + 0x24) + pVec[1]) >> 16;
    return 0;
}
#endif /* XENO_PC_PORT */

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_collision_sweep_attribute_aware (src/decomp/src/field/field_motion.c:717). */
#ifndef XM_T_WHOLE
#define XM_T_WHOLE
#define WHOLE(value) (((Fixed *)&(value))->part.whole)
#endif
#ifndef XM_S_field_collision_sweep_attribute_aware
#define XM_S_field_collision_sweep_attribute_aware
s32 xm_field_collision_sweep_attribute_aware(VECTOR *delta, xm_FieldActor *actor, SVECTOR *edge, s16 heading) __asm__("func_8007BAC0");
#endif
#ifndef XM_S_field_collision_walk_attribute_aware
#define XM_S_field_collision_walk_attribute_aware
s32 xm_field_collision_walk_attribute_aware(VECTOR *probe, s32 *position, xm_FieldActor *actor, SVECTOR *edge, SVECTOR *floor, s32 mode,
                  s32 *attribute) __asm__("func_8007BEF4");
#endif
s32 func_8007BAC0(VECTOR *delta, xm_FieldActor *actor, SVECTOR *edge, s16 heading) {
    VECTOR probe;
    VECTOR saved;
    SVECTOR floor;
    SVECTOR saved_floor;
    VECTOR d;
    VECTOR n;
    s32 attribute;
    s32 angle;
    s32 speed;

    angle = heading - 0x100;
    angle &= 0xFFF;
    probe.vx = delta->vx + (xm_gpu_get_cos(angle) << 6);
    probe.vz = delta->vz - (xm_gpu_get_sin(angle) << 6);
    if (xm_field_collision_walk_attribute_aware(&probe, actor->position, actor, edge, &floor, -1, &attribute) == -1) {
        probe.vx = delta->vx;
        probe.vy = delta->vy;
        probe.vz = delta->vz;
        xm_field_collision_slide_along_edge(heading, edge, &probe, attribute);
    } else {
        angle = heading + 0x100;
        angle &= 0xFFF;
        probe.vx = delta->vx + (xm_gpu_get_cos(angle) << 6);
        probe.vz = delta->vz - (xm_gpu_get_sin(angle) << 6);
        if (xm_field_collision_walk_attribute_aware(&probe, actor->position, actor, edge, &floor, -1, &attribute) == -1) {
            probe.vx = delta->vx;
            probe.vy = delta->vy;
            probe.vz = delta->vz;
            xm_field_collision_slide_along_edge(heading, edge, &probe, attribute);
        } else {
            angle = heading & 0xFFF;
            probe.vx = delta->vx + (xm_gpu_get_cos(angle) << 6);
            probe.vz = delta->vz - (xm_gpu_get_sin(angle) << 6);
            if (xm_field_collision_walk_attribute_aware(&probe, actor->position, actor, edge, &floor, -1, &attribute) == -1) {
                probe.vx = delta->vx;
                probe.vy = delta->vy;
                probe.vz = delta->vz;
                xm_field_collision_slide_along_edge(heading, edge, &probe, attribute);
            } else {
                probe.vx = delta->vx;
                probe.vy = delta->vy;
                probe.vz = delta->vz;
            }
        }
    }
    if (xm_field_collision_walk_attribute_aware(&probe, actor->position, actor, edge, &floor, 0, &attribute) == -1) {
        return -1;
    }
    saved.vx = probe.vx;
    saved.vy = probe.vy;
    saved.vz = probe.vz;
    saved_floor.vx = floor.vx;
    saved_floor.vy = floor.vy;
    saved_floor.vz = floor.vz;
    if (WHOLE(actor->position[1]) > floor.vy) {
    push:
        d.vx = -probe.vx >> 8;
        d.vy = ((floor.vy << 16) - actor->position[1]) >> 8;
        d.vz = -probe.vz >> 8;
        xm_VectorNormal(&d, &n);
        speed = xm_field_compute_planar_length(probe.vx >> 8, probe.vz >> 8);
        probe.vx = -(speed * n.vx) >> 4;
        probe.vy = (speed * n.vy) >> 4;
        probe.vz = -(speed * n.vz) >> 4;
        if (xm_field_collision_walk_attribute_aware(&probe, actor->position, actor, edge, &floor, 0, &attribute) == -1) {
            return -1;
        }
        actor->flags |= 0x4000000;
        goto done;
    }
    if (attribute & 0x200000) {
        goto push;
    }
    if (attribute & 0x420000) {
        if (!(actor->floor_attribute & 0x420000)) {
            goto restore;
        }
        goto push;
    }
    if (floor.vy < WHOLE(actor->position[1]) + 0x40) {
        goto push;
    }
restore:
    probe.vx = saved.vx;
    probe.vy = saved.vy;
    probe.vz = saved.vz;
    floor.vx = saved_floor.vx;
    floor.vy = saved_floor.vy;
    floor.vz = saved_floor.vz;
done:
    probe.vy = (floor.vy << 16) - actor->position[1];
    delta->vx = probe.vx;
    delta->vy = probe.vy;
    delta->vz = probe.vz;
    actor->unk72 = (actor->position[1] + delta->vy) >> 16;
    return 0;
}
#endif
#else
void* func_8007BAC0(s32* pVec, void* pActorData, s32* pOut, s16 angle) {
    u8* actorData = pActorData;
    s16 signedAngle = angle;
    s16 edge[4];
    s16 point[16];
    s16 savedPoint[16];
    s32 move[3];
    s32 savedMove[3];
    s32 flags = 0;
    s32 candidateAngle;
    s32 magnitude;
    VECTOR normalIn;
    VECTOR normalOut;

    candidateAngle = (signedAngle - 0x100) & 0x0FFF;
    move[0] = pVec[0] + (rsin(candidateAngle) << 6);
    move[1] = 0;
    move[2] = pVec[2] - (rcos(candidateAngle) << 6);
    if (func_8007BEF4(move, (s32*)(actorData + 0x20), actorData,
                      (s16*)pOut, point, -1, &flags) != -1) {
        candidateAngle = (signedAngle + 0x100) & 0x0FFF;
        move[0] = pVec[0] + (rsin(candidateAngle) << 6);
        move[1] = 0;
        move[2] = pVec[2] - (rcos(candidateAngle) << 6);
        if (func_8007BEF4(move, (s32*)(actorData + 0x20), actorData,
                          (s16*)pOut, point, -1, &flags) != -1) {
            candidateAngle = angle & 0x0FFF;
            move[0] = pVec[0] + (rsin(candidateAngle) << 6);
            move[1] = 0;
            move[2] = pVec[2] - (rcos(candidateAngle) << 6);
            if (func_8007BEF4(move, (s32*)(actorData + 0x20), actorData,
                              (s16*)pOut, point, -1, &flags) != -1) {
                move[0] = pVec[0];
                move[1] = pVec[1];
                move[2] = pVec[2];
            } else {
                move[0] = pVec[0];
                move[1] = pVec[1];
                move[2] = pVec[2];
                func_8007B6C4(signedAngle, (s16*)pOut, move);
            }
        } else {
            move[0] = pVec[0];
            move[1] = pVec[1];
            move[2] = pVec[2];
            func_8007B6C4(signedAngle, (s16*)pOut, move);
        }
    } else {
        move[0] = pVec[0];
        move[1] = pVec[1];
        move[2] = pVec[2];
        func_8007B6C4(signedAngle, (s16*)pOut, move);
    }

    if (func_8007BEF4(move, (s32*)(actorData + 0x20), actorData,
                      (s16*)pOut, point, 0, &flags) == -1) {
        return (void*)-1;
    }

    savedMove[0] = move[0];
    savedMove[1] = move[1];
    savedMove[2] = move[2];
    savedPoint[0] = point[0];
    savedPoint[1] = point[1];
    savedPoint[2] = point[2];
    savedPoint[3] = point[3];

    if (point[1] < *(s16*)(actorData + 0x26)) {
retry_along_normal:
        normalIn.vx = -move[0] >> 8;
        normalIn.vy = (((s32)point[1] << 16) - *(s32*)(actorData + 0x24)) >> 8;
        normalIn.vz = -move[2] >> 8;
        VectorNormal(&normalIn, &normalOut);

        magnitude = FieldGetVec2Magnitude(move[0] >> 8, move[2] >> 8);
        move[0] = -(magnitude * normalOut.vx) >> 4;
        move[1] = (magnitude * normalOut.vy) >> 4;
        move[2] = -(magnitude * normalOut.vz) >> 4;

        if (func_8007BEF4(move, (s32*)(actorData + 0x20), actorData,
                          (s16*)pOut, point, 0, &flags) == -1) {
            return (void*)-1;
        }

        *(u32*)actorData |= 0x04000000;
    } else {
        if ((flags & 0x00200000) != 0) {
            goto retry_along_normal;
        }

        if ((flags & 0x00420000) != 0) {
            if ((*(u32*)(actorData + 0x14) & 0x00420000) != 0) {
                goto retry_along_normal;
            }
        } else if (point[1] < *(s16*)(actorData + 0x26) + 0x40) {
            goto retry_along_normal;
        }

        move[0] = savedMove[0];
        move[1] = savedMove[1];
        move[2] = savedMove[2];
        point[0] = savedPoint[0];
        point[1] = savedPoint[1];
        point[2] = savedPoint[2];
        point[3] = savedPoint[3];
    }

    move[1] = ((s32)point[1] << 16) - *(s32*)(actorData + 0x24);
    pVec[0] = move[0];
    pVec[1] = move[1];
    pVec[2] = move[2];
    *(s16*)(actorData + 0x72) = (*(s32*)(actorData + 0x24) + pVec[1]) >> 16;
    return 0;
}
#endif /* XENO_PC_PORT */

static inline s32 FieldPackedXZ(const s16* vert) {
    return ((s32)vert[0] << 16) + vert[2];
}

static inline void FieldCopyCameraEdge(u8* out, const s16* a, const s16* b) {
    *(u16*)(out + 0x00) = (u16)a[0];
    *(u16*)(out + 0x02) = (u16)a[1];
    *(u16*)(out + 0x04) = (u16)a[2];
    *(u16*)(out + 0x08) = (u16)b[0];
    *(u16*)(out + 0x0A) = (u16)b[1];
    *(u16*)(out + 0x0C) = (u16)b[2];
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_8007BEF4);
#else
s32 func_8007BEF4(s32* move, s32* base, u8* actorData, s16* outEdge,
                  s16* outPoint, s32 mode, s32* outFlags) {
    s32 state = *(s16*)(actorData + 0x10);
    s16 triIndex = *(s16*)(actorData + state * 2 + 0x08);
    u8* triBase = (u8*)(uintptr_t)(u32)D_800AFB24[state];
    u8* vertBase = (u8*)(uintptr_t)(u32)D_800AFB34[state];
    s32 packedNewXZ;
    s32 packedOldXZ;
    s32 collisionMask = 0;
    s32 forceEdgeSearch;
    s32 steps = 0;
    s32 lastTri = triIndex;
    s32 sideMask = 0;

    if (triIndex == -1) {
        return -1;
    }

    outPoint[0] = (base[0] + move[0]) >> 16;
    outPoint[1] = 0;
    outPoint[2] = (base[2] + move[2]) >> 16;
    packedNewXZ = ((s32)outPoint[0] << 16) + outPoint[2];
    packedOldXZ = ((base[0] >> 16) << 16) + (base[2] >> 16);

    if (((*(u32*)(actorData + 0x04) >> (state + 3)) & 1) == 0) {
        collisionMask = (D_800B21CC == 0) ? -1 : 0;
    }

    /* NOTE: kept hoisted here. Moving this computation inside the loop below
     * (to mimic retail's duplicated `triBase + idx*14` scaling at 8007BFF0 and
     * 8007C00C) was TRIED and MEASURED: it made func_8007BEF4 1504 -> 1484 B,
     * i.e. 20 bytes WORSE, because GCC then treats the whole sequence as
     * loop-invariant and hoists more. Do not re-attempt without new evidence. */
    {
        s16* tri = (s16*)(triBase + triIndex * 14);
        u32 triFlags = ((u32*)(uintptr_t)D_800AFB20[0])[*((u8*)tri + 0x0C)];
        forceEdgeSearch = ((triFlags & 0x00400000) != 0) || mode == 0x80;
    }

    for (;;) {
        s16* tri = (s16*)(triBase + triIndex * 14);
        s16* v0 = (s16*)(vertBase + tri[0] * 8);
        s16* v1 = (s16*)(vertBase + tri[1] * 8);
        s16* v2 = (s16*)(vertBase + tri[2] * 8);
        s32 p0 = FieldPackedXZ(v0);
        s32 p1 = FieldPackedXZ(v1);
        s32 p2 = FieldPackedXZ(v2);
        s32 nextTri = -1;
        u32 flags;

        lastTri = triIndex;
        sideMask = 0;
        if (NormalClip(p0, p1, packedNewXZ) < 0) {
            sideMask |= 1;
        }
        if (NormalClip(p1, p2, packedNewXZ) < 0) {
            sideMask |= 2;
        }
        if (NormalClip(p2, p0, packedNewXZ) < 0) {
            sideMask |= 4;
        }

        switch (sideMask) {
        case 0:
            steps = 0xFF;
            break;
        case 1:
            nextTri = tri[3];
            break;
        case 2:
            nextTri = tri[4];
            break;
        case 3:
            if (NormalClip(p1, packedNewXZ, packedOldXZ) < 0) {
                nextTri = tri[3];
#ifndef FIELD_WALKMESH_MUTANT_KEEP_COMPOUND_SIDE
                sideMask = 1;
#endif
            } else {
                nextTri = tri[4];
#ifndef FIELD_WALKMESH_MUTANT_KEEP_COMPOUND_SIDE
                sideMask = 2;
#endif
            }
            break;
        case 4:
            nextTri = tri[5];
            break;
        case 5:
            if (NormalClip(p0, packedNewXZ, packedOldXZ) < 0) {
                nextTri = tri[5];
#ifndef FIELD_WALKMESH_MUTANT_KEEP_COMPOUND_SIDE
                sideMask = 4;
#endif
            } else {
                nextTri = tri[3];
#ifndef FIELD_WALKMESH_MUTANT_KEEP_COMPOUND_SIDE
                sideMask = 1;
#endif
            }
            break;
        case 6:
            if (NormalClip(p2, packedNewXZ, packedOldXZ) >= 0) {
                nextTri = tri[5];
#ifndef FIELD_WALKMESH_MUTANT_KEEP_COMPOUND_SIDE
                sideMask = 4;
#endif
            } else {
                nextTri = tri[4];
#ifndef FIELD_WALKMESH_MUTANT_KEEP_COMPOUND_SIDE
                sideMask = 2;
#endif
            }
            break;
        case 7:
            /* Retail's switch has EIGHT cases: the dispatch range check is
             * `sltiu ..., 0x8` (x2) in
             * asm/field/matchings/main/misc4/func_8007BEF4.s and jtbl_8006FB8C has
             * eight entries. Index 7's target is 0x8007C244 =
             * `addiu $s0, $zero, -0x1` (line 241), i.e. triIndex = -1 - the same
             * body our `default:` had, which is why GCC emitted a literal-0 8th
             * table slot instead of a real address. */
            triIndex = -1;
            break;
        default:
            triIndex = -1;
            break;
        }

        if (sideMask != 0 && sideMask < 7) {
            triIndex = (s16)nextTri;
        }

        if (triIndex != -1) {
            tri = (s16*)(triBase + triIndex * 14);
            flags = ((u32*)(uintptr_t)D_800AFB20[0])[*((u8*)tri + 0x0C)] & collisionMask;
            *outFlags = flags;

            if ((((*(u32*)actorData >> 8) & 7) & (flags >> 5)) != 0) {
                triIndex = -1;
            } else if ((flags & 0x00800000) != 0 && *(s16*)(actorData + 0x10) == 0) {
                triIndex = -1;
            } else if ((flags & 0x00400000) != 0) {
                if (!forceEdgeSearch) {
                    func_8007B07C((s16*)(vertBase + tri[0] * 8),
                                  (s16*)(vertBase + tri[1] * 8),
                                  (s16*)(vertBase + tri[2] * 8),
                                  outPoint, (VECTOR*)((u8*)outPoint + 0x08));
                    if (outPoint[1] < *(s16*)((u8*)base + 0x06)) {
                        triIndex = -1;
                    }
                }
            }
        }

        if (triIndex != -1) {
            steps++;
            if (steps < 0x20) {
                continue;
            }
        }
        break;
    }

    if (triIndex != -1 && steps != 0x20) {
        if (mode == -1) {
            return 0;
        }

        {
            s16* tri = (s16*)(triBase + triIndex * 14);
            func_8007B07C((s16*)(vertBase + tri[0] * 8),
                          (s16*)(vertBase + tri[1] * 8),
                          (s16*)(vertBase + tri[2] * 8),
                          outPoint, (VECTOR*)((u8*)outPoint + 0x08));
        }
        return 0;
    }

    if (sideMask == 1 || sideMask == 2 || sideMask == 4) {
        s16* tri = (s16*)(triBase + lastTri * 14);
        /* Retail's three arms share the final b.z store at 8007C628.
         * Keep the matching-build expression unchanged here; the native
         * branches below restore that shared-tail effect for edges 1/2. */
        if (sideMask == 1) {
            const s16* a = (const s16*)(vertBase + tri[0] * 8);
            const s16* b = (const s16*)(vertBase + tri[1] * 8);

            ((u16*)outEdge)[0x0] = (u16)a[0];
            ((u16*)outEdge)[0x1] = (u16)a[1];
            ((u16*)outEdge)[0x2] = (u16)a[2];
            ((u16*)outEdge)[0x4] = (u16)b[0];
            ((u16*)outEdge)[0x5] = (u16)b[1];
#ifdef XENO_PC_PORT
            /* 8007C4C0 / 8007C570 jump to the b.z store at 8007C634. */
            ((u16*)outEdge)[0x6] = (u16)b[2];
#endif
        } else if (sideMask == 2) {
            const s16* a = (const s16*)(vertBase + tri[1] * 8);
            const s16* b = (const s16*)(vertBase + tri[2] * 8);

            ((u16*)outEdge)[0x0] = (u16)a[0];
            ((u16*)outEdge)[0x1] = (u16)a[1];
            ((u16*)outEdge)[0x2] = (u16)a[2];
            ((u16*)outEdge)[0x4] = (u16)b[0];
            ((u16*)outEdge)[0x5] = (u16)b[1];
#ifdef XENO_PC_PORT
            /* 8007C4C0 / 8007C570 jump to the b.z store at 8007C634. */
            ((u16*)outEdge)[0x6] = (u16)b[2];
#endif
        } else {
            const s16* a = (const s16*)(vertBase + tri[2] * 8);
            const s16* b = (const s16*)(vertBase + tri[0] * 8);

            ((u16*)outEdge)[0x0] = (u16)a[0];
            ((u16*)outEdge)[0x1] = (u16)a[1];
            ((u16*)outEdge)[0x2] = (u16)a[2];
            ((u16*)outEdge)[0x4] = (u16)b[0];
            ((u16*)outEdge)[0x5] = (u16)b[1];
            ((u16*)outEdge)[0x6] = (u16)b[2];
        }
    }

    return -1;
}
#endif /* XENO_PC_PORT */

void func_8007C670(s32* arg0, s32* arg1, s32 arg2) {
    s32 value = *arg0;

    if (arg2 >= 0) {
        *arg0 = value;
        value += arg2;
    } else {
        *arg0 = value;
    }

    *arg1 = value;
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_8007C694);
#else
s32 func_8007C694(s32* move, s32* base, void* pActorData,
                  s16* outEdge, s16* outPoint, s32 mode) {
    u8* actorData = pActorData;
    s32 state = *(s16*)(actorData + 0x10);
    s16 triIndex = *(s16*)(actorData + state * 2 + 0x08);
    u8* triBase = (u8*)(uintptr_t)(u32)D_800AFB24[state];
    u8* vertBase = (u8*)(uintptr_t)(u32)D_800AFB34[state];
    s32 packedNewXZ;
    s32 packedOldXZ;
    s32 collisionMask = 0;
    s32 steps = 0;
    s32 lastTri = triIndex;
    s32 sideMask = 0;

    if (triIndex == -1) {
        return -1;
    }

    outPoint[0] = (base[0] + move[0]) >> 16;
    outPoint[1] = 0;
    outPoint[2] = (base[2] + move[2]) >> 16;
    packedNewXZ = ((s32)outPoint[0] << 16) + outPoint[2];
    packedOldXZ = ((base[0] >> 16) << 16) + (base[2] >> 16);

    if (((*(u32*)(actorData + 0x04) >> (state + 3)) & 1) == 0) {
        collisionMask = (D_800B21CC == 0) ? -1 : 0;
    }

    for (;;) {
        s16* tri = (s16*)(triBase + triIndex * 14);
        s16* v0 = (s16*)(vertBase + tri[0] * 8);
        s16* v1 = (s16*)(vertBase + tri[1] * 8);
        s16* v2 = (s16*)(vertBase + tri[2] * 8);
        s32 p0 = FieldPackedXZ(v0);
        s32 p1 = FieldPackedXZ(v1);
        s32 p2 = FieldPackedXZ(v2);
        s32 nextTri = -1;
        u32 flags;

        lastTri = triIndex;
        sideMask = 0;
        if (NormalClip(p0, p1, packedNewXZ) < 0) {
            sideMask |= 1;
        }
        if (NormalClip(p1, p2, packedNewXZ) < 0) {
            sideMask |= 2;
        }
        if (NormalClip(p2, p0, packedNewXZ) < 0) {
            sideMask |= 4;
        }

        switch (sideMask) {
        case 0:
            steps = 0xFF;
            break;
        case 1:
            nextTri = tri[3];
            break;
        case 2:
            nextTri = tri[4];
            break;
        case 3:
            if (NormalClip(p1, packedNewXZ, packedOldXZ) < 0) {
                nextTri = tri[3];
                sideMask = 1;
            } else {
                nextTri = tri[4];
                sideMask = 2;
            }
            break;
        case 4:
            nextTri = tri[5];
            break;
        case 5:
            if (NormalClip(p0, packedNewXZ, packedOldXZ) < 0) {
                nextTri = tri[5];
                sideMask = 4;
            } else {
                nextTri = tri[3];
                sideMask = 1;
            }
            break;
        case 6:
            if (NormalClip(p2, packedNewXZ, packedOldXZ) >= 0) {
                nextTri = tri[5];
                sideMask = 4;
            } else {
                nextTri = tri[4];
                sideMask = 2;
            }
            break;
        case 7:
            /* Same missing case as in func_8007BEF4 above: the retail dispatch in
             * asm/field/matchings/main/misc4/func_8007C694.s has two
             * `sltiu ..., 0x8` range checks (eight cases) and jtbl_8006FBAC has
             * eight entries; index 7's target 0x8007C980 is
             * `addiu $s0, $zero, -0x1` (line 214) = triIndex = -1. */
            triIndex = -1;
            break;
        default:
            triIndex = -1;
            break;
        }

        if (sideMask != 0 && triIndex != -1) {
            triIndex = (s16)nextTri;
        }

        if (triIndex == -1) {
            break;
        }

        tri = (s16*)(triBase + triIndex * 14);
        flags = ((u32*)(uintptr_t)D_800AFB20[0])[*((u8*)tri + 0x0C)] & collisionMask;

        if ((((*(u32*)actorData >> 9) & 3) & (flags >> 3)) != 0 ||
            (((*(u32*)actorData >> 8) & 7) & (flags >> 5)) != 0 ||
            ((flags & 0x00800000) != 0 && *(s16*)(actorData + 0x10) == 0)) {
            triIndex = -1;
            break;
        }

        steps++;
        if (steps < 0x20) {
            continue;
        }
        break;
    }

    if (triIndex != -1 && steps != 0x20) {
        if (mode == -1) {
            return 0;
        }

        {
            s16* tri = (s16*)(triBase + triIndex * 14);
            func_8007B07C((s16*)(vertBase + tri[0] * 8),
                          (s16*)(vertBase + tri[1] * 8),
                          (s16*)(vertBase + tri[2] * 8),
                          outPoint, (VECTOR*)((u8*)outPoint + 0x08));
        }
        return 0;
    }

    if (sideMask == 1 || sideMask == 2 || sideMask == 4) {
        s16* tri = (s16*)(triBase + lastTri * 14);
        if (sideMask == 1) {
            FieldCopyCameraEdge((u8*)outEdge, (s16*)(vertBase + tri[0] * 8),
                                (s16*)(vertBase + tri[1] * 8));
        } else if (sideMask == 2) {
            FieldCopyCameraEdge((u8*)outEdge, (s16*)(vertBase + tri[1] * 8),
                                (s16*)(vertBase + tri[2] * 8));
        } else {
            FieldCopyCameraEdge((u8*)outEdge, (s16*)(vertBase + tri[2] * 8),
                                (s16*)(vertBase + tri[0] * 8));
        }
    }

    return -1;
}
#endif /* XENO_PC_PORT */

extern s32 D_800ADC10;

void* func_8007CD3C(s32 a0) {
    s32 old = D_800ADC10;
    D_800ADC10 = old + a0;
    return (void*)((old << 2) + 0x1F800000);
}

void func_8007CD60(s32 a0) {
    D_800ADC10 -= a0;
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_8007CD80);
#else
s32 func_8007CD80(void* arg0, void* arg1, void* arg2) {
    s16 posX = *(s16*)((u8*)arg0 + 0x02);
    s16 posZ = *(s16*)((u8*)arg0 + 0x0A);
    s16 clampedX;
    s16 clampedZ;
    s32 packedPos = ((s32)posX << 16) + posZ;
    s32 packedClamped;
    u8* triBase;
    u8* vertBase;
    s16 triIndex;
    s16 edgeTri = 0;
    s32 edge = 0;
    s32 steps = 0;

    if (posX < *(s16*)((u8*)&g_Scene + 0x4C)) {
        clampedX = *(s16*)((u8*)&g_Scene + 0x4C);
    } else {
        s32 maxX = *(s16*)((u8*)&g_Scene + 0x4C) +
                   *(s16*)((u8*)&g_Scene + 0x50);
        clampedX = (maxX < posX) ? (s16)maxX : posX;
    }

    if (*(s16*)((u8*)&g_Scene + 0x4E) < posZ) {
        clampedZ = *(s16*)((u8*)&g_Scene + 0x4E);
    } else {
        s32 maxZ = *(s16*)((u8*)&g_Scene + 0x4E) +
                   *(s16*)((u8*)&g_Scene + 0x52);
        clampedZ = (posZ < maxZ) ? (s16)maxZ : posZ;
    }

    packedClamped = ((s32)clampedX << 16) + clampedZ;
    triIndex = func_8007B1C4(clampedX, clampedZ, D_800AFB54 - 1,
                             (s16*)arg2, (s32*)((u8*)arg2 + 0x08));
    triBase = (u8*)(uintptr_t)D_800AFB20[D_800AFB54];
    vertBase = (u8*)(uintptr_t)D_800AFB20[D_800AFB54 + 4];

    for (;;) {
        s16* tri;
        s16* v0;
        s16* v1;
        s16* v2;
        s32 p0;
        s32 p1;
        s32 p2;
        s32 sideMask;
        s32 nextTri;

        if (triIndex < 0) {
            break;
        }

        tri = (s16*)(triBase + triIndex * 14);
        v0 = (s16*)(vertBase + tri[0] * 8);
        v1 = (s16*)(vertBase + tri[1] * 8);
        v2 = (s16*)(vertBase + tri[2] * 8);
        p0 = FieldPackedXZ(v0);
        p1 = FieldPackedXZ(v1);
        p2 = FieldPackedXZ(v2);

        sideMask = 0;
        {
            s32 nclip;

            gte_NormalClip(p0, p1, packedPos, &nclip);
            if (nclip < 0) {
                sideMask |= 1;
            }
            gte_NormalClip(p1, p2, packedPos, &nclip);
            if (nclip < 0) {
                sideMask |= 2;
            }
            gte_NormalClip(p2, p0, packedPos, &nclip);
            if (nclip < 0) {
                sideMask |= 4;
            }
        }

        nextTri = -1;
        switch (sideMask) {
        case 0:
            steps = 0xFF;
            break;
        case 1:
            nextTri = *(s16*)((u8*)triBase + triIndex * 14 + 6);
            break;
        case 2:
            nextTri = *(s16*)((u8*)triBase + triIndex * 14 + 8);
            break;
        case 3: {
            s32 nclip;

            gte_NormalClip(p1, packedPos, packedClamped, &nclip);
            nextTri = (nclip < 0)
                ? *(s16*)((u8*)triBase + triIndex * 14 + 6)
                : *(s16*)((u8*)triBase + triIndex * 14 + 8);
            break;
        }
        case 4:
            nextTri = *(s16*)((u8*)triBase + triIndex * 14 + 10);
            break;
        case 5: {
            s32 nclip;

            gte_NormalClip(p0, packedPos, packedClamped, &nclip);
            nextTri = (nclip < 0)
                ? *(s16*)((u8*)triBase + triIndex * 14 + 10)
                : *(s16*)((u8*)triBase + triIndex * 14 + 6);
            break;
        }
        case 6: {
            s32 nclip;

            gte_NormalClip(p2, packedPos, packedClamped, &nclip);
            nextTri = (nclip >= 0)
                ? *(s16*)((u8*)triBase + triIndex * 14 + 10)
                : *(s16*)((u8*)triBase + triIndex * 14 + 8);
            break;
        }
        case 7:
            /* Retail switch admits 8 values: dispatch is
             * `sltiu $v0, $a1, 0x8` (x2) in
             * asm/field/matchings/main/misc4/func_8007CD80.s lines 159/161
             * (0x8007CF94 / 0x8007CF9C), and jtbl_8006FBCC has eight entries.
             * $a1 is sideMask. Index 7's target 0x8007D0F0 is
             * `addiu $a3, $zero, -0x1` (line 250). $a3 is the live triangle
             * index written by cases 1-6 (nextTri in this C). */
#ifdef FIELD_WALKMESH_MUTANT_CASE7_WALK
            nextTri = tri[3];
#else
            nextTri = -1;
#endif
            break;
        default:
            nextTri = -1;
            break;
        }

        if (sideMask != 0) {
            edgeTri = triIndex;
            triIndex = (s16)nextTri;
            edge = sideMask;
        }

        {
            s16* curTri;
            u32 flags;

            if (triIndex < 0) {
                break;
            }

            curTri = (s16*)(triBase + triIndex * 14);
            flags = ((u32*)(uintptr_t)D_800AFB20[0])[*((u8*)curTri + 0x0C)];
            if ((flags & 0x800000) == 0) {
                triIndex = -1;
                break;
            }
        }

        steps++;
        if (triIndex == -1 || steps >= 0xF0) {
            break;
        }
        if (sideMask == 0) {
            break;
        }
    }

    if (triIndex != -1 && steps != 0xF0) {
        return 0;
    }

    if (edge == 1 || edge == 2 || edge == 4) {
        s16* tri = (s16*)(triBase + edgeTri * 14);
        s16* a;
        s16* b;

        if (edge == 1) {
            a = (s16*)(vertBase + tri[0] * 8);
            b = (s16*)(vertBase + tri[1] * 8);
        } else if (edge == 2) {
            a = (s16*)(vertBase + tri[1] * 8);
            b = (s16*)(vertBase + tri[2] * 8);
        } else {
            a = (s16*)(vertBase + tri[2] * 8);
            b = (s16*)(vertBase + tri[0] * 8);
        }
        FieldCopyCameraEdge((u8*)arg1, a, b);
    }

    *(s16*)((u8*)arg2 + 0x00) = posX;
    *(s16*)((u8*)arg2 + 0x02) = posZ;
    *(s16*)((u8*)arg2 + 0x04) = clampedX;
    *(s16*)((u8*)arg2 + 0x06) = clampedZ;
    return -1;
}
#endif /* XENO_PC_PORT */

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_8007D3D4);
#else
s32 func_8007D3D4(u8* actorData, s32 idx, s32* outHeight0,
                  VECTOR* outNormal, s16* outTriangle, s32* outHeight1) {
    s16 triIndex;
    u8* triBase;
    u8* vertBase;
    s16 point[4];
    s32 packedNewXZ;
    s32 packedOldXZ;
    s32 collisionMask;
    s32 steps;

    triIndex = *(s16*)(actorData + idx * 2 + 0x08);
    triBase = (u8*)(uintptr_t)(u32)D_800AFB24[idx];
    vertBase = (u8*)(uintptr_t)(u32)D_800AFB34[idx];

    if (triIndex == -1) {
        return -1;
    }

    point[0] = (*(s32*)(actorData + 0x20) + *(s32*)(actorData + 0x30)) >> 16;
    point[1] = 0;
    point[2] = (*(s32*)(actorData + 0x28) + *(s32*)(actorData + 0x38)) >> 16;
    packedNewXZ = (point[0] << 16) + point[2];
    packedOldXZ = ((*(s32*)(actorData + 0x20) >> 16) << 16) +
                  (*(s32*)(actorData + 0x28) >> 16);

    collisionMask = 0;
    if (((*(u32*)(actorData + 0x04) >> (idx + 3)) & 1) == 0) {
        collisionMask = (D_800B21CC == 0) ? -1 : 0;
    }

    steps = 0;
    for (;;) {
        s16* tri = (s16*)(triBase + triIndex * 14);
        s16* v0 = (s16*)(vertBase + tri[0] * 8);
        s16* v1 = (s16*)(vertBase + tri[1] * 8);
        s16* v2 = (s16*)(vertBase + tri[2] * 8);
        s32 p0 = (v0[0] << 16) + v0[2];
        s32 p1 = (v1[0] << 16) + v1[2];
        s32 p2 = (v2[0] << 16) + v2[2];
        s32 sideMask = 0;
        s32 nextTri = -1;

        if (NormalClip(p0, p1, packedNewXZ) < 0) {
            sideMask |= 1;
        }
        if (NormalClip(p1, p2, packedNewXZ) < 0) {
            sideMask |= 2;
        }
        if (NormalClip(p2, p0, packedNewXZ) < 0) {
            sideMask |= 4;
        }

        /* Retail's case-body emission order is 0,3,5,1,6,2,4,7, recovered from
         * jtbl_8006FBEC's target addresses (0x8007D5A8, 0x8007D5B0, 0x8007D5D0,
         * 0x8007D5E8, 0x8007D604, 0x8007D61C, 0x8007D638, 0x8007D654). Index i
         * still dispatches to case value i, and every body ends in `break`, so
         * this label order is behaviour-neutral.
         *
         * GCC 2.7.2 lays the bodies out by CFG, not source order: indexing the
         * cached `tri` local ranks the emitted table (0,2,6,1,5,3,4,7) for any
         * source order. Writing the address as `triBase + triIndex * 14` in each
         * body reproduces retail's shape (0x8007D5E8/0x8007D61C/0x8007D638
         * compute it, and cases 3/5/6 branch into those shared suffixes), which
         * is what lets this label order canonicalise to the retail table. */
        switch (sideMask) {
        case 0:
            steps = 0xFF;
            break;
        case 3:
            nextTri = (NormalClip(p1, packedNewXZ, packedOldXZ) < 0)
                ? *(s16*)((u8*)triBase + triIndex * 14 + 6)
                : *(s16*)((u8*)triBase + triIndex * 14 + 8);
            break;
        case 5:
            nextTri = (NormalClip(p0, packedNewXZ, packedOldXZ) < 0)
                ? *(s16*)((u8*)triBase + triIndex * 14 + 10)
                : *(s16*)((u8*)triBase + triIndex * 14 + 6);
            break;
        case 1:
#ifdef FIELD_WALKMESH_MUTANT_D3D4_SWAP12
            nextTri = *(s16*)((u8*)triBase + triIndex * 14 + 8);
#else
            nextTri = *(s16*)((u8*)triBase + triIndex * 14 + 6);
#endif
            break;
        case 6:
            nextTri = (NormalClip(p2, packedNewXZ, packedOldXZ) >= 0)
                ? *(s16*)((u8*)triBase + triIndex * 14 + 10)
                : *(s16*)((u8*)triBase + triIndex * 14 + 8);
            break;
        case 2:
#ifdef FIELD_WALKMESH_MUTANT_D3D4_SWAP12
            nextTri = *(s16*)((u8*)triBase + triIndex * 14 + 6);
#else
            nextTri = *(s16*)((u8*)triBase + triIndex * 14 + 8);
#endif
            break;
        case 4:
            nextTri = *(s16*)((u8*)triBase + triIndex * 14 + 10);
            break;
        case 7:
            /* Retail's switch really does have a case 7: its dispatch range check is
             * `sltiu $v0, $s0, 0x8` (asm/field/matchings/main/misc4/func_8007D3D4.s
             * lines 127/129) and jtbl_8006FBEC has EIGHT entries. Our C previously
             * stopped at case 6 and let `default:` absorb index 7, which is why the
             * built table had only 7 entries where retail has 8 - the last 4 bytes of
             * the field rodata discrepancy. The index-7 target in retail is
             * 0x8007D654 = `addiu $s1, $zero, -0x1`, i.e. nextTri = -1. */
            nextTri = -1;
            break;
        default:
            nextTri = -1;
            break;
        }

        if (sideMask == 0) {
            steps++;
        } else {
            triIndex = nextTri;
            if (triIndex == -1) {
                return -1;
            }
            steps++;
            if (steps < 0x20) {
                continue;
            }
            if (steps == 0x20) {
                return -1;
            }
        }

        tri = (s16*)(triBase + triIndex * 14);
        v0 = (s16*)(vertBase + tri[0] * 8);
        v1 = (s16*)(vertBase + tri[1] * 8);
        v2 = (s16*)(vertBase + tri[2] * 8);
        func_8007B07C(v0, v1, v2, point, outNormal);

        *outTriangle = triIndex;

        {
            s32 extraHeight = ((s8)*((u8*)tri + 0x0D)) << 2;
            u32 flags = ((u32*)(uintptr_t)D_800AFB20[0])[*((u8*)tri + 0x0C)];
            s32 blocked = flags & (collisionMask & 0x800000);

            if (*(s16*)(actorData + 0x10) != idx) {
                if (blocked != 0) {
                    *outHeight0 = 0x7FFFFFFF;
                    *outHeight1 = 0x7FFFFFFF;
                    return 0;
                }
                *outHeight0 = point[1];
                func_8007C670(outHeight0, outHeight1, extraHeight);
                return 0;
            }

            if (blocked != 0) {
                *outHeight0 = 0x7FFFFFFF;
                *outHeight1 = 0x7FFFFFFF;
                return 0;
            }

            if (*(s32*)(actorData + 0x30) == 0 &&
                *(s32*)(actorData + 0x34) == 0 &&
                *(s32*)(actorData + 0x38) == 0) {
                *outHeight0 = point[1];
            } else {
                *outHeight0 = *(s16*)(actorData + 0x72);
            }
            func_8007C670(outHeight0, outHeight1, extraHeight);
            return 0;
        }
    }
}
#endif /* XENO_PC_PORT */

extern s32 func_8007D8B4(s32 a, s32 b, s32 c);

void func_8007D818(s32* pVec, VECTOR* pOut) {
    s32 result = func_8007D8B4(pVec[0], pVec[1], pVec[2]);
    pVec[0] >>= 12;
    pVec[1] >>= 12;
    pVec[2] >>= 12;
    if (result < 0) {
        pVec[0] = -pVec[0];
        pVec[1] = -pVec[1];
        pVec[2] = -pVec[2];
    }
    VectorNormal((VECTOR*)pVec, pOut);
}

#ifndef XENO_PC_PORT
/* func_8007D8B4: abs via if (v < 0) v = -v (the third on the raw arg), then the
 * three dominance tests with a final return 0 (store-flag & mask). */
#endif
s32 func_8007D8B4(s32 x, s32 y, s32 z) {
#ifdef XENO_PC_PORT
    s32 ax = x < 0 ? -x : x;
    s32 ay = y < 0 ? -y : y;
    s32 az = z < 0 ? -z : z;
    if (ax >= ay) {
        if (ax >= az) return x;
    } else if (ay >= az) {
        return y;
    }
    return z;
#else
    s32 ax = x;
    s32 ay = y;
    s32 az = z;

    if (ax < 0) {
        ax = -ax;
    }
    if (ay < 0) {
        ay = -ay;
    }
    if (z < 0) {
        az = -z;
    }
    if (ax >= ay && ax >= az) {
        return x;
    }
    if (ay >= ax && ay >= az) {
        return y;
    }
    if (az >= ax && az >= ay) {
        return z;
    }
    return 0;
#endif
}
