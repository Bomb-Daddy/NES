#include "gk198_tax_forecast.h"
#include "gk1_app_state.h"
#include "gk193_auto_source_entry.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define CK(c) do{if(!(c)){fprintf(stderr,"GK198 FAIL %u: %s\n",(unsigned)__LINE__,#c);return 1;}checks++;}while(0)
static int private_cases(const char *csv,const char *source, Gk77Profile expected, unsigned *checkcount){
 Gk1TemplateDataset *ds=calloc(1,sizeof *ds);
 Gk1App *a=calloc(1,sizeof *a);
 if(!a||!ds)return 0;
 if(gk1_template_load(csv,ds)!=GK1_LOAD_OK)return 0;
 gk1_app_init(a);
 if(!gk1_app_install_templates(a,ds))return 0;
 a->rules_profile=0;
 Gk77Profile detected=0;
 if(gk193_enter_original_auto(a,csv,source,0,&detected)!=GK193_OK||detected!=expected)return 0;
 if(gk187_open(&a->stock_checkpoint,&a->classic_assign)!=GK187_OK)return 0;
 Gk79Checkpoint pristine=a->stock_checkpoint;
 Gk198Forecast f;
 uint8_t current=(uint8_t)a->classic_assign.mutable_world.countries[0].tax;
 uint8_t candidate=current==100?99u:100u;
 if(gk198_classic_tax_forecast(&a->classic_assign,&a->stock_checkpoint,candidate,&f)!=GK198_OK)return 0;
 (*checkcount)++;
 if(f.committed_rate!=current||f.proposed_rate!=candidate||f.remaining!=3u||f.action_count!=0u)return 0;
 (*checkcount)++;
 if(gk187_set_tax(&a->classic_assign,&a->stock_checkpoint,candidate)!=GK187_OK)return 0;
 if(gk187_transfer(&a->classic_assign,&a->stock_checkpoint,0,4,1)!=GK187_OK)return 0;
 if(gk198_classic_tax_forecast(&a->classic_assign,&a->stock_checkpoint,current,&f)!=GK198_OK)return 0;
 (*checkcount)++;
 if(f.committed_rate!=candidate||f.proposed_rate!=current||f.remaining!=1u||f.action_count!=2u)return 0;
 (*checkcount)++;
 if(memcmp(&pristine,&a->stock_checkpoint,sizeof pristine)!=0)return 0;
 (*checkcount)++;
 Gk198Forecast sentinel;memset(&sentinel,0xa5,sizeof sentinel);f=sentinel;
 a->classic_assign.mutable_world.countries[0].tax=(uint16_t)(candidate==99?98:99);
 if(gk198_classic_tax_forecast(&a->classic_assign,&a->stock_checkpoint,current,&f)!=GK198_SOURCE||
    memcmp(&f,&sentinel,sizeof f)!=0)return 0;
 (*checkcount)++;
 free(a);free(ds);return 1;
}
int main(int argc,char **argv){
 unsigned checks=0;
 Gk75SpringFields f={61,40,30,20,100,208,430,186,118,20};
 Gk198Forecast a={0},b={0},copy={0};
 CK(gk198_compare_fields(&f,20,&a)==GK198_OK);
 CK(a.proposed_rate==20&&a.committed_rate==20);
 CK(a.gold_delta==0);
 CK(a.committed.gross_tax==a.proposed.gross_tax);
 CK(a.proposed.after.food==a.committed.after.food);
 CK(a.proposed.after.troops==a.committed.after.troops);
 CK(gk198_compare_fields(&f,100,&b)==GK198_OK);
 CK(b.proposed.gross_tax>a.committed.gross_tax);
 CK(b.proposed.after.gold>=a.committed.after.gold);
 CK(b.gold_delta==(int32_t)b.proposed.after.gold-(int32_t)a.committed.after.gold);
 CK(b.proposed.after.food==a.committed.after.food);
 CK(b.proposed.payroll==a.committed.payroll);
 CK(memcmp(&f,&(Gk75SpringFields){61,40,30,20,100,208,430,186,118,20},sizeof f)==0);
 memset(&copy,0xAA,sizeof copy);
 Gk198Forecast untouched=copy;
 CK(gk198_compare_fields(&f,0,&copy)==GK198_INVALID);
 CK(memcmp(&copy,&untouched,sizeof copy)==0);
 CK(gk198_compare_fields(&f,101,&copy)==GK198_INVALID);
 CK(memcmp(&copy,&untouched,sizeof copy)==0);
 Gk75SpringFields invalid=f;invalid.food=1000;
 CK(gk198_compare_fields(&invalid,60,&copy)==GK198_ARITHMETIC);
 CK(memcmp(&copy,&untouched,sizeof copy)==0);
 invalid=f;invalid.tax=0;
 CK(gk198_compare_fields(&invalid,60,&copy)==GK198_INVALID);
 CK(memcmp(&copy,&untouched,sizeof copy)==0);
 CK(gk198_compare_fields(NULL,50,&copy)==GK198_INVALID);
 CK(gk198_compare_fields(&f,50,NULL)==GK198_INVALID);
 CK(gk198_classic_tax_forecast(NULL,NULL,50,&copy)==GK198_INVALID);
 Gk187ClassicAssign source={0};Gk79Checkpoint checkpoint={0};
 CK(gk198_classic_tax_forecast(&source,&checkpoint,50,&copy)==GK198_SOURCE);
 CK(memcmp(&copy,&untouched,sizeof copy)==0);
 CK(gk198_classic_tax_forecast(&source,&checkpoint,0,&copy)==GK198_INVALID);
 CK(memcmp(&copy,&untouched,sizeof copy)==0);
 /* Source-invariant reference: rate changes do not alter current population,
  * food, treasury or original immutable country records. */
 CK(f.gold==430&&f.food==208&&f.tax==20);
 CK(a.before_gold==430&&a.before_food==208);
 CK(argc==1||argc==5);
 if(argc==5){
  CK(private_cases(argv[1],argv[2],GK77_FRESH_MONGOL_WORKRAM,&checks));
  CK(private_cases(argv[1],argv[3],GK77_ARCHIVED_MONGOL_SAVERAM,&checks));
  CK(private_cases(argv[1],argv[4],GK77_ARCHIVED_WORLD_SAVERAM,&checks));
 }
 puts("GK198 source-bounded Spring tax what-if checks PASS");
 printf("checks=%u; no original first-Spring or next-season parity claimed\n",checks);
 return 0;
}
