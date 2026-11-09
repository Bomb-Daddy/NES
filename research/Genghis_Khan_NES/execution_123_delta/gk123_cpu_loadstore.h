#ifndef GK123_CPU_LOADSTORE_H
#define GK123_CPU_LOADSTORE_H
#include <stdint.h>
/* Official NMOS 2A03 load/store opcodes: 31 forms, no illegal-op aliases.
 * CPU address/flag semantics only, NOT a substitute for cartridge- or
 * hardware-proven original world initialization. */
typedef enum {GK123_A,GK123_X,GK123_Y} Gk123Reg;
typedef enum {GK123_IMM,GK123_ZP,GK123_ZPX,GK123_ZPY,GK123_ABS,
              GK123_ABSX,GK123_ABSY,GK123_INDX,GK123_INDY} Gk123Mode;
typedef struct {Gk123Reg reg;Gk123Mode mode;uint8_t store;} Gk123LoadStore;
int gk123_decode(uint8_t opcode,Gk123LoadStore *out);
uint16_t gk123_zp_offset(uint8_t zp,uint8_t index);
uint16_t gk123_address(uint8_t lo,uint8_t hi,uint8_t index);
uint8_t gk123_load_flags(uint8_t original_status,uint8_t loaded);
uint8_t gk123_cycles(uint8_t opcode,uint8_t page_crossed);
#endif
