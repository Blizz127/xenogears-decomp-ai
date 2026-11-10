#include "common.h"
#include "psyq/libgpu.h"
#include "system/menu.h"
#include "system/archive.h"
extern void* D_8005945C;
extern void* D_8006259C;
extern const char D_801C5000[16];
extern unsigned int ResolveArchiveEntryPointers(u32*);
extern void* LZSSHeapDecompress(void*,int);
extern void func_8002DD20(u32*);
extern void func_80026338(u8*,s32,u32*,s32*,s32*,s32*,s32*,s32*);
extern void* HeapAlloc(u_int,u_int);
extern u_int HeapFree(void*);
extern void SoundAddSedsEntry(SoundFile*);
extern int ArchiveDecodeAlignedSize(unsigned int);
extern s32 ArchiveReadFileToBuffer(s32,void*,u32,u32);
extern void ArchiveCdDataSync(int);

#include "common.h"

#include "psyq/libgpu.h"
#include "system/controller.h"
#include "system/debug.h"
#include "system/math.h"
#include "system/menu.h"
#include "main/game.h"

// Confirmation window choices
#define MENU_CHOICE_NO 0
#define MENU_CHOICE_YES 1

// Shop mode choices
#define MENU_CHOICE_EXIT 0x0
#define MENU_CHOICE_SELL 0x1
#define MENU_CHOICE_BUY 0x2

// Sell mode menu choice
#define MENU_CHOCIE_EQUIPMENT 0x0
#define MENU_CHOICE_ACCESSORIES 0x1
#define MENU_CHOICE_WEAPONS 0x2
#define MENU_CHOICE_ITEMS 0x3

#define MENU_STAT_CHANGE_INCREASE 0x0
#define MENU_STAT_CHANGE_DECREASE 0x1

#define SHOP_DATA_INITIALIZE 0x0
#define SHOP_DATA_FREE 0x10

// Selection modes
#define MENU_AUTO_ADVANCE 0
#define MENU_MANUAL_CHOICE 0xFF

// Max items to display in the window at a time
#define MAX_ITEMS_IN_VIEW 8

// Debug-related?
extern s32* D_8005917C;

extern int D_801D1F50; // Number of items
extern s32 D_801D1FD0[];
extern s32 D_801D1FD4[];
extern s32 D_801D1FE8[];
extern u16 D_801D2260;


extern s32 D_801D1F54[];
// = {
//    MENU_TEX_BALL_CURSOR_1, MENU_TEX_STRING_BUY,
//    MENU_TEX_BALL_CURSOR_2, MENU_TEX_STRING_SELL,
//    MENU_TEX_BALL_CURSOR_3, MENU_TEX_STRING_EXIT
//    }

extern s32 D_801D1FCC[];
extern s32 D_801D1FD8[];

extern u_short D_801D21F0[];

extern s32 D_801D1FCC[];

extern s32 D_801D2248;
extern s32 D_801D224C;
extern s32 D_801D2250;
extern s32 D_801D2254;
extern s32 D_801D2258;
extern s32 D_801D225C;

// Character portrait X positions
extern int D_801D21CC[];

// Cursor stuff
extern int D_801D201C[];
extern int D_801D2094[];
extern int D_801D2114[];

extern int D_801D1FF8[];
extern int D_801D2008[];

// Argument passed to the menu by the outside.
// For the shop menu, this is expected to be the index
// of the shop to load and run.
extern u8 D_80059171;

extern s32 D_801D1F50;


extern int D_801D2194[];
extern int D_801D21B0[];

// Explanations stuff
extern u_char D_801D2210[]; // Buy menu explanation textures
// = { MENU_TEX_QUANTITY_EXPLANATION, 
//     MENU_TEX_STRING_ATTACK, 
//     MENU_TEX_STRING_DEFENSE, 
//     MENU_TEX_MINUS 
//    }

extern u_char D_801D2214[]; // Sell menu explanation textures
// = { MENU_TEX_QUANTITY_EXPLANATION, MENU_TEX_PLUS }

extern int D_801D2218[]; // Buy menu explanation graphics X positions
extern int D_801D2228[]; // Sell menu explanation graphics X positions
extern int D_801D2230[]; // Buy menu explanation graphics Y positions
extern int D_801D2240[]; // Sell menu explanation graphics Y positions

u_char ShopMenuBuyMenu(void);
/* Declared here rather than relying on the definition in misc5.c: the split
 * puts this call in a different TU, and without the u8 parameter GCC passes
 * the argument unmasked (retail: `andi $a1,$s0,0xFF`). */
void func_801CCE1C(void* output, u8 characterId);



/* Part of the split shop-menu TU. Retail interleaves unmatched (INCLUDE_ASM)
 * regions with C runs; cc1 emits every file-scope __asm__ block before every
 * compiled body, so each part below is (one retail asm run) + (the C run that
 * follows it) - the layout the compiler produces inside a single TU. */
#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): item_shop_choice_window_set_cursor (src/decomp/src/ovl2601/item_shop_framework.c:2103). */
#ifndef XM_T_MenuMover
#define XM_T_MenuMover
typedef struct {
    s32 x0;
    s32 x1;
    s32 y0;
    s32 y1;
    s32 step_x; 
    s32 step_y;
    s32 acc_x;  
    s32 acc_y;
    u8 neg_x;   
    u8 neg_y;
    u8 speed;   
    u8 done;
} MenuMover;
#endif
#ifndef XM_T_MenuBuffer
#define XM_T_MenuBuffer
typedef struct {
    DRAWENV draw;
    DISPENV disp;
    u_long ot[16];
    u32 *ot_big; 
} MenuBuffer;
#endif
#ifndef XM_T_MenuLight
#define XM_T_MenuLight
typedef struct {
    SVECTOR direction;   
    s16 unknown8[5];     
    u8 unknown12[0xE];
    MATRIX colour;       
} MenuLight;
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
#ifndef XM_T_MenuCardFile
#define XM_T_MenuCardFile
typedef struct MenuCardFile {
    s32 frames[6]; 
    char name[21]; 
    u8 pad2d[0x2B];
    u8 state;      
    u8 pad59[3];
} MenuCardFile;
#endif
#ifndef XM_T_MenuCardHeader
#define XM_T_MenuCardHeader
typedef struct MenuCardHeader {
    POLY_FT4 label[2];
    POLY_FT4 name[4]; 
} MenuCardHeader;
#endif
#ifndef XM_T_MenuCard
#define XM_T_MenuCard
typedef struct MenuCard {
    MenuCardFile files[32];       
    TIM_IMAGE icon;               
    u8 heads[32][0x200];          
    u8 save_magic[2];             
    u8 save_icon_flag;            
    u8 save_blocks;               
    char save_title[0x5C];        
    u8 save_palette[0x20];        
    u8 save_icon[0x80];           
    u8 pad4c94[0x100];
    MenuCardHeader card_headers[2]; 
    s32 result[2];                
    s32 cursor;                   
    s32 unknown4f80;
    s32 file_count;               
    u8 scanned[2];                
    u8 unknown4f8a[2];
    u8 unknown4f8c[2];
    u8 ours[32];                  
    u8 file_slots[32];            
    char prefix[13];              
    u8 pad4fdb[9];
    u8 present[2];                
    u8 mode;                      
    u8 busy;                      
    u8 present_shown[2];          
    u8 pad4fea[2];
    s32 events[4];                
    char title[30];               
    u8 unknown501a;
    u8 unknown501b;
    u32 other_prefix[4];          
    u8 pad502c[8];
} MenuCard;
#endif
#ifndef XM_T_EquipInfo
#define XM_T_EquipInfo
typedef struct EquipInfo {
    u16 users;   
    u16 unk2;
    u16 price;   
    u8 kind;     
    u8 unk7;
    u16 value;   
    u8 a;        
    u8 b;        
    u8 power;    

    u8 unkD[3];
} EquipInfo;
#endif
#ifndef XM_T_AccessoryInfo
#define XM_T_AccessoryInfo
typedef struct AccessoryInfo {
    u16 users;   
    u16 price;   
    u8 unk4[4];
    u8 amount;   
    u8 kind;     
    s16 value;   
    s16 stats;   
    u16 groups;  
} AccessoryInfo;
#endif
#ifndef XM_T_GearEngineInfo
#define XM_T_GearEngineInfo
typedef struct GearEngineInfo {
    u32 users;   
    u32 unk4;
    u16 unk8;
    u16 price;   
    u8 unkC[8];
    u8 unk14;
    u8 unk15;
    u8 unk16;
    u8 unk17;
} GearEngineInfo;
#endif
#ifndef XM_T_GearPartInfo
#define XM_T_GearPartInfo
typedef struct GearPartInfo {
    u32 users;
    u8 unk4[2];
    u16 unk6;
    u8 unk8[2];
    u16 price;   
    u8 unkC;
    u8 unkD;
    u8 unkE;
    u8 unkF;
} GearPartInfo;
#endif
#ifndef XM_T_GearFrameInfo
#define XM_T_GearFrameInfo
typedef struct GearFrameInfo {
    u32 users;
    u16 price;   
    u8 unk6[2];
    u16 unk8;
    u16 unkA;
    u8 unkC[8];
} GearFrameInfo;
#endif
#ifndef XM_T_GearAccessoryInfo
#define XM_T_GearAccessoryInfo
typedef struct GearAccessoryInfo {
    u32 users;
    u16 price;   
    u16 unk6;    
    u16 groups;  
    u8 unkA[3];
    u8 unkD;     
    u8 unkE;     
    u8 unkF;
    u8 unk10[4]; 
    u8 unk14;    
    u8 kind;     
    u16 value;   
    u8 unk18;    
    u8 unk19;
    u8 unk1A;    
    u8 unk1B;    
} GearAccessoryInfo;
#endif
#ifndef XM_T_GearWeaponInfo
#define XM_T_GearWeaponInfo
typedef struct GearWeaponInfo {
    u8 attrs[4]; 
    u32 users;   
    u16 price;   
    u8 unkA[4];
    u8 unkE;
    u8 kind;     
    u8 unk10;
    u8 unk11;
    u16 unk12;
} GearWeaponInfo;
#endif
#ifndef XM_T_ItemInfo
#define XM_T_ItemInfo
typedef struct ItemInfo {
    u16 unk0;
    u16 price;   
    u16 target;  
    u8 use;      

    u8 unk7;
    u8 amount;   
    u8 unk9;
    s16 flags;   
    s16 stats;   
    u8 unkE[2];
} ItemInfo;
#endif
#ifndef XM_T_ArtInfo
#define XM_T_ArtInfo
typedef struct ArtInfo {
    u16 target; 
    u8 pad2[0xF];
    u8 unk11; 
    u8 pad12[0x1];
    u8 cost; 
    u8 pad14[0x3];
    u8 unk17; 
    u8 pad18[0xC];
    u16 gearCost; 
    u8 pad26[0x2];
} ArtInfo;
#endif
#ifndef XM_T_GearSummary
#define XM_T_GearSummary
typedef struct GearSummary {
    u32 hp;
    u32 max_hp;        
    u16 defense;       
    u16 ether_defense; 
    u16 value68;       
    u16 value6a;       
    u16 fuel;          
    u16 max_fuel;      
    u16 attack;        
    u8 hit;            
    u8 speed;          
    u8 frame_factor;   
    u8 value9d;        
    u8 guard;          
    u8 pad1b;
} GearSummary;
#endif
#ifndef XM_T_MenuTables
#define XM_T_MenuTables
typedef struct MenuTables {
    EquipInfo *equipment;                
    AccessoryInfo *accessories;          
    GearEngineInfo *engines;             
    GearPartInfo *parts;                 
    GearFrameInfo *frames;               
    GearAccessoryInfo *gear_accessories; 
    GearWeaponInfo *gear_weapons;        
    ItemInfo *items;                     
    struct ArtInfo *arts[31];            
    GearSummary gear;                    
    u16 stats[9];                        


    u8 padca[2];
} MenuTables;
#endif
#ifndef XM_T_MenuFlags
#define XM_T_MenuFlags
typedef struct MenuFlags {
    u8 field_blocks_shown[3]; 
    u8 cursor_shown;          
    u8 sprite_shown;          
    u8 field_menu_shown;      
    u8 field_menu2_shown;     
    u8 detail_shown;          
    u8 equipment_shown;       
    u8 images_shown;          
    u8 lists_shown;           
    u8 file_info_shown;       
    u8 list_labels_shown[8];  
    u8 row_labels_shown[6];   
    u8 extra_labels_shown[6]; 
    u8 panels_shown[7];       
    u8 panels_growing[7];     
    u8 messages_shown;        
    u8 markers_shown;         
    u8 party[3];              
    u8 card_message_shown;    
    u8 labels_shown[4];       
    u8 labels10e0_shown[8];   
    u8 labels14e0_shown[6];   
    u8 party_panels_shown;    
    u8 name_entry_shown;      
    u8 item_list_shown;       
    u8 scroll_shown;          
    u8 arts_list_shown;       
    u8 equip_labels_shown;    
    u8 equip_list_shown;      
    u8 status_list_shown;     
    u8 label17e0_shown;       
    u8 unknown4f;
    u8 cursors_shown[3];      

    u8 marks_shown;           
    u8 labels18e0_shown[6];   
    u8 unknown5a[2];
    u8 sound_labels_shown[4]; 
    u8 ready[3];              
    u8 model_shown;           
    u8 price_shown;           
    u8 gear_shown;            
    u8 gear_parts_shown;      
    u8 unknown67;
    u8 card_mode;             
    u8 pad69[3];
} MenuFlags;
#endif
#ifndef XM_T_MenuFieldMenu
#define XM_T_MenuFieldMenu
typedef struct MenuFieldMenu {
    POLY_FT4 polys[18]; 
    POLY_FT4 cursor[2]; 
    s32 count; 
    u8 start; 
    u8 pad325[0x3];
} MenuFieldMenu;
#endif
#ifndef XM_T_MenuFieldMenu2
#define XM_T_MenuFieldMenu2
typedef struct MenuFieldMenu2 {
    POLY_FT4 polys[14]; 
    POLY_FT4 polys2[8]; 
    u8 start; 
    u8 pad371[0x3];
} MenuFieldMenu2;
#endif
#ifndef XM_T_MenuPrims
#define XM_T_MenuPrims
typedef struct MenuPrims {
    POLY_FT4 sprite[2];     
    POLY_G4 shade[2];       
    POLY_F4 fade[2];        
    LINE_F3 upper[2];       
    LINE_F3 lower[2];       
    DR_MODE mode_label[2];  
    DR_MODE mode_sprite[2]; 
    u8 sprite_buffer;       
    u8 shade_buffer;        
    u8 pad15a;
    u8 width;               
} MenuPrims;
#endif
#ifndef XM_T_MenuView
#define XM_T_MenuView
typedef struct MenuView {
    POLY_FT4 image[2]; 
    POLY_FT4 frame[9][2]; 
    POLY_FT4 levelDigits[6][2]; 
    POLY_FT4 hpDigits[3][2]; 
    POLY_FT4 hpMaxDigits[3][2]; 
    POLY_FT4 epDigits[2][2]; 
    POLY_FT4 epMaxDigits[2][2]; 
    POLY_FT4 name[2]; 
    u8 levelCount; 
    u8 level2Count; 
    u8 hpCount; 
    u8 hpMaxCount; 
    u8 epCount; 
    u8 epMaxCount; 
    u8 frameBuffer; 
    u8 buffer; 
    u8 shown; 
    u8 nameBuffer; 
    u8 frameCount; 
    u8 pad87B[0x1];
} MenuView;
#endif
#ifndef XM_T_MenuFileInfo
#define XM_T_MenuFileInfo
typedef struct MenuFileInfo {
    POLY_FT4 chars[64]; 
    POLY_FT4 cursor[2]; 
    POLY_G4 band[2]; 
    MenuView views[3]; 
    POLY_FT4 colon0[4]; 
    POLY_FT4 colon1[4]; 
    POLY_FT4 timeDigits[7][2]; 
    POLY_FT4 title[32]; 
    POLY_FT4 discLabel[2]; 
    POLY_FT4 discMark[2]; 
    POLY_FT4 discDigits[2][2]; 
    u8 rebuilt; 
    u8 pad2DBD[0x3];
} MenuFileInfo;
#endif
#ifndef XM_T_MenuImages
#define XM_T_MenuImages
typedef struct MenuImages {
    POLY_FT4 packets[56];  
    POLY_FT4 packets2[56]; 
    RECT screen;           
    s32 count;             
    s32 count2;            
    u8 buffer;             
    u8 buffer2;            
    u8 dim;                
    u8 dimmed;             
} MenuImages;
#endif
#ifndef XM_T_MenuSpriteLists
#define XM_T_MenuSpriteLists
typedef struct MenuSpriteLists {
    POLY_FT4 first[32];  
    POLY_FT4 second[96]; 
    s32 first_count;     
    s32 second_count;    
    u8 first_buffer;     
    u8 second_buffer;    
    u8 pad140a[2];
} MenuSpriteLists;
#endif
#ifndef XM_T_MenuDetail
#define XM_T_MenuDetail
typedef struct MenuDetail {
    POLY_FT4 portrait[2];        
    POLY_FT4 name[2];            
    POLY_FT4 parts[66];          
    POLY_FT4 level[6];           
    POLY_FT4 level2[6];          
    POLY_FT4 value3C[16];        
    POLY_FT4 value40[16];        
    POLY_FT4 exp[14];            
    POLY_FT4 expNext[14];        
    POLY_FT4 hp[10];             
    POLY_FT4 hpMax[10];          
    POLY_FT4 ep[10];             
    POLY_FT4 epMax[10];          
    POLY_FT4 list1C70[10];       
    POLY_FT4 tabs[4];            
    SVECTOR portraitAt[4];       
    SVECTOR nameAt[4];           
    SVECTOR partsAt[33][4];      
    SVECTOR levelAt[3][4];       
    SVECTOR level2At[3][4];      
    SVECTOR value3CAt[8][4];     
    SVECTOR value40At[8][4];     
    SVECTOR expAt[7][4];         
    SVECTOR expNextAt[7][4];     
    SVECTOR hpAt[5][4];          
    SVECTOR hpMaxAt[5][4];       
    SVECTOR epAt[5][4];          
    SVECTOR epMaxAt[5][4];       
    SVECTOR list1C70At[5][4];    
    SVECTOR tabsAt[2][4];        
    u8 buffer;                   
    u8 levelCount;               
    u8 level2Count;              
    u8 value3CCount;             
    u8 value40Count;             
    u8 expCount;                 
    u8 expNextCount;             
    u8 hpCount;                  
    u8 hpMaxCount;               
    u8 epCount;                  
    u8 epMaxCount;               
    u8 list1C70Count;            
    u8 count;                    
    u8 tabCount;                 
    u8 tabBuffer;                
    u8 pad2AEF[0x1];
} MenuDetail;
#endif
#ifndef XM_T_MenuEquipPanel
#define XM_T_MenuEquipPanel
typedef struct MenuEquipPanel {
    POLY_FT4 polys[94];            
    POLY_FT4 rowA[7][8];           
    POLY_FT4 rowB[7][8];           
    POLY_G4 bars[7][2];            
    POLY_G4 highlights[7][2];      
    SVECTOR verts[188];            
    SVECTOR rowAAt[7][16];         
    SVECTOR rowBAt[7][16];         
    SVECTOR barAt[7][4];           
    SVECTOR highlightAt[7][4];     
    u8 rowACount[7];               
    u8 rowABuffer[7];              
    u8 rowBCount[7];               
    u8 rowBBuffer[7];              
    u8 barBuffer[7];               
    u8 highlightBuffer[7];         
    u8 rowShown[7];                
    u8 buffer;                     
    u8 highlighted;                
    u8 kind;                       
} MenuEquipPanel;
#endif
#ifndef XM_T_MenuLabel
#define XM_T_MenuLabel
typedef struct {
    POLY_FT4 polys[2]; 
    SVECTOR verts[4];  
    RECT rect;         
    u8 *pixels;        
    u8 highlight;      
    u8 buffer;         
    u8 width;          
    u8 projected;      
} MenuLabel;
#endif
#ifndef XM_T_MenuEquipLabels
#define XM_T_MenuEquipLabels
typedef struct MenuEquipLabels {
    MenuLabel labels[5]; 
    u16 stats[9]; 
    u8 pad292[0x2];
    u8 visible[5]; 
    u8 count; 
    u8 pad29A[0x2];
    u8 parts[3][5]; 
} MenuEquipLabels;
#endif
#ifndef XM_T_MenuPanel
#define XM_T_MenuPanel
typedef struct MenuPanel {
    POLY_FT4 corner[8];       
    POLY_FT4 edge[4][4];      
    POLY_FT4 bar_side[2];     
    POLY_FT4 bar_ends[4];     
    POLY_G4 fill[2];          
    DR_MODE fill_mode[2];     
    SVECTOR corner_at[16];    
    SVECTOR edge_at[4][2][4]; 
    SVECTOR fill_at[4];       
    SVECTOR side_at[4];       
    SVECTOR ends_at[8];       
    s32 corner_parts;         
    s32 flat;                 

    s32 ot_entry;             
    u8 buffer;                
    u8 has_bar;               
    u8 pad71e[2];
} MenuPanel;
#endif
#ifndef XM_T_MenuGrowth
#define XM_T_MenuGrowth
typedef struct MenuGrowth {
    u16 x, y, w, h;   
    u16 cur_w, cur_h; 
    s32 ot_entry;     
    u8 index;         
    u8 done;          
    u8 flat;          
    u8 has_bar;       
    u8 pad14[4];
} MenuGrowth;
#endif
#ifndef XM_T_MenuFieldBlock
#define XM_T_MenuFieldBlock
typedef struct MenuFieldBlock {
    POLY_FT4 portrait[2]; 
    POLY_FT4 name[2]; 
    POLY_FT4 list0[54]; 
    POLY_FT4 list6[6]; 
    POLY_FT4 list8[6]; 
    POLY_FT4 list1[6]; 
    POLY_FT4 list2[6]; 
    POLY_FT4 list3[4]; 
    POLY_FT4 list4[4]; 
    POLY_FT4 list5[14]; 
    POLY_FT4 list7[14]; 
    u8 buffer; 
    u8 count6; 
    u8 count8; 
    u8 count1; 
    u8 count2; 
    u8 count3; 
    u8 count4; 
    u8 count5; 
    u8 count7; 
    u8 count0; 
    u8 pad127A[0x2];
} MenuFieldBlock;
#endif
#ifndef XM_T_MenuSlotImage
#define XM_T_MenuSlotImage
typedef struct MenuSlotImage {
    POLY_FT4 icon[2];  
    LINE_F3 lineA[2];  
    LINE_F3 lineB[2];  
    POLY_F4 box[2];    
    SVECTOR iconAt[4]; 
    SVECTOR lineAAt[4]; 
    SVECTOR lineBAt[4]; 
    DR_MODE boxMode[2]; 
} MenuSlotImage;
#endif
#ifndef XM_T_MenuMarkers
#define XM_T_MenuMarkers
typedef struct MenuMarkers {
    POLY_FT4 polys[8];
    u8 shown[4];     
    u8 at_cursor[4]; 
    u8 buffer[4];    
} MenuMarkers;
#endif
#ifndef XM_T_MenuItemList
#define XM_T_MenuItemList
typedef struct MenuItemList {
    MenuLabel names[16]; 
    MenuLabel values[16]; 
    MenuLabel extra[3]; 
    u8 *unk1180; 
    u8 shown[16]; 
    u8 extraShown; 
    u8 pad1195[0x3];
} MenuItemList;
#endif
#ifndef XM_T_MenuArtsList
#define XM_T_MenuArtsList
typedef struct MenuArtsList {
    MenuLabel names[14]; 
    MenuLabel values[14]; 
    MenuLabel headA; 
    MenuLabel headB; 
    MenuLabel extra[2]; 
    MenuLabel footer; 
    u8 *texts; 
    u8 shown[14]; 
    u8 extraShown; 
    u8 pad1093[0x1];
} MenuArtsList;
#endif
#ifndef XM_T_MenuEquipList
#define XM_T_MenuEquipList
typedef struct MenuEquipList {
    MenuLabel names[8]; 
    MenuLabel values[8]; 
    MenuLabel title; 
    MenuLabel extra[3]; 
    u8 *texts[4]; 
    u8 shown[8]; 
    u8 extraShown; 
    u8 padA19[0x3];
} MenuEquipList;
#endif
#ifndef XM_T_MenuStatusList
#define XM_T_MenuStatusList
typedef struct MenuStatusList {
    MenuLabel names[13]; 
    MenuLabel values[13]; 
    MenuLabel title; 
    POLY_FT4 lists[13][10]; 
    POLY_G4 gauges[13][2]; 
    u8 *unk2578; 
    u8 counts[13]; 
    u8 starts[13]; 
    u8 shown[13]; 
    u8 gaugeShown[13]; 
    u8 gaugeBuffer[13]; 
} MenuStatusList;
#endif
#ifndef XM_T_MenuScrollBar
#define XM_T_MenuScrollBar
typedef struct MenuScrollBar {
    POLY_FT4 polys[2];
    SVECTOR verts[4]; 
    u8 buffer;        
    u8 pad71[3];
} MenuScrollBar;
#endif
#ifndef XM_T_MenuMarkerQuads
#define XM_T_MenuMarkerQuads
typedef struct MenuMarkerQuads {
    POLY_FT4 polys[8];
    SVECTOR verts[16]; 
    u8 buffer;         
    u8 pad1c1[3];
} MenuMarkerQuads;
#endif
#ifndef XM_T_MenuCursor
#define XM_T_MenuCursor
typedef struct MenuCursor {
    POLY_FT4 polys[2];
    SVECTOR verts[4]; 
    s32 frame;        
    u8 timer;         
    u8 buffer;        
    u8 pad76[2];
} MenuCursor;
#endif
#ifndef XM_T_MenuIndicator
#define XM_T_MenuIndicator
typedef struct MenuIndicator {
    POLY_F4 fills[2]; 
    POLY_FT4 spriteA[24]; 
    POLY_FT4 spriteB[24]; 
    s32 unk7B0; 
    s32 unk7B4; 
    u8 buffer; 
    u8 pad7B9[0x3];
} MenuIndicator;
#endif
#ifndef XM_T_ShopDetails
#define XM_T_ShopDetails
typedef struct ShopDetails {
    POLY_FT4 members[18];     
    POLY_FT4 group2D0[18];    
    POLY_FT4 heading[44];     
    POLY_FT4 digits1[18];     
    POLY_FT4 digits2[18];     
    POLY_FT4 group1220[18];   
    POLY_FT4 digits4[18];     
    POLY_FT4 price[20];       
    POLY_FT4 digits3[18];     
    POLY_FT4 rows[8][8];      
    POLY_FT4 cells_a[9][6];   
    POLY_FT4 cells_b[9][6];   
    LINE_F3 bar_upper[18];    
    LINE_F3 bar_lower[18];    
    LINE_F2 frame[2];         
    u8 unk3C10[0x20];
    MenuLabel names_a[8];     
    MenuLabel names_b[8];     
    MenuLabel label4430;      
    MenuLabel label44B0;      
    MenuLabel label4530;      
    MenuLabel label45B0;      
    void *resources[9];       
    u8 amounts[0x30];         
    u8 name_shown[8];         
    u8 row_count[8];          
    u8 row_buffer[8];         
    u8 bar_shown[9];          
    u8 members_count;         
    u8 members_buffer;        
    u8 label4430_shown;       
    u8 group2D0_buffer;       
    u8 group2D0_count;        
    u8 heading_buffer;        
    u8 heading_count;         
    u8 digits1_buffer;        
    u8 digits1_count;         
    u8 digits2_buffer;        
    u8 digits2_count;         
    u8 digits3_buffer;        
    u8 digits3_count;         
    u8 digits_shown;          
    u8 group1220_buffer;      
    u8 group1220_count;       
    u8 label44B0_shown;       
    u8 label4530_shown;       
    u8 digits4_buffer;        
    u8 digits4_count;         
    u8 digits4_shown;         
    u8 price_buffer;          
    u8 price_count;           
    u8 cells_a_count[9];      
    u8 cells_b_count[9];      
    u8 cells_a_buffer[9];     
    u8 cells_b_buffer[9];     
    u16 attack[16];           

    u16 defense[16];          
    u8 stock[5][20];          
    u8 unk4784;
    u8 label45B0_shown;       
    u8 unk4786[2];
} ShopDetails;
#endif
#ifndef XM_T_GearScreen
#define XM_T_GearScreen
typedef struct GearScreen {
    POLY_FT4 backdrop[2];      
    u8 unk50[0x30];
    POLY_FT4 packets[2];       
    POLY_FT4 flicker[3][4];    
    POLY_FT4 lamps[66];        
    POLY_FT4 indicator[36];    
    POLY_FT4 frame[28];        
    POLY_FT4 parts[5][10];     
    u8 flicker_count;          
    u8 lamp_count[3];          
    u8 flicker_buffer;         
    u8 lamp_buffer[3];         
    u8 flicker_shown;          
    u8 lamp_state[3];          
    u8 flicker_timer;          
    u8 lamp_timer[3];          
    u8 flicker_frame;          
    u8 buffer;                 
    s16 indicator_frame;       
    s16 lamp_frame[2];         
    u8 part_count[5];          
    u8 parts_buffer;           
    u8 unk1EEE[2];
    u16 flicker_x, flicker_y;  
    u16 lamp_x[2];             
    u16 lamp_y[2];             
    u16 indicator_x, indicator_y; 
} GearScreen;
#endif
#ifndef XM_T_ModelParts
#define XM_T_ModelParts
typedef struct ModelParts {
    void *data0; 
    void *data1; 
    s16 position[3]; 
    u8 unkE[4];
    u8 unk12;    
} ModelParts;
#endif
#ifndef XM_T_MenuSheetEntry
#define XM_T_MenuSheetEntry
typedef struct {
    s32 first;
    s32 mode;   
    s32 clut_x; 
    s32 clut_y;
    s32 page_x; 
    s32 page_y;
} MenuSheetEntry;
#endif
#ifndef XM_T_MenuStatusPanel
#define XM_T_MenuStatusPanel
typedef struct MenuStatusPanel {
    POLY_FT4 layout[18];  
    POLY_FT4 extra[10];   
    POLY_FT4 face[2];     
    POLY_FT4 label[2];    
    POLY_FT4 level[6];    
    POLY_FT4 level2[6];   
    POLY_FT4 hp[10];      
    POLY_FT4 hp_max[10];  
    POLY_FT4 ep[6];       
    POLY_FT4 ep_max[6];   
    u8 level_count;       
    u8 level2_count;      
    u8 hp_count;          
    u8 hp_max_count;      
    u8 ep_count;          
    u8 ep_max_count;      
    u8 buffer;            
    u8 shown;             
    u8 layout_count;      
    u8 extra_count;       
    u8 padbea[2];
} MenuStatusPanel;
#endif
#ifndef XM_T_NameEntry
#define XM_T_NameEntry
typedef struct NameEntry {
    POLY_FT4 confirm[2];     
    POLY_FT4 back[2];        
    POLY_FT4 frame[4];       
    POLY_FT4 cursor[2];      
    POLY_FT4 chars[72];      
    POLY_FT4 name[2];        
    LINE_F3 line_a[2];       
    LINE_F3 line_b[2];       
    LINE_F2 caret[2];        
    SVECTOR cursor_at[4];    
    SVECTOR name_at[4];      
    u8 parts_buffer;         
    u8 cursor_buffer;        
    u8 chars_buffer;         
    u8 name_buffer;          
    u8 lines_buffer;         
    u8 shown;                
    u8 grid_shown;           
    u8 blink;                
    u8 length;               
    u8 max_length;           
    u8 frame_count;          
    u8 pad_DEB;
} NameEntry;
#endif
#ifndef XM_T_MenuState
#define XM_T_MenuState
typedef struct MenuState {
    MenuMover movers[3];        
    MenuBuffer buffers[2];      
    MenuBuffer *current;        
    SVECTOR angles;             
    VECTOR offset;              
    MATRIX matrix;              
    u8 unknown210[8];
    SVECTOR angles2;            
    VECTOR offset2;             
    MATRIX matrix2;             
    u8 unknown250[0x48];
    MenuLight light;            
    s32 frame_counter;          
    void *sheet;                
    void *label_text;           
    struct SoundBank *effects;  
    s32 word2e8;                
    s32 time[7];                
    s32 buffer_index;           
    u8 present[16];             
    u8 digits[9];               
    u8 input;                   
    u8 card_poll_timer;         
    u8 drawing;                 
    u8 unknown328;
    u8 view_motion;             
    u8 sounds;                  
    u8 party_count;             
    struct MenuCard *card;      
    struct MenuTables *tables;  
    u8 cards_present;           
    u8 unknown335;
    u8 cursor;                  
    u8 cursor_shown;            
    u8 choice;                  
    u8 choice_shown;            
    u8 choice_count;            
    u8 fighters;                
    struct MenuFlags *flags;    
    struct MenuFieldMenu *field_menu;   
    struct MenuFieldMenu2 *field_menu2; 
    struct MenuPrims *prims;    
    struct MenuFileInfo *file_info; 
    struct MenuImages *images;  
    struct MenuSpriteLists *lists; 
    struct MenuDetail *detail;  
    struct MenuEquipPanel *equip_panel; 
    struct MenuEquipLabels *equip_labels; 
    struct MenuPanel *panels[7];   
    struct MenuGrowth *growth[7];  
    struct MenuFieldBlock *field_blocks[3]; 
    struct MenuSlotImage *slots[32]; 
    struct MenuMarkers *markers;   
    struct MenuItemList *item_list; 
    struct MenuArtsList *arts_list; 
    struct MenuEquipList *equip_list; 
    struct MenuStatusList *status_list; 
    struct MenuScrollBar *scroll;  
    struct MenuMarkerQuads *marks; 
    struct MenuCursor *cursors[2]; 
    struct MenuIndicator *indicator; 
    struct ShopDetails *details;   
    struct GearScreen *gear_screen; 
    struct ModelParts *model_parts[2]; 
    u8 unknown460[0xC];
    MenuSheetEntry sheet_entries[4]; 
    s32 unknown4cc;
    s32 unknown4d0;
    s32 unknown4d4;
    u8 load_state;              
    u8 unknown4d9;
    u8 unknown4da[2];
    u8 first_member;            
    u8 unknown4dd[3];
    MenuLabel labels[4];        
    MenuLabel list_labels[8];   
    MenuLabel row_labels[6];    
    MenuLabel extra_labels[6];  
    MenuLabel labels10e0[8];    
    MenuLabel labels14e0[6];    
    MenuLabel labels17e0[2];    
    MenuLabel labels18e0[6];    
    MenuLabel sound_labels[4];  
    MenuLabel *message_labels[4]; 
    struct MenuStatusPanel *member_panels[6]; 
    struct MenuStatusPanel *party_panels[3];  
    u8 members[11];             
    u8 unknown1e1f;
    struct NameEntry *name_entry; 
    u8 *portrait_table;         
    u8 portraits[3];            
    u8 unknown1e2b;
    u8 *shop_tables;            
    u8 shop_items[0x30];        
    u8 shop_kinds[0x30];        
    u8 *gear_tables;            
    u8 debug_show;              
    u8 debug_value;             
} MenuState;
#endif
#ifndef XM_S_item_shop_choice_window_set_cursor
#define XM_S_item_shop_choice_window_set_cursor
void xm_item_shop_choice_window_set_cursor(u8 menu) __asm__("func_801CC720");
#endif
#ifndef XM_S_menu_state_current
#define XM_S_menu_state_current
extern MenuState *xm_menu_state_current __asm__("g_Menu");
#endif
#ifndef XM_S_item_shop_choice_window_images
#define XM_S_item_shop_choice_window_images
extern s32 xm_item_shop_choice_window_images[24] __asm__("D_801D1F6C");
#endif
#ifndef XM_S_sprite_sheet_draw_scaled
#define XM_S_sprite_sheet_draw_scaled
s32 xm_sprite_sheet_draw_scaled(void *sheet, s32 id, POLY_FT4 *prims, s32 index, s32 x, s32 y, s32 scale) __asm__("func_8002675C");
#endif
#ifndef XM_S_item_shop_highlight_place
#define XM_S_item_shop_highlight_place
void xm_item_shop_highlight_place(s32 position, u8 frame) __asm__("ShopMenuMovePointerCursor");
#endif
void func_801CC720(u8 menu) {
    s32 id;
    s32 i;

    xm_menu_state_current->lists->first_count = 0;
    xm_menu_state_current->lists->second_count = 0;
    for (i = 0; i < xm_menu_state_current->choice_count; i++) {
        if (i == xm_menu_state_current->choice) {
            id = xm_item_shop_choice_window_images[(menu + xm_menu_state_current->cursor) * 8 + i * 2] + 0xD;
        } else {
            id = xm_item_shop_choice_window_images[(menu + xm_menu_state_current->cursor) * 8 + i * 2];
        }
        xm_menu_state_current->lists->first_count +=
            xm_sprite_sheet_draw_scaled(xm_menu_state_current->sheet, id,
                          xm_menu_state_current->lists->first + xm_menu_state_current->lists->first_count * 2, xm_menu_state_current->buffer_index,
                          0xA0, 0x96, 0x1000);
        xm_menu_state_current->lists->second_count += xm_sprite_sheet_draw_scaled(
            xm_menu_state_current->sheet, xm_item_shop_choice_window_images[(menu + xm_menu_state_current->cursor) * 8 + i * 2 + 1],
            xm_menu_state_current->lists->second + xm_menu_state_current->lists->second_count * 2, xm_menu_state_current->buffer_index, 0xA0, 0x96,
            0x1000);
    }
    xm_menu_state_current->lists->first_buffer = xm_menu_state_current->buffer_index;
    xm_menu_state_current->lists->second_buffer = xm_menu_state_current->buffer_index;
    xm_item_shop_highlight_place(xm_menu_state_current->choice + 3, 1);
    xm_menu_state_current->flags->sprite_shown = 1;
}
#endif
#endif

u_char ShopMenuShopModeMenuHandleSelectedOption() {
    u_char isRunning;
    u_char result;

    isRunning = TRUE;
    switch (g_Menu->menu1Choice) {
        case MENU_CHOICE_EXIT:
            isRunning = FALSE;
            break;

        case MENU_CHOICE_SELL:
            result = ShopMenuSellModeMenu();
            break;

        case MENU_CHOICE_BUY:
            result = ShopMenuBuyMenu();
            break;
    }
    
    if (result) {
        ShopMenuStartCloseMenuTransition();
        func_801CBC88(1, 4, g_Menu->unk6E0, &D_801D1FCC, g_Menu->pManager->unkC);
    }
    
    func_801D1F10();

    // Render the selection menu as active again
    g_Menu->pSelectionMenu->unk1192 = FALSE;
    g_Menu->pSelectionMenu->unk1193 = TRUE;

    g_Menu->pManager->unk4 = TRUE;
    g_Menu->pManager->unk3 = TRUE;
    g_Menu->unk337 = 0xFF;
    g_Menu->pManager->unkA = FALSE;
    
    return isRunning;
}

// Buy / Sell / Exit menu
void ShopMenuShopModeMenuMain(void) {
    u_char isRunning;

    isRunning = TRUE;
    g_Menu->menu1Choice = 2;

    ShopMenuInitializeShopModeSelectionMenu(4, &D_801D1F54);

    func_801CBC88(1, 4, g_Menu->unk6E0, &D_801D1FCC, g_Menu->pManager->unkC);
    
    while (isRunning) {
        ShopMenuUpdateAndRender();

        switch (g_Menu->input) {
            case MENU_INPUT_CONFIRM:
                ShopMenuPlaySoundEffect(2);

                // Render the selection menu as disabled
                g_Menu->pSelectionMenu->unk1192 = TRUE;

                func_801C6430();
                func_801CBC88(0, 4, g_Menu->unk6E0, &D_801D1FCC, g_Menu->pManager->unkC);
                g_Menu->unk348->unk15B = 0x4C;
                isRunning = ShopMenuShopModeMenuHandleSelectedOption();
                g_Menu->unk348->unk15B = 0x40;
                break;

            case MENU_INPUT_BACK:
                isRunning = FALSE;
                break;

            case MENU_INPUT_DOWN:
                if (g_Menu->menu1Choice) {
                    g_Menu->menu1Choice--;
                } else {
                    g_Menu->menu1Choice = 2;
                }
                break;

            case MENU_INPUT_UP:
                if (++g_Menu->menu1Choice >= 3) {
                    g_Menu->menu1Choice = 0;
                }
                break;
        }

        // Update the selected option to the active one if we need to
        if (g_Menu->menu1Choice != g_Menu->unk337) {
            ShopMenuUpdateShopModeSelectionMenu(3, g_Menu->menu1Choice, &D_801D1F54);
            func_801CBCF0(4, g_Menu->unk6E0, &D_801D1FCC, &D_801D1FD8, 
                g_Menu->pManager->unkC, g_Menu->menu1Choice, 0, 0
            );
            g_Menu->unk337 = g_Menu->menu1Choice;
        }
    }
}

void ShopMenuMain(void) {
    ShopMenuUnk32CManager(MENU_DATA_INITIALIZE);
    ShopMenuSetManager(MENU_DATA_INITIALIZE);
    ShopMenuSelectionMenuManager(MENU_DATA_INITIALIZE);
    ShopMenuUnk354Manager(MENU_DATA_INITIALIZE);
    ShopMenuUnk330Manager(MENU_DATA_INITIALIZE);
    ShopMenuUnk348Manager(MENU_DATA_INITIALIZE);
    ShopMenuUnk1E20Manager(MENU_DATA_INITIALIZE);
    ShopMenuShopManager(MENU_DATA_INITIALIZE);
    g_Menu->pSelectionMenu->unk1180.x = 0x2C0;
    g_Menu->pSelectionMenu->unk1180.y = 0x100;
    g_Menu->pSelectionMenu->unk1180.w = 0x140;
    g_Menu->pSelectionMenu->unk1180.h = 0xE0;
    g_Menu->unk348->unk15B = 0x40;
    ShopMenuInitialize();
    ShopMenuResetRenderContext();
    func_801C5EE8();
    ShopMenuInitializeBackgrounds();
    ShopMenuInitializeWindowBorders();
    ShopMenuInitializeShopData();
    g_Menu->shouldDrawMenu = TRUE;
    g_Menu->unk32A = TRUE;
    ShopMenuShopModeMenuMain();
    ShopMenuFree();
}

void func_801CCE1C(void* output, u8 characterId) {
    u8* out = output;
    u8* stats = (u8*)&g_GameState + 0x26C + characterId * 0xA4;

    if (stats[0x56] == 4) {
        *(u16*)(out + 0xB8) = stats[0x04] + stats[0x1C];
    } else {
        *(u16*)(out + 0xB8) = stats[0x58] + stats[0x28] + stats[0x04];
    }

    *(u16*)(out + 0xB8) = stats[0x58] + stats[0x28] + stats[0x04];
    *(u16*)(out + 0xBA) = stats[0x5E] + stats[0x2E];
    *(u16*)(out + 0xBC) = stats[0x59] + stats[0x29] + stats[0x2D];
    *(u16*)(out + 0xBE) = stats[0x5F] + stats[0x2F];
    *(u16*)(out + 0xC0) = stats[0x5B] + stats[0x2B];
    *(u16*)(out + 0xC2) = 99;
    *(u16*)(out + 0xC4) = stats[0x5C] + stats[0x2C];
    *(u16*)(out + 0xC6) = 10;
    *(u16*)(out + 0xC8) = stats[0x5A] + stats[0x2A];

    if (*(u16*)(out + 0xB8) >= 1000) *(u16*)(out + 0xB8) = 999;
    if (*(u16*)(out + 0xBA) >= 100) *(u16*)(out + 0xBA) = 99;
    if (*(u16*)(out + 0xBC) >= 1000) *(u16*)(out + 0xBC) = 999;
    if (*(u16*)(out + 0xBE) >= 100) *(u16*)(out + 0xBE) = 99;
    if (*(u16*)(out + 0xC0) >= 1000) *(u16*)(out + 0xC0) = 999;
    if (*(u16*)(out + 0xC2) >= 100) *(u16*)(out + 0xC2) = 99;
    if (*(u16*)(out + 0xC4) >= 1000) *(u16*)(out + 0xC4) = 999;
    if (*(u16*)(out + 0xC6) >= 100) *(u16*)(out + 0xC6) = 99;
    if (*(u16*)(out + 0xC8) >= 100) *(u16*)(out + 0xC8) = 99;
}

// Render Shop Stuff
