#include "gk329_archived_seed_replay.h"
#include <stdlib.h>
#define CHECK(x) do { if(!(x)) {fprintf(stderr,"GK329 failed at %d: %s\n",__LINE__,#x);exit(1);} } while(0)
#include <string.h>
#include <stdio.h>
int main(void){
 uint8_t pretend_rom[16]={0},pretend_fields[40712]={0};
 Gk329Receipt out,prior;
 memset(&out,0xa7,sizeof out);prior=out;
 CHECK(gk329_replay_archived_next_draw(0,pretend_rom,sizeof pretend_rom,pretend_fields,sizeof pretend_fields,&out)==GK329_NOT_ARCHIVED_SOURCE);
 CHECK(memcmp(&out,&prior,sizeof out)==0);
 CHECK(gk329_replay_archived_next_draw(1,pretend_rom,sizeof pretend_rom,pretend_fields,sizeof pretend_fields,&out)==GK329_NOT_ARCHIVED_SOURCE);
 CHECK(memcmp(&out,&prior,sizeof out)==0);
 CHECK(gk329_replay_archived_next_draw(0,pretend_rom,sizeof pretend_rom,pretend_fields,sizeof pretend_fields-1,&out)==GK329_NOT_ARCHIVED_SOURCE);
 CHECK(memcmp(&out,&prior,sizeof out)==0);
 CHECK(gk329_replay_archived_next_draw(0,NULL,sizeof pretend_rom,pretend_fields,sizeof pretend_fields,&out)==GK329_ARGUMENT);
 CHECK(memcmp(&out,&prior,sizeof out)==0);
 CHECK(gk329_replay_archived_next_draw(0,pretend_rom,sizeof pretend_rom,pretend_fields,sizeof pretend_fields,NULL)==GK329_ARGUMENT);
 CHECK(gk329_replay_archived_next_draw(2,pretend_rom,sizeof pretend_rom,pretend_fields,sizeof pretend_fields,&out)==GK329_ARGUMENT);
 CHECK(memcmp(&out,&prior,sizeof out)==0);
 printf("GK329 public invalid-origin and atomic-output controls PASS\n");return 0;
}
