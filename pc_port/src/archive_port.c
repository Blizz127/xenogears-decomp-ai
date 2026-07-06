/*
 * archive_port.c - synchronous CD/archive read path for the PC port.
 *
 * On PSX, the game's archive layer streams files off the CD with an asynchronous
 * state machine: ArchiveReadFile() arms CdReadyCallback/CdSyncCallback +
 * CdControlF(CdlSetloc), and the registered callbacks (func_8002B084 /
 * func_8002BA58 / func_8002B2F0, all still raw MIPS) copy each delivered sector
 * into the destination buffer as the drive spools. ArchiveCdDataSync() then busy-
 * polls g_ArchiveCdDriveState until the callbacks drive it back to IDLE.
 *
 * In the port that whole dance is unavailable: the copy callbacks are unported
 * no-op stubs, and reproducing PSX async-CD callback timing on top of PsyCross's
 * spooler thread is fragile. PsyCross does, however, expose a fully synchronous
 * libcd read (CdControlB(CdlSetloc) seeks + selects data mode, CdRead/CdReadSync
 * deliver 2048 data bytes per sector straight from the disc image). So we exclude
 * the game's src/.../system/archive.c from the port build (see build_port.sh) and
 * provide a synchronous ArchiveReadFile here. The arithmetic-only archive helpers
 * (ArchiveSetIndex, ArchiveDecode*, ArchiveReadFileToBuffer/FromCdSector) still
 * come from the compiled libarchive.c and call straight into this function.
 *
 * Everything else that lived in archive.c (the async ArchiveCdDriveCommandHandler,
 * ArchiveCdSeek*, ArchiveCdSetMode, the stream-file helpers) becomes a logged
 * no-op stub. None of those are on the synchronous overlay-load path; they get
 * real implementations here only if a [stub] log shows the game needs them.
 */

#include "common.h"
#include "system/archive.h"
#include "psyq/libcd.h"
#include "psyq/libgpu.h"   /* RECT, LoadImage, DrawSync for the opt-in 0xBB VRAM uploader */

/* Forward-declare the two host-libc calls used for the partial-sector bounce.
 * Pulling in <stdlib.h> here fails to compile: the game headers (via common.h)
 * leave the TU without int32_t, which glibc's <stdlib.h> then references. memcpy
 * is already declared through common.h (psyq/memory.h). */
extern void* malloc(unsigned long size);
extern void  free(void* ptr);
extern char* getenv(const char* name);   /* opt-in XENO_FIELD_0BB_VRAM_UPLOAD gate */

/* Set by ArchiveReadFileToBuffer/ArchiveReadFileFromCdSector (libarchive.c) just
 * before they call us: the absolute CD sector and the byte length to read. */
extern s32 g_ArchiveCurFileSector;
extern int ArchiveReadFileToBuffer(int entryIndex, void* pDestBuffer, int arg2, int flags);
extern int ArchiveDecodeSize(int entryIndex);
extern int ArchiveDecodeSector(int entryIndex);
extern int ArchiveDecodeAlignedSize(unsigned int entryIndex);
extern void* HeapAlloc(u_int size, u_int allocMode);
extern void ArchiveChangeStreamingFile(void* pStreamFile);
extern void ArchiveClearStreamFileSections(void);
extern void ArchiveCdDataSync(int mode);
extern u8 D_800B2394;

static u16 ArchivePsxQueueGetIndex(u8* pEntries, int index) {
    return *(u16*)(pEntries + index * 8);
}

static u32 ArchivePsxQueueGetData(u8* pEntries, int index) {
    return *(u32*)(pEntries + index * 8 + 4);
}

static void ArchivePsxQueueSetEntry(u8* pEntries, int index, u16 archiveIndex, u32 pData) {
    *(u16*)(pEntries + index * 8) = archiveIndex;
    *(u32*)(pEntries + index * 8 + 4) = pData;
}

static int ArchiveReadPsxStreamQueue(u8* pEntries, int arg1) {
    int count;
    int i;

    if (pEntries == NULL || ArchivePsxQueueGetIndex(pEntries, 0) == 0) {
        return -3;
    }

    for (count = 0; ArchivePsxQueueGetIndex(pEntries, count) != 0; count += 1) {
    }
    if (count == 0) {
        return -3;
    }

    for (i = 0; i < count - 1; i += 1) {
        int minIndex = i;
        int j;
        for (j = i + 1; j < count; j += 1) {
            if (ArchivePsxQueueGetIndex(pEntries, j) < ArchivePsxQueueGetIndex(pEntries, minIndex)) {
                minIndex = j;
            }
        }
        if (minIndex != i) {
            u16 tmpIndex = ArchivePsxQueueGetIndex(pEntries, i);
            u32 tmpData = ArchivePsxQueueGetData(pEntries, i);
            ArchivePsxQueueSetEntry(pEntries, i,
                                    ArchivePsxQueueGetIndex(pEntries, minIndex),
                                    ArchivePsxQueueGetData(pEntries, minIndex));
            ArchivePsxQueueSetEntry(pEntries, minIndex, tmpIndex, tmpData);
        }
    }

    ArchiveCdDataSync(0);
    D_8004FE18 = g_CurArchiveOffset;
    D_8004FDFC = 0;

    for (i = 0; i < count; i += 1) {
        u32 pData = ArchivePsxQueueGetData(pEntries, i);
        if (pData != 0) {
            ArchiveReadFileToBuffer(ArchivePsxQueueGetIndex(pEntries, i),
                                    (void*)(unsigned long)pData,
                                    arg1,
                                    CdlModeSpeed);
            ArchiveCdDataSync(0);
        }
    }

    g_ArchiveCurFileSize = 0;
    D_8004FDFC = 0;
    g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_IDLE;
    return 0;
}

int* ArchiveAllocStreamFile(int numEntries, int allocMode) {
    int* pStreamFile;

    if (numEntries > 0) {
        pStreamFile = HeapAlloc((numEntries * (CD_SECTOR_SIZE + sizeof(ArchiveStreamFileSectionHeader))) + STREAM_FILE_HEADER_SIZE, allocMode);
        if (pStreamFile) {
            *pStreamFile = numEntries;
            ArchiveChangeStreamingFile(pStreamFile);
            ArchiveClearStreamFileSections();
            return pStreamFile;
        }
    }
    return NULL;
}

/*
 * PcPortDrain0xBBToVram - opt-in (XENO_FIELD_0BB_VRAM_UPLOAD=1) synchronous port
 * of the streamed field-VRAM uploader (retail func_8002BF38, unported raw MIPS).
 *
 * The per-map archive ((mapNum<<1)+0xB9; 0xBB on Map1) carries the field's VRAM
 * payload (model textures + background strips). Retail streams it to VRAM at
 * field-in via the CD callbacks; the port reads it synchronously in
 * func_80029EB0 below and previously discarded it, leaving the texture pages
 * the model packets sample (tpage 0x15/0x95 ~ VRAM (320,256)) empty.
 *
 * Layout (decoded from func_8002BF38.s, verified live on Map1):
 *   0x800-aligned sections: 1 header sector + stripCount strip sectors (one
 *   strip per sector). Header: +0x00 u32 tag (0x1200|0x1201; else end),
 *   +0x04/+0x08 u16 -> x, +0x06/+0x0A u16 -> y, +0x0C u16 width, +0x14 u32
 *   blockCount (first section), +0x18 u32 stripCount, +0x1C u16[] heights.
 *   Strip i pixels: sector off+(1+i)*0x800, width*height*2 bytes raw 15-bit
 *   RGB; y += height per strip. Base coords are 0 (caller args) -> absolute.
 *
 * Diagnostics: logs every uploaded strip's rect + classifies overlap with the
 * model texture page column (VRAM x 320..448, y 256..511 for tpage 0x15/0x95)
 * and the actor-sprite band (x 448..704, y 256..511, incl. Fei's tpage 0x19 at
 * 576,256 -- retail reconciles that overlap via the per-frame sprite re-blit
 * func_80025044, which the port dispatch calls). Hard bounds-check before
 * every LoadImage; invalid section/strip stops the parse loudly.
 */
static void PcPortDrain0xBBToVram(u8* buf, s32 sizeBytes) {
    s32 off = 0;
    s32 blockCount = -1;
    s32 sectionsDone = 0;
    s32 stripsDone = 0;
    s32 modelPageStrips = 0;
    s32 spriteBandStrips = 0;

    while (off + 0x800 <= sizeBytes) {
        u8* p = buf + off;
        u32 tag = *(u32*)p;
        s32 x, y, width, stripCount, i;
        u8* heightTable;

        if (tag != 0x1200 && tag != 0x1201) {
            if (sectionsDone == 0) {
                printf("[field-0bb] first tag 0x%08x not 0x1200/0x1201 -> not a VRAM stream, no upload\n", tag);
            } else {
                printf("[field-0bb] end of stream: %d sections, %d strips (modelPage=%d spriteBand=%d)\n",
                       sectionsDone, stripsDone, modelPageStrips, spriteBandStrips);
            }
            return;
        }

        x = *(u16*)(p + 0x4) + *(u16*)(p + 0x8);
        y = *(u16*)(p + 0x6) + *(u16*)(p + 0xA);
        width = *(u16*)(p + 0xC);
        if (blockCount < 0) {
            blockCount = *(s32*)(p + 0x14);
        }
        stripCount = *(s32*)(p + 0x18);
        heightTable = p + 0x1C;

        if (stripCount <= 0 || stripCount > 1024 || 0x1C + stripCount * 2 > 0x800) {
            printf("[field-0bb] STOP: section %d bad stripCount=%d at off 0x%x\n", sectionsDone, stripCount, off);
            return;
        }

        for (i = 0; i < stripCount; i++) {
            s32 height = *(u16*)(heightTable + i * 2);
            s32 sectorOff = off + (1 + i) * 0x800;
            RECT rect;
            s32 hitsModelPage;
            s32 hitsSpriteBand;

            if (width <= 0 || height <= 0 ||
                x < 0 || y < 0 || x + width > 1024 || y + height > 512 ||
                sectorOff + width * height * 2 > sizeBytes) {
                printf("[field-0bb] STOP: section %d strip %d invalid rect/span x=%d y=%d w=%d h=%d srcOff=0x%x\n",
                       sectionsDone, i, x, y, width, height, sectorOff);
                return;
            }

            hitsModelPage = (x < 448 && x + width > 320 && y + height > 256);
            hitsSpriteBand = (x < 704 && x + width > 448 && y + height > 256);
            if (hitsModelPage) {
                modelPageStrips++;
            }
            if (hitsSpriteBand) {
                spriteBandStrips++;
            }

            rect.x = (short)x;
            rect.y = (short)y;
            rect.w = (short)width;
            rect.h = (short)height;
            LoadImage(&rect, (u_long*)(buf + sectorOff));

            if (stripsDone < 10 || hitsModelPage || hitsSpriteBand) {
                if (stripsDone < 10 || (modelPageStrips + spriteBandStrips) <= 12) {
                    printf("[field-0bb] strip %d sec=%d rect=(x=%d y=%d w=%d h=%d) bytes=%d%s%s\n",
                           stripsDone, sectionsDone, x, y, width, height, width * height * 2,
                           hitsModelPage ? " [MODEL-PAGE]" : "",
                           hitsSpriteBand ? " [SPRITE-BAND]" : "");
                }
            }
            stripsDone++;
            y += height;
        }

        off += (1 + stripCount) * 0x800;
        sectionsDone++;
        if (blockCount > 0 && sectionsDone >= blockCount) {
            DrawSync(0);
            printf("[field-0bb] done: %d/%d sections, %d strips (modelPage=%d spriteBand=%d)\n",
                   sectionsDone, blockCount, stripsDone, modelPageStrips, spriteBandStrips);
            return;
        }
    }
    DrawSync(0);
    printf("[field-0bb] buffer end: %d sections, %d strips\n", sectionsDone, stripsDone);
}

int func_80029EB0(s32 archiveIndex, void* pStreamFile, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7, s32 arg8, s32 arg9) {
    int nStreamSectors;
    int result;
    u8* pReadBuffer;

    (void)arg3;
    (void)arg4;
    (void)arg5;
    (void)arg6;
    (void)arg7;
    (void)arg8;
    (void)arg9;

    if (pStreamFile == NULL) {
        return -4;
    }

    nStreamSectors = *(s32*)pStreamFile;
    if (nStreamSectors < 2) {
        return -4;
    }

    if (archiveIndex <= 0 || ArchiveDecodeSize(archiveIndex) <= 0) {
        return -3;
    }

    ArchiveCdDataSync(0);
    D_8004FE18 = g_CurArchiveOffset;
    ArchiveChangeStreamingFile(pStreamFile);

    g_ArchiveCurFileSector = ArchiveDecodeSector(archiveIndex);
    g_ArchiveCurFileSize = ArchiveDecodeAlignedSize(archiveIndex);
    D_8004FE2C = (ArchiveStreamFileSectionHeader*)((u8*)pStreamFile + 4);
    D_8004FE40 = nStreamSectors;
    D_8004FDFC = 1;
    D_8004FE38 = arg2 & 0xFFFF;
    D_8004FE34 = 0;
    ArchiveClearStreamFileSections();

    pReadBuffer = (u8*)malloc(g_ArchiveCurFileSize);
    if (pReadBuffer == NULL) {
        D_8004FDFC = 0;
        g_ArchiveCurFileSize = 0;
        g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_IDLE;
        return -1;
    }

    {
        /* ArchiveReadFile clears g_ArchiveCurFileSize on completion, so capture
         * the byte length now for the opt-in VRAM uploader. */
        s32 vramUploadSize = g_ArchiveCurFileSize;
        const char* vramUploadEnv = getenv("XENO_FIELD_0BB_VRAM_UPLOAD");

        result = ArchiveReadFile(archiveIndex, pReadBuffer, arg2, 0);

        if (vramUploadEnv != NULL && vramUploadEnv[0] == '1' && result >= 0 && vramUploadSize > 0) {
            printf("[field-0bb] archive 0x%x: %d bytes read, draining to VRAM\n", archiveIndex, vramUploadSize);
            PcPortDrain0xBBToVram(pReadBuffer, vramUploadSize);
        }
    }
    free(pReadBuffer);
    return result;
}

int func_80029AFC(StreamDataQueueEntry* pEntries, int arg1, int arg2) {
    int count;
    int i;

    (void)arg2;

    if ((void*)pEntries == (void*)&D_800B2394) {
        return ArchiveReadPsxStreamQueue((u8*)pEntries, arg1);
    }

    if (pEntries == NULL || pEntries[0].archiveIndex == 0) {
        return -3;
    }

    for (count = 0; pEntries[count].archiveIndex != 0; count += 1) {
    }
    if (count == 0) {
        return -3;
    }

    for (i = 0; i < count - 1; i += 1) {
        int minIndex = i;
        int j;
        for (j = i + 1; j < count; j += 1) {
            if ((u16)pEntries[j].archiveIndex < (u16)pEntries[minIndex].archiveIndex) {
                minIndex = j;
            }
        }
        if (minIndex != i) {
            StreamDataQueueEntry tmp = pEntries[i];
            pEntries[i] = pEntries[minIndex];
            pEntries[minIndex] = tmp;
        }
    }

    ArchiveCdDataSync(0);
    D_8004FE18 = g_CurArchiveOffset;
    D_8004FDFC = 0;

    for (i = 0; i < count; i += 1) {
        if (pEntries[i].pData != NULL) {
            ArchiveReadFileToBuffer((u16)pEntries[i].archiveIndex,
                                    pEntries[i].pData,
                                    arg1,
                                    CdlModeSpeed);
            ArchiveCdDataSync(0);
        }
    }

    g_ArchiveCurFileSize = 0;
    D_8004FDFC = 0;
    g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_IDLE;
    return 0;
}

int ArchiveReadFile(u32 dbgEntryIndex, u8* pDestBuffer, s32 arg2, s32 flags) {
    int nSectors;
    CdlLOC loc;

    (void)dbgEntryIndex;
    (void)arg2;

    /* Streaming reads (field BG/audio: CdlModeStream 0x100, and the 0x200 ADPCM
     * path) aren't ported yet -- they need the section-queue machinery. Fail
     * gracefully so callers fall back rather than read garbage. */
    if (flags & (CdlModeStream | 0x200)) {
        return -4;
    }

    if (pDestBuffer == NULL || g_ArchiveCurFileSize <= 0) {
        g_ArchiveCurFileSize = 0;
        D_8004FDFC = 0;
        g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_IDLE;
        return 0;
    }

    /* Round the byte length up to whole 2048-byte data sectors. */
    nSectors = (g_ArchiveCurFileSize + (CD_SECTOR_SIZE - 1)) / CD_SECTOR_SIZE;

    /* Same sector addressing the hardware path uses (CdIntToPos on the raw
     * sector). CdControlB(CdlSetloc) seeks the image and selects RM_DATA so
     * CdReadSync extracts the 2048-byte data payload from each MODE2/2352 sector. */
    CdIntToPos(g_ArchiveCurFileSector, &loc);
    CdControlB(CdlSetloc, (u_char*)&loc, NULL);

    if (nSectors * CD_SECTOR_SIZE == (int)g_ArchiveCurFileSize) {
        /* File is a whole number of sectors: read straight into the buffer. */
        CdRead(nSectors, (u_long*)pDestBuffer, 0);
        CdReadSync(0, NULL);
    } else {
        /* Partial last sector. The game sizes buffers to ArchiveDecodeAlignedSize
         * (4-byte aligned, NOT sector-rounded), so pDestBuffer holds exactly
         * g_ArchiveCurFileSize bytes. CdRead always delivers whole sectors, so
         * reading directly would scribble up to 2047 bytes past the buffer into
         * the next heap block (on PSX the async sector-copy callbacks only write
         * the file's bytes). Bounce through a sector-rounded scratch buffer and
         * copy back just the file's bytes. */
        int nAlignedBytes = nSectors * CD_SECTOR_SIZE;
        u8* pBounce = (u8*)malloc(nAlignedBytes);
        if (pBounce == NULL) {
            return -1;
        }
        CdRead(nSectors, (u_long*)pBounce, 0);
        CdReadSync(0, NULL);
        memcpy(pDestBuffer, pBounce, g_ArchiveCurFileSize);
        free(pBounce);
    }

    /* Mark the transfer complete so ArchiveDataSync()/ArchiveCdDataSync() return
     * immediately (g_ArchiveCdDriveState IDLE, no in-flight file). */
    g_ArchiveCurFileSize = 0;
    D_8004FDFC = 0;
    g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_IDLE;
    return 0;
}

void ArchiveCdSetMode(u_char mode) {
    int i;

    g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_RESET_MODE;
    for (i = 0; i < 4; i += 1) {
        D_80059F18[i] = 0;
    }

    D_80059F18[0] = mode;
    CdControlF(CdlSetmode, D_80059F18);
    CdControlF(CdlPause, NULL);
    g_ArchiveCdDriveState = ARCHIVE_CD_DRIVE_IDLE;
}
