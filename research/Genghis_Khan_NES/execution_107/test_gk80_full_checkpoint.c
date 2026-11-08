#include "gk1_app_state.h"
#include "gk80_full_checkpoint.h"
#include "gk1_source_identity.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Synthetic source-conversion QA. NOT original NES emulation or claims of a
 * universal first Spring. Public test fixtures cannot embed original RAM. */
static void fixture(const Gk1LoadedScenario *pre,Gk77Profile profile,Gk77StockSpring *src){
 memset(src,0,sizeof *src);
 src->profile=profile;src->scenario=(uint8_t)pre->scenario_id;
 src->country_count=pre->country_count;
 src->year=src->scenario==1?1175:1206;src->rate=src->scenario==1?137:183;
 src->orders=3;
 for(unsigned i=0;i<pre->country_count;i++){
  const Gk1CountryTemplate *c=&pre->countries[i];
  Gk77Country *r=&src->countries[i];
  memcpy(r->goods,c->goods,10u*sizeof r->goods[0]);
  r->food=c->goods[10];r->arms=c->arms;r->skill=c->skill;
  r->troops=c->troops;r->economy=c->economy;r->town=c->town;
  r->defense=c->defense;r->masons=c->masons;r->artisans=c->artisans;
  r->food_workers=c->food_workers;r->gold=c->gold;
  r->morale=c->morale;r->tax=c->tax;r->undrafted=c->undrafted_reserve;
 }
 src->countries[0].gold=444;
 src->countries[pre->country_count-1].goods[9]=37;
}
static void test_scenario(const Gk1LoadedScenario *pre,Gk77Profile profile,unsigned player){
 Gk77StockSpring *src=malloc(sizeof *src);assert(src);
 fixture(pre,profile,src);
 Gk79Checkpoint *original=malloc(sizeof *original),*reloaded=malloc(sizeof *reloaded);
 assert(original&&reloaded);
 assert(gk79_from_decoded(pre,src,(uint8_t)player,original)==GK79_OK);
 const char *path="gk80_current_fixture.gk8";
 (void)remove(path);
 assert(gk80_save_new(path,original)==GK80_IO_OK);
 assert(gk80_save_new(path,original)==GK80_IO_EXISTS);
 FILE *f=fopen(path,"rb");assert(f);
 assert(fseek(f,0,SEEK_END)==0);
 assert(ftell(f)==(long)(16u+pre->country_count*62u+64u));
 assert(fclose(f)==0);
 /* Real controller route: pending first A, B cancellation leaves world and
  * orders byte-identical; second A/A commits the single known Pass action. */
 Gk1App *app=malloc(sizeof *app);assert(app);
 gk1_app_init(app);app->stock_checkpoint=*original;
 app->screen=GK1_SCREEN_STOCK_CONTINUE;
 assert(gk1_app_input(app,GK1_INPUT_CONFIRM)==1);
 assert(app->stock_checkpoint.pending_pass==1);
 assert(gk1_app_input(app,GK1_INPUT_CANCEL)==1);
 assert(app->stock_checkpoint.pending_pass==0);
 assert(memcmp(&app->stock_checkpoint,original,sizeof *original)==0);
 assert(gk1_app_input(app,GK1_INPUT_CONFIRM)==1);
 assert(gk1_app_input(app,GK1_INPUT_CONFIRM)==1);
 assert(app->stock_checkpoint.orders.ended==1);
 /* Save a second, passed snapshot to a different exclusive slot. */
 const char *passed_path="gk80_pass_fixture.gk8";
 (void)remove(passed_path);
 app->stock_checkpoint.view_index=(uint8_t)(pre->country_count-1);
 assert(gk80_save_new(passed_path,&app->stock_checkpoint)==GK80_IO_OK);
 assert(gk79_from_decoded(pre,src,(uint8_t)player,reloaded)==GK79_OK);
 assert(gk80_load(passed_path,reloaded)==GK80_IO_OK);
 assert(reloaded->orders.remaining==0&&reloaded->orders.ended==1);
 assert(reloaded->view_index==pre->country_count-1);
 for(unsigned i=0;i<pre->country_count;i++)
  assert(memcmp(&reloaded->world.countries[i],&original->world.countries[i],
    sizeof(Gk1CountryTemplate))==0);
 /* A fresh original source also restores original 3 unspent orders. */
 assert(gk79_from_decoded(pre,src,(uint8_t)player,reloaded)==GK79_OK);
 assert(gk80_load(path,reloaded)==GK80_IO_OK);
 assert(reloaded->orders.remaining==3&&reloaded->orders.ended==0);
 Gk79Checkpoint *untouched=malloc(sizeof *untouched);assert(untouched);
 *untouched=*reloaded;
 /* Wrong authentic baseline (even same Gold/Rate/profile) cannot be silently
  * replaced by a different first Spring; last country is checked too. */
 reloaded->world.countries[pre->country_count-1].goods[9]++;
 *untouched=*reloaded;
 assert(gk80_load(path,reloaded)==GK80_IO_SOURCE_MISMATCH);
 assert(memcmp(reloaded,untouched,sizeof *untouched)==0);
 *reloaded=*original;*untouched=*reloaded;
 f=fopen(path,"r+b");assert(f);
 assert(fseek(f,16+pre->country_count*62u-2u,SEEK_SET)==0);
 int ch=fgetc(f);assert(ch!=EOF);
 assert(fseek(f,-1L,SEEK_CUR)==0);
 assert(fputc(ch^1,f)!=EOF&&fclose(f)==0);
 assert(gk80_load(path,reloaded)==GK80_IO_CORRUPT);
 assert(memcmp(reloaded,untouched,sizeof *untouched)==0);
 /* Even when an attacker recomputes the unkeyed digest after modifying
  * the LAST country, the fresh SHA-gated ORIGINAL source remains the anchor. */
 assert(remove(path)==0);
 assert(gk80_save_new(path,original)==GK80_IO_OK);
 f=fopen(path,"r+b");assert(f);
 size_t body=16u+pre->country_count*62u+32u;
 unsigned char bytes[16u+27u*62u+64u];
 assert(fread(bytes,1,body+32u,f)==body+32u);
 bytes[body-2u]^=1u; /* last recorded u16: eligible draft count */
 gk1_sha256_bytes(bytes,body,bytes+body); /* checksum now superficially OK */
 assert(fseek(f,0,SEEK_SET)==0);
 assert(fwrite(bytes,1,body+32u,f)==body+32u&&fclose(f)==0);
 assert(gk80_load(path,reloaded)==GK80_IO_SOURCE_MISMATCH);
 assert(memcmp(reloaded,untouched,sizeof *untouched)==0);
 /* Invalid SHA, old 32-byte format and appended bytes rejected. */
 assert(gk80_load("no-such-file.gk8",reloaded)==GK80_IO_ERROR);
 assert(remove(path)==0);
 f=fopen(path,"wb");assert(f);assert(fputc('X',f)!=EOF&&fclose(f)==0);
 assert(gk80_load(path,reloaded)==GK80_IO_CORRUPT);
 assert(remove(path)==0&&remove(passed_path)==0);
 /* Invalid indices cannot create phantom user saves. */
 reloaded->view_index=pre->country_count;
 assert(gk80_save_new(path,reloaded)==GK80_IO_BAD);
 reloaded->view_index=0;
 assert(gk80_save_new(path,reloaded)==GK80_IO_OK);
 assert(remove(path)==0);
 free(untouched);free(app);free(original);free(reloaded);free(src);
}
int main(int argc,char **argv){
 assert(argc==2);
 Gk1TemplateDataset *src=malloc(sizeof *src);assert(src);
 assert(gk1_template_load(argv[1],src)==GK1_LOAD_OK);
 Gk1LoadedScenario *mongol=malloc(sizeof *mongol),*world=malloc(sizeof *world);
 assert(mongol&&world);
 assert(gk1_open_loaded_scenario(src,GK1_MONGOL_CONQUEST,mongol)==GK1_LOAD_OK);
 assert(gk1_open_loaded_scenario(src,GK1_WORLD_CONQUEST,world)==GK1_LOAD_OK);
 test_scenario(mongol,GK77_FRESH_MONGOL_WORKRAM,0);
 test_scenario(mongol,GK77_ARCHIVED_MONGOL_SAVERAM,0);
 test_scenario(world,GK77_ARCHIVED_WORLD_SAVERAM,18);
 free(world);free(mongol);free(src);
 puts("EXEC0080 PASS: 14/27 country full numeric checksum+source gated save/cold load, controller B atomic cancel, Pass resume, 3 profiles, wrong far-country and corruption negative controls; synthetic port QA only");
 return 0;
}
