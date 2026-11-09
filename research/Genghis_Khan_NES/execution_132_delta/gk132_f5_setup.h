#ifndef GK132_F5_SETUP_H
#define GK132_F5_SETUP_H
#include <stdint.h>
/* Original-stock USA ROM verified source inspection. This documents the
 * conditional $8050 routine, not execution of that routine from reset.
 * In particular, the result MUST NOT seed $00F5 or unlock gameplay. */
typedef enum {
 GK132_OK=0,GK132_BAD=1,GK132_SOURCE_REFUSED=2,
 GK132_SOURCE_CHANGED=3
} Gk132Result;
typedef enum {
 GK132_F5_UNDETERMINED=0, /* input RAM not proven */
 GK132_F5_WOULD_WRITE_TWO=1, /* only IF CPU enters verified $8050 */
 GK132_F5_WOULD_NOT_WRITE=2 /* branch returns without writing */
} Gk132ConditionalResult;
typedef struct {
 uint16_t entry_pc;    /* $8050 in authenticated original bank 14 */
 uint16_t next_read_pc;/* $8060 */
 uint16_t b8_addr,ba_addr,f5_addr;
 uint16_t possible_writer_pc; /* $805C conditional STA */
 uint8_t possible_writer_value;/* immediate #$02 */
 uint8_t control_is_unproven;  /* true until source call path is traced */
 uint8_t first_spring_playable;/* must remain false */
} Gk132SourceFacts;
/* On failure, outputs unchanged; ROM bytes never returned. */
Gk132Result gk132_verify_conditional_f5_writer(const char *private_rom,
 Gk132SourceFacts *out);
/* Inputs are 1 only when respective RAM values have actual provenance.
 * Evaluates conditional 6502 branches; does not execute original game. */
Gk132ConditionalResult gk132_conditional_if_called(uint8_t b8_known,
 uint8_t b8,uint8_t ba_known,uint8_t ba);
#endif
