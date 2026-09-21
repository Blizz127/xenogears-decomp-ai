void func_8007254C(void) {
    /* g_CamInterpolation fields */
    g_CamInterpolation.atStepDistance = 8;
    g_CamInterpolation.eyeStepDistance = 8;
    g_CamInterpolation.targetAngleY = 0;
    g_CamInterpolation.curAngleY = 0;

    /* g_Scene defaults */
    *(s32*)((u8*)&g_Scene + 0x5C) = 0x400000;
    *(s32*)((u8*)&g_Scene + 0x60) = 0x08000000;
    *(s16*)((u8*)&g_Scene + 0x56) = 0x800;
    *(s32*)((u8*)&g_Scene + 0xA8) = 0;
    *(s32*)((u8*)&g_Scene + 0xA4) = 0;
    *(s32*)((u8*)&g_Scene + 0xA0) = 0;
    *(s32*)((u8*)&g_Scene + 0xB4) = 0;
    *(s32*)((u8*)&g_Scene + 0xB0) = 0;
    *(s32*)((u8*)&g_Scene + 0xAC) = 0;
    *(s16*)((u8*)&g_Scene + 0x98) = 0;
    *(s16*)((u8*)&g_Scene + 0x9C) = 0;
    *(s32*)((u8*)&g_Scene + 0x48) = 0;
    *(s8*)((u8*)&g_Scene + 0x64) = 0;
    *(s8*)((u8*)&g_Scene + 0x65) = 0;
    *(s16*)((u8*)&g_Scene + 0x66) = 0;
    *(s16*)((u8*)&g_Scene + 0x54) = 0;
    *(s16*)((u8*)&g_Scene + 0x58) = 0;
    *(s32*)((u8*)&g_Scene + 0x7C) = 0x800;
    *(s16*)((u8*)&g_Scene + 0x40) = 0;
    *(s16*)((u8*)&g_Scene + 0x42) = 0;
    *(s16*)((u8*)&g_Scene + 0x44) = 0;

    /* Camera vectors — asm 80072640-800726B0, $s0 = &g_CamInterpolation
     * (0x800AF984). Negative-offset decode: -0x104 eye.vx, -0xF4..-0xEC at,
     * -0xE4/-0xE0/-0xDC up, -0xD4..-0xCC eye2, -0xC4..-0xBC at2,
     * -0xB4/-0xB0/-0xAC D_800AF8D0, -0xA4..-0x9C D_800AF8E0 shake. */
    g_CameraEye.vx = 0;  g_CameraEye.vy = 0;  g_CameraEye.vz = 0;
    g_CameraAt.vx = 0;   g_CameraAt.vy = 0;   g_CameraAt.vz = 0;
    g_CameraUp.vx = 0;
    g_CameraUp.vy = 0x10000000;  /* asm 2B68: sw $v0, -0xE0($s0) = 0x800AF8A4 */
    g_CameraUp.vz = 0;
    g_CameraEye2.vx = 0;
    g_CameraEye2.vy = 0;
    g_CameraEye2.vz = 0;
    *(s32*)((u8*)&g_Scene + 0x68) = 0x200;
    *(s16*)((u8*)&g_Scene + 0x6C) = 0x1E;
    *(s16*)((u8*)&g_Scene + 0x6E) = 0x1000;
    g_CameraAt2.vx = 0;
    g_CameraAt2.vy = 0;
    g_CameraAt2.vz = 0;
    D_800AF8D0.vx = 0;
    D_800AF8D0.vy = 0x10000000;  /* asm 2B6C: sw $v0, -0xB0($s0) = 0x800AF8D4 */
    D_800AF8D0.vz = 0;
    D_800AF8E0 = 0;  /* asm 2B98-2BA0: shake offsets zeroed */
    D_800AF8E4 = 0;
    D_800AF8E8 = 0;

    /* XENO_PC_PORT TODO: func_80070594 writes a matrix at an unnamed
     * camera work variable (PSX 0x800AF9B0). Skip until the symbol is
     * resolved — it initializes camera transform state not needed until
     * the camera update functions run. */
}
