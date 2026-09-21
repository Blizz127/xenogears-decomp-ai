#include "common.h"
#include "psyq/libdma.h"
#include "psyq/libcd.h"
#include "psyq/stdio.h"

/**
 * Internal types
 */
typedef struct {
    unsigned char sync;
    unsigned char ready;
    unsigned char c;
} Intr_t;

typedef struct {
    int unk0;
    int unk4;
} Result_t;

typedef struct {
    int unk0;
    int unk4;
    char* unk8;
} Alarm_t;

extern u32 CD_nopen;

/**
 * Callbacks
 */
extern CdlCB CD_cbsync;
extern CdlCB CD_cbready;

/**
 * State
 */
extern int CD_status;
extern int CD_status1;
extern unsigned char CD_mode;
extern unsigned char CD_com;
extern CdlLOC CD_pos;
extern int CD_debug;
extern volatile Intr_t Intr;
extern volatile Alarm_t Alarm;

extern int D_80056570[];
extern int ComAttr[];

extern Result_t Result[3];

/**
 * CD Drive + DMA Registers
 */
extern volatile u8* reg0; // CD Index/Status.
extern volatile u8* reg1;
extern volatile u8* reg2;
extern volatile u8* reg3; // CD Interrupt Enable.
extern volatile u32* com_delay;
extern volatile u32* cdrom_delay;
extern volatile u32* d_pcr;
extern volatile u32* d3_madr;
extern volatile u32* d3_bcr;
extern volatile u32* d3_chcr;
extern volatile u16* _spu;

/**
 * Strings
 */
extern int CD_comstr[];
//  char* CD_comstr[32] = {"CdlSync",    "CdlNop",
//      "CdlSync",    "CdlNop",
//      "CdlSetloc",  "CdlPlay",
//      "CdlForward", "CdlBackward",
//      "CdlReadN",   "CdlStandby",
//      "CdlStop",    "CdlPause",
//      "CdlReset",   "CdlMute",
//      "CdlDemute",  "CdlSetfilter",
//      "CdlSetmode", "?",
//      "CdlGetlocL", "CdlGetlocP",
//      "?",          "CdlGetTN",
//      "CdlGetTD",   "CdlSeekL",
//      "CdlSeekP",   "?",
//      "?",          "?",
//      "?",          "CdlReadS",
//      "?",          "?",
//      "?",          "?",
//  };

extern char* CD_intstr[];
//  char* CD_intstr[8] = {
//      "NoIntr",  "DataReady", "Complete", "Acknowledge",
//      "DataEnd", "DiskError", "?",        "?",
//  };

extern char* CD_str0;    // "CD timeout: "
extern char* CD_str1;    // "%s:(%s) Sync=%s, Ready=%s\n"
extern char* CD_str2;    // "%s...\n"
extern char* CD_str3;    // "%s: no param\n"
extern char CD_str4;     // "CD_cw"
extern char* D_80018F3C; // "<NULL>"
extern char D_80018F18;
extern char D_80018F24;
extern char D_80018F30;
extern char D_80018EB0;
extern char D_80018EB8;
extern char D_80018E54; // "DiskError: "
extern char D_80018E60; // "com=&s,code=(%02x:%02x)\n"
extern char D_80018E7C; // "CDROM unknown intr"
extern char D_80018E90; // "(%d)\n"

/**
 * Unknown
 */
extern int D_8005678C;

// TODO: Get defs from .h file instead of extern.
extern int Vsync(int mode);
extern int CheckCallback();

/**
 * Fn Foward Decls
 */
int getintr();

static void rescpy(void* _dst, void* _src, u32 _size) {
    char* pDst = (char*)_dst;
    char* pSrc = (char*)_src;

    if (pDst == 0)
        return;

    while (_size--) {
        *pDst++ = *pSrc++;
    }
}

/* Keep static: inlined into CD_sync/CD_ready/CD_cw; the &callback uses in
 * CD_initintr/CD_init link to identical bytes via section-relative relocs. */
static void callback() {
    u8 masked;
    u32 status;

    masked = *reg0 & 0x3;
    while ((status = getintr())) {
        if ((status & 0x4) != 0 && CD_cbready) {
            CD_cbready(Intr.ready, (void*)&Result[1]);
        }
        if ((status & 0x2) != 0 && CD_cbsync) {
            CD_cbsync(Intr.sync, (void*)&Result);
        }
    }
    *reg0 = masked;
}

static void set_alarm(char* name) {
    ((Alarm_t*)&Alarm)->unk0 = Vsync(-1) + 0x3C0;
    ((Alarm_t*)&Alarm)->unk4 = 0;
    ((Alarm_t*)&Alarm)->unk8 = name;
}

static int get_alarm() {
    if (((Alarm_t*)&Alarm)->unk0 < Vsync(-1) ||
        ((Alarm_t*)&Alarm)->unk4++ > 0x3C0000) {
        puts((char*)&CD_str0);
        printf((char*)&CD_str1, ((Alarm_t*)&Alarm)->unk8, CD_comstr[CD_com],
               CD_intstr[Intr.sync], CD_intstr[Intr.ready]);
        CD_flush();
        return -1;
    } else {
        return 0;
    }
}

int getintr(void) {
    volatile u8 nReg;
    u8 bHasError;
    s32 i;
    s32 j;

    /*
    After a primitive command is executed, an 8-byte value is always returned.
    g_CdReadyResult and g_CdSyncResult stores the global return buffers for Ready and Sync.
    */
    volatile char buffer[8];

    // CDROM REG0: Set bank to 1
    *reg0 = 1;

    // CDROM REG3 (HINTSTS): Mask out HC05 interrupt type
    nReg = *reg3 & 0x7;

    if (nReg == 0) {
        return 0;
    }

    bHasError = 0;

    while (nReg != (*reg3 & 7)) {
        nReg = *reg3 & 0x7;
    }

    for (i = 0; i < 8; i++) {
        if ((*reg0 & 0x20) == 0) {
            break;
        }
        buffer[i] = *reg1;
    }

    for (j = i; j < 8; j++) {
        buffer[j] = 0;
    }
    // REG0: Set bank to 1
    *reg0 = 1;

    // REG3 (HCLRCTL): Acknowledge and clear HC05 interrupts
    *reg3 = 0x7;

    // REG2 (HINTMSK): Enable all interrupt types on HC05
    *reg2 = 0x7;

    // Anything but ACK
    if ((nReg != 3) || (ComAttr[CD_com + 0x20] != 0)) {
        if ((CD_status & 0x10) == 0 && (buffer[0] & 0x10) != 0) {
            CD_nopen++;
        }
        
        CD_status = buffer[0];
        CD_status1 = buffer[1];

        /*
        Usually the first return byte is the status byte, depending on the primitive command.
        
        Status bit flags:
        CdlStatPlay 0x80 1: CD-DA playing
        CdlStatSeek 0x40 1: seeking
        CdlStatRead 0x20 1: reading data sector
        CdlStatShellOpen 0x10 1: shell open*
        CdlStatSeekError 0x04 1: error during seeking/reading
        CdlStatStandby 0x02 1: motor rotating
        CdlStatError 0x01 1: command issue error
        */
        bHasError = CD_status;
        bHasError &= (0x1 | 0x4 | 0x8 | 0x10);
    }

    // DiskError - Log errors if debug level is appropriate
    if ((nReg == 5) && (0 < CD_debug)) {
        printf(&D_80018E54);
        if (CD_debug > 0)
            printf(&D_80018E60, CD_comstr[CD_com], CD_status, CD_status1);
    }

    switch (nReg) {
        // Command ACK
        case 3:
            if (bHasError) {
                Intr.sync = 5;
                rescpy(&Result[0], buffer, sizeof(Result_t));
                return 2;
            }
            
            if (D_80056570[CD_com] != 0) {
                Intr.sync = 3;
                rescpy(&Result[0], buffer, sizeof(Result_t));
                return 1;
            }

            Intr.sync = 2;
            rescpy(&Result[0], buffer, sizeof(Result_t));
            return 2;
        // Command Complete
        case 2:
            Intr.sync = bHasError ? 5 : 2;
            rescpy(&Result[0], buffer, sizeof(Result_t));
            return 2;

        // DataReady
        case 1:
            if (bHasError && i == 1) {
                bHasError = 0;
            }

            Intr.ready = bHasError ? 5 : 1;
            rescpy(&Result[1], buffer, sizeof(Result_t));

            // Set bank to 1
            *reg0 = 0;

            // HCHPCTL
            *reg3 = 0;
            return 4;

        // DataEnd
        case 4:
            Intr.ready = Intr.c = 4;
            rescpy(&Result[2], buffer, sizeof(Result_t));
            rescpy(&Result[1], buffer, sizeof(Result_t));
            return 4;
        
        // DiskError
        case 5:
            Intr.sync = Intr.ready = 5;
            rescpy(&Result[0], buffer, sizeof(Result_t));
            rescpy(&Result[1], buffer, sizeof(Result_t));
            return 6;

        default:
            puts(&D_80018E7C);
            printf(&D_80018E90, nReg);
            return 0;
    }
}

int CD_sync(int arg0, char* arg1) {
    int status;
    set_alarm(&D_80018EB0);

    while (true) {
        if (get_alarm() != 0) {
            return -1;
        }

        if (CheckCallback() != 0) {
            callback();
        }

        status = Intr.sync;

        if (status == 2 || status == 5) {
            Intr.sync = 2;
            rescpy(arg1, &Result, sizeof(Result[0]));
            return status;
        }

        if (arg0 != 0) {
            break;
        }
    }

    return 0;
}

int CD_ready(int arg0, char* arg1) {
    int status;
    set_alarm(/*name=*/&D_80018EB8);

    while (true) {
        if (get_alarm() != 0) {
            return -1;
        }

        if (CheckCallback() != 0) {
            callback();
        }

        status = Intr.c;

        if (status != 0) {
            Intr.c = 0;
            rescpy(arg1, &Result[2], sizeof(Result[0]));
            return status;
        }

        status = Intr.ready;

        if (status != 0) {
            Intr.ready = 0;
            rescpy(arg1, &Result[1], sizeof(Result[0]));
            return status;
        }

        if (arg0 != 0) {
            break;
        }
    }

    return 0;
}

int CD_cw(u8 arg0, u8* arg1, u8* arg2, int arg3) {
    int i;

    if (!(CD_debug < 2)) {
        printf((char*)&CD_str2, CD_comstr[arg0]);
    }

    if ((ComAttr[0x40 + arg0] != 0) && (arg1 == 0)) {
        if (CD_debug > 0) {
            printf((char*)&CD_str3, CD_comstr[arg0]);
        }
        return -2;
    }

    CD_sync(0, 0);

    if (arg0 == CdlSetloc) {
        for (i = 0; i < 0x4; ++i) {
            *(u8*)((u32)&CD_pos + i) = arg1[i];
        }
    }

    if (arg0 == CdlSetmode) {
        CD_mode = *arg1;
    }

    Intr.sync = 0;
    if (ComAttr[arg0] != 0) {
        Intr.ready = 0;
    }

    *reg0 = 0;
    for (i = 0; i < ComAttr[0x40 + arg0]; ++i) {
        *reg2 = arg1[i];
    }
    CD_com = arg0;
    *reg1 = CD_com;

    if (arg3 != 0)
        return 0;

    set_alarm(/*name=*/&CD_str4);

    while (Intr.sync == 0) {
        if (get_alarm() != 0) {
            return -1;
        }

        if (CheckCallback() != 0) {
            callback();
        }
    }

    rescpy(arg2, &Result, sizeof(Result[0]));
    return -!(Intr.sync ^ 0x5);
}

int CD_vol(CdlATV* vol) {
    *reg0 = 2;
    *reg2 = vol->val0;
    *reg3 = vol->val1;
    *reg0 = 3;
    *reg1 = vol->val2;
    *reg2 = vol->val3;
    *reg3 = 0x20;
    return 0;
}

int CD_flush() {
    *reg0 = 1;
    while (*reg3 & 7) {
        *reg0 = 1;
        *reg3 = 7;
        *reg2 = 7;
    }

    Intr.ready = Intr.c = 0;
    Intr.sync = 2;

    *reg0 = 0;
    *reg3 = 0;
    *com_delay = 0x1325;
}

int CD_initvol(void) {
    CdlATV vol;
    // 0x1F801c00 + 0x1b8 = 0x1F801db8 (current main vol left/right).
    if (_spu[0xDC] == 0 && _spu[0xDD] == 0) {
        // 0x1F801c00 + 0x180 = 0x1F801d80 (main vol left/right).
        _spu[0xC0] = 0x3FFF;
        _spu[0xC1] = 0x3FFF;
    }
    // 0x1F801c00 + 0x1b0 = 0x1F801db0 (CD volume left/right).
    _spu[0xD8] = 0x3FFF;
    _spu[0xD9] = 0x3FFF;
    // Enable spu, unmute spu and enable cd audio.
    _spu[0xD5] = (1 << 15) | (1 << 14) | (1 << 0);

    vol.val0 = vol.val2 = 0x80;
    vol.val1 = vol.val3 = 0;

    *reg0 = 2;
    *reg2 = vol.val0;
    *reg3 = vol.val1;
    *reg0 = 3;
    *reg1 = vol.val2;
    *reg2 = vol.val3;
    *reg3 = 0x20;
    return 0;
}

int CD_initintr() {
    CD_cbready = 0;
    CD_cbsync = 0;
    CD_status1 = 0;
    CD_status = 0;
    ResetCallback();
    InterruptCallback(2, &callback);
}

int CD_init(void) {
    puts(&D_80018F18);
    printf(&D_80018F24, &D_8005678C);
    CD_com = 0;
    CD_mode = 0;
    CD_initintr();
    CD_flush();
    CD_cw(CdlNop, 0, 0, 0);

    if (CD_status & CdlStatShellOpen) {
        CD_cw(CdlNop, 0, 0, 0);
    }

    // NOTE: 0xA is the init command.
    if (CD_cw(0xA, 0, 0, 0) != 0) {
        return -1;
    }

    if (CD_cw(CdlDemute, 0, 0, 0) != 0) {
        return -1;
    }

    if (CD_sync(0, 0) != 2) {
        return -1;
    }

    return 0;
}

int CD_datasync(int arg0) {
    set_alarm(/*name=*/&D_80018F30);
    while (true) {
        if (get_alarm() != 0) {
            return -1;
        }
        if (*d3_chcr & 0x1000000) {
            if (arg0 != 0) {
                return 1;
            }
        } else {
            return 0;
        }
    }
    return 0;
}

int CD_getsector(void* madr, size_t size) {
    *reg0 = 0;
    *reg3 = 0x80;
    *cdrom_delay = 0x20943;
    *com_delay = 0x1323;
    // DMA3 Master Enable.
    *d_pcr |= 0x8000;
    *d3_madr = (u32)madr;
    *d3_bcr = size | 0x10000;
    while (!(*reg0 & 0x40))
        ;
    // Start DMA transfer.
    *d3_chcr = DMA_CHCR_CDROM_NORMAL;
    // Wait until transfer is complete.
    while (*d3_chcr & DMA_CHCR_START_BUSY_ENABLED)
        ;
    *com_delay = 0x1325;
    return 0;
}

int CD_getsector2(void* madr, size_t size) {
    volatile u32 sp0;
    *reg0 = 0;
    *reg3 = 0x80;
    *cdrom_delay = 0x21020843;
    *com_delay = 0x1325;
    *d_pcr |= 0x8000;
    *d3_madr = (u32)madr;
    *d3_bcr = size | 0x10000;
    while (!(*reg0 & 0x40))
        ;
    // Start DMA transfer w/ "chopping" (cycle stealing by CPU).
    *d3_chcr = DMA_CHCR_CDROM_CHOPPED;
    sp0 = *d3_chcr;
    return 0;
}

void CD_set_test_parmnum(int arg0) {
    ComAttr[0x59] = arg0;
}
