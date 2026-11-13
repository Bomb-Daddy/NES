#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "gk189_classic_cold_save.h"
#include "gk162_original_entry.h"
#include "gk1_source_identity.h"
#include "gk247_receipt_file_guard.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <io.h>
#include <share.h>
#else
#include <unistd.h>
#endif
#define GK189_HEAD 64u
#define GK189_TAIL 32u
#define GK189_LIMIT (GK189_HEAD+27u*GK77_RECORD_BYTES+GK189_TAIL)
static void put16(uint8_t *p,unsigned v){p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);}
static unsigned get16(const uint8_t *p){return (unsigned)p[0]|((unsigned)p[1]<<8);}
static void wipe(void *v,size_t n){volatile uint8_t *p=v;while(n--)*p++=0;}
/* The .gc19 full cold-save is a different, larger private file than the
 * 96-byte legacy GC18 receipt. Its original NES record count is exactly
 * 14 or 27. Validate the OPEN handle against either canonical byte size
 * before decoding; do not trust a pathname, extension, or embedded count.
 * This is an IO guard only; original SHA, replay and CSV checks follow. */
int gk249_classic_cold_file_guard(int fd,const char *path){
 return gk247_receipt_file_guard(fd,path,
           GK189_HEAD+14u*GK77_RECORD_BYTES+GK189_TAIL) ||
        gk247_receipt_file_guard(fd,path,
           GK189_HEAD+27u*GK77_RECORD_BYTES+GK189_TAIL);
}
static int profile_numbers(unsigned p,unsigned count,unsigned year,unsigned rate){
 return (p==GK77_FRESH_MONGOL_WORKRAM&&count==14u&&year==1175u&&rate==121u)||
        (p==GK77_ARCHIVED_MONGOL_SAVERAM&&count==14u&&year==1175u&&rate==101u)||
        (p==GK77_ARCHIVED_WORLD_SAVERAM&&count==27u&&year==1206u&&rate==114u);
}
static int source_valid(const Gk1App *a){
 const Gk79Checkpoint *c;
 if(!a||a->rules_profile!=0u||!a->classic_assign.active)return 0;
 c=&a->stock_checkpoint;
 return c->source_verified==1u&&c->valid==1u&&
  profile_numbers(c->profile,c->world.country_count,c->year,c->rate)&&
  c->view_index<c->world.country_count&&
  c->player_index<c->world.country_count&&
  c->orders.remaining==3u&&c->orders.ended==0u&&
  gk85_raw_source_intact(c)&&gk88_stock_projection_intact(c)&&
  gk187_intact(&a->classic_assign,c);
}
Gk189Result gk189_save_new(const char *path,const Gk1App *app){
 if(!path||!*path||!app)return GK189_INVALID;
 if(!source_valid(app))return GK189_SOURCE;
 const Gk79Checkpoint *cp=&app->stock_checkpoint;
 const Gk187ClassicAssign *s=&app->classic_assign;
 uint8_t b[GK189_LIMIT];memset(b,0,sizeof b);
 unsigned count=cp->world.country_count;
 size_t payload=GK189_HEAD+(size_t)count*GK77_RECORD_BYTES;
 size_t n=payload+GK189_TAIL;
 memcpy(b,"GC19",4);b[4]=1u;b[5]=cp->profile;
 b[6]=(uint8_t)cp->world.scenario_id;b[7]=cp->player_index;
 b[8]=(uint8_t)count;b[9]=cp->view_index;
 b[10]=s->action_count;b[11]=s->passed;
 put16(b+12,cp->year);put16(b+14,cp->rate);
 memcpy(b+16,cp->raw_sha256,32);
 for(unsigned i=0;i<s->action_count;i++){
  b[48+4*i]=s->actions[i].from;b[49+4*i]=s->actions[i].to;
  put16(b+50+4*i,s->actions[i].quantity);
 }
 memcpy(b+GK189_HEAD,cp->stock_raw,(size_t)count*GK77_RECORD_BYTES);
 gk1_sha256_bytes(b,payload,b+payload);
 int fd=-1;
#ifdef _WIN32
 errno_t e=_sopen_s(&fd,path,_O_WRONLY|_O_BINARY|_O_CREAT|_O_EXCL,
                    _SH_DENYNO,_S_IREAD|_S_IWRITE);
 if(e){wipe(b,sizeof b);return e==EEXIST?GK189_EXISTS:GK189_IO;}
 FILE *f=_fdopen(fd,"wb");
#else
 fd=open(path,O_WRONLY|O_CREAT|O_EXCL
#ifdef O_NOFOLLOW
 |O_NOFOLLOW
#endif
#ifdef O_CLOEXEC
 |O_CLOEXEC
#endif
 ,0600);
 if(fd<0){int e=errno;wipe(b,sizeof b);return e==EEXIST?GK189_EXISTS:GK189_IO;}
 FILE *f=fdopen(fd,"wb");
#endif
 if(!f){
#ifdef _WIN32
  _close(fd);
#else
  close(fd);
#endif
  (void)remove(path);wipe(b,sizeof b);return GK189_IO;
 }
 size_t wrote=fwrite(b,1,n,f);
 int bad=ferror(f)||wrote!=n||fflush(f)!=0;
 if(!bad){
#ifdef _WIN32
  bad=(_commit(fd)!=0);
#else
  bad=(fsync(fd)!=0);
#endif
 }
 if(fclose(f)!=0)bad=1;
 wipe(b,sizeof b);
 if(bad){(void)remove(path);return GK189_IO;}
 return GK189_OK;
}
Gk189Result gk189_cold_load(const char *path,const char *csv,Gk1App *app){
 if(!path||!*path||!csv||!*csv||!app)return GK189_INVALID;
 if(app->rules_profile>1u||!app->world_ready[0]||!app->world_ready[1]||
    app->worlds[0].phase!=GK1_PRE_SETUP_TEMPLATE||
    app->worlds[1].phase!=GK1_PRE_SETUP_TEMPLATE||
    !gk1_verified_original_csv(csv))return GK189_SOURCE;
 int fd=-1;FILE *f=NULL;
#ifdef _WIN32
 errno_t e=_sopen_s(&fd,path,_O_RDONLY|_O_BINARY,_SH_DENYWR,0);
 if(e)return e==ENOENT?GK189_MISSING:GK189_IO;
 /* Win32: reject symlinks/junctions, multi-link files and wrong size
  * using the same opened HANDLE as the read. Deny concurrent writers. */
 if(!gk249_classic_cold_file_guard(fd,path)){_close(fd);return GK189_IO;}
 f=_fdopen(fd,"rb");
#else
 fd=open(path,O_RDONLY
#ifdef O_NOFOLLOW
 |O_NOFOLLOW
#endif
#ifdef O_CLOEXEC
 |O_CLOEXEC
#endif
 );
 if(fd<0)return errno==ENOENT?GK189_MISSING:GK189_IO;
 /* POSIX: same open-handle exact-size/owner/private/link validation.
  * Both country counts are permitted, all intermediate sizes refused. */
 if(!gk249_classic_cold_file_guard(fd,path)){close(fd);return GK189_IO;}
 f=fdopen(fd,"rb");
#endif
 if(!f){
#ifdef _WIN32
  _close(fd);
#else
  close(fd);
#endif
  return GK189_IO;
 }
 uint8_t b[GK189_LIMIT+1];
 size_t n=fread(b,1,sizeof b,f);
 int bad=ferror(f);
 if(fclose(f)!=0)bad=1;
 if(bad){wipe(b,sizeof b);return GK189_IO;}
 if(n<GK189_HEAD+GK189_TAIL||n>GK189_LIMIT||
    memcmp(b,"GC19",4)||b[4]!=1u||b[5]<1u||b[5]>3u||
    b[6]!=(b[5]==3u?2u:1u)||b[7]>=b[8]||
    b[8]!=(b[6]==1u?14u:27u)||b[9]>=b[8]||
    b[10]>3u||b[11]>1u||(b[11]&&b[10]>=3u)||
    !profile_numbers(b[5],b[8],get16(b+12),get16(b+14))){
  wipe(b,sizeof b);return GK189_CORRUPT;
 }
 for(unsigned i=48u+4u*b[10];i<GK189_HEAD;i++)if(b[i]!=0){
  wipe(b,sizeof b);return GK189_CORRUPT;
 }
 size_t payload=GK189_HEAD+(size_t)b[8]*GK77_RECORD_BYTES;
 if(n!=payload+GK189_TAIL){wipe(b,sizeof b);return GK189_CORRUPT;}
 uint8_t digest[32];gk1_sha256_bytes(b,payload,digest);
 if(memcmp(digest,b+payload,32)){wipe(b,sizeof b);return GK189_CORRUPT;}
 gk1_sha256_bytes(b+GK189_HEAD,payload-GK189_HEAD,digest);
 if(memcmp(digest,b+16,32)||!gk77_known_source_digest((Gk77Profile)b[5],digest)){
  wipe(b,sizeof b);return GK189_SOURCE;
 }
 Gk77StockSpring *src=calloc(1,sizeof *src);
 Gk79Checkpoint *cp=calloc(1,sizeof *cp);
 Gk1App *stage=malloc(sizeof *stage);
 if(!src||!cp||!stage){free(src);free(cp);free(stage);wipe(b,sizeof b);return GK189_IO;}
 src->profile=(Gk77Profile)b[5];src->scenario=b[6];
 src->country_count=b[8];src->orders=3u;
 src->year=(uint16_t)get16(b+12);src->rate=(uint16_t)get16(b+14);
 Gk189Result result=GK189_CORRUPT;
 for(unsigned i=0;i<src->country_count;i++)
  if(gk77_decode(b+GK189_HEAD+i*GK77_RECORD_BYTES,
       GK77_RECORD_BYTES,&src->countries[i])!=GK77_OK)goto cleanup;
 if(gk79_from_decoded(&app->worlds[src->scenario-1u],src,b[7],cp)!=GK79_OK)goto cleanup;
 cp->source_verified=1u;cp->view_index=b[9];
 if(!gk85_raw_source_intact(cp)||!gk88_stock_projection_intact(cp)){
  result=GK189_SOURCE;goto cleanup;
 }
 *stage=*app;
 stage->rules_profile=0u;
 if(gk162_commit_verified_checkpoint(stage,csv,cp)!=GK162_ENTRY_OK){
  result=GK189_SOURCE;goto cleanup;
 }
 if(gk187_open(&stage->stock_checkpoint,&stage->classic_assign)!=GK187_OK){
  result=GK189_SOURCE;goto cleanup;
 }
 for(unsigned i=0;i<b[10];i++){
  uint8_t from=b[48+4*i],to=b[49+4*i];
  uint16_t v=(uint16_t)get16(b+50+4*i);
  if((from==5u&&to==5u?
       (v<=100u?gk187_set_tax(&stage->classic_assign,&stage->stock_checkpoint,(uint8_t)v):GK187_INVALID):
       gk187_transfer(&stage->classic_assign,&stage->stock_checkpoint,from,to,v))!=GK187_OK)
   goto cleanup;
 }
 if(b[11]&&gk187_pass(&stage->classic_assign,&stage->stock_checkpoint)!=GK187_OK)goto cleanup;
 if(!gk187_intact(&stage->classic_assign,&stage->stock_checkpoint))goto cleanup;
 stage->stock_checkpoint.view_index=b[9];
 /* EXEC0251: a cold save represents a strategic command state, not an
  * in-progress Assign dialog.  Returning to Assign after a completed/Pass
  * season strands the controller on an action that must refuse all orders.
  * Resume in the original-source-attested 14-command menu instead; select
  * the zero-cost VIEW command when no orders remain.  Never grant a season,
  * spend an order, change an authentic record, or bypass integrity checks. */
 stage->stock_checkpoint.pending_pass=0u;
 stage->screen=GK1_SCREEN_STOCK_COMMANDS;
 stage->stock_command_cursor=(uint8_t)(stage->classic_assign.budget.remaining?0u:6u);
 strcpy(stage->status,stage->classic_assign.budget.remaining?
  "PRIVATE CLASSIC COLD-RESUMED: COMMAND MENU / NES PARITY UNVERIFIED":
  "PRIVATE CLASSIC COLD-RESUMED: 0 ORDERS / VIEW OR STORAGE; NEXT NES SEASON OPEN");
 *app=*stage;
 result=GK189_OK;
cleanup:
 wipe(b,sizeof b);wipe(src,sizeof *src);wipe(cp,sizeof *cp);wipe(stage,sizeof *stage);
 free(src);free(cp);free(stage);
 return result;
}
