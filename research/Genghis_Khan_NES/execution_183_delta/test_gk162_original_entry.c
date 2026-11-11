#include "gk162_original_entry.h"
#include "gk117_stock_pristine_gate.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do{ checks++;if(!(c)){fprintf(stderr,"GK162 failure line %d: %s\n",__LINE__,#c);exit(2);} }while(0)
static unsigned checks=0;
static void run(const Gk1TemplateDataset *dataset,const char *csv,
 const char *source,Gk77Profile profile){
 Gk1App *a=calloc(1,sizeof *a),*before=malloc(sizeof *before);
 CHECK(a&&before);gk1_app_init(a);CHECK(gk1_app_install_templates(a,dataset));
 unsigned w=profile==GK77_ARCHIVED_WORLD_SAVERAM?1u:0u;
 /* An earlier Enhanced chain must never leak into a new source profile. */
 a->enhanced_assign.active=1;a->enhanced_summer.active=1;
 a->enhanced_fall.active=1;a->enhanced_winter.active=1;
 a->enhanced_year2.active=1;a->enhanced_continuation.active=1;
 a->rules_profile=0u;a->screen=GK1_SCREEN_SPRING_ARCHIVE;
 *before=*a;
 CHECK(gk162_enter_original_checkpoint(a,csv,"/not-a-real-private-source",profile,0)==GK162_ENTRY_SOURCE);
 CHECK(!memcmp(before,a,sizeof *a));
 Gk77Profile mismatch=profile==GK77_ARCHIVED_WORLD_SAVERAM?
      GK77_FRESH_MONGOL_WORKRAM:GK77_ARCHIVED_WORLD_SAVERAM;
 CHECK(gk162_enter_original_checkpoint(a,csv,source,mismatch,0)==GK162_ENTRY_SOURCE);
 CHECK(!memcmp(before,a,sizeof *a));
 CHECK(gk162_enter_original_checkpoint(a,csv,source,profile,0)==GK162_ENTRY_OK);
 CHECK(a->screen==GK1_SCREEN_STOCK_CONTINUE&&a->rules_profile==0u&&
       a->active_world==w&&a->stock_checkpoint.source_verified);
 CHECK(a->stock_checkpoint.orders.remaining==3u&&
       a->stock_checkpoint.orders.season==GK71_SPRING&&
       a->spring_stock_count==(w?27u:14u));
 CHECK(!a->enhanced_assign.active&&!a->enhanced_summer.active&&
       !a->enhanced_fall.active&&!a->enhanced_winter.active&&
       !a->enhanced_year2.active&&!a->enhanced_continuation.active);
 CHECK(!memcmp(&a->worlds[0],&before->worlds[0],sizeof a->worlds[0])&&
       !memcmp(&a->worlds[1],&before->worlds[1],sizeof a->worlds[1]));
 CHECK(gk85_raw_source_intact(&a->stock_checkpoint)&&
       gk88_stock_projection_intact(&a->stock_checkpoint));
 Gk79Checkpoint *original=malloc(sizeof *original);CHECK(original);
 *original=a->stock_checkpoint;
 CHECK(gk1_app_input(a,GK1_INPUT_SETUP));
 CHECK(a->screen==GK1_SCREEN_STOCK_COMMANDS);
 a->stock_command_cursor=6u;CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM));
 CHECK(a->screen==GK1_SCREEN_STOCK_CONTINUE&&a->stock_checkpoint.orders.remaining==3u);
 CHECK(!memcmp(original->stock_raw,a->stock_checkpoint.stock_raw,sizeof original->stock_raw));
 CHECK(gk1_app_input(a,GK1_INPUT_SETUP));
 a->stock_command_cursor=12u;CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM));
 CHECK(a->stock_checkpoint.pending_pass==1u&&a->stock_checkpoint.orders.remaining==3u);
 CHECK(gk1_app_input(a,GK1_INPUT_CANCEL));
 CHECK(a->stock_checkpoint.pending_pass==0u&&a->stock_checkpoint.orders.remaining==3u);
 CHECK(!memcmp(&original->world,&a->stock_checkpoint.world,sizeof original->world));
 CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM));CHECK(a->stock_checkpoint.pending_pass==1u);
 CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM));
 CHECK(a->stock_checkpoint.orders.remaining==0u&&a->stock_checkpoint.orders.ended==1u);
 CHECK(!memcmp(&original->world,&a->stock_checkpoint.world,sizeof original->world));
 CHECK(!memcmp(original->stock_raw,a->stock_checkpoint.stock_raw,sizeof original->stock_raw));
 CHECK(gk88_stock_projection_intact(&a->stock_checkpoint));
 const char *receipt="GK162_private_receipt.gks";remove(receipt);
 CHECK(gk79_save_receipt_new(receipt,&a->stock_checkpoint)==GK79_IO_OK);
 CHECK(gk79_save_receipt_new(receipt,&a->stock_checkpoint)==GK79_IO_EXISTS);
 /* A cold load must first repeat genuine source authentication. */
 Gk79Checkpoint *cold=malloc(sizeof *cold);CHECK(cold);
 CHECK(gk117_open_attested_original(csv,&a->worlds[w],source,profile,0,cold)==GK117_OK);
 CHECK(gk79_load_receipt(receipt,cold)==GK79_IO_OK);
 CHECK(cold->orders.remaining==0u&&cold->orders.ended==1u);
 CHECK(!memcmp(&original->world,&cold->world,sizeof original->world));
 CHECK(remove(receipt)==0);
 /* Existing original session is NEVER partially clobbered by bad source. */
 *before=*a;
 CHECK(gk162_enter_original_checkpoint(a,csv,"/missing",profile,0)==GK162_ENTRY_SOURCE);
 CHECK(!memcmp(before,a,sizeof *a));
 free(cold);free(original);free(before);free(a);
}
static void switch_regression(const Gk1TemplateDataset *dataset,
 const char *csv,const char *fresh,const char *mongol,const char *world){
 Gk1App *a=calloc(1,sizeof *a),*before=malloc(sizeof *before);
 CHECK(a&&before);gk1_app_init(a);CHECK(gk1_app_install_templates(a,dataset));
 CHECK(gk162_enter_original_checkpoint(a,csv,fresh,GK77_FRESH_MONGOL_WORKRAM,0)==GK162_ENTRY_OK);
 CHECK(a->active_world==0u&&a->spring_stock_count==14u&&a->stock_checkpoint.year==1175u);
 a->enhanced_year2.active=1u;a->enhanced_winter.active=1u;
 a->enhanced_continuation.active=1u;a->stock_storage_request=2u;
 a->spring_archive_cursor=13u;a->spring_preview_mode=1u;
 a->stock_trade_item=16u;
 *before=*a;
 CHECK(gk162_enter_original_checkpoint(a,csv,mongol,GK77_FRESH_MONGOL_WORKRAM,0)==GK162_ENTRY_SOURCE);
 CHECK(memcmp(before,a,sizeof *a)==0);
 CHECK(gk162_enter_original_checkpoint(a,csv,world,GK77_ARCHIVED_WORLD_SAVERAM,0)==GK162_ENTRY_OK);
 CHECK(a->active_world==1u&&a->spring_stock_count==27u&&a->stock_checkpoint.year==1206u);
 CHECK(!a->enhanced_year2.active&&!a->enhanced_winter.active&&
  !a->stock_trade_item&&!a->spring_archive_cursor&&!a->spring_preview_mode&&
  !a->enhanced_continuation.active&&!a->stock_storage_request);
 CHECK(gk85_raw_source_intact(&a->stock_checkpoint)&&gk88_stock_projection_intact(&a->stock_checkpoint));
 a->rules_profile=1u;a->spring_archive_cursor=26u;a->enhanced_year2.active=1u;a->enhanced_continuation.active=1u;
 CHECK(gk162_enter_original_checkpoint(a,csv,mongol,GK77_ARCHIVED_MONGOL_SAVERAM,0)==GK162_ENTRY_OK);
 CHECK(a->rules_profile==1u&&a->active_world==0u&&a->spring_stock_count==14u&&a->stock_checkpoint.year==1175u);
 CHECK(!a->spring_archive_cursor&&!a->enhanced_year2.active&&!a->enhanced_continuation.active);
 CHECK(gk85_raw_source_intact(&a->stock_checkpoint));
 free(before);free(a);
}
int main(int argc,char **argv){
 CHECK(argc==2||argc==5);
 Gk1TemplateDataset *dataset=malloc(sizeof *dataset);CHECK(dataset);
 CHECK(gk1_template_load(argv[1],dataset)==GK1_LOAD_OK);
 CHECK(gk1_verified_original_csv(argv[1]));
 Gk1App *app=calloc(1,sizeof *app);CHECK(app);gk1_app_init(app);
 CHECK(gk162_enter_original_checkpoint(NULL,argv[1],"bad",GK77_FRESH_MONGOL_WORKRAM,0)==GK162_ENTRY_ARGUMENT);
 CHECK(gk162_enter_original_checkpoint(app,argv[1],"bad",GK77_FRESH_MONGOL_WORKRAM,0)==GK162_ENTRY_TEMPLATE);
 free(app);
 if(argc==5){
  run(dataset,argv[1],argv[2],GK77_FRESH_MONGOL_WORKRAM);
  run(dataset,argv[1],argv[3],GK77_ARCHIVED_MONGOL_SAVERAM);
  run(dataset,argv[1],argv[4],GK77_ARCHIVED_WORLD_SAVERAM);
  switch_regression(dataset,argv[1],argv[2],argv[3],argv[4]);
  printf("GK162 %u checks PASS: real archived/fresh original-stock Spring CONTINUATION, controller VIEW/PASS, cancel, save/cold-load, profile isolation. No independent source generation.\n",checks);
 }else printf("GK162 %u public negative controls PASS; private source not provided.\n",checks);
 free(dataset);return 0;
}
