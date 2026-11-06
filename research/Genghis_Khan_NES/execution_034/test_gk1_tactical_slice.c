#include "gk1_tactical_slice.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

static int tests=0;
#define CHECK(c) do { ++tests; if(!(c)) { fprintf(stderr,"FAIL %s:%d: %s\n",__FILE__,__LINE__,#c); exit(1);} } while(0)

static void test_source_tables_and_rng(void) {
  const int costs[]={8,6,3,4,5,4,5}, pcts[]={7,5,255,2,5,255,255};
  for(unsigned i=0;i<7;i++){CHECK(gk1_mobility_cost(i)==costs[i]);CHECK(gk1_hazard_percent(i)==pcts[i]);}
  CHECK(gk1_mobility_cost(7)==-1); CHECK(gk1_hazard_percent(7)==-1);
  const unsigned terrains[]={3,0,1,4,2};
  const unsigned stage[]={39,37,38,38,40}, final[]={34,32,33,33,40};
  for(unsigned i=0;i<5;i++) {
    Gk1CasualtyResult r;
    CHECK(gk1_casualties(40,terrains[i],GK1_SEED_INPUT,&r)==0);
    CHECK(r.after_percent==stage[i] && r.after_random==final[i]);
    CHECK(r.next_seed==(terrains[i]==2 ? GK1_SEED_INPUT:GK1_SEED_AFTER_HAZARD));
    CHECK(r.has_random==(terrains[i]!=2));
    if(terrains[i]!=2) CHECK(r.rng_remainder==2);
  }
  Gk1CasualtyResult r;
  CHECK(gk1_casualties(20,3,GK1_SEED_INPUT,&r)==0 && r.after_percent==19 && r.after_random==14);
  CHECK(gk1_casualties(0,3,GK1_SEED_INPUT,&r)==0 && r.after_random==0);
  CHECK(gk1_casualties(65535,2,GK1_SEED_INPUT,&r)==0 && r.after_random==65535);
  CHECK(gk1_casualties(40,7,GK1_SEED_INPUT,&r)==-1);
  CHECK(gk1_casualties(40,3,UINT64_C(0x1000000000000),&r)==-1);
  CHECK(gk1_casualties(40,3,GK1_SEED_INPUT,NULL)==-1);
}

static void unchanged_on_unknown(Gk1BattleState s,uint8_t x,uint8_t y,Gk1Occupancy occ) {
  Gk1BattleState sentinel=gk1_checkpoint_fixture(10,2,GK1_OCC_EMPTY);
  Gk1BattleState out=sentinel;
  CHECK(gk1_apply_observed_step(&s,x,y,occ,&out)==GK1_UNVERIFIED);
  CHECK(memcmp(&out,&sentinel,sizeof(out))==0);
}
static void test_atomic_observed_cases(void) {
  Gk1BattleState s=gk1_checkpoint_fixture(2,3,GK1_OCC_EMPTY),out={0};
  CHECK(gk1_apply_observed_step(&s,8,7,GK1_OCC_EMPTY,&out)==GK1_REJECTED);
  CHECK(memcmp(&out,&s,sizeof(out))==0);
  s=gk1_checkpoint_fixture(4,3,GK1_OCC_EMPTY);
  CHECK(gk1_apply_observed_step(&s,8,7,GK1_OCC_EMPTY,&out)==GK1_FINISHED);
  CHECK(out.mover.soldiers==34 && out.mover.x==8 && out.mover.y==7 && out.mover.mobility==10);
  CHECK(out.completion_word==1 && out.rng48==GK1_SEED_AFTER_HAZARD && out.phase==GK1_PHASE_ORDERS);
  CHECK(out.gold==s.gold && out.food==s.food && out.other_units_guard==s.other_units_guard);
  s=gk1_checkpoint_fixture(10,3,GK1_OCC_EMPTY);
  CHECK(gk1_apply_observed_step(&s,8,7,GK1_OCC_EMPTY,&out)==GK1_STEP_PENDING);
  CHECK(out.mover.soldiers==34 && out.mover.mobility==6 && out.completion_word==0);
  CHECK(out.phase==GK1_PHASE_MOVE_WHERE && out.has_pending_step==1);
  Gk1BattleState finished={0};
  CHECK(gk1_finish_observed_path(&out,&finished)==GK1_FINISHED);
  CHECK(finished.mover.soldiers==34 && finished.mover.mobility==10);
  CHECK(finished.phase==GK1_PHASE_ORDERS && finished.completion_word==1 && finished.has_pending_step==0);
  CHECK(finished.rng48==GK1_SEED_AFTER_HAZARD);
  s=gk1_checkpoint_fixture(10,0,GK1_OCC_EMPTY);
  CHECK(gk1_apply_observed_step(&s,8,7,GK1_OCC_EMPTY,&out)==GK1_FINISHED);
  CHECK(out.mover.soldiers==32 && out.mover.mobility==10 && out.completion_word==1);
  s=gk1_checkpoint_fixture(10,3,GK1_OCC_FRIEND_SLOT1);
  CHECK(gk1_apply_observed_step(&s,8,7,GK1_OCC_FRIEND_SLOT1,&out)==GK1_FINISHED);
  CHECK(out.mover.soldiers==74 && out.friendly_slot1.soldiers==0);
  CHECK(out.friendly_slot1.x==8 && out.friendly_slot1.y==7);
  CHECK(out.mover.mobility==10 && out.completion_word==2);
  CHECK(out.rng48==GK1_SEED_AFTER_HAZARD);
  s=gk1_checkpoint_fixture(10,3,GK1_OCC_ENEMY_SLOT10);
  CHECK(gk1_apply_observed_step(&s,8,7,GK1_OCC_ENEMY_SLOT10,&out)==GK1_REJECTED);
  CHECK(memcmp(&out,&s,sizeof(out))==0);
}
static void test_hostile_rejections(void) {
  Gk1BattleState s=gk1_checkpoint_fixture(10,3,GK1_OCC_EMPTY);
  unchanged_on_unknown(s,7,6,GK1_OCC_EMPTY);
  unchanged_on_unknown(s,8,7,GK1_OCC_FRIEND_SLOT1);
  unchanged_on_unknown(s,8,7,(Gk1Occupancy)77);
  s.mover.soldiers=39;unchanged_on_unknown(s,8,7,GK1_OCC_EMPTY);
  s=gk1_checkpoint_fixture(10,3,GK1_OCC_EMPTY);s.rng48^=1;unchanged_on_unknown(s,8,7,GK1_OCC_EMPTY);
  s=gk1_checkpoint_fixture(10,3,GK1_OCC_EMPTY);s.combined_mod_profile=0;unchanged_on_unknown(s,8,7,GK1_OCC_EMPTY);
  s=gk1_checkpoint_fixture(10,3,GK1_OCC_EMPTY);s.exact_checkpoint_unit_profile=0;unchanged_on_unknown(s,8,7,GK1_OCC_EMPTY);
  s=gk1_checkpoint_fixture(10,3,GK1_OCC_EMPTY);s.completion_word=1;unchanged_on_unknown(s,8,7,GK1_OCC_EMPTY);
  s=gk1_checkpoint_fixture(10,1,GK1_OCC_EMPTY);unchanged_on_unknown(s,8,7,GK1_OCC_EMPTY);
  s=gk1_checkpoint_fixture(3,3,GK1_OCC_EMPTY);unchanged_on_unknown(s,8,7,GK1_OCC_EMPTY);
  s=gk1_checkpoint_fixture(10,3,GK1_OCC_EMPTY);s.enemy_slot10.soldiers=69;unchanged_on_unknown(s,8,7,GK1_OCC_EMPTY);
  s=gk1_checkpoint_fixture(10,3,GK1_OCC_EMPTY);s.friendly_slot1.x=8;unchanged_on_unknown(s,8,7,GK1_OCC_EMPTY);
  s=gk1_checkpoint_fixture(10,3,GK1_OCC_FRIEND_SLOT1);s.friendly_slot1.soldiers=39;unchanged_on_unknown(s,8,7,GK1_OCC_FRIEND_SLOT1);
  s=gk1_checkpoint_fixture(10,3,GK1_OCC_EMPTY);
  Gk1BattleState out=gk1_checkpoint_fixture(2,3,GK1_OCC_EMPTY), sentinel=out;
  CHECK(gk1_finish_observed_path(&s,&out)==GK1_UNVERIFIED);
  CHECK(memcmp(&out,&sentinel,sizeof(out))==0);
  CHECK(gk1_apply_observed_step(NULL,8,7,GK1_OCC_EMPTY,&out)==GK1_UNVERIFIED);
  CHECK(gk1_apply_observed_step(&s,8,7,GK1_OCC_EMPTY,NULL)==GK1_UNVERIFIED);
  CHECK(gk1_finish_observed_path(NULL,&out)==GK1_UNVERIFIED);
  CHECK(gk1_finish_observed_path(&s,NULL)==GK1_UNVERIFIED);
}
int main(void){ test_source_tables_and_rng(); test_atomic_observed_cases();test_hostile_rejections();printf("EXEC0034 C integration slice PASS: %d individual assertions\n",tests);return 0;}
