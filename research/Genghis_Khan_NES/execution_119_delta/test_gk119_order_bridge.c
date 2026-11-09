#include "gk1_app_state.h"
#include "gk1_source_identity.h"
#include "gk117_stock_pristine_gate.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void original_profile(const Gk1TemplateDataset *source,
 const char *csv,const char *record,Gk77Profile profile){
 Gk1App *app=calloc(1,sizeof *app);assert(app);
 gk1_app_init(app);
 assert(gk1_app_install_templates(app,source));
 const uint8_t world=(uint8_t)(profile==GK77_ARCHIVED_WORLD_SAVERAM?1u:0u);
 assert(gk117_open_attested_original(csv,&app->worlds[world],record,
   profile,0u,&app->stock_checkpoint)==GK117_OK);
 const Gk79Checkpoint original=app->stock_checkpoint;
 uint8_t remaining=99;
 app->rules_profile=1u;
 assert(gk119_effective_remaining(&app->stock_checkpoint,
   &app->enhanced_assign,app->rules_profile,&remaining)==GK119_OK);
 assert(remaining==3u);
 app->screen=GK1_SCREEN_STOCK_COMMANDS;
 app->stock_command_cursor=0u;
 assert(gk1_app_input(app,GK1_INPUT_CONFIRM));
 assert(app->screen==GK1_SCREEN_ENHANCED_ASSIGN);
 app->assign_from=0u;app->assign_to=4u;
 assert(gk1_app_input(app,GK1_INPUT_CONFIRM));
 assert(app->enhanced_assign.action_count==1u);
 assert(app->enhanced_assign.budget.remaining==2u);
 assert(gk119_effective_remaining(&app->stock_checkpoint,
   &app->enhanced_assign,app->rules_profile,&remaining)==GK119_OK);
 assert(remaining==2u);
 /* Save legacy-compatible (pass byte 0) receipt; pre-Pass coldload must work. */
 const char *path="gk119_private_compat.ga16";
 (void)remove(path);
 assert(gk116_save_new(path,&app->stock_checkpoint,
    &app->enhanced_assign)==GK116_OK);
 Gk116EnhancedAssign old_loaded={0};
 assert(gk116_cold_load(path,&app->stock_checkpoint,1u,&old_loaded)==GK116_OK);
 assert(old_loaded.pass_ended==0u && old_loaded.budget.remaining==2u);
 assert(remove(path)==0);
 assert(gk1_app_input(app,GK1_INPUT_CANCEL));
 assert(app->screen==GK1_SCREEN_STOCK_COMMANDS);
 app->stock_command_cursor=12u;
 assert(gk1_app_input(app,GK1_INPUT_CONFIRM));
 assert(app->stock_checkpoint.pending_pass==1u);
 assert(app->enhanced_assign.budget.remaining==2u);
 assert(gk1_app_input(app,GK1_INPUT_CONFIRM));
 assert(app->stock_checkpoint.pending_pass==0u);
 assert(app->enhanced_assign.pass_ended==1u);
 assert(app->enhanced_assign.budget.remaining==0u);
 assert(app->enhanced_assign.budget.ended==1u);
 assert(gk116_attestation_intact(&app->enhanced_assign,&app->stock_checkpoint));
 assert(gk119_effective_remaining(&app->stock_checkpoint,
   &app->enhanced_assign,app->rules_profile,&remaining)==GK119_OK);
 assert(remaining==0u);
 assert(memcmp(&original,&app->stock_checkpoint,sizeof original)==0);
 Gk116EnhancedAssign finished=app->enhanced_assign;
 assert(gk116_transfer(&app->enhanced_assign,&app->stock_checkpoint,0,4,1)==GK116_NO_ORDERS);
 assert(gk119_pass(&app->stock_checkpoint,&app->enhanced_assign,1)==GK119_NO_ORDERS);
 assert(memcmp(&finished,&app->enhanced_assign,sizeof finished)==0);
 /* Pass flag is durable, old 96-byte receipt format and old positions held. */
 assert(gk116_save_new(path,&app->stock_checkpoint,
    &app->enhanced_assign)==GK116_OK);
 FILE *f=fopen(path,"rb");assert(f);
 unsigned char bytes[97];size_t got=fread(bytes,1,sizeof bytes,f);assert(fclose(f)==0);
 assert(got==96u && bytes[14]==1u && bytes[13]==1u);
 Gk116EnhancedAssign saved={0};
 assert(gk116_cold_load(path,&app->stock_checkpoint,1u,&saved)==GK116_OK);
 assert(!memcmp(&saved,&app->enhanced_assign,sizeof saved));
 /* Corrupt Pass flag, payload and an extra byte; each refusal is atomic. */
 for(int test=0;test<3;test++){
  f=fopen(path,"wb");assert(f);
  unsigned char altered[97];memcpy(altered,bytes,96);
  if(test==0)altered[14]=2;
  if(test==1)altered[48]^=1;
  if(test==2)altered[96]=0x5A;
  assert(fwrite(altered,1,test==2?97:96,f)==(size_t)(test==2?97:96));
  assert(fclose(f)==0);
  Gk116EnhancedAssign before=saved;
  assert(gk116_cold_load(path,&app->stock_checkpoint,1u,&saved)==GK116_CORRUPT);
  assert(!memcmp(&before,&saved,sizeof saved));
 }
 assert(remove(path)==0);
 /* Classic cannot borrow Enhanced order credit or continue a mixed budget. */
 Gk79Checkpoint stock_pre=app->stock_checkpoint;
 app->rules_profile=0;
 assert(gk119_pass(&app->stock_checkpoint,&app->enhanced_assign,
    app->rules_profile)==GK119_PROFILE);
 assert(!memcmp(&stock_pre,&app->stock_checkpoint,sizeof stock_pre));
 /* Attestation tamper fail closed; no state writes. */
 app->rules_profile=1;
 app->stock_checkpoint.stock_raw[0][0]^=1;
 Gk79Checkpoint bad=app->stock_checkpoint;
 assert(gk119_pass(&app->stock_checkpoint,&app->enhanced_assign,1)==GK119_SOURCE);
 assert(!memcmp(&bad,&app->stock_checkpoint,sizeof bad));
 free(app);
}
int main(int argc,char **argv){
 Gk79Checkpoint fake={0};Gk116EnhancedAssign unused={0};
 /* Verify route across all UI screens including back-navigation and
  * stock quote viewer; inactive sessions use original stock GK80. */
 const int live_screens[]={GK1_SCREEN_ENHANCED_ASSIGN,
    GK1_SCREEN_STOCK_CONTINUE,GK1_SCREEN_STOCK_COMMANDS,
    GK1_SCREEN_STOCK_TRADE_REFERENCE};
 for(unsigned i=0;i<4;i++){
  assert(gk119_receipt_screen(live_screens[i],&unused)==(i==0));
  unused.active=1u;
  assert(gk119_receipt_screen(live_screens[i],&unused)==1);
  unused.active=0u;
 }
 assert(!gk119_receipt_screen(GK1_SCREEN_TITLE,&unused));
 unused.active=1u;
 assert(!gk119_receipt_screen(GK1_SCREEN_CAMPAIGNS,&unused));
 assert(!gk119_receipt_screen(GK1_SCREEN_STOCK_COMMANDS,NULL));
 unused.active=0u;
 uint8_t orders=9;
 assert(gk119_effective_remaining(&fake,&unused,1,&orders)==GK119_SOURCE);
 assert(orders==9);
 assert(gk119_pass(&fake,&unused,1)==GK119_SOURCE);
 assert(gk116_pass_remaining(&unused,&fake)==GK116_SOURCE_CHANGED);
 if(argc!=5){puts("GK119 public negative/rollback checks PASS; authentic stock positive NOT RUN (private files not supplied)");return 0;}
 Gk1TemplateDataset *source=malloc(sizeof *source);assert(source);
 assert(gk1_template_load(argv[1],source)==GK1_LOAD_OK);
 assert(gk1_verified_original_csv(argv[1]));
 original_profile(source,argv[1],argv[2],GK77_FRESH_MONGOL_WORKRAM);
 original_profile(source,argv[1],argv[3],GK77_ARCHIVED_MONGOL_SAVERAM);
 original_profile(source,argv[1],argv[4],GK77_ARCHIVED_WORLD_SAVERAM);
 free(source);
 puts("GK119 PASS: authentic private stock fresh/archived/world source attestation, one shared Enhanced 3-order budget, Pass after Assign, cold load old/new receipts, corruption and rollback, Classic isolation; NO new NES Mesen run");
 return 0;
}
