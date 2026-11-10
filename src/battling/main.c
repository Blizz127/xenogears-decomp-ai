/* battling main: retail functions in retail order (asm/battling/193A0.s).
 * Functions still listed as INCLUDE_ASM are not byte-matching C yet. The whole
 * file is retail-build only. */
#include "common.h"
#include "system/memory.h"

#ifndef XENO_PC_PORT
void func_8007639C(void *, s32);
void func_8008EF74(s32 arg0, void *arg1);
void func_8008F060(s32 arg0, void *arg1);
#endif

#ifndef XENO_PC_PORT
void func_8008D0A4(void *arg0, void *arg1);

s32 ArchiveDecodeSize();
void ArchiveReadFileToBuffer(s32, s32, s32, s32);
void SetGeomOffset(s32, s32);
void SetGeomScreen(s32);
void func_8002DFF0(s32, u32);
void func_80089210(s32, u32);
void func_80089534(); /* (s16, s16); func_8008976C forwards its s32 arguments unconverted */
extern s16 D_8009285C;
extern s16 D_8009286C;
void *func_80089B44(s32);
void func_80089EB4(void *);
s32 *func_80089F8C(s32 *);
void func_8002CBBC(s32);
extern s16 D_80092800;
extern s16 D_80092804;
extern s16 D_80092808;
extern s16 D_8009280C;
s32 GetTPage(s32, s32, s16, s16);
void func_8002C8CC(void *, s32, s32);
void func_8002CB54(void *, void *, void *);
void func_8002CC54(s32);
void func_8002CC74(s16, s16);
s32 func_800303C8(void *, s32);
void func_800732AC(s32, s32, s32);
extern s32 D_80092810;
void func_80030A30(s32, s32);
void ClearOTagR(s32, s16);
extern s32 D_80050100;
extern u8 D_800928A0;
extern s32 D_800928E4;
extern s32 D_80091C30;
s32 GetRCnt(s32);
extern s32 D_80092820;
void ArchiveCdDataSync(s32);
void func_8008AF6C(s32);
void func_8008C298();
extern s32 D_80092828;
void func_8008BD70(s32, s32, s32, s32);
extern s32 D_80092838;
extern s32 D_8009283C;
extern s32 D_80092840;
extern s32 D_80092844;
extern s32 D_80091F60;
extern s32 D_80091F70;
extern s32 D_80091F80;
extern s32 D_80091F90;
extern s32 D_80091FA0;
void func_800767C8();
void func_80090E10();
s32 func_8008F580(void *);
s32 rand();
extern s32 D_8009284C;
void func_8007639C(void *, s32);
extern u8 D_80092884;
void func_8008F7B8(void *);
s32 func_8008F5B4(void *, s32);
void func_8008FFEC(void *, s32);
s32 func_800891C0(s32 arg0);
void func_800896C4(s32 arg0, u32 arg1);
void func_8008973C(void);
void *func_80089C54(void);
void func_80089CD8(void *arg0);
void func_80089D5C(void *arg0);
void func_80089E2C(void *arg0, s32 arg1);
void func_80089E3C(s32 *arg0);
void func_80089E48(s32 *arg0);
void func_80089E54(void *arg0, s32 arg1);
void func_80089E64(void *arg0, s32 arg1);
void func_80089E74(void);
void func_80089FC4(void);
void func_80089FF8(void *arg0);
void func_8008A0B4(void *arg0, s8 arg1, s8 arg2, s8 arg3);
void func_8008A0F4(void *arg0);
void func_8008A110(s16 arg0, s16 arg1);
void func_8008A128(s16 arg0, s16 arg1);
void func_8008A140(s16 arg0, s16 arg1, s16 arg2, s16 arg3);
void func_8008A168(void);
void func_8008A184(void *arg0, void *arg1);
void func_8008A254(void);
void func_8008A3A0(void);
void func_8008A3A8(void *arg0);
void func_8008A5BC(void *arg0);
void func_8008A618(void);
void func_8008A62C(void);
void func_8008ABAC(void *arg0);
void func_8008AC0C(void *arg0);
void func_8008AC7C(s32 arg0);
void func_8008AC8C(void);
void func_8008B070(s32 arg0);
s16 func_8008B5FC(s32 arg0, s32 arg1, s32 arg2);
void func_8008C0BC(void *arg0, s32 arg1);
void func_8008C120(void *arg0);
void func_8008C2C0(s32 arg0);
void func_8008C2E8(void *arg0);
void func_8008CD54(void);
void func_8008CED4(void);
void func_8008DCA8(s32 arg0);
void func_8008DF30(void);
void func_8008E064(void);
void func_8008E6F8(void *arg0);
void func_8008EF00(s32 arg0, void *arg1);
void func_8008F260(s32 arg0, void *arg1, s8 arg2);
s32 func_8008F4F4(void *arg0, s32 arg1);
s32 func_8008F570(void *arg0);
s32 func_8008F720(void *arg0, s32 arg1);
void func_8008F900(void *arg0);
void func_8008FC7C(void *arg0);
void func_8008FE80(s32 arg0);
s32 func_80090258(void *arg0, s32 arg1);
void func_80090504(void *arg0);
void func_80090C88(void *arg0);

INCLUDE_ASM("asm/battling/nonmatchings/main", BattlingMain);

s32 func_800891C0(s32 arg0) {
    s32 temp_v0;

    temp_v0 = HeapAlloc(ArchiveDecodeSize(), 1);
    ArchiveReadFileToBuffer(arg0, temp_v0, 0, 0);
    return temp_v0;
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_display_set_screen_scale (src/decomp/src/menu/arena_scene_graph_and_opponent.c:206). */
#ifndef XM_T_MATRIX
#define XM_T_MATRIX
typedef struct {
    short m[3][3];
    long t[3];
} MATRIX;
#endif
#ifndef XM_S_arena_display_set_screen_scale
#define XM_S_arena_display_set_screen_scale
void xm_arena_display_set_screen_scale(s32 width, s32 height) __asm__("func_80089210");
#endif
#ifndef XM_S_arena_display_unread_identity
#define XM_S_arena_display_unread_identity
extern MATRIX xm_arena_display_unread_identity __asm__("D_8009A2D8");
#endif
#ifndef XM_S_arena_identity_matrix
#define XM_S_arena_identity_matrix
extern MATRIX xm_arena_identity_matrix __asm__("D_80091C0C");
#endif
#ifndef XM_S_arena_display_screen_scale
#define XM_S_arena_display_screen_scale
extern MATRIX xm_arena_display_screen_scale __asm__("D_80096FE0");
#endif
void xm_arena_display_set_screen_scale(s32 width, s32 height) __asm__("func_80089210");
void xm_arena_display_set_screen_scale(s32 width, s32 height) {
    s32 sx = ((width << 12) / 320) * height / width;
    s32 sy = ((height << 12) / 240) * height / width;

    xm_arena_display_unread_identity = xm_arena_identity_matrix;
    xm_arena_display_screen_scale = xm_arena_identity_matrix;
    xm_arena_display_screen_scale.m[0][0] = sx;
    xm_arena_display_screen_scale.m[1][1] = sy;
}
#endif
INCLUDE_ASM("asm/battling/nonmatchings/main", func_80089330);
void *SetDefDrawEnv();
void func_80045534(void *, void *);
void func_800453E8(void *, s32);
void func_8004546C(void *, s32);
/* D_8009A0D8: 2 x 0xF8-byte draw buffers: DRAWENV at +0 (tpage +0x14,
 * dtd +0x16, dfe +0x17, isbg +0x18, dr_env +0x1C), DR_AREA +0xD0,
 * DR_OFFSET +0xDC. */
extern u8 D_8009A0D8[];

void func_80089534(s32 w, s32 h) {
    u16 tpage;

    SetGeomOffset(w / 2, h / 2);
    SetGeomScreen(0x180);
    func_8002DFF0(w, h);
    if (h > 0x100) {
        SetDefDrawEnv(D_8009A0D8, 0, 0, w, h);
        SetDefDrawEnv(D_8009A0D8 + 0xF8, 0, 0, w, h);
        D_8009A0D8[0xF8 + 0x17] = 0;
        D_8009A0D8[0x17] = 0;
    } else {
        SetDefDrawEnv(D_8009A0D8, 0, 0, w, h);
        SetDefDrawEnv(D_8009A0D8 + 0xF8, 0, 0x100, w, h);
    }
    D_8009A0D8[0x16] = D_8009A0D8[0xF8 + 0x16] = 1;
    D_8009A0D8[0xF8 + 0x18] = 0;
    D_8009A0D8[0x18] = 0;
    tpage = GetTPage(0, 2, 0x280, 0);
    *(u16 *)(D_8009A0D8 + 0xF8 + 0x14) = tpage;
    *(u16 *)(D_8009A0D8 + 0x14) = tpage;
    func_80045534(D_8009A0D8 + 0x1C, D_8009A0D8);
    func_80045534(D_8009A0D8 + 0xF8 + 0x1C, D_8009A0D8 + 0xF8);
    func_800453E8(D_8009A0D8 + 0xD0, (s32)D_8009A0D8);
    func_800453E8(D_8009A0D8 + 0xF8 + 0xD0, (s32)(D_8009A0D8 + 0xF8));
    func_8004546C(D_8009A0D8 + 0xDC, (s32)(D_8009A0D8 + 8));
    func_8004546C(D_8009A0D8 + 0xF8 + 0xDC, (s32)(D_8009A0D8 + 0xF8 + 8));
}


void func_800896C4(s32 arg0, u32 arg1) {
    SetGeomOffset(arg0 / 2, (s32) (arg1 + (arg1 >> 0x1F)) >> 1);
    SetGeomScreen((s32) (arg0 << 8) / arg0);
    func_8002DFF0(arg0, arg1);
    func_80089210(arg0, arg1);
}


void func_8008973C(void) {
    func_80089534(D_8009285C, D_8009286C);
}

void func_8008976C(s32 arg0, s32 arg1) {
    func_80089330();
    func_80089534(arg0, arg1);
}
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_display_set_layer_area (src/decomp/src/menu/arena_scene_graph_and_opponent.c:305). */
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
#ifndef XM_T_RECT
#define XM_T_RECT
typedef struct {
    short x, y;
    short w, h;
} RECT;
#endif
#ifndef XM_S_arena_display_set_layer_area
#define XM_S_arena_display_set_layer_area
void xm_arena_display_set_layer_area(OtPair *layer, s32 x, s32 y, s32 w, s32 h, s32 second) __asm__("func_800897AC");
#endif
#ifndef XM_S_SetDrawArea
#define XM_S_SetDrawArea
void xm_SetDrawArea(DR_AREA *p, RECT *r) __asm__("func_800453E8");
#endif
#ifndef XM_S_SetDrawOffset
#define XM_S_SetDrawOffset
void xm_SetDrawOffset(DR_OFFSET *p, u_short *ofs) __asm__("func_8004546C");
#endif
void func_800897AC(OtPair *layer, s32 x, s32 y, s32 w, s32 h, s32 second) {
    RECT area;
    s16 offset[2];

    area.x = x;
    area.y = y;
    area.w = w;
    area.h = h;
    xm_SetDrawArea(&layer->area[0], &area);
    area.y = y + second;
    xm_SetDrawArea(&layer->area[1], &area);
    offset[0] = x;
    offset[1] = y;
    xm_SetDrawOffset(&layer->offset[0], offset);
    offset[1] = y + second;
    xm_SetDrawOffset(&layer->offset[1], offset);
    layer->tile[0].len = 3;
    layer->tile[0].rgbc = 0x60000000;
    layer->tile[0].x0 = 0;
    layer->tile[0].y0 = 0;
    layer->tile[0].w = w;
    layer->tile[0].h = h;
    layer->tile[1] = layer->tile[0];
    layer->flags |= 0xC;
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_look_at_build_matrix (src/decomp/src/menu/arena_scene_graph_and_opponent.c:332). */
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
#ifndef XM_S_arena_look_at_build_matrix
#define XM_S_arena_look_at_build_matrix
void xm_arena_look_at_build_matrix(MATRIX *m, SVECTOR *eye, SVECTOR *at, SVECTOR *up) __asm__("func_800898BC");
#endif
#ifndef XM_S_arena_look_at_forward
#define XM_S_arena_look_at_forward
extern VECTOR xm_arena_look_at_forward __asm__("D_8009A0C8");
#endif
#ifndef XM_S_arena_look_at_axis_y
#define XM_S_arena_look_at_axis_y
extern VECTOR xm_arena_look_at_axis_y __asm__("D_8009A918");
#endif
#ifndef XM_S_VectorNormal
#define XM_S_VectorNormal
long xm_VectorNormal(VECTOR *v0, VECTOR *v1) __asm__("VectorNormal");
#endif
#ifndef XM_S_arena_look_at_axis_z
#define XM_S_arena_look_at_axis_z
extern VECTOR xm_arena_look_at_axis_z __asm__("D_80096F98");
#endif
#ifndef XM_S_OuterProduct12
#define XM_S_OuterProduct12
void xm_OuterProduct12(VECTOR *v0, VECTOR *v1, VECTOR *v2) __asm__("OuterProduct12");
#endif
#ifndef XM_S_arena_look_at_axis_x
#define XM_S_arena_look_at_axis_x
extern VECTOR xm_arena_look_at_axis_x __asm__("D_80097000");
#endif
#ifndef XM_S_ApplyMatrix
#define XM_S_ApplyMatrix
VECTOR *xm_ApplyMatrix(MATRIX *m, SVECTOR *v0, VECTOR *v1) __asm__("ApplyMatrix");
#endif
#ifndef XM_S_MulMatrix2
#define XM_S_MulMatrix2
MATRIX *xm_MulMatrix2(MATRIX *m0, MATRIX *m1) __asm__("MulMatrix2");
#endif
void func_800898BC(MATRIX *m, SVECTOR *eye, SVECTOR *at, SVECTOR *up) {
    xm_arena_look_at_forward.vx = at->vx - eye->vx;
    xm_arena_look_at_forward.vy = at->vy - eye->vy;
    xm_arena_look_at_forward.vz = at->vz - eye->vz;
    xm_arena_look_at_axis_y.vx = up->vx;
    xm_arena_look_at_axis_y.vy = up->vy;
    xm_arena_look_at_axis_y.vz = up->vz;
    xm_VectorNormal(&xm_arena_look_at_forward, &xm_arena_look_at_axis_z);
    xm_OuterProduct12(&xm_arena_look_at_axis_y, &xm_arena_look_at_axis_z, &xm_arena_look_at_forward);
    xm_VectorNormal(&xm_arena_look_at_forward, &xm_arena_look_at_axis_x);
    xm_OuterProduct12(&xm_arena_look_at_axis_z, &xm_arena_look_at_axis_x, &xm_arena_look_at_forward);
    xm_VectorNormal(&xm_arena_look_at_forward, &xm_arena_look_at_axis_y);
    m->m[0][0] = xm_arena_look_at_axis_x.vx;
    m->m[0][1] = xm_arena_look_at_axis_x.vy;
    m->m[0][2] = xm_arena_look_at_axis_x.vz;
    m->m[1][0] = xm_arena_look_at_axis_y.vx;
    m->m[1][1] = xm_arena_look_at_axis_y.vy;
    m->m[1][2] = xm_arena_look_at_axis_y.vz;
    m->m[2][0] = xm_arena_look_at_axis_z.vx;
    m->m[2][1] = xm_arena_look_at_axis_z.vy;
    m->m[2][2] = xm_arena_look_at_axis_z.vz;
    xm_ApplyMatrix(m, eye, &xm_arena_look_at_forward);
    xm_MulMatrix2(&xm_arena_display_screen_scale, m);
    m->t[0] = -xm_arena_look_at_forward.vx;
    m->t[1] = -xm_arena_look_at_forward.vy;
    m->t[2] = -xm_arena_look_at_forward.vz;
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_node_aim_rig_camera (src/decomp/src/menu/arena_scene_graph_and_opponent.c:363). */
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
#ifndef XM_S_arena_node_aim_rig_camera
#define XM_S_arena_node_aim_rig_camera
void xm_arena_node_aim_rig_camera(LightRig *view, VECTOR *position, VECTOR *focus) __asm__("func_80089A98");
#endif
#ifndef XM_S_arena_view_origin
#define XM_S_arena_view_origin
extern VECTOR xm_arena_view_origin __asm__("D_80096FA8");
#endif
void func_80089A98(LightRig *view, VECTOR *position, VECTOR *focus) {
    SVECTOR up;
    SVECTOR from;
    SVECTOR origin;

    up.vy = 0x1000;
    up.vz = 0;
    up.vx = 0;
    xm_arena_view_origin = *focus;
    from.vx = position->vx - focus->vx;
    from.vy = position->vy - focus->vy;
    from.vz = position->vz - focus->vz;
    origin.vz = 0;
    origin.vx = 0;
    origin.vy = 0;
    xm_arena_look_at_build_matrix(&view->camera->view, &from, &origin, &up);
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_node_reset (src/decomp/src/menu/arena_scene_graph_and_opponent.c:383). */
#ifndef XM_S_arena_node_reset
#define XM_S_arena_node_reset
Node *xm_arena_node_reset(Node *node) __asm__("func_80089B44");
#endif
Node *xm_arena_node_reset(Node *node) __asm__("func_80089B44");
Node *xm_arena_node_reset(Node *node) {
    node->type = 0;
    node->parent = NULL;
    node->child = NULL;
    node->next = NULL;
    node->callback = NULL;
    node->data = NULL;
    node->position.vz = 0;
    node->position.vy = 0;
    node->position.vx = 0;
    node->angles.vz = 0;
    node->angles.vy = 0;
    node->angles.vx = 0;
    node->offset.vz = 0;
    node->offset.vy = 0;
    node->offset.vx = 0;
    node->unk6C = xm_arena_identity_matrix;
    node->unk4C = node->unk6C;
    node->view = node->unk4C;
    return node;
}
#endif

void *func_80089C54(void) {
    HeapSetCurrentContentType(8);
    return func_80089B44(HeapAlloc(0x9C, 0));
}

void func_80089C88(void *parent, void *child) {
    void *p;

    *(void **)((u8 *)child + 0x8C) = parent;
    if (*(void **)((u8 *)parent + 0x94) == NULL) {
        *(void **)((u8 *)parent + 0x94) = child;
        return;
    }
    p = *(void **)((u8 *)parent + 0x94);
    while (*(void **)((u8 *)p + 0x90) != NULL) {
        p = *(void **)((u8 *)p + 0x90);
    }
    *(void **)((u8 *)p + 0x90) = child;
}


void func_80089CD8(void *arg0) {
    void *temp_v0;
    void *temp_v1;
    void *var_v1;

    if (arg0 != NULL) {
        temp_v1 = *(void **)((s8*)(arg0) + 0x8C);
        if (temp_v1 != NULL) {
            *(void **)((s8*)(arg0) + 0x8C) = NULL;
            temp_v0 = *(void **)((s8*)(temp_v1) + 0x94);
            if (temp_v0 != NULL) {
                if (temp_v0 == arg0) {
                    *(void **)((s8*)(temp_v1) + 0x94) = (void *) *(void **)((s8*)(arg0) + 0x90);
                } else {
                    var_v1 = temp_v0;
                    if (*(s32 *)((s8*)(var_v1) + 0x90) != arg0) {
                        do {
                            var_v1 = *(void **)((s8*)(var_v1) + 0x90);
                        } while (*(s32 *)((s8*)(var_v1) + 0x90) != arg0);
                    }
                    *(void **)((s8*)(var_v1) + 0x90) = (void *) *(void **)((s8*)(arg0) + 0x90);
                }
                *(void **)((s8*)(arg0) + 0x90) = NULL;
            }
        }
    }
}


void func_80089D5C(void *arg0) {
    s32 temp_v1;
    void *temp_a0;

    if (arg0 != NULL) {
        func_80089D5C(*(void **)((s8*)(arg0) + 0x94));
        func_80089D5C(*(void **)((s8*)(arg0) + 0x90));
        temp_v1 = *(s32 *)((s8*)(arg0) + 0);
        switch (temp_v1) {                          /* irregular */
        case 1:
            func_80089FF8(*(void **)((s8*)(arg0) + 4));
            break;
        case 2:
            func_80089EB4(*(void **)((s8*)(arg0) + 4));
            break;
        case 5:
            func_8008C120(*(void **)((s8*)(arg0) + 4));
            break;
        }
        temp_a0 = *(void **)((s8*)(arg0) + 4);
        if (temp_a0 != NULL) {
            HeapFree(temp_a0);
        }
        HeapFree(arg0);
    }
}


void func_80089E2C(void *arg0, s32 arg1) {
    *(s32 *)((s8*)(arg0) + 4) = arg1;
    *(s32 *)((s8*)(arg0) + 0) = 1;
}


void func_80089E3C(s32 *arg0) {
    *arg0 = 3;
}


void func_80089E48(s32 *arg0) {
    *arg0 = 4;
}


void func_80089E54(void *arg0, s32 arg1) {
    *(s32 *)((s8*)(arg0) + 4) = arg1;
    *(s32 *)((s8*)(arg0) + 0) = 2;
}


void func_80089E64(void *arg0, s32 arg1) {
    *(s32 *)((s8*)(arg0) + 4) = arg1;
    *(s32 *)((s8*)(arg0) + 0) = 6;
}


void func_80089E74(void) {
    void *temp_v0;

    HeapSetCurrentContentType(4);
    temp_v0 = HeapAlloc(0x1C, 0);
    *(s16 *)((s8*)(temp_v0) + 0x14) = 0x1000;
    *(s16 *)((s8*)(temp_v0) + 0x12) = 0x1000;
    *(s16 *)((s8*)(temp_v0) + 0x10) = 0x1000;
    *(s32 *)((s8*)(temp_v0) + 4) = 0;
    *(s32 *)((s8*)(temp_v0) + 8) = 0;
}

extern s32 D_80091C2C;

void func_80089EB4(void *arg0) {
    s32 i;
    u8 *e;

    if (*(void **)((u8 *)arg0 + 4) != NULL) {
        HeapFree(*(void **)((u8 *)arg0 + 4));
    }
    if (*(void **)((u8 *)arg0 + 8) != NULL) {
        if (D_80091C2C == 0) {
            i = *(s16 *)((u8 *)arg0 + 2);
            while (--i != -1) {
                e = (u8 *)(i * 0x14 + *(s32 *)((u8 *)arg0 + 8));
                if (*(s32 *)e != 0) {
                    HeapFree(*(void **)(e + 4));
                }
            }
        }
        HeapFree(*(void **)((u8 *)arg0 + 8));
    }
}

s32 *func_80089F8C(s32 *arg0) {
    arg0[1] = 1;
    arg0[0] = 0;
    arg0[2] = 0;
    arg0[3] = 0;
    arg0[4] = 0;
    arg0[5] = 0;
    arg0[7] = 0;
    ((u8 *)arg0)[0x1A] = 0x40;
    ((u8 *)arg0)[0x19] = 0x40;
    ((u8 *)arg0)[0x18] = 0x40;
    return arg0;
}

void func_80089FC4(void) {
    HeapSetCurrentContentType(1);
    func_80089F8C(HeapAlloc(0x20, 0));
}


void func_80089FF8(void *arg0) {
    s32 temp_a0;

    temp_a0 = *(s32 *)((s8*)(arg0) + 8);
    if (temp_a0 != 0) {
        HeapDelayedFree(temp_a0, 2);
    }
    func_8002CBBC(*(s32 *)((s8*)(arg0) + 0x14));
}

void func_8002DDE4(s32, s32, s32, s32, s32, s32, s32);

void func_8008A040(s32 arg0) {
    if (D_80092800 > 0) {
        func_8002DDE4(arg0, 1, D_80092800, D_80092804, 1, D_80092808, D_8009280C);
    } else {
        func_8002DDE4(arg0, 0, 0, 0, 0, 0, 0);
    }
}


void func_8008A0B4(void *arg0, s8 arg1, s8 arg2, s8 arg3) {
    void *temp_v1;

    *(s8 *)((s8*)(*(void **)((s8*)(arg0) + 4)) + 0x18) = arg1;
    *(s8 *)((s8*)(*(void **)((s8*)(arg0) + 4)) + 0x19) = arg2;
    *(s8 *)((s8*)(*(void **)((s8*)(arg0) + 4)) + 0x1A) = arg3;
    temp_v1 = *(void **)((s8*)(arg0) + 4);
    *(s32 *)((s8*)(temp_v1) + 0) = (s32) (*(s32 *)((s8*)(temp_v1) + 0) | 0x10);
}


void func_8008A0F4(void *arg0) {
    s32 *temp_v0;

    temp_v0 = *(s32 **)((s8*)(arg0) + 4);
    *temp_v0 &= ~0x10;
}


void func_8008A110(s16 arg0, s16 arg1) {
    D_80092800 = arg0;
    D_80092804 = arg1;
}


void func_8008A128(s16 arg0, s16 arg1) {
    D_80092808 = arg0;
    D_8009280C = arg1;
}


void func_8008A140(s16 arg0, s16 arg1, s16 arg2, s16 arg3) {
    D_80092800 = arg0;
    D_80092804 = arg1;
    D_80092808 = arg2;
    D_8009280C = arg3;
}


void func_8008A168(void) {
    D_80092808 = -1;
    D_80092800 = -1;
}


void func_8008A184(void *arg0, void *arg1) {
    *(void **)((s8*)(arg0) + 0x14) = arg1;
    *(s32 *)((s8*)(arg0) + 0x10) = func_800303C8(arg1, 1);
    func_8002CB54(*(void **)((s8*)(arg0) + 0x14), arg0 + 8, arg0 + 0xC);
    if (D_80092800 >= 0) {
        func_8002CC54(GetTPage(0, 1, D_80092800, D_80092804) & 0xFFFF);
    }
    if (D_80092808 >= 0) {
        func_8002CC74(D_80092808, D_8009280C);
    }
    func_8002C8CC(*(void **)((s8*)(arg0) + 0x14), *(s32 *)((s8*)(arg0) + 8), 2);
    func_800732AC(*(s32 *)((s8*)(arg0) + 0xC), *(s32 *)((s8*)(arg0) + 8), *(s32 *)((s8*)(*(void **)((s8*)(arg0) + 0x14)) + 0x34));
    *(s32 *)((s8*)(arg0) + 0) = (s32) (*(s32 *)((s8*)(arg0) + 0) | 2);
}


void func_8008A254(void) {
    void *temp_v0;

    HeapSetCurrentContentType(0xB);
    temp_v0 = HeapAlloc(0x14, 0);
    *(s32 *)((s8*)(temp_v0) + 8) = 0x10;
    *(s32 *)((s8*)(temp_v0) + 4) = 0x10;
    *(s32 *)((s8*)(temp_v0) + 0) = 0x10;
    *(s16 *)((s8*)(temp_v0) + 0x10) = 0;
    *(s16 *)((s8*)(temp_v0) + 0xE) = 0;
    *(s16 *)((s8*)(temp_v0) + 0xC) = 0;
}

void func_8008A298(s32 arg0) {
    HeapFree(arg0);
}
void *func_8008A2B8(u16 count) {
    u8 *obj;
    u8 *buf;
    s32 i;

    HeapSetCurrentContentType(0xC);
    obj = HeapAlloc(0x68, 0);
    HeapSetCurrentContentType(0xC);
    buf = HeapAlloc(count * 8, 0);
    *(u8 **)(obj + 4) = buf;
    *(u8 **)(obj + 8) = buf + count * 4;
    obj[0x16] = 1;
    obj[0x17] = 0xE;
    *(s32 *)obj = 0;
    *(s16 *)(obj + 0x14) = count;
    *(u8 **)(obj + 0xC) = *(u8 **)(obj + 4) + (count * 4 - 4);
    *(u8 **)(obj + 0x10) = *(u8 **)(obj + 8) + (count * 4 - 4);
    for (i = 1; i != count;) {
        i *= 2;
        if (i > 0x4000) {
            obj[0x17] = 0xE;
            break;
        }
        obj[0x17]--;
    }
    return obj;
}


void func_8008A3A0(void) {

}


void func_8008A3A8(void *arg0) {
    HeapDelayedFree(*(s32 *)((s8*)(arg0) + 4), 3);
    HeapFree(arg0);
}

/* func_8008A254 allocates and returns this 0x14-byte block; the TU
 * prototypes it as void, so the call goes through a returning alias. */
typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s16 unkC;
    s16 unkE;
    s16 unk10;
    s16 pad12;
} BattlingXform;
BattlingXform *func_8008A254__ret(void) asm("func_8008A254");

void *func_8008A3E0(s32 arg0) {
    u8 *obj;
    u8 *n0;
    u8 *n1;
    u8 *n2;
    u8 *n3;

    HeapSetCurrentContentType(9);
    obj = HeapAlloc(0x28C, 0);
    n0 = obj + 0x14;
    n1 = obj + 0xB0;
    n2 = obj + 0x14C;
    n3 = obj + 0x1E8;
    *(s32 *)(obj + 0) = 0;
    *(u8 **)(obj + 4) = n0;
    *(u8 **)(obj + 8) = n1;
    *(u8 **)(obj + 0xC) = n2;
    *(u8 **)(obj + 0x10) = n3;
    func_80089B44(n0);
    func_80089B44(n1);
    func_80089B44(n2);
    func_80089B44(n3);
    func_80089E64(*(void **)(obj + 8), func_8008A254__ret());
    func_80089E64(*(void **)(obj + 0xC), func_8008A254__ret());
    func_80089E64(*(void **)(obj + 0x10), func_8008A254__ret());
    *(s32 *)(*(u8 **)(obj + 8) + 0x38) = *(s32 *)(*(u8 **)(obj + 0xC) + 0x38) = *(s32 *)(*(u8 **)(obj + 0x10) + 0x38) = -2;
    *(s32 *)(*(u8 **)(obj + 8) + 0x34) = 0;
    *(s32 *)(*(u8 **)(obj + 8) + 0x3C) = 1;
    *(s32 *)(*(u8 **)(obj + 0xC) + 0x34) = -1;
    *(s32 *)(*(u8 **)(obj + 0xC) + 0x3C) = -1;
    *(s32 *)(*(u8 **)(obj + 0x10) + 0x34) = 1;
    *(s32 *)(*(u8 **)(obj + 0x10) + 0x3C) = -1;
    (*(BattlingXform **)(*(u8 **)(obj + 8) + 4))->unkC = (*(BattlingXform **)(*(u8 **)(obj + 8) + 4))->unkE = (*(BattlingXform **)(*(u8 **)(obj + 8) + 4))->unk10 = 0x800;
    (*(BattlingXform **)(*(u8 **)(obj + 0xC) + 4))->unkC = (*(BattlingXform **)(*(u8 **)(obj + 0xC) + 4))->unkE = (*(BattlingXform **)(*(u8 **)(obj + 0xC) + 4))->unk10 = 0;
    **(BattlingXform **)(*(u8 **)(obj + 0x10) + 4) = **(BattlingXform **)(*(u8 **)(obj + 0xC) + 4);
    *(s32 *)(obj + 0x284) = arg0;
    obj[0x28A] = 0;
    obj[0x289] = 0;
    obj[0x288] = 0;
    obj[0x28A] = 0x10;
    obj[0x289] = 0x10;
    obj[0x288] = 0x10;
    func_8008ABAC(obj + 8);
    return obj;
}


void func_8008A5BC(void *arg0) {
    HeapFree(*(void **)((s8*)(arg0) + 0xB4));
    HeapFree(*(void **)((s8*)(arg0) + 0x150));
    HeapFree(*(void **)((s8*)(arg0) + 0x1EC));
    func_8008A3A8(*(s32 *)((s8*)(arg0) + 0x284));
    HeapFree(arg0);
}


void func_8008A618(void) {
    D_80092810 = 1;
}


void func_8008A62C(void) {
    D_80092810 = 0;
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_node_draw_model (src/decomp/src/menu/arena_scene_graph_and_opponent.c:753). */
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
#ifndef XM_T_RenderPacket
#define XM_T_RenderPacket
typedef struct RenderPacket {
    u8 unk0[3];
    u8 code;
    s32 value;
} RenderPacket;
#endif
#ifndef XM_S_arena_node_draw_model
#define XM_S_arena_node_draw_model
void xm_arena_node_draw_model(NodeModel *model) __asm__("func_8008A63C");
#endif
#ifndef XM_S_model_box_test_mode
#define XM_S_model_box_test_mode
extern s32 xm_model_box_test_mode __asm__("D_80050104");
#endif
#ifndef XM_S_arena_node_color_overrides_on
#define XM_S_arena_node_color_overrides_on
extern s32 xm_arena_node_color_overrides_on __asm__("D_80092810");
#endif
#ifndef XM_S_model_draw_sprite_model
#define XM_S_model_draw_sprite_model
s32 xm_model_draw_sprite_model(SpriteModel *model, RenderPacket *packets, u32 *ot, s32 mode) __asm__("func_8002C700");
#endif
#ifndef XM_S_arena_draw_buffer_index
#define XM_S_arena_draw_buffer_index
extern u8 xm_arena_draw_buffer_index __asm__("D_800928A0");
#endif
#ifndef XM_S_arena_current_layer_ot
#define XM_S_arena_current_layer_ot
extern u32 *xm_arena_current_layer_ot __asm__("D_800928E4");
#endif
void func_8008A63C(NodeModel *model) {
    xm_model_box_test_mode = 0;
    if (xm_arena_node_color_overrides_on) {
        if (model->flags & 0x10) {
            gte_SetBackColor(model->r, model->g, model->b);
        } else {
            gte_SetBackColor(0x40, 0x40, 0x40);
        }
    }
    xm_model_draw_sprite_model(model->file, model->packets[xm_arena_draw_buffer_index], xm_arena_current_layer_ot, model->unk4);
}
#endif
/* CVECTOR layout (libgte): r, g, b, cd; byte-aligned, so copies use lwl/lwr. */
typedef struct {
    u8 r;
    u8 g;
    u8 b;
    u8 cd;
} BattlingCVec;
/* Same declaration as func_8008A78C's: 2 x 4-byte colours, one per buffer. */
extern s32 D_80092818[];
extern s32 D_80092914;

void func_8008A6F8(BattlingCVec *col) {
    BattlingCVec *cur;

    cur = (BattlingCVec *)&D_80092818[D_800928A0];
    if (col->r == cur->r && col->g == cur->g && col->b == cur->b) {
        D_80092914 = 0;
    } else {
        D_80092914 = 1;
        *cur = *col;
        cur->cd = 0x20;
    }
}

extern s32 D_80092818[];

void func_8008A78C(void *arg0) {
    u8 *model;
    s32 idx;
    s32 n;
    u8 *prim;
    s32 val;

    model = *(u8 **)(*(u8 **)((u8 *)arg0 + 4) + 0xC);
    idx = D_800928A0 * 4;
    prim = *(u8 **)(idx + (s32)model + 0x10);
    n = *(s16 *)(model + 2);
    val = *(s32 *)((u8 *)D_80092818 + idx);
    while (--n != -1) {
        *(s32 *)(prim + 4) = val;
        prim += 0x14;
    }
}

INCLUDE_ASM("asm/battling/nonmatchings/main", func_8008A7E0);

void func_8008ABAC(void *arg0) {
    func_80030A30(0, *(s32 *)((s8*)(*(void **)((s8*)(arg0) + 0)) + 4));
    func_80030A30(1, *(s32 *)((s8*)(*(void **)((s8*)(arg0) + 4)) + 4));
    func_80030A30(2, *(s32 *)((s8*)(*(void **)((s8*)(arg0) + 8)) + 4));
}


void func_8008AC0C(void *arg0) {
    ClearOTagR(*(s32 *)((s8*)(((D_800928A0 * 4) + arg0)) + 4), *(s16 *)((s8*)(arg0) + 0x14));
    D_800928E4 = *(s32 *)((s8*)(((D_800928A0 * 4) + arg0)) + 4);
    D_80050100 = (s32) *(u8 *)((s8*)(arg0) + 0x17);
}


void func_8008AC7C(s32 arg0) {
    D_80091C30 = arg0;
}


void func_8008AC8C(void) {
    D_80092820 = GetRCnt(0xF2000001);
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_display_compact_layer (src/decomp/src/menu/arena_scene_graph_and_opponent.c:881). */
#ifndef XM_T_TAG_LEN
#define XM_T_TAG_LEN
#define TAG_LEN(tag) (((u8 *)(tag))[3])
#endif
#ifndef XM_S_arena_display_compact_layer
#define XM_S_arena_display_compact_layer
void xm_arena_display_compact_layer(s32 frames) __asm__("func_8008ACB8");
#endif
#ifndef XM_S_arena_display_layer_to_compact
#define XM_S_arena_display_layer_to_compact
extern OtPair *xm_arena_display_layer_to_compact __asm__("D_80091C30");
#endif
#ifndef XM_S_arena_display_frame_start_time
#define XM_S_arena_display_frame_start_time
extern s32 xm_arena_display_frame_start_time __asm__("D_80092820");
#endif
#ifndef XM_S_GetRCnt
#define XM_S_GetRCnt
long xm_GetRCnt(unsigned long spec) __asm__("GetRCnt");
#endif
void func_8008ACB8(s32 frames) {
    OtPair *pair = xm_arena_display_layer_to_compact;
    s32 start;
    s32 limit;
    s32 elapsed;
    s32 i;
    u32 *entry;
    u32 *tag;

    if (pair == NULL) {
        return;
    }
    start = xm_arena_display_frame_start_time;
    xm_arena_display_layer_to_compact = NULL;
    if (frames != 0) {
        limit = frames * 240;
    } else {
        limit = 0xC0;
    }
    for (i = pair->length - 1; i >= 4; i--) {
        elapsed = xm_GetRCnt(0xF2000001) - start;
        if (elapsed < 0) {
            elapsed += 0x10000;
        }
        if (elapsed > limit) {
            return;
        }
        entry = (u32 *)pair->ot[xm_arena_draw_buffer_index][i];
        tag = (u32 *)((*entry & 0xFFFFFF) - 0x80000000);
        if (TAG_LEN(tag) == 0) {
            while (i >= 5) {
                tag = (u32 *)((*tag & 0xFFFFFF) - 0x80000000);
                i--;
                if (TAG_LEN(tag) != 0) {
                    break;
                }
            }
            *entry = (*entry & 0xFF000000) | ((u32)tag & 0xFFFFFF);
        }
    }
}
#endif
void AddPrim(void *ot, void *p);
void AddPrims(void *ot, void *p0, void *p1);
void func_800453E8(void *, s32);
void func_8004546C(void *, s32);
extern s32 D_80092868;
extern s32 D_80092938;

void func_8008AE1C(void *arg0) {
    void *temp_v0;

    func_8008AC7C((s32)arg0);
    temp_v0 = (void *)((D_800928A0 * 4) + (s32)arg0);
    AddPrims((void *)D_80092938, *(void **)((s8*)(temp_v0) + 0xC), *(void **)((s8*)(temp_v0) + 4));
    if (!(*(u8 *)((s8*)(arg0) + 0x16) & 4)) {
        func_800453E8(arg0 + ((D_800928A0 * 0xC) + 0x18), D_80092868);
    }
    if (!(*(u8 *)((s8*)(arg0) + 0x16) & 8)) {
        func_8004546C(arg0 + ((D_800928A0 * 0xC) + 0x30), D_80092868 + 8);
    }
    if (*(u8 *)((s8*)(arg0) + 0x16) & 0x10) {
        AddPrim((void *)D_80092938, arg0 + ((D_800928A0 * 0x10) + 0x48));
    }
    AddPrim((void *)D_80092938, arg0 + ((D_800928A0 * 0xC) + 0x30));
    AddPrim((void *)D_80092938, arg0 + ((D_800928A0 * 0xC) + 0x18));
}

/* Retail returns arg0 (v0 = s0 at the end), but the TU prototypes it as
 * `void func_8008AF6C(s32)`. The definition is given a C name of its own
 * bound to the retail symbol; alternatively change that TU prototype to
 * `void *func_8008AF6C(u8 *)` and define it under its own name. */
void *func_8008AF6C__ret(u8 *arg0) asm("func_8008AF6C");

/* Relocates a loaded block: every stored pointer is rebased by the distance
 * between its old load address (+0x1C) and arg0. */
void *func_8008AF6C__ret(u8 *arg0) {
    s32 delta;
    s32 sub;
    s32 *table;
    u32 i;
    s32 *p;

    delta = arg0 - *(u8 **)(arg0 + 0x1C);
    sub = *(s32 *)(arg0 + 0xC);
    *(u8 **)(arg0 + 0x1C) = arg0;
    *(s32 *)(arg0 + 0) += delta;
    *(s32 *)(arg0 + 4) += delta;
    *(s32 *)(arg0 + 0x10) += delta;
    *(s32 *)(arg0 + 0x14) += delta;
    *(s32 *)(arg0 + 0x18) += delta;
    *(s32 *)(arg0 + 0x20) += delta;
    *(s32 *)(arg0 + 0x24) += delta;
    if (sub != 0) {
        *(s32 *)(arg0 + 0xC) = sub + delta;
        func_8008A040(sub + delta);
    }
    table = *(s32 **)(arg0 + 8);
    if (table != NULL) {
        *(s32 **)(arg0 + 8) = (s32 *)((u8 *)table + delta);
        for (i = 1; i < **(s32 **)(arg0 + 8) + 1; i++) {
            p = (s32 *)(i * 4 + *(s32 *)(arg0 + 8));
            if (*p != 0) {
                *p += delta;
            }
        }
    }
    return arg0;
}


void func_8008B070(s32 arg0) {
    s32 temp_v0;

    HeapSetCurrentContentType(0xA);
    temp_v0 = HeapAlloc(ArchiveDecodeSize(arg0), 0);
    ArchiveReadFileToBuffer(arg0, temp_v0, 0, 0);
    ArchiveCdDataSync(0);
    func_8008AF6C(temp_v0);
}

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s16 unkC;
    s16 unkE;
    s16 unk10;
    s16 unk12;
} BattlingAnimEntry;

void func_8008B0D8(void *arg0) {
    s32 i;
    BattlingAnimEntry *e;

    *(s16 *)((u8 *)arg0 + 0x10) = 0;
    *(s16 *)((u8 *)arg0 + 0x12) = 0;
    e = *(BattlingAnimEntry **)((u8 *)arg0 + 8);
    for (i = 0; i < *(s16 *)(*(u8 **)arg0 + 6); i++) {
        e->unkC = 0;
        e->unkE = 0;
        e->unk4 = e->unk0;
        e->unk10 = 0;
        e++;
    }
}

INCLUDE_ASM("asm/battling/nonmatchings/main", func_8008B13C);
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_node_build_model_set (src/decomp/src/menu/arena_scene_graph_and_opponent.c:1093). */
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
#ifndef XM_T_MoveSlot
#define XM_T_MoveSlot
typedef struct MoveSlot {
    u8 unk0[0x3];
    u8 usable;
} MoveSlot;
#endif
#ifndef XM_T_ModelFile
#define XM_T_ModelFile
typedef struct ModelFile {
    u32 *hierarchy;    
    u8 *models;        
    u32 *animations;   
    u8 *target;        
    struct SceneHeader *header; 
    s32 unk14;
    struct MoveSlot *slots; 
    u8 *base;          
    u8 *image;         
    u8 *unk24;
} ModelFile;
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
#ifndef XM_S_arena_node_build_model_set
#define XM_S_arena_node_build_model_set
Node *xm_arena_node_build_model_set(ModelFile *file) __asm__("func_8008B38C");
#endif
#ifndef XM_S_model_relocate_group
#define XM_S_model_relocate_group
s32 xm_model_relocate_group(ModelGroup *group) __asm__("func_8002C3E8");
#endif
#ifndef XM_S_heap_set_next_class
#define XM_S_heap_set_next_class
void xm_heap_set_next_class(s32 kind) __asm__("HeapSetCurrentContentType");
#endif
#ifndef XM_S_heap_alloc
#define XM_S_heap_alloc
void *xm_heap_alloc(s32 size, s32 mode) __asm__("HeapAlloc");
#endif
#ifndef XM_S_arena_node_alloc_model_set
#define XM_S_arena_node_alloc_model_set
ModelSet *xm_arena_node_alloc_model_set(void) __asm__("func_80089E74");
#endif
#ifndef XM_S_arena_node_alloc
#define XM_S_arena_node_alloc
Node *xm_arena_node_alloc(void) __asm__("func_80089C54");
#endif
#ifndef XM_S_arena_node_set_model_set
#define XM_S_arena_node_set_model_set
void xm_arena_node_set_model_set(Node *node, ModelSet *set) __asm__("func_80089E54");
#endif
#ifndef XM_S_arena_node_alloc_model
#define XM_S_arena_node_alloc_model
NodeModel *xm_arena_node_alloc_model(void) __asm__("func_80089FC4");
#endif
#ifndef XM_S_arena_node_set_model
#define XM_S_arena_node_set_model
void xm_arena_node_set_model(Node *node, NodeModel *model) __asm__("func_80089E2C");
#endif
#ifndef XM_S_arena_node_init_model
#define XM_S_arena_node_init_model
void xm_arena_node_init_model(NodeModel *model, SpriteModel *file) __asm__("func_8008A184");
#endif
#ifndef XM_S_arena_node_add_child
#define XM_S_arena_node_add_child
void xm_arena_node_add_child(Node *parent, Node *child) __asm__("func_80089C88");
#endif
#ifndef XM_S_arena_node_bind_animation
#define XM_S_arena_node_bind_animation
void xm_arena_node_bind_animation(AnimRecord *record, Player *player, Node *root) __asm__("func_8008B13C");
#endif
Node *func_8008B38C(ModelFile *file) {
    Node *root;
    u32 i;
    u32 *data = (u32 *)file->models;
    u32 *hierarchy = file->hierarchy;
    u32 *animations = file->animations;
    u32 count = hierarchy[0];
    HierarchyRecord *records = (HierarchyRecord *)(hierarchy + 1);
    Node **nodes;
    ModelSet *set;
    Node *node;
    NodeModel *model;
    Player *player;

    xm_model_relocate_group((ModelGroup *)data);
    xm_heap_set_next_class(0x12);
    nodes = xm_heap_alloc(count * 4, 0);
    set = xm_arena_node_alloc_model_set();
    root = xm_arena_node_alloc();
    xm_arena_node_set_model_set(root, set);
    set->nodes = nodes;
    set->nodeCount = count;
    set->records = records;
    for (i = 0; i < count; i++) {
        node = xm_arena_node_alloc();
        nodes[i] = node;
        if (records[i].model != -1) {
            model = xm_arena_node_alloc_model();
            xm_arena_node_set_model(node, model);
            xm_arena_node_init_model(model, (SpriteModel *)((u8 *)data + (records[i].model * 0x38 + 0x10)));
        }
        if (records[i].parent == -1) {
            xm_arena_node_add_child(root, node);
        } else {
            xm_arena_node_add_child(nodes[records[i].parent], node);
        }
        node->angles.vx = records[i].angle.vx;
        node->angles.vy = records[i].angle.vy;
        node->angles.vz = records[i].angle.vz;
        node->offset.vx = records[i].offset[0];
        node->offset.vy = records[i].offset[1];
        node->offset.vz = records[i].offset[2];
    }
    if (animations != NULL) {
        data = animations;
        xm_heap_set_next_class(0x11);
        player = set->players = xm_heap_alloc(data[0] * sizeof(Player), 0);
        set->count = data[0];
        for (i = 0; i < data[0]; i++) {
            if (data[i + 1] != 0) {
                xm_arena_node_bind_animation((AnimRecord *)data[i + 1], &player[i], root);
            } else {
                player[i].header = NULL;
            }
        }
    }
    return root;
}
#endif
void func_8008B5DC(void *arg0, s32 arg1) {
    func_8008B730(arg0, arg1, 1);
}

s16 func_8008B5FC(s32 arg0, s32 arg1, s32 arg2) {
    s16 temp_a0;
    s16 var_v0;
    s32 temp_v1;

    temp_a0 = arg0 & 0xFFF;
    temp_v1 = (temp_a0 - arg1) & 0xFFF;
    var_v0 = temp_a0;
    if (temp_v1 != 0) {
        if (temp_v1 < 0x800) {
            var_v0 = temp_a0 - (temp_v1 / arg2);
        } else {
            var_v0 = temp_a0 + ((s32) (0x1000 - temp_v1) / arg2);
        }
    }
    return var_v0;
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_angle_turn_toward (src/decomp/src/menu/arena_scene_graph_and_opponent.c:1178). */
#ifndef XM_S_arena_angle_turn_toward
#define XM_S_arena_angle_turn_toward
s32 xm_arena_angle_turn_toward(s32 from, s32 to, s32 step) __asm__("func_8008B650");
#endif
#ifndef XM_S_rand
#define XM_S_rand
int xm_rand(void) __asm__("rand");
#endif
s32 xm_arena_angle_turn_toward(s32 from, s32 to, s32 step) __asm__("func_8008B650");
s32 xm_arena_angle_turn_toward(s32 from, s32 to, s32 step) {
    s16 angle = from;
    s16 target = to;
    s32 diff = (from - to) & 0xFFF;

    if (diff != 0) {
        if (diff == 0x800 ? (xm_rand() & 1) : diff < 0x800) {
            angle -= step;
            if (((angle - target) & 0xFFF) > 0x800) {
                angle = target;
            }
        } else {
            angle += step;
            if (((angle - target) & 0xFFF) < 0x800) {
                angle = target;
            }
        }
    }
    return angle;
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_node_step_anim_player (src/decomp/src/menu/arena_scene_graph_and_opponent.c:1206). */
#ifndef XM_S_arena_node_step_anim_player
#define XM_S_arena_node_step_anim_player
s32 xm_arena_node_step_anim_player(Player *player, s32 frames, s32 steps) __asm__("func_8008B730");
#endif
#ifndef XM_S_arena_angle_ease_toward
#define XM_S_arena_angle_ease_toward
s16 xm_arena_angle_ease_toward(s32 angle, s32 target, s32 steps) __asm__("func_8008B5FC");
#endif
s32 func_8008B730(Player *player, s32 frames, s32 steps) {
    Key *key;
    Channel *channel;
    u8 *code;
    s8 command;
    s16 value; 
    s32 i;
    s32 j;

    if (player->header == NULL) {
        return 1;
    }
    if (frames == 0) {
        return;
    }
    if (player->frame + frames > player->header->frames) {
        frames = player->header->frames - player->frame;
    }
    player->frame += frames;
    steps -= frames;
    if (steps <= 0) {
        steps = 1;
    }
    key = player->keys;
    if (steps == 1) {
        for (i = 0; i < player->header->keys; i++, key++) {
            *key->target = key->value;
        }
    } else {
        for (i = 0; i < player->header->keys; i++, key++) {
            value = *key->target;
            if (key->angular) {
                *key->target = xm_arena_angle_ease_toward(value, key->value, steps);
            } else {
                *key->target = value + (key->value - value) / steps;
            }
        }
    }
    channel = player->channels;
    for (i = 0; i < player->header->channels; i++, channel++) {
        for (j = 0; j < frames; j++) {
            if (channel->hold) {
                channel->hold--;
            } else {
                code = channel->current++;
                command = *(s8 *)code;
                if (command & 0x80) {
                    value = command & 0x3F;
                    if (command & 0x40) {
                        channel->current = code + 2;
                        value |= (s8)code[1] << 6;
                        channel->delta = value;
                    } else {
                        channel->hold = value;
                    }
                } else {
                    channel->delta = ((u8)command << 25) >> 25; 
                }
            }
            channel->value += channel->delta;
        }
        if (steps == 1) {
            *channel->target = channel->value;
        } else {
            value = *channel->target;
            if (channel->angular) {
                *channel->target = xm_arena_angle_ease_toward(value, channel->value, steps);
            } else {
                *channel->target = value + (channel->value - value) / steps;
            }
        }
    }
    if (player->frame == player->header->frames) {
        if (steps == 1) {
            return 1;
        }
    }
    return 0;
}
#endif
void *func_8008BA2C(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 *p;
    s32 i;
    s32 cursor;
    u8 *obj;

    HeapSetCurrentContentType(3);
    obj = HeapAlloc(0x84, 2);
    i = 0x1F;
    p = (s32 *)(obj + 0x7C);
    do {
        *p = 0;
        i--;
        p--;
    } while (i >= 0);
    *(s32 *)(obj + 0x80) = arg2;
    *(s32 *)(obj + 0x70) = func_800405E4();
    *(s32 *)(obj + 0x7C) = arg0;
    *(s32 *)(obj + 0x10) = arg1;
    cursor = *(s32 *)(obj + 0x80) + arg3 * 4;
    *(s32 *)(obj + 0x74) = cursor;
    *(s32 *)(obj + 0x78) = cursor;
    func_8008BB3C(obj);
    return obj;
}
void func_8008BAE0(s32 arg0) {
    HeapFree(arg0);
}
extern s32 D_80096D88;
extern s32 D_80096D8C;

/* Hand-written in retail (context-switch module, see func_8008BB1C): reads the
 * saved pair into $t0/$t1, so both temporaries are pinned. */
void func_8008BB00(s32 *arg0) {
    register s32 t0 asm("$8");
    register s32 t1 asm("$9");

    t0 = D_80096D88;
    t1 = D_80096D8C;
    arg0[0] = t0;
    arg0[1] = t1;
}

/* Retail is hand-written: it belongs to the context-switch module with
 * func_8008BB00 / func_8008BB3C / func_8008BC04 (which save $k0/$k1/$sp into
 * D_80096D88). gcc 2.7.2 copies through $v0 (two scalars) or $v0/$v1 (an
 * 8-byte struct, move_by_pieces), never $t0, so the temporary is pinned. */
void func_8008BB1C(s32 *arg0) {
    register s32 t asm("$8");

    t = arg0[0];
    D_80096D88 = t;
    t = arg0[1];
    D_80096D8C = t;
}

INCLUDE_ASM("asm/battling/nonmatchings/main", func_8008BB3C);
INCLUDE_ASM("asm/battling/nonmatchings/main", func_8008BC04);
/* VECTOR layout (libgte): vx, vy, vz, pad. Shared by func_8008BCC8 / func_8008E78C. */
#ifndef BATTLING_VEC_DEFINED
#define BATTLING_VEC_DEFINED
typedef struct {
    s32 vx;
    s32 vy;
    s32 vz;
    s32 pad;
} BattlingVec;
#endif
long VectorNormal();
void func_8008C3A8(s32, s32, u16);
extern BattlingVec D_8009A2C8;

void func_8008BCC8(void *arg0, s32 arg1) {
    BattlingVec dir;
    s32 unused[4];

    dir.vx = -8;
    dir.vy = -8;
    dir.vz = -16;
    VectorNormal(&dir, &D_8009A2C8);
    D_8009A2C8.vx <<= 4;
    D_8009A2C8.vy <<= 4;
    D_8009A2C8.vz <<= 4;
    func_8008C3A8(*(s32 *)((u8 *)arg0 + 8), arg1, *(u16 *)((u8 *)arg0 + 2));
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_mesh_draw_groups (src/decomp/src/menu/arena_scene_graph_and_opponent.c:1341). */
#ifndef XM_T_ModelPrim
#define XM_T_ModelPrim
typedef struct {
    u32 tag;
    u32 colour;        
    u8 unk8[0xC];
} ModelPrim;
#endif
#ifndef XM_T_PrimitiveGroup
#define XM_T_PrimitiveGroup
typedef struct PrimitiveGroup {
    u8 type;
    u8 unk1;
    s16 count;
} PrimitiveGroup;
#endif
#ifndef XM_S_arena_mesh_draw_groups
#define XM_S_arena_mesh_draw_groups
void xm_arena_mesh_draw_groups(SpriteModel *mesh, ModelPrim *prims, u32 *ot, u8 *work) __asm__("func_8008BD70");
#endif
#ifndef XM_S_model_current_primitive_group
#define XM_S_model_current_primitive_group
extern PrimitiveGroup *xm_model_current_primitive_group __asm__("D_80059528");
#endif
#ifndef XM_S_model_current_packet
#define XM_S_model_current_packet
extern RenderPacket *xm_model_current_packet __asm__("D_80059424");
#endif
#ifndef XM_S_model_ot
#define XM_S_model_ot
extern u32 *xm_model_ot __asm__("D_80059568");
#endif
#ifndef XM_S_model_current_vertices
#define XM_S_model_current_vertices
extern SVECTOR *xm_model_current_vertices __asm__("D_8005953C");
#endif
#ifndef XM_S_model_submitted_primitive_count
#define XM_S_model_submitted_primitive_count
extern s32 xm_model_submitted_primitive_count __asm__("D_800595C0");
#endif
#ifndef XM_S_arena_mesh_draw_flat_quads
#define XM_S_arena_mesh_draw_flat_quads
void xm_arena_mesh_draw_flat_quads(u8 *prims, s32 count) __asm__("func_8008C620");
#endif
#ifndef XM_S_arena_mesh_draw_flat_triangles
#define XM_S_arena_mesh_draw_flat_triangles
void xm_arena_mesh_draw_flat_triangles(u8 *prims, s32 count) __asm__("func_8008C4B0");
#endif
void xm_arena_mesh_draw_groups(SpriteModel *mesh, ModelPrim *prims, u32 *ot, u8 *work) __asm__("func_8008BD70");
void xm_arena_mesh_draw_groups(SpriteModel *mesh, ModelPrim *prims, u32 *ot, u8 *work) {
    PrimitiveGroup *group;
    s32 groups = mesh->group_count;

    xm_model_current_primitive_group = (PrimitiveGroup *)mesh->unk10;
    xm_model_current_packet = (RenderPacket *)prims;
    xm_model_ot = ot;
    xm_model_current_vertices = (SVECTOR *)work;
    xm_model_submitted_primitive_count += mesh->primitive_count;
    while (--groups != -1) {
        group = xm_model_current_primitive_group;
        xm_model_current_primitive_group = group + 1;
        if (group->type & 8) {
            xm_arena_mesh_draw_flat_quads((u8 *)xm_model_current_primitive_group, group->count);
        } else {
            xm_arena_mesh_draw_flat_triangles((u8 *)xm_model_current_primitive_group, group->count);
        }
        xm_model_current_primitive_group = (PrimitiveGroup *)((u8 *)xm_model_current_primitive_group + group->count * 8);
    }
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_mesh_build_packets (src/decomp/src/menu/arena_scene_graph_and_opponent.c:1365). */
#ifndef XM_T_ModelPrims
#define XM_T_ModelPrims
typedef struct {
    s16 vertices;      
    s16 count;         
    void *vertexData;  
    u8 *work;          
    SpriteModel *mesh; 
    ModelPrim *prims[2]; 
} ModelPrims;
#endif
#ifndef XM_S_arena_mesh_build_packets
#define XM_S_arena_mesh_build_packets
void xm_arena_mesh_build_packets(ModelPrims *prims, SpriteModel *mesh) __asm__("func_8008BE4C");
#endif
#ifndef XM_S_arena_copy_words
#define XM_S_arena_copy_words
void xm_arena_copy_words(void *dst, void *src, s32 size) __asm__("func_800732AC");
#endif
void func_8008BE4C(ModelPrims *prims, SpriteModel *mesh) {
    s32 n; 
    s32 i;
    s32 j;
    s32 triangles;
    s32 quads;
    PrimitiveGroup *group;
    u8 *vertex;
    u8 *packet;

    prims->vertices = mesh->vertex_count;
    prims->count = mesh->primitive_count;
    prims->vertexData = mesh->vertices;
    prims->mesh = mesh;
    xm_model_current_primitive_group = (PrimitiveGroup *)mesh->unk10;
    xm_heap_set_next_class(0x13);
    vertex = prims->work = xm_heap_alloc(prims->vertices * 8, 2);
    n = prims->vertices;
    while (--n != -1) {
        ((s16 *)vertex)[1] = 0;
        vertex += 8;
    }
    xm_heap_set_next_class(5);
    triangles = 0;
    quads = 0;
    n = mesh->group_count;
    while (--n != -1) {
        group = xm_model_current_primitive_group;
        xm_model_current_primitive_group = group + 1;
        if (group->type & 8) {
            quads += group->count;
        } else {
            triangles += group->count;
        }
        xm_model_current_primitive_group = (PrimitiveGroup *)((u8 *)xm_model_current_primitive_group + group->count * 8);
    }
    n = triangles * 0x14 + quads * 0x18;
    packet = xm_heap_alloc(n * 2, 2);
    prims->prims[0] = (ModelPrim *)packet;
    prims->prims[1] = (ModelPrim *)(packet + n);
    i = mesh->group_count;
    xm_model_current_primitive_group = (PrimitiveGroup *)mesh->unk10;
    while (--i != -1) {
        group = xm_model_current_primitive_group;
        xm_model_current_primitive_group = group + 1;
        if (group->type & 8) {
            j = group->count;
            while (--j != -1) {
                TAG_LEN(packet) = 5;
                ((u32 *)packet)[1] = 0x28403030;
                packet += 0x18;
            }
        } else {
            j = group->count;
            while (--j != -1) {
                TAG_LEN(packet) = 4;
                ((u32 *)packet)[1] = 0x20403030;
                packet += 0x14;
            }
        }
        xm_model_current_primitive_group = (PrimitiveGroup *)((u8 *)xm_model_current_primitive_group + group->count * 8);
    }
    xm_arena_copy_words(prims->prims[1], prims->prims[0], n);
}
#endif

void func_8008C0BC(void *arg0, s32 arg1) {
    *(s32 *)((s8*)(arg0) + 0) = 5;
    *(s32 *)((s8*)(arg0) + 4) = arg1;
}

typedef struct {
    s32 type;
    s32 *src;
    s32 unk8;
    s32 unkC;
} BattlingTreeEntry;

void *func_8008C0CC(s32 *arg0) {
    BattlingTreeEntry *entry;

    HeapSetCurrentContentType(7);
    entry = HeapAlloc(0x10, 2);
    entry->type = *arg0;
    entry->src = arg0;
    entry->unk8 = D_80092828;
    entry->unkC = 0;
    return entry;
}


void func_8008C120(void *arg0) {
    s32 temp_a0_2;
    void *temp_a0;
    void *temp_s0;

    temp_s0 = *(void **)((s8*)(arg0) + 0xC);
    if (temp_s0 != NULL) {
        temp_a0 = *(void **)((s8*)(temp_s0) + 8);
        if (temp_a0 != NULL) {
            HeapFree(temp_a0);
        }
        temp_a0_2 = *(s32 *)((s8*)(temp_s0) + 0x10);
        if (temp_a0_2 != 0) {
            HeapDelayedFree(temp_a0_2, 2);
        }
        HeapFree(temp_s0);
    }
}

extern s32 D_80092824;
void *func_8008C188(void *arg0, void *arg1) {
    s32 count;
    void *node;
    s32 buf;
    void *child;
    s32 *entry;

    HeapSetCurrentContentType(6);
    node = func_80089C54();
    entry = func_8008C0CC(arg0);
    func_8008C0BC(node, entry);
    if (entry[0] == 1) {
        count = *(s32 *)(*(u8 **)((u8 *)arg0 + 4) + 0x14);
        HeapSetCurrentContentType(5);
        buf = HeapAlloc(0x18, 2);
        entry[3] = buf;
        func_8008BE4C(buf, count);
    }
    child = *(void **)((u8 *)arg0 + 0x94);
    D_80092824++;
    if (child != NULL) {
        func_80089C88(node, func_8008C188(child, node));
    }
    child = *(void **)((u8 *)arg0 + 0x90);
    if (child != NULL) {
        func_80089C88(arg1, func_8008C188(child, arg1));
    }
    return node;
}
extern s32 D_80092824;
/* K&R definition: func_8008C1DC calls it with no arguments, and retail
 * forwards whatever a0 holds to func_8008C188. */
void func_8008C298(arg0)
    s32 arg0;
{
    D_80092824 = 0;
    func_8008C188(arg0, 0);
}

void func_8008C2C0(s32 arg0) {
    D_80092828 = arg0;
    func_8008C298();
}


void func_8008C2E8(void *arg0) {
    void *temp_a0;
    void *temp_a0_2;
    void *temp_a0_3;
    void *temp_v0;

    temp_a0 = *(void **)((s8*)(arg0) + 4);
    if ((*(s32 *)((s8*)(temp_a0) + 0) == 1) && !(**(s32 **)((s8*)(*(void **)((s8*)(temp_a0) + 4)) + 4) & 1)) {
        temp_v0 = *(void **)((s8*)(temp_a0) + 0xC);
        func_8008BD70(*(s32 *)((s8*)(temp_v0) + 0xC), *(s32 *)((s8*)(((D_800928A0 * 4) + temp_v0)) + 0x10), D_800928E4 + 4, *(s32 *)((s8*)(temp_v0) + 8));
    }
    temp_a0_2 = *(void **)((s8*)(arg0) + 0x94);
    if (temp_a0_2 != NULL) {
        func_8008C2E8(temp_a0_2);
    }
    temp_a0_3 = *(void **)((s8*)(arg0) + 0x90);
    if (temp_a0_3 != NULL) {
        func_8008C2E8(temp_a0_3);
    }
}

INCLUDE_ASM("asm/battling/nonmatchings/main", func_8008C3A8);
INCLUDE_ASM("asm/battling/nonmatchings/main", func_8008C4B0);
INCLUDE_ASM("asm/battling/nonmatchings/main", func_8008C620);
/* 8-byte, 2-byte-aligned vector (SVECTOR layout): copies of it compile to
 * lwl/lwr + swl/swr pairs. Shared by func_8008C7C0 / func_8008C9B8 / func_8008CC2C / func_8008CCB0 / func_8008CF9C. */
#ifndef BATTLING_SVEC_DEFINED
#define BATTLING_SVEC_DEFINED
typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} BattlingSVec;
#endif

void func_8008C7C0(void *arg0) {
    *(BattlingSVec *)((u8 *)arg0 + 0x20) = *(BattlingSVec *)arg0;
    *(BattlingSVec *)((u8 *)arg0 + 0x18) = *(BattlingSVec *)((u8 *)arg0 + 0x20);
    *(BattlingSVec *)((u8 *)arg0 + 0x10) = *(BattlingSVec *)((u8 *)arg0 + 0x18);
}

/* GPU LINE_F4 primitive (libgpu LINE_F4 layout): tag, r0 g0 b0 code,
 * x0 y0 .. x3 y3, pad (0x55555555 terminator). */
typedef struct {
    u32 tag;
    u8 r0;
    u8 g0;
    u8 b0;
    u8 code;
    s16 x0;
    s16 y0;
    s16 x1;
    s16 y1;
    s16 x2;
    s16 y2;
    s16 x3;
    s16 y3;
    u32 pad;
} BattlingLineF4;

void func_8008C828(void *arg0, void *arg1) {
    BattlingLineF4 *line;

    line = (BattlingLineF4 *)((u8 *)arg0 + 0x28);
    *((u8 *)line + 3) = 6;
    line->code = 0x4C;
    line->pad = 0x55555555;
    line->code |= 2;
    line->r0 = *((u8 *)arg1 + 0x74);
    line->g0 = *((u8 *)arg1 + 0x75);
    line->b0 = *((u8 *)arg1 + 0x76);
    line[1] = line[0];
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_spark_draw_line4 (src/decomp/src/menu/arena_scene_graph_and_opponent.c:1549). */
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
#ifndef XM_T_SparkLine4
#define XM_T_SparkLine4
typedef struct {
    SVECTOR pos;
    SVECTOR vel;
    SVECTOR trail[3];
    LINE_F4 line[2];
} SparkLine4;
#endif
#ifndef XM_S_arena_spark_draw_line4
#define XM_S_arena_spark_draw_line4
void xm_arena_spark_draw_line4() __asm__("func_8008C8B4");
#endif
#ifndef XM_S_RotTransPers4
#define XM_S_RotTransPers4
long xm_RotTransPers4(SVECTOR *v0, SVECTOR *v1, SVECTOR *v2, SVECTOR *v3, long *sxy0, long *sxy1,
                   long *sxy2, long *sxy3, long *p, long *flag) __asm__("RotTransPers4");
#endif
#ifndef XM_S_gpu_ot_link_line_f4
#define XM_S_gpu_ot_link_line_f4
void xm_gpu_ot_link_line_f4(u_long *ot, LINE_F4 *prim) __asm__("func_80031750");
#endif
void func_8008C8B4(SparkLine4 *spark, u32 *ot) {
    LINE_F4 *line = &spark->line[xm_arena_draw_buffer_index];
    long depth;
    s32 otz;

    otz = xm_RotTransPers4(&spark->pos, &spark->trail[0], &spark->trail[1], &spark->trail[2],
                        (long *)&line->x0, (long *)&line->x1, (long *)&line->x2, (long *)&line->x3,
                        &depth, &depth);
    spark->trail[2] = spark->trail[1];
    spark->trail[1] = spark->trail[0];
    spark->trail[0] = spark->pos;
    xm_gpu_ot_link_line_f4((u_long *)(ot + (otz >> 2)), line);
}
#endif
/* 8-byte, 2-byte-aligned vector (SVECTOR layout): copies of it compile to
 * lwl/lwr + swl/swr pairs. Shared by func_8008C7C0 / func_8008C9B8 / func_8008CC2C / func_8008CCB0 / func_8008CF9C. */
#ifndef BATTLING_SVEC_DEFINED
#define BATTLING_SVEC_DEFINED
typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} BattlingSVec;
#endif

void func_8008C9B8(void *arg0) {
    *(BattlingSVec *)((u8 *)arg0 + 0x18) = *(BattlingSVec *)arg0;
    *(BattlingSVec *)((u8 *)arg0 + 0x10) = *(BattlingSVec *)((u8 *)arg0 + 0x18);
}

/* GPU LINE_F3 primitive (libgpu LINE_F3 layout): tag, r0 g0 b0 code, x0 y0,
 * x1 y1, x2 y2, pad (0x55555555 terminator). */
typedef struct {
    u32 tag;
    u8 r0;
    u8 g0;
    u8 b0;
    u8 code;
    s16 x0;
    s16 y0;
    s16 x1;
    s16 y1;
    s16 x2;
    s16 y2;
    u32 pad;
} BattlingLineF3;

void func_8008CA00(void *arg0, void *arg1) {
    BattlingLineF3 *line;

    line = (BattlingLineF3 *)((u8 *)arg0 + 0x20);
    *((u8 *)line + 3) = 5;
    line->code = 0x48;
    line->pad = 0x55555555;
    line->code |= 2;
    line->r0 = *((u8 *)arg1 + 0x74);
    line->g0 = *((u8 *)arg1 + 0x75);
    line->b0 = *((u8 *)arg1 + 0x76);
    line[1] = line[0];
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_spark_draw_line3 (src/decomp/src/menu/arena_scene_graph_and_opponent.c:1581). */
#ifndef XM_T_LINE_F3
#define XM_T_LINE_F3
typedef struct {
    u_long tag;
    u_char r0, g0, b0, code;
    short x0, y0;
    short x1, y1;
    short x2, y2;
    u_long pad;
} LINE_F3;
#endif
#ifndef XM_T_SparkLine3
#define XM_T_SparkLine3
typedef struct {
    SVECTOR pos;
    SVECTOR vel;
    SVECTOR trail[2];
    LINE_F3 line[2];
} SparkLine3;
#endif
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
#ifndef XM_T_gte_stszotz
#define XM_T_gte_stszotz
#define gte_stszotz(r0)                                                        \
    __asm__ volatile("mfc2 $12, $19;"                                          \
                     "nop;"                                                    \
                     "sra $12, $12, 2;"                                        \
                     "sw $12, 0(%0)"                                           \
                     :                                                         \
                     : "r"(r0)                                                 \
                     : "$12", "memory")
#endif
#ifndef XM_S_arena_spark_draw_line3
#define XM_S_arena_spark_draw_line3
void xm_arena_spark_draw_line3() __asm__("func_8008CA84");
#endif
#ifndef XM_S_arena_spark_view_origin
#define XM_S_arena_spark_view_origin
extern SVECTOR *xm_arena_spark_view_origin __asm__("D_80092830");
#endif
#ifndef XM_S_arena_spark_gte_scratch
#define XM_S_arena_spark_gte_scratch
extern SVECTOR *xm_arena_spark_gte_scratch __asm__("D_8009282C");
#endif
#ifndef XM_S_gpu_ot_link_line_f3
#define XM_S_gpu_ot_link_line_f3
void xm_gpu_ot_link_line_f3(u_long *ot, LINE_F3 *prim) __asm__("func_80031708");
#endif
void func_8008CA84(SparkLine3 *spark, u32 *ot) {
    SVECTOR *origin = xm_arena_spark_view_origin;
    SVECTOR *work = xm_arena_spark_gte_scratch;
    LINE_F3 *line;
    s32 otz;

    work[0].vx = spark->pos.vx - origin->vx;
    work[0].vy = spark->pos.vy - origin->vy;
    work[0].vz = spark->pos.vz - origin->vz;
    work[1].vx = spark->trail[0].vx - origin->vx;
    work[1].vy = spark->trail[0].vy - origin->vy;
    work[1].vz = spark->trail[0].vz - origin->vz;
    work[2].vx = spark->trail[1].vx - origin->vx;
    work[2].vy = spark->trail[1].vy - origin->vy;
    work[2].vz = spark->trail[1].vz - origin->vz;
    gte_ldv3c(xm_arena_spark_gte_scratch);
    gte_rtpt();
    line = &spark->line[xm_arena_draw_buffer_index];
    spark->trail[1] = spark->trail[0];
    spark->trail[0] = spark->pos;
    gte_stsxy3(&line->x0, &line->x1, &line->x2);
    gte_stszotz(&otz);
    xm_gpu_ot_link_line_f3((u_long *)(ot + (otz >> 2)), line);
}
#endif
/* 8-byte, 2-byte-aligned vector (SVECTOR layout): copies of it compile to
 * lwl/lwr + swl/swr pairs. Shared by func_8008C7C0 / func_8008C9B8 / func_8008CC2C / func_8008CCB0 / func_8008CF9C. */
#ifndef BATTLING_SVEC_DEFINED
#define BATTLING_SVEC_DEFINED
typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} BattlingSVec;
#endif

void func_8008CC2C(void *arg0) {
    *(BattlingSVec *)((u8 *)arg0 + 0x10) = *(BattlingSVec *)arg0;
}

/* GPU LINE_F2 primitive (libgpu LINE_F2 layout): tag, r0 g0 b0 code, x0 y0, x1 y1. */
typedef struct {
    u32 tag;
    u8 r0;
    u8 g0;
    u8 b0;
    u8 code;
    s16 x0;
    s16 y0;
    s16 x1;
    s16 y1;
} BattlingLineF2;

void func_8008CC54(void *arg0, void *arg1) {
    BattlingLineF2 *line;

    line = (BattlingLineF2 *)((u8 *)arg0 + 0x18);
    *((u8 *)line + 3) = 3;
    line->code = 0x42;
    line->r0 = *((u8 *)arg1 + 0x74);
    line->g0 = *((u8 *)arg1 + 0x75);
    line->b0 = *((u8 *)arg1 + 0x76);
    line[1] = line[0];
}

/* 8-byte, 2-byte-aligned vector (SVECTOR layout): copies of it compile to
 * lwl/lwr + swl/swr pairs. Shared by func_8008C7C0 / func_8008C9B8 / func_8008CC2C / func_8008CCB0 / func_8008CF9C. */
#ifndef BATTLING_SVEC_DEFINED
#define BATTLING_SVEC_DEFINED
typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} BattlingSVec;
#endif
long RotTransPers3();
void func_800316C0(u32 *pPrim, u32 nextAddr);

/* Projects a 2-point line (current and previous position) into the LINE_F2
 * of the current buffer, then links it into the OT. */
void func_8008CCB0(void *arg0, u32 *ot) {
    s32 tmp;
    u8 *prim;
    s32 z;

    prim = (u8 *)arg0 + (D_800928A0 * 16 + 0x18);
    z = RotTransPers3(arg0, (u8 *)arg0 + 0x10, &tmp, prim + 8, prim + 0xC, &tmp, &tmp, &tmp);
    *(BattlingSVec *)((u8 *)arg0 + 0x10) = *(BattlingSVec *)arg0;
    func_800316C0(ot + (z >> 2), (u32)prim);
}


void func_8008CD54(void) {

}

/* GPU TILE primitive (libgpu TILE layout): tag, r0 g0 b0 code, x0 y0, w h. */
typedef struct {
    u32 tag;
    u8 r0;
    u8 g0;
    u8 b0;
    u8 code;
    s16 x0;
    s16 y0;
    s16 w;
    s16 h;
} BattlingTile;

void func_8008CD5C(void *arg0, void *arg1) {
    BattlingTile *tile;
    s16 h;

    tile = (BattlingTile *)((u8 *)arg0 + 0x10);
    *((u8 *)tile + 3) = 3;
    tile->code = 0x62;
    h = rand() % 2 + 2;
    tile->h = h;
    tile->w = h * 2;
    tile->r0 = *((u8 *)arg1 + 0x74);
    tile->g0 = *((u8 *)arg1 + 0x75);
    tile->b0 = *((u8 *)arg1 + 0x76);
    tile[1] = tile[0];
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_spark_draw_tile (src/decomp/src/menu/arena_scene_graph_and_opponent.c:1651). */
#ifndef XM_T_TILE
#define XM_T_TILE
typedef struct {
    u_long tag;
    u_char r0, g0, b0, code;
    short x0, y0;
    short w, h;
} TILE;
#endif
#ifndef XM_T_SparkTile
#define XM_T_SparkTile
typedef struct {
    SVECTOR pos;
    SVECTOR vel;
    TILE tile[2];
} SparkTile;
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
#ifndef XM_T_gte_stsxy
#define XM_T_gte_stsxy
#define gte_stsxy(r0) __asm__ volatile("swc2 $14, 0(%0)" : : "r"(r0) : "memory")
#endif
#ifndef XM_S_arena_spark_draw_tile
#define XM_S_arena_spark_draw_tile
void xm_arena_spark_draw_tile() __asm__("func_8008CE0C");
#endif
#ifndef XM_S_gpu_ot_link_tile
#define XM_S_gpu_ot_link_tile
void xm_gpu_ot_link_tile(u_long *ot, void *prim) __asm__("func_80031804");
#endif
void func_8008CE0C(SparkTile *spark, u32 *ot) {
    SVECTOR *origin = xm_arena_spark_view_origin;
    SVECTOR v;
    TILE *tile;
    s32 otz;

    v.vx = spark->pos.vx - origin->vx;
    v.vy = spark->pos.vy - origin->vy;
    v.vz = spark->pos.vz - origin->vz;
    gte_ldv0(&v);
    gte_rtps();
    tile = &spark->tile[xm_arena_draw_buffer_index];
    gte_stsxy(&tile->x0);
    gte_stszotz(&otz);
    xm_gpu_ot_link_tile((u_long *)(ot + (otz >> 2)), tile);
}
#endif

void func_8008CED4(void) {

}

/* GPU TILE_1 primitive (libgpu TILE_1 layout): tag, r0 g0 b0 code, x0 y0. */
typedef struct {
    u32 tag;
    u8 r0;
    u8 g0;
    u8 b0;
    u8 code;
    s16 x0;
    s16 y0;
} BattlingTile1;

void func_8008CEDC(void *arg0, void *arg1) {
    BattlingTile1 *tile;

    tile = (BattlingTile1 *)((u8 *)arg0 + 0x10);
    *((u8 *)tile + 3) = 2;
    tile->code = 0x6A;
    tile->r0 = *((u8 *)arg1 + 0x74);
    tile->g0 = *((u8 *)arg1 + 0x75);
    tile->b0 = *((u8 *)arg1 + 0x76);
    tile[1] = tile[0];
}

long RotTransPers();
void func_80031870(u32 *pPrim, u32 nextAddr);

void func_8008CF30(void *arg0, u32 *ot) {
    s32 flag;
    u8 *prim;

    prim = (u8 *)arg0 + (D_800928A0 * 0xC + 0x10);
    func_80031870(ot + (RotTransPers(arg0, prim + 8, &flag, &flag) >> 2), (u32)prim);
}

/* 8-byte, 2-byte-aligned vector (SVECTOR layout): copies of it compile to
 * lwl/lwr + swl/swr pairs. Shared by func_8008C7C0 / func_8008C9B8 / func_8008CC2C / func_8008CCB0 / func_8008CF9C. */
#ifndef BATTLING_SVEC_DEFINED
#define BATTLING_SVEC_DEFINED
typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} BattlingSVec;
#endif

void func_8008CF9C(void *arg0, BattlingSVec *out) {
    *out = *(BattlingSVec *)((u8 *)arg0 + 0x1C);
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_spark_place_in_rotated_box (src/decomp/src/menu/arena_scene_graph_and_opponent.c:1697). */
#ifndef XM_T_Emitter
#define XM_T_Emitter
typedef struct Emitter Emitter;
#endif
#ifndef XM_T_Spark
#define XM_T_Spark
typedef struct {
    SVECTOR pos; 
    SVECTOR vel;
} Spark;
#endif
#ifndef XM_T_Emitter__def
#define XM_T_Emitter__def
struct Emitter {
    s16 gravity;            
    s16 unk2;
    s16 unk4;
    s16 unk6;
    s16 unk8;
    s16 count;              
    s16 size;               
    s16 unkE;
    u8 *sparks;             
    SVECTOR base;           
    SVECTOR origin;         
    SVECTOR range;          
    SVECTOR offset;         
    SVECTOR angles;         
    SVECTOR turn;           
    s16 spread;             
    s16 unk46;
    s16 speed;              
    s16 speed_range;        
    s16 unk4C;
    s16 placement;          
    void (*reset)(void *spark);          
    void (*draw)(void *spark, u32 *ot);  
    void (*setup)(void *spark, Emitter *emitter); 
    void (*place)(Emitter *emitter, SVECTOR *pos); 
    void (*update)(Spark *spark);        
    s16 unk64;
    s16 shape;              
    s16 unk68;
    s16 life;               
    u8 unk6C[0x8];
    u8 r;                   
    u8 g;
    u8 b;
    u8 unk77[0x5];
};
#endif
#ifndef XM_S_arena_spark_place_in_rotated_box
#define XM_S_arena_spark_place_in_rotated_box
void xm_arena_spark_place_in_rotated_box(Emitter *source, SVECTOR *pos) __asm__("func_8008CFC4");
#endif
#ifndef XM_S_libgte_rotate_svector
#define XM_S_libgte_rotate_svector
void xm_libgte_rotate_svector(SVECTOR *v0, VECTOR *v1) __asm__("ApplyRotMatrix");
#endif
void func_8008CFC4(Emitter *source, SVECTOR *pos) {
    SVECTOR v;
    VECTOR r;

    v.vx = xm_rand() % source->range.vx - source->offset.vx;
    v.vy = xm_rand() % source->range.vy - source->offset.vy;
    v.vz = xm_rand() % source->range.vz - source->offset.vz;
    xm_libgte_rotate_svector(&v, &r);
    pos->vx = source->origin.vx + r.vx;
    pos->vy = source->origin.vy + r.vy;
    pos->vz = source->origin.vz + r.vz;
}
#endif

void func_8008D0A4(void *arg0, void *arg1) {
    s32 temp_hi;
    s32 temp_hi_2;

    temp_hi = rand() % (s16) *(s16 *)((s8*)(arg0) + 0x24);
    *(s16 *)((s8*)(arg1) + 0) = (s16) ((*(u16 *)((s8*)(arg0) + 0x1C) + temp_hi) - *(u16 *)((s8*)(arg0) + 0x2C));
    temp_hi_2 = rand(temp_hi) % (s16) *(s16 *)((s8*)(arg0) + 0x26);
    *(s16 *)((s8*)(arg1) + 2) = (s16) ((*(u16 *)((s8*)(arg0) + 0x1E) + temp_hi_2) - *(u16 *)((s8*)(arg0) + 0x2E));
    *(s16 *)((s8*)(arg1) + 4) = (s16) ((*(u16 *)((s8*)(arg0) + 0x20) + (rand(temp_hi_2) % (s16) *(s16 *)((s8*)(arg0) + 0x28))) - *(u16 *)((s8*)(arg0) + 0x30));
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_spark_place_in_rotated_rect (src/decomp/src/menu/arena_scene_graph_and_opponent.c:1719). */
#ifndef XM_S_arena_spark_place_in_rotated_rect
#define XM_S_arena_spark_place_in_rotated_rect
void xm_arena_spark_place_in_rotated_rect(Emitter *source, SVECTOR *pos) __asm__("func_8008D14C");
#endif
void func_8008D14C(Emitter *source, SVECTOR *pos) {
    SVECTOR v;
    VECTOR r;

    v.vx = xm_rand() % source->range.vx - source->offset.vx;
    v.vy = 0;
    v.vz = xm_rand() % source->range.vz - source->offset.vz;
    xm_libgte_rotate_svector(&v, &r);
    pos->vx = source->origin.vx + r.vx;
    pos->vy = source->origin.vy + r.vy;
    pos->vz = source->origin.vz + r.vz;
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_spark_place_in_rotated_ellipse (src/decomp/src/menu/arena_scene_graph_and_opponent.c:1734). */
#ifndef XM_S_arena_spark_place_in_rotated_ellipse
#define XM_S_arena_spark_place_in_rotated_ellipse
void xm_arena_spark_place_in_rotated_ellipse(Emitter *source, SVECTOR *pos) __asm__("func_8008D208");
#endif
#ifndef XM_S_gpu_get_sin
#define XM_S_gpu_get_sin
s32 xm_gpu_get_sin(s32 angle) __asm__("rcos");
#endif
#ifndef XM_S_gpu_get_cos
#define XM_S_gpu_get_cos
s32 xm_gpu_get_cos(s32 angle) __asm__("rsin");
#endif
void func_8008D208(Emitter *source, SVECTOR *pos) {
    SVECTOR v;
    VECTOR r;
    s32 angle = xm_rand();
    s32 radius = xm_rand();

    v.vx = (xm_gpu_get_sin(angle) * (radius % source->range.vx)) >> 13;
    v.vy = 0;
    v.vz = (xm_gpu_get_cos(angle) * (radius % source->range.vz)) >> 13;
    xm_libgte_rotate_svector(&v, &r);
    pos->vx = source->origin.vx + r.vx;
    pos->vy = source->origin.vy + r.vy;
    pos->vz = source->origin.vz + r.vz;
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_spark_place_on_rotated_ring (src/decomp/src/menu/arena_scene_graph_and_opponent.c:1751). */
#ifndef XM_S_arena_spark_place_on_rotated_ring
#define XM_S_arena_spark_place_on_rotated_ring
void xm_arena_spark_place_on_rotated_ring(Emitter *source, SVECTOR *pos) __asm__("func_8008D304");
#endif
void func_8008D304(Emitter *source, SVECTOR *pos) {
    SVECTOR v;
    VECTOR r;
    s32 angle;

    v.vx = (xm_gpu_get_sin(angle) * source->range.vx) >> 12;
    v.vy = xm_rand() % source->range.vy - source->range.vy / 2;
    v.vz = (xm_gpu_get_cos(angle) * source->range.vz) >> 12;
    xm_libgte_rotate_svector(&v, &r);
    pos->vx = source->origin.vx + r.vx;
    pos->vy = source->origin.vy + r.vy;
    pos->vz = source->origin.vz + r.vz;
}
#endif
extern u8 D_80091C74[];
extern s32 D_80091CC4[];
extern s32 D_80091CDC;

void *func_8008D3F4(s32 type, s32 arg1) {
    u8 *obj;
    s32 *rec;

    HeapSetCurrentContentType(0x15);
    obj = HeapAlloc(0x7C, 0);
    *(u16 *)(obj + 0x24) = 0x1000;
    *(u16 *)(obj + 0x26) = 0x1000;
    *(u16 *)(obj + 0x28) = 0x1000;
    *(s16 *)(obj + 0x44) = 1;
    *(s16 *)(obj + 0x4E) = arg1;
    *(s16 *)(obj + 2) = 0;
    *(s16 *)(obj + 4) = 0;
    *(s16 *)(obj + 6) = 0;
    *(s16 *)(obj + 0x14) = 0;
    *(s16 *)(obj + 0x16) = 0;
    *(s16 *)(obj + 0x18) = 0;
    *(s16 *)(obj + 0x34) = 0;
    *(s16 *)(obj + 0x36) = 0;
    *(s16 *)(obj + 0x38) = 0;
    *(s16 *)(obj + 0x3C) = 0;
    *(s16 *)(obj + 0x3E) = 0;
    *(s16 *)(obj + 0x40) = 0;
    *(s16 *)(obj + 0) = 0;
    *(s16 *)(obj + 0x46) = 0;
    *(s16 *)(obj + 0x48) = 0x100;
    *(s16 *)(obj + 0x4A) = 0x100;
    *(s16 *)(obj + 0x2C) = (s16)*(u16 *)(obj + 0x24) / 2;
    *(s16 *)(obj + 0x2E) = (s16)*(u16 *)(obj + 0x26) / 2;
    *(s16 *)(obj + 0x30) = (s16)*(u16 *)(obj + 0x28) / 2;
    *(s32 *)(obj + 0x5C) = D_80091CC4[(s16)arg1];
    *(s32 *)(obj + 0x60) = D_80091CDC;
    *(s32 *)(obj + 0x10) = 0;
    *(s16 *)(obj + 0x66) = type;
    *(s16 *)(obj + 0x64) = 0;
    *(s16 *)(obj + 0x6A) = 100;
    obj[0x74] = 0xFF;
    obj[0x75] = 0xFF;
    obj[0x76] = 0xFF;
    *(s16 *)(obj + 0x68) = 0;
    rec = (s32 *)(D_80091C74 + *(s16 *)(obj + 0x66) * 16);
    *(s16 *)(obj + 0xC) = rec[3];
    *(s32 *)(obj + 0x50) = rec[1];
    *(s32 *)(obj + 0x54) = rec[2];
    *(s32 *)(obj + 0x58) = rec[0];
    return obj;
}

void func_8008D580(void *arg0) {
    s32 i;
    u8 *p;

    p = *(u8 **)((s8*)(arg0) + 0x10);
    for (i = 0; i < *(s16 *)((s8*)(arg0) + 0xA); i++) {
        *(s16 *)(p + 6) = 0;
        p += *(s16 *)((s8*)(arg0) + 0xC);
    }
}

/* The 8-byte `vars` area in retail's frame is unused stack. A top-tested
 * `for (i = 0; i < n; i++)` produces it too (a dead CCmode pseudo gets a
 * stack slot), but that form flips the s1/s2 allocation of arg0 and i, so
 * the slot is reproduced with an unused local instead. */
void func_8008D5C0(void *arg0, s32 arg1) {
    void (*init)(void *, void *);
    s32 i;
    u8 *p;
    s32 unused[2];

    if (*(void **)((u8 *)arg0 + 0x10) != NULL) {
        HeapDelayedFree(*(void **)((u8 *)arg0 + 0x10), 3);
    }
    *(s16 *)((u8 *)arg0 + 0xA) = arg1;
    HeapSetCurrentContentType(0x14);
    p = HeapAlloc(*(s16 *)((u8 *)arg0 + 0xC) * *(s16 *)((u8 *)arg0 + 0xA), 0);
    init = *(void (**)(void *, void *))((u8 *)arg0 + 0x58);
    i = 0;
    *(void **)((u8 *)arg0 + 0x10) = p;
    if (*(s16 *)((u8 *)arg0 + 0xA) > 0) {
        do {
            init(p, arg0);
            i++;
            *(s16 *)(p + 6) = 0;
            p += *(s16 *)((u8 *)arg0 + 0xC);
        } while (i < *(s16 *)((u8 *)arg0 + 0xA));
    }
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_spark_launch (src/decomp/src/menu/arena_scene_graph_and_opponent.c:1850). */
#ifndef XM_T_gte_rtv0
#define XM_T_gte_rtv0
#define gte_rtv0() __asm__ volatile("nop;nop;.word 0x4A486012")
#endif
#ifndef XM_T_gte_stsv
#define XM_T_gte_stsv
#define gte_stsv(r0)                                                           \
    __asm__ volatile("mfc2 $12, $9;"                                           \
                     "mfc2 $13, $10;"                                          \
                     "mfc2 $14, $11;"                                          \
                     "sh $12, 0(%0);"                                          \
                     "sh $13, 2(%0);"                                          \
                     "sh $14, 4(%0)"                                           \
                     :                                                         \
                     : "r"(r0)                                                 \
                     : "$12", "$13", "$14", "memory")
#endif
#ifndef XM_T_gte_lddp
#define XM_T_gte_lddp
#define gte_lddp(r0) __asm__ volatile("mtc2 %0, $8" : : "r"(r0))
#endif
#ifndef XM_T_gte_ldsv
#define XM_T_gte_ldsv
#define gte_ldsv(r0)                                                           \
    __asm__ volatile("lhu $12, 0(%0);"                                         \
                     "lhu $13, 2(%0);"                                         \
                     "lhu $14, 4(%0);"                                         \
                     "mtc2 $12, $9;"                                           \
                     "mtc2 $13, $10;"                                          \
                     "mtc2 $14, $11"                                           \
                     :                                                         \
                     : "r"(r0)                                                 \
                     : "$12", "$13", "$14", "memory")
#endif
#ifndef XM_T_gte_gpf12
#define XM_T_gte_gpf12
#define gte_gpf12() __asm__ volatile("nop;nop;.word 0x4B98003D")
#endif
#ifndef XM_S_arena_spark_launch
#define XM_S_arena_spark_launch
void xm_arena_spark_launch(Emitter *emitter, MATRIX *rotation, s32 count) __asm__("func_8008D680");
#endif
#ifndef XM_S_gpu_build_rotation_matrix
#define XM_S_gpu_build_rotation_matrix
MATRIX *xm_gpu_build_rotation_matrix(SVECTOR *angles, MATRIX *m) __asm__("RotMatrix");
#endif
#ifndef XM_S_libgte_multiply_matrix_in_place
#define XM_S_libgte_multiply_matrix_in_place
MATRIX *xm_libgte_multiply_matrix_in_place(MATRIX *m0, MATRIX *m1) __asm__("MulMatrix");
#endif
#ifndef XM_S_SetRotMatrix
#define XM_S_SetRotMatrix
void xm_SetRotMatrix(MATRIX *m) __asm__("SetRotMatrix");
#endif
void func_8008D680(Emitter *emitter, MATRIX *rotation, s32 count) {
    SVECTOR dir;
    SVECTOR unit;
    MATRIX local;
    MATRIX world;
    MATRIX turned;
    u8 *spark;
    void (*place)(Emitter *, SVECTOR *);
    void (*reset)(void *);
    s32 left;
    s32 i;
    s32 angle;
    s32 heading;

    reset = emitter->reset;
    place = emitter->place;
    world = *rotation;
    emitter->origin.vx = emitter->base.vx + rotation->t[0];
    emitter->origin.vy = emitter->base.vy + rotation->t[1];
    emitter->origin.vz = emitter->base.vz + rotation->t[2];
    xm_gpu_build_rotation_matrix(&emitter->angles, &local);
    xm_libgte_multiply_matrix_in_place(&world, &local);
    xm_gpu_build_rotation_matrix(&emitter->turn, &turned);
    xm_libgte_multiply_matrix_in_place(&turned, &world);
    xm_SetRotMatrix(&turned);
    left = count;
    spark = emitter->sparks;
    for (i = 0; i < emitter->count; i++) {
        if (((Spark *)spark)->pos.pad == 0) {
            if (--left == -1) {
                break;
            }
            angle = xm_rand() % emitter->spread;
            heading = xm_rand();
            dir.vy = -xm_gpu_get_cos(angle);
            angle = xm_gpu_get_sin(angle);
            dir.vx = (xm_gpu_get_sin(heading) * angle) >> 12;
            dir.vz = (xm_gpu_get_cos(heading) * angle) >> 12;
            heading = emitter->speed + xm_rand() % emitter->speed_range; 
            gte_ldv0(&dir);
            gte_rtv0();
            gte_stsv(&unit);
            gte_lddp(heading);
            gte_ldsv(&unit);
            gte_gpf12();
            gte_stsv(&((Spark *)spark)->vel);
        }
        spark += emitter->size;
    }
    xm_SetRotMatrix(&world);
    left = count;
    spark = emitter->sparks;
    for (i = 0; i < emitter->count; i++) {
        if (((Spark *)spark)->pos.pad == 0) {
            if (--left == -1) {
                break;
            }
            place(emitter, (SVECTOR *)spark);
            ((Spark *)spark)->pos.pad = emitter->life;
            reset(spark);
        }
        spark += emitter->size;
    }
}
#endif
s32 func_80082488(void *, s32);

void func_8008D980(void *arg0) {
    s32 y;

    if (*(s16 *)((u8 *)arg0 + 0xA) > 0) {
        y = *(s16 *)((u8 *)arg0 + 2);
        if (func_80082488(arg0, 0) < y) {
            *(s16 *)((u8 *)arg0 + 0xA) = -*(s16 *)((u8 *)arg0 + 0xA) / 2;
        }
    }
}

void func_8008D9F0(void *arg0) {
    s32 v;

    if (*(s16 *)((s8*)(arg0) + 2) > 0) {
        v = -*(s16 *)((s8*)(arg0) + 0xA) / 2;
        *(s16 *)((s8*)(arg0) + 0xA) = v;
        v = (s16)v;
        if (v < 0) {
            v = -v;
        }
        if (v < 8) {
            *(s16 *)((s8*)(arg0) + 6) = 0;
        }
    }
}

extern s32 D_8009282C;
extern s32 D_80092830;
extern s32 D_80096FA8;
extern s32 D_80096FAC;
extern s32 D_80096FB0;

void func_8008DA48(void *arg0, s32 arg1) {
    void (*update)(void *, s32);
    s32 i;
    s32 v;
    s32 unused[10];
    u8 *p;


    D_8009282C = 0x1F800000;
    D_80092830 = 0x1F800030;
    *(s16 *)0x1F800030 = D_80096FA8;
    *(s16 *)0x1F800032 = D_80096FAC;
    *(s16 *)0x1F800034 = D_80096FB0;
    p = *(u8 **)((u8 *)arg0 + 0x10);
    update = *(void (**)(void *, s32))((u8 *)arg0 + 0x54);
    for (i = 0; i < *(s16 *)((u8 *)arg0 + 0xA); i++) {
        if (*(s16 *)(p + 6) != 0) {
            *(s16 *)(p + 6) -= 1;
            *(u16 *)(p + 0xA) += *(u16 *)((u8 *)arg0 + 0);
            *(u16 *)(p + 0) += *(u16 *)(p + 8);
            *(u16 *)(p + 2) += *(u16 *)(p + 0xA);
            *(u16 *)(p + 4) += *(u16 *)(p + 0xC);
            if (*(s16 *)(p + 2) > 0) {
                v = -*(s16 *)(p + 0xA) * 2 / 3;
                *(s16 *)(p + 0xA) = v;
                v = (s16)v;
                if (v < 0) {
                    v = -v;
                }
                if (v < 4) {
                    *(s16 *)(p + 6) = 0;
                }
            }
            update(p, arg1);
        }
        p += *(s16 *)((u8 *)arg0 + 0xC);
    }
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_spark_copy_part_matrix (src/decomp/src/menu/arena_scene_graph_and_opponent.c:1969). */
#ifndef XM_S_arena_spark_copy_part_matrix
#define XM_S_arena_spark_copy_part_matrix
void xm_arena_spark_copy_part_matrix(Node *model, s16 part, MATRIX *out) __asm__("func_8008DBC0");
#endif
void func_8008DBC0(Node *model, s16 part, MATRIX *out) {
    MATRIX unused; 

    *out = ((ModelSet *)model->data)->nodes[part]->unk4C;
}
#endif
extern void *D_80092834;
void *func_8008D3F4(s32, s32);
void func_8008D5C0(void *, s32);

void func_8008DC28(void) {
    s32 state;
    void *sprite;

    sprite = func_8008D3F4(1, 0);
    *(u8 *)((u8 *)sprite + 0x74) = 0xFF;
    *(u8 *)((u8 *)sprite + 0x75) = 0xA0;
    *(s8 *)((u8 *)sprite + 0x76) = 0x70;
    func_8008D5C0(sprite, 0x100);
    state = 4;
    *(s16 *)sprite = state;
    *(s16 *)((u8 *)sprite + 0x44) = 0x60;
    *(s16 *)((u8 *)sprite + 0x4A) = 0x60;
    *(s16 *)((u8 *)sprite + 0x48) = state;
    *(s16 *)((u8 *)sprite + 0x68) = 0;
    *(s16 *)((u8 *)sprite + 0x6A) = 0x20;
    D_80092834 = sprite;
}


void func_8008DCA8(s32 arg0) {
    D_80092838 = arg0;
}

#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): updateBattlingSparkBurst (src/battling/bl_8008C7C0.c). */
#ifndef XM_T_OTag
#define XM_T_OTag
typedef u64 OTag;
#endif
#ifndef XM_T_BattlingNode
#define XM_T_BattlingNode
typedef struct BattlingNode {
     s32 m0_type;
     void* m4_data;           
     void (*m8_callback)(void);
     MATRIX mC_worldMatrix;
     SVECTOR m2C_position;
     VECTOR m34_translation;  
     SVECTOR m44_rotation;    
     MATRIX m4C_localMatrix;
     MATRIX m6C_baseMatrix;
     struct BattlingNode* m8C_parent;
     struct BattlingNode* m90_next;
     struct BattlingNode* m94_child;
     s32 m98;
} BattlingNode;
#endif
#ifndef XM_T_BattlingParticle
#define XM_T_BattlingParticle
typedef struct BattlingParticle {
     s16 m0_x;
     s16 m2_y;
     s16 m4_z;
     s16 m6_life;             
     s16 m8_vx;
     s16 mA_vy;
     s16 mC_vz;
} BattlingParticle;
#endif
#ifndef XM_T_BattlingParticleResetFn
#define XM_T_BattlingParticleResetFn
typedef void (*BattlingParticleResetFn)(void* particle);
#endif
#ifndef XM_T_BattlingParticleRenderFn
#define XM_T_BattlingParticleRenderFn
typedef void (*BattlingParticleRenderFn)(void* particle, OTag* ot);
#endif
#ifndef XM_T_BattlingParticleInitFn
#define XM_T_BattlingParticleInitFn
typedef void (*BattlingParticleInitFn)(void* particle, struct BattlingEmitter* e);
#endif
#ifndef XM_T_BattlingEmitterShapeFn
#define XM_T_BattlingEmitterShapeFn
typedef void (*BattlingEmitterShapeFn)(struct BattlingEmitter* e, SVECTOR* out);
#endif
#ifndef XM_T_BattlingEmitter
#define XM_T_BattlingEmitter
typedef struct BattlingEmitter {
     s16 m0_gravity;
     s16 m2;
     s16 m4;
     s16 m6;
     s16 m8;
     s16 mA_count;
     s16 mC_particleSize;     
     s16 mE;
     BattlingParticle* m10_particles;
     s16 m14_x;
     s16 m16_y;
     s16 m18_z;
     s16 m1A;
     SVECTOR m1C_origin;
     s16 m24_size[3];
     s16 m2A;
     s16 m2C_halfSize[3];
     s16 m32;
     SVECTOR m34_rotation;
     SVECTOR m3C_rotation2;
     s16 m44_spread;
     s16 m46;
     s16 m48_speedMin;
     s16 m4A_speedRange;
     s16 m4C;
     s16 m4E_shape;
     BattlingParticleResetFn m50_reset;
     BattlingParticleRenderFn m54_render;
     BattlingParticleInitFn m58_init;
     BattlingEmitterShapeFn m5C_shape;
     void* m60_groundBounce;               
     s16 m64;
     s16 m66_type;
     s16 m68;
     s16 m6A_life;
     s32 m6C;
     s32 m70;
     u8 m74_r;
     u8 m75_g;
     u8 m76_b;
     u8 m77;
     s32 m78;
} BattlingEmitter;
#endif
#ifndef XM_T_gte_SetTransMatrix
#define XM_T_gte_SetTransMatrix
#define gte_SetTransMatrix(r0)                                                \
    __asm__ volatile("lw $12, 20(%0); lw $13, 24(%0); ctc2 $12, $5; "         \
                     "lw $14, 28(%0); ctc2 $13, $6; ctc2 $14, $7"              \
                     : : "r"(r0) : "$12", "$13", "$14")
#endif
#ifndef XM_T_gte_SetRotMatrix
#define XM_T_gte_SetRotMatrix
#define gte_SetRotMatrix(r0)                                                  \
    __asm__ volatile("lw $12, 0(%0); lw $13, 4(%0); ctc2 $12, $0; "           \
                     "ctc2 $13, $1; lw $12, 8(%0); lw $13, 12(%0); "           \
                     "lw $14, 16(%0); ctc2 $12, $2; ctc2 $13, $3; ctc2 $14, $4" \
                     : : "r"(r0) : "$12", "$13", "$14")
#endif
#ifndef XM_S_updateBattlingSparkBurst
#define XM_S_updateBattlingSparkBurst
void xm_updateBattlingSparkBurst(OTag* arg0, BattlingNode* gear, MATRIX* m, VECTOR* pos) __asm__("func_8008DCB8");
#endif
#ifndef XM_S_battlingSparkEmitter
#define XM_S_battlingSparkEmitter
extern BattlingEmitter * xm_battlingSparkEmitter __asm__("D_80092834");
#endif
#ifndef XM_S_battlingSparkBurst
#define XM_S_battlingSparkBurst
extern s32 xm_battlingSparkBurst __asm__("D_80092838");
#endif
#ifndef XM_S_getBattlingBoneMatrix
#define XM_S_getBattlingBoneMatrix
void xm_getBattlingBoneMatrix(BattlingNode* gear, s16 bone, MATRIX* out) __asm__("func_8008DBC0");
#endif
#ifndef XM_S_MatrixNormal
#define XM_S_MatrixNormal
MATRIX* xm_MatrixNormal(MATRIX* m, MATRIX* n) __asm__("func_80048E94");
#endif
#ifndef XM_S_emitBattlingParticles
#define XM_S_emitBattlingParticles
void xm_emitBattlingParticles(BattlingEmitter* pool, MATRIX* m, s32 count) __asm__("func_8008D680");
#endif
#ifndef XM_S_updateBattlingEmitterParticles
#define XM_S_updateBattlingEmitterParticles
void xm_updateBattlingEmitterParticles(BattlingEmitter* pool, OTag* arg1, MATRIX* m) __asm__("func_8008DA48");
#endif
void func_8008DCB8(OTag* arg0, BattlingNode* gear, MATRIX* m, VECTOR* pos) {
    MATRIX norm;
    MATRIX bone;
    BattlingEmitter* pool;

    pool = xm_battlingSparkEmitter;
    if (xm_battlingSparkBurst >= 0x10) {
        xm_getBattlingBoneMatrix(gear, 0x27, &bone);
        xm_MatrixNormal(&bone, &norm);
        norm.t[2] = 0;
        norm.t[1] = 0;
        norm.t[0] = 0;
        pool->m14_x = pos->vx;
        pool->m16_y = pos->vy;
        pool->m18_z = pos->vz;
        pool->m3C_rotation2.vx = 0;
        pool->m3C_rotation2.vy = 0;
        pool->m3C_rotation2.vz = 0;
        pool->m34_rotation.vx = 0;
        pool->m34_rotation.vy = 0;
        pool->m34_rotation.vz = 0x800;
        xm_emitBattlingParticles(pool, &norm, xm_battlingSparkBurst >> 4);
        xm_battlingSparkBurst -= 4;
    }
    gte_SetTransMatrix(m);
    gte_SetRotMatrix(m);
    xm_updateBattlingEmitterParticles(pool, arg0, m);
}
#endif
INCLUDE_ASM("asm/battling/nonmatchings/main", func_8008DDFC);
INCLUDE_ASM("asm/battling/nonmatchings/main", func_8008DE54);

void func_8008DF30(void) {
    D_80092844 = 0;
    D_8009283C = 0;
    D_80092840 = 0;
}

s32 LoadImage();
/* RECT layout (libgpu): x, y, w, h. */
typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} BattlingRect;
extern u16 D_80091CE0[];

void func_8008DF50(void) {
    BattlingRect rect;
    u16 *a;
    u16 *b;
    u16 *clut;
    s32 i;

    if (D_80092844 == 0) {
        D_80092844 = HeapAlloc(0x1500, 1);
        D_8009283C = HeapAlloc(0x2BC0, 1);
        D_80092840 = HeapAlloc(0x2BC0, 1);
    }
    i = 0x1570;
    a = (u16 *)D_8009283C + 0x1570;
    b = (u16 *)D_80092840 + 0x1570;
    do {
        *b = 0;
        *a = 0;
        a--;
        i--;
        b--;
    } while (i != -1);
    for (i = 0; i < 0x1500; i++) {
        ((u8 *)D_80092844)[i] = 0;
    }
    for (i = 0, clut = D_80091CE0; i < 0x100; i++) {
        *clut |= 0x8000;
        clut++;
    }
    rect.y = 0x1FD;
    rect.w = 0xFF;
    rect.x = 0;
    rect.h = 1;
    LoadImage(&rect, D_80091CE0);
}


void func_8008E064(void) {
    if (D_80092844 != 0) {
        HeapDelayedFree(D_80092844, 2);
        HeapDelayedFree(D_8009283C, 2);
        HeapDelayedFree(D_80092840, 2);
        D_80092844 = 0;
        D_8009283C = 0;
        D_80092840 = 0;
    }
}

void func_8008E0C8(void) {
    s32 n;
    u32 *src;
    u32 *dst;
    u32 v;
    u8 *out;

    n = 0xA7F;
    out = (u8 *)D_80092844;
    dst = (u32 *)D_8009283C;
    src = (u32 *)D_80092840;
    do {
        v = *src++;
        n--;
        *dst++ = v;
        *out++ = v;
        *out++ = v >> 16;
    } while (n != -1);
}
INCLUDE_ASM("asm/battling/nonmatchings/main", func_8008E120);
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_glow_draw_shade_tile (src/decomp/src/menu/arena_scene_graph_and_opponent.c:2154). */
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
#ifndef XM_T_getcode
#define XM_T_getcode
#define getcode(p) (u_char)(((P_TAG *)(p))->code)
#endif
#ifndef XM_T_setSemiTrans
#define XM_T_setSemiTrans
#define setSemiTrans(p, abe) \
    ((abe) ? setcode(p, getcode(p) | 0x02) : setcode(p, getcode(p) & ~0x02))
#endif
#ifndef XM_T_DR_MODE
#define XM_T_DR_MODE
typedef struct {
    u_long tag;
    u_long code[2];
} DR_MODE;
#endif
#ifndef XM_S_arena_glow_draw_shade_tile
#define XM_S_arena_glow_draw_shade_tile
void xm_arena_glow_draw_shade_tile(u32 *ot, s32 level, s32 subtract) __asm__("func_8008E2B8");
#endif
#ifndef XM_S_arena_glow_shade_tiles
#define XM_S_arena_glow_shade_tiles
extern TILE xm_arena_glow_shade_tiles[2] __asm__("D_80096DE0");
#endif
#ifndef XM_S_AddPrim
#define XM_S_AddPrim
void xm_AddPrim(void *ot, void *p) __asm__("AddPrim");
#endif
#ifndef XM_S_SetDrawMode
#define XM_S_SetDrawMode
void xm_SetDrawMode(DR_MODE *p, int dfe, int dtd, int tpage, RECT *tw) __asm__("SetDrawMode");
#endif
#ifndef XM_S_arena_glow_blend_modes
#define XM_S_arena_glow_blend_modes
extern DR_MODE xm_arena_glow_blend_modes[2] __asm__("D_80096E00");
#endif
#ifndef XM_S_GetTPage
#define XM_S_GetTPage
u_short xm_GetTPage(int tp, int abr, int x, int y) __asm__("GetTPage");
#endif
void func_8008E2B8(u32 *ot, s32 level, s32 subtract) {
    TILE *tile = &xm_arena_glow_shade_tiles[xm_arena_draw_buffer_index];

    *(u32 *)&tile->r0 = level | (level << 8) | (level << 16) | 0x60000000;
    setlen(tile, 3);
    *(u32 *)&tile->x0 = 0;
    *(u32 *)&tile->w = 0xDA0140;
    setSemiTrans(tile, 1);
    xm_AddPrim(ot, tile);
    if (subtract) {
        xm_SetDrawMode(&xm_arena_glow_blend_modes[xm_arena_draw_buffer_index], 0, 1, xm_GetTPage(0, 2, 0, 0), NULL);
    } else {
        xm_SetDrawMode(&xm_arena_glow_blend_modes[xm_arena_draw_buffer_index], 0, 1, xm_GetTPage(0, 1, 0, 0), NULL);
    }
    xm_AddPrim(ot, &xm_arena_glow_blend_modes[xm_arena_draw_buffer_index]);
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_glow_draw (src/decomp/src/menu/arena_scene_graph_and_opponent.c:2174). */
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
#ifndef XM_T_setShadeTex
#define XM_T_setShadeTex
#define setShadeTex(p, tge) \
    ((tge) ? setcode(p, getcode(p) | 0x01) : setcode(p, getcode(p) & ~0x01))
#endif
#ifndef XM_S_arena_glow_draw
#define XM_S_arena_glow_draw
void xm_arena_glow_draw(u32 *ot, s32 level, s32 brighten) __asm__("func_8008E3CC");
#endif
#ifndef XM_S_arena_glow_quads
#define XM_S_arena_glow_quads
extern POLY_FT4 xm_arena_glow_quads[2] __asm__("D_80096D90");
#endif
#ifndef XM_S_GetClut
#define XM_S_GetClut
u_short xm_GetClut(int x, int y) __asm__("GetClut");
#endif
#ifndef XM_S_LoadImage
#define XM_S_LoadImage
int xm_LoadImage(RECT *rect, u_long *p) __asm__("LoadImage");
#endif
#ifndef XM_S_arena_glow_image
#define XM_S_arena_glow_image
extern u8 *xm_arena_glow_image __asm__("D_80092844");
#endif
void func_8008E3CC(u32 *ot, s32 level, s32 brighten) {
    POLY_FT4 *quad = &xm_arena_glow_quads[xm_arena_draw_buffer_index];
    TILE *tile;
    RECT rect;
    s32 shade;

    setlen(quad, 9);
    shade = level * 2 / 3;
    *(u32 *)&quad->r0 = shade | (shade << 8) | (shade << 16) | 0x2C000000;
    setShadeTex(quad, shade == 0x80);
    *(u32 *)&quad->x0 = 0;
    *(u32 *)&quad->x1 = 0x140;
    *(u32 *)&quad->x2 = 0xDA0000;
    *(u32 *)&quad->x3 = 0xDA0140;
    *(u16 *)&quad->u0 = 0;
    *(u16 *)&quad->u1 = 0x6F;
    *(u16 *)&quad->u2 = 0x2A00;
    *(u16 *)&quad->u3 = 0x2A6F;
    setSemiTrans(quad, 1);
    quad->tpage = xm_GetTPage(1, 1, 0x140, 0x100);
    quad->clut = xm_GetClut(0, 0x1FD);
    xm_AddPrim(ot, quad);
    rect.x = 0x140;
    rect.y = 0x100;
    rect.w = 0x38;
    rect.h = 0x2B;
    xm_LoadImage(&rect, (u_long *)xm_arena_glow_image);
    if (brighten) {
        tile = &xm_arena_glow_shade_tiles[xm_arena_draw_buffer_index];
        if (level > 0x80) {
            shade = level * 2;
            *(u32 *)&tile->r0 = shade | (shade << 8) | (shade << 16) | 0x60000000;
            setlen(tile, 3);
            *(u32 *)&tile->x0 = 0;
            *(u32 *)&tile->w = 0xDA0140;
            setSemiTrans(tile, 1);
            xm_AddPrim(ot, tile);
        }
    }
    xm_SetDrawMode(&xm_arena_glow_blend_modes[xm_arena_draw_buffer_index], 0, 1, xm_GetTPage(0, 2, 0, 0), NULL);
    xm_AddPrim(ot, &xm_arena_glow_blend_modes[xm_arena_draw_buffer_index]);
}
#endif
void func_80039FF8();
/* D_80096EA0: 4 x 0x24-byte sound slots (+1 active, +2 voice, +3 owner,
 * +4 u16 age, +8 sound id, +0xC voice mask, +0x10 VECTOR pos, +0x20 pos src). */
extern u8 D_80096EA0[];

void func_8008E620(void) {
    s32 mask;
    u32 i;
    u8 *slot;

    mask = 0x300;
    func_80039FF8();
    for (i = 0; i < 4; i++) {
        slot = D_80096EA0 + i * 0x24;
        *(s32 *)(slot + 0xC) = mask;
        mask <<= 2;
        slot[1] = 0;
        *(s16 *)(slot + 4) = 0;
        slot[2] = i * 2;
    }
}

extern u8 D_80096EA0[];

void func_8008E67C(void) {
    if (*(u16 *)(D_80096EA0 + 4) != 0xFFFF) {
        *(u16 *)(D_80096EA0 + 4) += 1;
    }
    if (*(u16 *)(D_80096EA0 + 0x28) != 0xFFFF) {
        *(u16 *)(D_80096EA0 + 0x28) += 1;
    }
    if (*(u16 *)(D_80096EA0 + 0x4C) != 0xFFFF) {
        *(u16 *)(D_80096EA0 + 0x4C) += 1;
    }
    if (*(u16 *)(D_80096EA0 + 0x70) != 0xFFFF) {
        *(u16 *)(D_80096EA0 + 0x70) += 1;
    }
}


void func_8008E6F8(void *arg0) {
    s32 *var_v0;
    u8 temp_v1;

    temp_v1 = *(u8 *)((s8*)(arg0) + 0x909);
    switch (temp_v1) {                              /* irregular */
    case 9:
        var_v0 = &D_80091FA0;
        break;
    case 29:
        var_v0 = &D_80091F60;
        break;
    case 27:
        var_v0 = &D_80091F80;
        break;
    case 36:
        var_v0 = &D_80091F70;
        break;
    default:
        var_v0 = &D_80091F90;
        break;
    }
    *(s32 **)((s8*)(arg0) + 0x1664) = var_v0;
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_sound_start_voice (src/decomp/src/menu/arena_scene_graph_and_opponent.c:2273). */
#ifndef XM_T_SoundVoice
#define XM_T_SoundVoice
typedef struct {
    u8 mode;        
    u8 active;
    u8 voice;       
    u8 unk3;
    u16 age;        
    s32 sound;      
    s32 mask;       
    VECTOR pos;     
    VECTOR *follow; 
} SoundVoice;
#endif
#ifndef XM_S_arena_sound_start_voice
#define XM_S_arena_sound_start_voice
void xm_arena_sound_start_voice(s32 sound, s32 mode, VECTOR *pos, s32 tag) __asm__("func_8008E78C");
#endif
#ifndef XM_S_arena_sound_voices
#define XM_S_arena_sound_voices
extern SoundVoice xm_arena_sound_voices[4] __asm__("D_80096EA0");
#endif
#ifndef XM_S_arena_sound_age_voices
#define XM_S_arena_sound_age_voices
void xm_arena_sound_age_voices(void) __asm__("func_8008E67C");
#endif
#ifndef XM_S_sound_play_effect_on_channel_volume_pan
#define XM_S_sound_play_effect_on_channel_volume_pan
void xm_sound_play_effect_on_channel_volume_pan(s32 effect, s16 channel, s16 volume, s16 pan) __asm__("func_80039F9C");
#endif
void func_8008E78C(s32 sound, s32 mode, VECTOR *pos, s32 tag) {
    s32 oldest = 0;
    SoundVoice *chosen = &xm_arena_sound_voices[3];
    SoundVoice *voice;
    s32 i;

    for (i = 0; i < 4; i++) {
        voice = &xm_arena_sound_voices[i];
        if (voice->active == 0) {
            chosen = voice;
            break;
        }
        if (mode == 0 && voice->mode == 0) {
            chosen = voice;
            break;
        }
        if (oldest < voice->age) {
            oldest = voice->age;
            chosen = voice;
        }
    }
    xm_arena_sound_age_voices();
    voice = chosen;
    voice->mode = mode;
    voice->sound = sound;
    voice->active = 1;
    voice->unk3 = tag;
    voice->follow = pos;
    if (pos != NULL) {
        voice->pos = *pos;
    }
    voice->age = 0;
    if (mode == 0) {
        xm_sound_play_effect_on_channel_volume_pan(voice->sound, voice->voice, 0x7F, 0x40);
    }
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_sound_update_voices (src/decomp/src/menu/arena_scene_graph_and_opponent.c:2312). */
#ifndef XM_T_gte_stsz
#define XM_T_gte_stsz
#define gte_stsz(r0) __asm__ volatile("swc2 $19, 0(%0)" : : "r"(r0) : "memory")
#endif
#ifndef XM_S_arena_sound_update_voices
#define XM_S_arena_sound_update_voices
void xm_arena_sound_update_voices(void) __asm__("func_8008E8B0");
#endif
#ifndef XM_S_sound_set_effect_pan_on_channel
#define XM_S_sound_set_effect_pan_on_channel
void xm_sound_set_effect_pan_on_channel(s32 sound, s32 pan) __asm__("func_8003A55C");
#endif
#ifndef XM_S_sound_set_effect_volume_on_channel
#define XM_S_sound_set_effect_volume_on_channel
void xm_sound_set_effect_volume_on_channel(s32 sound, s32 volume) __asm__("func_8003A344");
#endif
void func_8008E8B0(void) {
    SVECTOR v;
    SVECTOR screen;
    s32 sz;
    SoundVoice *voice;
    s32 volume;
    s32 x;
    s32 pan;
    s32 i;

    for (i = 0; i < 4; i++) {
        voice = &xm_arena_sound_voices[i];
        if (voice->active && voice->mode != 0) {
            if (voice->mode == 1) {
                v.vx = voice->pos.vx;
                v.vy = voice->pos.vy;
                v.vz = voice->pos.vz;
            } else {
                v.vx = voice->follow->vx;
                v.vy = voice->follow->vy;
                v.vz = voice->follow->vz;
            }
            v.vx -= xm_arena_view_origin.vx;
            v.vy -= xm_arena_view_origin.vy;
            v.vz -= xm_arena_view_origin.vz;
            gte_ldv0(&v);
            gte_rtps();
            gte_stsxy(&screen);
            gte_stsz(&sz);
            x = screen.vx;
            if (x < 0) {
                x = 0;
            }
            if (x > 0x140) {
                x = 0x140;
            }
            volume = (0x3000 - sz) * 0x7F / 0x3000;
            if (volume < 0x28) {
                volume = 0x28;
            }
            if (volume > 0x7F) {
                volume = 0x7F;
            }
            pan = x * 0x7F / 0x140;
            if (voice->age != 0) {
                xm_sound_set_effect_pan_on_channel(voice->voice, pan);
                xm_sound_set_effect_volume_on_channel(voice->voice, volume);
            } else {
                xm_sound_play_effect_on_channel_volume_pan(voice->sound, voice->voice, volume, pan);
            }
        }
    }
    xm_arena_sound_age_voices();
}
#endif
s32 func_8003A5D0(s32);
extern u8 D_80096EA0[];

void func_8008EADC(void) {
    s32 i;
    u8 *slot;

    for (i = 0; i < 4; i++) {
        slot = D_80096EA0 + i * 0x24;
        if (!(func_8003A5D0(*(s32 *)(slot + 8)) & *(s32 *)(slot + 0xC))) {
            slot[1] = 0;
        }
    }
    func_8008E67C();
}

/* D_80059488 is the VSync frame counter. Retail schedules its load right
 * after `lui $a0` (ahead of the addu), which only a volatile-declared extern
 * reproduces; a `*(volatile s32 *)&D_80059488` cast at the site does not. */
extern volatile s32 D_80059488;

void func_8008EB4C(s32 arg0) {
    if (arg0 != 0) {
        func_8008E78C(arg0 + 0x60000, 0, 0, D_80059488);
    }
}

void func_8008EB88(void *arg0, s32 arg1, s32 arg2, s32 arg3) {
    if (arg1 != 0) {
        func_8008E78C(arg1 + 0x60000, arg3, arg2,
                      (arg1 & 0x7F) | ((*(u32 *)((u8 *)arg0 + 0xD0) >> 0x14) & 0x80));
    }
}
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_sound_play_command_sound (src/decomp/src/menu/arena_scene_graph_and_opponent.c:2397). */
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
#ifndef XM_S_arena_sound_play_command_sound
#define XM_S_arena_sound_play_command_sound
void xm_arena_sound_play_command_sound(Actor *owner, s32 index, VECTOR *pos, s32 mode) __asm__("func_8008EBD0");
#endif
#ifndef XM_S_arena_sound_command_effect_pairs
#define XM_S_arena_sound_command_effect_pairs
extern u8 xm_arena_sound_command_effect_pairs[128] __asm__("D_80091EE0");
#endif
void func_8008EBD0(Actor *owner, s32 index, VECTOR *pos, s32 mode) {
    s32 entry;

    if (index == 0) {
        index = xm_rand() % 6 + 1;
    }
    entry = owner->sounds[index];
    if (entry != 0xFF) {
        index = xm_arena_sound_command_effect_pairs[entry * 2];
        if (index != 0) {
            xm_arena_sound_start_voice(index | 0x60000, mode, pos, (index & 0x7F) | ((owner->flags >> 20) & 0x80));
        }
        index = xm_arena_sound_command_effect_pairs[entry * 2 + 1];
        if (index != 0) {
            xm_arena_sound_start_voice(index | 0x60000, mode, pos, (index & 0x7F) | ((owner->flags >> 20) & 0x80));
        }
    }
}
#endif
void func_8003A20C(s32);
extern u8 D_80096EA0[];

void func_8008ECEC(s32 arg0) {
    s32 i;
    u8 *slot;

    for (i = 0; i < 4; i++) {
        slot = D_80096EA0 + i * 0x24;
        if (slot[1] != 0 && slot[3] == (arg0 & 0xFF)) {
            func_8003A20C(slot[2]);
            slot[1] = 0;
        }
    }
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_sound_stop_command_sound (src/decomp/src/menu/arena_scene_graph_and_opponent.c:2432). */
#ifndef XM_S_arena_sound_stop_command_sound
#define XM_S_arena_sound_stop_command_sound
s32 xm_arena_sound_stop_command_sound(Actor *owner, s32 index) __asm__("func_8008ED6C");
#endif
#ifndef XM_S_arena_sound_stop_tag
#define XM_S_arena_sound_stop_tag
void xm_arena_sound_stop_tag(u8 tag) __asm__("func_8008ECEC");
#endif
s32 func_8008ED6C(Actor *owner, s32 index) {
    s32 entry;

    entry = owner->sounds[index];
    if (entry != 0xFF) {
        index = xm_arena_sound_command_effect_pairs[entry * 2];
        if (index != 0) {
            xm_arena_sound_stop_tag((index & 0x7F) | ((owner->flags >> 20) & 0x80));
        }
        index = xm_arena_sound_command_effect_pairs[entry * 2 + 1];
        if (index != 0) {
            xm_arena_sound_stop_tag((index & 0x7F) | ((owner->flags >> 20) & 0x80));
        }
    }
}
#endif
s32 func_8008B650(s32, s16, s32, s32);

void func_8008EE1C(void *arg0, s16 arg1, s16 arg2) {
    s32 dec;
    s32 inc;
    s32 flag;
    s32 i;
    s32 unused[4];

    dec = *(s32 *)((u8 *)arg0 + 0x9C);
    inc = *(s32 *)((u8 *)arg0 + 0x98);
    if (*(s32 *)((u8 *)arg0 + 0xD0) & 0x100) {
        flag = 0;
        dec = dec * 2 / 3;
    } else {
        flag = 1;
    }
    for (i = 0; i < 2; i++) {
        if (arg2 != 0 && flag != 0) {
            *(s32 *)((u8 *)arg0 + 0x48) += inc;
            if (*(s32 *)((u8 *)arg0 + 0x48) > arg2) {
                *(s32 *)((u8 *)arg0 + 0x48) = arg2;
            }
        } else {
            *(s32 *)((u8 *)arg0 + 0x48) -= dec;
            if (*(s32 *)((u8 *)arg0 + 0x48) < 0) {
                *(s32 *)((u8 *)arg0 + 0x48) = 0;
            }
        }
    }
    *(s32 *)((u8 *)arg0 + 0x58) = func_8008B650(*(s32 *)((u8 *)arg0 + 0x58), arg1, 0x40, flag);
    *(s16 *)((u8 *)arg0 + 0xCE) = 0;
}


void func_8008EF00(s32 arg0, void *arg1) {
    func_800767C8();
    *(s16 *)((s8*)(arg1) + 4) = 0x3C;
}

void func_8008EF30(s32 arg0, void *arg1, s32 arg2) {
    s16 duration;

    func_8008FE80(arg0);
    duration = 0x1E;
    if (arg2 == 0) {
        duration = 0xA;
    }
    *(s16 *)((u8 *)arg1 + 4) = duration;
}

#ifndef XENO_PC_PORT
void func_8008EF74(s32 arg0, void *arg1) {
    func_8007639C(arg0, 3);
    *(s16 *)((s8*)(arg1) + 4) = 0x3C;
}
#endif

typedef struct {
    u32 pad0 : 13;
    u32 flag13 : 1;
    u32 pad14 : 18;
} BattlingAiFlags;

void func_8008EFA8(void *arg0, void *arg1) {
    BattlingAiFlags *flags;

    flags = (BattlingAiFlags *)((u8 *)arg1 + 0x2C);
    flags->flag13 ^= 1;
    if (flags->flag13) {
        *(s32 *)((u8 *)arg0 + 0xD0) |= 2;
        *(s16 *)((u8 *)arg1 + 4) = 0x3C;
    } else {
        *(s32 *)((u8 *)arg0 + 0xD0) &= ~2;
        *(s32 *)((u8 *)arg0 + 0xD0) &= ~0x38;
        *(s16 *)((u8 *)arg1 + 4) = 0x1E;
    }
}

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_brain_command_magic_jumper (src/decomp/src/menu/arena_scene_graph_and_opponent.c:2512). */
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
#ifndef XM_S_arena_brain_command_magic_jumper
#define XM_S_arena_brain_command_magic_jumper
void xm_arena_brain_command_magic_jumper(Actor *actor, Brain *brain) __asm__("func_8008F014");
#endif
#ifndef XM_S_arena_actor_queue_input
#define XM_S_arena_actor_queue_input
void xm_arena_actor_queue_input(Actor *actor, u8 input) __asm__("func_8007639C");
#endif
void func_8008F014(Actor *actor, Brain *brain) {
    xm_arena_actor_queue_input(actor, 4);
    xm_arena_actor_queue_input(actor, 3);
    brain->timer = 0x3C;
}
#endif

#ifndef XENO_PC_PORT
void func_8008F060(s32 arg0, void *arg1) {
    func_8007639C(arg0, 4);
    *(s16 *)((s8*)(arg1) + 4) = 0x3C;
}
#endif

#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_brain_command_run_away (src/decomp/src/menu/arena_scene_graph_and_opponent.c:2526). */
#ifndef XM_S_arena_brain_command_run_away
#define XM_S_arena_brain_command_run_away
void xm_arena_brain_command_run_away(Actor *actor, Brain *brain) __asm__("func_8008F094");
#endif
#ifndef XM_S_arena_actors_flat_distance
#define XM_S_arena_actors_flat_distance
extern s32 xm_arena_actors_flat_distance __asm__("D_8009284C");
#endif
void func_8008F094(Actor *actor, Brain *brain) {
    if (xm_arena_actors_flat_distance > 0x800) {
        brain->timer = 1;
        brain->unkC = 0;
    } else if (--brain->timer == -1) {
        brain->unkA = xm_rand() % 0x600 + 0x500;
        brain->timer = xm_rand() % 50 + 10;
        brain->unkC = 0xFF;
    }
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): arena_brain_command_give_chase (src/decomp/src/menu/arena_scene_graph_and_opponent.c:2539). */
#ifndef XM_S_arena_brain_command_give_chase
#define XM_S_arena_brain_command_give_chase
void xm_arena_brain_command_give_chase(Actor *actor, Brain *brain) __asm__("func_8008F17C");
#endif
void func_8008F17C(Actor *actor, Brain *brain) {
    if (xm_arena_actors_flat_distance < 0x100) {
        brain->timer = 1;
        brain->unkC = 0;
    } else if (--brain->timer == -1) {
        brain->unkA = xm_rand() % 0x600 - 0x300;
        brain->timer = xm_rand() % 120 + 10;
        brain->unkC = 0xFF;
    }
}
#endif

void func_8008F260(s32 arg0, void *arg1, s8 arg2) {
    *(s8 *)((s8*)(arg1) + 0xF) = arg2;
    func_80090E10();
}

INCLUDE_ASM("asm/battling/nonmatchings/main", func_8008F280);

s32 func_8008F4F4(void *arg0, s32 arg1) {
    return ((*(s16 *)((s8*)(arg0) + 0xBC) * arg1) / 255) < *(s16 *)((s8*)(arg0) + 0xB4);
}

s32 func_8008F530(void *arg0, s32 arg1) {
    if (arg1 != 0) {
        return func_80073DE4(arg0, *(s16 *)((u8 *)arg0 + 0xBE));
    }
    return *(s16 *)((u8 *)arg0 + 0xB6) < 0x1000 - *(s16 *)((u8 *)arg0 + 0xBE);
}

s32 func_8008F570(void *arg0) {
    return 0x1000 - *(s16 *)((s8*)(arg0) + 0xBE);
}

s32 func_8008F580(void *arg0) {
    s16 temp_v1;
    s32 temp_a1;

    temp_a1 = *(s32 *)((s8*)(*(void **)((s8*)(arg0) + 0x15FC)) + 0x24);
    temp_v1 = *(s16 *)((s8*)(arg0) + 0xB6);
    if ((temp_a1 - 0x200) >= temp_v1) {
        return 2;
    }
    return temp_a1 >= temp_v1;
}

#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): shouldBattlingAiFireEther (src/battling/bl_8008C7C0.c). */
#ifndef XM_T_BattlingMove
#define XM_T_BattlingMove
typedef struct BattlingMove {
     u8 m0_anim;
     u8 m1_lunge;
     u8 m2_effects;            
     u8 m3_alive;
} BattlingMove;
#endif
#ifndef XM_T_BattlingGearFlags
#define XM_T_BattlingGearFlags
typedef struct BattlingGearFlags {
    u32 b0 : 1;
    u32 b1 : 1;                         
    u32 b2 : 1;
    u32 b3_combo : 3;                   
    u32 b6 : 1;                         
    u32 b7 : 3;
    u32 b10_burning : 1;                
    u32 b11_animEnd : 1;
    u32 b12 : 1;
    u32 b13 : 2;
    u32 b15_scriptTurn : 1;             
    u32 b16 : 1;
    u32 b17_facing : 1;
    u32 b18 : 1;
    u32 b19 : 1;
    u32 b20 : 3;
    u32 b23 : 1;
    u32 b24 : 1;
    u32 b25 : 1;
    u32 b26_effectStart : 1;            
    u32 b27_side : 1;                   
    u32 b28 : 1;
    u32 b29_ground : 2;                 
    u32 b31 : 1;
} BattlingGearFlags;
#endif
#ifndef XM_T_BattlingGearFlags2
#define XM_T_BattlingGearFlags2
typedef struct BattlingGearFlags2 {
    u32 b0_1 : 2;
    u32 b2_3 : 2;
    u32 b4 : 1;
    u32 b5 : 1;                         
    u32 b6 : 1;
    u32 b7_31 : 25;
} BattlingGearFlags2;
#endif
#ifndef XM_T_BattlingGearShape
#define XM_T_BattlingGearShape
typedef struct BattlingGearShape {
     VECTOR m0_points[4];     
     s16 m40_effect;
     u8 m42;
     u8 m43_part;
     union {                  
        u32 flags;
        struct {
            u32 b0 : 1;
            u32 b1 : 1;
            u32 b2 : 6;
            u32 state : 8;              
            u32 type : 8;
            u32 damage : 8;
        } bits;
        struct {
            u8 m44;
            u8 m45_state;
            u8 m46_type;
            u8 m47_damage;
        } b;
    } m44_hit;
     u8 m48_frame;            
     u8 m49[3];
     s32 m4C_handle;          
     BattlingMove* m50_move;   
} BattlingGearShape;
#endif
#ifndef XM_T_BattlingProjectile
#define XM_T_BattlingProjectile
typedef struct BattlingProjectile {
     VECTOR m0_position;
     VECTOR m10_prevPosition;
     SVECTOR m20_velocity;     
     SVECTOR m28_direction;    
     u8 m30_active;
     u8 m31_speed;
     u8 m32_drawType;          
     u8 m33_moveType;          
     s32 m34_distance;         
     s32 m38_handle;
     s16 m3C_damage;
     s16 m3E_homing;           
     s16 m40_life;
     u8 m42[0x2];
} BattlingProjectile;
#endif
#ifndef XM_T_BattlingGearParams
#define XM_T_BattlingGearParams
typedef struct BattlingGearParams {
     u8 m0_topPart;
     u8 m1_foot1Part;
     u8 m2_foot2Part;
     u8 m3_bodyRadius;
     s16 m4_foot1Vertex;
     s16 m6_foot2Vertex;
     s16 m8_topVertex;
     u8 mA_pad[2];
     u16 mC_speedScale;
     u8 mE_hiddenPartCount;
     u8 mF_moveScale;
     u8 m10_glowR;
     u8 m11_glowG;
     u8 m12_glowB;
     u8 m13_radius;          
     s16 m14_victoryPitch;                 
     s16 m16_victorySpin;
     s16 m18_victoryY;
     s16 m1A_victoryZ;
     s16 m1C_victoryScale;
     s16 m1E_victoryYaw;
     u16 m20_scale;
     u8 m22_pad[2];
     s16 m24_winCamAngle;
     s16 m26_winCamHeight;
     s16 m28_winCamTargetHeight;
     s16 m2A_winCamDistance;
     s16 m2C_winAnimLoops;
} BattlingGearParams;
#endif
#ifndef XM_T_BattlingReplayFrame
#define XM_T_BattlingReplayFrame
typedef struct BattlingReplayFrame {
     s16 m0_x;
     s16 m2_y;
     s16 m4_z;
     union {
        u16 flags;                      
        struct {
            s16 angle : 12;
            u16 b12 : 1;
            u16 b13 : 1;
            s16 b14 : 1;                
            u16 b15 : 1;
        } bits;
    } m6_angleFlags;
     u8 m8_anim;
     u8 m9_frameStep;
     u16 mA : 8;
     u16 mA_b8 : 1;
     u16 mA_pad : 7;
} BattlingReplayFrame;
#endif
#ifndef XM_T_BattlingAiState
#define XM_T_BattlingAiState
typedef struct BattlingAiState {
     struct BattlingGear* m0_gear;
     s16 m4_timer;
     u8 m6;
     u8 m7;
     u8 m8_state;
     u8 m9_repeatCount;
     s16 mA_turn;
     s16 mC_speed;
     u8 mE_boost;
     u8 mF_comLevel;
     s32 m10_aiParam0;
     s32 m14_aiParam1;
     s32 m18_aiParam2;
     s32 m1C_aiParam3;
     s32 m20_waitStep;
     s32 m24_gaugeLimit;
     s32 m28_retreatDistance;
     u32 m2C_b0_7 : 8;
     u32 m2C_b8 : 1;
     u32 m2C_b9 : 1;
     u32 m2C_b10 : 1;
     u32 m2C_b11 : 1;
     u32 m2C_b12 : 1;
     u32 m2C_b13 : 1;
     u32 m2C_pad14 : 2;
     u32 m2C_b16_23 : 8;      
     u32 m2C_pad24 : 8;
     s16 m30_hitsTaken;
} BattlingAiState;
#endif
#ifndef XM_T_BattlingGearDef
#define XM_T_BattlingGearDef
typedef struct BattlingGearDef {
     s16 m0_power;            
     s16 m2;
     s16 m4_unlockLevel;                  
     u8 m6_ai[4];             
     u8 mA_moves[14];         
     u8 m18_etherPower;
     u8 m19[7];
} BattlingGearDef;
#endif
#ifndef XM_T_BattlingGear
#define XM_T_BattlingGear
typedef struct BattlingGear {
     VECTOR m0_position;
     VECTOR m10_velocity;
     VECTOR m20_knockback;
     VECTOR m30_lungeVelocity;
     s32 m40_moveSpeed;
     s32 m44_lungeSpeed;
     s32 m48_speed;          
     u8 m4C_animation;
     u8 m4D;
     u8 m4E_lastAnimation;
     u8 m4F_animSpeed;
     s16 m50_animFrame;      
     u8 m52_moveAnimSpeed;
     u8 m53;
     s32 m54_rotY;
     s32 m58_targetRotation; 
     BattlingNode* m5C_model; 
     BattlingNode* m60_node;  
     u8 m64_pad[0x8];
     VECTOR m6C_prevPosition;
     u8* m7C_animParams;
     BattlingMove* m80_moves; 
     BattlingMove* m84_move;  
     s32 m88_inputLeft;
     s32 m8C_inputUp;
     s32 m90_inputRight;
     s32 m94_inputDown;
     s32 m98_accel;
     s32 m9C_decel;
     u8 mA0_pad[0x8];
     s32 mA8;
     u8 mAC_pad[0x4];
     s32 mB0_groundY;
     s16 mB4_hp;
     s16 mB6_gauge;          
     s16 mB8_barHp;
     s16 mBA_damage;
     s16 mBC_maxHp;
     s16 mBE_gaugeCost;
     u8 mC0_hpBar;
     u8 mC1_bar;
     u8 mC2_flash;
     u8 mC3_backDashTimer;
     u8 mC4_moveState;
     u8 mC5_action;
     u8 mC6_prevMoveState;
     u8 mC7_pad;
     s16 mC8_comboTimer;
     s16 mCA_downTimer;
     s16 mCC_sideInputRotation;
     s16 mCE_inputRotation;
     union {
        u32 flags;                      


        BattlingGearFlags bits;
    } mD0_status;
     union {
        u32 flags;
        BattlingGearFlags2 bits;
    } mD4_cues;
     struct BattlingGear* mD8_opponent;
     u8 mDC_pad[0xC];
     s32 mE8_stun;
     u8 mEC_pad[0x4];
     s16 mF0_comboHits;
     s16 mF2_wins;           
     s32 mF4;
     u8 mF8_pad[0x4];
     s32 mFC_turnRotation;
     s32 m100_hitType;
     BattlingGearShape m104_shapes[16];
     s32 m644_moveDamage;
     s32 m648_baseRotation;
     BattlingProjectile m64C_projectiles[9];
     u8 m8B0_pad[0x44];
     s32 m8F4_nearestProjectileDist;
     BattlingProjectile* m8F8_nearestProjectile;
     BattlingGearParams* m8FC_params;
     s16* m900_eventOffsets;  
     u8* m904_hiddenParts;
     u8 m908_hiddenPartCount;
     u8 m909_gearId;         
     u8 m90A_flags;
     u8 m90B_dustTimer;
     s16 m90C_attackRotation;
     u8 m90E_holdAnimation;
     u8 m90F_pad;
     u8 m910_damageSfxTimer;
     u8 m911_partCount;
     u8 m912_count;
     u8 m913_pad;
     s16 m914_inputLock;
     s16 m916_stagger;
     u8 m918_glow;           
     u8 m919_pad[0x3];
     VECTOR m91C_lastHitPoint;
     VECTOR m92C_topPoint;
     VECTOR m93C_footPoint;
     VECTOR m94C_target;     
     VECTOR m95C_prevFootPoint;
     s16 m96C_prevFoot2Y;
     s16 m96E_prevFoot1Y;
     s32 m970_hitDamage;
     VECTOR m974_hitPoint;
     u8 m984_pad[0x14];
     s16 m998_stepStartFrame;
     s16 m99A_stepFrames;
     s16 m99C_blendSteps;
     s16 m99E_frameAccum;
     u8 m9A0_inputQueue[32];
     u8 m9C0_inputQueueWrite;
     u8 m9C1_inputQueueRead;
     u8 m9C2_inputQueueCount;
     u8 m9C3_move;           
     u8 m9C4_pad[0x8];
     BattlingReplayFrame m9CC_replay[256];
     BattlingReplayFrame* m15CC_replayCur;
     BattlingReplayFrame* m15D0_replayNext;
     CVECTOR m15D4_color;    
     s16 m15D8[7];
     u8 m15E6_pad[0x2];
     s16 m15E8;
     s16 m15EA;
     s16 m15EC;
     s16 m15EE;
     s16 m15F0_moveScale;
     s16 m15F2_speedScale;
     s16 m15F4_baseSpeedScale;
     s16 m15F6;
     s16 m15F8;
     u8 m15FA_pad[0x2];
     struct BattlingAiState* m15FC_ai;
     BattlingGearDef* m1600_def;
     POLY_FT4 m1604_shadow[2];
     s32 m1654_etherHpLoss;
     s32 m1658_boostHpLoss;
     s32 m165C_etherPresses;
     s32 m1660_attackPresses;
     u8* m1664_hitSounds;    
     s16 m1668_hitsTaken;
     u8 m166A_pad[0x2];
} BattlingGear;
#endif
#ifndef XM_S_shouldBattlingAiFireEther
#define XM_S_shouldBattlingAiFireEther
s32 xm_shouldBattlingAiFireEther(BattlingGear* gear, s32 arg1) __asm__("func_8008F5B4");
#endif
#ifndef XM_S_canBattlingGearUseEther
#define XM_S_canBattlingGearUseEther
s32 xm_canBattlingGearUseEther(BattlingGear* gear, s32 arg1) __asm__("func_8008F530");
#endif
#ifndef XM_S_isBattlingGearHpAbove
#define XM_S_isBattlingGearHpAbove
s32 xm_isBattlingGearHpAbove(BattlingGear* gear, s32 percent) __asm__("func_8008F4F4");
#endif
s32 xm_shouldBattlingAiFireEther(BattlingGear* gear, s32 arg1) __asm__("func_8008F5B4");
s32 xm_shouldBattlingAiFireEther(BattlingGear* gear, s32 arg1) {
    BattlingAiState* ai;

    ai = gear->m15FC_ai;
    if ((xm_rand() & 0xFF) < (ai->m10_aiParam0 * 320) >> 4) {
        if (!(xm_canBattlingGearUseEther(gear, 0) != 0 || xm_isBattlingGearHpAbove(gear, 0xC0) != 0 ||
              (xm_rand() & 0xFF) < (ai->m1C_aiParam3 * 192) >> 4 ||
              (xm_isBattlingGearHpAbove(gear, 0x80) != 0 && gear->mD8_opponent->mB4_hp < gear->mB4_hp))) {
            return 0;
        }
    } else if ((xm_rand() & 0xFF) >= (ai->m1C_aiParam3 * 320) >> 4) {
        return 0;
    }
    if ((xm_rand() & 0xFF) < ai->m18_aiParam2) {
        if (gear->mD8_opponent->mC4_moveState == 4) {
            return 0;
        }
        if (xm_isBattlingGearHpAbove(gear->mD8_opponent, 0x20) != 0) {
            return 1;
        }
        if (gear->mD8_opponent->mB4_hp < gear->mB4_hp) {
            return 0;
        }
    }
    return 1;
}
#endif

s32 func_8008F720(void *arg0, s32 arg1) {
    s32 var_v0;
    void *temp_s1;

    temp_s1 = *(void **)((s8*)(arg0) + 0x15FC);
    if ((arg1 == 0) || ((rand() & 0xFF) >= *(s32 *)((s8*)(temp_s1) + 0x10)) || (var_v0 = 0, ((D_8009284C < 0x600) == 0))) {
        if (func_8008F580(arg0) == 0) {
            return (rand() & 0xFF) < *(s32 *)((s8*)(temp_s1) + 0x1C);
        }
        var_v0 = 1;
        /* Duplicate return node #6. Try simplifying control flow for better match */
        return var_v0;
    }
    return var_v0;
}

#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): rollBattlingAiDecisions (src/battling/bl_8008C7C0.c). */
#ifndef XM_S_rollBattlingAiDecisions
#define XM_S_rollBattlingAiDecisions
void xm_rollBattlingAiDecisions(BattlingAiState* ai) __asm__("func_8008F7B8");
#endif
void xm_rollBattlingAiDecisions(BattlingAiState* ai) __asm__("func_8008F7B8");
void xm_rollBattlingAiDecisions(BattlingAiState* ai) {
    s32 b;
    s32 r;

    ai->m2C_b9 = (xm_rand() & 0xFF) < ai->m10_aiParam0;
    b = 0;
    ai->m2C_b10 = (xm_rand() & 0xFF) < ai->m14_aiParam1;
    ai->m2C_b12 = xm_rand();
    ai->m2C_b11 = (xm_rand() & 0xFF) < ai->m18_aiParam2;
    if ((xm_rand() & 0xFF) < ai->m10_aiParam0) {
        r = xm_rand();
        r %= 10;
        b = r < 3;
    }
    ai->m2C_b8 = b;
    ai->m2C_b0_7 = xm_rand();
    ai->m30_hitsTaken = ai->m0_gear->m1668_hitsTaken;
}
#endif

void func_8008F900(void *arg0) {
    if (((*(s32 *)((s8*)(*(void **)((s8*)(arg0) + 0xD8)) + 0xD0) & 0x60000000) != 0x20000000) || !(rand() & 3)) {
        if (D_80092884 != 0) {
            func_800767C8(arg0);
            goto block_5;
        }
        if ((*(s32 *)((s8*)(arg0) + 0xD0) & 0x60000000) == 0x20000000) {
block_5:
            func_8007639C(arg0, 4);
        }
        func_8007639C(arg0, 3);
    }
}

#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): isBattlingGearInWater (src/battling/bl_8008C7C0.c). */
#ifndef XM_S_isBattlingGearInWater
#define XM_S_isBattlingGearInWater
s32 xm_isBattlingGearInWater(BattlingGear* gear) __asm__("func_8008F9B0");
#endif
#ifndef XM_S_getBattlingMapCell
#define XM_S_getBattlingMapCell
u32 xm_getBattlingMapCell(VECTOR* pos) __asm__("func_800828C4");
#endif
s32 func_8008F9B0(BattlingGear* gear) {
    VECTOR v;

    v = gear->m0_position;
    v.vx -= 0x3F80;
    v.vz -= 0x3F80;
    if (v.vx > 0 && v.vz > 0) {
        return 1;
    }
    return (xm_getBattlingMapCell(&gear->m0_position) & 0x3000000) == 0x1000000;
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): steerBattlingAiOutOfWater (src/battling/bl_8008C7C0.c). */
#ifndef XM_S_steerBattlingAiOutOfWater
#define XM_S_steerBattlingAiOutOfWater
s32 xm_steerBattlingAiOutOfWater(BattlingGear* gear, BattlingAiState* st) __asm__("func_8008FA2C");
#endif
#ifndef XM_S_ratan2
#define XM_S_ratan2
s32 xm_ratan2(s32 y, s32 x) __asm__("ratan2");
#endif
#ifndef XM_S_battlingGearAngle
#define XM_S_battlingGearAngle
extern s32 xm_battlingGearAngle __asm__("D_80092934");
#endif
s32 func_8008FA2C(BattlingGear* gear, BattlingAiState* st) {
    VECTOR v;

    v = gear->m0_position;
    v.vx -= 0x3F80;
    v.vz -= 0x3F80;
    if ((xm_ratan2(v.vx, v.vz) & 0xFFF) > 0x200) {
        st->mA_turn = 0x800 - xm_battlingGearAngle;
    } else {
        st->mA_turn = 0xC00 - xm_battlingGearAngle;
    }
    st->mC_speed = 0xFF;
    return 0;
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): avoidBattlingAiWater (src/battling/bl_8008C7C0.c). */
#ifndef XM_S_avoidBattlingAiWater
#define XM_S_avoidBattlingAiWater
s32 xm_avoidBattlingAiWater(BattlingGear* gear, BattlingAiState* st) __asm__("func_8008FACC");
#endif
#ifndef XM_S_battlingEventFight
#define XM_S_battlingEventFight
extern u8 xm_battlingEventFight __asm__("D_800928C4");
#endif
s32 func_8008FACC(BattlingGear* gear, BattlingAiState* st) {
    if (xm_battlingEventFight == 0 || !(gear->mD0_status.flags & 0x8000000)) {
        return 0;
    }
    if (xm_isBattlingGearInWater(gear) != 0) {
        xm_steerBattlingAiOutOfWater(gear, st);
        return 1;
    }
    if (xm_isBattlingGearInWater(gear->mD8_opponent) != 0) {
        if (gear->mD8_opponent->m8F4_nearestProjectileDist < 0x800) {
            gear->mD0_status.flags |= 0x8000;
            st->mE_boost = 1;
            st->mC_speed = 0xFF;
            st->mA_turn = 0x800;
            gear->m58_targetRotation = 0x800;
            return 1;
        }
        if (gear->mD8_opponent->m8F4_nearestProjectileDist > 0x1000) {
            gear->mD0_status.flags &= ~0x8000;
            st->mE_boost = 0;
            st->mC_speed = 0;
            gear->m58_targetRotation = 0x800;
            return 1;
        }
#ifdef PLATFORM_PC
        return 1; 
#endif
    }
#ifdef PLATFORM_PC
    return 0; 
#endif
}
#endif
void func_80076424(void *arg0);

void func_8008FBD8(void *arg0, void *arg1) {
    u8 level;

    level = *((u8 *)arg1 + 0xF);
    if (level >= 2) {
        *(s32 *)((u8 *)arg0 + 0xD0) |= 2;
        func_80076424(arg0);
    } else if (level != 0) {
        if (*((u8 *)arg1 + 0x2C) & 1) {
            *(s32 *)((u8 *)arg0 + 0xD0) |= 2;
            func_80076424(arg0);
        }
    } else {
        if ((u8)(*((u8 *)arg1 + 0x2C) % 6) == 0) {
            *(s32 *)((u8 *)arg0 + 0xD0) |= 2;
            func_80076424(arg0);
        }
    }
}


void func_8008FC7C(void *arg0) {
    void *temp_a0;

    temp_a0 = *(void **)((s8*)(arg0) + 0x15FC);
    *(s32 *)((s8*)(temp_a0) + 0x20) = 0;
    *(s8 *)((s8*)(temp_a0) + 8) = 0;
    *(s16 *)((s8*)(temp_a0) + 4) = (s16) (((2 - *(u8 *)((s8*)(temp_a0) + 0xF)) * 0x1E) + 0x5A);
    func_8008F7B8(temp_a0);
}

void func_80090504__2arg(void *, s32) asm("func_80090504");

void func_8008FCC8(void *arg0, void *arg1) {
    *(s16 *)((u8 *)arg1 + 0xC) = 0;
    *((u8 *)arg1 + 0xE) = 0;
    *(s16 *)((u8 *)arg1 + 0xA) = 0;
    if (func_8008FACC(arg0, arg1) != 0) {
        return;
    }
    if (D_8009284C < 0x200) {
        if (*(s32 *)((u8 *)arg1 + 0x2C) & 0x1000) {
            func_80090174(arg0);
        } else if (*((u8 *)arg1 + 0xF) != 0) {
            func_80090894(arg0, 1);
        }
        if (*(*(u8 **)((u8 *)arg0 + 0xD8) + 0xC5) == 2) {
            func_8008FBD8(arg0, arg1);
        }
        if (*(s16 *)((u8 *)arg0 + 0x1668) + 2 < *(s16 *)((u8 *)arg1 + 0x30)) {
            func_80090174(arg0);
        }
    }
    if (*(s32 *)(*(u8 **)((u8 *)arg0 + 0xD8) + 0x8F4) < 0x400) {
        func_80090894(arg0, 3);
        *((u8 *)arg1 + 0xE) = 1;
    }
    switch (*(s32 *)((u8 *)arg1 + 0x20)) {
    case 0:
        if (--*(s16 *)((u8 *)arg1 + 4) < 0) {
            *(s32 *)((u8 *)arg1 + 0x20) += 1;
        }
        if (*(*(u8 **)((u8 *)arg0 + 0xD8) + 0xC4) == 0 && (*(s32 *)((u8 *)arg1 + 0x2C) & 0x1000)) {
            if (func_8008F5B4(arg0, 0) != 0) {
                func_8008F900(arg0);
            }
            func_80090504__2arg(arg0, 0);
        }
        break;
    case 1:
        if (func_8008F530(arg0, 0) != 0) {
            func_80090504__2arg(arg0, 0);
        }
        break;
    }
}


void func_8008FE80(s32 arg0) {
    s32 temp_a0;
    s32 var_s0;

    temp_a0 = rand() % 10;
    var_s0 = 1;
    if (temp_a0 >= 2) {
        var_s0 = 2;
    }
    if (temp_a0 >= 5) {
        var_s0 += 1;
    }
    if (var_s0 != 0) {
        do {
            var_s0 -= 1;
            func_8007639C(arg0, (rand() & 1) + 1);
        } while (var_s0 != 0);
    }
}

void func_8008FF24(void *arg0, void *arg1) {

    if (*(u8 *)((s8*)(*(void **)((s8*)(arg0) + 0xD8)) + 0xC4) == 4) {
        if ((*(s32 *)((s8*)(arg1) + 0x2C) & 0xA00) == 0x200) {
            func_8008F900(arg0);
            *(s16 *)((s8*)(arg1) + 4) = 3;
        }
    } else {
        func_8008FE80((s32)arg0);
        *(s16 *)((s8*)(arg1) + 4) = (2 - *(u8 *)((s8*)(arg1) + 0xF)) * 20 + (s16)(rand() % 20 + 1);
    }
    func_8008F7B8(arg1);
}

/* The TU prototypes this `void func_8008FFEC(void *, s32)`, but retail
 * returns 1 (nothing to pick) or 0, so it is defined under a returning alias. */
s32 func_8008FFEC__ret(void *arg0, void *arg1) asm("func_8008FFEC");
/* D_800925A4: 14 x 3 status ids. */
extern u8 D_800925A4[];

s32 func_8008FFEC__ret(void *arg0, void *arg1) {
    s32 n;
    s32 i;
    s32 k;

    n = *((u8 *)arg0 + 0x911);
    if (n == 0) {
        return 1;
    }
    k = 0;
    n = rand() % n;
    for (i = 0; i < 14; i++, k += 3) {
        if (*(u8 *)(*(s32 *)((u8 *)arg0 + 0x1600) + i + 0xA) != 0 &&
            *(u8 *)(i * 4 + *(s32 *)((u8 *)arg0 + 0x80) + 3) != 0) {
            if (n == 0) {
                if (D_800925A4[k] != 0) {
                    func_8007639C(arg0, D_800925A4[k]);
                }
                if (D_800925A4[k + 1] != 0) {
                    func_8007639C(arg0, D_800925A4[k + 1]);
                }
                if (D_800925A4[k + 2] != 0) {
                    func_8007639C(arg0, D_800925A4[k + 2]);
                }
                break;
            }
            n--;
        }
    }
    *(s16 *)((u8 *)arg1 + 4) = (2 - *((u8 *)arg1 + 0xF)) * 20 + (s16)(rand() % 20 + 1);
    func_8008F7B8(arg1);
    return 0;
}

void func_80090174(void *arg0) {
    u8 *ai;

    ai = *(u8 **)((u8 *)arg0 + 0x15FC);
    ai[8] = 1;
    func_8008FF24(arg0, ai);
    if (ai[0xF] >= 2) {
        ai[9] = rand() % 8 + 1;
    } else if (ai[0xF] != 0) {
        ai[9] = rand() % 6 + 1;
    } else {
        ai[9] = rand() % 4 + 1;
    }
    *(s16 *)(ai + 0xC) = 0;
    ai[0xE] = 0;
    func_8008F7B8(ai);
}


s32 func_80090258(void *arg0, s32 arg1) {
    s32 var_v0;

    if (*(u8 *)((s8*)(arg0) + 0x911) != 0) {
        var_v0 = 1;
        if (rand() & 1) {
            if (!(rand() & 1) || (func_8008F5B4(arg0, 0) == 0)) {
                if (D_8009284C < 0x1001) {
                    func_8008FFEC(arg0, arg1);
                    return 0;
                }
                goto block_7;
            }
            goto block_8;
        }
        /* Duplicate return node #9. Try simplifying control flow for better match */
        return var_v0;
    }
    if (func_8008F5B4(arg0, 0) == 0) {
block_7:
        return 1;
    }
block_8:
    func_8008F900(arg0);
    var_v0 = 0;
    return var_v0;
}

INCLUDE_ASM("asm/battling/nonmatchings/main", func_8009031C);

void func_80090504(void *arg0) {
    s32 temp_v1;
    s32 var_v0;
    void *temp_s0;

    temp_s0 = *(void **)((s8*)(arg0) + 0x15FC);
    *(s8 *)((s8*)(temp_s0) + 8) = 3;
    var_v0 = rand();
    temp_v1 = var_v0;
    if (temp_v1 < 0) {
        var_v0 = temp_v1 + 3;
    }
    *(s8 *)((s8*)(temp_s0) + 9) = (s8) ((temp_v1 - ((var_v0 >> 2) * 4)) + 1);
    *(s16 *)((s8*)(temp_s0) + 4) = 0;
    func_8008F7B8(temp_s0);
    *(s8 *)((s8*)(temp_s0) + 0xE) = func_8008F720(arg0, 1);
    *(s8 *)((s8*)(temp_s0) + 0x2E) = 0;
}

#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): updateBattlingAiApproach (src/battling/bl_8008C7C0.c). */
#ifndef XM_T_BL_NEAREST_SHOT_MOVE
#define XM_T_BL_NEAREST_SHOT_MOVE
#define BL_NEAREST_SHOT_MOVE(g) ((g)->m8F8_nearestProjectile->m33_moveType)
#endif
#ifndef XM_S_updateBattlingAiApproach
#define XM_S_updateBattlingAiApproach
void xm_updateBattlingAiApproach(BattlingGear* gear, BattlingAiState* st) __asm__("func_80090580");
#endif
#ifndef XM_S_enterBattlingAiWait
#define XM_S_enterBattlingAiWait
void xm_enterBattlingAiWait(BattlingGear* gear) __asm__("func_8008FC7C");
#endif
#ifndef XM_S_pushBattlingGearInput
#define XM_S_pushBattlingGearInput
void xm_pushBattlingGearInput(BattlingGear* g, u8 v) __asm__("func_8007639C");
#endif
#ifndef XM_S_fireBattlingAiEther
#define XM_S_fireBattlingAiEther
void xm_fireBattlingAiEther(BattlingGear* gear) __asm__("func_8008F900");
#endif
#ifndef XM_S_battlingGearDistance
#define XM_S_battlingGearDistance
extern s32 xm_battlingGearDistance __asm__("D_8009284C");
#endif
#ifndef XM_S_enterBattlingAiMelee
#define XM_S_enterBattlingAiMelee
void xm_enterBattlingAiMelee(BattlingGear* gear) __asm__("func_80090174");
#endif
#ifndef XM_S_getBattlingAiGaugeRoom
#define XM_S_getBattlingAiGaugeRoom
s32 xm_getBattlingAiGaugeRoom(BattlingGear* gear) __asm__("func_8008F580");
#endif
#ifndef XM_S_tryBattlingAiSpecialOrEther
#define XM_S_tryBattlingAiSpecialOrEther
s32 xm_tryBattlingAiSpecialOrEther(BattlingGear* gear, BattlingAiState* st) __asm__("func_80090258");
#endif
void func_80090580(BattlingGear* gear, BattlingAiState* st) {
    s32 r;

    if (xm_isBattlingGearInWater(gear->mD8_opponent) != 0 && xm_battlingEventFight != 0 && (gear->mD0_status.flags & 0x8000000)) {
        xm_enterBattlingAiWait(gear);
        return;
    }
    if (BL_NEAREST_SHOT_MOVE(gear->mD8_opponent) == 1 && gear->mD8_opponent->m8F4_nearestProjectileDist < 0x500 && st->mF_comLevel != 0) {
        st->mE_boost = 1;
        st->mA_turn = st->m2C_b12 ? 0x400 : -0x400;
        if (st->mF_comLevel >= 2 && (xm_rand() & 0xFF) < st->m14_aiParam1 && xm_shouldBattlingAiFireEther(gear, 0) != 0 && st->m2C_b12) {
            if ((xm_rand() & 3) == 0) {
                xm_pushBattlingGearInput(gear, 4);
            }
            xm_fireBattlingAiEther(gear);
            st->m2C_b16_23 = 0;
        }
        st->m2C_b16_23++;
    }
    if (xm_battlingGearDistance < 0x180) {
        if ((xm_rand() & 3) == 0) {
            xm_pushBattlingGearInput(gear, 4);
        }
        xm_enterBattlingAiMelee(gear);
    }
    if (xm_getBattlingAiGaugeRoom(gear) == 0) {
        st->mE_boost = 0;
    }
    if (st->m4_timer < 0) {
        st->mA_turn = xm_rand() % 0x600 - 0x300;
        r = xm_rand();
        st->m4_timer = (st->m2C_b10 ? r % 120 : r % 100) + 10;
        st->mC_speed = 0xFF;
        if (st->m9_repeatCount != 0) {
            st->m9_repeatCount--;
        } else {
            if ((xm_rand() & 3) == 0) {
                xm_pushBattlingGearInput(gear, 4);
            }
            if (xm_tryBattlingAiSpecialOrEther(gear, st) != 0) {
                xm_enterBattlingAiWait(gear);
            }
            st->m9_repeatCount = xm_rand() % 4 + 1;
        }
    }
    st->m4_timer--;
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): enterBattlingAiRetreat (src/battling/bl_8008C7C0.c). */
#ifndef XM_S_enterBattlingAiRetreat
#define XM_S_enterBattlingAiRetreat
void xm_enterBattlingAiRetreat(BattlingGear* gear, s32 arg1) __asm__("func_80090894");
#endif
#ifndef XM_S_tryBattlingGearBackDash
#define XM_S_tryBattlingGearBackDash
s32 xm_tryBattlingGearBackDash(BattlingGear* g) __asm__("func_800767C8");
#endif
#ifndef XM_S_shouldBattlingAiBoost
#define XM_S_shouldBattlingAiBoost
s32 xm_shouldBattlingAiBoost(BattlingGear* gear, s32 flag) __asm__("func_8008F720");
#endif
void func_80090894(BattlingGear* gear, s32 arg1) {
    BattlingAiState* ai;

    ai = gear->m15FC_ai;
    ai->m8_state = 2;
    if (xm_rand() % 3 == 0) {
        xm_tryBattlingGearBackDash(gear);
    }
    ai->m28_retreatDistance = xm_rand() % 0x600 + 0x100;
    ai->m4_timer = 0;
    ai->m9_repeatCount = xm_rand() % 5 + 3;
    xm_rollBattlingAiDecisions(ai);
    ai->mE_boost = xm_shouldBattlingAiBoost(gear, 0);
    ai->m2C_b16_23 = 0;
}
#endif
#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): updateBattlingAiRetreat (src/battling/bl_8008C7C0.c). */
#ifndef XM_S_updateBattlingAiRetreat
#define XM_S_updateBattlingAiRetreat
void xm_updateBattlingAiRetreat(BattlingGear* gear, BattlingAiState* st) __asm__("func_80090990");
#endif
#ifndef XM_S_battlingRubberBandMode
#define XM_S_battlingRubberBandMode
extern u8 xm_battlingRubberBandMode __asm__("D_80092884");
#endif
#ifndef XM_S_battlingGearDistance3D
#define XM_S_battlingGearDistance3D
extern s32 xm_battlingGearDistance3D __asm__("D_80092850");
#endif
#ifndef XM_S_queueBattlingAiSpecialMove
#define XM_S_queueBattlingAiSpecialMove
s32 xm_queueBattlingAiSpecialMove(BattlingGear* gear, BattlingAiState* st) __asm__("func_8008FFEC");
#endif
void func_80090990(BattlingGear* gear, BattlingAiState* st) {
    s32 r;

    if (xm_avoidBattlingAiWater(gear, st) != 0) {
        return;
    }
    if (BL_NEAREST_SHOT_MOVE(gear->mD8_opponent) == 1 && gear->mD8_opponent->m8F4_nearestProjectileDist < 0x500 && st->mF_comLevel != 0) {
        st->mE_boost = 1;
        st->mA_turn = st->m2C_b12 ? 0x400 : -0x400;
        if (st->mF_comLevel >= 2 && xm_shouldBattlingAiFireEther(gear, 0) != 0 && st->m2C_b12) {
            if ((xm_rand() & 3) == 0) {
                xm_pushBattlingGearInput(gear, 4);
            }
            xm_fireBattlingAiEther(gear);
            st->m2C_b16_23 = 0;
        }
        st->m2C_b16_23++;
    }
    if (xm_getBattlingAiGaugeRoom(gear) == 0) {
        st->mE_boost = 0;
    }
    if (xm_battlingGearDistance > st->m28_retreatDistance || (xm_battlingRubberBandMode != 0 && xm_battlingGearDistance3D > 0x4B0)) {
        if (xm_battlingRubberBandMode != 0) {
            xm_pushBattlingGearInput(gear, 4);
        }
        xm_queueBattlingAiSpecialMove(gear, st);
        xm_enterBattlingAiWait(gear);
    }
    if (st->m4_timer < 0 || st->mC_speed == 0) {
        st->mA_turn = xm_rand() % 0x600 + 0x500;
        r = xm_rand();
        st->m4_timer = (st->m2C_b10 ? r % 40 : r % 60) + 10;
        st->mC_speed = 0xFF;
        if (st->m9_repeatCount != 0) {
            st->m9_repeatCount--;
        } else {
            if (xm_shouldBattlingAiFireEther(gear, 0) != 0) {
                xm_pushBattlingGearInput(gear, 4);
                xm_fireBattlingAiEther(gear);
            }
            st->m9_repeatCount = xm_rand() % 5 + 3;
        }
    }
    st->m4_timer--;
}
#endif

void func_80090C88(void *arg0) {
    void *temp_a0;
    void *temp_v1;

    temp_v1 = *(void **)((s8*)(arg0) + 0x1600);
    temp_a0 = *(void **)((s8*)(arg0) + 0x15FC);
    *(s32 *)((s8*)(temp_a0) + 0x10) = (s32) *(u8 *)((s8*)(temp_v1) + 6);
    *(s32 *)((s8*)(temp_a0) + 0x14) = (s32) *(u8 *)((s8*)(temp_v1) + 7);
    *(s32 *)((s8*)(temp_a0) + 0x18) = (s32) *(u8 *)((s8*)(temp_v1) + 8);
    *(s32 *)((s8*)(temp_a0) + 0x1C) = (s32) *(u8 *)((s8*)(temp_v1) + 9);
}

void func_80076424(void *arg0);
/* Retail passes a second argument to these two one-parameter functions
 * (a1 = the AI block / 0); the TU prototypes take one, so call aliases. */
s32 func_8008F570__2arg(void *, void *) asm("func_8008F570");
void func_80090504__2arg(void *, s32) asm("func_80090504");
/* D_80096F30 / D_80096F64: the two 0x34-byte battling AI blocks. */
extern u8 D_80096F30[];
extern u8 D_80096F64[];
extern u8 D_80099D98[];

void func_80090CC0(void *arg0) {
    u8 *ai;

    ai = D_80096F30;
    if (*(s32 *)((s8*)(arg0) + 0xD0) & 0x08000000) {
        ai = D_80096F64;
    }
    *(u8 **)((s8*)(arg0) + 0x15FC) = ai;
    *(s8 *)(ai + 6) = 0x10;
    *(void **)(ai + 0) = arg0;
    *(s16 *)(ai + 4) = 0;
    *(s8 *)(ai + 7) = 0xA;
    *(s16 *)(ai + 0xA) = 0;
    *(s16 *)(ai + 0xC) = 0;
    *(s8 *)(ai + 8) = 0;
    *(s8 *)(ai + 9) = 0;
    *(s32 *)((s8*)(arg0) + 0x48) = 0;
    *(s32 *)(ai + 0x24) = func_8008F570__2arg(arg0, ai);
    *(u8 *)(ai + 0xF) = D_80099D98[0];
    func_80090C88(arg0);
    switch (rand() % 3) {
    case 0:
        func_8008FC7C(arg0);
        break;
    case 1:
        func_80090174(arg0);
        break;
    case 2:
        func_80090894(arg0, 0);
        break;
    case 3:
        func_80090504__2arg(arg0, 0);
        break;
    }
    func_80076424(arg0);
}

void func_80090E10(void *arg0) {
    u8 *ai;

    if (*(s32 *)((u8 *)arg0 + 0xD0) & 0x40) {
        ai = *(u8 **)((u8 *)arg0 + 0x15FC);
        ai[0xF] = D_80099D98[0];
        switch (ai[8]) {
        case 0:
            func_8008FCC8(arg0, ai);
            break;
        case 1:
            func_8009031C(arg0, ai);
            break;
        case 2:
            func_80090990(arg0, ai);
            break;
        case 3:
            func_80090580(arg0, ai);
            break;
        }
        if (ai[0xE] != 0) {
            *(s32 *)((u8 *)arg0 + 0xD0) |= 0x8000;
        }
        if (*(s32 *)(ai + 0x2C) & 0x2000) {
            *(s32 *)((u8 *)arg0 + 0xD0) |= 2;
        }
        func_8008EE1C(arg0, *(s16 *)(ai + 0xA), *(s16 *)(ai + 0xC));
    }
}

INCLUDE_ASM("asm/battling/nonmatchings/main", func_80090F38);
#endif
