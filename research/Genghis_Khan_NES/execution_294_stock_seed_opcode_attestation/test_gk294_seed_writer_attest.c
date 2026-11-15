/* EXEC0294: hostile source-claim attestation, no NES ROM shipped.
 * Optional argv[2] PRIVATE original stock ROM enables the authenticated
 * positive branch; without ROM run only stage/rollback/invalid-input QA.
 * Six accepted raw code-pattern locations are STATIC candidates, emphatically
 * NOT actual observed writes or proof of 6502 reachability.
 */
#include "gk1_app_state.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CK(x) do{if(!(x)){fprintf(stderr,"GK294 FAIL line %d: %s\n",__LINE__,#x);return 1;}checks++;}while(0)
static const Gk294Write candidates[6]={
 {1000u,0x9dfdu,9u,0x84u},
 {2000u,0x8919u,8u,0x85u},
 {3000u,0x8676u,5u,0x86u},
 {4000u,0x8896u,1u,0x87u},
 {5000u,0x882fu,3u,0x88u},
 {6000u,0x855bu,0u,0x89u}
};
int main(int argc,char **argv){
 unsigned checks=0;
 CK(argc==2||argc==3);
 Gk294SourceProof out;memset(&out,0xa5,sizeof out);
 Gk294SourceProof orig=out;
 CK(gk294_attest_seed_writers(NULL,candidates,6,&out)==GK294_ARGUMENT);
 CK(gk294_attest_seed_writers("",candidates,6,&out)==GK294_ARGUMENT);
 CK(gk294_attest_seed_writers("/missing/rom.nes",NULL,6,&out)==GK294_ARGUMENT);
 CK(gk294_attest_seed_writers("/missing/rom.nes",candidates,0,&out)==GK294_ARGUMENT);
 CK(gk294_attest_seed_writers("/missing/rom.nes",candidates,65,&out)==GK294_ARGUMENT);
 CK(gk294_attest_seed_writers("/missing/rom.nes",candidates,6,NULL)==GK294_ARGUMENT);
 CK(gk294_attest_seed_writers("/missing/rom.nes",candidates,6,&out)==GK294_ROM_IO);
 CK(memcmp(&out,&orig,sizeof out)==0);
 Gk294Write trial[6];memcpy(trial,candidates,sizeof trial);
 trial[0].source_pc=0x7fffu;
 CK(gk294_attest_seed_writers("/missing/rom.nes",trial,6,&out)==GK294_BANK_MISMATCH);
 memcpy(trial,candidates,sizeof trial);trial[0].mapped_prg_bank=15;
 CK(gk294_attest_seed_writers("/missing/rom.nes",trial,6,&out)==GK294_BANK_MISMATCH);
 memcpy(trial,candidates,sizeof trial);trial[0].destination_zp=0x83;
 CK(gk294_attest_seed_writers("/missing/rom.nes",trial,6,&out)==GK294_BANK_MISMATCH);
 memcpy(trial,candidates,sizeof trial);trial[0].cpu_cycle=2000;
 CK(gk294_attest_seed_writers("/missing/rom.nes",trial,6,&out)==GK294_CHRONOLOGY);
 CK(memcmp(&out,&orig,sizeof out)==0);
 Gk1App *app=calloc(1,sizeof *app),*before=malloc(sizeof *before);
 Gk1TemplateDataset *dataset=malloc(sizeof *dataset);
 CK(app&&before&&dataset);
 gk1_app_init(app);
 CK(gk1_template_load(argv[1],dataset)==GK1_LOAD_OK);
 CK(gk1_app_install_templates(app,dataset));
 app->active_world=0;app->rules_profile=0;
 app->setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
 app->setup_difficulty[0]=1;
 memcpy(before,app,sizeof *before);
 CK(gk1_app_attest_seed_writer_source(app,"/missing/rom.nes",candidates,6,&out)==GK294_ROM_IO);
 CK(!memcmp(before,app,sizeof *app));
 CK(!memcmp(&out,&orig,sizeof out));
 app->rules_profile=1;
 CK(gk1_app_attest_seed_writer_source(app,"/missing/rom.nes",candidates,6,&out)==GK294_STAGE);
 app->rules_profile=0;
 app->stock_checkpoint.valid=1;
 CK(gk1_app_attest_seed_writer_source(app,"/missing/rom.nes",candidates,6,&out)==GK294_STAGE);
 app->stock_checkpoint.valid=0;
 app->worlds[0].countries[0].gold++;
 CK(gk1_app_attest_seed_writer_source(app,"/missing/rom.nes",candidates,6,&out)==GK294_STAGE);
 app->worlds[0].countries[0].gold--;
 CK(!memcmp(before,app,sizeof *app));
 CK(!memcmp(&out,&orig,sizeof out));
 if(argc==3){
  const char *rom=argv[2];
  CK(gk1_verified_original_csv(argv[1]));
  for(unsigned world=0;world<=1;world++){
   app->active_world=(uint8_t)world;
   app->setup_stage[world]=GK53_SETUP_AWAIT_SOURCE;
   for(unsigned diff=1;diff<=5;diff++){
    app->setup_difficulty[world]=(uint8_t)diff;
    memcpy(before,app,sizeof *before);
    CK(gk1_app_attest_seed_writer_source(app,rom,candidates,6,&out)==GK294_OK);
    CK(!memcmp(before,app,sizeof *app));
    CK(out.count==6&&out.unique_target_mask==0x3fu&&
       out.source_candidates_only==1&&out.original_rom_sha256_verified==1);
    Gk294SourceProof replay={0};
    CK(gk1_app_attest_seed_writer_source(app,rom,candidates,6,&replay)==GK294_OK);
    CK(!memcmp(&out,&replay,sizeof out));
   }
  }
  orig=out;
  memcpy(trial,candidates,sizeof trial);trial[0].source_pc=0xffffu;
  trial[0].mapped_prg_bank=15u;
  CK(gk294_attest_seed_writers(rom,trial,6,&out)==GK294_SOURCE_MISMATCH);
  memcpy(trial,candidates,sizeof trial);trial[0].source_pc=0xbfffu;
  CK(gk294_attest_seed_writers(rom,trial,6,&out)==GK294_SOURCE_MISMATCH);
  memcpy(trial,candidates,sizeof trial);trial[3].source_pc++;
  CK(gk294_attest_seed_writers(rom,trial,6,&out)==GK294_SOURCE_MISMATCH);
  CK(!memcmp(&out,&orig,sizeof out));
  memcpy(trial,candidates,sizeof trial);trial[3].mapped_prg_bank=2;
  CK(gk294_attest_seed_writers(rom,trial,6,&out)==GK294_SOURCE_MISMATCH);
  memcpy(trial,candidates,sizeof trial);trial[3].destination_zp=0x88;
  CK(gk294_attest_seed_writers(rom,trial,6,&out)==GK294_SOURCE_MISMATCH);
  memcpy(trial,candidates,sizeof trial);trial[0].destination_zp=0x85;
  CK(gk294_attest_seed_writers(rom,trial,6,&out)==GK294_SOURCE_MISMATCH);
  CK(gk294_attest_seed_writers(rom,candidates,5,&out)==GK294_INCOMPLETE);
  CK(!memcmp(&out,&orig,sizeof out));
  CK(gk294_attest_seed_writers("/missing/rom.nes",candidates,6,&out)==GK294_ROM_IO);
  CK(!memcmp(&out,&orig,sizeof out));
  /* Corrupt authenticated source WITHOUT publishing it. Refuse exact-size
   * mutation, truncated and extended input, always preserving out. */
  const char *badpath="GK1_EXEC0294_transient_BAD_ROM_DO_NOT_PUBLISH.bin";
  unsigned char *bytes=malloc(262161u);CK(bytes);
  FILE *src=fopen(rom,"rb");CK(src);
  CK(fread(bytes,1,262161u,src)==262160u);
  CK(fclose(src)==0);
  for(unsigned which=0;which<3;which++){
   FILE *bad=fopen(badpath,"wb");CK(bad);
   size_t length=262160u;
   if(which==0)bytes[100u]^=1u;
   if(which==1)length--;
   if(which==2){length++;bytes[262160u]=0u;}
   CK(fwrite(bytes,1,length,bad)==length);
   CK(fclose(bad)==0);
   CK(gk294_attest_seed_writers(badpath,candidates,6,&out)==GK294_ROM_IDENTITY);
   CK(!memcmp(&out,&orig,sizeof out));
   CK(remove(badpath)==0);
   if(which==0)bytes[100u]^=1u;
  }
  free(bytes);
 }
 printf("GK294 %u assertions PASS — source candidates %s; NO actual Mesen seed writes or first Spring\n",
   checks,argc==3?"ROM-AUTHENTICATED":"NOT RUN WITHOUT PRIVATE ROM");
 free(app);free(before);free(dataset);return 0;
}
