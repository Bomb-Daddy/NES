#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "gk170_slot_preview.h"
#include "gk1_source_identity.h"
#include "gk166_fresh_entry.h"
#include "gk117_stock_pristine_gate.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <sys/stat.h>
#include <unistd.h>
#endif
static unsigned checks=0;
#define CHECK(x) do{++checks;if(!(x)){fprintf(stderr,"GK170 FAIL line %d: %s\n",__LINE__,#x);return 2;}}while(0)
int main(int argc,char **argv){
 CHECK(argc==2||argc==5);
 Gk1App *live=calloc(1,sizeof *live),*unchanged=malloc(sizeof *unchanged);
 Gk1TemplateDataset *ds=malloc(sizeof *ds);
 CHECK(live&&unchanged&&ds);
 gk1_app_init(live);
 CHECK(gk1_template_load(argv[1],ds)==GK1_LOAD_OK);
 CHECK(gk1_app_install_templates(live,ds));
 CHECK(gk1_verified_original_csv(argv[1]));
 *unchanged=*live;
 Gk170SlotPreview p={.status=GK170_SLOT_VERIFIED,.year=9999u};
 CHECK(gk170_preview_private_slot(NULL,argv[1],live,&p)==GK170_SLOT_INVALID);
 CHECK(p.status==GK170_SLOT_VERIFIED&&p.year==9999u);
 CHECK(gk170_preview_private_slot("/never/existed/GK170.gk17",argv[1],live,&p)==GK170_SLOT_UNAVAILABLE);
 CHECK(p.status==GK170_SLOT_UNAVAILABLE&&p.year==0u);
 CHECK(memcmp(live,unchanged,sizeof *live)==0);
#ifdef _WIN32
 /* Native Windows execution not certified in this Linux research run. */
#else
 char badpath[180];snprintf(badpath,sizeof badpath,"/tmp/gk170_negative_%ld.gk17",(long)getpid());
 (void)unlink(badpath);
 FILE *bad=fopen(badpath,"wb");CHECK(bad!=NULL);
 unsigned char bogus[100]={0};memcpy(bogus,"BAD SAVE",8);
 CHECK(fwrite(bogus,1,sizeof bogus,bad)==sizeof bogus);CHECK(fclose(bad)==0);
 CHECK(chmod(badpath,0600)==0);
 CHECK(gk170_preview_private_slot(badpath,argv[1],live,&p)==GK170_SLOT_REFUSED);
 CHECK(p.status==GK170_SLOT_REFUSED&&p.year==0u&&p.rate==0u);
 CHECK(memcmp(live,unchanged,sizeof *live)==0);
 CHECK(chmod(badpath,0644)==0);
 CHECK(gk170_preview_private_slot(badpath,argv[1],live,&p)==GK170_SLOT_UNAVAILABLE);
 CHECK(memcmp(live,unchanged,sizeof *live)==0);
 CHECK(unlink(badpath)==0);
#endif
 if(argc==5){
  Gk77Profile profiles[3]={GK77_FRESH_MONGOL_WORKRAM,
   GK77_ARCHIVED_MONGOL_SAVERAM,GK77_ARCHIVED_WORLD_SAVERAM};
  for(unsigned i=0;i<3u;i++){
   const unsigned w=i==2u?1u:0u;
   CHECK(gk117_open_attested_original(argv[1],&live->worlds[w],argv[i+2],
        profiles[i],0u,&live->stock_checkpoint)==GK117_OK);
   live->rules_profile=0u;
   char path[180];snprintf(path,sizeof path,"/tmp/gk170_private_%ld_%u.gk17",
#ifdef _WIN32
     1000L,
#else
     (long)getpid(),
#endif
     i);
   (void)remove(path);
   CHECK(gk167_save_private_new(path,live)==GK167_OK);
   *unchanged=*live;
   CHECK(gk170_preview_private_slot(path,argv[1],live,&p)==GK170_SLOT_VERIFIED);
   CHECK(p.year==(i==2u?1206u:1175u));
   CHECK(p.profile==profiles[i]&&p.country_count==(i==2u?27u:14u));
   CHECK(p.player_index==0u&&p.orders_remaining==3u&&!p.orders_ended);
   CHECK(memcmp(live,unchanged,sizeof *live)==0);
   CHECK(remove(path)==0);
  }
 }
 printf("GK170 %u PASS; %s; title-only read, no game/saved state mutation, no NES new spring\n",
  checks,argc==5?"three private original-source profiles":"public negative fixtures");
 free(ds);free(live);free(unchanged);return 0;
}
