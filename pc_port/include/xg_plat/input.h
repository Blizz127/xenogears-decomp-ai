/* xg_plat/input.h -- input interface: keyboard bindings for the pad.
 * Backend: PsyCross keyboard mapping (src/plat/xg_plat_psycross.c). */
#ifndef XG_PLAT_INPUT_H
#define XG_PLAT_INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    XG_PLAT_BTN_SQUARE, XG_PLAT_BTN_CIRCLE, XG_PLAT_BTN_TRIANGLE, XG_PLAT_BTN_CROSS,
    XG_PLAT_BTN_L1, XG_PLAT_BTN_L2, XG_PLAT_BTN_R1, XG_PLAT_BTN_R2,
    XG_PLAT_BTN_START, XG_PLAT_BTN_SELECT,
    XG_PLAT_BTN_LEFT, XG_PLAT_BTN_RIGHT, XG_PLAT_BTN_UP, XG_PLAT_BTN_DOWN,
    XG_PLAT_BTN_COUNT
} XgPlatButton;

/* Name used in config files ("circle", "l1", "up", ...), or NULL. */
const char* xg_plat_input_button_name(XgPlatButton button);
/* Bind a keyboard key by its SDL key name ("Z", "Left Shift", ...).
 * Returns 1 when the key name is known. */
int xg_plat_input_bind_key(XgPlatButton button, const char* key_name);

/* Pad state for the game's PadRead calls (link-time routing,
 * build_port.sh XG_PLAT_WRAP_SYMS): pad 1 in the low 16 bits, pad 2 in the
 * high 16, PSX button bit layout, bit set = pressed. */
unsigned int xg_plat_input_read_pads(int id);

#ifdef __cplusplus
}
#endif

#endif
