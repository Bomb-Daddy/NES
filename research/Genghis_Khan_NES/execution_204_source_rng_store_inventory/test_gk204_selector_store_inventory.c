#include "gk204_selector_store_inventory.h"
#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks;
#define CHECK(c) do{checks++;if(!(c)){fprintf(stderr,"GK204 FAIL %u: %s\n",__LINE__,#c);return 1;}}while(0)
int main(int argc,char **argv){
 Gk204Inventory result,baseline;
 memset(&result,0xa5,sizeof result);baseline=result;
 CHECK(gk204_inventory_bytes(NULL,0,&result)==GK204_ARGUMENT);
 CHECK(memcmp(&result,&baseline,sizeof result)==0);
 CHECK(gk204_inventory_file(NULL,&result)==GK204_ARGUMENT);
 CHECK(memcmp(&result,&baseline,sizeof result)==0);
 uint8_t *bytes=calloc(262161u,1u);CHECK(bytes!=NULL);
 CHECK(gk204_inventory_bytes(bytes,262160u,&result)==GK204_SOURCE_REFUSED);
 CHECK(memcmp(&result,&baseline,sizeof result)==0);
 CHECK(gk204_inventory_file("/does-not-exist.nes",&result)==GK204_SOURCE_REFUSED);
 CHECK(memcmp(&result,&baseline,sizeof result)==0);
 Gk1App app;gk1_app_init(&app);
 CHECK(gk1_app_source_selector_store_inventory(&app,"notfound",&result)==GK204_ARGUMENT);
 CHECK(memcmp(&result,&baseline,sizeof result)==0);
 if(argc==2){
  CHECK(gk204_inventory_file(argv[1],&result)==GK204_OK);
  CHECK(result.verified_original_stock&&result.source_only_not_runtime);
  CHECK(result.rng_selector==17u&&result.site_count==21u);
  CHECK(result.raw_direct_store_count==9u);
  CHECK(result.raw_direct_rmw_count==5u);
  CHECK(result.raw_indexed_count==6u);
  CHECK(result.raw_absolute_count==1u);
  CHECK(result.adjacent_lda_rng_store_count==0u);
  unsigned fixed=0,matched=0,bank10=0;
  for(unsigned i=0;i<result.site_count;i++){
   Gk204Site *s=&result.sites[i];
   if(s->bank==15){
    fixed++;
    if(s->kind==GK204_RAW_DIRECT_STORE&&s->opcode==0x85u&&
       s->adjacent_lda_immediate&&s->immediate_value==23u&&
       (s->hypothetical_pc==0xef3fu||s->hypothetical_pc==0xef4du))matched++;
   }
   if(s->bank==10)bank10++;
  }
  CHECK(fixed==2u&&matched==2u&&bank10==6u);
  baseline=result;
  FILE *f=fopen(argv[1],"rb");CHECK(f!=NULL);
  CHECK(fread(bytes,1,262161u,f)==262160u);CHECK(fclose(f)==0);
  bytes[16+15*16384+0x2f3f]^=1u;
  CHECK(gk204_inventory_bytes(bytes,262160u,&result)==GK204_SOURCE_REFUSED);
  CHECK(memcmp(&result,&baseline,sizeof result)==0);
  CHECK(gk204_inventory_bytes(bytes,262159u,&result)==GK204_SOURCE_REFUSED);
  CHECK(memcmp(&result,&baseline,sizeof result)==0);
  app.setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
  CHECK(gk1_app_source_selector_store_inventory(&app,argv[1],&result)==GK204_OK);
  CHECK(memcmp(&result,&baseline,sizeof result)==0);
  CHECK(app.screen==GK1_SCREEN_TITLE&&app.stock_checkpoint.valid==0);
  app.setup_stage[0]=GK53_SETUP_PLAYERS;
  CHECK(gk1_app_source_selector_store_inventory(&app,argv[1],&result)==GK204_ARGUMENT);
  CHECK(memcmp(&result,&baseline,sizeof result)==0);
 }
 free(bytes);printf("GK204 PASS %u assertions%s\n",checks,argc==2?" on authenticated private original":" public rejection only");
 return 0;
}
