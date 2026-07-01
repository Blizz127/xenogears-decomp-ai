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

/* Forward-declare the two host-libc calls used for the partial-sector bounce.
 * Pulling in <stdlib.h> here fails to compile: the game headers (via common.h)
 * leave the TU without int32_t, which glibc's <stdlib.h> then references. memcpy
 * is already declared through common.h (psyq/memory.h). */
extern void* malloc(unsigned long size);
extern void  free(void* ptr);

/* Set by ArchiveReadFileToBuffer/ArchiveReadFileFromCdSector (libarchive.c) just
 * before they call us: the absolute CD sector and the byte length to read. */
extern s32 g_ArchiveCurFileSector;

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
