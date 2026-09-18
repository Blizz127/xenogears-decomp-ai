#include "common.h"
#include "libspu_internal.h"
#include "psyq/stdarg.h"
#include "psyq/libdma.h"
#include "psyq/libmctrl.h"

/* Timeout report strings. TU-owned bytes (yaml [0x9C6C, .rodata,
 * psyq/libspu/Spu]) kept in retail order; the D_ names match the disassembly
 * labels so relocs read the same. Retail addresses them absolutely
 * (lui+addiu), so each use pins the arg regs and materializes the address
 * explicitly (same idiom as system/memory.c's HEAP_FMT_PTR). */
const char D_8001946C[] = "SPU:T/O [%s]\n";
const char D_8001947C[] = "wait (reset)";
const char D_8001948C[] = "wait (wrdy H -> L)";
const char D_800194A0[] = "wait (dmaf clear/W)";

#ifdef XENO_PC_PORT
#define SPU_STR_REG(name) const char* name
#define SPU_STR_REG1(name) const char* name
#define SPU_STR_PTR(dst, sym) ((dst) = (sym))
#else
#define SPU_STR_REG(name) register const char* name asm("$4")
#define SPU_STR_REG1(name) register const char* name asm("$5")
#define SPU_STR_PTR(dst, sym) __asm__ volatile( \
    "lui %0,%%hi(" #sym ")\n\taddiu %0,%0,%%lo(" #sym ")" : "=r" (dst))
#endif



long _spu_init(long bHot) {
    u32 dmaTimer;
    int i;

    *_spu_sys_pcr |= DMA_DPCR_SPU_PRIORITY_HIGH | DMA_DPCR_MASK_DMA4_ENABLE;
    _spu_RXX->rxx.main_vol.left = 0;
    _spu_RXX->rxx.main_vol.right = 0;
    _spu_RXX->rxx.spucnt = 0;
    _spu_transMode = 0;
    _spu_addrMode = 0;
    _spu_tsa = 0;
    _spu_Fw1ts();
    _spu_RXX->rxx.main_vol.left = 0;
    _spu_RXX->rxx.main_vol.right = 0;

    dmaTimer = 0;
    while (_spu_RXX->rxx.spustat & 0x7FF)
    {
        if (++dmaTimer > DMA_TIMEOUT)
        {
            {
                SPU_STR_REG(f);
                SPU_STR_REG1(s);
                SPU_STR_PTR(f, D_8001946C);
                SPU_STR_PTR(s, D_8001947C);
                printf(f, s);
            }
            break;
        }
    }

	// TODO(jperos): These are definitely macros
    _spu_mem_mode = 2;
    _spu_mem_mode_plus = 3;
    _spu_mem_mode_unit = 8;
    _spu_mem_mode_unitM = 7;
    _spu_RXX->rxx.data_trans = 4;

    _spu_RXX->rxx.rev_vol.left = 0;
    _spu_RXX->rxx.rev_vol.right = 0;
    _spu_RXX->rxx.key_off[0] = 0xFFFF;
    _spu_RXX->rxx.key_off[1] = 0xFFFF;
    _spu_RXX->rxx.rev_mode[0] = 0;
    _spu_RXX->rxx.rev_mode[1] = 0;

    for (i = 0; i < 10; i++)
    {
        _spu_RQ[i] = 0;
    }

    if (!bHot)
    {
        _spu_RXX->rxx.chan_fm[0] = 0;
        _spu_RXX->rxx.chan_fm[1] = 0;
        _spu_RXX->rxx.noise_mode[0] = 0;
        _spu_RXX->rxx.noise_mode[1] = 0;
        _spu_RXX->rxx.cd_vol.left = 0;
        _spu_RXX->rxx.cd_vol.right = 0;
        _spu_RXX->rxx.ex_vol.left = 0;
        _spu_RXX->rxx.ex_vol.right = 0;
        _spu_tsa = _SPU_DEFAULT_ADDR;

        _spu_FwriteByIO(&_spu_dummy, ARRAY_COUNT(_spu_dummy));

        for(i = 0; i < NUM_VOICES; i++)
        {
            _spu_RXX->rxx.voice[i].volume.left = 0; /* left volume */
            _spu_RXX->rxx.voice[i].volume.right = 0; /* right volume */
            _spu_RXX->rxx.voice[i].pitch = 0x3fff;  /* pitch */
            _spu_RXX->rxx.voice[i].addr = _SPU_DEFAULT_ADDR;  /* addr */
            _spu_RXX->rxx.voice[i].adsr[0] = 0; /* adsr1 */
            _spu_RXX->rxx.voice[i].adsr[1] = 0; /* adsr2 */
        }

        _spu_RXX->rxx.key_on[0] = 0xFFFF;
        _spu_RXX->rxx.key_on[1] = 0xFF;
        _spu_Fw1ts();
        _spu_Fw1ts();
        _spu_Fw1ts();
        _spu_Fw1ts();
        _spu_RXX->rxx.key_off[0] = 0xFFFF;
        _spu_RXX->rxx.key_off[1] = 0xFF;
        _spu_Fw1ts();
        _spu_Fw1ts();
        _spu_Fw1ts();
        _spu_Fw1ts();
    }

    _spu_inTransfer = 1;
    _spu_RXX->rxx.spucnt = SPU_CTRL_MASK_MUTE_SPU | SPU_CTRL_MASK_SPU_ENABLE;
    _spu_transferCallback = NULL;
    _spu_IRQCallback = NULL;
    return 0;
}

void _spu_FwriteByIO(void* data, u32 size) {
    u16 initStatus;
    s32 dmaTimer;
    s32 blockSize;
    u16 spucnt;
    u16* data16 = data;

	// TODO(jperos): 0x7FF is the 11 lowest bits. Should macro this in a way that makes sense for whatever this represents
    initStatus = _spu_RXX->rxx.spustat & 0x7FF;
    _spu_RXX->rxx.trans_addr = _spu_tsa;
    _spu_Fw1ts();

    while (size != 0)
    {
		// TODO(jperos): I've seen 0x40 in a few other transfers - is this max chunk size?
        if (size <= 0x40)
        {
            blockSize = size;
        }
        else
        {
            blockSize = 0x40;
        }

        dmaTimer = 0;
        while (dmaTimer < blockSize)
        {
            _spu_RXX->rxx.trans_fifo = *data16++;
             dmaTimer += 2;
        }

        spucnt = _spu_RXX->rxx.spucnt;
        spucnt &= ~SPU_CTRL_TRANSFER_MODE_DMA_READ;
        spucnt |= SPU_CTRL_TRANSFER_MODE_MANUAL_WRITE;
        _spu_RXX->rxx.spucnt = spucnt;
        _spu_Fw1ts();

        dmaTimer = 0;
        while (_spu_RXX->rxx.spustat & SPU_STAT_MASK_DATA_TRANSFER_BUSY)
        {
            if (++dmaTimer > DMA_TIMEOUT)
            {
                {
                    SPU_STR_REG(f);
                    SPU_STR_REG1(s);
                    SPU_STR_PTR(f, D_8001946C);
                    SPU_STR_PTR(s, D_8001948C);
                    printf(f, s);
                }
                break;
            }
        }

        _spu_Fw1ts();
        _spu_Fw1ts();

        size -= blockSize;
    }

    spucnt = _spu_RXX->rxx.spucnt;
    spucnt &= ~SPU_CTRL_TRANSFER_MODE_DMA_READ;
    spucnt |= SPU_CTRL_TRANSFER_MODE_STOP;
    _spu_RXX->rxx.spucnt = spucnt;

    dmaTimer = 0;
    while ((_spu_RXX->rxx.spustat & 0x7FF) != initStatus)
    {
        if (++dmaTimer > DMA_TIMEOUT)
        {
            {
                SPU_STR_REG(f);
                SPU_STR_REG1(s);
                SPU_STR_PTR(f, D_8001946C);
                SPU_STR_PTR(s, D_800194A0);
                printf(f, s);
            }
            break;
        }
    }
}

void _spu_FiDMA(void) {
    s32 i;

    if (_spu_dma_mode == SPU_DMA_MODE_WRITE)
    {
        _spu_Fw1ts();
    }

    _spu_RXX->rxx.spucnt &= ~SPU_CTRL_TRANSFER_MODE_DMA_READ;

    for(i = 0; _spu_RXX->rxx.spucnt & SPU_CTRL_TRANSFER_MODE_DMA_READ; )
    {
        if (++i > DMA_TIMEOUT) {
            break;
        }
    }

    if (_spu_transferCallback)
    {
        _spu_transferCallback();
    }
    else
    {
        DeliverEvent(0xF0000009, 0x20);
    }
}

void _spu_Fr_(s32 madr, u16 trans_addr, s32 bcr) {
    _spu_RXX->rxx.trans_addr = trans_addr;
    _spu_Fw1ts();
    _spu_RXX->rxx.spucnt |= SPU_CTRL_TRANSFER_MODE_DMA_READ;
    _spu_Fw1ts();
    _spu_FsetDelayR();
    *_spu_madr = madr;
    *_spu_bcr = (bcr << 16) | 16;
    _spu_dma_mode = SPU_DMA_MODE_READ;
    *_spu_chcr = DMA_CHCR_SPU_READ;
}


s32 _spu_t(s32 operation, ...) {
    u16 spuTransferMode;
    s32 chcr;
    u32 dmaTimer;
    u32 addr;
    va_list args;
    u32 arg;
    u16 controlRegister;

    va_start(args, operation);

    switch (operation) {
        case SPU_DMA_CMD_SETADDR:
            arg = va_arg(args, u32);
            _spu_RXX->rxx.trans_addr = _spu_tsa = arg >> _spu_mem_mode_plus;
            break;
        case SPU_DMA_CMD_WRITE:
            _spu_dma_mode = SPU_DMA_MODE_WRITE;

            dmaTimer = 0;
            while (_spu_RXX->rxx.trans_addr != _spu_tsa) {
                if (++dmaTimer > DMA_TIMEOUT) {
                    return DMA_TRANSFER_TIMEOUT;
                }
            }

            // _spu_RXX->rxx.spucnt = (_spu_RXX->rxx.spucnt & 0xFFCF) | 0x20;
            controlRegister = _spu_RXX->rxx.spucnt;
            controlRegister &= ~SPU_CTRL_TRANSFER_MODE_DMA_READ;
            controlRegister |= SPU_CTRL_TRANSFER_MODE_DMA_WRITE;
            _spu_RXX->rxx.spucnt = controlRegister;
            break;
        case SPU_DMA_CMD_READ:
            _spu_dma_mode = SPU_DMA_MODE_READ;

            dmaTimer = 0;
            while (_spu_RXX->rxx.trans_addr != _spu_tsa) {
                if (++dmaTimer > DMA_TIMEOUT) {
                    return DMA_TRANSFER_TIMEOUT;
                }
            }

            // _spu_RXX->rxx.spucnt |= 0x30;
            controlRegister = _spu_RXX->rxx.spucnt;
            controlRegister &= ~SPU_CTRL_TRANSFER_MODE_DMA_READ;
            controlRegister |= SPU_CTRL_TRANSFER_MODE_DMA_READ;
            _spu_RXX->rxx.spucnt = controlRegister;
            break;
        case SPU_DMA_CMD_EXEC:
            if (_spu_dma_mode == SPU_DMA_MODE_READ) {
                spuTransferMode = SPU_CTRL_TRANSFER_MODE_DMA_READ;
            } else {
                spuTransferMode = SPU_CTRL_TRANSFER_MODE_DMA_WRITE;
            }

            dmaTimer = 0;
            while ((_spu_RXX->rxx.spucnt & SPU_CTRL_TRANSFER_MODE_DMA_READ) != spuTransferMode) {
                if (++dmaTimer > DMA_TIMEOUT) {
                    return DMA_TRANSFER_TIMEOUT;
                }
            }

            if (_spu_dma_mode == SPU_DMA_MODE_READ) {
                _spu_FsetDelayR();
            } else {
                _spu_FsetDelayW();
            }

            arg = va_arg(args, u32);
            _spu_transfer_startaddr = arg;
            arg = va_arg(args, u32);
            _spu_transfer_time = (arg / DMA_BLOCK_SIZE);
            _spu_transfer_time += ((arg % DMA_BLOCK_SIZE) ? 1 : 0);
            *_spu_madr = _spu_transfer_startaddr;
            *_spu_bcr = (_spu_transfer_time << 16) | 16;
            if (_spu_dma_mode == SPU_DMA_MODE_READ) {
                chcr = DMA_CHCR_SPU_READ;
            } else {
                chcr = DMA_CHCR_SPU_WRITE;
            }
            *_spu_chcr = chcr;
            break;
    }

    return DMA_TRANSFER_SUCCESS;
}

s32 _spu_Fw(s32 addr, s32 size) {
    if (_spu_transMode == SPU_TRANSFER_BY_DMA) {
        _spu_t(SPU_DMA_CMD_SETADDR, _spu_tsa << _spu_mem_mode_plus);
        _spu_t(SPU_DMA_CMD_WRITE);
        _spu_t(SPU_DMA_CMD_EXEC, addr, size);
    }
    else
    {
        _spu_FwriteByIO(addr, size);
    }
    return size;
}

s32 _spu_Fr(s32 addr, s32 size) {
    _spu_t(SPU_DMA_CMD_SETADDR, _spu_tsa << _spu_mem_mode_plus);
    _spu_t(SPU_DMA_CMD_READ);
    _spu_t(SPU_DMA_CMD_EXEC, addr, size);
    return size;
}

void _spu_FsetRXX(u32 offset, u32 value, u32 mode)
{
    if (mode == 0) {
        volatile u16* pRegs = (volatile u16*)_spu_RXX;
        pRegs[offset] = value;
    }
    else {
        volatile u16* pRegs = (volatile u16*)_spu_RXX;
        pRegs[offset] = value >> _spu_mem_mode_plus;
    }
}

// Set Register Address (value)
// Note: Addresses are stored in SPU RAM as 16 bit values, as 1/8th ( >> 3 ) versions of the actual memory address
u32 _spu_FsetRXXa(s32 offset, u32 value) {
    u32 translated_address;

    // Align the address value to 8 bits
    if ((_spu_mem_mode != 0) && ((value % _spu_mem_mode_unit) != 0)) {
        value += _spu_mem_mode_unit;
        value &= ~_spu_mem_mode_unitM;
    }

    // Shift it to translate it to the actual SPU-ranged value
    translated_address = value >> _spu_mem_mode_plus;
    switch (offset) {
        case SPU_ADDRESS_MODE_SPU:
            return translated_address & 0xFFFF;
        case SPU_ADDRESS_MODE_ALIGNED:
            return value;
        default:
            _spu_RXX->raw[offset] = translated_address;
    }

    return value;
}

// Get Register Address (value)
// Note: Addresses are stored in SPU RAM as 16 bit values, as 1/8th ( >> 3 ) versions of the actual memory address
s32 _spu_FgetRXXa(s32 offset, s32 addressing_mode) {
    u16 value;

    value = _spu_RXX->raw[offset];
    switch (addressing_mode) {
        case SPU_ADDRESS_MODE_SPU:
            return value;
        default:
            return value << _spu_mem_mode_plus;
    }
}

void _spu_FsetPCR(s32 bKindOfHighPriority) {
    *_spu_sys_pcr &= ~DMA_DPCR_MASK_DMA4_PRIORITY;
    if (bKindOfHighPriority) {
        *_spu_sys_pcr |= (DMA_PRIORITY_HIGH << DMA_DPCR_DMA4_PRIORITY_SHIFT);
    } else {
        *_spu_sys_pcr |= (DMA_PRIORITY_MEDIUM << DMA_DPCR_DMA4_PRIORITY_SHIFT);
    }
}

// NOTE(jperos): I'm currently unaware of exactly what these DMA timing override codes mean exactly.
//               From: https://psx-spx.consoledev.net/memorycontrol/ :
//                   1F801014h - SPU Delay/Size (200931E1h) (use 220931E1h for SPU-RAM reads)
void _spu_FsetDelayW(void) {
    *_spu_delay = (*_spu_delay & ~MCTRL_DELAY_DMA_TIMING_OVERRIDE_MASK) |
                  MCTRL_DELAY_DMA_TIMING_SELECT;
}

void _spu_FsetDelayR(void) {
    *_spu_delay = (*_spu_delay & ~MCTRL_DELAY_DMA_TIMING_OVERRIDE_MASK) |
                  ((2 << MCTRL_DELAY_DMA_TIMING_OVERRIDE_SHIFT) | MCTRL_DELAY_DMA_TIMING_SELECT);
}

void _spu_Fw1ts(void) {
    volatile s32 counter;
    volatile s32 value;

    value = 13;
    counter = 0;

    while (counter < 60) {
        value = value * 13;
        counter++;
    }
}
