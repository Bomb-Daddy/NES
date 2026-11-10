#include "gk1_app_state.h"
#include "gk134_stock_command_route.h"
#include "gk117_stock_pristine_gate.h"
#include "gk167_private_cold_save.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#define CHECK(x) do{checks++;if(!(x)){fprintf(stderr,"GK169 failure line %d: %s\n",__LINE__,#x);return 2;}}while(0)
static unsigned checks;
int main(int argc,char **argv){
 CHECK(argc==1||argc==5);
 CHECK(strcmp(gk134_command_name(13u),"OTHER")==0);
 CHECK(gk134_command_route(13u)==GK134_ROUTE_PORT_STORAGE);
 CHECK(gk134_command_route(14u)==GK134_ROUTE_INVALID);
 Gk1App *a=calloc(1,sizeof *a);CHECK(a!=NULL);
 gk1_app_init(a);
 a->screen=GK1_SCREEN_STOCK_COMMANDS;a->rules_profile=0u;
 a->stock_command_cursor=13u;
 Gk79Checkpoint *snap=malloc(sizeof *snap);CHECK(snap!=NULL);
 *snap=a->stock_checkpoint;
 CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)==0); /* no fake source */
 CHECK(a->screen==GK1_SCREEN_STOCK_COMMANDS && !a->stock_storage_request);
 CHECK(memcmp(&a->stock_checkpoint,snap,sizeof *snap)==0);
 if(argc==5){
  Gk1TemplateDataset *data=malloc(sizeof *data);CHECK(data!=NULL);
  CHECK(gk1_template_load(argv[1],data)==GK1_LOAD_OK);
  CHECK(gk1_verified_original_csv(argv[1]));
  CHECK(gk1_app_install_templates(a,data));
  const Gk77Profile ps[3]={GK77_FRESH_MONGOL_WORKRAM,
   GK77_ARCHIVED_MONGOL_SAVERAM,GK77_ARCHIVED_WORLD_SAVERAM};
  for(unsigned i=0;i<3u;i++){
   const unsigned w=i==2u?1u:0u;
   CHECK(gk117_open_attested_original(argv[1],&a->worlds[w],argv[i+2],
      ps[i],0u,&a->stock_checkpoint)==GK117_OK);
   a->rules_profile=0u;a->screen=GK1_SCREEN_STOCK_COMMANDS;
   a->stock_command_cursor=13u;
   *snap=a->stock_checkpoint;
   CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
   CHECK(a->screen==GK1_SCREEN_STOCK_STORAGE);
   CHECK(!a->stock_storage_request && !a->stock_storage_confirm);
   CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1); /* arm save */
   CHECK(a->stock_storage_confirm && !a->stock_storage_request);
   CHECK(gk1_app_input(a,GK1_INPUT_CANCEL)==1);
   CHECK(a->screen==GK1_SCREEN_STOCK_COMMANDS);
   CHECK(!a->stock_storage_request&&!a->stock_storage_confirm);
   CHECK(memcmp(&a->stock_checkpoint,snap,sizeof *snap)==0);
   CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
   CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1); /* first A */
   CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1); /* second A */
   CHECK(a->stock_storage_request==1u); /* shell must consume once */
   CHECK(memcmp(&a->stock_checkpoint,snap,sizeof *snap)==0);
#ifndef _WIN32
   /* Dispatch the exact same hardened I/O invoked by the native pad shell,
    * then verify the controller-origin request round-trips the authenticated
    * source on disk, rejects overwrites and leaves no transient state. */
   char path[160];snprintf(path,sizeof path,
     "/tmp/gk169_private_pad_%ld_%u.gk17",(long)getpid(),i);
   (void)unlink(path);
   CHECK(gk167_save_private_new(path,a)==GK167_OK);
   CHECK(gk167_save_private_new(path,a)==GK167_EXISTS);
   Gk1App *cold=calloc(1,sizeof *cold);CHECK(cold!=NULL);
   gk1_app_init(cold);
   CHECK(gk1_app_install_templates(cold,data));
   CHECK(gk167_cold_load_private(path,argv[1],cold)==GK167_OK);
   CHECK(cold->stock_checkpoint.valid&&cold->stock_checkpoint.source_verified);
   CHECK(cold->stock_checkpoint.profile==ps[i]);
   CHECK(cold->stock_checkpoint.orders.remaining==3u);
   CHECK(cold->stock_checkpoint.world.country_count==(i==2u?27u:14u));
   CHECK(memcmp(cold->stock_checkpoint.stock_raw,
      a->stock_checkpoint.stock_raw,sizeof cold->stock_checkpoint.stock_raw)==0);
   CHECK(unlink(path)==0);
   CHECK(gk167_cold_load_private(path,argv[1],cold)==GK167_IO);
   free(cold);
#endif
   CHECK(gk1_app_input(a,GK1_INPUT_CANCEL)==1);
   CHECK(!a->stock_storage_request);
   CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
   CHECK(gk1_app_input(a,GK1_INPUT_DOWN)==1);
   CHECK(a->stock_storage_cursor==1u);
   CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
   CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
   CHECK(a->stock_storage_request==2u); /* cold-load request */
   CHECK(gk1_app_input(a,GK1_INPUT_CANCEL)==1);
   CHECK(memcmp(&a->stock_checkpoint,snap,sizeof *snap)==0);
   CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
   a->rules_profile=1u;
   CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
   CHECK(a->screen==GK1_SCREEN_STOCK_COMMANDS && !a->stock_storage_request);
   CHECK(memcmp(&a->stock_checkpoint,snap,sizeof *snap)==0);
  }
  free(data);
 }
 printf("GK169 %u PASS: stock-source-gated PORT-ONLY OTHER private storage UI; %s; no original NES gameplay parity\n",
    checks,argc==5?"three authentic private checkpoint profiles":"public negative only");
 free(snap);free(a);return 0;
}
