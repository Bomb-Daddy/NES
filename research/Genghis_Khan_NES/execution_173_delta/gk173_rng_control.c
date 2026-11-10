#include "gk173_rng_control.h"
#include <limits.h>
Gk173Status gk173_note_completed(Gk173RngControl *out,
 uint8_t opcode,uint16_t source_pc,uint16_t destination_pc,
 uint8_t mapper_prg_register,uint64_t completed_cycles){
 if(!out||(mapper_prg_register>31u&&mapper_prg_register!=0xffu))
  return GK173_INVALID;
 unsigned kind=0;
 if(opcode==0x20u && destination_pc==0xef85u)kind=1u;
 else if(opcode==0x20u && destination_pc==0xc289u)kind=2u;
 else if(opcode==0x4cu && destination_pc==0xc289u)
  kind=source_pc==0xc1b8u?3u:4u;
 else if(opcode==0x6cu && destination_pc==0xc289u)kind=5u;
 else return GK173_IGNORED;
 if(out->total==UINT32_MAX ||
   (kind==1u&&out->dispatch_jsr_ef85==UINT32_MAX)||
   (kind==2u&&out->direct_jsr_c289==UINT32_MAX)||
   (kind==3u&&out->fixed_trampoline_jmp_c289==UINT32_MAX)||
   (kind==4u&&out->other_jmp_c289==UINT32_MAX)||
   (kind==5u&&out->indirect_jmp_c289==UINT32_MAX))return GK173_INVALID;
 Gk173RngControl n=*out;
 if(!n.total){n.first_pc=source_pc;n.first_target=destination_pc;
  n.first_opcode=opcode;n.first_prg_register=mapper_prg_register;
  n.first_completed_cycles=completed_cycles;}
 n.total++;n.last_pc=source_pc;n.last_target=destination_pc;
 n.last_opcode=opcode;n.last_prg_register=mapper_prg_register;
 n.last_completed_cycles=completed_cycles;
 switch(kind){case 1u:n.dispatch_jsr_ef85++;break;
  case 2u:n.direct_jsr_c289++;break;
  case 3u:n.fixed_trampoline_jmp_c289++;break;
  case 4u:n.other_jmp_c289++;break;
  default:n.indirect_jmp_c289++;break;}
 *out=n;return GK173_NOTED;
}
