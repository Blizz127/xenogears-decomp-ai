/* Landed hook integration. New master OFF never touches game or GL state. */
#include "hd2d_integration.h"
#include "hd2d_sprite.h"
#include "port_dev_menu.h"
#include "quick_checkpoint.h"
#include <GL/gl.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

extern int g_GameSceneMapNum;
#pragma weak PcPort_Hd2dSceneInputs
extern void PcPort_Hd2dSpritePresentDone(void) __attribute__((weak));
static XgHd2dConfig config;
static int initialized;

static void initialize(void)
{
    if (initialized) return;
    initialized=1;
    XgHd2d_DefaultConfig(&config);
    /* Integrator field-14 checkpoint verifies Fei's starting house. */
    config.maps[0]=14; config.map_count=1;
    for (int e=0;e<XG_HD2D_EFFECT_COUNT;++e) config.effect[e].enabled=1;
    /* Private diagnostic override; production opening maps need approval. */
    const char *maps=getenv("XENO_HD2D_OPENING_MAPS");
    if (maps && *maps) {
        int parsed[XG_HD2D_MAX_MAPS]; unsigned count=0; const char *p=maps;
        while (*p && count<XG_HD2D_MAX_MAPS) {
            char *end; long v=strtol(p,&end,10);
            if (end==p || v<0 || v>=1024 || (*end && *end!=',')) { count=0; break; }
            parsed[count++]=(int)v; p=*end ? end+1 : end;
            if (*end && !*p) { count=0; break; }
        }
        if (!*p && count) PcPort_Hd2dSetOpeningMaps(parsed,count);
    }
    const char *on=getenv("XENO_HD2D_OCTOPATH");
    config.enabled=on && !strcmp(on,"1");
}
int PcPort_Hd2dEnabled(void) { initialize(); return config.enabled; }
void PcPort_Hd2dSetEnabled(int enabled)
{
    initialize(); enabled=!!enabled;
    if (config.enabled!=enabled && PcPort_Hd2dSpritePresentDone)
        PcPort_Hd2dSpritePresentDone(); /* CPU-only packet lifetime invalidation. */
    config.enabled=enabled;
}
const XgHd2dConfig *PcPort_Hd2dConfiguration(void) { initialize(); return &config; }
int PcPort_Hd2dSetIntensity(int e,float value)
{
    initialize();
    if (e<0 || e>=XG_HD2D_EFFECT_COUNT || !isfinite(value) || value<0 || value>1) return 0;
    config.effect[e].intensity=value; return 1;
}
int PcPort_Hd2dSetOpeningMaps(const int *maps,unsigned count)
{
    initialize();
    if (!maps || !count || count>XG_HD2D_MAX_MAPS) return 0;
    for (unsigned i=0;i<count;++i) {
        if (maps[i]<0 || maps[i]>=1024) return 0;
        for (unsigned j=0;j<i;++j) if (maps[i]==maps[j]) return 0;
    }
    memcpy(config.maps,maps,count*sizeof(*maps)); config.map_count=count; return 1;
}
int PcPort_Hd2dCurrentMap(void) { return g_GameSceneMapNum & 0x3fff; }
int PcPort_Hd2dOpeningActive(void)
{
    if (!PcPort_Hd2dEnabled()) return 0;
    if (!PcPort_QuickCheckpointFieldIsActive()) return 0;
    int map=PcPort_Hd2dCurrentMap();
    for (unsigned i=0;i<config.map_count;++i) if (config.maps[i]==map) return 1;
    return 0;
}
static void toggle_master(void) { PcPort_Hd2dSetEnabled(!PcPort_Hd2dEnabled()); }
#define CONTROL(NAME,INDEX) \
    static int get_##NAME(void) { initialize(); return config.effect[INDEX].enabled; } \
    static void toggle_##NAME(void) { initialize(); config.effect[INDEX].enabled^=1; }
CONTROL(dof,XG_HD2D_DOF)
CONTROL(bloom,XG_HD2D_BLOOM)
CONTROL(light,XG_HD2D_LIGHT)
CONTROL(shadow,XG_HD2D_SHADOW)
CONTROL(vignette,XG_HD2D_VIGNETTE)
CONTROL(fog,XG_HD2D_FOG)
CONTROL(particles,XG_HD2D_PARTICLES)
CONTROL(grade,XG_HD2D_GRADE)
__attribute__((constructor)) static void register_menu(void)
{
    initialize();
    PcPort_DevMenuRegisterEnhancement("HD-2D (Octopath style)",PcPort_Hd2dEnabled,toggle_master);
    PcPort_DevMenuRegisterEnhancement("Depth of field / tilt shift",get_dof,toggle_dof);
    PcPort_DevMenuRegisterEnhancement("Bloom / glow",get_bloom,toggle_bloom);
    PcPort_DevMenuRegisterEnhancement("Warm lighting",get_light,toggle_light);
    PcPort_DevMenuRegisterEnhancement("Sprite shadows",get_shadow,toggle_shadow);
    PcPort_DevMenuRegisterEnhancement("Vignette",get_vignette,toggle_vignette);
    PcPort_DevMenuRegisterEnhancement("Fog",get_fog,toggle_fog);
    PcPort_DevMenuRegisterEnhancement("Particles",get_particles,toggle_particles);
    PcPort_DevMenuRegisterEnhancement("Colour grading",get_grade,toggle_grade);
}
void PcPort_Hd2dPostProcess(int path,int width,int height)
{
    if (!PcPort_Hd2dEnabled()) return;
    /* Both landed hooks run after GPU submission has consumed this OT. */
    if (PcPort_Hd2dSpritePresentDone) PcPort_Hd2dSpritePresentDone();
    if (!PcPort_Hd2dOpeningActive() || width<=0 || height<=0) return;
    XgHd2dFrame frame={0};
    frame.field_active=1; frame.field_id=PcPort_Hd2dCurrentMap();
    GLint viewport[4]; glGetIntegerv(GL_VIEWPORT,viewport);
    if (viewport[0]<0 || viewport[1]<0 || viewport[2]<=0 || viewport[3]<=0 ||
        viewport[0]>width-viewport[2] || viewport[1]>height-viewport[3]) return;
    frame.x=viewport[0]; frame.y=viewport[1]; frame.width=viewport[2]; frame.height=viewport[3];
    if (path==0 && PcPort_Hd2dSceneInputs) {
        XgHd2dFrame produced={0};
        if (PcPort_Hd2dSceneInputs(path,width,height,&produced) &&
            produced.field_active && produced.field_id==frame.field_id &&
            produced.x==frame.x && produced.y==frame.y &&
            produced.width==frame.width && produced.height==frame.height) frame=produced;
    }
    /* VRAM-path metadata cannot inherit attachments from an earlier scene. */
    XgHd2d_Present(&config,&frame);
}
int PcPort_Hd2dTextureOverride(int tpage,int clut,int u0,int v0,int u1,int v1,
    unsigned *texture,float uv[4])
{
    (void)tpage; (void)clut; (void)u0; (void)v0; (void)u1; (void)v1;
    (void)texture; (void)uv;
    /* No retail-derived area pack or guessed UV association is supplied. */
    return 0;
}
