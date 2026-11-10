#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "gk167_private_cold_save.h"
#include "gk1_source_identity.h"
#include "gk162_original_entry.h"
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <io.h>
#include <share.h>
#else
#include <unistd.h>
#endif

/* Versioned, explicit little-endian format. Owner-private file contains
 * ORIGINAL STOCK NES RAM bytes and must never be part of a source ZIP.
 * 20-byte metadata + original-record digest + Nx83 bytes + file SHA256.
 */
#define GK167_HEAD 52u
#define GK167_TAIL 32u
#define GK167_LIMIT (GK167_HEAD + 27u*GK77_RECORD_BYTES + GK167_TAIL)
static void put16(unsigned char *p,unsigned x){p[0]=(unsigned char)x;p[1]=(unsigned char)(x>>8);}
static unsigned get16(const unsigned char *p){return (unsigned)p[0]+((unsigned)p[1]<<8);}
static void wipe(void *buf,size_t n){volatile unsigned char *p=buf;while(n--)*p++=0;}
static int profile_ok(const Gk79Checkpoint *s){
 if(!s||!s->valid||!s->source_verified||!gk85_raw_source_intact(s)||
    !gk88_stock_projection_intact(s)||s->orders.playable!=1u||
    s->orders.scenario!=(uint8_t)s->world.scenario_id||
    s->orders.country_id!=(unsigned)s->player_index+1u||
    s->orders.season!=GK71_SPRING||s->orders.governor!=GK71_BASE||
    s->orders.allowance!=3u||s->orders.completed!=0u||
    !((s->orders.remaining==3u&&s->orders.ended==0u)||
      (s->orders.remaining==0u&&s->orders.ended==1u))||
    s->view_index>=s->world.country_count||
    s->world.phase!=(Gk1ScenarioPhase)GK71_RESERVED_VERIFIED_PLAYABLE_PHASE)
   return 0;
 if(s->profile==GK77_FRESH_MONGOL_WORKRAM)
   return s->year==1175u&&s->rate==121u&&s->world.country_count==14u&&s->player_index==0u;
 if(s->profile==GK77_ARCHIVED_MONGOL_SAVERAM)
   return s->year==1175u&&s->rate==101u&&s->world.country_count==14u&&s->player_index==0u;
 if(s->profile==GK77_ARCHIVED_WORLD_SAVERAM)
   return s->year==1206u&&s->rate==114u&&s->world.country_count==27u&&
     (s->player_index==0u||s->player_index==6u||
      s->player_index==10u||s->player_index==18u);
 return 0;
}
static size_t encode(const Gk79Checkpoint *s,unsigned char b[GK167_LIMIT]){
 unsigned count=s->world.country_count;
 size_t body=GK167_HEAD+(size_t)count*GK77_RECORD_BYTES;
 memset(b,0,GK167_LIMIT);
 memcpy(b,"GK17",4);b[4]=1u;b[5]=s->profile;
 b[6]=(uint8_t)s->world.scenario_id;b[7]=s->player_index;
 b[8]=(uint8_t)count;b[9]=s->orders.remaining;
 b[10]=s->orders.ended;b[11]=s->view_index;
 put16(b+12,s->year);put16(b+14,s->rate);
 memcpy(b+20,s->raw_sha256,32);
 memcpy(b+GK167_HEAD,s->stock_raw,(size_t)count*GK77_RECORD_BYTES);
 gk1_sha256_bytes(b,body,b+body);
 return body+GK167_TAIL;
}
Gk167Result gk167_save_private_new(const char *path,const Gk1App *app){
 if(!path||!*path||!app||app->rules_profile!=0u||
    !profile_ok(&app->stock_checkpoint)||app->enhanced_assign.active||
    app->enhanced_summer.active||app->enhanced_fall.active||
    app->enhanced_winter.active||app->enhanced_year2.active)return GK167_INVALID;
 unsigned char b[GK167_LIMIT];
 size_t n=encode(&app->stock_checkpoint,b);
 int fd=-1;
#ifdef _WIN32
 errno_t e=_sopen_s(&fd,path,_O_CREAT|_O_EXCL|_O_WRONLY|_O_BINARY,
                       _SH_DENYNO,_S_IREAD|_S_IWRITE);
 if(e){wipe(b,sizeof b);return e==EEXIST?GK167_EXISTS:GK167_IO;}
 FILE *f=_fdopen(fd,"wb");
#else
 fd=open(path,O_WRONLY|O_CREAT|O_EXCL
#ifdef O_NOFOLLOW
         |O_NOFOLLOW
#endif
   ,0600);
 if(fd<0){int e=errno;wipe(b,sizeof b);return e==EEXIST?GK167_EXISTS:GK167_IO;}
 FILE *f=fdopen(fd,"wb");
#endif
 if(!f){
#ifdef _WIN32
  _close(fd);
#else
  close(fd);
#endif
  (void)remove(path);wipe(b,sizeof b);return GK167_IO;
 }
 size_t wrote=fwrite(b,1,n,f);
 /* A successful save is not durable until the data has been flushed from
  * stdio and the operating system file cache. Never report success for a
  * partially persisted owner-private original-stock checkpoint. */
 int bad=ferror(f);
 if(!bad&&fflush(f)!=0)bad=1;
 if(!bad){
#ifdef _WIN32
   if(_commit(fd)!=0)bad=1;
#else
   if(fsync(fd)!=0)bad=1;
#endif
 }
 int closed=fclose(f);
 wipe(b,sizeof b);
 if(bad||closed||wrote!=n){(void)remove(path);return GK167_IO;}
 return GK167_OK;
}
Gk167Result gk167_cold_load_private(const char *path,const char *csv,Gk1App *app){
 if(!path||!*path||!csv||!*csv||!app||app->rules_profile>1u||
    !app->world_ready[0]||!app->world_ready[1]||
    app->worlds[0].phase!=GK1_PRE_SETUP_TEMPLATE||
    app->worlds[1].phase!=GK1_PRE_SETUP_TEMPLATE||
    !gk1_verified_original_csv(csv))return GK167_INVALID;
 /* The payload includes private licensed original NES RAM, so refuse paths
  * that can expose it to other users or redirect the read through a symlink.
  * Inspection and reading use the SAME file descriptor (no TOCTOU gap).
  * On Windows, the existing fopen path remains untested for ACL isolation. */
 FILE *f=NULL;
#ifndef _WIN32
 int fd=open(path,O_RDONLY
#ifdef O_NOFOLLOW
    |O_NOFOLLOW
#endif
#ifdef O_CLOEXEC
    |O_CLOEXEC
#endif
 );
 if(fd<0)return GK167_IO;
 struct stat st;
 if(fstat(fd,&st)!=0||!S_ISREG(st.st_mode)||
    (st.st_mode&077)!=0||st.st_nlink!=1||
    st.st_size<(off_t)(GK167_HEAD+GK167_TAIL)||
    st.st_size>(off_t)GK167_LIMIT){close(fd);return GK167_IO;}
 f=fdopen(fd,"rb");
 if(!f){close(fd);return GK167_IO;}
#else
 f=fopen(path,"rb");if(!f)return GK167_IO;
#endif
 unsigned char b[GK167_LIMIT+1u];
 size_t n=fread(b,1,sizeof b,f);
 int bad=ferror(f),closed=fclose(f);
 if(bad||closed){wipe(b,sizeof b);return GK167_IO;}
 if(n<GK167_HEAD+GK167_TAIL||n>GK167_LIMIT||memcmp(b,"GK17",4)||b[4]!=1u||
    b[5]<1u||b[5]>3u||b[6]!=(b[5]==3u?2u:1u)||
    b[8]!=(b[6]==1u?14u:27u)||b[11]>=b[8]||
    b[16]!=0u||b[17]!=0u||b[18]!=0u||b[19]!=0u){
  wipe(b,sizeof b);return GK167_CORRUPT;
 }
 const size_t body=GK167_HEAD+(size_t)b[8]*GK77_RECORD_BYTES;
 if(n!=body+GK167_TAIL){wipe(b,sizeof b);return GK167_CORRUPT;}
 unsigned char sha[32];gk1_sha256_bytes(b,body,sha);
 if(memcmp(sha,b+body,GK167_TAIL)!=0){wipe(b,sizeof b);return GK167_CORRUPT;}
 gk1_sha256_bytes(b+GK167_HEAD,body-GK167_HEAD,sha);
 if(memcmp(sha,b+20,32)!=0||
    !gk77_known_source_digest((Gk77Profile)b[5],sha)){
  wipe(b,sizeof b);return GK167_SOURCE;
 }
 Gk77StockSpring *source=calloc(1,sizeof *source);
 Gk79Checkpoint *stock=malloc(sizeof *stock);
 if(!source||!stock){free(source);free(stock);wipe(b,sizeof b);return GK167_IO;}
 source->profile=(Gk77Profile)b[5];source->scenario=b[6];
 source->country_count=b[8];source->orders=3u;
 source->year=(uint16_t)get16(b+12);source->rate=(uint16_t)get16(b+14);
 Gk167Result result=GK167_CORRUPT;
 for(unsigned i=0;i<b[8];i++){
  if(gk77_decode(b+GK167_HEAD+i*GK77_RECORD_BYTES,
     GK77_RECORD_BYTES,&source->countries[i])!=GK77_OK)goto end;
 }
 if(gk79_from_decoded(&app->worlds[b[6]-1u],source,b[7],stock)!=GK79_OK)goto end;
 stock->source_verified=1u;
 stock->view_index=b[11];
 stock->orders.remaining=b[9];stock->orders.ended=b[10];
 stock->pending_pass=0u;stock->raw_view=0u;
 if(!profile_ok(stock)){result=GK167_SOURCE;goto end;}
 /* Existing source attestation gate accepts only the original 3-order moment.
  * Validate saved 0-order PASS independently above, then commit the exact
  * authenticated 3-order initial checkpoint and apply the stored PASS state.
  * No seasonal advancement or new source data is fabricated. */
 Gk79Checkpoint *initial=malloc(sizeof *initial);
 if(!initial){result=GK167_IO;goto end;}
 *initial=*stock; initial->orders.remaining=3u; initial->orders.ended=0u;
 Gk162EntryStatus rc=gk162_commit_verified_checkpoint(app,csv,initial);
 wipe(initial,sizeof *initial);free(initial);
 if(rc!=GK162_ENTRY_OK){result=GK167_SOURCE;goto end;}
 app->rules_profile=0u;
 app->stock_checkpoint.view_index=stock->view_index;
 app->stock_checkpoint.orders.remaining=stock->orders.remaining;
 app->stock_checkpoint.orders.ended=stock->orders.ended;
 strcpy(app->status,"PRIVATE STOCK SAVE RESTORED; SOURCE SHA VERIFIED; VIEW/PASS ONLY");
 result=GK167_OK;
end:
 wipe(source,sizeof *source);wipe(stock,sizeof *stock);wipe(b,sizeof b);
 free(source);free(stock);return result;
}
