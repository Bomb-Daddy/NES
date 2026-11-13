/* EXEC0264: a new campaign must never inherit the prior source-attested
 * Classic/Enhanced controller, budget, or private save request. No ROM data
 * is used in this lifecycle test and no original Spring is generated. */
#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks=0;
#define CK(x) do{++checks;if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);exit(1);}}while(0)
int main(int argc,char **argv){
 if(argc!=2)return 2;
 Gk1TemplateDataset *ds=malloc(sizeof *ds);
 Gk1App *s=malloc(sizeof *s),*before=malloc(sizeof *before);
 if(!ds||!s||!before)return 3;
 CK(gk1_template_load(argv[1],ds)==GK1_LOAD_OK);
 gk1_app_init(s);CK(gk1_app_install_templates(s,ds));
 for(unsigned profile=0;profile<=1;profile++){
  s->rules_profile=(uint8_t)profile;
  s->screen=GK1_SCREEN_STOCK_COMMANDS;
  s->stock_checkpoint.valid=1;
  s->stock_checkpoint.source_verified=1;
  s->stock_checkpoint.pending_pass=1;
  s->stock_checkpoint.stock_raw[0][0]=37;
  s->classic_assign.active=1;
  s->enhanced_assign.active=1;
  s->enhanced_summer.active=1;
  s->enhanced_fall.active=1;
  s->enhanced_winter.active=1;
  s->stock_storage_request=2;
  s->stock_storage_confirm=1;
  s->stock_command_cursor=13;
  s->country_cursor[0]=13;s->country_cursor[1]=26;
  s->worlds[0].countries[0].gold=777;
  s->worlds[1].countries[0].gold=888;
  s->setup_candidate[0]=0;
  s->setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
  /* A malformed replacement is atomic, even with live old state. */
  memcpy(before,s,sizeof *s);
  ds->countries[0].country_id=99;
  CK(!gk1_app_install_templates(s,ds));
  CK(memcmp(s,before,sizeof *s)==0);
  ds->countries[0].country_id=1;
  CK(gk1_app_install_templates(s,ds));
  CK(s->rules_profile==profile);
  CK(s->screen==GK1_SCREEN_TITLE);
  CK(s->stock_checkpoint.valid==0 && s->stock_checkpoint.source_verified==0);
  CK(s->stock_checkpoint.pending_pass==0 && s->stock_checkpoint.stock_raw[0][0]==0);
  CK(s->classic_assign.active==0 && s->enhanced_assign.active==0);
  CK(s->enhanced_summer.active==0 && s->enhanced_fall.active==0 && s->enhanced_winter.active==0);
  CK(s->stock_storage_request==0 && s->stock_storage_confirm==0);
  CK(s->stock_command_cursor==0 && s->country_cursor[0]==0 && s->country_cursor[1]==0);
  CK(s->setup_candidate[0]==255 && s->setup_stage[0]==GK53_SETUP_PLAYERS);
  CK(s->world_ready[0]==1 && s->world_ready[1]==1);
  CK(s->worlds[0].phase==GK1_PRE_SETUP_TEMPLATE && s->worlds[1].phase==GK1_PRE_SETUP_TEMPLATE);
  CK(s->worlds[0].country_count==14 && s->worlds[1].country_count==27);
  CK(s->worlds[0].countries[0].gold==425);
  CK(s->worlds[1].countries[0].gold==ds->countries[14].gold);
  CK(gk1_app_input(s,GK1_INPUT_CONFIRM)==1 && s->screen==GK1_SCREEN_CAMPAIGNS);
 }
 printf("EXEC0264 atomic fresh campaign/safe source reset: %u checks PASS\n",checks);
 free(before);free(s);free(ds);return 0;
}
