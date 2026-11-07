#ifndef GK1_PAD_ROUTER_H
#define GK1_PAD_ROUTER_H
/* EXEC0043: portable controller-only input state machine. No dependence on
 * NES ROM or claims of physical Xbox verification. */
#include <stdint.h>
#include <stddef.h>

enum {
 GK1_PAD_UP=1u<<0, GK1_PAD_DOWN=1u<<1,
 GK1_PAD_LEFT=1u<<2, GK1_PAD_RIGHT=1u<<3,
 GK1_PAD_A=1u<<4, GK1_PAD_B=1u<<5,
 GK1_PAD_X=1u<<6, GK1_PAD_Y=1u<<7, GK1_PAD_MENU=1u<<8,
 GK1_PAD_VIEW=1u<<9
};
typedef enum {
 GK1_PAD_ACT_UP, GK1_PAD_ACT_DOWN, GK1_PAD_ACT_LEFT,
 GK1_PAD_ACT_RIGHT, GK1_PAD_ACT_A, GK1_PAD_ACT_B,
 GK1_PAD_ACT_X, GK1_PAD_ACT_Y, GK1_PAD_ACT_MENU,
 GK1_PAD_ACT_VIEW
} Gk1PadAction;
typedef struct {
 uint16_t buttons; /* mapped D-pad + A/B/X/Y, not platform raw bitfield */
 int16_t lx,ly; /* normalized: left/up negative, right/down positive */
 int16_t hx,hy; /* hat D-pad; values -32767, 0, 32767 or similar */
} Gk1PadSample;
typedef struct {
 uint16_t held;
 int8_t analog_x,analog_y,hat_x,hat_y;
 uint32_t repeat_due_ms[4];
 int connected;
} Gk1PadRouter;
void gk1_pad_init(Gk1PadRouter *r);
/* On first connection, seed held states without phantom presses. */
void gk1_pad_seed(Gk1PadRouter *r,const Gk1PadSample *s);
/* Disconnect resets all edges/repeat timers; reconnect must seed again. */
void gk1_pad_disconnect(Gk1PadRouter *r);
/* Translate input once per frame. Returns count, never writes beyond cap.
 * Remainder is intentionally dropped if caller capacity is too small.
 * A/B/X/Y only fire on rising edges; D-pad/left stick repeat in menus.
 */
size_t gk1_pad_step(Gk1PadRouter *r,const Gk1PadSample *s,uint32_t dt_ms,
                   Gk1PadAction *out,size_t cap);
#endif
