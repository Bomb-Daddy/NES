#include "gk325_opening_sequence.h"
#include <stdio.h>
#include <string.h>
static int checks=0;
#define CHECK(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static const Gk325Sample fresh[]={
 {491,1174,425,129,60,0,0,0,0},
 {1241,1174,425,129,60,0,0,0,0},
 {1366,1174,425,258,60,0,0,0,0},
 {1666,1175,425,258,60,121,0,0,0},
 {1966,1175,425,258,60,121,0,0,0},
 {2266,1175,430,208,61,121,0,1,0},
 {2391,1175,430,208,61,121,0,1,0},
 {2691,1175,430,208,61,121,3,0,1}
};
int main(void){
 Gk325Trace a={GK325_MONGOL,GK325_FRESH_RESET,GK325_WORK_RAM,14,fresh,sizeof fresh/sizeof fresh[0]};
 Gk325Audit out={0};
 CHECK(gk325_audit_trace(&a,&out)==GK325_OK);
 CHECK(out.setup_frame==1366 && out.calendar_frame==1666);
 CHECK(out.finance_frame==2266 && out.event_frame==2266 && out.command_frame==2691);
 CHECK(out.source_year==1175 && out.source_gold==430 && out.source_food==208);
 CHECK(out.source_troops==61 && out.source_rate==121 && out.source_orders==3);
 CHECK(out.observed_stock==1 && out.port_generated==0 && out.port_orders==0);
 Gk325Sample other[8];memcpy(other,fresh,sizeof other);
 other[7].gold=439;other[7].rate=101; a.samples=other;
 CHECK(gk325_audit_trace(&a,&out)==GK325_OK); /* not a universal constant! */
 a.samples=fresh;Gk325Audit poison;memset(&poison,0xa5,sizeof poison);
 Gk325Audit previous=poison;
 a.bank=GK325_SAVE_RAM;
 CHECK(gk325_audit_trace(&a,&poison)==GK325_WRONG_SOURCE);
 CHECK(memcmp(&poison,&previous,sizeof poison)==0);
 a.bank=GK325_WORK_RAM;a.country_count=27;
 CHECK(gk325_audit_trace(&a,&poison)==GK325_WRONG_SOURCE);
 CHECK(memcmp(&poison,&previous,sizeof poison)==0);
 a.country_count=14;a.world=GK325_WORLD;
 CHECK(gk325_audit_trace(&a,&poison)==GK325_WRONG_SOURCE);
 a.world=GK325_MONGOL;a.samples=other;
 memcpy(other,fresh,sizeof other);other[7].source_orders=2;
 CHECK(gk325_audit_trace(&a,&poison)==GK325_MISSING_PHASE);
 memcpy(other,fresh,sizeof other);other[6].source_orders=3;
 CHECK(gk325_audit_trace(&a,&poison)==GK325_INCONSISTENT);
 memcpy(other,fresh,sizeof other);other[7].observed_command_menu=0;
 CHECK(gk325_audit_trace(&a,&poison)==GK325_INCONSISTENT);
 memcpy(other,fresh,sizeof other);other[5].observed_event_dialog=0;other[6].observed_event_dialog=0;
 CHECK(gk325_audit_trace(&a,&poison)==GK325_MISSING_PHASE);
 memcpy(other,fresh,sizeof other);other[3].year=1173;
 CHECK(gk325_audit_trace(&a,&poison)==GK325_INCONSISTENT);
 memcpy(other,fresh,sizeof other);other[4].frame=1366;
 CHECK(gk325_audit_trace(&a,&poison)==GK325_INCONSISTENT);
 memcpy(other,fresh,sizeof other);other[2].food=129;
 CHECK(gk325_audit_trace(&a,&poison)==GK325_MISSING_PHASE);
 memcpy(other,fresh,sizeof other);other[3].rate=0;other[4].rate=0;
 CHECK(gk325_audit_trace(&a,&poison)==GK325_MISSING_PHASE);
 CHECK(memcmp(&poison,&previous,sizeof poison)==0);
 a.samples=fresh;
 CHECK(gk325_audit_trace(0,&poison)==GK325_BAD_INPUT);
 CHECK(gk325_audit_trace(&a,0)==GK325_BAD_INPUT);
 CHECK(memcmp(&poison,&previous,sizeof poison)==0);
 const Gk325Sample archiveM={500,1175,439,208,61,101,3,0,1};
 Gk325Trace m={GK325_MONGOL,GK325_ARCHIVED_RESTORED,GK325_SAVE_RAM,14,&archiveM,1};
 CHECK(gk325_audit_trace(&m,&out)==GK325_OK);
 CHECK(out.command_frame==500 && out.setup_frame==0 && out.port_orders==0);
 const Gk325Sample archiveW={700,1206,193,332,116,114,3,0,1};
 Gk325Trace w={GK325_WORLD,GK325_ARCHIVED_RESTORED,GK325_SAVE_RAM,27,&archiveW,1};
 CHECK(gk325_audit_trace(&w,&out)==GK325_OK);
 CHECK(out.source_gold==193 && out.source_food==332 && out.source_troops==116);
 w.bank=GK325_WORK_RAM;CHECK(gk325_audit_trace(&w,&poison)==GK325_WRONG_SOURCE);
 w.bank=GK325_SAVE_RAM;w.condition=GK325_FRESH_RESET;
 CHECK(gk325_audit_trace(&w,&poison)==GK325_WRONG_SOURCE);
 CHECK(memcmp(&poison,&previous,sizeof poison)==0);
 printf("GK325 C11 source-stage witness assertions PASS (%d); NOT native first Spring generation\n",checks);
 return 0;
}
