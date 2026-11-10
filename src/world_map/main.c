#include "common.h"
#include "world_map/wm_obj.h"
#include "world_map/wm_spad.h"

#ifndef XENO_PC_PORT
extern WmObj * D_8009BE24;
s32 func_8008868C(s32);
s32 func_80088570(s32 arg0);
#endif

#ifndef XENO_PC_PORT
void VectorNormal(void *, void *);
void func_80093534(void *arg0);
s32 func_80094154(void *arg0, void *arg1);
extern WmMat D_8009A180;
extern u16 D_8009AF80[];
extern u16 D_8009AF90[];
extern u16 D_8009B674[];
extern WmObj * D_8009BE24;
extern s32 D_8009C610;
extern u8 * D_8009C620;
extern WmSVec D_8009CD68[];
extern WmSVec D_8009CD6C[];
extern s16 D_8009CD70[];
extern u8 g_GameState[];
s32 SquareRoot0(s32);
void func_80089160(s32, void *, void *);
void func_8008BFD4(s32, void *, s32, s32);
int abs(int);
extern s16 D_8009AFDC[];
void func_800848B4(s32, s32);
s32 func_80087C6C(s32 arg0);
s32 func_80087FD0(s32 arg0);
s32 func_80088B40(s32 arg0);
s32 func_80088D00(s32 arg0);
#endif

#ifndef XENO_PC_PORT
void RotMatrix(s32, void *);
extern u16 D_8009B624[];
extern u16 D_8009B626[];
extern WmObj * D_8009BE24;
extern s32 D_8009C610;
extern u8 *D_8009C620;
extern u16 D_8009B64C[];
extern u16 D_8009B64E[];
s32 func_80087804(s32 arg0);
s32 func_80087A8C(s32 arg0);
#endif

#ifndef XENO_PC_PORT
extern WmObj * D_8009BE24;
void func_800976C8(void);
#endif

#ifdef XENO_PC_PORT
/* Port view of the overlay data used by the bodies the port runs
 * (pc_port/src/world_map_port_data.h, owned by the port). */
#include "../../pc_port/src/world_map_port_data.h"
/* Prototypes the port-compiled bodies need (the retail ones sit in the
 * retail-only blocks). func_8008BFD4 and func_80087904 are still asm: the
 * port provides them. */
void func_8008BFD4(s32, void *, s32, s32);
void func_80087904(void *, s32, u16, s32);
s32 func_800879E0(s32);
void func_800848B4(s32, s32);
void func_80089160(s32, void *, void *);
s32 func_80093978(s32, s32);
void func_80097070(void *, void *);
void func_80087B84(void *, void *, void *);
s32 func_80087F60(s32);
void func_800894C8(s32);
void func_80093354(void *);
void func_80093534(void *);
s32 func_80094154(void *, void *);
int abs(int);
s32 func_8008868C(s32);
#endif

#ifndef XENO_PC_PORT
extern WmObj *D_8009BE24;
void HeapFree(s32);
void func_8002CBBC(s32);
extern u8 *D_8009C620;
extern s16 D_8009D7E0;
s32 ArchiveDecodeAlignedSize(s32);
s32 HeapAlloc(s32, s32);
void func_80029AFC(s16 *, s32, s32);
extern s32 D_8005945C;
extern s16 D_8009D3F8;
extern s32 D_8009D3FC;
extern s16 D_8009D400;
extern s32 D_8009D404;
extern s16 D_8009D408;
extern s32 D_8009D40C;
extern s32 D_8009D528;
void func_80087904(void *, s32, u16, s32);
extern u16 D_8009B64C[];
extern u16 D_8009B64E[];
extern s32 D_8009C610;
int abs(int);
s32 SquareRoot0(s32);
s32 func_80097770(s32 arg0, s32 arg1);
void func_80084818(void);
void func_80071FEC(void);
s32 func_800879E0(s32 arg0);
s32 func_80094154(void *arg0, void *arg1);
#endif

#ifndef XENO_PC_PORT
int abs(int);
void func_800963E4(s32 *);
extern s32 D_8009BE08;
extern s32 D_8009BE44;
extern s32 D_8009D788[];
extern s32 D_8009D808;
void func_800964B0(s32 *);
extern s32 D_8009C624[];
extern s32 D_8009D3C0;
void func_80093484(void *, s32, s32, s32);
extern s16 D_8009BD38;
extern u16 D_8009BD3A;
extern s16 D_8009BD3C;
extern s32 D_8009D3F0;
s32 func_800771D8(s32 arg0, s32 arg1, s32 arg2);
void func_800809EC(u8 *arg0, s32 arg1, s8 arg2, s8 arg3, s32 arg4);
s32 func_80096328(void);
s32 func_800965A4(void);
void func_80076DA4(void *arg0, void *arg1);
void func_80076F54(void *arg0);
#endif

#ifndef XENO_PC_PORT
void func_800848B4(s32 arg0, s32 arg1);
extern u16 D_8009B674[];
extern s32 D_8009C610;
void HeapFree(s32);
extern s32 D_8009C184[];
extern u16 D_8009A450;
extern u16 D_8009A46C[];
extern WmObj *D_8009BE24;
extern u16 D_8009A698;
extern u16 D_8009A6AC[];
extern u16 D_8009B69C[];
extern u8 *D_8009C620;
extern u16 D_8009B6B0[];
s32 func_800879E0(s32);
void func_8008BFD4(s32, void *, s32, s32);
s32 func_80087F60(s32 arg0);
void func_80097D64(void);
s32 func_8007A9B4(s32 arg0);
s32 func_8008032C(s32 arg0);
s32 func_80088D64(s32 arg0);
s32 func_80088EA0(s32 arg0);
s32 func_800879A8(s32 arg0);
s32 func_80088DE4(s32 arg0);
s32 func_80088E68(s32 arg0);
s32 func_80088F1C(s32 arg0);
#endif

#ifndef XENO_PC_PORT
s32 ArchiveDecodeAlignedSize(s32);
s32 HeapAlloc(s32, s32);
void func_80029AFC(s16 *, s32, s32);
extern s32 D_8004F304;
extern s32 D_8006259C;
extern s32 D_8009BCC8;
extern s32 D_8009C614;
extern s32 D_8009C884;
extern s32 D_8009C888;
extern s32 D_8009C88C;
extern s32 D_8009CC98;
extern s32 D_8009D3C8;
extern s32 D_8009D3D0;
extern s16 D_8009D3F8;
extern s32 D_8009D3FC;
extern s16 D_8009D400;
extern s32 D_8009D404;
extern s16 D_8009D408;
extern s32 D_8009D40C;
extern s16 D_8009D410;
extern s32 D_8009D414;
extern s16 D_8009D418;
extern s32 D_8009D41C;
extern s16 D_8009D420;
extern s32 D_8009D424;
extern s32 D_8009D800;
extern s32 D_8009BC38;
extern s32 D_8009BCB0;
s32 func_80097770(s32, s32);
extern s32 D_8009C5AC;
extern s32 D_8009C5B0;
extern s32 D_8009C5B4;
void func_80089160(s32, void *, void *);
void func_80089514(s32);
void func_8003A89C(); /* (AudioManager*, s32 level, s32 steps); one caller passes only a0 */
extern s32 D_80062528;
void func_80039E60(s32);
void func_8003A3B8(s32, s32, s32);
extern s32 D_8009CCA4;
extern s32 D_8009D3CC;
void HeapFree(s32);
void func_80074F04(void);
void func_800750DC(void);
void func_80084818();
void func_80086124(void);
void func_80086568(void);
void func_800866C8(void);
void func_80088FF4(void);
void func_80089128(void);
void func_800976A0(void);
void func_80097D64();
extern s16 D_8006F94E;
extern u16 D_8006F950;
extern s16 D_8006F954;
extern s32 D_8009BBC4;
extern s32 D_8009BC3C;
extern s32 D_8009BCB4;
extern u16 D_8009BD3A;
extern s32 D_8009C180;
void func_8003852C(s32);
void func_80039FF8();
extern s16 D_8009BD38;
extern s16 D_8009BD3C;
extern s32 D_8009BE0C;
extern WmObj *D_8009BE24;
extern s32 D_8009D144;
extern s32 D_8009D3F0;
extern s32 D_8009BE28;
extern s32 D_8009BE2C;
extern s32 D_8009BE30;
extern s32 D_8009D55C;
extern s32 D_8009D560;
extern s32 D_8009D564;
void func_80093354(void *arg0);
s32 func_80093A5C(s32, s32);
extern u8 *D_8009C620;
extern s32 D_8009A758;
void DrawSync(s32);
void Vsync(s32);
s32 func_80096668(void);
void func_800967E4();
void func_80097BC0(s32 *);
extern s32 D_8009AC60;
void RotMatrix(s32, void *);
void OuterProduct12(void *, void *, void *);
void TransposeMatrix(void *, void *);
void VectorNormal(void *, void *);
extern s32 D_8009BCC0;
void SpriteSetScale(void *, s32);
void *func_80024524(s32, s32, s32, s32, s32, s32);
void func_800245D8(void *, s32);
extern s32 D_8009CD34[];
extern s32 D_8009CD38;
extern u8 g_GameState[];
extern s32 D_8009CD3C;
s32 func_8008C364(void *, s32);
extern s32 D_8009BE10;
void func_800848B4(s32 arg0, s32 arg1);
extern s32 D_8009BD34;
extern s32 D_8009C7E8;
extern u16 D_8009CD4C;
extern s32 D_8009CEC0;
void func_800976C8();
s32 func_80071A50(void);
void func_80072090(void);
void func_8007369C(void);
void func_80076858(s32 arg0, void *arg1, void *arg2, void *arg3, void *arg4);
s32 func_80076A14(void);
s32 func_80076C18(s32 arg0, s32 arg1, s32 arg2);
s32 func_80076C3C(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s32 func_80076C68(s32 arg0, s16 arg1, s16 arg2, s16 arg3);
s32 func_80076C88(s32 arg0, s32 arg1);
s32 func_80076CB4(s32 arg0, s32 arg1);
s32 func_80076CD4(s32 arg0, s32 arg1);
s32 func_80076CF4(void);
s32 func_80076D1C(s32 arg0, s32 arg1);
s32 func_80076D50(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s32 func_80076D8C(s32 arg0, s32 arg1, s32 arg2);
void func_80077480(void);
s32 func_80077954(void);
void func_80077CC0(void);
s32 func_80078948(void);
void func_80078D24(void);
s32 func_80078E2C(s32 arg0);
s32 func_8007A410(s32 arg0);
s32 func_8007A568(void);
void func_8007A8AC(void);
s32 func_8007AD34(s32 arg0);
s32 func_8007BA08(void);
void func_8007C260(void);
s32 func_8007C724(s32 arg0);
s32 func_8007D690(s32 arg0);
s32 func_8007E450(s32 arg0);
void func_80080218(void);
s32 func_80080578(s32 arg0);
s32 func_80080900(s32 arg0);
s32 func_80080944(s32 arg0);
void func_8008106C(void);
s32 func_800813E8(s32 arg0);
s32 func_80081FB4(s32 arg0);
void func_800826B4(void);
s32 func_800827C8(s32 arg0);
s32 func_800834D0(void);
s32 func_800834D8(s32 arg0);
void func_800837DC(void);
s32 func_800838E8(s32 arg0);
s32 func_80087710(s32 arg0);
s32 func_80087734(s32 arg0);
s32 func_800877E0(s32 arg0);
void func_80087B84(void *arg0, void *arg1, void *arg2);
s32 func_80088E1C(s32 arg0);
s32 func_80088F54(void);
s32 func_80088F5C(void);
void func_800894C8(s32 arg0);
s32 func_8008A52C(s32 arg0);
s32 func_8008B498(s32 arg0);
s32 func_8008BD1C(s32 arg0);
s32 func_8008C6EC(s32 arg0);
s32 func_8008D520(s32 arg0);
s32 func_8008DE9C(s32 arg0);
s32 func_800907C4(void);
void func_80090A18(void);
s32 func_80092234(s32 arg0);
void func_8009766C(void);
void func_800977E0(s32 arg0, s16 arg1);
#endif

#ifndef XENO_PC_PORT
s32 ArchiveDecodeAlignedSize(s32);
void ArchiveReadFileToBuffer(s32, s32, s32, s32);
s32 HeapAlloc(s32, s32);
extern s32 D_8006259C;
extern s32 D_8009D3C8;
void HeapFree(s32);
extern s32 D_8009BE14;
extern s32 D_8009BE18;
extern s32 D_8009D30C;
extern s32 D_8009D780;
extern s32 D_8009D7D0;
void DrawSync(s32);
void EnterCriticalSection();
void ExitCriticalSection();
void FlushCache();
void Vsync(s32);
extern s32 D_8009D554;
extern s32 D_8009D7CC;
extern u8 *D_8009C620;
s16 func_80084DB8(s32, s16);
extern s16 D_8009D7E0;
extern s32 D_8009D7E8;
extern s32 D_8009D7EC;
extern s32 D_8009CEB4;
extern s32 D_8009D150;
extern s32 D_8009D7F8;
extern s32 D_8009D7FC;
extern s32 D_8009BDF4;
extern s32 D_8009BE1C;
extern s32 D_8009BE20;
extern u8 g_GameState[];
void func_800346D4(s32 *);
extern s32 D_8009D498;
extern s32 D_8009BD64;
extern s32 D_8009D160;
extern s32 D_8009D2B4;
s32 func_80093E8C();
void *func_80093660(s32, s32);
s32 ratan2(s32, s32, s32);
s32 rcos(s16);
s32 rsin(s16);
s32 func_8002C3D8();
extern s32 D_8009BE08;
extern s32 D_8009D3C0;
extern s32 D_8009D7D4;
extern s32 D_8009BCB8;
extern s32 D_8009BE44;
void func_800967E4();
extern WmObj *D_8009BE24;
void OuterProduct0(s32 *, s32 *, s32 *);
extern s32 D_8009BB4C[];
extern s32 D_8009BB5C[];
extern s32 D_8009BB6C[];
extern s32 D_8009BB7C[];
extern s32 D_8009BB8C[];
extern s32 D_8009BB9C[];
extern s32 D_8009C7F0[];
extern s32 D_8009C828[];
extern s32 D_8009C844[];
extern s32 D_8009C874[];
void func_800721E4(void);
void func_8007474C(void);
void func_80074F04(void);
void func_800750DC(void);
void func_800762FC(void);
s32 func_80076BC4(void);
s32 func_80076BDC(void *arg0, s16 arg1);
void func_800848B4(s32 arg0, s32 arg1);
s16 func_80084D00(s32 arg0, s16 *arg1);
void func_80086124(void);
void func_80086568(void);
void func_800866C8(void);
void func_80088FF4(void);
void func_80089128(void);
void func_8008DFF4(void *arg0);
void func_8008E034(void *arg0);
void func_80092DD0(void);
void func_800931B0(void);
void func_80093354(void *arg0);
void func_800933EC(void *arg0);
void func_80093534(void *arg0);
void func_800935DC(void *arg0, void *arg1, void *arg2);
s32 func_80093FE4(void);
u32 func_80094004(void);
s32 func_80094028(void *arg0);
void func_800941C4(void *arg0, void *arg1, void *arg2, s16 *arg3);
void func_800952B0(void *arg0, void *arg1, void *arg2);
void func_800960BC(void);
s32 func_80096668(void);
void func_80096694(void);
void func_800976A0(void);
void func_800976FC(s32 arg0, s32 arg1);
void func_80097718(s32 arg0, s32 arg1);
void func_80098044(void);
#endif

#ifndef XENO_PC_PORT
INCLUDE_RODATA("asm/world_map/nonmatchings/main", D_8006FAF0);
#endif
INCLUDE_ASM("asm/world_map/nonmatchings/main", WorldMapMain);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800712D0);


s32 func_80071A50(void) {
    return 1;
}


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_open_map_frame_update (src/decomp/src/worldmap/worldmap.c:327). */
#ifndef XM_T_SVECTOR
#define XM_T_SVECTOR
typedef struct {
    short vx, vy, vz, pad;
} SVECTOR;
#endif
#ifndef XM_T_VECTOR
#define XM_T_VECTOR
typedef struct {
    long vx, vy, vz, pad;
} VECTOR;
#endif
#ifndef XM_T_ViewSetup
#define XM_T_ViewSetup
typedef struct ViewSetup {
    SVECTOR eye;
    SVECTOR at;
    VECTOR up;
} ViewSetup;
#endif
#ifndef XM_T_Camera
#define XM_T_Camera
typedef struct Camera {
    VECTOR target;
} Camera;
#endif
#ifndef XM_T_RECT
#define XM_T_RECT
typedef struct {
    short x, y;
    short w, h;
} RECT;
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
#ifndef XM_T_DisplayBuffer
#define XM_T_DisplayBuffer
typedef struct DisplayBuffer {
    DRAWENV draw;
    DISPENV disp;
    u_long *ot;    
    void *packets; 
} DisplayBuffer;
#endif
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
#ifndef XM_S_worldmap_open_map_frame_update
#define XM_S_worldmap_open_map_frame_update
s32 xm_worldmap_open_map_frame_update(void) __asm__("func_80071A58");
#endif
#ifndef XM_S_worldmap_view_kind
#define XM_S_worldmap_view_kind
extern s32 xm_worldmap_view_kind __asm__("D_8009D144");
#endif
#ifndef XM_S_worldmap_camera_build_from_angles
#define XM_S_worldmap_camera_build_from_angles
void xm_worldmap_camera_build_from_angles(void *) __asm__("func_80097440");
#endif
#ifndef XM_S_worldmap_view_setup
#define XM_S_worldmap_view_setup
extern ViewSetup xm_worldmap_view_setup __asm__("D_8009BD40");
#endif
#ifndef XM_S_worldmap_camera_build_look_at
#define XM_S_worldmap_camera_build_look_at
void xm_worldmap_camera_build_look_at(void *) __asm__("func_80097244");
#endif
#ifndef XM_S_worldmap_effects_run_emitters
#define XM_S_worldmap_effects_run_emitters
void xm_worldmap_effects_run_emitters(void) __asm__("func_80089748");
#endif
#ifndef XM_S_worldmap_effects_draw_particles
#define XM_S_worldmap_effects_draw_particles
void xm_worldmap_effects_draw_particles(void) __asm__("func_80089C78");
#endif
#ifndef XM_S_worldmap_actor_draw_sprites
#define XM_S_worldmap_actor_draw_sprites
void xm_worldmap_actor_draw_sprites(void) __asm__("func_80085CDC");
#endif
#ifndef XM_S_worldmap_billboards_draw
#define XM_S_worldmap_billboards_draw
void xm_worldmap_billboards_draw(void) __asm__("func_8008615C");
#endif
#ifndef XM_S_worldmap_footprints_draw
#define XM_S_worldmap_footprints_draw
void xm_worldmap_footprints_draw(void) __asm__("func_800747DC");
#endif
#ifndef XM_S_worldmap_objects_draw
#define XM_S_worldmap_objects_draw
void xm_worldmap_objects_draw(void) __asm__("func_800848F4");
#endif
#ifndef XM_S_worldmap_terrain_wrap_origin
#define XM_S_worldmap_terrain_wrap_origin
void xm_worldmap_terrain_wrap_origin(void *) __asm__("func_800980D4");
#endif
#ifndef XM_S_worldmap_terrain_origin
#define XM_S_worldmap_terrain_origin
extern VECTOR xm_worldmap_terrain_origin __asm__("D_8009BBB4");
#endif
#ifndef XM_S_worldmap_terrain_crossed_edges
#define XM_S_worldmap_terrain_crossed_edges
extern s16 xm_worldmap_terrain_crossed_edges __asm__("D_8009D558");
#endif
#ifndef XM_S_worldmap_terrain_update_grid
#define XM_S_worldmap_terrain_update_grid
void xm_worldmap_terrain_update_grid(Camera *) __asm__("func_800981C8");
#endif
#ifndef XM_S_worldmap_camera
#define XM_S_worldmap_camera
extern Camera xm_worldmap_camera __asm__("D_8009BE28");
#endif
#ifndef XM_S_worldmap_stream_wait_for_slot
#define XM_S_worldmap_stream_wait_for_slot
void xm_worldmap_stream_wait_for_slot(void) __asm__("func_80096130");
#endif
#ifndef XM_S_worldmap_terrain_load_new_edges
#define XM_S_worldmap_terrain_load_new_edges
void xm_worldmap_terrain_load_new_edges(void) __asm__("func_80098CC0");
#endif
#ifndef XM_S_worldmap_terrain_classify_blocks
#define XM_S_worldmap_terrain_classify_blocks
void xm_worldmap_terrain_classify_blocks(Camera *) __asm__("func_800983A0");
#endif
#ifndef XM_S_worldmap_terrain_draw
#define XM_S_worldmap_terrain_draw
void xm_worldmap_terrain_draw(u_long *ot, s32, Camera *) __asm__("func_8009932C");
#endif
#ifndef XM_S_worldmap_current_display_buffer
#define XM_S_worldmap_current_display_buffer
extern DisplayBuffer *xm_worldmap_current_display_buffer __asm__("D_8009BE3C");
#endif
#ifndef XM_S_worldmap_wave_phase_x
#define XM_S_worldmap_wave_phase_x
extern s32 xm_worldmap_wave_phase_x __asm__("D_8009C5BC");
#endif
#ifndef XM_S_worldmap_horizon_draw
#define XM_S_worldmap_horizon_draw
void xm_worldmap_horizon_draw(void) __asm__("func_80073B04");
#endif
#ifndef XM_S_worldmap_sky_draw
#define XM_S_worldmap_sky_draw
void xm_worldmap_sky_draw(void) __asm__("func_800737EC");
#endif
#ifndef XM_S_worldmap_clouds_draw
#define XM_S_worldmap_clouds_draw
void xm_worldmap_clouds_draw(void) __asm__("func_80086798");
#endif
#ifndef XM_S_game_data
#define XM_S_game_data
extern GameData xm_game_data __asm__("g_GameState");
#endif
#ifndef XM_S_worldmap_map_overlay_draw
#define XM_S_worldmap_map_overlay_draw
void xm_worldmap_map_overlay_draw(void) __asm__("func_800740B8");
#endif
s32 func_80071A58(void) {
    SVECTOR unused; 

    if (xm_worldmap_view_kind == 0) {
        xm_worldmap_camera_build_from_angles(&xm_worldmap_view_setup);
    } else {
        xm_worldmap_camera_build_look_at(&xm_worldmap_view_setup);
    }
    xm_worldmap_effects_run_emitters();
    xm_worldmap_effects_draw_particles();
    xm_worldmap_actor_draw_sprites();
    xm_worldmap_billboards_draw();
    xm_worldmap_footprints_draw();
    xm_worldmap_objects_draw();
    xm_worldmap_terrain_wrap_origin(&xm_worldmap_terrain_origin);
    if (xm_worldmap_terrain_crossed_edges != 0) {
        xm_worldmap_terrain_update_grid(&xm_worldmap_camera);
        xm_worldmap_stream_wait_for_slot();
        xm_worldmap_terrain_load_new_edges();
    }
    xm_worldmap_terrain_classify_blocks(&xm_worldmap_camera);
    xm_worldmap_terrain_draw(xm_worldmap_current_display_buffer->ot, (s32)xm_worldmap_current_display_buffer->packets, &xm_worldmap_camera);
    xm_worldmap_wave_phase_x += 0x40;
    xm_worldmap_horizon_draw();
    xm_worldmap_sky_draw();
    xm_worldmap_clouds_draw();
    if (xm_game_data.worldmap.unk76 == 0) {
        xm_worldmap_map_overlay_draw();
    }
    return 1;
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80071B9C);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_read_party_models (src/decomp/src/worldmap/worldmap.c:390). */
#ifndef XM_T_FileRequest
#define XM_T_FileRequest
typedef struct FileRequest {
    u16 file;
    void *destination;
} FileRequest;
#endif
#ifndef XM_S_worldmap_read_party_models
#define XM_S_worldmap_read_party_models
void xm_worldmap_read_party_models(void) __asm__("func_80071CDC");
#endif
#ifndef XM_S_worldmap_character_models
#define XM_S_worldmap_character_models
extern void *xm_worldmap_character_models[3] __asm__("D_8009CD34");
#endif
#ifndef XM_S_heap_alloc
#define XM_S_heap_alloc
void *xm_heap_alloc(s32 size, s32 mode) __asm__("HeapAlloc");
#endif
#ifndef XM_S_cd_get_aligned_file_size
#define XM_S_cd_get_aligned_file_size
s32 xm_cd_get_aligned_file_size(s32 file) __asm__("ArchiveDecodeAlignedSize");
#endif
#ifndef XM_S_worldmap_gear_models
#define XM_S_worldmap_gear_models
extern void *xm_worldmap_gear_models[3] __asm__("D_8009BDF8");
#endif
#ifndef XM_S_worldmap_party_count
#define XM_S_worldmap_party_count
extern s32 xm_worldmap_party_count __asm__("D_8009C170");
#endif
#ifndef XM_S_worldmap_read_list
#define XM_S_worldmap_read_list
extern FileRequest xm_worldmap_read_list[] __asm__("D_8009D3F8");
#endif
#ifndef XM_S_cd_read_file_list
#define XM_S_cd_read_file_list
s32 xm_cd_read_file_list(FileRequest *list, s32 mode, s32 unused) __asm__("func_80029AFC");
#endif
void func_80071CDC(void) {
    s32 i;
    s32 j;
    s32 member;
    u8 gear;

    for (i = 0; i < 3; i++) {
        member = xm_game_data.party[i];
        if (member != 0xFF) {
            xm_worldmap_character_models[i] = xm_heap_alloc(xm_cd_get_aligned_file_size(member + 2), 0);
            j = xm_game_data.characters[member].gearId;
            if (j != 0xFF) {
                xm_worldmap_gear_models[i] = xm_heap_alloc(xm_cd_get_aligned_file_size(j + 0x13), 0);
            } else {
                xm_worldmap_gear_models[i] = NULL;
            }
        } else {
            xm_worldmap_gear_models[i] = NULL;
            xm_worldmap_character_models[i] = NULL;
        }
    }
    i = 0;
    xm_worldmap_party_count = 0;
    for (j = 0; j < 3; j++) {
        member = xm_game_data.party[j];
        if (member != 0xFF) {
            xm_worldmap_read_list[i].file = member + 2;
            xm_worldmap_read_list[i].destination = xm_worldmap_character_models[j];
            i++;
            xm_worldmap_party_count++;
            gear = xm_game_data.characters[member].gearId;
            if (gear != 0xFF) {
                xm_worldmap_read_list[i].file = gear + 0x13;
                xm_worldmap_read_list[i].destination = xm_worldmap_gear_models[j];
                i++;
            }
        }
    }
    xm_worldmap_read_list[i].file = 0;
    xm_worldmap_read_list[i].destination = NULL;
    xm_cd_read_file_list(xm_worldmap_read_list, 0, 0);
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_read_area_files (src/decomp/src/worldmap/worldmap.c:434). */
#ifndef XM_S_worldmap_read_area_files
#define XM_S_worldmap_read_area_files
void xm_worldmap_read_area_files(void) __asm__("func_80071EF0");
#endif
#ifndef XM_S_worldmap_terrain_image
#define XM_S_worldmap_terrain_image
extern void *xm_worldmap_terrain_image __asm__("D_8009C59C");
#endif
#ifndef XM_S_worldmap_terrain_image_file
#define XM_S_worldmap_terrain_image_file
extern s32 xm_worldmap_terrain_image_file __asm__("D_8009C17C");
#endif
#ifndef XM_S_worldmap_area_image
#define XM_S_worldmap_area_image
extern void *xm_worldmap_area_image __asm__("D_8009BD20");
#endif
#ifndef XM_S_worldmap_area_image_file
#define XM_S_worldmap_area_image_file
extern s32 xm_worldmap_area_image_file __asm__("D_8009C174");
#endif
#ifndef XM_S_worldmap_area_data
#define XM_S_worldmap_area_data
extern void *xm_worldmap_area_data __asm__("D_8009C180");
#endif
#ifndef XM_S_worldmap_area_data_file
#define XM_S_worldmap_area_data_file
extern s32 xm_worldmap_area_data_file __asm__("D_8009D3C4");
#endif
void func_80071EF0(void) {
    xm_worldmap_terrain_image = xm_heap_alloc(xm_cd_get_aligned_file_size(xm_worldmap_terrain_image_file), 1);
    xm_worldmap_area_image = xm_heap_alloc(xm_cd_get_aligned_file_size(xm_worldmap_area_image_file), 1);
    xm_worldmap_area_data = xm_heap_alloc(xm_cd_get_aligned_file_size(xm_worldmap_area_data_file), 1);
    xm_worldmap_read_list[0].file = xm_worldmap_area_data_file;
    xm_worldmap_read_list[1].file = xm_worldmap_terrain_image_file;
    xm_worldmap_read_list[2].file = xm_worldmap_area_image_file;
    xm_worldmap_read_list[3].file = 0;
    xm_worldmap_read_list[0].destination = xm_worldmap_area_data;
    xm_worldmap_read_list[1].destination = xm_worldmap_terrain_image;
    xm_worldmap_read_list[2].destination = xm_worldmap_area_image;
    xm_worldmap_read_list[3].destination = NULL;
    xm_cd_read_file_list(xm_worldmap_read_list, 0, 0);
}
#endif


#ifndef XENO_PC_PORT
void func_80071FEC(void) {
    s32 temp_v0;

    D_8005945C = HeapAlloc(ArchiveDecodeAlignedSize(0x26), 1);
    temp_v0 = HeapAlloc(ArchiveDecodeAlignedSize(0x25), 1);
    D_8009D3F8 = 0x25;
    D_8009D528 = temp_v0;
    D_8009D3FC = temp_v0;
    D_8009D400 = 0x26;
    D_8009D408 = 0;
    D_8009D40C = 0;
    D_8009D404 = D_8005945C;
    func_80029AFC(&D_8009D3F8, 0, 0);
}
#endif



#ifndef XENO_PC_PORT
void func_80072090(void) {
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v0_4;
    s32 temp_v0_5;

    D_8004F304 += 1;
    D_8009D3F8 = (s16) D_8009CC98;
    temp_v0 = HeapAlloc(ArchiveDecodeAlignedSize(D_8009CC98), 1);
    D_8009C88C = temp_v0;
    D_8009D3FC = temp_v0;
    D_8009D400 = (s16) D_8009D3D0;
    temp_v0_2 = HeapAlloc(ArchiveDecodeAlignedSize(D_8009D3D0), 0);
    D_8009C884 = temp_v0_2;
    D_8009D404 = temp_v0_2;
    D_8009D408 = (s16) D_8009D3C8;
    temp_v0_3 = HeapAlloc(ArchiveDecodeAlignedSize(D_8009D3C8), 0);
    D_8006259C = temp_v0_3;
    D_8009D40C = temp_v0_3;
    D_8009D410 = (s16) D_8009D800;
    temp_v0_4 = HeapAlloc(ArchiveDecodeAlignedSize(D_8009D800), 0);
    D_8009C888 = temp_v0_4;
    D_8009D414 = temp_v0_4;
    D_8009D418 = (s16) D_8009BCC8;
    temp_v0_5 = HeapAlloc(ArchiveDecodeAlignedSize(D_8009BCC8), 0);
    D_8009C614 = temp_v0_5;
    D_8009D41C = temp_v0_5;
    D_8009D420 = 0;
    D_8009D424 = 0;
    func_80029AFC(&D_8009D3F8, 0, 0);
}
#endif



#ifndef XENO_PC_PORT
void func_800721E4(void) {
    s32 temp_v0;

    temp_v0 = HeapAlloc(ArchiveDecodeAlignedSize(D_8009D3C8), 0);
    D_8006259C = temp_v0;
    ArchiveReadFileToBuffer(D_8009D3C8, temp_v0, 0, 0);
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80072238);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_open_map_leave (src/decomp/src/worldmap/worldmap_open_map.c:393). */
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
#ifndef XM_T_MATRIX
#define XM_T_MATRIX
typedef struct {
    short m[3][3];
    long t[3];
} MATRIX;
#endif
#ifndef XM_T_SpritePart
#define XM_T_SpritePart
typedef struct {
    s16 x, y;              
    u8 u, v;               
    u8 w, h;               
    u8 byte8, byte9;       
    u16 tpage;             
    u16 clut;              
    u8 unknowne[2];
    u32 colour;            
    u32 flags;             
} SpritePart;
#endif
#ifndef XM_T_SpriteRendererEntry
#define XM_T_SpriteRendererEntry
typedef struct {
    s8 byte0;              
    s8 byte1;              
    s16 half2;
    s16 half4;
    s16 half6;
} SpriteRendererEntry;
#endif
#ifndef XM_T_SpriteOffset
#define XM_T_SpriteOffset
typedef struct {
    s8 x, y;
} SpriteOffset;
#endif
#ifndef XM_T_SpriteRenderer
#define XM_T_SpriteRenderer
typedef struct {
    s16 angle_x, angle_y, angle_z; 
    s16 scale_x, scale_y, scale_z; 
    MATRIX matrix;                 
    SpritePart *parts[2];          
    SpriteRendererEntry *pointer34; 
    struct Sprite *next_pending;   
    SpriteOffset offset;           
    u8 unknown3e[2];
    s32 word40;                    
    SVECTOR light_angles;          
    u16 light_colour[3];           
} SpriteRenderer;
#endif
#ifndef XM_T_Sprite
#define XM_T_Sprite
typedef struct Sprite {
    s32 x, y, z;                
    s32 speed_x, speed_y, speed_z; 
    s32 speed;                  
    s32 gravity;                
    SpriteRenderer *renderer;   
    void *image;                
    u8 red, green, blue;     
    u8 colour_flags;         
    s16 scale;               
    s16 depth;               
    s16 half30;              
    s16 direction;           
    u16 frame;               
    u16 height;              
    u16 extent_depth;        
    u16 rate;                
    union {
        u32 word;
        struct {
        unsigned sides : 2;      
        unsigned unknown2 : 1;
        unsigned flip : 1;       
        unsigned flip_y : 1;
        unsigned blend : 3;      
        unsigned unknown8 : 8;
        unsigned field16 : 4;
        unsigned mode : 4;       
        unsigned no_view : 1;    
        unsigned unknown25 : 3;
        unsigned dirty : 1;      
        unsigned unknown29 : 3;
        } bits;
        u8 bytes[4];         
    } render;                
    u32 flags;               
    s32 *resource_block;     
    s32 *animations;         
    s32 resource;            
    s32 word50;              
    u16 *frame_table;        
    u16 *animation;          
    u16 *facings;            
    u16 *word60;             
    u8 *script;              
    void (*callback)(struct Sprite *sprite); 
    void *block;             
    struct Sprite *parent;   
    struct Sprite *partner;  
    s32 word78;              
    void *sequencer;         
    u16 word80;              
    u16 word82;              
    s16 ground;              
    u16 size;                
    u8 *frames;              
    s8 stack_top;            
    u8 unknown8d;
    u8 stack[0x10];          
    s16 countdown;           
    s16 target_x, target_y, target_z; 
    u8 unknowna6[2];
    struct {
        unsigned sequencer_owned : 1; 
        unsigned bounce : 10;    
        unsigned frame : 6;      
        unsigned step : 3;       
        unsigned phase : 2;      
        unsigned field22 : 6;    
        unsigned field28 : 2;
        unsigned unknown30 : 2;
    } frame_bits;            
    union {
        u32 word;
        struct {
        unsigned unknown0 : 2;
        unsigned mirror : 1;     
        unsigned frame_flip : 1; 
        unsigned unknown4 : 1;
        unsigned owns_children : 1; 
        unsigned double_step : 1;
        unsigned divisor : 12;   
        unsigned unknown19 : 13;
        } bits;
        u8 bytes[4];
    } motion;                
    union {
        u32 wordb0;          
        u8 byteb0;
        struct {
            unsigned unknown0 : 8;
            unsigned passive_children : 1; 
            unsigned share_rate : 1;       
            unsigned unknown10 : 22;
        } bits;
    } b0;                    
} Sprite;
#endif
#ifndef XM_T_WorldmapActor
#define XM_T_WorldmapActor
typedef struct WorldmapActor {
    s16 command;     
    s16 command_arg;
    s16 unk4;
    s16 unk6;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    s32 unk14;
    s32 kind;        
    s32 update;      
    s16 state;       
    s16 wait;        
    s16 unk24;
    s16 unk26;
    VECTOR position; 
    VECTOR motion;   
    s16 heading;     
    s16 turn;        
    Sprite *handle;  
    union {
        s16 *script; 
        u16 value;   
        s32 step;
    } u;             
    s32 unk54;
    s32 unk58;
    s32 unk5C;
    s32 unk60;
    s32 unk64;
    s32 unk68;
    s32 unk6C;
    s32 unk70;
    s32 unk74;
    s32 unk78;
    s32 unk7C;
} WorldmapActor;
#endif
#ifndef XM_S_worldmap_open_map_leave
#define XM_S_worldmap_open_map_leave
void xm_worldmap_open_map_leave(void) __asm__("func_8007299C");
#endif
#ifndef XM_S_worldmap_loop_result
#define XM_S_worldmap_loop_result
extern s32 xm_worldmap_loop_result __asm__("D_8009D7CC");
#endif
#ifndef XM_S_sound_set_seq_fade
#define XM_S_sound_set_seq_fade
void xm_sound_set_seq_fade(SoundSeq *seq, s32 fade, s32 frames) __asm__("func_8003A89C");
#endif
#ifndef XM_S_mode_music_seq
#define XM_S_mode_music_seq
extern s32 xm_mode_music_seq __asm__("D_80062528");
#endif
#ifndef XM_S_sound_stop_all_effects
#define XM_S_sound_stop_all_effects
void xm_sound_stop_all_effects(void) __asm__("func_80039FF8");
#endif
#ifndef XM_S_sound_remove_effect_bank
#define XM_S_sound_remove_effect_bank
void xm_sound_remove_effect_bank(SoundBank *bank) __asm__("func_8003852C");
#endif
#ifndef XM_S_sound_effect_bank
#define XM_S_sound_effect_bank
extern struct SoundBank *xm_sound_effect_bank __asm__("D_8006259C");
#endif
#ifndef XM_S_heap_free
#define XM_S_heap_free
s32 xm_heap_free(void *data) __asm__("HeapFree");
#endif
#ifndef XM_S_worldmap_actor_slots
#define XM_S_worldmap_actor_slots
extern WorldmapActor *xm_worldmap_actor_slots __asm__("D_8009BE24");
#endif
#ifndef XM_S_sprite_destroy
#define XM_S_sprite_destroy
void xm_sprite_destroy(Sprite *sprite) __asm__("func_800230A8");
#endif
#ifndef XM_S_worldmap_save_state
#define XM_S_worldmap_save_state
void xm_worldmap_save_state(void) __asm__("func_80075460");
#endif
#ifndef XM_S_worldmap_path_window_close
#define XM_S_worldmap_path_window_close
void xm_worldmap_path_window_close(void) __asm__("func_80092DD0");
#endif
#ifndef XM_S_worldmap_destination_window_close
#define XM_S_worldmap_destination_window_close
void xm_worldmap_destination_window_close(void) __asm__("func_800931B0");
#endif
#ifndef XM_S_worldmap_objects_free
#define XM_S_worldmap_objects_free
void xm_worldmap_objects_free(void) __asm__("func_80084818");
#endif
#ifndef XM_S_worldmap_billboards_free_quads
#define XM_S_worldmap_billboards_free_quads
void xm_worldmap_billboards_free_quads(void) __asm__("func_80086124");
#endif
#ifndef XM_S_sprite_free_queues
#define XM_S_sprite_free_queues
void xm_sprite_free_queues(void) __asm__("GfxFreeWorkBuffers");
#endif
#ifndef XM_S_worldmap_clouds_free
#define XM_S_worldmap_clouds_free
void xm_worldmap_clouds_free(void) __asm__("func_80086568");
#endif
#ifndef XM_S_worldmap_clouds_free_quads
#define XM_S_worldmap_clouds_free_quads
void xm_worldmap_clouds_free_quads(void) __asm__("func_800866C8");
#endif
#ifndef XM_S_worldmap_footprints_free
#define XM_S_worldmap_footprints_free
void xm_worldmap_footprints_free(void) __asm__("func_8007474C");
#endif
#ifndef XM_S_worldmap_texture_anim_free
#define XM_S_worldmap_texture_anim_free
void xm_worldmap_texture_anim_free(void) __asm__("func_80074F04");
#endif
#ifndef XM_S_worldmap_texture_anim2_free
#define XM_S_worldmap_texture_anim2_free
void xm_worldmap_texture_anim2_free(void) __asm__("func_800750DC");
#endif
#ifndef XM_S_worldmap_effects_free_slots
#define XM_S_worldmap_effects_free_slots
void xm_worldmap_effects_free_slots(void) __asm__("func_80088FF4");
#endif
#ifndef XM_S_worldmap_effects_free_quads
#define XM_S_worldmap_effects_free_quads
void xm_worldmap_effects_free_quads(void) __asm__("func_80089128");
#endif
#ifndef XM_S_worldmap_terrain_free_blocks
#define XM_S_worldmap_terrain_free_blocks
void xm_worldmap_terrain_free_blocks(void) __asm__("func_80097D64");
#endif
#ifndef XM_S_worldmap_display_buffers
#define XM_S_worldmap_display_buffers
extern DisplayBuffer xm_worldmap_display_buffers[2] __asm__("D_8009BBC8");
#endif
#ifndef XM_S_worldmap_actor_free_slots
#define XM_S_worldmap_actor_free_slots
void xm_worldmap_actor_free_slots(void) __asm__("func_800976A0");
#endif
#ifndef XM_S_worldmap_stream_free
#define XM_S_worldmap_stream_free
void xm_worldmap_stream_free(void) __asm__("func_800960BC");
#endif
void func_8007299C(void) {
    s32 i;

    if (xm_worldmap_loop_result == 0) {
        xm_sound_set_seq_fade((SoundSeq *)xm_mode_music_seq, 0, 0xF0);
    }
    xm_sound_stop_all_effects();
    xm_sound_remove_effect_bank(xm_sound_effect_bank);
    xm_heap_free(xm_sound_effect_bank);
    for (i = 0; i < 0x40; i++) {
        if (xm_worldmap_actor_slots[i].handle != NULL) {
            xm_sprite_destroy(xm_worldmap_actor_slots[i].handle);
            xm_worldmap_actor_slots[i].handle = NULL;
        }
    }
    if (xm_worldmap_loop_result == 1) {
        xm_worldmap_save_state();
        xm_game_data.entry[2] |= 0x8000;
    }
    xm_worldmap_path_window_close();
    xm_worldmap_destination_window_close();
    xm_worldmap_objects_free();
    xm_worldmap_billboards_free_quads();
    xm_sprite_free_queues();
    xm_worldmap_clouds_free();
    xm_worldmap_clouds_free_quads();
    xm_worldmap_footprints_free();
    xm_worldmap_texture_anim_free();
    xm_worldmap_texture_anim2_free();
    xm_worldmap_effects_free_slots();
    xm_worldmap_effects_free_quads();
    xm_worldmap_terrain_free_blocks();
    xm_heap_free(xm_worldmap_display_buffers[0].ot);
    xm_heap_free(xm_worldmap_display_buffers[1].ot);
    xm_heap_free(xm_worldmap_display_buffers[0].packets);
    xm_heap_free(xm_worldmap_display_buffers[1].packets);
    xm_heap_free(xm_worldmap_area_data);
    for (i = 0; i < 3; i++) {
        if (xm_worldmap_character_models[i] != NULL) {
            xm_heap_free(xm_worldmap_character_models[i]);
        }
        if (xm_worldmap_gear_models[i] != NULL) {
            xm_heap_free(xm_worldmap_gear_models[i]);
        }
    }
    xm_worldmap_actor_free_slots();
    xm_worldmap_stream_free();
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_init_display (src/decomp/src/worldmap/worldmap_open_map.c:443). */
#ifndef XM_S_worldmap_init_display
#define XM_S_worldmap_init_display
void xm_worldmap_init_display(void) __asm__("func_80072BB0");
#endif
#ifndef XM_S_ResetGraph
#define XM_S_ResetGraph
int xm_ResetGraph(int mode) __asm__("ResetGraph");
#endif
#ifndef XM_S_worldmap_projection_distance
#define XM_S_worldmap_projection_distance
extern s32 xm_worldmap_projection_distance __asm__("D_8009BCDC");
#endif
#ifndef XM_S_SetGeomScreen
#define XM_S_SetGeomScreen
void xm_SetGeomScreen(long h) __asm__("SetGeomScreen");
#endif
#ifndef XM_S_SetDefDrawEnv
#define XM_S_SetDefDrawEnv
DRAWENV *xm_SetDefDrawEnv(DRAWENV *env, int x, int y, int w, int h) __asm__("SetDefDrawEnv");
#endif
#ifndef XM_S_SetDefDispEnv
#define XM_S_SetDefDispEnv
DISPENV *xm_SetDefDispEnv(DISPENV *env, int x, int y, int w, int h) __asm__("SetDefDispEnv");
#endif
#ifndef XM_S_model_set_color
#define XM_S_model_set_color
void xm_model_set_color(s32 r, s32 g, s32 b) __asm__("func_8002C6E0");
#endif
#ifndef XM_S_SetBackColor
#define XM_S_SetBackColor
void xm_SetBackColor(long rbk, long gbk, long bbk) __asm__("SetBackColor");
#endif
#ifndef XM_S_SetFarColor
#define XM_S_SetFarColor
void xm_SetFarColor(long r, long g, long b) __asm__("SetFarColor");
#endif
#ifndef XM_S_worldmap_background_color
#define XM_S_worldmap_background_color
extern u8 xm_worldmap_background_color[3] __asm__("D_8009BB48");
#endif
#ifndef XM_S_SetFogNearFar
#define XM_S_SetFogNearFar
void xm_SetFogNearFar(long a, long b, long h) __asm__("func_80048AB0");
#endif
void func_80072BB0(void) {
    xm_ResetGraph(1);
    xm_worldmap_projection_distance = 0x100;
    xm_SetGeomScreen(0x100);
    xm_SetDefDrawEnv(&xm_worldmap_display_buffers[0].draw, 0, 0, 0x140, 0xD8);
    xm_SetDefDrawEnv(&xm_worldmap_display_buffers[1].draw, 0, 0xD8, 0x140, 0xD8);
    xm_SetDefDispEnv(&xm_worldmap_display_buffers[0].disp, 0, 0xD8, 0x140, 0xD8);
    xm_SetDefDispEnv(&xm_worldmap_display_buffers[1].disp, 0, 0, 0x140, 0xD8);
    xm_worldmap_display_buffers[1].draw.isbg = 1;
    xm_worldmap_display_buffers[0].draw.isbg = 1;
    xm_worldmap_display_buffers[1].draw.dtd = 1;
    xm_worldmap_display_buffers[0].draw.dtd = 1;
    if (xm_worldmap_loop_result == 2) {
        xm_worldmap_display_buffers[0].draw.r0 = 0;
        xm_worldmap_display_buffers[0].draw.g0 = 0;
        xm_worldmap_display_buffers[0].draw.b0 = 0;
        xm_worldmap_display_buffers[1].draw.r0 = 0;
        xm_worldmap_display_buffers[1].draw.g0 = 0;
        xm_worldmap_display_buffers[1].draw.b0 = 0;
    } else {
        xm_worldmap_display_buffers[0].draw.r0 = 0;
        xm_worldmap_display_buffers[0].draw.g0 = 0;
        xm_worldmap_display_buffers[0].draw.b0 = 0x70;
        xm_worldmap_display_buffers[1].draw.r0 = 0;
        xm_worldmap_display_buffers[1].draw.g0 = 0;
        xm_worldmap_display_buffers[1].draw.b0 = 0x70;
    }
    xm_worldmap_display_buffers[1].disp.screen.y = 0xA;
    xm_worldmap_display_buffers[0].disp.screen.y = 0xA;
    xm_worldmap_display_buffers[1].disp.screen.w = 0x100;
    xm_worldmap_display_buffers[0].disp.screen.w = 0x100;
    xm_worldmap_display_buffers[1].disp.screen.x = 0;
    xm_worldmap_display_buffers[0].disp.screen.x = 0;
    xm_worldmap_display_buffers[1].disp.screen.h = 0xD8;
    xm_worldmap_display_buffers[0].disp.screen.h = 0xD8;
    xm_model_set_color(0x80, 0x80, 0x80);
    xm_SetBackColor(0x80, 0x80, 0x80);
    xm_SetFarColor(xm_worldmap_background_color[0], xm_worldmap_background_color[1], xm_worldmap_background_color[2]);
    xm_SetFogNearFar(xm_worldmap_loop_result == 2 ? 0xB00 : 0x800, 0xE80, xm_worldmap_projection_distance);
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80072DB4);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_choose_movement_mode (src/decomp/src/worldmap/worldmap_open_map.c:561). */
#ifndef XM_S_worldmap_choose_movement_mode
#define XM_S_worldmap_choose_movement_mode
void xm_worldmap_choose_movement_mode(void) __asm__("func_80073300");
#endif
#ifndef XM_S_worldmap_movement_mode
#define XM_S_worldmap_movement_mode
extern s32 xm_worldmap_movement_mode __asm__("D_8009BE10");
#endif
__asm__(".xeno_jt 4 24 jtbl_8006FB0C");
void func_80073300(void) {
    if (xm_game_data.worldmap.flags & 0x4000) {
        switch (xm_game_data.worldmap.flags & 0x1FFF) {
        case 0:
            break;
        case 1:
            xm_worldmap_movement_mode = 4;
            break;
        case 2:
            xm_worldmap_movement_mode = 5;
            break;
        case 3:
            xm_worldmap_movement_mode = 7;
            break;
        case 4:
            xm_worldmap_movement_mode = 7;
            break;
        }
    } else if ((xm_game_data.inGear[0] | xm_game_data.inGear[1] | xm_game_data.inGear[2]) == 0) {
        xm_worldmap_movement_mode = 1;
    } else {
        xm_worldmap_movement_mode = 2;
    }
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_restore_player_position (src/decomp/src/worldmap/worldmap_open_map.c:587). */
#ifndef XM_S_worldmap_restore_player_position
#define XM_S_worldmap_restore_player_position
void xm_worldmap_restore_player_position(void) __asm__("func_80073398");
#endif
#ifndef XM_S_worldmap_player_position
#define XM_S_worldmap_player_position
extern VECTOR xm_worldmap_player_position __asm__("D_8009C5AC");
#endif
#ifndef XM_S_worldmap_player_heading
#define XM_S_worldmap_player_heading
extern s32 xm_worldmap_player_heading __asm__("D_8009C584");
#endif
#ifndef XM_S_worldmap_load_flying_vehicle_position
#define XM_S_worldmap_load_flying_vehicle_position
void xm_worldmap_load_flying_vehicle_position(VECTOR *position) __asm__("func_8008DFF4");
#endif
__asm__(".xeno_jt 4 28 jtbl_8006FB24");
void func_80073398(void) {
    xm_game_data.worldmap.unk6A = 0;
    switch (xm_worldmap_movement_mode) {
    case 1:
    case 2:
        xm_worldmap_player_position.vx = xm_game_data.worldmap.x << 12;
        xm_worldmap_player_position.vz = xm_game_data.worldmap.z << 12;
        xm_worldmap_player_heading = xm_game_data.worldmap.heading;
        break;
    case 4:
    case 5:
    case 7:
        xm_worldmap_load_flying_vehicle_position(&xm_worldmap_player_position);
        xm_worldmap_player_heading = xm_game_data.worldmap.vehicle_heading;
        break;
    }
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_place_player_at_arrival_point (src/decomp/src/worldmap/worldmap_open_map.c:607). */
#ifndef XM_T_WorldmapSpot
#define XM_T_WorldmapSpot
typedef struct WorldmapSpot {
    s16 x;
    s16 id;
    s16 z;
    s16 pad;
} WorldmapSpot;
#endif
#ifndef XM_S_worldmap_place_player_at_arrival_point
#define XM_S_worldmap_place_player_at_arrival_point
void xm_worldmap_place_player_at_arrival_point(s32 id) __asm__("func_80073448");
#endif
#ifndef XM_S_worldmap_arrival_points
#define XM_S_worldmap_arrival_points
extern WorldmapSpot *xm_worldmap_arrival_points __asm__("D_8009D3F4");
#endif
void func_80073448(s32 id) {
    s32 unused; 
    WorldmapSpot *spot;

    if (xm_game_data.worldmap.flags & 0x2000) {
        xm_game_data.worldmap.flags &= ~0x2000;
        xm_worldmap_load_flying_vehicle_position(&xm_worldmap_player_position);
        xm_worldmap_player_heading = xm_game_data.worldmap.vehicle_heading;
        return;
    }
    for (spot = xm_worldmap_arrival_points; spot->id != -1; spot++) {
        if (spot->id == id) {
            xm_worldmap_player_position.vx = spot->x << 12;
            xm_worldmap_player_position.vy = 0;
            xm_worldmap_player_position.vz = spot->z << 12;
            return;
        }
    }
    xm_worldmap_player_position.vx = 0;
    xm_worldmap_player_position.vy = 0;
    xm_worldmap_player_position.vz = 0;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_unpack_area_data (src/decomp/src/worldmap/worldmap_open_map.c:631). */
#ifndef XM_T_AreaHeader
#define XM_T_AreaHeader
typedef struct {
    s32 unk0;
    s32 spots;          
    s32 models;         
    s32 meshes;         
    s32 placements;     
    s32 billboards;     
    s32 emitters;       
    s32 names;          
    s32 animations;     
    s32 animations2;    
    s32 unk28;
    s32 encounters[16]; 
} AreaHeader;
#endif
#ifndef XM_T_BillboardList
#define XM_T_BillboardList
typedef struct BillboardList {
    u8 *data;  
    s32 count;
} BillboardList;
#endif
#ifndef XM_T_AreaObject
#define XM_T_AreaObject
typedef struct AreaObject {
    s32 unk0;          
    s32 unk4;          
    s16 unk8;          
    s16 unkA;          
    s32 life;          
    s16 unk10;         
    s16 unk12;         
    SVECTOR position;  
    SVECTOR angle;     
    SVECTOR unk24;     
    SVECTOR direction; 
    s32 speed;         
    s16 accel[3];      
    s16 pad3E;
    u16 spread[2];     
    s32 rot;           
    s32 spin;          
    u8 rgb[3];         
    u8 flags;          
    s32 fade;          
} AreaObject;
#endif
#ifndef XM_T_SpotHeader
#define XM_T_SpotHeader
typedef struct {
    s32 spots;
    s32 table;
} SpotHeader;
#endif
#ifndef XM_S_worldmap_unpack_area_data
#define XM_S_worldmap_unpack_area_data
void xm_worldmap_unpack_area_data(void) __asm__("func_80073530");
#endif
#ifndef XM_S_text_unpack_lzss_alloc
#define XM_S_text_unpack_lzss_alloc
void *xm_text_unpack_lzss_alloc(void *data, s32 mode) __asm__("LZSSHeapDecompress");
#endif
#ifndef XM_S_worldmap_object_models
#define XM_S_worldmap_object_models
extern void *xm_worldmap_object_models __asm__("D_8009CD48");
#endif
#ifndef XM_S_worldmap_object_meshes
#define XM_S_worldmap_object_meshes
extern void *xm_worldmap_object_meshes __asm__("D_8009D308");
#endif
#ifndef XM_S_worldmap_object_placement_list
#define XM_S_worldmap_object_placement_list
extern void *xm_worldmap_object_placement_list __asm__("D_8009BD30");
#endif
#ifndef XM_S_worldmap_billboard_lists
#define XM_S_worldmap_billboard_lists
extern BillboardList *xm_worldmap_billboard_lists __asm__("D_8009C7EC");
#endif
#ifndef XM_S_worldmap_effect_emitters
#define XM_S_worldmap_effect_emitters
extern AreaObject *xm_worldmap_effect_emitters __asm__("D_8009BCC0");
#endif
#ifndef XM_S_worldmap_name_table
#define XM_S_worldmap_name_table
extern void *xm_worldmap_name_table __asm__("D_8009D784");
#endif
#ifndef XM_S_worldmap_texture_anim_section
#define XM_S_worldmap_texture_anim_section
extern s32 *xm_worldmap_texture_anim_section __asm__("D_8009D77C");
#endif
#ifndef XM_S_worldmap_texture_anim2_section
#define XM_S_worldmap_texture_anim2_section
extern s32 *xm_worldmap_texture_anim2_section __asm__("D_8009D7C8");
#endif
#ifndef XM_S_worldmap_encounter_sets
#define XM_S_worldmap_encounter_sets
extern void *xm_worldmap_encounter_sets[16] __asm__("D_8009D73C");
#endif
#ifndef XM_S_worldmap_path_tables
#define XM_S_worldmap_path_tables
extern s32 *xm_worldmap_path_tables __asm__("D_8009BD00");
#endif
void func_80073530(void) {
    u8 *block;
    u8 *base;
    AreaHeader *area;
    s32 *table;
    s32 i;

    block = xm_worldmap_area_data;
    xm_worldmap_area_data = xm_text_unpack_lzss_alloc(block, 0);
    xm_heap_free(block);
    base = xm_worldmap_area_data;
    area = (AreaHeader *)base;
    block = base + area->spots;
    xm_worldmap_object_models = base + area->models;
    xm_worldmap_object_meshes = base + area->meshes;
    xm_worldmap_object_placement_list = base + area->placements;
    xm_worldmap_billboard_lists = (BillboardList *)(base + area->billboards);
    xm_worldmap_effect_emitters = (AreaObject *)(base + area->emitters);
    xm_worldmap_name_table = base + area->names;
    xm_worldmap_texture_anim_section = (s32 *)(base + area->animations);
    xm_worldmap_texture_anim2_section = (s32 *)(base + area->animations2);
    for (i = 0; i < 16; i++) {
        xm_worldmap_encounter_sets[i] = xm_worldmap_area_data + area->encounters[i];
    }
    xm_worldmap_arrival_points = (WorldmapSpot *)(block + ((SpotHeader *)block)->spots);
    xm_worldmap_path_tables = table = (s32 *)(block + ((SpotHeader *)block)->table);
    table[0] = (s32)block + table[0];
    xm_worldmap_path_tables[1] = (s32)block + table[1];
    xm_worldmap_path_tables[2] = (s32)block + table[2];
    xm_worldmap_path_tables[3] = (s32)block + table[3];
}
#endif


#ifndef XENO_PC_PORT
void func_8007369C(void) {
    D_8009BC38 = HeapAlloc(0x1000, 0);
    D_8009BCB0 = HeapAlloc(0x1000, 0);
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800736DC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800737EC);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_horizon_init (src/decomp/src/worldmap/worldmap_open_map.c:732). */
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
#ifndef XM_T_DR_TWIN
#define XM_T_DR_TWIN
typedef struct {
    u_long tag;
    u_long code[2];
} DR_TWIN;
#endif
#ifndef XM_S_worldmap_horizon_init
#define XM_S_worldmap_horizon_init
void xm_worldmap_horizon_init(void) __asm__("func_800739B8");
#endif
#ifndef XM_S_GetTPage
#define XM_S_GetTPage
u_short xm_GetTPage(int tp, int abr, int x, int y) __asm__("GetTPage");
#endif
#ifndef XM_S_GetClut
#define XM_S_GetClut
u_short xm_GetClut(int x, int y) __asm__("GetClut");
#endif
#ifndef XM_S_worldmap_horizon_quads
#define XM_S_worldmap_horizon_quads
extern POLY_FT4 xm_worldmap_horizon_quads[2][2] __asm__("D_8009C744");
#endif
#ifndef XM_S_SetSemiTrans
#define XM_S_SetSemiTrans
void xm_SetSemiTrans(void *p, int abe) __asm__("SetSemiTrans");
#endif
#ifndef XM_S_SetTexWindow
#define XM_S_SetTexWindow
void xm_SetTexWindow(DR_TWIN *p, RECT *tw) __asm__("func_800453AC");
#endif
#ifndef XM_S_worldmap_horizon_texture_windows
#define XM_S_worldmap_horizon_texture_windows
extern DR_TWIN xm_worldmap_horizon_texture_windows[2] __asm__("D_8009D3D8");
#endif
void func_800739B8(void) {
    RECT window;
    POLY_FT4 *quad;
    u16 tpage;
    u16 clut;
    s32 i;

    tpage = xm_GetTPage(0, 1, 0x380, 0x100);
    clut = xm_GetClut(0x110, 0x1FE);
    quad = xm_worldmap_horizon_quads[0];
    for (i = 0; i < 4; i++, quad++) {
        ((u8 *)quad)[3] = 9;
        quad->code = 0x2C;
        quad->r0 = 0x30;
        quad->g0 = 0x30;
        quad->b0 = 0x30;
        quad->u0 = 0;
        quad->v0 = 0;
        quad->u1 = 0xFF;
        quad->v1 = 0;
        quad->u2 = 0;
        quad->v2 = 0x3F;
        quad->u3 = 0xFF;
        quad->v3 = 0x3F;
        quad->tpage = tpage;
        quad->clut = clut;
        xm_SetSemiTrans(quad, 1);
    }
    window.x = 0;
    window.y = 0;
    window.w = 0x80;
    window.h = 0;
    xm_SetTexWindow(&xm_worldmap_horizon_texture_windows[0], &window);
    window.x = 0;
    window.y = 0;
    window.w = 0;
    window.h = 0;
    xm_SetTexWindow(&xm_worldmap_horizon_texture_windows[1], &window);
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80073B04);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80073E30);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800740B8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80074594);


#ifndef XENO_PC_PORT
void func_8007474C(void) {
    HeapFree(D_8009BE18);
    HeapFree(D_8009BE14);
    HeapFree(D_8009D30C);
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_footprints_add (src/decomp/src/worldmap/worldmap_open_map.c:994). */
#ifndef XM_S_worldmap_footprints_add
#define XM_S_worldmap_footprints_add
void xm_worldmap_footprints_add(s16 id, VECTOR *position) __asm__("func_80074794");
#endif
#ifndef XM_S_worldmap_footprints
#define XM_S_worldmap_footprints
extern WorldmapSpot *xm_worldmap_footprints __asm__("D_8009D30C");
#endif
#ifndef XM_S_worldmap_footprint_count
#define XM_S_worldmap_footprint_count
extern s32 xm_worldmap_footprint_count __asm__("D_8009BE38");
#endif
void func_80074794(s16 id, VECTOR *position) {
    xm_worldmap_footprints[xm_worldmap_footprint_count].x = position->vx >> 12;
    xm_worldmap_footprints[xm_worldmap_footprint_count].z = position->vz >> 12;
    xm_worldmap_footprints[xm_worldmap_footprint_count].id = id;
    xm_worldmap_footprint_count = (xm_worldmap_footprint_count + 1) & 0xF;
}
#endif
INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800747DC);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_texture_anim_create (src/decomp/src/worldmap/worldmap_open_map.c:1129). */
#ifndef XM_T_TexAnimFrame
#define XM_T_TexAnimFrame
typedef struct {
    s16 image;    
    s16 duration; 
} TexAnimFrame;
#endif
#ifndef XM_T_TexAnimSlot
#define XM_T_TexAnimSlot
typedef struct {
    RECT rect;
    s32 unk8;
    TexAnimFrame *frames;
} TexAnimSlot;
#endif
#ifndef XM_T_TexAnim
#define XM_T_TexAnim
typedef struct TexAnim {
    u8 *images;
    TexAnimSlot *slot;
    s16 frame;
    s16 timer;
} TexAnim;
#endif
#ifndef XM_S_worldmap_texture_anim_create
#define XM_S_worldmap_texture_anim_create
void xm_worldmap_texture_anim_create(void) __asm__("func_80074E58");
#endif
#ifndef XM_S_worldmap_texture_anim_count
#define XM_S_worldmap_texture_anim_count
extern s32 xm_worldmap_texture_anim_count __asm__("D_8009CC9C");
#endif
#ifndef XM_S_worldmap_texture_anims
#define XM_S_worldmap_texture_anims
extern struct TexAnim *xm_worldmap_texture_anims __asm__("D_8009D780");
#endif
#ifndef XM_S_worldmap_texture_anim_slots
#define XM_S_worldmap_texture_anim_slots
extern TexAnimSlot xm_worldmap_texture_anim_slots[2] __asm__("D_8009A1E8");
#endif
void func_80074E58(void) {
    TexAnim *anim;
    s32 i;
    s32 count;

    count = *xm_worldmap_texture_anim_section;
    xm_worldmap_texture_anim_count = count;
    xm_worldmap_texture_anims = anim = xm_heap_alloc(count * sizeof(TexAnim), 0);
    for (i = 0; i < xm_worldmap_texture_anim_count; i++, anim++) {
        anim->images = (u8 *)xm_worldmap_texture_anim_section + xm_worldmap_texture_anim_section[i + 1];
        anim->slot = &xm_worldmap_texture_anim_slots[i];
        anim->frame = 0;
        anim->timer = 1;
    }
}
#endif


#ifndef XENO_PC_PORT
void func_80074F04(void) {
    HeapFree(D_8009D780);
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_texture_anim_advance (src/decomp/src/worldmap/worldmap_open_map.c:1151). */
#ifndef XM_S_worldmap_texture_anim_advance
#define XM_S_worldmap_texture_anim_advance
void xm_worldmap_texture_anim_advance(void) __asm__("func_80074F2C");
#endif
#ifndef XM_S_LoadImage
#define XM_S_LoadImage
int xm_LoadImage(RECT *rect, u_long *p) __asm__("LoadImage");
#endif
void func_80074F2C(void) {
    TexAnim *anim;
    s32 i;

    anim = xm_worldmap_texture_anims;
    for (i = 0; i < xm_worldmap_texture_anim_count; i++, anim++) {
        if (--anim->timer == 0) {
            anim->frame++;
            anim->timer = anim->slot->frames[anim->frame].duration;
            if (anim->timer < 0) {
                anim->frame = 0;
                anim->timer = anim->slot->frames[0].duration;
            }
            xm_LoadImage(&anim->slot->rect,
                      (u_long *)(anim->images + anim->slot->frames[anim->frame].image * 16));
        }
    }
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_texture_anim2_create (src/decomp/src/worldmap/worldmap_open_map.c:1171). */
#ifndef XM_S_worldmap_texture_anim2_create
#define XM_S_worldmap_texture_anim2_create
void xm_worldmap_texture_anim2_create(void) __asm__("func_80075030");
#endif
#ifndef XM_S_worldmap_texture_anim2_count
#define XM_S_worldmap_texture_anim2_count
extern s32 xm_worldmap_texture_anim2_count __asm__("D_8009CD64");
#endif
#ifndef XM_S_worldmap_texture_anims2
#define XM_S_worldmap_texture_anims2
extern TexAnim *xm_worldmap_texture_anims2 __asm__("D_8009D7D0");
#endif
#ifndef XM_S_worldmap_texture_anim2_slots
#define XM_S_worldmap_texture_anim2_slots
extern TexAnimSlot xm_worldmap_texture_anim2_slots[3] __asm__("D_8009A250");
#endif
void func_80075030(void) {
    TexAnim *anim;
    s32 i;
    s32 count;

    count = *xm_worldmap_texture_anim2_section;
    xm_worldmap_texture_anim2_count = count;
    xm_worldmap_texture_anims2 = anim = xm_heap_alloc(count * sizeof(TexAnim), 0);
    for (i = 0; i < xm_worldmap_texture_anim2_count; i++, anim++) {
        anim->images = (u8 *)xm_worldmap_texture_anim2_section + xm_worldmap_texture_anim2_section[i + 1];
        anim->slot = &xm_worldmap_texture_anim2_slots[i];
        anim->frame = 0;
        anim->timer = 1;
    }
}
#endif


#ifndef XENO_PC_PORT
void func_800750DC(void) {
    HeapFree(D_8009D7D0);
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_texture_anim2_advance (src/decomp/src/worldmap/worldmap_open_map.c:1193). */
#ifndef XM_S_worldmap_texture_anim2_advance
#define XM_S_worldmap_texture_anim2_advance
void xm_worldmap_texture_anim2_advance(void) __asm__("func_80075104");
#endif
void func_80075104(void) {
    TexAnim *anim;
    RECT *rect;
    s32 size;
    s32 i;

    anim = xm_worldmap_texture_anims2;
    for (i = 0; i < xm_worldmap_texture_anim2_count; i++, anim++) {
        if (--anim->timer == 0) {
            anim->frame++;
            anim->timer = anim->slot->frames[anim->frame].duration;
            if (anim->timer < 0) {
                anim->frame = 0;
                anim->timer = anim->slot->frames[0].duration;
            }
            rect = &anim->slot->rect;
            size = rect->h * rect->w * 2;
            xm_LoadImage(rect,
                      (u_long *)(anim->images + anim->slot->frames[anim->frame].image * size));
        }
    }
}
#endif

#ifndef XENO_PC_PORT
extern s32 D_8009BCC4;
extern s32 D_8009BE40;
extern s32 D_8009D64C;
extern s32 D_8009D80C;
extern s16 D_8009C854[];
void func_80075228(void) {
    s32 i;

    for (i = 15; i >= 0; i--) {
        D_8009C854[i] = 0;
    }
    D_8009D64C = 1;
    if (*(u16 *)(g_GameState + 0x1834) & 0x4000) {
        D_8009BE40 = 0x300;
    } else {
        D_8009BE40 = 0x180;
    }
    D_8009BCC4 = 1;
    D_8009D80C = 0;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_encounter_update_timers (src/decomp/src/worldmap/worldmap_open_map.c:1236). */
#ifndef XM_S_worldmap_encounter_update_timers
#define XM_S_worldmap_encounter_update_timers
void xm_worldmap_encounter_update_timers(void) __asm__("func_8007528C");
#endif
#ifndef XM_S_worldmap_encounter_reroll_timer
#define XM_S_worldmap_encounter_reroll_timer
extern s32 xm_worldmap_encounter_reroll_timer __asm__("D_8009D64C");
#endif
#ifndef XM_S_worldmap_encounter_timer_count
#define XM_S_worldmap_encounter_timer_count
extern s32 xm_worldmap_encounter_timer_count __asm__("D_8009BCC4");
#endif
#ifndef XM_S_rand
#define XM_S_rand
int xm_rand(void) __asm__("rand");
#endif
#ifndef XM_S_worldmap_encounter_period
#define XM_S_worldmap_encounter_period
extern s32 xm_worldmap_encounter_period __asm__("D_8009BE40");
#endif
#ifndef XM_S_worldmap_encounter_timers
#define XM_S_worldmap_encounter_timers
extern s16 xm_worldmap_encounter_timers[16] __asm__("D_8009C854");
#endif
#ifndef XM_S_worldmap_encounter_expired_count
#define XM_S_worldmap_encounter_expired_count
extern s32 xm_worldmap_encounter_expired_count __asm__("D_8009D80C");
#endif
void func_8007528C(void) {
    s32 i;
    s32 j;
    s32 value;

    if (--xm_worldmap_encounter_reroll_timer == 0) {
        for (i = 0; i < xm_worldmap_encounter_timer_count; i++) {
            if (i != 0) {
                do {
                    value = xm_rand() % xm_worldmap_encounter_period + 1;
                    for (j = 0; j < i; j++) {
                        if (xm_worldmap_encounter_timers[j] == value) {
                            break;
                        }
                    }
                } while (j < i);
            } else {
                value = xm_rand() % xm_worldmap_encounter_period + 1;
            }
            xm_worldmap_encounter_timers[i] = value;
        }
        xm_worldmap_encounter_reroll_timer = xm_worldmap_encounter_period;
    }
    xm_worldmap_encounter_expired_count = 0;
    for (i = 0; i < xm_worldmap_encounter_timer_count; i++) {
        if (--xm_worldmap_encounter_timers[i] == 0) {
            xm_worldmap_encounter_expired_count++;
        }
    }
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_save_state (src/decomp/src/worldmap/worldmap_open_map.c:1268). */
#ifndef XM_T_ActorSet
#define XM_T_ActorSet
typedef struct {
    WorldmapActor actor[64];
} ActorSet;
#endif
#ifndef XM_T_TimerSet
#define XM_T_TimerSet
typedef struct {
    s16 timer[16];
} TimerSet;
#endif
#ifndef XM_T_TrailPoint
#define XM_T_TrailPoint
typedef struct TrailPoint {
    VECTOR position;
    u16 heading;
    u16 pad;
} TrailPoint;
#endif
#ifndef XM_T_WorldmapQueue
#define XM_T_WorldmapQueue
typedef struct {
    TrailPoint points[32];
} WorldmapQueue;
#endif
#ifndef XM_T_WorldmapSave
#define XM_T_WorldmapSave
typedef struct {
    ActorSet actors;       
    VECTOR position;       
    s32 unk2010;           
    s32 timer_period;      
    s32 timer_count;       
    s32 timer_countdown;   
    TimerSet timers;       
    WorldmapQueue queue;   
    s32 queue_count;       
    s32 camera_angle[2];   
    s32 camera_distance;   
    s32 unk22D0;           
    VECTOR unk22D4;        
    s32 unk22E4[2];        
    VECTOR camera_target;  
} WorldmapSave;
#endif
#ifndef XM_S_mode_snapshot_block
#define XM_S_mode_snapshot_block
extern u8 xm_mode_snapshot_block[] __asm__("D_8005A4E4");
#endif
#ifndef XM_S_worldmap_camera_follow_target
#define XM_S_worldmap_camera_follow_target
extern Camera xm_worldmap_camera_follow_target __asm__("D_8009D55C");
#endif
#ifndef XM_S_worldmap_camera_follow_heading
#define XM_S_worldmap_camera_follow_heading
extern u16 xm_worldmap_camera_follow_heading __asm__("D_8009D52C");
#endif
#ifndef XM_S_worldmap_trail_points
#define XM_S_worldmap_trail_points
extern TrailPoint xm_worldmap_trail_points[32] __asm__("D_8009CEC4");
#endif
#ifndef XM_S_worldmap_trail_index
#define XM_S_worldmap_trail_index
extern s16 xm_worldmap_trail_index __asm__("D_8009D154");
#endif
#ifndef XM_S_worldmap_camera_angle
#define XM_S_worldmap_camera_angle
extern SVECTOR xm_worldmap_camera_angle __asm__("D_8009BD38");
#endif
#ifndef XM_S_worldmap_camera_distance
#define XM_S_worldmap_camera_distance
extern s32 xm_worldmap_camera_distance __asm__("D_8009D3F0");
#endif
#ifndef XM_S_worldmap_view_center_y
#define XM_S_worldmap_view_center_y
extern s32 xm_worldmap_view_center_y __asm__("D_8009BE0C");
#endif
#ifndef XM_S_worldmap_camera_block_cell
#define XM_S_worldmap_camera_block_cell
extern SVECTOR xm_worldmap_camera_block_cell __asm__("D_8009C838");
#endif
void func_80075460(void) {
    WorldmapSave *save;

    save = &xm_mode_snapshot_block;
    save->actors = *(ActorSet *)xm_worldmap_actor_slots;
    save->position.vx = xm_worldmap_camera_follow_target.target.vx;
    save->position.vy = xm_worldmap_camera_follow_target.target.vy;
    save->position.vz = xm_worldmap_camera_follow_target.target.vz;
    save->unk2010 = (s16)xm_worldmap_camera_follow_heading;
    save->timer_period = xm_worldmap_encounter_period;
    save->timer_count = xm_worldmap_encounter_timer_count;
    save->timer_countdown = xm_worldmap_encounter_reroll_timer;
    save->timers = *(TimerSet *)xm_worldmap_encounter_timers;
    save->queue = *(WorldmapQueue *)xm_worldmap_trail_points;
    save->queue_count = xm_worldmap_trail_index;
    save->camera_angle[0] = ((s32 *)&xm_worldmap_camera_angle)[0];
    save->camera_angle[1] = ((s32 *)&xm_worldmap_camera_angle)[1];
    save->camera_distance = xm_worldmap_camera_distance;
    save->unk22D0 = xm_worldmap_view_center_y;
    save->unk22E4[0] = ((s32 *)&xm_worldmap_camera_block_cell)[0];
    save->unk22E4[1] = ((s32 *)&xm_worldmap_camera_block_cell)[1];
    save->unk22D4.vx = xm_worldmap_terrain_origin.vx;
    save->unk22D4.vy = xm_worldmap_terrain_origin.vy;
    save->unk22D4.vz = xm_worldmap_terrain_origin.vz;
    save->camera_target.vx = xm_worldmap_camera.target.vx;
    save->camera_target.vy = xm_worldmap_camera.target.vy;
    save->camera_target.vz = xm_worldmap_camera.target.vz;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_restore_state (src/decomp/src/worldmap/worldmap_open_map.c:1298). */
#ifndef XM_S_worldmap_restore_state
#define XM_S_worldmap_restore_state
void xm_worldmap_restore_state(void) __asm__("func_8007565C");
#endif
void func_8007565C(void) {
    WorldmapSave *save;

    save = &xm_mode_snapshot_block;
    *(ActorSet *)xm_worldmap_actor_slots = save->actors;
    xm_worldmap_player_position = save->position;
    xm_worldmap_camera_follow_target.target = save->position;
    *(TimerSet *)xm_worldmap_encounter_timers = save->timers;
    xm_worldmap_camera_follow_heading = save->unk2010;
    xm_worldmap_encounter_period = save->timer_period;
    xm_worldmap_encounter_timer_count = save->timer_count;
    xm_worldmap_encounter_reroll_timer = save->timer_countdown;
    *(WorldmapQueue *)xm_worldmap_trail_points = save->queue;
    xm_worldmap_trail_index = save->queue_count;
    xm_worldmap_camera_angle = *(SVECTOR *)save->camera_angle;
    xm_worldmap_camera_distance = save->camera_distance;
    xm_worldmap_view_center_y = save->unk22D0;
    xm_worldmap_terrain_origin = save->unk22D4;
    xm_worldmap_camera_block_cell = *(SVECTOR *)save->unk22E4;
    xm_worldmap_camera.target = save->camera_target;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_suspend_open_map (src/decomp/src/worldmap/worldmap_open_map.c:1322). */
#ifndef XM_S_worldmap_suspend_open_map
#define XM_S_worldmap_suspend_open_map
void xm_worldmap_suspend_open_map(void) __asm__("func_800758C0");
#endif
#ifndef XM_S_worldmap_saved_gear_riding_lock
#define XM_S_worldmap_saved_gear_riding_lock
extern s32 xm_worldmap_saved_gear_riding_lock __asm__("D_8009D14C");
#endif
#ifndef XM_S_mode_gear_riding_lock
#define XM_S_mode_gear_riding_lock
extern u8 xm_mode_gear_riding_lock __asm__("D_80059179");
#endif
#ifndef XM_S_worldmap_terrain_get_layer
#define XM_S_worldmap_terrain_get_layer
s16 xm_worldmap_terrain_get_layer(VECTOR *position) __asm__("func_80093F18");
#endif
#ifndef XM_S_worldmap_stream_drain
#define XM_S_worldmap_stream_drain
void xm_worldmap_stream_drain(void) __asm__("func_80096694");
#endif
#ifndef XM_S_worldmap_suspend_memory
#define XM_S_worldmap_suspend_memory
extern void *xm_worldmap_suspend_memory __asm__("D_8009C7E4");
#endif
#ifndef XM_S_worldmap_read_shared_files
#define XM_S_worldmap_read_shared_files
void xm_worldmap_read_shared_files(void) __asm__("func_80071FEC");
#endif
#ifndef XM_S_worldmap_saved_vram_page
#define XM_S_worldmap_saved_vram_page
extern void *xm_worldmap_saved_vram_page __asm__("D_8009C800");
#endif
#ifndef XM_S_worldmap_saved_vram_cluts
#define XM_S_worldmap_saved_vram_cluts
extern void *xm_worldmap_saved_vram_cluts __asm__("D_8009C890");
#endif
#ifndef XM_S_StoreImage
#define XM_S_StoreImage
int xm_StoreImage(RECT *rect, u_long *p) __asm__("StoreImage");
#endif
#ifndef XM_S_worldmap_display_buffer_index
#define XM_S_worldmap_display_buffer_index
extern s32 xm_worldmap_display_buffer_index __asm__("D_8009D7F0");
#endif
#ifndef XM_S_MoveImage
#define XM_S_MoveImage
int xm_MoveImage(RECT *rect, int x, int y) __asm__("MoveImage");
#endif
#ifndef XM_S_DrawSync
#define XM_S_DrawSync
int xm_DrawSync(int mode) __asm__("DrawSync");
#endif
#ifndef XM_S_worldmap_fade_saved_screen
#define XM_S_worldmap_fade_saved_screen
void xm_worldmap_fade_saved_screen(s32 frames, s32 level, s32 step, s32 abr) __asm__("func_80072DB4");
#endif
#ifndef XM_S_cd_get_pending_read_count
#define XM_S_cd_get_pending_read_count
s32 xm_cd_get_pending_read_count(void) __asm__("ArchiveDataSync");
#endif
#ifndef XM_S_text_unpack_lzss
#define XM_S_text_unpack_lzss
void *xm_text_unpack_lzss(void *source, void *destination) __asm__("LZSSDecompress");
#endif
#ifndef XM_S_worldmap_packed_menu_overlay
#define XM_S_worldmap_packed_menu_overlay
extern void *xm_worldmap_packed_menu_overlay __asm__("D_8009D528");
#endif
#ifndef XM_S_cd_sync_reads
#define XM_S_cd_sync_reads
s32 xm_cd_sync_reads(s32 mode) __asm__("ArchiveCdDataSync");
#endif
void func_800758C0(void) {
    RECT rect;
    void *block;
    s32 i;

    xm_game_data.worldmap.unk6A = 1;
    xm_game_data.entry[0] = (xm_worldmap_camera_angle.vy + 0x2000) & 0x3FFF;
    for (i = 0; i < 3; i++) {
        (&xm_game_data.worldmap.unk70)[i] = xm_game_data.inGear[i];
    }
    xm_worldmap_saved_gear_riding_lock = xm_mode_gear_riding_lock;
    if (xm_worldmap_terrain_get_layer(&xm_worldmap_camera_follow_target.target) == 4) {
        xm_mode_gear_riding_lock = 1;
    }
    xm_worldmap_stream_drain();
    xm_worldmap_billboards_free_quads();
    xm_worldmap_clouds_free_quads();
    xm_worldmap_effects_free_quads();
    xm_heap_free(xm_worldmap_display_buffers[0].packets);
    xm_heap_free(xm_worldmap_display_buffers[1].packets);
    block = xm_heap_alloc(4, 1);
    xm_heap_free(block);
    block = (void *)((u32)block & 0xFFFFFF);
    xm_worldmap_suspend_memory = xm_heap_alloc((u32)block - 0x1C4FFC, 1);
    xm_worldmap_read_shared_files();
    xm_worldmap_saved_vram_page = xm_heap_alloc(0x10000, 0);
    xm_worldmap_saved_vram_cluts = xm_heap_alloc(0xC800, 0);
    rect.x = 0x180;
    rect.y = 0x100;
    rect.w = 0x80;
    rect.h = 0x100;
    xm_StoreImage(&rect, xm_worldmap_saved_vram_page);
    rect.x = 0;
    rect.y = 0x1B0;
    rect.w = 0x140;
    rect.h = 0x50;
    xm_StoreImage(&rect, xm_worldmap_saved_vram_cluts);
    if (xm_worldmap_display_buffer_index == 0) {
        rect.x = 0;
        rect.y = 0xD8;
        rect.w = 0x140;
        rect.h = 0xD8;
        xm_MoveImage(&rect, 0, 0);
    }
    rect.x = 0;
    rect.y = 0;
    rect.w = 0x140;
    rect.h = 0xD8;
    xm_MoveImage(&rect, 0x2C0, 0x100);
    xm_DrawSync(0);
    xm_worldmap_fade_saved_screen(0x10, 0, 8, 2);
    while (xm_cd_get_pending_read_count() >= 2) {
    }
    xm_text_unpack_lzss(xm_worldmap_packed_menu_overlay, xm_worldmap_suspend_memory);
    xm_heap_free(xm_worldmap_packed_menu_overlay);
    rect.x = 0;
    rect.y = 0;
    rect.w = 0x140;
    rect.h = 0xD8;
    xm_MoveImage(&rect, 0, 0xE0);
    xm_DrawSync(0);
    xm_cd_sync_reads(0);
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_resume_open_map (src/decomp/src/worldmap/worldmap_open_map.c:1388). */
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
#ifndef XM_S_worldmap_resume_open_map
#define XM_S_worldmap_resume_open_map
void xm_worldmap_resume_open_map(void) __asm__("func_80075B58");
#endif
#ifndef XM_S_heap_select_owner_tag
#define XM_S_heap_select_owner_tag
void xm_heap_select_owner_tag(s32 tag, char **names) __asm__("HeapChangeCurrentUser");
#endif
#ifndef XM_S_cd_select_directory
#define XM_S_cd_select_directory
s32 xm_cd_select_directory(s32 group, s32 index) __asm__("ArchiveSetIndex");
#endif
#ifndef XM_S_menu_state_resource_file
#define XM_S_menu_state_resource_file
extern void *xm_menu_state_resource_file __asm__("D_8005945C");
#endif
#ifndef XM_S_cd_read_file
#define XM_S_cd_read_file
s32 xm_cd_read_file(s32 file, void *destination, s32 mode, s32 flags) __asm__("ArchiveReadFileToBuffer");
#endif
#ifndef XM_S_ClearOTagR
#define XM_S_ClearOTagR
u_long *xm_ClearOTagR(u_long *ot, int n) __asm__("ClearOTagR");
#endif
#ifndef XM_S_worldmap_upload_area_image
#define XM_S_worldmap_upload_area_image
void xm_worldmap_upload_area_image(void) __asm__("func_8008440C");
#endif
#ifndef XM_S_worldmap_terrain_alloc_packets
#define XM_S_worldmap_terrain_alloc_packets
void xm_worldmap_terrain_alloc_packets(void) __asm__("func_800978FC");
#endif
#ifndef XM_S_worldmap_effects_alloc_quads
#define XM_S_worldmap_effects_alloc_quads
void xm_worldmap_effects_alloc_quads(void) __asm__("func_8008901C");
#endif
#ifndef XM_S_worldmap_clouds_alloc_quads
#define XM_S_worldmap_clouds_alloc_quads
void xm_worldmap_clouds_alloc_quads(void) __asm__("func_800865A0");
#endif
#ifndef XM_S_worldmap_billboards_alloc_quads
#define XM_S_worldmap_billboards_alloc_quads
void xm_worldmap_billboards_alloc_quads(void) __asm__("func_80085FE0");
#endif
#ifndef XM_S_text_load_palette
#define XM_S_text_load_palette
void xm_text_load_palette(s16 x, s16 y) __asm__("SystemTransferPaletteToVRAM");
#endif
#ifndef XM_S_VSync
#define XM_S_VSync
int xm_VSync(int mode) __asm__("Vsync");
#endif
#ifndef XM_S_pad_clear_queue
#define XM_S_pad_clear_queue
void xm_pad_clear_queue(void) __asm__("ControllerResetState");
#endif
#ifndef XM_S_worldmap_menu_requested
#define XM_S_worldmap_menu_requested
extern s32 xm_worldmap_menu_requested __asm__("D_8009D804");
#endif
#ifndef XM_S_worldmap_apply_party_riding_changes
#define XM_S_worldmap_apply_party_riding_changes
void xm_worldmap_apply_party_riding_changes(void) __asm__("func_80075D4C");
#endif
void func_80075B58(void) {
    RECT rect;

    xm_heap_select_owner_tag(3, 0);
    xm_cd_select_directory(0x24, 0);
    xm_heap_free(xm_menu_state_resource_file);
    xm_heap_free(xm_worldmap_suspend_memory);
    xm_worldmap_init_display();
    xm_worldmap_area_image = xm_heap_alloc(xm_cd_get_aligned_file_size(xm_worldmap_area_image_file), 1);
    xm_cd_read_file(xm_worldmap_area_image_file, xm_worldmap_area_image, 0, 0);
    xm_worldmap_fade_saved_screen(0x10, 0x80, -8, 2);
    rect.x = 0x2C0;
    rect.y = 0x100;
    rect.w = 0x140;
    rect.h = 0xD8;
    xm_MoveImage(&rect, 0, 0);
    xm_MoveImage(&rect, 0, 0xD8);
    xm_ClearOTagR(xm_worldmap_current_display_buffer->ot, 0x400);
    xm_cd_sync_reads(0);
    xm_worldmap_upload_area_image();
    rect.x = 0x180;
    rect.y = 0x100;
    rect.w = 0x80;
    rect.h = 0x100;
    xm_LoadImage(&rect, xm_worldmap_saved_vram_page);
    rect.x = 0;
    rect.y = 0x1B0;
    rect.w = 0x140;
    rect.h = 0x50;
    xm_LoadImage(&rect, xm_worldmap_saved_vram_cluts);
    xm_DrawSync(0);
    xm_heap_free(xm_worldmap_saved_vram_page);
    xm_heap_free(xm_worldmap_saved_vram_cluts);
    xm_worldmap_terrain_alloc_packets();
    xm_worldmap_effects_alloc_quads();
    xm_worldmap_clouds_alloc_quads();
    xm_worldmap_billboards_alloc_quads();
    xm_text_load_palette(0x130, 0x1E0);
    xm_VSync(0);
    xm_pad_clear_queue();
    xm_worldmap_menu_requested = 0;
    xm_game_data.worldmap.unk6A = 0;
    xm_mode_gear_riding_lock = xm_worldmap_saved_gear_riding_lock;
    xm_worldmap_apply_party_riding_changes();
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_apply_party_riding_changes (src/decomp/src/worldmap/worldmap_open_map.c:1438). */
#ifndef XM_T_VehicleSpot
#define XM_T_VehicleSpot
typedef struct {
    u16 flags; 
    u16 x;
    u16 z;
} VehicleSpot;
#endif
#ifndef XM_T_VEHICLE_SPOTS
#define XM_T_VEHICLE_SPOTS
#define VEHICLE_SPOTS ((VehicleSpot *)&xm_game_data.vars[21])
#endif
void func_80075D4C(void) {
    WorldmapActor *actors;
    u8 *applied;  

    s32 i;
    s32 count;
    u8 state;

    i = 0;
    actors = xm_worldmap_actor_slots;
    applied = (u8 *)&xm_game_data.worldmap.unk70;
    do {
        state = xm_game_data.inGear[i];
        if (state != applied[i * 2]) {
            if (state == 0) {
                actors[i + 1].position.vx = actors[i + 4].position.vx;
                actors[i + 1].position.vy = actors[i + 4].position.vy;
                actors[i + 1].position.vz = actors[i + 4].position.vz;
                actors[i + 1].unk58 = actors[i + 4].unk58;
            } else {
                VEHICLE_SPOTS[i].flags = 0x400;
                actors[i + 4].unk24 = 0;
                actors[i + 4].position.vx = actors[i + 1].position.vx;
                actors[i + 4].position.vy = actors[i + 1].position.vy;
                actors[i + 4].position.vz = actors[i + 1].position.vz;
                actors[i + 4].unk58 = actors[i + 1].unk58;
            }
        }
        i++;
    } while (i < 3);
    count = 0;
    for (i = 0; i < 3; i++) {
        if (xm_game_data.party[i] != 0xFF && xm_game_data.inGear[i] == 1) {
            count++;
        }
    }
    if (!(xm_game_data.worldmap.flags & 0x4000)) {
        xm_worldmap_movement_mode = count != 0 ? 2 : 1;
    }
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_encounter_roll (src/decomp/src/worldmap/worldmap_open_map.c:1483). */
#ifndef XM_T_BattleFormation
#define XM_T_BattleFormation
typedef struct BattleFormation {
    u8 battle;         

    u8 flags;          













    u8 stage;          


    u8 scriptSet;      

    u8 partyGroups[3]; 


    u8 unk7;           
    u8 enemyIds[8];    

    u8 enemyFlags[8];  


    u8 enemyGroups[8]; 



} BattleFormation;
#endif
#ifndef XM_T_EncounterSet
#define XM_T_EncounterSet
typedef struct EncounterSet {
    BattleFormation formations[16];
} EncounterSet;
#endif
#ifndef XM_S_worldmap_encounter_roll
#define XM_S_worldmap_encounter_roll
s32 xm_worldmap_encounter_roll(VECTOR *position, s32 scene) __asm__("func_80075E7C");
#endif
#ifndef XM_S_worldmap_terrain_get_cell_flags
#define XM_S_worldmap_terrain_get_cell_flags
s32 xm_worldmap_terrain_get_cell_flags(VECTOR *position) __asm__("func_80094028");
#endif
#ifndef XM_S_worldmap_layer4_encounter_kinds
#define XM_S_worldmap_layer4_encounter_kinds
extern s16 xm_worldmap_layer4_encounter_kinds[16] __asm__("D_8009A3A0");
#endif
#ifndef XM_S_worldmap_encounter_level_brackets
#define XM_S_worldmap_encounter_level_brackets
extern u16 xm_worldmap_encounter_level_brackets[] __asm__("D_8009B578");
#endif
#ifndef XM_S_formation_encounter_set
#define XM_S_formation_encounter_set
extern EncounterSet xm_formation_encounter_set __asm__("D_800658DC");
#endif
#ifndef XM_S_formation_selected_index
#define XM_S_formation_selected_index
extern u8 xm_formation_selected_index __asm__("D_80059508");
#endif
s32 func_80075E7C(VECTOR *position, s32 scene) {
    u8 weights[16];
    s32 kind;
    s32 bracket;
    s32 total;
    s32 roll;
    s32 formation;
    s32 result;
    s32 i;
    u8 *row;

    kind = (s16)xm_worldmap_terrain_get_cell_flags(position);
    if (xm_worldmap_terrain_get_layer(position) == 4) {
        kind = xm_worldmap_layer4_encounter_kinds[kind];
    }
    bracket = 1;
    while (scene >= xm_worldmap_encounter_level_brackets[bracket]) {
        bracket++;
    }
    bracket--;
    total = 0;
    row = (u8 *)xm_worldmap_encounter_sets[kind] + 0x200; 
    row += bracket * 16;
    for (i = 0; i < 16; i++) {
        weights[i] = row[i];
        total += row[i];
    }
    result = 0;
    if (total > 0) {
        roll = xm_rand() % total + 1;
        formation = 0;
        do {
            roll--;
        next:
            if (weights[formation] == 0) {
                formation++;
                goto next;
            }
            weights[formation]--;
        } while (roll > 0);
        xm_formation_encounter_set = *(EncounterSet *)xm_worldmap_encounter_sets[kind];
        xm_formation_selected_index = formation;
        result = 1;
    }
    return result;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_draw_distant_landmark (src/decomp/src/worldmap/worldmap_open_map.c:1554). */
#ifndef XM_T_LandmarkScratch
#define XM_T_LandmarkScratch
typedef struct {
    VECTOR position;  
    u8 pad10[0x10];
    s32 flag;         
    u8 pad24[4];
    s32 depth;        
    u8 pad2C[0x74];
    SVECTOR origin;   
    u8 padA8[0x48];
    MATRIX local;     
    MATRIX view;      
} LandmarkScratch;
#endif
#ifndef XM_T_LANDMARK_SCRATCH
#define XM_T_LANDMARK_SCRATCH
#define LANDMARK_SCRATCH ((LandmarkScratch *)0x1F800000)
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
#ifndef XM_T_DVECTOR
#define XM_T_DVECTOR
typedef struct {
    short vx, vy;
} DVECTOR;
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
#ifndef XM_T_Actor
#define XM_T_Actor
typedef struct Actor {
    ModelTable *models;     
    ModelPart *parts;       
    s32 *entries;           
    EntryTable *shared;     
    s32 pc;                 
    s32 *locals;            
    s32 *globals;           
    s16 scale;              
    s16 h1E;                
    u8 index;               
    u8 b21;                 
    u8 b22;                 
    u8 b23;                 
    s16 size[3];            
    u8 reference;           
    u8 depth;               
    u8 queue_source[4];     
    u8 queue_entry[4];      
    u8 active;              
    u8 b35;                 
    u8 b36;                 
    u8 scaled;              
    u8 b38;                 
    u8 b39;                 
    s16 h3A;                
    u16 h3C;                
    s16 h3E;                
    u16 h40;                
    u16 h42;                
    u16 h44;                
    u16 h46;                
    s16 h48;                
    u16 flags;              



    s32 w4C;                
    s32 w50;                
    s32 w54;                
    s16 aim_actor;          
    s16 aim_node;           
    u8 parent;              
    u8 inherit;             
    s16 parent_node;        
    s16 groundY;            


    u8 b62;                 
    u8 b63;                 
    s16 aim_offset[3];      
    s16 offset[3];          
    s16 spin[3];            
    s16 spin_accel[3];      
    s16 drift[3];           
    s16 drift_accel[3];     
    s16 target[3];          
    s16 h8E;                
    s16 h90;                
    s16 h92;                
    u16 shift_x;            
    u16 shift_y;            
    s16 anim_state;         
    s16 anim_loop;          
    s16 anim_frame;         
    s16 anim_frames;        
    u8 *anim_pos;           
    u8 *anim_start;         
    void *group;            
    void *blockAC;          
    SoundOwner *ownerB0;    
    SoundOwner *ownerB4;    
    POLY_FT4 prims[2];      
    u8 pad108[2];
    u16 mask;               
    u8 channel_count;       
    u8 surfaceCount;        
    u8 imageCount;          
    u8 pad10F;
    struct ColorFade *channels; 
    struct Surface *surfaces;   
    struct ImageAnim *images;   
    s32 previous[3];        
    s32 moved[3];           

} Actor;
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
#ifndef XM_T_gte_rtps
#define XM_T_gte_rtps
#define gte_rtps() __asm__ volatile("nop;nop;.word 0x4A180001")
#endif
#ifndef XM_T_gte_stflg
#define XM_T_gte_stflg
#define gte_stflg(r0)                                                          \
    __asm__ volatile("cfc2 $12, $31;"                                          \
                     "nop;"                                                    \
                     "sw $12, 0(%0)"                                           \
                     :                                                         \
                     : "r"(r0)                                                 \
                     : "$12", "memory")
#endif
#ifndef XM_T_gte_stsz
#define XM_T_gte_stsz
#define gte_stsz(r0) __asm__ volatile("swc2 $19, 0(%0)" : : "r"(r0) : "memory")
#endif
#ifndef XM_S_worldmap_draw_distant_landmark
#define XM_S_worldmap_draw_distant_landmark
void xm_worldmap_draw_distant_landmark(void) __asm__("func_80076098");
#endif
#ifndef XM_S_worldmap_wrap_world_offset
#define XM_S_worldmap_wrap_world_offset
void xm_worldmap_wrap_world_offset(VECTOR *offset) __asm__("func_80093534");
#endif
#ifndef XM_S_gear_model_actors
#define XM_S_gear_model_actors
extern Actor *xm_gear_model_actors[10] __asm__("D_801E8670");
#endif
#ifndef XM_S_worldmap_identity_matrix
#define XM_S_worldmap_identity_matrix
extern MATRIX xm_worldmap_identity_matrix __asm__("D_8009A180");
#endif
#ifndef XM_S_CompMatrix
#define XM_S_CompMatrix
MATRIX *xm_CompMatrix(MATRIX *m0, MATRIX *m1, MATRIX *m2) __asm__("CompMatrix");
#endif
#ifndef XM_S_worldmap_camera_matrix
#define XM_S_worldmap_camera_matrix
extern MATRIX xm_worldmap_camera_matrix __asm__("D_8009C808");
#endif
#ifndef XM_S_SetRotMatrix
#define XM_S_SetRotMatrix
void xm_SetRotMatrix(MATRIX *m) __asm__("SetRotMatrix");
#endif
#ifndef XM_S_SetTransMatrix
#define XM_S_SetTransMatrix
void xm_SetTransMatrix(MATRIX *m) __asm__("SetTransMatrix");
#endif
#ifndef XM_S_gear_model_color_matrix
#define XM_S_gear_model_color_matrix
extern MATRIX *xm_gear_model_color_matrix __asm__("D_801E8644");
#endif
#ifndef XM_S_worldmap_light_color_matrix
#define XM_S_worldmap_light_color_matrix
extern MATRIX xm_worldmap_light_color_matrix __asm__("D_8009A140");
#endif
#ifndef XM_S_gear_model_step_and_draw
#define XM_S_gear_model_step_and_draw
void xm_gear_model_step_and_draw(MATRIX *m, MATRIX *light, u_long *ot, s32 buffer, s32 elapsed) __asm__("func_801E7D14");
#endif
#ifndef XM_S_worldmap_light_direction_matrix
#define XM_S_worldmap_light_direction_matrix
extern MATRIX xm_worldmap_light_direction_matrix __asm__("D_8009A160");
#endif
void func_80076098(void) {
    s32 *flag;
    s32 *depth;

    LANDMARK_SCRATCH->position.vy = 0xA0;
    LANDMARK_SCRATCH->position.vx = 0x4E0E - (xm_worldmap_camera.target.vx >> 12);
    LANDMARK_SCRATCH->position.vz = 0x1B68 - (xm_worldmap_camera.target.vz >> 12);
    xm_worldmap_wrap_world_offset(&LANDMARK_SCRATCH->position);
    xm_gear_model_actors[0]->parts->translation[0] = LANDMARK_SCRATCH->position.vx;
    xm_gear_model_actors[0]->parts->translation[1] = 0;
    xm_gear_model_actors[0]->parts->translation[2] = -LANDMARK_SCRATCH->position.vz;
    xm_gear_model_actors[0]->parts->rotation.vx = xm_gear_model_actors[0]->parts->rotation.vy = xm_gear_model_actors[0]->parts->rotation.vz = 0;
    

    *(s16 *)&xm_gear_model_actors[0]->parent = -0x100;
    xm_gear_model_actors[0]->scale = 0x40;
    LANDMARK_SCRATCH->local = xm_worldmap_identity_matrix;
    LANDMARK_SCRATCH->local.t[0] = LANDMARK_SCRATCH->position.vx;
    LANDMARK_SCRATCH->local.t[1] = LANDMARK_SCRATCH->position.vy;
    LANDMARK_SCRATCH->local.t[2] = -LANDMARK_SCRATCH->position.vz;
    xm_CompMatrix(&xm_worldmap_camera_matrix, &LANDMARK_SCRATCH->local, &LANDMARK_SCRATCH->view);
    xm_SetRotMatrix(&LANDMARK_SCRATCH->view);
    xm_SetTransMatrix(&LANDMARK_SCRATCH->view);
    LANDMARK_SCRATCH->origin.vx = LANDMARK_SCRATCH->origin.vy = LANDMARK_SCRATCH->origin.vz = 0;
    gte_ldv0(&LANDMARK_SCRATCH->origin);
    gte_rtps();
    flag = &LANDMARK_SCRATCH->flag;
    gte_stflg(flag);
    if (*flag >= 0) {
        depth = &LANDMARK_SCRATCH->depth;
        gte_stsz(depth);
        if (*depth < 0xD00) {
            xm_gear_model_color_matrix = &xm_worldmap_light_color_matrix;
            xm_SetBackColor(0x40, 0x40, 0x40);
            xm_gear_model_step_and_draw(&xm_worldmap_camera_matrix, &xm_worldmap_light_direction_matrix, xm_worldmap_current_display_buffer->ot, xm_worldmap_display_buffer_index, 1);
        }
    }
}
#endif


#ifndef XENO_PC_PORT
void func_800762FC(void) {
    DrawSync(0);
    Vsync(0);
    EnterCriticalSection();
    DrawSync(0);
    Vsync(0);
    FlushCache();
    ExitCriticalSection();
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_run_pause_screen (src/decomp/src/worldmap/worldmap_open_map.c:1607). */
#ifndef XM_S_worldmap_run_pause_screen
#define XM_S_worldmap_run_pause_screen
void xm_worldmap_run_pause_screen(void) __asm__("func_8007634C");
#endif
#ifndef XM_S_pad_vblank_count
#define XM_S_pad_vblank_count
extern s32 xm_pad_vblank_count __asm__("D_80059488");
#endif
#ifndef XM_S_PutDispEnv
#define XM_S_PutDispEnv
DISPENV *xm_PutDispEnv(DISPENV *env) __asm__("PutDispEnv");
#endif
#ifndef XM_S_PutDrawEnv
#define XM_S_PutDrawEnv
DRAWENV *xm_PutDrawEnv(DRAWENV *env) __asm__("PutDrawEnv");
#endif
#ifndef XM_S_sound_silence_voices
#define XM_S_sound_silence_voices
void xm_sound_silence_voices(void) __asm__("SoundMuteAllSpuChannels");
#endif
#ifndef XM_S_sprite_upload_pause_image
#define XM_S_sprite_upload_pause_image
void xm_sprite_upload_pause_image(s32 x, s32 y) __asm__("GraphicsDrawPauseLetters");
#endif
#ifndef XM_S_worldmap_pad_unread_port1_repeated
#define XM_S_worldmap_pad_unread_port1_repeated
extern u16 xm_worldmap_pad_unread_port1_repeated __asm__("D_8009BD1C");
#endif
#ifndef XM_S_worldmap_pad_unread_port1_pressed
#define XM_S_worldmap_pad_unread_port1_pressed
extern u16 xm_worldmap_pad_unread_port1_pressed __asm__("D_8009BD14");
#endif
#ifndef XM_S_worldmap_pad_unread_port1_held
#define XM_S_worldmap_pad_unread_port1_held
extern u16 xm_worldmap_pad_unread_port1_held __asm__("D_8009CD50");
#endif
#ifndef XM_S_worldmap_pad_unread_port0_repeated
#define XM_S_worldmap_pad_unread_port0_repeated
extern u16 xm_worldmap_pad_unread_port0_repeated __asm__("D_8009BD18");
#endif
#ifndef XM_S_worldmap_pad_port0_pressed
#define XM_S_worldmap_pad_port0_pressed
extern u16 xm_worldmap_pad_port0_pressed __asm__("D_8009BD10");
#endif
#ifndef XM_S_worldmap_pad_port0_held
#define XM_S_worldmap_pad_port0_held
extern u16 xm_worldmap_pad_port0_held __asm__("D_8009CD4C");
#endif
#ifndef XM_S_pad_dequeue_state
#define XM_S_pad_dequeue_state
u32 xm_pad_dequeue_state(void) __asm__("ControllerPopState");
#endif
#ifndef XM_S_pad_port0_held
#define XM_S_pad_port0_held
extern u16 xm_pad_port0_held __asm__("g_C1ButtonState");
#endif
#ifndef XM_S_pad_port1_held
#define XM_S_pad_port1_held
extern u16 xm_pad_port1_held __asm__("g_C2ButtonState");
#endif
#ifndef XM_S_pad_port0_pressed
#define XM_S_pad_port0_pressed
extern u16 xm_pad_port0_pressed __asm__("g_C1ButtonStateReleased");
#endif
#ifndef XM_S_pad_port1_pressed
#define XM_S_pad_port1_pressed
extern u16 xm_pad_port1_pressed __asm__("g_C2ButtonStateReleased");
#endif
#ifndef XM_S_pad_port0_repeated
#define XM_S_pad_port0_repeated
extern u16 xm_pad_port0_repeated __asm__("g_C1ButtonStatePressedOnce");
#endif
#ifndef XM_S_pad_port1_repeated
#define XM_S_pad_port1_repeated
extern u16 xm_pad_port1_repeated __asm__("g_C2ButtonStatePressedOnce");
#endif
#ifndef XM_S_sound_restore_voices
#define XM_S_sound_restore_voices
void xm_sound_restore_voices(void) __asm__("SoundEnableAllSpuChannels");
#endif
void func_8007634C(void) {
    RECT rect;
    s32 saved;

    saved = xm_pad_vblank_count;
    xm_DrawSync(0);
    xm_VSync(0);
    if (xm_worldmap_display_buffer_index == 0) {
        rect.x = 0;
        rect.y = 0xD8;
        rect.w = 0x140;
        rect.h = 0xD8;
        xm_MoveImage(&rect, 0, 0);
    }
    xm_PutDispEnv(&xm_worldmap_display_buffers[1].disp);
    xm_PutDrawEnv(&xm_worldmap_display_buffers[1].draw);
    xm_sound_silence_voices();
    do {
        xm_DrawSync(0);
        xm_VSync(0);
        xm_sprite_upload_pause_image(0x88, 0x64);
        xm_worldmap_pad_unread_port1_repeated = 0;
        xm_worldmap_pad_unread_port1_pressed = 0;
        xm_worldmap_pad_unread_port1_held = 0;
        xm_worldmap_pad_unread_port0_repeated = 0;
        xm_worldmap_pad_port0_pressed = 0;
        xm_worldmap_pad_port0_held = 0;
        while (xm_pad_dequeue_state() != 0) {
            xm_worldmap_pad_port0_held |= xm_pad_port0_held;
            xm_worldmap_pad_unread_port1_held |= xm_pad_port1_held;
            xm_worldmap_pad_port0_pressed |= xm_pad_port0_pressed;
            xm_worldmap_pad_unread_port1_pressed |= xm_pad_port1_pressed;
            xm_worldmap_pad_unread_port0_repeated |= xm_pad_port0_repeated;
            xm_worldmap_pad_unread_port1_repeated |= xm_pad_port1_repeated;
        }
    } while (!(xm_worldmap_pad_port0_pressed & 0x800));
    xm_sound_restore_voices();
    xm_DrawSync(0);
    xm_VSync(0);
    rect.x = 0;
    rect.y = 0;
    rect.w = 0x140;
    rect.h = 0xD8;
    xm_MoveImage(&rect, 0, 0xD8);
    xm_PutDispEnv(&xm_worldmap_display_buffers[xm_worldmap_display_buffer_index].disp);
    xm_PutDrawEnv(&xm_worldmap_display_buffers[xm_worldmap_display_buffer_index].draw);
    xm_pad_vblank_count = saved;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_wait_for_controller (src/decomp/src/worldmap/worldmap_open_map.c:1658). */
#ifndef XM_S_worldmap_wait_for_controller
#define XM_S_worldmap_wait_for_controller
void xm_worldmap_wait_for_controller(void) __asm__("func_80076594");
#endif
#ifndef XM_S_pad_get_controller_kind
#define XM_S_pad_get_controller_kind
s32 xm_pad_get_controller_kind(s32 port) __asm__("ControllerGetType");
#endif
void func_80076594(void) {
    RECT rect;
    s32 saved;

    saved = xm_pad_vblank_count;
    xm_DrawSync(0);
    xm_VSync(0);
    if (xm_worldmap_display_buffer_index == 0) {
        rect.x = 0;
        rect.y = 0xD8;
        rect.w = 0x140;
        rect.h = 0xD8;
        xm_MoveImage(&rect, 0, 0);
    }
    xm_PutDispEnv(&xm_worldmap_display_buffers[1].disp);
    xm_PutDrawEnv(&xm_worldmap_display_buffers[1].draw);
    xm_sound_silence_voices();
    do {
        xm_DrawSync(0);
        xm_VSync(0);
        xm_sprite_upload_pause_image(0x88, 0x64);
        xm_worldmap_pad_unread_port1_repeated = 0;
        xm_worldmap_pad_unread_port1_pressed = 0;
        xm_worldmap_pad_unread_port1_held = 0;
        xm_worldmap_pad_unread_port0_repeated = 0;
        xm_worldmap_pad_port0_pressed = 0;
        xm_worldmap_pad_port0_held = 0;
        while (xm_pad_dequeue_state() != 0) {
            xm_worldmap_pad_port0_held |= xm_pad_port0_held;
            xm_worldmap_pad_unread_port1_held |= xm_pad_port1_held;
            xm_worldmap_pad_port0_pressed |= xm_pad_port0_pressed;
            xm_worldmap_pad_unread_port1_pressed |= xm_pad_port1_pressed;
            xm_worldmap_pad_unread_port0_repeated |= xm_pad_port0_repeated;
            xm_worldmap_pad_unread_port1_repeated |= xm_pad_port1_repeated;
        }
    } while (xm_pad_get_controller_kind(0) == 0);
    xm_sound_restore_voices();
    xm_DrawSync(0);
    xm_VSync(0);
    rect.x = 0;
    rect.y = 0;
    rect.w = 0x140;
    rect.h = 0xD8;
    xm_MoveImage(&rect, 0, 0xD8);
    xm_PutDispEnv(&xm_worldmap_display_buffers[xm_worldmap_display_buffer_index].disp);
    xm_PutDrawEnv(&xm_worldmap_display_buffers[xm_worldmap_display_buffer_index].draw);
    xm_pad_vblank_count = saved;
}
#endif

#ifndef XENO_PC_PORT
extern s32 D_80062648;
void func_800767D4(s32 arg0, s32 arg1) {
    s32 temp_v0;

    func_80039CC4();
    func_800399D4(D_80062528);
    memcpy(&D_80062648, arg0, ArchiveDecodeAlignedSize(arg1));
    temp_v0 = func_80039850(&D_80062648);
    D_80062528 = temp_v0;
    func_80039A80(temp_v0, 0x7F, 0);
}
#endif


#ifndef XENO_PC_PORT
void func_80076858(s32 arg0, void *arg1, void *arg2, void *arg3, void *arg4) {
    s32 temp_a0;
    s32 temp_t0;
    s32 temp_v0;
    s32 temp_v1;

    temp_v0 = 0x1000 - arg0;
    temp_t0 = (s32) (temp_v0 * temp_v0 * 8) >> 0xC;
    temp_a0 = ((s32) (temp_v0 * arg0) >> 8) + 0x8000;
    temp_v1 = (s32) (arg0 * arg0 * 8) >> 0xC;
    *(s32 *)((s8*)(arg4) + 0) = (s32) ((*(s16 *)((s8*)(arg1) + 0) * temp_t0) + (*(s16 *)((s8*)(arg2) + 0) * temp_a0) + (*(s16 *)((s8*)(arg3) + 0) * temp_v1));
    *(s32 *)((s8*)(arg4) + 4) = (s32) ((*(s16 *)((s8*)(arg1) + 2) * temp_t0) + (*(s16 *)((s8*)(arg2) + 2) * temp_a0) + (*(s16 *)((s8*)(arg3) + 2) * temp_v1));
    *(s32 *)((s8*)(arg4) + 8) = (s32) ((*(s16 *)((s8*)(arg1) + 4) * temp_t0) + (*(s16 *)((s8*)(arg2) + 4) * temp_a0) + (*(s16 *)((s8*)(arg3) + 4) * temp_v1));
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_unpack_scene_area_data (src/decomp/src/worldmap/worldmap_open_map.c:1737). */
#ifndef XM_S_worldmap_unpack_scene_area_data
#define XM_S_worldmap_unpack_scene_area_data
void xm_worldmap_unpack_scene_area_data(void) __asm__("func_80076954");
#endif
void func_80076954(void) {
    void *block;

    block = xm_worldmap_area_data;
    xm_worldmap_area_data = xm_text_unpack_lzss_alloc(block, 0);
    xm_heap_free(block);
    xm_worldmap_object_models = (u8 *)xm_worldmap_area_data + ((AreaHeader *)xm_worldmap_area_data)->models;
    xm_worldmap_object_meshes = (u8 *)xm_worldmap_area_data + ((AreaHeader *)xm_worldmap_area_data)->meshes;
    xm_worldmap_object_placement_list = (u8 *)xm_worldmap_area_data + ((AreaHeader *)xm_worldmap_area_data)->placements;
    xm_worldmap_billboard_lists = (BillboardList *)((u8 *)xm_worldmap_area_data + ((AreaHeader *)xm_worldmap_area_data)->billboards);
    xm_worldmap_effect_emitters = (AreaObject *)((u8 *)xm_worldmap_area_data + ((AreaHeader *)xm_worldmap_area_data)->emitters);
    xm_worldmap_texture_anim_section = (s32 *)((u8 *)xm_worldmap_area_data + ((AreaHeader *)xm_worldmap_area_data)->animations);
    xm_worldmap_texture_anim2_section = (s32 *)((u8 *)xm_worldmap_area_data + ((AreaHeader *)xm_worldmap_area_data)->animations2);
}
#endif


s32 func_80076A14(void) {
    return 1;
}


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene_frame_billboards_update (src/decomp/src/worldmap/worldmap_open_map.c:1758). */
#ifndef XM_S_worldmap_scene_frame_billboards_update
#define XM_S_worldmap_scene_frame_billboards_update
s32 xm_worldmap_scene_frame_billboards_update(void) __asm__("func_80076A1C");
#endif
s32 func_80076A1C(void) {
    if (xm_worldmap_view_kind == 0) {
        xm_worldmap_camera_build_from_angles(&xm_worldmap_view_setup);
    } else {
        xm_worldmap_camera_build_look_at(&xm_worldmap_view_setup);
    }
    xm_worldmap_effects_run_emitters();
    xm_worldmap_effects_draw_particles();
    xm_worldmap_billboards_draw();
    xm_worldmap_objects_draw();
    xm_worldmap_terrain_wrap_origin(&xm_worldmap_terrain_origin);
    if (xm_worldmap_terrain_crossed_edges != 0) {
        xm_worldmap_terrain_update_grid(&xm_worldmap_camera);
        xm_worldmap_stream_wait_for_slot();
        xm_worldmap_terrain_load_new_edges();
    }
    xm_worldmap_terrain_classify_blocks(&xm_worldmap_camera);
    xm_worldmap_terrain_draw(xm_worldmap_current_display_buffer->ot, (s32)xm_worldmap_current_display_buffer->packets, &xm_worldmap_camera);
    xm_worldmap_wave_phase_x += 0x40;
    xm_worldmap_horizon_draw();
    xm_worldmap_sky_draw();
    xm_worldmap_clouds_draw();
    return 1;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_actor_script_run (src/decomp/src/worldmap/worldmap_open_map.c:1789). */
#ifndef XM_T_ScriptOp
#define XM_T_ScriptOp
typedef s32 (*ScriptOp)(WorldmapActor *actor, s32 operand1, s32 operand2, s32 operand3);
#endif
#ifndef XM_S_worldmap_actor_script_run
#define XM_S_worldmap_actor_script_run
s32 xm_worldmap_actor_script_run(s32 index) __asm__("func_80076B34");
#endif
#ifndef XM_S_worldmap_actor_script_handlers
#define XM_S_worldmap_actor_script_handlers
extern ScriptOp xm_worldmap_actor_script_handlers[12] __asm__("D_8009A3C0");
#endif
s32 func_80076B34(s32 index) {
    s32 unused[2]; 
    WorldmapActor *actor;
    s32 step;
    s32 word;

    actor = &xm_worldmap_actor_slots[index];
    step = 0;
    do {
        actor->u.script += step;
        word = *(s32 *)actor->u.script;
        step = xm_worldmap_actor_script_handlers[word & 0xFFFF](actor, word >> 16, actor->u.script[2], actor->u.script[3]);
    } while (step != 0);
    return 1;
}
#endif
#ifndef XENO_PC_PORT
s32 func_80076BC4(void) {
    D_8009D554 = 0;
    D_8009D7CC = 0;
    return 0;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80076BDC(void *arg0, s16 arg1) {
    s16 temp_v0;
    s16 temp_v0_2;

    temp_v0_2 = *(s16 *)((s8*)(arg0) + 0x22);
    if (temp_v0_2 == 0) {
        *(s16 *)((s8*)(arg0) + 0x22) = arg1;
        return 0;
    }
    temp_v0 = temp_v0_2 - 1;
    *(s16 *)((s8*)(arg0) + 0x22) = temp_v0;
    return ((temp_v0 << 0x10) < 1) * 2;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80076C18(s32 arg0, s32 arg1, s32 arg2) {
    func_80097770(arg1, arg2);
    return 4;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80076C3C(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    D_8009C5AC = arg1 << 0xC;
    D_8009C5B0 = arg2 << 0xC;
    D_8009C5B4 = arg3 << 0xC;
    return 4;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80076C68(s32 arg0, s16 arg1, s16 arg2, s16 arg3) {
    *(s16 *)0x1F8000A0 = arg1;
    *(s16 *)0x1F8000A2 = arg2;
    *(s16 *)0x1F8000A4 = arg3;
    return 4;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80076C88(s32 arg0, s32 arg1) {
    func_80089160(arg1, 0x1F8000A0, 0);
    return 2;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80076CB4(s32 arg0, s32 arg1) {
    func_800894C8(arg1);
    return 2;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80076CD4(s32 arg0, s32 arg1) {
    func_80089514(arg1);
    return 2;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80076CF4(void) {
    func_8003A89C(D_80062528);
    return 4;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80076D1C(s32 arg0, s32 arg1) {
    func_80039E60((*(u16 *)((s8*)((*(void **)&D_8006259C)) + 0x14) << 0x10) | arg1);
    return 2;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80076D50(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    func_8003A3B8((*(u16 *)((s8*)((*(void **)&D_8006259C)) + 0x14) << 0x10) | arg1, arg2, arg3);
    return 4;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80076D8C(s32 arg0, s32 arg1, s32 arg2) {
    D_8009CCA4 = arg1;
    D_8009D3CC = arg2;
    return 4;
}
#endif



#ifndef XENO_PC_PORT
void func_80076DA4(void *arg0, void *arg1) {
    s32 temp_a0;
    s32 temp_a1;
    s32 temp_a2;
    s32 temp_a3;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 var_v0;
    s32 var_v0_2;
    s32 var_v0_3;

    temp_a3 = *(s32 *)((s8*)(arg0) + 0x38);
    temp_a2 = *(s32 *)((s8*)(arg0) + 0x50);
    temp_a1 = *(s32 *)((s8*)(arg0) + 0x58);
    if (((temp_a3 != temp_a2) | (*(s32 *)((s8*)(arg0) + 0x3C) != *(s32 *)((s8*)(arg0) + 0x54)) | (*(s32 *)((s8*)(arg0) + 0x40) != temp_a1)) != 0) {
        *(s32 *)((s8*)(arg1) + 0) = (s32) (temp_a2 - temp_a3);
        *(s32 *)((s8*)(arg1) + 4) = (s32) (*(s32 *)((s8*)(arg0) + 0x54) - *(s32 *)((s8*)(arg0) + 0x3C));
        *(s32 *)((s8*)(arg1) + 8) = (s32) (*(s32 *)((s8*)(arg0) + 0x58) - *(s32 *)((s8*)(arg0) + 0x40));
        func_80093484(arg1, temp_a1, temp_a2, temp_a3);
        *(s32 *)((s8*)(arg1) + 0x10) = (s32) ((s32) *(s32 *)((s8*)(arg1) + 0) >> 3);
        *(s32 *)((s8*)(arg1) + 0x14) = (s32) ((s32) *(s32 *)((s8*)(arg1) + 4) >> 3);
        temp_v1 = *(s32 *)((s8*)(arg1) + 0x10);
        *(s32 *)((s8*)(arg1) + 0x18) = (s32) ((s32) *(s32 *)((s8*)(arg1) + 8) >> 3);
        var_v0 = abs(temp_v1);
        if (var_v0 < 0x40) {
            *(s32 *)((s8*)(arg0) + 0x38) = (s32) *(s32 *)((s8*)(arg0) + 0x50);
        } else {
            *(s32 *)((s8*)(arg0) + 0x38) = (s32) (*(s32 *)((s8*)(arg0) + 0x38) + temp_v1);
        }
        temp_v1_2 = *(s32 *)((s8*)(arg1) + 0x14);
        var_v0_2 = abs(temp_v1_2);
        if (var_v0_2 < 0x40) {
            *(s32 *)((s8*)(arg0) + 0x3C) = (s32) *(s32 *)((s8*)(arg0) + 0x54);
        } else {
            *(s32 *)((s8*)(arg0) + 0x3C) = (s32) (*(s32 *)((s8*)(arg0) + 0x3C) + temp_v1_2);
        }
        temp_a0 = *(s32 *)((s8*)(arg1) + 0x18);
        var_v0_3 = abs(temp_a0);
        if (var_v0_3 < 0x40) {
            *(s32 *)((s8*)(arg0) + 0x40) = (s32) *(s32 *)((s8*)(arg0) + 0x58);
        } else {
            *(s32 *)((s8*)(arg0) + 0x40) = (s32) (*(s32 *)((s8*)(arg0) + 0x40) + temp_a0);
        }
    }
    D_8009BD38 = (s16) ((s32) *(s32 *)((s8*)(arg0) + 0x38) >> 0xC);
    (*(s16*)&D_8009BD3A) = (s16) ((s32) *(s32 *)((s8*)(arg0) + 0x3C) >> 0xC);
    D_8009BD3C = (s16) ((s32) *(s32 *)((s8*)(arg0) + 0x40) >> 0xC);
}
#endif



#ifndef XENO_PC_PORT
void func_80076F54(void *arg0) {
    s32 temp_a0;
    s32 temp_a1;
    s32 var_v0;

    temp_a0 = *(s32 *)((s8*)(arg0) + 0x5C);
    if (temp_a0 != D_8009D3F0) {
        temp_a1 = (s32) (temp_a0 - D_8009D3F0) >> 3;
        var_v0 = abs(temp_a1);
        if (var_v0 < 0x40) {
            D_8009D3F0 = temp_a0;
            return;
        }
        D_8009D3F0 += temp_a1;
    }
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_camera_ease_position (src/decomp/src/worldmap/worldmap_open_map.c:1958). */
#ifndef XM_S_worldmap_camera_ease_position
#define XM_S_worldmap_camera_ease_position
void xm_worldmap_camera_ease_position(WorldmapActor *actor, VECTOR *work) __asm__("func_80076FA8");
#endif
#ifndef XM_S_worldmap_wrap_offset
#define XM_S_worldmap_wrap_offset
void xm_worldmap_wrap_offset(VECTOR *offset) __asm__("func_80093484");
#endif
#pragma push_macro("GROUND_SCROLL")
#undef GROUND_SCROLL
#define GROUND_SCROLL ((s32 *)&xm_worldmap_terrain_origin)
void func_80076FA8(WorldmapActor *actor, VECTOR *work) {
    if ((actor->position.vx != xm_worldmap_camera_follow_target.target.vx) | (actor->position.vy != xm_worldmap_camera_follow_target.target.vy) |
        (actor->position.vz != xm_worldmap_camera_follow_target.target.vz)) {
        work[0].vx = xm_worldmap_camera_follow_target.target.vx - actor->position.vx;
        work[0].vy = xm_worldmap_camera_follow_target.target.vy - actor->position.vy;
        work[0].vz = xm_worldmap_camera_follow_target.target.vz - actor->position.vz;
        xm_worldmap_wrap_offset(&work[0]);
        work[1].vx = work[0].vx >> 3;
        work[1].vy = work[0].vy >> 3;
        work[1].vz = work[0].vz >> 3;
        if (abs(work[1].vx) < 0x200) {
            GROUND_SCROLL[0] += work[0].vx;
            actor->position.vx = xm_worldmap_camera_follow_target.target.vx;
        } else {
            GROUND_SCROLL[0] += work[1].vx;
            actor->position.vx += work[1].vx;
        }
        if (abs(work[1].vy) < 0x200) {
            actor->position.vy = xm_worldmap_camera_follow_target.target.vy;
        } else {
            actor->position.vy += work[1].vy;
        }
        if (abs(work[1].vz) < 0x200) {
            GROUND_SCROLL[2] += work[0].vz;
            actor->position.vz = xm_worldmap_camera_follow_target.target.vz;
        } else {
            GROUND_SCROLL[2] += work[1].vz;
            actor->position.vz += work[1].vz;
        }
    }
    xm_worldmap_camera.target = actor->position;
}
#pragma pop_macro("GROUND_SCROLL")
#endif


#ifndef XENO_PC_PORT
s32 func_800771D8(s32 arg0, s32 arg1, s32 arg2) {
    if (arg0 != arg1) {
        if (abs(arg1 - arg0) < abs(arg2)) {
            arg0 = arg1;
        } else {
            arg0 += arg2;
        }
    }
    return arg0;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80077214);


#ifndef XENO_PC_PORT
void func_80077480(void) {
    func_80084818();
    func_80086124();
    func_80086568();
    func_800866C8();
    func_80074F04();
    func_800750DC();
    func_80088FF4();
    func_80089128();
    func_80097D64();
    HeapFree(D_8009BC38);
    HeapFree(D_8009BCB0);
    HeapFree(D_8009BC3C);
    HeapFree(D_8009BCB4);
    HeapFree(D_8009C180);
    func_800976A0();
    D_8006F94E = 0x11;
    D_8006F954 = 7;
    D_8009BBC4 = 1;
    D_8006F950 = D_8009BD3A;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007756C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800776E0);


#ifndef XENO_PC_PORT
s32 func_80077954(void) {
    return 1;
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene_frame_no_effects_update (src/decomp/src/worldmap/worldmap_open_map.c:2163). */
#ifndef XM_S_worldmap_scene_frame_no_effects_update
#define XM_S_worldmap_scene_frame_no_effects_update
s32 xm_worldmap_scene_frame_no_effects_update(void) __asm__("func_8007795C");
#endif
s32 func_8007795C(void) {
    if (xm_worldmap_view_kind == 0) {
        xm_worldmap_camera_build_from_angles(&xm_worldmap_view_setup);
    } else {
        xm_worldmap_camera_build_look_at(&xm_worldmap_view_setup);
    }
    xm_worldmap_billboards_draw();
    xm_worldmap_objects_draw();
    xm_worldmap_terrain_wrap_origin(&xm_worldmap_terrain_origin);
    if (xm_worldmap_terrain_crossed_edges != 0) {
        xm_worldmap_terrain_update_grid(&xm_worldmap_camera);
        xm_worldmap_stream_wait_for_slot();
        xm_worldmap_terrain_load_new_edges();
    }
    xm_worldmap_terrain_classify_blocks(&xm_worldmap_camera);
    xm_worldmap_terrain_draw(xm_worldmap_current_display_buffer->ot, (s32)xm_worldmap_current_display_buffer->packets, &xm_worldmap_camera);
    xm_worldmap_wave_phase_x += 0x40;
    xm_worldmap_horizon_draw();
    xm_worldmap_sky_draw();
    xm_worldmap_clouds_draw();
    return 1;
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80077A64);


#ifndef XENO_PC_PORT
void func_80077CC0(void) {
    func_80039FF8();
    func_8003852C(D_8006259C);
    HeapFree(D_8006259C);
    func_80084818();
    func_80086568();
    func_800866C8();
    func_80074F04();
    func_800750DC();
    func_80088FF4();
    func_80089128();
    func_80097D64();
    HeapFree(D_8009BC38);
    HeapFree(D_8009BCB0);
    HeapFree(D_8009BC3C);
    HeapFree(D_8009BCB4);
    HeapFree(D_8009C180);
    func_800976A0();
    D_8006F94E = 0x10E;
    D_8006F954 = 0;
    D_8009BBC4 = 1;
    D_8006F950 = D_8009BD3A;
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene9_camera_flight_start (src/decomp/src/worldmap/worldmap_open_map.c:2267). */
#ifndef XM_S_worldmap_scene9_camera_flight_start
#define XM_S_worldmap_scene9_camera_flight_start
s32 xm_worldmap_scene9_camera_flight_start(s32 index) __asm__("func_80077DC8");
#endif
#ifndef XM_S_sound_play_effect
#define XM_S_sound_play_effect
void xm_sound_play_effect(s32 effect) __asm__("func_80039E60");
#endif
s32 func_80077DC8(s32 index) {
    WorldmapActor *actor;

    xm_worldmap_view_center_y = 0x78;
    xm_worldmap_camera_distance = 0x200000;
    actor = &xm_worldmap_actor_slots[index];
    actor->state = 0;
    actor->unk58 = 0;
    actor->unk54 = 0;
    actor->u.script = NULL;
    xm_worldmap_camera_angle.vz = 0;
    xm_worldmap_camera_angle.vy = 0;
    xm_worldmap_camera_angle.vx = 0;
    xm_worldmap_view_kind = 1;
    xm_sound_play_effect((xm_sound_effect_bank->id << 16) | 0xA4);
    actor->wait = 0x18;
    return 1;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene9_camera_flight_update (src/decomp/src/worldmap/worldmap_scenes_9_10.c:51). */
#ifndef XM_T_PathScratch
#define XM_T_PathScratch
typedef struct {
    VECTOR at;         
    VECTOR eye;        
    s32 distance;      
    u8 pad24[0x7C];
    SVECTOR points[3]; 
} PathScratch;
#endif
#ifndef XM_T_PATH_SCRATCH
#define XM_T_PATH_SCRATCH
#define PATH_SCRATCH ((PathScratch *)0x1F800000)
#endif
#ifndef XM_T_VIEW
#define XM_T_VIEW
#define VIEW xm_worldmap_view_setup
#endif
#ifndef XM_S_worldmap_scene9_camera_flight_update
#define XM_S_worldmap_scene9_camera_flight_update
s32 xm_worldmap_scene9_camera_flight_update(s32 index) __asm__("func_80077E68");
#endif
#ifndef XM_S_worldmap_scene9_camera_path_points
#define XM_S_worldmap_scene9_camera_path_points
extern SVECTOR xm_worldmap_scene9_camera_path_points[12] __asm__("D_8009A3F0");
#endif
#ifndef XM_S_worldmap_eval_quadratic_bspline
#define XM_S_worldmap_eval_quadratic_bspline
void xm_worldmap_eval_quadratic_bspline(s32 t, SVECTOR *p0, SVECTOR *p1, SVECTOR *p2, VECTOR *out) __asm__("func_80076858");
#endif
#ifndef XM_S_worldmap_get_matrix_angles
#define XM_S_worldmap_get_matrix_angles
void xm_worldmap_get_matrix_angles(MATRIX *m, SVECTOR *angle) __asm__("func_80097070");
#endif
#ifndef XM_S_worldmap_effects_start_emitters
#define XM_S_worldmap_effects_start_emitters
void xm_worldmap_effects_start_emitters(s32 group, SVECTOR *position, SVECTOR *angle) __asm__("func_80089160");
#endif
#ifndef XM_S_sound_slide_effect_volume
#define XM_S_sound_slide_effect_volume
void xm_sound_slide_effect_volume(s32 id, s32 volume, s32 frames) __asm__("func_8003A3B8");
#endif
#ifndef XM_S_worldmap_actor_request
#define XM_S_worldmap_actor_request
s32 xm_worldmap_actor_request(s32 index, s32 argument) __asm__("func_80097770");
#endif
#ifndef XM_S_worldmap_screen_fade_rate
#define XM_S_worldmap_screen_fade_rate
extern s32 xm_worldmap_screen_fade_rate __asm__("D_8009CCA4");
#endif
#ifndef XM_S_worldmap_screen_fade_step
#define XM_S_worldmap_screen_fade_step
extern s32 xm_worldmap_screen_fade_step __asm__("D_8009D3CC");
#endif
#ifndef XM_S_worldmap_loop_running
#define XM_S_worldmap_loop_running
extern s32 xm_worldmap_loop_running __asm__("D_8009D554");
#endif
#ifndef XM_S_worldmap_get_ground_distance
#define XM_S_worldmap_get_ground_distance
s32 xm_worldmap_get_ground_distance(VECTOR *a, VECTOR *b) __asm__("func_80094154");
#endif
#ifndef XM_S_sound_set_effect_volume
#define XM_S_sound_set_effect_volume
void xm_sound_set_effect_volume(s32 id, s32 volume) __asm__("func_8003A2E4");
#endif
s32 func_80077E68(s32 index) {
    WorldmapActor *actor;
    SVECTOR *points;
    s32 segment;
    s32 distance;
    PathScratch *scratch;

    actor = &xm_worldmap_actor_slots[index];
    scratch = PATH_SCRATCH;
    points = xm_worldmap_scene9_camera_path_points;
    segment = actor->u.step >> 12;
    points += segment;
    actor->unk54 = segment;
    if (points[2].pad != -1) {
        PATH_SCRATCH->points[0].vx = points[0].vx;
        PATH_SCRATCH->points[0].vz = points[0].vz;
        PATH_SCRATCH->points[1].vx = points[1].vx;
        PATH_SCRATCH->points[1].vz = points[1].vz;
        PATH_SCRATCH->points[2].vx = points[2].vx;
        PATH_SCRATCH->points[2].vz = points[2].vz;
        PATH_SCRATCH->points[0].vy = points[0].vy;
        PATH_SCRATCH->points[1].vy = points[1].vy;
        PATH_SCRATCH->points[2].vy = points[2].vy;
        xm_worldmap_eval_quadratic_bspline(actor->u.step & 0xFFF, &PATH_SCRATCH->points[0], &PATH_SCRATCH->points[1],
                      &PATH_SCRATCH->points[2], &scratch->at);
        VIEW.at.vz = 0;
        VIEW.at.vx = 0;
        VIEW.up.vz = 0;
        VIEW.up.vx = 0;
        VIEW.up.vy = -0x1000;
        VIEW.eye.vx = scratch->at.vx >> 16;
        VIEW.eye.vz = -(scratch->at.vz >> 16);
        VIEW.eye.vy = scratch->at.vy >> 16;
        VIEW.at.vy = xm_worldmap_camera.target.vy >> 12;
    }
    xm_worldmap_camera_build_look_at(&xm_worldmap_view_setup);
    xm_worldmap_get_matrix_angles(&xm_worldmap_camera_matrix, &xm_worldmap_camera_angle);
    switch (actor->state) {
    case 0:
        actor->unk58 += 5;
        if (--actor->wait < 0) {
            actor->unk58 = 0x80;
            actor->state++;
            scratch->points[0].vy = -0x20D;
            scratch->points[0].vx = (xm_worldmap_camera.target.vx >> 12) + 0x32;
            scratch->points[0].vz = (xm_worldmap_camera.target.vz >> 12) - 0x70;
            xm_worldmap_effects_start_emitters(8, &scratch->points[0], NULL);
        }
        break;
    case 1:
        if (actor->u.step >= 0x6000) {
            actor->state++;
        }
        break;
    case 2:
        actor->unk58 -= 2;
        if (actor->unk58 < 0x19) {
            actor->unk58 = 0x18;
            actor->state++;
        }
        break;
    case 3:
        if (actor->u.step > 0x7FFF) {
            actor->state++;
        }
        break;
    case 4:
        actor->unk58 -= 1;
        if (actor->unk58 < 5) {
            actor->unk58 = 4;
            actor->state++;
        }
        break;
    case 5:
        if (actor->u.step > 0x83FF) {
            xm_sound_slide_effect_volume((xm_sound_effect_bank->id << 16) | 0xA4, 0, 0x100);
            xm_worldmap_actor_request(0, 0xD);
            xm_worldmap_screen_fade_rate = 2;
            xm_worldmap_screen_fade_step = 4;
            actor->wait = 0x50;
            actor->unk58 = 0;
            actor->state++;
        }
        break;
    case 6:
        if (--actor->wait <= 0) {
            xm_worldmap_loop_running = 0;
            xm_worldmap_loop_result = 0;
        }
        break;
    }
    actor->u.step += actor->unk58;
    if (actor->state < 6) {
        scratch->at.vx = VIEW.at.vx << 12;
        scratch->at.vy = VIEW.at.vy << 12;
        scratch->at.vz = VIEW.at.vz << 12;
        scratch->eye.vx = VIEW.eye.vx << 12;
        scratch->eye.vy = VIEW.eye.vy << 12;
        scratch->eye.vz = VIEW.eye.vz << 12;
        distance = xm_worldmap_get_ground_distance(&scratch->at, &scratch->eye) >> 3;
        scratch->distance = distance;
        xm_sound_set_effect_volume((xm_sound_effect_bank->id << 16) | 0xA4, 0x87 - distance);
    }
    return 1;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene9_rig_start (src/decomp/src/worldmap/worldmap_scenes_9_10.c:158). */
#ifndef XM_S_worldmap_scene9_rig_start
#define XM_S_worldmap_scene9_rig_start
s32 xm_worldmap_scene9_rig_start(s32 index) __asm__("func_8007828C");
#endif
#ifndef XM_S_worldmap_objects_link
#define XM_S_worldmap_objects_link
void xm_worldmap_objects_link(s32 parent, s32 child) __asm__("func_800848B4");
#endif
s32 func_8007828C(s32 index) {
    WorldmapActor *actor;

    xm_worldmap_objects_link(0, 1);
    xm_worldmap_objects_link(0, 2);
    xm_worldmap_objects_link(0, 3);
    xm_worldmap_objects_link(0, 4);
    xm_worldmap_objects_link(0, 5);
    xm_worldmap_objects_link(0, 6);
    xm_worldmap_objects_link(0, 7);
    xm_worldmap_objects_link(0, 8);
    xm_worldmap_objects_link(0, 9);
    xm_worldmap_objects_link(0, 0xA);
    xm_worldmap_objects_link(0, 0xB);
    xm_worldmap_objects_link(0, 0xC);
    xm_worldmap_objects_link(0, 0xD);
    actor = &xm_worldmap_actor_slots[index];
    actor->state = 0;
    actor->position = xm_worldmap_player_position;
    actor->unk54 = 0x40;
    actor->unk58 = 0x200;
    actor->unk5C = 0x60;
    actor->unk60 = 0x300;
    actor->u.script = NULL;
    actor->unk64 = 0x50;
    xm_worldmap_camera.target = actor->position;
    return 1;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene9_rig_update (src/decomp/src/worldmap/worldmap_scenes_9_10.c:201). */
#ifndef XM_T_GROUND_SCROLL
#define XM_T_GROUND_SCROLL
#define GROUND_SCROLL ((s32 *)&xm_worldmap_terrain_origin)
#endif
#ifndef XM_T_RigScratch
#define XM_T_RigScratch
typedef struct {
    VECTOR position;   
    u8 pad10[0x90];
    SVECTOR angle[4];  
    u8 padC0[0x30];
    MATRIX matrix[4];  
} RigScratch;
#endif
#ifndef XM_T_RIG_SCRATCH
#define XM_T_RIG_SCRATCH
#define RIG_SCRATCH ((RigScratch *)0x1F800000)
#endif
#ifndef XM_T_SceneObject
#define XM_T_SceneObject
typedef struct SceneObject {
    s16 visible;
    s16 unk2;
    u16 flags;                  
    s16 unk6;
    VECTOR position;            
    SVECTOR angle;              
    MATRIX matrix;              
    SpriteModel *def;           
    s32 unk44;                  
    void *prims;                
    void *prims2;               
    struct SceneObject *parent; 
} SceneObject;
#endif
#ifndef XM_S_worldmap_scene9_rig_update
#define XM_S_worldmap_scene9_rig_update
s32 xm_worldmap_scene9_rig_update(s32 index) __asm__("func_800783E8");
#endif
#ifndef XM_S_worldmap_wrap_position
#define XM_S_worldmap_wrap_position
void xm_worldmap_wrap_position(VECTOR *position) __asm__("func_80093354");
#endif
#ifndef XM_S_worldmap_objects
#define XM_S_worldmap_objects
extern SceneObject *xm_worldmap_objects __asm__("D_8009C620");
#endif
#ifndef XM_S_RotMatrixYXZ
#define XM_S_RotMatrixYXZ
MATRIX *xm_RotMatrixYXZ(SVECTOR *r, MATRIX *m) __asm__("RotMatrixYXZ");
#endif
s32 func_800783E8(s32 index) {
    WorldmapActor *actor;

    actor = &xm_worldmap_actor_slots[index];
    actor->position.vz -= 0x4000;
    GROUND_SCROLL[2] -= 0x4000;
    xm_worldmap_wrap_position(&actor->position);
    RIG_SCRATCH->position.vx = actor->position.vx >> 12;
    RIG_SCRATCH->position.vy = actor->position.vy >> 12;
    RIG_SCRATCH->position.vz = actor->position.vz >> 12;
    xm_worldmap_objects[0].position = RIG_SCRATCH->position;
    xm_worldmap_camera.target = actor->position;
    actor->u.step = (actor->u.step + actor->unk54) & 0xFFF;
    actor->unk58 = (actor->unk58 + actor->unk5C) & 0xFFF;
    actor->unk60 = (actor->unk60 + actor->unk64) & 0xFFF;
    RIG_SCRATCH->angle[0].vx = RIG_SCRATCH->angle[0].vz = RIG_SCRATCH->angle[1].vx =
        RIG_SCRATCH->angle[1].vz = RIG_SCRATCH->angle[2].vx = RIG_SCRATCH->angle[2].vz = 0;
    RIG_SCRATCH->angle[0].vy = actor->u.step;
    RIG_SCRATCH->angle[1].vy = actor->unk58;
    RIG_SCRATCH->angle[2].vy = actor->unk60;
    RIG_SCRATCH->angle[3].vx = RIG_SCRATCH->angle[3].vy = 0;
    RIG_SCRATCH->angle[3].vz = actor->unk60;
    xm_RotMatrixYXZ(&RIG_SCRATCH->angle[0], &RIG_SCRATCH->matrix[0]);
    xm_RotMatrixYXZ(&RIG_SCRATCH->angle[1], &RIG_SCRATCH->matrix[1]);
    xm_RotMatrixYXZ(&RIG_SCRATCH->angle[2], &RIG_SCRATCH->matrix[2]);
    xm_RotMatrixYXZ(&RIG_SCRATCH->angle[3], &RIG_SCRATCH->matrix[3]);
    xm_worldmap_objects[1].matrix = xm_worldmap_objects[2].matrix = xm_worldmap_objects[3].matrix = RIG_SCRATCH->matrix[3];
    xm_worldmap_objects[4].matrix = xm_worldmap_objects[5].matrix = RIG_SCRATCH->matrix[2];
    xm_worldmap_objects[6].matrix = xm_worldmap_objects[10].matrix = xm_worldmap_objects[8].matrix = xm_worldmap_objects[12].matrix =
        RIG_SCRATCH->matrix[0];
    xm_worldmap_objects[7].matrix = xm_worldmap_objects[11].matrix = xm_worldmap_objects[9].matrix = xm_worldmap_objects[13].matrix =
        RIG_SCRATCH->matrix[1];
    return 1;
}
#endif


s32 func_80078948(void) {
    return 1;
}


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene_frame_update (src/decomp/src/worldmap/worldmap_scenes_9_10.c:242). */
#ifndef XM_S_worldmap_scene_frame_update
#define XM_S_worldmap_scene_frame_update
s32 xm_worldmap_scene_frame_update(void) __asm__("func_80078950");
#endif
s32 func_80078950(void) {
    if (xm_worldmap_view_kind == 0) {
        xm_worldmap_camera_build_from_angles(&xm_worldmap_view_setup);
    } else {
        xm_worldmap_camera_build_look_at(&xm_worldmap_view_setup);
    }
    xm_worldmap_effects_run_emitters();
    xm_worldmap_effects_draw_particles();
    xm_worldmap_objects_draw();
    xm_worldmap_terrain_wrap_origin(&xm_worldmap_terrain_origin);
    if (xm_worldmap_terrain_crossed_edges != 0) {
        xm_worldmap_terrain_update_grid(&xm_worldmap_camera);
        xm_worldmap_stream_wait_for_slot();
        xm_worldmap_terrain_load_new_edges();
    }
    xm_worldmap_terrain_classify_blocks(&xm_worldmap_camera);
    xm_worldmap_terrain_draw(xm_worldmap_current_display_buffer->ot, (s32)xm_worldmap_current_display_buffer->packets, &xm_worldmap_camera);
    xm_worldmap_wave_phase_x += 0x40;
    xm_worldmap_horizon_draw();
    xm_worldmap_sky_draw();
    xm_worldmap_clouds_draw();
    return 1;
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80078A60);


#ifndef XENO_PC_PORT
void func_80078D24(void) {
    func_80039FF8();
    func_8003852C(D_8006259C);
    HeapFree(D_8006259C);
    func_80084818();
    func_80086568();
    func_800866C8();
    func_80074F04();
    func_800750DC();
    func_80088FF4();
    func_80089128();
    func_80097D64();
    HeapFree(D_8009BC38);
    HeapFree(D_8009BCB0);
    HeapFree(D_8009BC3C);
    HeapFree(D_8009BCB4);
    HeapFree(D_8009C180);
    func_800976A0();
    D_8006F94E = 0x110;
    D_8006F954 = 0;
    D_8009BBC4 = 1;
    D_8006F950 = D_8009BD3A;
}
#endif



s32 func_80078E2C(s32 arg0) {
    void *temp_v1;

    D_8009BE0C = 0x78;
    D_8009D3F0 = 0x200000;
    D_8009D144 = 0;
    temp_v1 = (u8 *)D_8009BE24 + (arg0 << 7);
    *(s16 *)((s8*)(temp_v1) + 0x20) = 0x10;
    *(s32 *)((s8*)(temp_v1) + 0x58) = 0;
    *(s32 *)((s8*)(temp_v1) + 0x54) = 0;
    *(s32 *)((s8*)(temp_v1) + 0x50) = 0;
    D_8009BD38 = -0xC0;
    (*(s16*)&D_8009BD3A) = 0x680;
    D_8009BD3C = 0;
    *(s16 *)((s8*)(temp_v1) + 0x22) = 0x40;
    *(s32 *)((s8*)(temp_v1) + 0x74) = 0;
    return 1;
}


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene10_script_update (src/decomp/src/worldmap/worldmap_scenes_9_10.c:373). */
#ifndef XM_T_xm_CameraScratch
#define XM_T_xm_CameraScratch
typedef struct {
    VECTOR delta;     
    u8 pad10[0x90];
    SVECTOR view;     
} xm_CameraScratch;
#endif
#ifndef XM_T_xm_SpritePart
#define XM_T_xm_SpritePart
typedef struct {
    s16 x, y;              
    u8 u, v;               
    u8 w, h;               
    u8 byte8, byte9;       
    u16 tpage;             
    u16 clut;              
    u8 unknowne[2];
    u32 colour;            
    u32 flags;             
} xm_SpritePart;
#endif
#ifndef XM_T_xm_SpriteRendererEntry
#define XM_T_xm_SpriteRendererEntry
typedef struct {
    s8 byte0;              
    s8 byte1;              
    s16 half2;
    s16 half4;
    s16 half6;
} xm_SpriteRendererEntry;
#endif
#ifndef XM_T_xm_SpriteOffset
#define XM_T_xm_SpriteOffset
typedef struct {
    s8 x, y;
} xm_SpriteOffset;
#endif
#ifndef XM_T_xm_SpriteRenderer
#define XM_T_xm_SpriteRenderer
typedef struct {
    s16 angle_x, angle_y, angle_z; 
    s16 scale_x, scale_y, scale_z; 
    MATRIX matrix;                 
    xm_SpritePart *parts[2];          
    xm_SpriteRendererEntry *pointer34; 
    struct xm_Sprite *next_pending;   
    xm_SpriteOffset offset;           
    u8 unknown3e[2];
    s32 word40;                    
    SVECTOR light_angles;          
    u16 light_colour[3];           
} xm_SpriteRenderer;
#endif
#ifndef XM_T_xm_Sprite
#define XM_T_xm_Sprite
typedef struct xm_Sprite {
    s32 x, y, z;                
    s32 speed_x, speed_y, speed_z; 
    s32 speed;                  
    s32 gravity;                
    xm_SpriteRenderer *renderer;   
    void *image;                
    u8 red, green, blue;     
    u8 colour_flags;         
    s16 scale;               
    s16 depth;               
    s16 half30;              
    s16 direction;           
    u16 frame;               
    u16 height;              
    u16 extent_depth;        
    u16 rate;                
    union {
        u32 word;
        struct {
        unsigned sides : 2;      
        unsigned unknown2 : 1;
        unsigned flip : 1;       
        unsigned flip_y : 1;
        unsigned blend : 3;      
        unsigned unknown8 : 8;
        unsigned field16 : 4;
        unsigned mode : 4;       
        unsigned no_view : 1;    
        unsigned unknown25 : 3;
        unsigned dirty : 1;      
        unsigned unknown29 : 3;
        } bits;
        u8 bytes[4];         
    } render;                
    u32 flags;               
    s32 *resource_block;     
    s32 *animations;         
    s32 resource;            
    s32 word50;              
    u16 *frame_table;        
    u16 *animation;          
    u16 *facings;            
    u16 *word60;             
    u8 *script;              
    void (*callback)(struct xm_Sprite *sprite); 
    void *block;             
    struct xm_Sprite *parent;   
    struct xm_Sprite *partner;  
    s32 word78;              
    void *sequencer;         
    u16 word80;              
    u16 word82;              
    s16 ground;              
    u16 size;                
    u8 *frames;              
    s8 stack_top;            
    u8 unknown8d;
    u8 stack[0x10];          
    s16 countdown;           
    s16 target_x, target_y, target_z; 
    u8 unknowna6[2];
    struct {
        unsigned sequencer_owned : 1; 
        unsigned bounce : 10;    
        unsigned frame : 6;      
        unsigned step : 3;       
        unsigned phase : 2;      
        unsigned field22 : 6;    
        unsigned field28 : 2;
        unsigned unknown30 : 2;
    } frame_bits;            
    union {
        u32 word;
        struct {
        unsigned unknown0 : 2;
        unsigned mirror : 1;     
        unsigned frame_flip : 1; 
        unsigned unknown4 : 1;
        unsigned owns_children : 1; 
        unsigned double_step : 1;
        unsigned divisor : 12;   
        unsigned unknown19 : 13;
        } bits;
        u8 bytes[4];
    } motion;                
    union {
        u32 wordb0;          
        u8 byteb0;
        struct {
            unsigned unknown0 : 8;
            unsigned passive_children : 1; 
            unsigned share_rate : 1;       
            unsigned unknown10 : 22;
        } bits;
    } b0;                    
} xm_Sprite;
#endif
#ifndef XM_T_xm_WorldmapActor
#define XM_T_xm_WorldmapActor
typedef struct xm_WorldmapActor {
    s16 command;     
    s16 command_arg;
    s16 unk4;
    s16 unk6;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    s32 unk14;
    s32 kind;        
    s32 update;      
    s16 state;       
    s16 wait;        
    s16 unk24;
    s16 unk26;
    VECTOR position; 
    VECTOR motion;   
    s16 heading;     
    s16 turn;        
    xm_Sprite *handle;  
    union {
        s16 *script; 
        u16 value;   
        s32 step;
    } u;             
    s32 unk54;
    s32 unk58;
    s32 unk5C;
    s32 unk60;
    s32 unk64;
    s32 unk68;
    s32 unk6C;
    s32 unk70;
    s32 unk74;
    s32 unk78;
    s32 unk7C;
} xm_WorldmapActor;
#endif
#ifndef XM_T_xm_SoundBank
#define XM_T_xm_SoundBank
typedef struct xm_SoundBank {
    u8 unk0[0x10];
    u16 flags;
    u8 unk12[2];
    u16 id;
    u16 unk16;         
    u16 volumes;       
    u8 unk1A[2];
    struct xm_SoundBank *next;
    u16 effect[1];     
} xm_SoundBank;
#endif
#ifndef XM_T_xm_ViewSetup
#define XM_T_xm_ViewSetup
typedef struct xm_ViewSetup {
    SVECTOR eye;
    SVECTOR at;
    VECTOR up;
} xm_ViewSetup;
#endif
#ifndef XM_T_xm_Camera
#define XM_T_xm_Camera
typedef struct xm_Camera {
    VECTOR target;
} xm_Camera;
#endif
#ifndef XM_T_VIEW_VECTORS
#define XM_T_VIEW_VECTORS
#define VIEW_VECTORS ((SVECTOR *)&xm_worldmap_view_setup)
#endif
#ifndef XM_S_worldmap_scene10_script_update
#define XM_S_worldmap_scene10_script_update
s32 xm_worldmap_scene10_script_update(s32 index) __asm__("func_80078EA4");
#endif
#ifndef XM_S_worldmap_camera_place_orbit
#define XM_S_worldmap_camera_place_orbit
void xm_worldmap_camera_place_orbit(xm_ViewSetup *view, xm_Camera *camera, s32 distance, SVECTOR *angle) __asm__("func_80096F18");
#endif
s32 func_80078EA4(s32 index) {
    xm_CameraScratch *scratch;
    xm_WorldmapActor *actor;
    s16 trigger;

    actor = &xm_worldmap_actor_slots[index];
    scratch = (xm_CameraScratch *)0x1F800000;
    switch (actor->state) {
    case 0:
        if (--actor->wait <= 0) {
            actor->wait = 0x20;
            actor->state++;
            xm_worldmap_actor_request(5, 1);
            xm_sound_play_effect((xm_sound_effect_bank->id << 16) | 0x62);
            xm_sound_play_effect((xm_sound_effect_bank->id << 16) | 0x63);
        }
        break;
    case 1:
        if (--actor->wait <= 0) {
            actor->wait = 0x20;
            actor->state++;
            xm_worldmap_actor_request(3, 1);
        }
        break;
    case 2:
        if (--actor->wait <= 0) {
            actor->wait = 0x30;
            actor->state++;
            xm_worldmap_actor_request(4, 1);
            xm_worldmap_actor_request(2, 1);
            xm_worldmap_actor_request(6, 1);
        }
        break;
    case 3:
        if (--actor->wait <= 0) {
            actor->wait = 0x28;
            actor->state++;
        }
        break;
    case 4:
        actor->wait--;
        xm_worldmap_camera_place_orbit(&xm_worldmap_view_setup, &xm_worldmap_camera, xm_worldmap_camera_distance, &xm_worldmap_camera_angle);
        scratch->view.vx = xm_rand() % 12 - 6;
        scratch->view.vy = xm_rand() % 12 - 6;
        VIEW_VECTORS[0].vx += scratch->view.vx;
        VIEW_VECTORS[1].vx += scratch->view.vx;
        VIEW_VECTORS[0].vy += scratch->view.vy;
        VIEW_VECTORS[1].vy += scratch->view.vy;
        if (actor->wait <= 0) {
            actor->wait = 0x78;
            actor->state++;
            xm_sound_play_effect((xm_sound_effect_bank->id << 16) | 0x79);
            xm_sound_slide_effect_volume((xm_sound_effect_bank->id << 16) | 0x62, 0, 0x100);
            xm_sound_slide_effect_volume((xm_sound_effect_bank->id << 16) | 0x63, 0, 0x100);
        }
        break;
    case 5:
        actor->wait--;
        xm_worldmap_camera_place_orbit(&xm_worldmap_view_setup, &xm_worldmap_camera, xm_worldmap_camera_distance, &xm_worldmap_camera_angle);
        scratch->view.vx = xm_rand() % 4 - 2;
        scratch->view.vy = xm_rand() % 4 - 2;
        VIEW_VECTORS[0].vx += scratch->view.vx;
        VIEW_VECTORS[0].vy += scratch->view.vy;
        VIEW_VECTORS[1].vx += scratch->view.vx;
        VIEW_VECTORS[1].vy += scratch->view.vy;
        if (actor->wait <= 0) {
            actor->wait = 0x5A;
            actor->state++;
        }
        break;
    case 6:
        if (--actor->wait <= 0) {
            actor->wait = 0x50;
            actor->state++;
            xm_worldmap_actor_request(0, 0xD);
            xm_sound_slide_effect_volume((xm_sound_effect_bank->id << 16) | 0x79, 0, 0x100);
            xm_worldmap_screen_fade_rate = 2;
            xm_worldmap_screen_fade_step = 4;
        }
        break;
    case 7:
        if (--actor->wait <= 0) {
            actor->wait = 0;
            xm_worldmap_loop_running = 0;
            xm_worldmap_loop_result = 0;
        }
        break;
    case 16:
        trigger = actor->unk4;
        if (trigger == 1) {
            xm_sound_slide_effect_volume((xm_sound_effect_bank->id << 16) | 0x36, 0, 8);
            xm_worldmap_actor_request(0, 0xD);
            xm_worldmap_screen_fade_step = 0x20;
            xm_worldmap_screen_fade_rate = trigger;
            actor->wait = 8;
            actor->unk4 = 0;
            actor->state++;
        }
        break;
    case 17:
        if (--actor->wait <= 0) {
            xm_worldmap_camera_distance = 0x960000;
            xm_worldmap_camera_angle.vx = -0x30;
            xm_worldmap_camera_angle.vy = 0x40;
            xm_worldmap_camera_angle.vz = 0;
            actor->wait = 0x10;
            actor->state++;
        }
        break;
    case 18:
        if (--actor->wait <= 0) {
            xm_worldmap_actor_request(0, 0xC);
            xm_worldmap_screen_fade_rate = 1;
            xm_worldmap_screen_fade_step = 0x80;
            actor->state = 0;
            actor->wait = 1;
        }
        break;
    }
    if (actor->state != 4 && actor->state != 5) {
        xm_worldmap_camera_place_orbit(&xm_worldmap_view_setup, &xm_worldmap_camera, xm_worldmap_camera_distance, &xm_worldmap_camera_angle);
    }
    return 1;
}
#endif

s32 func_800794D8(s32 arg0) {
    WmObj *obj;
    u8 *scene;

    obj = &D_8009BE24[arg0];
    scene = D_8009C620;
    obj->pos[0] = 0x02000000;
    obj->pos[2] = 0x01500000;
    obj->unk20 = 0;
    *(s16 *)(scene + 0x558) = 0;
    *(s16 *)(scene + 0x55A) = 0x780;
    *(s16 *)(scene + 0x55C) = 0;
    RotMatrixYXZ(scene + 0x558, scene + 0x560);
    return 1;
}

s32 func_80079538(s32 arg0) {
    WmObj *obj;
    u8 *scene;

    obj = &D_8009BE24[arg0];
    if (obj->unk4 != 0) {
        scene = D_8009C620;
        obj->unk4 = 0;
        *(s16 *)(scene + 0x540) = 1;
    }
    func_80093354(obj->pos);
    obj->pos[1] = func_80093A5C(obj->pos[0], obj->pos[2]) + 0x18000;
    *(s32 *)(D_8009C620 + 0x548) = obj->pos[0] >> 0xC;
    *(s32 *)(D_8009C620 + 0x54C) = obj->pos[1] >> 0xC;
    *(s32 *)(D_8009C620 + 0x550) = obj->pos[2] >> 0xC;
    return 1;
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene10_landing_rig_start (src/decomp/src/worldmap/worldmap_scenes_9_10.c:532). */
#ifndef XM_S_worldmap_scene10_landing_rig_start
#define XM_S_worldmap_scene10_landing_rig_start
s32 xm_worldmap_scene10_landing_rig_start(s32 index) __asm__("func_800795E4");
#endif
s32 func_800795E4(s32 index) {
    WorldmapActor *actor;

    xm_worldmap_objects_link(0, 1);
    xm_worldmap_objects_link(0, 2);
    xm_worldmap_objects_link(0, 3);
    xm_worldmap_objects_link(0, 4);
    xm_worldmap_objects_link(0, 5);
    xm_worldmap_objects_link(0, 6);
    xm_worldmap_objects_link(0, 7);
    xm_worldmap_objects_link(0, 8);
    xm_worldmap_objects_link(0, 9);
    xm_worldmap_objects_link(0, 0xA);
    xm_worldmap_objects_link(0, 0xB);
    xm_worldmap_objects_link(0, 0xC);
    xm_worldmap_objects_link(0, 0xD);
    actor = &xm_worldmap_actor_slots[index];
    actor->state = 0;
    actor->position = xm_worldmap_player_position;
    actor->unk54 = 0x40;
    actor->unk58 = 0x200;
    actor->unk5C = 0x60;
    actor->unk60 = 0x300;
    actor->u.script = NULL;
    actor->unk64 = 0x50;
    xm_worldmap_camera.target = actor->position;
    xm_worldmap_objects[0].angle.vx = -0x100;
    xm_worldmap_objects[0].angle.vy = 0;
    xm_worldmap_objects[0].angle.vz = -0x40;
    xm_sound_play_effect((xm_sound_effect_bank->id << 16) | 0x36);
    return 1;
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80079778);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007A06C);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene10_grow_sprites_14_15_start (src/decomp/src/worldmap/worldmap_scenes_9_10.c:702). */
#ifndef XM_S_worldmap_scene10_grow_sprites_14_15_start
#define XM_S_worldmap_scene10_grow_sprites_14_15_start
s32 xm_worldmap_scene10_grow_sprites_14_15_start(s32 index) __asm__("func_8007A144");
#endif
#ifndef XM_S_worldmap_build_translucent_quads
#define XM_S_worldmap_build_translucent_quads
void xm_worldmap_build_translucent_quads(SceneObject *object, POLY_FT4 *quads, s32 count) __asm__("func_8007A06C");
#endif
s32 func_8007A144(s32 index) {
    WorldmapActor *actor;

    actor = &xm_worldmap_actor_slots[index];
    actor->state = 0;
    actor->unk70 = 0;
    actor->unk6C = 0;
    xm_worldmap_build_translucent_quads(&xm_worldmap_objects[14], xm_worldmap_objects[14].prims, xm_worldmap_objects[14].def->primitive_count);
    xm_worldmap_build_translucent_quads(&xm_worldmap_objects[15], xm_worldmap_objects[15].prims, xm_worldmap_objects[15].def->primitive_count);
    return 3;
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene10_grow_sprites_14_15_update (src/decomp/src/worldmap/worldmap_scenes_9_10.c:716). */
#ifndef XM_T_ScaleScratch
#define XM_T_ScaleScratch
typedef struct {
    VECTOR scale[2];
    u8 pad20[0x80];
    SVECTOR angle;    
    u8 padA8[0x48];
    MATRIX matrix[2]; 
} ScaleScratch;
#endif
#ifndef XM_T_SCALE_SCRATCH
#define XM_T_SCALE_SCRATCH
#define SCALE_SCRATCH ((ScaleScratch *)0x1F800000)
#endif
#ifndef XM_S_worldmap_scene10_grow_sprites_14_15_update
#define XM_S_worldmap_scene10_grow_sprites_14_15_update
s32 xm_worldmap_scene10_grow_sprites_14_15_update(s32 index) __asm__("func_8007A1B4");
#endif
#ifndef XM_S_ScaleMatrix
#define XM_S_ScaleMatrix
MATRIX *xm_ScaleMatrix(MATRIX *m, VECTOR *v) __asm__("ScaleMatrix");
#endif
s32 func_8007A1B4(s32 index) {
    SceneObject *objects;
    WorldmapActor *actor;
    s32 result;

    objects = xm_worldmap_objects;
    actor = &xm_worldmap_actor_slots[index];
    objects[14].position = objects[0].position;
    objects[15].position = objects[14].position;
    actor->unk6C += 0xC0;
    result = 1;
    if (actor->unk6C >= 0x800) {
        actor->unk70 = (actor->unk70 + 0x100) & 0x7FFF;
    }
    if (actor->unk70 >= 0x6000) {
        result = 3;
        actor->unk4 = 0;
        actor->unk70 = 0;
        actor->unk6C = 0;
    }
    SCALE_SCRATCH->matrix[0] = xm_worldmap_identity_matrix;
    SCALE_SCRATCH->matrix[1] = SCALE_SCRATCH->matrix[0];
    SCALE_SCRATCH->scale[0].vx = SCALE_SCRATCH->scale[0].vz = actor->unk6C;
    SCALE_SCRATCH->scale[1].vx = SCALE_SCRATCH->scale[1].vz = actor->unk70;
    SCALE_SCRATCH->scale[0].vy = SCALE_SCRATCH->scale[1].vy = 0x1000;
    xm_ScaleMatrix(&SCALE_SCRATCH->matrix[0], &SCALE_SCRATCH->scale[0]);
    xm_ScaleMatrix(&SCALE_SCRATCH->matrix[1], &SCALE_SCRATCH->scale[1]);
    objects[14].matrix = SCALE_SCRATCH->matrix[0];
    objects[15].matrix = SCALE_SCRATCH->matrix[1];
    return result;
}
#endif


s32 func_8007A410(s32 arg0) {
    *(s16 *)((s8*)(((u8 *)D_8009BE24 + (arg0 << 7))) + 0x22) = 0x60;
    return 3;
}


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene10_group9_trail_update (src/decomp/src/worldmap/worldmap_scenes_9_10.c:758). */
#ifndef XM_S_worldmap_scene10_group9_trail_update
#define XM_S_worldmap_scene10_group9_trail_update
s32 xm_worldmap_scene10_group9_trail_update(s32 index) __asm__("func_8007A430");
#endif
#ifndef XM_S_worldmap_effects_stop_emitters
#define XM_S_worldmap_effects_stop_emitters
void xm_worldmap_effects_stop_emitters(s32 group) __asm__("func_800894C8");
#endif
#pragma push_macro("SCRIPT_VECTOR")
#undef SCRIPT_VECTOR
#define SCRIPT_VECTOR ((SVECTOR *)0x1F8000A0)
s32 func_8007A430(s32 index) {
    WorldmapActor *actor;
    s32 result;

    actor = &xm_worldmap_actor_slots[index];
    result = 1;
    if (actor->unk4 == result) {
        actor->unk4 = 0;
        actor->position.vx = xm_worldmap_objects[0].position.vx << 12;
        actor->position.vy = xm_worldmap_objects[0].position.vy << 12;
        actor->position.vz = xm_worldmap_objects[0].position.vz << 12;
    }
    if (--actor->wait > 0) {
        actor->position.vz += 0x20000;
        SCRIPT_VECTOR->vx = actor->position.vx >> 12;
        SCRIPT_VECTOR->vy = actor->position.vy >> 12;
        SCRIPT_VECTOR->vz = actor->position.vz >> 12;
        xm_worldmap_effects_start_emitters(9, SCRIPT_VECTOR, NULL);
    } else {
        actor->wait = 0x60;
        actor->position = xm_worldmap_player_position;
        xm_worldmap_effects_stop_emitters(9);
        result = 3;
    }
    return result;
}
#pragma pop_macro("SCRIPT_VECTOR")
#endif


s32 func_8007A568(void) {
    return 3;
}


s32 func_8007A570(s32 arg0) {
    WmObj* obj;

    obj = &D_8009BE24[arg0];
    obj->unk4 = 0;
    *(s16 *)WM_SPAD(0x1F8000A0) = *(s32 *)(D_8009C620 + 8);
    *(s16 *)WM_SPAD(0x1F8000A2) = *(s32 *)(D_8009C620 + 0xC);
    *(s16 *)WM_SPAD(0x1F8000A4) = *(s32 *)(D_8009C620 + 0x10);
    func_80089160(0xA, WM_SPAD(0x1F8000A0), 0);
    return 3;
}

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007A5DC);


#ifndef XENO_PC_PORT
void func_8007A8AC(void) {
    func_80039FF8();
    func_8003852C(D_8006259C);
    HeapFree(D_8006259C);
    func_80084818();
    func_80086568();
    func_800866C8();
    func_80074F04();
    func_800750DC();
    func_80088FF4();
    func_80089128();
    func_80097D64();
    HeapFree(D_8009BC38);
    HeapFree(D_8009BCB0);
    HeapFree(D_8009BC3C);
    HeapFree(D_8009BCB4);
    HeapFree(D_8009C180);
    func_800976A0();
    D_8006F94E = 0x11A;
    D_8006F954 = 0;
    D_8009BBC4 = 1;
    D_8006F950 = D_8009BD3A;
}
#endif



#ifndef XENO_PC_PORT
s32 func_8007A9B4(s32 arg0) {
    void *temp_v1;

    temp_v1 = (u8 *)D_8009BE24 + (arg0 << 7);
    *(s32 *)((s8*)(temp_v1) + 0x50) = 0;
    *(u16 *)((s8*)(temp_v1) + 0x20) = (u16) D_8009A450;
    *(u16 *)((s8*)(temp_v1) + 0x22) = D_8009A46C[*(s32 *)((s8*)(temp_v1) + 0x50)];
    return 1;
}
#else
s32 func_8007A9B4(s32 arg0) {
    u8 *actor = (u8 *)D_8009BE24 + (arg0 << 7);

    *(s32 *)(actor + 0x50) = 0;
    *(u16 *)(actor + 0x20) = D_8009A450;
    *(u16 *)(actor + 0x22) = D_8009A46C[0];
    return 1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007A9F8);


#ifndef XENO_PC_PORT
s32 func_8007AD34(s32 arg0) {
    D_8009BE0C = 0x78;
    D_8009D3F0 = 0x1C0000;
    D_8009BD38 = 0x40;
    (*(s16*)&D_8009BD3A) = 0x480;
    D_8009BD3C = 0;
    D_8009BE28 = D_8009C5AC;
    D_8009D55C = D_8009C5AC;
    D_8009BE2C = D_8009C5B0;
    D_8009D560 = D_8009C5B0;
    D_8009BE30 = D_8009C5B4;
    D_8009D564 = D_8009C5B4;
    *(s32 *)((s8*)(((u8 *)D_8009BE24 + (arg0 << 7))) + 0x50) = 0x1000;
    return 1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007ADD4);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene14_grow_objects_4_5_start (src/decomp/src/worldmap/worldmap_scene14.c:272). */
#ifndef XM_S_worldmap_scene14_grow_objects_4_5_start
#define XM_S_worldmap_scene14_grow_objects_4_5_start
s32 xm_worldmap_scene14_grow_objects_4_5_start(s32 index) __asm__("func_8007B200");
#endif
s32 func_8007B200(s32 index) {
    WorldmapActor *actor;
    SceneObject *object;
    ScaleScratch *scratch;
    s32 i;

    scratch = SCALE_SCRATCH;
    i = 0;
    actor = &xm_worldmap_actor_slots[index];
    object = &xm_worldmap_objects[4];
    do {
        xm_worldmap_build_translucent_quads(object, object->prims, object->def->primitive_count);
        object++;
        i++;
    } while (i < 2);
    actor->unk54 = 0;
    actor->u.step = 0;
    scratch->matrix[0] = xm_worldmap_identity_matrix;
    scratch->scale[0].vx = scratch->scale[0].vz = actor->u.step;
    scratch->scale[0].vy = 0x1000;
    xm_ScaleMatrix(&scratch->matrix[0], &scratch->scale[0]);
    object[0].matrix = scratch->matrix[0];
    object[1].matrix = scratch->matrix[0];
    object[1].visible = 1;
    object[0].visible = 1;
    return 3;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene14_grow_objects_4_5_update (src/decomp/src/worldmap/worldmap_scene14.c:301). */
#ifndef XM_S_worldmap_scene14_grow_objects_4_5_update
#define XM_S_worldmap_scene14_grow_objects_4_5_update
s32 xm_worldmap_scene14_grow_objects_4_5_update(s32 index) __asm__("func_8007B394");
#endif
s32 func_8007B394(s32 index) {
    WorldmapActor *actor;
    SceneObject *objects;
    s32 x;

    actor = &xm_worldmap_actor_slots[index];
    objects = xm_worldmap_objects;
    if (actor->unk4 != 0) {
        actor->unk4 = 0;
        objects[5].visible = 0;
        objects[4].visible = 0;
    }
    x = xm_worldmap_player_position.vx >> 12;
    objects[4].position.vy = objects[5].position.vy = -0x40;
    objects[4].position.vx = objects[5].position.vx = x;
    objects[4].position.vz = objects[5].position.vz = xm_worldmap_player_position.vz >> 12;
    if ((actor->u.step += 0x180) > 0x800) {
        actor->unk54 += 0x180;
    }
    if (actor->u.step > 0x7F00) {
        actor->u.step = 0x7F00;
    }
    if (actor->unk54 > 0x7F00) {
        actor->unk54 = 0x7F00;
    }
    SCALE_SCRATCH->matrix[0] = xm_worldmap_identity_matrix;
    SCALE_SCRATCH->matrix[1] = SCALE_SCRATCH->matrix[0];
    SCALE_SCRATCH->scale[0].vx = SCALE_SCRATCH->scale[0].vz = actor->u.step;
    SCALE_SCRATCH->scale[1].vx = SCALE_SCRATCH->scale[1].vz = actor->unk54;
    SCALE_SCRATCH->scale[0].vy = SCALE_SCRATCH->scale[1].vy = 0x1000;
    xm_ScaleMatrix(&SCALE_SCRATCH->matrix[0], &SCALE_SCRATCH->scale[0]);
    xm_ScaleMatrix(&SCALE_SCRATCH->matrix[1], &SCALE_SCRATCH->scale[1]);
    objects[4].matrix = SCALE_SCRATCH->matrix[0];
    objects[5].matrix = SCALE_SCRATCH->matrix[1];
    return 1;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene14_grow_objects_6_7_start (src/decomp/src/worldmap/worldmap_scene14.c:339). */
#ifndef XM_S_worldmap_scene14_grow_objects_6_7_start
#define XM_S_worldmap_scene14_grow_objects_6_7_start
s32 xm_worldmap_scene14_grow_objects_6_7_start(s32 index) __asm__("func_8007B604");
#endif
s32 func_8007B604(s32 index) {
    WorldmapActor *actor;
    SceneObject *object;
    ScaleScratch *scratch;
    s32 i;

    scratch = SCALE_SCRATCH;
    i = 0;
    actor = &xm_worldmap_actor_slots[index];
    object = &xm_worldmap_objects[6];
    do {
        xm_worldmap_build_translucent_quads(object, object->prims, object->def->primitive_count);
        object++;
        i++;
    } while (i < 2);
    actor->unk54 = 0;
    actor->u.step = 0;
    scratch->matrix[0] = xm_worldmap_identity_matrix;
    scratch->scale[0].vx = scratch->scale[0].vz = actor->u.step;
    scratch->scale[0].vy = 0x1000;
    xm_ScaleMatrix(&scratch->matrix[0], &scratch->scale[0]);
    object[0].matrix = scratch->matrix[0];
    object[1].matrix = scratch->matrix[0];
    object[1].visible = 1;
    object[0].visible = 1;
    return 3;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene14_grow_objects_6_7_update (src/decomp/src/worldmap/worldmap_scene14.c:369). */
#ifndef XM_S_worldmap_scene14_grow_objects_6_7_update
#define XM_S_worldmap_scene14_grow_objects_6_7_update
s32 xm_worldmap_scene14_grow_objects_6_7_update(s32 index) __asm__("func_8007B798");
#endif
s32 func_8007B798(s32 index) {
    WorldmapActor *actor;
    SceneObject *objects;
    s32 x;

    actor = &xm_worldmap_actor_slots[index];
    objects = xm_worldmap_objects;
    if (actor->unk4 != 0) {
        actor->unk4 = 0;
        objects[7].visible = 0;
        objects[6].visible = 0;
    }
    x = xm_worldmap_player_position.vx >> 12;
    objects[6].position.vy = objects[7].position.vy = -0x40;
    objects[6].position.vx = objects[7].position.vx = x;
    objects[6].position.vz = objects[7].position.vz = xm_worldmap_player_position.vz >> 12;
    if ((actor->u.step += 0x180) > 0x800) {
        actor->unk54 += 0x180;
    }
    if (actor->u.step > 0x7F00) {
        actor->u.step = 0x7F00;
    }
    if (actor->unk54 > 0x7F00) {
        actor->unk54 = 0x7F00;
    }
    SCALE_SCRATCH->matrix[0] = xm_worldmap_identity_matrix;
    SCALE_SCRATCH->matrix[1] = SCALE_SCRATCH->matrix[0];
    SCALE_SCRATCH->scale[0].vx = SCALE_SCRATCH->scale[0].vz = actor->u.step;
    SCALE_SCRATCH->scale[1].vx = SCALE_SCRATCH->scale[1].vz = actor->unk54;
    SCALE_SCRATCH->scale[0].vy = SCALE_SCRATCH->scale[1].vy = 0x1000;
    xm_ScaleMatrix(&SCALE_SCRATCH->matrix[0], &SCALE_SCRATCH->scale[0]);
    xm_ScaleMatrix(&SCALE_SCRATCH->matrix[1], &SCALE_SCRATCH->scale[1]);
    objects[6].matrix = SCALE_SCRATCH->matrix[0];
    objects[7].matrix = SCALE_SCRATCH->matrix[1];
    return 1;
}
#endif


s32 func_8007BA08(void) {
    return 3;
}


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene14_exhaust_trail_update (src/decomp/src/worldmap/worldmap_scene14.c:413). */
#ifndef XM_S_worldmap_scene14_exhaust_trail_update
#define XM_S_worldmap_scene14_exhaust_trail_update
s32 xm_worldmap_scene14_exhaust_trail_update(s32 index) __asm__("func_8007BA10");
#endif
#ifndef XM_S_worldmap_scene14_exhaust_angle
#define XM_S_worldmap_scene14_exhaust_angle
extern SVECTOR xm_worldmap_scene14_exhaust_angle __asm__("D_8009A488");
#endif
#pragma push_macro("SCRIPT_VECTOR")
#undef SCRIPT_VECTOR
#define SCRIPT_VECTOR ((SVECTOR *)0x1F8000A0)
s32 func_8007BA10(s32 index) {
    WorldmapActor *actor;
    s32 result;

    result = 1;
    actor = &xm_worldmap_actor_slots[index];
    if (actor->unk4 == 1) {
        actor->unk4 = 0;
        actor->wait = 0x3C;
        actor->position.vx = xm_worldmap_player_position.vx;
        actor->position.vy = xm_worldmap_player_position.vy;
        actor->position.vz = xm_worldmap_player_position.vz;
    }
    if (--actor->wait > 0) {
        actor->position.vx -= 0x95D0;
        actor->position.vz += 0x2F130;
        SCRIPT_VECTOR->vx = actor->position.vx >> 12;
        SCRIPT_VECTOR->vy = actor->position.vy >> 12;
        SCRIPT_VECTOR->vz = actor->position.vz >> 12;
        xm_worldmap_effects_start_emitters(9, SCRIPT_VECTOR, &xm_worldmap_scene14_exhaust_angle);
    } else {
        actor->wait = 0x3C;
        actor->position = xm_worldmap_player_position;
        xm_worldmap_effects_stop_emitters(9);
        result = 3;
    }
    return result;
}
#pragma pop_macro("SCRIPT_VECTOR")
#endif

s32 func_8007BB60(s32 arg0) {
    WmObj *temp_v0;
    u8 *scene;

    func_800848B4(0, 1);
    func_800848B4(0, 2);
    func_800848B4(0, 3);
    temp_v0 = &D_8009BE24[arg0];
    scene = D_8009C620;
    temp_v0->unk50 = 0;
    temp_v0->unk54 = 0x100;
    temp_v0->unk58 = 0x100;
    (*(s16 *)((u8 *)(scene) + 0x18)) = 0;
    (*(s16 *)((u8 *)(scene) + 0x1A)) = 0;
    (*(s16 *)((u8 *)(scene) + 0x1C)) = 0;
    RotMatrixYXZ(scene + 0x18, scene + 0x20, scene);
    return 3;
}

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007BBEC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007BF50);


#ifndef XENO_PC_PORT
void func_8007C260(void) {
    func_80039FF8();
    func_8003852C(D_8006259C);
    HeapFree(D_8006259C);
    func_80084818();
    func_80086568();
    func_800866C8();
    func_80074F04();
    func_800750DC();
    func_80088FF4();
    func_80089128();
    func_80097D64();
    HeapFree(D_8009BC38);
    HeapFree(D_8009BCB0);
    HeapFree(D_8009BC3C);
    HeapFree(D_8009BCB4);
    HeapFree(D_8009C180);
    func_800976A0();
    D_8006F94E = 0x111;
    D_8006F954 = 2;
    D_8009BBC4 = 1;
    D_8006F950 = D_8009BD3A;
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): worldmapMode12_taskDirector_init (src/worldmap/w_8007BF50.c). */
#ifndef XM_T_FP_VEC3
#define XM_T_FP_VEC3
typedef struct {
    s32 vx, vy, vz;
} FP_VEC3;
#endif
#ifndef XM_T_SubSprite
#define XM_T_SubSprite
typedef struct {
     s16 m0_offsetX;
     s16 m2_offsetY;
     u8 m4_texcoordX;
     u8 m5_texcoordY;
     u8 m6_width;
     u8 m7_height;
     s8 m8_widthAdjust;
     s8 m9_heightAdjust;
     u16 mA_tpage;
     u16 mC_clut;
     u16 mE;
     u32 m10_colorAndCode;
     union {                     
                   u32 raw;
                   struct {
                       u32 x0_transformId : 3;
                       u32 x3_8bit : 1;         
                       u32 x4_tileFlipX : 1;
                       u32 x5_tileFlipY : 1;
                       u32 x6 : 26;
                   } bits;
               } m14_attrs;
} SubSprite;
#endif
#ifndef XM_T_SubgroupTransform
#define XM_T_SubgroupTransform
typedef struct {
     s8 m0_translateX;
     s8 m1_translateY;
     s16 m2_rotateX;
     s16 m4_rotateY;
     s16 m6_rotateZ;
} SubgroupTransform;
#endif
#ifndef XM_T_SpriteObject
#define XM_T_SpriteObject
typedef struct SpriteObject {
     s16 m0_rotation[3];
     s16 m6_scale[3];
     MATRIX mC_matrix;
     SubSprite* m2C_subSpriteBuffer;
     SubSprite* m30_subSprites;
     SubgroupTransform* m34_perSubgroupTransform; 
     struct SpriteActorCore* m38_pNext;
     s8 m3C_anchorX;
     s8 m3D_anchorY;                          
} SpriteObject;
#endif
#ifndef XM_T_uptr
#define XM_T_uptr
typedef u32 uptr;
#endif
#ifndef XM_T_Vec2s16
#define XM_T_Vec2s16
typedef struct {
    s16 vx, vy;
} Vec2s16;
#endif
#ifndef XM_T_SpriteVramData
#define XM_T_SpriteVramData
typedef struct {
     uptr m0_spriteData;
     Vec2s16 m4_vramLocation;
     Vec2s16 m8_clut;
     u8* mC_clutData;
     uptr m10_startOfAnimationContainer;
} SpriteVramData;
#endif
#ifndef XM_T_SpriteActorExtra
#define XM_T_SpriteActorExtra
typedef struct {
     s32 m0;
     s32 m4_gravityOverride;
     s32 m8;
     u16 mC_animationSignal;                    
     Vec2s16 mE_vramLocation;
     s16 m12;
     s16 m14_actorId;
     s16 m16;
     s16* m18_vramLocations;
} SpriteActorExtra;
#endif
#ifndef XM_T_SpriteActorCore
#define XM_T_SpriteActorCore
typedef struct SpriteActorCore {
     FP_VEC3 m0_position;
     FP_VEC3 mC_step;
     s32 m18_moveSpeed;
     s32 m1C_gravity;
     SpriteObject* m20_object;
     SpriteVramData* m24_vramData;
     u8 m28_r;
     u8 m29_g;
     u8 m2A_b;
     u8 m2B_code;                   
     s16 m2C_scale;
     s16 m2E_otIndex;
     s16 m30_otOffset;
     s16 m32_direction;
     s16 m34_currentSpriteFrame;
     s16 m36_height;
     s16 m38_centerHeight;
     s16 m3A_scaleModifier;
     u32 m3C_x0_type : 2;           
               u32 m3C_x2 : 1;
               u32 m3C_x3_flipX : 1;
               u32 m3C_x4 : 1;
               u32 m3C_x5_blendMode : 3;      
               u32 m3C_x8 : 8;
               u32 m3C_x10 : 4;
               u32 m3C_x14 : 4;
               u32 m3C_x18 : 1;
               u32 m3C_x19 : 1;
               u32 m3C_x1A : 1;                
               u32 m3C_x1B : 1;
               u32 m3C_x1C_matrixDirty : 1;
               u32 m3C_x1D : 1;
               u32 m3C_x1E_bundleChanged : 1;
               u32 m3C_x1F : 1;
     union {                     
                   u32 raw;
                   struct {
                       u32 x0 : 1;
                       u32 x1 : 1;
                       u32 x2_subSpriteCount : 6;
                       u32 x8 : 5;
                       u32 xD_mode : 4;          
                       u32 x11 : 1;
                       u32 x12 : 1;              
                       u32 x13 : 1;
                       u32 x14 : 1;              
                       u32 x15 : 11;
                   } bits;
               } m40_draw;
     void* m44_currentAnimationBundle;
     void* m48_defaultAnimationBundle;
     void* m4C_specialAnimation;
     u8* m50_soundEffectSeq;
     u16* m54_frameTable;
     u8* m58_startOfCurrentAnimation;
     u8* m5C;
     u8* m60_endOfAnimationContainer;
     u8* m64_spriteByteCode;
     void (*m68)(struct SpriteActorCore*);
     void* m6C_owner;
     struct SpriteActorCore* m70_parent;
     struct SpriteActorCore* m74_targetEntitySprite;
     u32 m78_terrainTriangle;
     SpriteActorExtra* m7C_extra;
     s16 m80_angle;
     s16 m82_entityScale;
     s16 m84_maxY;
     u16 m86_thisSize;
     u8* m88_stack2;
     s8 m8C_stackPosition;
     s8 m8D_parentAnimationId;
     u8 m8E_stack[16];
     s16 m9E_wait;
     SVECTOR mA0_destination;
     u32 mA8_x0 : 1;
               u32 mA8_x1_bounce : 10;
               u32 mA8_xB_frame : 6;
               u32 mA8_x11 : 3;
               u32 mA8_x14 : 2;
               u32 mA8_x16_bytecodeStep : 6;
               u32 mA8_x1C : 2;
               u32 mA8_x1E : 2;
#if VERSION_JP
     u32 mAC_x0 : 4;                
#else
     u32 mAC_x0 : 2;
#endif
               u32 mAC_x2_facing : 1;
               u32 mAC_x3 : 1;
               u32 mAC_x4 : 1;
               u32 mAC_x5 : 1;
               u32 mAC_x6 : 1;
               u32 mAC_x7_timeScale : 12;
#if VERSION_JP
               u32 mAC_x13 : 3;
#else
               u32 mAC_x13 : 5;
#endif
     u8 mAF_animationId;            
     s32 mB0_x0_animationId : 8;
               u32 mB0_x8 : 1;
               u32 mB0_x9 : 1;
               u32 mB0_xA : 1;
               u32 mB0_xB : 1;
               u32 mB0_xC : 20;
} SpriteActorCore;
#endif
#ifndef XM_T_sptr
#define XM_T_sptr
typedef s32 sptr;
#endif
#ifndef XM_T_sWorldmapStateEntry
#define XM_T_sWorldmapStateEntry
typedef struct sWorldmapStateEntry {
     s16 m0_state;
     s16 m2_waitTimer;
     s16 m4_request;
     s16 m6_targetEntity;
     s32 m8;           
     s32 mC;
     s32 m10;
     s32 m14;
     s32 (*m18_init)(s32 index);
     s32 (*m1C_update)(s32 index);
     s16 m20_subState;
     s16 m22_timer;
     s16 m24_hidden;
     s16 m26;
     VECTOR m28_position;
     VECTOR m38_step;
     s16 m48_direction;
     s16 m4A_stepScale;
     SpriteActorCore* m4C_sprite; 
     sptr m50;         
     sptr m54;         
     sptr m58;         
     s32 m5C;
     s32 m60;
     sptr m64;         
     sptr m68;         
     s32 m6C;
     s32 m70;
     s32 m74;          
     s32 m78;
     s32 m7C;
} sWorldmapStateEntry;
#endif
#ifndef XM_T_sWorldmapState
#define XM_T_sWorldmapState
typedef struct sWorldmapState {
     sWorldmapStateEntry m0_entries[64];
} sWorldmapState;
#endif
#ifndef XM_S_worldmapMode12_taskDirector_init
#define XM_S_worldmapMode12_taskDirector_init
s32 xm_worldmapMode12_taskDirector_init(s32) __asm__("func_8007C36C");
#endif
#ifndef XM_S_gWorldmapState
#define XM_S_gWorldmapState
extern sWorldmapState * xm_gWorldmapState __asm__("D_8009BE24");
#endif
#ifndef XM_S_worldmapMode12DirectorStates
#define XM_S_worldmapMode12DirectorStates
extern u16 xm_worldmapMode12DirectorStates[] __asm__("D_8009A4D8");
#endif
#ifndef XM_S_worldmapMode12DirectorTimers
#define XM_S_worldmapMode12DirectorTimers
extern u16 xm_worldmapMode12DirectorTimers[] __asm__("D_8009A4E8");
#endif
s32 func_8007C36C(s32 index) {
    sWorldmapStateEntry* e = &xm_gWorldmapState->m0_entries[index];

    e->m50 = 0;
    e->m20_subState = xm_worldmapMode12DirectorStates[0];
    e->m22_timer = xm_worldmapMode12DirectorTimers[e->m50];
    e->m50++;
    return 1;
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007C3B8);


#ifndef XENO_PC_PORT
s32 func_8007C724(s32 arg0) {
    void *temp_v1;

    D_8009BE0C = 0x78;
    D_8009D3F0 = 0x400000;
    D_8009D144 = 1;
    temp_v1 = (u8 *)D_8009BE24 + (arg0 << 7);
    *(s32 *)((s8*)(temp_v1) + 0x7C) = 0x1000;
    D_8009BD38 = -0x40;
    (*(s16*)&D_8009BD3A) = 0;
    D_8009BD3C = 0;
    D_8009BE28 = D_8009C5AC;
    D_8009D55C = D_8009C5AC;
    D_8009BE2C = D_8009C5B0;
    D_8009D560 = D_8009C5B0;
    D_8009BE30 = D_8009C5B4;
    D_8009D564 = D_8009C5B4;
    *(s32 *)((s8*)(temp_v1) + 0x58) = 0x40;
    *(s32 *)((s8*)(temp_v1) + 0x50) = 0;
    return 1;
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene12_camera_shots_update (src/decomp/src/worldmap/worldmap_scene12.c:179). */
#ifndef XM_T_ShotScratch
#define XM_T_ShotScratch
typedef struct {
    VECTOR point;     
    u8 pad10[0x90];
    SVECTOR spot;     
} ShotScratch;
#endif
#ifndef XM_S_worldmap_scene12_camera_shots_update
#define XM_S_worldmap_scene12_camera_shots_update
s32 xm_worldmap_scene12_camera_shots_update(s32 index) __asm__("func_8007C7D8");
#endif
#ifndef XM_S_worldmap_scene12_shot_path1_points
#define XM_S_worldmap_scene12_shot_path1_points
extern SVECTOR xm_worldmap_scene12_shot_path1_points[14] __asm__("D_8009A4F8");
#endif
#ifndef XM_S_worldmap_scene12_shot_path2_points
#define XM_S_worldmap_scene12_shot_path2_points
extern SVECTOR xm_worldmap_scene12_shot_path2_points[7] __asm__("D_8009A568");
#endif
__asm__(".xeno_jt 8 24 jtbl_8006FD58");
__asm__(".xeno_jt 8 28 jtbl_8006FD70");
s32 func_8007C7D8(s32 index) {
    WorldmapActor *actor;
    ShotScratch *scratch;
    SVECTOR *points;
    s32 shake;

    actor = &xm_worldmap_actor_slots[index];
    scratch = (ShotScratch *)0x1F800000;
    switch (actor->unk4) {
    case 1:
        actor->state = 1;
        actor->unk5C = 0x8200;
        actor->unk4 = 0;
        actor->unk60 = 1;
        break;
    case 2:
        actor->unk4 = 0;
        actor->state = 3;
        break;
    case 3:
        actor->unk4 = 0;
        actor->state = 4;
        break;
    case 4:
        actor->state = 5;
        actor->unk5C = 0xA800;
        actor->unk4 = 0;
        actor->unk60 = 8;
        break;
    case 6:
        actor->state = 6;
        actor->unk4 = 0;
        actor->u.step = 0;
        actor->unk58 = 0x40;
        break;
    }
    if (xm_worldmap_view_kind == 0) {
        xm_worldmap_camera_place_orbit(&xm_worldmap_view_setup, &xm_worldmap_camera, xm_worldmap_camera_distance, &xm_worldmap_camera_angle);
    }
    switch (actor->state) {
    case 2:
        actor->unk58 -= actor->unk60;
        if (actor->unk58 < 0) {
            actor->unk58 = 0;
            actor->state = 0;
        }
    case 1:
        actor->u.step += actor->unk58;
        if (actor->u.step > actor->unk5C) {
            actor->state = 2;
        }
    case 0:
    follow:
        scratch->point.vy = 0;
        scratch->point.vx = 0;
        scratch->point.vz = xm_worldmap_camera_follow_target.target.vz - xm_worldmap_camera.target.vz;
        xm_worldmap_wrap_offset(&scratch->point);
        GROUND_SCROLL[2] += scratch->point.vz;
        xm_worldmap_camera.target.vz = xm_worldmap_camera_follow_target.target.vz;
        points = &xm_worldmap_scene12_shot_path1_points[actor->u.step >> 12];
        if (points[2].pad != -1) {
            xm_worldmap_eval_quadratic_bspline(actor->u.step & 0xFFF, &points[0], &points[1], &points[2], &scratch->point);
            VIEW.at.vz = 0;
            VIEW.at.vx = 0;
            VIEW.up.vz = 0;
            VIEW.up.vx = 0;
            VIEW.eye.vx = scratch->point.vx >> 16;
            VIEW.up.vy = -0x1000;
            VIEW.eye.vz = -(scratch->point.vz >> 16);
            VIEW.eye.vy = scratch->point.vy >> 16;
            VIEW.at.vy = xm_worldmap_camera.target.vy >> 12;
        }
        xm_worldmap_camera_build_look_at(&xm_worldmap_view_setup);
        xm_worldmap_get_matrix_angles(&xm_worldmap_camera_matrix, &xm_worldmap_camera_angle);
        break;
    case 3:
        actor->unk7C = 0xF000;
        actor->state = 0;
        goto follow;
    case 4:
        scratch->spot.vy = 0;
        scratch->spot.vx = xm_worldmap_camera.target.vx >> 12;
        scratch->spot.vz = xm_worldmap_camera.target.vz >> 12;
        xm_worldmap_effects_start_emitters(0x1A, &scratch->spot, NULL);
        xm_worldmap_effects_start_emitters(0x1B, &scratch->spot, NULL);
        actor->unk7C -= 0x120;
        if (actor->unk7C < 0x1000) {
            actor->unk7C = 0x1000;
            actor->state = 0;
            xm_worldmap_effects_stop_emitters(0x1A);
            xm_worldmap_effects_stop_emitters(0x1B);
        }
        goto follow;
    case 5:
        actor->unk58 = 0x100;
        actor->state = 1;
        goto follow;
    case 6:
        scratch->point.vy = 0;
        scratch->point.vx = 0;
        scratch->point.vz = xm_worldmap_camera_follow_target.target.vz - xm_worldmap_camera.target.vz;
        xm_worldmap_wrap_offset(&scratch->point);
        GROUND_SCROLL[2] += scratch->point.vz;
        xm_worldmap_camera.target.vx = xm_worldmap_camera_follow_target.target.vx;
        xm_worldmap_camera.target.vy = xm_worldmap_camera_follow_target.target.vy;
        xm_worldmap_camera.target.vz = xm_worldmap_camera_follow_target.target.vz;
        actor->u.step += actor->unk58;
        if (actor->u.step > 0x4800) {
            actor->unk58--;
            if (actor->unk58 < 0) {
                actor->unk58 = 0;
            }
        }
        points = &xm_worldmap_scene12_shot_path2_points[actor->u.step >> 12];
        if (points[2].pad != -1) {
            xm_worldmap_eval_quadratic_bspline(actor->u.step & 0xFFF, &points[0], &points[1], &points[2], &scratch->point);
            VIEW.at.vz = 0;
            VIEW.at.vx = 0;
            VIEW.up.vz = 0;
            VIEW.up.vx = 0;
            VIEW.eye.vx = scratch->point.vx >> 16;
            VIEW.up.vy = -0x1000;
            VIEW.eye.vz = -(scratch->point.vz >> 16);
            VIEW.eye.vy = scratch->point.vy >> 16;
            VIEW.at.vy = xm_worldmap_camera.target.vy >> 12;
        }
        xm_worldmap_camera_build_look_at(&xm_worldmap_view_setup);
        xm_worldmap_get_matrix_angles(&xm_worldmap_camera_matrix, &xm_worldmap_camera_angle);
        break;
    }
    shake = xm_rand() % (actor->unk7C >> 12) - (actor->unk7C >> 13);
    scratch->spot.vy = shake;
    VIEW.eye.vy += shake;
    VIEW.at.vy += scratch->spot.vy;
    return 1;
}
#endif

s32 func_8007CC6C(s32 arg0) {
    WmObj *obj;

    func_800848B4(4, 0);
    func_800848B4(4, 2);
    func_800848B4(4, 1);
    func_800848B4(4, 3);
    *(s16 *)(D_8009C620 + 0x150) = 0;
    *(s16 *)(D_8009C620 + 0x16C) = 0;
    *(s16 *)(D_8009C620 + 0x16A) = 0;
    *(s16 *)(D_8009C620 + 0x168) = 0;
    RotMatrixYXZ(D_8009C620 + 0x168, D_8009C620 + 0x170);
    obj = &D_8009BE24[arg0];
    obj->unk40 = -0x4000;
    obj->pos[0] = 0xD00000;
    obj->unk3C = 0;
    obj->unk38 = 0;
    obj->pos[1] = 0;
    obj->pos[2] = 0x400000;
    return 1;
}

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007CD20);

s32 func_8007CE84(s32 arg0) {
    WmObj *obj;

    func_800848B4(5, 6);
    *(s16 *)(D_8009C620 + 0x1A4) = 0;
    *(s16 *)(D_8009C620 + 0x1C0) = 0;
    *(s16 *)(D_8009C620 + 0x1BE) = 0;
    *(s16 *)(D_8009C620 + 0x1BC) = 0;
    RotMatrixYXZ(D_8009C620 + 0x1BC, D_8009C620 + 0x1C4);
    obj = &D_8009BE24[arg0];
    obj->unk40 = -0x4000;
    obj->pos[0] = 0xB00000;
    obj->unk20 = 0;
    obj->unk3C = 0;
    obj->unk38 = 0;
    obj->pos[1] = 0;
    obj->pos[2] = 0x200000;
    return 1;
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene12_drift_object5_update (src/decomp/src/worldmap/worldmap_scene12.c:401). */
#ifndef XM_T_ActorScratch
#define XM_T_ActorScratch
typedef struct {
    VECTOR work;      
    u8 pad10[0x20];
    VECTOR start;     
    u8 pad40[0x50];
    VECTOR probe;     
    SVECTOR position; 
    SVECTOR angle;    
} ActorScratch;
#endif
#ifndef XM_T_ACTOR_SCRATCH
#define XM_T_ACTOR_SCRATCH
#define ACTOR_SCRATCH ((ActorScratch *)0x1F800000)
#endif
#ifndef XM_T_SCRIPT_VECTOR
#define XM_T_SCRIPT_VECTOR
#define SCRIPT_VECTOR ((SVECTOR *)0x1F8000A0)
#endif
#ifndef XM_S_worldmap_scene12_drift_object5_update
#define XM_S_worldmap_scene12_drift_object5_update
s32 xm_worldmap_scene12_drift_object5_update(s32 index) __asm__("func_8007CF18");
#endif
#ifndef XM_S_worldmap_terrain_get_wave_height
#define XM_S_worldmap_terrain_get_wave_height
s32 xm_worldmap_terrain_get_wave_height(s32 x, s32 z) __asm__("func_80093A5C");
#endif
s32 func_8007CF18(s32 index) {
    WorldmapActor *actor;
    SceneObject *objects;
    SVECTOR *vector;

    actor = &xm_worldmap_actor_slots[index];
    objects = xm_worldmap_objects;
    if (actor->unk4 == 1) {
        actor->unk4 = 0;
        actor->state = 1;
    }
    actor->position.vz += actor->motion.vz;
    xm_worldmap_wrap_position(&actor->position);
    actor->position.vy = xm_worldmap_terrain_get_wave_height(actor->position.vx, actor->position.vz) - 0x4000;
    objects[5].position.vx = actor->position.vx >> 12;
    objects[5].position.vy = actor->position.vy >> 12;
    objects[5].position.vz = actor->position.vz >> 12;
    ACTOR_SCRATCH->position.vx = actor->position.vx >> 12;
    vector = SCRIPT_VECTOR;
    ACTOR_SCRATCH->position.vy = actor->position.vy >> 12;
    ACTOR_SCRATCH->position.vz = actor->position.vz >> 12;
    xm_worldmap_effects_start_emitters(0x15, SCRIPT_VECTOR, NULL);
    if (actor->state == 1) {
        vector->vx = actor->position.vx >> 12;
        vector->vy = actor->position.vy >> 12;
        vector->vz = actor->position.vz >> 12;
        xm_worldmap_effects_start_emitters(0x1E, SCRIPT_VECTOR, NULL);
    }
    return 1;
}
#endif

s32 func_8007D078(s32 arg0) {
    WmObj *obj;

    func_800848B4(9, 7);
    func_800848B4(9, 8);
    *(s16 *)(D_8009C620 + 0x2F4) = 0;
    *(s16 *)(D_8009C620 + 0x310) = 0;
    *(s16 *)(D_8009C620 + 0x30E) = 0;
    *(s16 *)(D_8009C620 + 0x30C) = 0;
    RotMatrixYXZ(D_8009C620 + 0x30C, D_8009C620 + 0x314);
    obj = &D_8009BE24[arg0];
    obj->unk40 = -0x4000;
    obj->unk3C = 0;
    obj->unk38 = 0;
    obj->pos[0] = 0xC00000;
    obj->pos[1] = 0;
    obj->pos[2] = 0;
    return 1;
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene12_drift_object9_update (src/decomp/src/worldmap/worldmap_scene12.c:455). */
#ifndef XM_S_worldmap_scene12_drift_object9_update
#define XM_S_worldmap_scene12_drift_object9_update
s32 xm_worldmap_scene12_drift_object9_update(s32 index) __asm__("func_8007D110");
#endif
s32 func_8007D110(s32 index) {
    WorldmapActor *actor;
    SceneObject *objects;

    actor = &xm_worldmap_actor_slots[index];
    objects = xm_worldmap_objects;
    if (actor->unk4 == 1) {
        actor->unk4 = 0;
        objects[9].visible = 1;
        objects[7].visible = 1;
        objects[8].visible = 1;
        xm_worldmap_effects_stop_emitters(0x16);
        return 3;
    }
    actor->position.vz += actor->motion.vz;
    xm_worldmap_wrap_position(&actor->position);
    actor->position.vy = xm_worldmap_terrain_get_wave_height(actor->position.vx, actor->position.vz) - 0x4000;
    objects[9].position.vx = actor->position.vx >> 12;
    objects[9].position.vy = actor->position.vy >> 12;
    objects[9].position.vz = actor->position.vz >> 12;
    SCRIPT_VECTOR->vx = actor->position.vx >> 12;
    SCRIPT_VECTOR->vy = actor->position.vy >> 12;
    SCRIPT_VECTOR->vz = actor->position.vz >> 12;
    xm_worldmap_effects_start_emitters(0x16, SCRIPT_VECTOR, NULL);
    return 1;
}
#endif

s32 func_8007D228(s32 arg0) {
    WmObj* obj;

    func_800848B4(0xA, 0xB);
    *(s16*)(D_8009C620 + 0x348) = 0;
    *(s16*)(D_8009C620 + 0x364) = 0;
    *(s16*)(D_8009C620 + 0x362) = 0;
    *(s16*)(D_8009C620 + 0x360) = 0;
    RotMatrixYXZ(D_8009C620 + 0x360, D_8009C620 + 0x368);
    obj = &D_8009BE24[arg0];
    obj->unk40 = -0x4000;
    obj->pos[0] = 0xE00000;
    obj->unk3C = 0;
    obj->unk38 = 0;
    obj->pos[1] = 0;
    obj->pos[2] = 0x100000;
    return 1;
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene12_drift_object10_update (src/decomp/src/worldmap/worldmap_scene12.c:504). */
#ifndef XM_S_worldmap_scene12_drift_object10_update
#define XM_S_worldmap_scene12_drift_object10_update
s32 xm_worldmap_scene12_drift_object10_update(s32 index) __asm__("func_8007D2B8");
#endif
s32 func_8007D2B8(s32 index) {
    WorldmapActor *actor;
    SceneObject *objects;

    actor = &xm_worldmap_actor_slots[index];
    objects = xm_worldmap_objects;
    if (actor->unk4 == 1) {
        actor->unk4 = 0;
        objects[10].visible = 1;
        objects[11].visible = 1;
        xm_worldmap_effects_stop_emitters(0x17);
        SCRIPT_VECTOR->vx = actor->position.vx >> 12;
        SCRIPT_VECTOR->vy = actor->position.vy >> 12;
        SCRIPT_VECTOR->vz = actor->position.vz >> 12;
        xm_worldmap_effects_start_emitters(0x1C, SCRIPT_VECTOR, NULL);
        return 3;
    }
    actor->position.vz += actor->motion.vz;
    xm_worldmap_wrap_position(&actor->position);
    actor->position.vy = xm_worldmap_terrain_get_wave_height(actor->position.vx, actor->position.vz) - 0x4000;
    objects[10].position.vx = actor->position.vx >> 12;
    objects[10].position.vy = actor->position.vy >> 12;
    objects[10].position.vz = actor->position.vz >> 12;
    SCRIPT_VECTOR->vx = actor->position.vx >> 12;
    SCRIPT_VECTOR->vy = actor->position.vy >> 12;
    SCRIPT_VECTOR->vz = actor->position.vz >> 12;
    xm_worldmap_effects_start_emitters(0x17, SCRIPT_VECTOR, NULL);
    return 1;
}
#endif

s32 func_8007D414(s32 arg0) {
    WmObj* obj;

    func_800848B4(0xC, 0xF);
    *(s16*)(D_8009C620 + 0x3F0) = 0;
    *(s16*)(D_8009C620 + 0x40C) = 0;
    *(s16*)(D_8009C620 + 0x40A) = 0;
    *(s16*)(D_8009C620 + 0x408) = 0;
    RotMatrixYXZ(D_8009C620 + 0x408, D_8009C620 + 0x410);
    obj = &D_8009BE24[arg0];
    obj->unk40 = -0x4000;
    obj->pos[0] = 0xE80000;
    obj->unk3C = 0;
    obj->unk38 = 0;
    obj->pos[1] = 0;
    obj->pos[2] = 0x280000;
    return 1;
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene12_drift_object12_update (src/decomp/src/worldmap/worldmap_scene12.c:556). */
#ifndef XM_S_worldmap_scene12_drift_object12_update
#define XM_S_worldmap_scene12_drift_object12_update
s32 xm_worldmap_scene12_drift_object12_update(s32 index) __asm__("func_8007D4A4");
#endif
s32 func_8007D4A4(s32 index) {
    WorldmapActor *actor;
    SceneObject *objects;

    actor = &xm_worldmap_actor_slots[index];
    objects = xm_worldmap_objects;
    if (actor->unk4 == 1) {
        actor->unk4 = 0;
        objects[12].visible = 1;
        objects[15].visible = 1;
        xm_worldmap_effects_stop_emitters(0x18);
        SCRIPT_VECTOR->vx = actor->position.vx >> 12;
        SCRIPT_VECTOR->vy = actor->position.vy >> 12;
        SCRIPT_VECTOR->vz = actor->position.vz >> 12;
        xm_worldmap_effects_start_emitters(0x1D, SCRIPT_VECTOR, NULL);
        return 3;
    }
    actor->position.vz += actor->motion.vz;
    xm_worldmap_wrap_position(&actor->position);
    actor->position.vy = xm_worldmap_terrain_get_wave_height(actor->position.vx, actor->position.vz) - 0x4000;
    objects[12].position.vx = actor->position.vx >> 12;
    objects[12].position.vy = actor->position.vy >> 12;
    objects[12].position.vz = actor->position.vz >> 12;
    SCRIPT_VECTOR->vx = actor->position.vx >> 12;
    SCRIPT_VECTOR->vy = actor->position.vy >> 12;
    SCRIPT_VECTOR->vz = actor->position.vz >> 12;
    xm_worldmap_effects_start_emitters(0x18, SCRIPT_VECTOR, NULL);
    return 1;
}
#endif

s32 func_8007D600(s32 arg0) {
    WmObj* obj;

    func_800848B4(0xD, 0xE);
    *(s16*)(D_8009C620 + 0x444) = 0;
    *(s16*)(D_8009C620 + 0x460) = 0;
    *(s16*)(D_8009C620 + 0x45E) = 0;
    *(s16*)(D_8009C620 + 0x45C) = 0;
    RotMatrixYXZ(D_8009C620 + 0x45C, D_8009C620 + 0x464);
    obj = &D_8009BE24[arg0];
    obj->unk40 = -0x4000;
    obj->pos[0] = 0xF80000;
    obj->unk3C = 0;
    obj->unk38 = 0;
    obj->pos[1] = 0;
    obj->pos[2] = 0x380000;
    return 1;
}


s32 func_8007D690(s32 arg0) {
    void *temp_s0;
    void *temp_s1;

    temp_s1 = ((void *)D_8009C620);
    temp_s0 = (u8 *)D_8009BE24 + (arg0 << 7);
    *(s32 *)((s8*)(temp_s0) + 0x30) = (s32) (*(s32 *)((s8*)(temp_s0) + 0x30) + *(s32 *)((s8*)(temp_s0) + 0x40));
    func_80093354(temp_s0 + 0x28);
    *(s32 *)((s8*)(temp_s0) + 0x2C) = (s32) (func_80093A5C(*(s32 *)((s8*)(temp_s0) + 0x28), *(s32 *)((s8*)(temp_s0) + 0x30)) - 0x4000);
    *(s32 *)((s8*)(temp_s1) + 0x44C) = (s32) ((s32) *(s32 *)((s8*)(temp_s0) + 0x28) >> 0xC);
    *(s32 *)((s8*)(temp_s1) + 0x450) = (s32) ((s32) *(s32 *)((s8*)(temp_s0) + 0x2C) >> 0xC);
    *(s32 *)((s8*)(temp_s1) + 0x454) = (s32) ((s32) *(s32 *)((s8*)(temp_s0) + 0x30) >> 0xC);
    *(s16 *)WM_SPAD(0x1F8000A0) = (s16) ((s32) *(s32 *)((s8*)(temp_s0) + 0x28) >> 0xC);
    *(s16 *)WM_SPAD(0x1F8000A2) = (s16) ((s32) *(s32 *)((s8*)(temp_s0) + 0x2C) >> 0xC);
    *(s16 *)WM_SPAD(0x1F8000A4) = (s16) ((s32) *(s32 *)((s8*)(temp_s0) + 0x30) >> 0xC);
    func_80089160(0x19, WM_SPAD(0x1F8000A0), 0);
    return 1;
}


s32 func_8007D774(s32 arg0) {
    WmObj *obj;

    *(s16 *)(D_8009C620 + 0x540) = 1;
    *(s16 *)(D_8009C620 + 0x55C) = 0;
    *(s16 *)(D_8009C620 + 0x55A) = 0;
    *(s16 *)(D_8009C620 + 0x558) = 0;
    RotMatrixYXZ(D_8009C620 + 0x558, D_8009C620 + 0x560);
    obj = &D_8009BE24[arg0];
    obj->pos[0] = 0xD00000;
    obj->pos[2] = 0x400000;
    obj->pos[1] = 0xFFD80000;
    obj->unk3C = 0;
    obj->unk38 = 0;
    obj->unk40 = -0x6000;
    return 3;
}

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007D7FC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007D918);

#ifndef XENO_PC_PORT
extern u16 D_8009A5CC[];
extern s32 D_8009D3D4;
void func_8007DCE0(void) {
    func_8003A89C(D_80062528, 0, 0xF0);
    func_80039FF8();
    func_8003852C(D_8006259C);
    HeapFree(D_8006259C);
    func_80084818();
    func_80086568();
    func_800866C8();
    func_80074F04();
    func_800750DC();
    func_80088FF4();
    func_80089128();
    func_80097D64();
    HeapFree(D_8009BC38);
    HeapFree(D_8009BCB0);
    HeapFree(D_8009BC3C);
    HeapFree(D_8009BCB4);
    HeapFree(D_8009C180);
    func_800976A0();
    D_8006F954 = D_8009A5CC[D_8009D3D4];
    D_8006F94E = 0x1A1;
    D_8009BBC4 = 1;
    D_8006F950 = D_8009BD3A;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene15_director_start (src/decomp/src/worldmap/worldmap_scene12.c:785). */
#ifndef XM_T_Sequence
#define XM_T_Sequence
typedef struct {
    s16 *states;
    u16 *durations;
} Sequence;
#endif
#ifndef XM_S_worldmap_scene15_director_start
#define XM_S_worldmap_scene15_director_start
s32 xm_worldmap_scene15_director_start(s32 index) __asm__("func_8007DE14");
#endif
#ifndef XM_S_worldmap_entry_index
#define XM_S_worldmap_entry_index
extern s32 xm_worldmap_entry_index __asm__("D_8009D3D4");
#endif
#ifndef XM_S_worldmap_scene15_cue_sequences
#define XM_S_worldmap_scene15_cue_sequences
extern Sequence xm_worldmap_scene15_cue_sequences[] __asm__("D_8009A65C");
#endif
s32 func_8007DE14(s32 index) {
    s32 unused[2]; 
    WorldmapActor *actor;
    s32 sequence;

    sequence = xm_worldmap_entry_index;
    actor = &xm_worldmap_actor_slots[index];
    actor->unk54 = (s32)xm_worldmap_scene15_cue_sequences[sequence].states;
    actor->unk58 = (s32)xm_worldmap_scene15_cue_sequences[sequence].durations;
    actor->u.step = 0;
    actor->state = *(s16 *)actor->unk54;
    actor->wait = ((u16 *)actor->unk58)[actor->u.step];
    actor->u.step++;
    return 1;
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007DE98);


#ifndef XENO_PC_PORT
s32 func_8007E450(s32 arg0) {
    void *temp_v1;

    D_8009D144 = 0;
    temp_v1 = (u8 *)D_8009BE24 + (arg0 << 7);
    *(s32 *)((s8*)(temp_v1) + 0x7C) = 0x1000;
    D_8009BE0C = 0x78;
    D_8009BE28 = D_8009C5AC;
    D_8009D55C = D_8009C5AC;
    D_8009BE2C = D_8009C5B0;
    D_8009D560 = D_8009C5B0;
    D_8009BE30 = D_8009C5B4;
    D_8009D564 = D_8009C5B4;
    *(s16 *)((s8*)(temp_v1) + 4) = 1;
    *(s32 *)((s8*)(temp_v1) + 0x58) = 0x40;
    *(s16 *)((s8*)(temp_v1) + 0x20) = 0;
    *(s32 *)((s8*)(temp_v1) + 0x50) = 0;
    return 1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007E4E4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007EBBC);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene15_flying_vehicle_start (src/decomp/src/worldmap/worldmap_scene15.c:489). */
#ifndef XM_S_worldmap_scene15_flying_vehicle_start
#define XM_S_worldmap_scene15_flying_vehicle_start
s32 xm_worldmap_scene15_flying_vehicle_start(s32 index) __asm__("func_8007ECA4");
#endif
#ifndef XM_S_worldmap_build_blue_translucent_quads
#define XM_S_worldmap_build_blue_translucent_quads
void xm_worldmap_build_blue_translucent_quads(SceneObject *object, POLY_FT4 *quads, s32 count, s32 abr) __asm__("func_8007EBBC");
#endif
#ifndef XM_S_worldmap_scene15_vehicle_stop_points
#define XM_S_worldmap_scene15_vehicle_stop_points
extern SVECTOR xm_worldmap_scene15_vehicle_stop_points[3] __asm__("D_8009A674");
#endif
s32 func_8007ECA4(s32 index) {
    SceneObject *objects;
    WorldmapActor *actor;

    xm_worldmap_objects_link(1, 2);
    xm_worldmap_objects_link(1, 3);
    objects = xm_worldmap_objects;
    actor = &xm_worldmap_actor_slots[index];
    xm_worldmap_build_blue_translucent_quads(&objects[1], objects[1].prims, objects[1].def->primitive_count, 3);
    xm_worldmap_build_blue_translucent_quads(&objects[2], objects[2].prims, objects[2].def->primitive_count, 3);
    xm_worldmap_build_blue_translucent_quads(&objects[3], objects[3].prims, objects[3].def->primitive_count, 1);
    xm_worldmap_objects[1].visible = 0;
    xm_worldmap_objects[1].angle.vz = 0;
    xm_worldmap_objects[1].angle.vy = 0;
    xm_worldmap_objects[1].angle.vx = 0;
    xm_RotMatrixYXZ(&xm_worldmap_objects[1].angle, &xm_worldmap_objects[1].matrix);
    actor->motion.vx = -0x85A;
    actor->state = 0;
    actor->motion.vy = 0;
    actor->motion.vz = 0xDA6;
    actor->u.step = xm_worldmap_scene15_vehicle_stop_points[xm_worldmap_entry_index].vx << 12;
    actor->unk54 = xm_worldmap_scene15_vehicle_stop_points[xm_worldmap_entry_index].vz << 12;
    actor->position.vx = xm_worldmap_player_position.vx - actor->motion.vx * 0x3680;
    actor->position.vy = xm_worldmap_player_position.vy;
    actor->position.vz = xm_worldmap_player_position.vz - actor->motion.vz * 0x3680;
    return 1;
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007EE34);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene15_flame_start (src/decomp/src/worldmap/worldmap_scene15.c:760). */
#ifndef XM_S_worldmap_scene15_flame_start
#define XM_S_worldmap_scene15_flame_start
s32 xm_worldmap_scene15_flame_start(s32 index) __asm__("func_8007F8AC");
#endif
#ifndef XM_S_worldmap_scene15_flame_sizes
#define XM_S_worldmap_scene15_flame_sizes
extern u16 xm_worldmap_scene15_flame_sizes[5] __asm__("D_8009A68C");
#endif
s32 func_8007F8AC(s32 index) {
    WorldmapActor *actor;
    SceneObject *object;
    s32 slot;
    s32 abr;

    slot = index - 4;
    actor = &xm_worldmap_actor_slots[index];
    object = &xm_worldmap_objects[slot + 4];
    abr = 3;
    if (slot == 4) {
        abr = 1;
    }
    xm_worldmap_build_blue_translucent_quads(object, object->prims, object->def->primitive_count, abr);
    actor->motion.vx = -0x85A;
    actor->motion.vz = 0xDA6;
    actor->state = 0;
    actor->motion.vy = 0;
    actor->unk5C = xm_worldmap_scene15_flame_sizes[slot];
    actor->wait = 0x3C;
    return 1;
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007F968);

#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): worldmapMode15_taskImpactRings_init (src/worldmap/w_8007CC6C.c). */
#ifndef XM_T_DynVertOffset
#define XM_T_DynVertOffset
typedef struct {
     s16 m0_offset[3];
     s16 m6_index;
} DynVertOffset;
#endif
#ifndef XM_T_ModelDynVertEntry
#define XM_T_ModelDynVertEntry
typedef struct {
     s32 m0_count;
     DynVertOffset* m4_positions;
     DynVertOffset* m8_normals;
} ModelDynVertEntry;
#endif
#ifndef XM_T_ModelDynVerts
#define XM_T_ModelDynVerts
typedef struct {
     s32 m0_count;
     ModelDynVertEntry m4_entries[1];
} ModelDynVerts;
#endif
#ifndef XM_T_ModelBlock
#define XM_T_ModelBlock
typedef struct {
     u16 m0_flags;
     u16 m2_numVertices;
     u16 m4_numPrims;
     u16 m6_numMeshBlocks;
     SVECTOR* m8_vertices;
     SVECTOR* mC_normals;
     u8* m10_meshBlocks;
     u8* m14_displayList;
     u8* m18_instanceData;
     ModelDynVerts* m1C_dynamicVertices;
     s16 m20_boundingBox[8];
     s32 m30_instanceDataSize;
     s32 m34_primBufferSize;
} ModelBlock;
#endif
#ifndef XM_T_sWorldmapCollisionTri
#define XM_T_sWorldmapCollisionTri
typedef struct sWorldmapCollisionTri {
     s16 m0_vertices[3];
     s16 m6_neighbors[3];
     u16 mC_type;
} sWorldmapCollisionTri;
#endif
#ifndef XM_T_sWorldmapCollisionMesh
#define XM_T_sWorldmapCollisionMesh
typedef struct sWorldmapCollisionMesh {
     s32 m0_numTriangles;
#ifndef PLATFORM_PC
     SVECTOR* m4_vertices;
#else
     u32 m4_vertices; 
#endif
     sWorldmapCollisionTri m8_triangles[1];
} sWorldmapCollisionMesh;
#endif
#ifndef XM_T_sWorldmapModel
#define XM_T_sWorldmapModel
typedef struct sWorldmapModel {
     s16 m0_hidden;          
     s16 m2_modelBlockIndex; 
     s16 m4_flags;
     s16 m6;
     VECTOR m8_position;         
     SVECTOR m18_rotation;
     MATRIX m20_rotationMatrix;
     ModelBlock* m40_modelBlock;
     sWorldmapCollisionMesh* m44_collisionMesh;
     u8* m48_primBuffers[2];        
     struct sWorldmapModel* m50_parentModel;
} sWorldmapModel;
#endif
#ifndef XM_S_worldmapMode15_taskImpactRings_init
#define XM_S_worldmapMode15_taskImpactRings_init
s32 xm_worldmapMode15_taskImpactRings_init(s32) __asm__("func_8007FC8C");
#endif
#ifndef XM_S_worldmapModels
#define XM_S_worldmapModels
extern sWorldmapModel * xm_worldmapModels __asm__("D_8009C620");
#endif
#ifndef XM_S_setWorldmapModelSemiTransGrey
#define XM_S_setWorldmapModelSemiTransGrey
s32 xm_setWorldmapModelSemiTransGrey(sWorldmapModel* model, POLY_FT4* poly, s32 count) __asm__("func_8007A06C");
#endif
s32 func_8007FC8C(s32 index) {
    sWorldmapModel* models = xm_worldmapModels;
    sWorldmapStateEntry* e = &xm_gWorldmapState->m0_entries[index];

    xm_setWorldmapModelSemiTransGrey(&models[9], (POLY_FT4*)models[9].m48_primBuffers[0], models[9].m40_modelBlock->m4_numPrims);
    xm_setWorldmapModelSemiTransGrey(&models[10], (POLY_FT4*)models[10].m48_primBuffers[0], models[10].m40_modelBlock->m4_numPrims);
    e->m28_position.vx = 0x1498000;
    e->m28_position.vy = -0x80000;
    e->m54 = -0x800;
    e->m20_subState = 0;
    e->m28_position.vz = 0x4AF2000;
    e->m50 = 0;
    e->m58 = 0x80;
    return 3;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene15_grow_objects_9_10_update (src/decomp/src/worldmap/worldmap_scene15.c:913). */
#ifndef XM_S_worldmap_scene15_grow_objects_9_10_update
#define XM_S_worldmap_scene15_grow_objects_9_10_update
s32 xm_worldmap_scene15_grow_objects_9_10_update(s32 index) __asm__("func_8007FD30");
#endif
#ifndef XM_S_worldmap_set_quad_colors
#define XM_S_worldmap_set_quad_colors
void xm_worldmap_set_quad_colors(POLY_FT4 *quads, s32 count, s32 r, s32 g, s32 b) __asm__("func_800809EC");
#endif
s32 func_8007FD30(s32 index) {
    SceneObject *object;
    WorldmapActor *actor;

    actor = &xm_worldmap_actor_slots[index];
    object = &xm_worldmap_objects[9];
    if (actor->unk4 == 1) {
        actor->unk4 = 0;
    }
    object[0].position.vx = object[1].position.vx = actor->position.vx >> 12;
    object[0].position.vy = object[1].position.vy = actor->position.vy >> 12;
    object[0].position.vz = object[1].position.vz = actor->position.vz >> 12;
    object[1].matrix = xm_worldmap_identity_matrix;
    object[0].matrix = object[1].matrix;
    SCALE_SCRATCH->scale[0].vx = SCALE_SCRATCH->scale[0].vz = actor->u.step;
    SCALE_SCRATCH->scale[1].vx = SCALE_SCRATCH->scale[1].vz = actor->unk54;
    SCALE_SCRATCH->scale[0].vy = SCALE_SCRATCH->scale[1].vy = 0x1000;
    xm_ScaleMatrix(&object[0].matrix, &SCALE_SCRATCH->scale[0]);
    xm_ScaleMatrix(&object[1].matrix, &SCALE_SCRATCH->scale[1]);
    if ((actor->u.step += 0x180) > 0x7FFF) {
        actor->u.step = 0x7FFF;
    }
    if ((actor->unk54 += 0x180) > 0x7FFF) {
        actor->unk54 = 0x7FFF;
    }
    xm_worldmap_set_quad_colors((&object->prims)[xm_worldmap_display_buffer_index], object->def->primitive_count, actor->unk58, actor->unk58, actor->unk58);
    object++;
    xm_worldmap_set_quad_colors((&object->prims)[xm_worldmap_display_buffer_index], object->def->primitive_count, actor->unk58, actor->unk58, actor->unk58);
    if ((actor->unk58 -= 3) < 0) {
        actor->unk58 = 0;
        return 3;
    }
    return 1;
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007FF70);


#ifndef XENO_PC_PORT
void func_80080218(void) {
    func_80039FF8();
    func_8003852C(D_8006259C);
    HeapFree(D_8006259C);
    func_80084818();
    func_80086124();
    func_80086568();
    func_800866C8();
    func_80074F04();
    func_800750DC();
    func_80088FF4();
    func_80089128();
    func_80097D64();
    HeapFree(D_8009BC38);
    HeapFree(D_8009BCB0);
    HeapFree(D_8009BC3C);
    HeapFree(D_8009BCB4);
    HeapFree(D_8009C180);
    func_800976A0();
    D_8006F94E = 0x84;
    D_8006F954 = 2;
    D_8009BBC4 = 1;
    D_8006F950 = D_8009BD3A;
}
#endif



#ifndef XENO_PC_PORT
s32 func_8008032C(s32 arg0) {
    void *temp_v1;

    temp_v1 = (u8 *)D_8009BE24 + (arg0 << 7);
    *(s32 *)((s8*)(temp_v1) + 0x50) = 0;
    *(u16 *)((s8*)(temp_v1) + 0x20) = (u16) D_8009A698;
    *(u16 *)((s8*)(temp_v1) + 0x22) = D_8009A6AC[*(s32 *)((s8*)(temp_v1) + 0x50)];
    return 1;
}
#else
s32 func_8008032C(s32 arg0) {
    u8 *actor = (u8 *)D_8009BE24 + (arg0 << 7);

    *(s32 *)(actor + 0x50) = 0;
    *(u16 *)(actor + 0x20) = D_8009A698;
    *(u16 *)(actor + 0x22) = D_8009A6AC[0];
    return 1;
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): worldmapMode13_taskDirector_update (src/worldmap/w_8007FD30.c). */
#ifndef XM_T_sWorldmapSeqHeader
#define XM_T_sWorldmapSeqHeader
typedef struct {
     u8 m0[0x14];
     u16 m14_id;
} sWorldmapSeqHeader;
#endif
#ifndef XM_S_worldmapMode13_taskDirector_update
#define XM_S_worldmapMode13_taskDirector_update
s32 xm_worldmapMode13_taskDirector_update(s32) __asm__("func_80080370");
#endif
#ifndef XM_S_worldmapMode13DirectorStates
#define XM_S_worldmapMode13DirectorStates
extern u16 xm_worldmapMode13DirectorStates[] __asm__("D_8009A698");
#endif
#ifndef XM_S_worldmapMode13DirectorTimers
#define XM_S_worldmapMode13DirectorTimers
extern u16 xm_worldmapMode13DirectorTimers[] __asm__("D_8009A6AC");
#endif
#ifndef XM_S_changeWorldmapEntityState
#define XM_S_changeWorldmapEntityState
s32 xm_changeWorldmapEntityState(s32 index, s32 state) __asm__("func_80097770");
#endif
#ifndef XM_S_worldmapFadePolyAbr
#define XM_S_worldmapFadePolyAbr
extern s32 xm_worldmapFadePolyAbr __asm__("D_8009CCA4");
#endif
#ifndef XM_S_worldmapFadePolyStep
#define XM_S_worldmapFadePolyStep
extern s32 xm_worldmapFadePolyStep __asm__("D_8009D3CC");
#endif
#ifndef XM_S_playSoundEffectInFreeSlot
#define XM_S_playSoundEffectInFreeSlot
void xm_playSoundEffectInFreeSlot(s32) __asm__("func_80039E60");
#endif
#ifndef XM_S_fieldSeq
#define XM_S_fieldSeq
extern sWorldmapSeqHeader * xm_fieldSeq __asm__("D_8006259C");
#endif
#ifndef XM_S_continueWorldmapLoop
#define XM_S_continueWorldmapLoop
extern s32 xm_continueWorldmapLoop __asm__("D_8009D554");
#endif
#ifndef XM_S_exitWorldMapMode
#define XM_S_exitWorldMapMode
extern s32 xm_exitWorldMapMode __asm__("D_8009D7CC");
#endif
s32 func_80080370(s32 index) {
    sWorldmapStateEntry* e = &xm_gWorldmapState->m0_entries[index];

    switch (e->m20_subState) {
    case 0:
        break;
    case 1:
        e->m22_timer--;
        if (e->m22_timer < 0) {
            e->m20_subState = xm_worldmapMode13DirectorStates[e->m50];
            e->m22_timer = xm_worldmapMode13DirectorTimers[e->m50];
            e->m50++;
        }
        break;
    case 2:
        xm_changeWorldmapEntityState(3, 1);
        e->m20_subState = 1;
        break;
    case 3:
        xm_changeWorldmapEntityState(2, 2);
        e->m20_subState = 1;
        break;
    case 4:
        xm_changeWorldmapEntityState(0, 0xD);
        xm_worldmapFadePolyAbr = 1;
        xm_worldmapFadePolyStep = 0x80;
        e->m20_subState = 1;
        break;
    case 5:
        xm_changeWorldmapEntityState(0, 0xC);
        xm_changeWorldmapEntityState(4, 1);
        xm_worldmapFadePolyAbr = 1;
        xm_worldmapFadePolyStep = 0x80;
        e->m20_subState = 1;
        break;
    case 6:
        xm_changeWorldmapEntityState(2, 3);
        e->m20_subState = 1;
        break;
    case 7:
        xm_changeWorldmapEntityState(0, 0xD);
        xm_worldmapFadePolyAbr = 2;
        xm_worldmapFadePolyStep = 4;
        e->m20_subState = 1;
        break;
    case 8:
        xm_playSoundEffectInFreeSlot((xm_fieldSeq->m14_id << 16) | 0x16);
        xm_playSoundEffectInFreeSlot((xm_fieldSeq->m14_id << 16) | 0x17);
        xm_playSoundEffectInFreeSlot((xm_fieldSeq->m14_id << 16) | 0x18);
        e->m20_subState = 1;
        break;
    case 64:
        xm_continueWorldmapLoop = 0;
        xm_exitWorldMapMode = 0;
        e->m20_subState = 0;
        break;
    }
    return 1;
}
#endif


#ifndef XENO_PC_PORT
s32 func_80080578(s32 arg0) {
    void *temp_v1;

    D_8009D144 = 0;
    temp_v1 = (u8 *)D_8009BE24 + (arg0 << 7);
    *(s32 *)((s8*)(temp_v1) + 0x7C) = 0x1000;
    D_8009BE0C = 0x78;
    D_8009BE28 = D_8009C5AC;
    D_8009D55C = D_8009C5AC;
    D_8009BE2C = D_8009C5B0;
    D_8009D560 = D_8009C5B0;
    D_8009BE30 = D_8009C5B4;
    D_8009D564 = D_8009C5B4;
    *(s16 *)((s8*)(temp_v1) + 4) = 1;
    *(s16 *)((s8*)(temp_v1) + 0x20) = 0;
    return 1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80080600);


s32 func_80080900(s32 arg0) {
    void *temp_v1;

    temp_v1 = (u8 *)D_8009BE24 + (arg0 << 7);
    *(s32 *)((s8*)(temp_v1) + 0x28) = (s32) D_8009C5AC;
    *(s32 *)((s8*)(temp_v1) + 0x2C) = (s32) D_8009C5B0;
    *(s32 *)((s8*)(temp_v1) + 0x30) = (s32) D_8009C5B4;
    return 1;
}



s32 func_80080944(s32 arg0) {
    void *temp_a2;

    temp_a2 = (u8 *)D_8009BE24 + (arg0 << 7);
    if (*(s16 *)((s8*)(temp_a2) + 4) == 1) {
        *(s16 *)((s8*)(temp_a2) + 4) = 0;
        *(s16 *)WM_SPAD(0x1F8000A0) = (s16) ((s32) *(s32 *)((s8*)(temp_a2) + 0x28) >> 0xC);
        *(s16 *)WM_SPAD(0x1F8000A2) = (s16) ((s32) *(s32 *)((s8*)(temp_a2) + 0x2C) >> 0xC);
        *(s16 *)WM_SPAD(0x1F8000A4) = (s16) ((s32) *(s32 *)((s8*)(temp_a2) + 0x30) >> 0xC);
        func_80089160(0x28, WM_SPAD(0x1F8000A0), 0);
        func_80089160(0x29, WM_SPAD(0x1F8000A0), 0);
        func_80089160(0x2A, WM_SPAD(0x1F8000A0), 0);
    }
    return 1;
}



#ifndef XENO_PC_PORT
void func_800809EC(u8 *arg0, s32 arg1, s8 arg2, s8 arg3, s32 arg4) {
    u8 pad[4];
    s32 i = 0;

    if (arg1 > 0) {
        do {
            i++;
            arg0[4] = arg2;
            arg0[5] = arg3;
            arg0[6] = arg4;
            arg0 += 0x28;
        } while (i < arg1);
    }
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene13_grow_objects_0_1_start (src/decomp/src/worldmap/worldmap_scene13.c:245). */
#ifndef XM_S_worldmap_scene13_grow_objects_0_1_start
#define XM_S_worldmap_scene13_grow_objects_0_1_start
s32 xm_worldmap_scene13_grow_objects_0_1_start(s32 index) __asm__("func_80080A28");
#endif
s32 func_80080A28(s32 index) {
    WorldmapActor *actor;
    SceneObject *object;

    object = xm_worldmap_objects;
    actor = &xm_worldmap_actor_slots[index];
    actor->position.vx = xm_worldmap_player_position.vx;
    actor->position.vy = -0x80000;
    actor->position.vz = xm_worldmap_player_position.vz;
    actor->u.step = 0;
    actor->unk54 = -0x800;
    actor->unk58 = 0x80;
    xm_worldmap_build_translucent_quads(object, object->prims, object->def->primitive_count);
    object++;
    xm_worldmap_build_translucent_quads(object, object->prims, object->def->primitive_count);
    return 3;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene13_grow_objects_0_1_update (src/decomp/src/worldmap/worldmap_scene13.c:264). */
#ifndef XM_S_worldmap_scene13_grow_objects_0_1_update
#define XM_S_worldmap_scene13_grow_objects_0_1_update
s32 xm_worldmap_scene13_grow_objects_0_1_update(s32 index) __asm__("func_80080AC4");
#endif
s32 func_80080AC4(s32 index) {
    SceneObject *object;
    WorldmapActor *actor;

    object = xm_worldmap_objects;
    actor = &xm_worldmap_actor_slots[index];
    if (actor->unk4 == 1) {
        actor->unk4 = 0;
    }
    object[0].position.vx = object[1].position.vx = actor->position.vx >> 12;
    object[0].position.vy = object[1].position.vy = actor->position.vy >> 12;
    object[0].position.vz = object[1].position.vz = actor->position.vz >> 12;
    object[1].matrix = xm_worldmap_identity_matrix;
    object[0].matrix = object[1].matrix;
    SCALE_SCRATCH->scale[0].vx = SCALE_SCRATCH->scale[0].vz = actor->u.step;
    SCALE_SCRATCH->scale[1].vx = SCALE_SCRATCH->scale[1].vz = actor->unk54;
    SCALE_SCRATCH->scale[0].vy = SCALE_SCRATCH->scale[1].vy = 0x1000;
    xm_ScaleMatrix(&object[0].matrix, &SCALE_SCRATCH->scale[0]);
    xm_ScaleMatrix(&object[1].matrix, &SCALE_SCRATCH->scale[1]);
    if ((actor->u.step += 0x180) > 0x7FFF) {
        actor->u.step = 0x7FFF;
    }
    if ((actor->unk54 += 0x180) > 0x7FFF) {
        actor->unk54 = 0x7FFF;
    }
    xm_worldmap_set_quad_colors((&object->prims)[xm_worldmap_display_buffer_index], object->def->primitive_count, actor->unk58, actor->unk58, actor->unk58);
    object++;
    xm_worldmap_set_quad_colors((&object->prims)[xm_worldmap_display_buffer_index], object->def->primitive_count, actor->unk58, actor->unk58, actor->unk58);
    if ((actor->unk58 -= 4) < 0) {
        actor->unk58 = 0;
        return 3;
    }
    return 1;
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80080D00);


#ifndef XENO_PC_PORT
void func_8008106C(void) {
    func_80039FF8();
    func_8003852C(D_8006259C);
    HeapFree(D_8006259C);
    func_80084818();
    func_80086568();
    func_800866C8();
    func_80074F04();
    func_800750DC();
    func_80088FF4();
    func_80089128();
    func_80097D64();
    HeapFree(D_8009BC38);
    HeapFree(D_8009BCB0);
    HeapFree(D_8009BC3C);
    HeapFree(D_8009BCB4);
    HeapFree(D_8009C180);
    func_800976A0();
    D_8006F94E = 0x1FA;
    D_8006F954 = 0;
    D_8009BBC4 = 1;
    D_8006F950 = D_8009BD3A;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80081174);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800811C0);


#ifndef XENO_PC_PORT
s32 func_800813E8(s32 arg0) {
    void *temp_v1;

    D_8009D144 = 0;
    temp_v1 = (u8 *)D_8009BE24 + (arg0 << 7);
    *(s32 *)((s8*)(temp_v1) + 0x7C) = 0x1000;
    D_8009BE0C = 0x78;
    D_8009BE28 = D_8009C5AC;
    D_8009D55C = D_8009C5AC;
    D_8009BE2C = D_8009C5B0;
    D_8009D560 = D_8009C5B0;
    D_8009BE30 = D_8009C5B4;
    D_8009D564 = D_8009C5B4;
    *(s16 *)((s8*)(temp_v1) + 4) = 1;
    *(s16 *)((s8*)(temp_v1) + 0x20) = 0;
    return 1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80081470);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene16_build_translucent_quads (src/decomp/src/worldmap/worldmap_scenes_16_18.c:242). */
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
#ifndef XM_T_setcode
#define XM_T_setcode
#define setcode(p, _code) (((P_TAG *)(p))->code = (u_char)(_code))
#endif
#ifndef XM_T_setPolyFT4
#define XM_T_setPolyFT4
#define setPolyFT4(p) setlen(p, 9), setcode(p, 0x2c)
#endif
#ifndef XM_T_getcode
#define XM_T_getcode
#define getcode(p) (u_char)(((P_TAG *)(p))->code)
#endif
#ifndef XM_T_setSemiTrans
#define XM_T_setSemiTrans
#define setSemiTrans(p, abe) \
    ((abe) ? setcode(p, getcode(p) | 0x02) : setcode(p, getcode(p) & ~0x02))
#endif
#ifndef XM_S_worldmap_scene16_build_translucent_quads
#define XM_S_worldmap_scene16_build_translucent_quads
void xm_worldmap_scene16_build_translucent_quads(SceneObject *object, POLY_FT4 *quads, s32 count, s32 abr) __asm__("func_800816DC");
#endif
#ifndef XM_S_memcpy
#define XM_S_memcpy
void *xm_memcpy() __asm__("memcpy");
#endif
void func_800816DC(SceneObject *object, POLY_FT4 *quads, s32 count, s32 abr) {
    s32 i;

    for (i = 0; i < count; i++) {
        setPolyFT4(quads);
        quads->tpage = xm_GetTPage(0, abr, 0x180, 0);
        setSemiTrans(quads, 1);
        quads++;
    }
    xm_memcpy(object->prims2, object->prims, count * sizeof(POLY_FT4));
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene16_fade_object2_start (src/decomp/src/worldmap/worldmap_scenes_16_18.c:255). */
#ifndef XM_S_worldmap_scene16_fade_object2_start
#define XM_S_worldmap_scene16_fade_object2_start
s32 xm_worldmap_scene16_fade_object2_start(s32 index) __asm__("func_800817A0");
#endif
s32 func_800817A0(s32 index) {
    WorldmapActor *actor;
    SceneObject *objects;

    actor = &xm_worldmap_actor_slots[index];
    objects = xm_worldmap_objects;
    actor->state = 0;
    actor->position.vx = xm_worldmap_player_position.vx;
    actor->position.vy = xm_worldmap_player_position.vy - 0x100000;
    actor->position.vz = xm_worldmap_player_position.vz;
    actor->u.step = 0;
    actor->unk54 = 0;
    actor->unk58 = 0;
    xm_worldmap_scene16_build_translucent_quads(&objects[2], objects[2].prims, objects[2].def->primitive_count, 1);
    objects[2].position.vx = actor->position.vx >> 12;
    objects[2].position.vy = actor->position.vy >> 12;
    objects[2].position.vz = actor->position.vz >> 12;
    return 1;
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80081868);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene16_fade_objects_0_1_start (src/decomp/src/worldmap/worldmap_scenes_16_18.c:320). */
#ifndef XM_S_worldmap_scene16_fade_objects_0_1_start
#define XM_S_worldmap_scene16_fade_objects_0_1_start
s32 xm_worldmap_scene16_fade_objects_0_1_start(s32 index) __asm__("func_800819C8");
#endif
s32 func_800819C8(s32 index) {
    SceneObject *objects;
    WorldmapActor *actor;

    objects = xm_worldmap_objects;
    actor = &xm_worldmap_actor_slots[index];
    actor->state = 0;
    actor->position.vx = xm_worldmap_player_position.vx;
    actor->position.vy = xm_worldmap_player_position.vy;
    actor->position.vz = xm_worldmap_player_position.vz;
    actor->u.step = 0;
    actor->unk54 = 0;
    actor->unk58 = 0;
    objects[0].matrix = xm_worldmap_identity_matrix;
    SCALE_SCRATCH->scale[0].vx = SCALE_SCRATCH->scale[0].vz = 0x1000;
    SCALE_SCRATCH->scale[0].vy = 0x7000;
    xm_ScaleMatrix(&objects[0].matrix, &SCALE_SCRATCH->scale[0]);
    objects[1].matrix = objects[0].matrix;
    xm_worldmap_scene16_build_translucent_quads(objects, objects->prims, objects->def->primitive_count, 3);
    objects++;
    xm_worldmap_scene16_build_translucent_quads(objects, objects->prims, objects->def->primitive_count, 3);
    return 1;
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80081B24);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80081C3C);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene16_haze_update (src/decomp/src/worldmap/worldmap_scenes_16_18.c:401). */
#ifndef XM_T_QuadBuffer
#define XM_T_QuadBuffer
typedef struct QuadBuffer {
    POLY_FT4 quads[0xC0];
} QuadBuffer;
#endif
#ifndef XM_T_setaddr
#define XM_T_setaddr
#define setaddr(p, _addr) (((P_TAG *)(p))->addr = (u_long)(_addr))
#endif
#ifndef XM_T_getaddr
#define XM_T_getaddr
#define getaddr(p) (u_long)(((P_TAG *)(p))->addr)
#endif
#ifndef XM_T_addPrim
#define XM_T_addPrim
#define addPrim(ot, p) setaddr(p, getaddr(ot)), setaddr(ot, p)
#endif
#ifndef XM_T_DR_MOVE
#define XM_T_DR_MOVE
typedef struct {
    u_long tag;
    u_long code[5];
} DR_MOVE;
#endif
#ifndef XM_S_worldmap_scene16_haze_update
#define XM_S_worldmap_scene16_haze_update
s32 xm_worldmap_scene16_haze_update(void) __asm__("func_80081D80");
#endif
#ifndef XM_S_worldmap_scene16_haze_quads
#define XM_S_worldmap_scene16_haze_quads
extern QuadBuffer *xm_worldmap_scene16_haze_quads[2] __asm__("D_8009D158");
#endif
#ifndef XM_S_worldmap_scene16_haze_spreads
#define XM_S_worldmap_scene16_haze_spreads
extern u16 *xm_worldmap_scene16_haze_spreads __asm__("D_8009D148");
#endif
#ifndef XM_S_SetDrawMove
#define XM_S_SetDrawMove
void xm_SetDrawMove(DR_MOVE *p, RECT *rect, int x, int y) __asm__("SetDrawMove");
#endif
#ifndef XM_S_worldmap_scene16_haze_copies
#define XM_S_worldmap_scene16_haze_copies
extern DR_MOVE xm_worldmap_scene16_haze_copies[2] __asm__("D_8009D164");
#endif
s32 func_80081D80(void) {
    RECT rect;
    POLY_FT4 *quad;
    u16 *spread;
    s32 row;
    s32 next;
    s32 offset;

    row = 0;
    quad = xm_worldmap_scene16_haze_quads[xm_worldmap_display_buffer_index]->quads;
    spread = xm_worldmap_scene16_haze_spreads;
    do {
        offset = xm_rand() % *spread - (*spread >> 1);
        spread++;
        quad->y0 = row;
        quad->y1 = row;
        quad->v0 = row;
        quad->v1 = row;
        quad->u0 = 0;
        quad->u1 = 0xC0;
        quad->u2 = 0;
        quad->u3 = 0xC0;
        quad->x0 = offset + 0x40;
        quad->x1 = offset + 0x100;
        quad->x2 = offset + 0x40;
        quad->x3 = offset + 0x100;
        next = row + 1;
        quad->y2 = next;
        quad->y3 = next;
        quad->v2 = next;
        quad->v3 = next;
        row = next;
        addPrim(xm_worldmap_current_display_buffer->ot, quad);
        quad++;
    } while (row < 0xC0);
    rect.x = 0x40;
    rect.w = 0xC0;
    rect.h = 0xD8;
    rect.y = xm_worldmap_display_buffer_index * 0xD8;
    xm_SetDrawMove(&xm_worldmap_scene16_haze_copies[xm_worldmap_display_buffer_index], &rect, 0x280, 0x100);
    addPrim(xm_worldmap_current_display_buffer->ot, &xm_worldmap_scene16_haze_copies[xm_worldmap_display_buffer_index]);
    return 1;
}
#endif


s32 func_80081FB4(s32 arg0) {
    void *temp_v0;

    temp_v0 = (u8 *)D_8009BE24 + (arg0 << 7);
    *(s16 *)((s8*)(temp_v0) + 0x20) = 0;
    *(s32 *)((s8*)(temp_v0) + 0x50) = 1;
    return 1;
}


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80081FD8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80082324);


#ifndef XENO_PC_PORT
void func_800826B4(void) {
    func_80039FF8();
    func_8003852C(D_8006259C);
    HeapFree(D_8006259C);
    func_80084818();
    func_80086124();
    func_80086568();
    func_800866C8();
    func_80074F04();
    func_800750DC();
    func_80088FF4();
    func_80089128();
    func_80097D64();
    HeapFree(D_8009BC38);
    HeapFree(D_8009BCB0);
    HeapFree(D_8009BC3C);
    HeapFree(D_8009BCB4);
    HeapFree(D_8009C180);
    func_800976A0();
    D_8006F94E = 0x269;
    D_8006F954 = 2;
    D_8009BBC4 = 1;
    D_8006F950 = D_8009BD3A;
}
#endif



#ifndef XENO_PC_PORT
s32 func_800827C8(s32 arg0) {
    *(s32 **)((s8*)(((arg0 << 7) + (uintptr_t)D_8009BE24)) + 0x50) = &D_8009A758;
    return 1;
}
#else
s32 func_800827C8(s32 arg0) {
    u8 *actor = (u8 *)D_8009BE24 + (arg0 << 7);

    *(u32 *)(actor + 0x50) = 0x8009A758;
    return 1;
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene17_camera_start (src/decomp/src/worldmap/worldmap_scenes_16_18.c:663). */
#ifndef XM_S_worldmap_scene17_camera_start
#define XM_S_worldmap_scene17_camera_start
s32 xm_worldmap_scene17_camera_start(s32 index) __asm__("func_800827EC");
#endif
s32 func_800827EC(s32 index) {
    WorldmapActor *actor;

    actor = &xm_worldmap_actor_slots[index];
    actor->unk7C = 0x1000;
    actor->position.vx = xm_worldmap_camera_follow_target.target.vx = xm_worldmap_camera.target.vx = xm_worldmap_player_position.vx;
    actor->position.vy = xm_worldmap_camera_follow_target.target.vy = xm_worldmap_camera.target.vy = xm_worldmap_player_position.vy;
    actor->position.vz = xm_worldmap_camera_follow_target.target.vz = xm_worldmap_camera.target.vz = xm_worldmap_player_position.vz;
    actor->state = 0;
    actor->unk4 = 0;
    actor->unk5C = 0x500000;
    xm_worldmap_camera_angle.vx = -0x220;
    xm_worldmap_camera_angle.vy = 0;
    xm_worldmap_camera_angle.vz = 0;
    actor->motion.vx = actor->u.step = -0x220 << 12;
    xm_worldmap_view_center_y = 0x78;
    xm_worldmap_view_kind = 0;
    xm_worldmap_camera_distance = 0x500000;
    actor->motion.vy = actor->unk54 = xm_worldmap_camera_angle.vy << 12;
    actor->motion.vz = actor->unk58 = xm_worldmap_camera_angle.vz << 12;
    return 1;
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800828DC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80082F64);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene17_build_black_triangles (src/decomp/src/worldmap/worldmap_scenes_16_18.c:895). */
#ifndef XM_T_POLY_FT3
#define XM_T_POLY_FT3
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
} POLY_FT3;
#endif
#ifndef XM_S_worldmap_scene17_build_black_triangles
#define XM_S_worldmap_scene17_build_black_triangles
void xm_worldmap_scene17_build_black_triangles(SceneObject *object, POLY_FT3 *prims, s32 count, s32 abr) __asm__("func_80083108");
#endif
void func_80083108(SceneObject *object, POLY_FT3 *prims, s32 count, s32 abr) {
    s32 i;

    for (i = count - 1; i != -1; i--) {
        ((u8 *)prims)[3] = 7;
        prims->code = 0x24;
        prims->tpage = xm_GetTPage(0, abr, 0x2C0, 0x100);
        prims->r0 = 0;
        prims->g0 = 0;
        prims->b0 = 0;
        prims->code |= 2;
        prims++;
    }
    xm_memcpy(object->prims2, object->prims, count * sizeof(POLY_FT3));
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_set_triangle_colors (src/decomp/src/worldmap/worldmap_scenes_16_18.c:912). */
#ifndef XM_T_setRGB0
#define XM_T_setRGB0
#define setRGB0(p, _r0, _g0, _b0) ((p)->r0 = _r0, (p)->g0 = _g0, (p)->b0 = _b0)
#endif
#ifndef XM_S_worldmap_set_triangle_colors
#define XM_S_worldmap_set_triangle_colors
void xm_worldmap_set_triangle_colors(POLY_FT3 *prims, s32 count, s32 r, s32 g, s32 b) __asm__("func_800831D8");
#endif
void func_800831D8(POLY_FT3 *prims, s32 count, s32 r, s32 g, s32 b) {
    for (count--; count != -1; count--) {
        setRGB0(prims, r, g, b);
        prims++;
    }
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene17_pulse_start (src/decomp/src/worldmap/worldmap_scenes_16_18.c:920). */
#ifndef XM_S_worldmap_scene17_pulse_start
#define XM_S_worldmap_scene17_pulse_start
s32 xm_worldmap_scene17_pulse_start(s32 index) __asm__("func_80083214");
#endif
s32 func_80083214(s32 index) {
    SceneObject *object;

    object = &xm_worldmap_objects[78 + index];
    xm_worldmap_scene17_build_black_triangles(object, object->prims, object->def->primitive_count, 3);
    return 1;
}
#endif
INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80083264);


s32 func_800834D0(void) {
    return 1;
}



#ifndef XENO_PC_PORT
s32 func_800834D8(s32 arg0) {
    void *temp_a0;

    temp_a0 = (u8 *)D_8009BE24 + (arg0 << 7);
    if (*(s16 *)((s8*)(temp_a0) + 4) == 1) {
        *(s16 *)((s8*)(temp_a0) + 4) = 0;
        DrawSync(0);
        Vsync(0);
        func_80097D64();
        func_80097BC0(&D_8009C5AC);
        do {
            func_800967E4();
            Vsync(0);
        } while (func_80096668() > 0);
    }
    return 1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008355C);


#ifndef XENO_PC_PORT
void func_800837DC(void) {
    func_80039FF8();
    func_8003852C(D_8006259C);
    HeapFree(D_8006259C);
    func_80084818();
    func_80086568();
    func_800866C8();
    func_80074F04();
    func_800750DC();
    func_80088FF4();
    func_80089128();
    func_80097D64();
    HeapFree(D_8009BC38);
    HeapFree(D_8009BCB0);
    HeapFree(D_8009BC3C);
    HeapFree(D_8009BCB4);
    HeapFree(D_8009C180);
    func_800976A0();
    D_8006F94E = 0x269;
    D_8006F954 = 4;
    D_8009BBC4 = 1;
    D_8006F950 = D_8009BD3A;
}
#endif



#ifndef XENO_PC_PORT
s32 func_800838E8(s32 arg0) {
    *(s32 **)((s8*)(((arg0 << 7) + (uintptr_t)D_8009BE24)) + 0x50) = &D_8009AC60;
    return 1;
}
#else
s32 func_800838E8(s32 arg0) {
    u8 *actor = (u8 *)D_8009BE24 + (arg0 << 7);

    *(u32 *)(actor + 0x50) = 0x8009AC60;
    return 1;
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene18_camera_start (src/decomp/src/worldmap/worldmap_scenes_16_18.c:1098). */
#ifndef XM_S_worldmap_scene18_camera_start
#define XM_S_worldmap_scene18_camera_start
s32 xm_worldmap_scene18_camera_start(s32 index) __asm__("func_8008390C");
#endif
s32 func_8008390C(s32 index) {
    WorldmapActor *actor;

    actor = &xm_worldmap_actor_slots[index];
    actor->unk7C = 0x1000;
    actor->position.vx = xm_worldmap_camera_follow_target.target.vx = xm_worldmap_camera.target.vx = xm_worldmap_player_position.vx;
    actor->position.vy = xm_worldmap_camera_follow_target.target.vy = xm_worldmap_camera.target.vy = xm_worldmap_player_position.vy;
    actor->position.vz = xm_worldmap_camera_follow_target.target.vz = xm_worldmap_camera.target.vz = xm_worldmap_player_position.vz;
    actor->state = 0;
    actor->unk4 = 0;
    actor->unk5C = 0x960000;
    xm_worldmap_camera_angle.vx = -0x20;
    xm_worldmap_camera_angle.vy = 0x400;
    xm_worldmap_camera_angle.vz = 0;
    actor->motion.vx = actor->u.step = -0x20 << 12;
    xm_worldmap_view_center_y = 0x78;
    xm_worldmap_view_kind = 0;
    xm_worldmap_camera_distance = 0x960000;
    actor->motion.vy = actor->unk54 = xm_worldmap_camera_angle.vy << 12;
    actor->motion.vz = actor->unk58 = xm_worldmap_camera_angle.vz << 12;
    return 1;
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80083A00);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene18_lift_start (src/decomp/src/worldmap/worldmap_objects_effects_party.c:247). */
#ifndef XM_S_worldmap_scene18_lift_start
#define XM_S_worldmap_scene18_lift_start
s32 xm_worldmap_scene18_lift_start(s32 index) __asm__("func_80083FE4");
#endif
s32 func_80083FE4(s32 index) {
    WorldmapActor *actor;

    actor = &xm_worldmap_actor_slots[index];
    actor->position.vx = 0x1800000;
    actor->position.vy = 0x80000;
    actor->position.vz = 0x1A00000;
    actor->state = 0;
    xm_worldmap_objects[8].visible = 1;
    xm_worldmap_objects[7].visible = 1;
    xm_worldmap_objects[7].position.vx = xm_worldmap_objects[8].position.vx = actor->position.vx >> 12;
    xm_worldmap_objects[7].position.vy = xm_worldmap_objects[8].position.vy = actor->position.vy >> 12;
    xm_worldmap_objects[7].position.vz = xm_worldmap_objects[8].position.vz = actor->position.vz >> 12;
    return 1;
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_scene18_lift_update (src/decomp/src/worldmap/worldmap_objects_effects_party.c:266). */
#ifndef XM_S_worldmap_scene18_lift_update
#define XM_S_worldmap_scene18_lift_update
s32 xm_worldmap_scene18_lift_update(s32 index) __asm__("func_80084068");
#endif
#ifndef XM_S_worldmap_step_value_toward
#define XM_S_worldmap_step_value_toward
s32 xm_worldmap_step_value_toward(s32 value, s32 target, s32 delta) __asm__("func_800771D8");
#endif
__asm__(".xeno_jt 8 20 jtbl_800704C8");
s32 func_80084068(s32 index) {
    WorldmapActor *actor;
    ActorScratch *scratch;
    SceneObject *objects;
    SceneObject *lift;

    actor = &xm_worldmap_actor_slots[index];
    objects = xm_worldmap_objects;
    lift = &objects[7];
    scratch = (ActorScratch *)0x1F800000;
    switch (actor->unk4) {
    case 1:
        actor->unk4 = 0;
        actor->state = 1;
        objects[8].visible = 0;
        objects[7].visible = 0;
        break;
    case 2:
        actor->unk4 = 0;
        actor->state = 2;
        xm_worldmap_effects_stop_emitters(3);
        xm_worldmap_effects_stop_emitters(5);
        xm_worldmap_effects_stop_emitters(6);
        xm_worldmap_effects_stop_emitters(7);
        xm_worldmap_effects_stop_emitters(10);
        break;
    case 3:
        actor->unk4 = 0;
        actor->state = 3;
        xm_worldmap_effects_stop_emitters(7);
        xm_worldmap_effects_stop_emitters(8);
        xm_worldmap_effects_stop_emitters(9);
        break;
    case 4:
        actor->unk4 = 0;
        actor->state = 4;
        xm_worldmap_effects_stop_emitters(5);
        xm_worldmap_effects_stop_emitters(6);
        xm_worldmap_effects_stop_emitters(7);
        xm_worldmap_effects_stop_emitters(10);
        xm_worldmap_effects_stop_emitters(11);
        xm_worldmap_effects_stop_emitters(12);
        break;
    }
    switch (actor->state) {
    case 0:
        break;
    case 1:
        actor->position.vy = xm_worldmap_step_value_toward(actor->position.vy, -0x180000, -0x400);
        scratch->position.vx = actor->position.vx >> 12;
        scratch->position.vz = actor->position.vz >> 12;
        scratch->position.vy = actor->position.vy >> 12;
        xm_worldmap_effects_start_emitters(3, &scratch->position, NULL);
        xm_worldmap_effects_start_emitters(5, &scratch->position, NULL);
        xm_worldmap_effects_start_emitters(6, &scratch->position, NULL);
        xm_worldmap_effects_start_emitters(7, &scratch->position, NULL);
        xm_worldmap_effects_start_emitters(10, &scratch->position, NULL);
        break;
    case 2:
        actor->position.vy = xm_worldmap_step_value_toward(actor->position.vy, -0x180000, -0x400);
        scratch->position.vx = actor->position.vx >> 12;
        scratch->position.vz = actor->position.vz >> 12;
        scratch->position.vy = actor->position.vy >> 12;
        xm_worldmap_effects_start_emitters(7, &scratch->position, NULL);
        xm_worldmap_effects_start_emitters(8, &scratch->position, NULL);
        xm_worldmap_effects_start_emitters(9, &scratch->position, NULL);
        break;
    case 3:
        actor->position.vy = xm_worldmap_step_value_toward(actor->position.vy, -0x180000, -0x400);
        scratch->position.vx = actor->position.vx >> 12;
        scratch->position.vz = actor->position.vz >> 12;
        scratch->position.vy = actor->position.vy >> 12;
        xm_worldmap_effects_start_emitters(5, &scratch->position, NULL);
        xm_worldmap_effects_start_emitters(6, &scratch->position, NULL);
        xm_worldmap_effects_start_emitters(7, &scratch->position, NULL);
        xm_worldmap_effects_start_emitters(10, &scratch->position, NULL);
        xm_worldmap_effects_start_emitters(11, &scratch->position, NULL);
        xm_worldmap_effects_start_emitters(12, &scratch->position, NULL);
        break;
    case 4:
        actor->position.vy = xm_worldmap_step_value_toward(actor->position.vy, -0x180000, -0x400);
        scratch->position.vx = actor->position.vx >> 12;
        scratch->position.vz = actor->position.vz >> 12;
        scratch->position.vy = actor->position.vy >> 12;
        xm_worldmap_effects_start_emitters(13, &scratch->position, NULL);
        scratch->position.vy = 0;
        xm_worldmap_effects_start_emitters(14, &scratch->position, NULL);
        break;
    }
    lift[0].position.vx = lift[1].position.vx = actor->position.vx >> 12;
    lift[0].position.vy = lift[1].position.vy = actor->position.vy >> 12;
    lift[0].position.vz = lift[1].position.vz = actor->position.vz >> 12;
    return 1;
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008440C);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_objects_build (src/decomp/src/worldmap/worldmap_objects_effects_party.c:421). */
#ifndef XM_T_ScenePlacement
#define XM_T_ScenePlacement
typedef struct {
    u16 def;
    u16 flags;
    s16 x, y, z;
    s16 ax, ay, az;
} ScenePlacement;
#endif
#ifndef XM_T_ModelListEntry
#define XM_T_ModelListEntry
typedef struct {
    s32 unk0;
    u8 *first;
    u8 *second;
} ModelListEntry;
#endif
#ifndef XM_T_ModelList
#define XM_T_ModelList
typedef struct {
    s32 last;
    ModelListEntry entries[1];
} ModelList;
#endif
#ifndef XM_T_Model
#define XM_T_Model
typedef struct {
    u8 unk0[4];
    u8 *table0;
    u8 *table4;
    u8 *table8;
    u8 *primitives;
    u8 unk14[4];
    ModelList *list; 
    u8 unk1C[0x1C];
} Model;
#endif
#ifndef XM_T_ModelGroup
#define XM_T_ModelGroup
typedef struct {
    s32 count;
    s32 flags;       
    u8 unk8[0xC];
    Model models[1];
} ModelGroup;
#endif
#ifndef XM_T_SpriteDefTable
#define XM_T_SpriteDefTable
typedef struct {
    u8 header[0x10];
    SpriteModel defs[1];
} SpriteDefTable;
#endif
#ifndef XM_T_RenderPacket
#define XM_T_RenderPacket
typedef struct RenderPacket {
    u8 unk0[3];
    u8 code;
    s32 value;
} RenderPacket;
#endif
#ifndef XM_S_worldmap_objects_build
#define XM_S_worldmap_objects_build
void xm_worldmap_objects_build(void) __asm__("func_80084580");
#endif
#ifndef XM_S_worldmap_object_model_count
#define XM_S_worldmap_object_model_count
extern s16 xm_worldmap_object_model_count __asm__("D_8009BD28");
#endif
#ifndef XM_S_model_relocate_group
#define XM_S_model_relocate_group
s32 xm_model_relocate_group(ModelGroup *group) __asm__("func_8002C3E8");
#endif
#ifndef XM_S_worldmap_object_count
#define XM_S_worldmap_object_count
extern s16 xm_worldmap_object_count __asm__("D_8009D7E0");
#endif
#ifndef XM_S_SetColorMatrix
#define XM_S_SetColorMatrix
void xm_SetColorMatrix(MATRIX *m) __asm__("SetColorMatrix");
#endif
#ifndef XM_S_SetLightMatrix
#define XM_S_SetLightMatrix
void xm_SetLightMatrix(MATRIX *m) __asm__("SetLightMatrix");
#endif
#ifndef XM_S_model_alloc_packet_buffers
#define XM_S_model_alloc_packet_buffers
void xm_model_alloc_packet_buffers(SpriteModel *def, void **prims, void **prims2, SceneObject *object) __asm__("func_8002CB54");
#endif
#ifndef XM_S_model_build_packets
#define XM_S_model_build_packets
void xm_model_build_packets(SpriteModel *model, RenderPacket *packets, s32 mode) __asm__("func_8002C8CC");
#endif
#ifndef XM_S_model_ot_depth_shift
#define XM_S_model_ot_depth_shift
extern s32 xm_model_ot_depth_shift __asm__("D_80050100");
#endif
#ifndef XM_S_worldmap_walker_face
#define XM_S_worldmap_walker_face
extern s32 xm_worldmap_walker_face __asm__("D_8009C16C");
#endif
#ifndef XM_S_worldmap_walker_object
#define XM_S_worldmap_walker_object
extern s32 xm_worldmap_walker_object __asm__("D_8009C840");
#endif
void func_80084580(void) {
    ScenePlacement *placement;
    s32 *base;
    s32 *offsets;
    s32 i;

    i = 0;
    xm_worldmap_object_model_count = xm_model_relocate_group(xm_worldmap_object_models);
    base = xm_worldmap_object_meshes;
    offsets = base + 1;
    xm_worldmap_object_meshes = offsets;
    for (; i < xm_worldmap_object_model_count; i++) {
        offsets[i] = (s32)base + offsets[i];
    }
    xm_worldmap_object_count = *(s16 *)xm_worldmap_object_placement_list;
    xm_worldmap_objects = xm_heap_alloc(xm_worldmap_object_count * sizeof(SceneObject), 0);
    placement = (ScenePlacement *)((u8 *)xm_worldmap_object_placement_list + 2);
    xm_SetColorMatrix(&xm_worldmap_light_color_matrix);
    xm_SetLightMatrix(&xm_worldmap_light_direction_matrix);
    for (i = 0; i < xm_worldmap_object_count; i++, placement++) {
        xm_worldmap_objects[i].visible = 0;
        xm_worldmap_objects[i].unk2 = placement->def;
        xm_worldmap_objects[i].flags = placement->flags;
        xm_worldmap_objects[i].position.vx = placement->x;
        xm_worldmap_objects[i].position.vy = placement->y;
        xm_worldmap_objects[i].position.vz = -placement->z;
        xm_worldmap_objects[i].angle.vx = placement->ax;
        xm_worldmap_objects[i].angle.vy = placement->ay;
        xm_worldmap_objects[i].angle.vz = placement->az;
        xm_RotMatrixYXZ(&xm_worldmap_objects[i].angle, &xm_worldmap_objects[i].matrix);
        xm_worldmap_objects[i].def = &((SpriteDefTable *)xm_worldmap_object_models)->defs[xm_worldmap_objects[i].unk2];
        xm_model_alloc_packet_buffers(xm_worldmap_objects[i].def, &xm_worldmap_objects[i].prims, &xm_worldmap_objects[i].prims2, &xm_worldmap_objects[i]);
        xm_model_build_packets(xm_worldmap_objects[i].def, xm_worldmap_objects[i].prims, 1);
        xm_memcpy(xm_worldmap_objects[i].prims2, xm_worldmap_objects[i].prims, xm_worldmap_objects[i].def->packet_size);
        xm_worldmap_objects[i].unk44 = ((s32 *)xm_worldmap_object_meshes)[xm_worldmap_objects[i].unk2];
        ((s32 *)xm_worldmap_objects[i].unk44)[1] = xm_worldmap_objects[i].unk44 + ((s32 *)xm_worldmap_objects[i].unk44)[1];
        xm_worldmap_objects[i].parent = NULL;
    }
    xm_model_ot_depth_shift = 2;
    xm_worldmap_walker_face = -1;
    xm_worldmap_walker_object = -1;
    xm_worldmap_objects[4].visible = 1;
}
#endif


#ifndef XENO_PC_PORT
void func_80084818(void) {
    s32 temp_a0;
    s32 var_s0;
    s32 var_s1;
    u8 pad[4];

    var_s1 = 0;
    if (D_8009D7E0 > 0) {
        var_s0 = 0;
        do {
            var_s1 += 1;
            HeapFree(*(s32 *)((s8*)((var_s0 + (uintptr_t)D_8009C620)) + 0x48));
            temp_a0 = *(s32 *)((s8*)((var_s0 + (uintptr_t)D_8009C620)) + 0x40);
            var_s0 += 0x54;
            func_8002CBBC(temp_a0);
        } while (var_s1 < D_8009D7E0);
    }
    HeapFree((s32)D_8009C620);
}
#endif



void func_800848B4(s32 arg0, s32 arg1) {
    *(s32 *)((s8*)(((arg1 * 0x54) + (uintptr_t)D_8009C620)) + 0x50) = (s32) WM_GADDR(D_8009C620 + (arg0 * 0x54));
}


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800848F4);


#ifndef XENO_PC_PORT
s16 func_80084D00(s32 arg0, s16 *arg1) {
    s16 temp_v0;
    s16 temp_v0_2;
    s16 var_s0;
    void *var_s1;

    var_s1 = ((void *)D_8009C620);
    var_s0 = 0;
    if (D_8009D7E0 > 0) {
loop_1:
        if (*(u16 *)((s8*)(var_s1) + 4) & 1) {
            temp_v0 = func_80084DB8(arg0, var_s0);
            if (temp_v0 != 0) {
                *arg1 = var_s0;
                return temp_v0;
            }
        }
        temp_v0_2 = var_s0 + 1;
        var_s0 = temp_v0_2;
        var_s1 += 0x54;
        if (temp_v0_2 >= D_8009D7E0) {
            goto block_5;
        }
        goto loop_1;
    }
block_5:
    return 0;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80084DB8);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_mesh_project_onto_face (src/decomp/src/worldmap/worldmap_objects_effects_party.c:682). */
#ifndef XM_T_MeshFace
#define XM_T_MeshFace
typedef struct {
    s16 corner[3];
    s16 next[3]; 
    u16 kind;    
} MeshFace;
#endif
#ifndef XM_T_FaceScratch
#define XM_T_FaceScratch
typedef struct {
    VECTOR p[3];      
    VECTOR normal;    
    VECTOR side;      
    u8 pad50[0xA0];
    MATRIX m;         
    MATRIX probe;     
} FaceScratch;
#endif
#ifndef XM_T_FACE_SCRATCH
#define XM_T_FACE_SCRATCH
#define FACE_SCRATCH ((FaceScratch *)0x1F800000)
#endif
#ifndef XM_T_Mesh
#define XM_T_Mesh
typedef struct {
    s32 unk0;
    SVECTOR *vertices;
    MeshFace faces[1];
} Mesh;
#endif
#ifndef XM_S_worldmap_mesh_project_onto_face
#define XM_S_worldmap_mesh_project_onto_face
void xm_worldmap_mesh_project_onto_face(VECTOR *position, VECTOR *offset, VECTOR *normal, u16 index, u16 face) __asm__("func_80085158");
#endif
#ifndef XM_S_RotTrans
#define XM_S_RotTrans
void xm_RotTrans(SVECTOR *v0, VECTOR *v1, long *flag) __asm__("RotTrans");
#endif
#ifndef XM_S_OuterProduct0
#define XM_S_OuterProduct0
void xm_OuterProduct0(VECTOR *v0, VECTOR *v1, VECTOR *v2) __asm__("OuterProduct0");
#endif
#ifndef XM_S_VectorNormal
#define XM_S_VectorNormal
long xm_VectorNormal(VECTOR *v0, VECTOR *v1) __asm__("VectorNormal");
#endif
#ifndef XM_S_worldmap_set_height_on_plane
#define XM_S_worldmap_set_height_on_plane
void xm_worldmap_set_height_on_plane(VECTOR *point, VECTOR *origin, VECTOR *normal) __asm__("func_800935DC");
#endif
void func_80085158(VECTOR *position, VECTOR *offset, VECTOR *normal, u16 index, u16 face) {
    long flag;
    VECTOR *edge1;
    VECTOR *edge2;
    SceneObject *object;
    MeshFace *corners;
    SVECTOR *vertices;
    s32 y;

    object = &xm_worldmap_objects[index];
    offset->vx = (position->vx >> 12) - object->position.vx;
    offset->vz = object->position.vz - (position->vz >> 12);
    FACE_SCRATCH->m = object->matrix;
    FACE_SCRATCH->m.t[2] = 0;
    FACE_SCRATCH->m.t[0] = 0;
    y = object->position.vy;
    FACE_SCRATCH->p[1].vz = 0x800;
    FACE_SCRATCH->p[1].vy = 0x800;
    FACE_SCRATCH->p[1].vx = 0x800;
    FACE_SCRATCH->m.t[1] = y;
    edge1 = &FACE_SCRATCH->p[1];
    xm_ScaleMatrix(&FACE_SCRATCH->m, edge1);
    xm_SetRotMatrix(&FACE_SCRATCH->m);
    xm_SetTransMatrix(&FACE_SCRATCH->m);
    edge2 = &FACE_SCRATCH->p[2];
    corners = ((Mesh *)object->unk44)->faces;
    corners += face;
    vertices = ((Mesh *)object->unk44)->vertices;
    xm_RotTrans(&vertices[corners->corner[0]], &FACE_SCRATCH->p[0], &flag);
    xm_RotTrans(&vertices[corners->corner[1]], &FACE_SCRATCH->p[1], &flag);
    xm_RotTrans(&vertices[corners->corner[2]], &FACE_SCRATCH->p[2], &flag);
    edge1->vx -= FACE_SCRATCH->p[0].vx;
    edge1->vy -= FACE_SCRATCH->p[0].vy;
    edge1->vz -= FACE_SCRATCH->p[0].vz;
    edge2->vx -= FACE_SCRATCH->p[0].vx;
    edge2->vy -= FACE_SCRATCH->p[0].vy;
    edge2->vz -= FACE_SCRATCH->p[0].vz;
    xm_OuterProduct0(&FACE_SCRATCH->p[2], &FACE_SCRATCH->p[1], &FACE_SCRATCH->p[2]);
    edge2->vx >>= 2;
    edge2->vy >>= 2;
    edge2->vz >>= 2;
    xm_VectorNormal(&FACE_SCRATCH->p[2], normal);
    xm_worldmap_set_height_on_plane(offset, &FACE_SCRATCH->p[0], normal);
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_mesh_is_face_plane_crossed (src/decomp/src/worldmap/worldmap_objects_effects_party.c:729). */
#ifndef XM_S_worldmap_mesh_is_face_plane_crossed
#define XM_S_worldmap_mesh_is_face_plane_crossed
s32 xm_worldmap_mesh_is_face_plane_crossed(VECTOR *position, s32 height, u16 index, u16 face) __asm__("func_80085418");
#endif
#ifndef XM_S_ApplyMatrixLV
#define XM_S_ApplyMatrixLV
VECTOR *xm_ApplyMatrixLV(MATRIX *m, VECTOR *v0, VECTOR *v1) __asm__("ApplyMatrixLV");
#endif
s32 func_80085418(VECTOR *position, s32 height, u16 index, u16 face) {
    long flag;
    SceneObject *object;
    MeshFace *corners;
    SVECTOR *vertices;
    VECTOR *origin;
    VECTOR *edge1;
    VECTOR *edge2;
    s32 depth;
    s32 y;
    s16 z;

    object = &xm_worldmap_objects[index];
    FACE_SCRATCH->m = object->matrix;
    FACE_SCRATCH->m.t[2] = 0;
    FACE_SCRATCH->m.t[0] = 0;
    FACE_SCRATCH->m.t[1] = object->position.vy;
    FACE_SCRATCH->p[1].vz = 0x800;
    FACE_SCRATCH->p[1].vy = 0x800;
    FACE_SCRATCH->p[1].vx = 0x800;
    edge1 = &FACE_SCRATCH->p[1];
    xm_ScaleMatrix(&FACE_SCRATCH->m, edge1);
    xm_SetRotMatrix(&FACE_SCRATCH->m);
    xm_SetTransMatrix(&FACE_SCRATCH->m);
    edge2 = &FACE_SCRATCH->p[2];
    corners = ((Mesh *)object->unk44)->faces;
    corners += face;
    vertices = ((Mesh *)object->unk44)->vertices;
    xm_RotTrans(&vertices[corners->corner[0]], &FACE_SCRATCH->p[0], &flag);
    xm_RotTrans(&vertices[corners->corner[1]], &FACE_SCRATCH->p[1], &flag);
    xm_RotTrans(&vertices[corners->corner[2]], &FACE_SCRATCH->p[2], &flag);
    origin = &FACE_SCRATCH->p[0];
    edge1->vx -= origin->vx;
    edge1->vy -= origin->vy;
    edge1->vz -= origin->vz;
    edge2->vx -= origin->vx;
    edge2->vy -= origin->vy;
    edge2->vz -= origin->vz;
    xm_OuterProduct0(&FACE_SCRATCH->p[2], &FACE_SCRATCH->p[1], &FACE_SCRATCH->p[2]);
    edge2->vx >>= 2;
    edge2->vy >>= 2;
    edge2->vz >>= 2;
    xm_VectorNormal(&FACE_SCRATCH->p[2], &FACE_SCRATCH->normal);
    FACE_SCRATCH->probe.m[1][0] = FACE_SCRATCH->probe.m[0][0] =
        (position->vx >> 12) - object->position.vx - origin->vx;
    y = (position->vy >> 12) - origin->vy;
    FACE_SCRATCH->probe.m[0][1] = y;
    z = object->position.vz;
    depth = position->vz >> 12;
    FACE_SCRATCH->probe.m[1][1] = y - height;
    z -= depth;
    z -= origin->vz;
    FACE_SCRATCH->probe.m[1][2] = FACE_SCRATCH->probe.m[0][2] = z;
    xm_ApplyMatrixLV(&FACE_SCRATCH->probe, &FACE_SCRATCH->normal, &FACE_SCRATCH->side);
    return (FACE_SCRATCH->side.vx ^ FACE_SCRATCH->side.vy) >> 31;
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80085760);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_actor_draw_sprites (src/decomp/src/worldmap/worldmap_objects_effects_party.c:908). */
#ifndef XM_T_DepthScratch
#define XM_T_DepthScratch
typedef struct {
    SVECTOR vertex;
    VECTOR offset;
    s32 depth[64];
} DepthScratch;
#endif
#ifndef XM_T_DEPTH_SCRATCH
#define XM_T_DEPTH_SCRATCH
#define DEPTH_SCRATCH ((DepthScratch *)0x1F800000)
#endif
#ifndef XM_S_sprite_set_view_matrix
#define XM_S_sprite_set_view_matrix
void xm_sprite_set_view_matrix(MATRIX *view) __asm__("func_80024FF4");
#endif
#ifndef XM_S_sprite_draw
#define XM_S_sprite_draw
void xm_sprite_draw(Sprite *sprite, u_long *ot) __asm__("func_8001E298");
#endif
#ifndef XM_S_sprite_set_facing
#define XM_S_sprite_set_facing
void xm_sprite_set_facing(Sprite *sprite, s32 angle) __asm__("func_800223B0");
#endif
#ifndef XM_S_sprite_vm_tick
#define XM_S_sprite_vm_tick
void xm_sprite_vm_tick(Sprite *sprite) __asm__("AnimScriptTick");
#endif
void func_80085CDC(void) {
    WorldmapActor *actor;
    Sprite *model;
    s32 i;
    s32 offset;
    s32 diff;
    s32 facing;
    s32 heading;
    DepthScratch *scratch;

    scratch = DEPTH_SCRATCH;
    actor = xm_worldmap_actor_slots;
    for (i = 0; i < 0x40; i++, actor++) {
        if ((actor->unk24 == 0) & (actor->handle != NULL)) {
            scratch->offset.vx = actor->position.vx - xm_worldmap_camera.target.vx;
            scratch->offset.vz = actor->position.vz - xm_worldmap_camera.target.vz;
            xm_worldmap_wrap_offset(&scratch->offset);
            actor->handle->x = scratch->offset.vx * 16;
            actor->handle->z = -scratch->offset.vz * 16;
            actor->handle->y = actor->position.vy * 16;
        }
    }
    xm_SetRotMatrix(&xm_worldmap_camera_matrix);
    xm_SetTransMatrix(&xm_worldmap_camera_matrix);
    actor = xm_worldmap_actor_slots;
    for (i = 0; i < 0x40; i++, actor++) {
        model = actor->handle;
        if ((actor->unk24 == 0) & (model != NULL)) {
            scratch->vertex.vx = model->x >> 16;
            scratch->vertex.vy = actor->handle->y >> 16;
            scratch->vertex.vz = actor->handle->z >> 16;
            gte_ldv0(&scratch->vertex);
            gte_rtps();
            gte_stsz(&scratch->depth[i]);
        }
    }
    xm_sprite_set_view_matrix(&xm_worldmap_camera_matrix);
    actor = xm_worldmap_actor_slots;
    for (i = 0; i < 0x40; i++, actor++) {
        if ((actor->unk24 == 0) & (actor->handle != NULL) & (scratch->depth[i] < 0xB00)) {
            xm_sprite_draw(actor->handle, &xm_worldmap_current_display_buffer->ot[scratch->depth[i] >> 4]);
            heading = actor->heading;
            facing = actor->unk5C;
            diff = heading - facing;
            if (diff < 0) {
                diff += 0x1000;
            }
            
            if (diff < 0x801) {
                if (diff < 0x100) {
                    diff = heading;
                } else {
                    diff = facing + 0x100;
                }
            } else {
                diff -= 0x1000;
                if (diff >= -0xFF) {
                    diff = heading;
                } else {
                    diff = facing - 0x100;
                }
            }
            actor->unk5C = diff;
            xm_sprite_set_facing(actor->handle, (diff - (u16)xm_worldmap_camera_angle.vy - 0x400) & 0xFFF);
            xm_sprite_vm_tick(actor->handle);
        }
    }
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_billboards_resolve_lists (src/decomp/src/worldmap/worldmap_objects_effects_party.c:978). */
#ifndef XM_S_worldmap_billboards_resolve_lists
#define XM_S_worldmap_billboards_resolve_lists
void xm_worldmap_billboards_resolve_lists(void) __asm__("func_80085F58");
#endif
#ifndef XM_S_worldmap_billboard_cluts
#define XM_S_worldmap_billboard_cluts
extern u16 xm_worldmap_billboard_cluts[16] __asm__("D_8009D478");
#endif
void func_80085F58(void) {
    BillboardList *texture;
    s32 i;

    texture = xm_worldmap_billboard_lists;
    for (i = 0; i < 0x100; i++, texture++) {
        if (texture->data != NULL) {
            texture->data += (s32)xm_worldmap_billboard_lists;
        }
    }
    for (i = 0; i < 0x10; i++) {
        xm_worldmap_billboard_cluts[i] = xm_GetClut(0xF0, i + 0x1F0);
    }
}
#endif
INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80085FE0);


#ifndef XENO_PC_PORT
void func_80086124(void) {
    HeapFree(D_8009D7EC);
    HeapFree(D_8009D7E8);
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008615C);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_clouds_scatter (src/decomp/src/worldmap/worldmap_objects_effects_party.c:1086). */
#ifndef XM_T_Drift
#define XM_T_Drift
typedef struct Drift {
    s32 x;
    s32 unk4;
    s32 z;
    s32 unkC;
} Drift;
#endif
#ifndef XM_T_DriftVelocity
#define XM_T_DriftVelocity
typedef struct DriftVelocity {
    s16 dx;
    s16 unk2;
    s16 dz;
    s16 unk6;
} DriftVelocity;
#endif
#ifndef XM_S_worldmap_clouds_scatter
#define XM_S_worldmap_clouds_scatter
void xm_worldmap_clouds_scatter(void) __asm__("func_800863E0");
#endif
#ifndef XM_S_worldmap_cloud_positions
#define XM_S_worldmap_cloud_positions
extern Drift *xm_worldmap_cloud_positions __asm__("D_8009D150");
#endif
#ifndef XM_S_worldmap_cloud_velocities
#define XM_S_worldmap_cloud_velocities
extern DriftVelocity *xm_worldmap_cloud_velocities __asm__("D_8009CEB4");
#endif
#ifndef XM_S_worldmap_cloud_template_points
#define XM_S_worldmap_cloud_template_points
extern Drift xm_worldmap_cloud_template_points[5] __asm__("D_8009AF30");
#endif
void func_800863E0(void) {
    Drift *drift;
    DriftVelocity *velocity;
    s32 row;
    s32 column;
    s32 i;

    xm_worldmap_cloud_positions = xm_heap_alloc(0x50 * sizeof(Drift), 0);
    xm_worldmap_cloud_velocities = xm_heap_alloc(0x50 * sizeof(DriftVelocity), 0);
    drift = xm_worldmap_cloud_positions;
    for (row = 0; row < 4; row++) {
        for (column = 0; column < 4; column++) {
            for (i = 0; i < 5; i++, drift++) {
                drift->x = (xm_worldmap_cloud_template_points[i].x + (column << 11)) << 12;
                drift->z = (xm_worldmap_cloud_template_points[i].z + (row << 11)) << 12;
                drift->unk4 = (-(xm_rand() >> 10) * 8 - 0x200) << 12;
            }
        }
    }
    velocity = xm_worldmap_cloud_velocities;
    for (i = 0; i < 0x50; i++) {
        column = (xm_rand() & 3) + 1; 
        velocity->dx = column * 0xDDB;
        velocity->dz = -(column << 11);
        velocity->unk2 = xm_rand() & 1;
        velocity++;
    }
}
#endif


#ifndef XENO_PC_PORT
void func_80086568(void) {
    HeapFree(D_8009CEB4);
    HeapFree(D_8009D150);
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800865A0);


#ifndef XENO_PC_PORT
void func_800866C8(void) {
    HeapFree(D_8009D7FC);
    HeapFree(D_8009D7F8);
}
#endif


#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_clouds_move (src/decomp/src/worldmap/worldmap_objects_effects_party.c:1160). */
#ifndef XM_S_worldmap_clouds_move
#define XM_S_worldmap_clouds_move
void xm_worldmap_clouds_move(void) __asm__("func_80086700");
#endif
void func_80086700(void) {
    Drift *drift;
    DriftVelocity *velocity;
    s32 x;
    s32 z;
    s32 i;

    for (i = 0; i < 0x50; i++) {
        drift = &xm_worldmap_cloud_positions[i];
        velocity = &xm_worldmap_cloud_velocities[i];
        x = drift->x + velocity->dx;
        z = drift->z + velocity->dz;
        if (x > 0x1FFFFFF) {
            x -= 0x2000000;
        }
        if (x < 0) {
            x += 0x2000000;
        }
        if (z > 0x1FFFFFF) {
            z -= 0x2000000;
        }
        if (z < 0) {
            z += 0x2000000;
        }
        drift->x = x;
        drift->z = z;
    }
}
#endif
#else
/* FAKEMATCH: natural semantic reconstruction for port execution only. It is
 * not retail-byte-matched; the retail build continues to use the pinned asm. */
void func_80086700(void) {
    s32 i;
    s32 *pos;
    s16 *delta;
    s32 x;
    s32 z;

    i = 0;
    pos = (s32 *)(intptr_t)WM_GUEST_PTR(0x8009D150);
    delta = (s16 *)(intptr_t)WM_GUEST_PTR(0x8009CEB4);
    do {
        x = pos[0] + delta[0];
        z = pos[2] + delta[2];
        if (x > 0x1FFFFFF) x -= 0x2000000;
        if (x < 0) x += 0x2000000;
        if (z > 0x1FFFFFF) z -= 0x2000000;
        if (z < 0) z += 0x2000000;
        pos[0] = x;
        pos[2] = z;
        pos += 4;
        delta += 4;
        i++;
    } while (i < 0x50);
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80086798);


s32 func_80087710(s32 arg0) {
    void *temp_v0;

    temp_v0 = (u8 *)D_8009BE24 + (arg0 << 7);
    *(s32 *)((s8*)(temp_v0) + 0x50) = 0;
    *(s32 *)((s8*)(temp_v0) + 0x54) = 8;
    return 1;
}



#ifndef XENO_PC_PORT
s32 func_80087734(s32 arg0) {
    u16 temp_v0;
    void *temp_a1;
    void *temp_s0;

    temp_a1 = (u8 *)D_8009BE24 + (arg0 << 7);
    temp_s0 = ((void *)D_8009C620);
    *(s32 *)((s8*)(temp_a1) + 0x50) = (s32) ((*(s32 *)((s8*)(temp_a1) + 0x50) + *(s32 *)((s8*)(temp_a1) + 0x54)) & 0xFFF);
    *(s16 *)0x1F8000A8 = 0;
    *(s16 *)0x1F8000A0 = 0;
    *(u16 *)0x1F8000A2 = *(u16 *)((s8*)(temp_s0) + 0x40A);
    *(u16 *)0x1F8000AA = *(u16 *)((s8*)(temp_s0) + 0x4B2);
    temp_v0 = (u16) *(s32 *)((s8*)(temp_a1) + 0x50);
    *(u16 *)0x1F8000AC = temp_v0;
    *(u16 *)0x1F8000A4 = temp_v0;
    RotMatrix(0x1F8000A0, temp_s0 + 0x410);
    RotMatrix(0x1F8000A8, temp_s0 + 0x4B8);
    return 1;
}
#endif



s32 func_800877E0(s32 arg0) {
    void *temp_v0;

    temp_v0 = (u8 *)D_8009BE24 + (arg0 << 7);
    *(s32 *)((s8*)(temp_v0) + 0x50) = 0;
    *(s32 *)((s8*)(temp_v0) + 0x54) = 8;
    return 1;
}



s32 func_80087804(s32 arg0) {
    u8 pad[8];
    void *obj;
    u8 *base;
    s32 idxA;
    s32 idxB;
    void *a;
    void *b;
    u16 angle;

    obj = (u8 *)D_8009BE24 + (arg0 << 7);
    base = D_8009C620;
    idxA = D_8009B624[D_8009C610 * 2];
    idxB = D_8009B626[D_8009C610 * 2];
    *(s32 *)((s8*)(obj) + 0x50) = (*(s32 *)((s8*)(obj) + 0x50) + *(s32 *)((s8*)(obj) + 0x54)) & 0xFFF;
    *(s16 *)WM_SPAD(0x1F8000A8) = 0;
    *(s16 *)WM_SPAD(0x1F8000A0) = 0;
    a = base + idxA * 0x54;
    *(u16 *)WM_SPAD(0x1F8000A2) = *(u16 *)((s8*)(a) + 0x1A);
    b = base + idxB * 0x54;
    *(u16 *)WM_SPAD(0x1F8000AA) = *(u16 *)((s8*)(b) + 0x1A);
    angle = *(u16 *)((s8*)(obj) + 0x50);
    *(u16 *)WM_SPAD(0x1F8000AC) = angle;
    *(u16 *)WM_SPAD(0x1F8000A4) = angle;
    RotMatrix(WM_SPAD(0x1F8000A0), a + 0x20);
    RotMatrix(WM_SPAD(0x1F8000A8), b + 0x20);
    return 1;
}


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_rolling_pair_make_translucent (src/decomp/src/worldmap/worldmap_objects_effects_party.c:1582). */
#ifndef XM_S_worldmap_rolling_pair_make_translucent
#define XM_S_worldmap_rolling_pair_make_translucent
void xm_worldmap_rolling_pair_make_translucent(SceneObject *object, POLY_FT4 *quads, s32 count, s32 abr) __asm__("func_80087904");
#endif
void xm_worldmap_rolling_pair_make_translucent(SceneObject *object, POLY_FT4 *quads, s32 count, s32 abr) __asm__("func_80087904");
void xm_worldmap_rolling_pair_make_translucent(SceneObject *object, POLY_FT4 *quads, s32 count, s32 abr) {
    s32 i;

    for (i = 0; i < count; i++) {
        quads->tpage = xm_GetTPage(0, abr, 0x1A0, 0xA0);
        setSemiTrans(quads, 1);
        quads++;
    }
    xm_memcpy(object->prims2, object->prims, count * sizeof(POLY_FT4));
}
#endif


s32 func_800879A8(s32 arg0) {
    void *temp_v0;

    temp_v0 = (u8 *)D_8009BE24 + (arg0 << 7);
    *(s32 *)((s8*)(temp_v0) + 0x50) = 0;
    *(s32 *)((s8*)(temp_v0) + 0x54) = 0x10;
    func_800879E0(arg0);
    return 1;
}



s32 func_800879E0(s32 arg0) {
    s32 temp_v1;
    void *temp_a0;
    void *temp_s0;
    u8 pad[4];

    temp_v1 = D_8009C610 * 2;
    temp_a0 = D_8009C620 + (D_8009B64C[temp_v1] * 0x54);
    temp_s0 = D_8009C620 + (D_8009B64E[temp_v1] * 0x54);
    func_80087904(temp_a0, *(s32 *)((s8*)(temp_a0) + 0x48), *(u16 *)((s8*)(WM_GPTR(*(u32 *)((s8*)(temp_a0) + 0x40))) + 4), 3);
    func_80087904(temp_s0, *(s32 *)((s8*)(temp_s0) + 0x48), *(u16 *)((s8*)(WM_GPTR(*(u32 *)((s8*)(temp_s0) + 0x40))) + 4), 3);
    return 1;
}



s32 func_80087A8C(s32 arg0) {
    u8 pad[8];
    void *obj;
    u8 *base;
    s32 idxA;
    s32 idxB;
    u16 angle;
    void *b;

    obj = (u8 *)D_8009BE24 + (arg0 << 7);
    base = D_8009C620;
    idxA = D_8009B64C[D_8009C610 * 2];
    idxB = D_8009B64E[D_8009C610 * 2];
    *(s32 *)((s8*)(obj) + 0x50) = (*(s32 *)((s8*)(obj) + 0x50) + *(s32 *)((s8*)(obj) + 0x54)) & 0xFFF;
    *(s16 *)WM_SPAD(0x1F8000AC) = 0;
    *(s16 *)WM_SPAD(0x1F8000A4) = 0;
    *(s16 *)WM_SPAD(0x1F8000A8) = 0;
    *(s16 *)WM_SPAD(0x1F8000A0) = 0;
    angle = *(u16 *)((s8*)(obj) + 0x50);
    *(u16 *)WM_SPAD(0x1F8000AA) = angle;
    *(u16 *)WM_SPAD(0x1F8000A2) = angle;
    b = base + idxB * 0x54;
    RotMatrix(WM_SPAD(0x1F8000A0), base + idxA * 0x54 + 0x20);
    RotMatrix(WM_SPAD(0x1F8000A8), b + 0x20);
    return 1;
}



void func_80087B84(void *arg0, void *arg1, void *arg2) {
    *(s32 *)((s8*)(arg1) + 8) = 0;
    *(s32 *)((s8*)(arg1) + 0) = 0;
    *(s32 *)((s8*)(arg1) + 4) = 0x1000;
    OuterProduct12(arg1, arg0, arg1);
    VectorNormal(arg1, arg1);
    *(s16 *)((s8*)(arg2) + 0) = (s16) *(s32 *)((s8*)(arg1) + 0);
    *(s16 *)((s8*)(arg2) + 2) = (s16) *(s32 *)((s8*)(arg1) + 4);
    *(s16 *)((s8*)(arg2) + 4) = (s16) *(s32 *)((s8*)(arg1) + 8);
    OuterProduct12(arg0, arg1, arg1);
    VectorNormal(arg1, arg1);
    *(s16 *)((s8*)(arg2) + 6) = (s16) *(s32 *)((s8*)(arg1) + 0);
    *(s16 *)((s8*)(arg2) + 8) = (s16) *(s32 *)((s8*)(arg1) + 4);
    *(s16 *)((s8*)(arg2) + 0xA) = (s16) *(s32 *)((s8*)(arg1) + 8);
    *(s16 *)((s8*)(arg2) + 0xC) = (s16) *(s32 *)((s8*)(arg0) + 0);
    *(s16 *)((s8*)(arg2) + 0xE) = (s16) *(s32 *)((s8*)(arg0) + 4);
    *(s16 *)((s8*)(arg2) + 0x10) = (s16) *(s32 *)((s8*)(arg0) + 8);
    TransposeMatrix(arg2, arg2);
}



s32 func_80087C6C(s32 arg0) {
    WmVec *sp;
    WmObj *obj;
    u8 *rec;
    s32 recIdx;
    s32 i;

    recIdx = D_8009B674[D_8009C610];
    func_80087F60(arg0);
    obj = &D_8009BE24[arg0];
    sp = (WmVec *)WM_SPAD(0x1F800000);
    rec = D_8009C620 + recIdx * 0x54;
    *(s16 *)((s8*)(obj) + 0x4A) = 0x4000;
    *(s16 *)((s8*)(obj) + 0x20) = 0;
    if ((*(u16 *)((u8 *)&g_GameState + (0x1930))) < 0xCD) {
        *(s32 *)((s8*)(obj) + 0x28) = 0xD80000;
        *(s32 *)((s8*)(obj) + 0x30) = 0x07280000;
        *(WmMat *)(rec + 0x20) = D_8009A180;
        *(s32 *)((s8*)(obj) + 0x2C) = func_80093978(*(s32 *)((s8*)(obj) + 0x28), *(s32 *)((s8*)(obj) + 0x30));
    } else {
        if ((*(u16 *)((u8 *)&g_GameState + (0x184A))) == 0) {
            (*(u16 *)((u8 *)&g_GameState + (0x184A)))++;
            *(s32 *)((s8*)(obj) + 0x50) = 0;
            *(s32 *)((s8*)(obj) + 0x28) = D_8009AF80[0] << 0xC;
            *(s32 *)((s8*)(obj) + 0x30) = D_8009AF90[*(s32 *)((s8*)(obj) + 0x50)] << 0xC;
            *(s32 *)((s8*)(obj) + 0x2C) = func_80093978(*(s32 *)((s8*)(obj) + 0x28), *(s32 *)((s8*)(obj) + 0x30));
        } else {
            *(s32 *)((s8*)(obj) + 0x50) = (*(u16 *)((u8 *)&g_GameState + (0x1848)));
            *(s32 *)((s8*)(obj) + 0x28) = (*(u16 *)((u8 *)&g_GameState + (0x1844))) << 0xC;
            *(s32 *)((s8*)(obj) + 0x30) = (*(u16 *)((u8 *)&g_GameState + (0x1846))) << 0xC;
            *(s32 *)((s8*)(obj) + 0x2C) = func_80093978(*(s32 *)((s8*)(obj) + 0x28), *(s32 *)((s8*)(obj) + 0x30));
        }
        sp->vx = D_8009AF80[*(s32 *)((s8*)(obj) + 0x50)] << 0xC;
        sp->vz = D_8009AF90[*(s32 *)((s8*)(obj) + 0x50)] << 0xC;
        if (abs(func_80094154((u8 *)obj + 0x28, sp)) < 8) {
            *(s32 *)((s8*)(obj) + 0x50) = (*(s32 *)((s8*)(obj) + 0x50) + 1) & 7;
        }
        sp->vx = D_8009AF80[*(s32 *)((s8*)(obj) + 0x50)] - (*(s32 *)((s8*)(obj) + 0x28) >> 0xC);
        sp->vz = D_8009AF90[*(s32 *)((s8*)(obj) + 0x50)] - (*(s32 *)((s8*)(obj) + 0x30) >> 0xC);
        sp->vy = 0;
        func_80093534(sp);
        VectorNormal(sp, sp);
        for (i = 0; i < 32; i++) {
            D_8009CD68[i].vx = sp->vx;
            D_8009CD6C[i].vx = sp->vz;
        }
        *(s32 *)((s8*)(obj) + 0x58) = 1;
        *(s32 *)((s8*)(obj) + 0x54) = 0;
        *(s32 *)((s8*)(obj) + 0x38) = D_8009CD70[0];
        *(s32 *)((s8*)(obj) + 0x40) = D_8009CD6C[*(s32 *)((s8*)(obj) + 0x58)].vx;
        *(s32 *)((s8*)(obj) + 0x3C) = 0;
    }
    return 1;
}



s32 func_80087F60(s32 arg0) {
    u16 temp_s0;

    temp_s0 = D_8009B674[D_8009C610];
    func_800848B4(temp_s0, temp_s0 - 4);
    func_800848B4(temp_s0, temp_s0 - 3);
    func_800848B4(temp_s0, temp_s0 - 2);
    func_800848B4(temp_s0, temp_s0 - 1);
    return 1;
}



s32 func_80087FD0(s32 arg0) {
    WmVec *sp;
    u8 *obj;
    u8 *rec;
    s32 d;

    sp = (WmVec *)WM_SPAD(0x1F800000);
    obj = (u8 *)D_8009BE24 + (arg0 << 7);
    rec = D_8009C620 + D_8009B674[D_8009C610] * 0x54;
    if (*(u16 *)((u8 *)&g_GameState + 0x1930) < 0xCD) {
        *(s32 *)((s8*)(obj) + 0x2C) = func_80093978(*(s32 *)((s8*)(obj) + 0x28), *(s32 *)((s8*)(obj) + 0x30));
        *(s32 *)((s8*)(rec) + 8) = *(s32 *)((s8*)(obj) + 0x28) >> 0xC;
        *(s32 *)((s8*)(rec) + 0xC) = *(s32 *)((s8*)(obj) + 0x2C) >> 0xC;
        *(s32 *)((s8*)(rec) + 0x10) = *(s32 *)((s8*)(obj) + 0x30) >> 0xC;
    } else {
        sp->vx = D_8009AF80[*(s32 *)((s8*)(obj) + 0x50)] << 0xC;
        sp->vz = D_8009AF90[*(s32 *)((s8*)(obj) + 0x50)] << 0xC;
        if (abs(func_80094154(obj + 0x28, sp)) < 8) {
            *(s32 *)((s8*)(obj) + 0x50) = (*(s32 *)((s8*)(obj) + 0x50) + 1) & 7;
        }
        sp->vx = D_8009AF80[*(s32 *)((s8*)(obj) + 0x50)] - (*(s32 *)((s8*)(obj) + 0x28) >> 0xC);
        sp->vz = D_8009AF90[*(s32 *)((s8*)(obj) + 0x50)] - (*(s32 *)((s8*)(obj) + 0x30) >> 0xC);
        sp->vy = 0;
        func_80093534(sp);
        VectorNormal(sp, sp);
        *(s32 *)((s8*)(obj) + 0x38) = ((sp->vx + *(s32 *)((s8*)(obj) + 0x38) * 63) << 6) >> 0xC;
        *(s32 *)((s8*)(obj) + 0x40) = ((sp->vz + *(s32 *)((s8*)(obj) + 0x40) * 63) << 6) >> 0xC;
        VectorNormal(obj + 0x38, obj + 0x38);
        D_8009CD68[*(s32 *)((s8*)(obj) + 0x54)].vx = *(s32 *)((s8*)(obj) + 0x38);
        D_8009CD68[*(s32 *)((s8*)(obj) + 0x54)].vz = *(s32 *)((s8*)(obj) + 0x40);
        *(s32 *)((s8*)(obj) + 0x54) = (*(s32 *)((s8*)(obj) + 0x54) + 1) & 0x1F;
        sp->vx = *(s32 *)((s8*)(obj) + 0x38);
        sp->vy = *(s32 *)((s8*)(obj) + 0x3C);
        sp->vz = -*(s32 *)((s8*)(obj) + 0x40);
        func_80087B84(sp, WM_SPAD(0x1F800010), WM_SPAD(0x1F8000F0));
        *(WmMat *)(rec + 0x20) = *(WmMat *)WM_SPAD(0x1F8000F0);
        *(s32 *)((s8*)(obj) + 0x28) += D_8009CD68[*(s32 *)((s8*)(obj) + 0x58)].vx * (*(s16 *)((s8*)(obj) + 0x4A) >> 12);
        *(s32 *)((s8*)(obj) + 0x30) += D_8009CD68[*(s32 *)((s8*)(obj) + 0x58)].vz * (*(s16 *)((s8*)(obj) + 0x4A) >> 12);
        *(s32 *)((s8*)(obj) + 0x58) = (*(s32 *)((s8*)(obj) + 0x58) + 1) & 0x1F;
        func_80093354(obj + 0x28);
        *(s32 *)((s8*)(obj) + 0x2C) = func_80093978(*(s32 *)((s8*)(obj) + 0x28), *(s32 *)((s8*)(obj) + 0x30));
        *(s32 *)((s8*)(rec) + 8) = *(s32 *)((s8*)(obj) + 0x28) >> 0xC;
        *(s32 *)((s8*)(rec) + 0xC) = *(s32 *)((s8*)(obj) + 0x2C) >> 0xC;
        *(s32 *)((s8*)(rec) + 0x10) = *(s32 *)((s8*)(obj) + 0x30) >> 0xC;
        sp->vx = *(u16 *)((u8 *)&g_GameState + 0x182C) - (*(s32 *)((s8*)(obj) + 0x28) >> 0xC);
        sp->vz = *(u16 *)((u8 *)&g_GameState + 0x1830) - (*(s32 *)((s8*)(obj) + 0x30) >> 0xC);
        sp->vy = *(s16 *)((u8 *)&g_GameState + 0x182E);
        func_80093534(sp);
        d = SquareRoot0(sp->vx * sp->vx + sp->vz * sp->vz);
        if (d < 0x100 && sp->vy >= -0xBF) {
            *(s16 *)((s8*)(obj) + 0x4A) = 0;
        } else if (d < 0x300 && sp->vy >= -0xBF) {
            *(s16 *)((s8*)(obj) + 0x4A) -= 0x100;
            if (*(s16 *)((s8*)(obj) + 0x4A) < 0) {
                *(s16 *)((s8*)(obj) + 0x4A) = 0;
            }
        } else {
            *(s16 *)((s8*)(obj) + 0x4A) += 0x200;
            if (*(s16 *)((s8*)(obj) + 0x4A) > 0x4000) {
                *(s16 *)((s8*)(obj) + 0x4A) = 0x4000;
            }
        }
        if (*(s16 *)((s8*)(obj) + 0x4A) > 0x1000) {
            ((WmSVec *)((u8 *)sp + 0xA0))->vx = *(s32 *)((s8*)(rec) + 8);
            ((WmSVec *)((u8 *)sp + 0xA0))->vy = *(s32 *)((s8*)(rec) + 0xC);
            ((WmSVec *)((u8 *)sp + 0xA0))->vz = *(s32 *)((s8*)(rec) + 0x10);
            TransposeMatrix(rec + 0x20, (u8 *)sp + 0x150);
            func_80097070((u8 *)sp + 0x150, (u8 *)sp + 0xA8);
            func_80089160(0x13, (u8 *)sp + 0xA0, (u8 *)sp + 0xA8);
        } else {
            func_800894C8(0x13);
        }
    }
    *(s16 *)((u8 *)&g_GameState + 0x1844) = *(s32 *)((s8*)(obj) + 0x28) >> 0xC;
    *(s16 *)((u8 *)&g_GameState + 0x1846) = *(s32 *)((s8*)(obj) + 0x30) >> 0xC;
    *(s16 *)((u8 *)&g_GameState + 0x1848) = *(s32 *)((s8*)(obj) + 0x50);
    sp->vx = *(s32 *)((s8*)(obj) + 0x28);
    sp->vy = *(s32 *)((s8*)(obj) + 0x2C) + 0x30000;
    sp->vz = *(s32 *)((s8*)(obj) + 0x30);
    func_8008BFD4(arg0, sp, 0x80, 0xB0);
    return 1;
}



s32 func_80088570(s32 arg0) {
    void *temp_a1;
    u16 *pVisits;

    func_8008868C(arg0);
    temp_a1 = (u8 *)D_8009BE24 + (arg0 << 7);
    pVisits = (u16 *)((u8 *)&g_GameState + 0x1854);
    *(s32 *)((s8*)(temp_a1) + 0x54) = 4;
    *(s16 *)((s8*)(temp_a1) + 0x20) = 0;
    *(s32 *)((s8*)(temp_a1) + 0x50) = 0;
    *(s32 *)((s8*)(temp_a1) + 0x58) = 0;
    *(s32 *)((s8*)(temp_a1) + 0x5C) = 0xC;
    if (*pVisits == 0) {
        *pVisits = *pVisits + 1;
        *(s32 *)((s8*)(temp_a1) + 0x2C) = 0xFFD80000;
        *(s32 *)((s8*)(temp_a1) + 0x28) = 0;
        *(s32 *)((s8*)(temp_a1) + 0x30) = 0x04800000;
    } else {
        *(s32 *)((s8*)(temp_a1) + 0x28) = (s32) ((*(s16 *)((s8*)(&g_GameState) + 0x184E) << 0xC) + *(u16 *)((s8*)(&g_GameState) + 0x184C));
        *(s32 *)((s8*)(temp_a1) + 0x2C) = 0xFFD80000;
        *(s32 *)((s8*)(temp_a1) + 0x30) = (*(s16 *)((s8*)(&g_GameState) + 0x1852) << 0xC) + *(u16 *)((s8*)(&g_GameState) + 0x1850);
    }
    *(s32 *)((s8*)(temp_a1) + 0x40) = 0xB50;
    *(s32 *)((s8*)(temp_a1) + 0x38) = 0xB50;
    *(s32 *)((s8*)(temp_a1) + 0x3C) = 0;
    *(s16 *)((s8*)(temp_a1) + 0x4A) = 1;
    *(u16 *)((s8*)(&g_GameState) + 0x184C) = (u16) *(s32 *)((s8*)(temp_a1) + 0x28);
    *(s16 *)((s8*)(&g_GameState) + 0x184E) = (s16) ((s32) *(s32 *)((s8*)(temp_a1) + 0x28) >> 0xC);
    *(u16 *)((s8*)(&g_GameState) + 0x1850) = (u16) *(s32 *)((s8*)(temp_a1) + 0x30);
    *(s16 *)((s8*)(&g_GameState) + 0x1852) = (s16) ((s32) *(s32 *)((s8*)(temp_a1) + 0x30) >> 0xC);
    return 1;
}


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_airship_link_objects (src/decomp/src/worldmap/worldmap_objects_effects_party.c:1888). */
#ifndef XM_S_worldmap_airship_link_objects
#define XM_S_worldmap_airship_link_objects
s32 xm_worldmap_airship_link_objects(void) __asm__("func_8008868C");
#endif
#ifndef XM_S_worldmap_area_airship_objects
#define XM_S_worldmap_area_airship_objects
extern u16 xm_worldmap_area_airship_objects[10] __asm__("D_8009B688");
#endif
#ifndef XM_S_worldmap_area_index
#define XM_S_worldmap_area_index
extern s32 xm_worldmap_area_index __asm__("D_8009C610");
#endif
#ifndef XM_S_worldmap_airship_object_links
#define XM_S_worldmap_airship_object_links
extern s16 xm_worldmap_airship_object_links[30] __asm__("D_8009AFA0");
#endif
s32 xm_worldmap_airship_link_objects(void) __asm__("func_8008868C");
s32 xm_worldmap_airship_link_objects(void) {
    s32 i;
    s32 base;

    base = xm_worldmap_area_airship_objects[xm_worldmap_area_index];
    for (i = 0; xm_worldmap_airship_object_links[i] != -1; i += 2) {
        xm_worldmap_objects_link(base + xm_worldmap_airship_object_links[i], base + xm_worldmap_airship_object_links[i + 1]);
    }
    return 1;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_airship_update (src/decomp/src/worldmap/worldmap_objects_effects_party.c:1914). */
#ifndef XM_T_FlightScratch
#define XM_T_FlightScratch
typedef struct {
    VECTOR work;
    u8 pad10[0x90];
    SVECTOR rotor;       
    SVECTOR tail;        
    u8 padB0[0x40];
    MATRIX rotor_matrix; 
    MATRIX tail_matrix;  
} FlightScratch;
#endif
#ifndef XM_T_FLIGHT_SCRATCH
#define XM_T_FLIGHT_SCRATCH
#define FLIGHT_SCRATCH ((FlightScratch *)0x1F800000)
#endif
#ifndef XM_S_worldmap_airship_update
#define XM_S_worldmap_airship_update
s32 xm_worldmap_airship_update(s32 index) __asm__("func_80088720");
#endif
#ifndef XM_S_SquareRoot0
#define XM_S_SquareRoot0
long xm_SquareRoot0(long a) __asm__("SquareRoot0");
#endif
#ifndef XM_S_worldmap_queue_actor_cylinder
#define XM_S_worldmap_queue_actor_cylinder
void xm_worldmap_queue_actor_cylinder(s32 index, VECTOR *position, s32 x, s32 z) __asm__("func_8008BFD4");
#endif
s32 func_80088720(s32 index) {
    WorldmapActor *actor;
    FlightScratch *scratch;
    s32 base;

    actor = &xm_worldmap_actor_slots[index];
    base = xm_worldmap_area_airship_objects[xm_worldmap_area_index];
    actor->u.step = (actor->u.step + actor->unk54) & 0xFFF;
    actor->unk58 = (actor->unk58 + actor->unk5C) & 0xFFF;
    FLIGHT_SCRATCH->rotor.vx = FLIGHT_SCRATCH->rotor.vz = 0;
    FLIGHT_SCRATCH->rotor.vy = actor->u.step;
    FLIGHT_SCRATCH->tail.vx = FLIGHT_SCRATCH->tail.vz = 0;
    FLIGHT_SCRATCH->tail.vy = actor->unk58;
    xm_RotMatrixYXZ(&FLIGHT_SCRATCH->rotor, &FLIGHT_SCRATCH->rotor_matrix);
    xm_RotMatrixYXZ(&FLIGHT_SCRATCH->tail, &FLIGHT_SCRATCH->tail_matrix);
    xm_worldmap_objects[base + 5].matrix = FLIGHT_SCRATCH->rotor_matrix;
    scratch = FLIGHT_SCRATCH;
    xm_worldmap_objects[base].matrix = xm_worldmap_objects[base + 1].matrix = xm_worldmap_objects[base + 2].matrix =
        xm_worldmap_objects[base + 3].matrix = FLIGHT_SCRATCH->tail_matrix;
    scratch->work.vx = xm_game_data.worldmap.unk60 - (actor->position.vx >> 12);
    scratch->work.vz = xm_game_data.worldmap.unk64 - (actor->position.vz >> 12);
    scratch->work.vy = (s16)xm_game_data.worldmap.unk62;
    xm_worldmap_wrap_world_offset(&scratch->work);
    if (xm_SquareRoot0(scratch->work.vx * scratch->work.vx + scratch->work.vz * scratch->work.vz) < 0x300 && scratch->work.vy < -0x240) {
        actor->turn = 0;
    } else {
        actor->turn = 1;
    }
    actor->position.vx += actor->motion.vx * actor->turn;
    actor->position.vz += actor->motion.vz * actor->turn;
    xm_worldmap_wrap_position(&actor->position);
    scratch->work.vx = actor->position.vx >> 12;
    scratch->work.vy = actor->position.vy >> 12;
    scratch->work.vz = actor->position.vz >> 12;
    xm_worldmap_objects[base + 12].position = scratch->work;
    xm_worldmap_queue_actor_cylinder(index, &actor->position, 0x180, 0xC0);
    xm_game_data.flight.x_frac = actor->position.vx;
    xm_game_data.flight.x = actor->position.vx >> 12;
    xm_game_data.flight.z_frac = actor->position.vz;
    xm_game_data.flight.z = actor->position.vz >> 12;
    return 1;
}
#endif


s32 func_80088B40(s32 arg0) {
    s32 i;
    u8 *obj;
    u8 *rec;
    s32 ret;

    for (i = 0; D_8009AFDC[i] != -1; i++) { func_800848B4(0x45, D_8009AFDC[i]); }
    obj = (u8 *)D_8009BE24 + (arg0 << 7);
    rec = D_8009C620;
    *(s16 *)((s8*)(obj) + 0x20) = 0;
    if (*(u16 *)((u8 *)&g_GameState + 0x1930) == 0x99) {
        ret = 1;
        *(s32 *)((s8*)(obj) + 0x28) = 0x02000000;
        *(s32 *)((s8*)(obj) + 0x30) = 0x04120000;
        *(s32 *)((s8*)(obj) + 0x2C) = 0;
    } else {
        *(s16 *)((s8*)(rec) + 0x16A4) = 1;
        for (i = 0; D_8009AFDC[i] != -1; i++) { *(s16 *)(rec + D_8009AFDC[i] * 0x54) = 1; }
        ret = 3;
    }
    rec += 0x16A4;
    *(s32 *)((s8*)(rec) + 8) = *(s32 *)((s8*)(obj) + 0x28) >> 12;
    *(s32 *)((s8*)(rec) + 0xC) = *(s32 *)((s8*)(obj) + 0x2C) >> 12;
    *(s32 *)((s8*)(rec) + 0x10) = *(s32 *)((s8*)(obj) + 0x30) >> 12;
    return ret;
}


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_object69_resume (src/decomp/src/worldmap/worldmap_objects_effects_party.c:1991). */
#ifndef XM_S_worldmap_object69_resume
#define XM_S_worldmap_object69_resume
s32 xm_worldmap_object69_resume(void) __asm__("func_80088C90");
#endif
#ifndef XM_S_worldmap_object69_children
#define XM_S_worldmap_object69_children
extern s16 xm_worldmap_object69_children[10] __asm__("D_8009AFDC");
#endif
s32 func_80088C90(void) {
    SceneObject *objects;
    s32 i;

    objects = xm_worldmap_objects;
    objects[69].visible = 1;
    for (i = 0; xm_worldmap_object69_children[i] != -1; i++) {
        objects[xm_worldmap_object69_children[i]].visible = 1;
    }
    return 3;
}
#endif
s32 func_80088D00(s32 arg0) {
    u8 *a1;
    WmObj *obj;

    a1 = D_8009C620;
    obj = (WmObj *)((u8 *)D_8009BE24 + (arg0 << 7));
    if (*(u16 *)((u8 *)&g_GameState + 0x1930) == 0x99) {
        a1 += 0x16A4;
        *(s32 *)(a1 + 8) = obj->pos[0] >> 12;
        *(s32 *)(a1 + 0xC) = obj->pos[1] >> 12;
        *(s32 *)(a1 + 0x10) = obj->pos[2] >> 12;
    }
    return 1;
}



s32 func_80088D64(s32 arg0) {
    void *temp_a1;
    void *temp_a2;

    temp_a2 = D_8009C620 + (D_8009B69C[D_8009C610] * 0x54);
    temp_a1 = (u8 *)D_8009BE24 + (arg0 << 7);
    *(s32 *)((s8*)(temp_a1) + 0x28) = (s32) (*(s32 *)((s8*)(temp_a2) + 8) << 0xC);
    *(s32 *)((s8*)(temp_a1) + 0x2C) = (s32) ((*(s32 *)((s8*)(temp_a2) + 0xC) << 0xC) + 0x40000);
    *(s32 *)((s8*)(temp_a1) + 0x30) = (s32) (*(s32 *)((s8*)(temp_a2) + 0x10) << 0xC);
    return 1;
}



s32 func_80088DE4(s32 arg0) {
    func_8008BFD4(arg0, (u8 *)D_8009BE24 + (arg0 << 7) + 0x28, 0x68, 0x60);
    return 1;
}



s32 func_80088E1C(s32 arg0) {
    void *temp_v1;

    temp_v1 = (u8 *)D_8009BE24 + (arg0 << 7);
    *(s32 *)((s8*)(temp_v1) + 0x28) = (s32) (*(s32 *)((s8*)(((void *)D_8009C620)) + 0x18A4) << 0xC);
    *(s32 *)((s8*)(temp_v1) + 0x2C) = (s32) (*(s32 *)((s8*)(((void *)D_8009C620)) + 0x18A8) << 0xC);
    *(s32 *)((s8*)(temp_v1) + 0x30) = (s32) (*(s32 *)((s8*)(((void *)D_8009C620)) + 0x18AC) << 0xC);
    return 1;
}



s32 func_80088E68(s32 arg0) {
    func_8008BFD4(arg0, (u8 *)D_8009BE24 + (arg0 << 7) + 0x28, 0x10C, 0x1A6);
    return 1;
}



s32 func_80088EA0(s32 arg0) {
    void *temp_a1;
    void *temp_v1;

    temp_a1 = D_8009C620 + (D_8009B6B0[D_8009C610] * 0x54);
    temp_v1 = (u8 *)D_8009BE24 + (arg0 << 7);
    *(s32 *)((s8*)(temp_v1) + 0x28) = (s32) (*(s32 *)((s8*)(temp_a1) + 8) << 0xC);
    *(s32 *)((s8*)(temp_v1) + 0x2C) = (s32) (*(s32 *)((s8*)(temp_a1) + 0xC) << 0xC);
    *(s32 *)((s8*)(temp_v1) + 0x30) = (s32) (*(s32 *)((s8*)(temp_a1) + 0x10) << 0xC);
    return 1;
}



s32 func_80088F1C(s32 arg0) {
    func_8008BFD4(arg0, (u8 *)D_8009BE24 + (arg0 << 7) + 0x28, 0xC9, 0x392);
    return 1;
}



s32 func_80088F54(void) {
    return 3;
}



s32 func_80088F5C(void) {
    return 3;
}


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_effects_alloc_slots (src/decomp/src/worldmap/worldmap_objects_effects_party.c:2085). */
#ifndef XM_T_EffectSlot
#define XM_T_EffectSlot
typedef struct EffectSlot {
    s16 id;            
    s16 unk2;
    s32 timer;         
    VECTOR position;   
    VECTOR velocity;   
    VECTOR accel;      
    s16 rot[2];        
    s16 spin[2];       
    s32 colour;        
    s32 fade;          
    s16 code;          
    s16 pad4A;
} EffectSlot;
#endif
#ifndef XM_T_EFFECT_ENABLED
#define XM_T_EFFECT_ENABLED
#define EFFECT_ENABLED(slot) (((s16 *)&(slot)->timer)[1])
#endif
#ifndef XM_T_EFFECT_COUNT
#define XM_T_EFFECT_COUNT
#define EFFECT_COUNT(slot) (((s16 *)&(slot)->timer)[0])
#endif
#ifndef XM_S_worldmap_effects_alloc_slots
#define XM_S_worldmap_effects_alloc_slots
void xm_worldmap_effects_alloc_slots(void) __asm__("func_80088F64");
#endif
#ifndef XM_S_worldmap_effect_slots
#define XM_S_worldmap_effect_slots
extern EffectSlot *xm_worldmap_effect_slots __asm__("D_8009BDF4");
#endif
void func_80088F64(void) {
    AreaObject *object;
    EffectSlot *slot;
    s32 i;

    object = xm_worldmap_effect_emitters;
    for (i = 0x1FF; i != -1; i--) {
        object->unk4 = 0;
        object->unkA = 0;
        object->unk12 = 0;
        object->position.vz = 0;
        object->position.vy = 0;
        object->position.vx = 0;
        object->angle.vz = 0;
        object->angle.vy = 0;
        object->angle.vx = 0;
        object++;
    }
    xm_worldmap_effect_slots = slot = xm_heap_alloc(0x4C00, 0);
    for (i = 0xFF; i != -1; i--) {
        EFFECT_ENABLED(slot) = 0;
        EFFECT_COUNT(slot) = 0;
        slot++;
    }
}
#endif
#ifndef XENO_PC_PORT
void func_80088FF4(void) {
    HeapFree(D_8009BDF4);
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008901C);


#ifndef XENO_PC_PORT
void func_80089128(void) {
    HeapFree(D_8009BE1C);
    HeapFree(D_8009BE20);
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_effects_start_emitters (src/decomp/src/worldmap/worldmap_objects_effects_party.c:2151). */
#ifndef XM_T_SVECTOR_ZERO
#define XM_T_SVECTOR_ZERO
#define SVECTOR_ZERO(v) (*(s32 *)&(v)->vx = 0, (v)->vz = 0)
#endif
void xm_worldmap_effects_start_emitters(s32 group, SVECTOR *position, SVECTOR *angle) __asm__("func_80089160");
void xm_worldmap_effects_start_emitters(s32 group, SVECTOR *position, SVECTOR *angle) {
    AreaObject *object;
    s32 live;
    s32 i;

    live = 0;
    object = &xm_worldmap_effect_emitters[group * 8];
    for (i = 7; i != -1; i--) {
        if (object->flags & 0x80) {
            live++;
            break;
        }
    }
    object = &xm_worldmap_effect_emitters[group * 8];
    if ((position == NULL) & (angle == NULL)) {
        for (i = 7; i != -1; object++, i--) {
            if (live == 0) {
                object->flags |= 0x80;
                object->unk4 = object->unk0;
                object->unk12 = object->unk10;
                object->unkA = 0;
            }
            *(s32 *)&object->position.vx = *(s32 *)&object->angle.vx = 0;
            object->position.vz = object->angle.vz = 0;
        }
    } else if ((position != NULL) & (angle == NULL)) {
        for (i = 7; i != -1; object++, i--) {
            if (live == 0) {
                object->flags |= 0x80;
                object->unk4 = object->unk0;
                object->unk12 = object->unk10;
                object->unkA = 0;
            }
            object->position = *position;
            SVECTOR_ZERO(&object->angle);
        }
    } else if ((position == NULL) & (angle != NULL)) {
        for (i = 7; i != -1; object++, i--) {
            if (live == 0) {
                object->flags |= 0x80;
                object->unk4 = object->unk0;
                object->unk12 = object->unk10;
                object->unkA = 0;
            }
            SVECTOR_ZERO(&object->position);
            object->angle.vx = -angle->vx;
            object->angle.vy = -angle->vy;
            object->angle.vz = -angle->vz;
        }
    } else {
        for (i = 7; i != -1; object++, i--) {
            if (live == 0) {
                object->flags |= 0x80;
                object->unk4 = object->unk0;
                object->unk12 = object->unk10;
                object->unkA = 0;
            }
            object->position = *position;
            object->angle.vx = -angle->vx;
            object->angle.vy = -angle->vy;
            object->angle.vz = -angle->vz;
        }
    }
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_effects_aim_emitters (src/decomp/src/worldmap/worldmap_objects_effects_party.c:2218). */
#ifndef XM_S_worldmap_effects_aim_emitters
#define XM_S_worldmap_effects_aim_emitters
void xm_worldmap_effects_aim_emitters(s32 group, SVECTOR *direction, SVECTOR *angle) __asm__("func_800893E0");
#endif
void func_800893E0(s32 group, SVECTOR *direction, SVECTOR *angle) {
    AreaObject *object;
    s32 live;
    s32 i;

    live = 0;
    object = &xm_worldmap_effect_emitters[group * 8];
    for (i = 7; i != -1; i--) {
        if (object->flags & 0x80) {
            live++;
            break;
        }
    }
    object = &xm_worldmap_effect_emitters[group * 8];
    for (i = 7; i != -1; object++, i--) {
        if (live == 0) {
            object->flags |= 0x80;
            object->unk4 = object->unk0;
            object->unk12 = object->unk10;
            object->unkA = 0;
        }
        *(s32 *)&object->direction.vx = *(s32 *)&direction->vx;
        *(s32 *)&object->position.vx = 0;
        *(s32 *)&object->angle.vx = *(s32 *)&angle->vx;
        object->direction.vz = direction->vz;
        object->position.vz = 0;
        object->angle.vz = angle->vz;
    }
}
#endif


void func_800894C8(s32 arg0) {
    s32 var_a1;
    u8 *var_v1;

    var_a1 = 7;
    var_v1 = D_8009BCC0 + (arg0 * 0x2A0) + 0x4F;
    do {
        var_a1 -= 1;
        *var_v1 &= 0x7F;
        var_v1 += 0x54;
    } while (var_a1 != -1);
}


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_effects_stop_particles (src/decomp/src/worldmap/worldmap_objects_effects_party.c:2261). */
#ifndef XM_S_worldmap_effects_stop_particles
#define XM_S_worldmap_effects_stop_particles
void xm_worldmap_effects_stop_particles(s32 group) __asm__("func_80089514");
#endif
void func_80089514(s32 group) {
    EffectSlot *slot;
    s32 i;
    s32 j;

    slot = xm_worldmap_effect_slots;
    for (i = 0xFF; i != -1; i--) {
        for (j = 0; j < 8; j++) {
            if (slot->id == group * 8 + j && EFFECT_ENABLED(slot) != 0) {
                EFFECT_COUNT(slot) = 0;
                break;
            }
        }
        slot++;
    }
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_effects_age_particles (src/decomp/src/worldmap/worldmap_objects_effects_party.c:2280). */
#ifndef XM_S_worldmap_effects_age_particles
#define XM_S_worldmap_effects_age_particles
void xm_worldmap_effects_age_particles(void) __asm__("func_80089580");
#endif
void func_80089580(void) {
    EffectSlot *slot;
    s32 step;   
    s32 colour; 
    s32 i;
    s32 r;
    s32 g;
    s32 b;
    s32 px, py, pz;
    s32 vx, vy, vz;

    slot = xm_worldmap_effect_slots;
    for (i = 0xFF; i != -1; slot++, i--) {
        step = slot->timer;
        colour = step >> 16;
        step = (s16)step;
        if (colour != 0) {
            if (step > 0) {
                px = slot->position.vx;
                py = slot->position.vy;
                pz = slot->position.vz;
                vx = slot->velocity.vx;
                vy = slot->velocity.vy;
                vz = slot->velocity.vz;
                colour = slot->colour;
                EFFECT_COUNT(slot)--;
                step = slot->fade;
                px += vx;
                py += vy;
                pz += vz;
                vx += slot->accel.vx;
                vy += slot->accel.vy;
                vz += slot->accel.vz;
                r = (colour & 0xFF) + (s8)step;
                g = ((colour >> 8) & 0xFF) + (s8)(step >> 8);
                b = ((colour >> 16) & 0xFF) + (s8)(step >> 16);
                slot->rot[0] += slot->spin[0];
                slot->rot[1] += slot->spin[1];
                if (r < 0) {
                    r = 0;
                }
                if (r > 0xFF) {
                    r = 0xFF;
                }
                if (g < 0) {
                    g = 0;
                }
                if (g > 0xFF) {
                    g = 0xFF;
                }
                if (b < 0) {
                    b = 0;
                }
                if (b > 0xFF) {
                    b = 0xFF;
                }
                slot->colour = (colour & 0xFF000000) | (b << 16) | (g << 8) | r;
                slot->position.vx = px;
                slot->position.vy = py;
                slot->position.vz = pz;
                slot->velocity.vx = vx;
                slot->velocity.vy = vy;
                slot->velocity.vz = vz;
            } else {
                xm_worldmap_effect_emitters[slot->id].unkA--;
                slot->id = 0;
                slot->timer = 0;
            }
        }
    }
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_effects_run_emitters (src/decomp/src/worldmap/worldmap_objects_effects_party.c:2366). */
#ifndef XM_T_EmitScratch
#define XM_T_EmitScratch
typedef struct {
    VECTOR normal;  
    VECTOR random;  
    VECTOR offset;  
    u8 pad30[0xC0];
    MATRIX m;       
} EmitScratch;
#endif
#ifndef XM_T_EMIT_SCRATCH
#define XM_T_EMIT_SCRATCH
#define EMIT_SCRATCH ((EmitScratch *)0x1F800000)
#endif
#ifndef XM_S_ApplyMatrix
#define XM_S_ApplyMatrix
VECTOR *xm_ApplyMatrix(MATRIX *m, SVECTOR *v0, VECTOR *v1) __asm__("ApplyMatrix");
#endif
#ifndef XM_S_ratan2
#define XM_S_ratan2
long xm_ratan2(long y, long x) __asm__("ratan2");
#endif
void func_80089748(void) {
    AreaObject *object;
    EffectSlot *slot;
    EmitScratch *scratch;
    s32 i;
    s32 flags;
    
    s32 delay;
    s32 repeats;
    s32 value;

    object = xm_worldmap_effect_emitters;
    scratch = EMIT_SCRATCH;
    for (i = 0; i < 0x200; i++, object++) {
        flags = object->flags;
        if (!(flags & 0x80)) {
            continue;
        }
        if (flags & 0x10) {
            delay = object->unk4;
            repeats = delay >> 16;
            delay = (s16)delay;
            if (delay != 0) {
                delay--;
                goto store;
            }
            if (repeats == 0) {
                goto expire;
            }
            repeats--;
        }
        if (object->unk12 != 0) {
            object->unk12--;
            goto store;
        }
        object->unk12 = object->unk10;
        if ((((s16 *)&object->life)[1] != 0) & (object->unk8 > 0) & (object->unkA < object->unk8)) {
            value = 0xFF;
            slot = xm_worldmap_effect_slots;
            for (; value != -1; value--, slot++) {
                if (EFFECT_ENABLED(slot) == 0) {
                    slot->id = i;
                    slot->timer = object->life;
                    xm_RotMatrixYXZ(&object->angle, &scratch->m);
                    xm_ApplyMatrix(&scratch->m, &object->unk24, &scratch->offset);
                    if (!(flags & 0x20)) {
                        scratch->random.vy = (xm_rand() & 0xFFF) - 0x800;
                    } else {
                        scratch->random.vy = 0;
                    }
                    scratch->random.vx = (xm_rand() & 0xFFF) - 0x800;
                    scratch->random.vz = (xm_rand() & 0xFFF) - 0x800;
                    xm_VectorNormal(&scratch->random, &scratch->normal);
                    if (flags & 4) {
                        value = object->spread[0];
                    } else {
                        value = xm_rand() % object->spread[0];
                    }
                    slot->position.vx = (scratch->offset.vx << 12) + scratch->normal.vx * value +
                                        (object->position.vx << 12);
                    slot->position.vy = (scratch->offset.vy << 12) + scratch->normal.vy * value +
                                        (object->position.vy << 12);
                    slot->position.vz = (scratch->offset.vz << 12) + scratch->normal.vz * value +
                                        (object->position.vz << 12);
                    xm_ApplyMatrix(&scratch->m, &object->direction, &scratch->offset);
                    if (!(flags & 0x40)) {
                        scratch->random.vy = (xm_rand() & 0xFFF) - 0x800;
                    } else {
                        scratch->random.vy = 0;
                    }
                    scratch->random.vx = (xm_rand() & 0xFFF) - 0x800;
                    scratch->random.vz = (xm_rand() & 0xFFF) - 0x800;
                    xm_VectorNormal(&scratch->random, &scratch->normal);
                    if (flags & 8) {
                        value = object->spread[1];
                    } else {
                        value = xm_rand() % object->spread[1];
                    }
                    scratch->normal.vx = (scratch->offset.vx << 12) + scratch->normal.vx * value +
                                         (object->position.vx << 12);
                    scratch->normal.vy = (scratch->offset.vy << 12) + scratch->normal.vy * value +
                                         (object->position.vy << 12);
                    scratch->normal.vz = (scratch->offset.vz << 12) + scratch->normal.vz * value +
                                         (object->position.vz << 12);
                    scratch->normal.vx = (scratch->normal.vx - slot->position.vx) >> 12;
                    scratch->normal.vy = (scratch->normal.vy - slot->position.vy) >> 12;
                    scratch->normal.vz = (scratch->normal.vz - slot->position.vz) >> 12;
                    xm_VectorNormal(&scratch->normal, &scratch->random);
                    slot->velocity.vx = (scratch->random.vx * object->speed) >> 12;
                    slot->velocity.vy = (scratch->random.vy * object->speed) >> 12;
                    slot->velocity.vz = (scratch->random.vz * object->speed) >> 12;
                    flags &= 3; 
                    slot->unk2 = xm_ratan2(scratch->random.vy, scratch->random.vx);
                    slot->accel.vx = object->accel[0];
                    slot->accel.vy = object->accel[1];
                    slot->accel.vz = object->accel[2];
                    slot->colour = *(s32 *)object->rgb;
                    slot->fade = object->fade;
                    *(s32 *)slot->rot = object->rot;
                    *(s32 *)slot->spin = object->spin;
                    slot->code = (flags << 5) | 0x9D;
                    object->unkA++;
                    goto store;
                }
            }
        }
        goto store;
    expire:
        object->flags ^= 0x80;
    store:
        object->unk4 = (repeats << 16) | delay;
    }
    xm_worldmap_effects_age_particles();
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80089C78);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008A2C8);


#ifndef XENO_PC_PORT
s32 func_8008A52C(s32 arg0) {
    void *temp_a1;
    void *temp_s0;
    void *temp_v0;

    temp_s0 = (u8 *)D_8009BE24 + (arg0 << 7);
    temp_v0 = func_80024524(D_8009CD34[0], 0x100, 0x1E0, 0x140, 0x100, 0x40);
    *(void **)((s8*)(temp_s0) + 0x4C) = temp_v0;
    func_800245D8(temp_v0, 0);
    SpriteSetScale(*(void **)((s8*)(temp_s0) + 0x4C), 0x1800);
    temp_a1 = *(void **)((s8*)(temp_s0) + 0x4C);
    *(s32 *)((s8*)(temp_a1) + 0x3C) = (s32) (*(s32 *)((s8*)(temp_a1) + 0x3C) & ~4);
    return 1;
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_leader_start_at_saved_spot (src/decomp/src/worldmap/worldmap_objects_effects_party.c:2675). */
#ifndef XM_T_SPRITE_HIDDEN
#define XM_T_SPRITE_HIDDEN
#define SPRITE_HIDDEN 4
#endif
#ifndef XM_S_worldmap_leader_start_at_saved_spot
#define XM_S_worldmap_leader_start_at_saved_spot
s32 xm_worldmap_leader_start_at_saved_spot(s32 index) __asm__("func_8008A5B8");
#endif
#ifndef XM_S_sprite_create
#define XM_S_sprite_create
Sprite *xm_sprite_create(void *data, s16 clut_x, s16 clut_y, s16 texture_x, s16 texture_y, s32 unused) __asm__("func_80024524");
#endif
#ifndef XM_S_sprite_start_animation
#define XM_S_sprite_start_animation
void xm_sprite_start_animation(Sprite *sprite, s32 animation) __asm__("func_800245D8");
#endif
#ifndef XM_S_sprite_set_scale
#define XM_S_sprite_set_scale
void xm_sprite_set_scale(Sprite *sprite, s32 scale) __asm__("SpriteSetScale");
#endif
#ifndef XM_S_worldmap_terrain_get_height
#define XM_S_worldmap_terrain_get_height
s32 xm_worldmap_terrain_get_height(s32 x, s32 z) __asm__("func_80093978");
#endif
s32 func_8008A5B8(s32 index) {
    WorldmapActor *actor;
    TrailPoint *point;
    s32 i;
    s16 heading;

    actor = &xm_worldmap_actor_slots[index];
    actor->handle = xm_sprite_create(xm_worldmap_character_models[0], 0x100, 0x1E0, 0x140, 0x100, 0x40);
    xm_sprite_start_animation(actor->handle, 0);
    xm_sprite_set_scale(actor->handle, 0x1800);
    actor->handle->render.word &= ~SPRITE_HIDDEN;
    actor->position.vx = xm_game_data.worldmap.x << 12;
    actor->position.vz = xm_game_data.worldmap.z << 12;
    actor->position.vy = xm_worldmap_terrain_get_height(actor->position.vx, actor->position.vz);
    i = 0;
    point = xm_worldmap_trail_points;
    heading = xm_game_data.worldmap.heading;
    actor->unk24 = 1;
    actor->turn = 8;
    actor->motion.vz = 0;
    actor->motion.vy = 0;
    actor->motion.vx = 0;
    actor->state = 2;
    xm_worldmap_trail_index = 0;
    actor->heading = heading;
    do {
        point->position = actor->position;
        i++;
        point->heading = actor->heading;
        point++;
    } while (i < 0x20);
    xm_worldmap_camera_follow_target.target = actor->position;
    xm_worldmap_camera_follow_heading = actor->heading;
    return 1;
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008A72C);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_second_member_start (src/decomp/src/worldmap/worldmap_objects_effects_party.c:3026). */
#ifndef XM_S_worldmap_second_member_start
#define XM_S_worldmap_second_member_start
s32 xm_worldmap_second_member_start(s32 index) __asm__("func_8008B2BC");
#endif
s32 func_8008B2BC(s32 index) {
    WorldmapActor *actor;
    s32 result;

    actor = &xm_worldmap_actor_slots[index];
    result = 1;
    if (xm_game_data.party[1] != 0xFF) {
        actor->handle = xm_sprite_create(xm_worldmap_character_models[1], 0x110, 0x1E0, 0x150, 0x100, 0x40);
        xm_sprite_start_animation(actor->handle, 0);
        xm_sprite_set_scale(actor->handle, 0x1800);
        actor->handle->render.word &= ~SPRITE_HIDDEN;
        actor->unk24 = 0;
    } else {
        actor->unk24 = 1;
        result = 3;
    }
    actor->position.vx = xm_game_data.worldmap.x << 12;
    actor->position.vz = xm_game_data.worldmap.z << 12;
    actor->position.vy = xm_worldmap_terrain_get_height(actor->position.vx, actor->position.vz);
    actor->motion.vz = 0;
    actor->motion.vy = 0;
    actor->motion.vx = 0;
    actor->heading = xm_game_data.worldmap.heading;
    actor->turn = 8;
    actor->unk58 = 0xF;
    actor->unk5C = actor->heading;
    switch (xm_worldmap_movement_mode) {
    case 1:
    case 2:
    case 3:
        if (xm_game_data.inGear[1] == 0) {
            actor->position.vx = xm_worldmap_player_position.vx;
            actor->position.vz = xm_worldmap_player_position.vz;
            actor->position.vy = xm_worldmap_terrain_get_height(actor->position.vx, actor->position.vz);
            actor->heading = xm_worldmap_player_heading;
        } else {
            actor->state = 1;
            actor->unk24 = 1;
        }
        break;
    case 4:
    case 5:
    case 6:
        actor->state = 3;
        actor->unk24 = 1;
        if (xm_game_data.party[1] != 0xFF) {
            xm_game_data.inGear[1] = 1;
        }
        break;
    case 7:
        actor->state = 2;
        actor->unk24 = 1;
        if (xm_game_data.party[1] != 0xFF) {
            xm_game_data.inGear[1] = 1;
        }
        break;
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
        break;
    }
    return result;
}
#endif


#ifndef XENO_PC_PORT
s32 func_8008B498(s32 arg0) {
    s32 var_s1;
    void *temp_a0;
    void *temp_s0;
    void *temp_v0;

    temp_s0 = (u8 *)D_8009BE24 + (arg0 << 7);
    var_s1 = 1;
    if (*(u8 *)((s8*)(&g_GameState) + 0x1D35) != 0xFF) {
        temp_v0 = func_80024524(D_8009CD38, 0x110, 0x1E0, 0x150, 0x100, 0x40);
        *(void **)((s8*)(temp_s0) + 0x4C) = temp_v0;
        func_800245D8(temp_v0, 0);
        SpriteSetScale(*(void **)((s8*)(temp_s0) + 0x4C), 0x1800);
        temp_a0 = *(void **)((s8*)(temp_s0) + 0x4C);
        *(s32 *)((s8*)(temp_a0) + 0x3C) = (s32) (*(s32 *)((s8*)(temp_a0) + 0x3C) & ~4);
    } else {
        var_s1 = 3;
    }
    return var_s1;
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_second_member_start_at_saved_spot (src/decomp/src/worldmap/worldmap_objects_effects_party.c:3112). */
#ifndef XM_S_worldmap_second_member_start_at_saved_spot
#define XM_S_worldmap_second_member_start_at_saved_spot
s32 xm_worldmap_second_member_start_at_saved_spot(s32 index) __asm__("func_8008B54C");
#endif
s32 func_8008B54C(s32 index) {
    WorldmapActor *actor;
    s16 heading;

    actor = &xm_worldmap_actor_slots[index];
    actor->handle = xm_sprite_create(xm_worldmap_character_models[1], 0x110, 0x1E0, 0x150, 0x100, 0x40);
    xm_sprite_start_animation(actor->handle, 0);
    xm_sprite_set_scale(actor->handle, 0x1800);
    actor->handle->render.word &= ~SPRITE_HIDDEN;
    actor->position.vx = xm_game_data.worldmap.x << 12;
    actor->position.vz = xm_game_data.worldmap.z << 12;
    actor->position.vy = xm_worldmap_terrain_get_height(actor->position.vx, actor->position.vz);
    heading = xm_game_data.worldmap.heading;
    actor->unk24 = 1;
    actor->turn = 8;
    actor->state = 2;
    actor->motion.vz = 0;
    actor->motion.vy = 0;
    actor->motion.vx = 0;
    actor->unk58 = 0xF;
    actor->heading = heading;
    return 1;
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008B644);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_third_member_start (src/decomp/src/worldmap/worldmap_objects_effects_party.c:3279). */
#ifndef XM_S_worldmap_third_member_start
#define XM_S_worldmap_third_member_start
s32 xm_worldmap_third_member_start(s32 index) __asm__("func_8008BB40");
#endif
s32 func_8008BB40(s32 index) {
    WorldmapActor *actor;
    s32 result;

    actor = &xm_worldmap_actor_slots[index];
    result = 1;
    if (xm_game_data.party[2] != 0xFF) {
        actor->handle = xm_sprite_create(xm_worldmap_character_models[2], 0x120, 0x1E0, 0x160, 0x100, 0x40);
        xm_sprite_start_animation(actor->handle, 0);
        xm_sprite_set_scale(actor->handle, 0x1800);
        actor->handle->render.word &= ~SPRITE_HIDDEN;
        actor->unk24 = 0;
    } else {
        actor->unk24 = 1;
        result = 3;
    }
    actor->position.vx = xm_game_data.worldmap.x << 12;
    actor->position.vz = xm_game_data.worldmap.z << 12;
    actor->position.vy = xm_worldmap_terrain_get_height(actor->position.vx, actor->position.vz);
    actor->motion.vz = 0;
    actor->motion.vy = 0;
    actor->motion.vx = 0;
    actor->heading = xm_game_data.worldmap.heading;
    actor->turn = 8;
    actor->unk58 = 0x1E;
    actor->unk5C = actor->heading;
    switch (xm_worldmap_movement_mode) {
    case 1:
    case 2:
    case 3:
        if (xm_game_data.inGear[2] == 0) {
            actor->position.vx = xm_worldmap_player_position.vx;
            actor->position.vz = xm_worldmap_player_position.vz;
            actor->position.vy = xm_worldmap_terrain_get_height(actor->position.vx, actor->position.vz);
            actor->heading = xm_worldmap_player_heading;
        } else {
            actor->state = 1;
            actor->unk24 = 1;
        }
        break;
    case 4:
    case 5:
    case 6:
        actor->state = 3;
        actor->unk24 = 1;
        if (xm_game_data.party[2] != 0xFF) {
            xm_game_data.inGear[2] = 1;
        }
        break;
    case 7:
        actor->state = 2;
        actor->unk24 = 1;
        if (xm_game_data.party[2] != 0xFF) {
            xm_game_data.inGear[2] = 1;
        }
        break;
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
        break;
    }
    return result;
}
#endif


#ifndef XENO_PC_PORT
s32 func_8008BD1C(s32 arg0) {
    s32 var_s1;
    void *temp_a0;
    void *temp_s0;
    void *temp_v0;

    temp_s0 = (u8 *)D_8009BE24 + (arg0 << 7);
    var_s1 = 1;
    if (*(u8 *)((s8*)(&g_GameState) + 0x1D36) != 0xFF) {
        temp_v0 = func_80024524(D_8009CD3C, 0x120, 0x1E0, 0x160, 0x100, 0x40);
        *(void **)((s8*)(temp_s0) + 0x4C) = temp_v0;
        func_800245D8(temp_v0, 0);
        SpriteSetScale(*(void **)((s8*)(temp_s0) + 0x4C), 0x1800);
        temp_a0 = *(void **)((s8*)(temp_s0) + 0x4C);
        *(s32 *)((s8*)(temp_a0) + 0x3C) = (s32) (*(s32 *)((s8*)(temp_a0) + 0x3C) & ~4);
    } else {
        var_s1 = 3;
    }
    return var_s1;
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_third_member_start_at_saved_spot (src/decomp/src/worldmap/worldmap_objects_effects_party.c:3365). */
#ifndef XM_S_worldmap_third_member_start_at_saved_spot
#define XM_S_worldmap_third_member_start_at_saved_spot
s32 xm_worldmap_third_member_start_at_saved_spot(s32 index) __asm__("func_8008BDD0");
#endif
s32 func_8008BDD0(s32 index) {
    WorldmapActor *actor;
    s16 heading;

    actor = &xm_worldmap_actor_slots[index];
    actor->handle = xm_sprite_create(xm_worldmap_character_models[2], 0x120, 0x1E0, 0x160, 0x100, 0x40);
    xm_sprite_start_animation(actor->handle, 0);
    xm_sprite_set_scale(actor->handle, 0x1800);
    actor->handle->render.word &= ~SPRITE_HIDDEN;
    actor->position.vx = xm_game_data.worldmap.x << 12;
    actor->position.vz = xm_game_data.worldmap.z << 12;
    actor->position.vy = xm_worldmap_terrain_get_height(actor->position.vx, actor->position.vz);
    heading = xm_game_data.worldmap.heading;
    actor->unk24 = 1;
    actor->turn = 8;
    actor->state = 2;
    actor->motion.vz = 0;
    actor->motion.vy = 0;
    actor->motion.vx = 0;
    actor->unk58 = 0x1E;
    actor->heading = heading;
    return 1;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_actor_step_to_target (src/decomp/src/worldmap/worldmap_objects_effects_party.c:3391). */
#ifndef XM_T_ABS
#define XM_T_ABS
#define ABS(x) (((x)>=0)?(x):(-(x)))
#endif
#ifndef XM_S_worldmap_actor_step_to_target
#define XM_S_worldmap_actor_step_to_target
s32 xm_worldmap_actor_step_to_target(WorldmapActor *actor) __asm__("func_8008BEC8");
#endif
s32 func_8008BEC8(WorldmapActor *actor) {
    s32 arrived;
    s32 distance;
    s32 position;
    s32 cell;

    position = actor->position.vx;
    cell = position >> 12;
    distance = actor->u.step - cell;
    arrived = 0;
    if (ABS(distance) >= 5) {
        actor->position.vx = position + actor->motion.vx * (actor->turn / 2);
    } else {
        arrived = 1;
    }
    position = actor->position.vz;
    cell = position >> 12;
    distance = actor->unk54 - cell;
    if (ABS(distance) >= 5) {
        actor->position.vz = position + actor->motion.vz * (actor->turn / 2);
    } else {
        arrived |= 2;
    }
    xm_worldmap_wrap_position(&actor->position);
    actor->position.vy = xm_worldmap_terrain_get_height(actor->position.vx, actor->position.vz);
    return arrived;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_queue_actor_cylinder (src/decomp/src/worldmap/worldmap_objects_effects_party.c:3420). */
#ifndef XM_T_PlaceRequest
#define XM_T_PlaceRequest
typedef struct PlaceRequest {
    s16 actor;
    s16 pad2;
    s32 px, py, pz;
    s32 z;
    s16 x;
    s16 pad16;
} PlaceRequest;
#endif
#ifndef XM_S_worldmap_actor_cylinders
#define XM_S_worldmap_actor_cylinders
extern PlaceRequest xm_worldmap_actor_cylinders[32] __asm__("D_8009BE6C");
#endif
#ifndef XM_S_worldmap_actor_cylinder_count
#define XM_S_worldmap_actor_cylinder_count
extern s16 xm_worldmap_actor_cylinder_count __asm__("D_8009BD04");
#endif
void xm_worldmap_queue_actor_cylinder(s32 index, VECTOR *position, s32 x, s32 z) __asm__("func_8008BFD4");
void xm_worldmap_queue_actor_cylinder(s32 index, VECTOR *position, s32 x, s32 z) {
    PlaceRequest *request;

    request = &xm_worldmap_actor_cylinders[xm_worldmap_actor_cylinder_count];
    request->actor = index;
    request->px = position->vx;
    request->py = position->vy;
    request->pz = position->vz;
    request->z = (s16)z;
    request->x = x;
    xm_worldmap_actor_cylinder_count = (xm_worldmap_actor_cylinder_count + 1) & 0x1F;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): processWorldmapDynamicCollisions (src/worldmap/w_8008BD1C.c). */
#ifndef XM_T_sWorldmapDynamicCollision
#define XM_T_sWorldmapDynamicCollision
typedef struct {
     s16 m0_entityIndex;
     s32 m4_position[3];
     s32 m10_height;
     s16 m14_radius;           
} sWorldmapDynamicCollision;
#endif
#ifndef XM_S_processWorldmapDynamicCollisions
#define XM_S_processWorldmapDynamicCollisions
void xm_processWorldmapDynamicCollisions(VECTOR* position, s32 radius, s32 height, u8* outType, u8* outIndex) __asm__("func_8008C040");
#endif
#ifndef XM_S_worldmapDynamicCollisions
#define XM_S_worldmapDynamicCollisions
extern sWorldmapDynamicCollision xm_worldmapDynamicCollisions[32] __asm__("D_8009BE6C");
#endif
#ifndef XM_S_currentWorldmapDynamicCollisionSlot
#define XM_S_currentWorldmapDynamicCollisionSlot
extern s16 xm_currentWorldmapDynamicCollisionSlot __asm__("D_8009BD04");
#endif
#ifndef XM_S_adjustWorldModelLocation
#define XM_S_adjustWorldModelLocation
void xm_adjustWorldModelLocation(VECTOR* delta) __asm__("func_80093534");
#endif
void func_8008C040(VECTOR* position, s32 radius, s32 height, u8* outType, u8* outIndex) {
    s32 i;
    s32 top;
    s32 bottom;
    s32 dist;
    sWorldmapDynamicCollision* entry;
    VECTOR delta;
    VECTOR delta2;                 

    *outType = 0;
    *outIndex = 0;
    entry = xm_worldmapDynamicCollisions;
    if (xm_currentWorldmapDynamicCollisionSlot > 0) {
    for (i = 0; i < xm_currentWorldmapDynamicCollisionSlot; i++, entry++) {
        top = position->vy;
        if (top < entry->m4_position[1]) {
            bottom = top - (height << 12);
            top = entry->m4_position[1];
        } else {
            bottom = entry->m4_position[1] - (entry->m10_height << 12);
        }
        if ((top - bottom) >> 12 < height + entry->m10_height) {
            delta.vx = (entry->m4_position[0] - position->vx) >> 12;
            delta.vz = (entry->m4_position[2] - position->vz) >> 12;
            xm_adjustWorldModelLocation(&delta);
            dist = xm_SquareRoot0(delta.vx * delta.vx + delta.vz * delta.vz);
            if (dist < entry->m14_radius + radius) {
                *outType = 2;
                *outIndex = entry->m0_entityIndex;
                return;
            }
            if (dist < entry->m14_radius + radius + 0x10) {
                *outType = 1;
                *outIndex = entry->m0_entityIndex;
                return;
            }
        }
    }
    }
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_actor_emit_on_layer3 (src/decomp/src/worldmap/worldmap_objects_effects_party.c:3481). */
#ifndef XM_S_worldmap_actor_emit_on_layer3
#define XM_S_worldmap_actor_emit_on_layer3
void xm_worldmap_actor_emit_on_layer3(s32 effect, WorldmapActor *actor, ActorScratch *scratch) __asm__("func_8008C1DC");
#endif
void func_8008C1DC(s32 effect, WorldmapActor *actor, ActorScratch *scratch) {
    if (xm_worldmap_terrain_get_layer(&actor->position) == 3) {
        scratch->position.vx = actor->position.vx >> 12;
        scratch->position.vy = actor->position.vy >> 12;
        scratch->position.vz = actor->position.vz >> 12;
        scratch->angle.vz = 0;
        scratch->angle.vx = 0;
        scratch->angle.vy = actor->unk5C;
        xm_worldmap_effects_start_emitters(effect, &scratch->position, &scratch->angle);
        return;
    }
    xm_worldmap_effects_stop_emitters(effect);
}
#endif

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_create_gear_sprite (src/decomp/src/worldmap/worldmap_objects_effects_party.c:3496). */
#ifndef XM_S_worldmap_create_gear_sprite
#define XM_S_worldmap_create_gear_sprite
void xm_worldmap_create_gear_sprite(WorldmapActor *actor, s32 member) __asm__("func_8008C28C");
#endif
#ifndef XM_S_worldmap_gear_sprite_x
#define XM_S_worldmap_gear_sprite_x
extern s16 xm_worldmap_gear_sprite_x[3] __asm__("D_8009B18C");
#endif
#ifndef XM_S_worldmap_gear_sprite_y
#define XM_S_worldmap_gear_sprite_y
extern s16 xm_worldmap_gear_sprite_y[3] __asm__("D_8009B194");
#endif
#ifndef XM_S_worldmap_gear_sprite_width
#define XM_S_worldmap_gear_sprite_width
extern s16 xm_worldmap_gear_sprite_width[3] __asm__("D_8009B19C");
#endif
#ifndef XM_S_worldmap_gear_sprite_height
#define XM_S_worldmap_gear_sprite_height
extern s16 xm_worldmap_gear_sprite_height[3] __asm__("D_8009B1A4");
#endif
void func_8008C28C(WorldmapActor *actor, s32 member) {
    actor->handle = xm_sprite_create(xm_worldmap_gear_models[member], xm_worldmap_gear_sprite_x[member], xm_worldmap_gear_sprite_y[member],
                                  xm_worldmap_gear_sprite_width[member], xm_worldmap_gear_sprite_height[member], 0x40);
    if (xm_game_data.inGear[member] == 1) {
        xm_sprite_start_animation(actor->handle, 0);
    } else {
        xm_sprite_start_animation(actor->handle, 3);
    }
    xm_sprite_set_scale(actor->handle, 0x2000);
    actor->handle->render.word &= ~SPRITE_HIDDEN;
}
#endif
#else
/* FAKEMATCH: natural semantic reconstruction for port execution only. The
 * matching build keeps retail asm; see docs/FAKEMATCHES.md. */
void func_8008C28C(WmObj *arg0, s32 arg1) {
    u8 *package_table = (u8 *)PSX_ADDR(0x8009BDF8);
    s16 *tex_x = (s16 *)PSX_ADDR(0x8009B18C);
    s16 *tex_y = (s16 *)PSX_ADDR(0x8009B194);
    s16 *clut_x = (s16 *)PSX_ADDR(0x8009B19C);
    s16 *clut_y = (s16 *)PSX_ADDR(0x8009B1A4);
    u32 package_bits = *(u32 *)(package_table + arg1 * 4);
    void *sprite = func_80024524(
        package_bits ? PSX_ADDR(package_bits) : NULL,
        tex_x[arg1], tex_y[arg1], clut_x[arg1], clut_y[arg1], 0x40);

    arg0->unk4C = (s32)(intptr_t)sprite;
    func_800245D8(sprite, g_GameState[0x22B1 + arg1] == 1 ? 0 : 3);
    SpriteSetScale((void *)(intptr_t)arg0->unk4C, 0x2000);
    *(s32 *)((u8 *)(intptr_t)arg0->unk4C + 0x3C) &= ~4;
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008C364);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008C530);


#ifndef XENO_PC_PORT
s32 func_8008C6EC(s32 arg0) {
    s32 temp_a0;
    void *temp_s0;

    temp_s0 = (u8 *)D_8009BE24 + (arg0 << 7);
    temp_a0 = func_8008C364(temp_s0, 0);
    if (D_8009BE10 > 0) {
        if (D_8009BE10 >= 4) {
            if (D_8009BE10 < 8) {
                *(s16 *)((s8*)(temp_s0) + 0x24) = 1;
            }
        }
    }
    return temp_a0;
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): worldmapMode0_taskPlayerGear_initSaved (src/worldmap/w_8008BD1C.c). */
#ifndef XM_T_SpriteActor
#define XM_T_SpriteActor
typedef struct {
     SpriteActorCore m0_spriteActorCore;
     SpriteObject mB4_spriteObject;
     SpriteActorExtra mF4_extra;
     SpriteVramData m110_vramData;
     SubgroupTransform m124_subgroupTransforms[8];
} SpriteActor;
#endif
#ifndef XM_T_AnimationBundle
#define XM_T_AnimationBundle
typedef struct {
     s32 m0;
     s32 m4_animationsOffset;
     s32 m8_spriteDataOffset;
     s32 mC_offset;
} AnimationBundle;
#endif
#ifndef XM_T_GameStateCharacterSub4
#define XM_T_GameStateCharacterSub4
typedef struct {
     u8 m0_weaponPower;             
     u8 m1[3];
     u16 m4_nextWeaponStatusMask;            
     u8 m6_nextWeaponStatusChance;             
     u8 m7_nextWeaponStatusType;             
} GameStateCharacterSub4;
#endif
#ifndef XM_T_GameStateCharacter
#define XM_T_GameStateCharacter
typedef struct {
     u16 m0_weaponStatusMask;
     u8 m2_weaponStatusChance;
     u8 m3_weaponStatusType;
     GameStateCharacterSub4 m4_weaponSlots[4];
     u8 m24[4];
     u8 m28_attackBonus;
     u8 m29_defenceBonus;
     u8 m2A_agilityBonus;
     u8 m2B_etherBonus;
     u8 m2C_etherDefenceBonus;
     u8 m2D_accessoryDefence;
     u8 m2E_hitBonus;
     u8 m2F_evadeBonus;
     u8 m30_maxHpBonus;
     u8 m31_maxMpBonus;
     u16 m32_accessoryFlags;
     u16 m34_monsterFlags;          
     u16 m36_monsterFlags2;
     u16 m38_weaknessFlags;
     s16 m3A_killCount;
     s32 m3C_experience;
     s32 m40_experience2;
     s32 m44_expToNextLevel;
     s32 m48_expToNextLevel2;
     u16 m4C_HP;
     u16 m4E_MaxHP;
     u16 m50_MP;
     u16 m52_MaxMP;
     u8 m54_infinityCharge;
     u8 m55_deathblowLearnRate;
     u8 m56_battleCommandLoadout;
     u8 m57;
     u8 m58_attack;
     u8 m59_defence;
     u8 m5A_agility;
     u8 m5B_ether;
     u8 m5C_etherDefence;
     u8 m5D;
     u8 m5E_hitPercentage;
     u8 m5F_evadePercentage;
     u8 m60_effectChance;
     u8 m61;
     u8 m62_Level;
     u8 m63_Level2;
     u8 m64;
     u8 m65;
     u8 m66;
     u8 m67;
     u8 m68[2];
     u8 m6A_weaponModifier[5]; 
     u8 m6F_equipment[5];      
     u8 m74_accessorySlots[3];
     u8 m77_baseWeight;
     u8 m78_weightGain;
     u8 m79_weightTenths;
     u16 m7A_commandEnabledBF;
     u16 m7C_statusFlags[4];       

     u16 m84_statusFlags2[6];       
     u16 m90_deathblowProgress[8];
     u8 mA0_gearNum;
     u8 mA1_deathblowLearnBonus;
     u8 mA2[2];
} GameStateCharacter;
#endif
#ifndef XM_T_GameStateGearSlot
#define XM_T_GameStateGearSlot
typedef struct {
     u8 m0_weaponPower;             
     u8 m1_weaponStatusChance;
     u8 m2_weaponStatusType;             
     u8 m3_weaponId;
     u8 m4[2];
     u16 m6_nextWeaponStatusMask; 
} GameStateGearSlot;
#endif
#ifndef XM_T_GameStateGear
#define XM_T_GameStateGear
typedef struct {
     u8 m0[2];
     u8 m2_frame;           
     u8 m3_engine;          
     u8 m4_slotItem[4];     
     u8 m8_armor;           
     u8 m9_accessorySlots[3];
     u8 mC_weapon[4];       

     s16 m10_weaponStatusMask;
     GameStateGearSlot m12_slots[4]; 
     u8 m32[0x38 - 0x32];
     u16 m38_fuel;    
     u16 m3A_maxFuel;
     u8 m3C_engineOutput;
     u8 m3D[2];
     u8 m3F_attackMultiplier;
     u16 m40_accessoryDefence;               
     u16 m42_accessoryEtherDefence;
     u16 m44_accessoryWeight;
     u16 m46;               
     u16 m48;
     u8 m4A_agilityPenalty;
     u8 m4B;
     u8 m4C;
     u8 m4D_accessoryResponse;
     u8 m4E_accessoryAgility;
     u8 m4F_frameRepairLevel;
     u8 m50_terrainAgility[4];
     u8 m54_accessoryEtherAmp;
     u8 m55_accessoryBonuses[3];             
     u8 m58[0x5C - 0x58];
     u8 m5C_extraFileIndex;
     u8 m5D_weaponAnimOffsets[3];
     s32 m60_hp;
     s32 m64_maxHp;
     u16 m68_weight;
     u16 m6A;
     u16 m6C;
     u16 m6E_elementResistanceMask;
     u16 m70_defence;
     u16 m72_etherDefence;
     u8 m74_baseAttackMultiplier;
     u8 m75_weightAllowance;
     u8 m76[0x7C - 0x76];
     u16 m7C_statusFlags;         
     u16 m7E_statusFlags1;
     u16 m80_statusFlags2;
     u16 m82_statusFlags3;
     u16 m84_statusFlags4;
     u16 m86_statusFlags5;
     u8 m88_elementResistances[0x98 - 0x88];   
     u8 m98_agility;
     u8 m99_defenceDown;
     u8 m9A[2];
     u8 m9C;
     u8 m9D;
     u8 m9E_etherAmp;
     u8 m9F_response;
     u8 mA0[0xA4 - 0xA0];
} GameStateGear;
#endif
#ifndef XM_T_GameStateSize20
#define XM_T_GameStateSize20
typedef struct {
     u16 m0_unlockedAbilitiesBitField[8];
     u8 m10[0x17 - 0x10];
     u8 m17_energyPerTurn; 
     u8 m18[2];
     u16 m1A_unlockedGearAbilities;
     u8 m1C[4];
} GameStateSize20;
#endif
#ifndef XM_T_GameState
#define XM_T_GameState
typedef struct {
     u8 m0_names[31][0x14];
     GameStateCharacter m26C_party[11];
     GameStateGear m978_gears[20];
     u8 m1648[0x16C0 - 0x1648];
     GameStateSize20 m16C0_abilities[11];
     u16 m1820_worldmapPosition[6]; 
     u16 m182C_vehiclePosition[4];  
     u16 m1834_vehicleMode;
     u16 m1836_restoreWorldmapPosition;      
     u8 m1838[0x183C - 0x1838];
     u16 m183C_isOnGearBackup[3]; 
     u16 m1842_disableWorldmapMinimap;
     u16 m1844_waypointShipPosition[2];
     u16 m1848_waypointShipWaypoint;
     u16 m184A_waypointShipPlaced;
     u16 m184C_flyingShipXLow;
     u16 m184E_flyingShipPosition[5];  
     u8 m1858[0x1924 - 0x1858];
     s32 m1924_gold;
     u8 m1928[0x1930 - 0x1928];
     u16 m1930_fieldVarsBackup[512];
     u16 m1D30_partyMemberBitField;
     u16 m1D32_partyFrameMask;
     u8 m1D34_currentParty[3];
     u8 m1D37;
    

     u8 m1D38_inventory1Counts[100];
     u8 m1D9C_inventory1Ids[100];
     u8 m1E00_inventory2Counts[200];
     u8 m1EC8_inventory2Ids[200];
     u8 m1F90_inventory0Counts[150];
     u8 m2026_inventory0Ids[150];
     u8 m20BC_inventory3Counts[100];
     u8 m2120_inventory3Ids[100];
     u8 m2184_inventory4Counts[150];
     u8 m221A_inventory4Ids[150];
     u8 m22B0;
     u8 m22B1_isOnGear[3];  
     u8 m22B4[0x22B6 - 0x22B4];
     u16 m22B6_gameFlags;
     u8 m22B8_ammoCounts[0x2318 - 0x22B8]; 

     u16 m2318_partyFrameLock;
     u16 m231A_fieldID;
     u16 m231C_cameraYaw;
     s16 m231E_worldmapInitialPositionIndex;
     u16 m2320_worldmapMode;
     u16 m2322_previouslyPlayingMusic;
     u16 m2324_battleScriptVars[16]; 
     u8 m2344[0x2358 - 0x2344];
} GameState;
#endif
#ifndef XM_S_worldmapMode0_taskPlayerGear_initSaved
#define XM_S_worldmapMode0_taskPlayerGear_initSaved
s32 xm_worldmapMode0_taskPlayerGear_initSaved(s32 index) __asm__("func_8008C75C");
#endif
#ifndef XM_S_createSpriteActor
#define XM_S_createSpriteActor
SpriteActor* xm_createSpriteActor(AnimationBundle* bundle, s16 clutX, s16 clutY, s16 vramX, s16 vramY, s16 arg5) __asm__("func_80024524");
#endif
#ifndef XM_S_worldmapPartyGearSprites
#define XM_S_worldmapPartyGearSprites
extern AnimationBundle * xm_worldmapPartyGearSprites[3] __asm__("D_8009BDF8");
#endif
#ifndef XM_S_spriteActorSetPlayingAnimation
#define XM_S_spriteActorSetPlayingAnimation
void xm_spriteActorSetPlayingAnimation(SpriteActorCore* actor, s32 animationId) __asm__("func_800245D8");
#endif
#ifndef XM_S_setGraphicEntityScale
#define XM_S_setGraphicEntityScale
void xm_setGraphicEntityScale(SpriteActorCore* actor, s32 scale) __asm__("SpriteSetScale");
#endif
#ifndef XM_S_gameState
#define XM_S_gameState
extern GameState xm_gameState __asm__("g_GameState");
#endif
#ifndef XM_S_worldmapGetAltitudeFor2dPoint
#define XM_S_worldmapGetAltitudeFor2dPoint
s32 xm_worldmapGetAltitudeFor2dPoint(s32 x, s32 z) __asm__("func_80093978");
#endif
s32 func_8008C75C(s32 index) {
    sWorldmapStateEntry* entry = &xm_gWorldmapState->m0_entries[index];
    s16 direction;

    entry->m4C_sprite = &xm_createSpriteActor(xm_worldmapPartyGearSprites[0], 0x100, 0x1FD, 0x140, 0x140, 0x40)->m0_spriteActorCore;
    xm_spriteActorSetPlayingAnimation(entry->m4C_sprite, 0);
    xm_setGraphicEntityScale(entry->m4C_sprite, 0x2000);
    entry->m4C_sprite->m3C_x2 = 0;
    entry->m28_position.vx = (u16)xm_gameState.m1930_fieldVarsBackup[0x16] << 12;
    entry->m28_position.vz = (u16)xm_gameState.m1930_fieldVarsBackup[0x17] << 12;
    entry->m28_position.vy = xm_worldmapGetAltitudeFor2dPoint(entry->m28_position.vx, entry->m28_position.vz);
    direction = xm_gameState.m1820_worldmapPosition[3];
    entry->m24_hidden = 1;
    entry->m38_step.vz = 0;
    entry->m38_step.vy = 0;
    entry->m38_step.vx = 0;
    entry->m4A_stepScale = 0xC;
    entry->m48_direction = direction;
    return 1;
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008C844);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_second_vehicle_start (src/decomp/src/worldmap/worldmap_vehicles.c:462). */
#ifndef XM_S_worldmap_second_vehicle_start
#define XM_S_worldmap_second_vehicle_start
s32 xm_worldmap_second_vehicle_start(s32 index) __asm__("func_8008D3F0");
#endif
#ifndef XM_S_worldmap_place_vehicle
#define XM_S_worldmap_place_vehicle
s32 xm_worldmap_place_vehicle(WorldmapActor *actor, s32 member) __asm__("func_8008C364");
#endif
s32 func_8008D3F0(s32 index) {
    WorldmapActor *actor;
    s32 result;

    actor = &xm_worldmap_actor_slots[index];
    result = xm_worldmap_place_vehicle(actor, 1);
    actor->motion.vz = 0;
    actor->motion.vy = 0;
    actor->motion.vx = 0;
    actor->heading = xm_game_data.worldmap.unk5C;
    actor->turn = 0xC;
    actor->unk58 = 0xF;
    actor->unk5C = actor->heading;
    if (xm_worldmap_movement_mode > 0) {
        if (xm_worldmap_movement_mode < 4) {
            if (xm_game_data.inGear[1] == 1) {
                actor->state = 1;
                actor->position.vx = xm_worldmap_player_position.vx;
                actor->position.vz = xm_worldmap_player_position.vz;
                actor->position.vy = xm_worldmap_terrain_get_height(actor->position.vx, actor->position.vz);
                actor->heading = xm_worldmap_player_heading;
            }
        } else if (xm_worldmap_movement_mode < 8) {
            actor->state = 2;
            actor->unk24 = 1;
        }
    }
    VEHICLE_SPOTS[1].x = actor->position.vx >> 12;
    VEHICLE_SPOTS[1].z = actor->position.vz >> 12;
    xm_game_data.worldmap.unk5C = actor->heading;
    return result;
}
#endif


#ifndef XENO_PC_PORT
s32 func_8008D520(s32 arg0) {
    s32 temp_a0;
    void *temp_s0;

    temp_s0 = (u8 *)D_8009BE24 + (arg0 << 7);
    temp_a0 = func_8008C364(temp_s0, 1);
    if (D_8009BE10 > 0) {
        if (D_8009BE10 >= 4) {
            if (D_8009BE10 < 8) {
                *(s16 *)((s8*)(temp_s0) + 0x24) = 1;
            }
        }
    }
    return temp_a0;
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): worldmapMode0_task_partyGear1_initSaved (src/worldmap/w_8008BD1C.c). */
#ifndef XM_S_worldmapMode0_task_partyGear1_initSaved
#define XM_S_worldmapMode0_task_partyGear1_initSaved
s32 xm_worldmapMode0_task_partyGear1_initSaved(s32 index) __asm__("func_8008D590");
#endif
s32 func_8008D590(s32 index) {
    sWorldmapStateEntry* entry = &xm_gWorldmapState->m0_entries[index];
    s16 direction;

    entry->m4C_sprite = &xm_createSpriteActor(xm_worldmapPartyGearSprites[1], 0x100, 0x1FC, 0x160, 0x140, 0x40)->m0_spriteActorCore;
    xm_spriteActorSetPlayingAnimation(entry->m4C_sprite, 0);
    xm_setGraphicEntityScale(entry->m4C_sprite, 0x2000);
    entry->m4C_sprite->m3C_x2 = 0;
    entry->m28_position.vx = (u16)xm_gameState.m1930_fieldVarsBackup[1 * 3 + 0x16] << 12;
    entry->m28_position.vz = (u16)xm_gameState.m1930_fieldVarsBackup[1 * 3 + 0x17] << 12;
    entry->m28_position.vy = xm_worldmapGetAltitudeFor2dPoint(entry->m28_position.vx, entry->m28_position.vz);
    direction = xm_gameState.m1820_worldmapPosition[3 + 1];
    entry->m24_hidden = 1;
    entry->m38_step.vz = 0;
    entry->m38_step.vy = 0;
    entry->m38_step.vx = 0;
    entry->m4A_stepScale = 0xC;
    entry->m48_direction = direction;
    return 1;
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008D678);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_third_vehicle_start (src/decomp/src/worldmap/worldmap_vehicles.c:738). */
#ifndef XM_S_worldmap_third_vehicle_start
#define XM_S_worldmap_third_vehicle_start
s32 xm_worldmap_third_vehicle_start(s32 index) __asm__("func_8008DD6C");
#endif
s32 func_8008DD6C(s32 index) {
    WorldmapActor *actor;
    s32 result;

    actor = &xm_worldmap_actor_slots[index];
    result = xm_worldmap_place_vehicle(actor, 2);
    actor->motion.vz = 0;
    actor->motion.vy = 0;
    actor->motion.vx = 0;
    actor->heading = xm_game_data.worldmap.unk5E;
    actor->turn = 0xC;
    actor->unk58 = 0x1F;
    actor->unk5C = actor->heading;
    if (xm_worldmap_movement_mode > 0) {
        if (xm_worldmap_movement_mode < 4) {
            if (xm_game_data.inGear[2] == 1) {
                actor->state = 1;
                actor->position.vx = xm_worldmap_player_position.vx;
                actor->position.vz = xm_worldmap_player_position.vz;
                actor->position.vy = xm_worldmap_terrain_get_height(actor->position.vx, actor->position.vz);
                actor->heading = xm_worldmap_player_heading;
            }
        } else if (xm_worldmap_movement_mode < 8) {
            actor->state = 2;
            actor->unk24 = 1;
        }
    }
    VEHICLE_SPOTS[2].x = actor->position.vx >> 12;
    VEHICLE_SPOTS[2].z = actor->position.vz >> 12;
    xm_game_data.worldmap.unk5E = actor->heading;
    return result;
}
#endif


#ifndef XENO_PC_PORT
s32 func_8008DE9C(s32 arg0) {
    s32 temp_a0;
    void *temp_s0;

    temp_s0 = (u8 *)D_8009BE24 + (arg0 << 7);
    temp_a0 = func_8008C364(temp_s0, 2);
    if (D_8009BE10 > 0) {
        if (D_8009BE10 >= 4) {
            if (D_8009BE10 < 8) {
                *(s16 *)((s8*)(temp_s0) + 0x24) = 1;
            }
        }
    }
    return temp_a0;
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): worldmapMode0_task_partyGear2_initSaved (src/worldmap/w_8008BD1C.c). */
#ifndef XM_S_worldmapMode0_task_partyGear2_initSaved
#define XM_S_worldmapMode0_task_partyGear2_initSaved
s32 xm_worldmapMode0_task_partyGear2_initSaved(s32 index) __asm__("func_8008DF0C");
#endif
s32 func_8008DF0C(s32 index) {
    sWorldmapStateEntry* entry = &xm_gWorldmapState->m0_entries[index];
    s16 direction;

    entry->m4C_sprite = &xm_createSpriteActor(xm_worldmapPartyGearSprites[2], 0x100, 0x1FB, 0x280, 0x100, 0x40)->m0_spriteActorCore;
    xm_spriteActorSetPlayingAnimation(entry->m4C_sprite, 0);
    xm_setGraphicEntityScale(entry->m4C_sprite, 0x2000);
    entry->m4C_sprite->m3C_x2 = 0;
    entry->m28_position.vx = (u16)xm_gameState.m1930_fieldVarsBackup[2 * 3 + 0x16] << 12;
    entry->m28_position.vz = (u16)xm_gameState.m1930_fieldVarsBackup[2 * 3 + 0x17] << 12;
    entry->m28_position.vy = xm_worldmapGetAltitudeFor2dPoint(entry->m28_position.vx, entry->m28_position.vz);
    direction = xm_gameState.m1820_worldmapPosition[3 + 2];
    entry->m24_hidden = 1;
    entry->m38_step.vz = 0;
    entry->m38_step.vy = 0;
    entry->m38_step.vx = 0;
    entry->m4A_stepScale = 0xC;
    entry->m48_direction = direction;
    return 1;
}
#endif


#ifndef XENO_PC_PORT
void func_8008DFF4(void *arg0) {
    *(s32 *)((s8*)(arg0) + 0) = (s32) (*(s16 *)((s8*)(&g_GameState) + 0x182C) << 0xC);
    *(s32 *)((s8*)(arg0) + 4) = (s32) (*(s16 *)((s8*)(&g_GameState) + 0x182E) << 0xC);
    *(s32 *)((s8*)(arg0) + 8) = (s32) (*(s16 *)((s8*)(&g_GameState) + 0x1830) << 0xC);
}
#endif



#ifndef XENO_PC_PORT
void func_8008E034(void *arg0) {
    *(s16 *)((s8*)(&g_GameState) + 0x182C) = (s16) ((s32) *(s32 *)((s8*)(arg0) + 0) >> 0xC);
    *(s16 *)((s8*)(&g_GameState) + 0x182E) = (s16) ((s32) *(s32 *)((s8*)(arg0) + 4) >> 0xC);
    *(s16 *)((s8*)(&g_GameState) + 0x1830) = (s16) ((s32) *(s32 *)((s8*)(arg0) + 8) >> 0xC);
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008E078);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_find_clear_heading (src/decomp/src/worldmap/worldmap_vehicles.c:847). */
#ifndef XM_S_worldmap_find_clear_heading
#define XM_S_worldmap_find_clear_heading
s32 xm_worldmap_find_clear_heading(VECTOR *position, s32 unused, s32 range) __asm__("func_8008E0F0");
#endif
#ifndef XM_S_gpu_get_sin
#define XM_S_gpu_get_sin
s32 xm_gpu_get_sin(s32 angle) __asm__("rcos");
#endif
#ifndef XM_S_gpu_get_cos
#define XM_S_gpu_get_cos
s32 xm_gpu_get_cos(s32 angle) __asm__("rsin");
#endif
#ifndef XM_S_worldmap_move_walking
#define XM_S_worldmap_move_walking
s32 xm_worldmap_move_walking(VECTOR *position, VECTOR *direction, VECTOR *out, s32 scale, s32 mode) __asm__("func_80095414");
#endif
s32 func_8008E0F0(VECTOR *position, s32 unused, s32 range) {
    VECTOR hit;
    VECTOR direction;
    s32 heading;

    for (heading = 0; heading < 0x1000; heading += 0x100) {
        direction.vx = xm_gpu_get_sin(heading);
        direction.vz = -xm_gpu_get_cos(heading);
        if (xm_worldmap_move_walking(position, &direction, &hit, range, 2) == 1) {
            return heading;
        }
    }
    return -1;
}
#endif
INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008E190);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008E4F4);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_flying_vehicle_start_airborne (src/decomp/src/worldmap/worldmap_flying_vehicle.c:184). */
#ifndef XM_S_worldmap_flying_vehicle_start_airborne
#define XM_S_worldmap_flying_vehicle_start_airborne
s32 xm_worldmap_flying_vehicle_start_airborne(s32 index) __asm__("func_8008E680");
#endif
s32 func_8008E680(s32 index) {
    WorldmapActor *actor;

    actor = &xm_worldmap_actor_slots[index];
    xm_worldmap_load_flying_vehicle_position(&actor->position);
    actor->position.vy = 0x80000;
    actor->heading = xm_game_data.worldmap.vehicle_heading;
    actor->turn = 0x20;
    actor->unk74 = 3;
    actor->unk68 = -0x280000;
    actor->motion.vz = 0;
    actor->motion.vy = 0;
    actor->motion.vx = 0;
    actor->unk24 = 0;
    actor->unk60 = 0;
    actor->unk64 = 0;
    actor->unk6C = 0;
    actor->state = 8;
    xm_worldmap_camera_follow_target.target = actor->position;
    xm_worldmap_camera_follow_heading = actor->heading;
    xm_worldmap_objects[0].position = actor->position;
    return 1;
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008E76C);

#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): worldmap_taskYggdrasilSubModels_init (src/worldmap/w_8008E034.c). */
#ifndef XM_S_worldmap_taskYggdrasilSubModels_init
#define XM_S_worldmap_taskYggdrasilSubModels_init
s32 xm_worldmap_taskYggdrasilSubModels_init(s32 index) __asm__("func_800906E0");
#endif
#ifndef XM_S_attachWorldmapModels
#define XM_S_attachWorldmapModels
void xm_attachWorldmapModels(s32 parent, s32 child) __asm__("func_800848B4");
#endif
#ifndef XM_S_worldMapGearMode
#define XM_S_worldMapGearMode
extern s32 xm_worldMapGearMode __asm__("D_8009BE10");
#endif
s32 func_800906E0(s32 index) {
    sWorldmapStateEntry* entry;

    xm_attachWorldmapModels(0, 2);
    xm_attachWorldmapModels(0, 3);
    entry = &xm_gWorldmapState->m0_entries[index];
    entry->m20_subState = 0;
    entry->m28_position = xm_worldmapModels[2].m8_position;
    entry->m38_step = xm_worldmapModels[3].m8_position;
    entry->m50 = 0;
    entry->m54 = 0x40;
    if (xm_worldMapGearMode < 8) {
        if (xm_worldMapGearMode >= 4) {
            entry->m20_subState = 3;
            entry->m5C = 0x80;
            entry->m58 = 0x80;
        }
    }
    return 1;
}
#endif


#ifndef XENO_PC_PORT
s32 func_800907C4(void) {
    func_800848B4(0, 2);
    func_800848B4(0, 3);
    return 1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800907F4);


#ifndef XENO_PC_PORT
void func_80090A18(void) {
    if ((D_8009CD4C & 1) && (D_8009CD4C & 2)) {
        D_8009CEC0 = 1;
    } else {
        D_8009CEC0 = 0;
    }
    D_8009BD34 = (D_8009CEC0 ^ D_8009C7E8) & D_8009CEC0;
    D_8009C7E8 = D_8009CEC0;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80090A84);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80090C68);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80090E14);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80090FB4);

#ifndef XENO_PC_PORT
extern WmVec D_8009D55C_vec asm("D_8009D55C");
extern s16 D_8009BD3A_arr[] asm("D_8009BD3A");
extern u16 D_8009D52C;
#endif
s32 func_80091430(s32 arg0) {
    WmObj *obj;

    obj = &D_8009BE24[arg0];
    *(WmVec *)obj->pos = D_8009D55C_vec;
    D_8009BD3C = 0;
    D_8009BD3A_arr[0] = D_8009D52C;
    obj->unk50 = (s16)D_8009D52C;
    obj->unk58 = D_8009BD3A_arr[0] << 0xC;
    if (D_8009BE10 < 8) {
        if (D_8009BE10 >= 6) {
            obj->unk20 = 3;
        }
    }
    return 1;
}

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800914D0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80091B54);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80091C18);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_camera_pick_pitch (src/decomp/src/worldmap/worldmap_steering_camera_terrain.c:723). */
#ifndef XM_S_worldmap_camera_pick_pitch
#define XM_S_worldmap_camera_pick_pitch
s32 xm_worldmap_camera_pick_pitch(s32 current, s16 *pitches, s16 *heights) __asm__("func_80091FF8");
#endif
#ifndef XM_S_worldmap_camera_orbit_distances
#define XM_S_worldmap_camera_orbit_distances
extern s32 xm_worldmap_camera_orbit_distances[4] __asm__("D_8009B214");
#endif
#ifndef XM_S_worldmap_terrain_get_cell
#define XM_S_worldmap_terrain_get_cell
u8 *xm_worldmap_terrain_get_cell(s32 x, s32 z) __asm__("func_80093660");
#endif
#ifndef XM_S_worldmap_camera_ground_margins
#define XM_S_worldmap_camera_ground_margins
extern s16 xm_worldmap_camera_ground_margins[4] __asm__("D_8009B234");
#endif
s32 func_80091FF8(s32 current, s16 *pitches, s16 *heights) {
    ActorScratch *scratch;
    s32 *distance;
    s32 i;
    s32 base;
    s32 left;
    s32 lowest;
    s32 floor;
    s32 row;
    s32 column;
    s32 value;
    s32 offsetZ;
    s32 gap;

    scratch = (ActorScratch *)0x1F800000;
    distance = xm_worldmap_camera_orbit_distances;
    scratch->angle.vy = xm_worldmap_camera_angle.vy;
    base = xm_worldmap_camera_follow_target.target.vy >> 12;
    scratch->angle.vz = xm_worldmap_camera_angle.vz;
    for (i = 0; i < 3; i++, pitches++, distance++, heights++) {
        scratch->angle.vx = *pitches;
        xm_worldmap_camera_place_orbit(&xm_worldmap_view_setup, &xm_worldmap_camera, *distance - ((xm_worldmap_projection_distance / 2) << 12), &scratch->angle);
        left = xm_worldmap_camera.target.vx + ((VIEW.eye.vx - 0x180) << 12);
        offsetZ = (VIEW.eye.vz + 0x180) << 12;
        value = xm_worldmap_camera.target.vz - offsetZ;
        left &= 0xFFF80000;
        value &= 0xFFF80000;
        scratch->work.vz = value;
        lowest = 0;
        for (row = 0; row < 6; row++) {
            scratch->work.vx = left;
            for (column = 0; column < 6; column++) {
                xm_worldmap_wrap_position(&scratch->work);
                value = *(s8 *)xm_worldmap_terrain_get_cell(scratch->work.vx, scratch->work.vz);
                scratch->work.vy = value;
                if (value < lowest) {
                    lowest = value;
                }
                scratch->work.vx += 0x80000;
            }
            scratch->work.vz += 0x80000;
        }
        floor = lowest * 8;
        if (*heights + base + 0x50 < floor) {
            break;
        }
    }
    if (i < current) {
        floor -= 0x50;
        gap = xm_worldmap_camera_ground_margins[current] + base - floor;
        if (gap < 0) {
            gap = -gap;
        }
        if (gap < 0x41) {
            i = current;
        }
    }
    return i;
}
#endif


#ifndef XENO_PC_PORT
s32 func_80092234(s32 arg0) {
    void *temp_a0;

    temp_a0 = (u8 *)D_8009BE24 + (arg0 << 7);
    if (D_8009BE10 > 0) {
        if (D_8009BE10 < 6) {
            D_8009BE0C = 0x8C;
        } else if (D_8009BE10 < 8) {
            D_8009BE0C = 0x78;
            *(s16 *)((s8*)(temp_a0) + 0x20) = 1;
        }
    }
    *(s32 *)((s8*)(temp_a0) + 0x50) = (s32) (D_8009BE0C << 0xC);
    return 1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800922AC);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_screen_fade_start (src/decomp/src/worldmap/worldmap_steering_camera_terrain.c:850). */
#ifndef XM_T_DR_TPAGE
#define XM_T_DR_TPAGE
typedef struct {
    u_long tag;
    u_long code[1];
} DR_TPAGE;
#endif
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
#ifndef XM_S_worldmap_screen_fade_start
#define XM_S_worldmap_screen_fade_start
s32 xm_worldmap_screen_fade_start(s32 index) __asm__("func_800923A8");
#endif
#ifndef XM_S_SetDrawTPage
#define XM_S_SetDrawTPage
void xm_SetDrawTPage(DR_TPAGE *p, int dfe, int dtd, int tpage) __asm__("SetDrawTPage");
#endif
#ifndef XM_S_worldmap_screen_fade_blend
#define XM_S_worldmap_screen_fade_blend
extern DR_TPAGE xm_worldmap_screen_fade_blend __asm__("D_8009D310");
#endif
#ifndef XM_S_worldmap_screen_fade_quads
#define XM_S_worldmap_screen_fade_quads
extern POLY_G4 xm_worldmap_screen_fade_quads[2] __asm__("D_8009CE6C");
#endif
s32 func_800923A8(s32 index) {
    WorldmapActor *actor;

    actor = &xm_worldmap_actor_slots[index];
    actor->u.step = 0xF8;
    xm_SetDrawTPage(&xm_worldmap_screen_fade_blend, 1, 0, xm_GetTPage(0, xm_worldmap_screen_fade_rate, 0x380, 0x100));
    setlen(&xm_worldmap_screen_fade_quads[0], 8);
    xm_worldmap_screen_fade_quads[0].code = 0x38;
    xm_worldmap_screen_fade_quads[0].r0 = actor->u.step;
    xm_worldmap_screen_fade_quads[0].g0 = actor->u.step;
    xm_worldmap_screen_fade_quads[0].b0 = actor->u.step;
    xm_worldmap_screen_fade_quads[0].r1 = actor->u.step;
    xm_worldmap_screen_fade_quads[0].g1 = actor->u.step;
    xm_worldmap_screen_fade_quads[0].b1 = actor->u.step;
    xm_worldmap_screen_fade_quads[0].r2 = actor->u.step;
    xm_worldmap_screen_fade_quads[0].g2 = actor->u.step;
    xm_worldmap_screen_fade_quads[0].b2 = actor->u.step;
    xm_worldmap_screen_fade_quads[0].r3 = actor->u.step;
    xm_worldmap_screen_fade_quads[0].g3 = actor->u.step;
    xm_worldmap_screen_fade_quads[0].b3 = actor->u.step;
    xm_SetSemiTrans(&xm_worldmap_screen_fade_quads[0], 1);
    xm_worldmap_screen_fade_quads[1] = xm_worldmap_screen_fade_quads[0];
    xm_worldmap_screen_fade_quads[0].x0 = 0;
    xm_worldmap_screen_fade_quads[0].y0 = 0;
    xm_worldmap_screen_fade_quads[0].x1 = 0x140;
    xm_worldmap_screen_fade_quads[0].y1 = 0;
    xm_worldmap_screen_fade_quads[0].x2 = 0;
    xm_worldmap_screen_fade_quads[0].y2 = 0xD8;
    xm_worldmap_screen_fade_quads[0].x3 = 0x140;
    xm_worldmap_screen_fade_quads[0].y3 = 0xD8;
    xm_worldmap_screen_fade_quads[1].x0 = 0;
    xm_worldmap_screen_fade_quads[1].y0 = 0;
    xm_worldmap_screen_fade_quads[1].x1 = 0x140;
    xm_worldmap_screen_fade_quads[1].y1 = 0;
    xm_worldmap_screen_fade_quads[1].x2 = 0;
    xm_worldmap_screen_fade_quads[1].y2 = 0xD8;
    xm_worldmap_screen_fade_quads[1].x3 = 0x140;
    xm_worldmap_screen_fade_quads[1].y3 = 0xD8;
    return 1;
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800925A0);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_path_window_open (src/decomp/src/worldmap/worldmap_steering_camera_terrain.c:951). */
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
#ifndef XM_T_WindowLine
#define XM_T_WindowLine
typedef struct {
    SPRT sprite[2][2];
    RECT rect;       
    s16 width;       
    u8 plane;        
    u8 slot;
    u8 row;          
    u8 unk5D;
    u16 clut;
} WindowLine;
#endif
#ifndef XM_T_WindowQueue
#define XM_T_WindowQueue
typedef struct WindowQueue {
    struct WindowQueue *next;
    s32 message;
} WindowQueue;
#endif
#ifndef XM_T_Window
#define XM_T_Window
typedef struct Window {
    s16 x;          
    s16 y;          
    s16 unk4;
    s16 unk6;
    s16 unk8;
    s16 width;
    s16 lines;
    s16 unkE;
    u16 flags;
    s16 stride;
    s16 unk14;
    s16 unk16;
    s16 unk18;      
    u8 unk1A[2];
    u8 *text;
    u8 *resume;     
    u8 unk24[4];
    WindowLine *layout;
    void *image;
    u8 unk30[0xC];  
    u8 unk3C[0xC];  
    TILE tile[2];   
    u8 unk68;
    u8 unk69;
    u8 unk6A;
    u8 unk6B;
    u8 unk6C;
    u8 unk6D;
    u8 unk6E;
    u8 unk6F;
    s32 values[4];  
    s16 selection; 
    s16 queued;
    s16 unk84;      
    s16 unk86;      
    s16 unk88;      
    u8 unk8A[2];
    WindowQueue *queue;
} Window;
#endif
#ifndef XM_S_worldmap_path_window_open
#define XM_S_worldmap_path_window_open
s32 xm_worldmap_path_window_open(void) __asm__("func_80092BE4");
#endif
#ifndef XM_S_worldmap_path_name_id
#define XM_S_worldmap_path_name_id
extern s16 xm_worldmap_path_name_id __asm__("D_8009BD24");
#endif
#ifndef XM_S_window_open
#define XM_S_window_open
void xm_window_open(Window *window, s32 vram_x, s32 vram_y, s32 x, s32 y, s32 columns, s32 rows) __asm__("func_80032F54");
#endif
#ifndef XM_S_worldmap_path_window
#define XM_S_worldmap_path_window
extern Window xm_worldmap_path_window __asm__("D_8009D498");
#endif
#ifndef XM_S_window_reset_if_idle
#define XM_S_window_reset_if_idle
void xm_window_reset_if_idle(Window *window) __asm__("func_80034614");
#endif
s32 func_80092BE4(void) {
    xm_worldmap_path_name_id = -1;
    xm_window_open(&xm_worldmap_path_window, 0x3C0, 0x180, 0xA0, 0x78, 0x20, 1);
    xm_worldmap_path_window.unk68 = 8;
    xm_worldmap_path_window.flags |= 2;
    xm_window_reset_if_idle(&xm_worldmap_path_window);
    return 1;
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80092C70);


#ifndef XENO_PC_PORT
void func_80092DD0(void) {
    func_800346D4(&D_8009D498);
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_destination_window_open (src/decomp/src/worldmap/worldmap_steering_camera_terrain.c:995). */
#ifndef XM_T_setXY4
#define XM_T_setXY4
#define setXY4(p, _x0, _y0, _x1, _y1, _x2, _y2, _x3, _y3) \
    (p)->x0 = _x0, (p)->y0 = _y0, (p)->x1 = _x1, (p)->y1 = _y1, \
    (p)->x2 = _x2, (p)->y2 = _y2, (p)->x3 = _x3, (p)->y3 = _y3
#endif
#ifndef XM_S_worldmap_destination_window_open
#define XM_S_worldmap_destination_window_open
s32 xm_worldmap_destination_window_open(void) __asm__("func_80092DF8");
#endif
#ifndef XM_S_worldmap_destination_name_id
#define XM_S_worldmap_destination_name_id
extern s16 xm_worldmap_destination_name_id __asm__("D_8009CE68");
#endif
#ifndef XM_S_worldmap_destination_window
#define XM_S_worldmap_destination_window
extern Window xm_worldmap_destination_window __asm__("D_8009BD64");
#endif
#ifndef XM_S_worldmap_destination_marker
#define XM_S_worldmap_destination_marker
extern POLY_FT4 xm_worldmap_destination_marker[2] __asm__("D_8009D2B8");
#endif
s32 func_80092DF8(void) {
    xm_worldmap_destination_name_id = -1;
    xm_window_open(&xm_worldmap_destination_window, 0x3C0, 0x18D, 0xA0, 0x78, 0x20, 4);
    xm_worldmap_destination_window.unk68 = 8;
    xm_worldmap_path_window.flags |= 2;
    xm_window_reset_if_idle(&xm_worldmap_destination_window);
    setlen(&xm_worldmap_destination_marker[0], 9);
    xm_worldmap_destination_marker[0].code = 0x2C;
    setXY4(&xm_worldmap_destination_marker[0], 0x98, 0x70, 0x128, 0x70, 0x98, 0xB4, 0x128, 0xB4);
    xm_worldmap_destination_marker[0].u0 = 0x80;
    xm_worldmap_destination_marker[0].v0 = 0;
    xm_worldmap_destination_marker[0].u1 = 0xFF;
    xm_worldmap_destination_marker[0].v1 = 0;
    xm_worldmap_destination_marker[0].u2 = 0x80;
    xm_worldmap_destination_marker[0].v2 = 0x3F;
    xm_worldmap_destination_marker[0].u3 = 0xFF;
    xm_worldmap_destination_marker[0].v3 = 0x3F;
    setRGB0(&xm_worldmap_destination_marker[0], 0x80, 0x80, 0x80);
    xm_worldmap_destination_marker[0].tpage = xm_GetTPage(0, 0, 0x380, 0x100);
    xm_worldmap_destination_marker[0].clut = xm_GetClut(0x130, 0x1E4);
    xm_SetSemiTrans(&xm_worldmap_destination_marker[0], 1);
    xm_worldmap_destination_marker[1] = xm_worldmap_destination_marker[0];
    return 1;
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80092FD8);


#ifndef XENO_PC_PORT
void func_800931B0(void) {
    func_800346D4(&D_8009BD64);
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): tweakWorldmapTexture (src/worldmap/w_800907F4.c). */
#ifndef XM_S_tweakWorldmapTexture
#define XM_S_tweakWorldmapTexture
void xm_tweakWorldmapTexture(u16* src, u16* dst, s32 steps, u8* color) __asm__("func_800931D8");
#endif
void func_800931D8(u16* src, u16* dst, s32 steps, u8* color) {
    u32 alpha;
    u32 red;
    u32 green;
    u32 blue;
    s32 r;
    s32 g;
    s32 b;
    s32 i;
    s32 j;
    u32 t;
    u16* s;
    u8 c[3];

    r = color[0];
    g = color[1];
    b = color[2];
    for (i = 0; i < steps; i++) {
        t = (i << 12) / steps;
        s = src;
        j = 0;
        while (j < 0x100) {
            if (*s == 0) {
                *dst = *s;
            } else {
                c[0] = (*s & 0x1F) << 3;
                c[1] = (*s >> 2) & 0xF8;
                c[2] = (*s >> 7) & 0xF8;
                alpha = *s & 0x8000;
                red = (c[0] * (0x1000 - t) + r * t) >> 15;
                green = c[1] * (0x1000 - t) + g * t;
                blue = c[2] * (0x1000 - t) + b * t;
                *dst = alpha | red | ((green >> 15) << 5) | ((blue >> 15) << 10);
            }
            j++;
            s++;
            dst++;
        }
    }
}
#endif


void func_80093354(void *arg0) {
    s32 temp_a1;
    s32 temp_a1_2;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 temp_v1_3;
    s32 temp_v1_4;

    temp_a1 = *(s32 *)((s8*)(arg0) + 0);
    temp_v1 = D_8009D160 << 0x17;
    if (temp_a1 >= temp_v1) {
        *(s32 *)((s8*)(arg0) + 0) = (s32) (temp_a1 - temp_v1);
    }
    temp_v1_2 = *(s32 *)((s8*)(arg0) + 0);
    if (temp_v1_2 < 0) {
        *(s32 *)((s8*)(arg0) + 0) = (s32) (temp_v1_2 + (D_8009D160 << 0x17));
    }
    temp_a1_2 = *(s32 *)((s8*)(arg0) + 8);
    temp_v1_3 = D_8009D2B4 << 0x17;
    if (temp_a1_2 >= temp_v1_3) {
        *(s32 *)((s8*)(arg0) + 8) = (s32) (temp_a1_2 - temp_v1_3);
    }
    temp_v1_4 = *(s32 *)((s8*)(arg0) + 8);
    if (temp_v1_4 < 0) {
        *(s32 *)((s8*)(arg0) + 8) = (s32) (temp_v1_4 + (D_8009D2B4 << 0x17));
    }
}



#ifndef XENO_PC_PORT
void func_800933EC(void *arg0) {
    s32 temp_a1;
    s32 temp_a1_2;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 temp_v1_3;
    s32 temp_v1_4;

    temp_a1 = *(s32 *)((s8*)(arg0) + 0);
    temp_v1 = D_8009D160 << 0xB;
    if (temp_a1 >= temp_v1) {
        *(s32 *)((s8*)(arg0) + 0) = (s32) (temp_a1 - temp_v1);
    }
    temp_v1_2 = *(s32 *)((s8*)(arg0) + 0);
    if (temp_v1_2 < 0) {
        *(s32 *)((s8*)(arg0) + 0) = (s32) (temp_v1_2 + (D_8009D160 << 0xB));
    }
    temp_a1_2 = *(s32 *)((s8*)(arg0) + 8);
    temp_v1_3 = D_8009D2B4 << 0xB;
    if (temp_a1_2 >= temp_v1_3) {
        *(s32 *)((s8*)(arg0) + 8) = (s32) (temp_a1_2 - temp_v1_3);
    }
    temp_v1_4 = *(s32 *)((s8*)(arg0) + 8);
    if (temp_v1_4 < 0) {
        *(s32 *)((s8*)(arg0) + 8) = (s32) (temp_v1_4 + (D_8009D2B4 << 0xB));
    }
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_wrap_offset (src/decomp/src/worldmap/worldmap_steering_camera_terrain.c:1125). */
#ifndef XM_S_worldmap_area_blocks_x
#define XM_S_worldmap_area_blocks_x
extern s32 xm_worldmap_area_blocks_x __asm__("D_8009D160");
#endif
#ifndef XM_S_worldmap_area_blocks_z
#define XM_S_worldmap_area_blocks_z
extern s32 xm_worldmap_area_blocks_z __asm__("D_8009D2B4");
#endif
void xm_worldmap_wrap_offset(VECTOR *offset) __asm__("func_80093484");
void xm_worldmap_wrap_offset(VECTOR *offset) {
    if (offset->vx < -0x4000000) {
        offset->vx += xm_worldmap_area_blocks_x << 23;
    } else if (offset->vx > 0x4000000) {
        offset->vx -= xm_worldmap_area_blocks_x << 23;
    }
    if (offset->vz < -0x4000000) {
        offset->vz += xm_worldmap_area_blocks_z << 23;
    } else if (offset->vz > 0x4000000) {
        offset->vz -= xm_worldmap_area_blocks_z << 23;
    }
}
#endif


void func_80093534(void *arg0) {
    s32 temp_v1;
    s32 temp_v1_2;
    s32 var_v0;
    s32 var_v0_2;

    temp_v1 = *(s32 *)((s8*)(arg0) + 0);
    if (temp_v1 < -0x4000) {
        var_v0 = temp_v1 + (D_8009D160 << 0xB);
        goto block_4;
    }
    if (temp_v1 >= 0x4001) {
        var_v0 = temp_v1 - (D_8009D160 << 0xB);
block_4:
        *(s32 *)((s8*)(arg0) + 0) = var_v0;
    }
    temp_v1_2 = *(s32 *)((s8*)(arg0) + 8);
    if (temp_v1_2 < -0x4000) {
        var_v0_2 = temp_v1_2 + (D_8009D2B4 << 0xB);
        goto block_9;
    }
    if (temp_v1_2 >= 0x4001) {
        var_v0_2 = temp_v1_2 - (D_8009D2B4 << 0xB);
block_9:
        *(s32 *)((s8*)(arg0) + 8) = var_v0_2;
    }
}



#ifndef XENO_PC_PORT
void func_800935DC(void *arg0, void *arg1, void *arg2) {
    s32 temp_lo;

    temp_lo = (s32) (-(*(s32 *)((s8*)(arg2) + 0) * (*(s32 *)((s8*)(arg0) + 0) - *(s32 *)((s8*)(arg1) + 0))) - (*(s32 *)((s8*)(arg2) + 8) * (*(s32 *)((s8*)(arg0) + 8) - *(s32 *)((s8*)(arg1) + 8)))) / (s32) *(s32 *)((s8*)(arg2) + 4);
    *(s32 *)((s8*)(arg0) + 4) = temp_lo;
    *(s32 *)((s8*)(arg0) + 4) = (s32) (temp_lo + *(s32 *)((s8*)(arg1) + 4));
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_terrain_get_cell (src/decomp/src/worldmap/worldmap_steering_camera_terrain.c:1160). */
#ifndef XM_S_worldmap_terrain_blocks
#define XM_S_worldmap_terrain_blocks
extern void *xm_worldmap_terrain_blocks[0x100] __asm__("D_8009C184");
#endif
u8 *xm_worldmap_terrain_get_cell(s32 x, s32 z) __asm__("func_80093660");
u8 *xm_worldmap_terrain_get_cell(s32 x, s32 z) {
    s16 block;
    s16 quadrant;
    s16 cell;

    block = ((z >> 20) / 8) * xm_worldmap_area_blocks_x + (x >> 20) / 8;
    x = (x >> 12) & 0x7FF;
    z = (z >> 12) & 0x7FF;
    quadrant = 0;
    if (x >= 0x400) {
        quadrant = 1;
        x -= 0x400;
    }
    if (z >= 0x400) {
        quadrant |= 2;
        z -= 0x400;
    }
    cell = ((z >> 4) / 8) * 9 + (x >> 4) / 8;
    return (u8 *)xm_worldmap_terrain_blocks[block] + quadrant * 0x144 + cell * 4;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_terrain_get_normal (src/decomp/src/worldmap/worldmap_steering_camera_terrain.c:1194). */
#ifndef XM_T_TerrainScratch
#define XM_T_TerrainScratch
typedef struct {
    VECTOR edge0;
    VECTOR edge1;
    VECTOR normal;
    u8 pad30[0x70];
    SVECTOR corners[4]; 
} TerrainScratch;
#endif
#ifndef XM_T_TERRAIN_SCRATCH
#define XM_T_TERRAIN_SCRATCH
#define TERRAIN_SCRATCH ((TerrainScratch *)0x1F800000)
#endif
#ifndef XM_S_worldmap_terrain_get_normal
#define XM_S_worldmap_terrain_get_normal
void xm_worldmap_terrain_get_normal(VECTOR *normal, s32 x, s32 z) __asm__("func_80093740");
#endif
#ifndef XM_S_worldmap_terrain_diagonal_normals
#define XM_S_worldmap_terrain_diagonal_normals
extern VECTOR xm_worldmap_terrain_diagonal_normals[2] __asm__("D_8009B244");
#endif
void func_80093740(VECTOR *normal, s32 x, s32 z) {
    u8 *cell;
    TerrainScratch *scratch;

    scratch = TERRAIN_SCRATCH;
    cell = xm_worldmap_terrain_get_cell(x, z);
    x = (u16)(x / 8);
    z = (u16)(z / 8);
    if (cell[1] & 0x80) {
        if (x * xm_worldmap_terrain_diagonal_normals[1].vx + -z * xm_worldmap_terrain_diagonal_normals[1].vz < 0) {
            scratch->edge0.vx = 0x10;
            scratch->edge0.vz = -0x10;
            scratch->edge1.vx = 0;
            scratch->edge1.vz = -0x10;
            scratch->edge0.vy = (s8)cell[0x28] - (s8)cell[0];
            scratch->edge1.vy = (s8)cell[0x24] - (s8)cell[0];
        } else {
            scratch->edge0.vx = 0x10;
            scratch->edge1.vx = 0x10;
            scratch->edge0.vz = 0;
            scratch->edge1.vz = -0x10;
            scratch->edge0.vy = (s8)cell[4] - (s8)cell[0];
            scratch->edge1.vy = (s8)cell[0x28] - (s8)cell[0];
        }
    } else {
        if ((x - 0x10000) * xm_worldmap_terrain_diagonal_normals[0].vx + -z * xm_worldmap_terrain_diagonal_normals[0].vz < 0) {
            scratch->edge0.vx = -0x10;
            scratch->edge0.vz = -0x10;
            scratch->edge1.vx = -0x10;
            scratch->edge1.vz = 0;
            scratch->edge0.vy = (s8)cell[0x24] - (s8)cell[4];
            scratch->edge1.vy = (s8)cell[0] - (s8)cell[4];
        } else {
            scratch->edge0.vx = 0;
            scratch->edge0.vz = -0x10;
            scratch->edge1.vx = -0x10;
            scratch->edge1.vz = -0x10;
            scratch->edge0.vy = (s8)cell[0x28] - (s8)cell[4];
            scratch->edge1.vy = (s8)cell[0x24] - (s8)cell[4];
        }
    }
    xm_OuterProduct0(&scratch->edge1, &scratch->edge0, &scratch->normal);
    xm_VectorNormal(&scratch->normal, normal);
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): worldmapGetAltitudeFor2dPoint (src/worldmap/w_800907F4.c). */
#ifndef XM_S_getWorldChunkForPosition
#define XM_S_getWorldChunkForPosition
u8* xm_getWorldChunkForPosition(s32 x, s32 z) __asm__("func_80093660");
#endif
#ifndef XM_S_interpolateWorldPosition
#define XM_S_interpolateWorldPosition
void xm_interpolateWorldPosition(VECTOR* out, s32 x, s32 z) __asm__("func_80093740");
#endif
#ifndef XM_S_averageVector
#define XM_S_averageVector
void xm_averageVector(VECTOR* a, VECTOR* b, VECTOR* n) __asm__("func_800935DC");
#endif
s32 func_80093978(s32 x, s32 z) {
    VECTOR pos;
    VECTOR base;
    VECTOR normal;
    u8* c;

    c = xm_getWorldChunkForPosition(x, z);
    pos.vx = (x / 8) & 0xFFFF;
    pos.vz = -((z / 8) & 0xFFFF);
    if (c[1] & 0x80) {
        base.vx = 0;
        base.vz = 0;
        base.vy = (s8)c[0] << 12;
    } else {
        base.vx = 0x10000;
        base.vz = 0;
        base.vy = (s8)c[4] << 12;
    }
    xm_interpolateWorldPosition(&normal, x, z);
    xm_averageVector(&pos, &base, &normal);
    return pos.vy << 3;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_terrain_get_wave_height (src/decomp/src/worldmap/worldmap_steering_camera_terrain.c:1265). */
#ifndef XM_S_worldmap_wave_phase_z
#define XM_S_worldmap_wave_phase_z
extern s32 xm_worldmap_wave_phase_z __asm__("D_8009C618");
#endif
s32 func_80093A5C(s32 x, s32 z) {
    u8 *cell;
    TerrainScratch *scratch;
    s32 ix, iz;
    s32 x0, x1, z0, z1;
    s32 amplitude;
    s32 phase_x, phase_z;
    s32 cx, cz;

    scratch = TERRAIN_SCRATCH;
    cell = xm_worldmap_terrain_get_cell(x, z);
    ix = ((u32)x >> 19) & 7;
    x0 = ix << 9;
    iz = ((u32)z >> 19) & 7;
    z0 = iz << 9;
    phase_x = xm_worldmap_wave_phase_x + x0;
    phase_z = xm_worldmap_wave_phase_z + z0;
    amplitude = (xm_gpu_get_sin(phase_z) << 4) >> 3;
    scratch->corners[0].vy = (xm_gpu_get_sin(phase_x) * amplitude >> 20) + ((s8 *)cell)[0] * 8;
    x1 = (ix + 1) << 9;
    phase_x = xm_worldmap_wave_phase_x + x1;
    phase_z = xm_worldmap_wave_phase_z + z0;
    amplitude = (xm_gpu_get_sin(phase_z) << 4) >> 3;
    scratch->corners[1].vy = (xm_gpu_get_sin(phase_x) * amplitude >> 20) + ((s8 *)cell)[4] * 8;
    z1 = (iz + 1) << 9;
    phase_x = xm_worldmap_wave_phase_x + x0;
    phase_z = xm_worldmap_wave_phase_z + z1;
    amplitude = (xm_gpu_get_sin(phase_z) << 4) >> 3;
    scratch->corners[2].vy = (xm_gpu_get_sin(phase_x) * amplitude >> 20) + ((s8 *)cell)[0x24] * 8;
    phase_x = xm_worldmap_wave_phase_x + x1;
    phase_z = xm_worldmap_wave_phase_z + z1;
    amplitude = (xm_gpu_get_sin(phase_z) << 4) >> 3;
    scratch->corners[3].vy = (xm_gpu_get_sin(phase_x) * amplitude >> 20) + ((s8 *)cell)[0x28] * 8;
    cx = (u16)(x / 8);
    cz = (u16)(z / 8);
    if (cell[1] & 0x80) {
        if (cx * xm_worldmap_terrain_diagonal_normals[1].vx + -cz * xm_worldmap_terrain_diagonal_normals[1].vz < 0) {
            scratch->edge0.vx = 0x80;
            scratch->edge0.vz = -0x80;
            scratch->edge1.vz = -0x80;
            scratch->edge1.vx = 0;
            scratch->edge0.vy = scratch->corners[3].vy - scratch->corners[0].vy;
            scratch->edge1.vy = scratch->corners[2].vy - scratch->corners[0].vy;
        } else {
            scratch->edge0.vx = 0x80;
            scratch->edge1.vx = 0x80;
            scratch->edge0.vz = 0;
            scratch->edge1.vz = -0x80;
            scratch->edge0.vy = scratch->corners[1].vy - scratch->corners[0].vy;
            scratch->edge1.vy = scratch->corners[3].vy - scratch->corners[0].vy;
        }
    } else {
        if ((cx - 0x10000) * xm_worldmap_terrain_diagonal_normals[0].vx + -cz * xm_worldmap_terrain_diagonal_normals[0].vz < 0) {
            scratch->edge0.vx = -0x80;
            scratch->edge0.vz = -0x80;
            scratch->edge1.vx = -0x80;
            scratch->edge1.vz = 0;
            scratch->edge0.vy = scratch->corners[2].vy - scratch->corners[1].vy;
            scratch->edge1.vy = scratch->corners[0].vy - scratch->corners[1].vy;
        } else {
            scratch->edge0.vx = 0;
            scratch->edge0.vz = -0x80;
            scratch->edge1.vx = -0x80;
            scratch->edge1.vz = -0x80;
            scratch->edge0.vy = scratch->corners[3].vy - scratch->corners[1].vy;
            scratch->edge1.vy = scratch->corners[2].vy - scratch->corners[1].vy;
        }
    }
    xm_OuterProduct0(&scratch->edge1, &scratch->edge0, &scratch->normal);
    xm_VectorNormal(&scratch->normal, &scratch->edge0);
    scratch->edge1.vx = (x >> 12) & 0x7F;
    scratch->edge1.vz = -((z >> 12) & 0x7F);
    if (cell[1] & 0x80) {
        scratch->normal.vx = 0;
        scratch->normal.vz = 0;
        scratch->normal.vy = scratch->corners[0].vy;
    } else {
        scratch->normal.vx = 0x80;
        scratch->normal.vz = 0;
        scratch->normal.vy = scratch->corners[1].vy;
    }
    xm_worldmap_set_height_on_plane(&scratch->edge1, &scratch->normal, &scratch->edge0);
    return scratch->edge1.vy << 12;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_terrain_get_attribute (src/decomp/src/worldmap/worldmap_steering_camera_terrain.c:1357). */
#ifndef XM_T_TerrainBlock
#define XM_T_TerrainBlock
typedef struct {
    u8 pad0[0x510];
    s16 attributes[256];
} TerrainBlock;
#endif
#ifndef XM_S_worldmap_terrain_get_attribute
#define XM_S_worldmap_terrain_get_attribute
s32 xm_worldmap_terrain_get_attribute(VECTOR *position) __asm__("func_80093E8C");
#endif
s32 xm_worldmap_terrain_get_attribute(VECTOR *position) __asm__("func_80093E8C");
s32 xm_worldmap_terrain_get_attribute(VECTOR *position) {
    s32 x, z;
    s16 block;
    s32 row;
    s16 *attributes;

    z = position->vz;
    row = (z >> 20) / 8;
    x = position->vx;
    block = row * xm_worldmap_area_blocks_x + (x >> 20) / 8;
    attributes = ((TerrainBlock *)xm_worldmap_terrain_blocks[block])->attributes;
    return attributes[(((z & 0x7FF000) >> 19) << 4) | ((x & 0x7FF000) >> 19)];
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_terrain_get_layer (src/decomp/src/worldmap/worldmap_steering_camera_terrain.c:1373). */
#ifndef XM_S_worldmap_terrain_split_points
#define XM_S_worldmap_terrain_split_points
extern VECTOR xm_worldmap_terrain_split_points[16] __asm__("D_8009B464");
#endif
#ifndef XM_S_worldmap_terrain_split_normals
#define XM_S_worldmap_terrain_split_normals
extern VECTOR xm_worldmap_terrain_split_normals[16] __asm__("D_8009B364");
#endif
s16 func_80093F18(VECTOR *position) {
    u32 attribute;
    s32 type;
    s32 dx, dz;

    attribute = xm_worldmap_terrain_get_attribute(position);
    type = attribute & 0xF;
    dx = (u16)(position->vx / 8) - xm_worldmap_terrain_split_points[type].vx;
    dz = (u16)(position->vz / 8) - xm_worldmap_terrain_split_points[type].vz;
    if ((dx * xm_worldmap_terrain_split_normals[type].vx >> 12) + (dz * xm_worldmap_terrain_split_normals[type].vz >> 12) > 0) {
        return (attribute >> 4) & 7;
    }
    return (attribute >> 7) & 7;
}
#endif


#ifndef XENO_PC_PORT
s32 func_80093FE4(void) {
    return func_80093E8C() & 0xF;
}
#endif



#ifndef XENO_PC_PORT
u32 func_80094004(void) {
    return (u32) (func_80093E8C() << 0x10) >> 0x1A;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80094028(void *arg0) {
    return ((u8) *(u8 *)((s8*)(func_80093660(*(s32 *)((s8*)(arg0) + 0), *(s32 *)((s8*)(arg0) + 8))) + 3) >> 2) & 0xF;
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_terrain_can_mode_enter_layer (src/decomp/src/worldmap/worldmap_steering_camera_terrain.c:1405). */
#ifndef XM_S_worldmap_terrain_can_mode_enter_layer
#define XM_S_worldmap_terrain_can_mode_enter_layer
s16 xm_worldmap_terrain_can_mode_enter_layer(s16 row, s16 column) __asm__("func_80094060");
#endif
#ifndef XM_S_worldmap_terrain_passable_layers
#define XM_S_worldmap_terrain_passable_layers
extern s16 xm_worldmap_terrain_passable_layers[] __asm__("D_8009BAC8");
#endif
s16 func_80094060(s16 row, s16 column) {
    return *(s16 *)((u8 *)xm_worldmap_terrain_passable_layers + row * 16 + column * 2);
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): checkWorldmapPositionSub1_1 (src/worldmap/w_800907F4.c). */
#ifndef XM_S_checkWorldmapPositionSub1_1
#define XM_S_checkWorldmapPositionSub1_1
s32 xm_checkWorldmapPositionSub1_1(VECTOR* collision, VECTOR* step, VECTOR* out) __asm__("func_80094088");
#endif
#ifndef XM_S_getWorldmapMaterial2
#define XM_S_getWorldmapMaterial2
s32 xm_getWorldmapMaterial2(VECTOR* p) __asm__("func_80093FE4");
#endif
#ifndef XM_S_worldmapMaterialWallDirections
#define XM_S_worldmapMaterialWallDirections
extern VECTOR xm_worldmapMaterialWallDirections[16] __asm__("D_8009B264");
#endif
s32 func_80094088(VECTOR* collision, VECTOR* step, VECTOR* out) {
    s32 i;
    s32 dot;

    i = (s16)xm_getWorldmapMaterial2(collision);
    dot = step->vx * xm_worldmapMaterialWallDirections[i].vx + step->vz * xm_worldmapMaterialWallDirections[i].vz;
    if (dot == 0) {
        out->vx = step->vx;
        out->vz = step->vz;
        return 0;
    }
    if (dot < 0) {
        out->vx = -xm_worldmapMaterialWallDirections[i].vx;
        out->vz = -xm_worldmapMaterialWallDirections[i].vz;
    } else {
        out->vx = xm_worldmapMaterialWallDirections[i].vx;
        out->vz = xm_worldmapMaterialWallDirections[i].vz;
    }
    return 1;
}
#endif


s32 func_80094154(void *arg0, void *arg1) {
    s32 temp_lo;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 var_v0;
    s32 var_v0_2;

    var_v0 = abs(*(s32 *)((s8*)(arg0) + 0) - *(s32 *)((s8*)(arg1) + 0));
    temp_v0 = var_v0 >> 0xC;
    temp_lo = temp_v0 * temp_v0;
    var_v0_2 = abs(*(s32 *)((s8*)(arg0) + 8) - *(s32 *)((s8*)(arg1) + 8));
    temp_v0_2 = var_v0_2 >> 0xC;
    return SquareRoot0(temp_lo + (temp_v0_2 * temp_v0_2));
}



#ifndef XENO_PC_PORT
void func_800941C4(void *arg0, void *arg1, void *arg2, s16 *arg3) {
    s16 temp_a0;
    s32 temp_a2;

    temp_a2 = *(s32 *)((s8*)(arg1) + 0);
    temp_a0 = (ratan2(*(s32 *)((s8*)(arg1) + 8) - *(s32 *)((s8*)(arg0) + 8), temp_a2 - *(s32 *)((s8*)(arg0) + 0), temp_a2) + 0x400) & 0xFFF;
    *arg3 = temp_a0;
    *(s32 *)((s8*)(arg2) + 0) = rcos(temp_a0);
    *(s32 *)((s8*)(arg2) + 8) = (s32) -rsin(*arg3);
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): setupWorldmapExits (src/worldmap/w_800907F4.c). */
#ifndef XM_T_sWorldmapExitDef
#define XM_T_sWorldmapExitDef
typedef struct sWorldmapExitDef {
     s16 m0_X;
     s16 m2_Y;
     s16 m4_width;
     s16 m6_height;
     s16 m8_destinationField;
     s16 mA_returnWorldmapMode;
     s16 mC_newWorldmapMode;
     s16 mE_type;
} sWorldmapExitDef;
#endif
#ifndef XM_S_setupWorldmapExits
#define XM_S_setupWorldmapExits
s32 xm_setupWorldmapExits(VECTOR* pos, s32 set) __asm__("func_80094238");
#endif
#ifndef XM_S_worldmapExitsDefs
#define XM_S_worldmapExitsDefs
extern sWorldmapExitDef* * xm_worldmapExitsDefs __asm__("D_8009BD00");
#endif
#ifndef XM_S_worldmapCurrentExit
#define XM_S_worldmapCurrentExit
extern sWorldmapExitDef * xm_worldmapCurrentExit __asm__("D_8009D7D8");
#endif
#ifndef XM_S_worldmapExitVar0
#define XM_S_worldmapExitVar0
extern s16 xm_worldmapExitVar0 __asm__("D_8009BD24");
#endif
#ifndef XM_S_worldmapExitVar1
#define XM_S_worldmapExitVar1
extern s16 xm_worldmapExitVar1 __asm__("D_8009CE68");
#endif
s32 func_80094238(VECTOR* pos, s32 set) {
    u16 x;
    u16 z;
    sWorldmapExitDef* e;

    x = pos->vx >> 12;
    e = xm_worldmapExitsDefs[set];
    z = pos->vz >> 12;
    while (e->m8_destinationField != -1) {
        if ((e->m0_X <= x) & (x <= e->m0_X + e->m4_width) & (e->m2_Y <= z) & (z <= e->m2_Y + e->m6_height)) {
            if (e->mE_type == 4) {
                xm_worldmapCurrentExit = (sWorldmapExitDef*)-1;
                xm_worldmapExitVar0 = -1;
                xm_worldmapExitVar1 = e->mC_newWorldmapMode;
            } else {
                xm_worldmapCurrentExit = e;
                xm_worldmapExitVar1 = -1;
                xm_worldmapExitVar0 = e->mC_newWorldmapMode;
            }
            return 1;
        }
        e++;
    }
    xm_worldmapCurrentExit = (sWorldmapExitDef*)-1;
    xm_worldmapExitVar0 = -1;
    xm_worldmapExitVar1 = -1;
    return 0;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): findWorldmapExitOfType (src/worldmap/w_800907F4.c). */
#ifndef XM_S_findWorldmapExitOfType
#define XM_S_findWorldmapExitOfType
s32 xm_findWorldmapExitOfType(VECTOR* pos, s32 set, s32 type) __asm__("func_80094364");
#endif
s32 func_80094364(VECTOR* pos, s32 set, s32 type) {
    u16 x;
    u16 z;
    sWorldmapExitDef* e;

    x = pos->vx >> 12;
    e = xm_worldmapExitsDefs[set];
    z = pos->vz >> 12;
    while (e->m8_destinationField != -1) {
        if ((e->m0_X <= x) & (x <= e->m0_X + e->m4_width) & (e->m2_Y <= z) & (z <= e->m2_Y + e->m6_height)) {
            if (e->mE_type == type) {
                xm_worldmapCurrentExit = e;
                return 1;
            }
        }
        e++;
    }
    return 0;
}
#endif

void func_80094434(void) {
}

#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): checkWorldmapPositionSub1_0_3 (src/worldmap/w_800907F4.c). */
#ifndef XM_S_checkWorldmapPositionSub1_0_3
#define XM_S_checkWorldmapPositionSub1_0_3
s32 xm_checkWorldmapPositionSub1_0_3(VECTOR* pos, VECTOR* step, VECTOR* out, s16 type) __asm__("func_8009443C");
#endif
#ifndef XM_S_checkWorldmapPositionSub0
#define XM_S_checkWorldmapPositionSub0
void xm_checkWorldmapPositionSub0(VECTOR* p) __asm__("func_80093354");
#endif
#ifndef XM_S_checkWorldmapPositionSub1_0_1
#define XM_S_checkWorldmapPositionSub1_0_1
s16 xm_checkWorldmapPositionSub1_0_1(s16 a, s16 b) __asm__("func_80094060");
#endif
#ifndef XM_S_getWorldmapGroundType
#define XM_S_getWorldmapGroundType
s16 xm_getWorldmapGroundType(VECTOR* position) __asm__("func_80093F18");
#endif
s32 func_8009443C(VECTOR* pos, VECTOR* step, VECTOR* out, s16 type) {
    s32 slope;

    slope = (step->vz << 12) / step->vx;
    out[0].vx = (out[1].vx & 0xFFF80000) - pos->vx;
    out[2].vx = out[0].vx - 1;
    out[3].vx = out[0].vx;
    out[2].vz = (slope * out[2].vx >> 12) + pos->vz;
    out[3].vz = (slope * out[3].vx >> 12) + pos->vz;
    out[2].vx += pos->vx;
    out[3].vx += pos->vx;
    xm_checkWorldmapPositionSub0(&out[2]);
    if (xm_checkWorldmapPositionSub1_0_1(type, xm_getWorldmapGroundType(&out[2])) == 0) {
        out[0] = out[2];
        return 1;
    }
    xm_checkWorldmapPositionSub0(&out[3]);
    return -(xm_checkWorldmapPositionSub1_0_1(type, xm_getWorldmapGroundType(&out[3])) == 0) & 3;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): checkWorldmapPositionSub1_0_4 (src/worldmap/w_800907F4.c). */
#ifndef XM_S_checkWorldmapPositionSub1_0_4
#define XM_S_checkWorldmapPositionSub1_0_4
s32 xm_checkWorldmapPositionSub1_0_4(VECTOR* pos, VECTOR* step, VECTOR* out, s16 type) __asm__("func_800945C8");
#endif
s32 func_800945C8(VECTOR* pos, VECTOR* step, VECTOR* out, s16 type) {
    s32 slope;

    slope = (step->vz << 12) / step->vx;
    out[0].vx = (pos->vx & 0xFFF80000) - pos->vx;
    out[2].vx = out[0].vx;
    out[3].vx = out[0].vx - 1;
    out[2].vz = (slope * out[2].vx >> 12) + pos->vz;
    out[3].vz = (slope * out[3].vx >> 12) + pos->vz;
    out[2].vx += pos->vx;
    out[3].vx += pos->vx;
    xm_checkWorldmapPositionSub0(&out[2]);
    if (xm_checkWorldmapPositionSub1_0_1(type, xm_getWorldmapGroundType(&out[2])) == 0) {
        out[0] = out[2];
        return 1;
    }
    xm_checkWorldmapPositionSub0(&out[3]);
    return -(xm_checkWorldmapPositionSub1_0_1(type, xm_getWorldmapGroundType(&out[3])) == 0) & 3;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): checkWorldmapPositionSub1_0_5 (src/worldmap/w_800907F4.c). */
#ifndef XM_S_checkWorldmapPositionSub1_0_5
#define XM_S_checkWorldmapPositionSub1_0_5
s32 xm_checkWorldmapPositionSub1_0_5(VECTOR* pos, VECTOR* step, VECTOR* out, s16 type) __asm__("func_80094750");
#endif
s32 func_80094750(VECTOR* pos, VECTOR* step, VECTOR* out, s16 type) {
    s32 slope;

    slope = (step->vx << 12) / step->vz;
    out[0].vz = (out[1].vz & 0xFFF80000) - pos->vz;
    out[2].vz = out[0].vz - 1;
    out[3].vz = out[0].vz;
    out[2].vx = (slope * out[2].vz >> 12) + pos->vx;
    out[3].vx = (slope * out[3].vz >> 12) + pos->vx;
    out[2].vz += pos->vz;
    out[3].vz += pos->vz;
    xm_checkWorldmapPositionSub0(&out[2]);
    if (xm_checkWorldmapPositionSub1_0_1(type, xm_getWorldmapGroundType(&out[2])) == 0) {
        out[0] = out[2];
        return 1;
    }
    xm_checkWorldmapPositionSub0(&out[3]);
    return (xm_checkWorldmapPositionSub1_0_1(type, xm_getWorldmapGroundType(&out[3])) == 0) << 1;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): checkWorldmapPositionSub1_0_2 (src/worldmap/w_800907F4.c). */
#ifndef XM_S_checkWorldmapPositionSub1_0_2
#define XM_S_checkWorldmapPositionSub1_0_2
s32 xm_checkWorldmapPositionSub1_0_2(VECTOR* pos, VECTOR* step, VECTOR* out, s16 type) __asm__("func_800948D8");
#endif
s32 func_800948D8(VECTOR* pos, VECTOR* step, VECTOR* out, s16 type) {
    s32 slope;

    slope = (step->vx << 12) / step->vz;
    out[0].vz = (pos->vz & 0xFFF80000) - pos->vz;
    out[2].vz = out[0].vz;
    out[3].vz = out[0].vz - 1;
    out[2].vx = (slope * out[2].vz >> 12) + pos->vx;
    out[3].vx = (slope * out[3].vz >> 12) + pos->vx;
    out[2].vz += pos->vz;
    out[3].vz += pos->vz;
    xm_checkWorldmapPositionSub0(&out[2]);
    if (xm_checkWorldmapPositionSub1_0_1(type, xm_getWorldmapGroundType(&out[2])) == 0) {
        out[0] = out[2];
        return 1;
    }
    xm_checkWorldmapPositionSub0(&out[3]);
    return (xm_checkWorldmapPositionSub1_0_1(type, xm_getWorldmapGroundType(&out[3])) == 0) << 1;
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80094A5C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800951A8);


#ifndef XENO_PC_PORT
void func_800952B0(void *arg0, void *arg1, void *arg2) {
    s32 temp_a3;
    s32 temp_v1;

    temp_a3 = *(s32 *)((s8*)(arg2) + 0);
    temp_v1 = (temp_a3 * *(s32 *)((s8*)(arg0) + 0)) + (*(s32 *)((s8*)(arg2) + 8) * *(s32 *)((s8*)(arg0) + 8));
    if (temp_v1 < 0) {
        *(s32 *)((s8*)(arg1) + 0) = (s32) -temp_a3;
        *(s32 *)((s8*)(arg1) + 8) = (s32) -*(s32 *)((s8*)(arg2) + 8);
    } else if (temp_v1 > 0) {
        *(s32 *)((s8*)(arg1) + 0) = temp_a3;
        *(s32 *)((s8*)(arg1) + 8) = (s32) *(s32 *)((s8*)(arg2) + 8);
    } else {
        *(s32 *)((s8*)(arg1) + 8) = 0;
        *(s32 *)((s8*)(arg1) + 0) = 0;
    }
    *(s32 *)((s8*)(arg1) + 4) = 0;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80095324);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80095414);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80095CD4);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_stream_reset (src/decomp/src/worldmap/worldmap_movement_terrain.c:841). */
#ifndef XM_T_DiscReadRequest
#define XM_T_DiscReadRequest
typedef struct DiscReadRequest {
    s32 sector;
    s32 bytes;
    u8 *destination;
} DiscReadRequest;
#endif
#ifndef XM_T_HostReadRequest
#define XM_T_HostReadRequest
typedef struct HostReadRequest {
    char *path;
    s32 offset;
    s32 bytes;
    u8 *destination;
} HostReadRequest;
#endif
#ifndef XM_S_worldmap_stream_reset
#define XM_S_worldmap_stream_reset
void xm_worldmap_stream_reset(void) __asm__("func_80095F78");
#endif
#ifndef XM_S_cd_has_pc_file_server
#define XM_S_cd_has_pc_file_server
s32 xm_cd_has_pc_file_server(void) __asm__("func_8002C3D8");
#endif
#ifndef XM_S_worldmap_stream_read_slot
#define XM_S_worldmap_stream_read_slot
extern s32 xm_worldmap_stream_read_slot __asm__("D_8009BCB8");
#endif
#ifndef XM_S_worldmap_stream_write_slot
#define XM_S_worldmap_stream_write_slot
extern s32 xm_worldmap_stream_write_slot __asm__("D_8009BE44");
#endif
#ifndef XM_S_worldmap_stream_state
#define XM_S_worldmap_stream_state
extern s32 xm_worldmap_stream_state __asm__("D_8009CD44");
#endif
#ifndef XM_S_worldmap_stream_disc_lists
#define XM_S_worldmap_stream_disc_lists
extern DiscReadRequest *xm_worldmap_stream_disc_lists[16] __asm__("D_8009D788");
#endif
#ifndef XM_S_worldmap_stream_disc_buffer
#define XM_S_worldmap_stream_disc_buffer
extern void *xm_worldmap_stream_disc_buffer __asm__("D_8009BE08");
#endif
#ifndef XM_S_worldmap_stream_drain_buffer
#define XM_S_worldmap_stream_drain_buffer
extern void *xm_worldmap_stream_drain_buffer __asm__("D_8009D7D4");
#endif
#ifndef XM_S_worldmap_stream_request_count
#define XM_S_worldmap_stream_request_count
extern s32 xm_worldmap_stream_request_count __asm__("D_8009D808");
#endif
#ifndef XM_S_worldmap_cd_sync_result
#define XM_S_worldmap_cd_sync_result
extern s8 xm_worldmap_cd_sync_result[8] __asm__("D_8009C588");
#endif
#ifndef XM_S_worldmap_stream_host_lists
#define XM_S_worldmap_stream_host_lists
extern HostReadRequest *xm_worldmap_stream_host_lists[16] __asm__("D_8009C624");
#endif
#ifndef XM_S_worldmap_stream_host_buffer
#define XM_S_worldmap_stream_host_buffer
extern void *xm_worldmap_stream_host_buffer __asm__("D_8009D3C0");
#endif
void func_80095F78(void) {
    s32 first;
    s32 second;
    s32 i;
    s8 *flag;

    first = xm_cd_has_pc_file_server();
    second = xm_cd_has_pc_file_server();
    if ((first == 0) | (second == -1)) {
        xm_worldmap_stream_read_slot = 0;
        xm_worldmap_stream_write_slot = 0;
        xm_worldmap_stream_state = 0;
        for (i = 0xF; i >= 0; i--) {
            xm_worldmap_stream_disc_lists[i] = NULL;
        }
        xm_worldmap_stream_disc_buffer = xm_heap_alloc(0x4200, 0);
        xm_worldmap_stream_drain_buffer = xm_heap_alloc(0x800, 0);
        xm_worldmap_stream_request_count = 0;
        for (i = 7, flag = &xm_worldmap_cd_sync_result[7]; i >= 0; i--) {
            *flag-- = 0;
        }
    } else {
        xm_worldmap_stream_read_slot = 0;
        xm_worldmap_stream_write_slot = 0;
        xm_worldmap_stream_state = 0;
        for (i = 0xF; i >= 0; i--) {
            xm_worldmap_stream_host_lists[i] = NULL;
        }
        xm_worldmap_stream_host_buffer = xm_heap_alloc(0x5800, 0);
        xm_worldmap_stream_drain_buffer = xm_heap_alloc(0x800, 0);
        xm_worldmap_stream_request_count = 0;
        for (i = 7, flag = &xm_worldmap_cd_sync_result[7]; i >= 0; i--) {
            *flag-- = 0;
        }
    }
}
#endif


#ifndef XENO_PC_PORT
void func_800960BC(void) {
    s32 temp_s0;
    s32 var_a0;

    temp_s0 = func_8002C3D8();
    if (((temp_s0 == 0) | (~func_8002C3D8() == 0)) != 0) {
        var_a0 = D_8009BE08;
    } else {
        var_a0 = D_8009D3C0;
    }
    HeapFree(var_a0);
    HeapFree(D_8009D7D4);
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_stream_wait_for_slot (src/decomp/src/worldmap/worldmap_movement_terrain.c:894). */
#ifndef XM_S_worldmap_stream_step
#define XM_S_worldmap_stream_step
s32 xm_worldmap_stream_step(void) __asm__("func_800967E4");
#endif
void func_80096130(void) {
    s32 first;
    s32 second;

    first = xm_cd_has_pc_file_server();
    second = xm_cd_has_pc_file_server();
    if ((first == 0) | (second == -1)) {
        while (xm_worldmap_stream_disc_lists[xm_worldmap_stream_write_slot] != NULL) {
            xm_VSync(0);
            xm_worldmap_stream_step();
        }
    } else {
        while (xm_worldmap_stream_host_lists[xm_worldmap_stream_write_slot] != NULL) {
            xm_VSync(0);
            xm_worldmap_stream_step();
        }
    }
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_stream_add_disc_request (src/decomp/src/worldmap/worldmap_movement_terrain.c:914). */
#ifndef XM_S_worldmap_stream_add_disc_request
#define XM_S_worldmap_stream_add_disc_request
s32 xm_worldmap_stream_add_disc_request(s32 sector, s32 bytes, u8 *destination) __asm__("func_8009623C");
#endif
s32 func_8009623C(s32 sector, s32 bytes, u8 *destination) {
    DiscReadRequest *request;
    s32 count;

    count = xm_worldmap_stream_request_count;
    if (count < 0x58) {
        xm_worldmap_stream_request_count = count + 1;
        request = (DiscReadRequest *)((u8 *)xm_worldmap_stream_disc_buffer + xm_worldmap_stream_write_slot * 0x420) + count;
        request->sector = sector;
        request->bytes = bytes;
        request->destination = destination;
        return 0;
    }
    return -1;
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_stream_add_host_request (src/decomp/src/worldmap/worldmap_movement_terrain.c:931). */
#ifndef XM_S_worldmap_stream_add_host_request
#define XM_S_worldmap_stream_add_host_request
s32 xm_worldmap_stream_add_host_request(char *path, s32 offset, s32 bytes, u8 *destination) __asm__("func_800962B0");
#endif
s32 func_800962B0(char *path, s32 offset, s32 bytes, u8 *destination) {
    HostReadRequest *request;
    s32 count;

    count = xm_worldmap_stream_request_count;
    if (count < 0x58) {
        xm_worldmap_stream_request_count = count + 1;
        request = (HostReadRequest *)((u8 *)xm_worldmap_stream_host_buffer + xm_worldmap_stream_write_slot * 0x580) + count;
        request->path = path;
        request->offset = offset;
        request->bytes = bytes;
        request->destination = destination;
        return 0;
    }
    return -1;
}
#endif
#ifndef XENO_PC_PORT
s32 func_80096328(void) {
    s32 *temp_s0;

    temp_s0 = D_8009BE08 + (D_8009BE44 * 0x420);
    if ((*temp_s0 != 0) && (D_8009D788[D_8009BE44] == 0)) {
        func_800963E4(temp_s0);
        D_8009D808 = 0;
        D_8009D788[D_8009BE44] = temp_s0;
        D_8009BE44 = (D_8009BE44 + 1) & 0xF;
        return 0;
    }
    D_8009D808 = 0;
    return -1;
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_stream_sort_disc_list (src/decomp/src/worldmap/worldmap_movement_terrain.c:966). */
#ifndef XM_S_worldmap_stream_sort_disc_list
#define XM_S_worldmap_stream_sort_disc_list
void xm_worldmap_stream_sort_disc_list(DiscReadRequest *list) __asm__("func_800963E4");
#endif
void xm_worldmap_stream_sort_disc_list(DiscReadRequest *list) __asm__("func_800963E4");
void xm_worldmap_stream_sort_disc_list(DiscReadRequest *list) {
    DiscReadRequest *first;
    DiscReadRequest *p;
    DiscReadRequest swap;

    p = list;
    first = p;
    while (p[1].sector != 0) {
        if ((u32)p[0].sector > (u32)p[1].sector) {
            swap.sector = p[0].sector;
            swap.bytes = p[0].bytes;
            swap.destination = p[0].destination;
            p[0].sector = p[1].sector;
            p[0].bytes = p[1].bytes;
            p[0].destination = p[1].destination;
            p[1].sector = swap.sector;
            p[1].bytes = swap.bytes;
            p[1].destination = swap.destination;
            if (first < p) {
                p--;
            }
        } else {
            p++;
        }
    }
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_stream_sort_host_list (src/decomp/src/worldmap/worldmap_movement_terrain.c:994). */
#ifndef XM_S_worldmap_stream_sort_host_list
#define XM_S_worldmap_stream_sort_host_list
void xm_worldmap_stream_sort_host_list(HostReadRequest *list) __asm__("func_800964B0");
#endif
void xm_worldmap_stream_sort_host_list(HostReadRequest *list) __asm__("func_800964B0");
void xm_worldmap_stream_sort_host_list(HostReadRequest *list) {
    HostReadRequest *first;
    HostReadRequest *p;
    HostReadRequest swap;

    p = list;
    first = p;
    while (p[1].path != NULL) {
        if ((u32)p[0].offset > (u32)p[1].offset) {
            swap.path = p[0].path;
            swap.offset = p[0].offset;
            swap.bytes = p[0].bytes;
            swap.destination = p[0].destination;
            p[0].path = p[1].path;
            p[0].offset = p[1].offset;
            p[0].bytes = p[1].bytes;
            p[0].destination = p[1].destination;
            p[1].path = swap.path;
            p[1].offset = swap.offset;
            p[1].bytes = swap.bytes;
            p[1].destination = swap.destination;
            if (first < p) {
                p--;
            }
        } else {
            p++;
        }
    }
}
#endif


#ifndef XENO_PC_PORT
s32 func_800965A4(void) {
    s32 *temp_s0;

    temp_s0 = D_8009D3C0 + (D_8009BE44 * 0x580);
    if ((*temp_s0 != 0) && (D_8009C624[D_8009BE44] == 0)) {
        func_800964B0(temp_s0);
        D_8009D808 = 0;
        D_8009C624[D_8009BE44] = temp_s0;
        D_8009BE44 = (D_8009BE44 + 1) & 0xF;
        return 0;
    }
    D_8009D808 = 0;
    return -1;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80096668(void) {
    s32 var_v0;

    var_v0 = D_8009BE44 - D_8009BCB8;
    if (var_v0 < 0) {
        var_v0 += 0x10;
    }
    return var_v0;
}
#endif



#ifndef XENO_PC_PORT
void func_80096694(void) {
    do {
        Vsync(0);
        func_800967E4();
    } while (func_80096668() != 0);
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_stream_read_host_list (src/decomp/src/worldmap/worldmap_movement_terrain.c:1061). */
#ifndef XM_S_worldmap_stream_read_host_list
#define XM_S_worldmap_stream_read_host_list
void xm_worldmap_stream_read_host_list(HostReadRequest *request) __asm__("func_800966CC");
#endif
#ifndef XM_S_worldmap_stream_unread_cleared_word_1
#define XM_S_worldmap_stream_unread_cleared_word_1
extern s32 xm_worldmap_stream_unread_cleared_word_1 __asm__("D_8009BE48");
#endif
#ifndef XM_S_worldmap_stream_unread_cleared_word_2
#define XM_S_worldmap_stream_unread_cleared_word_2
extern s32 xm_worldmap_stream_unread_cleared_word_2 __asm__("D_8009CCB0");
#endif
#ifndef XM_S_worldmap_stream_status_error_count
#define XM_S_worldmap_stream_status_error_count
extern s32 xm_worldmap_stream_status_error_count __asm__("D_8009CCA8");
#endif
#ifndef XM_S_worldmap_stream_read_error_count
#define XM_S_worldmap_stream_read_error_count
extern s32 xm_worldmap_stream_read_error_count __asm__("D_8009CCA0");
#endif
#ifndef XM_S_PCopen
#define XM_S_PCopen
int xm_PCopen(char *name, int flags, int perms) __asm__("PCopen");
#endif
#ifndef XM_S_PClseek
#define XM_S_PClseek
int xm_PClseek(int fd, int offset, int mode) __asm__("PClseek");
#endif
#ifndef XM_S_PCread
#define XM_S_PCread
int xm_PCread(int fd, char *buff, int len) __asm__("PCread");
#endif
#ifndef XM_S_PCclose
#define XM_S_PCclose
int xm_PCclose(int fd) __asm__("PCclose");
#endif
void func_800966CC(HostReadRequest *request) {
    s32 fd;
    s32 i;

    xm_worldmap_stream_unread_cleared_word_1 = 0;
    xm_worldmap_stream_unread_cleared_word_2 = 0;
    xm_worldmap_stream_status_error_count = 0;
    xm_worldmap_stream_read_error_count = 0;
    for (; request->path != NULL; request++) {
        for (i = 0; i < 8; i++) {
            fd = xm_PCopen(request->path, 0, 0);
            if (fd != -1) {
                break;
            }
        }
        if (fd == -1) {
            continue;
        }
        xm_PClseek(fd, request->offset, 0);
        for (i = 0; i < 8; i++) {
            if (xm_PCread(fd, request->destination, request->bytes) != 0) {
                break;
            }
        }
        for (i = 0; i < 8; i++) {
            if (xm_PCclose(fd) == 0) {
                break;
            }
        }
    }
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_stream_step (src/decomp/src/worldmap/worldmap_movement_terrain.c:1095). */
#ifndef XM_S_worldmap_stream_step_reader
#define XM_S_worldmap_stream_step_reader
s32 xm_worldmap_stream_step_reader(void) __asm__("func_800968E0");
#endif
#ifndef XM_S_worldmap_stream_start_disc_list
#define XM_S_worldmap_stream_start_disc_list
void xm_worldmap_stream_start_disc_list(DiscReadRequest *request) __asm__("func_8009699C");
#endif
s32 xm_worldmap_stream_step(void) __asm__("func_800967E4");
s32 xm_worldmap_stream_step(void) {
    s32 first;
    s32 second;
    s32 status;

    status = 0;
    first = xm_cd_has_pc_file_server();
    second = xm_cd_has_pc_file_server();
    if ((first == 0) | (second == -1)) {
        status = xm_worldmap_stream_step_reader();
        if (status == 0 && xm_worldmap_stream_disc_lists[xm_worldmap_stream_read_slot] != NULL) {
            xm_worldmap_stream_start_disc_list(xm_worldmap_stream_disc_lists[xm_worldmap_stream_read_slot]);
        }
    } else if (xm_worldmap_stream_host_lists[xm_worldmap_stream_read_slot] != NULL) {
        xm_worldmap_stream_read_host_list(xm_worldmap_stream_host_lists[xm_worldmap_stream_read_slot]);
        xm_worldmap_stream_host_lists[xm_worldmap_stream_read_slot] = NULL;
        xm_worldmap_stream_read_slot = (xm_worldmap_stream_read_slot + 1) & 0xF;
    }
    return status;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_stream_step_reader (src/decomp/src/worldmap/worldmap_movement_terrain.c:1117). */
#ifndef XM_S_worldmap_stream_wait
#define XM_S_worldmap_stream_wait
extern s32 xm_worldmap_stream_wait __asm__("D_8009BD2C");
#endif
__asm__(".xeno_jt 8 24 jtbl_80070CA0");
s32 func_800968E0(void) {
    switch (xm_worldmap_stream_state) {
    case 0:
        return 0;
    case 4:
        if (--xm_worldmap_stream_wait == 0) {
            xm_worldmap_stream_state++;
        }
    case 1:
    case 2:
    case 3:
        return 1;
    case 5:
        xm_worldmap_stream_state = 0;
        xm_worldmap_stream_disc_lists[xm_worldmap_stream_read_slot] = NULL;
        xm_worldmap_stream_read_slot = (xm_worldmap_stream_read_slot + 1) & 0xF;
        return 2;
    default:
        return 3;
    }
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8009699C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80096A6C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80096C0C);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_camera_place_orbit (src/decomp/src/worldmap/worldmap_movement_terrain.c:1295). */
#ifndef XM_T_OrbitScratch
#define XM_T_OrbitScratch
typedef struct {
    VECTOR offset;    
    VECTOR eye;       
    u8 pad20[0x80];
    SVECTOR angle;    
    u8 padA8[0x48];
    MATRIX rotation;  
} OrbitScratch;
#endif
#ifndef XM_T_ORBIT_SCRATCH
#define XM_T_ORBIT_SCRATCH
#define ORBIT_SCRATCH ((OrbitScratch *)0x1F800000)
#endif
void func_80096F18(ViewSetup *view, Camera *camera, s32 distance, SVECTOR *angle) {
    SVECTOR *rotation;

    view->at.vx = 0;
    view->at.vy = camera->target.vy >> 12;
    view->at.vz = 0;
    ORBIT_SCRATCH->angle.vx = angle->vx;
    rotation = &ORBIT_SCRATCH->angle;
    ORBIT_SCRATCH->angle.vy = angle->vy;
    ORBIT_SCRATCH->angle.vz = 0;
    xm_RotMatrixYXZ(&ORBIT_SCRATCH->angle, &ORBIT_SCRATCH->rotation);
    ORBIT_SCRATCH->offset.vx = 0;
    ORBIT_SCRATCH->offset.vy = 0;
    ORBIT_SCRATCH->offset.vz = -(distance >> 12);
    xm_ApplyMatrixLV(&ORBIT_SCRATCH->rotation, &ORBIT_SCRATCH->offset, &ORBIT_SCRATCH->eye);
    view->eye.vx = ORBIT_SCRATCH->eye.vx;
    view->eye.vy = view->at.vy + ORBIT_SCRATCH->eye.vy;
    view->eye.vz = ORBIT_SCRATCH->eye.vz;
    rotation->vx = 0;
    rotation->vy = angle->vy;
    rotation->vz = angle->vz;
    xm_RotMatrixYXZ(rotation, &ORBIT_SCRATCH->rotation);
    rotation->vx = 0;
    rotation->vy = -0x1000;
    rotation->vz = 0;
    xm_ApplyMatrix(&ORBIT_SCRATCH->rotation, rotation, &view->up);
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_get_matrix_angles (src/decomp/src/worldmap/worldmap_movement_terrain.c:1324). */
#ifndef XM_T_SCRATCH_MATRIX_A
#define XM_T_SCRATCH_MATRIX_A
#define SCRATCH_MATRIX_A ((MATRIX *)0x1F8000F0)
#endif
#ifndef XM_T_SCRATCH_MATRIX_B
#define XM_T_SCRATCH_MATRIX_B
#define SCRATCH_MATRIX_B ((MATRIX *)0x1F800110)
#endif
#ifndef XM_T_SCRATCH_MATRIX_C
#define XM_T_SCRATCH_MATRIX_C
#define SCRATCH_MATRIX_C ((MATRIX *)0x1F800130)
#endif
#ifndef XM_S_RotMatrixY
#define XM_S_RotMatrixY
MATRIX *xm_RotMatrixY(long r, MATRIX *m) __asm__("RotMatrixY");
#endif
#ifndef XM_S_MulMatrix0
#define XM_S_MulMatrix0
MATRIX *xm_MulMatrix0(MATRIX *m0, MATRIX *m1, MATRIX *m2) __asm__("MulMatrix0");
#endif
#ifndef XM_S_RotMatrixX
#define XM_S_RotMatrixX
MATRIX *xm_RotMatrixX(long r, MATRIX *m) __asm__("RotMatrixX");
#endif
void func_80097070(MATRIX *m, SVECTOR *angle) {
    if (m->m[2][0] | m->m[2][2]) {
        angle->vy = xm_ratan2(m->m[2][0], m->m[2][2]) & 0xFFF;
        *SCRATCH_MATRIX_A = *m;
        *SCRATCH_MATRIX_B = xm_worldmap_identity_matrix;
        xm_RotMatrixY(angle->vy, SCRATCH_MATRIX_B);
        xm_MulMatrix0(SCRATCH_MATRIX_A, SCRATCH_MATRIX_B, SCRATCH_MATRIX_C);
        angle->vx = xm_ratan2(SCRATCH_MATRIX_C->m[1][2], SCRATCH_MATRIX_C->m[1][1]);
        *SCRATCH_MATRIX_B = xm_worldmap_identity_matrix;
        xm_RotMatrixX(angle->vx, SCRATCH_MATRIX_B);
        xm_MulMatrix0(SCRATCH_MATRIX_C, SCRATCH_MATRIX_B, SCRATCH_MATRIX_A);
        angle->vz = -xm_ratan2(SCRATCH_MATRIX_A->m[1][0], SCRATCH_MATRIX_A->m[1][1]);
    }
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_camera_build_look_at (src/decomp/src/worldmap/worldmap_movement_terrain.c:1352). */
#ifndef XM_T_LookAtScratch
#define XM_T_LookAtScratch
typedef struct {
    VECTOR work;
    VECTOR right;
    VECTOR up;
    VECTOR forward;
    SVECTOR eye;
    MATRIX view;
} LookAtScratch;
#endif
#ifndef XM_T_LOOKAT_SCRATCH
#define XM_T_LOOKAT_SCRATCH
#define LOOKAT_SCRATCH ((LookAtScratch *)0x1F800000)
#endif
#ifndef XM_S_OuterProduct12
#define XM_S_OuterProduct12
void xm_OuterProduct12(VECTOR *v0, VECTOR *v1, VECTOR *v2) __asm__("OuterProduct12");
#endif
#ifndef XM_S_TransMatrix
#define XM_S_TransMatrix
MATRIX *xm_TransMatrix(MATRIX *m, VECTOR *v) __asm__("TransMatrix");
#endif
void func_80097244(void *arg) {
    ViewSetup *view;

    view = arg;
    LOOKAT_SCRATCH->work.vx = -view->eye.vx + view->at.vx;
    LOOKAT_SCRATCH->work.vy = -view->eye.vy + view->at.vy;
    LOOKAT_SCRATCH->work.vz = -view->eye.vz + view->at.vz;
    xm_VectorNormal(&LOOKAT_SCRATCH->work, &LOOKAT_SCRATCH->forward);
    xm_OuterProduct12(&LOOKAT_SCRATCH->forward, &view->up, &LOOKAT_SCRATCH->work);
    xm_VectorNormal(&LOOKAT_SCRATCH->work, &LOOKAT_SCRATCH->right);
    xm_OuterProduct12(&LOOKAT_SCRATCH->forward, &LOOKAT_SCRATCH->right, &LOOKAT_SCRATCH->work);
    xm_VectorNormal(&LOOKAT_SCRATCH->work, &LOOKAT_SCRATCH->up);
    xm_worldmap_camera_matrix.m[0][0] = LOOKAT_SCRATCH->right.vx;
    xm_worldmap_camera_matrix.m[0][1] = LOOKAT_SCRATCH->right.vy;
    xm_worldmap_camera_matrix.m[0][2] = LOOKAT_SCRATCH->right.vz;
    xm_worldmap_camera_matrix.m[1][0] = LOOKAT_SCRATCH->up.vx;
    xm_worldmap_camera_matrix.m[1][1] = LOOKAT_SCRATCH->up.vy;
    xm_worldmap_camera_matrix.m[1][2] = LOOKAT_SCRATCH->up.vz;
    xm_worldmap_camera_matrix.m[2][0] = LOOKAT_SCRATCH->forward.vx;
    xm_worldmap_camera_matrix.m[2][1] = LOOKAT_SCRATCH->forward.vy;
    xm_worldmap_camera_matrix.m[2][2] = LOOKAT_SCRATCH->forward.vz;
    LOOKAT_SCRATCH->eye.vx = -view->eye.vx;
    LOOKAT_SCRATCH->eye.vy = -view->eye.vy;
    LOOKAT_SCRATCH->eye.vz = -view->eye.vz;
    LOOKAT_SCRATCH->view = xm_worldmap_camera_matrix;
    xm_ApplyMatrix(&LOOKAT_SCRATCH->view, &LOOKAT_SCRATCH->eye, &LOOKAT_SCRATCH->work);
    xm_TransMatrix(&xm_worldmap_camera_matrix, &LOOKAT_SCRATCH->work);
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_camera_build_from_angles (src/decomp/src/worldmap/worldmap_movement_terrain.c:1382). */
#ifndef XM_T_SCRATCH_MATRIX_D
#define XM_T_SCRATCH_MATRIX_D
#define SCRATCH_MATRIX_D ((MATRIX *)0x1F800150)
#endif
#ifndef XM_T_SCRATCH_SVECTOR
#define XM_T_SCRATCH_SVECTOR
#define SCRATCH_SVECTOR ((SVECTOR *)0x1F8000A0)
#endif
#ifndef XM_T_SCRATCH_VECTOR
#define XM_T_SCRATCH_VECTOR
#define SCRATCH_VECTOR ((VECTOR *)0x1F800000)
#endif
#ifndef XM_S_RotMatrixZ
#define XM_S_RotMatrixZ
MATRIX *xm_RotMatrixZ(long r, MATRIX *m) __asm__("RotMatrixZ");
#endif
void func_80097440(void *arg) {
    SVECTOR *eye;

    eye = arg;
    *SCRATCH_MATRIX_A = xm_worldmap_identity_matrix;
    *SCRATCH_MATRIX_B = *SCRATCH_MATRIX_A;
    *SCRATCH_MATRIX_C = *SCRATCH_MATRIX_A;
    xm_RotMatrixX(-xm_worldmap_camera_angle.vx, SCRATCH_MATRIX_A);
    xm_RotMatrixY(-xm_worldmap_camera_angle.vy, SCRATCH_MATRIX_B);
    xm_RotMatrixZ(-xm_worldmap_camera_angle.vz, SCRATCH_MATRIX_C);
    xm_MulMatrix0(SCRATCH_MATRIX_A, SCRATCH_MATRIX_B, SCRATCH_MATRIX_D);
    xm_MulMatrix0(SCRATCH_MATRIX_C, SCRATCH_MATRIX_D, &xm_worldmap_camera_matrix);
    SCRATCH_SVECTOR->vx = -eye->vx;
    SCRATCH_SVECTOR->vy = -eye->vy;
    SCRATCH_SVECTOR->vz = -eye->vz;
    *SCRATCH_MATRIX_A = xm_worldmap_camera_matrix;
    xm_ApplyMatrix(SCRATCH_MATRIX_A, SCRATCH_SVECTOR, SCRATCH_VECTOR);
    xm_TransMatrix(&xm_worldmap_camera_matrix, SCRATCH_VECTOR);
}
#endif


#ifndef XENO_PC_PORT
void func_8009766C(void) {
    D_8009BE24 = (WmObj *)HeapAlloc(0x2000, 0);
    func_800976C8();
}
#endif



#ifndef XENO_PC_PORT
void func_800976A0(void) {
    HeapFree((s32)D_8009BE24);
}
#endif



#ifndef XENO_PC_PORT
void func_800976C8(void) {
    s32 i;

    for (i = 0; i < 0x40; i++) {
        D_8009BE24[i].unk4C = 0;
        D_8009BE24[i].unk18 = 0;
        D_8009BE24[i].unk1C = 0;
    }
}
#endif



#ifndef XENO_PC_PORT
void func_800976FC(s32 arg0, s32 arg1) {
    void *temp_a1;

    temp_a1 = (arg1 << 7) + (uintptr_t)D_8009BE24;
    *(s16 *)((s8*)(temp_a1) + 0) = 0;
    *(s32 *)((s8*)(temp_a1) + 0x18) = arg0;
}
#endif



#ifndef XENO_PC_PORT
void func_80097718(s32 arg0, s32 arg1) {
    s32 var_a2;
    void *var_v1;

    var_a2 = 0;
    var_v1 = ((void *)D_8009BE24);
loop_1:
    if (*(s32 *)((s8*)(var_v1) + 0x1C) == 0) {
        *(s16 *)((s8*)(var_v1) + 0) = 0;
        *(s16 *)((s8*)(var_v1) + 2) = 0;
        *(s16 *)((s8*)(var_v1) + 4) = 0;
        *(s32 *)((s8*)(var_v1) + 0x18) = arg0;
        *(s32 *)((s8*)(var_v1) + 0x1C) = arg1;
        *(s16 *)((s8*)(var_v1) + 0x20) = 0;
        *(s16 *)((s8*)(var_v1) + 0x22) = 0;
        return;
    }
    var_a2 += 1;
    var_v1 += 0x80;
    if (var_a2 >= 0x40) {
        return;
    }
    goto loop_1;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80097770(s32 arg0, s32 arg1) {
    void *temp_a0;

    temp_a0 = (u8 *)D_8009BE24 + (arg0 << 7);
    if (*(s16 *)((s8*)(temp_a0) + 4) != 0) {
        return 0;
    }
    *(s16 *)((s8*)(temp_a0) + 0) = 1;
    *(s16 *)((s8*)(temp_a0) + 4) = arg1;
    return 1;
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_actor_send_idle (src/decomp/src/worldmap/worldmap_movement_terrain.c:1466). */
#ifndef XM_S_worldmap_actor_send_idle
#define XM_S_worldmap_actor_send_idle
void xm_worldmap_actor_send_idle(s32 index) __asm__("func_800977A8");
#endif
void func_800977A8(s32 index) {
    WorldmapActor *actor;

    actor = &xm_worldmap_actor_slots[index];
    actor->command = 3;
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_actor_send_release (src/decomp/src/worldmap/worldmap_movement_terrain.c:1474). */
#ifndef XM_S_worldmap_actor_send_release
#define XM_S_worldmap_actor_send_release
void xm_worldmap_actor_send_release(s32 index) __asm__("func_800977C4");
#endif
void func_800977C4(s32 index) {
    WorldmapActor *actor;

    actor = &xm_worldmap_actor_slots[index];
    actor->command = 4;
}
#endif
#ifndef XENO_PC_PORT
void func_800977E0(s32 arg0, s16 arg1) {
    void *temp_v0;

    temp_v0 = (u8 *)D_8009BE24 + (arg0 << 7);
    *(s16 *)((s8*)(temp_v0) + 0) = 2;
    *(s16 *)((s8*)(temp_v0) + 2) = arg1;
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_actor_run_all (src/decomp/src/worldmap/worldmap_movement_terrain.c:1492). */
#ifndef XM_T_ActorFunc
#define XM_T_ActorFunc
typedef s32 (*ActorFunc)(s32 index);
#endif
#ifndef XM_S_worldmap_actor_run_all
#define XM_S_worldmap_actor_run_all
void xm_worldmap_actor_run_all(void) __asm__("func_80097800");
#endif
void func_80097800(void) {
    WorldmapActor *actor;
    s32 i;

    actor = xm_worldmap_actor_slots;
    for (i = 0; i < 0x40; i++, actor++) {
        if (actor->update != 0) {
            switch (actor->command) {
            case 3:
                break;
            case 0:
                actor->command = ((ActorFunc)actor->kind)(i);
                break;
            case 1:
                actor->command = ((ActorFunc)actor->update)(i);
                break;
            case 2:
                if (--actor->command_arg <= 0) {
                    actor->command = 1;
                }
                break;
            case 4:
                if (actor->handle != NULL) {
                    xm_sprite_destroy(actor->handle);
                }
                break;
            }
        }
    }
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_terrain_alloc_packets (src/decomp/src/worldmap/worldmap_movement_terrain.c:1529). */
#ifndef XM_T_xm_DisplayBuffer
#define XM_T_xm_DisplayBuffer
typedef struct xm_DisplayBuffer {
    DRAWENV draw;
    DISPENV disp;
    u_long *ot;    
    void *packets; 
} xm_DisplayBuffer;
#endif
#ifndef XM_T_xm_TriangleBuffer
#define XM_T_xm_TriangleBuffer
typedef struct {
    POLY_FT3 prims[0x800];
} xm_TriangleBuffer;
#endif
void func_800978FC(void) {
    POLY_FT3 *prim;
    s32 i;

    xm_worldmap_display_buffers[0].packets = xm_heap_alloc(0x10000, 1);
    xm_worldmap_display_buffers[1].packets = xm_heap_alloc(0x10000, 1);
    prim = xm_worldmap_display_buffers[0].packets;
    for (i = 0; i < 0x800; i++, prim++) {
        setlen(prim, 7);
        setcode(prim, 0x24);
        setRGB0(prim, 0x80, 0x80, 0x80);
    }
    *(xm_TriangleBuffer *)xm_worldmap_display_buffers[1].packets = *(xm_TriangleBuffer *)xm_worldmap_display_buffers[0].packets;
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800979C8);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_terrain_reset_at (src/decomp/src/worldmap/worldmap_movement_terrain.c:1593). */
#ifndef XM_T_TERRAIN_ORIGIN
#define XM_T_TERRAIN_ORIGIN
#define TERRAIN_ORIGIN xm_worldmap_terrain_origin
#endif
#ifndef XM_S_worldmap_terrain_reset_at
#define XM_S_worldmap_terrain_reset_at
void xm_worldmap_terrain_reset_at(VECTOR *position) __asm__("func_80097BC0");
#endif
#ifndef XM_S_worldmap_terrain_matrix
#define XM_S_worldmap_terrain_matrix
extern MATRIX xm_worldmap_terrain_matrix __asm__("D_8009D534");
#endif
#ifndef XM_S_worldmap_terrain_queue_missing_blocks
#define XM_S_worldmap_terrain_queue_missing_blocks
void xm_worldmap_terrain_queue_missing_blocks(void) __asm__("func_80097DC0");
#endif
void xm_worldmap_terrain_reset_at(VECTOR *position) __asm__("func_80097BC0");
void xm_worldmap_terrain_reset_at(VECTOR *position) {
    s32 i;

    xm_worldmap_terrain_matrix = xm_worldmap_identity_matrix;
    for (i = 0xFF; i >= 0; i--) {
        xm_worldmap_terrain_blocks[i] = NULL;
    }
    TERRAIN_ORIGIN.vx = position->vx & 0x7FFFFF;
    TERRAIN_ORIGIN.vy = 0;
    xm_worldmap_wave_phase_x = 0;
    xm_worldmap_wave_phase_z = 0x400;
    TERRAIN_ORIGIN.vz = position->vz & 0x7FFFFF;
    xm_worldmap_camera_block_cell.vx = 2;
    xm_worldmap_camera_block_cell.vy = 0;
    xm_worldmap_camera_block_cell.vz = 2;
    xm_worldmap_terrain_update_grid((Camera *)position);
    xm_worldmap_terrain_queue_missing_blocks();
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_terrain_reset_at_camera (src/decomp/src/worldmap/worldmap_movement_terrain.c:1613). */
#ifndef XM_S_worldmap_terrain_reset_at_camera
#define XM_S_worldmap_terrain_reset_at_camera
void xm_worldmap_terrain_reset_at_camera(Camera *camera) __asm__("func_80097CB8");
#endif
void func_80097CB8(Camera *camera) {
    s32 i;

    xm_worldmap_terrain_matrix = xm_worldmap_identity_matrix;
    for (i = 0xFF; i >= 0; i--) {
        xm_worldmap_terrain_blocks[i] = NULL;
    }
    xm_worldmap_wave_phase_x = 0;
    xm_worldmap_wave_phase_z = 0x400;
    xm_worldmap_terrain_update_grid(camera);
    xm_worldmap_terrain_queue_missing_blocks();
}
#endif


#ifndef XENO_PC_PORT
void func_80097D64(void) {
    s32 *var_s0;
    s32 temp_a0;
    s32 var_s1;

    var_s1 = 0;
    var_s0 = D_8009C184;
    do {
        temp_a0 = *var_s0;
        if (temp_a0 != 0) {
            HeapFree(temp_a0);
        }
        var_s1 += 1;
        var_s0 += 1;
    } while (var_s1 < 0x100);
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_terrain_queue_missing_blocks (src/decomp/src/worldmap/worldmap_movement_terrain.c:1639). */
#ifndef XM_T_BlockGrid
#define XM_T_BlockGrid
typedef struct BlockGrid {
    s16 cells[81];
} BlockGrid;
#endif
#ifndef XM_S_cd_get_file_sector
#define XM_S_cd_get_file_sector
s32 xm_cd_get_file_sector(s32 file) __asm__("ArchiveDecodeSector");
#endif
#ifndef XM_S_worldmap_terrain_row_file
#define XM_S_worldmap_terrain_row_file
extern s32 xm_worldmap_terrain_row_file __asm__("D_8009BCD8");
#endif
#ifndef XM_S_worldmap_terrain_grid
#define XM_S_worldmap_terrain_grid
extern BlockGrid xm_worldmap_terrain_grid __asm__("D_8009D570");
#endif
#ifndef XM_S_worldmap_stream_submit_disc_list
#define XM_S_worldmap_stream_submit_disc_list
s32 xm_worldmap_stream_submit_disc_list(void) __asm__("func_80096328");
#endif
#ifndef XM_S_cd_get_pc_file_name
#define XM_S_cd_get_pc_file_name
char *xm_cd_get_pc_file_name(s32 file) __asm__("ArchiveGetFilePath");
#endif
#ifndef XM_S_worldmap_stream_submit_host_list
#define XM_S_worldmap_stream_submit_host_list
s32 xm_worldmap_stream_submit_host_list(void) __asm__("func_800965A4");
#endif
void func_80097DC0(void) {
    s32 first;
    s32 second;
    s32 row;
    s32 column;
    s32 block;
    void *buffer;
    s32 sector;
    char *path;

    first = xm_cd_has_pc_file_server();
    second = xm_cd_has_pc_file_server();
    if ((first == 0) | (second == -1)) {
        sector = xm_cd_get_file_sector(xm_worldmap_terrain_row_file);
        for (row = 3; row < 6; row++) {
            for (column = 3; column < 6; column++) {
                block = xm_worldmap_terrain_grid.cells[row * 9 + column];
                if (xm_worldmap_terrain_blocks[block] == NULL) {
                    buffer = xm_heap_alloc(0x710, 0);
                    xm_worldmap_terrain_blocks[block] = buffer;
                    xm_worldmap_stream_add_disc_request(sector + block, 0x710, buffer);
                }
            }
        }
        xm_worldmap_stream_add_disc_request(0, 0, NULL);
        xm_worldmap_stream_submit_disc_list();
        for (row = 0; row < 9; row++) {
            for (column = 0; column < 9; column++) {
                block = xm_worldmap_terrain_grid.cells[row * 9 + column];
                if (xm_worldmap_terrain_blocks[block] == NULL) {
                    buffer = xm_heap_alloc(0x710, 0);
                    xm_worldmap_terrain_blocks[block] = buffer;
                    xm_worldmap_stream_add_disc_request(sector + block, 0x710, buffer);
                }
            }
        }
        xm_worldmap_stream_add_disc_request(0, 0, NULL);
        xm_worldmap_stream_submit_disc_list();
    } else {
        path = xm_cd_get_pc_file_name(xm_worldmap_terrain_row_file);
        for (row = 0; row < 9; row++) {
            for (column = 0; column < 9; column++) {
                block = xm_worldmap_terrain_grid.cells[row * 9 + column];
                if (xm_worldmap_terrain_blocks[block] == NULL) {
                    buffer = xm_heap_alloc(0x710, 0);
                    xm_worldmap_terrain_blocks[block] = buffer;
                    xm_worldmap_stream_add_host_request(path, block << 11, 0x710, buffer);
                }
            }
        }
        xm_worldmap_stream_add_host_request(NULL, 0, 0, NULL);
        xm_worldmap_stream_submit_host_list();
    }
}
#endif


#ifndef XENO_PC_PORT
void func_80098044(void) {
    OuterProduct0(D_8009BB6C, D_8009BB4C, D_8009C828);
    OuterProduct0(D_8009BB4C, D_8009BB7C, D_8009C844);
    OuterProduct0(D_8009BB8C, D_8009BB5C, D_8009C874);
    OuterProduct0(D_8009BB5C, D_8009BB9C, D_8009C7F0);
}
#endif


#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_terrain_wrap_origin (src/decomp/src/worldmap/worldmap_movement_terrain.c:1704). */
void func_800980D4(void *arg) {
    VECTOR *position;
    s32 x;
    s32 z;

    position = arg;
    x = position->vx;
    z = position->vz;
    xm_worldmap_terrain_crossed_edges = 0;
    if (x < -0x800000) {
        position->vx = x + 0x800000;
        xm_worldmap_terrain_crossed_edges = 4;
    } else if (x > 0x800000) {
        position->vx = x - 0x800000;
        xm_worldmap_terrain_crossed_edges = 8;
    }
    if (z < -0x800000) {
        position->vz += 0x800000;
        xm_worldmap_terrain_crossed_edges |= 1;
    } else if (z > 0x800000) {
        position->vz -= 0x800000;
        xm_worldmap_terrain_crossed_edges |= 2;
    }
    xm_worldmap_camera_block_cell.vx = (position->vx >> 23) + 2;
    xm_worldmap_camera_block_cell.vz = (position->vz >> 23) + 2;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_terrain_update_grid (src/decomp/src/worldmap/worldmap_movement_terrain.c:1733). */
#ifndef XM_S_worldmap_terrain_previous_grid
#define XM_S_worldmap_terrain_previous_grid
extern BlockGrid xm_worldmap_terrain_previous_grid __asm__("D_8009D318");
#endif
void func_800981C8(Camera *camera) {
    s32 width;
    s32 height;
    s32 x;
    s32 z;
    s32 left;
    s32 base;
    s32 i;
    s32 j;
    s16 *cell;

    width = xm_worldmap_area_blocks_x;
    height = xm_worldmap_area_blocks_z;
    x = (camera->target.vx >> 12) / 8 - ((xm_worldmap_camera_block_cell.vx + 2) << 8);
    z = (camera->target.vz >> 12) / 8 - ((xm_worldmap_camera_block_cell.vz + 2) << 8);
    if (x < 0) {
        x += width << 8;
    } else if (x > width << 8) {
        x -= width << 8;
    }
    if (z < 0) {
        z += height << 8;
    } else if (z > height << 8) {
        z -= height << 8;
    }
    x >>= 8;
    z >>= 8;
    xm_worldmap_terrain_previous_grid = xm_worldmap_terrain_grid;
    left = x;
    cell = xm_worldmap_terrain_grid.cells;
    for (j = 8; j != -1; j--) {
        x = left;
        if (z >= height) {
            z = 0;
        }
        base = z * width;
        for (i = 8; i != -1; i--) {
            if (x >= width) {
                x = 0;
            }
            *cell++ = base + x;
            x++;
        }
        z++;
    }
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_terrain_classify_blocks (src/decomp/src/worldmap/worldmap_movement_terrain.c:1799). */
#ifndef XM_T_GridScratch
#define XM_T_GridScratch
typedef struct {
    VECTOR view[4];  
    s32 x0;          
    s32 pad44;
    s32 z0;          
    u8 pad4C[0x54];
    SVECTOR v[9];    
    u8 padE8[8];
    MATRIX local;    
    MATRIX world;    
} GridScratch;
#endif
#ifndef XM_S_worldmap_terrain_visible_blocks
#define XM_S_worldmap_terrain_visible_blocks
extern s16 xm_worldmap_terrain_visible_blocks[25] __asm__("D_8009D618");
#endif
#ifndef XM_S_worldmap_terrain_quarter_visibility
#define XM_S_worldmap_terrain_quarter_visibility
extern s16 xm_worldmap_terrain_quarter_visibility[25][4] __asm__("D_8009D650");
#endif
#ifndef XM_S_worldmap_terrain_classify_quad
#define XM_S_worldmap_terrain_classify_quad
s16 xm_worldmap_terrain_classify_quad(SVECTOR *a, SVECTOR *b, SVECTOR *c, SVECTOR *d) __asm__("func_800987AC");
#endif
#ifndef XM_S_worldmap_terrain_always_visible_quarters
#define XM_S_worldmap_terrain_always_visible_quarters
extern s16 xm_worldmap_terrain_always_visible_quarters[4][25][4] __asm__("D_8009B7A8");
#endif
#pragma push_macro("GRID_SCRATCH")
#undef GRID_SCRATCH
#define GRID_SCRATCH ((GridScratch *)0x1F800000)
void func_800983A0(Camera *camera) {
    s16 *block;
    u32 *quarters;
    u32 visible;
    s32 row;
    s32 column;
    s16 result;
    GridScratch *scratch;

    scratch = GRID_SCRATCH;
    GRID_SCRATCH->local = xm_worldmap_terrain_matrix;
    xm_CompMatrix(&xm_worldmap_camera_matrix, &GRID_SCRATCH->local, &GRID_SCRATCH->world);
    xm_SetRotMatrix(&GRID_SCRATCH->world);
    xm_SetTransMatrix(&GRID_SCRATCH->world);
    block = xm_worldmap_terrain_visible_blocks;
    quarters = (u32 *)xm_worldmap_terrain_quarter_visibility[0];
    GRID_SCRATCH->x0 = -((camera->target.vx >> 12) & 0x7FF) - 0x1000;
    GRID_SCRATCH->z0 = -((camera->target.vz >> 12) & 0x7FF) - 0x1000;
    GRID_SCRATCH->v[8].vy = 0;
    GRID_SCRATCH->v[7].vy = 0;
    GRID_SCRATCH->v[6].vy = 0;
    GRID_SCRATCH->v[5].vy = 0;
    GRID_SCRATCH->v[4].vy = 0;
    GRID_SCRATCH->v[3].vy = 0;
    GRID_SCRATCH->v[2].vy = 0;
    GRID_SCRATCH->v[1].vy = 0;
    GRID_SCRATCH->v[0].vy = 0;
    GRID_SCRATCH->v[0].vz = -GRID_SCRATCH->z0;
    for (row = 0; row < 5; row++) {
        scratch->v[0].vx = scratch->x0;
        for (column = 0; column < 5; quarters += 2, scratch->v[0].vx += 0x800, block++, column++) {
            scratch->v[1].vx = scratch->v[0].vx + 0x800;
            scratch->v[1].vz = scratch->v[0].vz;
            scratch->v[2].vx = scratch->v[0].vx;
            scratch->v[2].vz = scratch->v[0].vz - 0x800;
            scratch->v[3].vx = scratch->v[0].vx + 0x800;
            scratch->v[3].vz = scratch->v[0].vz - 0x800;
            result = xm_worldmap_terrain_classify_quad(&scratch->v[0], &scratch->v[1], &scratch->v[2],
                                   &scratch->v[3]);
            *block = result;
            if (result == 0) {
                scratch->v[4].vx = scratch->v[0].vx + 0x400;
                scratch->v[4].vz = scratch->v[0].vz;
                scratch->v[5].vx = scratch->v[0].vx;
                scratch->v[5].vz = scratch->v[0].vz - 0x400;
                scratch->v[6].vx = scratch->v[1].vx;
                scratch->v[6].vz = scratch->v[5].vz;
                scratch->v[7].vx = scratch->v[4].vx;
                scratch->v[7].vz = scratch->v[2].vz;
                scratch->v[8].vx = scratch->v[4].vx;
                scratch->v[8].vz = scratch->v[5].vz;
                visible = xm_worldmap_terrain_classify_quad(&scratch->v[0], &scratch->v[4], &scratch->v[5],
                                        &scratch->v[8]) & 0xFFFF;
                visible |= xm_worldmap_terrain_classify_quad(&scratch->v[4], &scratch->v[1], &scratch->v[8],
                                         &scratch->v[6]) << 16;
                quarters[0] = visible;
                visible = xm_worldmap_terrain_classify_quad(&scratch->v[5], &scratch->v[8], &scratch->v[2],
                                        &scratch->v[7]) & 0xFFFF;
                visible |= xm_worldmap_terrain_classify_quad(&scratch->v[8], &scratch->v[6], &scratch->v[7],
                                         &scratch->v[3]) << 16;
            } else {
                visible = result & 0xFFFF;
                visible |= visible << 16;
                quarters[0] = visible;
            }
            quarters[1] = visible;
        }
        scratch->v[0].vz -= 0x800;
    }
    
    column = 0;
    if (((camera->target.vx >> 12) & 0x7FF) >= 0x400) {
        column = 1;
    }
    if (((camera->target.vz >> 12) & 0x7FF) >= 0x400) {
        column |= 2;
    }
    block = (s16 *)xm_worldmap_terrain_always_visible_quarters[column][0];
    quarters = (u32 *)xm_worldmap_terrain_quarter_visibility[0];
    for (row = 0; row < 25; row++) {
        quarters[0] |= ((u32 *)block)[0];
        quarters[1] |= ((u32 *)block)[1];
        block += 4;
        quarters += 2;
    }
}
#pragma pop_macro("GRID_SCRATCH")
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_terrain_classify_quad (src/decomp/src/worldmap/worldmap_movement_terrain.c:1892). */
#ifndef XM_T_GRID_SCRATCH
#define XM_T_GRID_SCRATCH
#define GRID_SCRATCH ((GridScratch *)0x1F800000)
#endif
#ifndef XM_T_gte_rt
#define XM_T_gte_rt
#define gte_rt() __asm__ volatile("nop;nop;.word 0x4A480012")
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
#ifndef XM_S_worldmap_terrain_cull_normal_0
#define XM_S_worldmap_terrain_cull_normal_0
extern VECTOR xm_worldmap_terrain_cull_normal_0 __asm__("D_8009C828");
#endif
#ifndef XM_S_worldmap_terrain_cull_normal_1
#define XM_S_worldmap_terrain_cull_normal_1
extern VECTOR xm_worldmap_terrain_cull_normal_1 __asm__("D_8009C844");
#endif
#ifndef XM_S_worldmap_terrain_cull_normal_2
#define XM_S_worldmap_terrain_cull_normal_2
extern VECTOR xm_worldmap_terrain_cull_normal_2 __asm__("D_8009C874");
#endif
#ifndef XM_S_worldmap_terrain_cull_normal_3
#define XM_S_worldmap_terrain_cull_normal_3
extern VECTOR xm_worldmap_terrain_cull_normal_3 __asm__("D_8009C7F0");
#endif
s16 func_800987AC(SVECTOR *a, SVECTOR *b, SVECTOR *c, SVECTOR *d) {
    VECTOR *view = GRID_SCRATCH->view;
    s16 out0;
    s16 out1;
    s16 out2;
    s16 out3;
    s32 d0;
    s32 d1;
    s32 d2;
    s32 d3;

    gte_ldv0(a);
    gte_rt();
    gte_stlvnl(&view[0]);
    gte_ldv0(b);
    gte_rt();
    gte_stlvnl(&view[1]);
    gte_ldv0(c);
    gte_rt();
    gte_stlvnl(&view[2]);
    gte_ldv0(d);
    gte_rt();
    gte_stlvnl(&view[3]);

    out0 = 0;
    d0 = (view[0].vx * xm_worldmap_terrain_cull_normal_0.vx + view[0].vz * xm_worldmap_terrain_cull_normal_0.vz) >> 12;
    d1 = (view[1].vx * xm_worldmap_terrain_cull_normal_0.vx + view[1].vz * xm_worldmap_terrain_cull_normal_0.vz) >> 12;
    d2 = (view[2].vx * xm_worldmap_terrain_cull_normal_0.vx + view[2].vz * xm_worldmap_terrain_cull_normal_0.vz) >> 12;
    d3 = (view[3].vx * xm_worldmap_terrain_cull_normal_0.vx + view[3].vz * xm_worldmap_terrain_cull_normal_0.vz) >> 12;
    if (d0 < 0 && d1 < 0) {
        out0++;
    }
    if (d1 < 0 && d3 < 0) {
        out0++;
    }
    if (d3 < 0 && d2 < 0) {
        out0++;
    }
    if (d2 < 0 && d0 < 0) {
        out0++;
    }
    if (out0 == 4) {
        return -1;
    }

    out1 = 0;
    d0 = (view[0].vx * xm_worldmap_terrain_cull_normal_1.vx + view[0].vz * xm_worldmap_terrain_cull_normal_1.vz) >> 12;
    d1 = (view[1].vx * xm_worldmap_terrain_cull_normal_1.vx + view[1].vz * xm_worldmap_terrain_cull_normal_1.vz) >> 12;
    d2 = (view[2].vx * xm_worldmap_terrain_cull_normal_1.vx + view[2].vz * xm_worldmap_terrain_cull_normal_1.vz) >> 12;
    d3 = (view[3].vx * xm_worldmap_terrain_cull_normal_1.vx + view[3].vz * xm_worldmap_terrain_cull_normal_1.vz) >> 12;
    if (d0 < 0 && d1 < 0) {
        out1++;
    }
    if (d1 < 0 && d3 < 0) {
        out1++;
    }
    if (d3 < 0 && d2 < 0) {
        out1++;
    }
    if (d2 < 0 && d0 < 0) {
        out1++;
    }
    if (out1 == 4) {
        return -1;
    }

    out2 = 0;
    d0 = (view[0].vy * xm_worldmap_terrain_cull_normal_2.vy + view[0].vz * xm_worldmap_terrain_cull_normal_2.vz) >> 12;
    d1 = (view[1].vy * xm_worldmap_terrain_cull_normal_2.vy + view[1].vz * xm_worldmap_terrain_cull_normal_2.vz) >> 12;
    d2 = (view[2].vy * xm_worldmap_terrain_cull_normal_2.vy + view[2].vz * xm_worldmap_terrain_cull_normal_2.vz) >> 12;
    d3 = (view[3].vy * xm_worldmap_terrain_cull_normal_2.vy + view[3].vz * xm_worldmap_terrain_cull_normal_2.vz) >> 12;
    if (d0 < 0 && d1 < 0) {
        out2++;
    }
    if (d1 < 0 && d3 < 0) {
        out2++;
    }
    if (d3 < 0 && d2 < 0) {
        out2++;
    }
    if (d2 < 0 && d0 < 0) {
        out2++;
    }
    if (out2 == 4) {
        return -1;
    }

    out3 = 0;
    d0 = (view[0].vy * xm_worldmap_terrain_cull_normal_3.vy + view[0].vz * xm_worldmap_terrain_cull_normal_3.vz) >> 12;
    d1 = (view[1].vy * xm_worldmap_terrain_cull_normal_3.vy + view[1].vz * xm_worldmap_terrain_cull_normal_3.vz) >> 12;
    d2 = (view[2].vy * xm_worldmap_terrain_cull_normal_3.vy + view[2].vz * xm_worldmap_terrain_cull_normal_3.vz) >> 12;
    d3 = (view[3].vy * xm_worldmap_terrain_cull_normal_3.vy + view[3].vz * xm_worldmap_terrain_cull_normal_3.vz) >> 12;
    if (d0 < 0 && d1 < 0) {
        out3++;
    }
    if (d1 < 0 && d3 < 0) {
        out3++;
    }
    if (d3 < 0 && d2 < 0) {
        out3++;
    }
    if (d2 < 0 && d0 < 0) {
        out3++;
    }
    if (out3 != 4) {
        return (out1 | out0 | out2 | out3) == 0;
    }
    return -1;
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80098CC0);

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_terrain_draw (src/decomp/src/worldmap/worldmap_movement_terrain.c:2165). */
#ifndef XM_T_TerrainDrawScratch
#define XM_T_TerrainDrawScratch
typedef struct {
    u8 pad0[0x288];
    u16 clut[0x40];    
    u16 tpage[8];      
    s32 x0;            
    s32 pad31C;
    s32 z0;            
    s32 pad324;
    SVECTOR corner[4]; 
    u8 pad348[8];
    MATRIX local;      
    MATRIX world;      
} TerrainDrawScratch;
#endif
#ifndef XM_S_worldmap_terrain_cluts
#define XM_S_worldmap_terrain_cluts
extern u16 xm_worldmap_terrain_cluts[0x40] __asm__("D_8009CCB4");
#endif
#ifndef XM_S_worldmap_terrain_tpages
#define XM_S_worldmap_terrain_tpages
extern u16 xm_worldmap_terrain_tpages[7] __asm__("D_8009CD54");
#endif
#ifndef XM_S_worldmap_terrain_packet_count
#define XM_S_worldmap_terrain_packet_count
extern s32 xm_worldmap_terrain_packet_count __asm__("D_8009D7DC");
#endif
#ifndef XM_S_worldmap_terrain_build_and_draw_quarter
#define XM_S_worldmap_terrain_build_and_draw_quarter
void xm_worldmap_terrain_build_and_draw_quarter(u32 *heights, u_long *ot, s32 packets, SVECTOR *origin) __asm__("func_80099708");
#endif
void func_8009932C(u_long *ot, s32 packets, Camera *camera) {
    TerrainDrawScratch *scratch;
    u8 *data;
    s32 row;
    s32 column;
    s32 cell;
    s16 all;

    scratch = (TerrainDrawScratch *)0x1F800000;
    for (column = 0; column < 0x40; column++) {
        scratch->clut[column] = xm_worldmap_terrain_cluts[column];
    }
    for (column = 0; column < 7; column++) {
        scratch->tpage[column] = xm_worldmap_terrain_tpages[column];
    }
    scratch->local = xm_worldmap_terrain_matrix;
    xm_CompMatrix(&xm_worldmap_camera_matrix, &scratch->local, &scratch->world);
    xm_SetRotMatrix(&scratch->world);
    xm_SetTransMatrix(&scratch->world);
    scratch->x0 = -((camera->target.vx >> 12) & 0x7FF) - 0x1000;
    cell = 0;
    scratch->z0 = -((camera->target.vz >> 12) & 0x7FF) - 0x1000;
    xm_worldmap_terrain_packet_count = 0;
    scratch->corner[0].vz = -scratch->z0;
    for (row = 0; row < 5; row++) {
        scratch->corner[0].vx = scratch->x0;
        for (column = 0; column < 5; column++, cell++, scratch->corner[0].vx += 0x800) {
            if (xm_worldmap_terrain_visible_blocks[cell] != -1) {
                data = xm_worldmap_terrain_blocks[xm_worldmap_terrain_grid.cells[(row + xm_worldmap_camera_block_cell.vz) * 9 + column + xm_worldmap_camera_block_cell.vx]];
                scratch->corner[3].vx = scratch->corner[1].vx = scratch->corner[0].vx + 0x400;
                scratch->corner[1].vz = scratch->corner[0].vz;
                scratch->corner[2].vx = scratch->corner[0].vx;
                scratch->corner[3].vz = scratch->corner[2].vz = scratch->corner[0].vz - 0x400;
                all = (u16)xm_worldmap_terrain_quarter_visibility[cell][3] |
                      ((u16)xm_worldmap_terrain_quarter_visibility[cell][2] |
                       ((u16)xm_worldmap_terrain_quarter_visibility[cell][0] | (u16)xm_worldmap_terrain_quarter_visibility[cell][1]));
                if (all != -1) {
                    xm_worldmap_terrain_build_and_draw_quarter((u32 *)data, ot, packets + (xm_worldmap_terrain_packet_count << 5), &scratch->corner[0]);
                    xm_worldmap_terrain_build_and_draw_quarter((u32 *)(data + 0x144), ot, packets + (xm_worldmap_terrain_packet_count << 5), &scratch->corner[1]);
                    xm_worldmap_terrain_build_and_draw_quarter((u32 *)(data + 0x288), ot, packets + (xm_worldmap_terrain_packet_count << 5), &scratch->corner[2]);
                    xm_worldmap_terrain_build_and_draw_quarter((u32 *)(data + 0x3CC), ot, packets + (xm_worldmap_terrain_packet_count << 5), &scratch->corner[3]);
                } else {
                    if (xm_worldmap_terrain_quarter_visibility[cell][0] != all) {
                        xm_worldmap_terrain_build_and_draw_quarter((u32 *)data, ot, packets + (xm_worldmap_terrain_packet_count << 5), &scratch->corner[0]);
                    }
                    if (xm_worldmap_terrain_quarter_visibility[cell][1] != all) {
                        xm_worldmap_terrain_build_and_draw_quarter((u32 *)(data + 0x144), ot, packets + (xm_worldmap_terrain_packet_count << 5), &scratch->corner[1]);
                    }
                    if (xm_worldmap_terrain_quarter_visibility[cell][2] != all) {
                        xm_worldmap_terrain_build_and_draw_quarter((u32 *)(data + 0x288), ot, packets + (xm_worldmap_terrain_packet_count << 5), &scratch->corner[2]);
                    }
                    if (xm_worldmap_terrain_quarter_visibility[cell][3] != all) {
                        xm_worldmap_terrain_build_and_draw_quarter((u32 *)(data + 0x3CC), ot, packets + (xm_worldmap_terrain_packet_count << 5), &scratch->corner[3]);
                    }
                }
            }
        }
        scratch->corner[0].vz -= 0x800;
    }
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): worldmap_terrain_build_and_draw_quarter (src/decomp/src/worldmap/worldmap_movement_terrain.c:2228). */
#ifndef XM_S_rcossin_tbl
#define XM_S_rcossin_tbl
extern s16 xm_rcossin_tbl[0x1000][2] __asm__("D_800523F0");
#endif
#ifndef XM_S_worldmap_terrain_draw_quarter_block
#define XM_S_worldmap_terrain_draw_quarter_block
void xm_worldmap_terrain_draw_quarter_block(u32 *cells, u_long *ot, s32 packets) __asm__("func_8009980C");
#endif
void func_80099708(u32 *heights, u_long *ot, s32 packets, SVECTOR *origin) {
    SVECTOR *vertex;
    u32 *cell;
    s32 j;
    s16 (*sine)[2];
    s32 row_phase;
    s32 left;
    s32 z;
    s32 x;
    s32 i;
    s32 phase;
    s32 swell;
    s32 height;
    s32 wave;

    vertex = (SVECTOR *)0x1F800000;
    cell = heights;
    j = 8;
    sine = xm_rcossin_tbl;
    row_phase = xm_worldmap_wave_phase_z;
    left = origin->vx;
    z = origin->vz;
    for (; j != -1; j--) {
        x = left;
        i = 8;
        swell = sine[row_phase & 0xFFF][0] * 2;
        phase = xm_worldmap_wave_phase_x;
        for (; i != -1; i--) {
            if (*cell & 0x1000) {
                height = (s32)(*cell << 24) >> 21;
                wave = (sine[phase & 0xFFF][0] * swell) >> 20;
                *(s32 *)&vertex->vx = ((wave + height) << 16) | (x & 0xFFFF);
            } else {
                *(s32 *)&vertex->vx = ((s32)(*cell << 24) >> 5) | (x & 0xFFFF);
            }
            vertex->vz = z;
            x += 0x80;
            vertex++;
            cell++;
            phase += 0x200;
        }
        z -= 0x80;
        row_phase += 0x200;
    }
    xm_worldmap_terrain_draw_quarter_block(heights, ot, packets);
}
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8009980C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80099BFC);
