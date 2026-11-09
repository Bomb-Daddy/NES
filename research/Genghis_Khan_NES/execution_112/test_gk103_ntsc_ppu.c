#include "gk103_ntsc_ppu.h"
#include "gk103_ppu_schedule.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TEST(c) do{if(!(c)){fprintf(stderr,"GK103 TEST FAILED line %d: %s\n",__LINE__,#c);return 1;}}while(0)
int main(void){
 Gk103Ppu p={0};uint8_t v=0xaa;
 TEST(!gk103_ppu_reset(0,0));TEST(!gk103_ppu_reset(&p,3));
 TEST(gk103_ppu_reset(&p,0));
 TEST(gk103_ppu_ctrl(&p,2000u,0x80u));TEST(p.warmup_ignored_ctrl&&!p.nmi_enable);
 TEST(gk103_ppu_advance(&p,27500u));TEST(p.vblank&&p.vblank_events==1u);
 TEST(gk103_ppu_status(&p,27501u,0x15u,&v));TEST(v==0x95u&&!p.vblank);
 TEST(gk103_ppu_status(&p,27503u,0x15u,&v));TEST(v==0x15u);
 TEST(!gk103_ppu_advance(&p,27502u));
 TEST(gk103_ppu_advance(&p,30000u));TEST(!p.vblank);
 TEST(gk103_ppu_ctrl(&p,30001u,0x80u));TEST(p.nmi_enable);
 TEST(gk103_ppu_advance(&p,58000u));TEST(p.vblank_events==2u&&p.nmi_edges==1u);
 TEST(gk103_ppu_take_nmi(&p));TEST(!gk103_ppu_take_nmi(&p));
 TEST(gk103_ppu_status(&p,58001u,0u,&v));TEST(v==0x80u);
 TEST(gk103_ppu_ctrl(&p,58002u,0x00u));TEST(gk103_ppu_ctrl(&p,58003u,0x80u));
 TEST(!gk103_ppu_take_nmi(&p)); /* status clear prevented immediate edge */
 TEST(gk103_ppu_reset(&p,0));
 TEST(gk103_ppu_advance(&p,27393u)); /* dot82179 near vblank start */
 TEST(!gk103_ppu_status(&p,27394u,0u,&v)); TEST(p.race_gate);
 TEST(gk103_ppu_reset(&p,0));TEST(gk103_ppu_advance(&p,30000u));
 TEST(!gk103_ppu_mask(&p,30001u,0x18u));TEST(p.rendering_unmodeled);
 TEST(!gk103_ppu_advance(&p,30002u));
 const char *rom=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(rom&&*rom){
  Gk83Trace a={0},b={0};
  Gk83Stop sa=gk103_trace_original_ppu_schedule(rom,0,150000,&a);
  Gk83Stop sb=gk103_trace_original_ppu_schedule(rom,0,150000,&b);
  TEST(sa==sb&&sa!=GK83_SOURCE_REFUSED&&sa!=GK83_INVALID);
  TEST(memcmp(&a,&b,sizeof a)==0);
  TEST(a.instructions>100u);
  printf("GK103 phase0 source gate=%u PC=$%04x inst=%u cycles=%llu vblank=%llu NMI=%u PPUreads=%u\n",
   (unsigned)sa,a.stop_pc,a.instructions,(unsigned long long)a.completed_cpu_cycles,
   (unsigned long long)a.ppu_scheduled_vblank,a.ppu_scheduled_nmi_injections,
   a.ppu_scheduled_status_reads);
  Gk83Trace other={0}; Gk83Stop so=gk103_trace_original_ppu_schedule(rom,2,150000,&other);
  TEST(so!=GK83_SOURCE_REFUSED && so!=GK83_INVALID);TEST(other.ppu_scheduled_ppu_phase==2u);
 }else puts("GK103 portable timing tests PASS; original ROM gate not exercised");
 Gk83Trace dest={0},saved;memset(&dest,0x57,sizeof dest);saved=dest;
 TEST(gk103_trace_original_ppu_schedule(NULL,0,100,&dest)==GK83_INVALID);
 TEST(gk103_trace_original_ppu_schedule("/notfound",3,100,&dest)==GK83_INVALID);
 TEST(gk103_trace_original_ppu_schedule("/notfound",0,100,&dest)==GK83_SOURCE_REFUSED);
 TEST(memcmp(&dest,&saved,sizeof dest)==0);
 return 0;
}
