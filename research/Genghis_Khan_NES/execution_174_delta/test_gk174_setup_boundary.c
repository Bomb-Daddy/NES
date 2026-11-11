#include "gk174_setup_boundary.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(x) do {if(!(x)){fprintf(stderr,"GK174 FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char **argv){
 REQUIRE(argc==1||argc==2);
 Gk174SetupBoundary pair,orig;
 memset(&pair,0xa7,sizeof pair);orig=pair;
 REQUIRE(!gk174_trace_setup_boundary(NULL,50000,&pair));
 REQUIRE(!gk174_trace_setup_boundary("",50000,&pair));
 REQUIRE(!gk174_trace_setup_boundary("/definitely/not/a/rom.nes",50000,&pair));
 REQUIRE(!gk174_trace_setup_boundary("/definitely/not/a/rom.nes",142,&pair));
 REQUIRE(memcmp(&pair,&orig,sizeof pair)==0);
 if(argc==2){
  REQUIRE(gk174_trace_setup_boundary(argv[1],50000,&pair));
  REQUIRE(pair.strict_gate==GK83_UNKNOWN_RAM_GATE);
  REQUIRE(pair.mesen_zero_gate==GK83_HARDWARE_GATE);
  REQUIRE(pair.strict.instructions==70u);
  REQUIRE(pair.mesen_zero.instructions==142u);
  REQUIRE(pair.strict.completed_cpu_cycles==243u);
  REQUIRE(pair.mesen_zero.first_hardware_read==0x2002u);
  REQUIRE(pair.completed_instruction_delta==72u);
  REQUIRE(pair.strict.executed_rng_transfers.total==0u);
  REQUIRE(pair.mesen_zero.executed_rng_transfers.total==0u);
  Gk174SetupBoundary repeat={0};
  REQUIRE(gk174_trace_setup_boundary(argv[1],50000,&repeat));
  REQUIRE(memcmp(&pair,&repeat,sizeof pair)==0);
  orig=pair;
  REQUIRE(!gk174_trace_setup_boundary("/invalid/file.nes",50000,&pair));
  REQUIRE(memcmp(&pair,&orig,sizeof pair)==0);
  printf("Original-stock source/verified Mesen setting: strict %u instructions -> zero RAM %u, first hardware read $%04X. NO PPU/Mesen runtime or first Spring.\n",pair.strict.instructions,pair.mesen_zero.instructions,pair.mesen_zero.first_hardware_read);
 }
 puts("GK174 source initial RAM provenance/gates/atomic rejection PASS");
 return 0;
}
