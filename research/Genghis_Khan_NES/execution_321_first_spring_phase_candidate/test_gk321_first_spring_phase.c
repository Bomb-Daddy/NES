#include "gk321_first_spring_phase.h"
#include <stdio.h>
#include <string.h>
static unsigned n=0;
#define CHECK(expr) do{n++;if(!(expr)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#expr);return 1;}}while(0)
int main(void){
 /* A bounded original-stock fresh Spring1175 arithmetic compatibility
  * witness, NOT a universal first Spring or proof of event order. Before
  * personnel: original 100 assigned people, setup food 129->258,
  * Gold425, 60 troops, proposed economy186/morale118/tax35. Actual separate
  * fresh captured end: Food208 Gold430 Troops61 Town5 Masons4. */
 const Gk321BeforeFinance fresh={60,4,3,6,27,258,425,186,118,35};
 const Gk321AfterPersonnel growth={61,5,4,6,27};
 Gk321PhaseResult out={0};
 CHECK(gk321_reconcile_phases(&fresh,&growth,&out)==GK321_OK);
 CHECK(out.food==208 && out.gold==430 && out.food_consumed==50);
 CHECK(out.gross_tax==65 && out.payroll==60 && out.paid_troops==60);
 CHECK(out.troops_after_personnel==61 && out.town_after_personnel==5);
 CHECK(!out.observed_stock_emulator&&!out.first_spring_generated&&!out.player_orders_granted);
 CHECK(gk321_compare_observed(&out,208,430)==GK321_OK);
 /* The independently archived stock Spring1175 is a different RNG/choices
  * history: Gold439, Rate101. Reject claiming one computed case covers it. */
 CHECK(gk321_compare_observed(&out,208,439)==GK321_INCONSISTENT);
 CHECK(gk321_compare_observed(&out,207,430)==GK321_INCONSISTENT);
 Gk321PhaseResult old=out;
 /* Post-personnel figures mistakenly fed to the earlier finance stage
  * change the economic result. Not evidence of an extra universal rule. */
 Gk321BeforeFinance wrong=fresh;
 wrong.troops=61;wrong.town=5;wrong.masons=4;
 CHECK(gk321_reconcile_phases(&wrong,&growth,&out)==GK321_OK);
 CHECK(gk321_compare_observed(&out,208,430)==GK321_INCONSISTENT);
 out=old;
 wrong=fresh;wrong.food=129;
 CHECK(gk321_reconcile_phases(&wrong,&growth,&out)==GK321_OK);
 CHECK(gk321_compare_observed(&out,208,430)==GK321_INCONSISTENT);
 out=old;
 /* Contract: all failure paths leave an existing good receipt byte-identical. */
 #define REJECT(b,l,e) do{CHECK(gk321_reconcile_phases((b),(l),&out)==(e));CHECK(memcmp(&out,&old,sizeof old)==0);}while(0)
 REJECT(NULL,&growth,GK321_ARGUMENT);
 REJECT(&fresh,NULL,GK321_ARGUMENT);
 Gk321BeforeFinance bad=fresh;bad.tax=1000;
 REJECT(&bad,&growth,GK321_INCONSISTENT);
 bad=fresh;bad.troops=999;
 REJECT(&bad,&growth,GK321_INCONSISTENT);
 Gk321AfterPersonnel badlate=growth;badlate.troops=59;
 REJECT(&fresh,&badlate,GK321_NEEDS_SOURCE);
 badlate=growth;badlate.food_workers=998;
 REJECT(&fresh,&badlate,GK321_INCONSISTENT);
 badlate=growth;badlate.town=1000;
 REJECT(&fresh,&badlate,GK321_INCONSISTENT);
 CHECK(gk321_reconcile_phases(NULL,NULL,NULL)==GK321_ARGUMENT);
 CHECK(gk321_compare_observed(NULL,208,430)==GK321_ARGUMENT);
 out.observed_stock_emulator=1;
 CHECK(gk321_compare_observed(&out,208,430)==GK321_NEEDS_SOURCE);
 out=old;out.first_spring_generated=1;
 CHECK(gk321_compare_observed(&out,208,430)==GK321_NEEDS_SOURCE);
 out=old;out.player_orders_granted=1;
 CHECK(gk321_compare_observed(&out,208,430)==GK321_NEEDS_SOURCE);
 printf("GK321 PASS %u assertions: bounded Spring phase arithmetic, growth separation, adversarial snapshots, atomic refusals; no game unlock\n",n);
 return 0;
}
