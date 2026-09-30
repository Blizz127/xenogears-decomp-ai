/* Mock GL verifies upload state restoration and packet identity without a GPU. */
#include "../src/hd2d/hd2d_sprite_transport.c"
#include <assert.h>
static int enabled, calls, links;
static void *linked_ot;
static GLint state[6]={17,19,8,11,13,15}, saved[6], tw, th;
static int Index(GLenum name) {
    switch(name) {
    case GL_TEXTURE_BINDING_2D:return 0; case GL_PIXEL_UNPACK_BUFFER_BINDING:return 1;
    case GL_UNPACK_ALIGNMENT:return 2; case GL_UNPACK_ROW_LENGTH:return 3;
    case GL_UNPACK_SKIP_ROWS:return 4; case GL_UNPACK_SKIP_PIXELS:return 5;
    default:assert(0);return 0;
    }
}
int PcPort_Hd2dEnabled(void) { return enabled; }
int PcPort_Hd2dOpeningActive(void) { assert(enabled); return 1; }
int PcPort_Hd2dCurrentMap(void) { return 7; }
void PcPort_AddPrimDomainAware(void *ot, void *prim) { links++; linked_ot=ot; ((P_TAG *)prim)->addr=42; }
void glGetIntegerv(GLenum n, GLint *v) { calls++; *v=state[Index(n)]; }
void glBindTexture(GLenum target, GLuint texture) { assert(target==GL_TEXTURE_2D); calls++; state[0]=texture; }
void glBindBuffer(GLenum target, GLuint buffer) { assert(target==GL_PIXEL_UNPACK_BUFFER); calls++; state[1]=buffer; }
void glPixelStorei(GLenum name, GLint value) { calls++; state[Index(name)]=value; }
void glGenTextures(GLsizei n, GLuint *v) { assert(n==1); calls++; *v=43; }
void glTexParameteri(GLenum target, GLenum name, GLint value) {
    assert(target==GL_TEXTURE_2D); calls++;
    if (name==GL_TEXTURE_MIN_FILTER || name==GL_TEXTURE_MAG_FILTER) assert(value==GL_NEAREST);
    else assert(value==GL_CLAMP_TO_EDGE);
}
void glTexImage2D(GLenum target, GLint level, GLint format, GLsizei w, GLsizei h,
    GLint border, GLenum input, GLenum type, const void *pixels) {
    assert(target==GL_TEXTURE_2D && !level && format==GL_RGBA8 && !border && input==GL_RGBA && type==GL_UNSIGNED_BYTE && pixels);
    assert(!state[1] && state[2]==1 && !state[3] && !state[4] && !state[5]); calls++; tw=w; th=h;
}
void glGetTexLevelParameteriv(GLenum target, GLint level, GLenum name, GLint *value) {
    assert(target==GL_TEXTURE_2D && !level); calls++; *value=name==GL_TEXTURE_WIDTH?tw:th;
}
void glDeleteTextures(GLsizei n, const GLuint *v) { (void)n;(void)v;calls++; }
int main(void)
{
    POLY_FT4 packet={0}, before, other;
    PcPortHd2dRetailQuad q={{-8,9,-8,9},{12,12,45,45},{1,2,1,2},{3,3,4,4},9,8,(void *)99,&packet};
    PcPortHd2dPartMapping m={123,7,0,2,0,9,8,{1,2,1,2},{3,3,4,4},0,0,16,32};
    PcPortHd2dPartProfile p={64,64,&m,1,"pc_port/assets/hd2d/test.rgba"};
    unsigned tex=0; float uv[4]={0}; unsigned char pixels[64*64*4]={0}; FILE *f;
    packet.code=0x2e; packet.r0=128;packet.g0=72;packet.b0=16;
    setXY4(&packet,-8,12,9,12,-8,45,9,45);setUV4(&packet,1,3,2,3,1,4,2,4);
    packet.tpage=9;packet.clut=8;before=packet;
    assert(!PcPort_Hd2dSpriteSubmitPart((void *)1,(void *)1,(void *)1));
    assert(!PcPort_Hd2dSpriteTextureOverride((void *)1,0,0,0,0,0,0,&tex,uv));
    assert(!calls && !links);
    f=fopen(p.rgba_path,"wb");assert(f);assert(fwrite(pixels,1,sizeof pixels,f)==sizeof pixels);assert(!fclose(f));
    enabled=1;memcpy(saved,state,sizeof state);
    assert(PcPort_Hd2dSpriteSubmitPart(&p,&m,&q));assert(links==1 && linked_ot==q.ot);
    assert(!memcmp(state,saved,sizeof state));
    /* Only linkage may differ; all retail geometry, colour and blend remain exact. */
    ((P_TAG *)&before)->addr=42;assert(!memcmp(&before,&packet,sizeof packet));
    assert(PcPort_Hd2dSpriteTextureOverride(&packet,9,8,1,3,2,4,&tex,uv));
    assert(tex==43 && uv[0]==.5f/64 && uv[2]==15.5f/64);
    other=packet;assert(!PcPort_Hd2dSpriteTextureOverride(&other,9,8,1,3,2,4,&tex,uv));
    packet.r0++;assert(!PcPort_Hd2dSpriteTextureOverride(&packet,9,8,1,3,2,4,&tex,uv));packet.r0--;
    PcPort_Hd2dSpritePresentDone();assert(!PcPort_Hd2dSpriteTextureOverride(&packet,9,8,1,3,2,4,&tex,uv));
    return 0;
}
