#include "common.h"

#include "libspu_internal.h"

void _spu_FiDMA(void);

void _SpuInit(s32 bHot) {
    s32 i;
    u_short v;

    ResetCallback();
    _spu_init(bHot);

    if (bHot == 0)
    {
        v = 0xC000;
        for(i = NUM_VOICES - 1; i >= 0; i--)
        {
            _spu_voice_centerNote[i] = v;
        }
    }


    SpuStart();
    _spu_rev_flag = 0;
    _spu_rev_reserve_wa = 0;
    _spu_rev_attr.mode = 0;
    _spu_rev_attr.depth.left = 0;
    _spu_rev_attr.depth.right = 0;
    _spu_rev_attr.delay = 0;
    _spu_rev_attr.feedback = 0;
    _spu_rev_offsetaddr = _spu_rev_startaddr[0];
    _spu_FsetRXX(SPU_RXX_REV_WA_START_ADDR, _spu_rev_offsetaddr, 0);
    _spu_AllocBlockNum = 0;
    _spu_AllocLastNum = 0;
    _spu_memList = 0;
    _spu_trans_mode = 0;
    _spu_transMode = 0;
    _spu_keystat = 0;
    _spu_RQmask = 0;
    _spu_RQvoice = 0;
    _spu_env = 0;
}

void SpuStart(void) {
    s32 event;

    if (g_SpuRunning == 0) {
        g_SpuRunning = 1;
        EnterCriticalSection();
        _SpuDataCallback(_spu_FiDMA);
        // TODO: who knows what these constants do
        event = OpenEvent(0xF0000009U, 0x20, 0x2000, NULL);
        _spu_EVdma = event;
        EnableEvent(event);
        ExitCriticalSection();
    }
}
