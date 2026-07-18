/* data_member_change_menu.c -- migrated member_change_menu overlay .data
 * (Xenogears PC port).  Same pattern/rationale as data_field.c: overlay
 * .data symbols are auto-generated as ZEROED stubs unless defined here.
 * These are the member-change menu's LAYOUT tables (window size, cursor
 * and character slot positions, texcoords) -- zeroed => a 0x0 window with
 * everything at position 0 => nothing renders.  Values transcribed
 * verbatim from build/out/member_change_menu.bin .data (file 0x6180+),
 * VRAM base 0x801C5000.  Defining them removes them from the zeroed
 * data-stub set so the menu gets real layout instead of zeros. */

int D_801CB180[4] = { 0, 0, 180, 276 };
int D_801CB190[4] = { 0, 0, 200, 200 };
int g_MemberChangeMenuCurserPositionsX[9] = { 32, 32, 32, 144, 144, 144, 144, 144, 144 };
int g_MemberChangeMenuCurserPositionsY[9] = { 38, 94, 150, 22, 54, 86, 118, 150, 182 };
int g_MemberChangeMenuBenchedCharPositionsX[17] = { 152, 160, 192, 224, 232, 264, 224, 232, 264, 126, 168, 200, 240, 272, 248, 272, 152 };
int g_MemberChangeMenuCurCharPositionsX[18] = { 40, 48, 80, 40, 48, 80, 40, 48, 80, 14, 56, 88, 56, 88, 64, 88, 40, 64 };
int g_MemberChangeMenuBenchedCharPositionsY[17] = { 14, 14, 14, 22, 22, 22, 30, 30, 30, 14, 14, 14, 22, 22, 30, 30, 24 };
int g_MemberChangeMenuCurCharPositionsY[35] = { 30, 30, 30, 54, 54, 54, 62, 62, 62, 30, 30, 30, 54, 54, 62, 62, 40, 22, 30, 30, 30, 54, 54, 54, 62, 62, 62, 22, 30, 30, 54, 54, 62, 62, 40 };
int g_MemberChangeMenuCharTexcoordsU[19] = { 24, 0, 24, 0, 24, 0, 24, 0, 24, 0, 24, 0, 24, 0, 24, 0, 24, 0, 24 };
int g_MemberChangeMenuCharTexcoordsV[19] = { 59, 72, 72, 85, 85, 98, 98, 111, 111, 124, 124, 137, 137, 150, 150, 163, 163, 176, 176 };
int D_801CB3DC[9] = { 21, 31, 65535, 17, 25, 62, 22, 25, 62 };
unsigned char D_801CB400[4] = { 9, 10, 11, 12 };
int D_801CB404[30] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30 };
int D_801CB47C[32] = { 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120, 128, 136, 144, 320, 176, 184, 192, 200, 208, 216, 224, 232, 240, 248, 256, 264, 272, 280, 288, 320 };
int D_801CB4FC[32] = { 14, 34, 54, 14, 34, 54, 14, 34, 54, 14, 34, 54, 14, 34, 54, 256, 14, 34, 54, 14, 34, 54, 14, 34, 54, 14, 34, 54, 14, 34, 54, 256 };
unsigned short D_801CB57C[16] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768 };
