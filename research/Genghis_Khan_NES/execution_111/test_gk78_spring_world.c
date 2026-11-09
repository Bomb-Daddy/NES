#include "gk78_spring_world.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void keep_atomicity_on_guard(const Gk1TemplateDataset *dataset){
 Gk1App app;Gk78SpringReceipt receipt;gk1_app_init(&app);
 assert(gk1_app_install_templates(&app,dataset));
 memset(&receipt,0xA5,sizeof receipt);
 Gk78SpringReceipt saved=receipt;
 Gk1LoadedScenario worlds[2];memcpy(worlds,app.worlds,sizeof worlds);
 assert(gk78_apply_spring_finance(&app,0,&receipt)==GK78_NOT_INITIALIZED);
 assert(gk78_apply_spring_finance(&app,1,&receipt)==GK78_NOT_INITIALIZED);
 assert(!memcmp(&saved,&receipt,sizeof receipt));
 assert(!memcmp(worlds,app.worlds,sizeof worlds));
 assert(gk78_apply_spring_finance(&app,2,&receipt)==GK78_BAD_INPUT);
 assert(gk78_apply_spring_finance(NULL,0,&receipt)==GK78_BAD_INPUT);
 assert(gk78_apply_spring_finance(&app,0,NULL)==GK78_BAD_INPUT);
 assert(!memcmp(worlds,app.worlds,sizeof worlds));
 assert(!memcmp(&saved,&receipt,sizeof receipt));
}
static void simulate_bounded_spring(const Gk1TemplateDataset *data,unsigned world){
 Gk1App app;gk1_app_init(&app);assert(gk1_app_install_templates(&app,data));
 Gk1LoadedScenario other=app.worlds[!world];
 Gk1LoadedScenario *s=&app.worlds[world];
 s->phase=(Gk1ScenarioPhase)GK71_RESERVED_VERIFIED_PLAYABLE_PHASE;
 Gk1LoadedScenario before=*s;
 Gk78SpringReceipt got={0};
 assert(gk78_apply_spring_finance(&app,(uint8_t)world,&got)==GK78_OK);
 assert(got.countries==(world?27:14));
 assert(!memcmp(&app.worlds[!world],&other,sizeof other));
 for(unsigned i=0;i<got.countries;i++){
  Gk75SpringResult independent;
  Gk1CountryTemplate *c=&app.worlds[world].countries[i];
  Gk1CountryTemplate *prior=&before.countries[i];
  assert(gk75_template_projection(prior,&independent)==GK75_OK);
  assert(c->gold==independent.after.gold && c->goods[10]==independent.after.food);
  assert(c->troops==independent.after.troops);
  assert(got.new_gold[i]==c->gold&&got.new_food[i]==c->goods[10]);
  assert(got.source_gold_before[i]==prior->gold);
  assert(got.source_food_before[i]==prior->goods[10]);
  assert(c->drafted_population==c->troops+c->town+c->masons+c->artisans+c->food_workers);
  assert(c->combined_population==c->drafted_population+c->undrafted_reserve);
  assert(c->country_id==prior->country_id && c->country_sram_base==prior->country_sram_base);
 }
 /* Deliberately corrupted LAST country triggers rollback of all prior
  * countries in the transaction, including receipt and other scenario. */
 s=&app.worlds[world];s->countries[s->country_count-1].tax=101;
 before=*s;memset(&got,0x7E,sizeof got);Gk78SpringReceipt sentinel=got;
 assert(gk78_apply_spring_finance(&app,(uint8_t)world,&got)==GK78_INCONSISTENT_COUNTRY);
 assert(!memcmp(&before,s,sizeof before));assert(!memcmp(&got,&sentinel,sizeof got));
 assert(!memcmp(&app.worlds[!world],&other,sizeof other));
}
static void original_1176_control(const Gk1TemplateDataset *data){
 Gk1App app;gk1_app_init(&app);assert(gk1_app_install_templates(&app,data));
 Gk1LoadedScenario *w=&app.worlds[0];
 w->phase=(Gk1ScenarioPhase)GK71_RESERVED_VERIFIED_PLAYABLE_PHASE;
 Gk1CountryTemplate *c=&w->countries[0];
 /* Previously source-traced test case AFTER common-economy step, NOT the
  * original first Spring and NOT a universal first-year outcome. */
 c->troops=61;c->town=5;c->masons=4;c->artisans=6;c->food_workers=27;
 c->goods[10]=289;c->gold=449;c->economy=177;c->morale=118;c->tax=35;
 c->drafted_population=103;c->combined_population=103+c->undrafted_reserve;
 Gk78SpringReceipt got;
 assert(gk78_apply_spring_finance(&app,0,&got)==GK78_OK);
 assert(w->countries[0].goods[10]==238 && w->countries[0].gold==449);
 assert(got.payroll>=61 && got.tax_gross>=61);
}
int main(int argc,char **argv){
 assert(argc==2);
 Gk1TemplateDataset d;assert(gk1_template_load(argv[1],&d)==GK1_LOAD_OK);
 keep_atomicity_on_guard(&d);simulate_bounded_spring(&d,0);
 simulate_bounded_spring(&d,1);original_1176_control(&d);
 puts("EXEC0078 PASS 14/27 source-derived Spring financial transactions: atomic rollback, source-known controlled case, PRE_SETUP locked; full first Spring NOT implemented");
 return 0;
}
