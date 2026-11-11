#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "gk167_private_cold_save.h"
#include "gk166_fresh_entry.h"
#include "gk164_mss_entry.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#ifndef _WIN32
#include <unistd.h>
#include <fcntl.h>
#endif
#define CHECK(q) do { if(!(q)){fprintf(stderr,"GK167 FAIL line %d: %s\n",__LINE__,#q);return 1;}checks++; }while(0)
static unsigned checks;
static int make_corrupt(const char *in,const char *out){
 FILE *r=fopen(in,"rb");if(!r)return 0;
 unsigned char bytes[2500];size_t n=fread(bytes,1,sizeof bytes,r);
 int err=ferror(r),cl=fclose(r);if(err||cl||n<100||n==sizeof bytes)return 0;
 bytes[59]^=0x5Au;
 FILE *f=fopen(out,"wbx");if(!f)return 0;
 size_t wrote=fwrite(bytes,1,n,f);err=ferror(f);cl=fclose(f);
 #ifndef _WIN32
 if(chmod(out,0600)!=0)return 0;
#endif
 return wrote==n&&!err&&!cl;
}
int main(int argc,char **argv){
 if(argc!=2&&argc!=4&&argc!=6)return 3;
 Gk1App *a=calloc(1,sizeof *a),*cold=calloc(1,sizeof *cold),
   *baseline=calloc(1,sizeof *baseline);
 Gk1TemplateDataset *dataset=calloc(1,sizeof *dataset);
 if(!a||!cold||!baseline||!dataset)return 2;
 CHECK(gk1_template_load(argv[1],dataset)==GK1_LOAD_OK);
 CHECK(gk1_verified_original_csv(argv[1]));
 gk1_app_init(a);gk1_app_init(cold);
 CHECK(gk1_app_install_templates(a,dataset));
 CHECK(gk1_app_install_templates(cold,dataset));
 *baseline=*cold;
 CHECK(gk167_save_private_new("/tmp/gk167_must_not_exist_private.gk17",a)==GK167_INVALID);
 CHECK(gk167_cold_load_private("/no/such/private.gk17",argv[1],cold)==GK167_IO);
 CHECK(memcmp(cold,baseline,sizeof *cold)==0);
 CHECK(gk167_cold_load_private("/no/such/private.gk17","/bad.csv",cold)==GK167_INVALID);
 CHECK(memcmp(cold,baseline,sizeof *cold)==0);
#ifndef _WIN32
 /* Public-data loader must refuse group/world-readable licensed memory,
  * symlinks and multiple hard links even when the underlying bytes exist.
  * With mode 0600, malformed data reaches checksum/format validation. */
 char tmp[]="/tmp/gk168_private_guard_XXXXXX";
 int guardfd=mkstemp(tmp);
 CHECK(guardfd>=0);
 unsigned char malformed[100]={0};
 CHECK(write(guardfd,malformed,sizeof malformed)==(ssize_t)sizeof malformed);
 CHECK(close(guardfd)==0);
 CHECK(chmod(tmp,0644)==0);
 CHECK(gk167_cold_load_private(tmp,argv[1],cold)==GK167_IO);
 CHECK(memcmp(cold,baseline,sizeof *cold)==0);
 CHECK(chmod(tmp,0600)==0);
 CHECK(gk167_cold_load_private(tmp,argv[1],cold)==GK167_CORRUPT);
 CHECK(memcmp(cold,baseline,sizeof *cold)==0);
 char alias[128];snprintf(alias,sizeof alias,"%s.link",tmp);
 CHECK(link(tmp,alias)==0);
 CHECK(gk167_cold_load_private(tmp,argv[1],cold)==GK167_IO);
 CHECK(memcmp(cold,baseline,sizeof *cold)==0);
 CHECK(unlink(alias)==0);
 CHECK(symlink(tmp,alias)==0);
 CHECK(gk167_cold_load_private(alias,argv[1],cold)==GK167_IO);
 CHECK(memcmp(cold,baseline,sizeof *cold)==0);
 CHECK(unlink(alias)==0);
 CHECK(unlink(tmp)==0);
#endif
 if(argc==4){
  const char *save=argv[3];
  char corrupt[4096],pass[4096];
  CHECK(strlen(save)<sizeof corrupt-12);
  snprintf(corrupt,sizeof corrupt,"%s.corrupt",save);
  snprintf(pass,sizeof pass,"%s.pass",save);
  (void)remove(save);(void)remove(corrupt);(void)remove(pass);
  a->rules_profile=0u;
  CHECK(gk166_enter_original_fresh_json(a,argv[1],argv[2],0)==GK166_OK);
  CHECK(a->stock_checkpoint.source_verified==1u);
  CHECK(a->stock_checkpoint.orders.remaining==3u);
  a->enhanced_continuation.active=1u;
  CHECK(gk167_save_private_new(save,a)==GK167_INVALID);
  a->enhanced_continuation.active=0u;
  CHECK(gk167_save_private_new(save,a)==GK167_OK);
#ifndef _WIN32
  struct stat st;
  CHECK(stat(save,&st)==0 && (st.st_mode&077)==0);
#endif
  CHECK(gk167_save_private_new(save,a)==GK167_EXISTS);
  CHECK(gk167_cold_load_private(save,argv[1],cold)==GK167_OK);
  CHECK(cold->screen==GK1_SCREEN_STOCK_CONTINUE);
  CHECK(cold->stock_checkpoint.source_verified==1u);
  CHECK(cold->stock_checkpoint.orders.remaining==3u);
  CHECK(cold->stock_checkpoint.profile==GK77_FRESH_MONGOL_WORKRAM);
  CHECK(cold->stock_checkpoint.world.countries[0].gold==430u);
  CHECK(cold->stock_checkpoint.world.countries[0].goods[10]==208u);
  CHECK(gk85_raw_source_intact(&cold->stock_checkpoint));
  CHECK(memcmp(cold->worlds,baseline->worlds,sizeof cold->worlds)==0);
  /* Legitimate PASS commits 3 -> 0 but never invents Summer/AI. */
  CHECK(gk79_pass(&a->stock_checkpoint)==GK79_OK);
  CHECK(a->stock_checkpoint.orders.remaining==0u);
  CHECK(gk167_save_private_new(pass,a)==GK167_OK);
  CHECK(gk167_cold_load_private(pass,argv[1],cold)==GK167_OK);
  CHECK(cold->stock_checkpoint.orders.ended&&cold->stock_checkpoint.orders.remaining==0u);
  CHECK(cold->stock_checkpoint.year==1175u);
  CHECK(make_corrupt(save,corrupt));
  *baseline=*cold;
  CHECK(gk167_cold_load_private(corrupt,argv[1],cold)==GK167_CORRUPT);
  CHECK(memcmp(cold,baseline,sizeof *cold)==0);
  CHECK(gk167_cold_load_private("/missing/private.gk17",argv[1],cold)==GK167_IO);
  CHECK(memcmp(cold,baseline,sizeof *cold)==0);
  CHECK(gk167_cold_load_private(save,"/bad.csv",cold)==GK167_INVALID);
  CHECK(memcmp(cold,baseline,sizeof *cold)==0);
  a->rules_profile=1u;
  CHECK(gk167_save_private_new("/tmp/gk167_private_must_not_exist.gk17",a)==GK167_INVALID);
  (void)remove(save);(void)remove(pass);(void)remove(corrupt);
 }
 if(argc==6){
  for(unsigned j=0;j<2u;j++){
   Gk77Profile profile=j?GK77_ARCHIVED_WORLD_SAVERAM:GK77_ARCHIVED_MONGOL_SAVERAM;
   const char *saved=argv[4+j];
   (void)remove(saved);
   gk1_app_init(a);gk1_app_init(cold);
   CHECK(gk1_app_install_templates(a,dataset));
   CHECK(gk1_app_install_templates(cold,dataset));
   a->rules_profile=0u;
   CHECK(gk164_enter_original_mss(a,argv[1],argv[2+j],profile,0u)==GK164_OK);
   CHECK(a->stock_checkpoint.world.country_count==(j?27u:14u));
   CHECK(gk167_save_private_new(saved,a)==GK167_OK);
   CHECK(gk167_cold_load_private(saved,argv[1],cold)==GK167_OK);
   CHECK(cold->screen==GK1_SCREEN_STOCK_CONTINUE&&cold->rules_profile==0u);
   CHECK(cold->stock_checkpoint.profile==profile);
   CHECK(cold->stock_checkpoint.rate==(j?114u:101u));
   CHECK(cold->stock_checkpoint.year==(j?1206u:1175u));
   CHECK(cold->stock_checkpoint.world.country_count==(j?27u:14u));
   CHECK(gk85_raw_source_intact(&cold->stock_checkpoint));
   CHECK(cold->stock_checkpoint.orders.remaining==3u);
   CHECK(cold->worlds[0].phase==GK1_PRE_SETUP_TEMPLATE);
   CHECK(cold->worlds[1].phase==GK1_PRE_SETUP_TEMPLATE);
   (void)remove(saved);
  }
 }
 printf("GK167 %u checks PASS: exact-source private original stock cold-resume %s; no NES first Spring generation\n",checks,argc==2?"NEGATIVE ONLY":argc==4?"FRESH POSITIVE":"ARCHIVED POSITIVE");
 free(a);free(cold);free(baseline);free(dataset);return 0;
}
