/*
 * data_field.c - migrated field overlay initialized data for the Xenogears PC port.
 *
 * D_800ADC44: UI texture descriptor table (field overlay .data).
 * Source: asm/field/data/3DF78.data.s label D_800ADC44 (file offset 0x3E154).
 * Field overlay base = 0x8006FAF0; RAM = 0x800ADC44.
 *
 * Full symbol size = 0x6C (108 bytes) = 54 halfwords = 9 entries × 6 halfwords.
 * FieldLoadUITextures reads the first 8 entries (48 halfwords / 96 bytes);
 * the 9th entry is part of the symbol but used elsewhere.
 *
 * Format per entry: {timX, timY, clutX, clutY, clutW, clutH}.
 * Entry 0: {672, 448, 0, 251, 0, 0}
 *
 * Defining this symbol removes it from the auto-generated zeroed data-stub set,
 * so FieldLoadUITextures gets real texture descriptors instead of zeros.
 * This does NOT affect the matching/decomp build (which links the asm .data
 * section directly); it only provides real data for the PC port.
 */

unsigned short D_800ADC44[54] = {
    /* entry 0: 800ADC44 */
    0x02A0, 0x01C0, 0x0000, 0x00FB, 0x0000, 0x0000,
    /* entry 1: 800ADC50 */
    0x0280, 0x01E0, 0x0100, 0x00F3, 0x0010, 0x0001,
    /* entry 2: 800ADC5C */
    0x029C, 0x01C0, 0x0100, 0x00F5, 0x0000, 0x0000,
    /* entry 3: 800ADC68 */
    0x0280, 0x01C0, 0x0100, 0x00F2, 0x0000, 0x0000,
    /* entry 4: 800ADC74 */
    0x0280, 0x01F0, 0x0100, 0x00F4, 0x0010, 0x0001,
    /* entry 5: 800ADC80 */
    0x03C0, 0x0140, 0x0100, 0x00F7, 0x0010, 0x0001,
    /* entry 6: 800ADC8C */
    0x0298, 0x01C0, 0x0100, 0x00F6, 0x0010, 0x0001,
    /* entry 7: 800ADC98 */
    0x0288, 0x01C0, 0x0100, 0x00F6, 0x0010, 0x0001,
    /* entry 8: 800ADCA4 */
    0x0380, 0x0100, 0x0000, 0x00E8, 0x0010, 0x0001,
};