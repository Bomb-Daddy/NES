/* Independent bounded Enhanced-only three-order replay/undo test.
 * Does not assert original-stock first Spring or original controller parity. */
#include "gk222_enhanced_spring_lab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(q) do { if(!(q)){fprintf(stderr,"EXEC0223 FAIL %s at %d\n",#q,__LINE__);return 1;} }while(0)
static int run(const Gk1LoadedScenario *source){
 Gk222Spring *s=calloc(1,sizeof *s),*saved=malloc(sizeof *saved);
 CHECK(s&&saved);
 CHECK(gk222_open(source,0,s)==GK222_OK);
 CHECK(gk222_verify_receipts(s)==GK222_OK);
 unsigned base_town=s->working.countries[0].town;
 unsigned base_food=s->working.countries[0].food_workers;
 CHECK(gk222_apply(s,0,GK222_PASS)==GK222_OK);
 CHECK(gk222_apply(s,0,GK222_ASSIGN_TOWN_TO_FOOD)==GK222_OK);
 CHECK(gk222_apply(s,0,GK222_PASS)==GK222_OK);
 CHECK(s->remaining==0 && s->ended);
 CHECK(gk222_verify_receipts(s)==GK222_OK);
 CHECK(gk222_undo_last(s)==GK222_OK);
 CHECK(gk222_undo_last(s)==GK222_OK);
 CHECK(s->remaining==2 && !s->ended);
 CHECK(s->working.countries[0].town==base_town);
 CHECK(s->working.countries[0].food_workers==base_food);
 CHECK(gk222_apply(s,0,GK222_ASSIGN_TOWN_TO_FOOD)==GK222_OK);
 CHECK(gk222_apply(s,0,GK222_PASS)==GK222_OK);
 CHECK(s->remaining==0 && s->ended && gk222_verify_receipts(s)==GK222_OK);
 *saved=*s;
 CHECK(gk222_apply(s,0,GK222_PASS)==GK222_NO_ORDERS);
 CHECK(memcmp(s,saved,sizeof *s)==0);
 CHECK(gk222_apply(s,1,GK222_PASS)==GK222_WRONG_COUNTRY);
 CHECK(memcmp(s,saved,sizeof *s)==0);
 s->order_receipts[1]=99;
 Gk222Spring before=*s;
 CHECK(gk222_undo_last(s)==GK222_CORRUPT_HISTORY);
 CHECK(memcmp(s,&before,sizeof before)==0);
 CHECK(gk222_apply(s,0,GK222_PASS)==GK222_CORRUPT_HISTORY);
 CHECK(memcmp(s,&before,sizeof before)==0);
 *s=*saved;
 s->working.countries[0].town++;
 before=*s;
 CHECK(gk222_undo_last(s)==GK222_CORRUPT_HISTORY);
 CHECK(memcmp(s,&before,sizeof before)==0);
 *s=*saved;
 while(s->actions_committed)CHECK(gk222_undo_last(s)==GK222_OK);
 CHECK(s->remaining==3 && !s->ended && gk222_verify_receipts(s)==GK222_OK);
 CHECK(gk222_undo_last(s)==GK222_NO_HISTORY);
 CHECK(s->working.countries[0].town==base_town);
 CHECK(s->working.countries[0].food_workers==base_food);
 free(saved);free(s);return 0;
}
int main(int argc,char **argv){
 CHECK(argc==2);
 Gk1TemplateDataset *dataset=calloc(1,sizeof *dataset);
 Gk1LoadedScenario *m=calloc(1,sizeof *m),*w=calloc(1,sizeof *w);
 CHECK(dataset&&m&&w);
 CHECK(gk1_template_load(argv[1],dataset)==GK1_LOAD_OK);
 CHECK(gk1_open_loaded_scenario(dataset,GK1_MONGOL_CONQUEST,m)==GK1_LOAD_OK);
 CHECK(gk1_open_loaded_scenario(dataset,GK1_WORLD_CONQUEST,w)==GK1_LOAD_OK);
 CHECK(run(m)==0);CHECK(run(w)==0);
 free(dataset);free(m);free(w);
 puts("EXEC0223 14/27-country bounded replay, LB undo, tamper atomicity PASS; not original NES");
 return 0;
}
