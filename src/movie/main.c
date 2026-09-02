/*
 * movie.bin — retail game state 6 (g_MainGameStates[6]: main MovieMain
 * 0x800737EC, state/BSS 0x80076F38, heap 0x80077454, archive 0x12).
 *
 * The shipping boot (func_80019578) sets D_8004FE44 = 1 (movie number),
 * D_8004FE46 = 1 (return state: Field), D_8004FE47 = 0 (skip allowed),
 * D_8004FE45 = disc number and calls ChangeGameState(6).  MovieMain loads the
 * movie player module (archive directory 0x18 file 1, placed at 0x801D3000;
 * see config/movie_player.yaml), plays the opening STR through it and returns
 * to the requested state.  When D_8004FE44 == 0xFF, or L2 (0x100) is held on
 * entry, MovieMain instead runs the developer movie menu.
 *
 * Retail authority: asm/movie/9F8.s (split by config/movie.yaml).
 *
 * Transcription status:
 *   - MovieMain, func_800763BC, func_80076488, func_800768D8, func_800769A4,
 *     func_8007625C, func_800747AC, func_80074AF0, func_80074B58,
 *     func_80074BA4, func_8007519C, func_80075D4C, func_80075D8C,
 *     func_80072D84, func_80072F98, func_80073328, func_800734B8: transcribed
 *     from the asm.
 *   - func_800704E8 (CD-ROM CHECK), func_80075534 (CD-ROM MONITOR),
 *     func_80072480 (DISC CHANGE) and their private helpers (func_80070DCC,
 *     func_800712C4, func_80071BA0, func_80071C34, func_80072428,
 *     func_8007293C, func_800729A8, func_80072A08, func_800753B8,
 *     func_8007548C, func_80075508, func_80076AF0, func_80076C68,
 *     func_80076CA4) are developer CD test tools reachable only from that
 *     menu.  They are NOT transcribed yet; the three menu entries are labeled
 *     placeholders that print a diagnostic and return to the menu.  Removal:
 *     transcribe the functions listed above from asm/movie/9F8.s.
 */
#include "common.h"
#include "main/main.h"
#include "system/archive.h"
#include "system/controller.h"
#include "system/font.h"
#include "system/memory.h"
#include "system/sound.h"

#include "psyq/libetc.h"
#include "psyq/libgpu.h"

#ifdef XENO_PC_PORT
#include <stdint.h>
#include <stdio.h>
extern unsigned char g_PsxRam[];
#endif

/* SLUS symbols */
extern u8 D_8004FE44;   /* movie number (bit 7: use D_80062514 as end frame) */
extern u8 D_8004FE45;   /* disc number (movie file selector) */
extern u8 D_8004FE46;   /* game state to enter after the movie */
extern u8 D_8004FE47;   /* nonzero: skipping disabled */
extern s32 D_8005A49C;
extern s32 D_8005A4A4;
extern s16 D_8005A4B8;
extern s32 D_8005A4DC;
extern u16 D_80062514;

extern int Vsync(int mode);
extern void ChangeGameState(unsigned int state);
extern void GameCheckAndHandleSoftReset(void);
extern int ArchiveDecodeSizeAbsolute(int entryIndex);
extern int ArchiveDecodeAlignedSize(unsigned int entryIndex);
extern int ArchiveGetDiscNumber(void);
extern void ArchiveCdDataSync(int mode);
extern s32 ArchiveReadFileFromCdSector(s32 sector, void* pDestBuffer, s32 fileSize, s32 arg3, u32 flags);
#ifdef XENO_PC_PORT
extern s32 ArchiveReadFileToBuffer(s32 index, void* pBuffer, u32 arg2, u32 flags);
#else
extern s32 ArchiveReadFileToBuffer(s32 index, s32 pBuffer, u32 arg2, u32 flags);
#endif
extern u32 func_8002C3D8(void);
extern void FontLoadFont(int sx, int sy, int w, int h, s32 f1, s32 f2, s32 f3, s32 f4, s32 f5, s32 f6, s32 f7);
extern void FontPrintf(char* pFormat, ...);
extern void FontDrawLetters(void* ot);
extern void SoundSetCdVolumeWithFade(s32 targetVolume, s32 fadeFrames);
extern int ControllerGetButtonState(int controllerIndex);
extern void HeapDebugDump(u_int mode, u_int startBlockIdx, int endBlockIdx, u_int flags);
extern int PCopen(char* name, int flag, int perm);
extern int PClseek(int fd, int offset, int mode);
extern int PCread(int fd, void* buf, int len);
extern int PCclose(int fd);

/* Movie player module (archive 0x18/1 at 0x801D3000) */
extern s32 func_801D3538(s32 width, s32 height, s32 arg2, s32 arg3, s32 ringSectors, s32 sectorSize, s32 mode);
extern void func_801D37CC(u16 fileIndex, s32 sectorOffset, u16 startFrame, u16 endFrame, u16 channel,
                          s32 flags, u16 loop, u16 x0, u16 y0, u16 x1, u16 y1, u16 lines,
                          void (*callback)(u16, u16, u16));
extern void func_801D3F7C(void);
extern void func_801D4318(void);
extern void func_801D43B0(void);
extern s32 D_801D68B4;  /* module: 1 = field (interlaced strip) mode */
extern s32 D_801E89D4;  /* module: skipped-frame counter ("Skp") */

/* ------------------------------------------------------------------------- */
/* Overlay data (.data 0x80076E44.., .bss 0x80076F38..0x80077454)            */
/* ------------------------------------------------------------------------- */

typedef struct {
    DRAWENV draw;    /* 0x00 */
    DISPENV disp;    /* 0x5C */
    u_long ot[32];   /* 0x70 */
    POLY_G4 box;     /* 0xF0 */
    POLY_G4 frame;   /* 0x114 */
} MovieRenderEnv;    /* 0x138 */

/* .data */
s32 D_80076EEC = 0;            /* menu pad repeat counter */
s32 D_80076EF0 = 0x10;         /* menu pad repeat threshold */
s32 D_80076EF4 = 0x499602D2;   /* rand state a */
s32 D_80076EF8 = 0x3ADE68B1;   /* rand state b */
s32 D_80076F04 = 0;            /* playback pad repeat counter */
s32 D_80076F08 = 0x5A;         /* playback pad repeat threshold */

/* .bss (order follows the retail layout) */
u8 D_80076F8C[4][4];           /* box colour A (r,g,b,pad) per corner */
u8 D_80076F9C[4][4];           /* box colour B */
s32 D_80076FAC[4];             /* box colour fade step */
s32 D_80076FBC[4];             /* box colour fade length */
u8 D_80076FCC[4][4];           /* frame colour A */
u8 D_80076FDC[4][4];           /* frame colour B */
s32 D_80076FEC[4];             /* frame colour fade step */
s32 D_80076FFC[4];             /* frame colour fade length */
s32 D_8007700C;
s32 D_80077010;                /* last frame number reported by the player */
s32 D_80077014;                /* 1: playback finished; >1: skip countdown */
s32 D_80077018;                /* display buffer selected by the frame callback */
s32 D_8007701C;                /* display buffer currently shown */
s32 D_80077020;                /* nonzero: hold decoding */
s32 D_80077024;                /* vertical offset of the picture (SCREEN DRAW) */
s32 D_80077028;                /* nonzero: skipping disabled */
u32 D_80077118;                /* menu cursor */
s32 D_8007711C;                /* MOVIE NUMBER */
MovieRenderEnv* D_80077120;    /* current render environment */
MovieRenderEnv D_80077124[2];
s32 D_80077394;                /* heap dump toggle */
s32 D_80077398;                /* MOVIE CHANNEL */
s32 D_8007739C;                /* END FRAME */
s32 D_800773A0;                /* SCREEN DRAW lines (-1: ALL) */
s32 D_800773A4;                /* START FRAME */
s32 D_800773A8;                /* start sector offset (+%4dSECT) */
s32 D_800773AC;                /* pad state */
s32 D_800773B0;
s32 D_800773B4;                /* previous pad state */
s32 D_800773B8[16][2];         /* Vsync(1) timings around the pump call */
s32 D_80077438;                /* REWIND */
s32 D_8007743C;                /* END FRAME state: 0 SET, 1 known, 2 ??? */
s32 D_80077440;                /* font loaded */
s32 D_80077444;                /* START FRAME state: 1 SET, 2 sector known */
s32 D_80077448;                /* MOVIE TYPE: 0 picture, 1 picture+ADPCM, 2 ADPCM */
s32 D_8007744C;
s32 D_80077450;                /* func_8002C3D8(): 0 CD MODE1, -1 MODE2, else PC HDD */
s32 D_80077454;                /* SCREEN MODE: 1 = 24-bit colour */

static const RECT D_800704E0 = { 0, 0, 0x280, 0x200 };

/* ------------------------------------------------------------------------- */

static void MovieSwapRenderEnv(void)
{
    if (D_80077120 == &D_80077124[0]) {
        D_80077120 = &D_80077124[1];
    } else {
        D_80077120 = &D_80077124[0];
    }
    D_8007744C = 1 - D_8007744C;
    ClearOTagR(D_80077120->ot, 32);
}

static void MoviePresent(void)
{
    DrawSync(0);
    Vsync(0);
    PutDrawEnv(&D_80077120->draw);
    PutDispEnv(&D_80077120->disp);
    DrawOTag(&D_80077120->ot[31]);
}

/* 0x80074AF0: menu colour RNG */
s32 func_80074AF0(void)
{
    s32 a = D_80076EF4 * 5 + 1;
    s32 b = D_80076EF8 * 7 + 3;
    s32 v;

    D_80076EF4 = a;
    D_80076EF8 = b;
    v = (a ^ b) + 1;
    D_80076EF4 = v;
    if (v < 0) {
        D_80076EF4 = -v;
    }
    return D_80076EF4;
}

/* 0x80074B58: clear the whole frame buffer */
void func_80074B58(void)
{
    RECT rect;

    rect.x = 0;
    rect.y = 0;
    rect.w = 0x280;
    rect.h = 0x1E0;
    ClearImage(&rect, 0, 0, 0);
    DrawSync(0);
}

/* 0x800747AC: menu pad handling with key repeat.  Returns -1/+1 for
 * left/right, and stores 1..4 in *pButton for triangle/circle/cross/square. */
s32 func_800747AC(s32 minCursor, u32 maxCursor, s32* pButton)
{
    D_800773B4 = D_800773AC;
    D_800773AC = ControllerGetButtonState(0);
    if (D_800773B4 == D_800773AC) {
        if (D_800773B4 != 0) {
            D_80076EEC++;
            if (D_80076EF0 < D_80076EEC) {
                D_800773B4 = 0;
                D_80076EF0 = 1;
                D_80076EEC = 0;
            }
        } else {
            D_80076EF0 = 0x10;
            D_80076EEC = 0;
        }
    } else {
        D_80076EF0 = 0x10;
        D_80076EEC = 0;
    }

    if (!(D_800773B4 & 0x1000) && (D_800773AC & 0x1000)) {
        D_80077118--;
        if ((s32)D_80077118 < minCursor) {
            D_80077118 = maxCursor;
        }
    }
    if (!(D_800773B4 & 0x4000) && (D_800773AC & 0x4000)) {
        D_80077118++;
        if (maxCursor < D_80077118) {
            D_80077118 = minCursor;
        }
    }

    *pButton = 0;
    if (!(D_800773B4 & 0x10) && (D_800773AC & 0x10)) {
        *pButton = 1;
    }
    if (!(D_800773B4 & 0x20) && (D_800773AC & 0x20)) {
        *pButton = 2;
    }
    if (!(D_800773B4 & 0x40) && (D_800773AC & 0x40)) {
        *pButton = 3;
    }
    if (!(D_800773B4 & 0x80) && (D_800773AC & 0x80)) {
        *pButton = 4;
    }
    if (!(D_800773B4 & 0x100) && (D_800773AC & 0x100)) {
        D_800773B0 = 1 - D_800773B0;
    }
    if (!(D_800773B4 & 0x800) && (D_800773AC & 0x800)) {
        D_80077394 = 1 - D_80077394;
    }
    if (!(D_800773B4 & 0x2000) && (D_800773AC & 0x2000)) {
        return 1;
    }
    if (!(D_800773B4 & 0x8000) && (D_800773AC & 0x8000)) {
        return -1;
    }
    return 0;
}

/* 0x80072D84: initialise the menu background boxes */
void func_80072D84(POLY_G4* p0, POLY_G4* p1, s16 x, s16 y, s32 w, s32 h)
{
    s32 i;
    s16 x1;
    s16 y1;

    for (i = 0; i < 4; i++) {
        D_80076F8C[i][0] = (func_80074AF0() & 0xFF) / 32 + 8;
        D_80076F8C[i][1] = 8;
        D_80076F8C[i][2] = (func_80074AF0() & 0xFF) / 3 + 0x10;
        D_80076F9C[i][0] = (func_80074AF0() & 0xFF) / 32 + 8;
        D_80076F9C[i][1] = 8;
        D_80076F9C[i][2] = (func_80074AF0() & 0xFF) / 3 + 0x10;
    }
    for (i = 0; i < 4; i++) {
        D_80076FAC[i] = 0;
        D_80076FBC[i] = (func_80074AF0() & 0xFF) + 0x20;
    }

    SetPolyG4(p0);
    SetSemiTrans(p0, 0);
    x1 = x + w;
    y1 = y + h;
    setXY4(p0, x, y, x1, y, x, y1, x1, y1);
    SetPolyG4(p1);
    SetSemiTrans(p1, 0);
    setXY4(p1, x, y, x1, y, x, y1, x1, y1);
}

/* 0x80073328: initialise the menu frame boxes */
void func_80073328(POLY_G4* p0, POLY_G4* p1, s16 x, s16 y, s32 w, s32 h)
{
    s32 i;
    s16 x1;
    s16 y1;

    for (i = 0; i < 4; i++) {
        D_80076FCC[i][0] = 0xFF;
        D_80076FCC[i][1] = 0xFF;
        D_80076FCC[i][2] = (func_80074AF0() & 0x3F) - 0x42;
        D_80076FDC[i][0] = 0xFF;
        D_80076FDC[i][1] = 0xFF;
        D_80076FDC[i][2] = (func_80074AF0() & 0x3F) - 0x42;
    }
    for (i = 0; i < 4; i++) {
        D_80076FEC[i] = 0;
        D_80076FFC[i] = (func_80074AF0() & 0xFF) + 0x20;
    }

    SetPolyG4(p0);
    x1 = x + w;
    y1 = y + h;
    setXY4(p0, x, y, x1, y, x, y1, x1, y1);
    SetPolyG4(p1);
    setXY4(p1, x, y, x1, y, x, y1, x1, y1);
}

static void MovieSetCorner(POLY_G4* p, s32 corner, u8 r, u8 g, u8 b)
{
    switch (corner) {
    case 0:
        setRGB0(p, r, g, b);
        break;
    case 1:
        setRGB1(p, r, g, b);
        break;
    case 2:
        setRGB2(p, r, g, b);
        break;
    case 3:
        setRGB3(p, r, g, b);
        break;
    }
}

/* 0x80072F98: animate the background box colours and link it into the OT */
void func_80072F98(u_long* ot, POLY_G4* p, s16 x, s16 y, s32 w, s32 h)
{
    s32 i;
    s16 x1 = x + w;
    s16 y1 = y + h;

    setXY4(p, x, y, x1, y, x, y1, x1, y1);
    for (i = 0; i < 4; i++) {
        s32 step;
        s32 len;
        u8 r, g, b;

        D_80076FAC[i]++;
        if (D_80076FBC[i] < D_80076FAC[i]) {
            D_80076FAC[i] = 0;
            D_80076FBC[i] = (func_80074AF0() & 0xFF) + 0x20;
            D_80076F8C[i][0] = D_80076F9C[i][0];
            D_80076F8C[i][1] = D_80076F9C[i][1];
            D_80076F8C[i][2] = D_80076F9C[i][2];
            D_80076F9C[i][0] = (func_80074AF0() & 0xFF) / 32 + 8;
            D_80076F9C[i][1] = 8;
            D_80076F9C[i][2] = (func_80074AF0() & 0xFF) / 3 + 0x10;
        }
        step = D_80076FAC[i];
        len = D_80076FBC[i];
        r = D_80076F8C[i][0] + ((D_80076F9C[i][0] - D_80076F8C[i][0]) * step) / len;
        g = D_80076F8C[i][1] + ((D_80076F9C[i][1] - D_80076F8C[i][1]) * step) / len;
        b = D_80076F8C[i][2] + ((D_80076F9C[i][2] - D_80076F8C[i][2]) * step) / len;
        MovieSetCorner(p, i, r, g, b);
    }
    AddPrim(ot, p);
}

/* 0x800734B8: animate the frame box colours and link it into the OT */
void func_800734B8(u_long* ot, POLY_G4* p, s16 x, s16 y, s32 w, s32 h)
{
    s32 i;
    s16 x1 = x + w;
    s16 y1 = y + h;

    setXY4(p, x, y, x1, y, x, y1, x1, y1);
    for (i = 0; i < 4; i++) {
        s32 step;
        s32 len;
        u8 r, g, b;

        D_80076FEC[i]++;
        if (D_80076FFC[i] < D_80076FEC[i]) {
            D_80076FEC[i] = 0;
            D_80076FFC[i] = (func_80074AF0() & 0xFF) + 0x20;
            D_80076FCC[i][0] = D_80076FDC[i][0];
            D_80076FCC[i][1] = D_80076FDC[i][1];
            D_80076FCC[i][2] = D_80076FDC[i][2];
            D_80076FDC[i][0] = 0xFF;
            D_80076FDC[i][1] = 0xFF;
            D_80076FDC[i][2] = (func_80074AF0() & 0x3F) - 0x42;
        }
        step = D_80076FEC[i];
        len = D_80076FFC[i];
        r = D_80076FCC[i][0] + ((D_80076FDC[i][0] - D_80076FCC[i][0]) * step) / len;
        g = D_80076FCC[i][1] + ((D_80076FDC[i][1] - D_80076FCC[i][1]) * step) / len;
        b = D_80076FCC[i][2] + ((D_80076FDC[i][2] - D_80076FCC[i][2]) * step) / len;
        MovieSetCorner(p, i, r, g, b);
    }
    AddPrim(ot, p);
}

/* 0x80075D4C: raw archive table size field for a directory entry */
s32 func_80075D4C(s32 entryIndex)
{
    u8* e = (u8*)(uintptr_t)g_ArchiveTable + entryIndex * ARCHIVE_HEADER_ENTRY_SIZE;
    return ((e[6] << 24) + (e[5] << 16) + (e[4] << 8)) | e[3];
}

/* ------------------------------------------------------------------------- */
/* STR sector scanning (START FRAME / END FRAME menu items)                   */
/* ------------------------------------------------------------------------- */

/* STR sector header as read from the disc (raw 0x920 sector for movie type 1,
 * 0x800 for type 0; the picture header sits at +8 in the raw sector). */
typedef struct {
    u16 status;      /* 0x160 = picture sector */
    u16 type;
    u16 sectorIndex; /* +4 */
    u16 sectorCount; /* +6 */
    u32 frameNumber; /* +8 */
} StrPictureHeader;

/* 0x80074BA4: locate the sector offset of frame `frame` in the selected
 * movie file.  Returns -1 when not found. */
s32 func_80074BA4(s32 frame)
{
    u8 sector[0x1000];
    s32 headerLen;
    s32 headerOffset;
    s32 sectorSize;
    s32 fileIndex;
    char* path;
    s32 result = 0;
    s32 best = 0;
    s32 pos;
    s32 start;

    if (D_80077448 == 0) {
        ArchiveSetIndex(0x18, 0);
        if (!(D_8007711C < ArchiveDecodeSizeAbsolute(2))) {
            return 0;
        }
        fileIndex = D_8007711C + 3;
        path = ArchiveGetFilePath(fileIndex);
        sectorSize = 0x800;
        headerLen = 0x20;
        headerOffset = 0;
    } else if (D_80077448 == 1) {
        ArchiveSetIndex(0x18, 1);
        if (!(D_8007711C < ArchiveDecodeSizeAbsolute(1))) {
            return 0;
        }
        fileIndex = D_8007711C + 2;
        path = ArchiveGetFilePath(fileIndex);
        sectorSize = 0x920;
        headerLen = 0x28;
        headerOffset = 8;
    } else if (D_80077448 == 2) {
        return -1;
    } else {
        return 0;
    }

    if (frame < 2) {
        return 0;
    }

    if (func_8002C3D8() != 0) {
        /* PC HDD development mode: the file is read directly. */
        StrPictureHeader* hdr = (StrPictureHeader*)(sector + headerOffset);
        s32 fd = PCopen(path, 0, 0);
        s32 got;
        s32 perFrame;

        PClseek(fd, 0, 2);
        PClseek(fd, 0, 0);
        PCread(fd, sector, headerLen);
        perFrame = hdr->sectorCount;
        pos = (frame - 1) * perFrame - ((frame - 1) >> 2);
        PClseek(fd, pos * sectorSize, 0);
        got = PCread(fd, sector, headerLen);
        if ((s32)hdr->frameNumber == frame && got != 0) {
            if (hdr->sectorIndex == 0) {
                PCclose(fd);
                return pos;
            }
            start = pos - hdr->sectorIndex - 2;
        } else {
            if (hdr->frameNumber < (u32)frame && pos > 0 && got != 0) {
                best = pos;
            }
            pos = (frame - 1) * perFrame - ((frame - 1) >> 2);
            pos += pos / 7;
            PClseek(fd, pos * sectorSize, 0);
            got = PCread(fd, sector, headerLen);
            if ((s32)hdr->frameNumber == frame && got != 0) {
                if (hdr->sectorIndex == 0) {
                    PCclose(fd);
                    return pos;
                }
                start = pos - hdr->sectorIndex - 2;
            } else {
                if (hdr->frameNumber < (u32)frame && best < pos && got != 0) {
                    best = pos;
                }
                start = (perFrame - 1) * (frame - 1);
                if (best > 0) {
                    start = best;
                }
            }
        }
        do {
            pos = start;
            PClseek(fd, start * sectorSize, 0);
            got = PCread(fd, sector, headerLen);
            start = pos + 1;
            if (hdr->status == 0x160) {
                start = pos + (hdr->sectorCount - hdr->sectorIndex);
            }
        } while ((s32)hdr->frameNumber != frame && got > 0);
        if (got == 0) {
            pos = -1;
        }
        PCclose(fd);
        return pos;
    } else {
        StrPictureHeader* hdr = (StrPictureHeader*)sector;
        s32 sectors;
        s32 perFrame;

        /* 0x80074F00..0x80075164 */
        ArchiveReadFileFromCdSector(ArchiveDecodeSector(fileIndex), sector, 0x800, 0, 0);
        ArchiveCdDataSync(0);
        sectors = (ArchiveDecodeAlignedSize(fileIndex) + sectorSize - 1) / sectorSize;
        perFrame = hdr->sectorCount;
        pos = (frame - 1) * perFrame - ((frame - 1) >> 2);
        ArchiveReadFileFromCdSector(ArchiveDecodeSector(fileIndex) + pos, sector, 0x800, 0, 0);
        ArchiveCdDataSync(0);
        /* Retail leaves `start` in $s2 (the caller's value) on the direct-hit
         * exit; 0 keeps the "start < sectors" result identical for any
         * non-empty file. */
        start = 0;
        if ((s32)hdr->frameNumber == frame && pos < sectors) {
            if (hdr->sectorIndex == 0) {
                goto done;
            }
            start = pos - hdr->sectorIndex - 2;
        } else {
            if (hdr->frameNumber < (u32)frame && best < pos && pos < sectors) {
                best = pos;
            }
            pos = (frame - 1) * perFrame - ((frame - 1) >> 2);
            pos += pos / 7;
            ArchiveReadFileFromCdSector(ArchiveDecodeSector(fileIndex) + pos, sector, 0x800, 0, 0);
            ArchiveCdDataSync(0);
            if ((s32)hdr->frameNumber == frame && pos < sectors) {
                if (hdr->sectorIndex == 0) {
                    goto done;
                }
                start = pos - hdr->sectorIndex - 2;
            } else {
                if (hdr->frameNumber < (u32)frame && best < pos && pos < sectors) {
                    best = pos;
                }
                start = (perFrame - 1) * (frame - 1);
                if (best > 0) {
                    start = best;
                }
            }
        }
        do {
            pos = start;
            ArchiveReadFileFromCdSector(ArchiveDecodeSector(fileIndex) + pos, sector, 0x800, 0, 0);
            ArchiveCdDataSync(0);
            start = pos + 1;
            if (hdr->status == 0x160) {
                start = pos + (hdr->sectorCount - hdr->sectorIndex);
            }
        } while ((s32)hdr->frameNumber != frame && start < sectors);
done:
        result = pos;
        if (start >= sectors) {
            result = -1;
        }
        return result;
    }
}

/* 0x8007519C: frame number stored in the last sector of the selected movie
 * file (the END FRAME probe).  Returns -1 when unavailable. */
s32 func_8007519C(void)
{
    u8 sector[0x800];
    s32 fileIndex;
    char* path;
    s32 sectorSize;
    s32 headerLen;
    s32 headerOffset;
    s32 result = -1;

    if (D_80077448 == 0) {
        ArchiveSetIndex(0x18, 0);
        if (!(D_8007711C < ArchiveDecodeSizeAbsolute(2))) {
            return 0;
        }
        fileIndex = D_8007711C + 3;
        path = ArchiveGetFilePath(fileIndex);
        sectorSize = 0x800;
        headerLen = 0x20;
        headerOffset = 0;
    } else if (D_80077448 == 1) {
        ArchiveSetIndex(0x18, 1);
        if (!(D_8007711C < ArchiveDecodeSizeAbsolute(1))) {
            return 0;
        }
        fileIndex = D_8007711C + 2;
        path = ArchiveGetFilePath(fileIndex);
        sectorSize = 0x920;
        headerLen = 0x28;
        headerOffset = 8;
    } else if (D_80077448 == 2) {
        return -1;
    } else {
        return 0;
    }

    if (func_8002C3D8() != 0) {
        StrPictureHeader* hdr = (StrPictureHeader*)(sector + headerOffset);
        s32 fd = PCopen(path, 0, 0);
        s32 size = PClseek(fd, 0, 2);

        PClseek(fd, 0, 0);
        PClseek(fd, size - sectorSize, 0);
        PCread(fd, sector, headerLen);
        if (hdr->status == 0x160) {
            result = hdr->frameNumber;
        }
        PCclose(fd);
        return result;
    } else {
        StrPictureHeader* hdr = (StrPictureHeader*)sector;
        s32 sectors = (ArchiveDecodeAlignedSize(fileIndex) + sectorSize - 1) / sectorSize;

        ArchiveReadFileFromCdSector(ArchiveDecodeSector(fileIndex) + sectors - 1, sector, 0x800, 0, 0);
        ArchiveCdDataSync(0);
        if (hdr->status == 0x160) {
            return hdr->frameNumber;
        }
        return -1;
    }
}

/* ------------------------------------------------------------------------- */
/* Playback                                                                  */
/* ------------------------------------------------------------------------- */

/* 0x800768D8: frame callback from the player module.  `y` is the VRAM row the
 * picture was decoded to; it selects which of the two display environments
 * shows it. */
void func_800768D8(u16 frame, u16 x, u16 y)
{
    D_80077010 = frame;
    if (D_801D68B4 == 1) {
        if (y != D_80077024) {
            D_80077018 = 0;
        } else {
            D_80077018 = 1;
        }
    } else if (y != D_80077024) {
        D_80077018 = 0;
        D_8007701C = 0;
    } else {
        D_80077018 = 1;
        D_8007701C = 1;
    }
    if (frame >= D_8007739C && D_80077438 == 0) {
        D_80077014 = 1;
    }
}

/* 0x800769A4: pad polling during playback; cross or start skips the movie
 * unless skipping is disabled. */
void func_800769A4(void)
{
    D_800773B4 = D_800773AC;
    D_800773AC = ControllerGetButtonState(0);
    if (D_800773B4 == D_800773AC) {
        if (D_800773B4 != 0) {
            D_80076F04++;
            if (D_80076F08 < D_80076F04) {
                D_800773B4 = 0;
                D_80076F08 = 1;
                D_80076F04 = 0;
            }
        } else {
            D_80076F08 = 0x5A;
            D_80076F04 = 0;
        }
    } else {
        D_80076F08 = 0x5A;
        D_80076F04 = 0;
    }

    if (!(D_800773B4 & 0x40) && (D_800773AC & 0x40) && D_80077028 == 0) {
        SoundSetCdVolumeWithFade(0, 10);
        D_80077014 = 5;
    }
    if (!(D_800773B4 & 0x800) && (D_800773AC & 0x800) && D_80077028 == 0) {
        SoundSetCdVolumeWithFade(0, 10);
        D_80077014 = 5;
    }
}

/* 0x80076488: play the selected movie file to completion or skip */
s32 func_80076488(void)
{
    RECT clear = D_800704E0;
    RECT copy;
    s32 fileIndex;
    s32 hasAudio;
    s32 pace;
    s32 i;

    D_80077014 = 0;
    D_80077020 = 0;
    if (D_80077448 == 0) {
        fileIndex = D_8007711C + 3;
        hasAudio = 0;
    } else {
        fileIndex = D_8007711C + 2;
        hasAudio = 1;
    }

    if (ArchiveDecodeSize(fileIndex) == 0x18) {
        SetDispMask(1);
        D_8004FE46 = 0;
        return 0;
    }

    Vsync(0);
    ClearImage(&clear, 0, 0, 0);
    DrawSync(0);
    Vsync(0);
    D_80077018 = 0;
    D_8007701C = 0;
    func_801D43B0();
    if (D_800773A0 > 0) {
        D_80077024 = (0xF0 - D_800773A0) / 2;
    } else {
        D_80077024 = 0;
    }
    func_801D3538(0x140, 0xF0, 0x80, 0x10, 0x20, 0x800, (u16)D_80077454);
    D_801D68B4 = 0;
    func_801D37CC((u16)fileIndex, D_800773A8, (u16)D_800773A4, (u16)D_8007739C, (u16)D_80077398,
                  hasAudio, D_80077438 != 0, 0, (u16)D_80077024, 0,
                  (u16)(D_80077024 + 0xF0), (u16)D_800773A0, func_800768D8);

    pace = 30;
    PutDrawEnv(&D_80077124[D_80077018].draw);
    PutDispEnv(&D_80077124[D_80077018].disp);
    SetDispMask(1);

    do {
        if (D_80077020 == 0) {
            s32 pumps = pace / 10;
            s32 slot = 1;

            for (i = 0; i < pumps; i++) {
                s32 before = Vsync(1);
                s32 after;

                func_801D3F7C();
                after = Vsync(1);
                if (slot < 0x20) {
                    D_800773B8[i][0] = before;
                    D_800773B8[i][1] = after;
                }
                slot += 2;
            }
            pace -= (pace / 10) * 10;
        }
        pace += 30;

        func_800769A4();
        GameCheckAndHandleSoftReset();
        Vsync(0);
        PutDispEnv(&D_80077124[D_8007701C].disp);
        D_8007701C = D_80077018;
        if (D_80077014 == 1) {
            break;
        }
        if (D_80077014 >= 2) {
            D_80077014--;
        }
    } while (1);

    func_801D4318();
    if (D_8007701C == 0) {
        DrawSync(0);
        Vsync(0);
        copy.x = 0;
        copy.y = 0xF0;
        copy.w = 0x1E0;
        copy.h = 0xF0;
        MoveImage(&copy, 0, 0);
        DrawSync(0);
        Vsync(0);
        PutDispEnv(&D_80077124[1].disp);
    }
    return 0;
}

/* 0x800763BC: shipping entry — play movie D_8007711C of the selected type */
void func_800763BC(u8 noSkip)
{
    s32 play;

    D_80077028 = noSkip;
    D_800773AC = -1;
    if (D_80077448 == 0) {
        ArchiveSetIndex(0x18, 0);
        play = D_8007711C < (s16)ArchiveDecodeSizeAbsolute(2);
    } else if (D_80077448 == 1) {
        ArchiveSetIndex(0x18, 1);
        play = D_8007711C < (s16)ArchiveDecodeSizeAbsolute(1);
    } else if (D_80077448 == 2) {
        play = 0;
    } else {
        play = 1;
    }

    if (play) {
        D_80077124[0].draw.isbg = 0;
        D_80077124[1].draw.isbg = 0;
        D_80077124[0].disp.isrgb24 = 1;
        D_80077124[1].disp.isrgb24 = 1;
        func_80076488();
    }
}

/* 0x8007625C: MOVIE START from the developer menu */
void func_8007625C(void)
{
    RECT clear = D_800704E0;
    s32 play;

    D_80077028 = 0;
    D_800773AC = -1;
    if (D_80077448 == 0) {
        ArchiveSetIndex(0x18, 0);
        play = D_8007711C < (s16)ArchiveDecodeSizeAbsolute(2);
    } else if (D_80077448 == 1) {
        ArchiveSetIndex(0x18, 1);
        play = D_8007711C < (s16)ArchiveDecodeSizeAbsolute(1);
    } else if (D_80077448 == 2) {
        play = 0;
    } else {
        play = 1;
    }

    if (play) {
        SetDispMask(0);
        D_80077124[0].draw.isbg = 0;
        D_80077124[1].draw.isbg = 0;
        if (D_80077454 != 0) {
            D_80077124[0].disp.isrgb24 = 1;
            D_80077124[1].disp.isrgb24 = 1;
        }
        func_80076488();
        Vsync(0);
        ClearImage(&clear, 0, 0, 0);
        DrawSync(0);
        Vsync(0);
        D_80077124[0].draw.isbg = 1;
        D_80077124[1].draw.isbg = 1;
        if (D_80077454 != 0) {
            D_80077124[0].disp.isrgb24 = 0;
            D_80077124[1].disp.isrgb24 = 0;
        }
    }
}

/* ------------------------------------------------------------------------- */
/* Developer CD tools                                                        */
/* ------------------------------------------------------------------------- */

/* 0x80075D8C: FAT CHECK — list archive table entries */
void func_80075D8C(void)
{
    s32 savedDir;
    s32 savedEntry;
    s32 button;
    s32 first = 0;
    s32 hex = 0;
    s32 names = 0;

    func_80074B58();
    ArchiveGetArchiveOffsetIndices(&savedDir, &savedEntry);
    ArchiveSetIndex(0, 0);
    do {
        s32 i;

        MovieSwapRenderEnv();
        FontPrintf("\n[ FAT CHECK MODE ");
        FontPrintf(hex ? "HEX ]\n" : "DEC ]\n");
        func_800747AC(0, 0, &button);
        if (!(D_800773B4 & 0x1000) && (D_800773AC & 0x1000) && first > 0) {
            first--;
        }
        if (!(D_800773B4 & 0x10) && (D_800773AC & 0x10)) {
            first -= 0x14;
            if (first < 0) {
                first = 0;
            }
        }
        if (!(D_800773B4 & 0x4000) && (D_800773AC & 0x4000) && first < 0x1235) {
            first++;
        }
        if (!(D_800773B4 & 0x40) && (D_800773AC & 0x40)) {
            first += 0x14;
            if (first > 0x1235) {
                first = 0x1235;
            }
        }
        if (!(D_800773B4 & 8) && (D_800773AC & 8)) {
            hex = 1 - hex;
        }
        if (!(D_800773B4 & 4) && (D_800773AC & 4)) {
            names = 1 - names;
        }

        for (i = first; i < first + 0x14; i++) {
            if (func_80075D4C(i) == 0) {
                FontPrintf(hex ? "No %4x NullFile\n" : "No %4d NullFile\n", i);
                continue;
            }
            FontPrintf(hex ? "No %4x Sect%6x " : "No %4d Sect%6d ", i, ArchiveDecodeSector(i + 1));
            if (names) {
                char* path;

                if (func_80075D4C(i) < 0) {
                    FontPrintf("[P%3d]\n", -func_80075D4C(i));
                    continue;
                }
                path = ArchiveGetFilePath(i + 1);
                if (path != NULL) {
                    char* p = path;

                    while (*p != 0) {
                        if (*p == '\\') {
                            path = p + 1;
                        }
                        p++;
                    }
                    FontPrintf("%s\n", path);
                } else {
                    FontPrintf("\n", path);
                }
            } else {
                FontPrintf(hex ? "Size%9x\n" : "Size%9d\n", func_80075D4C(i));
            }
        }

        FontPrintf("\nPUSH CIRCLE BUTTON TO MENU.");
        if (D_80077394 > 0) {
            HeapDebugDump(1, 0, 6, 0x808D);
        }
        FontDrawLetters(D_80077120->ot);
        func_80072F98(D_80077120->ot, &D_80077120->box, 8, 0x14, 0x130, 0xC0);
        func_800734B8(D_80077120->ot, &D_80077120->frame, 7, 0x13, 0x132, 0xC2);
        MoviePresent();
    } while (button != 2);
    ArchiveSetIndex(savedDir, savedEntry);
}

/* Placeholders for the untranscribed developer tools (see file header).
 * DIAGNOSTIC: each prints once and returns to the menu.  Removal: transcribe
 * func_800704E8 / func_80075534 / func_80072480 from asm/movie/9F8.s. */
void func_800704E8(void)
{
#ifdef XENO_PC_PORT
    printf("[movie-debug-tool] CD-ROM CHECK (func_800704E8) not transcribed\n");
#endif
}

void func_80075534(void)
{
#ifdef XENO_PC_PORT
    printf("[movie-debug-tool] CD-ROM MONITOR (func_80075534) not transcribed\n");
#endif
}

void func_80072480(void)
{
#ifdef XENO_PC_PORT
    printf("[movie-debug-tool] DISC CHANGE (func_80072480) not transcribed\n");
#endif
}

/* ------------------------------------------------------------------------- */
/* 0x800737EC: game state 6 main                                             */
/* ------------------------------------------------------------------------- */

void MovieMain(void)
{
    char trouble[8] = "trouble";
    void* pModule;
    void* pProbe;
    s32 button;
    s32 step;
    s32 stepScale;
    s32 i;

    HeapChangeCurrentUser(4, NULL);
    ArchiveSetIndex(0x18, 0);
    SoundSetCdVolumeWithFade(0, 0);
    DrawSync(0);
    Vsync(0);
    SetDispMask(0);

    /* Place the player module at 0x801D3000: size the top-of-heap block so it
     * ends where a fresh top allocation would start. */
    pProbe = HeapAlloc(4, 1);
#ifdef XENO_PC_PORT
    pModule = HeapAlloc((((u32)((u8*)pProbe - g_PsxRam)) & 0xFFFFFF) + 0xFFE2CFF8, 1);
#else
    pModule = HeapAlloc(((u32)pProbe & 0xFFFFFF) + 0xFFE2CFF8, 1);
#endif
    HeapFree(pProbe);
#ifdef XENO_PC_PORT
    ArchiveReadFileToBuffer(1, pModule, 0, 0);
#else
    ArchiveReadFileToBuffer(1, (s32)pModule, 0, 0);
#endif
    ArchiveCdDataSync(0);
    func_801D3538(0x140, 0x100, 0x80, 0x10, 0x20, 0x800, 3);

    D_80077450 = func_8002C3D8();
    D_800773A0 = -1;
    D_8007739C = 0xC80;
    D_80077394 = 0;
    D_800773B0 = 0;
    D_80077454 = 1;
    D_80077448 = 1;
    D_8007711C = 0;
    D_800773A4 = 1;
    D_80077398 = 1;
    D_80077444 = 2;
    D_8007743C = 0;
    D_800773A8 = 0;
    D_80077438 = 0;

    SetDefDrawEnv(&D_80077124[0].draw, 0, 0, 0x140, 0xF0);
    SetDefDispEnv(&D_80077124[0].disp, 0, 0xF0, 0x140, 0xF0);
    SetDefDrawEnv(&D_80077124[1].draw, 0, 0xF0, 0x140, 0xF0);
    SetDefDispEnv(&D_80077124[1].disp, 0, 0, 0x140, 0xF0);
    D_80077124[0].draw.dtd = 1;
    D_80077124[0].draw.isbg = 1;
    D_80077124[1].draw.dtd = 1;
    D_80077124[1].draw.isbg = 1;
    D_80077124[0].draw.r0 = 0;
    D_80077124[0].draw.g0 = 0;
    D_80077124[0].draw.b0 = 0;
    D_80077124[0].disp.isinter = 0;
    D_80077124[1].draw.r0 = 0;
    D_80077124[1].draw.g0 = 0;
    D_80077124[1].draw.b0 = 0;
    D_80077124[1].disp.isinter = 0;
    setRECT(&D_80077124[0].disp.screen, 0, 0xA, 0x100, 0xD8);
    setRECT(&D_80077124[1].disp.screen, 0, 0xA, 0x100, 0xD8);
    D_80077120 = &D_80077124[0];
    D_8007744C = 0;
    PutDrawEnv(&D_80077124[0].draw);
    PutDispEnv(&D_80077120->disp);

    if (D_80077450 != 0) {
        Vsync(2);
        D_800773AC = ControllerGetButtonState(0);
    } else {
        D_800773AC = 0;
    }

    if (D_8004FE44 != 0xFF && !(D_800773AC & 0x100)) {
        D_80077440 = 0;
        D_80077398 = 1;
        D_800773A4 = 1;
        D_80077448 = D_8004FE44 & 0x7F;
        D_8007711C = D_8004FE45;
        if (D_8004FE44 & 0x80) {
            D_8007739C = D_80062514;
        } else {
            D_8007739C = 0xE9;
        }
        func_800763BC(D_8004FE47);
        func_801D43B0();
        HeapFree(pModule);
        ChangeGameState(D_8004FE46);
        MainLoop(0);
    }

    /* Developer movie menu */
    func_80072D84(&D_80077124[0].box, &D_80077124[1].box, 0, 0, 0, 0);
    func_80073328(&D_80077124[0].frame, &D_80077124[1].frame, 0, 0, 0, 0);
    FontLoadFont(0x10, 0x10, 0x280, 0xF0, 0x400, 0, 0x280, 0, 0x280, 0x100, 0);
    D_80077440 = 1;
    SetDispMask(1);

    while (1) {
        u32 cursor;

        MovieSwapRenderEnv();
        if (D_80077450 == 0) {
            FontPrintf("  [ MOVIE CD-ROM MODE1 DISK %1d ]  \n\n", ArchiveGetDiscNumber());
        } else if (D_80077450 == -1) {
            FontPrintf("  [ MOVIE CD-ROM MODE2 DISK %1d ]  \n\n", ArchiveGetDiscNumber());
        } else {
            FontPrintf("  [ MOVIE PC HDD MODE  DISK %1d ]  \n\n", ArchiveGetDiscNumber());
        }
        FontPrintf("    ERROR %2d Sect %2d:%2d FM%3d\n", D_8005A4DC, D_8005A4A8, D_8005A4B4, D_8005A4B8);
        FontPrintf("    LesMem%2d NoMem%2d Skp%3d\n", D_8005A49C, D_8005A4A4, D_801E89D4, D_80062514);

        step = func_800747AC(0, 0xD, &button);
        stepScale = 1;
        if (D_800773AC & 0x10) {
            stepScale = 0x20;
        }
        if (D_800773AC & 0x80) {
            stepScale <<= 7;
        }

        if (D_80077118 == 0 && step != 0) {
            D_80077448 += step;
            if (D_80077448 < 0) {
                D_80077448 = 2;
            }
            if (D_80077448 >= 3) {
                D_80077448 = 0;
            }
            D_800773A4 = 1;
            D_800773A8 = 0;
            D_8007743C = 0;
            D_80077444 = 2;
        }
        if (D_80077118 == 1 && step != 0) {
            D_8007711C += step;
            if (D_8007711C < 0) {
                D_8007711C = 0x3F;
            }
            if (D_8007711C >= 0x40) {
                D_8007711C = 0;
            }
            D_800773A4 = 1;
            D_800773A8 = 0;
            D_8007743C = 0;
            D_80077444 = 2;
        }
        if (D_80077118 == 2) {
            if (step != 0) {
                D_800773A4 += step * stepScale;
                if (D_800773A4 <= 0) {
                    D_800773A4 = 1;
                }
                if (D_800773A4 >= 0x2000) {
                    D_800773A4 = 0x1FFF;
                }
                if (D_8007739C < D_800773A4) {
                    D_800773A4 = D_8007739C;
                }
                D_80077444 = 1;
            }
            if (D_80077118 == 2 && button == 2 && D_80077444 == 1) {
                D_800773A8 = func_80074BA4(D_800773A4);
                if (D_800773A4 >= D_8007739C) {
                    D_8007739C = D_800773A4;
                    D_8007743C = 0;
                }
                D_80077444 = 2;
            }
        }
        if (D_80077118 == 3) {
            if (step != 0) {
                D_8007739C += step * stepScale;
                if (D_8007739C <= 0) {
                    D_8007739C = 1;
                }
                if (D_8007739C >= 0x2000) {
                    D_8007739C = 0x1FFF;
                }
                if (D_8007739C < D_800773A4) {
                    D_8007739C = D_800773A4;
                }
                D_8007743C = 0;
            }
            if (D_80077118 == 3 && button == 2 && D_8007743C == 0) {
                s32 last = func_8007519C();

                if (last >= 0) {
                    D_8007739C = last;
                    D_8007743C = 1;
                    if (D_800773A4 >= last) {
                        D_800773A4 = last;
                        D_80077444 = 1;
                    }
                } else {
                    D_8007743C = 2;
                }
            }
        }
        if (D_80077118 == 4 && step != 0) {
            D_80077398 += step;
            if (D_80077398 < 0) {
                D_80077398 = 7;
            }
            if (D_80077398 >= 8) {
                D_80077398 = 0;
            }
        }
        if (D_80077118 == 5 && step != 0) {
            D_80077454 = 1 - D_80077454;
        }
        if (D_80077118 == 6) {
            if (step != 0) {
                D_800773A0 = (D_800773A0 + step * stepScale) & 0xFF;
            }
            if (button == 2) {
                D_800773A0 = -1;
            }
        }
        if (D_80077118 == 7 && step != 0) {
            D_80077438 = 1 - D_80077438;
        }

        for (i = 0; i < 0xE; i++) {
            FontPrintf(D_80077118 == (u32)i ? "  >" : "   ");
            switch (i) {
            case 0:
                FontPrintf(" MOVIE TYPE   ");
                switch (D_80077448) {
                case 0:
                    FontPrintf("PICTURE ONLY\n");
                    break;
                case 1:
                    FontPrintf("PICTURE+ADPCM\n");
                    break;
                case 2:
                    FontPrintf("ADPCM ONLY\n");
                    break;
                }
                break;
            case 1:
                FontPrintf(" MOVIE NUMBER %4d\n\n", D_8007711C);
                break;
            case 2:
                FontPrintf(" START FRAME  %4d ", D_800773A4);
                if (D_80077444 == 1) {
                    FontPrintf("SET");
                }
                if (D_80077444 == 2) {
                    if (D_800773A8 < 0) {
                        FontPrintf("EOF", D_800773A8);
                    } else {
                        FontPrintf("+%4dSECT", D_800773A8);
                    }
                }
                FontPrintf("\n");
                break;
            case 3:
                FontPrintf(" END   FRAME  %4d ", D_8007739C);
                if (D_8007743C == 0) {
                    FontPrintf("SET");
                }
                if (D_8007743C == 2) {
                    FontPrintf("???");
                }
                FontPrintf("\n");
                break;
            case 4:
                FontPrintf(" MOVIE CHANNEL %3d\n", D_80077398);
                break;
            case 5:
                FontPrintf(" SCREEN MODE  ");
                FontPrintf(D_80077454 != 0 ? "24 BIT COLOR" : "16 BIT COLOR");
                FontPrintf("\n");
                break;
            case 6:
                FontPrintf(" SCREEN DRAW  ");
                if (D_800773A0 < 0) {
                    FontPrintf("ALL", D_800773A0);
                } else {
                    FontPrintf("%3d", D_800773A0);
                }
                FontPrintf("\n");
                break;
            case 7:
                FontPrintf(" REWIND       ");
                FontPrintf(D_80077438 != 0 ? "ON" : "OFF");
                FontPrintf("\n\n");
                break;
            case 8:
                FontPrintf(" MOVIE START.\n\n");
                break;
            case 9:
                FontPrintf(" CD-ROM MONITOR.\n\n");
                break;
            case 10:
                FontPrintf(" CD-ROM CHECK.\n");
                break;
            case 11:
                FontPrintf(" FAT CHECK.\n\n");
                break;
            case 12:
                FontPrintf(" [DISC CHANGE.]\n");
                break;
            case 13:
                FontPrintf(" [RETURN TO KERNEL.]\n");
                break;
            }
        }

        if (D_80077394 > 0) {
            HeapDebugDump(1, 0, 6, 0x808D);
        }
        FontDrawLetters(D_80077120->ot);
        func_80072F98(D_80077120->ot, &D_80077120->box, 0x14, 0xC, 0x11C, 0xC6);
        func_800734B8(D_80077120->ot, &D_80077120->frame, 0x13, 0xB, 0x11E, 0xC8);
        MoviePresent();

        cursor = D_80077118;
        if ((cursor < 2 || cursor - 4 < 2 || cursor - 7 < 2) && button == 2) {
            D_8005A49C = 0;
            D_8005A4A4 = 0;
            D_8005A4A8 = 0;
            D_8005A4B4 = 0;
            func_8007625C();
            D_800773AC = -1;
        }
        if (D_80077118 == 9 && button == 2) {
            func_80075534();
        }
        if (D_80077118 == 0xA && button == 2) {
            func_800704E8();
        }
        if (D_80077118 == 0xB && button == 2) {
            func_80075D8C();
        }
        if (D_80077118 == 0xC && button == 2) {
            func_80072480();
        }
        if (D_80077118 == 0xD && button == 2) {
            HeapFree(pModule);
            MainLoop(0);
        }
        D_80077118 = cursor;
        GameCheckAndHandleSoftReset();
    }
}
