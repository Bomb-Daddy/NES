#include "gk239_owner_world_mss.h"
#include "gk1_source_identity.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define CHECK(expr) do {if(!(expr)){fprintf(stderr,"EXEC0239 line %d: %s\n",__LINE__,#expr);return 1;}checks++;}while(0)
int main(int argc,char **argv){
 if(argc!=2&&argc!=4)return 2;
 unsigned checks=0u;
 Gk1App *a=(Gk1App*)malloc(sizeof *a);
 Gk1App *before=(Gk1App*)malloc(sizeof *before);
 Gk1TemplateDataset *ds=(Gk1TemplateDataset*)malloc(sizeof *ds);
 if(!a||!before||!ds)return 3;
 gk1_app_init(a);
 CHECK(gk1_template_load(argv[1],ds)==GK1_LOAD_OK);
 CHECK(gk1_app_install_templates(a,ds));
 *before=*a;
 CHECK(gk239_owner_world_startup(a,argv[1],NULL,NULL,1,0)==GK239_UNREQUESTED);
 CHECK(gk239_owner_world_startup(a,argv[1],"",NULL,1,0)==GK239_UNREQUESTED);
 CHECK(gk239_owner_world_startup(a,argv[1],"/not/found.mss",NULL,1,1)==GK239_SMOKE_ISOLATED);
 CHECK(gk239_owner_world_startup(a,argv[1],"/not/found.mss","/not/found.json",1,0)==GK239_CONFLICTING_SOURCES);
 CHECK(gk239_owner_world_startup(a,argv[1],"/not/found.mss",NULL,0,0)==GK239_REFUSED);
 CHECK(gk239_owner_world_startup(a,argv[1],"/not/found.mss",NULL,1,0)==GK239_REFUSED);
 CHECK(gk239_owner_world_startup(a,argv[1],argv[1],NULL,1,0)==GK239_REFUSED);
 CHECK(gk239_owner_world_startup(a,"/not/found.csv","/not/found.mss",NULL,1,0)==GK239_REFUSED);
 CHECK(gk239_owner_world_startup(NULL,argv[1],"/not/found.mss",NULL,1,0)==GK239_REFUSED);
 CHECK(memcmp(a,before,sizeof *a)==0);
 CHECK(a->worlds[0].country_count==14u&&a->worlds[1].country_count==27u);
 CHECK(a->screen==GK1_SCREEN_TITLE);
 if(argc==4){
  /* Optional owner-local positive original World Mesen state and distinct
   * original Mongol state. Neither may ship with a public ZIP/installer. */
  CHECK(gk239_owner_world_startup(a,argv[1],argv[2],NULL,1,1)==GK239_SMOKE_ISOLATED);
  CHECK(memcmp(a,before,sizeof *a)==0);
  CHECK(gk239_owner_world_startup(a,argv[1],argv[3],NULL,1,0)==GK239_REFUSED);
  CHECK(memcmp(a,before,sizeof *a)==0);
  CHECK(gk239_owner_world_startup(a,argv[1],argv[2],NULL,1,0)==GK239_IMPORTED);
  CHECK(a->screen==GK1_SCREEN_STOCK_CONTINUE);
  CHECK(a->stock_checkpoint.valid&&a->stock_checkpoint.source_verified);
  CHECK(a->stock_checkpoint.profile==GK77_ARCHIVED_WORLD_SAVERAM);
  CHECK(a->stock_checkpoint.year==1206u&&a->stock_checkpoint.rate==114u);
  CHECK(a->stock_checkpoint.world.country_count==27u);
  CHECK(a->stock_checkpoint.orders.remaining==3u);
  CHECK(a->active_world==1u&&a->rules_profile==0u);
  CHECK(memcmp(a->worlds,before->worlds,sizeof a->worlds)==0);
  CHECK(gk85_raw_source_intact(&a->stock_checkpoint));
  *before=*a;
  CHECK(gk239_owner_world_startup(a,argv[1],argv[2],NULL,1,0)==GK239_REFUSED);
  CHECK(memcmp(a,before,sizeof *a)==0);
 }
 printf("EXEC0239 %u assertions PASS; archived original World owner state import; no NES first Spring RNG or Xbox\n",checks);
 free(a);free(before);free(ds);return 0;
}
