#ifndef GK99_XINPUT_BRIDGE_H
#define GK99_XINPUT_BRIDGE_H
#include "gk1_pad_router.h"
#include <stdint.h>
/* Portable decoder of raw documented XInput Gamepad fields. No Windows headers,
 * dynamic library, keyboard, filesystem or virtual hardware dependence.
 * Win32 poller uses this SAME decoder; Linux tests verify its contract,
 * but cannot certify Windows build or physical Xbox behavior. */
typedef struct {
 uint16_t buttons;
 int16_t thumb_lx,thumb_ly;
} Gk99XinputRaw;
/* Fail closed: NULL leaves output unchanged. All raw button combinations are
 * accepted; unknown/unused XInput buttons cannot invoke native commands. */
int gk99_xinput_sample(const Gk99XinputRaw *raw,Gk1PadSample *out);
/* Sticky 0..3 device selection. 0xff means none. If current disconnects,
 * pick lowest connected, with router reset performed by host. On invalid
 * candidate or output, do not mutate. mask has only 4 physically queried bits. */
int gk99_xinput_slot(uint8_t available_mask,uint8_t previous,uint8_t *chosen);
#endif
