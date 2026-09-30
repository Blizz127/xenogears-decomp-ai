/* Extra test link wrapper, compatible with existing footprint probe wrappers.
 * Link with --wrap=PcPort_Hd2dSpriteSubmitPart. Never changes production files. */
#include "hd2d_sprite_parts.h"
#include "hd2d_integration.h"
#include "hd2d_head_crop_variants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern unsigned g_PcPortPresentedFrames;
static FILE *variant_log;
static const void *registered_head_packet;
static unsigned registered_variant;
/* Call from the existing footprint TextureOverride wrapper AFTER its real
 * result, preserving both wrappers' ownership and avoiding duplicate symbols. */
void PcPort_Hd2dHeadCropGpuObserved(const void *packet,int accepted,unsigned texture)
{
    if (!PcPort_Hd2dEnabled()) return;
    if (packet!=registered_head_packet || !accepted || !texture || !variant_log) return;
    fprintf(variant_log,"present=%u variant=%u gpu_consumed=1 texture=%u\n",
        g_PcPortPresentedFrames+1,registered_variant,texture);
    fflush(variant_log);
    registered_head_packet=NULL;
}
extern int __real_PcPort_Hd2dSpriteSubmitPart(const PcPortHd2dPartProfile*,
    const PcPortHd2dPartMapping*,const PcPortHd2dRetailQuad*);
int __wrap_PcPort_Hd2dSpriteSubmitPart(const PcPortHd2dPartProfile *p,
    const PcPortHd2dPartMapping *m,const PcPortHd2dRetailQuad *q)
{
    static int requested=-1;
    PcPortHd2dPartMapping tuned;
    unsigned index, next_present;
    const Hd2dHeadCropVariant *variant;
    int result;
    if (!PcPort_Hd2dEnabled()) return 0;
    if (requested<0) {
        const char *value=getenv("XENO_HD2D_HEAD_CROP_VARIANTS");
        requested=value && !strcmp(value,"1");
    }
    if (!requested || !p || !m || !q || m->identity!=0xdb7d0105u ||
        m->map!=14 || m->actor!=1 || m->frame!=190 || m->part!=5 ||
        m->tpage!=26 || m->clut!=14353 || m->x!=100 || m->y!=37 ||
        m->width!=78 || m->height!=120 || p->width!=1254 || p->height!=1254)
        return __real_PcPort_Hd2dSpriteSubmitPart(p,m,q);
    next_present=g_PcPortPresentedFrames+1;
    index=Hd2dHeadCropVariantIndex(next_present);
    variant=hd2d_head_crop_variants+index;
    tuned=*m;
    tuned.x=variant->x;tuned.y=variant->y;
    tuned.width=variant->width;tuned.height=variant->height;
    result=__real_PcPort_Hd2dSpriteSubmitPart(p,&tuned,q);
    if (!variant_log) {
        const char *path=getenv("XENO_HD2D_HEAD_CROP_VARIANT_LOG");
        if (path) variant_log=fopen(path,"w");
    }
    if (result) { registered_head_packet=q->poly; registered_variant=index; }
    if (variant_log) {
        fprintf(variant_log,"present=%u variant=%u name=%s rect=%u,%u,%u,%u registered=%d\n",
            next_present,index,variant->name,variant->x,variant->y,variant->width,variant->height,result);
        fflush(variant_log);
    }
    return result;
}
