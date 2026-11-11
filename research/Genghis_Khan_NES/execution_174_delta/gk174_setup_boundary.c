#include "gk174_setup_boundary.h"
#include "gk91_reset_zero.h"
#include <string.h>
int gk174_trace_setup_boundary(const char *rom,uint32_t budget,
 Gk174SetupBoundary *out){
 if(!out||!rom||!*rom||budget<143u||budget>1000000u)return 0;
 Gk174SetupBoundary candidate;
 memset(&candidate,0,sizeof candidate);
 candidate.strict_gate=gk83_trace_original_reset(rom,budget,&candidate.strict);
 if(candidate.strict_gate!=GK83_UNKNOWN_RAM_GATE ||
    candidate.strict.first_unknown_ram_read!=0x00f5u ||
    candidate.strict.stop_pc!=0x8060u)return 0;
 candidate.mesen_zero_gate=gk91_trace_reset_mesen_allzeros(rom,budget,&candidate.mesen_zero);
 if(candidate.mesen_zero_gate!=GK83_HARDWARE_GATE ||
    candidate.mesen_zero.first_hardware_read!=0x2002u ||
    candidate.mesen_zero.stop_pc!=0xc5beu)return 0;
 if(candidate.mesen_zero.instructions<=candidate.strict.instructions ||
    candidate.mesen_zero.completed_cpu_cycles<=candidate.strict.completed_cpu_cycles)
  return 0;
 candidate.completed_instruction_delta=
  candidate.mesen_zero.instructions-candidate.strict.instructions;
 candidate.completed_cycle_delta=
  candidate.mesen_zero.completed_cpu_cycles-candidate.strict.completed_cpu_cycles;
 /* A zero transfer count before first PPUSTATUS does not establish that
  * character/difficulty/RATE RNG is never called later in original NES. */
 *out=candidate;
 return 1;
}
