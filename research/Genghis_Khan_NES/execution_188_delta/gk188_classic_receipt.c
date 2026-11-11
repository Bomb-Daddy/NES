#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "gk188_classic_receipt.h"
#include "gk1_source_identity.h"
#include <errno.h>
#include <stdint.h>
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

#define GK188_BYTES 96u
static void write16(uint8_t *p,unsigned v){p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);}
static unsigned read16(const uint8_t *p){return (unsigned)p[0]+((unsigned)p[1]<<8);}
static void encode(const Gk79Checkpoint *cp,const Gk187ClassicAssign *s,
                   uint8_t b[GK188_BYTES]){
 memset(b,0,GK188_BYTES);
 memcpy(b,"GC18",4);b[4]=1u;b[5]=cp->profile;
 b[6]=cp->world.country_count;b[7]=cp->player_index;
 b[8]=s->action_count;b[9]=s->passed;
 write16(b+10,cp->year);write16(b+12,cp->rate);
 memcpy(b+16,cp->raw_sha256,32);
 for(unsigned i=0;i<s->action_count;i++){
  const Gk187Action *a=&s->actions[i];
  b[48+4*i]=a->from;b[49+4*i]=a->to;write16(b+50+4*i,a->quantity);
 }
 uint8_t digest[32];gk1_sha256_bytes(b,80,digest);
 memcpy(b+80,digest,16);
}
Gk188Result gk188_save_new(const char *path,const Gk79Checkpoint *cp,
 const Gk187ClassicAssign *s){
 if(!path||!*path||!cp||!s)return GK188_INVALID;
 if(!gk187_intact(s,cp))return GK188_SOURCE;
 uint8_t b[GK188_BYTES];encode(cp,s,b);
 int fd=-1;
#ifdef _WIN32
 errno_t err=_sopen_s(&fd,path,_O_WRONLY|_O_CREAT|_O_EXCL|_O_BINARY,
                     _SH_DENYNO,_S_IREAD|_S_IWRITE);
 if(err)return err==EEXIST?GK188_EXISTS:GK188_IO;
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
 if(fd<0)return errno==EEXIST?GK188_EXISTS:GK188_IO;
 FILE *f=fdopen(fd,"wb");
#endif
 if(!f){
#ifdef _WIN32
 _close(fd);
#else
 close(fd);
#endif
 (void)remove(path);return GK188_IO;
 }
 size_t n=fwrite(b,1,GK188_BYTES,f);
 int bad=ferror(f)||n!=GK188_BYTES||fflush(f)!=0;
 if(!bad){
#ifdef _WIN32
  bad=(_commit(fd)!=0);
#else
  bad=(fsync(fd)!=0);
#endif
 }
 if(fclose(f)!=0)bad=1;
 if(bad){(void)remove(path);return GK188_IO;}
 return GK188_OK;
}
Gk188Result gk188_load(const char *path,const Gk79Checkpoint *cp,
 Gk187ClassicAssign *out){
 if(!path||!*path||!cp||!out)return GK188_INVALID;
 Gk187ClassicAssign fresh;
 if(gk187_open(cp,&fresh)!=GK187_OK)return GK188_SOURCE;

#ifdef _WIN32
 FILE *f=fopen(path,"rb");if(!f)return GK188_IO;
#else
 int fd=open(path,O_RDONLY
#ifdef O_NOFOLLOW
 |O_NOFOLLOW
#endif
#ifdef O_CLOEXEC
 |O_CLOEXEC
#endif
 );
 if(fd<0)return GK188_IO;
 struct stat st;
 if(fstat(fd,&st)!=0||!S_ISREG(st.st_mode)||st.st_nlink!=1||
    (st.st_mode&077)!=0||st.st_size!=(off_t)GK188_BYTES){
  close(fd);return GK188_IO;
 }
 FILE *f=fdopen(fd,"rb");if(!f){close(fd);return GK188_IO;}
#endif
 uint8_t b[GK188_BYTES+1];
 size_t n=fread(b,1,sizeof b,f);
 int bad=ferror(f);
 if(fclose(f)!=0)bad=1;
 if(bad)return GK188_IO;
 if(n!=GK188_BYTES||memcmp(b,"GC18",4)||b[4]!=1||
   b[5]!=cp->profile||b[6]!=cp->world.country_count||
   b[7]!=cp->player_index||b[8]>3u||b[9]>1u||
   read16(b+10)!=cp->year||read16(b+12)!=cp->rate||
   b[14]!=0||b[15]!=0||memcmp(b+16,cp->raw_sha256,32))return GK188_SOURCE;
 for(unsigned i=48+4u*b[8];i<80u;i++)if(b[i]!=0)return GK188_CORRUPT;
 uint8_t digest[32];gk1_sha256_bytes(b,80,digest);
 if(memcmp(b+80,digest,16))return GK188_CORRUPT;
 for(unsigned i=0;i<b[8];i++){
  Gk187Result r=gk187_transfer(&fresh,cp,b[48+4*i],b[49+4*i],
                               (uint16_t)read16(b+50+4*i));
  if(r!=GK187_OK)return GK188_CORRUPT;
 }
 if(b[9]&&gk187_pass(&fresh,cp)!=GK187_OK)return GK188_CORRUPT;
 if(!gk187_intact(&fresh,cp))return GK188_CORRUPT;
 *out=fresh;return GK188_OK;
}
