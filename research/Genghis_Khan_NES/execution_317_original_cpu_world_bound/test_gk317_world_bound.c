/* EXEC0317: do not accept a purported World CPU event transcript while the
 * existing native game is initializing Mongolia, or vice versa. Synthetic
 * claims only; neither original-ROM parity nor first-Spring acceptance. */
#include "gk1_app_state.h"
#include "gk316_cpu_claims_ingest.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define HASH "ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3"
static unsigned checks;
#define CHECK(x) do { ++checks; if(!(x)) { fprintf(stderr,"EXEC0317 FAIL line %d: %s\n",__LINE__,#x);return 2; } } while(0)
static int save(const char *path, int world) {
 FILE *f=fopen(path,"wb");
 if(!f) return 0;
 int ok=fprintf(f,"GK1_STOCK_CPU_CLAIMS_V1,%s,%c,010203040506\nC,7,c289,15,010203040506,111213141516,1\n",HASH,world?'W':'M')>0;
 if(fclose(f)!=0) ok=0;
 return ok;
}
int main(int argc,char **argv) {
 if(argc!=2) return 2;
 Gk1TemplateDataset *dataset=malloc(sizeof *dataset);
 Gk1App *app=malloc(sizeof *app);
 Gk316Trace *receipt=malloc(sizeof *receipt), *untouched=malloc(sizeof *untouched);
 const char *path="gk317_private_synthetic_claim.tmp";
 CHECK(dataset && app && receipt && untouched);
 CHECK(gk1_template_load(argv[1],dataset)==GK1_LOAD_OK);
 gk1_app_init(app);
 CHECK(gk1_app_install_templates(app,dataset));
 CHECK(app->world_ready[0] && app->world_ready[1]);
 CHECK(app->worlds[0].country_count==14 && app->worlds[1].country_count==27);
 CHECK(app->worlds[0].phase==GK1_PRE_SETUP_TEMPLATE && app->worlds[1].phase==GK1_PRE_SETUP_TEMPLATE);
 app->rules_profile=0;
 for(unsigned active=0;active<2;active++) {
  app->active_world=(uint8_t)active;
  app->setup_stage[active]=GK53_SETUP_AWAIT_SOURCE;
  memset(receipt,0xa6,sizeof *receipt);
  memcpy(untouched,receipt,sizeof *receipt);
  CHECK(save(path,1-(int)active));
  CHECK(gk1_app_original_cpu_claims_parse(app,path,receipt)==GK316_STAGE);
  CHECK(memcmp(receipt,untouched,sizeof *receipt)==0); /* atomic refusal */
  CHECK(save(path,(int)active));
  CHECK(gk1_app_original_cpu_claims_parse(app,path,receipt)==GK316_OK);
  CHECK(receipt->world==active && receipt->source_claim_only==1);
  CHECK(receipt->observed_stock_emulator==0 && receipt->first_spring_initialized==0);
  CHECK(receipt->player_orders_granted==0);
  CHECK(app->worlds[0].phase==GK1_PRE_SETUP_TEMPLATE && app->worlds[1].phase==GK1_PRE_SETUP_TEMPLATE);
 }
 app->active_world=0;
 app->rules_profile=1;
 memset(receipt,0x37,sizeof *receipt);
 memcpy(untouched,receipt,sizeof *receipt);
 CHECK(save(path,0));
 CHECK(gk1_app_original_cpu_claims_parse(app,path,receipt)==GK316_STAGE);
 CHECK(memcmp(receipt,untouched,sizeof *receipt)==0);
 CHECK(remove(path)==0);
 printf("EXEC0317 PASS: %u cross-world fail-closed C11 source/port checks; first Spring NOT implemented\n",checks);
 free(untouched);free(receipt);free(app);free(dataset);
 return 0;
}
