#include "gk1_app_state.h"
#include "gk103_ntsc_ppu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{ if(!(x)){fprintf(stderr,"GK154 assertion failed line %d: %s\n",__LINE__,#x);return 1;}checks++;}while(0)
int main(void){
 unsigned checks=0;
 Gk103Ppu initial={0};uint8_t val=0xa5u;
 CHECK(gk104_ppu_reset_origin(&initial,0u));
 Gk103Ppu s=initial;
 CHECK(!gk103_ppu_status(&s,27394u,0x15u,&val));
 CHECK(val==0xa5u);
 CHECK(s.race_gate==1u);
 CHECK(s.last_dot==initial.last_dot);
 Gk103Ppu p=initial;
 CHECK(!gk154_ppu_status_race_branch(&p,27394u,0x15u,2u,&val));
 CHECK(!memcmp(&p,&initial,sizeof p));CHECK(val==0xa5u);
 CHECK(!gk154_ppu_status_race_branch(&p,27390u,0x15u,0u,&val));
 CHECK(!memcmp(&p,&initial,sizeof p));
 Gk103Ppu before=initial,after=initial;
 CHECK(gk154_ppu_status_race_branch(&before,27394u,0x15u,0u,&val));
 CHECK(val==0x15u && before.vblank==0u && before.status_reads==1u);
 CHECK(gk154_ppu_status_race_branch(&after,27394u,0x15u,1u,&val));
 CHECK(val==0x95u && after.vblank==0u && after.status_reads==1u);
 CHECK(before.last_dot==after.last_dot && before.vblank_events==after.vblank_events);
 CHECK(!gk154_ppu_status_race_branch(NULL,27394u,0u,0u,&val));
 CHECK(!gk154_ppu_status_race_branch(&p,27394u,0u,0u,NULL));
 Gk1App *app=calloc(1,sizeof *app);CHECK(app!=NULL);gk1_app_init(app);
 Gk1App *save=malloc(sizeof *save);CHECK(save!=NULL);*save=*app;
 Gk83Trace out={0};out.stop_pc=0x55aau;
 CHECK(gk1_app_source_one_race_branch(app,"/nonexistent/stock.nes",0u,1u,&out)==GK153_TRACE_BAD_STAGE);
 CHECK(out.stop_pc==0x55aau);
 app->setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;*save=*app;
 CHECK(gk1_app_source_one_race_branch(app,"/nonexistent/stock.nes",0u,1u,&out)==GK153_TRACE_BAD_SOURCE);
 CHECK(out.stop_pc==0x55aau);CHECK(!memcmp(app,save,sizeof *app));
 CHECK(gk154_trace_original_one_race_branch("/nonexistent/stock.nes",0u,0u,100u,&out)==GK83_INVALID);
 CHECK(gk154_trace_original_one_race_branch("/nonexistent/stock.nes",0u,3u,100u,&out)==GK83_INVALID);
 CHECK(gk154_trace_original_one_race_branch("/nonexistent/stock.nes",GK103_VBLANK_START,1u,100u,&out)==GK83_INVALID);
 CHECK(out.stop_pc==0x55aau);
 const char* rom=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(rom&&*rom){
  Gk83Trace a={0},b={0},base={0};
  Gk83Stop r0=gk151_trace_original_even_prerender(rom,0u,0u,120000u,&base);
  CHECK(r0==GK83_HARDWARE_GATE && base.stop_pc==0xc5be);
  Gk83Stop r1=gk154_trace_original_one_race_branch(rom,0u,1u,120000u,&a);
  Gk83Stop r2=gk154_trace_original_one_race_branch(rom,0u,2u,120000u,&b);
  CHECK(r1!=GK83_INVALID&&r1!=GK83_SOURCE_REFUSED);
  CHECK(r2!=GK83_INVALID&&r2!=GK83_SOURCE_REFUSED);
  CHECK(a.hypothetical_race_consumed==1u&&a.hypothetical_race_choice==1u);
  CHECK(b.hypothetical_race_consumed==1u&&b.hypothetical_race_choice==2u);
  CHECK(a.hypothetical_race_source_pc==0xc5be && b.hypothetical_race_source_pc==0xc5be);
  CHECK(a.instructions>=base.instructions&&b.instructions>=base.instructions);
  CHECK(gk1_app_source_one_race_branch(app,rom,0u,1u,&out)==GK153_TRACE_BOUNDED);
  CHECK(out.hypothetical_race_consumed==1u);
  CHECK(!memcmp(app,save,sizeof *app));
 }
 free(save);free(app);
 printf("GK154 %u assertions PASS; optional original ROM source path = %s; Mesen/first Spring NOT RUN\n",checks,rom&&*rom?"authenticated":"not supplied");
 return 0;
}
