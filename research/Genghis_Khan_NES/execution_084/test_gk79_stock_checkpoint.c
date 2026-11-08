#include "gk1_app_state.h"
#include "gk79_stock_checkpoint.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Synthetic decoded-port fixture, NEVER called an original NES ROM run.
 * Real production path calls gk77_load -> gk79_open_original (SHA gate).
 */
static void sample(const Gk1LoadedScenario *w,Gk77Profile profile,Gk77StockSpring *s){
 memset(s,0,sizeof *s);
 s->profile=profile;
 s->scenario=(uint8_t)w->scenario_id;
 s->country_count=w->country_count;
 s->year=(uint16_t)(s->scenario==1?1175:1206);
 s->rate=137;
 s->orders=3;
 for(unsigned i=0;i<w->country_count;i++){
  const Gk1CountryTemplate *c=&w->countries[i];
  Gk77Country *out=&s->countries[i];
  out->arms=c->arms;out->skill=c->skill;out->troops=c->troops;
  out->economy=c->economy;out->town=c->town;out->defense=c->defense;
  out->masons=c->masons;out->artisans=c->artisans;
  out->food_workers=c->food_workers;out->gold=c->gold;
  out->morale=c->morale;out->tax=c->tax;out->food=c->goods[10];
  out->undrafted=c->undrafted_reserve;
  for(unsigned j=0;j<10;j++)out->goods[j]=c->goods[j];
 }
 /* A synthetic test deliberately varies initial state to verify NO fixed
  * first Spring Gold/Rate values were embedded by this native port. */
 s->countries[0].gold=608;
 s->countries[0].food=501;
 s->countries[0].troops=70;
 s->countries[0].goods[0]=23;
}
int main(int argc,char **argv){
 assert(argc==2 || argc==5);
 Gk1TemplateDataset *all=malloc(sizeof *all);assert(all);
 assert(gk1_template_load(argv[1],all)==GK1_LOAD_OK);
 Gk1App *app=malloc(sizeof *app);assert(app);
 gk1_app_init(app);assert(gk1_app_install_templates(app,all));
 Gk1App *before=malloc(sizeof *before);assert(before);*before=*app;
 Gk79Checkpoint cp;memset(&cp,0xa5,sizeof cp);
 Gk79Checkpoint prior=cp;
 assert(gk79_open_original(&app->worlds[0],"not-a-real-file",GK77_FRESH_MONGOL_WORKRAM,0,&cp)==GK79_SOURCE_REJECTED);
 assert(memcmp(&cp,&prior,sizeof cp)==0);
 Gk77StockSpring s;
 sample(&app->worlds[0],GK77_FRESH_MONGOL_WORKRAM,&s);
 assert(gk79_from_decoded(&app->worlds[0],&s,0,&cp)==GK79_OK);
 assert(cp.valid&&cp.year==1175&&cp.rate==137&&cp.orders.remaining==3);
 assert(cp.world.phase==(Gk1ScenarioPhase)GK71_RESERVED_VERIFIED_PLAYABLE_PHASE);
 assert(cp.world.countries[0].gold==608&&cp.world.countries[0].goods[10]==501);
 assert(cp.world.countries[0].troops==70&&cp.world.countries[0].goods[0]==23);
 assert(cp.world.countries[0].drafted_population==
        cp.world.countries[0].troops+cp.world.countries[0].town+
        cp.world.countries[0].masons+cp.world.countries[0].artisans+
        cp.world.countries[0].food_workers);
 assert(memcmp(before,app,sizeof *app)==0); /* PRE_SETUP app never touched */
 assert(gk79_pass(&cp)==GK79_OK);
 assert(cp.orders.ended&&cp.orders.remaining==0&&cp.orders.completed==0);
 Gk79Checkpoint passed=cp;
 assert(gk79_pass(&cp)==GK79_EXHAUSTED&&memcmp(&passed,&cp,sizeof cp)==0);
 assert(memcmp(before,app,sizeof *app)==0);
 /* Malicious profile/season/count/player rejected atomically. */
 memset(&cp,0xa5,sizeof cp);prior=cp;
 s.profile=GK77_ARCHIVED_WORLD_SAVERAM;
 assert(gk79_from_decoded(&app->worlds[0],&s,0,&cp)==GK79_BAD);
 assert(memcmp(&cp,&prior,sizeof cp)==0);
 s.profile=GK77_FRESH_MONGOL_WORKRAM;s.orders=2;
 assert(gk79_from_decoded(&app->worlds[0],&s,0,&cp)==GK79_BAD);
 assert(memcmp(&cp,&prior,sizeof cp)==0);
 s.orders=3;s.countries[13].troops=65535;
 assert(gk79_from_decoded(&app->worlds[0],&s,0,&cp)==GK79_BAD);
 assert(memcmp(&cp,&prior,sizeof cp)==0);
 s.countries[13].troops=before->worlds[0].countries[13].troops;
 assert(gk79_from_decoded(&app->worlds[0],&s,1,&cp)==GK79_UNSUPPORTED_PLAYER);
 assert(memcmp(&cp,&prior,sizeof cp)==0);
 /* World scenario 27 lands with a selectable original manual lord, no RNG. */
 sample(&app->worlds[1],GK77_ARCHIVED_WORLD_SAVERAM,&s);
 assert(gk79_from_decoded(&app->worlds[1],&s,6,&cp)==GK79_OK);
 assert(cp.world.country_count==27&&cp.orders.country_id==7&&cp.year==1206);
 assert(cp.orders.remaining==3);
 assert(memcmp(before,app,sizeof *app)==0);
 /* Route through real game app controller, after an externally authenticated
  * import in production; synthetic fixture here only tests routing. */
 app->stock_checkpoint=cp;
 app->screen=GK1_SCREEN_STOCK_CONTINUE;
 app->stock_checkpoint.pending_pass=0;
 assert(gk1_app_input(app,GK1_INPUT_CONFIRM)==1);
 assert(app->stock_checkpoint.pending_pass==1&&app->stock_checkpoint.orders.remaining==3);
 assert(gk1_app_input(app,GK1_INPUT_CANCEL)==1);
 assert(app->stock_checkpoint.pending_pass==0&&app->stock_checkpoint.orders.remaining==3);
 assert(gk1_app_input(app,GK1_INPUT_CONFIRM)==1);
 assert(gk1_app_input(app,GK1_INPUT_CONFIRM)==1);
 assert(app->stock_checkpoint.orders.remaining==0&&app->stock_checkpoint.orders.ended);
 assert(app->worlds[0].phase==GK1_PRE_SETUP_TEMPLATE && app->worlds[1].phase==GK1_PRE_SETUP_TEMPLATE);
 const char *savefile="gk79_fixture_pass_receipt.gks";
 (void)remove(savefile);
 assert(gk79_save_receipt_new(savefile,&app->stock_checkpoint)==GK79_IO_OK);
 assert(gk79_save_receipt_new(savefile,&app->stock_checkpoint)==GK79_IO_EXISTS);
 /* A cold load must reauthenticate original source from outside the receipt.
  * The test has a deliberately synthetic equivalent source, not real Mesen. */
 Gk79Checkpoint reloaded;
 assert(gk79_from_decoded(&app->worlds[1],&s,6,&reloaded)==GK79_OK);
 assert(reloaded.orders.remaining==3);
 assert(gk79_load_receipt(savefile,&reloaded)==GK79_IO_OK);
 assert(reloaded.orders.remaining==0 && reloaded.orders.ended);
 Gk79Checkpoint unchanged=reloaded;
 assert(gk79_load_receipt("no-such-gk79-file",&reloaded)==GK79_IO_ERROR);
 assert(memcmp(&unchanged,&reloaded,sizeof unchanged)==0);
 FILE *file=fopen(savefile,"r+b");assert(file);
 assert(fseek(file,9,SEEK_SET)==0 && fputc(2,file)!=EOF && fclose(file)==0);
 assert(gk79_load_receipt(savefile,&reloaded)==GK79_IO_CORRUPT);
 assert(memcmp(&unchanged,&reloaded,sizeof unchanged)==0);
 assert(remove(savefile)==0);
 assert(gk1_app_input(app,GK1_INPUT_CANCEL)==1);
 assert(app->screen==GK1_SCREEN_SPRING_ARCHIVE);
 /* OPTIONAL PRIVATE original-stock positive acceptance, outside public CTest.
  * These three source files are NOT bundled in the public source archive.
  * First is fresh nesWorkRam, next 2 archived nesSaveRam.
  */
 if(argc==5){
  const Gk77Profile profiles[3]={GK77_FRESH_MONGOL_WORKRAM,
     GK77_ARCHIVED_MONGOL_SAVERAM,GK77_ARCHIVED_WORLD_SAVERAM};
  const unsigned expected_gold[3]={430u,439u,193u};
  const unsigned expected_food[3]={208u,208u,332u};
  const unsigned expected_troops[3]={61u,61u,116u};
  const unsigned expected_rate[3]={121u,101u,114u};
  const unsigned expected_count[3]={14u,14u,27u};
  for(unsigned j=0;j<3;j++){
   unsigned world=j==2?1u:0u;
   Gk79Checkpoint positive={0};
   assert(gk79_open_original(&app->worlds[world],argv[2+j],profiles[j],0,&positive)==GK79_OK);
   assert(positive.valid&&positive.world.country_count==expected_count[j]);
   assert(positive.world.countries[0].gold==expected_gold[j]);
   assert(positive.world.countries[0].goods[10]==expected_food[j]);
   assert(positive.world.countries[0].troops==expected_troops[j]);
   assert(positive.rate==expected_rate[j]&&positive.orders.remaining==3);
   assert(positive.world.phase==(Gk1ScenarioPhase)GK71_RESERVED_VERIFIED_PLAYABLE_PHASE);
   assert(memcmp(before->worlds,app->worlds,sizeof app->worlds)==0);
   Gk79Checkpoint source_copy=positive;
   positive.view_index=(uint8_t)(expected_count[j]-1u);
   assert(positive.world.countries[positive.view_index].country_id==expected_count[j]);
   positive.view_index=0;
   assert(gk79_pass(&positive)==GK79_OK);
   assert(positive.orders.remaining==0u && positive.orders.ended==1u);
   for(unsigned k=0;k<expected_count[j];k++){
    assert(memcmp(&positive.world.countries[k],&source_copy.world.countries[k],
     sizeof(Gk1CountryTemplate))==0);
   }
   Gk79Checkpoint before_wrong=positive;
   Gk77Profile wrong=profiles[j]==GK77_ARCHIVED_WORLD_SAVERAM?
       GK77_ARCHIVED_MONGOL_SAVERAM:GK77_ARCHIVED_WORLD_SAVERAM;
   assert(gk79_open_original(&app->worlds[world],argv[2+j],wrong,0,&positive)!=GK79_OK);
   assert(memcmp(&before_wrong,&positive,sizeof positive)==0);
  }
  printf("EXEC0079 PRIVATE-source positives accepted: fresh14 Gold430 Rate121, "
         "archived14 Gold439 Rate101, archived27 Gold193 Rate114; "
         "all 41 original records sourced; negative wrong profile rejects; PASS\n");
 }
 free(before);free(app);free(all);
 puts("EXEC0079 synthetic port-conversion, authentic-source hash rejection, controller A/A Pass, B rollback, exclusive source-bound cold receipt, corruption rejection, preserved PRE_SETUP both campaigns PASS");
 return 0;
}
