#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
/* EXEC0246: private source-attested Classic Tax save/replay parity, not NES RNG. */
#include "gk188_classic_receipt.h"
#include "gk117_stock_pristine_gate.h"
#include "gk1_source_identity.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#endif
static unsigned assertions;
#define TEST(x) do { assert(x); assertions++; } while (0)
static void public_negative(void) {
 Gk79Checkpoint *cp=calloc(1,sizeof *cp);
 Gk187ClassicAssign *out=malloc(sizeof *out),*prior=malloc(sizeof *prior);
 TEST(cp&&out&&prior);
 memset(out,0xa6,sizeof *out); *prior=*out;
 TEST(gk188_load("nonexistent-gk246.gc18",cp,out)==GK188_SOURCE);
 TEST(!memcmp(out,prior,sizeof *out));
 TEST(gk188_load(NULL,cp,out)==GK188_INVALID);
 TEST(!memcmp(out,prior,sizeof *out));
 free(cp);free(out);free(prior);
}
static void private_profile(const char *csv,const char *records,Gk77Profile profile){
 Gk1TemplateDataset *ds=malloc(sizeof *ds);
 Gk1LoadedScenario *pre=malloc(sizeof *pre);
 Gk79Checkpoint *cp=calloc(1,sizeof *cp);
 Gk187ClassicAssign *live=malloc(sizeof *live),*loaded=malloc(sizeof *loaded),*prior=malloc(sizeof *prior);
 TEST(ds&&pre&&cp&&live&&loaded&&prior);
 TEST(gk1_template_load(csv,ds)==GK1_LOAD_OK);
 TEST(gk1_open_loaded_scenario(ds,profile==GK77_ARCHIVED_WORLD_SAVERAM?
      GK1_WORLD_CONQUEST:GK1_MONGOL_CONQUEST,pre)==GK1_LOAD_OK);
 TEST(gk117_open_attested_original(csv,pre,records,profile,0u,cp)==GK117_OK);
 /* Cover Tax-only, Tax->Assign, and Assign->Tax->Pass, all source-attested. */
 for(unsigned sequence=0;sequence<3;sequence++){
  TEST(gk187_open(cp,live)==GK187_OK);
  uint8_t tax=(uint8_t)(live->mutable_world.countries[0].tax==72u?73u:72u);
  if(sequence==2u)TEST(gk187_transfer(live,cp,0,4,1u)==GK187_OK);
  TEST(gk187_set_tax(live,cp,tax)==GK187_OK);
  if(sequence==1u)TEST(gk187_transfer(live,cp,0,4,1u)==GK187_OK);
  if(sequence==2u)TEST(gk187_pass(live,cp)==GK187_OK);
  TEST(gk187_intact(live,cp));
  char path[]="/tmp/gk246-classic-XXXXXX";
#ifndef _WIN32
  int fd=mkstemp(path);TEST(fd>=0);TEST(close(fd)==0);TEST(unlink(path)==0);
#else
  /* Private original records are not distributed with CI/Windows releases. */
  TEST(0 && "Private provenance test requires Linux secure temporary fixture");
#endif
  TEST(gk188_save_new(path,cp,live)==GK188_OK);
  memset(loaded,0xa6,sizeof *loaded);*prior=*loaded;
  TEST(gk188_load(path,cp,loaded)==GK188_OK);
  TEST(!memcmp(loaded,live,sizeof *live));
  TEST(gk187_intact(loaded,cp));
  TEST(loaded->mutable_world.countries[0].tax==tax);
  /* Valid receipt cannot be replayed against a different source digest. */
  cp->raw_sha256[0]^=1u;
  *loaded=*prior;
  TEST(gk188_load(path,cp,loaded)==GK188_SOURCE);
  TEST(!memcmp(loaded,prior,sizeof *prior));
  cp->raw_sha256[0]^=1u;
  /* Attack: forge a tax of 101 while recomputing receipt digest. The replay
   * must refuse it, rather than silently wrap uint16 to uint8. */
  unsigned tax_action=sequence==2u?1u:0u;
  FILE *f=fopen(path,"r+b");TEST(f!=NULL);
  uint8_t b[96];TEST(fread(b,1,sizeof b,f)==sizeof b);
  TEST(b[48+4*tax_action]==5&&b[49+4*tax_action]==5);
  b[50+4*tax_action]=101u;b[51+4*tax_action]=0u;
  uint8_t hash[32];gk1_sha256_bytes(b,80,hash);memcpy(b+80,hash,16);
  TEST(fseek(f,0,SEEK_SET)==0);
  TEST(fwrite(b,1,sizeof b,f)==sizeof b);
  TEST(fclose(f)==0);
  TEST(gk188_load(path,cp,loaded)==GK188_CORRUPT);
  TEST(!memcmp(loaded,prior,sizeof *prior));
  TEST(gk187_intact(live,cp));
  TEST(remove(path)==0);
 }
 free(ds);free(pre);free(cp);free(live);free(loaded);free(prior);
}
int main(int argc,char **argv){
 TEST(argc==1||argc==5);
 public_negative();
 if(argc==5){
  private_profile(argv[1],argv[2],GK77_FRESH_MONGOL_WORKRAM);
  private_profile(argv[1],argv[3],GK77_ARCHIVED_MONGOL_SAVERAM);
  private_profile(argv[1],argv[4],GK77_ARCHIVED_WORLD_SAVERAM);
 }
 printf("GK246: %u source/provenance/Tax+Assign atomic replay assertions PASS (%s); not original NES turn parity\n",assertions,argc==5?"private three original profiles":"public negative");
 return 0;
}
