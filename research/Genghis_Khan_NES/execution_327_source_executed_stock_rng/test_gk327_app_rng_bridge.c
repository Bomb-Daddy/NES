#include "gk327_app_rng_bridge.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
static unsigned checks;
#define CK(e) do {++checks;if(!(e)){fprintf(stderr,"FAIL %u line %d: %s\n",checks,__LINE__,#e);return 1;}}while(0)
int main(void){
 Gk1App *a=calloc(1,sizeof(*a));
 uint8_t *image=calloc(1,GK327_ROM_BYTES);
 CK(a&&image);CK(sizeof(*a)>sizeof(Gk327RngReceipt));
 image[0]='N';image[1]='E';image[2]='S';image[3]=26;
 image[4]=16;image[6]=0x12;
 uint8_t seed[6]={1,2,3,4,5,6};
 Gk327RngReceipt receipt;memset(&receipt,0xa5,sizeof receipt);
 Gk327RngReceipt unchanged=receipt;
 CK(gk327_app_source_rng_candidate(NULL,image,GK327_ROM_BYTES,seed,&receipt)==GK327_BAD_ARG);
 CK(gk327_app_source_rng_candidate(a,NULL,GK327_ROM_BYTES,seed,&receipt)==GK327_BAD_ARG);
 CK(gk327_app_source_rng_candidate(a,image,GK327_ROM_BYTES,NULL,&receipt)==GK327_BAD_ARG);
 CK(gk327_app_source_rng_candidate(a,image,GK327_ROM_BYTES,seed,NULL)==GK327_BAD_ARG);
 CK(gk327_app_source_rng_candidate(a,image,GK327_ROM_BYTES,seed,&receipt)==GK327_BAD_SOURCE);
 a->rules_profile=1;
 CK(gk327_app_source_rng_candidate(a,image,GK327_ROM_BYTES,seed,&receipt)==GK327_BAD_SOURCE);
 a->rules_profile=0;a->active_world=2;
 CK(gk327_app_source_rng_candidate(a,image,GK327_ROM_BYTES,seed,&receipt)==GK327_BAD_SOURCE);
 a->active_world=0;a->world_ready[0]=0;
 CK(gk327_app_source_rng_candidate(a,image,GK327_ROM_BYTES,seed,&receipt)==GK327_BAD_SOURCE);
 a->world_ready[0]=1;a->worlds[0].phase=GK1_PRE_SETUP_TEMPLATE;
 a->worlds[0].country_count=14;a->setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
 a->setup_ability_method[0]=1;a->setup_candidate[0]=0;
 CK(gk327_app_source_rng_candidate(a,image,GK327_ROM_BYTES,seed,&receipt)==GK327_BAD_SOURCE);
 CK(memcmp(&receipt,&unchanged,sizeof receipt)==0);
 /* A non-original ROM is refused after all genuine app gates have passed. */
 a->worlds[0].country_count=13;
 CK(gk327_app_source_rng_candidate(a,image,GK327_ROM_BYTES,seed,&receipt)==GK327_BAD_SOURCE);
 a->worlds[0].country_count=14;a->setup_candidate[0]=14;
 CK(gk327_app_source_rng_candidate(a,image,GK327_ROM_BYTES,seed,&receipt)==GK327_BAD_SOURCE);
 a->setup_candidate[0]=0;a->setup_stage[0]=GK53_SETUP_ABILITIES;
 CK(gk327_app_source_rng_candidate(a,image,GK327_ROM_BYTES,seed,&receipt)==GK327_BAD_SOURCE);
 a->setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;a->setup_ability_method[0]=0;
 CK(gk327_app_source_rng_candidate(a,image,GK327_ROM_BYTES,seed,&receipt)==GK327_BAD_SOURCE);
 a->setup_ability_method[0]=1;a->worlds[0].phase=(Gk1ScenarioPhase)(GK1_PRE_SETUP_TEMPLATE+1);
 CK(gk327_app_source_rng_candidate(a,image,GK327_ROM_BYTES,seed,&receipt)==GK327_BAD_SOURCE);
 a->worlds[0].phase=GK1_PRE_SETUP_TEMPLATE;
 a->active_world=1;a->world_ready[1]=1;a->worlds[1].phase=GK1_PRE_SETUP_TEMPLATE;
 a->worlds[1].country_count=27;a->setup_stage[1]=GK53_SETUP_AWAIT_SOURCE;
 a->setup_ability_method[1]=1;a->setup_candidate[1]=0;
 CK(gk327_app_source_rng_candidate(a,image,GK327_ROM_BYTES,seed,&receipt)==GK327_BAD_SOURCE);
 CK(memcmp(&receipt,&unchanged,sizeof receipt)==0);
 a->worlds[1].country_count=14;
 CK(gk327_app_source_rng_candidate(a,image,GK327_ROM_BYTES,seed,&receipt)==GK327_BAD_SOURCE);
 free(a);free(image);
 printf("GK327 actual Gk1App bridge: %u assertions PASS; no controller orders granted\n",checks);
 return 0;
}
