/* Packet identity transport: retail packets/UVs/colour/OT stay untouched. */
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include "common.h"
#include "psyq/libgpu.h"
#include "guest_prim_link.h"
#include "hd2d_sprite_parts.h"
#include "hd2d_integration.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PENDING_PARTS 16384
static struct PendingPart {
    const POLY_FT4 *packet;
    POLY_FT4 snapshot;
    const PcPortHd2dPartProfile *profile;
    PcPortHd2dPartMapping mapping;
} pending[MAX_PENDING_PARTS];
static size_t pending_count;
static GLuint atlas_texture;
static const PcPortHd2dPartProfile *uploaded_profile;

/* Called only after all queued GPU packets have been consumed. */
void PcPort_Hd2dSpritePresentDone(void) { pending_count = 0; }

static int PacketMatchesQuad(const POLY_FT4 *p, const PcPortHd2dRetailQuad *q)
{
    const short x[4] = {p->x0,p->x1,p->x2,p->x3}, y[4] = {p->y0,p->y1,p->y2,p->y3};
    const unsigned char u[4] = {p->u0,p->u1,p->u2,p->u3}, v[4] = {p->v0,p->v1,p->v2,p->v3};
    return (p->code & 0xfc) == 0x2c && p->tpage == q->tpage && p->clut == q->clut &&
        !memcmp(x,q->x,sizeof x) && !memcmp(y,q->y,sizeof y) &&
        !memcmp(u,q->u,sizeof u) && !memcmp(v,q->v,sizeof v);
}
static int LoadTexture(const PcPortHd2dPartProfile *p)
{
    FILE *file;
    unsigned char *pixels;
    size_t bytes;
    GLint binding, unpack_buffer, alignment, row_length, skip_rows, skip_pixels, width=0, height=0;
    GLuint texture=0;
    if (uploaded_profile == p && atlas_texture) return 1;
    /* Profiles are immutable; hot swapping requires an explicit renderer reset. */
    if (uploaded_profile || !PcPort_Hd2dPartProfileValidate(p)) return 0;
    bytes = (size_t)p->width * p->height * 4;
    file = fopen(p->rgba_path,"rb");
    if (!file) return 0;
    pixels = malloc(bytes);
    if (!pixels) { fclose(file); return 0; }
    if (fread(pixels,1,bytes,file) != bytes || fgetc(file) != EOF || ferror(file)) {
        free(pixels); fclose(file); return 0;
    }
    if (fclose(file)) { free(pixels); return 0; }
    glGetIntegerv(GL_TEXTURE_BINDING_2D,&binding);
    glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING,&unpack_buffer);
    glGetIntegerv(GL_UNPACK_ALIGNMENT,&alignment);
    glGetIntegerv(GL_UNPACK_ROW_LENGTH,&row_length);
    glGetIntegerv(GL_UNPACK_SKIP_ROWS,&skip_rows);
    glGetIntegerv(GL_UNPACK_SKIP_PIXELS,&skip_pixels);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH,0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS,0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS,0);
    glGenTextures(1,&texture);
    if (texture) {
        glBindTexture(GL_TEXTURE_2D,texture);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,p->width,p->height,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
        glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&height);
    }
    glBindTexture(GL_TEXTURE_2D,binding);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER,unpack_buffer);
    glPixelStorei(GL_UNPACK_ALIGNMENT,alignment);
    glPixelStorei(GL_UNPACK_ROW_LENGTH,row_length);
    glPixelStorei(GL_UNPACK_SKIP_ROWS,skip_rows);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS,skip_pixels);
    free(pixels);
    if (!texture || width != p->width || height != p->height) {
        if (texture) glDeleteTextures(1,&texture);
        return 0;
    }
    uploaded_profile=p; atlas_texture=texture;
    return 1;
}
int PcPort_Hd2dSpriteSubmitPart(const PcPortHd2dPartProfile *p,
    const PcPortHd2dPartMapping *m, const PcPortHd2dRetailQuad *q)
{
    size_t i;
    struct PendingPart *entry;
    if (!PcPort_Hd2dEnabled()) return 0;
    if (!PcPort_Hd2dOpeningActive() || !p || !m || !q || !q->poly || !q->ot ||
        pending_count == MAX_PENDING_PARTS || !PacketMatchesQuad(q->poly,q) || !LoadTexture(p)) return 0;
    for (i=0;i<pending_count;++i) if (pending[i].packet == q->poly) return 0;
    entry = pending + pending_count++;
    entry->packet = q->poly; entry->profile=p; entry->mapping=*m;
    PcPort_AddPrimDomainAware(q->ot,q->poly);
    /* Snapshot after linkage, including domain-aware packet tag. */
    memcpy(&entry->snapshot,q->poly,sizeof entry->snapshot);
    return 1;
}
int PcPort_Hd2dSpriteTextureOverride(const void *packet, int tpage, int clut,
    int u0, int v0, int u1, int v1, unsigned *texture, float uv[4])
{
    size_t i;
    if (!PcPort_Hd2dEnabled()) return 0;
    if (!PcPort_Hd2dOpeningActive() || !packet || !texture || !uv || !atlas_texture) return 0;
    for (i=0;i<pending_count;++i) {
        const struct PendingPart *e = pending+i;
        const PcPortHd2dPartMapping *m = &e->mapping;
        int j, min_u=255, min_v=255, max_u=0, max_v=0;
        if (e->packet != packet || memcmp(packet,&e->snapshot,sizeof e->snapshot) ||
            tpage != m->tpage || clut != m->clut || m->map != PcPort_Hd2dCurrentMap() || uploaded_profile != e->profile) continue;
        for (j=0;j<4;++j) {
            if (m->u[j]<min_u) min_u=m->u[j];
            if (m->u[j]>max_u) max_u=m->u[j];
            if (m->v[j]<min_v) min_v=m->v[j];
            if (m->v[j]>max_v) max_v=m->v[j];
        }
        if (u0 != min_u || v0 != min_v || u1 != max_u || v1 != max_v || u0 == u1 || v0 == v1) return 0;
        *texture=atlas_texture;
        uv[0]=(m->x+.5f)/e->profile->width;
        uv[1]=(m->y+.5f)/e->profile->height;
        uv[2]=(m->x+m->width-.5f)/e->profile->width;
        uv[3]=(m->y+m->height-.5f)/e->profile->height;
        return 1;
    }
    return 0;
}
