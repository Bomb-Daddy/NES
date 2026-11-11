#include "gk203_rng_selector_writers.h"
#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks=0;
#define CHECK(x) do{checks++;if(!(x)){fprintf(stderr,"GK203 FAIL %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char **argv){
 Gk203Proof proof,baseline;memset(&proof,0x5a,sizeof proof);baseline=proof;
 CHECK(gk203_from_bytes(NULL,0,&proof)==GK203_ARGUMENT);
 CHECK(memcmp(&proof,&baseline,sizeof proof)==0);
 CHECK(gk203_from_file(NULL,&proof)==GK203_ARGUMENT);
 CHECK(memcmp(&proof,&baseline,sizeof proof)==0);
 uint8_t *fake=calloc(262161u,1u);CHECK(fake!=NULL);
 CHECK(gk203_from_bytes(fake,262160u,&proof)==GK203_SOURCE_REFUSED);
 CHECK(memcmp(&proof,&baseline,sizeof proof)==0);
 CHECK(gk203_from_bytes(fake,262159u,&proof)==GK203_SOURCE_REFUSED);
 CHECK(memcmp(&proof,&baseline,sizeof proof)==0);
 Gk1App app;gk1_app_init(&app);
 CHECK(gk1_app_source_rng_selector_writers(&app,"nope",&proof)==GK203_ARGUMENT);
 CHECK(memcmp(&proof,&baseline,sizeof proof)==0);
 if(argc==2){
  CHECK(gk203_from_file(argv[1],&proof)==GK203_OK);
  CHECK(proof.authenticated_stock&&proof.analysis_only&&proof.rng_slot==17u);
  CHECK(proof.fixed_caller_count==2u&&proof.fixed_rng_caller_count==0u);
  CHECK(proof.fixed[0].pc==0xef3du&&proof.fixed[1].pc==0xef4bu);
  CHECK(proof.fixed[0].selector==23u&&proof.fixed[1].selector==23u);
  CHECK(proof.fixed[0].subcommand==5u&&proof.fixed[1].subcommand==4u);
  CHECK(proof.fixed[0].dispatched_target==0xc419u&&proof.fixed[1].dispatched_target==0xc419u);
  CHECK(proof.raw_immediate_rng_slot_writes==0u);
  CHECK(proof.raw_immediate_other_slot_writes>=2u);
  baseline=proof;
  FILE *f=fopen(argv[1],"rb");CHECK(f!=NULL);
  CHECK(fread(fake,1,262161u,f)==262160u);CHECK(fclose(f)==0);
  fake[16u+15u*16384u]^=1u;
  CHECK(gk203_from_bytes(fake,262160u,&proof)==GK203_SOURCE_REFUSED);
  CHECK(memcmp(&proof,&baseline,sizeof proof)==0);
  app.setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
  CHECK(gk1_app_source_rng_selector_writers(&app,argv[1],&proof)==GK203_OK);
  CHECK(memcmp(&proof,&baseline,sizeof proof)==0);
  CHECK(app.screen==GK1_SCREEN_TITLE&&!app.stock_checkpoint.valid);
  app.setup_stage[0]=GK53_SETUP_PLAYERS;
  CHECK(gk1_app_source_rng_selector_writers(&app,argv[1],&proof)==GK203_ARGUMENT);
  CHECK(memcmp(&proof,&baseline,sizeof proof)==0);
 }
 free(fake);
 printf("GK203 PASS %u assertions%s\n",checks,argc==2?" private original stock":" public refusal");
 return 0;
}
