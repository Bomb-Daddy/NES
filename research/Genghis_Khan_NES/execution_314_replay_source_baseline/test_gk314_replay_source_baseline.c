/* EXEC0314: user-visible source baseline survives separate tactical research
 * replay imports. These are PORT research receipts, not NES gameplay.
 * No ROM bytes or licensed captures are embedded in this test. */
#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks;
#define CHECK(c) do { ++checks; if (!(c)) { fprintf(stderr,"FAIL %s:%d: %s\n", __FILE__,__LINE__,#c); return 1; } } while (0)
static int intact(const Gk1App *a,const Gk1LoadedScenario baseline[2]) {
  for(unsigned w=0; w<2; w++) {
    CHECK(a->world_ready[w] == 1u);
    CHECK(a->worlds[w].phase == GK1_PRE_SETUP_TEMPLATE);
    CHECK(a->finance_source_baseline[w].phase == GK1_PRE_SETUP_TEMPLATE);
    CHECK(a->finance_source_baseline[w].country_count == (w==0 ? 14u : 27u));
    CHECK(memcmp(&a->finance_source_baseline[w],&baseline[w],sizeof baseline[w])==0);
  }
  return 0;
}
int main(int argc,char **argv) {
  if(argc != 2) return 2;
  Gk1TemplateDataset *dataset=malloc(sizeof *dataset);
  Gk1App *app=malloc(sizeof *app);
  Gk1App *before=malloc(sizeof *before);
  Gk1LoadedScenario *baseline=malloc(2u*sizeof *baseline);
  if(!dataset || !app || !before || !baseline) return 3;
  CHECK(gk1_template_load(argv[1],dataset)==GK1_LOAD_OK);
  gk1_app_init(app);
  CHECK(gk1_app_install_templates(app,dataset)==1);
  memcpy(baseline,app->finance_source_baseline,2u*sizeof *baseline);
  CHECK(intact(app,baseline)==0);
  /* Simulated controller draft changes the mutable world only. */
  app->worlds[0].countries[0].gold++;
  app->worlds[1].countries[5].gold++;
  Gk1LoadedScenario working[2];
  memcpy(working,app->worlds,sizeof working);
  CHECK(gk1_app_restore_path(app,1,NULL,0)==1);
  CHECK(intact(app,baseline)==0);
  CHECK(memcmp(working,app->worlds,sizeof working)==0);
  CHECK(gk1_app_restore_divide(app,NULL,0,0,7,7)==1);
  CHECK(intact(app,baseline)==0);
  CHECK(memcmp(working,app->worlds,sizeof working)==0);
  Gk1BattleSession session;
  CHECK(gk1_session_open_fixture(10,3,GK1_OCC_EMPTY,&session)==GK1_SESSION_OK);
  CHECK(gk1_app_restore_session(app,&session)==1);
  CHECK(intact(app,baseline)==0);
  CHECK(memcmp(working,app->worlds,sizeof working)==0);
  /* Hostile/tampered replay cannot alter either baseline or mutable countries. */
  memcpy(before,app,sizeof *before);
  CHECK(gk1_app_restore_path(app,1,(uint8_t[]){2},1)==0);
  CHECK(memcmp(before,app,sizeof *before)==0);
  CHECK(gk1_app_restore_divide(app,(uint8_t[]){20},1,0,7,7)==0);
  CHECK(memcmp(before,app,sizeof *before)==0);
  session.battle.mover.soldiers++;
  CHECK(gk1_app_restore_session(app,&session)==0);
  CHECK(memcmp(before,app,sizeof *before)==0);
  printf("EXEC0314 baseline replay preservation and hostile rollback: %u assertions PASS\n", checks);
  free(dataset);free(app);free(before);free(baseline);
  return 0;
}
