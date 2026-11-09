#include "gk117_stock_pristine_gate.h"
#include "gk1_source_identity.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char **argv){
 assert(argc==2||argc==3||argc==5);
 const char *csv=argv[1];
 Gk1TemplateDataset *d=malloc(sizeof *d);
 Gk1LoadedScenario *pre=malloc(sizeof *pre);
 Gk79Checkpoint *out=malloc(sizeof *out);
 Gk79Checkpoint *sentinel=malloc(sizeof *sentinel);
 assert(d&&pre&&out&&sentinel&&gk1_verified_original_csv(csv));
 assert(gk1_template_load(csv,d)==GK1_LOAD_OK);
 assert(gk1_open_loaded_scenario(d,GK1_MONGOL_CONQUEST,pre)==GK1_LOAD_OK);
 memset(out,0xa5,sizeof *out);*sentinel=*out;
 assert(gk117_open_attested_original(NULL,pre,"file",GK77_FRESH_MONGOL_WORKRAM,0,out)==GK117_ARGUMENT);
 assert(gk117_open_attested_original(csv,NULL,"file",GK77_FRESH_MONGOL_WORKRAM,0,out)==GK117_ARGUMENT);
 assert(gk117_open_attested_original(csv,pre,"file",0,0,out)==GK117_ARGUMENT);
 assert(gk117_open_attested_original("/missing",pre,"file",GK77_FRESH_MONGOL_WORKRAM,0,out)==GK117_CSV_SOURCE_REFUSED);
 assert(gk117_open_attested_original(csv,pre,"/missing",GK77_FRESH_MONGOL_WORKRAM,0,out)==GK117_ORIGINAL_RECORD_REFUSED);
 assert(memcmp(out,sentinel,sizeof *out)==0);
 /* Previously accepted as an ORIGINAL checkpoint, even though an unsourced
  * nonplayer source-template field had been silently altered. */
 pre->countries[13].extra_tax_used^=1u;
 assert(gk117_open_attested_original(csv,pre,"/missing",GK77_FRESH_MONGOL_WORKRAM,0,out)==GK117_TEMPLATE_DRIFT);
 pre->countries[13].extra_tax_used^=1u;
 pre->countries[0].country_name[0]^=1;
 assert(gk117_open_attested_original(csv,pre,"/missing",GK77_FRESH_MONGOL_WORKRAM,0,out)==GK117_TEMPLATE_DRIFT);
 pre->countries[0].country_name[0]^=1;
 pre->countries[2].goods[0]++;
 assert(gk117_open_attested_original(csv,pre,"/missing",GK77_FRESH_MONGOL_WORKRAM,0,out)==GK117_TEMPLATE_DRIFT);
 pre->countries[2].goods[0]--;
 assert(memcmp(out,sentinel,sizeof *out)==0);
 if(argc>=3){
  const char *private_records=argv[2];
  assert(gk117_open_attested_original(csv,pre,private_records,GK77_FRESH_MONGOL_WORKRAM,0,out)==GK117_OK);
  assert(out->source_verified&&out->year==1175&&out->rate==121&&out->orders.remaining==3);
  assert(out->world.countries[0].gold==430&&out->world.countries[0].goods[10]==208&&out->world.countries[0].troops==61);
  assert(gk85_raw_source_intact(out)&&gk88_stock_projection_intact(out));
  if(argc==5){
   const char *files[3]={argv[2],argv[3],argv[4]};
   const Gk77Profile profiles[3]={GK77_FRESH_MONGOL_WORKRAM,
       GK77_ARCHIVED_MONGOL_SAVERAM,GK77_ARCHIVED_WORLD_SAVERAM};
   const unsigned gold[3]={430u,439u,193u};
   const unsigned food[3]={208u,208u,332u};
   const unsigned troops[3]={61u,61u,116u};
   const unsigned rate[3]={121u,101u,114u};
   for(unsigned j=0;j<3;j++){
    Gk1ScenarioId world=(j==2)?GK1_WORLD_CONQUEST:GK1_MONGOL_CONQUEST;
    assert(gk1_open_loaded_scenario(d,world,pre)==GK1_LOAD_OK);
    assert(gk117_open_attested_original(csv,pre,files[j],profiles[j],0,out)==GK117_OK);
    assert(out->source_verified&&out->world.country_count==(j==2?27u:14u));
    assert(out->world.countries[0].gold==gold[j]&&out->world.countries[0].goods[10]==food[j]&&
           out->world.countries[0].troops==troops[j]&&out->rate==rate[j]);
    assert(out->orders.remaining==3u&&gk85_raw_source_intact(out));
    /* The canonical 41-country template cannot be bypassed by a truthful
     * stock RAM snapshot plus a modified unsourced original country field. */
    pre->countries[pre->country_count-1u].lord_name[0]^=1;
    Gk79Checkpoint snapshot=*out;
    assert(gk117_open_attested_original(csv,pre,files[j],profiles[j],0,out)==GK117_TEMPLATE_DRIFT);
    assert(memcmp(out,&snapshot,sizeof snapshot)==0);
    pre->countries[pre->country_count-1u].lord_name[0]^=1;
    /* A true wrong profile or source country count is rejected atomically. */
    Gk77Profile wrong=(j==2)?GK77_FRESH_MONGOL_WORKRAM:GK77_ARCHIVED_WORLD_SAVERAM;
    assert(gk117_open_attested_original(csv,pre,files[j],wrong,0,out)==GK117_ORIGINAL_RECORD_REFUSED);
    assert(memcmp(out,&snapshot,sizeof snapshot)==0);
   }
   assert(gk1_open_loaded_scenario(d,GK1_MONGOL_CONQUEST,pre)==GK1_LOAD_OK);
   assert(gk117_open_attested_original(csv,pre,argv[2],GK77_FRESH_MONGOL_WORKRAM,0,out)==GK117_OK);
   puts("GK117 authentic original capture 14 fresh + 14 archived + 27 world verified; no original NES rerun");
  }
  /* All 14 original source records remain unmodified by PASS (not season). */
  Gk1LoadedScenario *before=malloc(sizeof *before);assert(before);
  *before=out->world;
  assert(gk79_pass(out)==GK79_OK);
  assert(out->orders.remaining==0&&out->orders.ended==1);
  assert(memcmp(before,&out->world,sizeof *before)==0);
  assert(gk117_open_attested_original(csv,pre,private_records,GK77_ARCHIVED_MONGOL_SAVERAM,0,out)==GK117_ORIGINAL_RECORD_REFUSED);
  /* A wrong original-SRAM bank/profile cannot silently restore the session. */
  free(before);
  puts("GK117 PRIVATE original fresh-stock 14-country positive, three-order PASS, wrong bank negative PASS");
 }
 assert(gk1_open_loaded_scenario(d,GK1_WORLD_CONQUEST,pre)==GK1_LOAD_OK);
 pre->countries[26].tax^=1u;
 assert(gk117_open_attested_original(csv,pre,"/missing",GK77_ARCHIVED_WORLD_SAVERAM,0,out)==GK117_TEMPLATE_DRIFT);
 assert(gk1_open_loaded_scenario(d,GK1_WORLD_CONQUEST,pre)==GK1_LOAD_OK);
 assert(gk117_open_attested_original(csv,pre,"/missing",GK77_ARCHIVED_WORLD_SAVERAM,0,out)==GK117_ORIGINAL_RECORD_REFUSED);
 free(d);free(pre);free(out);free(sentinel);
 puts("GK117 all-world canonical source boundary, negative mutation/corruption, atomic rollback PASS");
 return 0;
}
