#include "gk1_app_state.h"
#include "gk77_stock_spring.h"
#include "gk79_stock_checkpoint.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(x) do{if(!(x)){fprintf(stderr,"GK85 failure at %d: %s\n",__LINE__,#x);return 1;}}while(0)
static int check_private(const Gk1LoadedScenario *pre,const char *path,Gk77Profile profile,unsigned count){
 Gk77StockSpring private_source={0};
 REQUIRE(gk77_load(path,profile,&private_source)==GK77_OK);
 REQUIRE(private_source.country_count==count);
 Gk79Checkpoint *cp=(Gk79Checkpoint*)calloc(1,sizeof *cp);REQUIRE(cp);
 REQUIRE(gk79_open_original(pre,path,profile,0,cp)==GK79_OK);
 uint8_t all[GK77_RECORD_BYTES*GK77_MAX_COUNTRIES];
 memset(all,0,sizeof all);
 for(unsigned i=0;i<count;i++){
  REQUIRE(memcmp(cp->stock_raw[i],private_source.countries[i].raw,83)==0);
  memcpy(all+i*83,cp->stock_raw[i],83);
  REQUIRE((unsigned)cp->world.countries[i].country_id==i+1u);
 }
 uint8_t digest[32];gk1_sha256_bytes(all,count*83u,digest);
 REQUIRE(memcmp(digest,cp->raw_sha256,32)==0);
 REQUIRE(gk85_raw_source_intact(cp)==1);
 Gk79Checkpoint corrupted=*cp;
 corrupted.stock_raw[count-1u][82u]^=1u;
 REQUIRE(gk85_raw_source_intact(&corrupted)==0);
 Gk79Checkpoint *rollback_before=malloc(sizeof *rollback_before);REQUIRE(rollback_before);
 *rollback_before=corrupted;
 REQUIRE(gk79_pass(&corrupted)==GK79_BAD);
 REQUIRE(memcmp(rollback_before,&corrupted,sizeof corrupted)==0);
 Gk1App *bad_app=malloc(sizeof *bad_app);REQUIRE(bad_app);gk1_app_init(bad_app);
 bad_app->screen=GK1_SCREEN_STOCK_CONTINUE;bad_app->stock_checkpoint=corrupted;
 REQUIRE(gk1_app_input(bad_app,GK1_INPUT_LABOR_PLAN)==1);
 REQUIRE(bad_app->stock_checkpoint.raw_view==0u);
 REQUIRE(gk1_app_input(bad_app,GK1_INPUT_CONFIRM)==1);
 REQUIRE(bad_app->stock_checkpoint.pending_pass==1u);
 REQUIRE(gk1_app_input(bad_app,GK1_INPUT_CONFIRM)==0);
 REQUIRE(bad_app->stock_checkpoint.orders.remaining==3u);
 free(bad_app);free(rollback_before);
 Gk79Checkpoint *before=malloc(sizeof *before);REQUIRE(before);
 *before=*cp;
 Gk1App *a=malloc(sizeof *a);REQUIRE(a);
 gk1_app_init(a);a->screen=GK1_SCREEN_STOCK_CONTINUE;a->stock_checkpoint=*cp;
 REQUIRE(gk1_app_input(a,GK1_INPUT_LABOR_PLAN)==1);
 REQUIRE(a->stock_checkpoint.raw_view==1u);
 REQUIRE(memcmp(before->stock_raw,a->stock_checkpoint.stock_raw,sizeof before->stock_raw)==0);
 REQUIRE(memcmp(before->raw_sha256,a->stock_checkpoint.raw_sha256,32)==0);
 REQUIRE(gk1_app_input(a,GK1_INPUT_RIGHT)==1);
 REQUIRE(a->stock_checkpoint.view_index==1u);
 REQUIRE(gk1_app_input(a,GK1_INPUT_LABOR_PLAN)==1);
 REQUIRE(a->stock_checkpoint.raw_view==0u);
 REQUIRE(gk1_app_input(a,GK1_INPUT_CONFIRM)==1); /* pending only */
 REQUIRE(a->stock_checkpoint.pending_pass==1u&&a->stock_checkpoint.orders.remaining==3);
 REQUIRE(gk1_app_input(a,GK1_INPUT_CANCEL)==1);
 REQUIRE(a->stock_checkpoint.pending_pass==0&&a->stock_checkpoint.orders.remaining==3);
 REQUIRE(memcmp(before->stock_raw,a->stock_checkpoint.stock_raw,sizeof before->stock_raw)==0);
 REQUIRE(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 REQUIRE(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 REQUIRE(a->stock_checkpoint.orders.ended&&a->stock_checkpoint.orders.remaining==0);
 REQUIRE(memcmp(before->stock_raw,a->stock_checkpoint.stock_raw,sizeof before->stock_raw)==0);
 REQUIRE(memcmp(before->raw_sha256,a->stock_checkpoint.raw_sha256,32)==0);
 REQUIRE(memcmp(before->world.countries,a->stock_checkpoint.world.countries,sizeof before->world.countries)==0);
 free(a);free(before);free(cp);return 0;
}
static int check_synthetic(const Gk1LoadedScenario *world,Gk77Profile p){
 Gk77StockSpring s={0};s.profile=p;s.scenario=(uint8_t)world->scenario_id;
 s.country_count=world->country_count;s.orders=3;s.rate=137;
 s.year=(uint16_t)(s.scenario==1?1175:1206);
 for(unsigned i=0;i<s.country_count;i++){
  const Gk1CountryTemplate *c=&world->countries[i];Gk77Country *r=&s.countries[i];
  r->arms=c->arms;r->skill=c->skill;r->troops=c->troops;r->economy=c->economy;
  r->town=c->town;r->defense=c->defense;r->masons=c->masons;r->artisans=c->artisans;
  r->food_workers=c->food_workers;r->gold=c->gold;r->morale=c->morale;r->tax=c->tax;
  r->food=c->goods[10];r->undrafted=c->undrafted_reserve;
  for(unsigned j=0;j<10u;j++)r->goods[j]=c->goods[j];
  for(unsigned j=0;j<83u;j++)r->raw[j]=(uint8_t)(i*23u+j*7u+11u);
 }
 Gk79Checkpoint *cp=calloc(1,sizeof *cp);REQUIRE(cp);
 REQUIRE(gk79_from_decoded(world,&s,0,cp)==GK79_OK);
 REQUIRE(gk85_raw_source_intact(cp)==1);
 for(unsigned i=0;i<s.country_count;i++)
  REQUIRE(memcmp(cp->stock_raw[i],s.countries[i].raw,83)==0);
 Gk79Checkpoint *unchanged=malloc(sizeof *unchanged);REQUIRE(unchanged);
 *unchanged=*cp;cp->stock_raw[s.country_count-1][82]^=0x80u;
 Gk79Checkpoint corrupt=*cp;
 REQUIRE(!gk85_raw_source_intact(cp));
 REQUIRE(gk79_pass(cp)==GK79_BAD&&memcmp(&corrupt,cp,sizeof corrupt)==0);
 *cp=*unchanged;
 Gk1App *a=malloc(sizeof *a);REQUIRE(a);gk1_app_init(a);
 a->stock_checkpoint=*cp;a->screen=GK1_SCREEN_STOCK_CONTINUE;
 REQUIRE(gk1_app_input(a,GK1_INPUT_LABOR_PLAN)==1);
 REQUIRE(a->stock_checkpoint.raw_view==1u&&gk85_raw_source_intact(&a->stock_checkpoint));
 REQUIRE(gk1_app_input(a,GK1_INPUT_LABOR_PLAN)==1);
 REQUIRE(a->stock_checkpoint.raw_view==0u);
 REQUIRE(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 REQUIRE(gk1_app_input(a,GK1_INPUT_CANCEL)==1);
 REQUIRE(memcmp(a->stock_checkpoint.stock_raw,unchanged->stock_raw,sizeof unchanged->stock_raw)==0);
 REQUIRE(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 REQUIRE(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 REQUIRE(a->stock_checkpoint.orders.ended&&a->stock_checkpoint.orders.remaining==0);
 REQUIRE(memcmp(a->stock_checkpoint.stock_raw,unchanged->stock_raw,sizeof unchanged->stock_raw)==0);
 free(a);free(unchanged);free(cp);return 0;
}
int main(int argc,char **argv){
 REQUIRE(argc==2);
 Gk1TemplateDataset *d=malloc(sizeof *d);Gk1App *a=malloc(sizeof *a);REQUIRE(d&&a);
 REQUIRE(gk1_template_load(argv[1],d)==GK1_LOAD_OK);
 gk1_app_init(a);REQUIRE(gk1_app_install_templates(a,d));
 REQUIRE(check_synthetic(&a->worlds[0],GK77_FRESH_MONGOL_WORKRAM)==0);
 REQUIRE(check_synthetic(&a->worlds[1],GK77_ARCHIVED_WORLD_SAVERAM)==0);
 const char *fresh=getenv("GK85_PRIVATE_FRESH");
 const char *mongol=getenv("GK85_PRIVATE_MONGOL");
 const char *world=getenv("GK85_PRIVATE_WORLD");
 if(fresh&&mongol&&world){
  REQUIRE(check_private(&a->worlds[0],fresh,GK77_FRESH_MONGOL_WORKRAM,14)==0);
  REQUIRE(check_private(&a->worlds[0],mongol,GK77_ARCHIVED_MONGOL_SAVERAM,14)==0);
  REQUIRE(check_private(&a->worlds[1],world,GK77_ARCHIVED_WORLD_SAVERAM,27)==0);
  puts("PRIVATE AUTHENTIC STOCK RAM: 41/41 full 83-byte raw records, SHA, controller view, pass and rollback PASS");
 }else{
  /* Public tests must not claim authentic NES runtime validation. */
  Gk77Country c={0};uint8_t raw[83];for(unsigned i=0;i<83;i++)raw[i]=(uint8_t)(i*7u+3u);
  REQUIRE(gk77_decode(raw,83,&c)==GK77_OK&&memcmp(raw,c.raw,83)==0);
  Gk77Country prior=c;
  REQUIRE(gk77_decode(raw,82,&c)==GK77_INVALID&&memcmp(&prior,&c,sizeof c)==0);
  puts("PUBLIC SYNTHETIC all 41 country raw-byte preservation/controller view/negative integrity/rollback PASS (private source not supplied)");
 }
 free(a);free(d);return 0;
}
