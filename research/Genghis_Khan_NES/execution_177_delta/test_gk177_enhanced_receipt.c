#define _POSIX_C_SOURCE 200809L
#include "gk177_enhanced_receipt.h"
#include "gk117_stock_pristine_gate.h"
#include "gk1_source_identity.h"
#include <assert.h>
#define CONT 118u
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
static unsigned checks=0;
#define CHECK(X) do{checks++;if(!(X)){fprintf(stderr,"GK177 check %u line %d: %s\n",checks,__LINE__,#X);abort();}}while(0)
static void negatives(void){
 Gk79Checkpoint o={0};Gk177Stages s={0},old={0};
 uint8_t bytes[GK177_BYTES]={0};
 CHECK(gk177_encode(&o,&s,bytes)==GK177_SOURCE);
 CHECK(gk177_encode(NULL,&s,bytes)==GK177_INPUT);
 CHECK(gk177_encode(&o,&s,NULL)==GK177_INPUT);
 CHECK(gk177_decode(&o,0u,bytes,sizeof bytes,&s)==GK177_SOURCE);
 CHECK(gk177_decode(&o,1u,bytes,sizeof bytes,&s)==GK177_CORRUPT);
 CHECK(gk177_decode(&o,1u,bytes,sizeof bytes-1u,&s)==GK177_CORRUPT);
 CHECK(gk177_decode(&o,1u,NULL,sizeof bytes,&s)==GK177_INPUT);
 CHECK(gk177_save_new("",&o,&s)==GK177_INPUT);
 CHECK(gk177_cold_load("",&o,1u,&s)==GK177_INPUT);
 CHECK(!memcmp(&old,&s,sizeof s));
}
static void private_positive(const char *csv,const char *records,Gk77Profile profile){
 Gk1TemplateDataset *d=malloc(sizeof *d);
 Gk1LoadedScenario *world=calloc(1,sizeof *world);
 Gk177Stages *s=calloc(1,sizeof *s),*decoded=calloc(1,sizeof *decoded);
 CHECK(d&&world&&s&&decoded);
 CHECK(gk1_template_load(csv,d)==GK1_LOAD_OK);
 CHECK(gk1_open_loaded_scenario(d,profile==GK77_ARCHIVED_WORLD_SAVERAM?
   GK1_WORLD_CONQUEST:GK1_MONGOL_CONQUEST,world)==GK1_LOAD_OK);
 Gk79Checkpoint *o=calloc(1,sizeof *o);CHECK(o);
 CHECK(gk117_open_attested_original(csv,world,records,
   profile,0,o)==GK117_OK);
 CHECK(gk116_open(o,1,&s->spring)==GK116_OK);
 CHECK(gk116_transfer(&s->spring,o,0,4,5)==GK116_OK);
 CHECK(gk116_pass_remaining(&s->spring,o)==GK116_OK);
 CHECK(gk158_open(&s->spring,o,&s->summer)==GK158_OK);
 CHECK(gk158_transfer(&s->summer,&s->spring,o,0,4,5)==GK158_OK);
 CHECK(gk158_pass(&s->summer,&s->spring,o)==GK158_OK);
 CHECK(gk159_open(&s->spring,&s->summer,o,&s->fall)==GK159_OK);
 CHECK(gk159_transfer(&s->fall,&s->spring,&s->summer,o,0,4,5)==GK159_OK);
 CHECK(gk159_pass(&s->fall,&s->spring,&s->summer,o)==GK159_OK);
 CHECK(gk160_open(&s->spring,&s->summer,&s->fall,o,&s->winter)==GK160_OK);
 CHECK(gk160_transfer(&s->winter,&s->spring,&s->summer,&s->fall,o,0,4,5)==GK160_OK);
 CHECK(gk160_pass(&s->winter,&s->spring,&s->summer,&s->fall,o)==GK160_OK);
 CHECK(gk161_open(&s->spring,&s->summer,&s->fall,&s->winter,o,&s->year2)==GK161_OK);
 CHECK(gk161_transfer(&s->year2,&s->spring,&s->summer,&s->fall,&s->winter,o,0,4,5)==GK161_OK);
 CHECK(gk161_pass(&s->year2,&s->spring,&s->summer,&s->fall,&s->winter,o)==GK161_OK);
 CHECK(gk176_open(&s->year2,&s->spring,&s->summer,&s->fall,
   &s->winter,o,&s->continuation)==GK176_OK);
 CHECK(gk176_transfer(&s->continuation,&s->year2,&s->spring,&s->summer,
  &s->fall,&s->winter,o,0,4,2)==GK176_OK);
 /* Exercise durable replay at every intermediate Spring/Summer/Fall/Winter,
  * then final 20th Enhanced season, including year rollover and pass. */
 uint8_t raw[GK177_BYTES];
 for(unsigned i=0;i<GK176_MAX_SEASONS;i++){
  if(i){
   CHECK(gk176_advance(&s->continuation,&s->year2,&s->spring,&s->summer,
     &s->fall,&s->winter,o)==GK176_OK);
   CHECK(gk176_transfer(&s->continuation,&s->year2,&s->spring,&s->summer,
     &s->fall,&s->winter,o,0,4,1)==GK176_OK);
  }
  CHECK(gk177_encode(o,s,raw)==GK177_OK);
  CHECK(gk177_decode(o,1u,raw,sizeof raw,decoded)==GK177_OK);
  CHECK(!memcmp(decoded,s,sizeof *s));
  CHECK(gk176_pass(&s->continuation,&s->year2,&s->spring,&s->summer,
    &s->fall,&s->winter,o)==GK176_OK);
 }
 CHECK(gk177_encode(o,s,raw)==GK177_OK);
 CHECK(gk177_decode(o,1,raw,sizeof raw,decoded)==GK177_OK);
 CHECK(!memcmp(decoded,s,sizeof *s));
 Gk177Stages *sentinel=malloc(sizeof *sentinel);CHECK(sentinel);
 *sentinel=*decoded;
 uint8_t forged[GK177_BYTES];memcpy(forged,raw,sizeof raw);
 forged[CONT+0]=0xffu; /* Overlong receipt; recompute digest to defeat hash-only guards */
 gk1_sha256_bytes(forged,416u,forged+416u);
 CHECK(gk177_decode(o,1u,forged,sizeof forged,decoded)==GK177_CORRUPT);
 CHECK(!memcmp(decoded,sentinel,sizeof *sentinel));
 memcpy(forged,raw,sizeof raw);forged[0]^=1u;
 CHECK(gk177_decode(o,1u,forged,sizeof forged,decoded)==GK177_CORRUPT);
 CHECK(!memcmp(decoded,sentinel,sizeof *sentinel));
 memcpy(forged,raw,sizeof raw);forged[398u]=1u;
 gk1_sha256_bytes(forged,416u,forged+416u);
 CHECK(gk177_decode(o,1u,forged,sizeof forged,decoded)==GK177_CORRUPT);
 CHECK(!memcmp(decoded,sentinel,sizeof *sentinel));
 CHECK(gk177_decode(o,0u,raw,sizeof raw,decoded)==GK177_SOURCE);
 CHECK(!memcmp(decoded,sentinel,sizeof *sentinel));
 char path[200];
#ifdef _WIN32
 snprintf(path,sizeof path,"gk177_replay_qa_%u.gk77",(unsigned)checks);
#else
 snprintf(path,sizeof path,"/tmp/gk177_replay_qa_%ld.gk77",(long)getpid());
#endif
 remove(path);
 CHECK(gk177_save_new(path,o,s)==GK177_OK);
 CHECK(gk177_save_new(path,o,s)==GK177_EXISTS);
 CHECK(gk177_cold_load(path,o,1u,decoded)==GK177_OK);
 CHECK(!memcmp(decoded,s,sizeof *s));
#ifndef _WIN32
 struct stat st;CHECK(stat(path,&st)==0&&!(st.st_mode&077));
 char hard[220],sym[220];snprintf(hard,sizeof hard,"%s.hard",path);
 snprintf(sym,sizeof sym,"%s.sym",path);
 (void)remove(hard);(void)remove(sym);
 CHECK(link(path,hard)==0);
 CHECK(gk177_cold_load(path,o,1u,decoded)==GK177_CORRUPT);
 CHECK(!memcmp(decoded,sentinel,sizeof *sentinel));
 CHECK(remove(hard)==0);
 CHECK(symlink(path,sym)==0);
 CHECK(gk177_cold_load(sym,o,1u,decoded)!=GK177_OK);
 CHECK(!memcmp(decoded,sentinel,sizeof *sentinel));
 CHECK(remove(sym)==0);
 CHECK(chmod(path,0644)==0);
 CHECK(gk177_cold_load(path,o,1u,decoded)==GK177_CORRUPT);
 CHECK(!memcmp(decoded,sentinel,sizeof *sentinel));
 CHECK(chmod(path,0600)==0);
#endif
 FILE *fp=fopen(path,"r+b");CHECK(fp!=NULL);CHECK(fputc('X',fp)!=EOF);CHECK(!fclose(fp));
 CHECK(gk177_cold_load(path,o,1u,decoded)==GK177_CORRUPT);
 CHECK(!memcmp(decoded,sentinel,sizeof *sentinel));
 CHECK(remove(path)==0);
 free(sentinel);free(decoded);free(s);free(o);free(world);free(d);
}
int main(int argc,char **argv){
 negatives();
 if(argc==3||argc==4)private_positive(argv[1],argv[2],
   (Gk77Profile)(argc==4?atoi(argv[3]):GK77_FRESH_MONGOL_WORKRAM));
 else CHECK(argc==2);
 printf("GK177 %u assertions PASS: source-attested Enhanced continuation only; Classic Spring NOT IMPLEMENTED\n",checks);
 return 0;
}
