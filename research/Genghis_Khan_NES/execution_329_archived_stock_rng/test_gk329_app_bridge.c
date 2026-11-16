#include "gk329_app_bridge.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"GK329 real app guard line %d\n",__LINE__);exit(1);}}while(0)
int main(void){
 Gk1App app={0}; uint8_t dummy[32]={0};Gk329Receipt r,prior;
 memset(&r,0x53,sizeof r);prior=r;
 CHECK(gk329_app_archived_rng_next(&app,dummy,sizeof dummy,dummy,sizeof dummy,&r)==GK329_NOT_ARCHIVED_SOURCE);
 CHECK(memcmp(&r,&prior,sizeof r)==0);
 app.rules_profile=1;
 CHECK(gk329_app_archived_rng_next(&app,dummy,sizeof dummy,dummy,sizeof dummy,&r)==GK329_NOT_ARCHIVED_SOURCE);
 CHECK(memcmp(&r,&prior,sizeof r)==0);
 app.rules_profile=0;app.active_world=2;
 CHECK(gk329_app_archived_rng_next(&app,dummy,sizeof dummy,dummy,sizeof dummy,&r)==GK329_NOT_ARCHIVED_SOURCE);
 CHECK(memcmp(&r,&prior,sizeof r)==0);
 CHECK(gk329_app_archived_rng_next(NULL,dummy,sizeof dummy,dummy,sizeof dummy,&r)==GK329_ARGUMENT);
 CHECK(memcmp(&r,&prior,sizeof r)==0);
 printf("GK329 real native Gk1App refusal/rollback PASS\n");return 0;
}
