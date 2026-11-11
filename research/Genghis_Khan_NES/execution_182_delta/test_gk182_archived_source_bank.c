#include "gk182_archived_source_bank.h"
#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks=0;
#define CHECK(x) do{checks++;if(!(x)){fprintf(stderr,"GK182 FAIL line %d: %s\n",__LINE__,#x);return 2;}}while(0)
int main(int argc,char **argv){
 CHECK(argc==1||argc==4);
 Gk182SavedBankProof *s=malloc(sizeof *s),*baseline=malloc(sizeof *s);
 Gk1App *app=malloc(sizeof *app);CHECK(app);gk1_app_init(app);
 CHECK(s&&baseline);memset(s,0x6b,sizeof *s);memcpy(baseline,s,sizeof *s);
 CHECK(gk182_original_saved_bank(NULL,"/none",GK77_ARCHIVED_MONGOL_SAVERAM,s)==GK182_ARGUMENT);
 CHECK(gk182_original_saved_bank("/none",NULL,GK77_ARCHIVED_MONGOL_SAVERAM,s)==GK182_ARGUMENT);
 CHECK(gk182_original_saved_bank("/none","/none",GK77_FRESH_MONGOL_WORKRAM,s)==GK182_ARGUMENT);
 CHECK(gk182_original_saved_bank("/none","/none",GK77_ARCHIVED_WORLD_SAVERAM,NULL)==GK182_ARGUMENT);
 CHECK(memcmp(s,baseline,sizeof *s)==0);
 CHECK(gk182_original_saved_bank("/none","/none",GK77_ARCHIVED_MONGOL_SAVERAM,s)==GK182_SOURCE);
 CHECK(memcmp(s,baseline,sizeof *s)==0);
 /* Live app requires complete genuine setup, and a matching 14/27 scenario.
  * A wrong world/profile may not borrow another campaign's source evidence. */
 CHECK(gk1_app_archived_source_bank(app,"/none","/none",
   GK77_ARCHIVED_MONGOL_SAVERAM,s)==GK182_ARGUMENT);
 CHECK(memcmp(s,baseline,sizeof *s)==0);
 if(argc==4){
  Gk77Profile profiles[2]={GK77_ARCHIVED_MONGOL_SAVERAM,GK77_ARCHIVED_WORLD_SAVERAM};
  uint64_t seeds[2]={0,0};
  for(unsigned i=0;i<2;i++){
   CHECK(gk182_original_saved_bank(argv[1],argv[2+i],profiles[i],s)==GK182_OK);
   CHECK(s->profile==profiles[i]&&s->original_mss_sha_verified&&s->stock_rom_sha_verified);
   CHECK(s->mapped_at_saved_instant.lower_prg_bank==0u);
   CHECK(s->mapped_at_saved_instant.upper_prg_bank==15u);
   CHECK(s->mapped_at_saved_instant.mapping_observed_at_saved_instant_only);
   CHECK(s->mapped_at_saved_instant.observed_country_ram_bank==GK139_BANK_SAVE);
   CHECK(s->conditional_source_sites.assumed_low_bank==0u);
   CHECK(s->conditional_source_sites.high_seed_candidates==4u);
   CHECK(s->conditional_source_sites.low_seed_candidates==0u);
   CHECK(s->proof_is_saved_instant_only&&!s->first_spring_generated);
   CHECK(s->saved_seed48<UINT64_C(0x1000000000000));
   seeds[i]=s->saved_seed48;
   Gk182SavedBankProof saved=*s;
   CHECK(gk182_original_saved_bank(argv[1],argv[3-i],profiles[i],s)==GK182_ARCHIVE);
   CHECK(memcmp(s,&saved,sizeof *s)==0);
   CHECK(gk182_original_saved_bank("/none",argv[2+i],profiles[i],s)==GK182_SOURCE);
   CHECK(memcmp(s,&saved,sizeof *s)==0);
  }
  CHECK(seeds[0]!=seeds[1]);
 }
 free(s);free(baseline);free(app);
 printf("GK182 %u checks PASS: independently archived Mesen saved-instant mapper/CPU source proof; NO original first Spring generated\n",checks);
 return 0;
}
