#include "gk90_verified_finance.h"
#include "gk1_source_identity.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(x) do {if(!(x)){fprintf(stderr,"GK90 line %u: %s\n",(unsigned)__LINE__,#x);return 1;}}while(0)
static int check_one(const char *csv,const Gk1LoadedScenario *base,
                     const char *source_path,Gk77Profile profile){
 Gk79Checkpoint *stock=calloc(1,sizeof *stock);REQUIRE(stock);
 REQUIRE(gk79_open_original(base,source_path,profile,0,stock)==GK79_OK);
 Gk79Checkpoint original=*stock;
 Gk89SpringReconciliation gap;
 unsigned count=base->country_count;
 for(unsigned i=0;i<count;i++){
  memset(&gap,0x55,sizeof gap);
  REQUIRE(gk90_compare_spring_verified(csv,base,stock,(uint8_t)i,&gap)==GK90_OK);
  REQUIRE(gap.country_id==i+1u);
  REQUIRE(gap.residual_gold==(int32_t)gap.observed_gold-gap.finance_gold);
 }
 REQUIRE(memcmp(&original,stock,sizeof original)==0);
 /* The previous GK89 function accepted a fabricated baseline for some
  * unrelated-country fields. GK90 rejects ALL such changes atomically. */
 Gk1LoadedScenario forged=*base;
 unsigned other=count-1u;
 forged.countries[other].gold++;
 memset(&gap,0xA5,sizeof gap);Gk89SpringReconciliation sentinel=gap;
 REQUIRE(gk90_compare_spring_verified(csv,&forged,stock,0,&gap)==GK90_TEMPLATE_DRIFT);
 REQUIRE(memcmp(&gap,&sentinel,sizeof gap)==0);
 forged=*base;forged.countries[other].country_rom_offset++;
 REQUIRE(gk90_compare_spring_verified(csv,&forged,stock,0,&gap)==GK90_TEMPLATE_DRIFT);
 REQUIRE(memcmp(&gap,&sentinel,sizeof gap)==0);
 forged=*base;forged.countries[other].specialty_name[0]^=1;
 REQUIRE(gk90_compare_spring_verified(csv,&forged,stock,0,&gap)==GK90_TEMPLATE_DRIFT);
 REQUIRE(memcmp(&gap,&sentinel,sizeof gap)==0);
 forged=*base;forged.countries[0].eligibility_percent++;
 REQUIRE(gk90_compare_spring_verified(csv,&forged,stock,0,&gap)==GK90_TEMPLATE_DRIFT);
 REQUIRE(memcmp(&gap,&sentinel,sizeof gap)==0);
 REQUIRE(gk90_compare_spring_verified("/definitely/missing/gk1.csv",base,stock,0,&gap)==GK90_CSV_NOT_AUTHENTIC);
 REQUIRE(memcmp(&gap,&sentinel,sizeof gap)==0);
 stock->stock_raw[0][11]^=1;
 REQUIRE(gk90_compare_spring_verified(csv,base,stock,0,&gap)==GK90_STOCK_REFUSED);
 REQUIRE(memcmp(&gap,&sentinel,sizeof gap)==0);
 REQUIRE(memcmp(&original,stock,sizeof original)!=0);
 free(stock);return 0;
}
int main(int argc,char **argv){
 REQUIRE(argc==2||argc==5);
 const char *csv=argv[1];
 REQUIRE(gk1_verified_original_csv(csv));
 Gk1TemplateDataset dataset;
 REQUIRE(gk1_template_load(csv,&dataset)==GK1_LOAD_OK);
 Gk1LoadedScenario baseline;
 REQUIRE(gk1_open_loaded_scenario(&dataset,GK1_MONGOL_CONQUEST,&baseline)==GK1_LOAD_OK);
 Gk89SpringReconciliation out;memset(&out,0xCC,sizeof out);
 Gk89SpringReconciliation sentinel=out;
 Gk79Checkpoint *fake=calloc(1,sizeof *fake);REQUIRE(fake);
 REQUIRE(gk90_compare_spring_verified(csv,&baseline,fake,0,&out)==GK90_STOCK_REFUSED);
 REQUIRE(memcmp(&out,&sentinel,sizeof out)==0);
 baseline.countries[13].goods[9]++;
 REQUIRE(gk90_compare_spring_verified(csv,&baseline,fake,0,&out)==GK90_TEMPLATE_DRIFT);
 REQUIRE(memcmp(&out,&sentinel,sizeof out)==0);
 REQUIRE(gk1_open_loaded_scenario(&dataset,GK1_MONGOL_CONQUEST,&baseline)==GK1_LOAD_OK);
 REQUIRE(gk90_compare_spring_verified(NULL,&baseline,fake,0,&out)==GK90_ARGUMENT);
 REQUIRE(memcmp(&out,&sentinel,sizeof out)==0);
 if(argc==5){
  REQUIRE(check_one(csv,&baseline,argv[2],GK77_FRESH_MONGOL_WORKRAM)==0);
  REQUIRE(check_one(csv,&baseline,argv[3],GK77_ARCHIVED_MONGOL_SAVERAM)==0);
  REQUIRE(gk1_open_loaded_scenario(&dataset,GK1_WORLD_CONQUEST,&baseline)==GK1_LOAD_OK);
  REQUIRE(check_one(csv,&baseline,argv[4],GK77_ARCHIVED_WORLD_SAVERAM)==0);
 }
 free(fake);
 puts("GK90 sealed public pre-setup provenance and independently authenticated STOCK Spring, atomic negative controls PASS");
 return 0;
}
