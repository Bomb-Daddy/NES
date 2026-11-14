#include "gk278_original_spring_coverage.h"
#include "gk1_source_identity.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"GK278 FAILED line %d: %s\n",__LINE__,#c);return 1;}checks++;}while(0)
int main(int argc,char **argv){
 unsigned checks=0;
 if(argc!=2&&argc!=5){fprintf(stderr,"usage: %s CSV [PRIVATE_FRESH PRIVATE_ARCHIVED_MONGOL PRIVATE_ARCHIVED_WORLD]\n",argv[0]);return 2;}
 CHECK(GK278_FIELD_COUNT==24u);
 CHECK(gk278_field_name(GK278_ARMS)!=NULL);
 CHECK(strcmp(gk278_field_name(GK278_FOOD),"Food")==0);
 CHECK(gk278_field_name((Gk278Field)99)==NULL);
 CHECK(gk1_verified_original_csv(argv[1]));
 Gk1TemplateDataset *all=calloc(1,sizeof *all);
 Gk1LoadedScenario *pre=calloc(1,sizeof *pre);
 Gk79Checkpoint *stock=calloc(1,sizeof *stock);
 Gk278Coverage *gap=malloc(sizeof *gap),*old=malloc(sizeof *old);
 CHECK(all&&pre&&stock&&gap&&old);
 CHECK(gk1_template_load(argv[1],all)==GK1_LOAD_OK);
 for(unsigned w=1;w<=2;w++){
  CHECK(gk1_open_loaded_scenario(all,(Gk1ScenarioId)w,pre)==GK1_LOAD_OK);
  memset(gap,0xa7,sizeof *gap);*old=*gap;
  CHECK(gk278_compare_original_spring_coverage(NULL,pre,stock,gap)==GK278_ARGUMENT);
  CHECK(!memcmp(old,gap,sizeof *gap));
  CHECK(gk278_compare_original_spring_coverage(argv[1],pre,stock,gap)==GK278_SOURCE);
  CHECK(!memcmp(old,gap,sizeof *gap));
  CHECK(gk278_compare_original_spring_coverage("/noncanonical.csv",pre,stock,gap)==GK278_TEMPLATE);
  CHECK(!memcmp(old,gap,sizeof *gap));
  pre->countries[0].skill++;
  CHECK(gk278_compare_original_spring_coverage(argv[1],pre,stock,gap)==GK278_TEMPLATE);
  CHECK(!memcmp(old,gap,sizeof *gap));
 }
 if(argc==5){
  Gk278Coverage *fresh=malloc(sizeof *fresh),*arch=malloc(sizeof *arch);
  CHECK(fresh&&arch);
  for(unsigned p=1;p<=3;p++){
   unsigned w=p==3u?2u:1u;
   CHECK(gk1_open_loaded_scenario(all,(Gk1ScenarioId)w,pre)==GK1_LOAD_OK);
   CHECK(gk79_open_original(pre,argv[1+p],(Gk77Profile)p,0,stock)==GK79_OK);
   Gk79Checkpoint *saved=malloc(sizeof *saved);CHECK(saved);
   *saved=*stock;
   CHECK(gk278_compare_original_spring_coverage(argv[1],pre,stock,gap)==GK278_OK);
   CHECK(gap->scenario==w&&gap->profile==p&&gap->country_count==(w==1u?14u:27u));
   CHECK(gap->source_observation_only==1u&&!gap->independent_initialization&&
    !gap->generated_rng&&!gap->generated_ai&&!gap->playable_order_granted);
   CHECK(gap->compared_cells==(w==1u?336u:648u));
   CHECK(gap->exact_cells+gap->unequal_cells==gap->compared_cells);
   CHECK(gap->unequal_cells>0u&&gap->countries_with_differences>0u);
   unsigned field_unequal_sum=0u;
   for(unsigned f=0;f<GK278_FIELD_COUNT;f++){
    CHECK(gap->field[f].unequal_countries<=gap->country_count);
    CHECK(gap->field[f].minimum_residual<=gap->field[f].maximum_residual);
    field_unequal_sum+=gap->field[f].unequal_countries;
   }
   CHECK(field_unequal_sum==gap->unequal_cells);
   unsigned row_unequal_sum=0u;
   for(unsigned i=0;i<gap->country_count;i++)row_unequal_sum+=gap->unequal_fields_per_country[i];
   CHECK(row_unequal_sum==gap->unequal_cells);
   /* Two legacy finance-projection fields must align with GK215's
    * independently gated original-Spring residual report. */
   Gk215Gap *legacy=malloc(sizeof *legacy);CHECK(legacy);
   CHECK(gk215_compare_original_spring(argv[1],pre,stock,legacy)==GK215_OK);
   CHECK(gap->field[GK278_GOLD].unequal_countries==legacy->gold_mismatches);
   CHECK(gap->field[GK278_FOOD].unequal_countries==legacy->food_mismatches);
   CHECK(gap->field[GK278_TROOPS].unequal_countries==legacy->troop_mismatches);
   free(legacy);
   CHECK(!memcmp(saved,stock,sizeof *stock));
   if(p==1u)*fresh=*gap;
   if(p==2u)*arch=*gap;
   *old=*gap;
   stock->stock_raw[0][0]^=1u;
   CHECK(gk278_compare_original_spring_coverage(argv[1],pre,stock,gap)==GK278_SOURCE);
   CHECK(!memcmp(old,gap,sizeof *gap));*stock=*saved;
   stock->world.countries[0].skill++;
   CHECK(gk278_compare_original_spring_coverage(argv[1],pre,stock,gap)==GK278_SOURCE);
   CHECK(!memcmp(old,gap,sizeof *gap));*stock=*saved;
   stock->orders.remaining=2u;
   CHECK(gk278_compare_original_spring_coverage(argv[1],pre,stock,gap)==GK278_SOURCE);
   CHECK(!memcmp(old,gap,sizeof *gap));*stock=*saved;
   printf("OBSERVED_RUNTIME_STOCK_%s: %u direct source-record cells, %u unequal / %u exact vs SOURCE-BOUNDED pre-setup finance (NOT generated Spring); Skill mismatches=%u, Arms=%u, Gold=%u, Food=%u\n",
    p==1u?"FRESH_RESET":p==2u?"ARCHIVED_MONGOL":"ARCHIVED_WORLD",
    gap->compared_cells,gap->unequal_cells,gap->exact_cells,
    gap->field[GK278_SKILL].unequal_countries,
    gap->field[GK278_ARMS].unequal_countries,
    gap->field[GK278_GOLD].unequal_countries,
    gap->field[GK278_FOOD].unequal_countries);
   free(saved);
  }
  /* Do not collapse two independently observed stock first-Spring
   * RNG histories into a fabricated universal stat initialization. */
  CHECK(fresh->profile==GK77_FRESH_MONGOL_WORKRAM);
  CHECK(arch->profile==GK77_ARCHIVED_MONGOL_SAVERAM);
  free(fresh);free(arch);
 }else puts("PUBLIC negative-only QA: no private records packaged or first Spring generated.");
 printf("GK278 %u assertions PASS; ORIGINAL SETUP/RNG STILL OPEN.\n",checks);
 free(all);free(pre);free(stock);free(gap);free(old);
 return 0;
}
