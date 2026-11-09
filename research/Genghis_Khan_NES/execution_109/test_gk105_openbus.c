#include "gk105_openbus.h"
#include "gk103_ntsc_ppu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"EXEC0105 FAILED %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void){
 Gk105Summary a,b;memset(&a,0xa7,sizeof a);b=a;
 CHECK(gk105_scan_lower7(NULL,0,100,&a)==GK83_INVALID);
 CHECK(gk105_scan_lower7("/absent",GK103_FRAME_DOTS,100,&a)==GK83_INVALID);
 CHECK(gk105_scan_lower7("/absent",GK103_VBLANK_START,100,&a)==GK83_INVALID);
 CHECK(gk105_scan_lower7("/absent",0,0,&a)==GK83_INVALID);
 CHECK(gk105_scan_lower7("/absent",0,1000001u,&a)==GK83_INVALID);
 CHECK(gk105_scan_lower7("/absent",0,100,&a)==GK83_SOURCE_REFUSED);
 CHECK(gk105_trace_original_low7("/absent",0,128u,100,&a.baseline)==GK83_INVALID);
 CHECK(memcmp(&a,&b,sizeof a)==0);
 const char *rom=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(rom&&*rom){
  uint32_t origins[]={0u,10240u,80000u};
  for(unsigned i=0;i<3;i++){
   Gk105Summary first={0},second={0};
   Gk83Stop stop1=gk105_scan_lower7(rom,origins[i],30000u,&first);
   Gk83Stop stop2=gk105_scan_lower7(rom,origins[i],30000u,&second);
   CHECK(stop1!=GK83_INVALID&&stop1!=GK83_SOURCE_REFUSED&&stop1==stop2);
   CHECK(first.trials==128u&&second.trials==128u);
   CHECK(memcmp(&first,&second,sizeof first)==0);
   CHECK(first.control_divergences<=127u&&first.register_divergences<=127u);
   CHECK(first.baseline.ppu_scheduled_frame_origin_dots==origins[i]);
   printf("SOURCE_HYPOTHESIS origin=%u control=%u registers=%u pc=%04x\n",origins[i],
    first.control_divergences,first.register_divergences,first.baseline.stop_pc);
  }
 }else puts("EXEC0105 portable rollback tests PASS (private ROM NOT RUN)");
 return 0;
}
