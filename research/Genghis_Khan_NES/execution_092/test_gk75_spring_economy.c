#include "gk75_spring_economy.h"
#include "gk1_app_state.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
static void known_source_case(void){
 /* v1.07 ORIGINAL-ROM-source annual audit's 1175 winter->spring controlled
  * pre-food state AFTER 4% common economy work. No original ROM run here. */
 Gk75SpringFields in={61,5,4,6,27,289,449,177,118,35};
 Gk75SpringResult out;
 assert(gk75_spring_food_tax_pay(&in,&out)==GK75_OK);
 assert(out.after.food==238 && out.after.gold==449);
 assert(out.after.troops==61 && out.payroll==61);
 assert(out.gross_tax==61 && out.food_consumed==51 && !out.starved);
 assert(!memcmp(&in,&(Gk75SpringFields){61,5,4,6,27,289,449,177,118,35},sizeof in));
 /* Town+10, Food workers-10, source-covered counterfactual: economy already
  * updated to187, producing gold453, all other payroll/food preserved. */
 in.town=15;in.food_workers=17;in.economy=187;
 assert(gk75_spring_food_tax_pay(&in,&out)==GK75_OK);
 assert(out.gross_tax==65&&out.after.gold==453&&out.after.food==238);
}
static void famine_and_payroll(void){
 Gk75SpringFields in={10,8,3,5,4,0,0,0,0,0};
 Gk75SpringResult out;
 assert(gk75_spring_food_tax_pay(&in,&out)==GK75_OK);
 assert(out.starved && out.after.food==0 && out.after.troops==0);
 assert(out.after.town==0&&out.after.masons==0&&out.after.artisans==0&&out.after.food_workers==0);
 in=(Gk75SpringFields){10,0,0,0,0,6,3,0,0,0};
 assert(gk75_spring_food_tax_pay(&in,&out)==GK75_OK);
 assert(out.after.food==1 && out.after.troops==3&&out.payroll==3 &&out.after.gold==0);
 assert(out.troops_lost==7 &&out.starved==0);
 /* Nonuniform starvation rounding: each group floored separately. */
 in=(Gk75SpringFields){9,1,1,1,1,3,50,0,0,0};
 assert(gk75_spring_food_tax_pay(&in,&out)==GK75_OK);
 assert(out.starved && out.after.food==0 && out.after.troops<=9);
 assert(out.after.town==0 && out.after.artisans==0);
}
static void caps_atomicity(void){
 Gk75SpringFields in={0,0,0,0,0,0,999,999,999,100};
 Gk75SpringResult out;
 assert(gk75_spring_food_tax_pay(&in,&out)==GK75_OK);
 assert(out.after.gold==999 && out.gross_tax==999 && out.payroll==0);
 out.after.gold=42;Gk75SpringResult before=out;
 in.troops=1000;
 assert(gk75_spring_food_tax_pay(&in,&out)==GK75_BAD_INPUT);
 assert(!memcmp(&out,&before,sizeof out));
 in.troops=999;in.town=1;
 assert(gk75_spring_food_tax_pay(&in,&out)==GK75_BAD_INPUT);
 assert(!memcmp(&out,&before,sizeof out));
 assert(gk75_spring_food_tax_pay(NULL,&out)==GK75_BAD_INPUT);
 assert(gk75_spring_food_tax_pay(&in,NULL)==GK75_BAD_INPUT);
 assert(!memcmp(&out,&before,sizeof out));
}
static void scenario_is_read_only(const char *path){
 Gk1TemplateDataset source;assert(gk1_template_load(path,&source)==GK1_LOAD_OK);
 Gk1App app;gk1_app_init(&app);assert(gk1_app_install_templates(&app,&source));
 Gk1LoadedScenario before[2];memcpy(before,app.worlds,sizeof before);
 for(unsigned w=0;w<2;w++)for(unsigned i=0;i<app.worlds[w].country_count;i++){
  Gk75SpringResult first,again;
  const Gk1CountryTemplate *c=&app.worlds[w].countries[i];
  assert(gk75_template_projection(c,&first)==GK75_OK);
  assert(gk75_template_projection(c,&again)==GK75_OK);
  assert(!memcmp(&first,&again,sizeof first));
  assert(app.worlds[w].phase==GK1_PRE_SETUP_TEMPLATE);
 }
 assert(!memcmp(app.worlds,before,sizeof before));
}
int main(int argc,char **argv){
 assert(argc==2);
 known_source_case();famine_and_payroll();caps_atomicity();scenario_is_read_only(argv[1]);
 puts("EXEC0075 ORIGINAL_6502_SOURCE_DERIVED Spring food/tax/payroll component PASS; pre-setup source-only projection, no true first-Spring claim");
 return 0;
}
