#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"GK297 FAIL %u: %s\n",__LINE__,#x);return 1;}checks++;}while(0)
int main(int argc,char **argv){
 unsigned checks=0;
 CHECK(argc==2||argc==3);
 Gk1App *app=calloc(1,sizeof *app),*unchanged=malloc(sizeof *unchanged);
 Gk1TemplateDataset *t=malloc(sizeof *t);
 CHECK(app&&unchanged&&t);
 gk1_app_init(app);
 CHECK(gk1_template_load(argv[1],t)==GK1_LOAD_OK);
 CHECK(gk1_app_install_templates(app,t));
 const uint8_t tails[2][2]={{0,0},{0x5a,0x7d}};
 Gk297ResetSeed out,control;memset(&out,0xa7,sizeof out);control=out;
 CHECK(gk297_reset_initializer_from_source(NULL,tails[0],&out)==GK297_ARGUMENT);
 CHECK(gk297_reset_initializer_from_source("/missing/rom",tails[0],&out)==GK297_ROM_IO);
 CHECK(gk297_reset_initializer_from_source("/missing/rom",NULL,&out)==GK297_ARGUMENT);
 CHECK(memcmp(&out,&control,sizeof out)==0);
 app->active_world=0;app->rules_profile=0;
 app->setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
 app->setup_difficulty[0]=1;
 memcpy(unchanged,app,sizeof *app);
 CHECK(gk1_app_original_reset_initializer(app,"/missing/rom",tails[0],&out)==GK297_ROM_IO);
 CHECK(memcmp(unchanged,app,sizeof *app)==0);
 app->rules_profile=1;
 CHECK(gk1_app_original_reset_initializer(app,"/missing/rom",tails[0],&out)==GK297_STAGE);
 app->rules_profile=0;
 app->worlds[0].countries[0].gold++;
 CHECK(gk1_app_original_reset_initializer(app,"/missing/rom",tails[0],&out)==GK297_STAGE);
 app->worlds[0].countries[0].gold--;
 CHECK(memcmp(unchanged,app,sizeof *app)==0);
 if(argc==3){
  for(unsigned j=0;j<2;j++){
   const uint8_t *tail=tails[j];
   CHECK(gk297_reset_initializer_from_source(argv[2],tail,&out)==GK297_OK);
   CHECK(out.reset_initializer_seed[0]==0x04&&out.reset_initializer_seed[1]==0xd2);
   CHECK(out.reset_initializer_seed[2]==0x04&&out.reset_initializer_seed[3]==0xd2);
   CHECK(out.reset_initializer_seed[4]==tail[0]&&out.reset_initializer_seed[5]==tail[1]);
   CHECK(out.original_source_written_mask==0x0f&&out.externally_observed_ram_mask==0x30);
   CHECK(out.original_rom_sha256_verified&&out.source_static_only);
   CHECK(!out.first_c289_seed_proven&&!out.gameplay_spring_unlocked);
   for(unsigned world=0;world<2;world++){
    app->active_world=(uint8_t)world;
    app->setup_stage[world]=GK53_SETUP_AWAIT_SOURCE;
    for(unsigned difficulty=1;difficulty<=5;difficulty++){
     app->setup_difficulty[world]=(uint8_t)difficulty;
     memcpy(unchanged,app,sizeof *app);
     CHECK(gk1_app_original_reset_initializer(app,argv[2],tail,&out)==GK297_OK);
     CHECK(!memcmp(app,unchanged,sizeof *app));
     CHECK(!out.first_c289_seed_proven&&!out.gameplay_spring_unlocked);
    }
   }
  }
  memset(&out,0xa7,sizeof out);control=out;
  CHECK(gk297_reset_initializer_from_source(argv[1],tails[0],&out)==GK297_ROM_IDENTITY);
  CHECK(memcmp(&out,&control,sizeof out)==0);
 }
 printf("GK297 PASS %u assertions: four source-known initializer bytes, unknown tail and first-C289 history, no Spring unlock\n",checks);
 free(app);free(unchanged);free(t);return 0;
}
