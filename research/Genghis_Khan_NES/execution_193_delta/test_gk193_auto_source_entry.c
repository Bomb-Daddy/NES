#include "gk193_auto_source_entry.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do {if(!(c)){fprintf(stderr,"GK193 FAIL line %d: %s\n",__LINE__,#c);return 1;}checks++;}while(0)
static unsigned checks=0;
static Gk1App *app,*baseline,*snapshot;
static int original_templates(const char *csv){
 Gk1TemplateDataset *d=malloc(sizeof *d);
 if(!d)return 0;
 int ok=gk1_template_load(csv,d)==GK1_LOAD_OK&&gk1_app_install_templates(app,d);
 free(d);return ok;
}
int main(int argc,char **argv){
 if(argc!=2&&argc!=5){fprintf(stderr,"Usage: test_gk193_auto_source_entry ORIGINAL_CSV [PRIVATE_FRESH_JSON PRIVATE_ARCHIVED_MONGOL_MSS PRIVATE_ARCHIVED_WORLD_MSS]\n");return 3;}
 app=malloc(sizeof *app);baseline=malloc(sizeof *baseline);snapshot=malloc(sizeof *snapshot);
 CHECK(app&&baseline&&snapshot);
 gk1_app_init(app);app->rules_profile=0u;CHECK(original_templates(argv[1]));
 CHECK(app->world_ready[0]&&app->world_ready[1]);
 CHECK(app->worlds[0].phase==GK1_PRE_SETUP_TEMPLATE);
 *baseline=*app;
 Gk77Profile detected=GK77_ARCHIVED_WORLD_SAVERAM;
 CHECK(gk193_enter_original_auto(NULL,argv[1],"x",0,&detected)==GK193_ARGUMENT);
 CHECK(gk193_enter_original_auto(app,argv[1],"x",0,NULL)==GK193_ARGUMENT);
 CHECK(gk193_enter_original_auto(app,argv[1],"x",1,&detected)==GK193_ARGUMENT);
 CHECK(gk193_enter_original_auto(app,argv[1],"/missing/gk193-source",0,&detected)==GK193_UNAVAILABLE);
 CHECK(detected==GK77_ARCHIVED_WORLD_SAVERAM);
 CHECK(memcmp(app,baseline,sizeof *app)==0);
 app->rules_profile=1u;
 CHECK(gk193_enter_original_auto(app,argv[1],"/missing/gk193-source",0,&detected)==GK193_TEMPLATE);
 CHECK(app->rules_profile==1u);
 *app=*baseline;
 /* A real snapshot must not be inferred from a renamed fake header. */
 const char *bad_path="gk193_bad_private_source.tmp";
 FILE *bad=fopen(bad_path,"wbx");CHECK(bad!=NULL);
 CHECK(fwrite("MSS\1",1,4,bad)==4);CHECK(fclose(bad)==0);
 CHECK(gk193_enter_original_auto(app,argv[1],bad_path,0,&detected)==GK193_SOURCE);
 CHECK(memcmp(app,baseline,sizeof *app)==0);
 CHECK(remove(bad_path)==0);
 if(argc==5){
  const Gk77Profile profiles[3]={GK77_FRESH_MONGOL_WORKRAM,
   GK77_ARCHIVED_MONGOL_SAVERAM,GK77_ARCHIVED_WORLD_SAVERAM};
  const unsigned counts[3]={14,14,27};
  const unsigned gold[3]={430,439,193};
  const unsigned rate[3]={121,101,114};
  const unsigned food[3]={208,208,332};
  for(unsigned i=0;i<3;i++){
   *app=*baseline;
   detected=GK77_ARCHIVED_WORLD_SAVERAM;
   CHECK(gk193_enter_original_auto(app,argv[1],argv[2+i],0,&detected)==GK193_OK);
   CHECK(detected==profiles[i]);
   CHECK(app->screen==GK1_SCREEN_STOCK_CONTINUE);
   CHECK(app->stock_checkpoint.valid&&app->stock_checkpoint.source_verified);
   CHECK(app->stock_checkpoint.profile==profiles[i]);
   CHECK(app->stock_checkpoint.orders.remaining==3);
   CHECK(app->stock_checkpoint.world.country_count==counts[i]);
   CHECK(app->stock_checkpoint.world.countries[0].gold==gold[i]);
   CHECK(app->stock_checkpoint.world.countries[0].goods[10]==food[i]);
   CHECK(app->stock_checkpoint.rate==rate[i]);
   CHECK(gk85_raw_source_intact(&app->stock_checkpoint));
   CHECK(gk88_stock_projection_intact(&app->stock_checkpoint));
   CHECK(memcmp(app->worlds,baseline->worlds,sizeof app->worlds)==0);
   *snapshot=*app;
   CHECK(gk193_enter_original_auto(app,"/invalid-canonical-csv",argv[2+i],0,&detected)==GK193_TEMPLATE);
   CHECK(memcmp(app,snapshot,sizeof *app)==0);
   CHECK(gk193_enter_original_auto(app,argv[1],bad_path,0,&detected)==GK193_UNAVAILABLE);
   CHECK(memcmp(app,snapshot,sizeof *app)==0);
   CHECK(detected==profiles[i]);
  }
 }
 printf("TESTED_PORT_LINUX GK193 %u assertions PASS; authentic private all-3 profiles %s; NO independent NES Spring RNG or full campaign\n",
   checks,argc==5?"YES":"NOT RUN");
 free(snapshot);free(baseline);free(app);return 0;
}
