#include "gk1_app_state.h"
#include "gk316_cpu_claims_ingest.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define H "ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3"
#define CK(x) do{total++;if(!(x)){fprintf(stderr,"GK316 FAIL %d: %s\n",__LINE__,#x);return 4;}}while(0)
static unsigned total;
static int write_private(const char *path,const char *s,size_t n){
 FILE *f=fopen(path,"wb");if(!f)return 0;
 int good=fwrite(s,1,n,f)==n && !ferror(f);
 if(fclose(f)!=0)good=0;
 return good;
}
static int run(const char *path,const char *input,Gk316Status expected,Gk316Trace *out){
 CK(write_private(path,input,strlen(input)));
 Gk316Trace original=*out;
 CK(gk316_load_cpu_claims(path,out)==expected);
 if(expected!=GK316_OK)CK(memcmp(out,&original,sizeof original)==0);
 return 0;
}
int main(int argc,char **argv){
 if(argc!=2)return 2;
 const char *name="gk316_cpu_claims_private_test.tmp";
 Gk316Trace *out=malloc(sizeof *out);
 Gk1TemplateDataset *dataset=malloc(sizeof *dataset);
 Gk1App *app=malloc(sizeof *app);
 CK(out&&dataset&&app);
 memset(out,0xa7,sizeof *out);
 const char valid[]="GK1_STOCK_CPU_CLAIMS_V1," H ",M,000102030405\n"
  "N,12,00\nS,14,c0ba,15,84,00,01,01,02,03\n"
  "C,33,c289,15,010102030405,111213141516,1245\n";
 CK(run(name,valid,GK316_OK,out)==0);
 CK(out->world==0&&out->event_count==3&&out->nmi_count==1&&
    out->store_count==1&&out->c289_count==1&&out->source_claim_only==1);
 CK(!out->observed_stock_emulator&&!out->first_spring_initialized&&!out->player_orders_granted);
 CK(out->events[1].pc==0xc0bau&&out->events[1].bank==15u&&out->events[1].a==1u);
 const char valid_world[]="GK1_STOCK_CPU_CLAIMS_V1," H ",W,ffffffffffff\n"
  "N,7,01\nC,8,c289,15,000000000001,ffffffffffff,32767\n";
 CK(run(name,valid_world,GK316_OK,out)==0);
 CK(out->world==1&&out->nmi_count==1&&out->store_count==0&&out->c289_count==1);
 const char shortline[]="GK1_STOCK_CPU_CLAIMS_V1," H ",M,000102030405\nC,1,c289,15,000000000001,ffffffffffff,0";
 CK(run(name,shortline,GK316_SYNTAX,out)==0);
 const char crlf[]="GK1_STOCK_CPU_CLAIMS_V1," H ",M,000102030405\r\nC,1,c289,15,000000000001,ffffffffffff,0\n";
 CK(run(name,crlf,GK316_SYNTAX,out)==0);
 const char unknown[]="GK1_STOCK_CPU_CLAIMS_V1," H ",M,000102030405\nX,1,c289,15,000000000001,ffffffffffff,0\n";
 CK(run(name,unknown,GK316_SYNTAX,out)==0);
 const char order[]="GK1_STOCK_CPU_CLAIMS_V1," H ",M,000102030405\nN,9,00\nC,8,c289,15,000000000001,ffffffffffff,0\n";
 CK(run(name,order,GK316_ORDER,out)==0);
 const char extra[]="GK1_STOCK_CPU_CLAIMS_V1," H ",M,000102030405\nC,1,c289,15,000000000001,ffffffffffff,0,EXTRA\n";
 CK(run(name,extra,GK316_SYNTAX,out)==0);
 const char badword[]="GK1_STOCK_CPU_CLAIMS_V1," H ",M,000102030405\nS,1,c0ba,15,93,00,01,01,02,03\nC,2,c289,15,000000000001,ffffffffffff,0\n";
 CK(run(name,badword,GK316_SYNTAX,out)==0);
 const char badcycle[]="GK1_STOCK_CPU_CLAIMS_V1," H ",M,000102030405\nC,18446744073709551616,c289,15,000000000001,ffffffffffff,0\n";
 CK(run(name,badcycle,GK316_SYNTAX,out)==0);
 /* Genuine source-world app gate, not merely a new detached parser. */
 CK(gk1_template_load(argv[1],dataset)==GK1_LOAD_OK);
 gk1_app_init(app);CK(gk1_app_install_templates(app,dataset));
 CK(write_private(name,valid,sizeof valid-1));
 Gk316Trace before=*out;
 CK(gk1_app_original_cpu_claims_parse(app,name,out)==GK316_STAGE);
 CK(memcmp(out,&before,sizeof before)==0);
 app->rules_profile=0;app->active_world=0;app->setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
 CK(gk1_app_original_cpu_claims_parse(app,name,out)==GK316_OK);
 CK(out->event_count==3 && !out->player_orders_granted);
 app->rules_profile=1;before=*out;
 CK(gk1_app_original_cpu_claims_parse(app,name,out)==GK316_STAGE);
 CK(memcmp(out,&before,sizeof before)==0);
 CK(remove(name)==0);
 CK(gk316_load_cpu_claims(name,out)==GK316_IO);
 CK(memcmp(out,&before,sizeof before)==0);
 printf("GK316 CPU claim ingestion and Gk1App Classic pre-setup stage gate PASS (%u checks; neither emulator proof nor first Spring)\n",total);
 free(out);free(dataset);free(app);return 0;
}