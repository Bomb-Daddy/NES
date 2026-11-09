#include "gk100_cpu_cycles.h"
#include "gk122_cpu_logic.h"
#include "gk123_cpu_loadstore.h"
#include "gk124_cpu_compare.h"
#include "gk125_cpu_arith.h"
uint8_t gk100_page_crossed(uint16_t base,uint16_t effective){
 return (uint8_t)(((base^effective)&0xff00u)!=0u);
}
uint8_t gk100_cpu_cycles(uint8_t op,uint8_t taken,uint8_t page){
 Gk125Arithmetic ar;
 if(gk125_decode(op,&ar))return gk125_cycles(op,page);
 Gk124Compare cmp;
 if(gk124_compare_decode(op,&cmp))return gk124_compare_cycles(op,page);
 Gk123LoadStore ls;
 if(gk123_decode(op,&ls))return gk123_cycles(op,page);
 Gk122Logic logic;
 if(gk122_decode(op,&logic)){
  switch(logic.mode){
   case GK122_IMM:return 2u;
   case GK122_ZP:return 3u;
   case GK122_ZPX:return 4u;
   case GK122_ABS:return 4u;
   case GK122_ABSX:case GK122_ABSY:return (uint8_t)(4u+(page?1u:0u));
   case GK122_INDX:return 6u;
   case GK122_INDY:return (uint8_t)(5u+(page?1u:0u));
  }
 }
 /* Full base-cycle table for the *implemented* 2A03 opcode subset, not
  * a claim to support all 256 opcodes. BRK remains a deliberate gate. */
 switch(op){
  case 0xea:case 0x18:case 0x38:case 0x78:case 0xd8: /* flag */
  case 0xb8:case 0x9a:case 0xba:case 0x8a:case 0x98:case 0xaa:case 0xa8:
  case 0xe8:case 0xca:case 0xc8:case 0x88:case 0x0a:case 0x4a:
  case 0xa9:case 0xa2:case 0xa0:case 0xc0:case 0xe0:case 0xc9:
  case 0x29:case 0x09:/* moved 0x69 to GK125 *//* moved 0xe9 to GK125 */ return 2u;
  case 0x24:case 0x08:case 0x48:case 0x84:case 0x86:case 0x85:case 0x05:/* moved 0xe5 to GK125 */case 0xc5:case 0xa6:case 0xa5:/* moved 0x65 to GK125 */
  case 0x4c:return 3u;
  case 0x2c:case 0x28:case 0x68:case 0x8d:case 0xad:case 0xae:case 0xac:
  case 0xcd:case 0x4d:/* moved 0x6d to GK125 */return 4u;
  case 0x06:case 0x46:case 0x66:case 0xc6:case 0xe6:case 0x26:case 0x9d:case 0x99:case 0x6c:return 5u;
  case 0x16:case 0x56:case 0x36:case 0x76:case 0xd6:case 0xf6:
  case 0x0e:case 0x4e:case 0x2e:case 0x6e:
  case 0x91:case 0x20:case 0x60:case 0x40:case 0xce:case 0xee:return 6u;
  case 0x1e:case 0x5e:case 0x3e:case 0x7e:case 0xde:case 0xfe:return 7u;
  case 0xbc:case 0xbd:case 0xb9:return (uint8_t)(4u+(page?1u:0u));
  case 0xb1:case 0xd1:/* moved 0xf1 to GK125 *//* moved 0x71 to GK125 */return (uint8_t)(5u+(page?1u:0u));
  case 0x50:case 0x70:case 0xb0:case 0x90:case 0xd0:case 0xf0:case 0x10:case 0x30:
   return (uint8_t)(2u+(taken?1u:0u)+((taken&&page)?1u:0u));
  default:return 0u;
 }
}
