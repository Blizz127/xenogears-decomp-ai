#include "hd2d_integration.h"
#include "hd2d_sprite.h"
#include <GL/gl.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
int g_GameSceneMapNum=2;
static int field_calls,gl_calls,producer_calls,present_calls,registered,producer_mode;
static XgHd2dFrame last;
void PcPort_DevMenuRegisterEnhancement(const char *label,int (*get)(void),void (*toggle)(void))
{
    assert(label && get && toggle);
    if (!registered) assert(!strcmp(label,"HD-2D (Octopath style)"));
    ++registered;
}
int PcPort_QuickCheckpointFieldIsActive(void) { ++field_calls; return 1; }
void glGetIntegerv(GLenum key,GLint *out)
{
    assert(key==GL_VIEWPORT); ++gl_calls;
    out[0]=0; out[1]=0; out[2]=640; out[3]=480;
}
int XgHd2d_Present(const XgHd2dConfig *c,const XgHd2dFrame *f)
{
    assert(c->enabled); ++present_calls; last=*f; return 1;
}
int PcPort_Hd2dSceneInputs(int path,int width,int height,XgHd2dFrame *out)
{
    assert(path==0 && width==640 && height==480); ++producer_calls;
    out->field_active=1; out->field_id=producer_mode==2 ? 3 : 2;
    out->width=640; out->height=480;
    out->linear_depth_verified=out->protection_verified=1;
    out->linear_depth_texture=11; out->protection_texture=12;
    return producer_mode!=0;
}
int main(void)
{
    assert(registered==9);
    assert(!PcPort_Hd2dEnabled());
    assert(PcPort_Hd2dConfiguration()->map_count==1 &&
        PcPort_Hd2dConfiguration()->maps[0]==14);
    PcPort_Hd2dPostProcess(0,640,480);
    assert(!field_calls && !gl_calls && !producer_calls && !present_calls);
    assert(!PcPort_Hd2dSetIntensity(-1,0.5f));
    assert(!PcPort_Hd2dSetIntensity(0,NAN));
    assert(!PcPort_Hd2dSetIntensity(0,1.1f));
    assert(PcPort_Hd2dSetIntensity(0,0.75f));
    int maps[]={2,4},duplicates[]={2,2},invalid[]={1024};
    assert(!PcPort_Hd2dSetOpeningMaps(duplicates,2));
    assert(!PcPort_Hd2dSetOpeningMaps(invalid,1));
    PcPort_Hd2dSetEnabled(1);
    PcPort_Hd2dPostProcess(0,640,480);
    assert(!gl_calls && !present_calls); /* unapproved map */
    assert(PcPort_Hd2dSetOpeningMaps(maps,2));
    g_GameSceneMapNum=12; PcPort_Hd2dPostProcess(0,640,480);
    assert(!gl_calls && !present_calls);
    g_GameSceneMapNum=2; producer_mode=1;
    PcPort_Hd2dPostProcess(0,640,480);
    assert(present_calls==1 && last.linear_depth_verified && last.protection_verified);
    PcPort_Hd2dPostProcess(1,640,480);
    assert(present_calls==2 && producer_calls==1);
    assert(!last.linear_depth_verified && !last.protection_verified);
    producer_mode=2; PcPort_Hd2dPostProcess(0,640,480);
    assert(!last.linear_depth_verified && !last.protection_verified); /* wrong map */
    int calls=gl_calls; PcPort_Hd2dSetEnabled(0);
    PcPort_Hd2dPostProcess(0,640,480); assert(gl_calls==calls);
    unsigned texture=99; float uv[]={1,2,3,4};
    assert(!PcPort_Hd2dTextureOverride(1,2,3,4,5,6,&texture,uv));
    assert(texture==99 && uv[0]==1 && uv[3]==4);
    puts("HD2D bridge: Off bypass, scope, menu registration, intensity and current-frame inputs PASS");
    return 0;
}
