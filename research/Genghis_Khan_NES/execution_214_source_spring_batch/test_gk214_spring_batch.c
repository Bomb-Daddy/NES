#include "gk214_spring_batch.h"
#include "gk1_app_state.h"
#include "../execution_049/gk1_scenario_factory.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"GK214 FAIL at %d: %s\n",__LINE__,#x);return 1;} checks++;}while(0)
int main(int argc,char **argv){
 unsigned checks=0;
 Gk214Batch *out=malloc(sizeof *out),*backup=malloc(sizeof *backup);
 Gk1TemplateDataset *source=malloc(sizeof *source);
 Gk1LoadedScenario *a=malloc(sizeof *a),*unaltered=malloc(sizeof *unaltered);
 CHECK(out&&backup&&source&&a&&unaltered);
 memset(out,0xAB,sizeof *out);memcpy(backup,out,sizeof *out);
 CHECK(gk214_project_spring_batch(NULL,out)==GK214_BAD_INPUT);
 CHECK(!memcmp(out,backup,sizeof *out));
 CHECK(gk214_project_spring_batch(a,NULL)==GK214_BAD_INPUT);
 if(argc<2){fprintf(stderr,"Usage: %s CANONICAL_41_COUNTRY_CSV\n",argv[0]);return 2;}
 CHECK(gk1_template_load(argv[1],source)==GK1_LOAD_OK);
 for(int branch=0;branch<2;branch++){
  const unsigned total=branch?27u:14u;
  CHECK(gk1_open_loaded_scenario(source,(Gk1ScenarioId)(branch+1),a)==GK1_LOAD_OK);
  memcpy(unaltered,a,sizeof *a);
  CHECK(gk214_project_spring_batch(a,out)==GK214_OK);
  CHECK(!memcmp(a,unaltered,sizeof *a));
  CHECK(out->count==total&&out->source_bounded_only&&
    !out->generated_setup&&!out->generated_rng&&!out->orders_granted);
  CHECK(out->after.phase==GK1_PRE_SETUP_TEMPLATE);
  CHECK(out->after.country_count==total&&out->after.scenario_id==a->scenario_id);
  unsigned taxes=0,payroll=0,food=0,loss=0,starved=0;
  for(unsigned c=0;c<total;c++){
   Gk75SpringResult single={0};
   CHECK(gk75_template_projection(&a->countries[c],&single)==GK75_OK);
   CHECK(!memcmp(&single,&out->country[c],sizeof single));
   CHECK(out->after.countries[c].gold==single.after.gold &&
     out->after.countries[c].troops==single.after.troops &&
     out->after.countries[c].goods[10]==single.after.food);
   CHECK(out->after.countries[c].country_id==a->countries[c].country_id &&
     out->after.countries[c].goods[0]==a->countries[c].goods[0] &&
     out->after.countries[c].skill==a->countries[c].skill &&
     out->after.countries[c].morale==a->countries[c].morale);
   taxes+=single.gross_tax;payroll+=single.payroll;
   food+=single.food_consumed;loss+=single.troops_lost;
   starved+=!!single.starved;
  }
  CHECK(out->gross_taxes==taxes&&out->payroll==payroll&&
    out->food_consumed==food&&out->troops_lost==loss&&
    out->starved_country_count==starved);
  memcpy(backup,out,sizeof *out);
  a->countries[0].country_id=99;
  CHECK(gk214_project_spring_batch(a,out)==GK214_BAD_COUNTRY);
  CHECK(!memcmp(out,backup,sizeof *out));
  *a=*unaltered;
  a->countries[total-1u].goods[10]=1000;
  CHECK(gk214_project_spring_batch(a,out)==GK214_BAD_FINANCE);
  CHECK(!memcmp(out,backup,sizeof *out));
  *a=*unaltered;
  a->countries[total-1u].index=99;
  CHECK(gk214_project_spring_batch(a,out)==GK214_BAD_COUNTRY);
  CHECK(!memcmp(out,backup,sizeof *out));
  *a=*unaltered;
  a->country_count--;
  CHECK(gk214_project_spring_batch(a,out)==GK214_BAD_INPUT);
  CHECK(!memcmp(out,backup,sizeof *out));
 }
 /* Exact SAME native app state, not a detached market/season prototype. */
 Gk1App *game=calloc(1,sizeof *game),*saved=malloc(sizeof *saved);
 CHECK(game&&saved);
 gk1_app_init(game);
 CHECK(gk1_app_spring_batch_projection(game,0,out)==GK214_BAD_INPUT);
 CHECK(gk1_app_install_templates(game,source));
 memcpy(saved,game,sizeof *game);
 for(uint8_t branch=0;branch<2u;branch++){
  CHECK(gk1_app_spring_batch_projection(game,branch,out)==GK214_OK);
  CHECK(out->count==(branch?27u:14u));
  CHECK(!out->orders_granted&&!out->generated_setup&&!out->generated_rng);
  CHECK(!memcmp(game,saved,sizeof *game));
 }
 game->worlds[0].phase=(Gk1ScenarioPhase)2;
 memcpy(backup,out,sizeof *out);
 CHECK(gk1_app_spring_batch_projection(game,0,out)==GK214_BAD_INPUT);
 CHECK(!memcmp(out,backup,sizeof *out));
 free(saved);free(game);
 printf("GK214 14/27-country atomic Spring finance engine %u assertions PASS; "
        "setup/RNG and playable original Spring NOT generated\n",checks);
 free(out);free(backup);free(source);free(a);free(unaltered);
 return 0;
}
