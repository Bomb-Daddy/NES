#include "gk1_app_state.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"GK212 FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char **argv){
 CHECK(argc==2||argc==4);
 CHECK(gk1_verified_original_csv(argv[1]));
 Gk1TemplateDataset data={0};
 CHECK(gk1_template_load(argv[1],&data)==GK1_LOAD_OK);
 Gk1LoadedScenario original={0};
 CHECK(gk1_open_loaded_scenario(&data,GK1_MONGOL_CONQUEST,&original)==GK1_LOAD_OK);
 Gk79Checkpoint fake={0};Gk212Variation result,sentinel;
 memset(&result,0xA5,sizeof result);sentinel=result;
 CHECK(gk212_compare_original_mongol_raw(argv[1],&original,&fake,&fake,&result)==GK212_WRONG_PROFILE);
 CHECK(memcmp(&result,&sentinel,sizeof result)==0);
 CHECK(gk212_compare_original_mongol_raw(argv[1],&original,NULL,&fake,&result)==GK212_INVALID);
 CHECK(memcmp(&result,&sentinel,sizeof result)==0);
 if(argc==4){
  Gk79Checkpoint fresh={0},archived={0};
  CHECK(gk79_open_original(&original,argv[2],GK77_FRESH_MONGOL_WORKRAM,0,&fresh)==GK79_OK);
  CHECK(gk79_open_original(&original,argv[3],GK77_ARCHIVED_MONGOL_SAVERAM,0,&archived)==GK79_OK);
  Gk79Checkpoint fresh0=fresh,archived0=archived;
  CHECK(gk212_compare_original_mongol_raw(argv[1],&original,&fresh,&archived,&result)==GK212_OK);
  CHECK(memcmp(&fresh,&fresh0,sizeof fresh)==0);
  CHECK(memcmp(&archived,&archived0,sizeof archived)==0);
  CHECK(result.country_count==14&&result.cells_compared==14u*83u);
  unsigned sum_offset=0,sum_country=0,n_offsets=0,n_countries=0;
  for(unsigned off=0;off<83;off++){
   unsigned changed=0;
   for(unsigned i=0;i<14;i++)
    changed+=(fresh.stock_raw[i][off]!=archived.stock_raw[i][off]);
   CHECK(result.differences_per_offset[off]==changed);
   CHECK(result.equal_in_two_histories[off]==(changed==0));
   n_offsets+=(changed!=0);sum_offset+=changed;
  }
  for(unsigned c=0;c<14;c++){
   unsigned changed=0;
   for(unsigned i=0;i<83;i++)
    changed+=(fresh.stock_raw[c][i]!=archived.stock_raw[c][i]);
   CHECK(result.differences_per_country[c]==changed);
   n_countries+=(changed!=0);sum_country+=changed;
  }
  CHECK(result.differing_cells==sum_offset&&sum_offset==sum_country);
  CHECK(result.offsets_differing==n_offsets&&result.countries_differing==n_countries);
  CHECK(result.differing_cells>0 && result.numeric_metric_differences>0);
  Gk112PairedHistories metrics={0};
  CHECK(gk112_compare_mongol_histories(argv[1],&original,&fresh,&archived,&metrics)==GK112_OK);
  unsigned total=0,n_fields=0;
  for(unsigned field=0;field<GK112_METRIC_COUNT;field++){
   total+=metrics.countries_changed[field];n_fields+=(metrics.countries_changed[field]>0);
  }
  CHECK(result.numeric_metric_differences==total&&result.numeric_fields_differing==n_fields);
  Gk1App app;gk1_app_init(&app);
  CHECK(gk1_app_install_templates(&app,&data));
  Gk1App old=app;Gk212Variation from_app={0};
  CHECK(gk1_app_original_raw_variation(&app,argv[1],&fresh,&archived,&from_app)==GK212_OK);
  CHECK(memcmp(&from_app,&result,sizeof result)==0);
  CHECK(memcmp(&old,&app,sizeof app)==0);
  memset(&from_app,0x8D,sizeof from_app);Gk212Variation unchanged=from_app;
  CHECK(gk212_compare_original_mongol_raw(argv[1],&original,&archived,&fresh,&from_app)==GK212_WRONG_PROFILE);
  CHECK(memcmp(&unchanged,&from_app,sizeof unchanged)==0);
  fresh.stock_raw[13][82]^=1; /* OUT-OF-PROJECTION BYTE CORRUPTION */
  CHECK(gk212_compare_original_mongol_raw(argv[1],&original,&fresh,&archived,&from_app)==GK212_SOURCE_REFUSED);
  CHECK(memcmp(&unchanged,&from_app,sizeof unchanged)==0);
  fresh=fresh0;
  archived.world.countries[13].skill++;
  CHECK(gk212_compare_original_mongol_raw(argv[1],&original,&fresh,&archived,&from_app)==GK212_SOURCE_REFUSED);
  CHECK(memcmp(&unchanged,&from_app,sizeof unchanged)==0);
  archived=archived0;
  original.countries[13].morale++;
  CHECK(gk212_compare_original_mongol_raw(argv[1],&original,&fresh,&archived,&from_app)==GK212_SETUP_REFUSED);
  CHECK(memcmp(&unchanged,&from_app,sizeof unchanged)==0);
  original.countries[13].morale--;
  CHECK(gk212_compare_original_mongol_raw("/missing/canonical.csv",&original,&fresh,&archived,&from_app)==GK212_SETUP_REFUSED);
  CHECK(memcmp(&unchanged,&from_app,sizeof unchanged)==0);
  printf("GK212 VERIFIED_STATIC ORIGINAL STOCK 2-HISTORY: raw %u/%u differing bytes, %u/83 byte offsets, %u/14 countries; numeric fields %u/24; no startup RNG inferred\n",
   result.differing_cells,result.cells_compared,result.offsets_differing,
   result.countries_differing,result.numeric_fields_differing);
 }else puts("GK212 public source-refusal controls PASS; authentic private profile comparisons NOT RUN");
 return 0;
}
