#include "gk329_archived_seed_replay.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
static unsigned pass=0;
#define CHECK(x) do{if(!(x)){fprintf(stderr,"assert %d: %s\n",__LINE__,#x);exit(1);}pass++;}while(0)
static uint8_t *readfile(const char*path,size_t *n){FILE*f=fopen(path,"rb");if(!f)return NULL;fseek(f,0,SEEK_END);long q=ftell(f);rewind(f);if(q<=0){fclose(f);return NULL;}uint8_t*b=malloc((size_t)q);if(!b){fclose(f);return NULL;}if(fread(b,1,(size_t)q,f)!=(size_t)q){free(b);fclose(f);return NULL;}fclose(f);*n=(size_t)q;return b;}
int main(int argc,char**argv){if(argc!=5)return 2;size_t nr,nm,nw,nf;uint8_t*r=readfile(argv[1],&nr),*m=readfile(argv[2],&nm),*w=readfile(argv[3],&nw);uint8_t *f=readfile(argv[4],&nf);CHECK(r&&m&&w&&f);
for(unsigned i=0;i<2;i++){
uint8_t*d=i?w:m;size_t n=i?nw:nm;Gk329Receipt out;memset(&out,0xa5,sizeof out);
Gk329Status s=gk329_replay_archived_next_draw((uint8_t)i,r,nr,d,n,&out);CHECK(s==GK329_OK);CHECK(out.evidence_is_archived_restored==1);CHECK(out.first_spring_generated==0&&out.player_orders_granted==0&&out.causal_spring_rng_known==0);CHECK(out.archived.country_bank==GK139_BANK_SAVE);CHECK(out.source_instruction_result.authentic_rom==1);
Gk329Receipt preserved;memset(&out,0x58,sizeof out);preserved=out;
CHECK(gk329_replay_archived_next_draw((uint8_t)i,r,nr,d,n-1,&out)!=GK329_OK);
CHECK(memcmp(&out,&preserved,sizeof out)==0);
uint8_t v=d[11];d[11]^=1u;
CHECK(gk329_replay_archived_next_draw((uint8_t)i,r,nr,d,n,&out)==GK329_NOT_ARCHIVED_SOURCE);
CHECK(memcmp(&out,&preserved,sizeof out)==0);d[11]=v;
CHECK(gk329_replay_archived_next_draw((uint8_t)i,r,nr-1,d,n,&out)==GK329_ROM_OR_CPU_REJECTED);
CHECK(memcmp(&out,&preserved,sizeof out)==0);
CHECK(gk329_replay_archived_next_draw((uint8_t)(1-i),r,nr,d,n,&out)==GK329_NOT_ARCHIVED_SOURCE);
CHECK(memcmp(&out,&preserved,sizeof out)==0);
}
Gk329Receipt out;memset(&out,0x51,sizeof out);Gk329Receipt before=out;
CHECK(gk329_replay_archived_next_draw(0,r,nr,f,nf,&out)==GK329_NOT_ARCHIVED_SOURCE);
CHECK(memcmp(&out,&before,sizeof out)==0);
CHECK(gk329_replay_archived_next_draw(1,r,nr,f,nf,&out)==GK329_NOT_ARCHIVED_SOURCE);
CHECK(memcmp(&out,&before,sizeof out)==0);
CHECK(gk329_replay_archived_next_draw(2,r,nr,m,nm,&out)==GK329_ARGUMENT);
free(r);free(m);free(w);free(f);printf("GK329 PRIVATE original ROM + archived Mesen SOURCE CPU replay: %u assertions PASS\n",pass);return 0;
}
