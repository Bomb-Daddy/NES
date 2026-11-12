#include "gk213_stock_spring_boundary.h"
#include "gk1_app_state.h"
#include "gk1_source_identity.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define CHECK(cond) do{if(!(cond)){fprintf(stderr,"GK213 FAIL line %d: %s\n",__LINE__,#cond);return 1;} n++;}while(0)
int main(int argc,char **argv){
 unsigned n=0;Gk213Boundary out,old;
 memset(&out,0xA5,sizeof out);old=out;
 CHECK(gk213_check_observed_mongol_boundary(NULL,NULL,NULL)==GK213_ARGUMENT);
 Gk211Report tr={0};Gk213Inputs in={0};
 CHECK(gk213_check_observed_mongol_boundary(&in,&tr,&out)==GK213_UNGROUNDED);
 CHECK(!memcmp(&out,&old,sizeof out));
 if(argc>=2){
  CHECK(gk211_verify_fresh_reset_sequence(argv[1],&tr)==GK211_OK);
  /* Only these inputs are from the one recorded, authenticated original
   * history; the post-setup stats are observations at the final snapshot,
   * not independently time-stamped intermediate source events. */
  in=(Gk213Inputs){129,425,60,4,3,6,27,186,118,35,61};
  CHECK(gk213_check_observed_mongol_boundary(&in,&tr,&out)==GK213_OK);
  CHECK(out.food_after_start_bonus==258u);
  CHECK(out.food_after_feeding==208u);
  CHECK(out.gold_after_taxes_and_wages==430u);
  CHECK(out.troops_before_growth==60u&&out.troops_after_observed_growth==61u);
  CHECK(out.tax_gross==65u&&out.wages==60u);
  CHECK(out.source_observation_only&&!out.generated_rng&&
        !out.independent_first_spring&&!out.orders_granted);
  old=out;
  Gk213Inputs mutated=in;
  mutated.stock_pre_food++;
  CHECK(gk213_check_observed_mongol_boundary(&mutated,&tr,&out)==GK213_UNGROUNDED);
  CHECK(!memcmp(&out,&old,sizeof out));
  mutated=in;mutated.observed_post_setup_economy++;
  /* Integer truncation leaves economy 186 and 187 observationally
   * indistinguishable: this calculation is NOT a uniqueness proof. */
  CHECK(gk213_check_observed_mongol_boundary(&mutated,&tr,&out)==GK213_OK);
  CHECK(out.gold_after_taxes_and_wages==old.gold_after_taxes_and_wages);
  mutated=in;mutated.observed_post_setup_economy+=4u;
  CHECK(gk213_check_observed_mongol_boundary(&mutated,&tr,&out)==GK213_INCONSISTENT);
  CHECK(!memcmp(&out,&old,sizeof out));
  mutated=in;mutated.observed_post_season_troops--;
  CHECK(gk213_check_observed_mongol_boundary(&mutated,&tr,&out)==GK213_UNGROUNDED);
  CHECK(!memcmp(&out,&old,sizeof out));
  /* Real existing game integration: all inputs come from verified ORIGINAL
   * owner-private WorkRAM evidence, not hard-coded into the game executable. */
  if(argc>=3){
   Gk1TemplateDataset all={0};
   Gk1App *app=calloc(1,sizeof *app);
   Gk1App *preserved=malloc(sizeof *preserved);
   CHECK(app&&preserved);
   CHECK(gk1_verified_original_csv(argv[2]));
   CHECK(gk1_template_load(argv[2],&all)==GK1_LOAD_OK);
   gk1_app_init(app);
   CHECK(gk1_app_install_templates(app,&all));
   *preserved=*app;
   CHECK(gk1_app_observed_first_spring_boundary(app,argv[2],argv[1],&out)==GK213_OK);
   CHECK(!memcmp(app,preserved,sizeof *app));
   CHECK(!out.independent_first_spring&&!out.orders_granted);
   old=out;
   app->worlds[0].countries[13].gold++;
   CHECK(gk1_app_observed_first_spring_boundary(app,argv[2],argv[1],&out)==GK213_UNGROUNDED);
   CHECK(!memcmp(&out,&old,sizeof out));
   CHECK(gk1_app_observed_first_spring_boundary(preserved,argv[2],"/invalid/fresh/source",&out)==GK213_UNGROUNDED);
   CHECK(!memcmp(&out,&old,sizeof out));
   free(preserved);free(app);
  }
  Gk211Report forged=tr;forged.stage[13].orders=2;
  CHECK(gk213_check_observed_mongol_boundary(&in,&forged,&out)==GK213_UNGROUNDED);
  CHECK(!memcmp(&out,&old,sizeof out));
 }
 printf("GK213 first-Spring source-bounded food/tax/pay arithmetic checks: %u PASS; generalized startup NOT implemented\n",n);
 return 0;
}
