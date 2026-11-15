#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"GK300 FAIL at %u: %s\n",(unsigned)__LINE__,#x);return 1;}checks++;}while(0)
static unsigned short be16(const uint8_t *p){return (unsigned short)(((unsigned)p[0]<<8)|(unsigned)p[1]);}
int main(int argc,char **argv){
 unsigned checks=0;
 CHECK(argc==2||argc==3);
 const uint8_t seed[6]={0x04,0xd2,0x04,0xd2,0x5a,0x7d};
 const uint8_t gates[8]={0,0,1,2,0,255,0,0};
 Gk300NmiResult result={0},sentinel;
 memset(&result,0xa5,sizeof result);sentinel=result;
 CHECK(gk300_model_nmi_gate_history(NULL,gates,8,&result)==GK300_ARGUMENT);
 CHECK(gk300_model_nmi_gate_history(seed,NULL,8,&result)==GK300_ARGUMENT);
 CHECK(gk300_model_nmi_gate_history(seed,gates,0,&result)==GK300_ARGUMENT);
 CHECK(gk300_model_nmi_gate_history(seed,gates,GK300_MAX_NMIS+1u,&result)==GK300_ARGUMENT);
 CHECK(gk300_model_nmi_gate_history(seed,gates,8,NULL)==GK300_ARGUMENT);
 CHECK(memcmp(&result,&sentinel,sizeof result)==0);
 CHECK(gk300_model_nmi_gate_history(seed,gates,8,&result)==GK300_OK);
 CHECK(result.enabled_nmis==5&&result.gated_nmis==3&&result.nmi_observations==8);
 CHECK(be16(result.after)==0x04dcu&&be16(result.after+2)==0x04dcu);
 CHECK(result.after[4]==0x5a&&result.after[5]==0x7d);
 CHECK(!result.original_rom_sha256_verified&&result.conditional_history);
 CHECK(!result.first_c289_seed_proven&&!result.gameplay_spring_unlocked);
 CHECK(memcmp(seed,(uint8_t[]){0x04,0xd2,0x04,0xd2,0x5a,0x7d},6)==0);
 const uint8_t wrap_seed[6]={0xff,0xfe,0xff,0xff,0,0xff};
 const uint8_t gate0[1]={0}, gate1[1]={1};
 CHECK(gk300_model_nmi_gate_history(wrap_seed,gate0,1,&result)==GK300_OK);
 CHECK(be16(result.after)==0x0000u&&be16(result.after+2)==0x0001u);
 CHECK(result.after[4]==0&&result.after[5]==0xff);
 CHECK(gk300_model_nmi_gate_history(wrap_seed,gate1,1,&result)==GK300_OK);
 CHECK(!memcmp(result.after,wrap_seed,6));
 uint8_t *max_gates=malloc(GK300_MAX_NMIS);CHECK(max_gates!=NULL);
 memset(max_gates,0,GK300_MAX_NMIS);
 CHECK(gk300_model_nmi_gate_history(seed,max_gates,GK300_MAX_NMIS,&result)==GK300_OK);
 CHECK(result.enabled_nmis==GK300_MAX_NMIS&&result.gated_nmis==0);
 CHECK(be16(result.after)==(uint16_t)(0x04d2u+2u*GK300_MAX_NMIS));
 CHECK(be16(result.after+2)==(uint16_t)(0x04d2u+2u*GK300_MAX_NMIS));
 memset(max_gates,1,GK300_MAX_NMIS);
 CHECK(gk300_model_nmi_gate_history(seed,max_gates,GK300_MAX_NMIS,&result)==GK300_OK);
 CHECK(memcmp(result.after,seed,6)==0&&result.gated_nmis==GK300_MAX_NMIS);
 for(unsigned i=0;i<GK300_MAX_NMIS;i++)max_gates[i]=(uint8_t)(i&1u);
 CHECK(gk300_model_nmi_gate_history(seed,max_gates,GK300_MAX_NMIS,&result)==GK300_OK);
 CHECK(result.enabled_nmis==4096&&result.gated_nmis==4096);
 CHECK(be16(result.after)==(uint16_t)(0x04d2u+8192u));
 free(max_gates);
 memset(&result,0xa5,sizeof result);sentinel=result;
 CHECK(gk300_authentic_nmi_gate_history("/private/does-not-exist.nes",seed,gates,8,&result)==GK300_ROM_IO);
 CHECK(gk300_authentic_nmi_gate_history(NULL,seed,gates,8,&result)==GK300_ARGUMENT);
 CHECK(gk300_authentic_nmi_gate_history("abc",seed,gates,0,&result)==GK300_ARGUMENT);
 CHECK(memcmp(&result,&sentinel,sizeof result)==0);
 Gk1App *app=calloc(1,sizeof *app),*unchanged=malloc(sizeof *unchanged);
 Gk1TemplateDataset *templates=malloc(sizeof *templates);
 CHECK(app&&unchanged&&templates);
 gk1_app_init(app);
 CHECK(gk1_template_load(argv[1],templates)==GK1_LOAD_OK);
 CHECK(gk1_app_install_templates(app,templates));
 app->active_world=0;app->rules_profile=0;
 app->setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
 memcpy(unchanged,app,sizeof *app);
 CHECK(gk1_app_original_nmi_seed_clock(app,"/private/missing",seed,gates,8,&result)==GK300_ROM_IO);
 CHECK(!memcmp(unchanged,app,sizeof *app));
 app->rules_profile=1;
 CHECK(gk1_app_original_nmi_seed_clock(app,"/private/missing",seed,gates,8,&result)==GK300_STAGE);
 app->rules_profile=0;app->worlds[0].countries[0].gold++;
 CHECK(gk1_app_original_nmi_seed_clock(app,"/private/missing",seed,gates,8,&result)==GK300_STAGE);
 app->worlds[0].countries[0].gold--;
 CHECK(!memcmp(unchanged,app,sizeof *app));
 if(argc==3){
  CHECK(gk300_authentic_nmi_gate_history(argv[2],seed,gates,8,&result)==GK300_OK);
  CHECK(result.original_rom_sha256_verified==1u&&result.enabled_nmis==5u);
  CHECK(result.conditional_history&&!result.first_c289_seed_proven&&!result.gameplay_spring_unlocked);
  for(unsigned w=0;w<2;w++){
   app->active_world=(uint8_t)w;
   app->setup_stage[w]=GK53_SETUP_AWAIT_SOURCE;
   for(unsigned d=1;d<=5;d++){
    app->setup_difficulty[w]=(uint8_t)d;
    memcpy(unchanged,app,sizeof *app);
    CHECK(gk1_app_original_nmi_seed_clock(app,argv[2],seed,gates,8,&result)==GK300_OK);
    CHECK(result.original_rom_sha256_verified==1u&&result.enabled_nmis==5u);
    CHECK(!result.first_c289_seed_proven&&!result.gameplay_spring_unlocked);
    CHECK(!memcmp(unchanged,app,sizeof *app));
   }
  }
  memset(&result,0xa5,sizeof result);sentinel=result;
  CHECK(gk300_authentic_nmi_gate_history(argv[1],seed,gates,8,&result)==GK300_ROM_IDENTITY);
  CHECK(!memcmp(&result,&sentinel,sizeof result));
  /* Verify corruption fails BEFORE output commit, even where all digits of
   * the expected ROM length are otherwise valid. No licensed file exported. */
  const char *tmp="gk300_PRIVATE_corrupt_rom.tmp";
  FILE *src=fopen(argv[2],"rb"),*dst=fopen(tmp,"wb");CHECK(src&&dst);
  unsigned char block[4096];size_t n,idx=0;int good=1;
  while((n=fread(block,1,sizeof block,src))>0){
   if(idx==0)block[16u+5u]^=0x80u;
   if(fwrite(block,1,n,dst)!=n){good=0;break;}
   idx+=n;
  }
  if(ferror(src)||fclose(src)||fclose(dst))good=0;
  CHECK(good&&idx==262160u);
  CHECK(gk300_authentic_nmi_gate_history(tmp,seed,gates,8,&result)==GK300_ROM_IDENTITY);
  CHECK(!memcmp(&result,&sentinel,sizeof result));
  CHECK(remove(tmp)==0);
 }
 free(app);free(unchanged);free(templates);
 printf("GK300 PASS %u assertions: exact 6502 zero-gated NMI +2 across high/low source-addressed words; two scenarios, unknown power-on tail, no false Spring unlock\n",checks);
 return 0;
}
