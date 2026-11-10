#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
static void expected_refusal(void){
 Gk148Tick ticks[GK148_TICKS]={0};
 for(unsigned i=0;i<GK148_TICKS;i++){
  ticks[i].stage=(uint8_t)i;ticks[i].frame=100u+i*125u;
  ticks[i].cycles=1000u+i*3722570u;
  if(i>=2u){ticks[i].bank=1u;ticks[i].countries=14u;
   ticks[i].year=i>=10u?1175u:1174u;
   ticks[i].gold=i>=12u?600u:590u;
   ticks[i].food=i>=12u?200u:i>=9u?250u:125u;
   ticks[i].troops=i>=12u?61u:60u;
   ticks[i].rate=i>=10u?113u:0u;
   ticks[i].orders=i>=14u?3u:0u;
  }
 }
 Gk148FreshTimeline out={0};memset(&out,0x5a,sizeof out);
 Gk148FreshTimeline saved=out;
 assert(gk148_analyze_ticks(NULL,GK148_TICKS,&out)==GK148_ARGUMENT);
 assert(gk148_analyze_ticks(ticks,GK148_TICKS-1u,&out)==GK148_ARGUMENT);
 assert(!memcmp(&out,&saved,sizeof out));
 assert(gk148_analyze_ticks(ticks,GK148_TICKS,&out)==GK148_OK);
 assert(out.original_fresh_attested==0u&&out.samples==15u&&
        out.first_world_setup==2u&&out.first_setup_food_change==9u&&
        out.first_year_1175==10u&&out.first_rate==10u&&
        out.first_spring_finance==12u&&out.first_three_orders==14u&&
        out.first_spring_generated_by_port==0u&&
        out.original_gameplay_commands_implemented==0u);
 saved=out;
 ticks[8].bank=2u; /* Wrong SaveRAM bank cannot masquerade as fresh */
 assert(gk148_analyze_ticks(ticks,GK148_TICKS,&out)==GK148_CHRONOLOGY);
 assert(!memcmp(&out,&saved,sizeof out));ticks[8].bank=1u;
 ticks[12].cycles=ticks[11].cycles;
 assert(gk148_analyze_ticks(ticks,GK148_TICKS,&out)==GK148_CHRONOLOGY);
 assert(!memcmp(&out,&saved,sizeof out));ticks[12].cycles=ticks[11].cycles+3722570u;
 ticks[12].orders=2u;
 assert(gk148_analyze_ticks(ticks,GK148_TICKS,&out)==GK148_CHRONOLOGY);
 assert(!memcmp(&out,&saved,sizeof out));ticks[12].orders=0u;
 ticks[13].countries=27u;
 assert(gk148_analyze_ticks(ticks,GK148_TICKS,&out)==GK148_CHRONOLOGY);
 assert(!memcmp(&out,&saved,sizeof out));ticks[13].countries=14u;
 ticks[14].rate=0u;
 assert(gk148_analyze_ticks(ticks,GK148_TICKS,&out)==GK148_CHRONOLOGY);
 assert(!memcmp(&out,&saved,sizeof out));ticks[14].rate=113u;
 /* Without exact original input this MUST NOT be called genuine gameplay. */
 assert(gk148_verify_private_fresh("no-such-private-state.bin",&out)==GK148_SOURCE_UNVERIFIED);
 assert(!memcmp(&out,&saved,sizeof out));
 Gk1App *app=calloc(1,sizeof *app);assert(app);
 gk1_app_init(app);
 assert(gk1_app_original_fresh_bank_trace(app,"no-such-private-state.bin",&out)==GK148_SETUP_GUARD);
 assert(!memcmp(&out,&saved,sizeof out));
 free(app);
}
int main(int argc,char **argv){
 expected_refusal();
 if(argc==1){puts("GK148 public bank-aware chronology and refusal PASS; private evidence NOT RUN");return 0;}
 assert(argc==3);
 Gk148FreshTimeline out={0};
 assert(gk148_verify_private_fresh(argv[2],&out)==GK148_OK);
 assert(out.original_fresh_attested==1u&&out.first_world_setup==2u&&
        out.first_setup_food_change==9u&&out.first_year_1175==10u&&
        out.first_spring_finance==12u&&out.first_three_orders==14u&&
        out.final_gold==430u&&out.final_food==208u&&out.final_troops==61u&&
        out.final_rate==121u&&out.first_frame==241u&&out.ready_frame==2691u&&
        out.ready_cycles==80136825u&&out.first_spring_generated_by_port==0u);
 Gk1TemplateDataset *ds=malloc(sizeof *ds);assert(ds);
 assert(gk1_template_load(argv[1],ds)==GK1_LOAD_OK);
 Gk1App *app=calloc(1,sizeof *app);assert(app);
 assert(gk1_app_install_templates(app,ds));
 Gk148FreshTimeline integrated={0};
 assert(gk1_app_original_fresh_bank_trace(app,argv[2],&integrated)==GK148_OK);
 assert(memcmp(&integrated,&out,sizeof out)==0);
 assert(app->worlds[0].phase==GK1_PRE_SETUP_TEMPLATE);
 assert(!app->stock_checkpoint.valid);
 free(ds);free(app);
 puts("GK148 EXACT PRIVATE Mesen fresh WorkRAM chronology 00..14 PASS; port has NOT generated first Spring");
 return 0;
}
