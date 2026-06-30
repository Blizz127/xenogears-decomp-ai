/*
 * data_kernel_menu.c - migrated game data for the Xenogears PC port.
 *
 * The KernelMenu's on-screen strings live in the retail executable's .rodata as
 * data symbols (PSX 0x800182xx). In the port those are zeroed data stubs, so the
 * menu drew its cursor but no text. These are the verbatim strings from
 * build/out/slus_006.64.elf; defining them removes them from the zeroed data-stub
 * set so FontPrintf has real text to render.
 *
 * (This is the per-symbol form of data migration; the scalable approach is to
 * bulk-load the whole .main section into g_PsxRam and alias the data symbols.)
 */

char D_800182B0[] = "%02d:%02d:%02d";
char D_800182C0[] = " XENOGEARS Kernel MENU\n  %s %s MODE\n\n";
char D_800182E8[] = "PC HDD";
char D_800182F0[] = "CD EMU";
char D_800182F8[] = "    Field\n    Battle\n    Worldmap\n    Battling\n    Menu\n    Movie\n\n";
