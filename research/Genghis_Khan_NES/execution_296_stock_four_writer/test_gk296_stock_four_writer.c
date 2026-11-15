/* EXEC0296: original source code cross-check, synthetic known values only.
 * No original game ROM, screenshots, private traces, or campaign saves here. */
#include "gk1_app_state.h"
#include "gk137_stock_rng.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(e) do{if(!(e)){fprintf(stderr,"EXEC0296 FAIL %u: %s\n",__LINE__,#e);return 1;}checks++;}while(0)
static const Gk296Witness four[4]={
 {{1000u,0xc0bau,15u,0x84u},0u,1u},
 {{1005u,0xc0bdu,15u,0x86u},0u,2u},
 {{1010u,0xc0c2u,15u,0x85u},0u,3u},
 {{1015u,0xc0c5u,15u,0x87u},0u,4u}
};
int main(int argc,char **argv){
 unsigned checks=0;
 CHECK(argc==2||argc==3);
 Gk1App *app=calloc(1,sizeof *app),*before=malloc(sizeof *before);
 Gk1TemplateDataset *dataset=malloc(sizeof *dataset);
 CHECK(app&&before&&dataset);
 gk1_app_init(app);
 CHECK(gk1_template_load(argv[1],dataset)==GK1_LOAD_OK);
 CHECK(gk1_app_install_templates(app,dataset));
 const uint8_t initial[6]={0,0,0,0,5,6},predraw[6]={1,3,2,4,5,6};
 Gk137StockRng rng={0};uint16_t draw=0;uint8_t after[6]={0};
 CHECK(gk137_from_nes_bytes(&rng,predraw)==GK137_OK);
 CHECK(gk137_draw15(&rng,&draw)==GK137_OK);
 gk137_to_nes_bytes(&rng,after);
 Gk296Report output;memset(&output,0xa5,sizeof output);
 const Gk296Report sentinel=output;
 CHECK(gk296_check_four_writer_first_draw(NULL,four,4,initial,predraw,after,draw,&output)==GK296_ARGUMENT);
 CHECK(gk296_check_four_writer_first_draw("/no/rom",NULL,4,initial,predraw,after,draw,&output)==GK296_ARGUMENT);
 CHECK(gk296_check_four_writer_first_draw("/no/rom",four,3,initial,predraw,after,draw,&output)==GK296_ARGUMENT);
 CHECK(gk296_check_four_writer_first_draw("/no/rom",four,4,initial,predraw,after,32768,&output)==GK296_ARGUMENT);
 CHECK(memcmp(&output,&sentinel,sizeof output)==0);
 Gk296Witness bad[4];memcpy(bad,four,sizeof bad);
 bad[0].source.cpu_cycle=1005;
 CHECK(gk296_check_four_writer_first_draw("/no/rom",bad,4,initial,predraw,after,draw,&output)==GK296_CHRONOLOGY);
 memcpy(bad,four,sizeof bad);bad[2].source.destination_zp=0x88;
 CHECK(gk296_check_four_writer_first_draw("/no/rom",bad,4,initial,predraw,after,draw,&output)==GK296_MASK);
 memcpy(bad,four,sizeof bad);bad[2].source.destination_zp=0x84;
 CHECK(gk296_check_four_writer_first_draw("/no/rom",bad,4,initial,predraw,after,draw,&output)==GK296_SEED||
       gk296_check_four_writer_first_draw("/no/rom",bad,4,initial,predraw,after,draw,&output)==GK296_MASK);
 memcpy(bad,four,sizeof bad);bad[2].before=99;
 CHECK(gk296_check_four_writer_first_draw("/no/rom",bad,4,initial,predraw,after,draw,&output)==GK296_SEED);
 uint8_t wrong[6];memcpy(wrong,predraw,6);wrong[4]++;
 CHECK(gk296_check_four_writer_first_draw("/no/rom",four,4,initial,wrong,after,draw,&output)==GK296_SEED);
 CHECK(gk296_check_four_writer_first_draw("/no/rom",four,4,initial,predraw,after,draw,&output)==GK296_WRITER_SOURCE);
 CHECK(memcmp(&output,&sentinel,sizeof output)==0);
 app->active_world=0;app->rules_profile=0;
 app->setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
 app->setup_difficulty[0]=1;
 memcpy(before,app,sizeof *before);
 CHECK(gk1_app_source_four_writer_first_draw(app,"/no/rom",four,4,initial,predraw,after,draw,&output)==GK296_WRITER_SOURCE);
 CHECK(!memcmp(before,app,sizeof *app));
 app->rules_profile=1;
 CHECK(gk1_app_source_four_writer_first_draw(app,"/no/rom",four,4,initial,predraw,after,draw,&output)==GK296_STAGE);
 app->rules_profile=0;
 app->worlds[0].countries[0].gold++;
 CHECK(gk1_app_source_four_writer_first_draw(app,"/no/rom",four,4,initial,predraw,after,draw,&output)==GK296_STAGE);
 app->worlds[0].countries[0].gold--;
 CHECK(!memcmp(before,app,sizeof *app));
 if(argc==3){
  for(unsigned scenario=0;scenario<2;scenario++){
   app->active_world=(uint8_t)scenario;
   app->setup_stage[scenario]=GK53_SETUP_AWAIT_SOURCE;
   for(unsigned difficulty=1;difficulty<=5;difficulty++){
    app->setup_difficulty[scenario]=(uint8_t)difficulty;
    memcpy(before,app,sizeof *before);
    CHECK(gk1_app_source_four_writer_first_draw(app,argv[2],four,4,initial,predraw,after,draw,&output)==GK296_OK);
    CHECK(output.source_writer_mask==0x0f&&output.unknown_poweron_mask==0x30);
    CHECK(output.original_rom_sha256_verified&&output.original_source_static_only);
    CHECK(output.first_c289_math_parity&&output.first_draw15==draw);
    CHECK(output.fresh_spring_implemented==0);
    CHECK(!memcmp(before,app,sizeof *app));
   }
  }
  memcpy(bad,four,sizeof bad);bad[0].source.source_pc++;
  CHECK(gk296_check_four_writer_first_draw(argv[2],bad,4,initial,predraw,after,draw,&output)==GK296_WRITER_SOURCE);
  memcpy(bad,four,sizeof bad);bad[1].source.mapped_prg_bank=14;
  CHECK(gk296_check_four_writer_first_draw(argv[2],bad,4,initial,predraw,after,draw,&output)==GK296_WRITER_SOURCE);
  uint8_t wrong_after[6];memcpy(wrong_after,after,6);wrong_after[5]^=1;
  CHECK(gk296_check_four_writer_first_draw(argv[2],four,4,initial,predraw,wrong_after,draw,&output)==GK296_C289);
  CHECK(gk296_check_four_writer_first_draw(argv[2],four,4,initial,predraw,after,(uint16_t)(draw^1u),&output)==GK296_C289);
 }
 CHECK(memcmp(&output,&sentinel,sizeof output)!=0 || argc==2);
 printf("GK296 PASS %u assertions; four source-written bytes, tail RAM origin UNKNOWN; no playable Spring\n",checks);
 free(app);free(before);free(dataset);return 0;
}
