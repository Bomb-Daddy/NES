#include "gk1_app_state.h"
#include "gk117_stock_pristine_gate.h"
#include "gk1_source_identity.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint16_t decode_le(const uint8_t *b){return (uint16_t)((unsigned)b[0]|((unsigned)b[1]<<8u));}
static void public_refusal(void){
 static const char *const names[10]={"FUR","SILK","JEWELS","METALS","MEDICINES",
    "WOODWORKS","CERAMICS","GLASS","TEXTILES","PERFUME"};
 for(unsigned i=0;i<10u;i++)assert(strcmp(gk147_goods_name(i),names[i])==0);
 assert(gk147_goods_name(10)==NULL);
 Gk1App *app=calloc(1,sizeof *app);assert(app);
 gk1_app_init(app);
 Gk147StockGoods untouched={0};memset(&untouched,0x5a,sizeof untouched);
 Gk147StockGoods mark=untouched;
 assert(gk1_app_source_goods_view(app,0,&untouched)==GK147_GOODS_UNVERIFIED);
 assert(!memcmp(&untouched,&mark,sizeof mark));
 assert(gk1_app_source_goods_view(app,0,NULL)==GK147_GOODS_INVALID);
 assert(gk147_source_goods(NULL,&app->enhanced_assign,0,0,&untouched)==GK147_GOODS_INVALID);
 assert(gk147_source_goods(&app->stock_checkpoint,NULL,0,0,&untouched)==GK147_GOODS_INVALID);
 assert(gk147_source_goods(&app->stock_checkpoint,&app->enhanced_assign,2,0,&untouched)==GK147_GOODS_INVALID);
 assert(gk147_source_goods(&app->stock_checkpoint,&app->enhanced_assign,0,27,&untouched)==GK147_GOODS_INVALID);
 assert(!memcmp(&untouched,&mark,sizeof mark));
 /* A synthetic checkpoint, even with apparently reasonable numeric fields,
  * is not evidence of stock presence, goods, Trade or three playable orders. */
 app->stock_checkpoint.valid=1;
 app->stock_checkpoint.world.country_count=14;
 app->stock_checkpoint.profile=GK77_FRESH_MONGOL_WORKRAM;
 app->screen=GK1_SCREEN_STOCK_CONTINUE;
 assert(gk1_app_source_goods_view(app,0,&untouched)==GK147_GOODS_UNVERIFIED);
 assert(!memcmp(&untouched,&mark,sizeof mark));
 free(app);
}
static void verified_original(const Gk1TemplateDataset *dataset,const char *csv,
 const char *file,Gk77Profile profile){
 Gk1App *app=calloc(1,sizeof *app);assert(app);
 assert(gk1_app_install_templates(app,dataset));
 const unsigned which=profile==GK77_ARCHIVED_WORLD_SAVERAM?1u:0u;
 assert(gk117_open_attested_original(csv,&app->worlds[which],file,profile,0,
    &app->stock_checkpoint)==GK117_OK);
 app->screen=GK1_SCREEN_STOCK_CONTINUE;
 const unsigned count=which?27u:14u;
 assert(app->stock_checkpoint.world.country_count==count);
 assert(app->stock_checkpoint.source_verified);
 const Gk79Checkpoint original=app->stock_checkpoint;
 Gk147StockGoods probe={0};
 for(unsigned i=0;i<count;i++){
  Gk147GoodsStatus r=gk1_app_source_goods_view(app,i,&probe);
  assert(r==GK147_GOODS_OK);
  assert(probe.country_index==i&&probe.country_count==count);
  assert(probe.profile==profile);
  const uint8_t *raw=original.stock_raw[i];
  for(unsigned j=0;j<10u;j++){
   assert(probe.goods[j]==decode_le(raw+0x22u+2u*j));
   assert(probe.goods[j]==original.world.countries[i].goods[j]);
  }
  assert(probe.food==decode_le(raw+0x36u));
  assert(probe.food==original.world.countries[i].goods[10]);
  /* Controller View cycles decoded -> raw -> finance -> setup deltas -> goods
   * -> decoded; only source-bound page navigation, no receipt mutation. */
 }
 assert(gk1_app_source_goods_view(app,count,&probe)==
   (count==GK77_MAX_COUNTRIES?GK147_GOODS_INVALID:GK147_GOODS_UNVERIFIED));
 app->stock_checkpoint.view_index=0;
 for(unsigned step=0;step<4;step++)assert(gk1_app_input(app,GK1_INPUT_LABOR_PLAN));
 assert(app->stock_reconcile_view==3u && app->stock_checkpoint.raw_view==0u);
 assert(gk1_app_source_goods_view(app,0,&probe)==GK147_GOODS_OK);
 assert(gk1_app_input(app,GK1_INPUT_LABOR_PLAN));
 assert(app->stock_reconcile_view==0u&&app->stock_checkpoint.raw_view==0u);
 assert(app->stock_checkpoint.orders.remaining==3u);
 /* Exclude UI-only cursor and source view toggles from immutable source proof. */
 assert(!memcmp(app->stock_checkpoint.stock_raw,original.stock_raw,sizeof original.stock_raw));
 assert(!memcmp(app->stock_checkpoint.raw_sha256,original.raw_sha256,32u));
 assert(!memcmp(&app->stock_checkpoint.world,&original.world,sizeof original.world));
 assert(!memcmp(&app->stock_checkpoint.orders,&original.orders,sizeof original.orders));
 /* Bad source bytes fail closed, outputs and world untouched. */
 Gk147StockGoods mark=probe;
 app->stock_checkpoint.stock_raw[0][0x22]^=1u;
 assert(gk1_app_source_goods_view(app,0,&probe)==GK147_GOODS_UNVERIFIED);
 assert(!memcmp(&probe,&mark,sizeof mark));
 app->stock_checkpoint.stock_raw[0][0x22]^=1u;
 /* Enhanced activity must never replace original specialty goods or
  * silently use its mutable world as a fake original source. */
 app->rules_profile=1u;
 assert(gk116_open(&app->stock_checkpoint,1u,&app->enhanced_assign)==GK116_OK);
 assert(gk116_transfer(&app->enhanced_assign,&app->stock_checkpoint,0,4,1u)==GK116_OK);
 assert(gk1_app_source_goods_view(app,0,&probe)==GK147_GOODS_OK);
 assert(!memcmp(&app->stock_checkpoint.world,&original.world,sizeof original.world));
 assert(app->enhanced_assign.budget.remaining==2u);
 const Gk116EnhancedAssign enhanced=app->enhanced_assign;
 app->enhanced_assign.mutable_world.countries[0].goods[0]^=1u;
 assert(gk1_app_source_goods_view(app,0,&probe)==GK147_GOODS_UNVERIFIED);
 assert(!memcmp(&probe,&mark,sizeof mark));
 app->enhanced_assign=enhanced;
 app->rules_profile=0u;
 assert(gk1_app_source_goods_view(app,0,&probe)==GK147_GOODS_UNVERIFIED);
 app->rules_profile=1u;
 assert(gk1_app_source_goods_view(app,0,&probe)==GK147_GOODS_OK);
 assert(!memcmp(&app->stock_checkpoint.world,&original.world,sizeof original.world));
 free(app);
}
int main(int argc,char **argv){
 public_refusal();
 if(argc==1){puts("GK147 public stock-source refusal / 10 names PASS; private stock cases NOT RUN");return 0;}
 assert(argc==5);
 Gk1TemplateDataset *d=malloc(sizeof *d);assert(d);
 assert(gk1_template_load(argv[1],d)==GK1_LOAD_OK);
 assert(gk1_verified_original_csv(argv[1]));
 verified_original(d,argv[1],argv[2],GK77_FRESH_MONGOL_WORKRAM);
 verified_original(d,argv[1],argv[3],GK77_ARCHIVED_MONGOL_SAVERAM);
 verified_original(d,argv[1],argv[4],GK77_ARCHIVED_WORLD_SAVERAM);
 free(d);
 puts("GK147 original Mesen fresh 14 + archived 14 + World 27 original 83B-record goods, Food, controller VIEW and immutability PASS; NO new Mesen run or source-derived Spring engine");
 return 0;
}
