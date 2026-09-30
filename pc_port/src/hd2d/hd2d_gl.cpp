/* Standalone desktop OpenGL backend, deliberately not linked until reviewed. */
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include "xg_plat/hd2d.h"
#include <cmath>
#include <cstdio>
#include <cstring>
#include <limits.h>

namespace {
GLuint program, vao, colorTexture, sampler;
int textureWidth, textureHeight;
char errorText[1024];
const char *fragmentSource =
#include "../../shaders/hd2d.frag.inc"
;
const char *vertexSource = R"GLSL(#version 330 core
void main() {
    vec2 p=vec2(float((gl_VertexID<<1)&2),float(gl_VertexID&2));
    gl_Position=vec4(p*2.0-1.0,0.0,1.0);
}
)GLSL";
const GLenum toggles[] = {GL_BLEND, GL_DEPTH_TEST, GL_STENCIL_TEST, GL_CULL_FACE,
    GL_SCISSOR_TEST, GL_FRAMEBUFFER_SRGB, GL_DITHER, GL_RASTERIZER_DISCARD,
    GL_COLOR_LOGIC_OP, GL_SAMPLE_ALPHA_TO_COVERAGE, GL_SAMPLE_COVERAGE};
struct State {
    GLint program, vao, active, readFbo, drawFbo, viewport[4], unpack;
    GLint textures[3], samplers[3], polygon[2], defaultRead, defaultDraw, profile;
    GLboolean mask[4], enabled[sizeof(toggles)/sizeof(*toggles)];
    State() {
        glGetIntegerv(GL_CURRENT_PROGRAM,&program); glGetIntegerv(GL_VERTEX_ARRAY_BINDING,&vao);
        glGetIntegerv(GL_ACTIVE_TEXTURE,&active); glGetIntegerv(GL_VIEWPORT,viewport);
        glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING,&readFbo);
        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING,&drawFbo);
        glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING,&unpack);
        glGetIntegerv(GL_CONTEXT_PROFILE_MASK,&profile);
        glGetIntegerv(GL_POLYGON_MODE,polygon); glGetBooleanv(GL_COLOR_WRITEMASK,mask);
        for (unsigned i=0;i<sizeof(toggles)/sizeof(*toggles);++i) {
            enabled[i]=glIsEnabled(toggles[i]); glDisable(toggles[i]);
        }
        for(int i=0;i<3;++i) {
            glActiveTexture(GL_TEXTURE0+i); glGetIntegerv(GL_TEXTURE_BINDING_2D,&textures[i]);
            glGetIntegeri_v(GL_SAMPLER_BINDING,i,&samplers[i]);
        }
        glBindFramebuffer(GL_READ_FRAMEBUFFER,0); glGetIntegerv(GL_READ_BUFFER,&defaultRead);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0); glGetIntegerv(GL_DRAW_BUFFER,&defaultDraw);
        glReadBuffer(GL_BACK); glDrawBuffer(GL_BACK);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
        glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE); glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    }
    ~State() {
        glUseProgram(program); glBindVertexArray(vao);
        for(int i=0;i<3;++i) {
            glActiveTexture(GL_TEXTURE0+i); glBindTexture(GL_TEXTURE_2D,textures[i]);
            glBindSampler(i,samplers[i]);
        }
        glActiveTexture(active); glBindBuffer(GL_PIXEL_UNPACK_BUFFER,unpack);
        glReadBuffer(defaultRead); glDrawBuffer(defaultDraw);
        glBindFramebuffer(GL_READ_FRAMEBUFFER,readFbo); glBindFramebuffer(GL_DRAW_FRAMEBUFFER,drawFbo);
        glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);
        glColorMask(mask[0],mask[1],mask[2],mask[3]);
        if ((profile & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT) && polygon[0]!=polygon[1]) {
            glPolygonMode(GL_FRONT,polygon[0]); glPolygonMode(GL_BACK,polygon[1]);
        } else glPolygonMode(GL_FRONT_AND_BACK,polygon[0]);
        for(unsigned i=0;i<sizeof(toggles)/sizeof(*toggles);++i)
            if(enabled[i]) glEnable(toggles[i]); else glDisable(toggles[i]);
    }
};
void fail(const char *message) { std::snprintf(errorText,sizeof(errorText),"%s",message); }
GLuint compile(GLenum type,const char *source) {
    GLuint shader=glCreateShader(type); if(!shader) { fail("cannot create shader"); return 0; }
    glShaderSource(shader,1,&source,NULL); glCompileShader(shader);
    GLint ok=0; glGetShaderiv(shader,GL_COMPILE_STATUS,&ok);
    if(!ok) { glGetShaderInfoLog(shader,sizeof(errorText),NULL,errorText); glDeleteShader(shader); return 0; }
    return shader;
}
bool initialize() {
    if(program) return true;
    GLuint vs=compile(GL_VERTEX_SHADER,vertexSource); if(!vs) return false;
    GLuint fs=compile(GL_FRAGMENT_SHADER,fragmentSource);
    if(!fs) { glDeleteShader(vs); return false; }
    GLuint p=glCreateProgram();
    glAttachShader(p,vs); glAttachShader(p,fs); glLinkProgram(p);
    glDeleteShader(vs); glDeleteShader(fs);
    GLint ok=0; glGetProgramiv(p,GL_LINK_STATUS,&ok);
    if(!ok) { glGetProgramInfoLog(p,sizeof(errorText),NULL,errorText); glDeleteProgram(p); return false; }
    glGenVertexArrays(1,&vao); glGenTextures(1,&colorTexture); glGenSamplers(1,&sampler);
    if(!vao || !colorTexture || !sampler) {
        glDeleteProgram(p); glDeleteVertexArrays(1,&vao); glDeleteTextures(1,&colorTexture);
        glDeleteSamplers(1,&sampler); vao=colorTexture=sampler=0;
        fail("cannot allocate GL objects"); return false;
    }
    glSamplerParameteri(sampler,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glSamplerParameteri(sampler,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glSamplerParameteri(sampler,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    program=p; return true;
}
bool textureValid(GLuint texture,GLint format,int width,int height) {
    if(!texture || !glIsTexture(texture)) return false;
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D,texture);
    GLint w=0,h=0,f=0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&h);
    glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_INTERNAL_FORMAT,&f);
    return w==width && h==height && f==format;
}
void scalar(const char *name,int v) { glUniform1i(glGetUniformLocation(program,name),v); }
void items(const char *name,const char *countName,const XgHd2dItem *items,unsigned count) {
    XgHd2dItem valid[XG_HD2D_MAX_ITEMS]; int n=0;
    if(count<=XG_HD2D_MAX_ITEMS) for(unsigned i=0;i<count;++i) {
        const XgHd2dItem &v=items[i];
        if(std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z) &&
           std::isfinite(v.w) && v.z>0 && v.w>0) valid[n++]=v;
    }
    scalar(countName,n);
    if(n) glUniform4fv(glGetUniformLocation(program,name),n,&valid[0].x);
}
}
extern "C" const char *XgHd2d_LastError(void) { return errorText; }
extern "C" int XgHd2d_Present(const XgHd2dConfig *config,const XgHd2dFrame *frame) {
    XgHd2dPlan plan=XgHd2d_Plan(config,frame);
    if(!plan.active) return 0; /* Includes master OFF: no GL queries or allocation. */
    if(frame->width>INT_MAX-frame->x || frame->height>INT_MAX-frame->y) { fail("invalid viewport"); return -1; }
    GLint major=0,minor=0,maxTexture=0;
    glGetIntegerv(GL_MAJOR_VERSION,&major); glGetIntegerv(GL_MINOR_VERSION,&minor);
    if(major<3 || (major==3 && minor<3)) { fail("desktop GL 3.3 required"); return -1; }
    glGetIntegerv(GL_MAX_TEXTURE_SIZE,&maxTexture);
    if(frame->width>maxTexture || frame->height>maxTexture) { fail("viewport exceeds texture limit"); return -1; }
    State saved;
    XgHd2dFrame supported=*frame;
    if(!frame->linear_depth_verified || !textureValid(frame->linear_depth_texture,GL_R32F,frame->width,frame->height))
        supported.linear_depth_verified=0;
    if(!frame->protection_verified || !textureValid(frame->protection_texture,GL_R8,frame->width,frame->height))
        supported.protection_verified=0;
    plan=XgHd2d_Plan(config,&supported);
    if(!plan.active) { fail("requested effects unavailable: missing depth or sprite/UI protection"); return 0; }
    if(!initialize()) return -1;
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D,colorTexture);
    if(textureWidth!=frame->width || textureHeight!=frame->height) {
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,frame->width,frame->height,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
        GLint actual=0,actualHeight=0; glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&actual);
        glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&actualHeight);
        if(actual!=frame->width || actualHeight!=frame->height) { fail("color capture allocation failed"); return -1; }
        textureWidth=frame->width; textureHeight=frame->height;
    }
    glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,frame->x,frame->y,frame->width,frame->height);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D,supported.linear_depth_verified ? frame->linear_depth_texture : 0);
    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D,supported.protection_verified ? frame->protection_texture : 0);
    for(int i=0;i<3;++i) glBindSampler(i,sampler);
    glUseProgram(program); glBindVertexArray(vao);
    scalar("sceneColor",0); scalar("sceneDepth",1); scalar("protection",2);
    scalar("hasDepth",supported.linear_depth_verified); scalar("hasProtection",supported.protection_verified);
    glUniform2f(glGetUniformLocation(program,"origin"),float(frame->x),float(frame->y));
    glUniform1fv(glGetUniformLocation(program,"amount"),XG_HD2D_EFFECT_COUNT,plan.intensity);
    if(plan.intensity[XG_HD2D_DOF]>0)
        glUniform4f(glGetUniformLocation(program,"focus"),config->focus_depth,config->focus_range,config->focus_tilt,0);
    else glUniform4f(glGetUniformLocation(program,"focus"),1,1,0,0);
    glUniform2f(glGetUniformLocation(program,"fogRange"),config->fog_start,config->fog_end);
    items("lights","lightCount",frame->lights,frame->light_count);
    items("shadows","shadowCount",frame->shadows,frame->shadow_count);
    items("particles","particleCount",frame->particles,frame->particle_count);
    glViewport(frame->x,frame->y,frame->width,frame->height);
    glDrawArrays(GL_TRIANGLES,0,3); errorText[0]=0; return 1;
}
extern "C" void XgHd2d_Shutdown(void) {
    if(!program && !vao && !colorTexture && !sampler) return;
    glDeleteProgram(program); glDeleteVertexArrays(1,&vao);
    glDeleteTextures(1,&colorTexture); glDeleteSamplers(1,&sampler);
    program=vao=colorTexture=sampler=0; textureWidth=textureHeight=0;
}
