#include "gk209_archived_rate_ambiguity.h"
#include <stdio.h>
#include <string.h>
static int checks=0;
#define CHECK(x) do{if(!(x)){fprintf(stderr,"GK209 FAIL:%d %s\n",__LINE__,#x);return 2;}checks++;}while(0)
int main(int argc,char **argv){
 Gk209RateAmbiguity out,old;
 memset(&out,0xa5,sizeof out);old=out;
 CHECK(gk209_archived_rate_ambiguity(0,"mss",GK77_ARCHIVED_MONGOL_SAVERAM,64,&out)==GK209_ARGUMENT);
 CHECK(gk209_archived_rate_ambiguity("rom",0,GK77_ARCHIVED_MONGOL_SAVERAM,64,&out)==GK209_ARGUMENT);
 CHECK(gk209_archived_rate_ambiguity("rom","mss",GK77_FRESH_MONGOL_WORKRAM,64,&out)==GK209_ARGUMENT);
 CHECK(gk209_archived_rate_ambiguity("rom","mss",GK77_ARCHIVED_MONGOL_SAVERAM,0,&out)==GK209_ARGUMENT);
 CHECK(gk209_archived_rate_ambiguity("rom","mss",GK77_ARCHIVED_WORLD_SAVERAM,65537,&out)==GK209_ARGUMENT);
 CHECK(!memcmp(&out,&old,sizeof out));
 CHECK(gk209_archived_rate_ambiguity("badrom","no_file",GK77_ARCHIVED_MONGOL_SAVERAM,64,&out)==GK209_ARCHIVE_REFUSED);
 CHECK(!memcmp(&out,&old,sizeof out));
 if(argc==4){
  const char *rom=argv[1],*mongol=argv[2],*world=argv[3];
  Gk209RateAmbiguity a,b;
  Gk209Status ra=gk209_archived_rate_ambiguity(rom,mongol,GK77_ARCHIVED_MONGOL_SAVERAM,4096,&a);
  Gk209Status rb=gk209_archived_rate_ambiguity(rom,world,GK77_ARCHIVED_WORLD_SAVERAM,4096,&b);
  CHECK(ra==GK209_OK||ra==GK209_NOT_A_DRAW_BOUNDARY);
  CHECK(rb==GK209_OK||rb==GK209_NOT_A_DRAW_BOUNDARY);
  if(ra==GK209_OK){
   const uint32_t mongol_expected[5]={45,31,28,42,29};
   for(unsigned d=0;d<5u;d++)CHECK(a.by_difficulty[d].matches==mongol_expected[d]);
   CHECK(a.candidate_difficulties==5u && a.uniquely_located_difficulties==0u);
   CHECK(a.observed_spring_rate==101u && a.window==4096u);
   CHECK(a.chronology_proved==0u&&a.first_spring_generated==0u);
   for(unsigned d=0;d<5;d++)CHECK(a.by_difficulty[d].inspected==4096u);
   Gk209RateAmbiguity tmp=out;
   CHECK(gk209_archived_rate_ambiguity(rom,world,GK77_ARCHIVED_MONGOL_SAVERAM,64,&out)==GK209_ARCHIVE_REFUSED);
   CHECK(!memcmp(&out,&tmp,sizeof out));
  }
  if(rb==GK209_OK){
   const uint32_t world_expected[5]={46,39,40,24,38};
   for(unsigned d=0;d<5u;d++)CHECK(b.by_difficulty[d].matches==world_expected[d]);
   CHECK(b.candidate_difficulties==5u && b.uniquely_located_difficulties==0u);
   CHECK(b.observed_spring_rate==114u && b.window==4096u);
   CHECK(b.chronology_proved==0u&&b.first_spring_generated==0u);
   for(unsigned d=0;d<5;d++)CHECK(b.by_difficulty[d].inspected==4096u);
  }
  printf("AUTHENTIC MONGOL archive diagnostic status=%d; WORLD=%d; no startup claim\n",ra,rb);
 }
 printf("GK209 PASS %d checks; no original session or first Spring generated\n",checks);
 return 0;
}
