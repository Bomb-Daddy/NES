#include "gk215_original_spring_gap.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"GK215 failure line %d: %s\n",__LINE__,#x);return 1;}checks++;}while(0)
int main(int argc,char **argv){
 int checks=0;
 if(argc!=2&&argc!=5){fprintf(stderr,"usage: %s CSV [FRESH_RAW ARCHIVED_MONGOL_RAW ARCHIVED_WORLD_RAW]\n",argv[0]);return 2;}
 CHECK(gk1_verified_original_csv(argv[1]));
 Gk1TemplateDataset *all=calloc(1,sizeof *all);
 Gk1LoadedScenario *pre=calloc(1,sizeof *pre),*saved=calloc(1,sizeof *saved);
 Gk79Checkpoint *stock=calloc(1,sizeof *stock),*prior=calloc(1,sizeof *prior);
 Gk215Gap *out=malloc(sizeof *out),*old=malloc(sizeof *old);
 CHECK(all&&pre&&saved&&stock&&prior&&out&&old);
 CHECK(gk1_template_load(argv[1],all)==GK1_LOAD_OK);
 for(unsigned scenario=1;scenario<=2;scenario++){
  CHECK(gk1_open_loaded_scenario(all,(Gk1ScenarioId)scenario,pre)==GK1_LOAD_OK);
  *saved=*pre;memset(out,0xa5,sizeof *out);*old=*out;
  CHECK(gk215_compare_original_spring(NULL,pre,stock,out)==GK215_BAD_ARGUMENT);
  CHECK(!memcmp(old,out,sizeof *out));
  CHECK(gk215_compare_original_spring(argv[1],pre,stock,out)==GK215_SOURCE_REFUSED);
  CHECK(!memcmp(old,out,sizeof *out));
  pre->countries[0].gold++;
  CHECK(gk215_compare_original_spring(argv[1],pre,stock,out)==GK215_TEMPLATE_REFUSED);
  CHECK(!memcmp(old,out,sizeof *out));*pre=*saved;
  pre->country_count--;
  CHECK(gk215_compare_original_spring(argv[1],pre,stock,out)==GK215_TEMPLATE_REFUSED);
  CHECK(!memcmp(old,out,sizeof *out));*pre=*saved;
  CHECK(gk215_compare_original_spring("/definitely/noncanonical.csv",pre,stock,out)==GK215_TEMPLATE_REFUSED);
  CHECK(!memcmp(old,out,sizeof *out));
 }
 if(argc==5){
  for(unsigned profile=1;profile<=3;profile++){
   unsigned scenario=profile==3u?2u:1u;
   CHECK(gk1_open_loaded_scenario(all,(Gk1ScenarioId)scenario,pre)==GK1_LOAD_OK);
   CHECK(gk79_open_original(pre,argv[profile+1],(Gk77Profile)profile,0,stock)==GK79_OK);
   *saved=*pre;*prior=*stock;
   CHECK(gk215_compare_original_spring(argv[1],pre,stock,out)==GK215_OK);
   CHECK(out->profile==profile&&out->scenario==scenario);
   CHECK(out->country_count==(scenario==1u?14u:27u));
   CHECK(out->source_observation_only==1u&&!out->generated_setup&&
     !out->generated_rng&&!out->generated_ai&&!out->orders_granted);
   CHECK(out->differing_countries+out->zero_residual_countries==out->country_count);
   CHECK(out->differing_countries>0u);
   CHECK(out->gold_mismatches<=out->country_count &&
         out->food_mismatches<=out->country_count &&
         out->troop_mismatches<=out->country_count);
   CHECK(!memcmp(pre,saved,sizeof *pre)&&!memcmp(stock,prior,sizeof *stock));
   printf("OBSERVED_RUNTIME_STOCK_%s (ARCHIVED SOURCE, NOT NEW MESEN): countries=%u finance_model_differing=%u gold_mismatches=%u food_mismatches=%u troop_mismatches=%u labor_mismatches=%u signed_gold_gap=%lld food_gap=%lld troops_gap=%lld, NO independently generated Spring\n",
     profile==1?"FRESH_WORKRAM":"ARCHIVED_SAVERAM",
     out->country_count,out->differing_countries,out->gold_mismatches,
     out->food_mismatches,out->troop_mismatches,out->workforce_mismatches,
     (long long)out->gold_residual_sum,(long long)out->food_residual_sum,
     (long long)out->troop_residual_sum);
   *old=*out;
   stock->stock_raw[0][0]^=1u;
   CHECK(gk215_compare_original_spring(argv[1],pre,stock,out)==GK215_SOURCE_REFUSED);
   CHECK(!memcmp(old,out,sizeof *out));*stock=*prior;
   stock->raw_sha256[0]^=1u;
   CHECK(gk215_compare_original_spring(argv[1],pre,stock,out)==GK215_SOURCE_REFUSED);
   CHECK(!memcmp(old,out,sizeof *out));*stock=*prior;
   stock->world.countries[0].gold++;
   CHECK(gk215_compare_original_spring(argv[1],pre,stock,out)==GK215_SOURCE_REFUSED);
   CHECK(!memcmp(old,out,sizeof *out));*stock=*prior;
   stock->orders.remaining=2;
   CHECK(gk215_compare_original_spring(argv[1],pre,stock,out)==GK215_SOURCE_REFUSED);
   CHECK(!memcmp(old,out,sizeof *out));*stock=*prior;
   stock->year++;
   CHECK(gk215_compare_original_spring(argv[1],pre,stock,out)==GK215_SOURCE_REFUSED);
   CHECK(!memcmp(old,out,sizeof *out));*stock=*prior;
   stock->source_verified=0;
   CHECK(gk215_compare_original_spring(argv[1],pre,stock,out)==GK215_SOURCE_REFUSED);
   CHECK(!memcmp(old,out,sizeof *out));*stock=*prior;
  }
 }else puts("PUBLIC NEGATIVE QA only; private original-stock positive paths not supplied, no authentic-source execution claimed.");
 printf("GK215 %d assertions PASSED; this diagnostic CANNOT generate original first Spring.\n",checks);
 free(all);free(pre);free(saved);free(stock);free(prior);free(out);free(old);
 return 0;
}
