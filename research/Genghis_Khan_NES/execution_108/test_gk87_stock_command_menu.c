#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(x) do{if(!(x)){fprintf(stderr,"GK87 FAILED line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static int exercise(const Gk1LoadedScenario *world,Gk77Profile profile){
 Gk77StockSpring origin={0}; origin.profile=profile;
 origin.scenario=(uint8_t)world->scenario_id;
 origin.country_count=world->country_count;
 origin.year=(uint16_t)(origin.scenario==1?1175:1206);
 origin.rate=origin.scenario==1?123u:117u;origin.orders=3;
 for(unsigned i=0;i<world->country_count;i++){
  const Gk1CountryTemplate *c=&world->countries[i];Gk77Country *t=&origin.countries[i];
  t->arms=c->arms;t->skill=c->skill;t->troops=c->troops;
  t->economy=c->economy;t->town=c->town;t->defense=c->defense;
  t->masons=c->masons;t->artisans=c->artisans;
  t->food_workers=c->food_workers;t->gold=c->gold;
  t->morale=c->morale;t->tax=c->tax;t->food=c->goods[10];
  t->undrafted=c->undrafted_reserve;
  for(unsigned k=0;k<10;k++)t->goods[k]=c->goods[k];
  for(unsigned j=0;j<83;j++)t->raw[j]=(uint8_t)(i*17+j*13+3);
 }
 Gk1App *app=calloc(1,sizeof *app);REQUIRE(app);
 gk1_app_init(app);
 REQUIRE(gk79_from_decoded(world,&origin,0,&app->stock_checkpoint)==GK79_OK);
 app->screen=GK1_SCREEN_STOCK_CONTINUE;
 const Gk1LoadedScenario untouched=app->stock_checkpoint.world;
 const Gk79Checkpoint start=app->stock_checkpoint;
 REQUIRE(gk1_app_input(app,GK1_INPUT_SETUP)==1);
 REQUIRE(app->screen==GK1_SCREEN_STOCK_COMMANDS);
 for(unsigned i=0;i<14;i++){
  REQUIRE(gk87_stock_command_name(i)!=NULL);
  REQUIRE(app->stock_command_cursor==i);
  REQUIRE(gk1_app_input(app,GK1_INPUT_CONFIRM)==1);
  if(i==6u){REQUIRE(app->screen==GK1_SCREEN_STOCK_CONTINUE);
   REQUIRE(gk1_app_input(app,GK1_INPUT_SETUP)==1);
  }
  else if(i==12u){
   REQUIRE(app->stock_checkpoint.pending_pass==1);
   REQUIRE(gk1_app_input(app,GK1_INPUT_CANCEL)==1);
   REQUIRE(app->stock_checkpoint.pending_pass==0);
   REQUIRE(gk1_app_input(app,GK1_INPUT_CONFIRM)==1);
   REQUIRE(app->stock_checkpoint.pending_pass==1);
   /* Moving cursor cancels an armed Pass: no delayed accidental commit. */
   REQUIRE(gk1_app_input(app,GK1_INPUT_DOWN)==1);
   REQUIRE(app->stock_checkpoint.pending_pass==0);
   REQUIRE(gk1_app_input(app,GK1_INPUT_UP)==1);
   REQUIRE(gk1_app_input(app,GK1_INPUT_CONFIRM)==1);
   REQUIRE(gk1_app_input(app,GK1_INPUT_CONFIRM)==1);
   REQUIRE(app->stock_checkpoint.orders.remaining==0 && app->stock_checkpoint.orders.ended==1);
   REQUIRE(gk1_app_input(app,GK1_INPUT_CONFIRM)==1);
   REQUIRE(app->stock_checkpoint.orders.remaining==0);
  }
  REQUIRE(memcmp(&untouched,&app->stock_checkpoint.world,sizeof untouched)==0);
  REQUIRE(memcmp(start.stock_raw,app->stock_checkpoint.stock_raw,sizeof start.stock_raw)==0);
  REQUIRE(memcmp(start.raw_sha256,app->stock_checkpoint.raw_sha256,32)==0);
  /* Retarget the next item using the actual two-column D-pad layout. */
  unsigned next=(i+1u)%14u;
  if((app->stock_command_cursor/7u)!=(next/7u))
   REQUIRE(gk1_app_input(app,next<7u?GK1_INPUT_LEFT:GK1_INPUT_RIGHT)==1);
  for(unsigned n=0;n<7u && app->stock_command_cursor%7u!=next%7u;n++)
   REQUIRE(gk1_app_input(app,GK1_INPUT_DOWN)==1);
  REQUIRE(app->stock_command_cursor==next);
 }
 REQUIRE(gk87_stock_command_name(14)==NULL);
 REQUIRE(app->stock_command_cursor==0);
 REQUIRE(gk1_app_input(app,GK1_INPUT_CANCEL)==1);
 REQUIRE(app->screen==GK1_SCREEN_STOCK_CONTINUE);
 REQUIRE(memcmp(&untouched,&app->stock_checkpoint.world,sizeof untouched)==0);
 REQUIRE(app->stock_checkpoint.orders.completed==0);
 /* Neither captured profile nor template is mutated, no synthetic season. */
 REQUIRE(app->stock_checkpoint.year==origin.year);
 Gk1App *bad=calloc(1,sizeof *bad);REQUIRE(bad);
 gk1_app_init(bad);bad->screen=GK1_SCREEN_STOCK_CONTINUE;
 bad->stock_checkpoint=start;bad->stock_checkpoint.stock_raw[0][0]^=0x80u;
 const Gk79Checkpoint invalid=bad->stock_checkpoint;
 REQUIRE(gk1_app_input(bad,GK1_INPUT_SETUP)==1);
 REQUIRE(bad->screen==GK1_SCREEN_STOCK_CONTINUE);
 REQUIRE(memcmp(&invalid,&bad->stock_checkpoint,sizeof invalid)==0);
 bad->screen=GK1_SCREEN_STOCK_COMMANDS;
 REQUIRE(gk1_app_input(bad,GK1_INPUT_CONFIRM)==0);
 REQUIRE(memcmp(&invalid,&bad->stock_checkpoint,sizeof invalid)==0);
 free(bad);free(app);return 0;
}
int main(int argc,char **argv){
 REQUIRE(argc==2);
 Gk1TemplateDataset *all=malloc(sizeof *all);REQUIRE(all);
 Gk1LoadedScenario *w=malloc(sizeof *w);REQUIRE(w);
 REQUIRE(gk1_template_load(argv[1],all)==GK1_LOAD_OK);
 REQUIRE(gk1_open_loaded_scenario(all,GK1_MONGOL_CONQUEST,w)==GK1_LOAD_OK);
 REQUIRE(exercise(w,GK77_FRESH_MONGOL_WORKRAM)==0);
 REQUIRE(gk1_open_loaded_scenario(all,GK1_WORLD_CONQUEST,w)==GK1_LOAD_OK);
 REQUIRE(exercise(w,GK77_ARCHIVED_WORLD_SAVERAM)==0);
 free(w);free(all);
 puts("GK87: integrated 14-command spring menu, 14/27 synthetic controller QA, Pass/VIEW only, no world mutation, rollback PASS");
 return 0;
}
