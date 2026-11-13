#include "gk245_owner_source.h"
#include "gk1_source_identity.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define CK(x) do{if(!(x)){fprintf(stderr,"GK245 test fail at %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char **argv){
 CK(argc==2||argc==3);
 Gk1TemplateDataset *ds=calloc(1,sizeof *ds);
 Gk1App *app=calloc(1,sizeof *app),*before=malloc(sizeof *before);
 CK(ds&&app&&before);
 CK(gk1_verified_original_csv(argv[1]));
 CK(gk1_template_load(argv[1],ds)==GK1_LOAD_OK);
 gk1_app_init(app);CK(gk1_app_install_templates(app,ds));
 memcpy(before,app,sizeof *app);
 CK(gk245_owner_source_startup(app,argv[1],1,0,NULL,NULL)==GK245_NO_REQUEST);
 CK(memcmp(app,before,sizeof *app)==0);
 CK(gk245_owner_source_startup(app,argv[1],1,1,"/never/fresh.json","/never/world.mss")==GK245_ISOLATED);
 CK(memcmp(app,before,sizeof *app)==0);
 CK(gk245_owner_source_startup(app,argv[1],1,0,"/never/fresh.json","/never/world.mss")==GK245_CONFLICT);
 CK(memcmp(app,before,sizeof *app)==0);
 CK(gk245_owner_source_startup(app,argv[1],1,0,"/never/fresh.json",NULL)==GK245_FRESH_REFUSED);
 CK(memcmp(app,before,sizeof *app)==0);
 CK(gk245_owner_source_startup(app,argv[1],1,0,NULL,"/never/world.mss")==GK245_ARCHIVED_REFUSED);
 CK(memcmp(app,before,sizeof *app)==0);
 CK(gk245_owner_source_startup(app,argv[1],0,0,NULL,"/never/world.mss")==GK245_ARCHIVED_REFUSED);
 CK(memcmp(app,before,sizeof *app)==0);
 /* Fresh and archive source never become interchangeable or pre-setup
  * bypassed by a failed owner request. */
 CK(app->worlds[0].phase==GK1_PRE_SETUP_TEMPLATE&&app->worlds[1].phase==GK1_PRE_SETUP_TEMPLATE);
 CK(!app->stock_checkpoint.valid);
 if(argc==3){
  /* Optional owner-only oracle. This licensed original Mesen state is NOT
   * bundled with source, the installer, public GitHub, or public CTest. */
  CK(gk245_owner_source_startup(app,argv[1],1,0,NULL,argv[2])==GK245_ARCHIVED_IMPORTED);
  CK(app->screen==GK1_SCREEN_STOCK_CONTINUE);
  CK(app->rules_profile==0u&&app->stock_checkpoint.valid);
  CK(app->stock_checkpoint.source_verified&&app->stock_checkpoint.year==1206u);
  CK(app->stock_checkpoint.world.country_count==27u);
  CK(app->stock_checkpoint.orders.remaining==3u);
  CK(gk85_raw_source_intact(&app->stock_checkpoint));
  CK(gk88_stock_projection_intact(&app->stock_checkpoint));
  CK(app->worlds[0].phase==GK1_PRE_SETUP_TEMPLATE);
  CK(app->worlds[1].phase==GK1_PRE_SETUP_TEMPLATE);
 }
 free(before);free(app);free(ds);
 puts("EXEC0245 PASS: common Win32/Linux owner source gate, conflict, isolation and failure atomicity; owner-only World SaveRAM positive oracle optional; original independent first Spring RNG STILL OPEN");
 return 0;
}
