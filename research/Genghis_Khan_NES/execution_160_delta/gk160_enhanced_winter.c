#include "gk160_enhanced_winter.h"
#include "gk1_source_identity.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>

static Gk160Result map_labor(Gk116Result r){
 if(r==GK116_OK)return GK160_OK;
 if(r==GK116_NO_WORKER)return GK160_NO_WORKER;
 if(r==GK116_NO_ORDERS)return GK160_NO_ORDERS;
 return GK160_INPUT;
}
static int fall_done(const Gk116EnhancedAssign *sp,
 const Gk158EnhancedSummer *su,const Gk159EnhancedFall *fa,
 const Gk79Checkpoint *original){
 return fa&&gk159_attestation_intact(fa,sp,su,original)&&
  fa->budget.ended==1u&&fa->budget.remaining==0u;
}
Gk160Result gk160_open(const Gk116EnhancedAssign *sp,
 const Gk158EnhancedSummer *su,const Gk159EnhancedFall *fa,
 const Gk79Checkpoint *original,Gk160EnhancedWinter *out){
 if(!out)return GK160_INPUT;
 if(!fall_done(sp,su,fa,original))return GK160_SOURCE;
 Gk160EnhancedWinter next={0};next.mutable_world=fa->mutable_world;
 next.budget=fa->budget;
 if(gk71_next_season(&next.mutable_world,&next.budget,GK71_WINTER)!=GK71_OK)
  return GK160_SOURCE;
 next.active=1u;*out=next;return GK160_OK;
}
int gk160_attestation_intact(const Gk160EnhancedWinter *wi,
 const Gk116EnhancedAssign *sp,const Gk158EnhancedSummer *su,
 const Gk159EnhancedFall *fa,const Gk79Checkpoint *original){
 if(!wi||wi->active!=1u||!fall_done(sp,su,fa,original)||
    wi->action_count>GK160_MAX_ACTIONS||wi->pass_ended>1u||
    wi->budget.season!=GK71_WINTER||wi->budget.governor!=GK71_BASE||
    wi->budget.allowance!=3u||wi->budget.playable!=1u||
    wi->budget.completed!=wi->action_count||
    (!wi->pass_ended&&wi->budget.remaining+wi->action_count!=3u)||
    (wi->pass_ended&&(wi->budget.remaining!=0u||!wi->budget.ended||
       wi->action_count>=3u))||
    wi->budget.ended!=(wi->budget.remaining==0u))return 0;
 Gk160EnhancedWinter replay={0};
 if(gk160_open(sp,su,fa,original,&replay)!=GK160_OK)return 0;
 for(unsigned i=0;i<wi->action_count;i++){
  const Gk116LaborAction *a=&wi->actions[i];
  if(gk116_worker_math(&replay.mutable_world,&replay.budget,
     sp->player_index,a->from,a->to,a->quantity)!=GK116_OK)return 0;
  replay.actions[replay.action_count++]=*a;
 }
 if(wi->pass_ended){
  if(gk71_pass(&replay.budget)!=GK71_OK)return 0;
  replay.pass_ended=1u;
 }
 return memcmp(&replay,wi,sizeof replay)==0;
}
Gk160Result gk160_transfer(Gk160EnhancedWinter *wi,
 const Gk116EnhancedAssign *sp,const Gk158EnhancedSummer *su,
 const Gk159EnhancedFall *fa,const Gk79Checkpoint *original,
 uint8_t from,uint8_t to,uint16_t quantity){
 if(!wi||!sp||!su||!fa||!original)return GK160_INPUT;
 if(!gk160_attestation_intact(wi,sp,su,fa,original))return GK160_SOURCE;
 if(wi->budget.ended||wi->pass_ended||wi->action_count>=GK160_MAX_ACTIONS)
  return GK160_NO_ORDERS;
 Gk160EnhancedWinter next=*wi;
 Gk160Result r=map_labor(gk116_worker_math(&next.mutable_world,&next.budget,
     sp->player_index,from,to,quantity));
 if(r!=GK160_OK)return r;
 next.actions[next.action_count++]=(Gk116LaborAction){from,to,quantity};
 if(!gk160_attestation_intact(&next,sp,su,fa,original))return GK160_SOURCE;
 *wi=next;return GK160_OK;
}
Gk160Result gk160_pass(Gk160EnhancedWinter *wi,
 const Gk116EnhancedAssign *sp,const Gk158EnhancedSummer *su,
 const Gk159EnhancedFall *fa,const Gk79Checkpoint *original){
 if(!wi||!sp||!su||!fa||!original)return GK160_INPUT;
 if(!gk160_attestation_intact(wi,sp,su,fa,original))return GK160_SOURCE;
 if(wi->budget.ended||wi->pass_ended)return GK160_NO_ORDERS;
 Gk160EnhancedWinter next=*wi;
 if(gk71_pass(&next.budget)!=GK71_OK)return GK160_INPUT;
 next.pass_ended=1u;
 if(!gk160_attestation_intact(&next,sp,su,fa,original))return GK160_SOURCE;
 *wi=next;return GK160_OK;
}
static void actions(uint8_t *p,const Gk116LaborAction *a,unsigned n){
 for(unsigned i=0;i<n;i++){
  p[4u*i]=a[i].from;p[4u*i+1u]=a[i].to;
  p[4u*i+2u]=(uint8_t)a[i].quantity;
  p[4u*i+3u]=(uint8_t)(a[i].quantity>>8);
 }
}
static uint16_t amount(const uint8_t *p){
 return (uint16_t)((unsigned)p[2]|((unsigned)p[3]<<8));
}
/* Independent GW20 record; never overwrites earlier GA16/GS18/GF19 saves.
 * 0..19 version/identity/action counts, 20..51 original source SHA,
 * 52..99 four seasonal 12-byte action logs, zeroed 100..127 reserved,
 * 128..159 SHA256 of first 128 bytes. */
static void encode(uint8_t raw[GK160_RECEIPT_BYTES],
 const Gk116EnhancedAssign *sp,const Gk158EnhancedSummer *su,
 const Gk159EnhancedFall *fa,const Gk160EnhancedWinter *wi){
 memset(raw,0,GK160_RECEIPT_BYTES);
 memcpy(raw,"GW20",4u);raw[4]=1u;raw[5]=sp->profile;
 raw[6]=(uint8_t)sp->mutable_world.scenario_id;raw[7]=sp->player_index;
 raw[8]=(uint8_t)sp->year;raw[9]=(uint8_t)(sp->year>>8);
 raw[10]=(uint8_t)sp->rate;raw[11]=(uint8_t)(sp->rate>>8);
 raw[12]=sp->action_count;raw[13]=sp->pass_ended;
 raw[14]=su->action_count;raw[15]=su->pass_ended;
 raw[16]=fa->action_count;raw[17]=fa->pass_ended;
 raw[18]=wi->action_count;raw[19]=wi->pass_ended;
 memcpy(raw+20u,sp->source_sha,32u);
 actions(raw+52u,sp->actions,sp->action_count);
 actions(raw+64u,su->actions,su->action_count);
 actions(raw+76u,fa->actions,fa->action_count);
 actions(raw+88u,wi->actions,wi->action_count);
 gk1_sha256_bytes(raw,128u,raw+128u);
}
Gk160Result gk160_save_new(const char *path,const Gk79Checkpoint *o,
 const Gk116EnhancedAssign *sp,const Gk158EnhancedSummer *su,
 const Gk159EnhancedFall *fa,const Gk160EnhancedWinter *wi){
 if(!path||!*path)return GK160_INPUT;
 if(!gk160_attestation_intact(wi,sp,su,fa,o))return GK160_SOURCE;
 uint8_t raw[GK160_RECEIPT_BYTES];encode(raw,sp,su,fa,wi);
 errno=0;FILE *fp=fopen(path,"wbx");
 if(!fp)return errno==EEXIST?GK160_EXISTS:GK160_IO;
 size_t n=fwrite(raw,1,sizeof raw,fp);int err=ferror(fp),close_err=fclose(fp);
 if(n!=sizeof raw||err||close_err){(void)remove(path);return GK160_IO;}
 return GK160_OK;
}
Gk160Result gk160_cold_load(const char *path,const Gk79Checkpoint *o,
 uint8_t enhanced,Gk116EnhancedAssign *spout,Gk158EnhancedSummer *suout,
 Gk159EnhancedFall *faout,Gk160EnhancedWinter *wiout){
 if(!path||!*path||!o||!spout||!suout||!faout||!wiout)return GK160_INPUT;
 if(enhanced!=1u)return GK160_SOURCE;
 Gk116EnhancedAssign sp={0};Gk158EnhancedSummer su={0};
 Gk159EnhancedFall fa={0};Gk160EnhancedWinter wi={0};
 if(gk116_open(o,1u,&sp)!=GK116_OK)return GK160_SOURCE;
 FILE *fp=fopen(path,"rb");if(!fp)return GK160_IO;
 uint8_t raw[GK160_RECEIPT_BYTES+1u];
 size_t n=fread(raw,1,sizeof raw,fp);int err=ferror(fp),close_err=fclose(fp);
 if(err||close_err)return GK160_IO;
 if(n!=GK160_RECEIPT_BYTES||memcmp(raw,"GW20",4u)||raw[4]!=1u||
    raw[12]>GK116_MAX_ACTIONS||raw[14]>GK158_MAX_ACTIONS||
    raw[16]>GK159_MAX_ACTIONS||raw[18]>GK160_MAX_ACTIONS||
    raw[13]>1u||raw[15]>1u||raw[17]>1u||raw[19]>1u)
  return GK160_CORRUPT;
 for(unsigned i=0;i<raw[12];i++){
  const uint8_t *a=raw+52u+4u*i;
  if(gk116_transfer(&sp,o,a[0],a[1],amount(a))!=GK116_OK)return GK160_CORRUPT;
 }
 if(raw[13]&&gk116_pass_remaining(&sp,o)!=GK116_OK)return GK160_CORRUPT;
 if(gk158_open(&sp,o,&su)!=GK158_OK)return GK160_CORRUPT;
 for(unsigned i=0;i<raw[14];i++){
  const uint8_t *a=raw+64u+4u*i;
  if(gk158_transfer(&su,&sp,o,a[0],a[1],amount(a))!=GK158_OK)return GK160_CORRUPT;
 }
 if(raw[15]&&gk158_pass(&su,&sp,o)!=GK158_OK)return GK160_CORRUPT;
 if(gk159_open(&sp,&su,o,&fa)!=GK159_OK)return GK160_CORRUPT;
 for(unsigned i=0;i<raw[16];i++){
  const uint8_t *a=raw+76u+4u*i;
  if(gk159_transfer(&fa,&sp,&su,o,a[0],a[1],amount(a))!=GK159_OK)
    return GK160_CORRUPT;
 }
 if(raw[17]&&gk159_pass(&fa,&sp,&su,o)!=GK159_OK)return GK160_CORRUPT;
 if(gk160_open(&sp,&su,&fa,o,&wi)!=GK160_OK)return GK160_CORRUPT;
 for(unsigned i=0;i<raw[18];i++){
  const uint8_t *a=raw+88u+4u*i;
  if(gk160_transfer(&wi,&sp,&su,&fa,o,a[0],a[1],amount(a))!=GK160_OK)
   return GK160_CORRUPT;
 }
 if(raw[19]&&gk160_pass(&wi,&sp,&su,&fa,o)!=GK160_OK)return GK160_CORRUPT;
 uint8_t exact[GK160_RECEIPT_BYTES];encode(exact,&sp,&su,&fa,&wi);
 if(memcmp(raw,exact,sizeof exact))return GK160_CORRUPT;
 *spout=sp;*suout=su;*faout=fa;*wiout=wi;
 return GK160_OK;
}
