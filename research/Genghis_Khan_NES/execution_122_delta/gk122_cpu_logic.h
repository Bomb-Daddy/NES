#ifndef GK122_CPU_LOGIC_H
#define GK122_CPU_LOGIC_H
#include <stdint.h>
/* Official NES 2A03 binary logic operations. These are CPU semantics, NOT
 * original-stock game RNG / Spring initialization. No private ROM bytes. */
typedef enum {GK122_ORA=0,GK122_AND=1,GK122_EOR=2} Gk122LogicKind;
typedef enum {GK122_IMM=0,GK122_ZP=1,GK122_ZPX=2,GK122_ABS=3,
              GK122_ABSX=4,GK122_ABSY=5,GK122_INDX=6,GK122_INDY=7} Gk122LogicMode;
typedef struct {Gk122LogicKind kind; Gk122LogicMode mode;} Gk122Logic;
/* Return 0 for non-logic/unsupported opcodes. The caller must use a real
 * bus read for non-immediate data; no invented RAM or PPU values. */
int gk122_decode(uint8_t op,Gk122Logic *out);
uint8_t gk122_result(Gk122LogicKind kind,uint8_t a,uint8_t memory);
uint8_t gk122_flags(uint8_t before,uint8_t value);
uint16_t gk122_zpx(uint8_t base,uint8_t x);
uint16_t gk122_indx(uint8_t lo,uint8_t hi);
uint16_t gk122_indy(uint8_t lo,uint8_t hi,uint8_t y);
#endif
