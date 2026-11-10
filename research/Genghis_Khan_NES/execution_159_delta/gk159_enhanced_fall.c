#include "gk159_enhanced_fall.h"
#include "gk1_source_identity.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>

static Gk159Result worker_result(Gk116Result r){
 if(r==GK116_OK)return GK159_OK;
 if(r==GK116_NO_WORKER)return GK159_NO_WORKER;
 if(r==GK116_NO_ORDERS)return GK159_NO_ORDERS;
 return GK159_INPUT;
}
static int summer_complete(const Gk116EnhancedAssign *spring,
 const Gk158EnhancedSummer *summer,const Gk79Checkpoint *original){
 return summer&&gk158_attestation_intact(summer,spring,original)&&
  summer->budget.ended==1u&&summer->budget.remaining==0u;
}
Gk159Result gk159_open(const Gk116EnhancedAssign *spring,
 const Gk158EnhancedSummer *summer,const Gk79Checkpoint *original,
 Gk159EnhancedFall *out){
 if(!out)return GK159_INPUT;
 if(!summer_complete(spring,summer,original))return GK159_SOURCE;
 Gk159EnhancedFall next={0};
 next.mutable_world=summer->mutable_world;
 next.budget=summer->budget;
 if(gk71_next_season(&next.mutable_world,&next.budget,GK71_FALL)!=GK71_OK)
  return GK159_SOURCE;
 next.active=1u;
 *out=next;
 return GK159_OK;
}
int gk159_attestation_intact(const Gk159EnhancedFall *f,
 const Gk116EnhancedAssign *spring,const Gk158EnhancedSummer *summer,
 const Gk79Checkpoint *original){
 if(!f||f->active!=1u||!summer_complete(spring,summer,original)||
    f->action_count>GK159_MAX_ACTIONS||f->pass_ended>1u||
    f->budget.season!=GK71_FALL||f->budget.governor!=GK71_BASE||
    f->budget.allowance!=3u||f->budget.playable!=1u||
    f->budget.completed!=f->action_count||
    (!f->pass_ended&&f->budget.remaining+f->action_count!=3u)||
    (f->pass_ended&&(f->budget.remaining!=0u||
      !f->budget.ended||f->action_count>=3u))||
    f->budget.ended!=(f->budget.remaining==0u))return 0;
 Gk159EnhancedFall replay={0};
 if(gk159_open(spring,summer,original,&replay)!=GK159_OK)return 0;
 for(unsigned i=0;i<f->action_count;i++){
  const Gk116LaborAction *a=&f->actions[i];
  if(gk116_worker_math(&replay.mutable_world,&replay.budget,
      spring->player_index,a->from,a->to,a->quantity)!=GK116_OK)return 0;
  replay.actions[replay.action_count++]=*a;
 }
 if(f->pass_ended){
  if(gk71_pass(&replay.budget)!=GK71_OK)return 0;
  replay.pass_ended=1u;
 }
 return memcmp(&replay,f,sizeof replay)==0;
}
Gk159Result gk159_transfer(Gk159EnhancedFall *fall,
 const Gk116EnhancedAssign *spring,const Gk158EnhancedSummer *summer,
 const Gk79Checkpoint *original,uint8_t from,uint8_t to,uint16_t quantity){
 if(!fall||!spring||!summer||!original)return GK159_INPUT;
 if(!gk159_attestation_intact(fall,spring,summer,original))return GK159_SOURCE;
 if(fall->budget.ended||fall->pass_ended||
    fall->action_count>=GK159_MAX_ACTIONS)return GK159_NO_ORDERS;
 Gk159EnhancedFall next=*fall;
 Gk159Result r=worker_result(gk116_worker_math(&next.mutable_world,&next.budget,
     spring->player_index,from,to,quantity));
 if(r!=GK159_OK)return r;
 next.actions[next.action_count++]=(Gk116LaborAction){from,to,quantity};
 if(!gk159_attestation_intact(&next,spring,summer,original))return GK159_SOURCE;
 *fall=next;return GK159_OK;
}
Gk159Result gk159_pass(Gk159EnhancedFall *fall,
 const Gk116EnhancedAssign *spring,const Gk158EnhancedSummer *summer,
 const Gk79Checkpoint *original){
 if(!fall||!spring||!summer||!original)return GK159_INPUT;
 if(!gk159_attestation_intact(fall,spring,summer,original))return GK159_SOURCE;
 if(fall->budget.ended||fall->pass_ended)return GK159_NO_ORDERS;
 Gk159EnhancedFall next=*fall;
 if(gk71_pass(&next.budget)!=GK71_OK)return GK159_INPUT;
 next.pass_ended=1u;
 if(!gk159_attestation_intact(&next,spring,summer,original))return GK159_SOURCE;
 *fall=next;return GK159_OK;
}
static void write_actions(uint8_t *dst,const Gk116LaborAction *a,unsigned n){
 for(unsigned i=0;i<n;i++){
  dst[4u*i]=a[i].from;dst[4u*i+1u]=a[i].to;
  dst[4u*i+2u]=(uint8_t)a[i].quantity;
  dst[4u*i+3u]=(uint8_t)(a[i].quantity>>8);
 }
}
static uint16_t quantity(const uint8_t *p){
 return (uint16_t)((unsigned)p[2]|((unsigned)p[3]<<8));
}
static void encode(uint8_t raw[GK159_RECEIPT_BYTES],
 const Gk116EnhancedAssign *sp,const Gk158EnhancedSummer *su,
 const Gk159EnhancedFall *fa){
 memset(raw,0,GK159_RECEIPT_BYTES);
 memcpy(raw,"GF19",4u);raw[4]=1u;raw[5]=sp->profile;
 raw[6]=(uint8_t)sp->mutable_world.scenario_id;raw[7]=sp->player_index;
 raw[8]=(uint8_t)sp->year;raw[9]=(uint8_t)(sp->year>>8);
 raw[10]=(uint8_t)sp->rate;raw[11]=(uint8_t)(sp->rate>>8);
 raw[12]=sp->action_count;raw[13]=sp->pass_ended;
 raw[14]=su->action_count;raw[15]=su->pass_ended;
 raw[16]=fa->action_count;raw[17]=fa->pass_ended;
 memcpy(raw+18u,sp->source_sha,32u);
 write_actions(raw+50u,sp->actions,sp->action_count);
 write_actions(raw+62u,su->actions,su->action_count);
 write_actions(raw+74u,fa->actions,fa->action_count);
 gk1_sha256_bytes(raw,112u,raw+112u);
}
Gk159Result gk159_save_new(const char *path,const Gk79Checkpoint *original,
 const Gk116EnhancedAssign *spring,const Gk158EnhancedSummer *summer,
 const Gk159EnhancedFall *fall){
 if(!path||!*path)return GK159_INPUT;
 if(!gk159_attestation_intact(fall,spring,summer,original))return GK159_SOURCE;
 uint8_t raw[GK159_RECEIPT_BYTES];encode(raw,spring,summer,fall);
 errno=0;FILE *fp=fopen(path,"wbx");
 if(!fp)return errno==EEXIST?GK159_EXISTS:GK159_IO;
 size_t n=fwrite(raw,1,sizeof raw,fp);int failed=ferror(fp),closed=fclose(fp);
 if(n!=sizeof raw||failed||closed){(void)remove(path);return GK159_IO;}
 return GK159_OK;
}
Gk159Result gk159_cold_load(const char *path,const Gk79Checkpoint *original,
 uint8_t enhanced,Gk116EnhancedAssign *spring_out,
 Gk158EnhancedSummer *summer_out,Gk159EnhancedFall *fall_out){
 if(!path||!*path||!spring_out||!summer_out||!fall_out)return GK159_INPUT;
 if(enhanced!=1u)return GK159_SOURCE;
 Gk116EnhancedAssign sp={0};Gk158EnhancedSummer su={0};Gk159EnhancedFall fa={0};
 if(gk116_open(original,1u,&sp)!=GK116_OK)return GK159_SOURCE;
 FILE *fp=fopen(path,"rb");if(!fp)return GK159_IO;
 uint8_t raw[GK159_RECEIPT_BYTES+1u];
 size_t n=fread(raw,1,sizeof raw,fp);int failed=ferror(fp),closed=fclose(fp);
 if(failed||closed)return GK159_IO;
 if(n!=GK159_RECEIPT_BYTES||memcmp(raw,"GF19",4u)||raw[4]!=1u||
    raw[12]>GK116_MAX_ACTIONS||raw[14]>GK158_MAX_ACTIONS||
    raw[16]>GK159_MAX_ACTIONS||raw[13]>1u||raw[15]>1u||raw[17]>1u)
  return GK159_CORRUPT;
 for(unsigned i=0;i<raw[12];i++){
  const uint8_t *a=raw+50u+4u*i;
  if(gk116_transfer(&sp,original,a[0],a[1],quantity(a))!=GK116_OK)
   return GK159_CORRUPT;
 }
 if(raw[13]&&gk116_pass_remaining(&sp,original)!=GK116_OK)return GK159_CORRUPT;
 if(gk158_open(&sp,original,&su)!=GK158_OK)return GK159_CORRUPT;
 for(unsigned i=0;i<raw[14];i++){
  const uint8_t *a=raw+62u+4u*i;
  if(gk158_transfer(&su,&sp,original,a[0],a[1],quantity(a))!=GK158_OK)
   return GK159_CORRUPT;
 }
 if(raw[15]&&gk158_pass(&su,&sp,original)!=GK158_OK)return GK159_CORRUPT;
 if(gk159_open(&sp,&su,original,&fa)!=GK159_OK)return GK159_CORRUPT;
 for(unsigned i=0;i<raw[16];i++){
  const uint8_t *a=raw+74u+4u*i;
  if(gk159_transfer(&fa,&sp,&su,original,a[0],a[1],quantity(a))!=GK159_OK)
   return GK159_CORRUPT;
 }
 if(raw[17]&&gk159_pass(&fa,&sp,&su,original)!=GK159_OK)return GK159_CORRUPT;
 uint8_t exact[GK159_RECEIPT_BYTES];encode(exact,&sp,&su,&fa);
 if(memcmp(raw,exact,sizeof exact))return GK159_CORRUPT;
 *spring_out=sp;*summer_out=su;*fall_out=fa;
 return GK159_OK;
}
