/* EXEC0042 adversarial native research-replay disk acceptance: NOT a campaign save. */
#include "gk1_replay_store.h"
#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
static unsigned checks=0;
#define CK(p) do{checks++; if(!(p)){fprintf(stderr,"FAIL %s:%d: %s\n",__FILE__,__LINE__,#p);exit(1);}}while(0)
static void corrupt_byte(const char *path,unsigned index,int extension){
 FILE *f=fopen(path,"r+b");CK(f!=NULL);CK(fseek(f,(long)index,SEEK_SET)==0);
 int old=fgetc(f);CK(old!=EOF);CK(fseek(f,-1,SEEK_CUR)==0);
 CK(fputc(old^0x80,f)!=EOF);CK(fclose(f)==0);
 if(extension){f=fopen(path,"ab");CK(f!=NULL);CK(fputc(3,f)!=EOF);CK(fclose(f)==0);}
}
static void open_case(Gk1BattleSession *s,unsigned i){
 static const uint8_t m[6]={10,4,2,10,10,10};
 static const uint8_t t[6]={3,3,3,3,3,0};
 static const Gk1Occupancy o[6]={GK1_OCC_EMPTY,GK1_OCC_EMPTY,GK1_OCC_EMPTY,
    GK1_OCC_FRIEND_SLOT1,GK1_OCC_ENEMY_SLOT10,GK1_OCC_EMPTY};
 CK(i<6);CK(gk1_session_open_fixture(m[i],t[i],o[i],s)==GK1_SESSION_OK);
}
int main(void){
 CK(gk1_store_save_new(NULL,NULL)==GK1_STORE_BAD);
 CK(gk1_store_load(NULL,NULL)==GK1_STORE_BAD);
 CK(gk1_store_load("does-not-exist-EXEC0042.gkr", &(Gk1BattleSession){0})==GK1_STORE_IO);
 for(unsigned k=0;k<6;k++) for(unsigned phase=0;phase<2;phase++){
  char path[96];snprintf(path,sizeof path,"EXEC0042-test-fixture-%u-%u.gkr",k,phase);
  (void)remove(path); /* test-owned build-directory fixture only, never a user save */
  Gk1BattleSession in;open_case(&in,k);
  Gk1MoveResult result;
  if(phase)CK(gk1_session_move(&in,8,7,&result)==GK1_SESSION_OK);
  Gk1BattleSession before=in;
  CK(gk1_store_save_new(path,&in)==GK1_STORE_OK);
  CK(gk1_store_save_new(path,&in)==GK1_STORE_EXISTS);
  Gk1BattleSession out;memset(&out,0xa5,sizeof out);
  CK(gk1_store_load(path,&out)==GK1_STORE_OK);
  uint8_t a[GK1_SESSION_SAVE_BYTES],b[GK1_SESSION_SAVE_BYTES];
  CK(gk1_session_encode(&in,a,sizeof a)==GK1_SESSION_OK);
  CK(gk1_session_encode(&out,b,sizeof b)==GK1_SESSION_OK);
  CK(memcmp(a,b,sizeof a)==0);
  Gk1App app;gk1_app_init(&app);
  CK(gk1_app_restore_session(&app,&out)==1);
  CK(app.screen==GK1_SCREEN_RESEARCH_BATTLE && app.case_index==k);
  CK(app.session.events==phase);
  CK(app.session.battle.mover.soldiers==in.battle.mover.soldiers);
  Gk1App old=app;
  Gk1BattleSession forged=out;forged.battle.mover.soldiers++;
  CK(gk1_app_restore_session(&app,&forged)==0);
  CK(memcmp(&app,&old,sizeof app)==0);
  /* Damage the on-disk body: CRC rejection and no output mutation. */
  corrupt_byte(path,10,0);
  Gk1BattleSession untouched=out;
  CK(gk1_store_load(path,&out)==GK1_STORE_CORRUPT);
  CK(memcmp(&out,&untouched,sizeof out)==0);
  CK(memcmp(&in,&before,sizeof in)==0);
  CK(remove(path)==0);
 }
 /* Legitimate pending-to-finished B then persistent cold reload. */
 Gk1BattleSession pending;open_case(&pending,0);Gk1MoveResult result;
 CK(gk1_session_move(&pending,8,7,&result)==GK1_SESSION_OK);
 CK(gk1_session_finish(&pending,&result)==GK1_SESSION_OK);
 CK(pending.events==2);
 const char *path="EXEC0042-test-finished.gkr";(void)remove(path);
 CK(gk1_store_save_new(path,&pending)==GK1_STORE_OK);
 Gk1BattleSession restored;CK(gk1_store_load(path,&restored)==GK1_STORE_OK);
 CK(restored.events==2 && restored.battle.completion_word==1);
 corrupt_byte(path,0,1); /* also reject appended bytes */
 CK(gk1_store_load(path,&restored)==GK1_STORE_CORRUPT);
 CK(restored.events==2);
 CK(remove(path)==0);
 CK(gk1_store_save_new("",&pending)==GK1_STORE_BAD);
 CK(gk1_store_load("",&restored)==GK1_STORE_BAD);
 char too_long[1025];memset(too_long,'a',sizeof too_long);too_long[1024]='\0';
 CK(gk1_store_save_new(too_long,&pending)==GK1_STORE_BAD);
 CK(gk1_store_load(too_long,&restored)==GK1_STORE_BAD);
 puts("EXEC0042 native replay exclusive-save/restore, six profiles, forged-state, corruption and atomicity PASS");
 printf("assertions=%u\n",checks);
 return 0;
}
