#include "gk134_stock_command_route.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <string.h>
#include <stdio.h>
#define EQ(i,e,r,a,p,want) assert(strcmp(gk134_menu_capability(i,e,r,a,p),want)==0)
int main(void){
 /* SOURCE-VALIDATED menu contracts: 3 available Classic orders. */
 EQ(0,0,3,0,0,"SET RATE"); EQ(1,0,3,0,0,"MANUAL");
 EQ(12,0,3,0,0,"PASS"); EQ(4,0,3,0,0,"PRICE VIEW");
 EQ(6,0,3,0,0,"READ ONLY"); EQ(13,0,3,0,0,"PORT SAVE");
 for(unsigned i=0;i<14;i++)
  if(i!=0&&i!=1&&i!=4&&i!=6&&i!=12&&i!=13)
   EQ(i,0,3,0,0,"LOCKED");
 /* Three actions consumed: don't mark TAX locked; leave Y UNDO accessible. */
 EQ(0,0,0,3,0,"UNDO ONLY");EQ(1,0,0,3,0,"UNDO ONLY");
 EQ(12,0,0,3,0,"ENDED");
 /* PASS ends the season irreversibly and is not a fourth action. */
 EQ(0,0,0,2,1,"ENDED");EQ(1,0,0,2,1,"ENDED");
 EQ(12,0,0,2,1,"ENDED");
 /* No source means no route authorization; presenter validates args. */
 EQ(0,0,4,0,0,"REFUSED"); EQ(0,2,3,0,0,"REFUSED");
 EQ(0,0,3,4,0,"REFUSED"); EQ(0,0,3,0,2,"REFUSED");
 EQ(14,0,3,0,0,"REFUSED");
 EQ(0,1,3,0,0,"CLASSIC ONLY");EQ(1,1,3,0,0,"ENHANCED");
 EQ(12,1,3,0,0,"PROFILE PASS");
 /* 14 commands x every remaining order value and the 2 profiles. */
 for(unsigned profile=0;profile<2;profile++)
  for(unsigned remaining=0;remaining<=3;remaining++)
   for(unsigned i=0;i<14;i++){
    const char *p=gk134_menu_capability(i,profile,remaining,3-remaining,0);
    assert(p && *p && strcmp(p,"REFUSED")!=0);
   }
 puts("EXEC0253 PASS: Classic Tax/Assign/Pass actionable labels and undo-only visibility; no new NES action");
 return 0;
}
