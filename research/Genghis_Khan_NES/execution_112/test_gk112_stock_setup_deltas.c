#include "gk112_stock_setup_deltas.h"
#include "gk1_app_state.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <string.h>
#define REQUIRE(x) do{if(!(x)){fprintf(stderr,"GK112 line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static int run_case(const char *csv,const Gk1TemplateDataset *dataset,
 const char *path,Gk77Profile profile,Gk79Checkpoint *saved){
 Gk1ScenarioId id=profile==GK77_ARCHIVED_WORLD_SAVERAM?
  GK1_WORLD_CONQUEST:GK1_MONGOL_CONQUEST;
 Gk1LoadedScenario template_world={0};
 REQUIRE(gk1_open_loaded_scenario(dataset,id,&template_world)==GK1_LOAD_OK);
 REQUIRE(gk79_open_original(&template_world,path,profile,0,saved)==GK79_OK);
 Gk79Checkpoint before=*saved;
 Gk1LoadedScenario original=template_world;
 Gk112StockDeltas metrics={0};
 REQUIRE(gk112_compare_setup(csv,&template_world,saved,&metrics)==GK112_OK);
 REQUIRE(memcmp(saved,&before,sizeof before)==0);
 REQUIRE(memcmp(&template_world,&original,sizeof original)==0);
 REQUIRE(metrics.profile==profile&&metrics.country_count==(id==GK1_MONGOL_CONQUEST?14:27));
 REQUIRE(!metrics.setup_resolved&&!metrics.first_spring_ready&&!metrics.orders_granted);
 REQUIRE(gk112_field_name(GK112_FOOD)!=NULL&&gk112_field_name(24)==NULL);
 for(unsigned f=0;f<GK112_METRIC_COUNT;f++){
  unsigned changed=0,up=0,down=0,totalabs=0;
  int signed_total=0;
  for(unsigned c=0;c<metrics.country_count;c++){
   int32_t d=metrics.delta[c][f];
   changed+=(d!=0);up+=(d>0);down+=(d<0);
   totalabs+=(unsigned)(d<0?-d:d);signed_total+=d;
  }
  REQUIRE(metrics.countries_changed[f]==changed);
  REQUIRE(metrics.countries_increased[f]==up&&metrics.countries_decreased[f]==down);
  REQUIRE(metrics.countries_changed[f]==up+down);
  REQUIRE(metrics.signed_total[f]==signed_total&&metrics.absolute_total[f]==totalabs);
 }
 for(unsigned c=0;c<metrics.country_count;c++){
  unsigned n=0;
  for(unsigned f=0;f<GK112_METRIC_COUNT;f++)n+=(metrics.delta[c][f]!=0);
  REQUIRE(metrics.fields_changed_in_country[c]==n);
 }
 Gk1App app;gk1_app_init(&app);
 REQUIRE(gk1_app_install_templates(&app,dataset));
 app.stock_checkpoint=before;
 Gk1App app_before=app;
 Gk112StockDeltas app_metrics={0};
 REQUIRE(gk1_app_stock_setup_deltas(&app,csv,&app_metrics)==GK112_OK);
 REQUIRE(memcmp(&app_metrics,&metrics,sizeof metrics)==0);
 REQUIRE(memcmp(&app,&app_before,sizeof app)==0);
 Gk112StockDeltas denied,sentinel;memset(&denied,0x45,sizeof denied);sentinel=denied;
 /* Corrupting any nonselected original record must refuse, without either
  * output mutation or automatic phase/order grant. */
 saved->stock_raw[saved->world.country_count-1][10]^=0x01;
 REQUIRE(gk112_compare_setup(csv,&template_world,saved,&denied)==GK112_STOCK_REFUSED);
 REQUIRE(memcmp(&denied,&sentinel,sizeof denied)==0);
 *saved=before;
 saved->world.countries[saved->world.country_count-1].skill++;
 REQUIRE(gk112_compare_setup(csv,&template_world,saved,&denied)==GK112_STOCK_REFUSED);
 REQUIRE(memcmp(&denied,&sentinel,sizeof denied)==0);
 *saved=before;
 template_world.countries[template_world.country_count-1].morale++;
 REQUIRE(gk112_compare_setup(csv,&template_world,saved,&denied)==GK112_TEMPLATE_REFUSED);
 REQUIRE(memcmp(&denied,&sentinel,sizeof denied)==0);
 template_world=original;
 REQUIRE(gk112_compare_setup("/missing/source.csv",&template_world,saved,&denied)==GK112_TEMPLATE_REFUSED);
 REQUIRE(memcmp(&denied,&sentinel,sizeof denied)==0);
 REQUIRE(gk112_compare_setup(csv,NULL,saved,&denied)==GK112_ARGUMENT);
 REQUIRE(gk112_compare_setup(csv,&template_world,saved,NULL)==GK112_ARGUMENT);
 REQUIRE(memcmp(&denied,&sentinel,sizeof denied)==0);
 *saved=before;
 printf("GK112 AUTHENTIC PROFILE %u: %u countries; source/stock guarded 24 fields; no automatic phase/order grant\n",
  (unsigned)profile,(unsigned)metrics.country_count);
 return 0;
}
int main(int argc,char **argv){
 REQUIRE(argc==2||argc==5);
 REQUIRE(gk1_verified_original_csv(argv[1]));
 Gk1TemplateDataset dataset;
 REQUIRE(gk1_template_load(argv[1],&dataset)==GK1_LOAD_OK);
 Gk1LoadedScenario pre={0};
 REQUIRE(gk1_open_loaded_scenario(&dataset,GK1_MONGOL_CONQUEST,&pre)==GK1_LOAD_OK);
 Gk79Checkpoint false_cp={0};Gk112StockDeltas output,sentinel;
 memset(&output,0x65,sizeof output);sentinel=output;
 REQUIRE(gk112_compare_setup(argv[1],&pre,&false_cp,&output)==GK112_STOCK_REFUSED);
 REQUIRE(memcmp(&output,&sentinel,sizeof output)==0);
 Gk112PairedHistories denied,denied_sentinel;
 memset(&denied,0x77,sizeof denied);denied_sentinel=denied;
 REQUIRE(gk112_compare_mongol_histories(argv[1],&pre,&false_cp,&false_cp,&denied)==GK112_PROFILE_REFUSED);
 REQUIRE(memcmp(&denied,&denied_sentinel,sizeof denied)==0);
 if(argc==5){
  Gk79Checkpoint fresh,mongol,world;
  REQUIRE(!run_case(argv[1],&dataset,argv[2],GK77_FRESH_MONGOL_WORKRAM,&fresh));
  REQUIRE(!run_case(argv[1],&dataset,argv[3],GK77_ARCHIVED_MONGOL_SAVERAM,&mongol));
  REQUIRE(!run_case(argv[1],&dataset,argv[4],GK77_ARCHIVED_WORLD_SAVERAM,&world));
  Gk79Checkpoint preserved_fresh=fresh,preserved_mongol=mongol;
  Gk112PairedHistories pair={0};
  REQUIRE(gk112_compare_mongol_histories(argv[1],&pre,&fresh,&mongol,&pair)==GK112_OK);
  REQUIRE(memcmp(&fresh,&preserved_fresh,sizeof fresh)==0);
  REQUIRE(memcmp(&mongol,&preserved_mongol,sizeof mongol)==0);
  REQUIRE(pair.country_count==14 && pair.first_difference_country>=1 && pair.first_difference_country<=14);
  unsigned changed_fields=0;
  for(unsigned c=0;c<14;c++){
   unsigned changed=0;
   for(unsigned f=0;f<24;f++)changed+=(pair.fresh_minus_archived[c][f]!=0);
   REQUIRE(changed==pair.fields_changed_in_country[c]);changed_fields+=changed;
  }
  unsigned changed_by_field=0;
  for(unsigned f=0;f<24;f++){
   unsigned n=0,abs=0;
   for(unsigned c=0;c<14;c++){
    int32_t d=pair.fresh_minus_archived[c][f];n+=(d!=0);abs+=(unsigned)(d<0?-d:d);
   }
   REQUIRE(n==pair.countries_changed[f]&&abs==pair.absolute_total[f]);changed_by_field+=n;
  }
  REQUIRE(changed_fields==changed_by_field&&changed_fields>0);
  REQUIRE(gk112_compare_mongol_histories(argv[1],&pre,&mongol,&fresh,&denied)==GK112_PROFILE_REFUSED);
  REQUIRE(memcmp(&denied,&denied_sentinel,sizeof denied)==0);
  fresh.stock_raw[13][42]^=1;
  REQUIRE(gk112_compare_mongol_histories(argv[1],&pre,&fresh,&mongol,&denied)==GK112_STOCK_REFUSED);
  REQUIRE(memcmp(&denied,&denied_sentinel,sizeof denied)==0);
  printf("GK112 PAIRED DIFFERENT ORIGINAL MONGOL HISTORIES: %u field/country mismatches; not RNG formula\n",changed_fields);
 }else puts("GK112 PUBLIC NEGATIVE CONTROLS PASS; PRIVATE ORIGINAL CAPTURES NOT RUN");
 return 0;
}
