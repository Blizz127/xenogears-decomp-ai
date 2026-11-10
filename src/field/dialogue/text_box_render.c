#include "common.h"
#include "field/actor.h"
#include "field/main.h"
#include "field/text_box.h"
#include "field/effects.h"
#include "system/memory.h"
#include "system/archive.h"
#include "system/sound.h"
#include "psyq/libetc.h"
#include "psyq/libcd.h"

extern u16 D_800C3900;
void func_80034874(void* arg0, u8 arg1);
int func_8003487C(void* arg0);

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_dialogue_update_choice (src/decomp/src/field/field_motion.c:1533). */
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
#ifndef XM_T_DialogueFrame
#define XM_T_DialogueFrame
typedef struct {
    RECT rect;                  
    u8 unk08[0x18 - 0x08];
    DR_MODE back_modes[2];      
    TILE back[2];               
    DR_MODE border_modes[2][10]; 
    SPRT border[2][10];         
} DialogueFrame;
#endif
#ifndef XM_T_DialogueChoice
#define XM_T_DialogueChoice
typedef struct {
    s16 status;       
    s16 first;        
    s16 count;        
    s16 index;        
    DR_MODE modes[2]; 
    SPRT cursor[2];   
} DialogueChoice;
#endif
#ifndef XM_T_DialoguePrompt
#define XM_T_DialoguePrompt
typedef struct {
    s16 status;       
    u8 unk02[2];
    DR_MODE modes[2]; 
    SPRT sprite[2];   
} DialoguePrompt;
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
#ifndef XM_T_DialogueWindow
#define XM_T_DialogueWindow
typedef struct DialogueWindow {
    DR_MODE modes[2]; 
    Window text;     
    s32 message;     
    DialogueFrame frame; 
    DialogueChoice choice; 
    DialoguePrompt prompt; 
    s16 timer;       
    s16 prompt_delay; 
    u16 style;       

    s16 busy;        
    u16 age;         
    s16 unk412;      
    s16 cleared;     
    s16 owner;       
    s16 unk418;      
    u8 unk41A[2];
    Fixed slide[2];  
    s32 slide_step[2]; 
    DR_MODE icon_modes[2]; 
    POLY_FT4 icon[2];  
    u8 unk494;       
    u8 unk495;       
    u8 unk496[0x498 - 0x496];
} DialogueWindow;
#endif
#ifndef XM_S_field_dialogue_update_choice
#define XM_S_field_dialogue_update_choice
void xm_field_dialogue_update_choice(s32 window, u_long *ot, s32 buffer) __asm__("func_8007DCF8");
#endif
#ifndef XM_S_field_dialogue_windows
#define XM_S_field_dialogue_windows
extern DialogueWindow xm_field_dialogue_windows[4] __asm__("g_FieldTextBoxes");
#endif
#ifndef XM_S_field_pad_port0_repeated
#define XM_S_field_pad_port0_repeated
extern u16 xm_field_pad_port0_repeated __asm__("D_800C3900");
#endif
#ifndef XM_S_window_highlight_line
#define XM_S_window_highlight_line
void xm_window_highlight_line(Window *window, s32 value) __asm__("func_80034874");
#endif
#ifndef XM_S_window_clear_highlight
#define XM_S_window_clear_highlight
void xm_window_clear_highlight(Window *window) __asm__("func_8003487C");
#endif
void func_8007DCF8(s32 window, u_long *ot, s32 buffer) {
    if (xm_field_dialogue_windows[window].choice.status == 0 && xm_field_dialogue_windows[window].timer == 0) {
        if (xm_field_dialogue_windows[window].age == 0) {
            if (xm_field_pad_port0_repeated & 0x4000) {
                xm_field_dialogue_windows[window].choice.index++;
                if (xm_field_dialogue_windows[window].choice.count - 1 < xm_field_dialogue_windows[window].choice.index) {
                    xm_field_dialogue_windows[window].choice.index = 0;
                }
            }
            if (xm_field_pad_port0_repeated & 0x1000) {
                xm_field_dialogue_windows[window].choice.index--;
                if (xm_field_dialogue_windows[window].choice.index < 0) {
                    xm_field_dialogue_windows[window].choice.index = xm_field_dialogue_windows[window].choice.count - 1;
                }
            }
            xm_window_highlight_line(&xm_field_dialogue_windows[window].text, xm_field_dialogue_windows[window].choice.index + xm_field_dialogue_windows[window].choice.first);
        } else {
            xm_window_clear_highlight(&xm_field_dialogue_windows[window].text);
        }
    }
}
#endif
#else
void func_8007DCF8(s32 index, void* ot, s32 renderContextIndex) {
    (void)ot;
    (void)renderContextIndex;

    /* Every field is addressed through the array element rather than through a
     * local base pointer: retail re-materialises g_FieldTextBoxes + offset for
     * each access (lui at / addu at,at,index*0x498), which is a large part of
     * this function's 468 bytes.  Not yet byte-exact: retail's compiler keeps
     * the window-not-initialised arm out of line at the end and we emit it
     * inline, which costs one extra jump (472 vs 468). */
    if (g_FieldTextBoxes[index].cursor.visibility != 0 ||
        g_FieldTextBoxes[index].windowOpenTimer != 0) {
        return;
    }

    if (g_FieldTextBoxes[index].order != 0) {
        func_8003487C((u8*)&g_FieldTextBoxes[index] + 0x18);
        return;
    }

    if (D_800C3900 & 0x4000) {
        g_FieldTextBoxes[index].cursor.curPosition++;
        if (g_FieldTextBoxes[index].cursor.maxPosition - 1 <
            g_FieldTextBoxes[index].cursor.curPosition) {
            g_FieldTextBoxes[index].cursor.curPosition = 0;
        }
    }

    if (D_800C3900 & 0x1000) {
        g_FieldTextBoxes[index].cursor.curPosition--;
        if (g_FieldTextBoxes[index].cursor.curPosition < 0) {
            g_FieldTextBoxes[index].cursor.curPosition =
                g_FieldTextBoxes[index].cursor.maxPosition - 1;
        }
    }

    func_80034874((u8*)&g_FieldTextBoxes[index] + 0x18,
                  g_FieldTextBoxes[index].cursor.curPosition +
                      g_FieldTextBoxes[index].cursor.startPosition);
}
#endif /* XENO_PC_PORT */

// https://decomp.me/scratch/NE0tE
extern u8 D_800B1DF4[];
extern RECT D_800AFC80[];
extern s32 D_800B068C[];
extern u16 D_800B2174[];
extern s32 D_800ADE90;
extern s32 D_800ADE94;
extern s32 D_800ADE98;
extern u16 D_800C2694;
extern u16 D_800ADF54;
extern u16 D_800ADF56;
extern s16 D_800B21D6;
extern void* D_800ADBF0;
/* Continue-arrow helpers and the per-rotation arrow RECT table used by
 * func_8007E1C0's 0x40A == 0 arm (retail 8007E4B8-8007E534). */
extern RECT D_800ADF04;
extern RECT D_800ADEDC;
extern s32 func_800347AC(void* pWindow);
extern s32 func_800347C0(void* pWindow);

void FieldTextBoxInitializePrimitives(int index);
void* GetStringEntry(void* arg0, s32 arg1);
s32 FieldScriptVMGetVariableValue(s32 index);
void func_80032F54(void* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7);
void func_800345E0(void* arg0);
void func_80034614(void* arg0);
void func_800346D4(void* arg0);
void func_80034714(void* arg0, void* arg1);
void func_80034888(void* arg0, void* arg1, s32 arg2);
void func_80034874(void* arg0, u8 arg1);
int func_8003487C(void* arg0);
int func_80033CD0(void* arg0);
void func_8007E1C0(void* arg0, s32 arg1, s32 arg2);

static inline void FieldTextBoxLinkPrim(void* ot, void* prim) {
    u32 old = *(u32*)ot;
    u32 addr = (u32)(uintptr_t)prim & 0x00FFFFFF;

    *(u32*)prim = (*(u32*)prim & 0xFF000000) | (old & 0x00FFFFFF);
    *(u32*)ot = (old & 0xFF000000) | addr;
}

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_dialogue_reset_windows (src/decomp/src/field/field_motion.c:1557). */
#ifndef XM_S_field_dialogue_reset_windows
#define XM_S_field_dialogue_reset_windows
void xm_field_dialogue_reset_windows(void) __asm__("FieldTextBoxInitialize");
#endif
#ifndef XM_S_field_texture_windows
#define XM_S_field_texture_windows
extern RECT xm_field_texture_windows[16] __asm__("D_800AFC80");
#endif
#ifndef XM_S_SetDrawMode
#define XM_S_SetDrawMode
void xm_SetDrawMode(DR_MODE *p, int dfe, int dtd, int tpage, RECT *tw) __asm__("SetDrawMode");
#endif
#ifndef XM_S_field_texture_window_modes
#define XM_S_field_texture_window_modes
extern DR_MODE xm_field_texture_window_modes[2][16] __asm__("D_800B1DF4");
#endif
#ifndef XM_S_GetTPage
#define XM_S_GetTPage
u_short xm_GetTPage(int tp, int abr, int x, int y) __asm__("GetTPage");
#endif
#ifndef XM_S_field_dialogue_build_packets
#define XM_S_field_dialogue_build_packets
void xm_field_dialogue_build_packets(s32 w) __asm__("FieldTextBoxInitializePrimitives");
#endif
#ifndef XM_S_field_dialogue_message_slots
#define XM_S_field_dialogue_message_slots
extern s32 xm_field_dialogue_message_slots[4] __asm__("D_800B068C");
#endif
void FieldTextBoxInitialize(void) {
    RECT area;
    RECT *window;
    s32 i;

    area.y = 0;
    area.x = 0;
    area.h = 0xFF;
    area.w = 0xFF;
    for (i = 0; i < 16; i++) {
        window = &xm_field_texture_windows[i];
        window->y = 0;
        xm_field_texture_windows[i].x = 0;
        window->h = 0xFF;
        xm_field_texture_windows[i].w = 0xFF;
        window->y = 0;
        xm_field_texture_windows[i].x = 0;
        window->h = 0xFF;
        xm_field_texture_windows[i].w = 0xFF;
        xm_SetDrawMode(&xm_field_texture_window_modes[0][i], 0, 0, xm_GetTPage(0, 0, 0x380, 0x100), window);
        xm_SetDrawMode(&xm_field_texture_window_modes[1][i], 0, 0, xm_GetTPage(0, 0, 0x380, 0x100), window);
    }
    for (i = 0; i < 4; i++) {
        xm_field_dialogue_windows[i].owner = 0xFF;
        xm_field_dialogue_windows[i].unk418 = 0xFF;
        xm_field_dialogue_windows[i].choice.status = -1;
        xm_field_dialogue_windows[i].prompt.status = -1;
        xm_field_dialogue_windows[i].busy = -1;
        xm_field_dialogue_windows[i].cleared = -1;
        xm_field_dialogue_windows[i].age = 0xFFFF;
        xm_field_dialogue_windows[i].owner = 0xFF;
        xm_field_dialogue_windows[i].unk412 = 0;
        xm_field_dialogue_build_packets(i);
        xm_field_dialogue_message_slots[i] = -1;
        xm_SetDrawMode(&xm_field_dialogue_windows[i].modes[0], 0, 0, xm_GetTPage(0, 0, 0x300, 0x100), &area);
        xm_SetDrawMode(&xm_field_dialogue_windows[i].modes[1], 0, 0, xm_GetTPage(0, 0, 0x300, 0x100), &area);
    }
}
#endif
#else
void FieldTextBoxInitialize(void) {
    RECT rect;
    s32 i;

    for (i = 0; i < 0x10; i++) {
        RECT* pRect = &D_800AFC80[i];
        short tpage = GetTPage(0, 0, 0x380, 0x100);

        pRect->x = 0;
        pRect->y = 0;
        pRect->w = 0xFF;
        pRect->h = 0xFF;
        SetDrawMode((DR_MODE*)(D_800B1DF4 + i * 0xC), 0, 0, tpage, pRect);
        SetDrawMode((DR_MODE*)(D_800B1DF4 + 0xC0 + i * 0xC), 0, 0, tpage, pRect);
    }

    rect.x = 0;
    rect.y = 0;
    rect.w = 0xFF;
    rect.h = 0xFF;

    for (i = 0; i < 4; i++) {
        u8* pTextBox = (u8*)&g_FieldTextBoxes[i];
        short tpage;

        *(s16*)(pTextBox + 0x416) = 0xFF;
        *(s16*)(pTextBox + 0x418) = 0xFF;
        *(s16*)(pTextBox + 0x37C) = -1;
        *(s16*)(pTextBox + 0x3C4) = -1;
        *(s16*)(pTextBox + 0x40E) = -1;
        *(s16*)(pTextBox + 0x414) = -1;
        *(u16*)(pTextBox + 0x410) = 0xFFFF;
        *(s16*)(pTextBox + 0x412) = 0;
        FieldTextBoxInitializePrimitives(i);

        D_800B068C[i] = -1;
        tpage = GetTPage(0, 0, 0x300, 0x100);
        SetDrawMode(&g_FieldTextBoxes[i].drawModes[0], 0, 0, tpage, &rect);
        SetDrawMode(&g_FieldTextBoxes[i].drawModes[1], 0, 0, tpage, &rect);
    }
}
#endif /* XENO_PC_PORT */

// _pad[0x4A] is possibly a RECT?
void func_8007E114(int index, int arg1, int arg2, int arg3, s32 arg4) {
    g_FieldTextBoxes[index]._pad[0x4A] = arg1;
    g_FieldTextBoxes[index]._pad[0x4B] = arg2;
    g_FieldTextBoxes[index]._pad[0x4C] = arg3;
    g_FieldTextBoxes[index]._pad[0x4D] = arg4;
}

#ifndef XENO_PC_PORT
/* func_8007E16C: the right-edge value is a separate block-local in each arm (a
 * shared x1 gets hoisted/allocated differently); int parameters. */
#endif
void func_8007E16C(void* arg0, s32 x, s32 y, s32 w, s32 h, s32 flipX) {
#ifdef XENO_PC_PORT
    u8* pPrim = arg0;
    s32 x1;
    s32 y1;

    if (flipX == 0) {
        x--;
        x1 = x + w;
        *(s16*)(pPrim + 0x08) = x;
        *(s16*)(pPrim + 0x10) = x1;
        *(s16*)(pPrim + 0x18) = x;
        *(s16*)(pPrim + 0x20) = x1;
    } else {
        x1 = x + w;
        *(s16*)(pPrim + 0x10) = x;
        *(s16*)(pPrim + 0x08) = x1;
        *(s16*)(pPrim + 0x20) = x;
        *(s16*)(pPrim + 0x18) = x1;
    }

    y1 = y + h;
    *(s16*)(pPrim + 0x0A) = y;
    *(s16*)(pPrim + 0x12) = y;
    *(s16*)(pPrim + 0x1A) = y1;
    *(s16*)(pPrim + 0x22) = y1;
#else
    u8* pPrim = arg0;
    s32 y1;

    if (flipX == 0) {
        s32 x1;
        x--;
        x1 = x + w;
        *(s16*)(pPrim + 0x08) = x;
        *(s16*)(pPrim + 0x10) = x1;
        *(s16*)(pPrim + 0x18) = x;
        *(s16*)(pPrim + 0x20) = x1;
    } else {
        s32 x2 = x + w;
        *(s16*)(pPrim + 0x10) = x;
        *(s16*)(pPrim + 0x08) = x2;
        *(s16*)(pPrim + 0x20) = x;
        *(s16*)(pPrim + 0x18) = x2;
    }

    y1 = y + h;
    *(s16*)(pPrim + 0x0A) = y;
    *(s16*)(pPrim + 0x12) = y;
    *(s16*)(pPrim + 0x1A) = y1;
    *(s16*)(pPrim + 0x22) = y1;
#endif
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/dialogue/text_box_render", func_8007E1C0);
#else
void func_8007E1C0(void* ot, s32 renderContextIndex, s32 textBoxIndex) {
    u8* pTextBox = (u8*)&g_FieldTextBoxes[textBoxIndex];
    s32 x;
    s32 y;
    s32 w;
    s32 h;
    s32 flags;
    FieldTextBoxBackground* pBackground;
    FieldTextBoxBorders* pBorders;
    DR_MODE* borderDrawModes;
    SPRT* borderSprites;
    s32 i;

    if (*(s16*)(pTextBox + 0x40E) != 0) {
        return;
    }

    x = *(s16*)(pTextBox + 0xAC);
    y = *(s16*)(pTextBox + 0xAE);
    w = *(s16*)(pTextBox + 0xB0);
    h = *(s16*)(pTextBox + 0xB2);

    if (*(s16*)(pTextBox + 0x408) != 0) {
        s32 openTimer = *(s16*)(pTextBox + 0x408);
        s32 baseTimer = D_800B21D6;
        s32 halfW;
        s32 halfH;
        s32 animW;
        s32 animH;

        animW = ((w << 16) / (baseTimer * 2)) * (baseTimer - openTimer);
        animH = ((h << 16) / (baseTimer * 2)) * (baseTimer - openTimer);
        halfW = (w + ((u32)w >> 31)) >> 1;
        halfH = (h + ((u32)h >> 31)) >> 1;
        w = (animW << 1) >> 16;
        h = (animH << 1) >> 16;
        x = x + halfW - (animW >> 16);
        y = y + halfH - (animH >> 16);
        if (w < 0x10) {
            x -= (0x10 - w) / 2;
            w = 0x10;
        }
        if (h < 0x10) {
            y -= (0x10 - h) / 2;
            h = 0x10;
        }

        *(s32*)(pTextBox + 0x41C) += *(s32*)(pTextBox + 0x424);
        *(s32*)(pTextBox + 0x420) += *(s32*)(pTextBox + 0x428);
        x += *(s16*)(pTextBox + 0x41E);
        y += *(s32*)(pTextBox + 0x420) >> 16;
    }

    if (*(s16*)(pTextBox + 0x3C4) != 0 || *(u16*)(pTextBox + 0x410) != 0 ||
        *(s16*)(pTextBox + 0x408) != 0 || (*(u16*)(pTextBox + 0x40C) & 0x40) ||
        *(s16*)(pTextBox + 0x37C) == 0) {
        *(s16*)(pTextBox + 0x40A) = 2;
    } else if (*(s16*)(pTextBox + 0x40A) != 0) {
        *(s16*)(pTextBox + 0x40A) -= 1;
    } else {
        /* Retail 8007E4B8-8007E5F8 (the 0x40A == 0 arm): build the
         * continue-arrow draw mode for this render context.
         *
         * func_800347AC / func_800347C0 return the two per-arrow words that
         * retail stores at g_FieldTextBoxes[textBoxIndex]+0x3E8/+0x3EA for the
         * current render context (stride 20), after which it re-links the
         * primitives into the OT. The rect comes from the D_800ADF04 RECT
         * table indexed by the rotation counter D_800ADE94. */
        u8* pWindow = pTextBox + 0x18;
        s32 arrowA = func_800347AC(pWindow);
        s32 arrowB = func_800347C0(pWindow);
        RECT rect;
        s32 tpage;
        u8* pArrowBase;

        rect.x = *(u16*)((u8*)&D_800ADF04 + D_800ADE94 * 8 + 0);
        rect.y = *(u16*)((u8*)&D_800ADF04 + D_800ADE94 * 8 + 2);
        rect.w = *(u16*)((u8*)&D_800ADF04 + D_800ADE94 * 8 + 4);
        rect.h = *(u16*)((u8*)&D_800ADF04 + D_800ADE94 * 8 + 6);

        tpage = GetTPage(0, 0, 0x298, 0x1C0);

        pArrowBase = (u8*)&g_FieldTextBoxes + textBoxIndex * 0x498;
        SetDrawMode((DR_MODE*)(pArrowBase + 0x3C4 + renderContextIndex * 12 + 4),
                    0, 0, (u16)tpage, &rect);

        *(s16*)(pArrowBase + 0x3E8 + renderContextIndex * 20) = (s16)arrowA;
        *(s16*)(pArrowBase + 0x3EA + renderContextIndex * 20) = (s16)arrowB;
    }

    pBorders = &g_FieldTextBoxes[textBoxIndex].borders;
    if (renderContextIndex == 0) {
        borderDrawModes = pBorders->drawModes1;
        borderSprites = pBorders->sprites1;
    } else {
        borderDrawModes = pBorders->drawModes2;
        borderSprites = pBorders->sprites2;
    }

    borderSprites[0].x0 = x - 8;
    borderSprites[0].y0 = y - 7;
    borderSprites[1].x0 = x + w - 8;
    borderSprites[1].y0 = y + 9;
    borderSprites[2].x0 = x + w - 8;
    borderSprites[2].y0 = y - 7;
    borderSprites[3].x0 = x - 8;
    borderSprites[3].y0 = y + 9;
    borderSprites[4].x0 = x - 8;
    borderSprites[4].y0 = y + h - 9;
    borderSprites[5].x0 = x + 8;
    borderSprites[5].y0 = y - 7;
    borderSprites[6].x0 = x + w - 8;
    borderSprites[6].y0 = y + h - 9;
    borderSprites[7].x0 = x + 8;
    borderSprites[7].y0 = y + h - 9;

    borderSprites[1].h = h - 0x12;
    borderSprites[3].h = h - 0x12;
    if ((s16)borderSprites[1].h < 0) {
        borderSprites[1].h = 0;
        borderSprites[3].h = 0;
    }
    borderSprites[5].w = w - 0x10;
    borderSprites[7].w = w - 0x10;

    flags = *(u16*)(pTextBox + 0x40C);
    if (!(flags & 0x40)) {
        for (i = 0; i < 8; i++) {
            FieldTextBoxLinkPrim(ot, &borderSprites[i]);
            FieldTextBoxLinkPrim(ot, &borderDrawModes[i]);
        }
    }

    {
        FieldTextBoxPortrait* pPortrait = &g_FieldTextBoxes[textBoxIndex].portrait;
        POLY_FT4* pPortraitPoly = &pPortrait->polys[renderContextIndex];
        s32 portraitW = (w - 4 < 0x40) ? w - 8 : 0x40;
        s32 portraitH = (h - 4 < 0x40) ? h - 8 : 0x40;
        s32 portraitX = x + 4;
        s32 portraitY = y + 4;

        if (flags & 0x20) {
            portraitX = x + w - portraitW - 4;
        }

        func_8007E16C(pPortraitPoly, portraitX, portraitY,
                      portraitW, portraitH, flags & 0x20);

        if (pPortrait->shouldRenderPortrait == 1) {
            FieldTextBoxLinkPrim(ot, pPortraitPoly);
            FieldTextBoxLinkPrim(ot, &pPortrait->drawModes[renderContextIndex]);
        }
    }

    /* Retail .L8007EA4C-.L8007EC28: the cursor SPRT + its draw mode. Entered
     * only while the box is settled (0x37C == 0, 0x410 == 0, 0x408 == 0); the
     * horizontal placement is x+0x16 when the portrait is suppressed or the
     * box is right-aligned (0x40C & 0x20), else x+0x5A. The vertical position
     * is (0x382 + 0x37E) * 14 + y + 8 and the rect comes from the
     * D_800ADEDC RECT table indexed by D_800ADE94. */
    if (*(s16*)(pTextBox + 0x37C) == 0 && *(u16*)(pTextBox + 0x410) == 0 &&
        *(s16*)(pTextBox + 0x408) == 0) {
        RECT cursorRect;
        s32 cursorTPage;
        s32 cursorIdx = textBoxIndex * 0x498 + renderContextIndex * 20;

        if (*(u8*)(pTextBox + 0x494) != 1 || (flags & 0x20)) {
            *(s16*)((u8*)&g_FieldTextBoxes + 0x3A4 + cursorIdx) = (s16)(x + 0x16);
        } else {
            *(s16*)((u8*)&g_FieldTextBoxes + 0x3A4 + cursorIdx) = (s16)(x + 0x5A);
        }
        *(s16*)((u8*)&g_FieldTextBoxes + 0x3A6 + cursorIdx) =
            (s16)((*(s16*)(pTextBox + 0x382) + *(s16*)(pTextBox + 0x37E)) * 14 +
                  y + 8);

        cursorRect.x = *(u16*)((u8*)&D_800ADEDC + D_800ADE94 * 8 + 0);
        cursorRect.y = *(u16*)((u8*)&D_800ADEDC + D_800ADE94 * 8 + 2);
        cursorRect.w = *(u16*)((u8*)&D_800ADEDC + D_800ADE94 * 8 + 4);
        cursorRect.h = *(u16*)((u8*)&D_800ADEDC + D_800ADE94 * 8 + 6);

        cursorTPage = GetTPage(0, 0, 0x288, 0x1C0);
        SetDrawMode((DR_MODE*)(pTextBox + 0x37C + renderContextIndex * 12 + 8),
                    0, 0, (u16)cursorTPage, &cursorRect);
    }

    pBackground = &g_FieldTextBoxes[textBoxIndex].background;
    pBackground->tiles[renderContextIndex].x0 = x;
    pBackground->tiles[renderContextIndex].y0 = y + 1;
    pBackground->tiles[renderContextIndex].w = w;
    pBackground->tiles[renderContextIndex].h = h - 2;
    if (!(flags & 0x40)) {
        FieldTextBoxLinkPrim(ot, &pBackground->tiles[renderContextIndex]);
        FieldTextBoxLinkPrim(ot, &pBackground->drawModes[renderContextIndex]);
    }
}
#endif /* XENO_PC_PORT */

extern u8 D_800594D4;
extern u8 D_800594D5;
extern u8 D_800594D6;
extern RECT D_800ADE9C[];
extern RECT D_800ADEDC;
extern RECT D_800ADF04;

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/dialogue/text_box_render", FieldTextBoxInitializePrimitives);
#else
void FieldTextBoxInitializePrimitives(int index) {
    int i;
    RECT rect;
    POLY_FT4* pPoly;
    POLY_FT4* pPoly2;
    TILE* pBackgroundTile;
    TILE* pBackgroundTile2;
    SPRT* pBorderSprite1;
    SPRT* pBorderSprite2;
    SPRT* pArrowSprite;
    SPRT* pArrowSprite2;
    SPRT* pCursorSprite2;
    u16* pWidth;
    u16* pHeight;

    // Background
    SetDrawMode(&g_FieldTextBoxes[index].background.drawModes[0], NULL, 0, GetTPage(0, 2, 0x280, 0x1F0), NULL);
    SetDrawMode(&g_FieldTextBoxes[index].background.drawModes[1], NULL, 0, GetTPage(0, 2, 0x280, 0x1F0), NULL);
    pBackgroundTile = &g_FieldTextBoxes[index].background.tiles[0];
    SetTile(pBackgroundTile);
    setRGB0(pBackgroundTile, D_800594D4, D_800594D5, D_800594D6);
    SetSemiTrans(pBackgroundTile, 1);
    pBackgroundTile2 =  &g_FieldTextBoxes[index].background.tiles[1];
    *pBackgroundTile2 = *pBackgroundTile;

    // Arrow
    rect.x = D_800ADF04.x;
    rect.y = D_800ADF04.y;
    rect.w = D_800ADF04.w;
    rect.h = D_800ADF04.h;
    SetDrawMode(&g_FieldTextBoxes[index].continueArrow.drawModes[0], NULL, 0, GetTPage(0, 0, 0x298, 0x1C0), &rect);
    SetDrawMode(&g_FieldTextBoxes[index].continueArrow.drawModes[1], NULL, 0, GetTPage(0, 0, 0x298, 0x1C0), &rect);
    SetSprt(&g_FieldTextBoxes[index].continueArrow.sprites[0]);
    pArrowSprite = &g_FieldTextBoxes[index].continueArrow.sprites[0];
    setRGB0(pArrowSprite, 0x80, 0x80, 0x80);
    setWH(pArrowSprite, 0xC, 0x8);
    pArrowSprite->clut = GetClut(0x100, 0xF6);
    pArrowSprite2 = &g_FieldTextBoxes[index].continueArrow.sprites[1];
    setUV0(pArrowSprite, 0x80, 0xC0);
    setXY0(pArrowSprite, 0x0, 0x0);
    *pArrowSprite2 = *pArrowSprite;

    // Cursor
    rect.x = D_800ADEDC.x;
    rect.y = D_800ADEDC.y;
    rect.w = D_800ADEDC.w;
    rect.h = D_800ADEDC.h;
    SetDrawMode(&g_FieldTextBoxes[index].cursor.drawModes[0], NULL, 0, GetTPage(0, 0, 0x288, 0x1C0), &rect);
    SetDrawMode(&g_FieldTextBoxes[index].cursor.drawModes[1], NULL, 0, GetTPage(0, 0, 0x288, 0x1C0), &rect);
    SetSprt(&g_FieldTextBoxes[index].cursor.sprites[0]);
    setRGB0(&g_FieldTextBoxes[index].cursor.sprites[0], 0x80, 0x80, 0x80);
    g_FieldTextBoxes[index].cursor.sprites[0].clut = GetClut(0x100, 0xF6);
    pCursorSprite2 = &g_FieldTextBoxes[index].cursor.sprites[1];
    setWH(&g_FieldTextBoxes[index].cursor.sprites[0], 0x8, 0xC);
    setUV0(&g_FieldTextBoxes[index].cursor.sprites[0], 0x80, 0xC0);
    setXY0(&g_FieldTextBoxes[index].cursor.sprites[0], 0x0, 0x0);
    *pCursorSprite2 = g_FieldTextBoxes[index].cursor.sprites[0];
    g_FieldTextBoxes[index].continueArrowTimer = 2;

    // Borders
    for (i = 0; i < 8; i++) {
        pWidth = &D_800ADE9C[i].w;
        pHeight = &D_800ADE9C[i].h;
        
        rect.x = D_800ADE9C[i].x;
        rect.y = D_800ADE9C[i].y;
        rect.w = *pWidth;
        rect.h = *pHeight;
        
        SetDrawMode(&g_FieldTextBoxes[index].borders.drawModes1[i], NULL, 0, GetTPage(0, 2, 0x280, 0x1F0), &rect);
        SetDrawMode(&g_FieldTextBoxes[index].borders.drawModes2[i], NULL, 0, GetTPage(0, 2, 0x280, 0x1F0), &rect);
        pBorderSprite1 = &g_FieldTextBoxes[index].borders.sprites1[i];
        SetSprt(pBorderSprite1);
        setRGB0(pBorderSprite1, 0x80, 0x80, 0x80);
        pBorderSprite1->clut = GetClut(0x100, 0xF4);
        SetSemiTrans(pBorderSprite1, 1);
        setUV0(pBorderSprite1, 0x80, 0xC0);
        setWH(pBorderSprite1, *pWidth, *pHeight);
        setXY0(pBorderSprite1, 0x0, 0x0);
        pBorderSprite2 =  &g_FieldTextBoxes[index].borders.sprites2[i];
        *pBorderSprite2 = *pBorderSprite1;
    }

    // Portrait
    rect.y = 0;
    rect.x = 0;
    rect.h = 0xFF;
    rect.w = 0xFF;
    SetDrawMode(&g_FieldTextBoxes[index].portrait.drawModes[0], NULL, 0, GetTPage(1, 0, 0x2C0, 0x100), &rect);
    SetDrawMode(&g_FieldTextBoxes[index].portrait.drawModes[1], NULL, 0, GetTPage(1, 0, 0x2C0, 0x100), &rect);
    pPoly = &g_FieldTextBoxes[index].portrait.polys[0];
    SetPolyFT4(pPoly);
    setRGB0(pPoly, 0x80, 0x80, 0x80);
    pPoly->clut = GetClut(0, 0xE0);
    pPoly2 = &g_FieldTextBoxes[index].portrait.polys[1];
    pPoly->tpage = GetTPage(1, 0, 0x2C0, 0x100);
    *pPoly2 = *pPoly;
}
#endif /* XENO_PC_PORT */

extern u8 D_800ADF34[];

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_dialogue_set_portrait_frame (src/decomp/src/field/field_motion.c:1861). */
#ifndef XM_S_field_dialogue_set_portrait_frame
#define XM_S_field_dialogue_set_portrait_frame
void xm_field_dialogue_set_portrait_frame(s32 window, s32 frame) __asm__("func_8007F5AC");
#endif
#ifndef XM_S_field_dialogue_portrait_uv_origins
#define XM_S_field_dialogue_portrait_uv_origins
extern DVECTOR xm_field_dialogue_portrait_uv_origins[8] __asm__("D_800ADF34");
#endif
#ifndef XM_S_GetClut
#define XM_S_GetClut
u_short xm_GetClut(int x, int y) __asm__("GetClut");
#endif
void func_8007F5AC(s32 window, s32 frame) {
    DialogueWindow *w;
    DVECTOR *tex;
    s16 *u;
    s16 *v;

    w = &xm_field_dialogue_windows[window];
    tex = xm_field_dialogue_portrait_uv_origins;
    u = &tex[frame].vx;
    v = &xm_field_dialogue_portrait_uv_origins[frame].vy;
    xm_field_dialogue_windows[window].icon[1].u0 = w->icon[0].u0 = *u;
    xm_field_dialogue_windows[window].icon[1].v0 = w->icon[0].v0 = *v;
    xm_field_dialogue_windows[window].icon[1].u1 = w->icon[0].u1 = *u + 0x40;
    xm_field_dialogue_windows[window].icon[1].v1 = w->icon[0].v1 = *v;
    xm_field_dialogue_windows[window].icon[1].u2 = w->icon[0].u2 = *u;
    xm_field_dialogue_windows[window].icon[1].v2 = w->icon[0].v2 = *v + 0x40;
    xm_field_dialogue_windows[window].icon[1].u3 = w->icon[0].u3 = *u + 0x40;
    xm_field_dialogue_windows[window].icon[1].v3 = w->icon[0].v3 = *v + 0x40;
    xm_field_dialogue_windows[window].icon[1].clut = w->icon[0].clut = xm_GetClut(0, frame + 0xE0);
}
#endif
#else
void func_8007F5AC(s32 textBoxIndex, s32 faceDirection) {
    /* Retail 8007F5AC .. 8007F6F8: both portrait primitives are written
     * unrolled, corner by corner (poly0 then poly1 for each byte), each store
     * addressing the array element directly, and the CLUT lookup is issued
     * only after all sixteen UV stores.
     * Not yet byte-exact: the retail compiler re-loads the source bytes for the
     * later stores while ours keeps them in registers, which is 12 bytes (3
     * instructions) of the 332. */
    u8* pUv = &D_800ADF34[faceDirection << 2];
    u8 u = pUv[0];
    u8 v = pUv[2];
    u16 clut;

    g_FieldTextBoxes[textBoxIndex].portrait.polys[0].u0 = u;
    g_FieldTextBoxes[textBoxIndex].portrait.polys[1].u0 = u;
    g_FieldTextBoxes[textBoxIndex].portrait.polys[0].v0 = v;
    g_FieldTextBoxes[textBoxIndex].portrait.polys[1].v0 = v;
    g_FieldTextBoxes[textBoxIndex].portrait.polys[0].u1 = u + 0x40;
    g_FieldTextBoxes[textBoxIndex].portrait.polys[1].u1 = u + 0x40;
    g_FieldTextBoxes[textBoxIndex].portrait.polys[0].v1 = v;
    g_FieldTextBoxes[textBoxIndex].portrait.polys[1].v1 = v;
    g_FieldTextBoxes[textBoxIndex].portrait.polys[0].u2 = u;
    g_FieldTextBoxes[textBoxIndex].portrait.polys[1].u2 = u;
    g_FieldTextBoxes[textBoxIndex].portrait.polys[0].v2 = v + 0x40;
    g_FieldTextBoxes[textBoxIndex].portrait.polys[1].v2 = v + 0x40;
    g_FieldTextBoxes[textBoxIndex].portrait.polys[0].u3 = u + 0x40;
    g_FieldTextBoxes[textBoxIndex].portrait.polys[1].u3 = u + 0x40;
    g_FieldTextBoxes[textBoxIndex].portrait.polys[0].v3 = v + 0x40;
    g_FieldTextBoxes[textBoxIndex].portrait.polys[1].v3 = v + 0x40;

    clut = GetClut(0, faceDirection + 0xE0);
    g_FieldTextBoxes[textBoxIndex].portrait.polys[0].clut = clut;
    g_FieldTextBoxes[textBoxIndex].portrait.polys[1].clut = clut;
}
#endif /* XENO_PC_PORT */

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_dialogue_close_window (src/decomp/src/field/field_motion.c:1883). */
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
#ifndef XM_S_field_dialogue_close_window
#define XM_S_field_dialogue_close_window
s32 xm_field_dialogue_close_window(s16 window) __asm__("func_8007F6F8");
#endif
#ifndef XM_S_window_reset_if_idle
#define XM_S_window_reset_if_idle
void xm_window_reset_if_idle(Window *window) __asm__("func_80034614");
#endif
#ifndef XM_S_window_end_wait
#define XM_S_window_end_wait
void xm_window_end_wait(Window *window) __asm__("func_800345E0");
#endif
#ifndef XM_S_window_close
#define XM_S_window_close
void xm_window_close(Window *window) __asm__("func_800346D4");
#endif
#ifndef XM_S_field_work
#define XM_S_field_work
extern FieldWork xm_field_work __asm__("g_FieldEffects");
#endif
s32 func_8007F6F8(s16 window) {
    if (xm_field_dialogue_windows[window].busy == 0) {
        xm_window_reset_if_idle(&xm_field_dialogue_windows[window].text);
        xm_window_end_wait(&xm_field_dialogue_windows[window].text);
        xm_window_close(&xm_field_dialogue_windows[window].text);
        xm_field_dialogue_windows[window].choice.status = -1;
        xm_field_dialogue_windows[window].busy = -1;
        xm_field_dialogue_windows[window].cleared = -1;
        xm_field_dialogue_windows[window].age = 0xFFFF;
        xm_field_dialogue_message_slots[window] = -1;
        xm_field_work.open_windows &= (1 << window) ^ 0xFF;
        xm_field_dialogue_windows[window].owner = 0xFF;
        xm_field_dialogue_windows[window].unk412 = 0;
        return 0;
    }
    return -1;
}
#endif
#else
void func_8007F6F8(s16 index) {
    u32 mask;

    if (g_FieldTextBoxes[index].visibility != 0) {
        return;
    }

    /* Each call re-derives the window base from the array element, as retail
     * does (lui at, %hi(g_FieldTextBoxes + 0x18); addu at, at, index*0x498). */
    func_80034614((u8*)&g_FieldTextBoxes[index] + 0x18);
    func_800345E0((u8*)&g_FieldTextBoxes[index] + 0x18);
    func_800346D4((u8*)&g_FieldTextBoxes[index] + 0x18);

    g_FieldTextBoxes[index].cursor.visibility = -1;
    g_FieldTextBoxes[index].visibility = -1;
    g_FieldTextBoxes[index].status = -1;
    g_FieldTextBoxes[index].order = 0xFFFF;
    D_800B068C[index] = -1;
    mask = 1 << index;
    D_800B2174[0] &= (u16)(mask ^ 0xFF);
    g_FieldTextBoxes[index].ownerActorID = 0xFF;
    g_FieldTextBoxes[index].unk412 = 0;
}
#endif /* XENO_PC_PORT */

// Project (0, Y, 0) from actor's model/local space to screen
#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_descriptor_get_screen_point (src/decomp/src/field/field_motion.c:1902). */
#ifndef XM_T_SavedView
#define XM_T_SavedView
typedef struct {
    s16 octant;
    s32 projection;
    s16 elevation;
} SavedView;
#endif
#ifndef XM_T_ModelLight
#define XM_T_ModelLight
typedef struct {
    s32 vx, vy, vz;
    u16 r, g, b;
} ModelLight;
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
#ifndef XM_T_FieldInstance
#define XM_T_FieldInstance
typedef struct {
    u8 unk00[4];
    SpriteModel *mesh; 
    void *packets[2]; 
    u8 unk10[2];
    s16 mode;        
    MorphState *anims; 
    s16 center[3];   
    s16 unk1E;
    s16 radius;      
} FieldInstance;
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
#ifndef XM_T_FieldMarker
#define XM_T_FieldMarker
typedef struct FieldMarker {
    SVECTOR v[4];
    POLY_FT4 poly[2];
} FieldMarker;
#endif
#ifndef XM_T_FieldDescriptor
#define XM_T_FieldDescriptor
typedef struct FieldDescriptor {
    FieldInstance *instance; 
    Sprite *model;           
    struct FieldMarker *shadow; 
    MATRIX matrix;           
    MATRIX transform;        
    FieldActor *actor;       
    SVECTOR rotation;        
    u16 flags;               
    u16 unk5A;               
} FieldDescriptor;
#endif
#ifndef XM_T_Attribute
#define XM_T_Attribute
typedef union {
    u32 word;
    u8 bytes[4];
} Attribute;
#endif
#ifndef XM_T_CollisionTriangle
#define XM_T_CollisionTriangle
typedef struct {
    s16 unk00[6];
    u8 attribute;
    u8 unk0D;
} CollisionTriangle;
#endif
#ifndef XM_T_FieldComponents
#define XM_T_FieldComponents
typedef struct {
    s32 descriptor_count;                      
    FieldDescriptor *descriptors;              
    s32 *geometry;                             
    s32 *collision;                            
    s32 *sprites;                              
    Attribute *collision_attributes;           
    CollisionTriangle *collision_triangles[4]; 
    void *collision_vertices[4];               
    s32 triangle_counts[4];                    
    s16 layer_count;                           
} FieldComponents;
#endif
#ifndef XM_T_FieldView
#define XM_T_FieldView
typedef struct FieldView {
    VECTOR eye;              
    VECTOR target;           
    VECTOR up;               
    VECTOR eye_goal;         
    VECTOR target_goal;      
    VECTOR unk050;           
    VECTOR shake_offset;     
    VECTOR saved_target;     
    VECTOR point_actor_a;    
    VECTOR saved_eye;        
    VECTOR point_actor_b;    
    s32 scripted_zoom;       
    s16 mode;                
    s16 scripted_elevation;  
    s16 scripted_heading;    
    s16 scripted_scale;      
    u16 scripted;            
    s16 target_steps;        
    VECTOR scripted_target;  
    VECTOR target_step;      
    s16 eye_steps;           
    s32 scripted_eye[3];     
    s32 unk0F0;              
    s32 eye_step[3];         
    s32 unk100;              
    s32 target_a;            
    s32 target_b;            
    s16 angle;               
    s16 view_angle;          
    MATRIX previous_view;    
    MATRIX orbit;            
    SVECTOR orbit_angles;    
    u32 flags;               
    s16 bounds[4];           
    SVECTOR heading_angles;  
    s32 heading_velocity;    
    s32 heading_high;        
    u8 heading_blocks[2];    
    s16 heading_steps;       
    s32 projection;          
    u16 elevation;           
    s16 distance;            
    s16 elevation_steps;     
    s32 elevation_value;     
    s32 elevation_step;      
    u32 heading;             
    s16 projection_steps;    
    s32 projection_value;    
    s32 projection_step;     
    s16 steps;               
    s32 start;               
    s32 step;                
    s16 shake;               
    s16 shake_time;          
    s16 shake_stop;          
    s32 shake_amplitude[3];  
    s32 shake_step[3];       
    SavedView saved_view;    
    SVECTOR world_angles;    
    SVECTOR anchor;          
    MATRIX scaled_world;     
    MATRIX unk204;           
    MATRIX world_matrix;     
    s32 scale;               
    ModelLight lights[3];    
    s16 back_color[3];       
    u8 unk28A[2];
    FieldComponents components; 
} FieldView;
#endif
#ifndef XM_S_field_descriptor_get_screen_point
#define XM_S_field_descriptor_get_screen_point
void xm_field_descriptor_get_screen_point(s32 index, s32 *x, s32 *y, s32 height) __asm__("func_8007F814");
#endif
#ifndef XM_S_CompMatrix
#define XM_S_CompMatrix
MATRIX *xm_CompMatrix(MATRIX *m0, MATRIX *m1, MATRIX *m2) __asm__("CompMatrix");
#endif
#ifndef XM_S_field_view
#define XM_S_field_view
extern FieldView xm_field_view __asm__("g_CameraEye");
#endif
#ifndef XM_S_SetRotMatrix
#define XM_S_SetRotMatrix
void xm_SetRotMatrix(MATRIX *m) __asm__("SetRotMatrix");
#endif
#ifndef XM_S_SetTransMatrix
#define XM_S_SetTransMatrix
void xm_SetTransMatrix(MATRIX *m) __asm__("SetTransMatrix");
#endif
#ifndef XM_S_RotTransPers
#define XM_S_RotTransPers
long xm_RotTransPers(SVECTOR *v0, long *sxy, long *p, long *flag) __asm__("RotTransPers");
#endif
void func_8007F814(s32 index, s32 *x, s32 *y, s32 height) {
    SVECTOR point;
    MATRIX m;
    long screen;
    long depth;
    long flag;

    xm_CompMatrix(&xm_field_view.scaled_world, &xm_field_view.components.descriptors[index].matrix, &m);
    xm_SetRotMatrix(&m);
    xm_SetTransMatrix(&m);
    point.vx = 0;
    point.vy = height;
    point.vz = 0;
    xm_RotTransPers(&point, &screen, &depth, &flag);
    *y = screen >> 16;
    *x = (s16)screen;
}
#endif
#else
void func_8007F814(s32 actorIndex, s32* screenX, s32* screenY, s32 yOffset) {
    MATRIX matrix;
    SVECTOR localPos;
    long screenXY;
    long depth;
    long flag;

    CompMatrix(&g_Scene.worldToScreenMatrix, &g_FieldActors[actorIndex].transformMatrix, &matrix);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);

    localPos.vx = 0;
    localPos.vy = yOffset;
    localPos.vz = 0;
    RotTransPers(&localPos, &screenXY, &depth, &flag);

    *screenX = (s16)screenXY;
    *screenY = (s16)(screenXY >> 16);
}
#endif /* XENO_PC_PORT */

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/dialogue/text_box_render", func_8007F8DC);
#else
s32 func_8007F8DC(s32 x, s32 y, s32 stringIndex, s32 textBoxIndex, s32 width, s32 height,
                  s32 ownerActorIndex, s32 talkingActorIndex, s32 mode, s32 orientationFlags,
                  s32 dialogFlags) {
    u8* pTextBox;
    ActorData* pOwnerActor;
    ActorData* pTalkingActor;
    s32 flags;
    s32 slot;
    s32 i;
    s32 targetX;
    s32 targetY;
    s32 boxPixelWidth;
    s32 boxPixelHeight;
    s32 boxOffset;
    s32 openTimer;
    s32 portraitFlags;

    pTalkingActor = (ActorData*)(uintptr_t)g_FieldActors[talkingActorIndex].pActorData;
    /* Retail s4 is argument 7 (owner); argument 8 remains the speaker used
     * for screen positioning and the final hidden-actor check. */
    pOwnerActor = (ActorData*)(uintptr_t)g_FieldActors[ownerActorIndex].pActorData;
    flags = pOwnerActor->dialogFlags >> 16;
    if (flags == 0) {
        flags = pOwnerActor->dialogFlags & 0xFFFF;
    }
    flags |= orientationFlags;

    slot = D_800ADE90 & 3;
    for (i = 0; i < 4; i++) {
        slot = D_800ADE90 & 3;
        D_800ADE90++;
        if (D_800B068C[slot] == -1) {
            D_800B068C[slot] = 0;
            break;
        }
    }

    pTextBox = (u8*)&g_FieldTextBoxes[textBoxIndex];
    *(s32*)(pTextBox + 0x88) = FieldScriptVMGetVariableValue(0x16);
    *(s32*)(pTextBox + 0x8C) = FieldScriptVMGetVariableValue(0x18);
    *(s32*)(pTextBox + 0x90) = FieldScriptVMGetVariableValue(0x1A);
    *(s32*)(pTextBox + 0x94) = FieldScriptVMGetVariableValue(0x1C);
    *(s16*)(pTextBox + 0x98) = *(s32*)(pTextBox + 0x94);

    if (mode == 2) {
        targetX = 0xA0;
        targetY = y + 0x20;
    } else if (mode == 3) {
        targetX = x + 8 + width * 2;
        targetY = y + 8 + height * 7;
    } else {
        func_8007F814(talkingActorIndex, &targetX, &targetY, -0x40);
    }

    if (pOwnerActor->faceId != 0xFF) {
        u32 actorFlags12C = *(u32*)((u8*)pOwnerActor + 0x12C);
        s32 portraitSelector = (actorFlags12C >> 1) & 0xE;

        if ((flags & 0x402) == 0) {
            func_8007F5AC(textBoxIndex, portraitSelector | 1);
        } else {
            func_8007F5AC(textBoxIndex, portraitSelector);
        }
        g_FieldTextBoxes[textBoxIndex].portrait.shouldRenderPortrait = 1;
        g_FieldTextBoxes[textBoxIndex].portrait.portraitID = pOwnerActor->faceId;
    } else {
        g_FieldTextBoxes[textBoxIndex].portrait.shouldRenderPortrait = 0;
        g_FieldTextBoxes[textBoxIndex].portrait.portraitID = 0x80;
    }

    boxPixelWidth = width * 4 + 0x10;
    boxPixelHeight = height * 14 + 0x10;
    g_FieldTextBoxes[textBoxIndex].cursor.visibility = -1;
    func_8007E114(textBoxIndex, x, y, boxPixelWidth, boxPixelHeight);

    portraitFlags = 0;
    if (pOwnerActor->faceId != 0xFF && (flags & 0x402) == 0) {
        portraitFlags = 0x44;
    }

    /* Asm 8007FCB4-8007FCE0: separate lhu of D_800ADF54 / D_800ADF56 at
     * textBoxIndex*4. Window width is DialogGetWidth as-is (width*2+8 is
     * only for boxOffset below). */
    func_80032F54(pTextBox + 0x18, *(&D_800ADF54 + textBoxIndex * 2),
                  *(&D_800ADF56 + textBoxIndex * 2), x + portraitFlags + 8, y + 8, width, mode,
                  height);

    if (flags & 0x400) {
        g_FieldTextBoxes[textBoxIndex].flags |= 0x20;
    }

    /* Asm 8007FD10-8007FD30: sb speed at box+0x80 (window+0x68). 1 when
     * D_800B21D6 == 8, else 2. `_pad[0x68]` is a short index and missed. */
    *(u8*)(pTextBox + 0x80) = (D_800B21D6 == 8) ? 1 : 2;
    /* XENO_PC_PORT: the string-entry pointer at 0xA8 is a 32-bit field on PSX;
     * storing a native 64-bit pointer here spills into 0xAC/0xAE (the box's
     * border x/y set by func_8007E114), collapsing the box to (0,0). Store only
     * the low 32 bits (port heap is in low 4GB). Match-preserving: the casts are
     * no-ops on 32-bit PSX. Paired read at the func_80034714 call below. */
    *(u32*)(pTextBox + 0xA8) = (u32)(uintptr_t)GetStringEntry(D_800ADBF0, stringIndex);
    g_FieldTextBoxes[textBoxIndex].visibility = 0;
    *(u16*)(pTextBox + 0x28) |= 2;
    g_FieldTextBoxes[textBoxIndex].ownerActorID = ownerActorIndex;
    g_FieldTextBoxes[textBoxIndex].talkingActorID = talkingActorIndex;
    g_FieldTextBoxes[textBoxIndex].windowOpenTimer = D_800B21D6;
    g_FieldTextBoxes[textBoxIndex].unk412 = (dialogFlags & 0x800) ? 1 : 0;

    boxOffset = width * 2 + 8;
    boxPixelHeight = height * 7 + 8;
    g_FieldTextBoxes[textBoxIndex].positionOffsetX = (targetX - boxOffset - x) << 16;
    g_FieldTextBoxes[textBoxIndex].positionOffsetY = (targetY - boxPixelHeight - y) << 16;

    if (dialogFlags & 0x100) {
        g_FieldTextBoxes[textBoxIndex].windowOpenTimer = 1;
        g_FieldTextBoxes[textBoxIndex].positionOffsetDeltaX = -g_FieldTextBoxes[textBoxIndex].positionOffsetX;
        g_FieldTextBoxes[textBoxIndex].positionOffsetDeltaY = -g_FieldTextBoxes[textBoxIndex].positionOffsetY;
    } else {
        openTimer = D_800B21D6;
        g_FieldTextBoxes[textBoxIndex].positionOffsetDeltaX = -g_FieldTextBoxes[textBoxIndex].positionOffsetX / openTimer;
        g_FieldTextBoxes[textBoxIndex].positionOffsetDeltaY = -g_FieldTextBoxes[textBoxIndex].positionOffsetY / openTimer;
    }

    if ((pTalkingActor->flags & 0x200) && !(flags & 1)) {
        g_FieldTextBoxes[textBoxIndex].status = 0;
        return -1;
    }

    return 0;
}
#endif /* XENO_PC_PORT */

void func_8007FFE8(void) {
    int i;

    for (i = 0; i < 4; i++) {
        if (!g_FieldTextBoxes[i].visibility) {
            func_8007F6F8((s16) i); // TODO: Clean up cast w/ function sig
        }
    }
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/dialogue/text_box_render", func_8008004C);
#else
void func_8008004C(void* ot, s32 renderContextIndex) {
    s32 skipIndex;
    s32 orders[4];
    s32 order;
    s32 i;

    D_800ADE98++;
    if ((D_800ADE98 & 3) == 0) {
        D_800ADE94++;
    }
    if (D_800ADE94 >= 5) {
        D_800ADE94 = 0;
    }

    skipIndex = 0xFF;
    for (i = 0; i < 4; i++) {
        if (*(s16*)((u8*)&g_FieldTextBoxes[i] + 0x412) != 0) {
            skipIndex = i;
        }
    }

    for (i = 0; i < 4; i++) {
        orders[i] = 0xFFFF;
    }

    /* Retail .L80080148-.L800802FC: the unordered pass. Runs over every entry
     * with 0x412 set and 0x40E clear, and - unlike the ordered pass below -
     * tests func_80033CD0 BEFORE the func_800345E0/34714/34888 group. */
    for (i = 0; i < 4; i++) {
        u8* pTextBox = (u8*)&g_FieldTextBoxes[i];
        u8* pWindow = pTextBox + 0x18;

        if (*(s16*)(pTextBox + 0x40E) != 0 || *(s16*)(pTextBox + 0x412) == 0) {
            continue;
        }

        *(s16*)(pTextBox + 0x3C4) = -1;
        if (*(s16*)(pTextBox + 0x408) == 0) {
            if (func_80033CD0(pWindow) != 0 && *(s16*)(pTextBox + 0x37C) != 0) {
                *(s16*)(pTextBox + 0x3C4) = 0;
            }

            if ((D_800C2694 & 0x20) != 0) {
                s32 actorIndex = *(s16*)(pTextBox + 0x416);
                ActorData* pActor = (ActorData*)(uintptr_t)g_FieldActors[actorIndex].pActorData;

                *(s16*)(pTextBox + 0x37C) = -1;
                pActor->unk81 = *(u8*)(pTextBox + 0x382) + *(u8*)(pTextBox + 0x37E);
                func_800345E0(pWindow);
            }

            if (*(s16*)(pWindow + 0x82) == 0) {
                func_80034714(pWindow, (void*)(uintptr_t)*(u32*)(pTextBox + 0xA8));
            }

            func_80034888(pWindow, ot, renderContextIndex);
        }

        FieldTextBoxLinkPrim(ot, pTextBox + renderContextIndex * sizeof(DR_MODE));
        func_8007E1C0(ot, renderContextIndex, i);
        func_8007DCF8(i, ot, renderContextIndex);
    }

    for (order = 0; order < 4; order++) {
        for (i = 0; i < 4; i++) {
            u8* pTextBox = (u8*)&g_FieldTextBoxes[i];
            u8* pWindow = pTextBox + 0x18;

            if (*(u16*)(pTextBox + 0x410) != order) {
                continue;
            }

            orders[i] = order;
            if (*(s16*)(pTextBox + 0x40E) != 0 || i == skipIndex) {
                continue;
            }

            *(s16*)(pTextBox + 0x3C4) = -1;
            if (*(s16*)(pTextBox + 0x408) == 0) {
                if ((D_800C2694 & 0x20) != 0 && order == 0) {
                    s32 actorIndex = *(s16*)(pTextBox + 0x416);
                    ActorData* pActor = (ActorData*)(uintptr_t)g_FieldActors[actorIndex].pActorData;

                    *(s16*)(pTextBox + 0x37C) = -1;
                    pActor->unk81 = *(u8*)(pTextBox + 0x382) + *(u8*)(pTextBox + 0x37E);
                    func_800345E0(pWindow);
                }

                if (*(s16*)(pWindow + 0x82) == 0) {
                    /* XENO_PC_PORT: paired 32-bit read of the 0xA8 string pointer
                     * (see the store in func_8007F8DC). */
                    func_80034714(pWindow, (void*)(uintptr_t)*(u32*)(pTextBox + 0xA8));
                }

                func_80034888(pWindow, ot, renderContextIndex);
                if (func_80033CD0(pWindow) != 0) {
                    if (*(s16*)(pTextBox + 0x37C) == 0) {
                        continue;
                    }
                    *(s16*)(pTextBox + 0x3C4) = 0;
                }
            }

            FieldTextBoxLinkPrim(ot, (u8*)pTextBox + renderContextIndex * sizeof(DR_MODE));
            func_8007E1C0(ot, renderContextIndex, i);
            func_8007DCF8(i, ot, renderContextIndex);
        }
    }

    for (i = 0; i < 4; i++) {
        g_FieldTextBoxes[i].order = orders[i];
    }

    FieldTextBoxLinkPrim(ot, D_800B1DF4 + g_FieldCurRenderContextIndex * 0xC0);
}
#endif /* XENO_PC_PORT */

void func_800805F4(void) {
    int i;

    for (i = 0; i < 4; i++) {
        if (!g_FieldTextBoxes[i].visibility) {
            // TODO: _pad[0x8] is probably a short holding some flags
            if (g_FieldTextBoxes[i].windowOpenTimer == 0 && !(g_FieldTextBoxes[i]._pad[0x8] & 4)) {
                func_8007F6F8( (i*0x10000) >> 0x10);
            }
            if (g_FieldTextBoxes[i].status == 0) {
                func_8007F6F8( (i*0x10000) >> 0x10);
            }
            if (g_FieldTextBoxes[i].windowOpenTimer) {
                g_FieldTextBoxes[i].windowOpenTimer--;
            }
        }
    }
}

// Get index of textbox with order 0
int func_800806E4(void) {
    int i;
    
    for (i = 0; i < 4; i++) {
        if (g_FieldTextBoxes[i].order == 0) {
            return i;
        }
    }
    return TEXT_BOX_UNINITIALIZED;
}


// Is textbox free to use?
int func_80080720(void) {
    int i;

    for (i = 0; i < 4; i++) {
        if (g_FieldTextBoxes[i].order == TEXT_BOX_UNINITIALIZED) {
            return 0;
        }
    }
    return -1;
}

// Get index of textbox with lowest order
int func_80080760(void) {
    int i;
    int nResult;
    int lowestOrder;
    
    lowestOrder = 0; // 0 = Top
    nResult = 0xFFFF;
    
    for (i = 0; i < 4; i++) {
        if (g_FieldTextBoxes[i].order != 0xFFFF && g_FieldTextBoxes[i].order >= lowestOrder) {
            lowestOrder = g_FieldTextBoxes[i].order;
            nResult = i;
        }
    }
    return nResult;
}

// Update textbox order
int func_800807B4(void) {
    int i;

    for (i = 0; i < 4; i++) {
        if (g_FieldTextBoxes[i].order != TEXT_BOX_UNINITIALIZED) {
            g_FieldTextBoxes[i].order++;
        }
    }

    for (i = 0; i < 4; i++) {
        if (g_FieldTextBoxes[i].order == TEXT_BOX_UNINITIALIZED) {
            g_FieldTextBoxes[i].order = 0;
            return i;
        }
    }

    return TEXT_BOX_UNINITIALIZED;
}
