#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "gk171_enhanced_title_resume.h"
#include "gk167_private_cold_save.h"
#include "gk117_stock_pristine_gate.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#endif
static unsigned checks;
#define C(x) do{++checks;if(!(x)){fprintf(stderr,"GK171 FAIL %d: %s\n",__LINE__,#x);return 2;}}while(0)
static int run_case(const char *csv,const char *raw,Gk77Profile profile,unsigned case_no){
 Gk1App *app=calloc(1,sizeof *app),*live=calloc(1,sizeof *live),*before=malloc(sizeof *before);
 Gk1TemplateDataset *ds=malloc(sizeof *ds);
 C(app&&live&&before&&ds);
 gk1_app_init(app);C(gk1_template_load(csv,ds)==GK1_LOAD_OK);
 C(gk1_app_install_templates(app,ds));
 unsigned w=profile==GK77_ARCHIVED_WORLD_SAVERAM?1u:0u;
 C(gk117_open_attested_original(csv,&app->worlds[w],raw,profile,0u,
     &app->stock_checkpoint)==GK117_OK);
 app->rules_profile=0u;
 char path[6][220];
 for(unsigned j=0;j<6;j++){
#ifndef _WIN32
  snprintf(path[j],sizeof path[j],"/tmp/gk171_%ld_%u_%u.private",(long)getpid(),case_no,j);
#else
  snprintf(path[j],sizeof path[j],"gk171_%u_%u.private",case_no,j);
#endif
  (void)remove(path[j]);
 }
 C(gk167_save_private_new(path[0],app)==GK167_OK);
 Gk116EnhancedAssign sp={0};Gk158EnhancedSummer su={0};
 Gk159EnhancedFall fa={0};Gk160EnhancedWinter wi={0};
 Gk161EnhancedYear2 y={0};
 C(gk116_open(&app->stock_checkpoint,1u,&sp)==GK116_OK);
 C(gk116_pass_remaining(&sp,&app->stock_checkpoint)==GK116_OK);
 C(gk116_save_new(path[1],&app->stock_checkpoint,&sp)==GK116_OK);
 C(gk158_open(&sp,&app->stock_checkpoint,&su)==GK158_OK);
 C(gk158_pass(&su,&sp,&app->stock_checkpoint)==GK158_OK);
 C(gk158_save_new(path[2],&app->stock_checkpoint,&sp,&su)==GK158_OK);
 C(gk159_open(&sp,&su,&app->stock_checkpoint,&fa)==GK159_OK);
 C(gk159_pass(&fa,&sp,&su,&app->stock_checkpoint)==GK159_OK);
 C(gk159_save_new(path[3],&app->stock_checkpoint,&sp,&su,&fa)==GK159_OK);
 C(gk160_open(&sp,&su,&fa,&app->stock_checkpoint,&wi)==GK160_OK);
 C(gk160_pass(&wi,&sp,&su,&fa,&app->stock_checkpoint)==GK160_OK);
 C(gk160_save_new(path[4],&app->stock_checkpoint,&sp,&su,&fa,&wi)==GK160_OK);
 C(gk161_open(&sp,&su,&fa,&wi,&app->stock_checkpoint,&y)==GK161_OK);
 C(gk161_save_new(path[5],&app->stock_checkpoint,&sp,&su,&fa,&wi,&y)==GK161_OK);
 Gk171Paths paths={.source_gk17=path[0],.canonical_csv=csv};
 for(unsigned stage=1;stage<=5u;stage++)paths.receipt[stage-1u]=path[stage];
 for(unsigned stage=1;stage<=5u;stage++){
  gk1_app_init(live);C(gk1_app_install_templates(live,ds));
  C(gk171_resume_stage(&paths,stage,live)==GK171_OK);
  C(live->rules_profile==1u&&live->stock_checkpoint.source_verified);
  C(live->screen==(Gk1Screen)(GK1_SCREEN_ENHANCED_ASSIGN+stage-1u));
  C(live->stock_checkpoint.orders.remaining==3u);
  C(gk116_attestation_intact(&live->enhanced_assign,&live->stock_checkpoint));
  if(stage>=2u)C(gk158_attestation_intact(&live->enhanced_summer,
    &live->enhanced_assign,&live->stock_checkpoint));
  if(stage>=3u)C(gk159_attestation_intact(&live->enhanced_fall,
    &live->enhanced_assign,&live->enhanced_summer,&live->stock_checkpoint));
  if(stage>=4u)C(gk160_attestation_intact(&live->enhanced_winter,
    &live->enhanced_assign,&live->enhanced_summer,&live->enhanced_fall,&live->stock_checkpoint));
  if(stage==5u)C(gk161_attestation_intact(&live->enhanced_year2,
    &live->enhanced_assign,&live->enhanced_summer,&live->enhanced_fall,
    &live->enhanced_winter,&live->stock_checkpoint));
 }
 gk1_app_init(live);C(gk1_app_install_templates(live,ds));
 unsigned selected=99u;
 C(gk171_resume_latest(&paths,live,&selected)==GK171_OK);
 C(selected==5u&&live->screen==GK1_SCREEN_ENHANCED_YEAR2);
 *before=*live;
 /* Tampered latest receipt must NOT cause silent fallback to Year1 Winter. */
 C(remove(path[5])==0);FILE *bad=fopen(path[5],"wb");C(bad!=NULL);
 C(fwrite("corrupt",1,7,bad)==7u&&fclose(bad)==0);
 C(gk171_resume_latest(&paths,live,&selected)==GK171_RECEIPT_REFUSED);
 C(memcmp(before,live,sizeof *live)==0&&selected==5u);
 C(remove(path[5])==0);
 C(gk171_resume_latest(&paths,live,&selected)==GK171_OK);
 C(selected==4u&&live->screen==GK1_SCREEN_ENHANCED_WINTER);
 /* Source authorization expires / missing: even an intact receipt is useless. */
 *before=*live;
 paths.source_gk17="/not/a/GK171/private/source.gk17";
 C(gk171_resume_stage(&paths,4u,live)==GK171_SOURCE_REFUSED);
 C(memcmp(before,live,sizeof *live)==0);
 for(unsigned j=0;j<6;j++)C(remove(path[j])==0||j==5u);
 free(app);free(live);free(before);free(ds);
 return 0;
}
int main(int argc,char **argv){
 C(argc==2||argc==5);
 Gk1App *s=calloc(1,sizeof *s),*copy=malloc(sizeof *copy);
 Gk1TemplateDataset *ds=malloc(sizeof *ds);C(s&&copy&&ds);
 gk1_app_init(s);C(gk1_template_load(argv[1],ds)==GK1_LOAD_OK);
 C(gk1_app_install_templates(s,ds)); *copy=*s;
 Gk171Paths p={.source_gk17="/not/a/private.gk17",.canonical_csv=argv[1],
  .receipt={"/none/a","/none/b","/none/c","/none/d","/none/e"}};
 unsigned stage=17u;
 C(gk171_resume_latest(&p,s,&stage)==GK171_NO_RECEIPT);
 C(stage==17u&&memcmp(s,copy,sizeof *s)==0);
 C(gk171_resume_stage(&p,1u,s)==GK171_SOURCE_REFUSED);
 C(memcmp(s,copy,sizeof *s)==0);
 C(gk171_resume_stage(&p,0u,s)==GK171_BAD_INPUT);
 C(gk171_resume_stage(&p,6u,s)==GK171_BAD_INPUT);
 C(gk171_resume_latest(NULL,s,&stage)==GK171_BAD_INPUT);
 C(memcmp(s,copy,sizeof *s)==0);
 free(s);free(copy);free(ds);
 if(argc==5){
  C(run_case(argv[1],argv[2],GK77_FRESH_MONGOL_WORKRAM,0)==0);
  C(run_case(argv[1],argv[3],GK77_ARCHIVED_MONGOL_SAVERAM,1)==0);
  C(run_case(argv[1],argv[4],GK77_ARCHIVED_WORLD_SAVERAM,2)==0);
 }
 printf("GK171 %u checks PASS; %s; original Spring and Classic commands still open\n",
   checks,argc==5?"owner-private three source-profile cold-resume":"public hostile negative");
 return 0;
}
