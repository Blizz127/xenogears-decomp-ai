#include "common.h"


#ifndef XENO_PC_PORT
extern u8* D_800D2D28;
#endif
#ifndef XENO_PC_PORT
extern u8 D_800D32A1[];
#endif
#ifndef XENO_PC_PORT
extern u8* D_800D367C;
#endif
extern void func_8008AC50(void);
extern void func_8008AB94(void);
extern u32 ArchiveDecodeAlignedSize(u32 file);
extern u8* func_8008ABB8(u32 size, u32 flags);
extern void ArchiveReadFileToBuffer(u32 file, u8* buf, u32 a2, u32 a3);
/* func_8007FD38.s: load archive file 1 or 2 (picked by the actor's
 * D_800D32A1 flag) into a fresh buffer once per battle; the D_800D2D28+0xAE
 * latch makes the whole body a no-op afterwards. */
void func_8007FD38(u8 index) {
    u32 file;
    u8* buf;

    if (D_800D2D28[0xAE] != 0) {
        return;
    }
    func_8008AC50();
    file = 2;
    if (D_800D32A1[(index & 0xFF) * 8] == 0) {
        file = 1;
    }
    func_8008AB94();
    buf = func_8008ABB8(ArchiveDecodeAlignedSize(file), 0);
    D_800D367C = buf;
    ArchiveReadFileToBuffer(file, buf, 0, 0x80);
    func_8008AC50();
    D_800D2D28[0xAE] = 1;
}


#ifndef XENO_PC_PORT
extern u8* D_800D2D28;
#endif
#ifndef XENO_PC_PORT
extern u32 D_800C3DE8;
#endif
extern void HeapFree(u32 p);


/* func_8007FDEC.s */
void func_8007FDEC(void) {
    if (D_800D2D28[0x96] != 0) {
        HeapFree(D_800C3DE8);
        D_800D2D28[0x96] = 0;
    }
}
