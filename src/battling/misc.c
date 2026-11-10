/* battling misc: retail functions in retail order (asm/battling/CB8.s).
 * Functions still listed as INCLUDE_ASM are not byte-matching C yet. The whole
 * file is retail-build only. */
#include "common.h"
#include "system/memory.h"

#ifndef XENO_PC_PORT
extern s32 D_800912F4;
extern u8 D_80099D98[];
extern u8 D_80099D98__s_u8 asm("D_80099D98");
s32 func_8007F97C(void);
#endif

#ifndef XENO_PC_PORT
extern s32 D_80092AD8;
void func_8007BB7C(void);
#endif

#ifndef XENO_PC_PORT
s32 GetStringEntry(s32, s32);
void func_80034714(s32 *, s32);
void func_8007191C(s32);
extern s32 D_80090F38[];
extern s32 D_800925DC[];
extern s8 D_80092604[];
extern s32 D_80092880[];
extern s8 D_80092884__a_s8[] asm("D_80092884");
extern s32 D_800928C8__a_s32[] asm("D_800928C8");
extern s32 D_80092900__a_s32[] asm("D_80092900");
extern s32 D_80092904[];
extern s32 D_80092954__a_s32[] asm("D_80092954");
extern s8 D_800929BC[];
extern s8 D_80099D9D__a_s8[] asm("D_80099D9D");
extern s8 D_80099D9E__a_s8[] asm("D_80099D9E");
void func_800719F0(void);

extern u8 D_80099DA1__a[] asm("D_80099DA1");
extern u8 D_80099D9A__a[] asm("D_80099D9A");
extern u8 D_80099D9D__a[] asm("D_80099D9D");
extern u8 D_80099D9E__a[] asm("D_80099D9E");
extern u8 D_80099D9F__a[] asm("D_80099D9F");
extern u8 D_80099DA2__a[] asm("D_80099DA2");
void func_80080090(void);
void func_800800CC(void);
void func_80080180(void);
void func_800801BC(void);
void func_800801F8(void);
void func_80080268(void);

extern u8 D_80099D98[];
extern u8 D_80099D9B[];
extern u8 D_80099D9C[];
extern s32 D_8006F980[];
void func_80080054(void);
void func_80080108(void);
void func_80080144(void);
void func_80088940(void);

void func_800346D4(s32 *);
void func_80083C0C(s32);
extern s32 D_80092954;
extern s32 D_800925F8;
extern s32 D_800925FC;
extern s32 D_80097058;
extern s32 D_800970E0;
extern s32 D_80098774;
extern s32 D_800987FC;
void func_80032F54(s32 *, s32, s32, s32, s32, s32, s32);
extern s32 D_800925D4;
extern s32 D_800925D8;
extern s32 D_8009868C;
extern s8 D_8009293C;
extern s32 D_800910C4;
extern s8 D_800928D4;
extern s32 D_80092900;
extern s32 D_80092A00;
extern s32 D_80092A10;
extern s32 D_80092A20;
extern s32 D_800970F8;
extern s32 D_80098814;
extern s8 D_80099D9D;
extern s8 D_80099D9E;
void SetGeomScreen(s32);
void func_80030988(s32, s32, s32, s32);
void func_8008976C(s32, s32);
void func_80089D5C(s32);
void func_8008A5BC(s32);
void func_8008BC04();
extern s32 D_800910F0;
extern s32 D_80092610;
void func_80074678(void *, s16, s16);
void func_80074998(void *);
void func_8008B0D8(void *);
void func_8008B730(void *, u8, u8);
void func_800732CC();
void func_80088AF8();
void func_8008DC28();
extern s32 D_8009292C;
extern s8 D_80099D9A;
extern s8 D_80099DA1;
extern s8 D_80099DA2;
extern s16 D_80099DA4;
extern s8 D_800928FC;
extern s32 D_80092918;
extern s32 D_80092944;
extern s32 D_80092950;
extern s16 D_80097102;
extern s16 D_8009881E;
extern s32 D_80092640;
extern s32 D_800928AC;
void func_8007BB7C();
void func_8007E24C();
void func_8008D580(s32);
void func_8008E620();
extern s8 D_80091150;
extern s8 D_80091151;
extern s32 D_80092644;
extern s32 D_80092890;
extern s32 D_80097014;
extern s32 D_80097080;
extern void *D_8009790C;
extern s32 D_80098730;
extern s32 D_8009879C;
extern void *D_80099028;
extern s32 D_800926DC;
extern s16 D_800926E8;
extern s16 D_800926EC;
extern s32 D_800912DC;
extern s8 D_8009273C;
extern s8 D_80092740;
extern s32 D_80092744;
extern u8 D_80099D9F;
s32 func_8007FF70(s32, s32, s32); /* retail: no andi on entry or return */
extern u8 D_80092884;
extern s32 D_80092924;
void func_800719F0();
void func_8008509C(s32, s32);
extern s32 D_80091660;
extern s32 D_80092734;
void func_80039FF8();
void func_80080964(s32);
s32 func_800891C0(s32);
extern s16 D_80091672;
extern s16 D_800916AE;
extern s8 D_80092758;
extern s32 D_800928C8;
extern s32 D_800928D8;
extern s32 D_80092940;
void ArchiveCdDataSync(s32);
extern s32 D_80092760;
void func_8007F8B4();
void func_80080B58();
void func_8008EB4C(s32);
extern s16 D_800915BE;
extern s16 D_80091636;
extern s16 D_80091762;
extern s8 D_800926FC;
extern s8 D_8009275C;
extern u16 D_8009286C;
extern s8 D_8009A1C4;
extern s8 D_8009A1C5;
extern s8 D_8009A1C6;
extern s16 D_8009A1CA;
extern s16 D_8009A1CE;
extern s8 D_8009A2BC;
extern s8 D_8009A2BD;
extern s8 D_8009A2BE;
extern s16 D_8009A2C2;
extern s16 D_8009A2C6;
extern s32 D_80092790;
void func_80080D20(s32);
void func_80086E24();
void func_8008AC0C(s32);
void func_8008AE1C(s32);
extern u8 D_800928A0;
void func_8008E3CC(s32, s32, s32);
void func_8007F258(s32, s32);
void func_8008E120();
extern s16 D_80092780;
extern s32 D_80092938;
void func_8003A838(s32, s32, s32);
void func_8008E064();
extern s32 D_80092784;
extern s32 D_80092948;
void *func_80089C54();
void func_80089C88(s32, void *);
void func_80089E2C(void *, s32);
s32 func_80089FC4();
void func_8008A184(s32, s32 *);
extern s32 D_80091FB0;
extern s32 D_80059488;
void func_80083BB4(s32);
extern u8 D_80092920;
s32 ArchiveDecodeAlignedSize(u16);
void ChangeGameState(s32);
void DrawSync(s32);
void Vsync(s32);
void func_8003852C(s32);
void func_800399D4(s32);
void func_80039C4C(s32);
void func_80088A40();
extern s8 D_8005061C;
extern s32 D_800917F0;
extern s32 D_800927C4;
void *func_8008820C();
void func_800888B0();
void func_800889C8();
extern s32 D_800927F4;
extern u16 g_C1ButtonState;
void func_800707A8(void);
void func_80070F80(s32 arg0);
void func_800718C0(void);
void func_800720C4(void);
void func_800720D4(void);
void func_800725A8(void);
void func_800726B4(void);
void func_80074AB4(void *arg0);
s32 func_8007570C(void *arg0, void *arg1, s32 arg2);
s32 func_80075738(void *arg0, void *arg1);
void func_80075748(void);
void func_8007639C(void *arg0, s8 arg1);
void func_80076424(void *arg0);
void func_80077A88(void *arg0);
void func_80078E94(void *arg0);
void func_80078ED4(void *arg0);
void func_80079A8C(void);
void func_80079B04(void);
void func_80079B0C(void);
void func_80079D08(void *arg0);
void func_80079DE0(void);
void func_8007A6D0(void *arg0);
void func_8007A730(void *arg0);
void func_8007A884(void);
s32 func_8007CD14(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s32 func_8007E624(void);
void func_8007E894(s16 arg0, s16 arg1);
void func_8007E954(s32 arg0);
void func_8007F834(void);
void func_8007F8E4(void);
void func_80080234(void);
void func_800808F4(void);
void func_80080920(void);
s32 func_800809BC(void);
void func_800809D8(void);
void func_80080A58(void);
void func_80080AA0(s32 arg0);
void func_80080C48(s32 arg0);
void func_80080D10(void);
void func_80081E00(void);
void func_80081E6C(void);
s32 func_80083CD8(void);
s32 func_800849E0(void *arg0);
void func_80084A40(void *arg0);
s32 func_80084A64(void *arg0);
void func_80084AE0(void);
void func_80084B48(void);
void func_80084BEC(void *arg0);
void func_80084FD0(void);
void func_80085014(void);
void func_80085070(void);
void func_8008518C(void *arg0, s32 arg1);
void func_800851D4(void);
void func_80085E34(void *arg0, void *arg1);
void func_80085E60(void *arg0, void *arg1);
void func_80085E90(s32 arg0, s16 *arg1, s32 arg2);
void func_80085EAC(s32 arg0, s16 *arg1, s32 arg2);
void func_800882D4(s32 arg0, s32 arg1, s32 arg2, s16 arg3);
void func_80088BD4(void);
void func_80088C28(void);


void func_800707A8(void) {
    func_80083C0C(3);
    func_800346D4(&D_80092954);
}

s32 func_800707D8(s32 arg0, s32 arg1, s32 arg2) {
    s32 var_v0;

    var_v0 = arg0 - arg1;
    if (var_v0 < 0) {
        var_v0 += 1;
        var_v0 -= arg2;
    } else {
        var_v0 -= 1;
        var_v0 += arg2;
    }
    return var_v0 / arg2;
}

extern s32 D_800925F4;
extern s32 D_8009867C[];
extern s32 D_80098680;
extern s32 D_80098684;
s32 func_800707D8(s32 arg0, s32 arg1, s32 arg2);

void func_80070808(void *arg0, s32 arg1) {
    D_8009867C[0] += func_800707D8(*(s32 *)((s8*)(arg0) + 0), D_8009867C[0], arg1);
    D_80098684 += func_800707D8(*(s32 *)((s8*)(arg0) + 8), D_80098684, arg1);
    D_80098680 += func_800707D8(*(s32 *)((s8*)(arg0) + 4), D_80098680 + D_800925F4, arg1);
}

extern s32 D_8009871C[];
extern s32 D_80098720__s asm("D_80098720");
extern s32 D_80098724;
s32 func_800707D8(s32 arg0, s32 arg1, s32 arg2);
void func_800828F8(void *, void *, s32);

void func_800708C4(void *arg0, s32 arg1) {
    s32 sp10[3];

    sp10[0] = func_800707D8(*(s32 *)((s8*)(arg0) + 0), D_8009871C[0], arg1);
    sp10[1] = func_800707D8(*(s32 *)((s8*)(arg0) + 4), D_80098720__s, arg1);
    sp10[2] = func_800707D8(*(s32 *)((s8*)(arg0) + 8), D_80098724, arg1);
    func_800828F8(D_8009871C, sp10, 0x3D00);
    D_8009871C[0] += sp10[0];
    D_80098720__s += sp10[1];
    D_80098724 += sp10[2];
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007099C);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80070C7C);

void func_80070F80(s32 arg0) {
    D_800925F8 = arg0;
    D_80098774 = 0;
    D_80097058 = 0;
    D_800925FC = 0;
    D_800987FC &= 0xFFFF7FFF;
    D_800970E0 &= 0xFFFF7FFF;
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_scene_walk_out_of_far_quadrant (src/decomp/src/menu/arena_camera_and_scenes.c:280). */
#ifndef XM_T_VECTOR
#define XM_T_VECTOR
typedef struct {
    long vx, vy, vz, pad;
} VECTOR;
#endif
#ifndef XM_T_MATRIX
#define XM_T_MATRIX
typedef struct {
    short m[3][3];
    long t[3];
} MATRIX;
#endif
#ifndef XM_T_SVECTOR
#define XM_T_SVECTOR
typedef struct {
    short vx, vy, vz, pad;
} SVECTOR;
#endif
#ifndef XM_T_Node
#define XM_T_Node
typedef struct Node {
    s32 type;          
    void *data;        
    void (*callback)(struct Node *node); 
    MATRIX view;       
    SVECTOR offset;    
    VECTOR position;   
    SVECTOR angles;    
    MATRIX unk4C;      
    MATRIX unk6C;      
    struct Node *parent; 
    struct Node *next;   
    struct Node *child;  
    s32 unk98;
} Node;
#endif
#ifndef XM_T_MoveSlot
#define XM_T_MoveSlot
typedef struct MoveSlot {
    u8 unk0[0x3];
    u8 usable;
} MoveSlot;
#endif
#ifndef XM_T_Trail
#define XM_T_Trail
typedef struct {
    VECTOR a;          
    VECTOR a_prev;     
    VECTOR b;          
    VECTOR b_prev;     
    s16 effect;        
    u8 unk42;
    u8 unk43;
    u32 unk44_0 : 1;
    u32 flip : 1;
    u32 unk44_2 : 6;
    u32 state : 8;     
    u32 unk46 : 8;
    u32 unk47 : 8;
    u8 frame;          
    u8 unk49[0x3];
    s32 style;         
    u8 *unk50;
} Trail;
#endif
#ifndef XM_T_Shot
#define XM_T_Shot
typedef struct {
    VECTOR pos;      
    VECTOR prev;     
    SVECTOR velocity; 
    SVECTOR dir;     
    u8 active;       
    u8 speed;
    u8 look;         
    u8 steer;        
    s32 dist;        
    s32 unk38;
    s16 unk3C;
    s16 homing;      
    s16 life;        
    u8 unk42[0x2];
} Shot;
#endif
#ifndef XM_T_SceneHeader
#define XM_T_SceneHeader
typedef struct SceneHeader {
    u8 core_part;    
    u8 foot_a_part;  
    u8 foot_b_part;
    u8 unk3;
    s16 foot_a_vertex;
    s16 foot_b_vertex;
    s16 core_vertex;
    u8 unkA[0x2];
    u16 unkC;
    u8 unkE;
    u8 unkF;
    u8 unk10[3];
    u8 unk13;
    s16 unk14;
    s16 unk16;
    s16 unk18;
    s16 unk1A;
    s16 unk1C;
    u16 unk1E;
    u16 unk20;       
    u8 unk22[0x2];
    s16 unk24;       
    s16 unk26;
    s16 unk28;
    s16 unk2A;
    s16 unk2C;       
    u8 unk2E[0x2];
    s32 unk30;       
} SceneHeader;
#endif
#ifndef XM_T_Pose
#define XM_T_Pose
typedef struct {
    s16 x, y, z;
    u16 flags;       
    s16 unk8;
    u16 unkA;        
} Pose;
#endif
#ifndef XM_T_Move
#define XM_T_Move
typedef struct {
    u8 unk0[0x6];
    u16 flags;       
    u8 anim;
    u8 unk9;
    u8 unkA;
} Move;
#endif
#ifndef XM_T_CVECTOR
#define XM_T_CVECTOR
typedef struct {
    u_char r, g, b, cd;
} CVECTOR;
#endif
#ifndef XM_T_Brain
#define XM_T_Brain
typedef struct Brain {
    struct Actor *owner;
    s16 timer;          
    u8 unk6;
    u8 unk7;
    u8 mode;            
    u8 unk9;
    s16 unkA;
    s16 unkC;
    u8 unkE;
    u8 unkF;
    s32 unk10;          
    s32 unk14;
    s32 unk18;          
    s32 unk1C;          
    s32 unk20;
    s32 unk24;          
    s32 unk28;
    u32 roll : 8;       
    u32 unk2C_8 : 1;
    u32 unk2C_9 : 1;
    u32 unk2C_10 : 1;
    u32 unk2C_11 : 1;
    u32 unk2C_12 : 1;
    u32 defending : 1;  
    u32 unk2C_14 : 2;
    u32 unk2E : 8;      
    u32 unk2C_24 : 8;
    s16 unk30;
} Brain;
#endif
#ifndef XM_T_MoveList
#define XM_T_MoveList
typedef struct MoveList {
    s16 base;       
    u8 unk2[0x2];
    s16 level;      
    u8 tendency[4]; 
    u8 learned[14]; 
    u8 unk18;       
    u8 unk19[0x7];
} MoveList;
#endif
#ifndef XM_T_POLY_FT4
#define XM_T_POLY_FT4
typedef struct {
    u_long tag;
    u_char r0, g0, b0, code;
    short x0, y0;
    u_char u0, v0;
    u_short clut;
    short x1, y1;
    u_char u1, v1;
    u_short tpage;
    short x2, y2;
    u_char u2, v2;
    u_short pad1;
    short x3, y3;
    u_char u3, v3;
    u_short pad2;
} POLY_FT4;
#endif
#ifndef XM_T_Actor
#define XM_T_Actor
typedef struct Actor {
    VECTOR pos;          
    VECTOR velocity;     
    VECTOR push;         
    VECTOR unk30;        
    s32 unk40;           
    s32 unk44;           
    s32 state;           
    u8 anim;             
    u8 unk4D;
    u8 unk4E;
    u8 unk4F;
    s16 event_frame;     
    u8 unk52;
    u8 unk53;
    s32 angle;           
    s32 target_angle;    
    Node *node;          
    void *object;        
    u8 unk64[0x8];
    VECTOR start;        
    s32 unk7C;
    struct MoveSlot *move_slots; 
    u8 *unk84;
    s32 unk88;
    s32 unk8C;
    s32 unk90;
    s32 unk94;
    s32 accel;           
    s32 brake;           
    u8 unkA0[0x8];
    s32 unkA8;
    u8 unkAC[0x4];
    s32 floor_y;         
    s16 hp;              
    s16 charge;          
    s16 unkB8;
    s16 unkBA;
    s16 max_hp;          
    s16 unkBE;           
    u8 unkC0;
    u8 unkC1;
    u8 level;            
    u8 unkC3;
    u8 unkC4;
    u8 unkC5;
    u8 unkC6;            
    u8 unkC7;
    s16 unkC8;
    s16 unkCA;
    s16 unkCC;
    s16 unkCE;
    u32 flags;           
    u32 unkD4;
    struct Actor *opponent; 
    u8 unkDC[0xC];
    s32 unkE8;
    u8 unkEC[0x4];
    s16 unkF0;           
    s16 unkF2;
    s32 unkF4;
    u8 unkF8[0x4];
    s32 unkFC;           
    s32 unk100;
    Trail trails[16];    
    s32 unk644;
    s32 unk648;          
    Shot shots[9];       
    u8 unk8B0[0x44];
    s32 nearest_dist;    
    Shot *nearest_shot;  
    SceneHeader *header; 
    u8 *unk900;
    u8 *visible;         
    u8 visible_count;
    u8 model_id;         
    u8 kind;             
    u8 unk90B;
    s16 unk90C;          
    u8 hold_anim;        
    u8 unk90F;
    u8 unk910;
    u8 move_count;       
    u8 parts_b;          
    u8 unk913;
    s16 unk914;
    s16 unk916;
    u8 glow;             
    u8 unk919[0x3];
    VECTOR hit_point;    
    VECTOR unk92C;       
    VECTOR home;         
    VECTOR core;         
    VECTOR start_home;   
    s16 foot_b_y;        
    s16 foot_a_y;
    s32 unk970;
    VECTOR hit_from;     
    u8 unk984[0x14];
    s16 unk998;
    s16 unk99A;
    s16 anim_speed;      
    s16 unk99E;
    u8 inputs[32];       
    u8 input_head;
    u8 input_tail;
    u8 input_count;
    u8 unk9C3;
    u8 unk9C4[0x8];
    u8 unk9CC[0xC00];    
    Pose *pose;          
    Move *move;          
    CVECTOR colour;        
    u8 unk15D8[0x10];
    s16 unk15E8;
    s16 unk15EA;
    s16 unk15EC;
    s16 unk15EE;
    s16 unk15F0;
    s16 unk15F2;
    s16 unk15F4;
    s16 unk15F6;
    s16 unk15F8;
    u8 unk15FA[0x2];
    struct Brain *brain; 
    struct MoveList *moves; 
    POLY_FT4 backdrop[2]; 
    s32 unk1654;
    s32 unk1658;
    s32 unk165C;
    s32 unk1660;
    u8 *sounds;          
    s16 unk1668;
} Actor;
#endif
#ifndef XM_S_arena_scene_walk_out_of_far_quadrant
#define XM_S_arena_scene_walk_out_of_far_quadrant
s32 xm_arena_scene_walk_out_of_far_quadrant(Actor *actor) __asm__("func_80070FD8");
#endif
#ifndef XM_S_ratan2
#define XM_S_ratan2
long xm_ratan2(long y, long x) __asm__("ratan2");
#endif
#ifndef XM_S_arena_actors_heading
#define XM_S_arena_actors_heading
extern s32 xm_arena_actors_heading __asm__("D_80092934");
#endif
s32 func_80070FD8(Actor *actor) {
    VECTOR pos = actor->pos;

    pos.vx -= 0x3F80;
    pos.vz -= 0x3F80;
    if ((xm_ratan2(pos.vx, pos.vz) & 0xFFF) > 0x200) {
        actor->target_angle = 0x800 - xm_arena_actors_heading;
    } else {
        actor->target_angle = 0xC00 - xm_arena_actors_heading;
    }
    actor->state = 0xFF;
    actor->unkCE = 0;
    return 0;
}
#endif
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007107C);
extern s32 D_800925E0;
extern s32 D_800925E4;
extern u8 *D_80092868;
extern u8 D_800929E4[];

void func_80071724(void *arg0) {
    *(s32 *)(D_80092868 + 0x7C) = D_800925E0 | (D_800925E4 << 0x10);
    AddPrim(arg0, D_80092868 + 0x74);
    AddPrim(arg0, D_800929E4 + D_800928A0 * 8);
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_scene_load_sprite_sheet (src/decomp/src/menu/arena_camera_and_scenes.c:595). */
#ifndef XM_T_MenuImageFile
#define XM_T_MenuImageFile
typedef struct MenuImageFile {
    u_long *unk0;
    u_long *sparkle0[12];  
    u_long *sparkle1;      
    u_long *backdrop;      
    u_long *font;          
    u_long *sparkle2;      
    u_long *effect;        
    u_long *icons[4];      
    u_long *name;          
    u_long *bar;           
    u_long *sheet;         
    u_long *banner;        
    u_long *sparkle4;      
    u_long *floor;         
    u_long *extra[9];      
} MenuImageFile;
#endif
#ifndef XM_T_RECT
#define XM_T_RECT
typedef struct {
    short x, y;
    short w, h;
} RECT;
#endif
#ifndef XM_T_TIM_IMAGE
#define XM_T_TIM_IMAGE
typedef struct {
    u_long mode;
    RECT *crect;
    u_long *caddr;
    RECT *prect;
    u_long *paddr;
} TIM_IMAGE;
#endif
#ifndef XM_T_DR_TPAGE
#define XM_T_DR_TPAGE
typedef struct {
    u_long tag;
    u_long code[1];
} DR_TPAGE;
#endif
#ifndef XM_T_DR_ENV
#define XM_T_DR_ENV
typedef struct {
    u_long tag;
    u_long code[15];
} DR_ENV;
#endif
#ifndef XM_T_DRAWENV
#define XM_T_DRAWENV
typedef struct {
    RECT clip;
    short ofs[2];
    RECT tw;
    u_short tpage;
    u_char dtd;
    u_char dfe;
    u_char isbg;
    u_char r0, g0, b0;
    DR_ENV dr_env;
} DRAWENV;
#endif
#ifndef XM_T_DISPENV
#define XM_T_DISPENV
typedef struct {
    RECT disp;
    RECT screen;
    u_char isinter;
    u_char isrgb24;
    u_char pad0, pad1;
} DISPENV;
#endif
#ifndef XM_T_SPRT_16
#define XM_T_SPRT_16
typedef struct {
    u_long tag;
    u_char r0, g0, b0, code;
    short x0, y0;
    u_char u0, v0;
    u_short clut;
} SPRT_16;
#endif
#ifndef XM_T_DR_AREA
#define XM_T_DR_AREA
typedef struct {
    u_long tag;
    u_long code[2];
} DR_AREA;
#endif
#ifndef XM_T_DR_OFFSET
#define XM_T_DR_OFFSET
typedef struct {
    u_long tag;
    u_long code[2];
} DR_OFFSET;
#endif
#ifndef XM_T_TILE
#define XM_T_TILE
typedef struct {
    u_long tag;
    u_char r0, g0, b0, code;
    short x0, y0;
    short w, h;
} TILE;
#endif
#ifndef XM_T_DisplayBuffer
#define XM_T_DisplayBuffer
typedef struct DisplayBuffer {
    DRAWENV draw;      
    DISPENV disp;      
    u32 ot;            
    SPRT_16 sprite;    
    u8 unk84[0x4C];
    DR_AREA area;      
    DR_OFFSET offset;  
    TILE background;   
} DisplayBuffer;
#endif
#ifndef XM_S_arena_scene_load_sprite_sheet
#define XM_S_arena_scene_load_sprite_sheet
void xm_arena_scene_load_sprite_sheet(MenuImageFile *files) __asm__("func_80071794");
#endif
#ifndef XM_S_OpenTIM
#define XM_S_OpenTIM
int xm_OpenTIM(u_long *addr) __asm__("OpenTIM");
#endif
#ifndef XM_S_ReadTIM
#define XM_S_ReadTIM
TIM_IMAGE *xm_ReadTIM(TIM_IMAGE *timimg) __asm__("ReadTIM");
#endif
#ifndef XM_S_LoadImage
#define XM_S_LoadImage
int xm_LoadImage(RECT *rect, u_long *p) __asm__("LoadImage");
#endif
#ifndef XM_S_SetDrawTPage
#define XM_S_SetDrawTPage
void xm_SetDrawTPage(DR_TPAGE *p, int dfe, int dtd, int tpage) __asm__("SetDrawTPage");
#endif
#ifndef XM_S_arena_scene_marker_tpages
#define XM_S_arena_scene_marker_tpages
extern DR_TPAGE xm_arena_scene_marker_tpages[2] __asm__("D_800929E4");
#endif
#ifndef XM_S_GetTPage
#define XM_S_GetTPage
u_short xm_GetTPage(int tp, int abr, int x, int y) __asm__("GetTPage");
#endif
#ifndef XM_S_arena_display_buffers
#define XM_S_arena_display_buffers
extern DisplayBuffer xm_arena_display_buffers[2] __asm__("D_8009A0D8");
#endif
#ifndef XM_S_GetClut
#define XM_S_GetClut
u_short xm_GetClut(int x, int y) __asm__("GetClut");
#endif
void func_80071794(MenuImageFile *files) {
    TIM_IMAGE image;
    RECT unused; 
    s16 *clut;

    xm_OpenTIM(files->sheet);
    xm_ReadTIM(&image);
    clut = (s16 *)image.caddr;
    clut[2] = -0x8000;
    clut[0] = 0;
    clut[3] = -1;
    xm_LoadImage(image.crect, image.caddr);
    xm_LoadImage(image.prect, image.paddr);
    xm_SetDrawTPage(&xm_arena_scene_marker_tpages[0], 0, 0, xm_GetTPage(0, 1, image.prect->x, image.prect->y));
    xm_arena_scene_marker_tpages[1] = xm_arena_scene_marker_tpages[0];
    xm_arena_display_buffers[0].sprite.u0 = (image.prect->x & 0x3F) * 4;
    xm_arena_display_buffers[0].sprite.v0 = image.prect->y;
    xm_arena_display_buffers[0].sprite.clut = xm_GetClut(image.crect->x, image.crect->y);
    xm_arena_display_buffers[1].sprite = xm_arena_display_buffers[0].sprite;
}
#endif

void func_800718C0(void) {
    D_800925D8 = -1;
    D_800925D4 = 0;
    func_80032F54(&D_8009868C, 0x140, 0x30, 0x1C, 0x9A, 0x40, 4);
}

extern s32 D_8009105C[];
extern s32 D_800925E0;
extern s32 D_800925E4;
extern s32 D_800925E8;
extern s32 D_800925EC;
extern s16 D_80092600;
extern s8 D_80092608;
extern s16 D_80098692;
extern s16 D_80098698;
extern s16 D_800970C4;
extern s16 D_800970CC;
extern s16 D_800987E0;
extern s16 D_800987E8;

void func_8007191C(s32 arg0) {
    D_80092608 = arg0 == 0;
    if (arg0 == 0) {
        func_8008EB4C(0x37);
        D_80098698 = 2;
        D_80098692 = 0xB4;
    } else {
        D_80098698 = 4;
        D_80098692 = 0x9A;
    }
    func_80070F80(D_8009105C[arg0]);
    D_800925E8 = 0xA0;
    D_800925E0 = 0xA0;
    D_800925EC = 0x6D;
    D_800925E4 = 0x6D;
    D_80092600 = 0;
    D_800987E0 = D_800987E8;
    D_800970C4 = D_800970CC;
}


void func_800719F0(void) {
    D_80099D9D__a_s8[0] = 0;
    D_80099D9E__a_s8[0] = 0;
    func_80083C0C(7);
    D_800928C8__a_s32[0] = 5;
    D_80092884__a_s8[0] = 0;
    func_80032F54(D_80092954__a_s32, 0x140, 0x70, 0xA2, 0x2A, 0x1C, 8);
    func_80034714(D_80092954__a_s32, GetStringEntry(D_80092880[0], 0x42));
    D_800929BC[0] = 0x1E;
    D_800925DC[0] = 0;
    func_80070F80(D_80090F38);
    D_80092604[0] = 0;
    D_80092904[0] = 0;
    D_80092900__a_s32[0] = 0;
    func_8007191C(9);
}

void func_800346A4(s32 *);
void func_80034800(s32 *, s32, s32, s32);
void func_80034874(s32 *, s32);
void func_80034888(s32 *, s32, s32);
void func_80036420();
void func_8007107C();
void func_8007099C(s32);
void func_80079DF0(void *, void *);
s32 func_800707D8(s32 arg0, s32 arg1, s32 arg2);
void func_80071724(void *arg0);
extern s32 D_800925E0;
extern s32 D_800925E4;
extern s32 D_800925E8;
extern s32 D_800925EC;
extern u8 D_800925F0;
extern s32 D_800928E8;
extern u16 g_C1ButtonStatePressedOnce;
extern u16 g_C1ButtonStateReleased;
extern u8 D_80092604__u8 asm("D_80092604");
extern s32 D_8009868C__a[] asm("D_8009868C");
extern s32 D_80097010;
extern s32 D_8009872C;

void func_80071AD0(void) {
    s32 *p;

    if ((D_800925F0 != 0) && (D_800928E8 & 4)) {
        func_80071724((void *)D_80092938);
    }
    func_80036420();
    p = D_8009868C__a;
    if ((*(u8*)&D_80092608) != 0) {
        if (g_C1ButtonStatePressedOnce & 0x1000) {
            func_8008EB4C(0x1E);
            D_80092604__u8 -= 1;
        }
        if (g_C1ButtonStatePressedOnce & 0x4000) {
            func_8008EB4C(0x1E);
            D_80092604__u8 += 1;
        }
        if ((s8)D_80092604__u8 >= 8) {
            D_80092604__u8 = 0;
        }
        if ((s8)D_80092604__u8 < 0) {
            D_80092604__u8 = 7;
        }
        func_80034800(D_80092954__a_s32, (D_800928E8 * 7) & 0x3F, 0xC0, 0x10);
        func_80034874(D_80092954__a_s32, (s8)D_80092604__u8);
        if (g_C1ButtonStateReleased & 0x20) {
            func_8008EB4C(0x21);
            func_8007191C((s8)D_80092604__u8 + 1);
            func_800346A4(p);
        }
    }
    if (*(u8 *)D_800925F8 == 0) {
        func_8007191C(0);
    }
    func_8007107C();
    D_800925E0 += func_800707D8(D_800925E8, D_800925E0, 4);
    D_800925E4 += func_800707D8(D_800925EC, D_800925E4, 4);
    if ((*(u8*)&D_80092608) != 0) {
        func_80034888(D_80092954__a_s32, D_80092938, D_800928A0);
    }
    if (D_800925D4 != D_800925D8) {
        *((s8 *)p + 0x68) = 3;
        func_800346A4(p);
        func_80034714(p, GetStringEntry(D_80092880[0], D_800925D4));
        D_800925D8 = D_800925D4;
    }
    func_80079DF0(&D_8009872C, &D_80097010);
    func_8007099C(D_80092904[0]);
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_scene_settle_actor_on_floor (src/decomp/src/menu/arena_camera_and_scenes.c:723). */
#ifndef XM_T_FloorStep
#define XM_T_FloorStep
typedef struct {
    s32 x;
    s32 z;
} FloorStep;
#endif
#ifndef XM_S_arena_scene_settle_actor_on_floor
#define XM_S_arena_scene_settle_actor_on_floor
void xm_arena_scene_settle_actor_on_floor(Actor *actor) __asm__("func_80071DA4");
#endif
#ifndef XM_S_arena_scene_floor_steps
#define XM_S_arena_scene_floor_steps
extern FloorStep xm_arena_scene_floor_steps[8] __asm__("D_80091084");
#endif
#ifndef XM_S_arena_stage_get_ground_height
#define XM_S_arena_stage_get_ground_height
s32 xm_arena_stage_get_ground_height(VECTOR *pos, s32 lift) __asm__("func_80082488");
#endif
#ifndef XM_S_arena_stage_get_ground_square
#define XM_S_arena_stage_get_ground_square
s32 xm_arena_stage_get_ground_square(VECTOR *pos) __asm__("func_800828C4");
#endif
void xm_arena_scene_settle_actor_on_floor(Actor *actor) __asm__("func_80071DA4");
void xm_arena_scene_settle_actor_on_floor(Actor *actor) {
    VECTOR *pos = &actor->pos;
    s32 tries = 0;
    u8 *steps_x = (u8 *)xm_arena_scene_floor_steps;
    s32 best;
    s32 highest;
    s32 dir;
    s32 floor;
    VECTOR probe;

    do {
        u8 *steps_z;

        pos->vy = xm_arena_stage_get_ground_height(pos, 1);
        highest = pos->vy;
        steps_z = (u8 *)xm_arena_scene_floor_steps + sizeof(s32);
        for (dir = 0; dir < 8; dir++) {
            probe = *pos;
            probe.vx += *(s32 *)(steps_x + dir * sizeof(FloorStep)) * 0xC0;
            probe.vz += *(s32 *)(steps_z + dir * sizeof(FloorStep)) * 0xC0;
            floor = xm_arena_stage_get_ground_height(&probe, 1);
            if (floor < highest - 0x40) {
                best = dir;
                highest = floor;
            }
        }
        if (highest >= pos->vy - 0x40) {
            pos->vy = xm_arena_stage_get_ground_height(pos, 1);
            break;
        }
        {
            u8 *steps_z = (u8 *)xm_arena_scene_floor_steps + sizeof(s32);

            pos->vx -= *(s32 *)(steps_x + best * sizeof(FloorStep)) * 0xC0;
            pos->vz -= *(s32 *)(steps_z + best * sizeof(FloorStep)) * 0xC0;
        }
        tries++;
    } while (tries < 20);
    actor->floor_y = xm_arena_stage_get_ground_height(&actor->pos, 1);
    actor->flags = (actor->flags & 0x9FFFFFFF) | ((((u32)xm_arena_stage_get_ground_square(&actor->pos) >> 24) & 3) << 29);
}
#endif
void func_80071DA4(s32 *);
extern s32 D_8009284C;
extern s32 D_80097010;
extern s16 D_800970C4;
extern s16 D_800970CC;
extern s32 D_80098720[];
extern s32 D_8009872C;
extern s16 D_800987E0;
extern s16 D_800987E8;

s32 func_80071F8C(s32 arg0) {
    switch (arg0) {
    case 0:
        func_80039C4C(D_80092948);
        func_80039FF8();
        return;
    case 1:
        func_80071DA4(&D_8009872C);
        if (((D_800987E0 << 8) / D_800987E8) > ((D_800970C4 << 8) / D_800970CC)) {
            D_800925D4 = 0x43;
            return;
        }
        D_800925D4 = 0x44;
        return;
    case 2:
        func_80071DA4(&D_80097010);
        return;
    case 3:
        D_80098720[0] = -D_8009284C;
        if (D_80098720[0] < -0x600) {
            D_80098720[0] = -0x600;
        }
        return;
    }
}


void func_800720C4(void) {
    D_8009293C = 0;
}


void func_800720D4(void) {
    if ((*(u8*)&D_8009293C) == 0) {
        func_80070F80(&D_800910C4);
        (*(u8*)&D_8009293C) = 1;
        D_800928D4 = 0;
        D_80099D9D = 0;
        D_80099D9E = 0;
        D_80092900 = 0;
        D_800925D4 = 0;
        D_800925D8 = 0;
        D_80092A00 = 1;
        D_80092A10 = 1;
        D_80092A20 = 1;
        D_800970F8 = 0;
        D_80098814 = 0;
    }
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80072170);

void func_800725A8(void) {

}

s32 func_8008A2B8(s32);
s32 func_8008A3E0(s32);
s32 func_8008C2C0(s32);
void func_80078F00(void *);
extern void *D_80092614;
extern s8 D_80092618;
extern s32 D_8009261C;
extern s32 D_80092620;
extern s32 D_80092624;
extern s32 D_80092628;
extern s32 D_8009262C;
extern u16 D_80092632;
extern s16 D_800928D0;
extern s32 D_80096FA8;
extern s32 D_80096FAC;
extern s32 D_80096FB0;

typedef struct {
    u8 pad0[0x14];
    s16 unk14;
    s16 unk16;
    s16 unk18;
    s16 unk1A;
    s16 unk1C;
    u16 unk1E;
} func_800725B0_s;

typedef struct {
    u8 pad0[0x5C];
    s32 unk5C;
    u8 pad60[0x89C];
    func_800725B0_s *unk8FC;
} func_800725B0_arg;

void func_800725B0(void *arg0) {
    func_800725B0_s *temp_v0;

    func_80030988(5, 4, 0x40, 0x40);
    D_800910F0 = func_8008A3E0(func_8008A2B8(0x10));
    D_80092610 = func_8008C2C0(((func_800725B0_arg *)arg0)->unk5C);
    func_8008976C(0x280, 0xDA);
    SetGeomScreen(0x400);
    D_800928D0 = 0;
    D_80092614 = arg0;
    func_80078F00(arg0);
    D_80096FB0 = 0;
    D_80096FAC = 0;
    D_80096FA8 = 0;
    temp_v0 = *(func_800725B0_s **)((s8*)(arg0) + 0x8FC);
    D_80092618 = 1;
    D_8009261C = temp_v0->unk14;
    D_80092620 = temp_v0->unk16;
    D_80092624 = temp_v0->unk18;
    D_80092628 = temp_v0->unk1A;
    D_8009262C = temp_v0->unk1C;
    D_80092632 = temp_v0->unk1E;
}


void func_800726B4(void) {
    func_8008BC04();
    func_8007F834();
    func_80030988(1, 1, 0x40, 0x40);
    func_8008A5BC(D_800910F0);
    func_80089D5C(D_80092610);
    func_8008976C(0x140, 0xDA);
    SetGeomScreen(0xC0);
    func_80083C0C(3);
    func_8007E954(0x100);
    func_80080D10();
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_winner_build_shadow_matrix (src/decomp/src/menu/arena_camera_and_scenes.c:950). */
#ifndef XM_S_arena_winner_build_shadow_matrix
#define XM_S_arena_winner_build_shadow_matrix
void xm_arena_winner_build_shadow_matrix(Node *model, MATRIX *matrix, MATRIX *out) __asm__("func_8007273C");
#endif
#ifndef XM_S_arena_identity_matrix
#define XM_S_arena_identity_matrix
extern MATRIX xm_arena_identity_matrix __asm__("D_80091C0C");
#endif
#ifndef XM_S_arena_view_origin
#define XM_S_arena_view_origin
extern VECTOR xm_arena_view_origin __asm__("D_80096FA8");
#endif
#ifndef XM_S_CompMatrix
#define XM_S_CompMatrix
MATRIX *xm_CompMatrix(MATRIX *m0, MATRIX *m1, MATRIX *m2) __asm__("CompMatrix");
#endif
void func_8007273C(Node *model, MATRIX *matrix, MATRIX *out) {
    MATRIX local;

    *out = *matrix;
    local = xm_arena_identity_matrix;
    local.t[0] = model->position.vx - xm_arena_view_origin.vx;
    local.t[1] = -xm_arena_view_origin.vy;
    local.t[2] = model->position.vz - xm_arena_view_origin.vz;
    xm_CompMatrix(matrix, &local, &local);
    out->t[0] = local.t[0];
    out->t[1] = local.t[1];
    out->t[2] = local.t[2];
}
#endif
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80072858);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80072D18);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80073064);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800730AC);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800730F4);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007313C);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800731F8);
/* The retail copy loop uses t0; preserve its register and byte-count contract. */
void func_800732AC(void *dst, void *src, s32 bytes) {
    register s32 word asm("$8");
    s32 end = bytes + (s32)src;
    do {
        word = *(s32 *)src;
        src = (u8 *)src + 4;
        *(s32 *)dst = word;
        dst = (u8 *)dst + 4;
    } while ((s32)src != end);
}

void func_800732CC(void) {
    void *temp_v0;

    temp_v0 = func_8008D3F4(3, 0);
    (*(u8 *)((u8 *)(temp_v0) + 0x74)) = 0x80;
    (*(u8 *)((u8 *)(temp_v0) + 0x75)) = 0x80;
    (*(u8 *)((u8 *)(temp_v0) + 0x76)) = 0xC0;
    func_8008D5C0(temp_v0, 0x60);
    (*(s16 *)((u8 *)(temp_v0) + 0)) = 4;
    (*(s16 *)((u8 *)(temp_v0) + 0x44)) = 0x300;
    (*(s16 *)((u8 *)(temp_v0) + 0x48)) = 8;
    (*(s16 *)((u8 *)(temp_v0) + 0x4A)) = 0x20;
    (*(s16 *)((u8 *)(temp_v0) + 0x68)) = 0;
    (*(s16 *)((u8 *)(temp_v0) + 0x6A)) = 0x20;
    D_80092644 = temp_v0;
}
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007334C);
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_actor_fire_shot (src/decomp/src/menu/arena_fighters_bout_and_effects.c:135). */
#ifndef XM_T_ShotKind
#define XM_T_ShotKind
typedef struct {
    u16 unk0;
    u8 unk2;
    u8 unk3;
    u8 speed;
    u8 unk5;
    u8 sound;
    u8 unk7;
} ShotKind;
#endif
#ifndef XM_S_arena_actor_fire_shot
#define XM_S_arena_actor_fire_shot
void xm_arena_actor_fire_shot(VECTOR *from, VECTOR *origin, Actor *actor, s32 kind, s32 power, s32 style) __asm__("func_80073424");
#endif
#ifndef XM_S_arena_actor_shot_kinds
#define XM_S_arena_actor_shot_kinds
extern ShotKind xm_arena_actor_shot_kinds[] __asm__("D_800910F4");
#endif
#ifndef XM_S_arena_vector_normalize_to_svector
#define XM_S_arena_vector_normalize_to_svector
void xm_arena_vector_normalize_to_svector(VECTOR *vector, void *out) __asm__("func_8008859C");
#endif
#ifndef XM_S_arena_sound_play_command_sound
#define XM_S_arena_sound_play_command_sound
void xm_arena_sound_play_command_sound(Actor *owner, s32 index, VECTOR *pos, s32 mode) __asm__("func_8008EBD0");
#endif
#ifndef XM_S_arena_gte_scale_svector
#define XM_S_arena_gte_scale_svector
void xm_arena_gte_scale_svector(SVECTOR *dir, SVECTOR *out, s32 scale) __asm__("func_80073064");
#endif
void func_80073424(VECTOR *from, VECTOR *origin, Actor *actor, s32 kind, s32 power, s32 style) {
    VECTOR toward;
    VECTOR aim;
    SVECTOR unused; 
    Actor *opponent;
    ShotKind *info;
    Shot *shot;
    s32 i;

    for (i = 0; i < 9; i++) {
        if (actor->shots[i].active == 0) {
            shot = &actor->shots[i];
            break;
        }
    }
    if (i == 8) {
        return;
    }
    opponent = actor->opponent;
    aim.vx = opponent->pos.vx - from->vx;
    aim.vy = opponent->pos.vy - from->vy - 0x90;
    aim.vz = opponent->pos.vz - from->vz;
    info = &xm_arena_actor_shot_kinds[kind];
    if (origin != NULL) {
        toward.vx = from->vx - origin->vx;
        toward.vy = from->vy - origin->vy;
        toward.vz = from->vz - origin->vz;
    } else {
        toward = aim;
    }
    xm_arena_vector_normalize_to_svector(&toward, &shot->dir);
    shot->homing = info->unk0;
    shot->speed = info->speed;
    shot->life = info->unk3;
    shot->look = info->unk2;
    shot->steer = info->unk5;
    shot->unk38 = style;
    shot->unk3C = power;
    if (info->sound != 0) {
        xm_arena_sound_play_command_sound(actor, info->sound, &shot->pos, 2);
    }
    xm_arena_gte_scale_svector(&shot->dir, &shot->velocity, shot->speed);
    shot->pos = *from;
    shot->prev = shot->pos;
    shot->active = 1;
}
#endif
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80073644);
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_actor_get_part_point (src/decomp/src/menu/arena_fighters_bout_and_effects.c:300). */
#ifndef XM_T_AnimRecord
#define XM_T_AnimRecord
typedef struct {
    u8 kind;           
    u8 node;           
    u16 value;         
} AnimRecord;
#endif
#ifndef XM_T_AnimHeader
#define XM_T_AnimHeader
typedef struct {
    s16 frames;
    s16 unk2;
    s16 keys;          
    s16 channels;      
    AnimRecord records[1]; 
} AnimHeader;
#endif
#ifndef XM_T_Key
#define XM_T_Key
typedef struct {
    s16 *target;
    s16 value;         
    s16 angular;       
} Key;
#endif
#ifndef XM_T_Channel
#define XM_T_Channel
typedef struct {
    u8 *start;
    u8 *current;       
    s16 *target;       
    u16 hold;          
    s16 value;         
    s16 delta;         
    s16 angular;       
} Channel;
#endif
#ifndef XM_T_Player
#define XM_T_Player
typedef struct {
    AnimHeader *header;
    Key *keys;         
    Channel *channels; 
    s32 unkC;
    s16 unk10;
    s16 frame;         
} Player;
#endif
#ifndef XM_T_HierarchyRecord
#define XM_T_HierarchyRecord
typedef struct {
    s16 parent;        
    s16 model;         
    struct { s16 vx, vy, vz; } angle; 
    s16 offset[3];     
} HierarchyRecord;
#endif
#ifndef XM_T_ModelSet
#define XM_T_ModelSet
typedef struct {
    s16 nodeCount;
    s16 count;         
    struct Node **nodes; 
    Player *players;   
    s32 unkC;
    s16 scale[3];      
    s16 unk16;
    HierarchyRecord *records; 
} ModelSet;
#endif
#ifndef XM_T_MorphDelta
#define XM_T_MorphDelta
typedef struct {
    s16 dx, dy, dz;
    s16 index;
} MorphDelta;
#endif
#ifndef XM_T_MorphTarget
#define XM_T_MorphTarget
typedef struct {
    s32 count;
    void *deltas;
    MorphDelta *normals;
} MorphTarget;
#endif
#ifndef XM_T_MorphTable
#define XM_T_MorphTable
typedef struct {
    s32 count;
    MorphTarget targets[1];
} MorphTable;
#endif
#ifndef XM_T_SpriteModel
#define XM_T_SpriteModel
typedef struct {
    u16 flags;          
    u16 vertex_count;
    u16 primitive_count;
    u16 group_count;    
    SVECTOR *vertices;
    SVECTOR *normals;
    u8 *unk10;          
    u8 *unk14;
    u8 *unk18;
    MorphTable *morphs; 
    SVECTOR box_min;    
    SVECTOR box_max;
    s32 aux_size;       
    s32 packet_size;    
} SpriteModel;
#endif
#ifndef XM_T_MorphChannel
#define XM_T_MorphChannel
typedef struct MorphChannel {
    s32 (*update)(struct MorphChannel *channel);
    s32 target;
    s32 weight;
    s32 step;
    u8 unk10[0x10];
} MorphChannel;
#endif
#ifndef XM_T_MorphState
#define XM_T_MorphState
typedef struct {
    SpriteModel *model;
    SVECTOR *vertices;
    SVECTOR *normals;
    s32 count;
    MorphChannel *channels;
} MorphState;
#endif
#ifndef XM_T_NodeModel
#define XM_T_NodeModel
typedef struct {
    u32 flags;
    s32 unk4;
    void *packets[2];  
    MorphState *morph; 
    SpriteModel *file; 
    u8 r, g, b;        
    u8 unk1B;
    s32 unk1C;
} NodeModel;
#endif
#ifndef XM_T_gte_rt
#define XM_T_gte_rt
#define gte_rt() __asm__ volatile("nop;nop;.word 0x4A480012")
#endif
#ifndef XM_S_arena_actor_get_part_point
#define XM_S_arena_actor_get_part_point
void xm_arena_actor_get_part_point(Actor *actor, s32 part, s32 vertex, VECTOR *out) __asm__("func_80073B7C");
#endif
#pragma push_macro("gte_SetRotMatrix")
#undef gte_SetRotMatrix
#define gte_SetRotMatrix(r0)                                                   \
    __asm__ volatile("lw $12, 0(%0);"                                          \
                     "lw $13, 4(%0);"                                          \
                     "ctc2 $12, $0;"                                           \
                     "ctc2 $13, $1;"                                           \
                     "lw $12, 8(%0);"                                          \
                     "lw $13, 12(%0);"                                         \
                     "lw $14, 16(%0);"                                         \
                     "ctc2 $12, $2;"                                           \
                     "ctc2 $13, $3;"                                           \
                     "ctc2 $14, $4"                                            \
                     :                                                         \
                     : "r"(r0)                                                 \
                     : "$12", "$13", "$14", "memory")
#pragma push_macro("gte_SetTransMatrix")
#undef gte_SetTransMatrix
#define gte_SetTransMatrix(r0)                                                 \
    __asm__ volatile("lw $12, 20(%0);"                                         \
                     "lw $13, 24(%0);"                                         \
                     "ctc2 $12, $5;"                                           \
                     "lw $14, 28(%0);"                                         \
                     "ctc2 $13, $6;"                                           \
                     "ctc2 $14, $7"                                            \
                     :                                                         \
                     : "r"(r0)                                                 \
                     : "$12", "$13", "$14", "memory")
#pragma push_macro("gte_ldv0")
#undef gte_ldv0
#define gte_ldv0(r0)                                                           \
    __asm__ volatile("lwc2 $0, 0(%0);"                                         \
                     "lwc2 $1, 4(%0)"                                          \
                     :                                                         \
                     : "r"(r0)                                                 \
                     : "memory")
#pragma push_macro("gte_rt")
#undef gte_rt
#define gte_rt() __asm__ volatile("nop;nop;.word 0x4A480012")
#pragma push_macro("gte_stlvnl")
#undef gte_stlvnl
#define gte_stlvnl(r0)                                                         \
    __asm__ volatile("swc2 $25, 0(%0);"                                        \
                     "swc2 $26, 4(%0);"                                        \
                     "swc2 $27, 8(%0)"                                         \
                     :                                                         \
                     : "r"(r0)                                                 \
                     : "memory")
void func_80073B7C(Actor *actor, s32 part, s32 vertex, VECTOR *out) {
    Node *node = ((ModelSet *)actor->node->data)->nodes[part];

    if (vertex != 0 && node->type == 1) {
        gte_SetRotMatrix(&node->unk4C);
        gte_SetTransMatrix(&node->unk4C);
        gte_ldv0(&((NodeModel *)node->data)->file->vertices[vertex - 1]);
        gte_rt();
        gte_stlvnl(out);
        out->vx += actor->pos.vx;
        out->vy += actor->pos.vy;
        out->vz += actor->pos.vz;
    } else {
        out->vx = node->unk4C.t[0] + actor->pos.vx;
        out->vy = node->unk4C.t[1] + actor->pos.vy;
        out->vz = node->unk4C.t[2] + actor->pos.vz;
    }
}
#pragma pop_macro("gte_SetRotMatrix")
#pragma pop_macro("gte_SetTransMatrix")
#pragma pop_macro("gte_ldv0")
#pragma pop_macro("gte_rt")
#pragma pop_macro("gte_stlvnl")
#endif
void func_80073CA4(s32 arg0) {
    s32 var_a1;
    s32 var_a2;
    void *temp_v1;

    var_a2 = 0;
    do {
        var_a1 = var_a2 * 0x54 + 0x104;
        temp_v1 = (void *)(arg0 + var_a1);
        if (*(u8 *)((s8*)(temp_v1) + 0x45) == 2) {
            *(u8 *)((s8*)(temp_v1) + 0x45) = 1U;
        } else {
            *(u8 *)((s8*)(temp_v1) + 0x45) = 0U;
        }
        var_a2 += 1;
    } while (var_a2 < 0x10);
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_actor_record_trail (src/decomp/src/menu/arena_fighters_bout_and_effects.c:335). */
#ifndef XM_T_HitSpec
#define XM_T_HitSpec
typedef struct {
    u8 unk0; 
    u8 type;
    u8 part_a;
    u8 part_b;
    s16 vertex_a;
    s16 vertex_b;
} HitSpec;
#endif
#ifndef XM_S_arena_actor_record_trail
#define XM_S_arena_actor_record_trail
void xm_arena_actor_record_trail(VECTOR *a, VECTOR *b, s32 flip, HitSpec *hit, Trail *trail, s32 part, Actor *owner) __asm__("func_80073CEC");
#endif
#ifndef XM_S_arena_actor_unread_added_trail_count
#define XM_S_arena_actor_unread_added_trail_count
extern s32 xm_arena_actor_unread_added_trail_count __asm__("D_80092650");
#endif
void func_80073CEC(VECTOR *a, VECTOR *b, s32 flip, HitSpec *hit, Trail *trail, s32 part, Actor *owner) {
    trail->a_prev = trail->a;
    trail->b_prev = trail->b;
    trail->a = *a;
    trail->b = *b;
    trail->unk43 = part;
    trail->flip = flip & 1;
    trail->state = 2;
    trail->unk47 = owner->unk644;
    trail->unk50 = owner->unk84;
    trail->unk46 = hit->type;
    xm_arena_actor_unread_added_trail_count++;
}
#endif
s32 func_80073DE4(void *arg0, s32 arg1) {
    s32 temp_a1;
    register s32 var_v0 asm("$2");

    temp_a1 = *(s16 *)((s8*)(arg0) + 0xB6) + arg1;
    var_v0 = 1;
    if (temp_a1 >= 0x1001) {
        var_v0 = ((temp_a1 - 0xFF1) / 20) < *(s16 *)((s8*)(arg0) + 0xB4);
    }
    return var_v0;
}

s32 func_80073E2C(void *arg0, s32 arg1, s32 arg2) {
    s32 q;

    if (D_800928C8 == 4 || D_800928C8 == 6) {
        return 1;
    }
    *(s16 *)((s8*)(arg0) + 0xB6) += arg1;
    if (*(s16 *)((s8*)(arg0) + 0xB6) > 0x1000) {
        q = (*(s16 *)((s8*)(arg0) + 0xB6) - 0x1000) / 20;
        if (q >= *(s16 *)((s8*)(arg0) + 0xB4)) {
            *(s16 *)((s8*)(arg0) + 0xB6) -= arg1;
            *(s16 *)((s8*)(arg0) + 0xBA) = 0;
            if (func_80083CD8() != 4) {
                func_8008EB4C(0x2E);
            }
            return 0;
        }
        *(s16 *)((s8*)(arg0) + 0xB6) = 0x1000;
        *(s16 *)((s8*)(arg0) + 0xBA) = q;
        switch (arg2) {
        case 1:
            *(s32 *)((s8*)(arg0) + 0x1654) += *(s16 *)((s8*)(arg0) + 0xBA);
            break;
        case 2:
            *(s32 *)((s8*)(arg0) + 0x1658) += *(s16 *)((s8*)(arg0) + 0xBA);
            break;
        }
    }
    return 1;
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_frame_event_effect (src/decomp/src/menu/arena_fighters_bout_and_effects.c:409). */
#ifndef XM_S_arena_frame_event_effect
#define XM_S_arena_frame_event_effect
void xm_arena_frame_event_effect(Actor *actor, HitSpec *hit) __asm__("func_80073F34");
#endif
#ifndef XM_S_arena_effect_pack_trail_key
#define XM_S_arena_effect_pack_trail_key
u32 xm_arena_effect_pack_trail_key(s32 flag, s32 top, s32 middle, s32 low) __asm__("func_8007CD14");
#endif
#ifndef XM_S_arena_effect_is_two_point_type
#define XM_S_arena_effect_is_two_point_type
s32 xm_arena_effect_is_two_point_type(s32 code) __asm__("func_8007D25C");
#endif
#ifndef XM_S_arena_effect_start_line_sparkle
#define XM_S_arena_effect_start_line_sparkle
void xm_arena_effect_start_line_sparkle(s32 column, VECTOR *from, VECTOR *to, s32 key) __asm__("func_8007CD44");
#endif
#ifndef XM_S_arena_effect_queue_bolt_by_type
#define XM_S_arena_effect_queue_bolt_by_type
void xm_arena_effect_queue_bolt_by_type(VECTOR *from, VECTOR *to, s32 code) __asm__("func_8007D65C");
#endif
#ifndef XM_S_arena_effect_set_sparkle2_color
#define XM_S_arena_effect_set_sparkle2_color
void xm_arena_effect_set_sparkle2_color(CVECTOR *color) __asm__("func_8007C100");
#endif
#ifndef XM_S_arena_effect_start_trail_sparkle
#define XM_S_arena_effect_start_trail_sparkle
void xm_arena_effect_start_trail_sparkle(s32 column, VECTOR *pos, s32 key, s32 size) __asm__("func_8007C880");
#endif
#ifndef XM_S_arena_effect_spawn_sparkle
#define XM_S_arena_effect_spawn_sparkle
s32 xm_arena_effect_spawn_sparkle(VECTOR *pos, u32 kind) __asm__("func_8007D190");
#endif
void func_80073F34(Actor *actor, HitSpec *hit) {
    VECTOR a;
    VECTOR b;
    s32 style;

    style = xm_arena_effect_pack_trail_key((actor->flags >> 27) & 1, hit->part_a, hit->vertex_a, 1);
    xm_arena_actor_get_part_point(actor, hit->part_a, hit->vertex_a, &a);
    if (xm_arena_effect_is_two_point_type(hit->type) != 0) {
        xm_arena_actor_get_part_point(actor, hit->part_b, hit->vertex_b, &b);
        if (hit->type == 0x10) {
            xm_arena_effect_start_line_sparkle((actor->flags >> 27) & 1, &a, &b, style);
        } else {
            xm_arena_effect_queue_bolt_by_type(&a, &b, hit->type);
        }
        return;
    }
    if (hit->part_a != hit->part_b || hit->vertex_a != hit->vertex_b) {
        xm_arena_actor_get_part_point(actor, hit->part_b, hit->vertex_b, &b);
        a.vx = (a.vx + b.vx) / 2;
        a.vy = (a.vy + b.vy) / 2;
        a.vz = (a.vz + b.vz) / 2;
    }
    xm_arena_effect_set_sparkle2_color(&actor->colour);
    if (hit->type >= 0x20) {
        xm_arena_effect_start_trail_sparkle((actor->flags >> 27) & 1, &a, style, hit->type - 0x20);
    } else {
        xm_arena_effect_spawn_sparkle(&a, hit->type);
    }
}
#endif
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800740E4);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80074678);
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_actor_apply_part_visibility (src/decomp/src/menu/arena_fighters_bout_and_effects.c:665). */
#ifndef XM_S_arena_actor_apply_part_visibility
#define XM_S_arena_actor_apply_part_visibility
void xm_arena_actor_apply_part_visibility(Actor *actor) __asm__("func_80074998");
#endif
void xm_arena_actor_apply_part_visibility(Actor *actor) __asm__("func_80074998");
void xm_arena_actor_apply_part_visibility(Actor *actor) {
    Node **nodes = ((ModelSet *)actor->node->data)->nodes;
    s32 i;

    for (i = 0; i < ((ModelSet *)actor->node->data)->nodeCount; i++) {
        if (nodes[i]->type == 1) {
            ((NodeModel *)nodes[i]->data)->flags &= ~1;
        }
    }
    for (i = 0; i < actor->visible_count; i++) {
        ((NodeModel *)nodes[actor->visible[i]]->data)->flags |= 1;
    }
    if (actor->model_id == 0xD) {
        ((NodeModel *)nodes[13]->data)->flags |= 1;
    }
}
#endif

void func_80074AB4(void *arg0) {
    u8 temp_a2;
    void *temp_s1;
    void *temp_s2;
    void *temp_v1;

    temp_v1 = *(void **)((s8*)(arg0) + 0x15CC);
    temp_s2 = *(void **)((s8*)(arg0) + 0x15D0);
    *(s32 *)((s8*)(arg0) + 0) = (s32) *(s16 *)((s8*)(temp_v1) + 0);
    *(s32 *)((s8*)(arg0) + 4) = (s32) *(s16 *)((s8*)(temp_v1) + 2);
    *(s32 *)((s8*)(arg0) + 8) = (s32) *(s16 *)((s8*)(temp_v1) + 4);
    *(u8 *)((s8*)(arg0) + 0x4C) = (u8) *(u8 *)((s8*)(temp_s2) + 8);
    *(s32 *)((s8*)(arg0) + 0x54) = (s32) ((s32) (*(u16 *)((s8*)(temp_v1) + 6) << 0x14) >> 0x14);
    temp_s1 = *(s32 *)((s8*)(*(void **)((s8*)(*(void **)((s8*)(arg0) + 0x5C)) + 4)) + 8) + (*(u8 *)((s8*)(temp_s2) + 8) * 0x14);
    if (*(u16 *)((s8*)(temp_s2) + 6) & 0x1000) {
        func_80074998(arg0);
        func_8008B0D8(temp_s1);
    }
    func_80074678(arg0, *(s16 *)((s8*)(temp_s1) + 0x12), *(u8 *)((s8*)(*(void **)((s8*)(arg0) + 0x15D0)) + 9));
    temp_a2 = *(u8 *)((s8*)(temp_s2) + 0xA);
    if (temp_a2 != 0) {
        func_8008B730(temp_s1, *(u8 *)((s8*)(temp_s2) + 9), temp_a2);
    }
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80074BA4);
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_bout_record_outcome (src/decomp/src/menu/arena_fighters_bout_and_effects.c:822). */
#ifndef XM_S_arena_bout_record_outcome
#define XM_S_arena_bout_record_outcome
void xm_arena_bout_record_outcome(s32 lost) __asm__("func_80075060");
#endif
#ifndef XM_S_arena_first_actor
#define XM_S_arena_first_actor
extern Actor xm_arena_first_actor __asm__("D_8009872C");
#endif
#ifndef XM_S_arena_brain_is_hp_above_fraction
#define XM_S_arena_brain_is_hp_above_fraction
s32 xm_arena_brain_is_hp_above_fraction(Actor *actor, s32 fraction) __asm__("func_8008F4F4");
#endif
#ifndef XM_S_mode_arena_bout_outcome
#define XM_S_mode_arena_bout_outcome
extern u8 xm_mode_arena_bout_outcome __asm__("D_80050622");
#endif
void func_80075060(s32 lost) {
    Actor *player = &xm_arena_first_actor;
    s32 limit;

    if (lost) {
        if (xm_arena_brain_is_hp_above_fraction(player, 0xE0)) {
            xm_mode_arena_bout_outcome = 2;
        } else if (xm_arena_brain_is_hp_above_fraction(player, 0x10)) {
            xm_mode_arena_bout_outcome = 1;
        } else {
            xm_mode_arena_bout_outcome = 3;
        }
    } else if (xm_arena_first_actor.unk1660 == 0 && xm_arena_first_actor.unk165C == 0) {
        xm_mode_arena_bout_outcome = 0x88;
    } else if (xm_arena_first_actor.unk165C == 0) {
        xm_mode_arena_bout_outcome = 0x82;
    } else {
        limit = player->max_hp * 0xB0 / 255;
        if (limit < player->unk1654) {
            xm_mode_arena_bout_outcome = 0x83;
        } else if (player->max_hp * 0xA0 / 255 < player->unk1658) {
            xm_mode_arena_bout_outcome = 0x84;
        } else if (limit < player->unk1658 + player->unk1654) {
            xm_mode_arena_bout_outcome = 0x85;
        } else {
            player = player->opponent;
            if (xm_arena_brain_is_hp_above_fraction(player, 0xE0)) {
                xm_mode_arena_bout_outcome = 0x86;
            } else if (xm_arena_brain_is_hp_above_fraction(player, 0x10)) {
                xm_mode_arena_bout_outcome = 0x81;
            } else {
                xm_mode_arena_bout_outcome = 0x87;
            }
        }
    }
}
#endif
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800751C8);

s32 func_8007570C(void *arg0, void *arg1, s32 arg2) {
    s32 var_v0;

    var_v0 = *(s16 *)((s8*)(arg1) + 0x15EC) * arg2;
    if (var_v0 < 0) {
        var_v0 += 0x1F;
    }
    return (var_v0 >> 5) - *(s16 *)((s8*)(arg0) + 0x15EE);
}


s32 func_80075738(void *arg0, void *arg1) {
    return *(s16 *)((s8*)(arg1) + 0x15EA) - *(s16 *)((s8*)(arg0) + 0x15E8);
}


void func_80075748(void) {

}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_actor_is_point_near_segment (src/decomp/src/menu/arena_fighters_bout_and_effects.c:984). */
#ifndef XM_S_arena_actor_is_point_near_segment
#define XM_S_arena_actor_is_point_near_segment
s32 xm_arena_actor_is_point_near_segment(s32 x0, s32 z0, s32 x1, s32 z1, s32 px, s32 pz, s32 radius) __asm__("func_80075750");
#endif
#ifndef XM_S_Square0
#define XM_S_Square0
void xm_Square0(VECTOR *v0, VECTOR *v1) __asm__("Square0");
#endif
#ifndef XM_S_SquareRoot0
#define XM_S_SquareRoot0
long xm_SquareRoot0(long a) __asm__("SquareRoot0");
#endif
s32 func_80075750(s32 x0, s32 z0, s32 x1, s32 z1, s32 px, s32 pz, s32 radius) {
    VECTOR d;
    VECTOR sq;
    s32 rx;
    s32 rz;
    s32 len;
    s32 ux;
    s32 uz;
    s32 along;
    s32 across;
    s32 hit;

    d.vx = x1 - x0;
    d.vy = z1 - z0;
    rx = px - x0;
    rz = pz - z0;
    d.vz = radius;
    xm_Square0(&d, &sq);
    len = xm_SquareRoot0(sq.vx + sq.vy);
    if (len == 0) {
        return 0;
    }
    ux = (d.vx << 12) / len;
    uz = (d.vy << 12) / len;
    along = (rx * ux + rz * uz) / 4096;
    across = (-(rx * uz) + rz * ux) / 4096;
    hit = (u32)(across * across - sq.vz) >> 31;
    if (along < 0) {
        return 0;
    }
    return along <= len + radius ? hit : 0;
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_actor_find_segment_crossing (src/decomp/src/menu/arena_fighters_bout_and_effects.c:1019). */
#ifndef XM_S_arena_actor_find_segment_crossing
#define XM_S_arena_actor_find_segment_crossing
s32 xm_arena_actor_find_segment_crossing(s32 x0, s32 z0, s32 x1, s32 z1, s32 px, s32 pz, s32 radius) __asm__("func_80075888");
#endif
#ifndef XM_S_arena_actor_crossing_point_x
#define XM_S_arena_actor_crossing_point_x
extern s32 xm_arena_actor_crossing_point_x __asm__("D_80092654");
#endif
#ifndef XM_S_arena_actor_crossing_point_z
#define XM_S_arena_actor_crossing_point_z
extern s32 xm_arena_actor_crossing_point_z __asm__("D_80092658");
#endif
s32 func_80075888(s32 x0, s32 z0, s32 x1, s32 z1, s32 px, s32 pz, s32 radius) {
    VECTOR d;
    VECTOR sq;
    s32 rx;
    s32 rz;
    s32 len;
    s32 ux;
    s32 uz;
    s32 along;
    s32 across;
    s32 diff;
    s32 hit;

    d.vx = x1 - x0;
    d.vy = z1 - z0;
    rx = px - x0;
    rz = pz - z0;
    d.vz = radius;
    xm_Square0(&d, &sq);
    len = xm_SquareRoot0(sq.vx + sq.vy);
    if (len == 0) {
        return 0;
    }
    ux = (d.vx << 12) / len;
    uz = (d.vy << 12) / len;
    along = (rx * ux + rz * uz) / 4096;
    across = (-(rx * uz) + rz * ux) / 4096;
    diff = across * across - sq.vz;
    hit = (u32)diff >> 31;
    if (along < 0) {
        hit = 0;
    }
    if (along > len + radius) {
        hit = 0;
    }
    if (hit) {
        if (diff < 0) {
            diff = -diff;
        }
        diff = along - xm_SquareRoot0(diff); 
        xm_arena_actor_crossing_point_x = diff * ux / 4096 + x0;
        xm_arena_actor_crossing_point_z = diff * uz / 4096 + z0;
    }
    return hit;
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_actor_is_point_near_quad (src/decomp/src/menu/arena_fighters_bout_and_effects.c:1067). */
#ifndef XM_S_arena_actor_is_point_near_quad
#define XM_S_arena_actor_is_point_near_quad
s32 xm_arena_actor_is_point_near_quad(VECTOR *quad, s32 px, s32 pz, s32 radius) __asm__("func_80075A4C");
#endif
s32 func_80075A4C(VECTOR *quad, s32 px, s32 pz, s32 radius) {
    xm_arena_actor_crossing_point_x = px;
    xm_arena_actor_crossing_point_z = pz;
    if (xm_arena_actor_is_point_near_segment(quad[1].vx, quad[1].vz, quad[0].vx, quad[0].vz, px, pz, radius)
        || xm_arena_actor_is_point_near_segment(quad[3].vx, quad[3].vz, quad[2].vx, quad[2].vz, px, pz, radius)
        || xm_arena_actor_is_point_near_segment(quad[0].vx, quad[0].vz, quad[2].vx, quad[2].vz, px, pz, radius)) {
        return 1;
    }
    return xm_arena_actor_is_point_near_segment(quad[1].vx, quad[1].vz, quad[3].vx, quad[3].vz, px, pz, radius) != 0;
}
#endif
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80075B50);

void func_8007639C(void *arg0, s8 arg1) {
    u8 temp_v0;

    if ((u8) *(u8 *)((s8*)(arg0) + 0x9C2) < 0x20U) {
        temp_v0 = *(u8 *)((s8*)(arg0) + 0x9C0);
        *(u8 *)((s8*)(arg0) + 0x9C0) = (u8) (temp_v0 + 1);
        *(s8 *)((s8*)((arg0 + (temp_v0 & 0x1F))) + 0x9A0) = arg1;
        *(u8 *)((s8*)(arg0) + 0x9C2) = (u8) (*(u8 *)((s8*)(arg0) + 0x9C2) + 1);
    }
}

u8 func_800763E4(u8 *arg0) {
    u8 idx;

    if (arg0[0x9C2] == 0) {
        return 0;
    }
    idx = arg0[0x9C1]++;
    idx = (arg0 + (idx & 0x1F))[0x9A0];
    arg0[0x9C2]--;
    return idx;
}

void func_80076424(void *arg0) {
    *(s8 *)((s8*)(arg0) + 0x9C2) = 0;
    *(s8 *)((s8*)(arg0) + 0x9C0) = 0;
    *(s8 *)((s8*)(arg0) + 0x9C1) = 0;
    *(s8 *)((s8*)(arg0) + 0x9C3) = 0;
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80076438);
extern s32 D_80091198[];
extern u8 D_80096FB8[];
extern u8 D_80096FBC[];

void func_800764CC(void *arg0) {
    s32 temp_lo;
    u32 temp_v1;
    u32 f;
    s32 var_a0;
    u8 idx;
    s16 *p;

    *(s32 *)((s8*)(arg0) + 0x40) = 0;
    *(s32 *)((s8*)(arg0) + 0x100) = 0;
    if (func_80083CD8() != 7) {
        f = *(u32 *)((s8*)(arg0) + 0xD0) & 0xFFFF7FFF;
        *(u32 *)((s8*)(arg0) + 0xD0) = f & ~2;
    }
    temp_lo = *(s16 *)((s8*)(arg0) + 0x15F4) * D_8009292C;
    temp_v1 = *(u32 *)((s8*)(arg0) + 0xD0) & 0xFFF7FFFF;
    *(u32 *)((s8*)(arg0) + 0xD0) = temp_v1;
    *(s16 *)((s8*)(arg0) + 0x15F2) = temp_lo >> 8;
    var_a0 = 0xF0;
    if (!(temp_v1 & 0x08000000)) {
        var_a0 = 0x28;
    }
    func_8007E894(var_a0, 0x28);
    idx = *(u8 *)((s8*)(arg0) + 0x9C3);
    if (idx != 0) {
        *(s32 *)(D_80096FB8 + ((*(u32 *)((s8*)(arg0) + 0xD0) >> 0x1B) & 1) * 0x14) = D_80091198[idx];
        p = *(s16 **)((s8*)(arg0) + 0x1600);
        D_80096FBC[((*(u32 *)((s8*)(arg0) + 0xD0) >> 0x1B) & 1) * 0x14] = (*((u8 *)p + *(u8 *)((s8*)(arg0) + 0x9C3) + 9) * *p) / 100;
    }
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007661C);
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_actor_queue_back_dash (src/decomp/src/menu/arena_fighters_bout_and_effects.c:1365). */
#ifndef XM_S_arena_actor_queue_back_dash
#define XM_S_arena_actor_queue_back_dash
s32 xm_arena_actor_queue_back_dash(Actor *actor) __asm__("func_800767C8");
#endif
#ifndef XM_S_arena_actors_flat_distance
#define XM_S_arena_actors_flat_distance
extern s32 xm_arena_actors_flat_distance __asm__("D_8009284C");
#endif
#ifndef XM_S_arena_actor_clear_inputs
#define XM_S_arena_actor_clear_inputs
void xm_arena_actor_clear_inputs(Actor *actor) __asm__("func_80076424");
#endif
#ifndef XM_S_arena_actor_queue_input
#define XM_S_arena_actor_queue_input
void xm_arena_actor_queue_input(Actor *actor, u8 input) __asm__("func_8007639C");
#endif
s32 func_800767C8(Actor *actor) {
    if (xm_arena_actors_flat_distance > 0x200 || actor->unkC5 == 4) {
        return;
    }
    if (actor->flags & 0x1000000) {
        return;
    }
    if ((actor->flags & 0x40000) && (actor->opponent->flags & 0x40000)
        && ((actor->flags & 0x60000000) != 0x20000000 || (actor->kind & 2))) {
        xm_arena_actor_clear_inputs(actor);
        xm_arena_actor_queue_input(actor, 5);
    }
}
#endif
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80076884);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80077038);
s32 rcos(s32);
s32 rsin(s32);

s32 func_80077584(arg0, arg1, arg2, arg3)
    void *arg0;
    s32 arg1;
    s32 arg2;
    s32 arg3;
{
    *(s32 *)((s8*)(arg0) + 0x20) += rcos(arg1) >> arg2;
    *(s32 *)((s8*)(arg0) + 0x28) += rsin(arg1) >> arg2;
    if (*(s32 *)((s8*)(arg0) + 0x14) >= -0x8B) {
        *(s32 *)((s8*)(arg0) + 0x14) -= arg3;
    }
    if (*(s32 *)((s8*)(arg0) + 0x14) < -0x82) {
        *(s32 *)((s8*)(arg0) + 0x14) = -0x82;
    }
}

void func_8007762C(void *arg0) {
    *(u8 *)((u8 *)arg0 + 0x4C) = 8;
    *(u8 *)((u8 *)arg0 + 0x4E) = 0xFF;
    *(s16 *)((u8 *)arg0 + 0xCA) = 0x3C;
    *(s16 *)((u8 *)arg0 + 0x916) = 0x28;
    *(s32 *)((u8 *)arg0 + 0xE8) = 0x28;
    *(u8 *)((u8 *)arg0 + 0xC4) = 4;
    *(u8 *)((u8 *)arg0 + 0xC5) = 0;
    *(s32 *)((u8 *)arg0 + 0xD0) |= 0x1000;
    *(s32 *)((u8 *)arg0 + 0xD4) &= ~0x10;
    *(s32 *)((u8 *)arg0 + 0xD0) &= ~0x400;
    func_80077584();
}
void func_80036258(s32, s32);

s32 func_800776A8(void *arg0, s32 arg1) {
    s32 flags;

    if (func_80083CD8() != 4) {
        flags = *(s32 *)((s8*)(arg0) + 0xD0);
        if (flags & 0x08000000) {
            if ((D_800928C8 == 2 || D_800928C8 == 4) && (D_80099D9C[0] & 1) && !(flags & 0x40)) {
                func_80036258(1, arg1);
            }
        } else {
            if ((D_80099D9B[0] & 1) && !(flags & 0x40)) {
                func_80036258(0, arg1);
            }
        }
    }
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80077770);
extern u8 D_80091178[];
extern u8 D_80091179[];

s32 func_80077A38(void *arg0, s32 arg1) {
    s32 temp_v0;

    temp_v0 = *(u8 *)((s8*)(arg0) + 0x9C3) * 2;
    if (arg1 != 0) {
        *(u8 *)((s8*)(arg0) + 0x9C3) = D_80091178[temp_v0];
    } else {
        *(u8 *)((s8*)(arg0) + 0x9C3) = D_80091179[temp_v0];
    }
    return *(s32 *)((s8*)(arg0) + 0x80) + (*(u8 *)((s8*)(arg0) + 0x9C3) * 4);
}


void func_80077A88(void *arg0) {
    *(s32 *)((s8*)(arg0) + 0xD0) = (s32) (*(s32 *)((s8*)(arg0) + 0xD0) | 0x80000);
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80077A9C);
typedef struct {
    s32 unk[4];
} func_80078154_copy16;

void func_80078154(void *arg0) {
    *(func_80078154_copy16 *)arg0 = *(func_80078154_copy16 *)((s8*)(arg0) + 0x93C);
    *(s8 *)((s8*)(arg0) + 0x4C) = 0;
    *(s8 *)((s8*)(arg0) + 0xC4) = 0;
    *(s8 *)((s8*)(arg0) + 0xC5) = 0;
    *(s32 *)((s8*)(arg0) + 0xD0) = (s32) (*(s32 *)((s8*)(arg0) + 0xD0) | 0x02000000);
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80078194);
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_actor_pick_anchor_across_line (src/decomp/src/menu/arena_fighters_bout_and_effects.c:2151). */
#ifndef XM_S_arena_actor_pick_anchor_across_line
#define XM_S_arena_actor_pick_anchor_across_line
s32 xm_arena_actor_pick_anchor_across_line(VECTOR *a, VECTOR *b, Actor *actor, VECTOR **anchor) __asm__("func_80078704");
#endif
#ifndef XM_S_arena_vector_get_flat_distance
#define XM_S_arena_vector_get_flat_distance
s32 xm_arena_vector_get_flat_distance(VECTOR *from, VECTOR *to) __asm__("func_80088838");
#endif
s32 func_80078704(VECTOR *a, VECTOR *b, Actor *actor, VECTOR **anchor) {
    s32 dz = a->vz - b->vz;
    s32 dx = b->vx - a->vx;
    s32 start = dz * actor->start.vx + dx * actor->start.vz + a->vx * b->vz - b->vx * a->vz;
    s32 first = dz * actor->unk92C.vx + dx * actor->unk92C.vz + a->vx * b->vz - b->vx * a->vz;
    s32 second = dz * actor->home.vx + dx * actor->home.vz + a->vx * b->vz - b->vx * a->vz;

    if (start < 0) {
        start = -1;
    } else if (start > 0) {
        start = 1;
    }
    if (first < 0) {
        first = -1;
    } else if (first > 0) {
        first = 1;
    }
    if (second < 0) {
        second = -1;
    } else if (second > 0) {
        second = 1;
    }
    first *= start;
    second *= start;
    if (first < 0) {
        if (second < 0) {
            VECTOR *p1 = &actor->unk92C;
            s32 d1 = xm_arena_vector_get_flat_distance(p1, a);
            VECTOR *p2 = &actor->home;
            s32 d2 = xm_arena_vector_get_flat_distance(p2, a);
            *anchor = d2 < d1 ? p1 : p2;
            return 0;
        }
        *anchor = &actor->unk92C;
        return 1;
    }
    if (second < 0) {
        *anchor = &actor->home;
        return 2;
    }
    {
        VECTOR *p1 = &actor->unk92C;
        s32 d1 = xm_arena_vector_get_flat_distance(p1, a);
        VECTOR *p2 = &actor->home;
        s32 d2 = xm_arena_vector_get_flat_distance(p2, a);
        *anchor = d1 < d2 ? p1 : p2;
    }
    return 3;
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_actors_push_apart (src/decomp/src/menu/arena_fighters_bout_and_effects.c:2207). */
#ifndef XM_S_arena_actors_push_apart
#define XM_S_arena_actors_push_apart
s32 xm_arena_actors_push_apart(Actor *first, Actor *second) __asm__("func_80078920");
#endif
#ifndef XM_S_arena_vector_get_flat_length
#define XM_S_arena_vector_get_flat_length
s32 xm_arena_vector_get_flat_length(VECTOR *vector) __asm__("func_80088754");
#endif
#ifndef XM_S_gpu_get_sin
#define XM_S_gpu_get_sin
s32 xm_gpu_get_sin(s32 angle) __asm__("rcos");
#endif
#ifndef XM_S_gpu_get_cos
#define XM_S_gpu_get_cos
s32 xm_gpu_get_cos(s32 angle) __asm__("rsin");
#endif
s32 func_80078920(Actor *first, Actor *second) {
    VECTOR unused; 
    VECTOR point_a;
    VECTOR point_b;
    VECTOR mid;
    VECTOR across;
    VECTOR *anchor_a;
    VECTOR *anchor_b;
    s32 radius_a;
    s32 radius_b;
    s32 dist;
    s32 sum;
    s32 angle; 
    s32 speed_a;
    s32 speed_b;
    s32 total;

    if (xm_arena_actors_flat_distance <= 0x300 && ((first->home.vy < second->unk92C.vy && second->home.vy < first->home.vy) ||
        (second->unk92C.vy < first->unk92C.vy && first->unk92C.vy < second->home.vy) ||
        (second->home.vy < first->unk92C.vy && first->home.vy < second->home.vy) ||
        (first->unk92C.vy < second->unk92C.vy && second->unk92C.vy < first->home.vy))) {
        mid.vx = (first->start.vx + second->start.vx) / 2;
        mid.vz = (first->start.vz + second->start.vz) / 2;
        across.vz = second->start.vx - first->start.vx + mid.vz;
        across.vx = first->start.vz - second->start.vz + mid.vx;
        radius_a = first->header->unk3;
        radius_b = second->header->unk3;
        xm_arena_actor_pick_anchor_across_line(&mid, &across, first, &anchor_a);
        xm_arena_actor_pick_anchor_across_line(&mid, &across, second, &anchor_b);
        point_a = *anchor_a;
        point_b = *anchor_b;
        point_a.vx -= first->start.vx;
        point_a.vy -= first->start.vy;
        point_a.vz -= first->start.vz;
        point_b.vx -= second->start.vx;
        point_b.vy -= second->start.vy;
        point_b.vz -= second->start.vz;
        point_a.vx += first->pos.vx;
        point_a.vy += first->pos.vy;
        point_a.vz += first->pos.vz;
        point_b.vx += second->pos.vx;
        point_b.vy += second->pos.vy;
        point_b.vz += second->pos.vz;
        dist = xm_arena_vector_get_flat_distance(&point_a, &point_b);
        total = radius_a + radius_b;
        if (dist < total) {
            angle = xm_arena_vector_get_flat_distance(&first->start, &point_a);
            if (xm_arena_vector_get_flat_distance(&first->start, &point_b) < angle) {
                dist += total;
            } else {
                dist = total - dist;
            }
            angle = xm_ratan2(first->start.vx - second->start.vx, first->start.vz - second->start.vz);
            speed_a = xm_arena_vector_get_flat_length(&first->velocity);
            speed_b = xm_arena_vector_get_flat_length(&second->velocity);
            sum = speed_a + speed_b;
            if (sum == 0) {
                speed_b = 1;
                speed_a = 1;
                sum = 2;
            }
            speed_a = (speed_a * dist << 8) / sum;
            speed_b = (speed_b * dist << 8) / sum;
            first->pos.vx += (xm_gpu_get_sin(angle) * speed_b) >> 20;
            first->pos.vz += (xm_gpu_get_cos(angle) * speed_b) >> 20;
            second->pos.vx -= (xm_gpu_get_sin(angle) * speed_a) >> 20;
            second->pos.vz -= (xm_gpu_get_cos(angle) * speed_a) >> 20;
        }
    }
}
#endif
void func_80078D20(void *arg0) {
    s32 temp_a0;
    s32 temp_a1;
    s32 temp_v1;
    u8 unused[16];

    (*(s32 *)((u8 *)(arg0) + 0xB0)) = func_80082488(arg0, 1);
    temp_v1 = ((*(s32 *)((u8 *)(arg0) + 0xD0)) & 0x9FFFFFFF) | ((((u32)func_800828C4(arg0) >> 0x18) & 3) << 0x1D);
    (*(s32 *)((u8 *)(arg0) + 0xD0)) = temp_v1;
    if ((temp_v1 & 0x60000000) == 0x60000000) {
        (*(s8 *)((u8 *)(arg0) + 0x90B)) = 0xF;
    }
    func_800828F8(arg0, arg0 + 0x10, 0x3E80);
    temp_a1 = (*(s32 *)((u8 *)(arg0) + 0xD0)) & 0xFFFBFFFF;
    (*(s32 *)((u8 *)(arg0) + 0xD0)) = temp_a1;
    if ((*(s32 *)((u8 *)(arg0) + 0xB0)) < ((*(s32 *)((u8 *)(arg0) + 4)) + (*(s32 *)((u8 *)(arg0) + 0x14)))) {
        (*(s32 *)((u8 *)(arg0) + 0xD0)) = (s32) (temp_a1 | 0x40000);
        if ((*(u8 *)((u8 *)(arg0) + 0xC4)) == 4) {
            if (!((*(s32 *)((u8 *)(arg0) + 0xD4)) & 0x10)) {
                func_8008EBD0(arg0, 0xD, arg0, 2);
                func_800776A8(arg0, 6);
            }
            temp_a0 = (*(s32 *)((u8 *)(arg0) + 0x14));
            (*(s32 *)((u8 *)(arg0) + 0xD4)) = (s32) ((*(s32 *)((u8 *)(arg0) + 0xD4)) | 0x10);
            if (temp_a0 >= 0x40) {
                (*(s32 *)((u8 *)(arg0) + 0x14)) = (s32) (-temp_a0 / 3);
            } else {
                goto block_8;
            }
        } else {
block_8:
            (*(s32 *)((u8 *)(arg0) + 0x14)) = 0;
        }
        (*(s32 *)((u8 *)(arg0) + 4)) = (s32) (*(s32 *)((u8 *)(arg0) + 0xB0));
    }
    (*(s32 *)((u8 *)(arg0) + 0)) = (s32) ((*(s32 *)((u8 *)(arg0) + 0)) + (*(s32 *)((u8 *)(arg0) + 0x10)));
    (*(s32 *)((u8 *)(arg0) + 4)) = (s32) ((*(s32 *)((u8 *)(arg0) + 4)) + (*(s32 *)((u8 *)(arg0) + 0x14)));
    (*(s32 *)((u8 *)(arg0) + 8)) = (s32) ((*(s32 *)((u8 *)(arg0) + 8)) + (*(s32 *)((u8 *)(arg0) + 0x18)));
}

void func_80078E94(void *arg0) {
    *(s32 *)((s8*)(*(void **)((s8*)(arg0) + 0x5C)) + 0x34) = (s32) *(s32 *)((s8*)(arg0) + 0);
    *(s32 *)((s8*)(*(void **)((s8*)(arg0) + 0x5C)) + 0x38) = (s32) *(s32 *)((s8*)(arg0) + 4);
    *(s32 *)((s8*)(*(void **)((s8*)(arg0) + 0x5C)) + 0x3C) = (s32) *(s32 *)((s8*)(arg0) + 8);
    *(s16 *)((s8*)(*(void **)((s8*)(arg0) + 0x5C)) + 0x46) = (s16) *(s32 *)((s8*)(arg0) + 0x54);
}


void func_80078ED4(void *arg0) {
    *(s16 *)((s8*)(arg0) + 0) = 0x100;
    *(s16 *)((s8*)(arg0) + 4) = 0x10;
    *(s16 *)((s8*)(arg0) + 2) = 0;
    *(s16 *)((s8*)(arg0) + 6) = 0;
    *(s16 *)((s8*)(arg0) + 8) = 0;
    *(s16 *)((s8*)(arg0) + 0xA) = 0x30;
    *(s16 *)((s8*)(arg0) + 0xC) = 0x30;
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80078F00);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007920C);
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_camera_frame_actors_for_bout (src/decomp/src/menu/arena_fighters_bout_and_effects.c:2505). */
#ifndef XM_S_arena_camera_frame_actors_for_bout
#define XM_S_arena_camera_frame_actors_for_bout
void xm_arena_camera_frame_actors_for_bout(Actor *first, Actor *second) __asm__("func_800796B8");
#endif
#ifndef XM_S_arena_scene_bout_end_active
#define XM_S_arena_scene_bout_end_active
extern u8 xm_arena_scene_bout_end_active __asm__("D_8009293C");
#endif
#ifndef XM_S_arena_vector_get_distance
#define XM_S_arena_vector_get_distance
s32 xm_arena_vector_get_distance(VECTOR *from, VECTOR *to) __asm__("func_800887A4");
#endif
#ifndef XM_S_arena_camera_focus
#define XM_S_arena_camera_focus
extern VECTOR xm_arena_camera_focus __asm__("D_8009867C");
#endif
#ifndef XM_S_arena_camera_position
#define XM_S_arena_camera_position
extern VECTOR xm_arena_camera_position __asm__("D_8009871C");
#endif
#ifndef XM_S_arena_camera_side_angle
#define XM_S_arena_camera_side_angle
extern s32 xm_arena_camera_side_angle __asm__("D_8009290C");
#endif
#ifndef XM_S_arena_camera_side_flipped
#define XM_S_arena_camera_side_flipped
extern u8 xm_arena_camera_side_flipped __asm__("D_800928F4");
#endif
#ifndef XM_S_arena_camera_ease_step_count
#define XM_S_arena_camera_ease_step_count
extern u8 xm_arena_camera_ease_step_count __asm__("D_8009287C");
#endif
#ifndef XM_S_arena_stage_keep_step_inside
#define XM_S_arena_stage_keep_step_inside
void xm_arena_stage_keep_step_inside(VECTOR *pos, VECTOR *step, s32 radius) __asm__("func_800828F8");
#endif
void func_800796B8(Actor *first, Actor *second) {
    VECTOR eye;
    VECTOR step;
    s32 angle;
    s32 dist;
    s32 radius;
    s32 floor;

    if (xm_arena_scene_bout_end_active != 0) {
        return;
    }
    angle = xm_ratan2(first->pos.vx - second->pos.vx, first->pos.vz - second->pos.vz);
    dist = xm_arena_vector_get_flat_distance(&first->pos, &second->pos);
    xm_arena_actors_heading = angle;
    xm_arena_actors_flat_distance = dist;
    dist = xm_arena_vector_get_distance(&first->pos, &second->pos);
    radius = dist * 2 / 3 + 0xC0;
    xm_arena_camera_focus.vx = (first->pos.vx + second->pos.vx) / 2;
    xm_arena_camera_focus.vy = (first->pos.vy + second->pos.vy) / 2 - 0xA0;
    xm_arena_camera_focus.vz = (first->pos.vz + second->pos.vz) / 2;
    eye.vx = xm_arena_camera_focus.vx + ((xm_gpu_get_sin(angle - 0x400) * radius) >> 12);
    eye.vz = xm_arena_camera_focus.vz + ((xm_gpu_get_cos(angle - 0x400) * radius) >> 12);
    step.vx = xm_arena_camera_focus.vx + ((xm_gpu_get_sin(angle + 0x400) * radius) >> 12);
    step.vz = xm_arena_camera_focus.vz + ((xm_gpu_get_cos(angle + 0x400) * radius) >> 12);
    eye.vx -= xm_arena_camera_position.vx;
    eye.vy -= xm_arena_camera_position.vy;
    eye.vz -= xm_arena_camera_position.vz;
    step.vx -= xm_arena_camera_position.vx;
    step.vy -= xm_arena_camera_position.vy;
    step.vz -= xm_arena_camera_position.vz;
    floor = xm_arena_vector_get_flat_length(&eye);
    if (xm_arena_vector_get_flat_length(&step) < floor) {
        xm_arena_camera_side_angle = 0x400;
        xm_arena_camera_side_flipped = 0;
    } else {
        xm_arena_camera_side_angle = -0x400;
        xm_arena_camera_side_flipped = 1;
    }
    dist /= 4;
    if (dist > 0x300) {
        dist = 0x300;
    }
    eye.vy = xm_arena_camera_focus.vy - 0x40 - dist;
    eye.vx = xm_arena_camera_focus.vx + ((xm_gpu_get_sin(angle + xm_arena_camera_side_angle) * radius) >> 12);
    eye.vz = xm_arena_camera_focus.vz + ((xm_gpu_get_cos(angle + xm_arena_camera_side_angle) * radius) >> 12);
    step.vx = (eye.vx - xm_arena_camera_position.vx) / xm_arena_camera_ease_step_count;
    step.vz = (eye.vz - xm_arena_camera_position.vz) / xm_arena_camera_ease_step_count;
    xm_arena_stage_keep_step_inside(&xm_arena_camera_position, &step, 0x3A00);
    xm_arena_camera_position.vx += step.vx;
    xm_arena_camera_position.vz += step.vz;
    floor = xm_arena_stage_get_ground_height(&xm_arena_camera_position, 0) - 0x100;
    if (floor < eye.vy) {
        eye.vy = floor;
    }
    xm_arena_camera_position.vy += (eye.vy - xm_arena_camera_position.vy) / xm_arena_camera_ease_step_count;
    xm_arena_camera_ease_step_count = 0x64;
}
#endif

void func_80079A8C(void) {
    func_800732CC();
    func_8008DC28();
    func_80088AF8();
    D_80099D9D = 1;
    D_80099DA1 = 3;
    D_8009292C = 0x100;
    D_80099D9E = 0;
    D_80099D9A = 0;
    D_80099DA2 = 0;
    D_80099DA4 = 0x100;
}


void func_80079B04(void) {

}


void func_80079B0C(void) {
    D_80092950 = 0;
    D_8009881E = 0;
    D_80097102 = 0;
    D_80092918 = 0;
    D_80092944 = 0;
    D_800928FC = 0;
}

void func_800346A4(s32 *);                   /* extern */
void func_8008DCA8(s32);                     /* extern */
extern s8 D_80050622;
extern s8 D_80091144;
extern s8 D_80091145;
extern s32 D_80092638;
extern s32 D_8009263C;
extern s32 D_80092648;
extern s8 D_80092664;
extern s8 D_800928B4;
extern s8 D_800928F0;
extern s8 D_800928F4;
extern s32 D_8009290C;
extern s32 D_8009294C;
extern u8 D_80097010__a[] asm("D_80097010");
extern u8 D_8009872C__a[] asm("D_8009872C");

void func_80078F00(void *);
void func_800831C8(void);

s32 func_80079B44(void) {
    *(s16 *)(D_8009872C__a + 0xCC) = 0x400;
    *(s16 *)(D_80097010__a + 0xCC) = -0x400;
    func_80078F00(D_8009872C__a);
    func_80078F00(D_80097010__a);
    func_8008D580(D_80092644);
    func_8007E24C();
    func_8007BB7C();
    func_800831C8();
    D_800928F4 = 1;
    D_8009290C = -1;
    func_8008DCA8(0);
    D_80092638 = 0;
    D_80092640 = 0;
    D_80092890 = 0;
    D_8009263C = 0x5A;
    D_8009294C = 0;
    D_80092648 = 0;
    func_8007F834();
    D_80050622 = 0;
    func_8008E620();
    func_800720C4();
    D_800928D4 = 0;
    D_800928F0 = 0;
    D_80091144 = 0;
    D_80091145 = 0;
    D_80092664 = 0;
    D_80092950 += 1;
    func_800346A4(&D_8009868C);
    D_80099D9A = 0;
    if (*(u8 *)&D_8005061C != 0) {
        switch ((D_80092950 - 1) % 5) {
        case 3:
            D_800928B4 = 1;
            return;
        case 4:
            D_800928B4 = 2;
            return;
        default:
            D_800928B4 = 0;
            break;
        }
    }
}


void func_80079D08(void *arg0) {
    void *temp_a1;
    void *temp_a1_2;
    void *temp_a1_3;

    temp_a1 = *(void **)((s8*)(arg0) + 0x15CC);
    *(u16 *)((s8*)(temp_a1) + 0xA) = (u16) ((*(u16 *)((s8*)(temp_a1) + 0xA) & 0xFEFF) | ((*(u32 *)((s8*)(arg0) + 0xD0) << 6) & 0x100));
    temp_a1_2 = *(void **)((s8*)(arg0) + 0x15CC);
    *(u16 *)((s8*)(temp_a1_2) + 6) = (u16) ((*(u16 *)((s8*)(temp_a1_2) + 6) & 0xDFFF) | (((u32) *(u32 *)((s8*)(arg0) + 0xD0) >> 2) & 0x2000));
    temp_a1_3 = *(void **)((s8*)(arg0) + 0x15CC);
    *(u16 *)((s8*)(temp_a1_3) + 6) = (u16) ((*(u16 *)((s8*)(temp_a1_3) + 6) & 0xBFFF) | (((u32) *(u32 *)((s8*)(arg0) + 0xD0) >> 5) & 0x4000));
}

typedef struct {
    u32 pad0 : 2;
    u32 b2 : 1;
    u32 pad3 : 12;
    u32 b15 : 1;
    u32 pad16 : 3;
    u32 b19 : 1;
    u32 pad20 : 12;
} func_80079D6C_d0;
typedef struct {
    u16 pad0 : 8;
    u16 b8 : 1;
    u16 pad9 : 7;
} func_80079D6C_fa;
typedef struct {
    u16 pad0 : 13;
    u16 b13 : 1;
    s16 b14 : 1;
    u16 pad15 : 1;
} func_80079D6C_f6;

void func_80079D6C(void *arg0) {
    ((func_80079D6C_d0 *)((s8*)(arg0) + 0xD0))->b2 = ((func_80079D6C_fa *)((s8*)(*(void **)((s8*)(arg0) + 0x15CC)) + 0xA))->b8;
    ((func_80079D6C_d0 *)((s8*)(arg0) + 0xD0))->b15 = ((func_80079D6C_f6 *)((s8*)(*(void **)((s8*)(arg0) + 0x15CC)) + 6))->b13;
    ((func_80079D6C_d0 *)((s8*)(arg0) + 0xD0))->b19 = ((func_80079D6C_f6 *)((s8*)(*(void **)((s8*)(arg0) + 0x15CC)) + 6))->b14;
}


void func_80079DE0(void) {
    D_80092640 = 0;
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_bout_update (src/decomp/src/menu/arena_fighters_bout_and_effects.c:2665). */
#ifndef XM_T_Settings
#define XM_T_Settings
typedef struct Settings {
    u8 level;     
    u8 unk1;
    u8 rate;      
    u8 option4;   
    u8 option5;   
    u8 com1;      
    u8 driven;    
    u8 option6;   
    u8 unk8;
    u8 speed;     
    u8 command;   
    u8 unkB;
    s16 unkC;
} Settings;
#endif
#ifndef XM_S_arena_bout_update
#define XM_S_arena_bout_update
void xm_arena_bout_update(Actor *first, Actor *second) __asm__("func_80079DF0");
#endif
#ifndef XM_S_arena_bout_frame_skip_timer
#define XM_S_arena_bout_frame_skip_timer
extern u8 xm_arena_bout_frame_skip_timer __asm__("D_80092664");
#endif
#ifndef XM_S_arena_settings
#define XM_S_arena_settings
extern Settings xm_arena_settings __asm__("D_80099D98");
#endif
#ifndef XM_S_arena_bout_pose_ring_index
#define XM_S_arena_bout_pose_ring_index
extern u8 xm_arena_bout_pose_ring_index __asm__("D_800928C0");
#endif
#ifndef XM_S_arena_actor_update_shots
#define XM_S_arena_actor_update_shots
s32 xm_arena_actor_update_shots(Actor *actor) __asm__("func_80073644");
#endif
#ifndef XM_S_arena_actor_start_frame
#define XM_S_arena_actor_start_frame
void xm_arena_actor_start_frame(Actor *actor) __asm__("func_800764CC");
#endif
#ifndef XM_S_arena_effect_hit_spark_count
#define XM_S_arena_effect_hit_spark_count
extern s32 xm_arena_effect_hit_spark_count __asm__("D_80092648");
#endif
#ifndef XM_S_arena_actor_test_hits
#define XM_S_arena_actor_test_hits
s32 xm_arena_actor_test_hits(Actor *actor) __asm__("func_80075B50");
#endif
#ifndef XM_S_console_place_cursor_and_line_start
#define XM_S_console_place_cursor_and_line_start
void xm_console_place_cursor_and_line_start(s32 x, s32 y) __asm__("func_8003708C");
#endif
#ifndef XM_S_arena_text_set_banner_timer
#define XM_S_arena_text_set_banner_timer
void xm_arena_text_set_banner_timer(s32 state) __asm__("func_8007E528");
#endif
#ifndef XM_S_arena_bout_fight_active
#define XM_S_arena_bout_fight_active
extern u8 xm_arena_bout_fight_active __asm__("D_800928D4");
#endif
#ifndef XM_S_pad_has_queue_overflowed
#define XM_S_pad_has_queue_overflowed
s32 xm_pad_has_queue_overflowed(void) __asm__("func_80036410");
#endif
#ifndef XM_S_pad_clear_queue
#define XM_S_pad_clear_queue
void xm_pad_clear_queue(void) __asm__("ControllerResetState");
#endif
#ifndef XM_S_pad_dequeue_state
#define XM_S_pad_dequeue_state
u32 xm_pad_dequeue_state(void) __asm__("ControllerPopState");
#endif
#ifndef XM_S_pad_port0_pressed
#define XM_S_pad_port0_pressed
extern u16 xm_pad_port0_pressed __asm__("g_C1ButtonStateReleased");
#endif
#ifndef XM_S_pad_port1_pressed
#define XM_S_pad_port1_pressed
extern u16 xm_pad_port1_pressed __asm__("g_C2ButtonStateReleased");
#endif
#ifndef XM_S_arena_bout_countdown_timer
#define XM_S_arena_bout_countdown_timer
extern s32 xm_arena_bout_countdown_timer __asm__("D_8009263C");
#endif
#ifndef XM_S_pad_get_controller_kind
#define XM_S_pad_get_controller_kind
s32 xm_pad_get_controller_kind(s32 port) __asm__("ControllerGetType");
#endif
#ifndef XM_S_arena_play_mode
#define XM_S_arena_play_mode
extern s32 xm_arena_play_mode __asm__("D_800928C8");
#endif
#ifndef XM_S_arena_menu_driving_pad_port
#define XM_S_arena_menu_driving_pad_port
extern u8 xm_arena_menu_driving_pad_port __asm__("D_800928FC");
#endif
#ifndef XM_S_arena_retreat_rule_enabled
#define XM_S_arena_retreat_rule_enabled
extern u8 xm_arena_retreat_rule_enabled __asm__("D_800928C4");
#endif
#ifndef XM_S_arena_menu_open_pause
#define XM_S_arena_menu_open_pause
void xm_arena_menu_open_pause(s32 mode) __asm__("func_80080C48");
#endif
#ifndef XM_S_arena_actor_read_pad
#define XM_S_arena_actor_read_pad
s32 xm_arena_actor_read_pad(Actor *actor) __asm__("func_80076884");
#endif
#ifndef XM_S_pad_merge_queued_states
#define XM_S_pad_merge_queued_states
void xm_pad_merge_queued_states(void) __asm__("func_80036420");
#endif
#ifndef XM_S_arena_bout_fight_frame_count
#define XM_S_arena_bout_fight_frame_count
extern s32 xm_arena_bout_fight_frame_count __asm__("D_80092944");
#endif
#ifndef XM_S_arena_brain_run_practice_command
#define XM_S_arena_brain_run_practice_command
void xm_arena_brain_run_practice_command(Actor *actor) __asm__("func_8008F280");
#endif
#ifndef XM_S_arena_brain_update
#define XM_S_arena_brain_update
void xm_arena_brain_update(Actor *actor) __asm__("func_80090E10");
#endif
#ifndef XM_S_arena_bout_round_over
#define XM_S_arena_bout_round_over
extern s32 xm_arena_bout_round_over __asm__("D_80092638");
#endif
#ifndef XM_S_arena_bout_update_referee
#define XM_S_arena_bout_update_referee
void xm_arena_bout_update_referee(Actor *first, Actor *second) __asm__("func_800751C8");
#endif
#ifndef XM_S_arena_actor_update_status
#define XM_S_arena_actor_update_status
s32 xm_arena_actor_update_status(Actor *actor) __asm__("func_80077038");
#endif
#ifndef XM_S_arena_actor_update_action
#define XM_S_arena_actor_update_action
void xm_arena_actor_update_action(Actor *actor) __asm__("func_80077A9C");
#endif
#ifndef XM_S_arena_actor_update_motion
#define XM_S_arena_actor_update_motion
void xm_arena_actor_update_motion(Actor *actor) __asm__("func_80078194");
#endif
#ifndef XM_S_arena_actor_apply_velocity
#define XM_S_arena_actor_apply_velocity
void xm_arena_actor_apply_velocity(Actor *actor) __asm__("func_80078D20");
#endif
#ifndef XM_S_arena_actor_place_model
#define XM_S_arena_actor_place_model
void xm_arena_actor_place_model(Actor *actor) __asm__("func_80078E94");
#endif
#ifndef XM_S_arena_scene_update_bout_end
#define XM_S_arena_scene_update_bout_end
void xm_arena_scene_update_bout_end(void) __asm__("func_80072170");
#endif
#ifndef XM_S_arena_actor_record_pose_and_animate
#define XM_S_arena_actor_record_pose_and_animate
void xm_arena_actor_record_pose_and_animate(Actor *actor) __asm__("func_80074BA4");
#endif
#ifndef XM_S_arena_actor_store_pose_flags
#define XM_S_arena_actor_store_pose_flags
void xm_arena_actor_store_pose_flags(Actor *actor) __asm__("func_80079D08");
#endif
#ifndef XM_S_arena_actor_update_anchors
#define XM_S_arena_actor_update_anchors
void xm_arena_actor_update_anchors(Actor *actor) __asm__("func_8007920C");
#endif
#ifndef XM_S_arena_frame_event_run
#define XM_S_arena_frame_event_run
void xm_arena_frame_event_run(Actor *actor, s16 frame, s16 count) __asm__("func_80074678");
#endif
#ifndef XM_S_arena_effect_advance_frame
#define XM_S_arena_effect_advance_frame
void xm_arena_effect_advance_frame(void) __asm__("func_8007BACC");
#endif
#ifndef XM_S_arena_rubber_band_enabled
#define XM_S_arena_rubber_band_enabled
extern u8 xm_arena_rubber_band_enabled __asm__("D_80092884");
#endif
void xm_arena_bout_update(Actor *first, Actor *second) __asm__("func_80079DF0");
void xm_arena_bout_update(Actor *first, Actor *second) {
    if (xm_arena_bout_frame_skip_timer != 0) {
        xm_arena_bout_frame_skip_timer--;
        return;
    }
    xm_arena_bout_frame_skip_timer = xm_arena_settings.rate;
    first->pose = (Pose *)first->unk9CC + xm_arena_bout_pose_ring_index;
    second->pose = (Pose *)second->unk9CC + xm_arena_bout_pose_ring_index;
    first->move = (Move *)first->pose;
    xm_arena_bout_pose_ring_index++;
    second->move = (Move *)second->pose;
    first->flags = (first->flags & ~0x40) | ((xm_arena_settings.com1 & 1) << 6);
    second->flags = (second->flags & ~0x40) | ((xm_arena_settings.driven & 1) << 6);
    xm_arena_actor_update_shots(first);
    xm_arena_actor_update_shots(second);
    xm_arena_actor_start_frame(first);
    xm_arena_actor_start_frame(second);
    xm_arena_effect_hit_spark_count = 0;
    xm_arena_actor_test_hits(first);
    xm_arena_actor_test_hits(second);
    xm_console_place_cursor_and_line_start(0xA, 0x60);
    xm_arena_text_set_banner_timer(0);
    if (xm_arena_bout_fight_active != 0) {
        if (xm_pad_has_queue_overflowed()) {
            xm_pad_clear_queue();
        } else {
        poll:
            if (xm_pad_dequeue_state()) {
                if ((((xm_pad_port0_pressed | xm_pad_port1_pressed) & 0x800) && xm_arena_bout_countdown_timer < 0x14) || !xm_pad_get_controller_kind(0) ||
                    (!xm_pad_get_controller_kind(1) && xm_arena_play_mode == 2)) {
                    if ((xm_pad_port0_pressed & 0x800) || !xm_pad_get_controller_kind(0)) {
                        xm_arena_menu_driving_pad_port = 0;
                    } else {
                        if (second->flags & 0x40) {
                            goto next;
                        }
                        xm_arena_menu_driving_pad_port = 1;
                    }
                    if (xm_arena_retreat_rule_enabled == 0) {
                        xm_arena_menu_open_pause(xm_arena_play_mode == 4 ? 2 : 1);
                    }
                }
            next:
                xm_arena_actor_read_pad(first);
                xm_arena_actor_read_pad(second);
                goto poll;
            }
        }
    } else {
        xm_pad_merge_queued_states();
    }
    if (xm_arena_bout_fight_active != 0) {
        if (xm_arena_bout_fight_frame_count != 0x2BF1F) {
            xm_arena_bout_fight_frame_count++;
        }
        if (xm_arena_play_mode == 4) {
            xm_arena_brain_run_practice_command(second);
        } else {
            xm_arena_brain_update(first);
            xm_arena_brain_update(second);
        }
    }
    if (xm_arena_bout_round_over != 0) {
        first->state = 0;
        second->state = 0;
        xm_arena_actor_clear_inputs(first);
        xm_arena_actor_clear_inputs(second);
    }
    xm_arena_bout_update_referee(first, second);
    xm_arena_actor_update_status(first);
    xm_arena_actor_update_status(second);
    xm_console_place_cursor_and_line_start(0x4A, 0);
    xm_arena_actor_update_action(first);
    xm_console_place_cursor_and_line_start(0x6A, 0);
    xm_arena_actor_update_action(second);
    xm_console_place_cursor_and_line_start(0xA, 0x80);
    xm_arena_actor_update_motion(first);
    xm_arena_actor_update_motion(second);
    xm_arena_actor_apply_velocity(second);
    xm_arena_actor_apply_velocity(first);
    xm_arena_actors_push_apart(first, second);
    xm_arena_actor_place_model(second);
    xm_arena_actor_place_model(first);
    xm_arena_scene_update_bout_end();
    xm_arena_actor_record_pose_and_animate(first);
    xm_arena_actor_record_pose_and_animate(second);
    xm_arena_actor_store_pose_flags(first);
    xm_arena_actor_store_pose_flags(second);
    xm_arena_actor_update_anchors(first);
    xm_arena_actor_update_anchors(second);
    xm_arena_frame_event_run(first, first->unk998, first->unk99A);
    xm_arena_frame_event_run(second, second->unk998, second->unk99A);
    xm_arena_effect_advance_frame();
    if (xm_arena_rubber_band_enabled != 0) {
        xm_arena_effect_queue_bolt_by_type(&first->core, &second->core, 0x13);
    }
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_bout_start_replay (src/decomp/src/menu/arena_fighters_bout_and_effects.c:2765). */
#ifndef XM_S_arena_bout_start_replay
#define XM_S_arena_bout_start_replay
void xm_arena_bout_start_replay(s32 frames) __asm__("func_8007A21C");
#endif
#ifndef XM_S_arena_bout_replay_timer
#define XM_S_arena_bout_replay_timer
extern s32 xm_arena_bout_replay_timer __asm__("D_800928AC");
#endif
#ifndef XM_S_arena_bout_saved_positions
#define XM_S_arena_bout_saved_positions
extern VECTOR xm_arena_bout_saved_positions[4] __asm__("D_80092A34");
#endif
#ifndef XM_S_arena_second_actor
#define XM_S_arena_second_actor
extern Actor xm_arena_second_actor __asm__("D_80097010");
#endif
void func_8007A21C(s32 frames) {
    if (frames < 0xFF) {
        xm_arena_bout_replay_timer = frames - 2;
        xm_arena_bout_pose_ring_index -= frames;
    } else {
        xm_arena_bout_replay_timer = 0xFF;
    }
    xm_arena_bout_saved_positions[0] = xm_arena_first_actor.pos;
    xm_arena_bout_saved_positions[1] = xm_arena_second_actor.pos;
    xm_arena_bout_saved_positions[2] = xm_arena_first_actor.home;
    xm_arena_bout_saved_positions[3] = xm_arena_second_actor.home;
    xm_arena_bout_saved_positions[0].vy = xm_arena_bout_saved_positions[1].vy = xm_arena_bout_saved_positions[2].vy = xm_arena_bout_saved_positions[3].vy = 0x100;
}
#endif
extern u16 g_C1ButtonStateReleased;
extern s32 D_8006FD4C;
extern s32 D_80092648;
extern u8 D_8009287C;
extern u8 D_800928C0;
extern s32 D_800928E8;
extern u16 g_C2ButtonState;
extern u16 g_C2ButtonStateReleased;

typedef struct {
    s32 unk[4];
} func_8007A344_copy16;
typedef struct {
    u8 pad0[0x15CC];
    s32 p15CC;
    void *p15D0;
} func_8007A344_s;
void func_80036420();
void func_8007D65C(s32, s32, s32);

void func_8007A344(void *arg0, void *arg1) {
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v1;
    u16 var_a0;
    u8 temp_a0;
    void *temp_v1_2;
    void *temp_v1_3;

    temp_a0 = D_800928C0 + 1;
    temp_v0 = (D_800928C0 * 0xC) + 0x9CC;
    temp_v1 = arg0 + temp_v0;
    temp_v0_2 = arg1 + temp_v0;
    D_800928C0 = temp_a0;
    ((func_8007A344_s *)arg0)->p15CC = temp_v1;
    ((func_8007A344_s *)arg1)->p15CC = temp_v0_2;
    temp_v0_3 = ((temp_a0 & 0xFF) * 0xC) + 0x9CC;
    ((func_8007A344_s *)arg0)->p15D0 = (void *) (arg0 + temp_v0_3);
    ((func_8007A344_s *)arg1)->p15D0 = (void *) (arg1 + temp_v0_3);
    if (D_800928AC == 0xFF) {
        temp_v1_2 = *(void **)((s8*)(arg0) + 0x15D0);
        *(u16 *)((s8*)(temp_v1_2) + 6) = (u16) (*(u16 *)((s8*)(temp_v1_2) + 6) | 0x1000);
        temp_v1_3 = *(void **)((s8*)(arg1) + 0x15D0);
        *(u16 *)((s8*)(temp_v1_3) + 6) = (u16) (*(u16 *)((s8*)(temp_v1_3) + 6) | 0x1000);
        *(s8 *)((s8*)(*(void **)((s8*)(arg0) + 0x15D0)) + 0xA) = 1;
        *(s8 *)((s8*)(*(void **)((s8*)(arg1) + 0x15D0)) + 0xA) = 1;
        *(s8 *)((s8*)(*(void **)((s8*)(arg0) + 0x15D0)) + 9) = 1;
        *(s8 *)((s8*)(*(void **)((s8*)(arg1) + 0x15D0)) + 9) = 1;
    }
    if (D_800928E8 & 8) {
        func_8007E894(0x10, 0x10);
        func_8007EBE0(&D_8006FD4C);
    }
    func_80073644(arg0);
    func_80073644(arg1);
    func_800764CC(arg0);
    func_800764CC(arg1);
    *(func_8007A344_copy16 *)((s8*)(arg0) + 0x95C) = *(func_8007A344_copy16 *)((s8*)(arg0) + 0x93C);
    *(func_8007A344_copy16 *)((s8*)(arg1) + 0x95C) = *(func_8007A344_copy16 *)((s8*)(arg1) + 0x93C);
    *(func_8007A344_copy16 *)((s8*)(arg0) + 0x6C) = *(func_8007A344_copy16 *)((s8*)(arg0) + 0);
    *(func_8007A344_copy16 *)((s8*)(arg1) + 0x6C) = *(func_8007A344_copy16 *)((s8*)(arg1) + 0);
    D_80092648 = 0;
    if ((u8) D_8009287C >= 2U) {
        D_8009287C = 1;
    }
    func_80036420();
    if (*(u8 *)&D_800928FC == 1) {
        if (g_C2ButtonStateReleased & 0x20) {
            func_80083C0C(8);
        }
        var_a0 = g_C2ButtonState;
    } else {
        if (g_C1ButtonStateReleased & 0x20) {
            func_80083C0C(8);
        }
        var_a0 = g_C1ButtonState;
    }
    func_800832C0(var_a0);
    func_80074AB4(arg0);
    func_80074AB4(arg1);
    func_80078E94(arg0);
    func_80078E94(arg1);
    *(s32 *)((s8*)(arg0) + 0xD0) = (s32) ((*(s32 *)((s8*)(arg0) + 0xD0) & 0x9FFFFFFF) | ((func_800828C4(arg0) & 0x03000000) << 5));
    *(s32 *)((s8*)(arg1) + 0xD0) = (s32) ((*(s32 *)((s8*)(arg1) + 0xD0) & 0x9FFFFFFF) | ((func_800828C4(arg1) & 0x03000000) << 5));
    func_80079D6C(arg0);
    func_80079D6C(arg1);
    func_8007920C(arg0);
    func_8007920C(arg1);
    func_80075B50(arg0);
    func_80075B50(arg1);
    func_8007BACC();
    if (D_80092884 != 0) {
        func_8007D65C((s32)arg0 + 0x94C, (s32)arg1 + 0x94C, 0x13);
    }
    if (D_800928AC == 0xFF) {
        func_8008369C();
    }
    if (D_800928AC == 0) {
        func_80083C0C(8);
    }
    D_800928AC--;
}


void func_8007A6D0(void *arg0) {
    D_8009292C = 0x100;
    if (D_800928AC < 0x78) {
        *(s8 *)((s8*)(arg0) + 0x4F) = 0x10;
        *(s8 *)((s8*)(arg0) + 0x52) = 0;
        *(s8 *)((s8*)(arg0) + 0x4C) = 0x10;
        *(s32 *)((s8*)(arg0) + 0xD0) = (s32) (*(s32 *)((s8*)(arg0) + 0xD0) | 0x400);
        return;
    }
    *(s8 *)((s8*)(arg0) + 0x4F) = 0x10;
    *(s8 *)((s8*)(arg0) + 0x52) = 0;
    *(s8 *)((s8*)(arg0) + 0x4C) = 0;
    *(s32 *)((s8*)(arg0) + 0xD0) = (s32) (*(s32 *)((s8*)(arg0) + 0xD0) | 0x400);
}


void func_8007A730(void *arg0) {
    D_8009292C = 0x100;
    *(s8 *)((s8*)(arg0) + 0x4F) = 0x10;
    *(s8 *)((s8*)(arg0) + 0x52) = 0;
    *(s8 *)((s8*)(arg0) + 0x4C) = 9;
    *(s32 *)((s8*)(arg0) + 0xD0) = (s32) (*(s32 *)((s8*)(arg0) + 0xD0) | 0x02000400);
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_camera_start_victory_view (src/decomp/src/menu/arena_fighters_bout_and_effects.c:2878). */
#ifndef XM_S_arena_camera_start_victory_view
#define XM_S_arena_camera_start_victory_view
void xm_arena_camera_start_victory_view(Actor *actor) __asm__("func_8007A768");
#endif
#ifndef XM_S_arena_camera_victory_angle
#define XM_S_arena_camera_victory_angle
extern s32 xm_arena_camera_victory_angle __asm__("D_80092668");
#endif
#ifndef XM_S_arena_camera_victory_focus_height
#define XM_S_arena_camera_victory_focus_height
extern s32 xm_arena_camera_victory_focus_height __asm__("D_8009266C");
#endif
#ifndef XM_S_arena_camera_victory_height
#define XM_S_arena_camera_victory_height
extern s32 xm_arena_camera_victory_height __asm__("D_80092670");
#endif
#ifndef XM_S_arena_camera_victory_distance
#define XM_S_arena_camera_victory_distance
extern s32 xm_arena_camera_victory_distance __asm__("D_80092674");
#endif
#ifndef XM_S_SetGeomScreen
#define XM_S_SetGeomScreen
void xm_SetGeomScreen(long h) __asm__("SetGeomScreen");
#endif
#ifndef XM_S_rand
#define XM_S_rand
int xm_rand(void) __asm__("rand");
#endif
void func_8007A768(Actor *actor) {
    SceneHeader *header = actor->header;
    s32 angle;
    s32 y;

    xm_arena_camera_victory_angle = header->unk24;
    xm_arena_camera_victory_focus_height = header->unk28;
    xm_arena_camera_victory_height = header->unk26;
    xm_arena_camera_victory_distance = header->unk2A;
    xm_SetGeomScreen(0x200);
    angle = xm_rand();
    xm_arena_camera_focus.vy = actor->pos.vy;
    xm_arena_camera_focus.vx = actor->pos.vx;
    xm_arena_camera_focus.vz = actor->pos.vz;
    y = xm_arena_camera_focus.vy - 0x400;
    xm_arena_camera_position.vy = y - xm_arena_camera_victory_height;
    xm_arena_camera_position.vx = xm_arena_camera_focus.vx + (((xm_gpu_get_sin(angle) << 2) * xm_arena_camera_victory_distance) >> 12);
    xm_arena_camera_position.vz = xm_arena_camera_focus.vz + (((xm_gpu_get_cos(angle) << 2) * xm_arena_camera_victory_distance) >> 12);
}
#endif

void func_8007A884(void) {
    s8 var_v0_2;
    void *var_v0;

    func_8007E24C();
    func_8008D580(D_80092644);
    func_8007BB7C();
    func_8007F834();
    func_8008E620();
    if (D_80092890 != 0) {
        var_v0 = D_8009790C;
    } else {
        var_v0 = D_80099028;
    }
    if (*(s16 *)((s8*)(var_v0) + 0x2C) != 0) {
        D_80091150 = 1;
        var_v0_2 = 0x10;
    } else {
        D_80091150 = 2;
        var_v0_2 = -1;
    }
    D_80091151 = var_v0_2;
    D_80098730 = 0x100;
    D_80097014 = 0x100;
    D_8009879C = 0x100;
    D_80097080 = 0x100;
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007A958);
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_bout_start_result_view (src/decomp/src/menu/arena_fighters_bout_and_effects.c:2980). */
#ifndef XM_S_arena_bout_start_result_view
#define XM_S_arena_bout_start_result_view
void xm_arena_bout_start_result_view(void) __asm__("func_8007AC3C");
#endif
#ifndef XM_S_arena_bout_motion_speed
#define XM_S_arena_bout_motion_speed
extern s32 xm_arena_bout_motion_speed __asm__("D_8009292C");
#endif
#ifndef XM_S_arena_bout_round_winner
#define XM_S_arena_bout_round_winner
extern s32 xm_arena_bout_round_winner __asm__("D_80092890");
#endif
void func_8007AC3C(void) {
    xm_arena_bout_replay_timer = 0x96;
    xm_arena_bout_motion_speed = 0x100;
    xm_SetGeomScreen(0x200);
    xm_arena_first_actor.pos = xm_arena_bout_saved_positions[0];
    xm_arena_second_actor.pos = xm_arena_bout_saved_positions[1];
    xm_arena_first_actor.home = xm_arena_bout_saved_positions[2];
    xm_arena_second_actor.home = xm_arena_bout_saved_positions[3];
    xm_arena_first_actor.start_home = xm_arena_first_actor.home;
    xm_arena_second_actor.start_home = xm_arena_second_actor.home;
    xm_arena_first_actor.start = xm_arena_first_actor.pos;
    xm_arena_second_actor.start = xm_arena_second_actor.pos;
    if (xm_arena_bout_round_winner != 0) {
        xm_arena_camera_start_victory_view(&xm_arena_second_actor);
    } else {
        xm_arena_camera_start_victory_view(&xm_arena_first_actor);
    }
}
#endif
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007AE10);
void func_8007B210(void *arg0, s8 arg1) {
    *(s32 *)((u8 *)arg0 + 0x15CC) = (s32)((u8 *)arg0 + 0x9CC);
    *(u8 *)((u8 *)arg0 + 0x4F) = 0x10;
    *(s8 *)((u8 *)arg0 + 0x4C) = arg1;
    *(u8 *)((u8 *)arg0 + 0x52) = 0;
    D_8009292C = 0x100;
    func_80074BA4();
    func_80074678(arg0, *(s16 *)((u8 *)arg0 + 0x998), *(s16 *)((u8 *)arg0 + 0x99A));
}
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007B270);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007B388);
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_effect_advance_frame (src/decomp/src/menu/arena_fighters_bout_and_effects.c:3300). */
#ifndef XM_T_SparkleKind
#define XM_T_SparkleKind
typedef struct {
    u8 *u;          
    u8 *v;          
    u8 w, h;        
    u8 frame_count; 
    u8 unkB;
    u16 clut;       
    u16 tpage;      
    s16 gravity;    
    u8 unk12[2];
} SparkleKind;
#endif
#ifndef XM_T_SparkleTrail
#define XM_T_SparkleTrail
typedef struct {
    s16 screen[6];        

    struct Sparkle *prev; 
    u16 stamp;            
    s16 owner;            
    s32 size;             
    s32 key;              
} SparkleTrail;
#endif
#ifndef XM_T_Sparkle
#define XM_T_Sparkle
typedef struct Sparkle {
    POLY_FT4 prim[2];     
    u8 active;           
    s8 frame;            
    u8 frame_count;      
    u8 type;             
    s16 x, y, z;         
    u8 unk5A[6];
    union {
        struct {
            SparkleKind *kind; 
            u16 gravity;       
            u16 fall_speed;    
        } fall;
        SparkleTrail trail;
        struct {
            struct Sparkle *prev; 
            s16 x, y, z;          
            u16 stamp;            
            s16 owner;            
            u8 unk6E[2];
            s32 key;              
        } line;
    } u;
} Sparkle;
#endif
#ifndef XM_T_SPARKLE_COUNT
#define XM_T_SPARKLE_COUNT
#define SPARKLE_COUNT 60
#endif
#ifndef XM_S_arena_effect_sparkles
#define XM_S_arena_effect_sparkles
extern Sparkle xm_arena_effect_sparkles[SPARKLE_COUNT] __asm__("D_80092AD8");
#endif
#ifndef XM_S_arena_effect_frame_count
#define XM_S_arena_effect_frame_count
extern s32 xm_arena_effect_frame_count __asm__("D_800926A4");
#endif
#ifndef XM_S_arena_effect_queued_line_count
#define XM_S_arena_effect_queued_line_count
extern s32 xm_arena_effect_queued_line_count __asm__("D_800926B0");
#endif
#ifndef XM_S_arena_effect_latched_line_count
#define XM_S_arena_effect_latched_line_count
extern s32 xm_arena_effect_latched_line_count __asm__("D_800926B4");
#endif
void func_8007BACC(void) {
    Sparkle *sparkle;
    s32 i;
    s32 count;

    sparkle = xm_arena_effect_sparkles;
    for (i = 0; i < SPARKLE_COUNT; i++, sparkle++) {
        if (sparkle->active) {
            if (sparkle->frame == sparkle->frame_count) {
                sparkle->active = 0;
            } else {
                sparkle->frame++;
                if (sparkle->type == 0) {
                    sparkle->u.fall.fall_speed += sparkle->u.fall.gravity;
                    sparkle->y += sparkle->u.fall.fall_speed;
                }
            }
        }
    }
    xm_arena_effect_frame_count++;
    count = xm_arena_effect_queued_line_count;
    xm_arena_effect_queued_line_count = 0;
    xm_arena_effect_latched_line_count = count;
}
#endif

#ifndef XENO_PC_PORT
void func_8007BB7C(void) {
    u8 *var_v1;
    s32 var_v0;

    var_v1 = (u8 *) &D_80092AD8;
    var_v0 = 0x3B;
    do {
        *(s8 *)((s8*)(var_v1) + 0x50) = 0;
        var_v0 -= 1;
        var_v1 += 0x7C;
    } while (var_v0 >= 0);
}
#endif

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007BBA0);
extern u8 D_800926B8[];
void func_8007C100(void* arg0) {
    struct Bytes { u8 bytes[4]; };
    *(struct Bytes*)D_800926B8 = *(struct Bytes*)arg0;
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_effect_start_falling_sparkle (src/decomp/src/menu/arena_fighters_bout_and_effects.c:3452). */
#ifndef XM_S_arena_effect_start_falling_sparkle
#define XM_S_arena_effect_start_falling_sparkle
void xm_arena_effect_start_falling_sparkle(SVECTOR *pos, s32 kind) __asm__("func_8007C124");
#endif
#ifndef XM_S_arena_effect_sparkle_kinds
#define XM_S_arena_effect_sparkle_kinds
extern SparkleKind xm_arena_effect_sparkle_kinds[5] __asm__("D_80092A74");
#endif
#ifndef XM_S_arena_effect_sparkle2_color
#define XM_S_arena_effect_sparkle2_color
extern CVECTOR xm_arena_effect_sparkle2_color __asm__("D_800926B8");
#endif
void func_8007C124(SVECTOR *pos, s32 kind) {
    Sparkle *sparkle = xm_arena_effect_sparkles;
    SparkleKind *info;
    POLY_FT4 *prim;
    s32 i;

    for (i = 0; i < SPARKLE_COUNT; i++, sparkle++) {
        if (!sparkle->active) {
            break;
        }
    }
    if (i == SPARKLE_COUNT) {
        return;
    }
    sparkle->active = 1;
    info = &xm_arena_effect_sparkle_kinds[kind];
    sparkle->type = 0;
    sparkle->frame = 0;
    sparkle->u.fall.kind = info;
    sparkle->frame_count = info->frame_count;
    sparkle->u.fall.gravity = info->gravity;
    sparkle->u.fall.fall_speed = 0;
    sparkle->x = pos->vx;
    sparkle->y = pos->vy;
    sparkle->z = pos->vz;
    prim = sparkle->prim;
    prim->tpage = info->tpage;
    if (kind == 2) {
        prim->code &= ~1;
        prim->r0 = xm_arena_effect_sparkle2_color.r;
        prim->g0 = xm_arena_effect_sparkle2_color.g;
        prim->b0 = xm_arena_effect_sparkle2_color.b;
    } else {
        prim->code |= 1;
    }
    prim->clut = info->clut;
    sparkle->prim[1] = *prim;
}
#endif
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007C280);
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_effect_start_trail_sparkle (src/decomp/src/menu/arena_fighters_bout_and_effects.c:3612). */
#ifndef XM_S_arena_frame_count
#define XM_S_arena_frame_count
extern s32 xm_arena_frame_count __asm__("D_800928E8");
#endif
#ifndef XM_S_arena_effect_side_palette_x
#define XM_S_arena_effect_side_palette_x
extern s16 xm_arena_effect_side_palette_x __asm__("D_80092698");
#endif
#ifndef XM_S_arena_effect_side_palette_y
#define XM_S_arena_effect_side_palette_y
extern s16 xm_arena_effect_side_palette_y __asm__("D_8009269C");
#endif
#ifndef XM_S_arena_effect_trail_texture_tpage
#define XM_S_arena_effect_trail_texture_tpage
extern u16 xm_arena_effect_trail_texture_tpage __asm__("D_80092694");
#endif
#ifndef XM_S_arena_effect_side_palette_clut
#define XM_S_arena_effect_side_palette_clut
extern u16 xm_arena_effect_side_palette_clut __asm__("D_800926A0");
#endif
#ifndef XM_S_arena_effect_trail_sizes
#define XM_S_arena_effect_trail_sizes
extern s16 xm_arena_effect_trail_sizes[] __asm__("D_80091228");
#endif
void func_8007C880(s32 column, VECTOR *pos, s32 key, s32 size) {
    Sparkle *sparkle;
    Sparkle *other;
    POLY_FT4 *prim;
    s32 i;

    for (i = 0, sparkle = xm_arena_effect_sparkles; i < SPARKLE_COUNT; i++, sparkle++) {
        if (sparkle->active && sparkle->u.trail.key == key && sparkle->u.trail.owner == xm_arena_frame_count) {
            return;
        }
    }
    sparkle = xm_arena_effect_sparkles;
    for (i = 0; i < SPARKLE_COUNT; i++, sparkle++) {
        if (!sparkle->active) {
            break;
        }
    }
    if (i == SPARKLE_COUNT) {
        return;
    }
    prim = sparkle->prim;
    prim->u0 = prim->u1 = prim->u2 = prim->u3 = (u8)xm_arena_effect_side_palette_x * 4 + 8 + column * 4;
    prim->v0 = prim->v1 = prim->v2 = prim->v3 = xm_arena_effect_side_palette_y;
    prim->tpage = xm_arena_effect_trail_texture_tpage;
    prim->clut = xm_arena_effect_side_palette_clut;
    prim->code &= ~1;
    sparkle->prim[1] = *prim;
    sparkle->frame_count = 7;
    sparkle->type = 1;
    sparkle->active = 1;
    sparkle->frame = 0;
    sparkle->x = pos->vx;
    sparkle->y = pos->vy;
    sparkle->z = pos->vz;
    sparkle->u.trail.owner = xm_arena_frame_count;
    sparkle->u.trail.key = key;
    sparkle->u.trail.prev = NULL;
    sparkle->u.trail.stamp = xm_arena_effect_frame_count;
    sparkle->u.trail.size = xm_arena_effect_trail_sizes[size];
    for (other = xm_arena_effect_sparkles, i = 0; i < SPARKLE_COUNT; i++, other++) {
        if (other->active && other->u.trail.key == key && other != sparkle && other->type == 1 &&
            other->u.trail.stamp == (u16)(xm_arena_effect_frame_count - 1)) {
            sparkle->u.trail.prev = other;
        }
    }
}
#endif
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007CAA4);

s32 func_8007CD14(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    return (arg3 & 0x7F) | ((arg0 << 7) & 0x80) | (arg1 << 0x18) | ((arg2 << 8) & 0xFFFF00);
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_effect_start_line_sparkle (src/decomp/src/menu/arena_fighters_bout_and_effects.c:3713). */
void func_8007CD44(s32 column, VECTOR *from, VECTOR *to, s32 key) {
    Sparkle *sparkle;
    Sparkle *other;
    POLY_FT4 *prim;
    s32 i;

    for (i = 0, sparkle = xm_arena_effect_sparkles; i < SPARKLE_COUNT; i++, sparkle++) {
        if (sparkle->active && sparkle->u.line.key == key && sparkle->u.line.owner == xm_arena_frame_count) {
            return;
        }
    }
    sparkle = xm_arena_effect_sparkles;
    for (i = 0; i < SPARKLE_COUNT; i++, sparkle++) {
        if (!sparkle->active) {
            break;
        }
    }
    if (i == SPARKLE_COUNT) {
        return;
    }
    sparkle->frame_count = 7;
    sparkle->type = 2;
    sparkle->active = 1;
    sparkle->frame = 0;
    sparkle->x = from->vx;
    sparkle->y = from->vy;
    sparkle->z = from->vz;
    sparkle->u.line.owner = xm_arena_frame_count;
    sparkle->u.line.x = to->vx;
    sparkle->u.line.y = to->vy;
    sparkle->u.line.z = to->vz;
    sparkle->u.line.stamp = xm_arena_effect_frame_count;
    prim = sparkle->prim;
    prim->u0 = prim->u1 = prim->u2 = prim->u3 = (u8)xm_arena_effect_side_palette_x * 4 + 8 + column * 4;
    prim->v0 = prim->v1 = prim->v2 = prim->v3 = xm_arena_effect_side_palette_y;
    sparkle->u.line.key = key;
    sparkle->u.line.prev = NULL;
    prim->tpage = xm_arena_effect_trail_texture_tpage;
    prim->clut = xm_arena_effect_side_palette_clut;
    prim->code &= ~1;
    sparkle->prim[1] = *prim;
    for (other = xm_arena_effect_sparkles, i = 0; i < SPARKLE_COUNT; i++, other++) {
        if (other->active && other->u.line.key == key && other != sparkle && other->type == 2 &&
            other->u.line.stamp == (u16)(xm_arena_effect_frame_count - 1)) {
            sparkle->u.line.prev = other;
        }
    }
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_effect_draw_all (src/decomp/src/menu/arena_fighters_bout_and_effects.c:3764). */
#ifndef XM_T_SceneScratch
#define XM_T_SceneScratch
typedef struct {
    VECTOR camera; 
    SVECTOR point; 
    SVECTOR from;  
    SVECTOR to;    
    SVECTOR extra; 
    VECTOR corner[6]; 
    u8 unk90[0x20];
    s32 depth;     
} SceneScratch;
#endif
#ifndef XM_T_SCENE_SCRATCH
#define XM_T_SCENE_SCRATCH
#define SCENE_SCRATCH ((SceneScratch *)0x1F800000)
#endif
#ifndef XM_T_gte_SetRotMatrix
#define XM_T_gte_SetRotMatrix
#define gte_SetRotMatrix(r0)                                                   \
    __asm__ volatile("lw $12, 0(%0);"                                          \
                     "lw $13, 4(%0);"                                          \
                     "ctc2 $12, $0;"                                           \
                     "ctc2 $13, $1;"                                           \
                     "lw $12, 8(%0);"                                          \
                     "lw $13, 12(%0);"                                         \
                     "lw $14, 16(%0);"                                         \
                     "ctc2 $12, $2;"                                           \
                     "ctc2 $13, $3;"                                           \
                     "ctc2 $14, $4"                                            \
                     :                                                         \
                     : "r"(r0)                                                 \
                     : "$12", "$13", "$14", "memory")
#endif
#ifndef XM_T_gte_SetTransMatrix
#define XM_T_gte_SetTransMatrix
#define gte_SetTransMatrix(r0)                                                 \
    __asm__ volatile("lw $12, 20(%0);"                                         \
                     "lw $13, 24(%0);"                                         \
                     "ctc2 $12, $5;"                                           \
                     "lw $14, 28(%0);"                                         \
                     "ctc2 $13, $6;"                                           \
                     "ctc2 $14, $7"                                            \
                     :                                                         \
                     : "r"(r0)                                                 \
                     : "$12", "$13", "$14", "memory")
#endif
#ifndef XM_S_arena_effect_draw_all
#define XM_S_arena_effect_draw_all
void xm_arena_effect_draw_all(MATRIX *view, u32 *ot) __asm__("func_8007CF78");
#endif
#ifndef XM_S_libgte_transpose_matrix
#define XM_S_libgte_transpose_matrix
void xm_libgte_transpose_matrix(MATRIX *m, MATRIX *out) __asm__("TransposeMatrix");
#endif
#ifndef XM_S_arena_effect_draw_falling_sparkles
#define XM_S_arena_effect_draw_falling_sparkles
void xm_arena_effect_draw_falling_sparkles(MATRIX *view, MATRIX *local, u32 *ot) __asm__("func_8007BBA0");
#endif
#ifndef XM_S_arena_effect_draw_trail_sparkles
#define XM_S_arena_effect_draw_trail_sparkles
void xm_arena_effect_draw_trail_sparkles(MATRIX *view, MATRIX *local, u32 *ot) __asm__("func_8007C280");
#endif
#ifndef XM_S_arena_effect_draw_line_sparkles
#define XM_S_arena_effect_draw_line_sparkles
void xm_arena_effect_draw_line_sparkles(MATRIX *view, MATRIX *unused, u32 *ot) __asm__("func_8007CAA4");
#endif
#ifndef XM_S_arena_effect_draw_ground_particles
#define XM_S_arena_effect_draw_ground_particles
void xm_arena_effect_draw_ground_particles(u32 *ot) __asm__("func_8007D918");
#endif
#ifndef XM_S_arena_effect_draw_thrown_cells
#define XM_S_arena_effect_draw_thrown_cells
void xm_arena_effect_draw_thrown_cells(u32 *ot) __asm__("func_8007E020");
#endif
#ifndef XM_S_arena_effect_draw_lines
#define XM_S_arena_effect_draw_lines
void xm_arena_effect_draw_lines(u32 *ot) __asm__("func_8007E3CC");
#endif
void func_8007CF78(MATRIX *view, u32 *ot) {
    MATRIX local;

    SCENE_SCRATCH->camera = xm_arena_view_origin;
    xm_libgte_transpose_matrix(view, &local);
    xm_arena_effect_draw_falling_sparkles(view, &local, ot);
    xm_arena_effect_draw_trail_sparkles(view, &local, ot);
    xm_arena_effect_draw_line_sparkles(view, NULL, ot);
    gte_SetRotMatrix(view);
    gte_SetTransMatrix(view);
    xm_arena_effect_draw_ground_particles(ot);
    xm_arena_effect_draw_thrown_cells(ot);
    xm_arena_effect_draw_lines(ot);
}
#endif
void func_8007E3CC(void*);
void func_8007D068(void* arg0) {
    struct Vector { s32 x,y,z,pad; };
    *(struct Vector*)0x1F800000 = *(struct Vector*)&D_80096FA8;
    func_8007E3CC(arg0);
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_effect_jitter_svector (src/decomp/src/menu/arena_fighters_bout_and_effects.c:3786). */
#ifndef XM_S_arena_effect_jitter_svector
#define XM_S_arena_effect_jitter_svector
void xm_arena_effect_jitter_svector(SVECTOR *pos) __asm__("func_8007D0B4");
#endif
void func_8007D0B4(SVECTOR *pos) {
    pos->vx += xm_rand() % 48 - 24;
    pos->vy += xm_rand() % 48 - 24;
    pos->vz += xm_rand() % 48 - 24;
}
#endif
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007D190);
s32 func_8007D25C(s32 arg0) {
    if (arg0 < 0x10) {
        return 0;
    }
    return arg0 < 0x20;
}
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_effect_jitter_vector (src/decomp/src/menu/arena_fighters_bout_and_effects.c:3839). */
#ifndef XM_S_arena_effect_jitter_vector
#define XM_S_arena_effect_jitter_vector
void xm_arena_effect_jitter_vector(VECTOR *from, VECTOR *to) __asm__("func_8007D274");
#endif
void func_8007D274(VECTOR *from, VECTOR *to) {
    to->vx = from->vx + xm_rand() % 64 - 32;
    to->vy = from->vy + xm_rand() % 64 - 32;
    to->vz = from->vz + xm_rand() % 64 - 32;
}
#endif
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007D334);
void func_8007D65C(s32 arg0, s32 arg1, s32 arg2) {
    switch (arg2) {
    case 0x11:
        arg2 = 0;
        break;
    case 0x12:
        arg2 = 1;
        break;
    case 0x13:
        arg2 = 2;
        break;
    default:
        return;
    }
    func_8007D334(arg0, arg1, arg2);
}
extern s32 D_800926BC;
extern s32 D_800926C0;
extern s32 D_800926C4;

void func_8007D6B8(void) {
    register s16 *var_a1 asm("$5");
    register u8 *var_v1 asm("$3");
    s32 allocation;
    register s32 tag asm("$8");
    register s32 color asm("$7");
    register s32 var_a0 asm("$4");
    register s32 var_a2 asm("$6");

    D_800926BC = HeapAlloc(0x9F6, 0);
    D_800926C0 = HeapAlloc(0xBF4, 0);
    allocation = (s32)HeapAlloc(0xBF4, 0);
    var_a2 = 0;
    tag = 2;
    color = 0x6880B0F0;
    var_a1 = D_800926BC;
    var_a0 = 0;
    D_800926C4 = allocation;
    var_v1 = (u8*)var_a1 + 4;
    do {
        *(s8 *)((s8*)((var_a0 + D_800926C0)) + 3) = tag;
        *(s32 *)((s8*)((var_a0 + D_800926C0)) + 4) = color;
        *(s8 *)((s8*)((var_a0 + D_800926C4)) + 3) = tag;
        var_a2 += 1;
        *(s32 *)((s8*)((var_a0 + D_800926C4)) + 4) = color;
        *(s16 *)((s8*)(var_v1) + 0) = 0;
        *(s16 *)((s8*)(var_v1) + -2) = 0;
        *var_a1 = 0;
        *(s8 *)((s8*)(var_v1) + 2) = 0;
        *(s16 *)((s8*)(var_v1) + 0) = 0;
        var_v1 += 10;
        *var_a1 = 0;
        var_a1 += 5;
        var_a0 += 0xC;
    } while (var_a2 < 0xFF);
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_effect_spawn_ground_particles (src/decomp/src/menu/arena_fighters_bout_and_effects.c:3949). */
#ifndef XM_T_SceneCell10
#define XM_T_SceneCell10
typedef struct {
    s16 unk0; 
    s16 unk2; 
    s16 unk4; 
    s8 unk6;  
    s8 unk7;  
    s16 unk8; 
} SceneCell10;
#endif
#ifndef XM_T_GroundSquare
#define XM_T_GroundSquare
typedef struct GroundSquare {
    u16 height;
    u16 unk2; 

} GroundSquare;
#endif
#ifndef XM_S_arena_effect_spawn_ground_particles
#define XM_S_arena_effect_spawn_ground_particles
void xm_arena_effect_spawn_ground_particles(VECTOR *pos, s32 count) __asm__("func_8007D7A8");
#endif
#ifndef XM_S_arena_effect_ground_particles
#define XM_S_arena_effect_ground_particles
extern SceneCell10 *xm_arena_effect_ground_particles __asm__("D_800926BC");
#endif
#ifndef XM_S_arena_stage_height_map
#define XM_S_arena_stage_height_map
extern GroundSquare *xm_arena_stage_height_map __asm__("D_800928DC");
#endif
void func_8007D7A8(VECTOR *pos, s32 count) {
    SceneCell10 *cell = xm_arena_effect_ground_particles;
    s32 i;
    s32 x;
    s32 z;

    for (i = 0; i < 251; i++, cell++) {
        if (count == 0) {
            break;
        }
        if (cell->unk6 == 0) {
            x = pos->vx + (xm_rand() % 64 - 32);
            cell->unk0 = x;
            x = (s16)x;
            z = pos->vz + (xm_rand() % 64 - 32);
            cell->unk4 = z;
            z = (s16)z;
            cell->unk2 = cell->unk8 = xm_arena_stage_height_map[(x >> 8) + (z >> 8) * 128].height;
            cell->unk7 = -(xm_rand() % 10 + 10);
            count--;
            cell->unk6 = 20;
        }
    }
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_effect_draw_ground_particles (src/decomp/src/menu/arena_fighters_bout_and_effects.c:3977). */
#ifndef XM_T_Tile1Words
#define XM_T_Tile1Words
typedef struct {
    u8 addr[3];
    u8 len;
    u32 rgbc;
    s16 x0, y0;
} Tile1Words;
#endif
#ifndef XM_T_gte_ldv0
#define XM_T_gte_ldv0
#define gte_ldv0(r0)                                                           \
    __asm__ volatile("lwc2 $0, 0(%0);"                                         \
                     "lwc2 $1, 4(%0)"                                          \
                     :                                                         \
                     : "r"(r0)                                                 \
                     : "memory")
#endif
#ifndef XM_T_gte_ldv1
#define XM_T_gte_ldv1
#define gte_ldv1(r0)                                                           \
    __asm__ volatile("lwc2 $2, 0(%0);"                                         \
                     "lwc2 $3, 4(%0)"                                          \
                     :                                                         \
                     : "r"(r0)                                                 \
                     : "memory")
#endif
#ifndef XM_T_gte_ldv2
#define XM_T_gte_ldv2
#define gte_ldv2(r0)                                                           \
    __asm__ volatile("lwc2 $4, 0(%0);"                                         \
                     "lwc2 $5, 4(%0)"                                          \
                     :                                                         \
                     : "r"(r0)                                                 \
                     : "memory")
#endif
#ifndef XM_T_gte_rtpt
#define XM_T_gte_rtpt
#define gte_rtpt() __asm__ volatile("nop;nop;.word 0x4A280030")
#endif
#ifndef XM_T_gte_stsxy3
#define XM_T_gte_stsxy3
#define gte_stsxy3(r0, r1, r2)                                                 \
    __asm__ volatile("swc2 $12, 0(%0);"                                        \
                     "swc2 $13, 0(%1);"                                        \
                     "swc2 $14, 0(%2)"                                         \
                     :                                                         \
                     : "r"(r0), "r"(r1), "r"(r2)                               \
                     : "memory")
#endif
#ifndef XM_T_gte_stsz1
#define XM_T_gte_stsz1
#define gte_stsz1(r0) __asm__ volatile("swc2 $17, 0(%0)" : : "r"(r0) : "memory")
#endif
#ifndef XM_T_LINK_PRIM
#define XM_T_LINK_PRIM
#define LINK_PRIM(ot, scratch, prim, len)                                      \
    prev = (ot)[(scratch)->depth >> 4];                                        \
    addr = (u32)(prim) & 0xFFFFFF;                                             \
    (ot)[(scratch)->depth >> 4] = addr;                                        \
    prev |= (len);                                                             \
    *(u32 *)addr = prev
#endif
#ifndef XM_T_gte_stsz2
#define XM_T_gte_stsz2
#define gte_stsz2(r0) __asm__ volatile("swc2 $18, 0(%0)" : : "r"(r0) : "memory")
#endif
#ifndef XM_T_gte_stsz
#define XM_T_gte_stsz
#define gte_stsz(r0) __asm__ volatile("swc2 $19, 0(%0)" : : "r"(r0) : "memory")
#endif
#ifndef XM_S_arena_effect_ground_particle_tiles
#define XM_S_arena_effect_ground_particle_tiles
extern Tile1Words *xm_arena_effect_ground_particle_tiles[2] __asm__("D_800926C0");
#endif
#ifndef XM_S_arena_draw_buffer_index
#define XM_S_arena_draw_buffer_index
extern u8 xm_arena_draw_buffer_index __asm__("D_800928A0");
#endif
void func_8007D918(u32 *ot) {
    SceneScratch *scratch = SCENE_SCRATCH;
    Tile1Words *tile = xm_arena_effect_ground_particle_tiles[xm_arena_draw_buffer_index];
    SceneCell10 *cell = &xm_arena_effect_ground_particles[xm_arena_draw_buffer_index];
    s32 cx = scratch->camera.vx;
    s32 cy = scratch->camera.vy;
    s32 cz = scratch->camera.vz;
    s32 loaded = 0;
    s32 i;
    u32 prev;
    u32 addr;

    for (i = xm_arena_draw_buffer_index; i < 0xFC; i += 2, cell += 2) {
        if (cell->unk6 == 0) {
            continue;
        }
        scratch->point.vx = cell->unk0 - cx;
        scratch->point.vy = cell->unk2 - cy;
        scratch->point.vz = cell->unk4 - cz;
        if (loaded == 0) {
            gte_ldv0(&scratch->point);
            loaded = 1;
        } else if (loaded == 1) {
            gte_ldv1(&scratch->point);
            loaded = 2;
        } else {
            gte_ldv2(&scratch->point);
            gte_rtpt();
            loaded = 0;
            gte_stsxy3(&tile[0].x0, &tile[1].x0, &tile[2].x0);
            gte_stsz1(&scratch->depth);
            LINK_PRIM(ot, scratch, &tile[0], 0x02000000);
            gte_stsz2(&scratch->depth);
            LINK_PRIM(ot, scratch, &tile[1], 0x02000000);
            gte_stsz(&scratch->depth);
            LINK_PRIM(ot, scratch, &tile[2], 0x02000000);
            tile += 3;
        }
        cell->unk2 += cell->unk7;
        if (cell->unk2 >= cell->unk8) {
            cell->unk6 = 0;
        }
        cell->unk7 += 2;
    }
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_effect_alloc_thrown_cells (src/decomp/src/menu/arena_fighters_bout_and_effects.c:4025). */
#ifndef XM_T_SceneCell12
#define XM_T_SceneCell12
typedef struct {
    s16 unk0, unk2, unk4; 
    s16 unk6;             
    s8 unk8;              
    s8 unk9;              
    s16 unkA;             
} SceneCell12;
#endif
#ifndef XM_T_TileWords
#define XM_T_TileWords
typedef struct {
    u8 addr[3];
    u8 len;
    u32 rgbc;
    s16 x0, y0;
    s16 w, h;
} TileWords;
#endif
#ifndef XM_S_arena_effect_alloc_thrown_cells
#define XM_S_arena_effect_alloc_thrown_cells
void xm_arena_effect_alloc_thrown_cells(void) __asm__("func_8007DB28");
#endif
#ifndef XM_S_arena_effect_thrown_cells
#define XM_S_arena_effect_thrown_cells
extern SceneCell12 *xm_arena_effect_thrown_cells __asm__("D_800926C8");
#endif
#ifndef XM_S_heap_alloc
#define XM_S_heap_alloc
void *xm_heap_alloc(s32 size, s32 mode) __asm__("HeapAlloc");
#endif
#ifndef XM_S_arena_effect_thrown_cell_tiles
#define XM_S_arena_effect_thrown_cell_tiles
extern TileWords *xm_arena_effect_thrown_cell_tiles[2] __asm__("D_800926CC");
#endif
#ifndef XM_S_arena_copy_words
#define XM_S_arena_copy_words
void xm_arena_copy_words(void *dst, void *src, s32 size) __asm__("func_800732AC");
#endif
void func_8007DB28(void) {
    SceneCell12 *cell;
    TileWords *tile;
    s32 i;

    xm_arena_effect_thrown_cells = xm_heap_alloc(0x1950, 0);
    tile = xm_heap_alloc(0x21C0, 0);
    xm_arena_effect_thrown_cell_tiles[0] = tile;
    xm_arena_effect_thrown_cell_tiles[1] = xm_heap_alloc(0x21C0, 0);
    cell = xm_arena_effect_thrown_cells;
    for (i = 0; i < 540; i++, cell++, tile++) {
        tile->len = 3;
        tile->rgbc = 0x60FFD0A0;
        tile->w = xm_rand() % 3 + 2;
        tile->h = xm_rand() % 3 + 2;
        cell->unk0 = cell->unk2 = cell->unk4 = 0;
        cell->unk8 = cell->unkA = cell->unk9 = cell->unk6 = 0;
    }
    xm_arena_copy_words(xm_arena_effect_thrown_cell_tiles[1], xm_arena_effect_thrown_cell_tiles[0], 0x21C0);
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_effect_throw_cells (src/decomp/src/menu/arena_fighters_bout_and_effects.c:4050). */
#ifndef XM_S_arena_effect_throw_cells
#define XM_S_arena_effect_throw_cells
void xm_arena_effect_throw_cells(VECTOR *from, VECTOR *to) __asm__("func_8007DC74");
#endif
#ifndef XM_S_arena_vector_get_length
#define XM_S_arena_vector_get_length
s32 xm_arena_vector_get_length(VECTOR *vector) __asm__("func_800886FC");
#endif
#ifndef XM_S_VectorNormal
#define XM_S_VectorNormal
long xm_VectorNormal(VECTOR *v0, VECTOR *v1) __asm__("VectorNormal");
#endif
void func_8007DC74(VECTOR *from, VECTOR *to) {
    VECTOR across;
    s32 nx;
    s32 nz;
    s32 life;
    s32 speed;
    s32 count;
    s32 drift;
    s32 height;
    s32 i;
    s32 r;
    SceneCell12 *cell;

    if (from->vy <= 0x80 && to->vy <= 0x80) {
        return;
    }
    across.vz = to->vx - from->vx;
    across.vx = from->vz - to->vz;
    across.vy = from->vy - to->vy;
    cell = xm_arena_effect_thrown_cells;
    count = xm_arena_vector_get_length(&across) / 6;
    if (count <= 0) {
        count = 1;
    }
    across.vy = 0x1000;
    drift = xm_arena_vector_get_flat_length(&across);
    if (drift < 0x10) {
        drift = 0;
    }
    if (across.vz != 0 || across.vx != 0) {
        xm_VectorNormal(&across, &across);
        nx = across.vx;
        nz = across.vz;
    }
    height = abs(from->vy - to->vy);
    life = height / 3 + 1;
    if ((u32)life > 0x78) {
        life = 0x78;
    }
    for (i = 0; i < 0x218; i++, cell++) {
        if (count == 0) {
            return;
        }
        if (cell->unk6 != 0) {
            continue;
        }
        if (drift != 0) {
            r = xm_rand() % 4096;
            speed = drift / 7;
            if (xm_rand() & 1) {
                cell->unk8 = ((nx * r) >> 14) + speed;
                cell->unk9 = ((nz * r) >> 14) + speed;
            } else {
                cell->unk8 = -((nx * r) >> 14) - speed;
                cell->unk9 = -((nz * r) >> 14) - speed;
            }
        } else {
            cell->unk8 = (xm_rand() & 0xF) - 8;
            cell->unk9 = (xm_rand() & 0xF) - 8;
        }
        if (height >= 9) {
            cell->unkA = -(xm_rand() % (height / 2)) + 1;
            cell->unk6 = life;
        } else {
            cell->unkA = -4;
            cell->unk6 = 0x14;
        }
        cell->unk0 = from->vx + (xm_rand() % 32 - 0x10) + cell->unk8 * 2;
        cell->unk4 = from->vz + (xm_rand() % 32 - 0x10) + cell->unk9 * 2;
        cell->unk2 = -0x10;
        count--;
    }
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_effect_draw_thrown_cells (src/decomp/src/menu/arena_fighters_bout_and_effects.c:4127). */
void func_8007E020(u32 *ot) {
    SceneScratch *scratch = SCENE_SCRATCH;
    s32 loaded = 0;
    s32 i;
    s32 cx = scratch->camera.vx;
    s32 cy = scratch->camera.vy;
    s32 cz = scratch->camera.vz;
    SceneCell12 *cell = xm_arena_effect_thrown_cells;
    TileWords *tile = xm_arena_effect_thrown_cell_tiles[xm_arena_draw_buffer_index];
    u32 prev;
    u32 addr;

    for (i = 0; i < 0x218; i++, cell++) {
        if (cell->unk6 == 0) {
            continue;
        }
        cell->unk0 += cell->unk8;
        cell->unk4 += cell->unk9;
        scratch->point.vx = cell->unk0 - cx;
        scratch->point.vy = cell->unk2 - cy;
        scratch->point.vz = cell->unk4 - cz;
        if (loaded == 0) {
            gte_ldv0(&scratch->point);
            loaded = 1;
        } else if (loaded == 1) {
            gte_ldv1(&scratch->point);
            loaded = 2;
        } else {
            gte_ldv2(&scratch->point);
            gte_rtpt();
            loaded = 0;
            gte_stsxy3(&tile[0].x0, &tile[1].x0, &tile[2].x0);
            gte_stsz1(&scratch->depth);
            LINK_PRIM(ot, scratch, &tile[0], 0x03000000);
            gte_stsz2(&scratch->depth);
            LINK_PRIM(ot, scratch, &tile[1], 0x03000000);
            gte_stsz(&scratch->depth);
            LINK_PRIM(ot, scratch, &tile[2], 0x03000000);
            tile += 3;
        }
        cell->unk2 += cell->unkA;
        cell->unkA += 2;
        if (cell->unk2 > 0x10) {
            cell->unk2 = 0x10;
        }
        cell->unk6--;
    }
}
#endif
extern s32 D_800926BC;
extern void *D_800926C8;
extern s8 D_8009791B;
extern s8 D_80099037;

typedef struct {
    s16 unk0;
    s16 unk2;
    s16 unk4;
    s16 unk6;
    s16 unk8;
    s16 unkA;
} func_8007E24C_s;

void func_8007E24C(void) {
    s32 var_a0;
    s32 var_a0_2;
    s32 var_v1_2;
    void *temp_v0;
    func_8007E24C_s *var_v1;

    var_v1 = D_800926C8;
    for (var_a0 = 0; var_a0 < 0x21C; var_a0++) {
        var_v1[var_a0].unk6 = 0;
        var_v1[var_a0].unk4 = 0;
        var_v1[var_a0].unk2 = 0;
        var_v1[var_a0].unk0 = 0;
    }
    var_a0_2 = 0;
    var_v1_2 = 0;
    D_8009791B = 0;
    D_80099037 = 0;
    do {
        *(s8 *)((s8*)((var_v1_2 + D_800926BC)) + 6) = 0;
        var_a0_2 += 1;
        temp_v0 = var_v1_2 + D_800926BC;
        *(s16 *)((s8*)(temp_v0) + 4) = 0;
        *(s16 *)((s8*)(temp_v0) + 2) = 0;
        *(s16 *)((s8*)(temp_v0) + 0) = 0;
        var_v1_2 += 0xA;
    } while (var_a0_2 < 0xFF);
}

extern s32 D_800926B0;
extern s32 D_800926B4;
extern s32 D_80094818;

typedef struct {
    u8 pad0[3];
    s8 unk3;
    u8 pad4[3];
    s8 unk7;
    u8 pad8[0x18];
} func_8007E2D8_s;

void func_8007E2D8(void) {
    s32 i;

    for (i = 0; i < 100; i++) {
        ((func_8007E2D8_s *)&D_80094818)[i].unk3 = 3;
        ((func_8007E2D8_s *)&D_80094818)[i].unk7 = 0x40;
    }
    D_800926B0 = 0;
    D_800926B4 = 0;
}

extern s32 D_800926B0;
extern s32 D_80094818;
void func_8007E31C(void *arg0, void *arg1, void *arg2) {
    void *temp_v1;

    if (D_800926B0 < 0x64) {
        temp_v1 = (D_800926B0 << 5) + (u8 *)&D_80094818;
        (*(s16 *)((u8 *)(temp_v1) + 0x10)) = (s16) (*(s32 *)((u8 *)(arg0) + 0));
        (*(s16 *)((u8 *)(temp_v1) + 0x12)) = (s16) (*(s32 *)((u8 *)(arg0) + 4));
        (*(s16 *)((u8 *)(temp_v1) + 0x14)) = (s16) (*(s32 *)((u8 *)(arg0) + 8));
        (*(s16 *)((u8 *)(temp_v1) + 0x18)) = (s16) (*(s32 *)((u8 *)(arg1) + 0));
        (*(s16 *)((u8 *)(temp_v1) + 0x1A)) = (s16) (*(s32 *)((u8 *)(arg1) + 4));
        (*(s16 *)((u8 *)(temp_v1) + 0x1C)) = (s16) (*(s32 *)((u8 *)(arg1) + 8));
        (*(u8 *)((u8 *)(temp_v1) + 4)) = (u8) (*(u8 *)((u8 *)(arg2) + 0));
        (*(u8 *)((u8 *)(temp_v1) + 5)) = (u8) (*(u8 *)((u8 *)(arg2) + 1));
        (*(u8 *)((u8 *)(temp_v1) + 6)) = (u8) (*(u8 *)((u8 *)(arg2) + 2));
        D_800926B0 += 1;
    }
}
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_effect_draw_lines (src/decomp/src/menu/arena_fighters_bout_and_effects.c:4231). */
#ifndef XM_T_LINE_F2
#define XM_T_LINE_F2
typedef struct {
    u_long tag;
    u_char r0, g0, b0, code;
    short x0, y0;
    short x1, y1;
} LINE_F2;
#endif
#ifndef XM_T_SceneLine
#define XM_T_SceneLine
typedef struct {
    LINE_F2 line;
    SVECTOR from; 
    SVECTOR to;   
} SceneLine;
#endif
#ifndef XM_S_arena_effect_lines
#define XM_S_arena_effect_lines
extern SceneLine xm_arena_effect_lines[100] __asm__("D_80094818");
#endif
void xm_arena_effect_draw_lines(u32 *ot) __asm__("func_8007E3CC");
#pragma push_macro("gte_ldv01")
#undef gte_ldv01
#define gte_ldv01(r0, r1)                                                      \
    __asm__ volatile("lwc2 $0, 0(%0);"                                         \
                     "lwc2 $1, 4(%0);"                                         \
                     "lwc2 $2, 0(%1);"                                         \
                     "lwc2 $3, 4(%1)"                                          \
                     :                                                         \
                     : "r"(r0), "r"(r1) : "memory")
#pragma push_macro("gte_rtpt")
#undef gte_rtpt
#define gte_rtpt() __asm__ volatile("nop;nop;.word 0x4A280030")
#pragma push_macro("gte_stsxy01")
#undef gte_stsxy01
#define gte_stsxy01(r0, r1)                                                    \
    __asm__ volatile("swc2 $12, 0(%0);"                                        \
                     "swc2 $13, 0(%1)"                                         \
                     :                                                         \
                     : "r"(r0), "r"(r1)                                        \
                     : "memory")
void xm_arena_effect_draw_lines(u32 *ot) {
    SceneScratch *scratch = SCENE_SCRATCH;
    SceneLine *line = xm_arena_effect_lines;
    s32 cx = scratch->camera.vx;
    s32 cy = scratch->camera.vy;
    s32 cz = scratch->camera.vz;
    s32 i;
    s32 z;
    u32 prev;
    u32 addr;

    for (i = 0; i < xm_arena_effect_latched_line_count; i++, line++) {
        scratch->from = line->from;
        scratch->to = line->to;
        scratch->from.vx -= cx;
        scratch->from.vy -= cy;
        scratch->from.vz -= cz;
        scratch->to.vx -= cx;
        scratch->to.vy -= cy;
        scratch->to.vz -= cz;
        gte_ldv01(&scratch->from, &scratch->to);
        gte_rtpt();
        gte_stsxy01(&line->line.x0, &line->line.x1);
        gte_stsz2(&z);
        prev = ot[z >> 4];
        addr = (u32)line & 0xFFFFFF;
        ot[z >> 4] = addr;
        *(u32 *)addr = prev | 0x03000000;
    }
}
#pragma pop_macro("gte_ldv01")
#pragma pop_macro("gte_rtpt")
#pragma pop_macro("gte_stsxy01")
#endif
extern u8 D_80092708;
void func_8007E528(s32 arg0) {
    if (D_80092708 == 0 && arg0 == 0xA) {
        func_8008EB4C(0x24);
    }
    D_80092708 = arg0;
}
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_text_draw_banner (src/decomp/src/menu/arena_menu_screens.c:142). */
#ifndef XM_T_SPRT
#define XM_T_SPRT
typedef struct {
    u_long tag;
    u_char r0, g0, b0, code;
    short x0, y0;
    u_char u0, v0;
    u_short clut;
    short w, h;
} SPRT;
#endif
#ifndef XM_T_SceneSprite
#define XM_T_SceneSprite
typedef struct {
    DR_TPAGE tpage;
    SPRT sprite;
} SceneSprite;
#endif
#ifndef XM_S_arena_text_draw_banner
#define XM_S_arena_text_draw_banner
void xm_arena_text_draw_banner(void *ot) __asm__("func_8007E574");
#endif
#ifndef XM_S_arena_text_banner_timer
#define XM_S_arena_text_banner_timer
extern u8 xm_arena_text_banner_timer __asm__("D_80092708");
#endif
#ifndef XM_S_AddPrim
#define XM_S_AddPrim
void xm_AddPrim(void *ot, void *p) __asm__("AddPrim");
#endif
#ifndef XM_S_arena_text_banner_sprites
#define XM_S_arena_text_banner_sprites
extern SceneSprite xm_arena_text_banner_sprites[2] __asm__("D_800954D8");
#endif
void func_8007E574(void *ot) {
    if (xm_arena_text_banner_timer != 0) {
        if (xm_arena_frame_count & 2) {
            xm_AddPrim(ot, &xm_arena_text_banner_sprites[xm_arena_draw_buffer_index].sprite);
            xm_AddPrim(ot, &xm_arena_text_banner_sprites[xm_arena_draw_buffer_index].tpage);
        }
        xm_arena_text_banner_timer--;
    }
}
#endif

s32 func_8007E624(void) {
    return D_800926DC;
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007E634);

void func_8007E894(s16 arg0, s16 arg1) {
    D_800926E8 = arg0;
    D_800926EC = arg1;
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007E8AC);

void func_8007E954(s32 arg0) {
    D_800912DC = arg0;
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_text_draw_char (src/decomp/src/menu/arena_menu_screens.c:257). */
#ifndef XM_T_Glyph
#define XM_T_Glyph
typedef struct {
    u8 u, v;   
    u8 width;
    u8 height;
} Glyph;
#endif
#ifndef XM_T_P_TAG
#define XM_T_P_TAG
typedef struct {
    unsigned addr : 24;
    unsigned len : 8;
    u_char r0, g0, b0, code;
} P_TAG;
#endif
#ifndef XM_T_setlen
#define XM_T_setlen
#define setlen(p, _len) (((P_TAG *)(p))->len = (u_char)(_len))
#endif
#ifndef XM_S_arena_text_draw_char
#define XM_S_arena_text_draw_char
s32 xm_arena_text_draw_char(s32 ch) __asm__("func_8007E964");
#endif
#ifndef XM_S_arena_text_quad_count
#define XM_S_arena_text_quad_count
extern s32 xm_arena_text_quad_count __asm__("D_800926DC");
#endif
#ifndef XM_S_arena_text_quads
#define XM_S_arena_text_quads
extern POLY_FT4 *xm_arena_text_quads[2] __asm__("D_800926D4");
#endif
#ifndef XM_S_arena_text_find_glyph
#define XM_S_arena_text_find_glyph
Glyph *xm_arena_text_find_glyph(s32 ch) __asm__("func_8007E8AC");
#endif
#ifndef XM_S_arena_text_cursor_x
#define XM_S_arena_text_cursor_x
extern s16 xm_arena_text_cursor_x __asm__("D_800926E8");
#endif
#ifndef XM_S_arena_text_cursor_y
#define XM_S_arena_text_cursor_y
extern s16 xm_arena_text_cursor_y __asm__("D_800926EC");
#endif
#ifndef XM_S_arena_text_width_scale
#define XM_S_arena_text_width_scale
extern s32 xm_arena_text_width_scale __asm__("D_800912DC");
#endif
#ifndef XM_S_arena_text_tpage
#define XM_S_arena_text_tpage
extern u16 xm_arena_text_tpage __asm__("D_800926E0");
#endif
#ifndef XM_S_arena_text_clut
#define XM_S_arena_text_clut
extern u16 xm_arena_text_clut __asm__("D_800926E4");
#endif
#ifndef XM_S_arena_text_red
#define XM_S_arena_text_red
extern u8 xm_arena_text_red __asm__("D_800926F0");
#endif
#ifndef XM_S_arena_text_green
#define XM_S_arena_text_green
extern u8 xm_arena_text_green __asm__("D_800926F4");
#endif
#ifndef XM_S_arena_text_blue
#define XM_S_arena_text_blue
extern u8 xm_arena_text_blue __asm__("D_800926F8");
#endif
s32 func_8007E964(s32 ch) {
    POLY_FT4 *quad;
    Glyph *glyph;
    s32 right;

    if (xm_arena_text_quad_count < 101) {
        quad = xm_arena_text_quads[xm_arena_draw_buffer_index];
        quad += xm_arena_text_quad_count;
        glyph = xm_arena_text_find_glyph(ch);
        if (glyph != NULL) {
            if (ch != '(') {
                ch = glyph->width | 3; 
                *(u32 *)&quad->x0 = xm_arena_text_cursor_x | (xm_arena_text_cursor_y << 16);
                right = xm_arena_text_cursor_x + ((ch * xm_arena_text_width_scale) >> 8);
                *(u32 *)&quad->x1 = right | (xm_arena_text_cursor_y << 16);
                *(u32 *)&quad->x2 = xm_arena_text_cursor_x | ((xm_arena_text_cursor_y + glyph->height) << 16);
                *(u32 *)&quad->x3 = right | ((xm_arena_text_cursor_y + glyph->height) << 16);
                *(u16 *)&quad->u0 = glyph->u | (glyph->v << 8);
                *(u16 *)&quad->u1 = (glyph->u + ch) | (glyph->v << 8);
                *(u16 *)&quad->u2 = glyph->u | ((glyph->v + (glyph->height + 1)) << 8);
                *(u16 *)&quad->u3 = (glyph->u + ch) | ((glyph->v + (glyph->height + 1)) << 8);
                quad->tpage = xm_arena_text_tpage;
                quad->clut = xm_arena_text_clut;
                setlen(quad, 9);
                *(u32 *)&quad->r0 = xm_arena_text_red | (xm_arena_text_green << 8) | (xm_arena_text_blue << 16) | 0x2C000000;
                xm_arena_text_quad_count++;
            }
            xm_arena_text_cursor_x += ((glyph->width * xm_arena_text_width_scale) >> 8) + 2;
        }
    }
}
#endif
u8 *func_8007E8AC(u8);

s32 func_8007EB6C(u8 *arg0) {
    s32 width;
    s32 total;
    u8 c;

    c = *arg0;
    total = 0;
    while (c != 0) {
        arg0++;
        width = func_8007E8AC(c)[2] * D_800912DC;
        c = *arg0;
        total += (width >> 8) + 2;
    }
    return total;
}

void func_8007EBE0(u8 *arg0) {
    s32 savedX;
    u8 c;

    c = *arg0;
    savedX = D_800926E8;
    while (c != 0) {
        arg0++;
        func_8007E964(c);
        c = *arg0;
    }
    D_800926E8 = savedX;
    D_800926EC += 0x14;
}
s32 func_8007EB6C(u8 *);

void func_8007EC54(u8 *arg0) {
    s32 savedX;
    u8 c;

    savedX = D_800926E8;
    D_800926E8 -= func_8007EB6C(arg0) / 2;
    c = *arg0;
    while (c != 0) {
        arg0++;
        func_8007E964(c);
        c = *arg0;
    }
    D_800926E8 = savedX;
    D_800926EC += 0x14;
}

s32 func_8007EB6C(u8 *);

void func_8007ECF0(u8 *arg0) {
    s32 savedX;
    u8 c;

    savedX = D_800926E8;
    D_800926E8 -= func_8007EB6C(arg0);
    c = *arg0;
    while (c != 0) {
        arg0++;
        func_8007E964(c);
        c = *arg0;
    }
    D_800926E8 = savedX;
    D_800926EC += 0x14;
}

void func_8007ED84(u8 *arg0, s32 arg1) {
    s32 savedX;
    u8 unused[8]; /* retail frame has 8 bytes of locals no code touches */
    u8 c;

    savedX = D_800926E8;
    D_800926E8 = savedX - arg1;
    c = *arg0;
    while (c != 0) {
        arg0++;
        func_8007E964(c);
        c = *arg0;
    }
    D_800926E8 = savedX;
    D_800926EC += 0x14;
}
extern u8 D_800926F0;
extern u8 D_800926F4;
extern u8 D_800926F8;
void func_8007EE08(s32 arg0) {
    s32 unset = 0xFF;

    if (arg0 != 0) {
        D_800926F4 = 0xFF;
        D_800926F8 = 0;
        D_800926F0 = D_80059488 * 0x14;
    } else {
        D_800926F0 = unset;
        D_800926F4 = unset;
        D_800926F8 = unset;
    }
}
#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): setBattlingTextBlinkBlue (src/battling/bl_8007E528.c). */
#ifndef XM_S_setBattlingTextBlinkBlue
#define XM_S_setBattlingTextBlinkBlue
void xm_setBattlingTextBlinkBlue(s32 arg0) __asm__("func_8007EE68");
#endif
#ifndef XM_S_g_playTimeInVsync
#define XM_S_g_playTimeInVsync
extern volatile s32 xm_g_playTimeInVsync __asm__("D_80059488");
#endif
#ifndef XM_S_battlingTextB
#define XM_S_battlingTextB
extern u8 xm_battlingTextB __asm__("D_800926F8");
#endif
#ifndef XM_S_battlingTextR
#define XM_S_battlingTextR
extern u8 xm_battlingTextR __asm__("D_800926F0");
#endif
#ifndef XM_S_battlingTextG
#define XM_S_battlingTextG
extern u8 xm_battlingTextG __asm__("D_800926F4");
#endif
void func_8007EE68(s32 arg0) {
    s32 c;
    s32 t0;
    s32 t1;
    s32 r0;
    s32 r1;

    if (arg0 != 0) {
        t0 = xm_g_playTimeInVsync;
        t1 = xm_g_playTimeInVsync;
        xm_battlingTextB = 0xFF;
        r0 = 0xFF - t0 * 20; 
        xm_battlingTextR = r0;
        r1 = 0xFF - t1 * 20;
        xm_battlingTextG = r1;
    } else {
        c = 0xFF;
        xm_battlingTextR = c;
        xm_battlingTextG = c;
        xm_battlingTextB = c;
    }
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_select_build_list (src/decomp/src/menu/arena_menu_screens.c:379). */
#ifndef XM_T_CharacterEntry
#define XM_T_CharacterEntry
typedef struct {
    u16 field0; 
    u8 value2;
    u8 value3;  
    u8 value4;  
    u8 pad5;
    u8 id;      
    u8 pad7;
} CharacterEntry;
#endif
#ifndef XM_T_StatusPair
#define XM_T_StatusPair
typedef union {
    u32 word;
    struct {
        u16 active;
        u16 permanent;
    } half;
} StatusPair;
#endif
#ifndef XM_T_CharacterRecord
#define XM_T_CharacterRecord
typedef struct {
    CharacterEntry entries[4]; 
    u8 pad20[0x28 - 0x20];
    u8 equipAttack;       
    u8 equipDefense;      
    u8 equipSpeed;        
    u8 equipEther;        
    u8 equipEtherDefense; 
    u8 bodyDefense;       
    u8 equip5E;           
    u8 equip5F;           
    u8 hpBonus;           
    u8 epBonus;           
    u16 flags32;          
    u16 flags34;          
    u16 flags36;          
    u16 weakness;         
    u16 field3A;          
    u32 expTotalA;        
    u32 expTotalB;        
    u32 expNextA;         
    u32 expNextB;         
    u16 hp;               
    u16 maxHp;            
    u16 ep;               
    u16 maxEp;            
    u8 field54;           
    u8 field55;
    u8 characterId;       
    u8 pad57;
    u8 attack;            
    u8 defense;           
    u8 speed;             
    u8 ether;             

















    u8 etherDefense;      
    u8 field5D;
    u8 field5E;           
    u8 field5F;           
    u8 field60;           
    u8 field61;
    u8 level;             
    u8 level2;            
    u8 field64[4];        
    u8 pad68[0x6A - 0x68];
    u8 weapons[5];        
    u8 entryItems[5];     

    u8 accessories[3];    
    u8 field77;           

    u8 field78;           
    u8 field79;           
    u16 status7A;         

    u16 status7C;         


    u16 status7E;         
    u16 status80;         
    u16 status82;         
    StatusPair status84;  
    StatusPair status88;  
    StatusPair status8C;  
    u16 useCounts[7];     
    u8 pad9E[0xA0 - 0x9E];
    u8 gearId;            
    u8 fieldA1;           
    u8 padA2[0xA4 - 0xA2];
} CharacterRecord;
#endif
#ifndef XM_T_GearEntry
#define XM_T_GearEntry
typedef struct {
    u16 field0; 
    u8 valueE;  
    u8 value10; 
    u8 value11; 
    u8 id;      
    u8 pad6[2];
} GearEntry;
#endif
#ifndef XM_T_GearRecord
#define XM_T_GearRecord
typedef struct GearRecord {
    u8 pad0[2];
    u8 engine;            
    u8 field3;            
    u8 partItems[4];      

    u8 frame;             
    u8 parts[3];          
    u8 weapons[4];        
    GearEntry entries[4]; 
    u8 pad30[0x38 - 0x30];
    u16 fuel;             
    u16 maxFuel;          
    u8 attack;            
    u8 pad3D;
    u8 field3E;
    u8 attackScale;       
    u16 equipBodyDefense; 
    u16 equipArmor;       
    u16 equip68a;         
    u16 equip68b;         
    u16 field48;          
    u8 speedPenalty;      
    u8 pad4B;
    u8 equipGuard;        
    u8 equipHitBonus;     
    u8 equipSpeed;        
    u8 field4F;           
    u8 speedBonus[4];     
    u8 equipFrameFactor;  
    u8 pad55;
    u8 equipAttackScale;  
    u8 chargeRate;        
    u8 pad58[0x5C - 0x58];
    u8 fileVariant;       
    u8 spriteVariants[3]; 
    u32 hp;               
    u32 maxHp;            
    u16 field68;
    u16 field6A;
    u16 field6C;
    u16 field6E;          
    u16 bodyDefense;      
    u16 armor;            
    u8 field74;
    u8 field75;
    u8 pad76[0x7C - 0x76];
    u16 status7C;         
    u16 field7E;          
    u16 status80;
    u16 status82;         
    StatusPair status84;  
    u8 resistances[16];   
    u8 speed;             
    u8 defense;           
    u8 pad9A[0x9C - 0x9A];
    u8 guard;             
    u8 field9D;
    u8 frameFactor;       
    s8 hitBonus;          

    u8 padA0[0xA4 - 0xA0];
} GearRecord;
#endif
#ifndef XM_T_CharacterSkills
#define XM_T_CharacterSkills
typedef struct {
    u16 counterSkills; 
    u16 levelSkills;   
    u16 unlocksA;      
    u16 unlocksB;      
    u8 pad8[0x17 - 0x8];
    u8 tier;           
    u8 pad18[0x1A - 0x18];
    u16 flags1A;       


    u8 pad1C[0x20 - 0x1C];
} CharacterSkills;
#endif
#ifndef XM_T_WorldmapReturn
#define XM_T_WorldmapReturn
typedef struct {
    u16 x;
    u16 z;
    u16 heading;
    u16 unk5A;
    u16 unk5C;
    u16 unk5E;
    u16 unk60;           
    u16 unk62;
    u16 unk64;
    u16 vehicle_heading; 
    u16 flags;           
    s16 unk6A;
    u16 unk6C;
    u16 unk6E;
    u16 unk70;
    u16 unk72;
    u16 unk74;
    u16 unk76;           
} WorldmapReturn;
#endif
#ifndef XM_T_FlightSave
#define XM_T_FlightSave
typedef struct {
    u16 x_frac;
    s16 x;
    u16 z_frac;
    s16 z;
    u16 count; 
} FlightSave;
#endif
#ifndef XM_T_SystemOptions
#define XM_T_SystemOptions
typedef struct {
    u32 version : 4;   
    u32 option4 : 1;
    u32 option5 : 1;
    u32 option6 : 7;
    u32 option13 : 3;  
    u32 complete : 1;  
    u32 unused : 15;
} SystemOptions;
#endif
#ifndef XM_T_GameData
#define XM_T_GameData
typedef struct GameData {
    u8 names[31][0x14];           

    CharacterRecord characters[11]; 
    GearRecord gears[20];           
    u8 unk1648[0x16C0 - 0x1648];
    CharacterSkills skills[11];     
    WorldmapReturn worldmap;        
    u16 unk1844[3];                 
    u16 unk184A;                    
    FlightSave flight;              
    u16 unk1856;                    
    u8 unk1858[0x1924 - 0x1858];
    u32 gold;                       
    u8 unk1928[0x1930 - 0x1928];
    u16 vars[0x200];              

    u16 joined;                   
    u16 available;                
    u8 party[3];                  
    u8 unk1D37;
    u8 weaponCounts[100];         
    u8 weaponIds[100];            
    u8 accessoryCounts[200];      
    u8 accessoryIds[200];         
    u8 itemCounts[150];           
    u8 itemIds[150];              
    u8 gearPartCounts[100];       
    u8 gearPartIds[100];          
    u8 gearAccessoryCounts[150];  
    u8 gearAccessoryIds[150];     
    u8 unk22B0;
    u8 inGear[3];                 
    u8 unk22B4[2];
    u16 flags;                    

    













    u8 ammo[48];                  
    u8 gearAmmo[48];              
    u16 locked;                   
    u16 map;                      
    u16 entry[3];                 

    u16 flagWords[17];            
    u8 progress[8];               
    SystemOptions options;        
    u8 unk2350[0x2355 - 0x2350];
    u8 flags2355;                 
    u8 unk2356[0x2358 - 0x2356];
} GameData;
#endif
#ifndef XM_T_ListEntry
#define XM_T_ListEntry
typedef struct ListEntry {
    s32 id;
    char *model; 
    u8 *name;    
} ListEntry;
#endif
#ifndef XM_S_arena_select_build_list
#define XM_S_arena_select_build_list
void xm_arena_select_build_list(s32 filter) __asm__("func_8007EEE8");
#endif
#ifndef XM_S_game_data
#define XM_S_game_data
extern GameData xm_game_data __asm__("g_GameState");
#endif
#ifndef XM_S_arena_select_entries
#define XM_S_arena_select_entries
extern ListEntry * *xm_arena_select_entries __asm__("D_800928EC");
#endif
#ifndef XM_S_arena_actor_move_lists
#define XM_S_arena_actor_move_lists
extern MoveList *xm_arena_actor_move_lists __asm__("D_80092874");
#endif
#ifndef XM_S_arena_select_entry_count
#define XM_S_arena_select_entry_count
extern s32 xm_arena_select_entry_count __asm__("D_80092888");
#endif
#ifndef XM_S_arena_select_gears
#define XM_S_arena_select_gears
extern ListEntry xm_arena_select_gears[49] __asm__("D_80091964");
#endif
#ifndef XM_S_arena_progress_add_unlocked_entry
#define XM_S_arena_progress_add_unlocked_entry
void xm_arena_progress_add_unlocked_entry(void) __asm__("func_8008895C");
#endif
void func_8007EEE8(s32 filter) {
    s32 level = xm_game_data.vars[0];
    ListEntry **list = xm_heap_alloc(0xC4, 1);
    MoveList *source;
    s32 i;

    xm_arena_select_entries = list;
    source = xm_arena_actor_move_lists;
    xm_arena_select_entry_count = 0;
    for (i = 0; i < 49; source++, i++) {
        if (!filter || source->level <= level) {
            xm_arena_select_entries[xm_arena_select_entry_count++] = &xm_arena_select_gears[i];
        }
    }
    if (filter) {
        xm_arena_progress_add_unlocked_entry();
    }
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_select_alloc_portrait_slots (src/decomp/src/menu/arena_menu_screens.c:400). */
#ifndef XM_T_GridCell
#define XM_T_GridCell
typedef struct {
    RECT clut;
    RECT image;
    u8 unk10[4];
} GridCell;
#endif
#ifndef XM_S_arena_select_alloc_portrait_slots
#define XM_S_arena_select_alloc_portrait_slots
void xm_arena_select_alloc_portrait_slots(void) __asm__("func_8007EFB4");
#endif
#ifndef XM_S_arena_select_portrait_slots
#define XM_S_arena_select_portrait_slots
extern GridCell *xm_arena_select_portrait_slots __asm__("D_8009270C");
#endif
void func_8007EFB4(void) {
    u8 unused[0x30]; 
    GridCell *cell;
    s32 id;
    s32 row;
    s32 col;
    s16 top;

    cell = xm_arena_select_portrait_slots = xm_heap_alloc(0x3D4, 0);
    id = 0x1FF;
    for (row = 0; row < 7; row++) {
        top = row * 0x20 + 0x1A0;
        for (col = 0; col < 7; col++) {
            s16 left = col << 6;

            cell->clut.x = 0x200;
            cell->clut.y = id--;
            cell->clut.w = 0x80;
            cell->clut.h = 1;
            cell->image.x = top;
            cell->image.y = left;
            cell->image.w = 0x1E;
            cell->image.h = 0x40;
            cell++;
        }
    }
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_select_draw_portrait (src/decomp/src/menu/arena_menu_screens.c:432). */
#ifndef XM_T_PolyFT4Words
#define XM_T_PolyFT4Words
typedef struct PolyFT4Words {
    u8 addr[3];
    u8 len;
    u32 rgbc;
    u32 xy0;
    u16 uv0;
    u16 clut;
    u32 xy1;
    u16 uv1;
    u16 tpage;
    u32 xy2;
    u16 uv2;
    u16 pad1;
    u32 xy3;
    u16 uv3;
    u16 pad2;
} PolyFT4Words;
#endif
#ifndef XM_S_arena_select_draw_portrait
#define XM_S_arena_select_draw_portrait
void xm_arena_select_draw_portrait(s32 index, PolyFT4Words *quad, s32 right_side, s32 x, s32 fade) __asm__("func_8007F05C");
#endif
#ifndef XM_S_arena_current_ot
#define XM_S_arena_current_ot
extern u32 *xm_arena_current_ot __asm__("D_80092938");
#endif
void func_8007F05C(s32 index, PolyFT4Words *quad, s32 right_side, s32 x, s32 fade) {
    GridCell *cell;
    s32 inset;
    s32 top;
    s32 bottom;
    s32 left;
    s32 width;
    s32 height;
    s32 u;

    if (right_side) {
        x += 0xD3;
    } else {
        x += 0x33;
    }
    if (index > xm_arena_select_entry_count - 1) {
        index -= xm_arena_select_entry_count;
    }
    if (index < 0) {
        index += xm_arena_select_entry_count;
    }
    index = xm_arena_select_entries[index]->id;
    cell = &xm_arena_select_portrait_slots[index];
    if (fade == 0x40) {
        quad->len = 9;
        ((u8 *)&quad->rgbc)[3] = 0x2D;
        quad->xy0 = x | 0x300000;
        quad->xy1 = (x + 0x3C) | 0x300000;
        quad->xy2 = x | 0x700000;
        quad->xy3 = (x + 0x3C) | 0x700000;
    } else {
        fade += 0x40;
        quad->len = 9;
        quad->rgbc = fade | (fade << 8) | (fade << 16) | 0x2C000000;
        fade -= 0x40;
        fade >>= 4; 
        inset = fade - 4;
        left = x - inset;
        top = 0x34 - fade;
        quad->xy0 = left | (top << 16);
        width = 0x34;
        quad->xy1 = (left + width + fade * 2) | (top << 16);
        height = 0x38;
        bottom = top + height + fade * 2;
        quad->xy2 = left | (bottom << 16);
        quad->xy3 = (left + width + fade * 2) | (bottom << 16);
    }
    u = cell->image.x * 2;
    quad->uv0 = u | (cell->image.y << 8);
    quad->uv1 = (u + 0x3B) | (cell->image.y << 8);
    quad->uv2 = u | ((cell->image.y + 0x3F) << 8);
    quad->uv3 = (u + 0x3B) | ((cell->image.y + 0x3F) << 8);
    quad->clut = xm_GetClut(cell->clut.x, cell->clut.y);
    quad->tpage = xm_GetTPage(1, 0, cell->image.x & 0xFF80, cell->image.y);
    xm_AddPrim(xm_arena_current_ot, quad);
}
#endif
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007F258);

void func_8007F834(void) {
    D_80092740 = 0;
    D_8009273C = 0;
    D_80092744 = 0;
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_menu_resume_bout (src/decomp/src/menu/arena_menu_screens.c:590). */
#ifndef XM_T_Menu
#define XM_T_Menu
typedef struct Menu Menu;
#endif
#ifndef XM_T_ActorStanceBits
#define XM_T_ActorStanceBits
typedef struct {
    u32 stance : 2;
    u32 prev_stance : 2;
    u32 unk4 : 28;
} ActorStanceBits;
#endif
#ifndef XM_T_ACTOR_STANCE_BITS
#define XM_T_ACTOR_STANCE_BITS
#define ACTOR_STANCE_BITS(actor) ((ActorStanceBits *)&(actor)->unkD4)
#endif
#ifndef XM_S_arena_menu_resume_bout
#define XM_S_arena_menu_resume_bout
void xm_arena_menu_resume_bout() __asm__("func_8007F854");
#endif
#ifndef XM_S_arena_menu_copy_one_more_frame
#define XM_S_arena_menu_copy_one_more_frame
extern s32 xm_arena_menu_copy_one_more_frame __asm__("D_800912F0");
#endif
#ifndef XM_S_arena_mode_set_state
#define XM_S_arena_mode_set_state
void xm_arena_mode_set_state(s32 mode) __asm__("func_80083C0C");
#endif
#ifndef XM_S_arena_current_menu
#define XM_S_arena_current_menu
extern Menu *xm_arena_current_menu __asm__("D_80092734");
#endif
#ifndef XM_S_arena_menu_hide_captions
#define XM_S_arena_menu_hide_captions
void xm_arena_menu_hide_captions(void) __asm__("func_8007F834");
#endif
void func_8007F854(void) {
    s32 unused[2]; 

    xm_arena_menu_copy_one_more_frame = 1;
    xm_arena_mode_set_state(1);
    xm_arena_current_menu = NULL;
    xm_arena_menu_hide_captions();
    ACTOR_STANCE_BITS(&xm_arena_first_actor)->prev_stance = 3;
    ACTOR_STANCE_BITS(&xm_arena_second_actor)->prev_stance = 3;
}
#endif
void func_8007F8B4(void) {
    u8 unused[8]; /* retail frame has 8 bytes of locals no code touches */

    func_80083C0C(1);
    D_80092734 = 0;
    func_8007F834();
}


void func_8007F8E4(void) {
    s32 var_a0;

    func_80080C48(0);
    if ((D_80099D9F != 0) || (D_80092950 == 1)) {
        var_a0 = 3;
    } else {
        D_80092950 -= 1;
        var_a0 = 6;
    }
    func_80083C0C(var_a0);
}

void func_8007F948(void *arg0, s32 arg1) {
    if (*(s16 *)((s8*)(arg0) + 0x12) == arg1) {
        func_8007EE08(1);
    } else {
        func_8007EE08(0);
    }
}


#ifndef XENO_PC_PORT
s32 func_8007F97C(void) {
    return *(s32 *)((u8 *) &D_800912F4 + (D_80099D98__s_u8 * 4));
}
#endif

extern char D_8006FF5C[];
extern char D_8006FF60[];
extern char D_8006FF68[];
extern char D_8006FF6C[];
extern char D_8006FF74[];
extern u8 D_80091300[];
extern s32 D_8009130C[];
void func_8007E894__int(s32, s32) asm("func_8007E894");

void func_8007F9A0(void *arg0) {
    char buf[8];
    char *str;

    func_8007E894__int((*(s16 *)((s8*)(arg0) + 0x24) + *(s16 *)((s8*)(arg0) + 0x28)) - 0xA, *(s16 *)((s8*)(arg0) + 0x14));
    func_8007EE08(0);
    func_8007F948(arg0, 0);
    func_8007ECF0((u8 *)func_8007F97C());
    func_8007F948(arg0, 1);
    Sprintf(buf, D_8006FF5C, D_80099DA1__a[0] + 1);
    D_8009292C = D_8009130C[D_80099DA1__a[0]];
    D_80099DA4 = D_8009292C;
    func_8007ECF0((u8 *)buf);
    func_8007F948(arg0, 2);
    Sprintf(buf, D_8006FF60, D_80091300[D_80099D9A__a[0]]);
    func_8007ECF0((u8 *)buf);
    func_8007F948(arg0, 3);
    str = D_8006FF6C;
    if (D_80099D9D__a[0] != 0) {
        str = D_8006FF68;
    }
    func_8007ECF0((u8 *)str);
    func_8007F948(arg0, 4);
    str = D_8006FF74;
    if (D_80099D9E__a[0] != 0) {
        str = D_8006FF68;
    }
    func_8007ECF0((u8 *)str);
    func_8007EE08(0);
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007FB0C);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007FBEC);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007FE48);
extern s32 D_8009274C;
extern s32 D_80092750;
s32 func_8007FF70(s32 arg0, s32 arg1, s32 arg2) {
    s32 var_s0;
    s32 step;
    s32 sfx;
    s32 buttons;

    var_s0 = arg0;
    step = 1;
    if (arg2 & 4) {
        step = -1;
    }
    sfx = 0;
    if (arg2 & 2) {
        buttons = D_8009274C;
    } else {
        buttons = D_80092750;
    }
    if (buttons & 0x2000) {
        var_s0 += step;
    }
    if (buttons & 0x8000) {
        var_s0 -= step;
    }
    if (buttons & 0xA000) {
        sfx = 1;
    }
    if (arg2 & 1) {
        if (var_s0 == -1) {
            var_s0 = arg1;
        }
        if (arg1 < var_s0) {
            var_s0 = 0;
        }
    } else {
        if (var_s0 == -1) {
            sfx = 0;
            var_s0 = 0;
        }
        if (arg1 < var_s0) {
            sfx = 0;
            var_s0 = arg1;
        }
    }
    if (sfx != 0) {
        func_8008EB4C(0x20);
    }
    return var_s0;
}


void func_80080054(void) {
    D_80099D98[0] = func_8007FF70(D_80099D98[0], 2, 0);
}


void func_80080090(void) {
    D_80099DA1__a[0] = func_8007FF70(D_80099DA1__a[0], 7, 2);
}


void func_800800CC(void) {
    D_80099D9A__a[0] = func_8007FF70(D_80099D9A__a[0], 4, 2);
}


void func_80080108(void) {
    D_80099D9B[0] = func_8007FF70(D_80099D9B[0], 1, 1);
}


void func_80080144(void) {
    D_80099D9C[0] = func_8007FF70(D_80099D9C[0], 1, 1);
}


void func_80080180(void) {
    D_80099D9D__a[0] = func_8007FF70(D_80099D9D__a[0], 1, 1);
}


void func_800801BC(void) {
    D_80099D9E__a[0] = func_8007FF70(D_80099D9E__a[0], 1, 1);
}


void func_800801F8(void) {
    D_80099D9F__a[0] = func_8007FF70(D_80099D9F__a[0], 3, 2);
}


void func_80080234(void) {
    D_80092884 = func_8007FF70(D_80092884, 1, 1);
}


void func_80080268(void) {
    D_80099DA2__a[0] = func_8007FF70(D_80099DA2__a[0], 0xD, 3);
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_select_step_first_pick (src/decomp/src/menu/arena_menu_screens.c:862). */
#ifndef XM_S_arena_select_step_first_pick
#define XM_S_arena_select_step_first_pick
void xm_arena_select_step_first_pick() __asm__("func_800802A4");
#endif
#ifndef XM_S_arena_menu_applied_pad_port
#define XM_S_arena_menu_applied_pad_port
extern s32 xm_arena_menu_applied_pad_port __asm__("D_80091364");
#endif
#ifndef XM_S_arena_select_confirmed_sides
#define XM_S_arena_select_confirmed_sides
extern u32 xm_arena_select_confirmed_sides __asm__("D_80092710");
#endif
#ifndef XM_S_arena_actor_release_model
#define XM_S_arena_actor_release_model
void xm_arena_actor_release_model(s32 which) __asm__("func_80085134");
#endif
#ifndef XM_S_arena_sound_play_effect
#define XM_S_arena_sound_play_effect
void xm_arena_sound_play_effect(s32 id) __asm__("func_8008EB4C");
#endif
#ifndef XM_S_arena_select_first_pick
#define XM_S_arena_select_first_pick
extern s32 xm_arena_select_first_pick __asm__("D_80092700");
#endif
#ifndef XM_S_arena_select_second_pick
#define XM_S_arena_select_second_pick
extern s32 xm_arena_select_second_pick __asm__("D_80092704");
#endif
#ifndef XM_S_arena_menu_step_setting
#define XM_S_arena_menu_step_setting
s32 xm_arena_menu_step_setting(s32 value, s32 max, s32 flags) __asm__("func_8007FF70");
#endif
#ifndef XM_S_arena_menu_buttons_held
#define XM_S_arena_menu_buttons_held
extern s32 xm_arena_menu_buttons_held __asm__("D_80092748");
#endif
#ifndef XM_S_arena_actor_load_model
#define XM_S_arena_actor_load_model
void xm_arena_actor_load_model(s32 which, s32 id) __asm__("func_8008509C");
#endif
void func_800802A4(void) {
    s32 same;

    if (xm_arena_menu_applied_pad_port == 0 && (xm_pad_port0_pressed & 0x40)) {
        xm_arena_select_confirmed_sides &= ~1;
        xm_arena_actor_release_model(0);
        xm_arena_sound_play_effect(0x22);
    }
    if (!(xm_arena_select_confirmed_sides & 1)) {
        same = xm_arena_select_first_pick == xm_arena_select_second_pick;
        do {
            xm_arena_select_first_pick = xm_arena_menu_step_setting(xm_arena_select_first_pick, xm_arena_select_entry_count - 1, 3);
        } while (xm_arena_select_first_pick == xm_arena_select_second_pick && !(xm_arena_menu_buttons_held & 1) && !same);
        if (xm_arena_menu_applied_pad_port == 0 && (xm_pad_port0_pressed & 0x20)) {
            xm_arena_select_confirmed_sides |= 1;
            xm_arena_sound_play_effect(0x21);
            xm_arena_actor_load_model(0, xm_arena_select_entries[xm_arena_select_first_pick]->id);
        }
    }
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_select_step_second_pick (src/decomp/src/menu/arena_menu_screens.c:884). */
#ifndef XM_S_arena_select_step_second_pick
#define XM_S_arena_select_step_second_pick
void xm_arena_select_step_second_pick() __asm__("func_8008040C");
#endif
#ifndef XM_S_arena_menu_buttons_pressed
#define XM_S_arena_menu_buttons_pressed
extern u32 xm_arena_menu_buttons_pressed __asm__("D_80092750");
#endif
void func_8008040C(void) {
    s32 same;

    if (xm_arena_menu_buttons_pressed & 0x40) {
        if (xm_arena_play_mode != 2) {
            xm_arena_select_confirmed_sides = 0;
            xm_arena_actor_release_model(1);
            xm_arena_sound_play_effect(0x22);
            return;
        }
        xm_arena_select_confirmed_sides &= ~2;
    }
    if (!(xm_arena_select_confirmed_sides & 2)) {
        same = xm_arena_select_first_pick == xm_arena_select_second_pick;
        do {
            xm_arena_select_second_pick = xm_arena_menu_step_setting(xm_arena_select_second_pick, xm_arena_select_entry_count - 1, 3);
        } while (xm_arena_select_first_pick == xm_arena_select_second_pick && !(xm_arena_menu_buttons_held & 1) && !same);
        if (xm_arena_menu_buttons_pressed & 0x20) {
            xm_arena_select_confirmed_sides |= 2;
            xm_arena_sound_play_effect(0x21);
            xm_arena_actor_load_model(1, xm_arena_select_entries[xm_arena_select_second_pick]->id);
        }
    }
}
#endif
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80080570);
s32 ArchiveDecodeSector(s32);
s32 ArchiveReadFileFromCdSector(s32, void *, s32, s32, u32);
s32 LoadImage(void *, void *);
extern s32 D_80092700;
extern s32 D_80092704;
extern s32 D_8009270C;
extern s32 D_80092710;
extern s32 D_80092714;
extern s32 D_80092720;

void func_80080644(s32 arg0, s32 arg1) {
    u8 *buf;
    u8 *buf2;
    u8 *rect;

    buf = HeapAlloc(0x2000, 0);
    ArchiveReadFileFromCdSector(ArchiveDecodeSector(6) + (arg0 * 2), buf, 0x1000, 0, 0);
    buf2 = buf + 0x1000;
    ArchiveReadFileFromCdSector(ArchiveDecodeSector(6) + (arg1 * 2), buf2, 0x1000, 0, 0);
    D_80092700 = arg0;
    D_80092704 = arg1;
    D_80092714 = arg0;
    D_80092720 = arg1;
    D_80092710 = 3;
    ArchiveCdDataSync(0);
    rect = (u8 *)D_8009270C + (arg0 * 0x14);
    LoadImage(rect, buf);
    LoadImage(rect + 8, buf + 0x100);
    rect = (u8 *)D_8009270C + (arg1 * 0x14);
    LoadImage(rect, buf2);
    LoadImage(rect + 8, buf + 0x1100);
    HeapDelayedFree(buf, 2);
}

extern s8 D_80091369;
extern s8 D_80091391;
extern s16 D_800916E2;
extern s16 D_8009171E;
extern s32 D_80092718;
extern s32 D_8009271C;
extern s32 D_80092724;
extern s32 D_80092728;

void func_80080780(s32 arg0) {
    u8 *rect;
    s32 i;
    s32 off;

    if (D_80092940 == 0) {
        ArchiveCdDataSync(0);
        rect = (u8 *)D_8009270C;
        for (i = 0; i < 0x31; i++) {
            off = i << 12;
            LoadImage(rect, (void *)(D_800928D8 + off));
            LoadImage(rect + 8, (void *)(D_800928D8 + off + 0x100));
            rect += 0x14;
        }
        HeapFree((void *)D_800928D8);
        D_80092940 = 1;
    }
    D_800928C8 = arg0;
    D_800916E2 = (arg0 == 4) ? 3 : 4;
    D_8009171E = 5;
    if (arg0 == 3) {
        D_80091369 = 0x27;
        D_80091391 = 0x28;
    } else {
        D_80091369 = 0x25;
        D_80091391 = 0x26;
    }
    func_80083C0C(1);
    D_80092710 = 0;
    D_80092728 = 0;
    D_80092724 = 0;
    D_8009271C = 0;
    D_80092718 = 0;
    D_80092714 = D_80092700;
    D_80092720 = D_80092704;
    func_80080964(5);
}


void func_800808F4(void) {
    D_80092924 = 1;
    func_8007F834();
}


void func_80080920(void) {
    D_80092924 = 1;
    func_800719F0();
    func_8008509C(0, 0);
    func_8008509C(1, 1);
}

extern s32 D_80092738;
extern u8 D_800915AC[];
void func_80080964(s32 arg0) {
    s32 prev;

    if (arg0 == 0xFF) {
        D_80092734 = D_80092738;
        return;
    }
    prev = D_80092734;
    D_80092734 = (s32)&D_800915AC[arg0 * 0x3C];
    D_80092738 = prev;
}

s32 func_800809BC(void) {
    return D_80092734 == (s32) &D_80091660;
}


void func_800809D8(void) {
    func_80039FF8();
    D_80092734 = 0;
    func_80080964(3);
    D_80091672 = 0;
    D_800916AE = 0;
    D_800928C8 = 0;
    D_80092758 = 0;
    func_8007F834();
    D_80092924 = 0;
    func_80080AA0(0);
    D_80092940 = 0;
    D_800928D8 = func_800891C0(6);
}


void func_80080A58(void) {
    if (D_80092940 == 0) {
        ArchiveCdDataSync(0);
        HeapFree(D_800928D8);
        D_80092940 = 1;
    }
}


void func_80080AA0(s32 arg0) {
    if (arg0 != 0) {
        D_80092760 = 0;
    }
    if (D_80092760 != 0) {
        HeapFree(D_80092760);
        D_80092760 = 0;
    }
}

void func_8007313C(s32, s32);                    /* extern */

void func_80080AE8(void) {
    struct { s16 x,y,w,h; } rect;
    register s32 width asm("$3");

    if (D_80092760 != 0) {
        DrawSync(0);
        width = 0x140;
        rect.x = width;
        rect.y = 0x100;
        rect.w = width;
        rect.h = 0xDA;
        func_8007313C(D_80092760, D_80092760 + 0x21E80);
        LoadImage(&rect.x, D_80092760);
    }
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_menu_store_screen_copy (src/decomp/src/menu/arena_menu_screens.c:1090). */
#ifndef XM_S_arena_menu_store_screen_copy
#define XM_S_arena_menu_store_screen_copy
void xm_arena_menu_store_screen_copy(void) __asm__("func_80080B58");
#endif
#ifndef XM_S_arena_menu_screen_copy
#define XM_S_arena_menu_screen_copy
extern void *xm_arena_menu_screen_copy __asm__("D_80092760");
#endif
#ifndef XM_S_heap_set_quiet_failures
#define XM_S_heap_set_quiet_failures
s32 xm_heap_set_quiet_failures(s32 quiet) __asm__("HeapToggleErrorHandler");
#endif
#ifndef XM_S_DrawSync
#define XM_S_DrawSync
int xm_DrawSync(int mode) __asm__("DrawSync");
#endif
#ifndef XM_S_MoveImage
#define XM_S_MoveImage
int xm_MoveImage(RECT *rect, int x, int y) __asm__("MoveImage");
#endif
#ifndef XM_S_StoreImage
#define XM_S_StoreImage
int xm_StoreImage(RECT *rect, u_long *p) __asm__("StoreImage");
#endif
void func_80080B58(void) {
    RECT area;

    if (xm_arena_menu_screen_copy == NULL) {
        xm_heap_set_quiet_failures(1);
        xm_arena_menu_screen_copy = xm_heap_alloc(0x22100, 0);
        xm_heap_set_quiet_failures(0);
    }
    xm_DrawSync(0);
    area = xm_arena_display_buffers[(xm_arena_draw_buffer_index + 1) & 1].draw.clip;
    xm_MoveImage(&area, 0x140, 0x100);
    if (xm_arena_menu_screen_copy != NULL) {
        xm_StoreImage(&area, xm_arena_menu_screen_copy);
    }
    xm_DrawSync(0);
}
#endif

void func_80080C48(s32 arg0) {
    func_80039FF8();
    func_8008EB4C(0x1F);
    if (arg0 == 1) {
        D_80092734 = 0;
        func_80080964(0);
        D_800915BE = 0;
        D_80091636 = 1;
        goto block_4;
    }
    if (arg0 == 2) {
        D_80092734 = 0;
        func_80080964(7);
        D_80091762 = 1;
block_4:
        func_80083C0C(0);
        D_80092758 = 1;
        D_800926FC = 0;
        D_8009275C = 1;
        func_80080B58();
        return;
    }
    func_8007F8B4();
}


void func_80080D10(void) {
    D_800926DC = 0;
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80080D20);
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_menu_init_captions (src/decomp/src/menu/arena_menu_screens.c:1170). */
#ifndef XM_T_Caption
#define XM_T_Caption
typedef struct {
    u8 *image;         
    SPRT sprite[2];  
    s16 width;         
    s16 x;             
} Caption;
#endif
#ifndef XM_S_arena_menu_init_captions
#define XM_S_arena_menu_init_captions
void xm_arena_menu_init_captions(void) __asm__("func_80080F04");
#endif
#ifndef XM_S_arena_menu_captions
#define XM_S_arena_menu_captions
extern Caption xm_arena_menu_captions[2] __asm__("D_80095510");
#endif
#ifndef XM_S_SetSprt
#define XM_S_SetSprt
void xm_SetSprt(SPRT *p) __asm__("SetSprt");
#endif
#ifndef XM_S_SetShadeTex
#define XM_S_SetShadeTex
void xm_SetShadeTex(void *p, int tge) __asm__("SetShadeTex");
#endif
#ifndef XM_S_text_plane0_clut
#define XM_S_text_plane0_clut
extern u16 xm_text_plane0_clut __asm__("g_SystemPalette1");
#endif
#ifndef XM_S_text_plane1_clut
#define XM_S_text_plane1_clut
extern u16 xm_text_plane1_clut __asm__("g_SystemPalette2");
#endif
#ifndef XM_S_arena_menu_caption_tpages
#define XM_S_arena_menu_caption_tpages
extern DR_TPAGE xm_arena_menu_caption_tpages[2] __asm__("D_80095570");
#endif
void func_80080F04(void) {
    u8 *pixels = xm_heap_alloc(0x6B4, 0);

    xm_arena_menu_captions[0].image = xm_arena_menu_captions[1].image = pixels;
    *(u32 *)&xm_arena_menu_captions[0].sprite[0].x0 = 0xB40000;
    *(u16 *)&xm_arena_menu_captions[0].sprite[0].u0 = 0x3000;
    xm_SetSprt(&xm_arena_menu_captions[0].sprite[0]);
    xm_SetShadeTex(&xm_arena_menu_captions[0].sprite[0], 1);
    xm_arena_menu_captions[0].sprite[0].h = 0xD;
    xm_arena_menu_captions[0].sprite[0].clut = xm_text_plane0_clut;
    xm_arena_menu_captions[0].sprite[1] = xm_arena_menu_captions[0].sprite[0];
    *(u32 *)&xm_arena_menu_captions[1].sprite[0].x0 = 0xC30000;
    *(u16 *)&xm_arena_menu_captions[1].sprite[0].u0 = 0x3000;
    xm_SetSprt(&xm_arena_menu_captions[1].sprite[0]);
    xm_SetShadeTex(&xm_arena_menu_captions[1].sprite[0], 1);
    xm_arena_menu_captions[1].sprite[0].h = 0xD;
    xm_arena_menu_captions[1].sprite[0].clut = xm_text_plane1_clut;
    xm_arena_menu_captions[1].sprite[1] = xm_arena_menu_captions[1].sprite[0];
    xm_SetDrawTPage(&xm_arena_menu_caption_tpages[0], 0, 0, xm_GetTPage(0, 0, 0x140, 0x30));
    xm_arena_menu_caption_tpages[1] = xm_arena_menu_caption_tpages[0];
}
#endif
s32 SystemRenderStringEntry(s32, s32, s32, s32);

void func_80081094(void *arg0, s32 arg1, s32 arg2) {
    s32 temp_v0;

    temp_v0 = SystemRenderStringEntry(GetStringEntry(D_80092880[0], arg1), *(s32 *)((s8*)(arg0) + 0), 0x3F, arg2);
    *(s16 *)((s8*)(arg0) + 0x2C) = temp_v0;
    *(s16 *)((s8*)(arg0) + 0x2E) = (0x140 - temp_v0) / 2;
}

extern s32 D_80095510;
extern s32 D_80095540;

void func_80081100(s32 arg0, s32 arg1) {
    struct { s16 x,y,w,h; } rect;

    if (arg1 == 0) {
        if (arg0 != *(u8*)&D_8009273C) {
            D_8009273C = arg0;
            func_80081094(&D_80095510, arg0, 0);
            goto block_5;
        }
    } else if (arg0 != *(u8*)&D_80092740) {
        D_80092740 = arg0;
        func_80081094(&D_80095540, arg0, 1);
block_5:
        rect.x = 0x140;
        rect.y = 0x30;
        rect.w = 0x42;
        rect.h = 0xD;
        LoadImage(&rect, D_80095510);
    }
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_menu_draw_captions (src/decomp/src/menu/arena_menu_screens.c:1227). */
#ifndef XM_S_arena_menu_draw_captions
#define XM_S_arena_menu_draw_captions
void xm_arena_menu_draw_captions(void *ot) __asm__("func_800811AC");
#endif
#ifndef XM_S_arena_menu_upper_caption
#define XM_S_arena_menu_upper_caption
extern u8 xm_arena_menu_upper_caption __asm__("D_8009273C");
#endif
#ifndef XM_S_arena_menu_lower_caption
#define XM_S_arena_menu_lower_caption
extern u8 xm_arena_menu_lower_caption __asm__("D_80092740");
#endif
void func_800811AC(void *ot) {
    Caption *caption;

    if (xm_arena_menu_upper_caption != 0) {
        caption = &xm_arena_menu_captions[0];
        caption->sprite[xm_arena_draw_buffer_index].w = caption->width;
        caption->sprite[xm_arena_draw_buffer_index].x0 = caption->x;
        xm_AddPrim(ot, &caption->sprite[xm_arena_draw_buffer_index]);
    }
    if (xm_arena_menu_lower_caption != 0) {
        caption = &xm_arena_menu_captions[1];
        caption->sprite[xm_arena_draw_buffer_index].w = caption->width;
        caption->sprite[xm_arena_draw_buffer_index].x0 = caption->x;
        xm_AddPrim(ot, &caption->sprite[xm_arena_draw_buffer_index]);
    }
    if (xm_arena_menu_upper_caption | xm_arena_menu_lower_caption) {
        xm_AddPrim(ot, ((u8 (*)[8])xm_arena_menu_caption_tpages)[xm_arena_draw_buffer_index]);
    }
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_menu_layout_page (src/decomp/src/menu/arena_menu_screens.c:1248). */
#ifndef XM_T_xm_Menu
#define XM_T_xm_Menu
typedef struct xm_Menu xm_Menu;
#endif
#ifndef XM_T_xm_MenuItem
#define XM_T_xm_MenuItem
typedef struct {
    u8 flags;          

    u8 caption;        
    u8 unk2[2];
    u8 *text;          
    void (*handler)(); 
    s32 arg;           
    u8 unk10[2];
    s16 half_width;    
} xm_MenuItem;
#endif
#ifndef XM_T_Menu__def
#define XM_T_Menu__def
struct xm_Menu {
    u8 title_width;    
    u8 unk1[3];
    xm_MenuItem *items;   
    s16 count;         
    s16 parent;        
    void (*draw)(xm_Menu *menu); 
    u8 unk10[2];
    s16 cursor;        
    s16 y;             
    s16 x;             
    u8 unk18[4];
    TILE panel[2];  
};
#endif
#ifndef XM_S_arena_menu_layout_page
#define XM_S_arena_menu_layout_page
void xm_arena_menu_layout_page(xm_Menu *menu) __asm__("func_800812BC");
#endif
#ifndef XM_S_arena_text_measure_width
#define XM_S_arena_text_measure_width
s32 xm_arena_text_measure_width(u8 *text) __asm__("func_8007EB6C");
#endif
#ifndef XM_S_arena_text_set_highlight
#define XM_S_arena_text_set_highlight
void xm_arena_text_set_highlight(s32 highlight) __asm__("func_8007EE08");
#endif
void func_800812BC(xm_Menu *menu) {
    xm_MenuItem *item;
    TILE *panel;
    s32 i;
    s32 widest;

    widest = 0;
    for (i = 0; i < menu->count; i++) {
        item = &menu->items[i];
        item->half_width = xm_arena_text_measure_width(item->text) / 2;
        widest = (widest < item->half_width) ? item->half_width : widest;
    }
    panel = &menu->panel[0];
    setlen(panel, 3);
    panel->w = widest * 2 + 0x14;
    panel->x0 = 0x96 - widest;
    menu->cursor = 0;
    *(u32 *)&panel->r0 = 0x60102020;
    menu->y = 0x6D - menu->count * 10;
    panel->code |= 2;
    panel->h = menu->count * 20 + 0x14;
    panel->y0 = menu->y - 10;
    for (i = 0; i < menu->count; i++) {
        item = &menu->items[i];
        if (item->flags & 2) {
            item->half_width = widest;
        }
    }
    if (menu->title_width != 0) {
        panel->w += menu->title_width;
        panel->x0 -= menu->title_width >> 1;
        menu->x = 0xA0 - (menu->title_width >> 1) - widest;
    }
    menu->panel[1] = menu->panel[0];
    xm_arena_text_set_highlight(0);
}
#endif
extern s32 D_80092700;
extern s32 D_80092704;
void func_800814AC(void) {
    u8 *entry;
    u32 i;

    i = 0;
    entry = D_800915AC;
    do {
        func_800812BC(entry);
        i++;
        entry += 0x3C;
    } while (i < 8);
    D_80092734 = 0;
    D_80092700 = 0;
    D_80092704 = 1;
    func_80080F04();
}
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_menu_draw_page (src/decomp/src/menu/arena_menu_screens.c:1299). */
#ifndef XM_S_arena_menu_draw_page
#define XM_S_arena_menu_draw_page
void xm_arena_menu_draw_page(xm_Menu *menu) __asm__("func_8008151C");
#endif
#ifndef XM_S_arena_menu_pages
#define XM_S_arena_menu_pages
extern xm_Menu xm_arena_menu_pages[8] __asm__("D_800915AC");
#endif
#ifndef XM_S_arena_text_move_cursor
#define XM_S_arena_text_move_cursor
void xm_arena_text_move_cursor(s32 x, s32 y) __asm__("func_8007E894");
#endif
#ifndef XM_S_arena_menu_highlight_entry
#define XM_S_arena_menu_highlight_entry
void xm_arena_menu_highlight_entry(xm_Menu *page, s32 entry) __asm__("func_8007F948");
#endif
#ifndef XM_S_arena_text_draw_line
#define XM_S_arena_text_draw_line
void xm_arena_text_draw_line(u8 *text) __asm__("func_8007EBE0");
#endif
#ifndef XM_S_arena_text_draw_line_shifted_left
#define XM_S_arena_text_draw_line_shifted_left
void xm_arena_text_draw_line_shifted_left(u8 *text, s32 offset) __asm__("func_8007ED84");
#endif
void xm_arena_menu_draw_page(xm_Menu *menu) __asm__("func_8008151C");
void xm_arena_menu_draw_page(xm_Menu *menu) {
    s32 i;

    if (menu == NULL) {
        return;
    }
    xm_arena_current_menu = menu;
    if (menu == &xm_arena_menu_pages[5]) {
        return;
    }
    if (menu->title_width != 0) {
        xm_arena_text_move_cursor(menu->x, menu->y);
    } else {
        xm_arena_text_move_cursor(0xA0, menu->y);
    }
    for (i = 0; i < menu->count; i++) {
        xm_arena_menu_highlight_entry(menu, i);
        if (menu->title_width != 0) {
            xm_arena_text_draw_line(menu->items[i].text);
        } else {
            xm_arena_text_draw_line_shifted_left(menu->items[i].text, menu->items[i].half_width);
        }
    }
    if (menu->draw != NULL) {
        menu->draw(menu);
    }
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_menu_apply_pad_input (src/decomp/src/menu/arena_menu_screens.c:1330). */
#ifndef XM_S_arena_menu_apply_pad_input
#define XM_S_arena_menu_apply_pad_input
s32 xm_arena_menu_apply_pad_input(xm_Menu *menu, s32 port) __asm__("func_8008162C");
#endif
#ifndef XM_S_arena_menu_show_caption
#define XM_S_arena_menu_show_caption
void xm_arena_menu_show_caption(s32 text, s32 lower) __asm__("func_80081100");
#endif
#ifndef XM_S_arena_menu_selected_caption
#define XM_S_arena_menu_selected_caption
extern s32 xm_arena_menu_selected_caption __asm__("D_80092744");
#endif
#ifndef XM_S_pad_port1_held
#define XM_S_pad_port1_held
extern u16 xm_pad_port1_held __asm__("g_C2ButtonState");
#endif
#ifndef XM_S_arena_menu_buttons_repeated
#define XM_S_arena_menu_buttons_repeated
extern u32 xm_arena_menu_buttons_repeated __asm__("D_8009274C");
#endif
#ifndef XM_S_pad_port1_repeated
#define XM_S_pad_port1_repeated
extern u16 xm_pad_port1_repeated __asm__("g_C2ButtonStatePressedOnce");
#endif
#ifndef XM_S_pad_port1_left_stick_y
#define XM_S_pad_port1_left_stick_y
extern u8 xm_pad_port1_left_stick_y __asm__("g_C2LeftStickYAxis");
#endif
#ifndef XM_S_pad_port1_left_stick_x
#define XM_S_pad_port1_left_stick_x
extern u8 xm_pad_port1_left_stick_x __asm__("g_C2LeftStickXAxis");
#endif
#ifndef XM_S_pad_port0_held
#define XM_S_pad_port0_held
extern u16 xm_pad_port0_held __asm__("g_C1ButtonState");
#endif
#ifndef XM_S_pad_port0_repeated
#define XM_S_pad_port0_repeated
extern u16 xm_pad_port0_repeated __asm__("g_C1ButtonStatePressedOnce");
#endif
#ifndef XM_S_pad_port0_left_stick_y
#define XM_S_pad_port0_left_stick_y
extern u8 xm_pad_port0_left_stick_y __asm__("g_C1LeftStickYAxis");
#endif
#ifndef XM_S_pad_port0_left_stick_x
#define XM_S_pad_port0_left_stick_x
extern u8 xm_pad_port0_left_stick_x __asm__("g_C1LeftStickXAxis");
#endif
#ifndef XM_S_arena_menu_stick_deflected
#define XM_S_arena_menu_stick_deflected
extern u8 xm_arena_menu_stick_deflected __asm__("D_80092764");
#endif
#ifndef XM_S_arena_menu_show_page
#define XM_S_arena_menu_show_page
void xm_arena_menu_show_page(s32 page) __asm__("func_80080964");
#endif
s32 xm_arena_menu_apply_pad_input(xm_Menu *menu, s32 port) __asm__("func_8008162C");
s32 xm_arena_menu_apply_pad_input(xm_Menu *menu, s32 port) {
    xm_MenuItem *item;
    void (*handler)();
    s32 type;
    s32 x;
    s32 y;

    item = &menu->items[menu->cursor];
    if (port == 0 || menu == &xm_arena_menu_pages[7]) {
        xm_arena_menu_show_caption(xm_arena_menu_selected_caption, 0);
        xm_arena_menu_selected_caption = menu->items[menu->cursor].caption;
    }
    xm_arena_menu_applied_pad_port = port;
    type = 0;
    if (port == 1) {
        xm_arena_menu_buttons_held = xm_pad_port1_held;
        xm_arena_menu_buttons_repeated = xm_pad_port1_repeated;
        xm_arena_menu_buttons_pressed = xm_pad_port1_pressed;
        type = xm_pad_get_controller_kind(1);
        x = xm_pad_port1_left_stick_y - 0x80;
        y = xm_pad_port1_left_stick_x - 0x80;
    } else if (port == 0) {
        xm_arena_menu_buttons_held = xm_pad_port0_held;
        xm_arena_menu_buttons_repeated = xm_pad_port0_repeated;
        xm_arena_menu_buttons_pressed = xm_pad_port0_pressed;
        type = xm_pad_get_controller_kind(0);
        x = xm_pad_port0_left_stick_y - 0x80;
        y = xm_pad_port0_left_stick_x - 0x80;
    }
    if (type == 3 || type == 4) {
        if (xm_SquareRoot0(x * x + y * y) > 0x40) {
            if (xm_arena_menu_stick_deflected == 0) {
                xm_arena_sound_play_effect(0x24);
                xm_arena_menu_stick_deflected = 1;
            }
            goto stick_done;
        }
    }
    xm_arena_menu_stick_deflected = 0;
stick_done:
    handler = item->handler;
    if (handler != NULL) {
        if (item->flags & 1) {
            handler(item->arg);
        } else if (xm_arena_menu_buttons_pressed & 0x20) {
            xm_arena_sound_play_effect(0x21);
            handler(item->arg);
        }
    }
    if (xm_arena_current_menu != NULL) {
        if ((xm_arena_menu_buttons_pressed & 0x40) && !(port == 1 && menu == &xm_arena_menu_pages[6])) {
            if (xm_arena_current_menu == &xm_arena_menu_pages[menu->parent] && menu->parent != 5 && menu->parent != 6) {
                xm_arena_sound_play_effect(0x24);
            } else {
                xm_arena_menu_show_page(menu->parent);
                xm_arena_sound_play_effect(0x22);
            }
        }
        if (xm_arena_menu_buttons_repeated & 0x1000) {
            xm_arena_sound_play_effect(0x1E);
            if (--menu->cursor < 0) {
                menu->cursor = menu->count - 1;
            }
            if (menu->items[menu->cursor].flags & 4) {
                menu->cursor--;
            }
        }
        if (xm_arena_menu_buttons_repeated & 0x4000) {
            xm_arena_sound_play_effect(0x1E);
            menu->cursor++;
            if (menu->items[menu->cursor].flags & 4) {
                menu->cursor++;
            }
        }
        if (menu->cursor < 0) {
            menu->cursor = menu->count - 1;
        }
        if (menu->cursor >= menu->count) {
            menu->cursor = 0;
        }
        if (menu->items[menu->cursor].flags & 4) {
            menu->cursor--;
        }
    }
}
#endif
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80081A44);
void func_80080AE8(void);
void func_80081A44(void);
void func_8008151C(s32);
void func_8008162C(s32, s32);
void func_80036420();
extern s32 D_800916D8;
extern u16 g_C1ButtonStateReleased;

void func_80081D2C(void) {
    s32 temp_s0;
    u8 unused[8]; /* retail frame has 8 bytes of locals no code touches */

    if (*(u8 *)&D_8009275C != 0) {
        func_80080AE8();
        D_8009275C = 0;
    }
    func_80036420();
    temp_s0 = D_80092734;
    if (temp_s0 == (s32) &D_800916D8) {
        func_80081A44();
        return;
    }
    if (temp_s0 != 0) {
        if ((g_C1ButtonStateReleased & 1) && ((u8) D_800926FC < 5U)) {
            D_800926FC += 1;
            func_80080AE8();
        }
        func_8008162C(temp_s0, *(u8 *)&D_800928FC);
    }
    func_8008151C(D_80092734);
}


void func_80081E00(void) {
    s16 temp_v1;

    D_8009A1CA = 0x60;
    D_8009A2C2 = 0x60;
    D_8009A1C4 = 0x7F;
    D_8009A1C5 = 0x7F;
    D_8009A1C6 = 0x7F;
    D_8009A2BC = 0x7F;
    D_8009A2BD = 0x7F;
    D_8009A2BE = 0x7F;
    temp_v1 = D_8009286C - 0x60;
    D_8009A1CE = temp_v1;
    D_8009A2C6 = temp_v1;
}


void func_80081E6C(void) {
    D_8009A1CA = 0;
    D_8009A2C2 = 0;
    D_8009A1C4 = 0;
    D_8009A1C5 = 0;
    D_8009A1C6 = 0;
    D_8009A2BC = 0;
    D_8009A2BD = 0;
    D_8009A2BE = 0;
    (*(u16*)&D_8009A1CE) = D_8009286C;
    (*(u16*)&D_8009A2C6) = D_8009286C;
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_stage_init (src/decomp/src/menu/arena_stage_views_and_hud.c:153). */
#ifndef XM_T_POLY_G4
#define XM_T_POLY_G4
typedef struct {
    u_long tag;
    u_char r0, g0, b0, code;
    short x0, y0;
    u_char r1, g1, b1, pad1;
    short x1, y1;
    u_char r2, g2, b2, pad2;
    short x2, y2;
    u_char r3, g3, b3, pad3;
    short x3, y3;
} POLY_G4;
#endif
#ifndef XM_S_arena_stage_init
#define XM_S_arena_stage_init
void xm_arena_stage_init(void) __asm__("func_80081ECC");
#endif
#ifndef XM_S_arena_hud_build_overlay_buffers
#define XM_S_arena_hud_build_overlay_buffers
void xm_arena_hud_build_overlay_buffers(void) __asm__("func_800875EC");
#endif
#ifndef XM_S_arena_stage_sky_gradients
#define XM_S_arena_stage_sky_gradients
extern POLY_G4 xm_arena_stage_sky_gradients[2] __asm__("D_80095580");
#endif
#ifndef XM_S_arena_stage_backdrop_tpages
#define XM_S_arena_stage_backdrop_tpages
extern DR_TPAGE xm_arena_stage_backdrop_tpages[4] __asm__("D_800955C8");
#endif
#ifndef XM_S_arena_stage_backdrop_sprites
#define XM_S_arena_stage_backdrop_sprites
extern SPRT xm_arena_stage_backdrop_sprites[6] __asm__("D_800955F8");
#endif
#ifndef XM_S_arena_stage_init_ground_pools
#define XM_S_arena_stage_init_ground_pools
void xm_arena_stage_init_ground_pools(void) __asm__("func_80087830");
#endif
void func_80081ECC(void) {
    POLY_G4 *sky;
    s16 *height;
    s32 i;

    xm_arena_hud_build_overlay_buffers();
    sky = &xm_arena_stage_sky_gradients[0];
    setlen(sky, 8);
    sky->code = 0x38;
    sky->r0 = 0x10;
    sky->g0 = 0x60;
    sky->b0 = 0x7F;
    *(u16 *)&sky->r1 = 0x6010;
    sky->b1 = 0x7F;
    *(u16 *)&sky->r2 = 0x7F7F;
    sky->b2 = 0x7F;
    *(u16 *)&sky->r3 = 0x7F7F;
    sky->b3 = 0x7F;
    *(u32 *)&sky->x0 = 0;
    *(u32 *)&sky->x1 = 0x140;
    *(u32 *)&sky->x2 = 0x600000;
    *(u32 *)&sky->x3 = 0x600140;
    xm_arena_stage_sky_gradients[1] = xm_arena_stage_sky_gradients[0];
    xm_SetDrawTPage(&xm_arena_stage_backdrop_tpages[0], 0, 0, xm_GetTPage(2, 2, 0, 0x100));
    xm_SetDrawTPage(&xm_arena_stage_backdrop_tpages[1], 0, 0, xm_GetTPage(2, 2, 0, 0));
    xm_SetDrawTPage(&xm_arena_stage_backdrop_tpages[2], 0, 0, xm_GetTPage(2, 2, 0x100, 0x100));
    xm_SetDrawTPage(&xm_arena_stage_backdrop_tpages[3], 0, 0, xm_GetTPage(2, 2, 0x100, 0));
    setlen(&xm_arena_stage_backdrop_sprites[0], 4);
    *(u32 *)&xm_arena_stage_backdrop_sprites[0].r0 = 0x64707070;
    xm_arena_stage_backdrop_sprites[0].code &= ~1; 
    xm_arena_stage_backdrop_sprites[0].code |= 2;  
    *(u32 *)&xm_arena_stage_backdrop_sprites[0].x0 = 0;
    *(u16 *)&xm_arena_stage_backdrop_sprites[0].u0 = 0;
    *(u32 *)&xm_arena_stage_backdrop_sprites[0].w = 0xDB0080;
    xm_arena_copy_words(&xm_arena_stage_backdrop_sprites[1], &xm_arena_stage_backdrop_sprites[0], sizeof(SPRT) * 5);
    xm_arena_stage_backdrop_sprites[3].u0 = 0x80;
    xm_arena_stage_backdrop_sprites[2].u0 = 0x80;
    xm_arena_stage_backdrop_sprites[3].x0 = 0x80;
    xm_arena_stage_backdrop_sprites[2].x0 = 0x80;
    xm_arena_stage_backdrop_sprites[5].x0 = 0x100;
    xm_arena_stage_backdrop_sprites[4].x0 = 0x100;
    xm_arena_stage_backdrop_sprites[5].w = 0x40;
    xm_arena_stage_backdrop_sprites[4].w = 0x40;
    height = (s16 *)xm_arena_stage_height_map;
    for (i = 0; i < 0x4000; i++) {
        *height *= 12;
        height += 2;
    }
    xm_arena_stage_init_ground_pools();
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_stage_mark_wide_view_cells (src/decomp/src/menu/arena_stage_views_and_hud.c:207). */
#ifndef XM_S_arena_stage_mark_wide_view_cells
#define XM_S_arena_stage_mark_wide_view_cells
void xm_arena_stage_mark_wide_view_cells(s32 x, s32 z, s32 direction) __asm__("func_80082178");
#endif
#ifndef XM_S_arena_stage_widen_row_spans
#define XM_S_arena_stage_widen_row_spans
void xm_arena_stage_widen_row_spans(s32 x0, s32 y0, s32 x1, s32 y1) __asm__("func_80087698");
#endif
void func_80082178(s32 x, s32 z, s32 direction) {
    s32 start_x;
    s32 start_z;
    s32 last_x;
    s32 last_z;
    s32 next_x;
    s32 next_z;
    s32 angle;
    s32 i;

    x >>= 8;
    z >>= 8;
    last_x = start_x = x + ((xm_gpu_get_sin(direction + 0x280) * 10) >> 12);
    last_z = start_z = z + ((xm_gpu_get_cos(direction + 0x280) * 10) >> 12);
    angle = direction + 0x580;
    for (i = 0; i < 6; i++) {
        next_x = x + ((xm_gpu_get_sin(angle) * 24) >> 12);
        next_z = z + ((xm_gpu_get_cos(angle) * 24) >> 12);
        xm_arena_stage_widen_row_spans(last_x, last_z, next_x, next_z);
        last_x = next_x;
        last_z = next_z;
        angle += 0x100;
    }
    next_x = x + ((xm_gpu_get_sin(direction - 0x280) * 10) >> 12);
    next_z = z + ((xm_gpu_get_cos(direction - 0x280) * 10) >> 12);
    xm_arena_stage_widen_row_spans(last_x, last_z, next_x, next_z);
    xm_arena_stage_widen_row_spans(start_x, start_z, next_x, next_z);
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_stage_mark_narrow_view_cells (src/decomp/src/menu/arena_stage_views_and_hud.c:238). */
#ifndef XM_S_arena_stage_mark_narrow_view_cells
#define XM_S_arena_stage_mark_narrow_view_cells
void xm_arena_stage_mark_narrow_view_cells(s32 x, s32 z, s32 direction) __asm__("func_80082300");
#endif
void func_80082300(s32 x, s32 z, s32 direction) {
    s32 start_x;
    s32 start_z;
    s32 last_x;
    s32 last_z;
    s32 next_x;
    s32 next_z;
    s32 angle;
    s32 i;

    x >>= 8;
    z >>= 8;
    last_x = start_x = x + ((xm_gpu_get_sin(direction + 0x100) * 16) >> 12);
    last_z = start_z = z + ((xm_gpu_get_cos(direction + 0x100) * 16) >> 12);
    angle = direction + 0x78A;
    for (i = 0; i < 3; i++) {
        next_x = x + ((xm_gpu_get_sin(angle) * 32) >> 12);
        next_z = z + ((xm_gpu_get_cos(angle) * 32) >> 12);
        xm_arena_stage_widen_row_spans(last_x, last_z, next_x, next_z);
        last_x = next_x;
        last_z = next_z;
        angle += 0x75;
    }
    next_x = x + ((xm_gpu_get_sin(direction - 0x100) * 16) >> 12);
    next_z = z + ((xm_gpu_get_cos(direction - 0x100) * 16) >> 12);
    xm_arena_stage_widen_row_spans(last_x, last_z, next_x, next_z);
    xm_arena_stage_widen_row_spans(start_x, start_z, next_x, next_z);
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_stage_get_ground_normal (src/decomp/src/menu/arena_stage_views_and_hud.c:268). */
#ifndef XM_S_arena_stage_get_ground_normal
#define XM_S_arena_stage_get_ground_normal
void xm_arena_stage_get_ground_normal(SVECTOR *out) __asm__("func_80082458");
#endif
#ifndef XM_S_arena_stage_ground_normal
#define XM_S_arena_stage_ground_normal
extern SVECTOR xm_arena_stage_ground_normal __asm__("D_80092768");
#endif
void func_80082458(SVECTOR *out) {
    *out = xm_arena_stage_ground_normal;
}
#endif
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80082488);
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_stage_get_cell_height (src/decomp/src/menu/arena_stage_views_and_hud.c:358). */
#ifndef XM_S_arena_stage_get_cell_height
#define XM_S_arena_stage_get_cell_height
s32 xm_arena_stage_get_cell_height(SVECTOR *pos) __asm__("func_80082880");
#endif
s32 func_80082880(SVECTOR *pos) {
    VECTOR unused[3]; 
    s16 x, z;

    x = pos->vx >> 8;
    z = pos->vz >> 8;
    return *(s16 *)&((s32 *)xm_arena_stage_height_map)[x + z * 128];
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_stage_get_ground_square (src/decomp/src/menu/arena_stage_views_and_hud.c:368). */
s32 func_800828C4(VECTOR *pos) {
    s32 x = pos->vx >> 8;
    s32 z = pos->vz >> 8;

    return ((s32 *)xm_arena_stage_height_map)[z * 128 + x];
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_stage_keep_step_inside (src/decomp/src/menu/arena_stage_views_and_hud.c:378). */
#ifndef XM_S_VectorNormalS
#define XM_S_VectorNormalS
long xm_VectorNormalS(VECTOR *v0, SVECTOR *v1) __asm__("VectorNormalS");
#endif
#ifndef XM_S_ApplyMatrixLV
#define XM_S_ApplyMatrixLV
VECTOR *xm_ApplyMatrixLV(MATRIX *m, VECTOR *v0, VECTOR *v1) __asm__("ApplyMatrixLV");
#endif
#ifndef XM_S_SetRotMatrix
#define XM_S_SetRotMatrix
void xm_SetRotMatrix(MATRIX *m) __asm__("SetRotMatrix");
#endif
#ifndef XM_S_libgte_rotate_vector
#define XM_S_libgte_rotate_vector
void xm_libgte_rotate_vector(VECTOR *v0, VECTOR *v1) __asm__("ApplyRotMatrixLV");
#endif
void xm_arena_stage_keep_step_inside(VECTOR *pos, VECTOR *step, s32 radius) __asm__("func_800828F8");
void xm_arena_stage_keep_step_inside(VECTOR *pos, VECTOR *step, s32 radius) {
    VECTOR local;
    VECTOR next;
    VECTOR square;
    MATRIX rim;
    MATRIX back;
    SVECTOR dir;
    s32 distance;

    local.vx = pos->vx + step->vx - 0x3F80;
    local.vz = pos->vz + step->vz - 0x3F80;
    xm_Square0(&local, &square);
    if (radius < xm_SquareRoot0(square.vx + square.vz)) {
        xm_VectorNormalS(&local, &dir);
        rim.m[2][1] = 0;
        rim.m[1][2] = 0;
        rim.m[1][0] = 0;
        rim.m[0][1] = 0;
        rim.m[1][1] = 0x1000;
        rim.m[2][2] = dir.vz;
        rim.m[0][0] = dir.vz;
        rim.m[0][2] = -dir.vx;
        rim.m[2][0] = dir.vx;
        xm_ApplyMatrixLV(&rim, step, &local);
        xm_libgte_transpose_matrix(&rim, &back);
        xm_SetRotMatrix(&back);
        local.vz = 0;
        for (;;) {
            xm_libgte_rotate_vector(&local, step);
            next.vx = pos->vx + step->vx - 0x3F80;
            next.vz = pos->vz + step->vz - 0x3F80;
            xm_Square0(&next, &square);
            distance = xm_SquareRoot0(square.vx + square.vz);
            if (radius >= distance) {
                break;
            }
            local.vz -= distance - radius - 8;
        }
    }
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_stage_apply_colors (src/decomp/src/menu/arena_stage_views_and_hud.c:421). */
#ifndef XM_T_Environment
#define XM_T_Environment
typedef struct Environment {
    u8 top[3];         
    u8 unk3;
    u8 unk4, unk5, unk6;
    u8 unk7;
    u8 bottom[3];      
    u8 unkB;
    u8 back[3];        
    u8 unkF;
    u8 dim;            
} Environment;
#endif
#ifndef XM_T_gte_ldrgb
#define XM_T_gte_ldrgb
#define gte_ldrgb(r0) __asm__ volatile("lwc2 $6, 0(%0)" : : "r"(r0) : "memory")
#endif
#ifndef XM_S_arena_stage_apply_colors
#define XM_S_arena_stage_apply_colors
void xm_arena_stage_apply_colors(void) __asm__("func_80082A70");
#endif
#ifndef XM_S_arena_stage_color_table
#define XM_S_arena_stage_color_table
extern Environment xm_arena_stage_color_table[] __asm__("D_8009178C");
#endif
#ifndef XM_S_arena_stage_index
#define XM_S_arena_stage_index
extern u8 xm_arena_stage_index __asm__("D_800928B4");
#endif
#ifndef XM_S_arena_current_stage_colors
#define XM_S_arena_current_stage_colors
extern Environment *xm_arena_current_stage_colors __asm__("D_8009288C");
#endif
#ifndef XM_S_arena_stage_back_color_red
#define XM_S_arena_stage_back_color_red
extern s32 xm_arena_stage_back_color_red __asm__("D_8009291C");
#endif
#ifndef XM_S_arena_stage_back_color_green
#define XM_S_arena_stage_back_color_green
extern s32 xm_arena_stage_back_color_green __asm__("D_80092910");
#endif
#ifndef XM_S_arena_stage_back_color_blue
#define XM_S_arena_stage_back_color_blue
extern s32 xm_arena_stage_back_color_blue __asm__("D_80092908");
#endif
#ifndef XM_S_model_set_color
#define XM_S_model_set_color
void xm_model_set_color(s32 r, s32 g, s32 b) __asm__("func_8002C6E0");
#endif
#ifndef XM_S_SetFarColor
#define XM_S_SetFarColor
void xm_SetFarColor(long r, long g, long b) __asm__("SetFarColor");
#endif
#ifndef XM_S_SetFogNearFar
#define XM_S_SetFogNearFar
void xm_SetFogNearFar(long a, long b, long h) __asm__("func_80048AB0");
#endif
#ifndef XM_S_model_color
#define XM_S_model_color
extern u32 xm_model_color __asm__("D_80059598");
#endif
void func_80082A70(void) {
    Environment *env;
    s32 top_r;
    s32 top_g;
    s32 top_b;
    s32 bottom_r;
    s32 bottom_g;
    s32 bottom_b;

    env = &xm_arena_stage_color_table[xm_arena_stage_index];
    xm_arena_current_stage_colors = env;
    top_r = env->top[0];
    top_g = env->top[1];
    top_b = env->top[2];
    xm_arena_stage_back_color_red = env->unk4;
    xm_arena_stage_back_color_green = env->unk5;
    xm_arena_stage_back_color_blue = env->unk6;
    bottom_r = env->bottom[0];
    bottom_g = env->bottom[1];
    bottom_b = env->bottom[2];
    xm_model_set_color(env->back[0], env->back[1], env->back[2]);
    xm_SetFarColor(bottom_r, bottom_g, bottom_b);
    xm_arena_stage_sky_gradients[0].r0 = top_r;
    xm_arena_stage_sky_gradients[1].r0 = top_r;
    xm_arena_stage_sky_gradients[0].g0 = top_g;
    xm_arena_stage_sky_gradients[1].g0 = top_g;
    xm_arena_stage_sky_gradients[0].b0 = top_b;
    xm_arena_stage_sky_gradients[1].b0 = top_b;
    *(u16 *)&xm_arena_stage_sky_gradients[0].r1 = top_r | (top_g << 8);
    xm_arena_stage_sky_gradients[0].b1 = top_b;
    *(u16 *)&xm_arena_stage_sky_gradients[1].r1 = top_r | (top_g << 8);
    xm_arena_stage_sky_gradients[1].b1 = top_b;
    *(u16 *)&xm_arena_stage_sky_gradients[0].r2 = bottom_r | (bottom_g << 8);
    xm_arena_stage_sky_gradients[0].b2 = bottom_b;
    *(u16 *)&xm_arena_stage_sky_gradients[1].r2 = bottom_r | (bottom_g << 8);
    xm_arena_stage_sky_gradients[1].b2 = bottom_b;
    *(u16 *)&xm_arena_stage_sky_gradients[0].r3 = bottom_r | (bottom_g << 8);
    xm_arena_stage_sky_gradients[0].b3 = bottom_b;
    *(u16 *)&xm_arena_stage_sky_gradients[1].r3 = bottom_r | (bottom_g << 8);
    xm_arena_stage_sky_gradients[1].b3 = bottom_b;
    xm_arena_display_buffers[0].background.r0 = bottom_r;
    xm_arena_display_buffers[0].background.g0 = bottom_g;
    xm_arena_display_buffers[0].background.b0 = bottom_b;
    xm_arena_display_buffers[1].background.r0 = bottom_r;
    xm_arena_display_buffers[1].background.g0 = bottom_g;
    xm_arena_display_buffers[1].background.b0 = bottom_b;
    xm_SetFogNearFar(0x800, 0x1800, 0xC0);
    xm_model_color = (xm_model_color & 0xFFFFFF) | 0x28000000;
    gte_ldrgb(&xm_model_color);
}
#endif
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80082C4C);
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_stage_draw_wall (src/decomp/src/menu/arena_stage_views_and_hud.c:530). */
#ifndef XM_T_gte_dpcs
#define XM_T_gte_dpcs
#define gte_dpcs() __asm__ volatile("nop;nop;.word 0x4A780010")
#endif
#ifndef XM_T_gte_strgb
#define XM_T_gte_strgb
#define gte_strgb(r0) __asm__ volatile("swc2 $22, 0(%0)" : : "r"(r0) : "memory")
#endif
#ifndef XM_S_arena_stage_draw_wall
#define XM_S_arena_stage_draw_wall
void xm_arena_stage_draw_wall(u32 *ot, VECTOR *pos) __asm__("func_80082E60");
#endif
#ifndef XM_S_arena_stage_wall_quads
#define XM_S_arena_stage_wall_quads
extern POLY_FT4 *xm_arena_stage_wall_quads[2] __asm__("D_80092788");
#endif
#pragma push_macro("gte_ldv3")
#undef gte_ldv3
#define gte_ldv3(r0, r1, r2)                                                   \
    __asm__ volatile("lwc2 $0, 0(%0);"                                         \
                     "lwc2 $1, 4(%0);"                                         \
                     "lwc2 $2, 0(%1);"                                         \
                     "lwc2 $3, 4(%1);"                                         \
                     "lwc2 $4, 0(%2);"                                         \
                     "lwc2 $5, 4(%2)"                                          \
                     :                                                         \
                     : "r"(r0), "r"(r1), "r"(r2)                               \
                     : "memory")
#pragma push_macro("gte_rtpt")
#undef gte_rtpt
#define gte_rtpt() __asm__ volatile("nop;nop;.word 0x4A280030")
#pragma push_macro("gte_dpcs")
#undef gte_dpcs
#define gte_dpcs() __asm__ volatile("nop;nop;.word 0x4A780010")
#pragma push_macro("gte_stsxy3")
#undef gte_stsxy3
#define gte_stsxy3(r0, r1, r2)                                                 \
    __asm__ volatile("swc2 $12, 0(%0);"                                        \
                     "swc2 $13, 0(%1);"                                        \
                     "swc2 $14, 0(%2)"                                         \
                     :                                                         \
                     : "r"(r0), "r"(r1), "r"(r2)                               \
                     : "memory")
#pragma push_macro("gte_stsz3")
#undef gte_stsz3
#define gte_stsz3(r0, r1, r2)                                                  \
    __asm__ volatile("swc2 $17, 0(%0);"                                        \
                     "swc2 $18, 0(%1);"                                        \
                     "swc2 $19, 0(%2)"                                         \
                     :                                                         \
                     : "r"(r0), "r"(r1), "r"(r2)                               \
                     : "memory")
#pragma push_macro("gte_stsxy")
#undef gte_stsxy
#define gte_stsxy(r0) __asm__ volatile("swc2 $14, 0(%0)" : : "r"(r0) : "memory")
#pragma push_macro("gte_stsz")
#undef gte_stsz
#define gte_stsz(r0) __asm__ volatile("swc2 $19, 0(%0)" : : "r"(r0) : "memory")
#pragma push_macro("gte_strgb")
#undef gte_strgb
#define gte_strgb(r0) __asm__ volatile("swc2 $22, 0(%0)" : : "r"(r0) : "memory")
void func_80082E60(u32 *ot, VECTOR *pos) {
    VECTOR centre;
    SVECTOR base0;
    SVECTOR base1;
    SVECTOR mid0;
    SVECTOR mid1;
    SVECTOR top0;
    SVECTOR top1;
    s32 z[4];
    POLY_FT4 *quad;
    POLY_FT4 *next;
    s32 angle;
    s32 depth;
    s32 i;

    centre = *pos;
    i = 0;
    quad = xm_arena_stage_wall_quads[xm_arena_draw_buffer_index];
    centre.vx -= 0x3F80;
    centre.vz -= 0x3F80;
    angle = xm_ratan2(centre.vx, centre.vz) & 0xFFF0;
    angle -= 0x100;
    mid0.vy = mid1.vy = -0x290;
    base0.vy = base1.vy = 0;
    top0.vy = top1.vy = -0x520;
    base0.vx = ((xm_gpu_get_sin(angle) * 0x3F80) >> 12) - (s16)(xm_arena_view_origin.vx - 0x3F80);
    base0.vz = ((xm_gpu_get_cos(angle) * 0x3F80) >> 12) - (s16)(xm_arena_view_origin.vz - 0x3F80);
    angle += 0x10;
    for (; i < 32; i++) {
        top0.vx = mid0.vx = base0.vx;
        top0.vz = mid0.vz = base0.vz;
        top1.vx = mid1.vx = base1.vx = ((xm_gpu_get_sin(angle) * 0x3F80) >> 12) - (s16)(xm_arena_view_origin.vx - 0x3F80);
        top1.vz = mid1.vz = base1.vz = ((xm_gpu_get_cos(angle) * 0x3F80) >> 12) - (s16)(xm_arena_view_origin.vz - 0x3F80);
        gte_ldv3(&base0, &base1, &mid0);
        gte_rtpt();
        gte_dpcs();
        gte_stsxy3(&quad[0].x0, &quad[0].x1, &quad[0].x2);
        gte_stsz3(&z[0], &z[1], &z[2]);
        gte_ldv3(&mid1, &top0, &top1);
        gte_rtpt();
        next = &quad[1];
        depth = z[0];
        if (depth < z[1]) {
            depth = z[1];
        }
        if (depth <= z[2]) {
            depth = z[2];
        }
        *(u32 *)&next->x0 = *(u32 *)&quad[0].x2;
        gte_stsxy(&quad[0].x3);
        gte_stsxy3(&quad[0].x3, &next->x2, &next->x3);
        gte_stsz(&z[3]);
        *(u32 *)&next->x1 = *(u32 *)&quad[0].x3;
        if (depth <= z[3]) {
            depth = z[3];
        }
        if (depth < 0x1C00) {
            depth >>= 4;
            gte_strgb(&quad[0].r0);
            gte_strgb(&next->r0);
            setlen(&quad[0], 9);
            quad[0].code = 0x2C;
            setlen(next, 9);
            next->code = 0x2C;
            xm_AddPrim(&ot[depth], &quad[0]);
            xm_AddPrim(&ot[depth], next);
        }
        quad += 2;
        angle += 0x10;
        base0.vx = base1.vx;
        base0.vz = base1.vz;
    }
}
#pragma pop_macro("gte_ldv3")
#pragma pop_macro("gte_rtpt")
#pragma pop_macro("gte_dpcs")
#pragma pop_macro("gte_stsxy3")
#pragma pop_macro("gte_stsz3")
#pragma pop_macro("gte_stsxy")
#pragma pop_macro("gte_stsz")
#pragma pop_macro("gte_strgb")
#endif
s32 rand(void);
s32 rcos(s32);
s32 rsin(s32);
extern s32 D_80092770;
extern s32 D_80092774;
extern s32 D_8009290C;
extern s32 D_8009871C[];
extern s32 D_80098720[];
extern s32 D_80098724;

void func_800831C8(void) {
    s32 r;
    s32 ang;

    r = (rand() & 0x1FFF) + 0x800;
    ang = (rand() % 0x600) + 0x500;
    D_8009871C[0] = ((rcos(ang) * r) >> 12) + 0x4000;
    D_80098724 = ((rsin(ang) * r) >> 12) + 0x4000;
    D_80098720[0] = -((rand() & 0x7FF) + 0x400);
    D_80092770 = 0x100;
    D_80092774 = 0x40;
    (*(s8*)&D_8009287C) = 0x40;
    D_8009290C = 0x400;
}

extern s32 D_800927AC;

void func_800832C0(s32 arg0) {
    if (arg0 & 0x8000) {
        D_800927AC += 0x20;
    }
    if (arg0 & 0x2000) {
        D_800927AC -= 0x20;
    }
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_camera_update_orbit (src/decomp/src/menu/arena_stage_views_and_hud.c:635). */
#ifndef XM_S_arena_camera_update_orbit
#define XM_S_arena_camera_update_orbit
void xm_arena_camera_update_orbit(s32 smooth) __asm__("func_80083310");
#endif
#ifndef XM_S_arena_camera_radius
#define XM_S_arena_camera_radius
extern s32 xm_arena_camera_radius __asm__("D_80092770");
#endif
#ifndef XM_S_arena_camera_orbit_angle
#define XM_S_arena_camera_orbit_angle
extern s32 xm_arena_camera_orbit_angle __asm__("D_800927AC");
#endif
#ifndef XM_S_arena_camera_orbit_speed
#define XM_S_arena_camera_orbit_speed
extern s32 xm_arena_camera_orbit_speed __asm__("D_800927B0");
#endif
void func_80083310(s32 smooth) {
    VECTOR eye;
    VECTOR step;
    VECTOR offset;
    VECTOR unused;   
    SVECTOR unused2;
    Actor *subject;
    Actor *other;
    s32 value; 

    if (xm_arena_bout_round_winner != 0) {
        subject = &xm_arena_second_actor;
        other = &xm_arena_first_actor;
    } else {
        subject = &xm_arena_first_actor;
        other = &xm_arena_second_actor;
    }
    xm_ratan2(subject->pos.vx - other->pos.vx, subject->pos.vz - other->pos.vz);
    if (xm_arena_bout_replay_timer > 0xB0) {
        value = 0x100;
    } else if (xm_arena_bout_replay_timer > 0xA0) {
        value = (xm_arena_bout_replay_timer - 0xA0) << 4;
    } else {
        value = 0;
    }
    offset.vx = other->pos.vx;
    offset.vy = other->pos.vy;
    offset.vz = other->pos.vz;
    offset.vx -= subject->pos.vx;
    offset.vy -= subject->pos.vy;
    offset.vz -= subject->pos.vz;
    offset.vx *= value;
    offset.vy *= value;
    offset.vz *= value;
    offset.vx /= 256;
    offset.vy /= 256;
    offset.vz /= 256;
    offset.vx += subject->pos.vx;
    offset.vy += subject->pos.vy;
    offset.vz += subject->pos.vz;
    offset.vy -= 0xA0;
    offset.vx -= xm_arena_camera_focus.vx;
    offset.vy -= xm_arena_camera_focus.vy;
    offset.vz -= xm_arena_camera_focus.vz;
    if (smooth) {
        offset.vx /= 3;
        offset.vy /= 3;
        offset.vz /= 3;
    }
    xm_arena_camera_radius = 0xC00;
    xm_arena_camera_focus.vx += offset.vx;
    xm_arena_camera_focus.vy += offset.vy;
    xm_arena_camera_focus.vz += offset.vz;
    value = xm_arena_camera_orbit_angle + xm_arena_bout_replay_timer * xm_arena_camera_orbit_speed;
    eye.vx = (xm_gpu_get_sin(value) * xm_arena_camera_radius) >> 12;
    eye.vz = (xm_gpu_get_cos(value) * xm_arena_camera_radius) >> 12;
    eye.vy = -(xm_arena_bout_replay_timer * 6 + 0x200);
    eye.vx += xm_arena_camera_focus.vx;
    eye.vy += xm_arena_camera_focus.vy;
    eye.vz += xm_arena_camera_focus.vz;
    step.vx = eye.vx - xm_arena_camera_position.vx;
    step.vz = eye.vz - xm_arena_camera_position.vz;
    xm_arena_stage_keep_step_inside(&xm_arena_camera_position, &step, 0x3D00);
    xm_arena_camera_position.vx += step.vx;
    xm_arena_camera_position.vz += step.vz;
    value = xm_arena_stage_get_ground_height(&xm_arena_camera_position, 0);
    if (value < eye.vy) {
        eye.vy = value;
    }
    xm_arena_camera_position.vy = eye.vy;
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_camera_start_orbit (src/decomp/src/menu/arena_stage_views_and_hud.c:708). */
#ifndef XM_S_arena_camera_start_orbit
#define XM_S_arena_camera_start_orbit
void xm_arena_camera_start_orbit(void) __asm__("func_8008369C");
#endif
void func_8008369C(void) {
    xm_arena_camera_orbit_angle = xm_rand();
    xm_arena_camera_orbit_speed = xm_rand() % 12 + 4;
    if (xm_rand() & 1) {
        xm_arena_camera_orbit_speed = -xm_arena_camera_orbit_speed;
    }
    xm_arena_camera_update_orbit(0);
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_camera_frame_actors_for_scene (src/decomp/src/menu/arena_stage_views_and_hud.c:721). */
#ifndef XM_S_arena_camera_frame_actors_for_scene
#define XM_S_arena_camera_frame_actors_for_scene
void xm_arena_camera_frame_actors_for_scene(Actor *first, Actor *second) __asm__("func_80083738");
#endif
#ifndef XM_S_arena_camera_pair_lift
#define XM_S_arena_camera_pair_lift
extern s32 xm_arena_camera_pair_lift __asm__("D_80092774");
#endif
#ifndef XM_S_arena_camera_unread_pair_heading
#define XM_S_arena_camera_unread_pair_heading
extern s32 xm_arena_camera_unread_pair_heading __asm__("D_8009277C");
#endif
void func_80083738(Actor *first, Actor *second) {
    VECTOR side;
    VECTOR other_side;
    VECTOR unused[2]; 
    s32 heading;
    s32 distance;
    s32 angle;
    s32 value; 

    heading = xm_ratan2(first->pos.vx - second->pos.vx, first->pos.vz - second->pos.vz);
    distance = xm_arena_vector_get_distance(&first->pos, &second->pos);
    angle = heading - 0x400;
    xm_arena_camera_radius = distance * 2 / 3 + 0xC0;
    xm_arena_camera_focus.vx = (first->pos.vx + second->pos.vx) / 2;
    xm_arena_camera_focus.vy = (first->pos.vy + second->pos.vy) / 2 - 0xA0;
    xm_arena_camera_focus.vz = (first->pos.vz + second->pos.vz) / 2;
    side.vx = xm_arena_camera_focus.vx + ((xm_gpu_get_sin(angle) * xm_arena_camera_radius) >> 12);
    side.vz = xm_arena_camera_focus.vz + ((xm_gpu_get_cos(angle) * xm_arena_camera_radius) >> 12);
    value = heading + 0x400;
    other_side.vx = xm_arena_camera_focus.vx + ((xm_gpu_get_sin(value) * xm_arena_camera_radius) >> 12);
    other_side.vz = xm_arena_camera_focus.vz + ((xm_gpu_get_cos(value) * xm_arena_camera_radius) >> 12);
    side.vx -= xm_arena_camera_position.vx;
    side.vy -= xm_arena_camera_position.vy;
    side.vz -= xm_arena_camera_position.vz;
    other_side.vx -= xm_arena_camera_position.vx;
    other_side.vy -= xm_arena_camera_position.vy;
    other_side.vz -= xm_arena_camera_position.vz;
    value = xm_arena_vector_get_flat_length(&side);
    if (xm_arena_vector_get_flat_length(&other_side) < value) {
        xm_arena_camera_side_angle = 0x400;
        xm_arena_camera_side_flipped = 0;
    } else {
        xm_arena_camera_side_angle = -0x400;
        xm_arena_camera_side_flipped = 1;
    }
    distance /= 4;
    if (distance > 0x300) {
        distance = 0x300;
    }
    side.vy = xm_arena_camera_focus.vy - xm_arena_camera_pair_lift - distance;
    side.vx = xm_arena_camera_focus.vx + ((xm_gpu_get_sin(heading + xm_arena_camera_side_angle) * xm_arena_camera_radius) >> 12);
    side.vz = xm_arena_camera_focus.vz + ((xm_gpu_get_cos(heading + xm_arena_camera_side_angle) * xm_arena_camera_radius) >> 12);
    value = xm_arena_stage_get_ground_height(&side, 0) - 0x100;
    if (value < side.vy) {
        side.vy = value;
    }
    side.vx = (side.vx - xm_arena_camera_position.vx) / xm_arena_camera_ease_step_count;
    side.vy = (side.vy - xm_arena_camera_position.vy) / xm_arena_camera_ease_step_count;
    side.vz = (side.vz - xm_arena_camera_position.vz) / xm_arena_camera_ease_step_count;
    xm_arena_camera_unread_pair_heading = heading;
    xm_arena_stage_keep_step_inside(&xm_arena_camera_position, &side, 0x3D00);
    xm_arena_camera_ease_step_count = 100;
    xm_arena_camera_position.vx += side.vx;
    xm_arena_camera_position.vy += side.vy;
    xm_arena_camera_position.vz += side.vz;
}
#endif
void func_80083B54(void* arg0, void* arg1) {
    struct Vector { s32 x, y, z, pad; };
    *(struct Vector*)arg0 = *(struct Vector*)D_8009871C;
    *(struct Vector*)arg1 = *(struct Vector*)D_8009867C;
}

void ClearImage(s16 *, s32, s32, s32); /* extern */

void func_80083BB4(s32 arg0) {
    struct { s16 x,y,w,h; } rect;

    rect.x = 0;
    rect.y = 0;
    if (arg0 != 0) {
        rect.w = 0x280;
    } else {
        rect.w = 0x140;
    }
    rect.h = 0x1E0;
    ClearImage(&rect.x, 0, 0, 0);
    DrawSync(0);
}

extern s32 D_80092794;
extern s32 D_8009294C;
void func_8007A21C(s32);
void func_8007AC3C(void);
void func_800725B0(void *);
extern u8 D_80097010__a[] asm("D_80097010");
extern u8 D_8009872C__a[] asm("D_8009872C");

void func_80083C0C(s32 arg0) {
    D_80092794 = arg0;
    switch (arg0) {
    case 3:
        func_80081E6C();
        return;
    case 4:
        func_8007A21C(D_8009294C);
        return;
    case 8:
        func_8007AC3C();
        return;
    case 6:
        if (*(s16 *)(D_8009872C__a + 0xF2) < *(s16 *)(D_80097010__a + 0xF2)) {
            func_800725B0(D_80097010__a);
        } else {
            func_800725B0(D_8009872C__a);
        }
        return;
    }
}


s32 func_80083CD8(void) {
    return D_80092790;
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80083CE8);
void func_80030A30(s32, void *);                 /* extern */
extern void *D_8009288C;
extern s32 D_8009871C[];
extern s32 D_80098724;

void func_80083DCC(void *arg0, void *arg1, s32 arg2) {
    u8 unused[0x20]; /* retail frame has 0x20 bytes of locals no code touches */
    s32 temp_a0;
    u8 temp_v1;
    void *temp_a0_10;
    void *temp_a0_2;
    void *temp_a0_3;
    void *temp_a0_4;
    void *temp_a0_5;
    void *temp_a0_6;
    void *temp_a0_7;
    void *temp_a0_8;
    void *temp_a0_9;
    void *temp_a1;
    void *temp_v0_2;

    temp_v1 = *(u8 *)((s8*)(arg1) + 0x918);
    temp_a1 = *(void **)((s8*)(((arg2 * 4) + arg0)) + 8);
    temp_a0 = temp_v1 & 0xFF;
    if (temp_a0 != 0) {
        *(u8 *)((s8*)(arg1) + 0x918) = temp_v1 - 0x18;
        if (temp_a0 < *(u8 *)((s8*)(arg1) + 0x918)) {
            *(u8 *)((s8*)(arg1) + 0x918) = 0U;
        }
    }
    if (*(s32 *)((s8*)(arg1) + 0xD4) & 0x20) {
        *(u16 *)((s8*)(*(void **)((s8*)(temp_a1) + 4)) + 0xC) = (u16) ((s32) (*(u8 *)((s8*)(*(void **)((s8*)(arg1) + 0xD8)) + 0x15D4) * temp_a0) / 16);
        *(u16 *)((s8*)(*(void **)((s8*)(temp_a1) + 4)) + 0xE) = (u16) ((s32) (*(u8 *)((s8*)(*(void **)((s8*)(arg1) + 0xD8)) + 0x15D5) * temp_a0) / 16);
        *(u16 *)((s8*)(*(void **)((s8*)(temp_a1) + 4)) + 0x10) = (u16) ((s32) (*(u8 *)((s8*)(*(void **)((s8*)(arg1) + 0xD8)) + 0x15D6) * temp_a0) / 16);
    } else {
        *(u16 *)((s8*)(*(void **)((s8*)(temp_a1) + 4)) + 0xC) = (u16) (temp_a0 * 0x10);
        *(u16 *)((s8*)(*(void **)((s8*)(temp_a1) + 4)) + 0xE) = (u16) (temp_a0 * 8);
        *(u16 *)((s8*)(*(void **)((s8*)(temp_a1) + 4)) + 0x10) = 0U;
    }
    if (*(u8 *)((s8*)(D_8009288C) + 0x10) != 0) {
        temp_a0_2 = *(void **)((s8*)(temp_a1) + 4);
        *(u16 *)((s8*)(temp_a0_2) + 0xC) = (u16) ((s16) *(u16 *)((s8*)(temp_a0_2) + 0xC) / 2);
        temp_a0_3 = *(void **)((s8*)(temp_a1) + 4);
        *(u16 *)((s8*)(temp_a0_3) + 0xE) = (u16) ((s16) *(u16 *)((s8*)(temp_a0_3) + 0xE) / 2);
        temp_a0_4 = *(void **)((s8*)(temp_a1) + 4);
        *(u16 *)((s8*)(temp_a0_4) + 0x10) = (u16) ((s16) *(u16 *)((s8*)(temp_a0_4) + 0x10) / 2);
    }
    *(s32 *)((s8*)(*(void **)((s8*)(temp_a1) + 4)) + 0) = (s32) *(s32 *)((s8*)(arg1) + 0);
    *(s32 *)((s8*)(*(void **)((s8*)(temp_a1) + 4)) + 4) = (s32) *(s32 *)((s8*)(arg1) + 4);
    *(s32 *)((s8*)(*(void **)((s8*)(temp_a1) + 4)) + 8) = (s32) *(s32 *)((s8*)(arg1) + 8);
    temp_a0_5 = *(void **)((s8*)(temp_a1) + 4);
    *(s32 *)((s8*)(temp_a0_5) + 0) = (s32) (*(s32 *)((s8*)(temp_a0_5) + 0) - *(s32 *)((s8*)(*(void **)((s8*)(arg1) + 0xD8)) + 0));
    temp_a0_6 = *(void **)((s8*)(temp_a1) + 4);
    *(s32 *)((s8*)(temp_a0_6) + 4) = (s32) (*(s32 *)((s8*)(temp_a0_6) + 4) - *(s32 *)((s8*)(*(void **)((s8*)(arg1) + 0xD8)) + 4));
    temp_a0_7 = *(void **)((s8*)(temp_a1) + 4);
    *(s32 *)((s8*)(temp_a0_7) + 8) = (s32) (*(s32 *)((s8*)(temp_a0_7) + 8) - *(s32 *)((s8*)(*(void **)((s8*)(arg1) + 0xD8)) + 8));
    func_80030A30(arg2, *(void **)((s8*)(temp_a1) + 4));
    temp_a1 = *(void **)((s8*)(arg0) + 0x10);
    temp_v0_2 = *(void **)((s8*)(temp_a1) + 4);
    *(s16 *)((s8*)(temp_v0_2) + 0x10) = 0;
    *(s16 *)((s8*)(temp_v0_2) + 0xE) = 0;
    *(s16 *)((s8*)(temp_v0_2) + 0xC) = 0;
    *(s32 *)((s8*)(*(void **)((s8*)(temp_a1) + 4)) + 0) = (s32) *(s32 *)((s8*)(arg1) + 0);
    *(s32 *)((s8*)(*(void **)((s8*)(temp_a1) + 4)) + 4) = (s32) *(s32 *)((s8*)(arg1) + 4);
    *(s32 *)((s8*)(*(void **)((s8*)(temp_a1) + 4)) + 8) = (s32) *(s32 *)((s8*)(arg1) + 8);
    temp_a0_8 = *(void **)((s8*)(temp_a1) + 4);
    *(s32 *)((s8*)(temp_a0_8) + 0) = (s32) (*(s32 *)((s8*)(temp_a0_8) + 0) - D_8009871C[0]);
    temp_a0_9 = *(void **)((s8*)(temp_a1) + 4);
    *(s32 *)((s8*)(temp_a0_9) + 4) = (s32) (*(s32 *)((s8*)(temp_a0_9) + 4) - D_80098720[0]);
    temp_a0_10 = *(void **)((s8*)(temp_a1) + 4);
    *(s32 *)((s8*)(temp_a0_10) + 8) = (s32) (*(s32 *)((s8*)(temp_a0_10) + 8) - D_80098724);
    func_80030A30(2, *(void **)((s8*)(temp_a1) + 4));
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_view_draw_arena (src/decomp/src/menu/arena_stage_views_and_hud.c:889). */
#ifndef XM_T_OtPair
#define XM_T_OtPair
typedef struct {
    s32 unk0;
    u32 *ot[2];        
    u32 *last[2];      
    s16 length;        
    u8 flags;          
    u8 shift;          
    DR_AREA area[2];   
    DR_OFFSET offset[2]; 
    TileWords tile[2]; 
} OtPair;
#endif
#ifndef XM_T_LightRig
#define XM_T_LightRig
typedef struct {
    s32 unk0;
    Node *camera;      
    Node *lights[3];   
    Node storage[4];   
    OtPair *layer;     
    u8 r, g, b;        
    u8 unk28B;
} LightRig;
#endif
#ifndef XM_T_Light
#define XM_T_Light
typedef struct Light {
    s32 direction[3];
    s16 colour[3];     
    s16 unk12;
} Light;
#endif
#ifndef XM_T_NODE_LIGHT
#define XM_T_NODE_LIGHT
#define NODE_LIGHT(node) ((Light *)(node)->data)
#endif
#ifndef XM_T_gte_SetBackColor
#define XM_T_gte_SetBackColor
#define gte_SetBackColor(r0, r1, r2)                                           \
    __asm__ volatile("sll $12, %0, 4;"                                         \
                     "sll $13, %1, 4;"                                         \
                     "sll $14, %2, 4;"                                         \
                     "ctc2 $12, $13;"                                          \
                     "ctc2 $13, $14;"                                          \
                     "ctc2 $14, $15"                                           \
                     :                                                         \
                     : "r"(r0), "r"(r1), "r"(r2)                               \
                     : "$12", "$13", "$14")
#endif
#ifndef XM_S_arena_view_draw_arena
#define XM_S_arena_view_draw_arena
s32 xm_arena_view_draw_arena(LightRig *rig) __asm__("func_800840CC");
#endif
#ifndef XM_S_arena_display_start_layer
#define XM_S_arena_display_start_layer
void xm_arena_display_start_layer(OtPair *pair) __asm__("func_8008AC0C");
#endif
#ifndef XM_S_arena_node_aim_rig_camera
#define XM_S_arena_node_aim_rig_camera
void xm_arena_node_aim_rig_camera(LightRig *view, VECTOR *position, VECTOR *focus) __asm__("func_80089A98");
#endif
#ifndef XM_S_arena_mode_state
#define XM_S_arena_mode_state
extern s32 xm_arena_mode_state __asm__("D_80092790");
#endif
#ifndef XM_S_arena_hud_draw
#define XM_S_arena_hud_draw
void xm_arena_hud_draw(Actor *left, Actor *right) __asm__("func_80087068");
#endif
#ifndef XM_S_arena_actor_draw_move_info
#define XM_S_arena_actor_draw_move_info
void xm_arena_actor_draw_move_info(Actor *actor) __asm__("func_8007661C");
#endif
#ifndef XM_S_arena_debug_enabled
#define XM_S_arena_debug_enabled
extern s32 xm_arena_debug_enabled __asm__("D_800911D4");
#endif
#ifndef XM_S_arena_menu_draw_overlay
#define XM_S_arena_menu_draw_overlay
void xm_arena_menu_draw_overlay(void *ot) __asm__("func_80080D20");
#endif
#ifndef XM_S_model_set_light
#define XM_S_model_set_light
void xm_model_set_light(s32 index, struct Light *light) __asm__("func_80030A30");
#endif
#ifndef XM_S_arena_view_light_actor
#define XM_S_arena_view_light_actor
void xm_arena_view_light_actor(LightRig *rig, Actor *actor, s32 index) __asm__("func_80083DCC");
#endif
#ifndef XM_S_arena_node_draw_tree
#define XM_S_arena_node_draw_tree
void xm_arena_node_draw_tree(Node *node) __asm__("func_8008A7E0");
#endif
#ifndef XM_S_arena_sound_update_voices
#define XM_S_arena_sound_update_voices
void xm_arena_sound_update_voices(void) __asm__("func_8008E8B0");
#endif
#ifndef XM_S_arena_effect_emit_hit_sparks
#define XM_S_arena_effect_emit_hit_sparks
void xm_arena_effect_emit_hit_sparks(u32 *ot, MATRIX *view) __asm__("func_8007334C");
#endif
#ifndef XM_S_arena_stage_draw_shadow
#define XM_S_arena_stage_draw_shadow
void xm_arena_stage_draw_shadow(Actor *actor, u32 *ot, MATRIX *view) __asm__("func_80087B74");
#endif
#ifndef XM_S_arena_stage_clear_row_spans
#define XM_S_arena_stage_clear_row_spans
void xm_arena_stage_clear_row_spans(void) __asm__("func_80087650");
#endif
#ifndef XM_S_arena_stage_uses_narrow_view
#define XM_S_arena_stage_uses_narrow_view
extern s32 xm_arena_stage_uses_narrow_view __asm__("D_800928B0");
#endif
#ifndef XM_S_arena_stage_draw_ground
#define XM_S_arena_stage_draw_ground
void xm_arena_stage_draw_ground(u32 *ot, s32 originX, s32 originZ) __asm__("func_8008779C");
#endif
#ifndef XM_S_arena_display_link_layer
#define XM_S_arena_display_link_layer
void xm_arena_display_link_layer(OtPair *layer) __asm__("func_8008AE1C");
#endif
#ifndef XM_S_arena_hud_link_overlay_tpage
#define XM_S_arena_hud_link_overlay_tpage
void xm_arena_hud_link_overlay_tpage(void) __asm__("func_80086E24");
#endif
#ifndef XM_S_gpu_ot_link_poly_g4
#define XM_S_gpu_ot_link_poly_g4
void xm_gpu_ot_link_poly_g4(u_long *ot, POLY_G4 *prim) __asm__("func_80031678");
#endif
s32 func_800840CC(LightRig *rig) {
    MATRIX floor;
    MATRIX camera;
    VECTOR unused; 
    OtPair *layer = rig->layer;
    Light *light;

    xm_arena_display_start_layer(layer);
    xm_arena_node_aim_rig_camera(rig, &xm_arena_camera_position, &xm_arena_camera_focus);
    if (xm_arena_mode_state != 4 && xm_arena_play_mode != 4 && xm_arena_mode_state != 8) {
        xm_arena_hud_draw(&xm_arena_first_actor, &xm_arena_second_actor);
    } else if (xm_arena_play_mode == 4) {
        xm_arena_actor_draw_move_info(&xm_arena_first_actor);
        if (xm_arena_debug_enabled != 0) {
            xm_arena_actor_draw_move_info(&xm_arena_second_actor);
        }
    }
    xm_arena_text_draw_banner(xm_arena_current_ot);
    xm_arena_menu_draw_overlay(xm_arena_current_ot);
    camera = rig->camera->view;
    ((Node *)xm_arena_first_actor.object)->view = ((Node *)xm_arena_second_actor.object)->view = camera;
    light = NODE_LIGHT(rig->lights[0]);
    light->colour[0] = light->colour[1] = light->colour[2] = 0x800;
    xm_model_set_light(0, NODE_LIGHT(rig->lights[0]));
    gte_SetBackColor(xm_arena_stage_back_color_red, xm_arena_stage_back_color_green, xm_arena_stage_back_color_blue);
    xm_arena_view_light_actor(rig, &xm_arena_first_actor, 1);
    xm_arena_node_draw_tree(xm_arena_first_actor.node);
    xm_arena_view_light_actor(rig, &xm_arena_second_actor, 1);
    xm_arena_node_draw_tree(xm_arena_second_actor.node);
    gte_SetRotMatrix(&camera);
    gte_SetTransMatrix(&camera);
    xm_arena_sound_update_voices();
    xm_arena_effect_draw_all(&camera, layer->ot[xm_arena_draw_buffer_index]);
    gte_SetRotMatrix(&camera);
    gte_SetTransMatrix(&camera);
    xm_arena_effect_emit_hit_sparks(layer->ot[xm_arena_draw_buffer_index], &rig->camera->view);
    floor = xm_arena_identity_matrix;
    floor.t[1] = -xm_arena_view_origin.vy;
    xm_CompMatrix(&camera, &floor, &floor);
    gte_SetRotMatrix(&floor);
    gte_SetTransMatrix(&floor);
    xm_arena_stage_apply_colors();
    xm_arena_stage_draw_wall(layer->ot[xm_arena_draw_buffer_index], &xm_arena_camera_position);
    xm_arena_stage_draw_shadow(&xm_arena_first_actor, layer->ot[xm_arena_draw_buffer_index], &floor);
    xm_arena_stage_draw_shadow(&xm_arena_second_actor, layer->ot[xm_arena_draw_buffer_index], &floor);
    gte_SetRotMatrix(&floor);
    gte_SetTransMatrix(&floor);
    xm_arena_stage_clear_row_spans();
    if (xm_arena_stage_uses_narrow_view != 0) {
        xm_arena_stage_mark_narrow_view_cells(xm_arena_camera_position.vx, xm_arena_camera_position.vz,
                      xm_ratan2(xm_arena_camera_position.vx - xm_arena_camera_focus.vx, xm_arena_camera_position.vz - xm_arena_camera_focus.vz));
    } else {
        xm_arena_stage_mark_wide_view_cells(xm_arena_camera_position.vx, xm_arena_camera_position.vz,
                      xm_ratan2(xm_arena_camera_position.vx - xm_arena_camera_focus.vx, xm_arena_camera_position.vz - xm_arena_camera_focus.vz));
    }
    xm_arena_stage_draw_ground(layer->ot[xm_arena_draw_buffer_index], xm_arena_camera_focus.vx, xm_arena_camera_focus.vz);
    xm_arena_display_link_layer(layer);
    xm_arena_hud_link_overlay_tpage();
    xm_gpu_ot_link_poly_g4((u_long *)xm_arena_current_ot, &xm_arena_stage_sky_gradients[xm_arena_draw_buffer_index]);
    return 0;
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_view_draw_actors_on_backdrop (src/decomp/src/menu/arena_stage_views_and_hud.c:953). */
#ifndef XM_S_arena_view_draw_actors_on_backdrop
#define XM_S_arena_view_draw_actors_on_backdrop
s32 xm_arena_view_draw_actors_on_backdrop(LightRig *rig) __asm__("func_800846A0");
#endif
#ifndef XM_S_arena_text_drop_quads
#define XM_S_arena_text_drop_quads
void xm_arena_text_drop_quads(void) __asm__("func_80080D10");
#endif
#ifndef XM_S_arena_node_disable_color_overrides
#define XM_S_arena_node_disable_color_overrides
void xm_arena_node_disable_color_overrides(void) __asm__("func_8008A62C");
#endif
#ifndef XM_S_arena_effect_draw_lines_only
#define XM_S_arena_effect_draw_lines_only
void xm_arena_effect_draw_lines_only(void *arg) __asm__("func_8007D068");
#endif
s32 func_800846A0(LightRig *rig) {
    MATRIX unused0; 
    MATRIX camera;
    VECTOR unused1; 
    OtPair *layer = rig->layer;
    Light *light;

    xm_arena_display_start_layer(layer);
    xm_arena_node_aim_rig_camera(rig, &xm_arena_camera_position, &xm_arena_camera_focus);
    xm_arena_text_drop_quads();
    camera = rig->camera->view;
    ((Node *)xm_arena_first_actor.object)->view = ((Node *)xm_arena_second_actor.object)->view = camera;
    xm_arena_node_disable_color_overrides();
    light = NODE_LIGHT(rig->lights[0]);
    light->colour[0] = light->colour[1] = light->colour[2] = 0x800;
    xm_model_set_light(0, NODE_LIGHT(rig->lights[0]));
    gte_SetBackColor(xm_arena_stage_back_color_red, xm_arena_stage_back_color_green, xm_arena_stage_back_color_blue);
    xm_arena_view_light_actor(rig, &xm_arena_first_actor, 1);
    xm_arena_node_draw_tree(xm_arena_first_actor.node);
    xm_arena_view_light_actor(rig, &xm_arena_second_actor, 1);
    xm_arena_node_draw_tree(xm_arena_second_actor.node);
    gte_SetRotMatrix(&camera);
    gte_SetTransMatrix(&camera);
    xm_arena_effect_emit_hit_sparks(layer->ot[xm_arena_draw_buffer_index], &rig->camera->view);
    xm_arena_effect_draw_lines_only(layer->ot[xm_arena_draw_buffer_index]);
    xm_arena_display_link_layer(layer);
    xm_arena_hud_link_overlay_tpage();
    xm_AddPrim(xm_arena_current_ot, &xm_arena_stage_backdrop_sprites[4 + xm_arena_draw_buffer_index]);
    xm_AddPrim(xm_arena_current_ot, &xm_arena_stage_backdrop_tpages[2 + xm_arena_draw_buffer_index]);
    xm_AddPrim(xm_arena_current_ot, &xm_arena_stage_backdrop_sprites[2 + xm_arena_draw_buffer_index]);
    xm_AddPrim(xm_arena_current_ot, &xm_arena_stage_backdrop_sprites[xm_arena_draw_buffer_index]);
    xm_AddPrim(xm_arena_current_ot, &xm_arena_stage_backdrop_tpages[xm_arena_draw_buffer_index]);
    return 0;
}
#endif

s32 func_800849E0(void *arg0) {
    s32 temp_s0;

    temp_s0 = *(s32 *)((s8*)(arg0) + 0x284);
    func_8008AC0C(temp_s0);
    func_80080D20(*(s32 *)((s8*)(((D_800928A0 * 4) + temp_s0)) + 4) + 4);
    func_8008AE1C(temp_s0);
    func_80086E24();
    return 0;
}


void func_80084A40(void *arg0) {
    func_8008AC0C(*(s32 *)((s8*)(arg0) + 0x284));
}


s32 func_80084A64(void *arg0) {
    s32 temp_s0;

    temp_s0 = *(s32 *)((s8*)(arg0) + 0x284);
    func_8008E3CC(*(s32 *)((s8*)(((D_800928A0 * 4) + temp_s0)) + 4) + 8, 0xC0, 0);
    func_80080D20(*(s32 *)((s8*)(((D_800928A0 * 4) + temp_s0)) + 4) + 4);
    func_8008AE1C(temp_s0);
    func_80086E24();
    return 0;
}


void func_80084AE0(void) {
    if (D_80092780 != 0) {
        func_8008E120();
        func_8007F258(D_80092938, 0);
        func_80080D20(D_80092938);
        func_8008E3CC(D_80092938, 0xC0, 1);
        func_8008BC04();
    }
}


void func_80084B48(void) {
    s16 temp_v0;

    if (D_80092780 != 0) {
        temp_v0 = D_80092780 - 4;
        D_80092780 = temp_v0;
        if (temp_v0 <= 0) {
            D_80092780 = 0;
            D_80092784 = 0;
            func_8003A838(D_80092948, 0x100, 0);
            D_8009292C = 0x100;
            func_8008E064();
            return;
        }
        func_8008E3CC(D_80092938, temp_v0, 1);
        return;
    }
    D_80092784 = 0;
}


void func_80084BEC(void *arg0) {
    s32 temp_s2;
    s32 temp_v0;
    void *temp_s1;

    temp_s2 = *(s32 *)((s8*)(*(void **)((s8*)(*(void **)((s8*)(*(void **)((s8*)(arg0) + 0x5C)) + 4)) + 4)) + 0x30);
    temp_s1 = func_80089C54();
    temp_v0 = func_80089FC4();
    func_80089E2C(temp_s1, temp_v0);
    func_8008A184(temp_v0, &D_80091FB0);
    func_80089C88(temp_s2, temp_s1);
    *(s16 *)((s8*)(temp_s1) + 0x46) = 0xC00;
    *(s16 *)((s8*)(temp_s1) + 0x44) = 0;
    *(s16 *)((s8*)(temp_s1) + 0x48) = 0x400;
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80084C88);

void func_80084FD0(void) {
    if ((D_80092784 != 0) && (D_80059488 & 1)) {
        func_8008E120();
    }
}


void func_80085014(void) {
    D_80092920 &= 0xFE;
    func_80083BB4(0);
    func_80083C0C(5);
    D_80099D9E = 1;
    D_80099D9D = 1;
    D_800928C8 = 6;
}


void func_80085070(void) {
    D_80099D9E = 0;
    D_80099D9D = 0;
    D_80092920 |= 1;
}

void ArchiveSetIndex(s32, s32);
extern s8 D_80097919;
extern s8 D_80099035;
extern s32 D_800927B4[];

void func_8008509C(s32 arg0, s32 arg1) {
    s32 *temp_s0;
    s32 *base;

    if (arg0 != 0) {
        D_80097919 = arg1;
    } else {
        D_80099035 = arg1;
    }
    base = D_800927B4;
    temp_s0 = &base[arg0];
    if (*temp_s0 != 0) {
        HeapFree((void *)*temp_s0);
        *temp_s0 = 0;
    }
    ArchiveSetIndex(0x30, 1);
    *temp_s0 = func_800891C0(arg1 + 2);
    ArchiveSetIndex(0x30, 0);
}

extern s32 D_800927B4[];
void func_80085134(s32 arg0) {
    ArchiveCdDataSync(0);
    if (D_800927B4[arg0] != 0) {
        HeapFree(D_800927B4[arg0]);
        D_800927B4[arg0] = 0;
    }
}

void func_8008518C(void *arg0, s32 arg1) {
    *(s32 *)((s8*)(arg0) + 4) = HeapAlloc(ArchiveDecodeAlignedSize(*(u16 *)((s8*)(arg0) + 0)), arg1);
}


void func_800851D4(void) {
    func_8003852C(D_800927C4);
    if (D_800917F0 != 0) {
        func_80039C4C(D_80092948);
        func_800399D4(D_80092948);
    }
    func_80088A40();
    ChangeGameState(1);
    DrawSync(0);
    Vsync(2);
    D_8005061C = 1;
    MainLoop(0);
}

s32 func_80085264(void) {
    s32 result = 0;
    if (D_80092794 == 5 || D_80092794 == 7) {
        result = 1;
    } else if (D_80092794 == 1) {
        if (D_800928C8 == 4 && *(u8*)&D_8005061C == 0) {
            result = 1;
        }
    }
    return result;
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800852C4);

void func_80085E34(void *arg0, void *arg1) {
    *(u16 *)((s8*)(arg1) + 0) = (u16) (*(u16 *)((s8*)(arg0) + 0) + 0x18);
    *(s16 *)((s8*)(arg1) + 2) = (s16) (*(u16 *)((s8*)(arg0) + 2) + 6);
    *(u16 *)((s8*)(arg1) + 0) = (u16) (*(u16 *)((s8*)(arg1) + 0) + 0x4F);
}


void func_80085E60(void *arg0, void *arg1) {
    *(u16 *)((s8*)(arg1) + 0) = (u16) (0x8B - *(u16 *)((s8*)(arg0) + 0));
    *(s16 *)((s8*)(arg1) + 2) = (s16) (0x20 - *(u16 *)((s8*)(arg0) + 2));
    *(u16 *)((s8*)(arg1) + 0) = (u16) (*(u16 *)((s8*)(arg1) + 0) + 0x4F);
}


void func_80085E90(s32 arg0, s16 *arg1, s32 arg2) {
    s16 var_v0;

    if (arg0 != 0) {
        var_v0 = 0xDA - arg2;
    } else {
        var_v0 = arg2 + 0x67;
    }
    *arg1 = var_v0;
}


void func_80085EAC(s32 arg0, s16 *arg1, s32 arg2) {
    s16 var_v0;

    if (arg0 != 0) {
        var_v0 = 0x20 - arg2;
    } else {
        var_v0 = arg2 + 6;
    }
    *arg1 = var_v0;
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_hud_build_overlay_buffer (src/decomp/src/menu/arena_stage_views_and_hud.c:1605). */
#ifndef XM_T_LINE_F4
#define XM_T_LINE_F4
typedef struct {
    u_long tag;
    u_char r0, g0, b0, code;
    short x0, y0;
    short x1, y1;
    short x2, y2;
    short x3, y3;
    u_long pad;
} LINE_F4;
#endif
#ifndef XM_T_POLY_F4
#define XM_T_POLY_F4
typedef struct {
    u_long tag;
    u_char r0, g0, b0, code;
    short x0, y0;
    short x1, y1;
    short x2, y2;
    short x3, y3;
} POLY_F4;
#endif
#ifndef XM_T_POLY_F3
#define XM_T_POLY_F3
typedef struct {
    u_long tag;
    u_char r0, g0, b0, code;
    short x0, y0;
    short x1, y1;
    short x2, y2;
} POLY_F3;
#endif
#ifndef XM_T_OverlayBuffer
#define XM_T_OverlayBuffer
typedef struct OverlayBuffer {
    DR_TPAGE tpage[2];   
    LINE_F4 frame[4];      
    POLY_F4 bars[6];       
    DR_TPAGE bar_tpage;  
    POLY_F4 bars_dim[6];   
    POLY_F4 bars_lit[6];   
    POLY_F3 arrows[2][3];  
    POLY_F4 marks[4];      
} OverlayBuffer;
#endif
#ifndef XM_T_DVECTOR
#define XM_T_DVECTOR
typedef struct {
    short vx, vy;
} DVECTOR;
#endif
#ifndef XM_S_arena_hud_build_overlay_buffer
#define XM_S_arena_hud_build_overlay_buffer
void xm_arena_hud_build_overlay_buffer(OverlayBuffer *buf) __asm__("func_80085EC8");
#endif
#ifndef XM_S_arena_hud_place_left_gauge_point
#define XM_S_arena_hud_place_left_gauge_point
void xm_arena_hud_place_left_gauge_point(DVECTOR *point, DVECTOR *out) __asm__("func_80085E34");
#endif
#ifndef XM_S_arena_hud_gauge_frame_layout
#define XM_S_arena_hud_gauge_frame_layout
extern DVECTOR xm_arena_hud_gauge_frame_layout[8] __asm__("D_800917F4");
#endif
#ifndef XM_S_arena_hud_place_right_gauge_point
#define XM_S_arena_hud_place_right_gauge_point
void xm_arena_hud_place_right_gauge_point(DVECTOR *point, DVECTOR *out) __asm__("func_80085E60");
#endif
#ifndef XM_S_MargePrim
#define XM_S_MargePrim
int xm_MargePrim(void *p0, void *p1) __asm__("func_80043F18");
#endif
void xm_arena_hud_build_overlay_buffer(OverlayBuffer *buf) __asm__("func_80085EC8");
void xm_arena_hud_build_overlay_buffer(OverlayBuffer *buf) {
    s32 i;

    xm_SetDrawTPage(&buf->tpage[0], 0, 1, xm_GetTPage(0, 2, 0, 0));
    xm_SetDrawTPage(&buf->tpage[1], 0, 0, xm_GetTPage(0, 1, 0, 0));
    setlen(&buf->frame[0], 6);
    *(u32 *)&buf->frame[0].r0 = 0x4C000000;
    buf->frame[0].pad = 0x55555555;
    setlen(&buf->frame[1], 6);
    *(u32 *)&buf->frame[1].r0 = 0x4C000000;
    buf->frame[1].pad = 0x55555555;
    setlen(&buf->frame[2], 6);
    *(u32 *)&buf->frame[2].r0 = 0x4C000000;
    buf->frame[2].pad = 0x55555555;
    setlen(&buf->frame[3], 6);
    *(u32 *)&buf->frame[3].r0 = 0x4C000000;
    buf->frame[3].pad = 0x55555555;
    xm_arena_hud_place_left_gauge_point(&xm_arena_hud_gauge_frame_layout[0], (DVECTOR *)&buf->frame[0].x0);
    xm_arena_hud_place_left_gauge_point(&xm_arena_hud_gauge_frame_layout[1], (DVECTOR *)&buf->frame[0].x1);
    xm_arena_hud_place_left_gauge_point(&xm_arena_hud_gauge_frame_layout[2], (DVECTOR *)&buf->frame[0].x2);
    xm_arena_hud_place_left_gauge_point(&xm_arena_hud_gauge_frame_layout[3], (DVECTOR *)&buf->frame[0].x3);
    xm_arena_hud_place_left_gauge_point(&xm_arena_hud_gauge_frame_layout[3], (DVECTOR *)&buf->frame[1].x0);
    xm_arena_hud_place_left_gauge_point(&xm_arena_hud_gauge_frame_layout[4], (DVECTOR *)&buf->frame[1].x1);
    xm_arena_hud_place_left_gauge_point(&xm_arena_hud_gauge_frame_layout[5], (DVECTOR *)&buf->frame[1].x2);
    xm_arena_hud_place_left_gauge_point(&xm_arena_hud_gauge_frame_layout[0], (DVECTOR *)&buf->frame[1].x3);
    xm_arena_hud_place_right_gauge_point(&xm_arena_hud_gauge_frame_layout[0], (DVECTOR *)&buf->frame[2].x0);
    xm_arena_hud_place_right_gauge_point(&xm_arena_hud_gauge_frame_layout[1], (DVECTOR *)&buf->frame[2].x1);
    xm_arena_hud_place_right_gauge_point(&xm_arena_hud_gauge_frame_layout[2], (DVECTOR *)&buf->frame[2].x2);
    xm_arena_hud_place_right_gauge_point(&xm_arena_hud_gauge_frame_layout[3], (DVECTOR *)&buf->frame[2].x3);
    xm_arena_hud_place_right_gauge_point(&xm_arena_hud_gauge_frame_layout[3], (DVECTOR *)&buf->frame[3].x0);
    xm_arena_hud_place_right_gauge_point(&xm_arena_hud_gauge_frame_layout[4], (DVECTOR *)&buf->frame[3].x1);
    xm_arena_hud_place_right_gauge_point(&xm_arena_hud_gauge_frame_layout[5], (DVECTOR *)&buf->frame[3].x2);
    xm_arena_hud_place_right_gauge_point(&xm_arena_hud_gauge_frame_layout[0], (DVECTOR *)&buf->frame[3].x3);
    xm_MargePrim(&buf->frame[0], &buf->frame[1]);
    xm_MargePrim(&buf->frame[2], &buf->frame[3]);
    buf->frame[0].x0 = 0x1D;
    buf->frame[2].x0 = 0x121;
    setlen(&buf->bars[0], 5);
    *(u32 *)&buf->bars[0].r0 = 0x280000FF;
    setlen(&buf->bars[1], 5);
    *(u32 *)&buf->bars[1].r0 = 0x280000FF;
    setlen(&buf->bars[2], 5);
    *(u32 *)&buf->bars[2].r0 = 0x280000FF;
    setlen(&buf->bars[3], 5);
    *(u32 *)&buf->bars[3].r0 = 0x280000FF;
    setlen(&buf->bars[4], 5);
    *(u32 *)&buf->bars[4].r0 = 0x280000FF;
    setlen(&buf->bars[5], 5);
    *(u32 *)&buf->bars[5].r0 = 0x280000FF;
    xm_arena_hud_place_left_gauge_point(&xm_arena_hud_gauge_frame_layout[0], (DVECTOR *)&buf->bars[0].x0);
    xm_arena_hud_place_left_gauge_point(&xm_arena_hud_gauge_frame_layout[7], (DVECTOR *)&buf->bars[0].x1);
    xm_arena_hud_place_left_gauge_point(&xm_arena_hud_gauge_frame_layout[5], (DVECTOR *)&buf->bars[0].x2);
    xm_arena_hud_place_left_gauge_point(&xm_arena_hud_gauge_frame_layout[4], (DVECTOR *)&buf->bars[0].x3);
    xm_arena_hud_place_left_gauge_point(&xm_arena_hud_gauge_frame_layout[7], (DVECTOR *)&buf->bars[1].x0);
    xm_arena_hud_place_left_gauge_point(&xm_arena_hud_gauge_frame_layout[6], (DVECTOR *)&buf->bars[1].x1);
    xm_arena_hud_place_left_gauge_point(&xm_arena_hud_gauge_frame_layout[4], (DVECTOR *)&buf->bars[1].x2);
    xm_arena_hud_place_left_gauge_point(&xm_arena_hud_gauge_frame_layout[3], (DVECTOR *)&buf->bars[1].x3);
    xm_arena_hud_place_left_gauge_point(&xm_arena_hud_gauge_frame_layout[6], (DVECTOR *)&buf->bars[2].x0);
    xm_arena_hud_place_left_gauge_point(&xm_arena_hud_gauge_frame_layout[1], (DVECTOR *)&buf->bars[2].x1);
    xm_arena_hud_place_left_gauge_point(&xm_arena_hud_gauge_frame_layout[3], (DVECTOR *)&buf->bars[2].x2);
    xm_arena_hud_place_left_gauge_point(&xm_arena_hud_gauge_frame_layout[2], (DVECTOR *)&buf->bars[2].x3);
    xm_arena_hud_place_right_gauge_point(&xm_arena_hud_gauge_frame_layout[0], (DVECTOR *)&buf->bars[3].x0);
    xm_arena_hud_place_right_gauge_point(&xm_arena_hud_gauge_frame_layout[7], (DVECTOR *)&buf->bars[3].x1);
    xm_arena_hud_place_right_gauge_point(&xm_arena_hud_gauge_frame_layout[5], (DVECTOR *)&buf->bars[3].x2);
    xm_arena_hud_place_right_gauge_point(&xm_arena_hud_gauge_frame_layout[4], (DVECTOR *)&buf->bars[3].x3);
    xm_arena_hud_place_right_gauge_point(&xm_arena_hud_gauge_frame_layout[7], (DVECTOR *)&buf->bars[4].x0);
    xm_arena_hud_place_right_gauge_point(&xm_arena_hud_gauge_frame_layout[6], (DVECTOR *)&buf->bars[4].x1);
    xm_arena_hud_place_right_gauge_point(&xm_arena_hud_gauge_frame_layout[4], (DVECTOR *)&buf->bars[4].x2);
    xm_arena_hud_place_right_gauge_point(&xm_arena_hud_gauge_frame_layout[3], (DVECTOR *)&buf->bars[4].x3);
    xm_arena_hud_place_right_gauge_point(&xm_arena_hud_gauge_frame_layout[6], (DVECTOR *)&buf->bars[5].x0);
    xm_arena_hud_place_right_gauge_point(&xm_arena_hud_gauge_frame_layout[1], (DVECTOR *)&buf->bars[5].x1);
    xm_arena_hud_place_right_gauge_point(&xm_arena_hud_gauge_frame_layout[3], (DVECTOR *)&buf->bars[5].x2);
    xm_arena_hud_place_right_gauge_point(&xm_arena_hud_gauge_frame_layout[2], (DVECTOR *)&buf->bars[5].x3);
    xm_SetDrawTPage(&buf->bar_tpage, 0, 1, xm_GetTPage(0, 1, 0, 0));
    xm_arena_copy_words(buf->bars_dim, buf->bars, sizeof(buf->bars));
    xm_arena_copy_words(buf->bars_lit, buf->bars, sizeof(buf->bars));
    for (i = 0; i < 6; i++) {
        setlen(&buf->bars_dim[i], 5);
        *(u32 *)&buf->bars_dim[i].r0 = 0x28806060;
    }
    for (i = 0; i < 6; i++) {
        setlen(&buf->bars_lit[i], 5);
        *(u32 *)&buf->bars_lit[i].r0 = 0x280000FF;
    }
    *(u32 *)&buf->arrows[0][0].x0 = 0x200014;
    *(u32 *)&buf->arrows[0][0].x1 = 0x20001C;
    *(u32 *)&buf->arrows[0][0].x2 = 0x28001C;
    *(u32 *)&buf->arrows[0][1].x0 = 0x200013;
    *(u32 *)&buf->arrows[0][1].x1 = 0x290013;
    *(u32 *)&buf->arrows[0][1].x2 = 0x29001B;
    *(u32 *)&buf->arrows[0][2].x0 = 0x320013;
    *(u32 *)&buf->arrows[0][2].x1 = 0x2A0013;
    *(u32 *)&buf->arrows[0][2].x2 = 0x2A001B;
    for (i = 0; i < 6; i++) {
        setlen(&buf->arrows[0][i], 4);
        *(u32 *)&buf->arrows[0][i].r0 = 0x2000FF00;
        setlen(&buf->arrows[1][i], 4);
        *(u32 *)&buf->arrows[1][i].r0 = 0x2000FF00;
        buf->arrows[1][i].y0 = buf->arrows[0][i].y0;
        buf->arrows[1][i].y1 = buf->arrows[0][i].y1;
        buf->arrows[1][i].y2 = buf->arrows[0][i].y2;
        buf->arrows[1][i].x0 = 0x140 - buf->arrows[0][i].x0;
        buf->arrows[1][i].x1 = 0x140 - buf->arrows[0][i].x1;
        buf->arrows[1][i].x2 = 0x140 - buf->arrows[0][i].x2;
    }
    buf->arrows[1][1].y0--;
    buf->arrows[1][1].x2--;
    setlen(&buf->marks[0], 5);
    *(u32 *)&buf->marks[0].r0 = 0x28000000;
    setlen(&buf->marks[1], 5);
    *(u32 *)&buf->marks[1].r0 = 0x28000000;
    buf->marks[0].x0 = buf->marks[0].x2 = 0x1E;
    buf->marks[0].x1 = 0x63;
    buf->marks[0].x3 = 0x5E;
    buf->marks[0].y0 = buf->marks[0].y1 = 9;
    buf->marks[0].y2 = buf->marks[0].y3 = 0x14;
    buf->marks[1].x0 = buf->marks[1].x2 = 0x122;
    buf->marks[1].x1 = 0xE3;
    buf->marks[1].x3 = 0xDE;
    buf->marks[1].y0 = buf->marks[1].y1 = 0x13;
    buf->marks[1].y2 = buf->marks[1].y3 = 0x1E;
    setlen(&buf->marks[2], 5);
    *(u32 *)&buf->marks[2].r0 = 0x280000FF;
    *(u32 *)&buf->marks[2].x0 = 0x320006;
    *(u32 *)&buf->marks[2].x1 = 0x36000A;
    *(u32 *)&buf->marks[2].x2 = 0x4C0006;
    *(u32 *)&buf->marks[2].x3 = 0x48000A;
    setlen(&buf->marks[3], 5);
    *(u32 *)&buf->marks[3].r0 = 0x280000FF;
    *(u32 *)&buf->marks[3].x0 = 0x32013A;
    *(u32 *)&buf->marks[3].x1 = 0x360136;
    *(u32 *)&buf->marks[3].x2 = 0x4C013A;
    *(u32 *)&buf->marks[3].x3 = 0x480136;
}
#endif
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800864B4);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800866D4);
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_hud_build_packets (src/decomp/src/menu/arena_stage_views_and_hud.c:1817). */
#ifndef XM_T_SpritePair
#define XM_T_SpritePair
typedef struct {
    SPRT s[2];
} SpritePair;
#endif
#ifndef XM_T_Hud
#define XM_T_Hud
typedef struct {
    POLY_FT4 name_l[2];    
    POLY_FT4 name_r[2];    
    SpritePair icon[4];   
    SpritePair gauge[4];  
    POLY_FT4 bar_l[2];     
    POLY_FT4 bar_r[2];     
} Hud;
#endif
#ifndef XM_S_arena_hud_build_packets
#define XM_S_arena_hud_build_packets
void xm_arena_hud_build_packets(MenuImageFile *files) __asm__("func_800868E0");
#endif
#ifndef XM_S_arena_hud_packets
#define XM_S_arena_hud_packets
extern Hud xm_arena_hud_packets __asm__("D_80095698");
#endif
#ifndef XM_S_arena_hud_build_tim_quad
#define XM_S_arena_hud_build_tim_quad
void xm_arena_hud_build_tim_quad(TIM_IMAGE *tim, s32 x, s32 y, POLY_FT4 *quad, s32 depth) __asm__("func_800864B4");
#endif
#ifndef XM_S_arena_hud_build_mirrored_tim_quad
#define XM_S_arena_hud_build_mirrored_tim_quad
void xm_arena_hud_build_mirrored_tim_quad(TIM_IMAGE *tim, s32 x, s32 y, POLY_FT4 *quad, s32 depth) __asm__("func_800866D4");
#endif
#ifndef XM_S_arena_hud_tpages
#define XM_S_arena_hud_tpages
extern DR_TPAGE xm_arena_hud_tpages[4] __asm__("D_80095918");
#endif
#ifndef XM_S_arena_hud_left_charge_bar_v
#define XM_S_arena_hud_left_charge_bar_v
extern u8 xm_arena_hud_left_charge_bar_v __asm__("D_80092860");
#endif
#ifndef XM_S_arena_hud_unread_right_charge_bar_v
#define XM_S_arena_hud_unread_right_charge_bar_v
extern u8 xm_arena_hud_unread_right_charge_bar_v __asm__("D_80092864");
#endif
#ifndef XM_S_arena_hud_gauge_palette
#define XM_S_arena_hud_gauge_palette
extern u16 xm_arena_hud_gauge_palette[16] __asm__("D_80091814");
#endif
void func_800868E0(MenuImageFile *files) {
    TIM_IMAGE tim;
    RECT rect;
    s16 *clut;
    Hud *hud = &xm_arena_hud_packets;
    SpritePair *pair;
    POLY_FT4 *bar;

    xm_OpenTIM(files->name);
    xm_ReadTIM(&tim);
    clut = (s16 *)tim.caddr;
    clut[0] = 0;
    clut[1] = 0x8000;
    xm_LoadImage(tim.crect, tim.caddr);
    pair = hud->icon;
    xm_LoadImage(tim.prect, tim.paddr);
    xm_arena_hud_build_tim_quad(&tim, 6, 7, hud->name_l, 0);
    xm_arena_hud_build_mirrored_tim_quad(&tim, 0x13A - tim.prect->w * 4, 7, hud->name_r, 0);
    xm_SetDrawTPage(&xm_arena_hud_tpages[0], 0, 1, xm_GetTPage(1, 0, 0x380, 0x100));
    xm_arena_hud_tpages[1] = xm_arena_hud_tpages[0];
    xm_SetDrawTPage(&xm_arena_hud_tpages[2], 0, 1, xm_GetTPage(0, 0, 0x380, 0x100));
    xm_arena_hud_tpages[3] = xm_arena_hud_tpages[2];
    setlen(&pair[0].s[0], 4);
    pair[0].s[0].code = 0x65;
    *(u32 *)&pair[0].s[0].x0 = 0x90007;
    *(u16 *)&pair[0].s[0].u0 = 0;
    *(u32 *)&pair[0].s[0].w = 0x160016;
    pair[0].s[0].clut = xm_GetClut(0, 0x1F6);
    xm_arena_hud_packets.icon[2] = xm_arena_hud_packets.icon[1] = pair[0];
    pair = &xm_arena_hud_packets.icon[2];
    pair[0].s[0].x0 = 0x123;
    pair[0].s[0].u0 = 0x20;
    pair[0].s[0].clut = xm_GetClut(0, 0x1F7);
    pair[1] = pair[0];
    xm_OpenTIM(files->bar);
    xm_ReadTIM(&tim);
    bar = xm_arena_hud_packets.bar_l;
    pair = xm_arena_hud_packets.gauge;
    xm_LoadImage(tim.prect, tim.paddr);
    xm_arena_hud_build_tim_quad(&tim, 6, 0x20, bar, 0);
    xm_arena_hud_build_mirrored_tim_quad(&tim, 0x13A - tim.prect->w * 4, 0x20, xm_arena_hud_packets.bar_r, 0);
    xm_arena_hud_left_charge_bar_v = xm_arena_hud_packets.bar_l[0].v0;
    xm_arena_hud_unread_right_charge_bar_v = xm_arena_hud_packets.bar_r[0].v0;
    *(u32 *)&xm_arena_hud_packets.bar_l[0].r0 = 0x2C000080;
    setlen(&xm_arena_hud_packets.bar_l[0], 9);
    *(u32 *)&xm_arena_hud_packets.bar_l[1].r0 = 0x2C000080;
    setlen(&xm_arena_hud_packets.bar_l[1], 9);
    *(u32 *)&xm_arena_hud_packets.bar_r[0].r0 = 0x2C000080;
    setlen(&xm_arena_hud_packets.bar_r[0], 9);
    *(u32 *)&xm_arena_hud_packets.bar_r[1].r0 = 0x2C000080;
    setlen(&xm_arena_hud_packets.bar_r[1], 9);
    setlen(&pair[0].s[0], 4);
    pair[0].s[0].code = 0x65;
    *(u32 *)&pair[0].s[0].w = 0x80040;
    *(u16 *)&pair[0].s[0].u0 = 0x80;
    xm_arena_hud_packets.bar_l[1].clut = xm_arena_hud_packets.name_l[0].clut;
    xm_arena_hud_packets.bar_l[0].clut = xm_arena_hud_packets.name_l[0].clut;
    xm_arena_hud_packets.bar_r[1].clut = xm_arena_hud_packets.name_l[0].clut;
    xm_arena_hud_packets.bar_r[0].clut = xm_arena_hud_packets.name_l[0].clut;
    pair[0].s[0].clut = xm_GetClut(0x3A0, 0x110);
    *(u32 *)&pair[0].s[0].x0 = 0xB001E;
    pair[2] = pair[0];
    pair = &xm_arena_hud_packets.gauge[2];
    *(u16 *)&pair[0].s[0].u0 = 0x880;
    *(u32 *)&pair[0].s[0].x0 = 0x1500E3;
    pair[-1] = pair[-2];
    xm_arena_hud_packets.gauge[3] = xm_arena_hud_packets.gauge[2];
    rect.x = 0x3A0;
    rect.y = 0x110;
    rect.w = 0x10;
    rect.h = 1;
    xm_LoadImage(&rect, (u_long *)xm_arena_hud_gauge_palette);
}
#endif
extern s32 D_8009A300;
void func_80086E24(void) {
    AddPrim(D_80092938, (D_800928A0 * 0x310) + (u8 *)&D_8009A300);
}
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_hud_draw_gauge_bar (src/decomp/src/menu/arena_stage_views_and_hud.c:1898). */
#ifndef XM_T_GaugeBar
#define XM_T_GaugeBar
typedef struct {
    POLY_F4 parts[3];
} GaugeBar;
#endif
#ifndef XM_S_arena_hud_draw_gauge_bar
#define XM_S_arena_hud_draw_gauge_bar
void xm_arena_hud_draw_gauge_bar(void *ot, GaugeBar *bar, s32 value, s32 mirrored) __asm__("func_80086E70");
#endif
#ifndef XM_S_arena_hud_set_gauge_x
#define XM_S_arena_hud_set_gauge_x
void xm_arena_hud_set_gauge_x(s32 mirrored, s16 *out, s32 x) __asm__("func_80085E90");
#endif
#ifndef XM_S_arena_hud_set_gauge_y
#define XM_S_arena_hud_set_gauge_y
void xm_arena_hud_set_gauge_y(s32 mirrored, s16 *out, s32 y) __asm__("func_80085EAC");
#endif
void func_80086E70(void *ot, GaugeBar *bar, s32 value, s32 mirrored) {
    s32 over;

    if (value >= 0x38) {
        xm_arena_hud_set_gauge_x(mirrored, &bar->parts[0].x1, 0x38);
        xm_arena_hud_set_gauge_x(mirrored, &bar->parts[0].x3, 0x34);
        xm_AddPrim(ot, &bar->parts[0]);
    } else {
        xm_arena_hud_set_gauge_x(mirrored, &bar->parts[0].x1, value);
        xm_arena_hud_set_gauge_x(mirrored, &bar->parts[0].x3, value - 4);
        xm_AddPrim(ot, &bar->parts[0]);
        return;
    }
    if (value >= 0x48) {
        xm_arena_hud_set_gauge_x(mirrored, &bar->parts[1].x1, 0x48);
        xm_arena_hud_set_gauge_x(mirrored, &bar->parts[1].x3, 0x40);
        xm_arena_hud_set_gauge_y(mirrored, &bar->parts[1].y3, 0x10);
        xm_AddPrim(ot, &bar->parts[1]);
    } else {
        xm_arena_hud_set_gauge_x(mirrored, &bar->parts[1].x1, value);
        over = value - 0x38;
        xm_arena_hud_set_gauge_x(mirrored, &bar->parts[1].x3, value - (over / 4 + 4));
        xm_arena_hud_set_gauge_y(mirrored, &bar->parts[1].y3, over / 2 + 8);
        xm_AddPrim(ot, &bar->parts[1]);
        return;
    }
    xm_arena_hud_set_gauge_x(mirrored, &bar->parts[2].x1, value);
    xm_arena_hud_set_gauge_x(mirrored, &bar->parts[2].x3, value - 8);
    xm_AddPrim(ot, &bar->parts[2]);
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_hud_color_charge_mark (src/decomp/src/menu/arena_stage_views_and_hud.c:1931). */
#ifndef XM_T_ModelTable
#define XM_T_ModelTable
typedef struct ModelTable {
    SpriteModel **models;
    u32 count;
} ModelTable;
#endif
#ifndef XM_T_EffectEntry
#define XM_T_EffectEntry
typedef struct {
    u8 used;
    u8 field1;     
    u8 kind;       
    u8 tag;        
    u16 params[6]; 
    u16 time;      
    u16 duration;  
} EffectEntry;
#endif
#ifndef XM_T_ModelPart
#define XM_T_ModelPart
typedef struct ModelPart {
    struct ModelPart *parent; 
    u8 dirty;         
    u8 rotate;        
    u8 yxz;           
    u8 visible;       
    u16 modelId;      
    u16 index;        
    MATRIX transform; 
    MATRIX world;     
    s16 scale[3];     
    u16 field52;      


    SVECTOR rotation;        
    s32 translation[3];      
    void *packets[2];        
    EffectEntry *effects[3]; 
} ModelPart;
#endif
#ifndef XM_T_EntryTable
#define XM_T_EntryTable
typedef struct EntryTable {
    s32 count;
    s32 *entries;
} EntryTable;
#endif
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
#ifndef XM_T_SoundOwner
#define XM_T_SoundOwner
typedef struct SoundOwner {
    u8 pad0[8];
    SoundBank *bank;        
    void *end;              
} SoundOwner;
#endif
#ifndef XM_T_EffectSprite
#define XM_T_EffectSprite
typedef struct {
    s16 x0, y0, z0, pad06;
    s16 x1, y1, z1;
    s16 projected; 
    s16 x2, y2, z2;
    s16 age;       
    s16 x3, y3, z3;
    s16 lifetime;  
    u16 color[3];  
    s16 fade[3];   
    POLY_FT4 packets[2]; 
} EffectSprite;
#endif
#ifndef XM_T_SpritePool
#define XM_T_SpritePool
typedef struct SpritePool {
    EffectSprite *records;
    s16 count;
    s16 next;
} SpritePool;
#endif
#ifndef XM_T_ColorFade
#define XM_T_ColorFade
typedef struct ColorFade {
    s16 id;           
    u8 solid;         
    u8 semiTrans;     
    SpritePool *pool; 
    EffectSprite *sprite; 
    SVECTOR ends[2];  
    union {
        struct {
            DVECTOR first[8];
            DVECTOR second[8];
        } screen;
        struct {
            VECTOR first[2];
            VECTOR second[2];
        } world;
    } history; 
    s16 time;     
    s16 count;    
    s16 max;      
    s16 duration; 
    s16 color[3]; 
    s16 step[3];  
} ColorFade;
#endif
#ifndef XM_T_SurfaceEntry
#define XM_T_SurfaceEntry
typedef struct {
    s16 h0, h2, h4, h6, h8, hA, hC, hE;
} SurfaceEntry;
#endif
#ifndef XM_T_SurfacePoint
#define XM_T_SurfacePoint
typedef struct {
    s16 length;
    s16 sag;
    s16 pos[3];
    u16 normalCount; 
    s32 normal[3];   
} SurfacePoint;
#endif
#ifndef XM_T_POLY_GT3
#define XM_T_POLY_GT3
typedef struct {
    u_long tag;
    u_char r0, g0, b0, code;
    short x0, y0;
    u_char u0, v0;
    u_short clut;
    u_char r1, g1, b1, p1;
    short x1, y1;
    u_char u1, v1;
    u_short tpage;
    u_char r2, g2, b2, p2;
    short x2, y2;
    u_char u2, v2;
    u_short pad2;
} POLY_GT3;
#endif
#ifndef XM_T_SurfacePoly
#define XM_T_SurfacePoly
typedef struct {
    s16 index[3];
    u8 pad6[2];
    POLY_GT3 prim[2];
} SurfacePoly;
#endif
#ifndef XM_T_Surface
#define XM_T_Surface
typedef struct Surface {
    u16 h0;
    u8 pad2[2];
    s16 rings;              
    s16 polys;              
    s16 points;             
    s16 entryCount;         
    u8 b[6];                
    u8 pad12[2];
    SVECTOR *centres;       
    SurfaceEntry *entries;  
    SurfacePoint **strands; 
    SurfacePoly *polyList;  
} Surface;
#endif
#ifndef XM_T_ColorRow
#define XM_T_ColorRow
typedef struct {
    u16 c[3];
} ColorRow;
#endif
#ifndef XM_T_FrameCurve
#define XM_T_FrameCurve
typedef s16 (*FrameCurve)();
#endif
#ifndef XM_T_ImageAnim
#define XM_T_ImageAnim
typedef struct ImageAnim {
    struct ImageAnim *target; 
    u16 *pixels;              
    u16 *pixels2;             
    u16 *work;                
    u8 mode;                  
    u8 dirty;                 
    s16 size;                 
    u16 time;                 
    u16 speed;                
    u16 frame;                
    u16 active;               
    ColorRow *colors;         
    s16 divisor;              
    s16 base;                 
    FrameCurve curve;         
    RECT rect;                
} ImageAnim;
#endif
#ifndef XM_S_arena_hud_color_charge_mark
#define XM_S_arena_hud_color_charge_mark
s32 xm_arena_hud_color_charge_mark(Actor *actor, POLY_F4 *packet) __asm__("func_80086FF8");
#endif
#ifndef XM_S_arena_brain_check_special_charge
#define XM_S_arena_brain_check_special_charge
s32 xm_arena_brain_check_special_charge(Actor *actor, s32 check) __asm__("func_8008F530");
#endif
s32 func_80086FF8(Actor *actor, POLY_F4 *packet) {
    if (xm_arena_brain_check_special_charge(actor, 0)) {
        return 0;
    }
    if (xm_arena_brain_check_special_charge(actor, 1)) {
        setlen(packet, 5);
        *(u32 *)&packet->r0 = 0x2800FFFF;
    } else {
        setlen(packet, 5);
        *(u32 *)&packet->r0 = 0x280000FF;
    }
    return 1;
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_hud_draw (src/decomp/src/menu/arena_stage_views_and_hud.c:1950). */
#ifndef XM_T_xm_Node
#define XM_T_xm_Node
typedef struct xm_Node {
    s32 type;          
    void *data;        
    void (*callback)(struct xm_Node *node); 
    MATRIX view;       
    SVECTOR offset;    
    VECTOR position;   
    SVECTOR angles;    
    MATRIX unk4C;      
    MATRIX unk6C;      
    struct xm_Node *parent; 
    struct xm_Node *next;   
    struct xm_Node *child;  
    s32 unk98;
} xm_Node;
#endif
#ifndef XM_T_xm_MoveSlot
#define XM_T_xm_MoveSlot
typedef struct xm_MoveSlot {
    u8 unk0[0x3];
    u8 usable;
} xm_MoveSlot;
#endif
#ifndef XM_T_xm_Trail
#define XM_T_xm_Trail
typedef struct {
    VECTOR a;          
    VECTOR a_prev;     
    VECTOR b;          
    VECTOR b_prev;     
    s16 effect;        
    u8 unk42;
    u8 unk43;
    u32 unk44_0 : 1;
    u32 flip : 1;
    u32 unk44_2 : 6;
    u32 state : 8;     
    u32 unk46 : 8;
    u32 unk47 : 8;
    u8 frame;          
    u8 unk49[0x3];
    s32 style;         
    u8 *unk50;
} xm_Trail;
#endif
#ifndef XM_T_xm_Shot
#define XM_T_xm_Shot
typedef struct {
    VECTOR pos;      
    VECTOR prev;     
    SVECTOR velocity; 
    SVECTOR dir;     
    u8 active;       
    u8 speed;
    u8 look;         
    u8 steer;        
    s32 dist;        
    s32 unk38;
    s16 unk3C;
    s16 homing;      
    s16 life;        
    u8 unk42[0x2];
} xm_Shot;
#endif
#ifndef XM_T_xm_SceneHeader
#define XM_T_xm_SceneHeader
typedef struct xm_SceneHeader {
    u8 core_part;    
    u8 foot_a_part;  
    u8 foot_b_part;
    u8 unk3;
    s16 foot_a_vertex;
    s16 foot_b_vertex;
    s16 core_vertex;
    u8 unkA[0x2];
    u16 unkC;
    u8 unkE;
    u8 unkF;
    u8 unk10[3];
    u8 unk13;
    s16 unk14;
    s16 unk16;
    s16 unk18;
    s16 unk1A;
    s16 unk1C;
    u16 unk1E;
    u16 unk20;       
    u8 unk22[0x2];
    s16 unk24;       
    s16 unk26;
    s16 unk28;
    s16 unk2A;
    s16 unk2C;       
    u8 unk2E[0x2];
    s32 unk30;       
} xm_SceneHeader;
#endif
#ifndef XM_T_xm_Pose
#define XM_T_xm_Pose
typedef struct {
    s16 x, y, z;
    u16 flags;       
    s16 unk8;
    u16 unkA;        
} xm_Pose;
#endif
#ifndef XM_T_xm_Move
#define XM_T_xm_Move
typedef struct {
    u8 unk0[0x6];
    u16 flags;       
    u8 anim;
    u8 unk9;
    u8 unkA;
} xm_Move;
#endif
#ifndef XM_T_xm_Brain
#define XM_T_xm_Brain
typedef struct xm_Brain {
    struct xm_Actor *owner;
    s16 timer;          
    u8 unk6;
    u8 unk7;
    u8 mode;            
    u8 unk9;
    s16 unkA;
    s16 unkC;
    u8 unkE;
    u8 unkF;
    s32 unk10;          
    s32 unk14;
    s32 unk18;          
    s32 unk1C;          
    s32 unk20;
    s32 unk24;          
    s32 unk28;
    u32 roll : 8;       
    u32 unk2C_8 : 1;
    u32 unk2C_9 : 1;
    u32 unk2C_10 : 1;
    u32 unk2C_11 : 1;
    u32 unk2C_12 : 1;
    u32 defending : 1;  
    u32 unk2C_14 : 2;
    u32 unk2E : 8;      
    u32 unk2C_24 : 8;
    s16 unk30;
} xm_Brain;
#endif
#ifndef XM_T_xm_MoveList
#define XM_T_xm_MoveList
typedef struct xm_MoveList {
    s16 base;       
    u8 unk2[0x2];
    s16 level;      
    u8 tendency[4]; 
    u8 learned[14]; 
    u8 unk18;       
    u8 unk19[0x7];
} xm_MoveList;
#endif
#ifndef XM_T_xm_Actor
#define XM_T_xm_Actor
typedef struct xm_Actor {
    VECTOR pos;          
    VECTOR velocity;     
    VECTOR push;         
    VECTOR unk30;        
    s32 unk40;           
    s32 unk44;           
    s32 state;           
    u8 anim;             
    u8 unk4D;
    u8 unk4E;
    u8 unk4F;
    s16 event_frame;     
    u8 unk52;
    u8 unk53;
    s32 angle;           
    s32 target_angle;    
    xm_Node *node;          
    void *object;        
    u8 unk64[0x8];
    VECTOR start;        
    s32 unk7C;
    struct xm_MoveSlot *move_slots; 
    u8 *unk84;
    s32 unk88;
    s32 unk8C;
    s32 unk90;
    s32 unk94;
    s32 accel;           
    s32 brake;           
    u8 unkA0[0x8];
    s32 unkA8;
    u8 unkAC[0x4];
    s32 floor_y;         
    s16 hp;              
    s16 charge;          
    s16 unkB8;
    s16 unkBA;
    s16 max_hp;          
    s16 unkBE;           
    u8 unkC0;
    u8 unkC1;
    u8 level;            
    u8 unkC3;
    u8 unkC4;
    u8 unkC5;
    u8 unkC6;            
    u8 unkC7;
    s16 unkC8;
    s16 unkCA;
    s16 unkCC;
    s16 unkCE;
    u32 flags;           
    u32 unkD4;
    struct xm_Actor *opponent; 
    u8 unkDC[0xC];
    s32 unkE8;
    u8 unkEC[0x4];
    s16 unkF0;           
    s16 unkF2;
    s32 unkF4;
    u8 unkF8[0x4];
    s32 unkFC;           
    s32 unk100;
    xm_Trail trails[16];    
    s32 unk644;
    s32 unk648;          
    xm_Shot shots[9];       
    u8 unk8B0[0x44];
    s32 nearest_dist;    
    xm_Shot *nearest_shot;  
    xm_SceneHeader *header; 
    u8 *unk900;
    u8 *visible;         
    u8 visible_count;
    u8 model_id;         
    u8 kind;             
    u8 unk90B;
    s16 unk90C;          
    u8 hold_anim;        
    u8 unk90F;
    u8 unk910;
    u8 move_count;       
    u8 parts_b;          
    u8 unk913;
    s16 unk914;
    s16 unk916;
    u8 glow;             
    u8 unk919[0x3];
    VECTOR hit_point;    
    VECTOR unk92C;       
    VECTOR home;         
    VECTOR core;         
    VECTOR start_home;   
    s16 foot_b_y;        
    s16 foot_a_y;
    s32 unk970;
    VECTOR hit_from;     
    u8 unk984[0x14];
    s16 unk998;
    s16 unk99A;
    s16 anim_speed;      
    s16 unk99E;
    u8 inputs[32];       
    u8 input_head;
    u8 input_tail;
    u8 input_count;
    u8 unk9C3;
    u8 unk9C4[0x8];
    u8 unk9CC[0xC00];    
    xm_Pose *pose;          
    xm_Move *move;          
    CVECTOR colour;        
    u8 unk15D8[0x10];
    s16 unk15E8;
    s16 unk15EA;
    s16 unk15EC;
    s16 unk15EE;
    s16 unk15F0;
    s16 unk15F2;
    s16 unk15F4;
    s16 unk15F6;
    s16 unk15F8;
    u8 unk15FA[0x2];
    struct xm_Brain *brain; 
    struct xm_MoveList *moves; 
    POLY_FT4 backdrop[2]; 
    s32 unk1654;
    s32 unk1658;
    s32 unk165C;
    s32 unk1660;
    u8 *sounds;          
    s16 unk1668;
} xm_Actor;
#endif
#ifndef XM_T_xm_OverlayBuffer
#define XM_T_xm_OverlayBuffer
typedef struct xm_OverlayBuffer {
    DR_TPAGE tpage[2];   
    LINE_F4 frame[4];      
    POLY_F4 bars[6];       
    DR_TPAGE bar_tpage;  
    POLY_F4 bars_dim[6];   
    POLY_F4 bars_lit[6];   
    POLY_F3 arrows[2][3];  
    POLY_F4 marks[4];      
} xm_OverlayBuffer;
#endif
#ifndef XM_T_xm_SpritePair
#define XM_T_xm_SpritePair
typedef struct {
    SPRT s[2];
} xm_SpritePair;
#endif
#ifndef XM_T_xm_Hud
#define XM_T_xm_Hud
typedef struct {
    POLY_FT4 name_l[2];    
    POLY_FT4 name_r[2];    
    xm_SpritePair icon[4];   
    xm_SpritePair gauge[4];  
    POLY_FT4 bar_l[2];     
    POLY_FT4 bar_r[2];     
} xm_Hud;
#endif
#ifndef XM_T_xm_GaugeBar
#define XM_T_xm_GaugeBar
typedef struct {
    POLY_F4 parts[3];
} xm_GaugeBar;
#endif
#ifndef XM_S_arena_hud_overlay_buffers
#define XM_S_arena_hud_overlay_buffers
extern xm_OverlayBuffer xm_arena_hud_overlay_buffers[2] __asm__("D_8009A2F8");
#endif
#ifndef XM_S_pad_vblank_count
#define XM_S_pad_vblank_count
extern s32 xm_pad_vblank_count __asm__("D_80059488");
#endif
void func_80087068(xm_Actor *left, xm_Actor *right) {
    xm_OverlayBuffer *buf;
    POLY_FT4 *bar;
    s32 n;
    s32 level;

    buf = &xm_arena_hud_overlay_buffers[xm_arena_draw_buffer_index];
    n = left->unkF2;
    if (n > 0) {
        xm_AddPrim(xm_arena_current_ot, &buf->arrows[0][0]);
    }
    if (n > 1) {
        xm_AddPrim(xm_arena_current_ot, &buf->arrows[0][1]);
    }
    if (n > 2) {
        xm_AddPrim(xm_arena_current_ot, &buf->arrows[0][2]);
    }
    n = right->unkF2;
    if (n > 0) {
        xm_AddPrim(xm_arena_current_ot, &buf->arrows[1][0]);
    }
    if (n > 1) {
        xm_AddPrim(xm_arena_current_ot, &buf->arrows[1][1]);
    }
    if (n > 2) {
        xm_AddPrim(xm_arena_current_ot, &buf->arrows[1][2]);
    }
    if (xm_arena_hud_color_charge_mark(left, &buf->marks[2])) {
        xm_AddPrim(xm_arena_current_ot, &buf->marks[2]);
    }
    if (xm_arena_hud_color_charge_mark(right, &buf->marks[3])) {
        xm_AddPrim(xm_arena_current_ot, &buf->marks[3]);
    }
    xm_AddPrim(xm_arena_current_ot, &xm_arena_hud_packets.name_l[xm_arena_draw_buffer_index]);
    xm_AddPrim(xm_arena_current_ot, &xm_arena_hud_packets.name_r[xm_arena_draw_buffer_index]);
    xm_AddPrim(xm_arena_current_ot, &xm_arena_hud_packets.icon[xm_arena_draw_buffer_index]);
    xm_AddPrim(xm_arena_current_ot, &xm_arena_hud_packets.icon[2 + xm_arena_draw_buffer_index]);
    xm_AddPrim(xm_arena_current_ot, &xm_arena_hud_tpages[xm_arena_draw_buffer_index]);
    xm_AddPrim(xm_arena_current_ot, &xm_arena_hud_packets.gauge[xm_arena_draw_buffer_index]);
    xm_AddPrim(xm_arena_current_ot, &xm_arena_hud_packets.gauge[2 + xm_arena_draw_buffer_index]);
    xm_AddPrim(xm_arena_current_ot, &xm_arena_hud_tpages[2 + xm_arena_draw_buffer_index]);
    xm_AddPrim(xm_arena_current_ot, &xm_arena_hud_packets.bar_r[xm_arena_draw_buffer_index]);
    xm_AddPrim(xm_arena_current_ot, &xm_arena_hud_packets.bar_l[xm_arena_draw_buffer_index]);
    xm_AddPrim(xm_arena_current_ot, &buf->marks[0]);
    xm_AddPrim(xm_arena_current_ot, &buf->marks[1]);
    xm_AddPrim(xm_arena_current_ot, &buf->frame[0]);
    xm_AddPrim(xm_arena_current_ot, &buf->frame[2]);
    xm_arena_hud_draw_gauge_bar(xm_arena_current_ot, (xm_GaugeBar *)&buf->bars[0], xm_arena_first_actor.unkC0, 0);
    xm_arena_hud_draw_gauge_bar(xm_arena_current_ot, (xm_GaugeBar *)&buf->bars[3], xm_arena_second_actor.unkC0, 1);
    n = left->charge >> 6;
    bar = &xm_arena_hud_packets.bar_l[xm_arena_draw_buffer_index];
    if (n > 0x38) {
        bar->r0 = xm_pad_vblank_count << 3;
    } else {
        bar->r0 = 0xFF;
    }
    bar->y0 = bar->y1 = 0x60 - n;
    bar->v0 = bar->v1 = xm_arena_hud_left_charge_bar_v - (n - 0x40);
    n = right->charge >> 6;
    bar = &xm_arena_hud_packets.bar_r[xm_arena_draw_buffer_index];
    if (n > 0x38) {
        bar->r0 = xm_pad_vblank_count << 3;
    } else {
        bar->r0 = 0xFF;
    }
    bar->y0 = bar->y1 = 0x60 - n;
    bar->v0 = bar->v1 = xm_arena_hud_left_charge_bar_v - (n - 0x40);
    if (left->level != 0) {
        level = 0x100 - left->level;
        for (n = 0; n < 3; n++) {
            buf->bars_lit[n].r0 = ((level * 3) >> 3) + ((left->level * 255) >> 8);
            buf->bars_lit[n].g0 = (level * 3) >> 3;
            buf->bars_lit[n].b0 = level >> 1;
        }
        xm_arena_hud_draw_gauge_bar(xm_arena_current_ot, (xm_GaugeBar *)&buf->bars_lit[0], left->unkC1, 0);
    }
    if (right->level != 0) {
        level = 0x100 - right->level;
        for (n = 3; n < 6; n++) {
            buf->bars_lit[n].r0 = ((level * 3) >> 3) + ((right->level * 255) >> 8);
            buf->bars_lit[n].g0 = (level * 3) >> 3;
            buf->bars_lit[n].b0 = level >> 1;
        }
        xm_arena_hud_draw_gauge_bar(xm_arena_current_ot, (xm_GaugeBar *)&buf->bars_lit[3], right->unkC1, 1);
    }
    for (n = 0; n < 6; n++) {
        xm_AddPrim(xm_arena_current_ot, &buf->bars_dim[n]);
    }
    xm_AddPrim(xm_arena_current_ot, buf);
}
#endif
extern u8 D_8009A2F8[];
void func_80085EC8(void*);
void func_800875EC(void) {
    struct Record { u32 words[0xC4]; };
    struct Record* record = (struct Record*)D_8009A2F8;
    func_80085EC8(record);
    record[1] = record[0];
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_stage_clear_row_spans (src/decomp/src/menu/arena_stage_views_and_hud.c:2048). */
#ifndef XM_S_arena_stage_span_right_columns
#define XM_S_arena_stage_span_right_columns
extern u8 *xm_arena_stage_span_right_columns __asm__("D_800927CC");
#endif
#ifndef XM_S_arena_stage_span_left_columns
#define XM_S_arena_stage_span_left_columns
extern u8 *xm_arena_stage_span_left_columns __asm__("D_800927D0");
#endif
void func_80087650(void) {
    s32 row;

    for (row = 0; row < 0x80; row++) {
        xm_arena_stage_span_right_columns[row] = 0;
        xm_arena_stage_span_left_columns[row] = 0xFF;
    }
}
#endif
extern u8 D_80091834[];
extern u8 D_800918B4[];
extern u8 *D_800927CC;
extern u8 *D_800927D0;

void func_80087698(s32 x0, s32 y0, s32 x1, s32 y1) {
    s32 tmp;
    s32 x;
    s32 dx;
    s32 v;
    u8 *p;

    if (y0 != y1) {
        if (y1 < y0) {
            tmp = x1;
            x1 = x0;
            x0 = tmp;
            tmp = y1;
            y1 = y0;
            y0 = tmp;
        }
        x = x0 << 8;
        dx = ((x1 - x0) << 8) / (y1 - y0);
        for (; y0 < y1; y0++, x += dx) {
            if (y0 >= 0) {
                if (y0 >= 0x80) {
                    return;
                }
                v = x >> 8;
                p = D_800927D0 + y0;
                if (v < *p) {
                    s32 t = D_80091834[y0];
                    if (v < t) {
                        v = t;
                    }
                    *p = v;
                }
                p = D_800927CC + y0;
                if (*p < v) {
                    s32 t = D_800918B4[y0] + 1;
                    if (t < v) {
                        v = t;
                    }
                    *p = v;
                }
            }
        }
    }
}

extern s32 D_80059598;
extern s32 D_80091934;
void func_800732AC(void *, void *, s32);
void func_80072D18(s32, s32, s32);

void func_8008779C(s32 arg0, s32 arg1, s32 arg2) {
    D_80059598 = (D_80059598 & 0xFFFFFF) | 0x24000000;
    __asm__ volatile ("lwc2\t$6, 0( %0 )" : : "r"(&D_80059598)); /* gte_ldrgb(&D_80059598) */
    func_800732AC((void *)0x1F800120, &D_80091934, 0x30);
    func_80072D18(arg0, arg1, arg2);
}

extern u8 *D_800927CC;
extern u8 *D_800927D0;
extern u8 *D_80092854[];
extern u8 *D_80092858;
void func_800732AC(void *, void *, s32);

void func_80087830(void) {
    s32 i;
    u8 *p;

    D_800927CC = (u8 *)0x1F800000;
    D_800927D0 = (u8 *)0x1F800080;
    D_80092854[0] = HeapAlloc(0xE100, 0);
    D_80092858 = HeapAlloc(0xE100, 0);
    p = D_80092854[0];
    for (i = 0; i < 0x708; i++) {
        p[3] = 7;
        p[7] = 0x24;
        p += 0x20;
    }
    func_800732AC(D_80092858, D_80092854[0], 0xE100);
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_stage_load_images (src/decomp/src/menu/arena_stage_views_and_hud.c:2133). */
#ifndef XM_T_MapTable
#define XM_T_MapTable
typedef struct {
    u16 uv[4][4];      
    u16 icons[8];      
} MapTable;
#endif
#ifndef XM_S_arena_stage_load_images
#define XM_S_arena_stage_load_images
void xm_arena_stage_load_images(MenuImageFile *files) __asm__("func_800878DC");
#endif
#ifndef XM_S_arena_stage_ground_draw_table
#define XM_S_arena_stage_ground_draw_table
extern MapTable xm_arena_stage_ground_draw_table __asm__("D_80091934");
#endif
#ifndef XM_S_arena_stage_shadow_clut
#define XM_S_arena_stage_shadow_clut
extern u16 xm_arena_stage_shadow_clut __asm__("D_800927D8");
#endif
#ifndef XM_S_arena_stage_shadow_tpage
#define XM_S_arena_stage_shadow_tpage
extern u16 xm_arena_stage_shadow_tpage __asm__("D_800927D4");
#endif
#ifndef XM_S_arena_stage_shadow_u
#define XM_S_arena_stage_shadow_u
extern u8 xm_arena_stage_shadow_u __asm__("D_800927DC");
#endif
#ifndef XM_S_arena_stage_shadow_v
#define XM_S_arena_stage_shadow_v
extern u8 xm_arena_stage_shadow_v __asm__("D_800927E0");
#endif
void func_800878DC(MenuImageFile *files) {
    TIM_IMAGE tim;
    s32 unused[2]; 
    s16 *clut;
    s32 i;

    for (i = 0; i < 4; i++) {
        xm_OpenTIM(files->icons[i]);
        xm_ReadTIM(&tim);
        xm_arena_stage_ground_draw_table.icons[i * 2 + 1] = xm_GetClut(tim.crect->x, tim.crect->y);
        xm_arena_stage_ground_draw_table.icons[i * 2] = xm_GetTPage(1, 1, tim.prect->x, tim.prect->y);
        xm_LoadImage(tim.crect, tim.caddr);
        xm_LoadImage(tim.prect, tim.paddr);
    }
    xm_OpenTIM(files->backdrop);
    xm_ReadTIM(&tim);
    xm_arena_stage_shadow_clut = xm_GetClut(tim.crect->x, tim.crect->y);
    xm_arena_stage_shadow_tpage = xm_GetTPage(0, 2, tim.prect->x, tim.prect->y);
    xm_arena_stage_shadow_u = tim.prect->x * 4;
    xm_arena_stage_shadow_v = tim.prect->y;
    clut = (s16 *)tim.caddr;
    clut[0] = 0;
    for (i = 1; i < 16; i++) {
        clut[i] |= 0x8000;
    }
    xm_LoadImage(tim.crect, tim.caddr);
    xm_LoadImage(tim.prect, tim.paddr);
    for (i = 0x1C; i < 0x25; i++) {
        xm_OpenTIM(files->extra[i - 0x1C]);
        xm_ReadTIM(&tim);
        xm_LoadImage(tim.crect, tim.caddr);
        xm_LoadImage(tim.prect, tim.paddr);
    }
}
#endif
extern u16 D_800927D4;
extern u16 D_800927D8;
extern u8 D_800927DC;
extern u8 D_800927E0;

void func_80087AB0(void *arg0) {
    struct Packet { u32 tag, color; s16 x0,y0; u16 uv0,clut; s16 x1,y1; u16 uv1,tpage; s16 x2,y2; u16 uv2,pad2; s16 x3,y3; u16 uv3,pad3; };
    s32 u, v;
    s32 temp_a1;
    s32 temp_v0;
    s32 temp_v1;
    void *var_a3;
    void *var_t0;

    var_t0 = arg0 + 0x162C;
    *(s32 *)((s8*)(arg0) + 0x1608) = 0x2C101010;
    *(s8 *)((s8*)(arg0) + 0x1607) = 9;
    *(u8 *)((s8*)(arg0) + 0x160B) = (u8) (*(u8 *)((s8*)(arg0) + 0x160B) | 2);
    var_a3 = arg0 + 0x1604;
    ((struct Packet*)((u8*)arg0 + 0x1604))->clut = (u16) D_800927D8;
    ((struct Packet*)((u8*)arg0 + 0x1604))->tpage = (u16) D_800927D4;
    u = D_800927DC;
    v = D_800927E0;
    temp_v1 = v << 8;
    ((struct Packet*)((u8*)arg0 + 0x1604))->uv0 = (s16) (u | temp_v1);
    temp_a1 = u + 0x3F;
    temp_v0 = (v + 0x3F) << 8;
    ((struct Packet*)((u8*)arg0 + 0x1604))->uv1 = (s16) (temp_a1 | temp_v1);
    ((struct Packet*)((u8*)arg0 + 0x1604))->uv2 = (s16) (u | temp_v0);
    ((struct Packet*)((u8*)arg0 + 0x1604))->uv3 = (s16) (temp_a1 | temp_v0);
    *(struct Packet*)((u8*)arg0 + 0x162C) = *(struct Packet*)((u8*)arg0 + 0x1604);
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_stage_draw_shadow (src/decomp/src/menu/arena_stage_views_and_hud.c:2186). */
#ifndef XM_T_gte_ldv3c
#define XM_T_gte_ldv3c
#define gte_ldv3c(r0)                                                          \
    __asm__ volatile("lwc2 $0, 0(%0);"                                         \
                     "lwc2 $1, 4(%0);"                                         \
                     "lwc2 $2, 8(%0);"                                         \
                     "lwc2 $3, 12(%0);"                                        \
                     "lwc2 $4, 16(%0);"                                        \
                     "lwc2 $5, 20(%0)"                                         \
                     :                                                         \
                     : "r"(r0)                                                 \
                     : "memory")
#endif
#ifndef XM_T_gte_nclip
#define XM_T_gte_nclip
#define gte_nclip() __asm__ volatile("nop;nop;.word 0x4B400006")
#endif
#ifndef XM_T_gte_stopz
#define XM_T_gte_stopz
#define gte_stopz(r0) __asm__ volatile("swc2 $24, 0(%0)" : : "r"(r0) : "memory")
#endif
#ifndef XM_T_gte_stsz3
#define XM_T_gte_stsz3
#define gte_stsz3(r0, r1, r2)                                                  \
    __asm__ volatile("swc2 $17, 0(%0);"                                        \
                     "swc2 $18, 0(%1);"                                        \
                     "swc2 $19, 0(%2)"                                         \
                     :                                                         \
                     : "r"(r0), "r"(r1), "r"(r2)                               \
                     : "memory")
#endif
#ifndef XM_T_gte_stsxy3_ft4
#define XM_T_gte_stsxy3_ft4
#define gte_stsxy3_ft4(r0)                                                     \
    __asm__ volatile("swc2 $12, 8(%0);"                                        \
                     "swc2 $13, 16(%0);"                                       \
                     "swc2 $14, 24(%0)"                                        \
                     :                                                         \
                     : "r"(r0)                                                 \
                     : "memory")
#endif
#ifndef XM_T_gte_rtps
#define XM_T_gte_rtps
#define gte_rtps() __asm__ volatile("nop;nop;.word 0x4A180001")
#endif
#ifndef XM_T_gte_stsxy
#define XM_T_gte_stsxy
#define gte_stsxy(r0) __asm__ volatile("swc2 $14, 0(%0)" : : "r"(r0) : "memory")
#endif
void func_80087B74(Actor *actor, u32 *ot, MATRIX *view) {
    SVECTOR corners[4];
    VECTOR centre;
    VECTOR unused; 
    SVECTOR normal;
    MATRIX m;
    s32 otz;
    s32 z0, z1, z2, z3;
    POLY_FT4 *quad;
    s32 size;
    s32 min;

    if ((actor->flags & 0x60000000) != 0x20000000) {
        size = (actor->home.vy + actor->unk92C.vy) / 32 + 0x90;
        if (size > 0) {
            centre.vx = (actor->home.vx + actor->unk92C.vx) / 2;
            centre.vz = (actor->home.vz + actor->unk92C.vz) / 2;
            centre.vy = 0;
            corners[0].vx = corners[1].vx = corners[0].vz = corners[2].vz = -size;
            corners[3].vx = corners[2].vx = corners[1].vz = corners[3].vz = size;
            corners[0].vy = corners[1].vy = corners[2].vy = corners[3].vy = 0;
            quad = &actor->backdrop[xm_arena_draw_buffer_index];
            m.t[0] = centre.vx - xm_arena_view_origin.vx;
            m.t[1] = xm_arena_stage_get_ground_height(&centre, 0);
            m.t[2] = centre.vz - xm_arena_view_origin.vz;
            xm_arena_stage_get_ground_normal(&normal);
            m.m[0][0] = 0x1000;
            m.m[0][1] = 0;
            m.m[0][2] = 0;
            m.m[1][0] = normal.vx;
            m.m[1][1] = normal.vy;
            m.m[1][2] = normal.vz;
            m.m[2][0] = 0;
            m.m[2][1] = 0;
            m.m[2][2] = 0x1000;
            xm_CompMatrix(view, &m, &m);
            gte_SetRotMatrix(&m);
            gte_SetTransMatrix(&m);
            gte_ldv3c(corners);
            gte_rtpt();
            gte_nclip();
            gte_stopz(&otz);
            if (otz >= 0) {
                gte_stsz3(&z0, &z1, &z2);
                gte_stsxy3_ft4(quad);
                gte_ldv0(&corners[3]);
                gte_rtps();
                gte_stsz(&z3);
                gte_stsxy(&quad->x3);
                min = z0;
                if (z1 < min) {
                    min = z1;
                }
                if (z2 < min) {
                    min = z2;
                }
                if (z3 < min) {
                    min = z3;
                }
                otz = min >> 4;
                xm_AddPrim(ot + otz, quad);
            }
        }
    }
}
#endif
extern s32 D_800928F8;
extern s32 D_8009A988;
void func_80087E38(void *arg0) {
    s32 count = D_800928F8;
    register u8 *base asm("$4");
    s16 *p;
    if (count < 31) {
        base = (u8 *)&D_8009A988;
        p = (s16 *)(base + count * 0x68);
        p[0] = *(s32 *)arg0;
        p[1] = *(s32 *)((u8 *)arg0 + 4);
        p[2] = *(s32 *)((u8 *)arg0 + 8);
        D_800928F8 = count + 1;
    }
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_debug_draw_path_markers (src/decomp/src/menu/arena_stage_views_and_hud.c:2270). */
#ifndef XM_T_PathMarker
#define XM_T_PathMarker
typedef struct PathMarker {
    LINE_F2 axes[2][3]; 
    s16 x, y, z;          
    u8 pad[2];
} PathMarker;
#endif
#ifndef XM_T_gte_ldv3
#define XM_T_gte_ldv3
#define gte_ldv3(r0, r1, r2)                                                   \
    __asm__ volatile("lwc2 $0, 0(%0);"                                         \
                     "lwc2 $1, 4(%0);"                                         \
                     "lwc2 $2, 0(%1);"                                         \
                     "lwc2 $3, 4(%1);"                                         \
                     "lwc2 $4, 0(%2);"                                         \
                     "lwc2 $5, 4(%2)"                                          \
                     :                                                         \
                     : "r"(r0), "r"(r1), "r"(r2)                               \
                     : "memory")
#endif
#ifndef XM_S_arena_debug_draw_path_markers
#define XM_S_arena_debug_draw_path_markers
void xm_arena_debug_draw_path_markers(u32 *ot) __asm__("func_80087EA0");
#endif
#ifndef XM_S_arena_debug_path_marker_count
#define XM_S_arena_debug_path_marker_count
extern s32 xm_arena_debug_path_marker_count __asm__("D_800928F8");
#endif
#ifndef XM_S_arena_debug_path_markers
#define XM_S_arena_debug_path_markers
extern PathMarker xm_arena_debug_path_markers[30] __asm__("D_8009A928");
#endif
#ifndef XM_S_gpu_ot_link_line_f2
#define XM_S_gpu_ot_link_line_f2
void xm_gpu_ot_link_line_f2(u_long *ot, LINE_F2 *prim) __asm__("func_800316C0");
#endif
void func_80087EA0(u32 *ot) {
    SVECTOR ends[6];
    PathMarker *point;
    s32 i;
    s32 j;

    for (i = 0; i < xm_arena_debug_path_marker_count; i++) {
        point = &xm_arena_debug_path_markers[i];
        ends[0].vx = point->x - 0x20;
        ends[0].vy = point->y;
        ends[0].vz = point->z;
        ends[1].vx = point->x + 0x20;
        ends[1].vy = point->y;
        ends[1].vz = point->z;
        ends[2].vx = point->x;
        ends[2].vy = point->y - 0x20;
        ends[2].vz = point->z;
        ends[3].vx = point->x;
        ends[3].vy = point->y + 0x20;
        ends[3].vz = point->z;
        ends[4].vx = point->x;
        ends[4].vy = point->y;
        ends[4].vz = point->z - 0x20;
        ends[5].vx = point->x;
        ends[5].vy = point->y;
        ends[5].vz = point->z + 0x20;
        for (j = 0; j < 6; j++) {
            ends[j].vx -= xm_arena_view_origin.vx;
            ends[j].vy -= xm_arena_view_origin.vy;
            ends[j].vz -= xm_arena_view_origin.vz;
        }
        gte_ldv3(&ends[0], &ends[1], &ends[2]);
        gte_rtpt();
        gte_stsxy3(&point->axes[xm_arena_draw_buffer_index][0].x0, &point->axes[xm_arena_draw_buffer_index][0].x1,
                   &point->axes[xm_arena_draw_buffer_index][1].x0);
        gte_ldv3(&ends[3], &ends[4], &ends[5]);
        gte_rtpt();
        gte_stsxy3(&point->axes[xm_arena_draw_buffer_index][1].x1, &point->axes[xm_arena_draw_buffer_index][2].x0,
                   &point->axes[xm_arena_draw_buffer_index][2].x1);
        setlen(&point->axes[xm_arena_draw_buffer_index][0], 3);
        *(u32 *)&point->axes[xm_arena_draw_buffer_index][0].r0 = 0x400000FF;
        setlen(&point->axes[xm_arena_draw_buffer_index][1], 3);
        *(u32 *)&point->axes[xm_arena_draw_buffer_index][1].r0 = 0x4000FF00;
        setlen(&point->axes[xm_arena_draw_buffer_index][2], 3);
        *(u32 *)&point->axes[xm_arena_draw_buffer_index][2].r0 = 0x40FF0000;
        xm_gpu_ot_link_line_f2((u_long *)ot, &point->axes[xm_arena_draw_buffer_index][0]);
        xm_gpu_ot_link_line_f2((u_long *)ot, &point->axes[xm_arena_draw_buffer_index][1]);
        xm_gpu_ot_link_line_f2((u_long *)ot, &point->axes[xm_arena_draw_buffer_index][2]);
    }
    xm_arena_debug_path_marker_count = 0;
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_debug_start_line (src/decomp/src/menu/arena_stage_views_and_hud.c:2325). */
#ifndef XM_T_Line3D
#define XM_T_Line3D
typedef struct {
    LINE_F2 packets[2]; 
    SVECTOR from;      
    SVECTOR to;        
    s16 timer;         
    s16 pad;
} Line3D;
#endif
#ifndef XM_S_arena_debug_start_line
#define XM_S_arena_debug_start_line
Line3D *xm_arena_debug_start_line(VECTOR *from, VECTOR *to, s32 colour) __asm__("func_8008820C");
#endif
#ifndef XM_S_arena_debug_lines
#define XM_S_arena_debug_lines
extern Line3D xm_arena_debug_lines[100] __asm__("D_80095938");
#endif
Line3D *xm_arena_debug_start_line(VECTOR *from, VECTOR *to, s32 colour) __asm__("func_8008820C");
Line3D *xm_arena_debug_start_line(VECTOR *from, VECTOR *to, s32 colour) {
    Line3D *line;
    s32 i;

    for (i = 0; i < 100; i++) {
        line = &xm_arena_debug_lines[i];
        if (line->timer == 0) {
            line->timer = 1;
            line->from.vx = from->vx;
            line->from.vy = from->vy;
            line->from.vz = from->vz;
            line->to.vx = to->vx;
            line->to.vy = to->vy;
            line->to.vz = to->vz;
            line->packets[0].r0 = (colour & 2) * 0x7F;
            line->packets[0].g0 = (colour & 4) * 0x3F;
            line->packets[0].b0 = (colour & 1) * 0xFF;
            line->packets[1].r0 = (colour & 2) * 0x7F;
            line->packets[1].g0 = (colour & 4) * 0x3F;
            line->packets[1].b0 = (colour & 1) * 0xFF;
            return line;
        }
    }
    return NULL;
}
#endif

void func_800882D4(s32 arg0, s32 arg1, s32 arg2, s16 arg3) {
    void *temp_v0;

    temp_v0 = func_8008820C();
    if (temp_v0 != NULL) {
        *(s16 *)((s8*)(temp_v0) + 0x30) = arg3;
    }
}

extern u8 D_80095968[];

void func_80088308(void) {
    s32 i;

    for (i = 0x141C; i >= 0; i -= 0x34) {
        *(s16 *)(D_80095968 + i) = 0;
    }
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_debug_draw_lines (src/decomp/src/menu/arena_stage_views_and_hud.c:2370). */
#ifndef XM_T_gte_ldv01
#define XM_T_gte_ldv01
#define gte_ldv01(r0, r1)                                                      \
    __asm__ volatile("lwc2 $0, 0(%0);"                                         \
                     "lwc2 $1, 4(%0);"                                         \
                     "lwc2 $2, 0(%1);"                                         \
                     "lwc2 $3, 4(%1)"                                          \
                     :                                                         \
                     : "r"(r0), "r"(r1) : "memory")
#endif
#ifndef XM_T_gte_stsxy01
#define XM_T_gte_stsxy01
#define gte_stsxy01(r0, r1)                                                    \
    __asm__ volatile("swc2 $12, 0(%0);"                                        \
                     "swc2 $13, 0(%1)"                                         \
                     :                                                         \
                     : "r"(r0), "r"(r1)                                        \
                     : "memory")
#endif
#ifndef XM_T_setcode
#define XM_T_setcode
#define setcode(p, _code) (((P_TAG *)(p))->code = (u_char)(_code))
#endif
#ifndef XM_S_arena_debug_draw_lines
#define XM_S_arena_debug_draw_lines
void xm_arena_debug_draw_lines(void *ot) __asm__("func_8008832C");
#endif
void func_8008832C(void *ot) {
    SVECTOR ends[2];
    Line3D *line;
    s32 i;

    for (i = 0; i < 100; i++) {
        if (xm_arena_debug_lines[i].timer != 0) {
            line = &xm_arena_debug_lines[i];
            line->timer--;
            ends[0] = line->from;
            ends[1] = line->to;
            ends[0].vx -= xm_arena_view_origin.vx;
            ends[0].vy -= xm_arena_view_origin.vy;
            ends[0].vz -= xm_arena_view_origin.vz;
            ends[1].vx -= xm_arena_view_origin.vx;
            ends[1].vy -= xm_arena_view_origin.vy;
            ends[1].vz -= xm_arena_view_origin.vz;
            gte_ldv01(&ends[0], &ends[1]);
            gte_rtpt();
            gte_stsxy01(&line->packets[xm_arena_draw_buffer_index].x0, &line->packets[xm_arena_draw_buffer_index].x1);
            setlen(&line->packets[xm_arena_draw_buffer_index], 3);
            setcode(&line->packets[xm_arena_draw_buffer_index], 0x40);
            xm_gpu_ot_link_line_f2(ot, &line->packets[xm_arena_draw_buffer_index]);
        }
    }
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_vector_normalize (src/decomp/src/menu/arena_stage_views_and_hud.c:2399). */
#ifndef XM_S_arena_vector_normalize
#define XM_S_arena_vector_normalize
void xm_arena_vector_normalize(VECTOR *vector, void *out) __asm__("func_800884E0");
#endif
#ifndef XM_S_model_get_largest_component
#define XM_S_model_get_largest_component
s32 xm_model_get_largest_component(s32 x, s32 y, s32 z) __asm__("func_8002DC9C");
#endif
void func_800884E0(VECTOR *vector, void *out) {
    VECTOR scaled = *vector;
    s32 square;
    s32 length;

    square = xm_model_get_largest_component(scaled.vx, scaled.vy, scaled.vz);
    if (square < 0) {
        square = -square;
    }
    length = xm_SquareRoot0(square);
    scaled.vx /= length;
    scaled.vy /= length;
    scaled.vz /= length;
    xm_VectorNormal(&scaled, out);
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_vector_normalize_to_svector (src/decomp/src/menu/arena_stage_views_and_hud.c:2417). */
void func_8008859C(VECTOR *vector, void *out) {
    VECTOR scaled = *vector;
    s32 square;
    s32 length;

    square = xm_model_get_largest_component(scaled.vx, scaled.vy, scaled.vz);
    if (square < 0) {
        square = -square;
    }
    length = xm_SquareRoot0(square);
    scaled.vx /= length;
    scaled.vy /= length;
    scaled.vz /= length;
    xm_VectorNormalS(&scaled, out);
}
#endif
s32 SquareRoot0(s32);                               /* extern */
s32 VectorNormalS(void *, void *);    /* extern */
s32 func_8002DC9C(s16, s16, s16);                   /* extern */

void func_80088658(void *arg0, void* arg1) {
    struct { s32 x,y,z,pad; } vector;
    s16 temp_a0;
    s16 temp_a1;
    s16 temp_a2;
    s32 temp_lo;
    s32 temp_lo_2;
    s32 temp_v0;
    s32 var_v0;

    temp_a0 = *(s16 *)((s8*)(arg0) + 0);
    vector.x = (s32) temp_a0;
    temp_a1 = *(s16 *)((s8*)(arg0) + 2);
    vector.y = (s32) temp_a1;
    temp_a2 = *(s16 *)((s8*)(arg0) + 4);
    vector.z = (s32) temp_a2;
    var_v0 = func_8002DC9C(temp_a0, temp_a1, temp_a2);
    if (var_v0 < 0) {
        var_v0 = -var_v0;
    }
    temp_v0 = SquareRoot0(var_v0);
    temp_lo = vector.x / temp_v0;
    temp_lo_2 = vector.y / temp_v0;
    vector.x = temp_lo;
    vector.y = temp_lo_2;
    vector.z = vector.z / temp_v0;
    VectorNormalS(&vector, arg1);
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_vector_get_length (src/decomp/src/menu/arena_stage_views_and_hud.c:2454). */
#ifndef XM_T_gte_ldlvl
#define XM_T_gte_ldlvl
#define gte_ldlvl(r0)                                                          \
    __asm__ volatile("lwc2 $9, 0(%0);"                                         \
                     "lwc2 $10, 4(%0);"                                        \
                     "lwc2 $11, 8(%0)"                                         \
                     :                                                         \
                     : "r"(r0) : "memory")
#endif
#ifndef XM_T_gte_sqr0
#define XM_T_gte_sqr0
#define gte_sqr0() __asm__ volatile("nop;nop;.word 0x4AA00428")
#endif
#ifndef XM_T_gte_stlvnl
#define XM_T_gte_stlvnl
#define gte_stlvnl(r0)                                                         \
    __asm__ volatile("swc2 $25, 0(%0);"                                        \
                     "swc2 $26, 4(%0);"                                        \
                     "swc2 $27, 8(%0)"                                         \
                     :                                                         \
                     : "r"(r0)                                                 \
                     : "memory")
#endif
s32 func_800886FC(VECTOR *vector) {
    VECTOR square;

    gte_ldlvl(vector);
    gte_sqr0();
    gte_stlvnl(&square);
    return xm_SquareRoot0(square.vx + square.vy + square.vz);
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_vector_get_flat_length (src/decomp/src/menu/arena_stage_views_and_hud.c:2464). */
s32 func_80088754(VECTOR *vector) {
    VECTOR square;

    gte_ldlvl(vector);
    gte_sqr0();
    gte_stlvnl(&square);
    return xm_SquareRoot0(square.vx + square.vz);
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_vector_get_distance (src/decomp/src/menu/arena_stage_views_and_hud.c:2474). */
s32 func_800887A4(VECTOR *from, VECTOR *to) {
    VECTOR delta;

    delta.vx = to->vx - from->vx;
    delta.vy = to->vy - from->vy;
    delta.vz = to->vz - from->vz;
    gte_ldlvl(&delta);
    gte_sqr0();
    gte_stlvnl(&delta);
    return xm_SquareRoot0(delta.vx + delta.vy + delta.vz);
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_vector_get_flat_distance (src/decomp/src/menu/arena_stage_views_and_hud.c:2487). */
s32 func_80088838(VECTOR *from, VECTOR *to) {
    VECTOR delta;

    delta.vx = to->vx - from->vx;
    delta.vz = to->vz - from->vz;
    gte_ldlvl(&delta);
    gte_sqr0();
    gte_stlvnl(&delta);
    return xm_SquareRoot0(delta.vx + delta.vz);
}
#endif
extern u8 D_8006F978[];

void func_800888B0(arg0)
    s32 arg0;
{
    s32 bit;

    bit = 1;
    bit <<= arg0 & 7;
    D_8006F978[arg0 >> 3] |= bit;
}

s32 func_800888E4(s32 arg0) {
    s32 bit = 1;
    bit <<= arg0 & 7;
    return D_8006F978[arg0 >> 3] & bit;
}

extern u8 D_8006F978[];

void func_80088908(s32 arg0) {
    s32 bit;

    bit = 1;
    bit <<= arg0 & 7;
    D_8006F978[arg0 >> 3] &= ~bit;
}


void func_80088940(void) {
    D_8006F980[0] |= 0x10000;
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_progress_add_unlocked_entry (src/decomp/src/menu/arena_stage_views_and_hud.c:2532). */
#ifndef XM_S_arena_progress_unlock_added
#define XM_S_arena_progress_unlock_added
extern u8 xm_arena_progress_unlock_added __asm__("D_800927EC");
#endif
void func_8008895C(void) {
    if (xm_game_data.options.complete && xm_arena_progress_unlock_added == 0) {
        xm_arena_progress_unlock_added = 1;
        xm_arena_select_entries[xm_arena_select_entry_count++] = &xm_arena_select_gears[22];
    }
}
#endif
s32 func_800888E4(s32);
void func_8008895C(void);
s32 func_800889C8__s32(void) asm("func_800889C8");

s32 func_800889C8__s32(void) {
    s32 i;

    for (i = 0; i < 0x31; i++) {
        if (i != 0x16 && func_800888E4(i) == 0) {
            return 0;
        }
    }
    D_8006F980[0] |= 0x10000;
    func_8008895C();
    return 0;
}

typedef struct {
    u32 unk0 : 4;
    u32 unk4 : 1;
    u32 unk5 : 1;
    u32 unk6 : 7;
    u32 unk13 : 3;
    u32 unk16 : 16;
} func_80088A40_bits;
extern func_80088A40_bits D_8006F980__bits[] asm("D_8006F980");

void func_80088A40(void) {
    if (*(u8 *)&D_8005061C != 0) {
        D_8006F980__bits[0].unk0 = 1;
        D_8006F980__bits[0].unk4 = D_80099D9B[0];
        D_8006F980__bits[0].unk5 = D_80099D9C[0];
        D_8006F980__bits[0].unk6 = D_80099D9F;
        D_8006F980__bits[0].unk13 = D_80099D98[0];
    }
}

extern s8 D_800927EC;

void func_80088AF8(void) {
    u32 *pbits;
    u32 bits;
    s32 i;
    u8 *p;

    if (*(u8 *)&D_8005061C != 0) {
        pbits = (u32 *)D_8006F980;
        bits = *pbits;
        D_800927EC = 0;
        if ((bits & 0xF) == 1) {
            D_80099D9B[0] = (bits >> 4) & 1;
            D_80099D9C[0] = (bits >> 5) & 1;
            D_80099D9F = (bits >> 6) & 0x7F;
            D_80099D98[0] = (bits >> 13) & 7;
            if (bits & 0x10000) {
                i = 7;
                p = (u8 *)pbits - 1;
                do {
                    *p = 0;
                    i--;
                    p--;
                } while (i >= 0);
            }
        } else {
            D_80099D9B[0] = 0;
            D_80099D9C[0] = 0;
            D_80099D9F = 2;
            D_80099D98[0] = 0;
            func_80088A40();
        }
    }
}


void func_80088BD4(void) {
    func_800888B0();
    func_800889C8();
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80088BFC);

void func_80088C28(void) {
    if (g_C1ButtonState & 4) {
        D_800927F4 += 1;
    }
    if (g_C1ButtonState & 1) {
        D_800927F4 -= 1;
    }
    if (D_800927F4 < 0) {
        D_800927F4 = 0;
    }
    HeapDebugDump(1, D_800927F4, 0xA, 0x80AD);
}

extern s32 D_8009A0D8;
void func_80088CBC(s32 arg0) {
    void *temp_s0;

    temp_s0 = (arg0 * 0xF8) + (u8 *)&D_8009A0D8;
    (*(s8 *)((u8 *)(temp_s0) + 0x77)) = 3;
    (*(s8 *)((u8 *)(temp_s0) + 0x7B)) = 0x7D;
    (*(s16 *)((u8 *)(temp_s0) + 0x80)) = 0x3000;
    (*(s16 *)((u8 *)(temp_s0) + 0x82)) = GetClut(0x3F0, 0xC0);
}
void ArchiveSetIndex(s32, s32);
void DrawSyncCallback(void *);
void FontLoadFont(s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
void InitGeom();
void func_8008A110(s32, s32);
void func_8008A128(s32, s32);
extern u8 func_80088BFC__data[] asm("func_80088BFC");
extern s32 D_80010000;
extern s32 D_80091BB0;
extern u8 *D_80092868;
extern void *D_80092870;
extern s16 D_80092898;
extern s32 D_8009289C;
extern s32 D_800928CC;
extern s16 D_800928D0;
extern s32 D_800928E8;
extern s32 D_80092930;
extern u8 D_8009A0D8__a[] asm("D_8009A0D8");

void func_80088D1C(void) {
    u8 unused[8]; /* retail frame has 8 bytes of locals no code touches */

    DrawSyncCallback(func_80088BFC__data + 4);
    InitGeom();
    HeapChangeCurrentUser(6, &D_80091BB0);
    ArchiveSetIndex(0x30, 0);
    FontLoadFont(4, 2, 0x138, 0xDA, 0x14, 1, 0x3C0, 0x1F0, 0x3C0, 0x1EF, 0);
    func_80088CBC(0);
    func_80088CBC(1);
    switch (D_80010000) {
    case -1:
        D_800928CC = 2;
        break;
    case 0:
        D_800928CC = 1;
        break;
    default:
        D_800928CC = 0;
        break;
    }
    D_80092868 = D_8009A0D8__a;
    D_80092870 = D_8009A0D8__a + 0xF8;
    func_8008A110(-1, -1);
    func_8008A128(-1, -1);
    D_80092898 = 2;
    D_8009289C = 1;
    D_80092930 = 0;
    D_800928E8 = 0;
    D_800928A0 = 0;
    D_80092920 = 0;
    D_80092930 = 0;
    D_800928D0 = 7;
    func_8008E620();
}

#endif
