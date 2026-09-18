s32 func_801DE5CC(s32 selectedArg, s32 pageArg, s32 categoryArg, s32 groupArg,
                  s32 gearModeArg) {
    u8 selected = (u8)selectedArg;
    s32 page = pageArg;
    s32 category = categoryArg;
    u8 group = (u8)groupArg;
    u8 gearMode = (u8)gearModeArg;
    MenuManager* manager = g_Menu->pManager;
    u8* work = MenuRawPointer(0x360);
    u8* state = (u8*)&g_GameState;
    u8* resources = (u8*)g_Menu->unk330;
    u8* listBuf;
    u8 type;
    s32 limit;
    u8* equippedWeapon = NULL;
    u8* equippedGearWeapon = NULL;
    u8* equippedAccessory = NULL;
    u16 slotMask = 0;
    u16 otherMask = 0;
    s32 filled;
    s32 i;
    s32 isAccessory;
    u32 rawPtr;
    u8* weapons;
    u8* accessories;
    u8* gearAccessories;
    u8* gearWeapons;
    listBuf = MenuRawPointer(0x434);
    memcpy(&rawPtr, resources + 0x0, 4);
    weapons = (u8*)(uintptr_t)rawPtr;
    memcpy(&rawPtr, resources + 0x4, 4);
    accessories = (u8*)(uintptr_t)rawPtr;
    memcpy(&rawPtr, resources + 0x14, 4);
    gearAccessories = (u8*)(uintptr_t)rawPtr;
    memcpy(&rawPtr, resources + 0x18, 4);
    gearWeapons = (u8*)(uintptr_t)rawPtr;

    if (group) {
        type = (u8)(category + 1);
        limit = 100;
        if (!gearMode) {
            u8 id = work[0x29C + category];
            equippedWeapon = weapons + (id << 4);
        } else {
            u8 id = work[0x29C + category];
            equippedGearWeapon = gearWeapons + id * 20;
        }
    } else if (!gearMode) {
        if (category) {
            type = 5;
            limit = 200;
            equippedAccessory = accessories + (work[0x2A5 + category] << 4);
            slotMask = *(u16*)(equippedAccessory + 0xE);
            otherMask = 0;
            for (i = 0; i < 3; i++) {
                u8* entry = accessories + (work[0x2A6 + i] << 4);
                otherMask |= *(u16*)(entry + 0xE);
            }
        } else {
            type = 0;
            limit = 100;
        }
    } else if (category) {
        type = 5;
        limit = 150;
        equippedAccessory = gearAccessories + work[0x2A5 + category] * 28;
        slotMask = *(u16*)(equippedAccessory + 8);
        otherMask = 0;
        for (i = 0; i < 3; i++) {
            u8* entry = gearAccessories + work[0x2A6 + i] * 28;
            otherMask |= *(u16*)(entry + 8);
        }
    } else {
        type = 0;
        limit = 100;
    }

    for (i = 0; i < 0x190; i++) D_801EA730[i] = 0;

    isAccessory = type == 5;
    filled = isAccessory;
    if (limit) {
        u8* charWeaponIds = state + 0x1D9C;
        u8* charWeaponCounts = state + 0x1D38;
        u8* charAccIds = state + 0x1EC8;
        u8* charAccCounts = state + 0x1E00;
        u8* gearWeaponIds = state + 0x2120;
        u8* gearWeaponCounts = state + 0x20BC;
        u8* gearAccIds = state + 0x221A;
        u8* gearAccCounts = state + 0x2184;
        u8* outIds = D_801EA730 + filled;
        u8* outCounts = D_801EA7F8 + filled;

        for (i = 0; i < limit; i++) {
            u8 accept = 0;
            u8 charId = manager->currentCharacterIDs[selected];
            if (!gearMode) {
                if (type < 5) {
                    u8* weapon = weapons + (charWeaponIds[i] << 4);
                    if (type == 0) {
                        if (func_801C865C(*(u16*)weapon, charId) &&
                            weapon[6] < 5 && charWeaponIds[i] < 0x32) {
                            accept = 1;
                        }
                    } else if (func_801C865C(*(u16*)weapon, charId) &&
                               weapon[6] == equippedWeapon[6] &&
                               charWeaponIds[i] >= 0x32) {
                        accept = 1;
                    }
                } else {
                    u8* item = accessories + (charAccIds[i] << 4);
                    if (func_801C865C(*(u16*)item, charId)) {
                        u16 flags = *(u16*)(item + 0xE);
                        if (!flags || (slotMask & flags)) accept = 1;
                        else if (!(otherMask & flags)) accept = 1;
                    }
                }
            } else {
                u8 gearId = state[0x30C + charId * 0xA4];
                if (type < 5) {
                    u8* weapon = gearWeapons + gearWeaponIds[i] * 20;
                    if (type == 0) {
                        if (func_801C8678(*(u32*)(weapon + 4), gearId) &&
                            weapon[0xF] < 5 && gearWeaponIds[i] < 0x32) {
                            accept = 1;
                        }
                    } else if (func_801C8678(*(u32*)(weapon + 4), gearId) &&
                               weapon[0xF] == equippedGearWeapon[0xF] &&
                               gearWeaponIds[i] >= 0x32) {
                        accept = 1;
                    }
                } else {
                    u8* item = gearAccessories + gearAccIds[i] * 28;
                    if (func_801C8678(*(u32*)item, gearId)) {
                        u16 flags = *(u16*)(item + 8);
                        if (!flags || (slotMask & flags)) accept = 1;
                        else if (!(otherMask & flags)) accept = 1;
                    }
                }
            }

            if (accept) {
                if (!gearMode) {
                    if (type == 5) {
                        outIds[0] = charAccIds[i];
                        outCounts[0] = charAccCounts[i];
                    } else {
                        outIds[0] = charWeaponIds[i];
                        outCounts[0] = charWeaponCounts[i];
                    }
                } else if (type == 5) {
                    outIds[0] = gearAccIds[i];
                    outCounts[0] = gearAccCounts[i];
                } else {
                    outIds[0] = gearWeaponIds[i];
                    outCounts[0] = gearWeaponCounts[i];
                }
                outIds++;
                outCounts++;
                filled++;
            }
        }
    }

    {
        u8* renderBuffer = HeapAlloc(0x3F6, 0);
        s32 row;
        s32 y = 0x12;
        s32 nameOff = 0;
        s32 countOff = 0x400;
        for (row = 0; row < 8; row++, page++) {
            u8 itemId = D_801EA730[page];
            if (itemId) {
                void* name;
                u16 digits[2];
                u8 digitBuf[8];
                u8 tens;
                u8 ones;
                u8* nameLine = listBuf + nameOff;
                u8* countLine = listBuf + countOff;
                s32 width;
                RECT rect;

                if (!gearMode) {
                    name = type == 5 ? GetAccessoryName(itemId)
                                     : GetWeaponName(itemId);
                } else {
                    name = type == 5 ? func_80033A2C(itemId)
                                     : func_80033A5C(itemId);
                }
                width = SystemRenderStringEntry(name, renderBuffer, 0x24, 0);
                nameLine[0x7E] = (u8)width;

                tens = (u8)(D_801EA7F8[page] / 10);
                ones = (u8)(D_801EA7F8[page] - tens * 10);
                digits[0] = tens ? (u16)(tens + 0x10) : 0xC3;
                digits[1] = (u16)(ones + 0x10);
                func_80033B34(digits, digitBuf, 2);
                width = SystemRenderStringEntry(digitBuf, renderBuffer, 0x24, 1);
                countLine[0x7E] = (u8)width;

                /* Retail: x=((row&1)*3)<<3+0x180; y=(row>>1)*0xD+0x80 */
                rect.x = (s16)((((row & 1) * 3) << 3) + 0x180);
                rect.y = (s16)((row >> 1) * 0xD + 0x80);
                rect.w = 0x28;
                rect.h = 0xD;
                LoadImage(&rect, (u_long*)renderBuffer);
                DrawSync(0);

                EquipListStageE7C50(nameLine, row, 0x80, 0x81);
                EquipListStageE7C50(countLine, row, 0x80, 0x82);
                func_801C851C((SVECTOR*)(nameLine + 0x50), 0xA8, y,
                              nameLine[0x7E], 0xD);
                func_801C851C((SVECTOR*)(countLine + 0x50), 0x10C, y,
                              countLine[0x7E], 0xD);
                nameLine[0x7D] = ((u8*)g_Menu)[0x308];
                countLine[0x7D] = ((u8*)g_Menu)[0x308];
                listBuf[0xA10 + row] = 1;
            } else {
                listBuf[0xA10 + row] = 0;
            }
            y += 0xD;
            nameOff += 0x80;
            countOff += 0x80;
        }
        HeapFree(renderBuffer);
    }

#ifndef XENO_EQUIP_LIST_TEST
    /* listBuf+0x800 is a PSX-width MenuString (0x80).  Native D36E0 writes
     * sizeof(MenuString)==152 — stage through a host MenuString and pack
     * polys/vertices/meta back so description lines at +0x880 stay intact. */
    {
        u8* line = listBuf + 0x800;
        MenuString nativeTitle;
        u8* nativeBytes = (u8*)&nativeTitle;
        s32 byte;

        memset(&nativeTitle, 0, sizeof(nativeTitle));
        for (byte = 0; byte < 0x50; byte++) {
            nativeBytes[byte] = line[byte];
        }
        memcpy(nativeTitle.vertices, line + 0x50, sizeof(SVECTOR) * 4);
        nativeTitle.width = line[0x7E];
        nativeTitle.unk7C = line[0x7C];
        nativeTitle.renderContext = line[0x7D];
        func_801D36E0(&nativeTitle, selected, gearMode, 0);
        for (byte = 0; byte < 0x50; byte++) {
            line[byte] = nativeBytes[byte];
        }
        memcpy(line + 0x50, nativeTitle.vertices, sizeof(SVECTOR) * 4);
        line[0x7C] = nativeTitle.unk7C;
        line[0x7D] = nativeTitle.renderContext;
        line[0x7E] = nativeTitle.width;
        line[0x7F] = nativeTitle.unk7F;
    }
#endif
    filled -= 8;
    ((u8*)manager)[0x4C] = 1;
    if (filled < 0) filled = 0;
    return filled;
}