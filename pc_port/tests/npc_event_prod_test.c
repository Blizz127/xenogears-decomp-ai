/* Production-linked sleep / dialog / walk-wait certificate.
 *
 * Drives the shipped live Map1 entries from their real start state:
 * FieldScriptVMHandlerSleep (0x26), func_8009C0B4 (0xD2 -> func_8009C5A8)
 * + func_8008004C (confirm) + func_800805F4/func_8007F6F8 (close),
 * func_80098038 (0x53 -> func_80099AC0), and the 0x45 family
 * func_80097864 -> func_80097A50. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "field/actor.h"
#include "field/main.h"
#include "field/camera.h"
#include "field/text_box.h"
#include "field/script_vm.h"

extern void FieldScriptVMHandlerSleep(void);
extern void func_80097864(void);
extern void func_80098038(void);
extern void func_8009C0B4(void);
extern s32 func_8009C5A8(s32 actorIndex, s32 mode);
extern void func_800805F4(void);
extern void func_8008004C(void* ot, s32 renderContextIndex);
extern s32 func_80097A50(s32 targetValue);
extern void func_800A1364(void);

s32 D_800B00C0;
s32 D_800AFD1C;
s32 g_FieldScriptMaxInstructionCount;
s32 D_800ADB2C;
s32 D_800AFD04;
s32 D_800C4268;
s32 D_800ADB64 = 0xFF;
s32 D_800ADB70 = 1;
s32 D_8005A444 = 0;
s32 D_8005A448 = 0;
s32 D_8005A44C = 0;
void* D_800ADBF0;
s32 D_800ADE90;
s32 D_800ADE94;
s32 D_800ADE98;
s32 D_800B068C[4];
s16 D_800B21D6 = 1;
u16 D_800C2694;
u16 D_800C3900;
u16 D_800ADF54[8];
void* D_800B1DF4;
s32 g_FieldCurRenderContextIndex;
s32 g_FieldNumActors = 1;
s32 D_800ADBFC = 1;
s32 g_PlayerActorIndex;
s32 D_800AFFEC;
s32 D_800ADB1C;
s32 D_800ADBE0 = 1;
s32 D_800ADBE4 = 1;
s32 D_800ADBEC = 1;
s32 g_FieldSystemMode;
char D_8006FD84;

ActorData* g_FieldScriptVMCurActor;
void* g_FieldScriptVMCurScriptData;
void* g_FieldScriptMemory;
ScriptsFile* g_FieldCurScriptFile;
FieldActor* volatile g_FieldActors;
u16 D_800B2174[1];
FieldTextBox g_FieldTextBoxes[4];
FieldScene g_Scene;
CameraInterpolation g_CamInterpolation;

static FieldActor s_fieldActors[2];
static ActorData s_actor;
static u8 s_sprite[0x200];
static u8 s_script[0x40];
static int s_failures;
static int s_pagesAdvanced;

static void check(int condition, const char* name)
{
    if (!condition) {
        fprintf(stderr, "ASSERTION %s\n", name);
        s_failures++;
    }
}

int FieldScriptVMGetInstructionArgument(int argumentIndex)
{
    u_char* pData = (u_char*)g_FieldScriptVMCurScriptData +
                    g_FieldScriptVMCurActor->scriptInstructionPointer + argumentIndex;
    return *pData | (pData[1] << 8);
}

short FieldScriptVMGetInstructionArgumentS16(int offset)
{
    u_char* pData = (u_char*)g_FieldScriptVMCurScriptData +
                    g_FieldScriptVMCurActor->scriptInstructionPointer + offset;
    return (short)(pData[0] + (pData[1] << 8));
}

int FieldScriptVMGetVariableValue(int index)
{
    (void)index;
    return 0;
}

int FieldScriptVMGetArgument(int index)
{
    int nArgument = FieldScriptVMGetInstructionArgument(index);
    if (!(nArgument & 0x8000)) {
        return FieldScriptVMGetVariableValue(nArgument & 0xFFFF);
    }
    return nArgument & 0x7FFF;
}

void FieldScriptMemoryWriteU16(u16 address, u16 value)
{
    (void)address;
    (void)value;
}

s32 func_8007B694(s32* arg0)
{
    return (s32)(-(s32)0) & 0xFFF;
    (void)arg0;
}

#ifdef rsin
#undef rsin
#endif
#ifdef rcos
#undef rcos
#endif
int rsin(int a)
{
    (void)a;
    return 0;
}

int rcos(int a)
{
    (void)a;
    return 0x1000;
}

int ratan2(int y, int x)
{
    (void)y;
    (void)x;
    return 0;
}

VECTOR* Square0(VECTOR* v0, VECTOR* v1)
{
    v1->vx = v0->vx * v0->vx;
    v1->vy = v0->vy * v0->vy;
    v1->vz = v0->vz * v0->vz;
    return v1;
}

int SquareRoot0(int v)
{
    int i;
    if (v <= 0) {
        return 0;
    }
    for (i = 1; i < v && i < 0x7FFF; i++) {
        if (i * i >= v) {
            return i;
        }
    }
    return i;
}

s32 func_8008A558(void) { return 0; }
u8 DialogGetWidth(u16* pDialogData, int dialogIndex)
{
    (void)pDialogData;
    (void)dialogIndex;
    return 8;
}
u8 DialogGetHeight(u16* pDialogData, int dialogIndex)
{
    (void)pDialogData;
    (void)dialogIndex;
    return 4;
}
MATRIX* CompMatrix(MATRIX* a, MATRIX* b, MATRIX* c)
{
    (void)a;
    (void)b;
    return c;
}
void SetRotMatrix(MATRIX* m) { (void)m; }
void SetTransMatrix(MATRIX* m) { (void)m; }
int RotTransPers(SVECTOR* v0, int* sxy, long* p, long* flag)
{
    (void)v0;
    *sxy = 0x004000A0;
    *p = 0;
    *flag = 0;
    return 0;
}
u_short GetClut(int x, int y)
{
    (void)x;
    (void)y;
    return 0;
}
void SetDrawMode(DR_MODE* p, int dfe, int dtd, int tpage, RECT16* tw)
{
    (void)p;
    (void)dfe;
    (void)dtd;
    (void)tpage;
    (void)tw;
}
void SetPolyFT4(POLY_FT4* p) { (void)p; }
void SetSemiTrans(void* p, int abe)
{
    (void)p;
    (void)abe;
}
void SetShadeTex(void* p, int tge)
{
    (void)p;
    (void)tge;
}
void SetSprt(SPRT* p) { (void)p; }
void SetTile(TILE* p) { (void)p; }
void AddPrim(void* ot, void* p)
{
    (void)ot;
    (void)p;
}

s32 func_8007F8DC(s32 x, s32 y, s32 stringIndex, s32 textBoxIndex, s32 width, s32 height,
                  s32 ownerActorIndex, s32 talkingActorIndex, s32 mode, s32 orientationFlags,
                  s32 dialogFlags)
{
    (void)x;
    (void)y;
    (void)stringIndex;
    (void)width;
    (void)height;
    (void)talkingActorIndex;
    (void)mode;
    (void)orientationFlags;
    (void)dialogFlags;
    g_FieldTextBoxes[textBoxIndex].visibility = 0;
    g_FieldTextBoxes[textBoxIndex].status = -1;
    g_FieldTextBoxes[textBoxIndex].ownerActorID = (s16)ownerActorIndex;
    g_FieldTextBoxes[textBoxIndex].talkingActorID = (s16)talkingActorIndex;
    g_FieldTextBoxes[textBoxIndex].windowOpenTimer = 1;
    g_FieldTextBoxes[textBoxIndex].order = 0;
    return 0;
}
void func_80032F54(void* a, s32 b, s32 c, s32 d, s32 e, s32 f, s32 g, s32 h)
{
    (void)a;
    (void)b;
    (void)c;
    (void)d;
    (void)e;
    (void)f;
    (void)g;
    (void)h;
}
void* GetStringEntry(void* arg0, s32 arg1)
{
    (void)arg0;
    (void)arg1;
    return (void*)(uintptr_t)0x100;
}
void func_800345E0(void* pWindow)
{
    (void)pWindow;
    s_pagesAdvanced++;
}
void func_80034614(void* p) { (void)p; }
void func_800346D4(void* p) { (void)p; }
void func_80034714(void* a, void* b)
{
    (void)a;
    (void)b;
}
void func_80034888(void* a, void* b, s32 c)
{
    (void)a;
    (void)b;
    (void)c;
}
s32 func_80033CD0(void* a)
{
    (void)a;
    return 1;
}


void func_80034874(void* a, u8 b)
{
    (void)a;
    (void)b;
}
int func_8003487C(void* a)
{
    (void)a;
    return 0;
}
s32 ArchiveDataSync(void) { return 0; }
void FieldLoadTIMWithClut(void) {}
void* HeapAlloc(u32 size, u32 tag)
{
    (void)tag;
    return calloc(1, size ? size : 1);
}
void HeapFree(void* p) { free(p); }
int ArchiveSetIndex(int a, int b)
{
    (void)a;
    (void)b;
    return 0;
}
int ArchiveDecodeAlignedSize(unsigned int e)
{
    (void)e;
    return 0x10;
}
int func_80029AFC(void* a, int b, int c)
{
    (void)a;
    (void)b;
    (void)c;
    return 0;
}
void func_800379C8(char* a, ...) { (void)a; }

void* g_FieldSpriteData;
s32 D_800B2264;
u16 D_800B21DC[8];
u8 D_800B225F[8];
static u8 s_spritePkg[16];

void func_80076AC0(s32 actorIndex, s32 a1, void* pkg, s32 a3, s32 a4, s32 a5,
                   s32 a6)
{
    (void)a1;
    (void)pkg;
    (void)a3;
    (void)a4;
    (void)a5;
    (void)a6;
    if (actorIndex >= 0 && actorIndex < 2) {
        s_fieldActors[actorIndex].pSpriteData = (u32)(uintptr_t)s_sprite;
    }
}

extern s16 D_800B06A4[];
extern s16 D_800B06A6[];
extern s16 D_800B06A8[];
s16 D_800B06A4[9];
s16 D_800B06A6[9];
s16 D_800B06A8[9];
s32 D_800ADB0C;
void* D_800ADB10;
void* D_800ADB14;
s16 D_800AEAE4[32];
u8 D_800AE1E0[4];
u8 D_800B00C8[64];
u8 D_800ADF34[32];

static void reset_actor(void)
{
    memset(&s_actor, 0, sizeof(s_actor));
    memset(s_sprite, 0, sizeof(s_sprite));
    memset(s_script, 0, sizeof(s_script));
    memset(s_fieldActors, 0, sizeof(s_fieldActors));
    memset(g_FieldTextBoxes, 0, sizeof(g_FieldTextBoxes));
    D_800B2174[0] = 0;
    D_800C2694 = 0;
    D_800C4268 = 0;
    D_800B00C0 = 0;
    D_800AFD1C = 0;
    s_pagesAdvanced = 0;
    g_FieldScriptMaxInstructionCount = 8;

    s_actor.moveSpeed = 0x100;
    s_actor.faceId = 0xFF;
    s_actor.curScriptIndex = 0;
    s_actor.scripts[0].flags_0 = 0xFFFF;
    s_actor.scripts[0].waitTimer = 0;
    s_actor.scripts[0].flags_0x17 = 0;
    s_actor.position.vx = 0;
    s_actor.position.vy = 0;
    s_actor.position.vz = 0;

    s_fieldActors[0].pActorData = (u32)(uintptr_t)&s_actor;
    s_fieldActors[0].pSpriteData = (u32)(uintptr_t)s_sprite;
    g_FieldActors = s_fieldActors;
    g_FieldScriptVMCurActor = &s_actor;
    g_FieldScriptVMCurScriptData = s_script;

    {
        int i;
        for (i = 0; i < 4; i++) {
            g_FieldTextBoxes[i].visibility = -1;
            g_FieldTextBoxes[i].status = -1;
            g_FieldTextBoxes[i].order = 0xFFFF;
            g_FieldTextBoxes[i].ownerActorID = 0xFF;
            D_800B068C[i] = -1;
        }
    }
}

static void test_sleep(void)
{
    u16 ip0;

    reset_actor();
    s_script[0] = 0x26;
    s_script[1] = 0x03;
    s_script[2] = 0x80; /* immediate 3 */
    s_actor.scriptInstructionPointer = 0;
    ip0 = s_actor.scriptInstructionPointer;

    FieldScriptVMHandlerSleep();
    check(s_actor.scriptInstructionPointer == ip0, "sleep.holds.then.advances");
    check(s_actor.scripts[0].waitTimer == 3, "sleep.holds.then.advances");
    FieldScriptVMHandlerSleep();
    check(s_actor.scriptInstructionPointer == ip0, "sleep.holds.then.advances");
    FieldScriptVMHandlerSleep();
    check(s_actor.scriptInstructionPointer == ip0, "sleep.holds.then.advances");
    FieldScriptVMHandlerSleep();
    check(s_actor.scriptInstructionPointer == (u16)(ip0 + 3),
          "sleep.holds.then.advances");
}

static void test_dialog(void)
{
    u16 ip0;
    s32 rc;

    reset_actor();
    s_script[0] = 0xD2;
    s_script[1] = 0;
    s_script[2] = 0;
    s_script[3] = 0;
    s_actor.scriptInstructionPointer = 0;
    ip0 = s_actor.scriptInstructionPointer;

    /* Live Map1 dialog opcode 0xD2 is func_8009C0B4 -> func_8009C5A8(cur, 0).
     * Opcode 0x04 is the yield/stop handler, not dialog. */
    func_8009C0B4();
    rc = (s_actor.scriptInstructionPointer == (u16)(ip0 + 4)) ? 0 : -1;
    check(rc == 0, "dialog.open.then.confirm.then.close");
    check(g_FieldTextBoxes[0].visibility == 0, "dialog.open.then.confirm.then.close");
    check((D_800B2174[0] & 1) != 0, "dialog.open.then.confirm.then.close");

    func_800805F4();
    D_800C2694 = 0x20;
    {
        u32 ot[8] = {0};
        func_8008004C(ot, 0);
    }
    check(s_pagesAdvanced > 0, "dialog.open.then.confirm.then.close");

    g_FieldTextBoxes[0].status = 0;
    func_800805F4();
    check(g_FieldTextBoxes[0].visibility != 0, "dialog.open.then.confirm.then.close");
    check(D_800B2174[0] == 0, "dialog.open.then.confirm.then.close");
}

static void run_walkwait(void (*handler)(void), u16 done_delta)
{
    u16 ip0;
    int ticks;
    int held;

    s_actor.scriptInstructionPointer = 0;
    s_actor.scripts[0].flags_0 = 0xFFFF;
    s_actor.scripts[0].flags_0x17 = 0;
    ip0 = s_actor.scriptInstructionPointer;

    held = 0;
    for (ticks = 0; ticks < 32; ticks++) {
        D_800B00C0 = 0;
        handler();
        if (s_actor.scriptInstructionPointer == ip0) {
            held++;
        } else {
            break;
        }
    }
    check(held > 1, "walkwait.not.instant");
    check(s_actor.scriptInstructionPointer == (u16)(ip0 + done_delta),
          "walkwait.not.instant");
}

static void test_walkwait(void)
{
    /* Live Map1 walk-wait is opcode 0x53: func_80098038 -> func_80099AC0. */
    reset_actor();
    s_script[0] = 0x53;
    s_script[1] = 0x00;
    s_script[2] = 0x08;
    s_script[3] = 0x80; /* immediate duration 8 */
    run_walkwait(func_80098038, 4);

    /* 0x45 family still uses the previously stubbed func_80097A50. */
    reset_actor();
    s_script[0] = 0x45;
    s_script[1] = 0x00;
    s_script[2] = 0x80;
    s_script[3] = 0x00;
    s_script[4] = 0x80;
    s_script[5] = 0x08;
    s_script[6] = 0x80;
    s_script[7] = 0x80;
    run_walkwait(func_80097864, 8);
    (void)func_80097A50;
}

static void test_object_loader_unspin(void)
{
    u16 ip0;

    reset_actor();
    memset(s_spritePkg, 0, sizeof(s_spritePkg));
    g_FieldSpriteData = s_spritePkg;
    D_800B2264 = 0;
    D_800AFD1C = 0;
    s_script[0] = 0x00;
    s_script[1] = 0x01;
    s_script[2] = 0x80;
    s_actor.scriptInstructionPointer = 0;
    ip0 = s_actor.scriptInstructionPointer;

    func_800A1364();
    check(s_actor.scriptInstructionPointer == (u16)(ip0 + 3),
          "object.loader.ip.advances");
    check(D_800B2264 == 1, "object.loader.ip.advances");
    check((s_actor.flags & 0x2000) != 0, "object.loader.ip.advances");
}

int main(void)
{
    (void)g_CamInterpolation;
    (void)g_Scene;
    test_sleep();
    test_dialog();
    test_walkwait();
    test_object_loader_unspin();
    if (s_failures != 0) {
        return 1;
    }
    printf("NPC EVENT certificate PASS\n");
    return 0;
}
