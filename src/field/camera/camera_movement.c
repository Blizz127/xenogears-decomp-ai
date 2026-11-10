#include "common.h"
#include "main/game.h"
#include "system/math.h"
#include "field/main.h"
#include "field/camera.h"
#include "field/actor.h"
#include "field/script_vm.h"

extern s16 g_CamEyeMovementDuration;
extern s16 g_CamAtMovementDuration;
extern VECTOR g_CamAtMovementFrom;
extern VECTOR g_CamAtMovementTo;
extern VECTOR g_CamEyeMovementFrom;
extern VECTOR g_CamEyeMovementTo;
extern VECTOR g_CameraEye;
extern VECTOR g_CameraEye2;
extern VECTOR g_CameraAt;
extern VECTOR g_CameraAt2;
extern u16 g_CamMovementFlags;
extern VECTOR g_CamAtMovementCurrent;
extern VECTOR g_CamAtMovementDelta;
extern VECTOR g_CamEyeMovementCurrent;
extern VECTOR g_CamEyeMovementDelta;
extern s16 g_FieldCameraMode;
extern s32 D_800AF930[];
extern s16 D_800AF936[];
extern s16 D_800AF938[];
extern s16 D_800AF93A[];
extern s32 D_800B21D8;
extern s16 g_FieldCameraModes[] asm("g_FieldCameraMode");


void FieldScriptWaitForCameraMovement(void) {
    int targetFlags;
    int camMovementFlags;
    
    targetFlags = FieldScriptVMGetArgument(1);
    camMovementFlags = FIELD_CAMERA_AT_MOVEMENT_ACTIVE | FIELD_CAMERA_EYE_MOVEMENT_ACTIVE;
    
    // Is camera target movement done?
    if (g_CamAtMovementDuration == 0) {
        camMovementFlags &= ~FIELD_CAMERA_AT_MOVEMENT_ACTIVE;
    }
    
    // Is camera position movement done?
    if (g_CamEyeMovementDuration == 0) {
        camMovementFlags &= FIELD_CAMERA_AT_MOVEMENT_ACTIVE;
    }
    
    D_800B00C0 = 1;

    // Advance instruction pointer only when certain camera movements are done
    if (!(camMovementFlags & targetFlags)) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
    }
}


void func_8008FABC(void) {
    s16 angleY;

    angleY = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(3));
    g_Scene.sceneAngle.vy = angleY;
    g_Scene.unk7C = angleY;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

/* FE6D — snapshot current SCRZ×scale / DIP / yaw into the scripted-cam scratch. */
void func_8008FB28(void) {
    D_800AF93A[0] = 0x1000;
    D_800AF938[0] = *(u16*)((u8*)&g_Scene + 0x56);
    D_800AF936[0] = *(u16*)((u8*)&g_Scene + 0x6C);
    D_800AF930[0] = (*(s32*)((u8*)&g_Scene + 0x68) * *(s16*)((u8*)&g_Scene + 0x6E)) >> 12;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

void func_8008FB98(void) {
#ifdef XENO_PC_PORT
    u16 angleY;
    u16 dip;
#else
    register u16 angleY asm("$3");
    register u16 dip asm("$4");
#endif

    g_FieldCameraMode = 1;
    g_FieldScriptMaxInstructionCount += 4;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;

    D_800AF93A[0] = 0x1000;
    D_800AF930[0] = ((s32)g_Scene.sceneScrZ * (s16)g_Scene.sceneScale) >> 12;
    angleY = g_Scene.sceneAngle.vy;
    dip = g_Scene.sceneDIP;
    g_CamInterpolation.atStepDistance = 0xC;
    g_CamInterpolation.eyeStepDistance = 0xC;
    D_800AF938[0] = angleY;
    g_Scene.unk48 |= 0x8000;
    D_800AF936[0] = dip;
}

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): OP_9A (src/field/f_8008FA38.c). */
#ifndef XM_T_LightDef
#define XM_T_LightDef
typedef struct {
     s32 m0_x;  
     s32 m4_y;
     s32 m8_z;
     u16 mC_r;  
     u16 mE_g;
     u16 m10_b;
} LightDef;
#endif
#ifndef XM_T_DynVertOffset
#define XM_T_DynVertOffset
typedef struct {
     s16 m0_offset[3];
     s16 m6_index;
} DynVertOffset;
#endif
#ifndef XM_T_ModelDynVertEntry
#define XM_T_ModelDynVertEntry
typedef struct {
     s32 m0_count;
     DynVertOffset* m4_positions;
     DynVertOffset* m8_normals;
} ModelDynVertEntry;
#endif
#ifndef XM_T_ModelDynVerts
#define XM_T_ModelDynVerts
typedef struct {
     s32 m0_count;
     ModelDynVertEntry m4_entries[1];
} ModelDynVerts;
#endif
#ifndef XM_T_ModelBlock
#define XM_T_ModelBlock
typedef struct {
     u16 m0_flags;
     u16 m2_numVertices;
     u16 m4_numPrims;
     u16 m6_numMeshBlocks;
     SVECTOR* m8_vertices;
     SVECTOR* mC_normals;
     u8* m10_meshBlocks;
     u8* m14_displayList;
     u8* m18_instanceData;
     ModelDynVerts* m1C_dynamicVertices;
     s16 m20_boundingBox[8];
     s32 m30_instanceDataSize;
     s32 m34_primBufferSize;
} ModelBlock;
#endif
#ifndef XM_T_sFieldEntitySub0
#define XM_T_sFieldEntitySub0
typedef struct sFieldEntitySub0 {
     s32 m0;
     ModelBlock* m4_pModelBlock;
     u8* m8_primBuffers[2];
     s16 m10;
     s16 m12_renderMode;
     void* m14_dynamicVertices;
     SVECTOR m18_boundingCenter;
     s16 m20_boundingRadius;
} sFieldEntitySub0;
#endif
#ifndef XM_T_FP_VEC3
#define XM_T_FP_VEC3
typedef struct {
    s32 vx, vy, vz;
} FP_VEC3;
#endif
#ifndef XM_T_SubSprite
#define XM_T_SubSprite
typedef struct {
     s16 m0_offsetX;
     s16 m2_offsetY;
     u8 m4_texcoordX;
     u8 m5_texcoordY;
     u8 m6_width;
     u8 m7_height;
     s8 m8_widthAdjust;
     s8 m9_heightAdjust;
     u16 mA_tpage;
     u16 mC_clut;
     u16 mE;
     u32 m10_colorAndCode;
     union {                     
                   u32 raw;
                   struct {
                       u32 x0_transformId : 3;
                       u32 x3_8bit : 1;         
                       u32 x4_tileFlipX : 1;
                       u32 x5_tileFlipY : 1;
                       u32 x6 : 26;
                   } bits;
               } m14_attrs;
} SubSprite;
#endif
#ifndef XM_T_SubgroupTransform
#define XM_T_SubgroupTransform
typedef struct {
     s8 m0_translateX;
     s8 m1_translateY;
     s16 m2_rotateX;
     s16 m4_rotateY;
     s16 m6_rotateZ;
} SubgroupTransform;
#endif
#ifndef XM_T_SpriteObject
#define XM_T_SpriteObject
typedef struct SpriteObject {
     s16 m0_rotation[3];
     s16 m6_scale[3];
     MATRIX mC_matrix;
     SubSprite* m2C_subSpriteBuffer;
     SubSprite* m30_subSprites;
     SubgroupTransform* m34_perSubgroupTransform; 
     struct SpriteActorCore* m38_pNext;
     s8 m3C_anchorX;
     s8 m3D_anchorY;                          
} SpriteObject;
#endif
#ifndef XM_T_uptr
#define XM_T_uptr
typedef u32 uptr;
#endif
#ifndef XM_T_Vec2s16
#define XM_T_Vec2s16
typedef struct {
    s16 vx, vy;
} Vec2s16;
#endif
#ifndef XM_T_SpriteVramData
#define XM_T_SpriteVramData
typedef struct {
     uptr m0_spriteData;
     Vec2s16 m4_vramLocation;
     Vec2s16 m8_clut;
     u8* mC_clutData;
     uptr m10_startOfAnimationContainer;
} SpriteVramData;
#endif
#ifndef XM_T_SpriteActorExtra
#define XM_T_SpriteActorExtra
typedef struct {
     s32 m0;
     s32 m4_gravityOverride;
     s32 m8;
     u16 mC_animationSignal;                    
     Vec2s16 mE_vramLocation;
     s16 m12;
     s16 m14_actorId;
     s16 m16;
     s16* m18_vramLocations;
} SpriteActorExtra;
#endif
#ifndef XM_T_SpriteActorCore
#define XM_T_SpriteActorCore
typedef struct SpriteActorCore {
     FP_VEC3 m0_position;
     FP_VEC3 mC_step;
     s32 m18_moveSpeed;
     s32 m1C_gravity;
     SpriteObject* m20_object;
     SpriteVramData* m24_vramData;
     u8 m28_r;
     u8 m29_g;
     u8 m2A_b;
     u8 m2B_code;                   
     s16 m2C_scale;
     s16 m2E_otIndex;
     s16 m30_otOffset;
     s16 m32_direction;
     s16 m34_currentSpriteFrame;
     s16 m36_height;
     s16 m38_centerHeight;
     s16 m3A_scaleModifier;
     u32 m3C_x0_type : 2;           
               u32 m3C_x2 : 1;
               u32 m3C_x3_flipX : 1;
               u32 m3C_x4 : 1;
               u32 m3C_x5_blendMode : 3;      
               u32 m3C_x8 : 8;
               u32 m3C_x10 : 4;
               u32 m3C_x14 : 4;
               u32 m3C_x18 : 1;
               u32 m3C_x19 : 1;
               u32 m3C_x1A : 1;                
               u32 m3C_x1B : 1;
               u32 m3C_x1C_matrixDirty : 1;
               u32 m3C_x1D : 1;
               u32 m3C_x1E_bundleChanged : 1;
               u32 m3C_x1F : 1;
     union {                     
                   u32 raw;
                   struct {
                       u32 x0 : 1;
                       u32 x1 : 1;
                       u32 x2_subSpriteCount : 6;
                       u32 x8 : 5;
                       u32 xD_mode : 4;          
                       u32 x11 : 1;
                       u32 x12 : 1;              
                       u32 x13 : 1;
                       u32 x14 : 1;              
                       u32 x15 : 11;
                   } bits;
               } m40_draw;
     void* m44_currentAnimationBundle;
     void* m48_defaultAnimationBundle;
     void* m4C_specialAnimation;
     u8* m50_soundEffectSeq;
     u16* m54_frameTable;
     u8* m58_startOfCurrentAnimation;
     u8* m5C;
     u8* m60_endOfAnimationContainer;
     u8* m64_spriteByteCode;
     void (*m68)(struct SpriteActorCore*);
     void* m6C_owner;
     struct SpriteActorCore* m70_parent;
     struct SpriteActorCore* m74_targetEntitySprite;
     u32 m78_terrainTriangle;
     SpriteActorExtra* m7C_extra;
     s16 m80_angle;
     s16 m82_entityScale;
     s16 m84_maxY;
     u16 m86_thisSize;
     u8* m88_stack2;
     s8 m8C_stackPosition;
     s8 m8D_parentAnimationId;
     u8 m8E_stack[16];
     s16 m9E_wait;
     SVECTOR mA0_destination;
     u32 mA8_x0 : 1;
               u32 mA8_x1_bounce : 10;
               u32 mA8_xB_frame : 6;
               u32 mA8_x11 : 3;
               u32 mA8_x14 : 2;
               u32 mA8_x16_bytecodeStep : 6;
               u32 mA8_x1C : 2;
               u32 mA8_x1E : 2;
#if VERSION_JP
     u32 mAC_x0 : 4;                
#else
     u32 mAC_x0 : 2;
#endif
               u32 mAC_x2_facing : 1;
               u32 mAC_x3 : 1;
               u32 mAC_x4 : 1;
               u32 mAC_x5 : 1;
               u32 mAC_x6 : 1;
               u32 mAC_x7_timeScale : 12;
#if VERSION_JP
               u32 mAC_x13 : 3;
#else
               u32 mAC_x13 : 5;
#endif
     u8 mAF_animationId;            
     s32 mB0_x0_animationId : 8;
               u32 mB0_x8 : 1;
               u32 mB0_x9 : 1;
               u32 mB0_xA : 1;
               u32 mB0_xB : 1;
               u32 mB0_xC : 20;
} SpriteActorCore;
#endif
#ifndef XM_T_sFieldScriptEntityScriptSlot
#define XM_T_sFieldScriptEntityScriptSlot
typedef struct {
     u16 m0_scriptPC;
     u8 m2_delay;
     u8 m3_scriptIndex; 
     u32 m4_x0_numSteps : 16;
              u32 m4_x10_status : 2;
              u32 m4_x12_priority : 4; 
              u32 m4_x16 : 1;
              u32 m4_x17_walkMode : 2;
              u32 m4_x19 : 1;
              u32 m4_x1A : 1;
              u32 m4_x1B : 2;
              u32 m4_x1D : 3;
} sFieldScriptEntityScriptSlot;
#endif
#ifndef XM_T_sFieldScriptEntity
#define XM_T_sFieldScriptEntity
typedef struct sFieldScriptEntity {
     u32 m0_fieldScriptFlags; 
     u32 m4_flags;
     s16 m8_currentWalkMeshTriangle[4];
     s16 m10_walkmeshId;
     u16 m12;
     u32 m14_currentTriangleFlag;
     s16 m18_boundingVolume[3];
     u16 m1E_collisionRadius; 
     VECTOR m20_position;
     VECTOR m30_stepVector;
     VECTOR m40_externalStep;
     VECTOR m50_surfaceNormal;
     SVECTOR m60_interactionOffset;
     s16 m68_oldPosition[3];
     s16 m6E_stuckCounter;
     s16 m70_rotationForRendering;
     s16 m72_elevation;
     u8 m74_touchedActor; 
     u8 m75_parentActorId; 
     u16 m76_timeScale; 
     u16 m78_stack[4];
     u8 m80_dialogPortrait; 
     u8 m81_selectedMultichoiceLine; 
     u8 m82_dialogWindowSize[2]; 
     s32 m84_dialogWindowFlags;
     s16 m88_dialogWindowPosition[2];
     sFieldScriptEntityScriptSlot m8C_scriptSlots[8];
     u16 mCC_scriptPC;
     u8 mCE_currentScriptSlot;
     u8 mCF_scriptSlotWaitedOn;
     VECTOR mD0_targetPositionOffset;
     s16 mE0_rotationLimit;
     u8 mE2_doorFrame;  
     u8 mE3_pushCounter;  
     s16 mE4_playableCharacterId;
     s16 mE6_idleAnimationId;
     s16 mE8_currentAnimationId;
     s16 mEA_forcedAnimation;
     s16 mEC_elevation;
     s16 mEE_spriteSplitHeight;
     s32 mF0_fallSpeed;
     s16 mF4_scale3d[3];
     s16 mFA;
     u8 mFC_spriteColor[3];
     u8 mFF_spriteColor2[3];
     s16 m102_numSteps;
     s16 m104_rotation;
     s16 m106_currentRotation;
     s16 m108_rotation3;
     u16 m10A_soundId;
     u8 m10C_soundVolume;
     u8 m10D_soundMode;  
     s16 m10E;
     s16* m110_modelRotation2;
     Vec2s16* m114_movementBoundingZone; 
     s32* m118_deformationStrength;
     s16 m11C_lockedRotation;
     s16 m11E_rotationSpeed;
     void* m120_special2dAnimation;
     s16 m124_special2dAnimationId;
     u8 m126_spriteSource;
     u8 m127_vramSlot;
     u16 m128_attachedMechaBone; 
     s16 m12A;
     u32 m12C_x0 : 2;
                u32 m12C_x2 : 3;
                u32 m12C_x5_doorMoving : 1; 
                u32 m12C_x6_stackDepth : 3; 
                u32 m12C_x9 : 3;
                u32 m12C_xC : 1;
                u32 m12C_xD : 3;
                u32 m12C_x10 : 2;
                u32 m12C_x12 : 10;
                u32 m12C_x1C : 4;
     u32 m130_x0 : 9;
                u32 m130_x9 : 10;
                u32 m130_x13 : 9;
                u32 m130_x1C : 2; 
                u32 m130_x1E : 2;
     u32 m134_x0 : 4;  
                u32 m134_x4 : 1;  
                u32 m134_x5 : 2;
                u32 m134_x7 : 1;  
                u32 m134_x8 : 24;
} sFieldScriptEntity;
#endif
#ifndef XM_T_sFieldEntity
#define XM_T_sFieldEntity
typedef struct sFieldEntity {
     sFieldEntitySub0* m0_model;
     SpriteActorCore* m4_pVramSpriteSheet;
     void* m8_2dSprite;
     MATRIX mC_matrix;
     MATRIX m2C_matrixBackup;
     sFieldScriptEntity* m4C_scriptEntity;
     SVECTOR m50_modelRotation;
     u16 m58_flags;
     u16 m5A_spriteFlags;
} sFieldEntity;
#endif
#ifndef XM_T_FieldBss_800AF880
#define XM_T_FieldBss_800AF880
typedef struct {
     VECTOR m0_cameraEye;          
     VECTOR m10_cameraAt;          
     VECTOR m20_cameraUp;          
     VECTOR m30_cameraEye2;        
     VECTOR m40_cameraAt2;         
     VECTOR m50_cameraUp2;         
     VECTOR m60_cameraOffset;      
     s32 m70_cameraTargetOverride[3];   
     s32 m7C;
     s32 m80_desiredCameraTarget[3];    
     s32 m8C;
     s32 m90_cameraPositionOverride[3]; 
     s32 m9C;
     s32 mA0_desiredCameraPosition[3];  
     s32 mAC;
     s32 mB0_op99Var1;             
     s16 mB4_cameraInterpolationMode; 
     s16 mB6_op99Var2;             
     s16 mB8_op99Var3;             
     s16 mBA_op99Var4;                      
     u16 mBC_cameraInterpolationFlags; 
     s16 mBE_cameraInterpolationTargetNumSteps; 
     s32 mC0_cameraInterpolationTargetStartPosition[3]; 
     s32 mCC;
     s32 mD0_cameraInterpolationTargetStep[3]; 
     s32 mDC;
     s16 mE0_cameraInterpolationPositionNumSteps; 
     s16 mE2;
     s32 mE4_cameraInterpolationStartPosition[3]; 
     s32 mF0;
     s32 mF4_cameraInterpolationPositionStep[3]; 
     s32 m100;
     s32 m104_op99Var5;            
     s32 m108_op99Var6;            
     s16 m10C_camera2Tan;          
     s16 m10E_cameraTan;           
     MATRIX m110_cameraMatrix;     
     MATRIX m130_projectionTempMatrix; 
     SVECTOR m150_projectionAngles; 
     s32 m158_op99Var7;            
     s16 m15C_cameraLimits[4];     
     SVECTOR m164_cameraRotationBetweenEyeAndAt; 
     s32 m16C_cameraDollyVar1;     
     s32 m170_opA0_var1;           
     u8 m174_dollySet;             
     u8 m175_dollyStop;            
     s16 m176_cameraDollyVar0;     
     s32 m178_sceneSCRZ;           
     s16 m17C_sceneDIP;            
     s16 m17E_sceneScale;          
     s16 m180_OP_A4Var0;           
     s16 m182;
     s32 m184_OP_A4Var1;           
     s32 m188_OP_A4Var2;           
     s32 m18C_opA0_var0;           
     s16 m190_OP_B6SubVar0;        
     s16 m192;
     s32 m194_OP_B6Var2;           
     s32 m198_OP_B6Var1;           
     s16 m19C_op9DVar0;            
     s16 m19E;
     s32 m1A0_op9DVar3;            
     s32 m1A4_op9DVar2;            
     s16 m1A8_cameraShakeEnabled;  
     s16 m1AA_cameraShakeCounter;  
     s16 m1AC_cameraShakeStopping;                     
     s16 m1AE;
     s32 m1B0_cameraShakeAmplitude[3]; 
     s32 m1BC_cameraShakeStep[3];  
     s16 m1C8_savedCameraDirection;                     
     s16 m1CA;
     s32 m1CC_savedSceneSCRZ;                     
     s16 m1D0_savedSceneDIP;                     
     s16 m1D2;
     SVECTOR m1D4_projectionAngles; 
     SVECTOR m1DC_cameraRotation;  
     MATRIX m1E4_projectionMatrix; 
     MATRIX m204_worldScaleMatrix; 
     MATRIX m224_projectionTempMatrix; 
     s32 m244_worldScaleFactor;    
     LightDef m248_lights[3];      
     s16 m284_ambient[3];          
     s16 m28A;
     s32 m28C_totalObjects;        
     sFieldEntity* m290_actorArray; 
     s32* m294_models;             
     s32* m298_walkMesh;           
     s32* m29C_actorSetupParams;   
     s32* m2A0_walkMeshVar1;       
     s32* m2A4_walkMeshTriangle[4]; 
     s32* m2B4_walkMeshVertices[4]; 
     s32 m2C4_walkMeshNumTrianglePerBlock[4]; 
     s16 m2D4_numWalkMesh;         
} FieldBss_800AF880;
#endif
#ifndef XM_T_MechaFieldArgs2
#define XM_T_MechaFieldArgs2
typedef MATRIX MechaFieldArgs2;
#endif
#ifndef XM_T_FieldBss_800B2184
#define XM_T_FieldBss_800B2184
typedef struct {
     SVECTOR m0_renderModelRotationAngles; 
     s16 m8_entityMoveSpeedFactor;              
     s16 mA_isFogSetup;              
     u8 mC_fogNearColor[3];            
     u8 mF;
     u8 m10_fogFarColor[3];           
     u8 m13;
     s16 m14_fogNear;             
     s16 m16_fogFar;             
     u8 m18_backgroundClearColor[4];           
     s16 m1C_OPX_0CParams0[3];          
     s16 m22_OPX_0CParams1[3];          
     s16 m28_soundMaxDistance;             
     s16 m2A_objectStepX;             
     s16 m2C_objectStepZ;             
     s16 m2E_objectStepY;             
     s16 m30_spriteTurnSpeed;             
     s16 m32;
     s32 m34_lastSoundEffectId;             
     s32 m38_objectOffsetX;             
     s32 m3C_objectOffsetY;             
     s32 m40_objectOffsetZ;             
     s32 m44;
     u8 m48_noUpdatesToPartyMembers;              
     u8 m49_cameraFollowHeight;              
     u8 m4A_partyPositioned;              
     u8 m4B_partyFollowDisabled;              
     u8 m4C_menuDisabled;              
     u8 m4D_compassDisabled;              
     u8 m4E_objectMoveFlags;              
     u8 m4F;
     s16 m50_linkOTIndex; 
     s16 m52_dialogWindowOpenAnimationNumFrames;             
     s32 m54_cameraSnapFrames;             
     s16 m58_mechaList[4];     
     s16 m60_mechaCurrentAnimation[4]; 
     u8 m68[0x30];
     MechaFieldArgs2 m98_mechaFieldArgs2[2]; 
     u8 mD8_mechaBackColor[3]; 
     u8 mDB;
     s32 mDC;
     s32 mE0_numMechas;             
     s32 mE4_partySpriteSetupBase;             
     s32 mE8_playerControlledActor;             
     s16 mEC_encounterMusicIds[16];         
     s16 m10C_battleMusicId;            
     s16 m10E;
     s32 m110_encounterDataCountdown;            
     s32 m114_encounterTimer;            
     s32 m118_encounterCount;            
     u16 m11C_encounterTriggerTimes[32];        
     s16 m15C_cameraTrackingMode;            
     s16 m15E_lightActorIds[3];         
     SVECTOR m164_lightNearColors[3];       
     SVECTOR m17C_lightFarColors[3];       
     s32 m194_lightRadii[3];           
     SVECTOR m1A0_lightPositions[3];       
     u16 m1B8_keptSoundChannels;            
     u16 m1BA_actorCameraTracked; 
     s16 m1BC_jumpCooldownReset;
     s16 m1BE_jumpCooldown;            
     s16 m1C0_customJumpMode;            
     s16 m1C2_jumpAnimationId; 
     s16 m1C4_warpPartyRequest;            
     s16 m1C6_numMaxMechaOverlayEntries; 
     s16 m1C8_gearSwitchOverride;            
     s16 m1CA_playerIsntPartyLeader;            
     s32 m1CC_savedPlayerScriptFlags;            
     u8 m1D0_reverseDpadDirections;             
     u8 m1D1_randomBattleTransitionEffect;             
     u8 m1D2_fieldBattleTransitionEffect;             
     u8 m1D3_spriteFogDisabled;             
     u8 m1D4_pauseDisabled;             
     u8 m1D5[7];
     s32 m1DC_partyToFollowStructMapping[3]; 
} FieldBss_800B2184;
#endif
#ifndef XM_S_OP_9A
#define XM_S_OP_9A
void xm_OP_9A(void) __asm__("func_8008FC4C");
#endif
#ifndef XM_S_fieldSceneState
#define XM_S_fieldSceneState
extern FieldBss_800AF880 xm_fieldSceneState __asm__("g_CameraEye");
#endif
#ifndef XM_S_pCurrentFieldScriptActor
#define XM_S_pCurrentFieldScriptActor
extern sFieldScriptEntity * xm_pCurrentFieldScriptActor __asm__("g_FieldScriptVMCurActor");
#endif
#ifndef XM_S_getImmediateOrVariableUnsigned
#define XM_S_getImmediateOrVariableUnsigned
s32 xm_getImmediateOrVariableUnsigned(s32) __asm__("FieldScriptVMGetArgument");
#endif
#ifndef XM_S_op9DVar1
#define XM_S_op9DVar1
extern FieldBss_800B2184 xm_op9DVar1 __asm__("D_800B2184");
#endif
void func_8008FC4C(void) {
    s32 steps;

    if (xm_fieldSceneState.mB4_cameraInterpolationMode != 1) {
        if (xm_fieldSceneState.mB4_cameraInterpolationMode >= 2) {
            return;
        }
        if (xm_fieldSceneState.mB4_cameraInterpolationMode != 0) {
            return;
        }
        xm_fieldSceneState.m158_op99Var7 &= 0x7FFF;
        xm_pCurrentFieldScriptActor->mCC_scriptPC += 3;
    } else {
        steps = xm_getImmediateOrVariableUnsigned(1);
        if (steps == 0) {
            xm_fieldSceneState.mB4_cameraInterpolationMode = 0;
            xm_fieldSceneState.m158_op99Var7 &= 0x7FFF;
            xm_pCurrentFieldScriptActor->mCC_scriptPC += 3;
            xm_op9DVar1.m54_cameraSnapFrames = 2;
        } else {
            xm_fieldSceneState.mB4_cameraInterpolationMode = 2;
            xm_fieldSceneState.m104_op99Var5 = steps;
            xm_fieldSceneState.m108_op99Var6 = steps;
        }
        xm_pCurrentFieldScriptActor->mCC_scriptPC += 3;
    }
}
#endif
#else
void func_8008FC4C(void) {
#ifdef XENO_PC_PORT
    ActorData* actor;
#else
    register ActorData* actor asm("$3");
#endif
    s16* cameraMode;
    s16 mode;
    s32 sceneFlags;
    s32 value;

    cameraMode = g_FieldCameraModes;
    mode = *cameraMode;
    if (mode == 1) {
        goto mode1;
    }
    if (mode >= 2) {
        return;
    }
    if (mode != 0) {
        return;
    }

    sceneFlags = *(volatile s32*)&g_Scene.unk48;
    actor = g_FieldScriptVMCurActor;
    *(volatile s32*)&g_Scene.unk48 = sceneFlags & 0x7FFF;
    goto advance;

mode1:
    value = FieldScriptVMGetArgument(1);
    if (value == 0) {
        *cameraMode = 0;
        sceneFlags = *(volatile s32*)&g_Scene.unk48;
        actor = g_FieldScriptVMCurActor;
        *(volatile s32*)&g_Scene.unk48 = sceneFlags & 0x7FFF;
        actor->scriptInstructionPointer += 3;
        D_800B21D8 = 2;
    } else {
        *cameraMode = 2;
        g_CamInterpolation.atStepDistance = value;
        g_CamInterpolation.eyeStepDistance = value;
    }

    actor = g_FieldScriptVMCurActor;
advance:
    actor->scriptInstructionPointer += 3;
}
#endif /* XENO_PC_PORT */

void FieldScriptSetCameraInterpolationStep(void) {
    g_CamInterpolation.atStepDistance = FieldScriptVMGetArgument(1);
    g_CamInterpolation.eyeStepDistance = FieldScriptVMGetArgument(3);
    
    if (g_CamInterpolation.atStepDistance == 0) {
        g_CamInterpolation.atStepDistance = 1;
    }
    if (g_CamInterpolation.eyeStepDistance == 0) {
        g_CamInterpolation.eyeStepDistance = 1;
    }
    g_FieldScriptMaxInstructionCount += 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

// Reset initial camera target movement to current camera target
void FieldScriptResetCameraTargetMovement(void) {
    g_CamAtMovementFrom.vx = g_CameraAt2.vx;
    g_CamAtMovementFrom.vy = g_CameraAt2.vy;
    g_CamAtMovementFrom.vz = g_CameraAt2.vz;
    g_FieldScriptMaxInstructionCount++;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void FieldScriptSetCameraTargetMovementFrom(void) {
    g_CamAtMovementFrom.vx = CONV_FROM_GTE(FieldScriptArgument1(ARG(1), SCRIPT_READ_U8_REL(7)));
    g_CamAtMovementFrom.vz = CONV_FROM_GTE(FieldScriptArgument2(ARG(2), SCRIPT_READ_U8_REL(7))); 
    g_CamAtMovementFrom.vy = CONV_FROM_GTE(FieldScriptArgument3(ARG(3), SCRIPT_READ_U8_REL(7)));
    g_FieldScriptMaxInstructionCount += 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 8;
}

// Set initial camera target movement to position of an actor
void FieldScriptSetCameraTargetMovementDestToActor(void) {
    ActorData* pActor = (ActorData*)(uintptr_t)g_FieldActors[func_8009CD7C(1)].pActorData;
    g_CamAtMovementTo.vx = pActor->position.vx;
    g_CamAtMovementTo.vy = pActor->position.vy;
    g_CamAtMovementTo.vz = pActor->position.vz;  
    g_FieldScriptMaxInstructionCount += 1;    
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void FieldScriptSetCameraTargetMovementDest(void) {
    g_CamAtMovementTo.vx = CONV_FROM_GTE(FieldScriptArgument1(ARG(1), SCRIPT_READ_U8_REL(7)));
    g_CamAtMovementTo.vz = CONV_FROM_GTE(FieldScriptArgument2(ARG(2), SCRIPT_READ_U8_REL(7)));
    g_CamAtMovementTo.vy = CONV_FROM_GTE(FieldScriptArgument3(ARG(3), SCRIPT_READ_U8_REL(7)));
    g_FieldScriptMaxInstructionCount += 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 8;
}

// Reset initial camera position movement to current camera position
void FieldScriptResetCameraPosMovement(void) {
    g_CamEyeMovementFrom.vx = g_CameraEye2.vx;
    g_CamEyeMovementFrom.vy = g_CameraEye2.vy;
    g_CamEyeMovementFrom.vz = g_CameraEye2.vz;
    g_FieldScriptMaxInstructionCount += 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

void FieldScriptSetCameraPosMovementFrom(void) {
    g_CamEyeMovementFrom.vx = CONV_FROM_GTE(FieldScriptArgument1(ARG(1), SCRIPT_READ_U8_REL(7)));
    g_CamEyeMovementFrom.vz = CONV_FROM_GTE(FieldScriptArgument2(ARG(2), SCRIPT_READ_U8_REL(7))); 
    g_CamEyeMovementFrom.vy = CONV_FROM_GTE(FieldScriptArgument3(ARG(3), SCRIPT_READ_U8_REL(7)));
    g_FieldScriptMaxInstructionCount += 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 8;
}

void FieldScriptSetCameraPosMovementDestToActor(void) {
    ActorData* pActor = (ActorData*)(uintptr_t)g_FieldActors[func_8009CD7C(1)].pActorData;
    g_CamEyeMovementTo.vx = pActor->position.vx;
    g_CamEyeMovementTo.vy = pActor->position.vy;
    g_CamEyeMovementTo.vz = pActor->position.vz;  
    g_FieldScriptMaxInstructionCount += 1;    
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void FieldScriptSetCameraPosMovementDest(void) {
    g_CamEyeMovementTo.vx = CONV_FROM_GTE(FieldScriptArgument1(ARG(1), SCRIPT_READ_U8_REL(7)));
    g_CamEyeMovementTo.vz = CONV_FROM_GTE(FieldScriptArgument2(ARG(2), SCRIPT_READ_U8_REL(7)));
    g_CamEyeMovementTo.vy = CONV_FROM_GTE(FieldScriptArgument3(ARG(3), SCRIPT_READ_U8_REL(7)));
    g_FieldScriptMaxInstructionCount += 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 8;
}

void FieldScriptResetCameraMovements(void) {
    g_CamAtMovementFrom.vx = g_CameraAt2.vx;
    g_CamAtMovementFrom.vy = g_CameraAt2.vy;
    g_CamAtMovementFrom.vz = g_CameraAt2.vz;
    
    g_CamAtMovementTo.vx = g_CameraAt2.vx;
    g_CamAtMovementTo.vy = g_CameraAt2.vy;
    g_CamAtMovementTo.vz = g_CameraAt2.vz;

    g_CamEyeMovementFrom.vx = g_CameraEye2.vx;
    g_CamEyeMovementFrom.vy = g_CameraEye2.vy;
    g_CamEyeMovementFrom.vz = g_CameraEye2.vz;
    
    g_CamEyeMovementTo.vx = g_CameraEye2.vx;
    g_CamEyeMovementTo.vy = g_CameraEye2.vy;
    g_CamEyeMovementTo.vz = g_CameraEye2.vz;
    
    g_FieldScriptMaxInstructionCount += 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/camera/camera_movement", FieldScriptStartCameraMovement);
#else
void FieldScriptStartCameraMovement(void) {
    s32 duration;
    s32 speed;
    s32 distance;
    s32 deltaX;
    s32 deltaY;
    s32 deltaZ;
    s32 fromX;
    s32 fromY;
    s32 fromZ;
    VECTOR direction;
    VECTOR normal;

    switch (SCRIPT_READ_U8_REL(1) & 0xF) {
    // Linearly move the camera target over a fixed number of frames.
    case 0:
        deltaZ = FieldScriptVMGetArgument(2);
        g_CamAtMovementDuration = deltaZ;
        if ((deltaZ << 16) == 0) {
            g_CamAtMovementDuration = deltaZ + 1;
            g_CamInterpolation.atStepDistance = 1;
        }

        deltaX = (g_CamAtMovementTo.vx - g_CamAtMovementFrom.vx) / g_CamAtMovementDuration;
        deltaY = (g_CamAtMovementTo.vy - g_CamAtMovementFrom.vy) / g_CamAtMovementDuration;
        deltaZ = (g_CamAtMovementTo.vz - g_CamAtMovementFrom.vz) / g_CamAtMovementDuration;
        fromX = g_CamAtMovementFrom.vx;
        fromY = g_CamAtMovementFrom.vy;
        fromZ = g_CamAtMovementFrom.vz;
        g_CamAtMovementCurrent.vx = fromX;
        g_CamAtMovementCurrent.vy = fromY;
        g_CamAtMovementCurrent.vz = fromZ;
        g_CamMovementFlags |= FIELD_CAMERA_AT_MOVEMENT_ACTIVE;
        g_CamAtMovementDelta.vx = deltaX;
        g_CamAtMovementDelta.vy = deltaY;
        g_CamAtMovementDelta.vz = deltaZ;

        if (SCRIPT_READ_U8_REL(1) & 0x80) {
            g_CameraAt.vx = g_CamAtMovementFrom.vx;
            g_CameraAt.vy = g_CamAtMovementFrom.vy;
            g_CameraAt.vz = g_CamAtMovementFrom.vz;
        }
        break;

    // Move the camera target toward its destination at a fixed speed.
    case 2:
        direction.vx = (g_CamAtMovementFrom.vx - g_CamAtMovementTo.vx) >> 16;
        direction.vy = (g_CamAtMovementFrom.vy - g_CamAtMovementTo.vy) >> 16;
        direction.vz = (g_CamAtMovementFrom.vz - g_CamAtMovementTo.vz) >> 16;
        VectorNormal(&direction, &normal);

        distance = FieldGetVec3Magnitude(
            (g_CamAtMovementFrom.vx - g_CamAtMovementTo.vx) >> 16,
            (g_CamAtMovementFrom.vy - g_CamAtMovementTo.vy) >> 16,
            (g_CamAtMovementFrom.vz - g_CamAtMovementTo.vz) >> 16);
        speed = FieldScriptVMGetArgument(2);

        duration = distance / speed;
        deltaX = normal.vx * speed;
        deltaY = normal.vy * speed;
        deltaZ = normal.vz * speed;
        g_CamAtMovementCurrent.vx = g_CamAtMovementFrom.vx;
        g_CamAtMovementCurrent.vy = g_CamAtMovementFrom.vy;
        g_CamAtMovementCurrent.vz = g_CamAtMovementFrom.vz;
        g_CamMovementFlags |= FIELD_CAMERA_AT_MOVEMENT_ACTIVE;
        g_CamAtMovementDelta.vx = -deltaX << 4;
        g_CamAtMovementDelta.vy = -deltaY << 4;
        g_CamAtMovementDelta.vz = -deltaZ << 4;
        g_CamAtMovementDuration = duration;

        if (SCRIPT_READ_U8_REL(1) & 0x80) {
            g_CameraAt.vx = g_CamAtMovementFrom.vx;
            g_CameraAt.vy = g_CamAtMovementFrom.vy;
            g_CameraAt.vz = g_CamAtMovementFrom.vz;
        }
        break;

    // Move the camera eye toward its destination at a fixed speed.
    case 3:
        direction.vx = (g_CamEyeMovementFrom.vx - g_CamEyeMovementTo.vx) >> 16;
        direction.vy = (g_CamEyeMovementFrom.vy - g_CamEyeMovementTo.vy) >> 16;
        direction.vz = (g_CamEyeMovementFrom.vz - g_CamEyeMovementTo.vz) >> 16;
        VectorNormal(&direction, &normal);

        distance = FieldGetVec3Magnitude(
            (g_CamEyeMovementFrom.vx - g_CamEyeMovementTo.vx) >> 16,
            (g_CamEyeMovementFrom.vy - g_CamEyeMovementTo.vy) >> 16,
            (g_CamEyeMovementFrom.vz - g_CamEyeMovementTo.vz) >> 16);
        speed = FieldScriptVMGetArgument(2);

        duration = distance / speed;
        deltaX = normal.vx * speed;
        deltaY = normal.vy * speed;
        deltaZ = normal.vz * speed;
        g_CamEyeMovementCurrent.vx = g_CamEyeMovementFrom.vx;
        g_CamEyeMovementCurrent.vy = g_CamEyeMovementFrom.vy;
        g_CamEyeMovementCurrent.vz = g_CamEyeMovementFrom.vz;
        g_CamMovementFlags |= FIELD_CAMERA_EYE_MOVEMENT_ACTIVE;
        g_CamEyeMovementDelta.vx = -deltaX << 4;
        g_CamEyeMovementDelta.vy = -deltaY << 4;
        g_CamEyeMovementDelta.vz = -deltaZ << 4;
        g_CamEyeMovementDuration = duration;

        if (SCRIPT_READ_U8_REL(1) & 0x80) {
            g_CameraEye.vx = g_CamEyeMovementFrom.vx;
            g_CameraEye.vy = g_CamEyeMovementFrom.vy;
            g_CameraEye.vz = g_CamEyeMovementFrom.vz;
        }
        break;

    // Linearly move the camera eye over a fixed number of frames.
    case 1:
        duration = FieldScriptVMGetArgument(2);
        g_CamEyeMovementDuration = duration;
        if ((duration << 16) == 0) {
            g_CamEyeMovementDuration = duration + 1;
            g_CamInterpolation.eyeStepDistance = 1;
        }

        deltaX = (g_CamEyeMovementTo.vx - g_CamEyeMovementFrom.vx) / g_CamEyeMovementDuration;
        deltaY = (g_CamEyeMovementTo.vy - g_CamEyeMovementFrom.vy) / g_CamEyeMovementDuration;
        deltaZ = (g_CamEyeMovementTo.vz - g_CamEyeMovementFrom.vz) / g_CamEyeMovementDuration;
        g_CamEyeMovementCurrent.vx = g_CamEyeMovementFrom.vx;
        g_CamEyeMovementCurrent.vy = g_CamEyeMovementFrom.vy;
        g_CamEyeMovementCurrent.vz = g_CamEyeMovementFrom.vz;
        g_CamMovementFlags |= FIELD_CAMERA_EYE_MOVEMENT_ACTIVE;
        g_CamEyeMovementDelta.vx = deltaX;
        g_CamEyeMovementDelta.vy = deltaY;
        g_CamEyeMovementDelta.vz = deltaZ;

        if (SCRIPT_READ_U8_REL(1) & 0x80) {
            g_CameraEye.vx = g_CamEyeMovementFrom.vx;
            g_CameraEye.vy = g_CamEyeMovementFrom.vy;
            g_CameraEye.vz = g_CamEyeMovementFrom.vz;
        }
        break;
    }

    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}
#endif /* XENO_PC_PORT */

// Write the current interpolated value
void FieldScriptWriteCurCameraTarget(void) {
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), CONV_TO_GTE(g_CameraAt.vx));
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(2), CONV_TO_GTE(g_CameraAt.vz));
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(3), CONV_TO_GTE(g_CameraAt.vy));
    g_FieldScriptMaxInstructionCount++;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
}

// Write the current interpolated value
void FieldScriptWriteCurCameraPosition(void) {
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), CONV_TO_GTE(g_CameraEye.vx));
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(2), CONV_TO_GTE(g_CameraEye.vz));
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(3), CONV_TO_GTE(g_CameraEye.vy));
    g_FieldScriptMaxInstructionCount++;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
}

// Write the target / desired value
void FieldScriptWriteCameraTweenTarget(void) {
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), CONV_TO_GTE(g_CameraAt2.vx));
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(2), CONV_TO_GTE(g_CameraAt2.vz));
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(3), CONV_TO_GTE(g_CameraAt2.vy));
    g_FieldScriptMaxInstructionCount++;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
}

// Write the target / desired value
void FieldScriptWriteCameraTweenPosition(void) {
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), CONV_TO_GTE(g_CameraEye2.vx));
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(2), CONV_TO_GTE(g_CameraEye2.vz));
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(3), CONV_TO_GTE(g_CameraEye2.vy));
    g_FieldScriptMaxInstructionCount++;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
}

/* Camera mode 1 exposes its yaw, pitch, and distance through script
 * variables.  A zero type byte means read the live value into the supplied
 * variable; any nonzero type byte writes the supplied immediate value back
 * into the camera-mode state. */
void func_80090C20(void) {
    if (SCRIPT_READ_U8_REL(3) == 0) {
        FieldScriptMemoryWriteU16(
            FieldScriptVMGetInstructionArgument(1) & 0xFFFF,
            D_800AF938[0]);
    } else {
        D_800AF938[0] = FieldScriptVMGetInstructionArgument(1);
    }

    g_FieldScriptMaxInstructionCount++;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

void func_80090CB8(void) {
    if (SCRIPT_READ_U8_REL(3) == 0) {
        FieldScriptMemoryWriteU16(
            FieldScriptVMGetInstructionArgument(1) & 0xFFFF,
            D_800AF936[0]);
    } else {
        D_800AF936[0] = FieldScriptVMGetInstructionArgument(1);
    }

    g_FieldScriptMaxInstructionCount++;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

void func_80090D50(void) {
    if (SCRIPT_READ_U8_REL(3) == 0) {
        FieldScriptMemoryWriteU16(
            FieldScriptVMGetInstructionArgument(1) & 0xFFFF,
            D_800AF930[0]);
    } else {
        D_800AF930[0] = FieldScriptVMGetInstructionArgument(1) & 0xFFFF;
    }

    g_FieldScriptMaxInstructionCount++;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

void func_80090DEC(void) {
    FieldScriptMemoryWriteU16(FieldScriptVMGetInstructionArgument(1) & 0xFFFF, D_800AF938[0]);
    FieldScriptMemoryWriteU16(FieldScriptVMGetInstructionArgument(3) & 0xFFFF, D_800AF936[0]);
    FieldScriptMemoryWriteU16(FieldScriptVMGetInstructionArgument(5) & 0xFFFF, D_800AF930[0]);
    g_FieldScriptMaxInstructionCount++;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
}

void func_80090E70(void) {
    VECTOR camAtDest;
    VECTOR camEyeDest;
    s32 yaw;
    s32 pitch;
    s32 halfDistance;
    long distance;
    u16 yawAngle;
    int pitchAngle;

    camAtDest.vx = g_CamAtMovementTo.vx;
    camAtDest.vy = g_CamAtMovementTo.vy;
    camAtDest.vz = g_CamAtMovementTo.vz;
    camEyeDest.vx = g_CamEyeMovementTo.vx;
    camEyeDest.vy = g_CamEyeMovementTo.vy;
    camEyeDest.vz = g_CamEyeMovementTo.vz;
    
    // Compute the half distance between the destination camera position and destination
    // camera target of the camera movement
    distance = FieldGetVec3Magnitude(
        CONV_TO_GTE(g_CamEyeMovementTo.vx - g_CamAtMovementTo.vx), 
        CONV_TO_GTE(g_CamEyeMovementTo.vy - g_CamAtMovementTo.vy), 
        CONV_TO_GTE(g_CamEyeMovementTo.vz - g_CamAtMovementTo.vz)
    );
    halfDistance = distance / 2;
    
    D_800AF93A[0] = 0x1000;

    // Cursed angle math
    yawAngle = ratan2(camAtDest.vz - camEyeDest.vz, camAtDest.vx - camEyeDest.vx);
    yaw = PSX_ANGLE((-yawAngle & 0xFFFF) - PSX_DEGREES(90));

    pitchAngle = -ratan2(
        FieldGetVec2Magnitude(
            CONV_TO_GTE(camEyeDest.vx - camAtDest.vx), 
            CONV_TO_GTE(camEyeDest.vz - camAtDest.vz)
        ), 
        CONV_TO_GTE(camAtDest.vy - camEyeDest.vy)
    );
    pitch = ((pitchAngle * 360) >> 12) + 91;
    
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), yaw);
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(2), pitch);
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(3), halfDistance);
    
    g_FieldScriptMaxInstructionCount += 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
}

void func_80091008(VECTOR* destination, VECTOR* origin, s32 angle) {
    MATRIX rotationMatrix;
    VECTOR offset;
    VECTOR rotatedOffset;
    SVECTOR rotation;

    rotation.vx = 0;
    rotation.vy = angle;
    rotation.vz = 0;
    PushMatrix();
    RotMatrix(&rotation, &rotationMatrix);

    offset.vx = origin->vx - destination->vx;
    offset.vy = origin->vy - destination->vy;
    offset.vz = origin->vz - destination->vz;
    ApplyMatrixLV(&rotationMatrix, &offset, &rotatedOffset);

    destination->vx = rotatedOffset.vx + origin->vx;
    destination->vz = rotatedOffset.vz + origin->vz;
    PopMatrix();
}

void func_800910C0(void) {
    VECTOR parameter;
    VECTOR vecResult;
    s32 angle;
    s32 temp_s1;
    s32 factor;
    s32 yAngle;

    parameter.vx = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(0xD)) << 16;
    parameter.vz = FieldScriptArgument2(3, SCRIPT_READ_U8_REL(0xD)) << 16;
    parameter.vy = FieldScriptArgument3(5, SCRIPT_READ_U8_REL(0xD)) << 16;
    yAngle = FieldScriptArgument4(7, SCRIPT_READ_U8_REL(0xD));
    temp_s1 = FieldScriptArgument5(9, SCRIPT_READ_U8_REL(0xD));
    factor = FieldScriptArgument6(0xB, SCRIPT_READ_U8_REL(0xD));

    angle = ((temp_s1 * 0xB60) >> 8) + 0xC00;
    vecResult.vy = ((-((rsin(angle) * factor) << 5) >> 16) * D_800AF93A[0] * 16) + parameter.vy;
    vecResult.vz = ((((rcos(angle) * factor) << 5) >> 16) * D_800AF93A[0] * 16) + parameter.vz;
    vecResult.vx = parameter.vx;

    func_80091008(&vecResult, &parameter, yAngle);

    FieldScriptMemoryWriteU16(FieldScriptVMGetInstructionArgument(0xE) & 0xFFFF, vecResult.vx >> 16);
    FieldScriptMemoryWriteU16(FieldScriptVMGetInstructionArgument(0x10) & 0xFFFF, vecResult.vz >> 16);
    FieldScriptMemoryWriteU16(FieldScriptVMGetInstructionArgument(0x12) & 0xFFFF, vecResult.vy >> 16);

    g_FieldScriptMaxInstructionCount += 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0x14;
}

void func_80091318(void) {
    VECTOR parameter;
    VECTOR vecResult;
    s32 angle;
    s32 temp_s1;
    s32 factor;
    s32 yAngle;
    u8 parameterType;

    parameterType = SCRIPT_READ_U8_REL(1);
    switch (parameterType) {
    case 0:
        parameter.vx = g_CamAtMovementFrom.vx;
        parameter.vy = g_CamAtMovementFrom.vy;
        parameter.vz = g_CamAtMovementFrom.vz;
        break;
    case 1:
        parameter.vx = g_CamAtMovementTo.vx;
        parameter.vy = g_CamAtMovementTo.vy;
        parameter.vz = g_CamAtMovementTo.vz;
        break;
    case 2:
        parameter.vx = g_CamEyeMovementFrom.vx;
        parameter.vy = g_CamEyeMovementFrom.vy;
        parameter.vz = g_CamEyeMovementFrom.vz;
        break;
    case 3:
        parameter.vx = g_CamEyeMovementTo.vx;
        parameter.vy = g_CamEyeMovementTo.vy;
        parameter.vz = g_CamEyeMovementTo.vz;
        break;
    }
    
    yAngle = FieldScriptArgument1(2, SCRIPT_READ_U8_REL(8));
    temp_s1 = FieldScriptArgument2(4, SCRIPT_READ_U8_REL(8));
    factor = FieldScriptArgument3(6, SCRIPT_READ_U8_REL(8));
    
    // 0xC00 = 270 degrees
    angle = ((temp_s1 * 0xB60) >> 8) + 0xC00;

    // (number << 5) >> 0x10 is division by 0x800 = 180 degrees
    vecResult.vy = ((-((rsin(angle) * factor) << 5) >> 0x10) * D_800AF93A[0] * 0x10) + parameter.vy;
    vecResult.vz = ((((rcos(angle) * factor) << 5) >> 0x10) * D_800AF93A[0] * 0x10) + parameter.vz;
    vecResult.vx = parameter.vx;

    // Rotate difference between parameter value and the result vector by Y angle,
    // then we add it to parameter and set our result vector to it
    func_80091008(&vecResult, &parameter, yAngle);
    
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(5), vecResult.vx >> 0x10);
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(6), vecResult.vz >> 0x10);
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(7), vecResult.vy >> 0x10);
    
    g_FieldScriptMaxInstructionCount += 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 15;
}

void FieldScriptWriteCameraMovementParameter(void) {
    int mode;
    VECTOR movementParam;

    mode = SCRIPT_READ_U8_REL(1);
    switch (mode) { 
        case 0:
            movementParam.vx  = g_CamAtMovementFrom.vx;
            movementParam.vy = g_CamAtMovementFrom.vy;
            movementParam.vz = g_CamAtMovementFrom.vz;
            break;
        case 1:
            movementParam.vx  = g_CamAtMovementTo.vx;
            movementParam.vy = g_CamAtMovementTo.vy;
            movementParam.vz = g_CamAtMovementTo.vz;
            break;
        case 2:
            movementParam.vx  = g_CamEyeMovementFrom.vx;
            movementParam.vy = g_CamEyeMovementFrom.vy;
            movementParam.vz = g_CamEyeMovementFrom.vz;
            break;
        case 3:
            movementParam.vx  = g_CamEyeMovementTo.vx;
            movementParam.vy = g_CamEyeMovementTo.vy;
            movementParam.vz = g_CamEyeMovementTo.vz;
            break;
    }
    
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG_ALIGNED(1), CONV_TO_GTE(movementParam.vx));
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG_ALIGNED(2), CONV_TO_GTE(movementParam.vz));
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG_ALIGNED(3), CONV_TO_GTE(movementParam.vy));

    g_FieldScriptMaxInstructionCount += 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 8;
}

void FieldScriptSetCameraMovementParameter(void) {
    VECTOR parameter;
    u8 srcParameter;
    u8 destParameter;

    srcParameter = SCRIPT_READ_U8_REL(1);
    switch (srcParameter) {
    case 0:
        parameter.vx = g_CamAtMovementFrom.vx;
        parameter.vy = g_CamAtMovementFrom.vy;
        parameter.vz = g_CamAtMovementFrom.vz;
        break;
    case 1:
        parameter.vx = g_CamAtMovementTo.vx;
        parameter.vy = g_CamAtMovementTo.vy;
        parameter.vz = g_CamAtMovementTo.vz;
        break;
    case 2:
        parameter.vx = g_CamEyeMovementFrom.vx;
        parameter.vy = g_CamEyeMovementFrom.vy;
        parameter.vz = g_CamEyeMovementFrom.vz;
        break;
    case 3:
        parameter.vx = g_CamEyeMovementTo.vx;
        parameter.vy = g_CamEyeMovementTo.vy;
        parameter.vz = g_CamEyeMovementTo.vz;
        break;
    }
    
    destParameter = SCRIPT_READ_U8_REL(2);
    switch (destParameter) {
    case 0:
        g_CamAtMovementFrom.vx = parameter.vx;
        g_CamAtMovementFrom.vy = parameter.vy;
        g_CamAtMovementFrom.vz = parameter.vz;
        break;
    case 1:
        g_CamAtMovementTo.vx = parameter.vx;
        g_CamAtMovementTo.vy = parameter.vy;
        g_CamAtMovementTo.vz = parameter.vz;
        break;
    case 2:
        g_CamEyeMovementFrom.vx = parameter.vx;
        g_CamEyeMovementFrom.vy = parameter.vy;
        g_CamEyeMovementFrom.vz = parameter.vz;
        break;
    case 3:
        g_CamEyeMovementTo.vx = parameter.vx;
        g_CamEyeMovementTo.vy = parameter.vy;
        g_CamEyeMovementTo.vz = parameter.vz;
        break;
    }
    
    g_FieldScriptMaxInstructionCount += 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}
