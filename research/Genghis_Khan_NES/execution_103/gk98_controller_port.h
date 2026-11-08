#ifndef GK98_CONTROLLER_PORT_H
#define GK98_CONTROLLER_PORT_H
#include <stdint.h>
/* NES $4016 JOYPAD 1 serial-bit model. Buttons are explicitly supplied by
 * caller, in NES shift order A,B,Select,Start,Up,Down,Left,Right. This is
 * a bounded peripheral component, NOT proof of stock-game controller events.
 * CPU read bus bits 1..7 are unknown on real hardware; supplied_high_bits
 * is a REQUIRED hypothesis, not an inferred electrical value. */
typedef struct {
 uint8_t buttons, latched, strobe, index;
 uint32_t reads, writes;
} Gk98Pad;
void gk98_pad_reset(Gk98Pad *pad, uint8_t buttons);
void gk98_pad_write(Gk98Pad *pad, uint8_t value);
uint8_t gk98_pad_read(Gk98Pad *pad, uint8_t supplied_high_bits);
#endif
