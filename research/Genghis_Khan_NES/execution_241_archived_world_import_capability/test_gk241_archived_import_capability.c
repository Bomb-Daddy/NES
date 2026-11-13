#include "gk241_archived_import_capability.h"
#include "gk239_owner_world_mss.h"
#include "gk1_source_identity.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define CHECK(test) do {if(!(test)){fprintf(stderr,"GK241 assertion %d: %s\n",__LINE__,#test);return 1;} checks++;}while(0)
int main(int argc,char **argv){
 if(argc!=2)return 2;
 unsigned checks=0;
 Gk1TemplateDataset *ds=malloc(sizeof *ds);
 Gk1App *app=malloc(sizeof *app);
 Gk1App *before=malloc(sizeof *before);
 if(!ds||!app||!before)return 3;
 gk1_app_init(app);
 CHECK(gk1_template_load(argv[1],ds)==GK1_LOAD_OK);
 CHECK(gk1_app_install_templates(app,ds));
 *before=*app;
#ifdef GK163_HAS_ZLIB
 CHECK(gk241_archived_import_backend()==GK241_ARCHIVE_BACKEND_ZLIB);
 CHECK(strstr(gk241_archived_refusal_message(),"EXACT ORIGINAL SAVERAM")!=NULL);
#else
 CHECK(gk241_archived_import_backend()==GK241_ARCHIVE_BACKEND_UNAVAILABLE);
 CHECK(strstr(gk241_archived_refusal_message(),"LACKS ZLIB")!=NULL);
#endif
 CHECK(gk239_owner_world_startup(app,argv[1],"/missing/stock.mss",NULL,1,1)==GK239_SMOKE_ISOLATED);
 CHECK(memcmp(app,before,sizeof *app)==0);
 CHECK(gk239_owner_world_startup(app,argv[1],"/missing/stock.mss","/missing/fresh.json",1,0)==GK239_CONFLICTING_SOURCES);
 CHECK(memcmp(app,before,sizeof *app)==0);
 CHECK(gk239_owner_world_startup(app,argv[1],"/missing/stock.mss",NULL,1,0)==GK239_REFUSED);
 CHECK(memcmp(app,before,sizeof *app)==0);
 CHECK(app->worlds[0].country_count==14u && app->worlds[1].country_count==27u);
 CHECK(app->screen==GK1_SCREEN_TITLE);
 CHECK(app->stock_checkpoint.valid==0u);
 printf("EXEC0241 %u assertion PASS; backend=%d (NO private original loaded, Windows NOT RUN)\n",checks,(int)gk241_archived_import_backend());
 free(before);free(app);free(ds);return 0;
}
