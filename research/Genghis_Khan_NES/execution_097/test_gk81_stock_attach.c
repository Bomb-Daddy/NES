#include "gk81_stock_attach.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv){
 Gk77StockSpring *source=malloc(sizeof *source);assert(source);
 memset(source,0xA5,sizeof *source);
 Gk77StockSpring *baseline=malloc(sizeof *baseline);assert(baseline);
 *baseline=*source;
 char path[2300]={0},preserved[2300];
 strcpy(path,"PRESERVED");strcpy(preserved,path);
 assert(gk81_stock_filename(GK77_FRESH_MONGOL_WORKRAM));
 assert(gk81_stock_filename(GK77_ARCHIVED_MONGOL_SAVERAM));
 assert(gk81_stock_filename(GK77_ARCHIVED_WORLD_SAVERAM));
 assert(!gk81_stock_filename((Gk77Profile)4));
 assert(gk81_stock_path("/tmp/data",GK77_FRESH_MONGOL_WORKRAM,path,sizeof path));
 assert(strcmp(path,"/tmp/data/GK1NativeResearch/stock_fresh_mongol_1175.records")==0);
 assert(gk81_stock_path("C:\\Users\\Test",GK77_ARCHIVED_WORLD_SAVERAM,path,sizeof path));
 assert(strcmp(path,"C:\\Users\\Test\\GK1NativeResearch\\stock_archived_world_1206.records")==0);
 strcpy(path,"PRESERVED");
 assert(!gk81_stock_path("relative",GK77_FRESH_MONGOL_WORKRAM,path,sizeof path));
 assert(strcmp(path,preserved)==0);
 assert(!gk81_stock_path("/tmp/../escape",GK77_FRESH_MONGOL_WORKRAM,path,sizeof path));
 assert(strcmp(path,preserved)==0);
 assert(!gk81_stock_path("/tmp",GK77_FRESH_MONGOL_WORKRAM,path,3));
 assert(strcmp(path,preserved)==0);
 assert(gk81_stock_attach("/definitely/not/existing",NULL,0,GK77_FRESH_MONGOL_WORKRAM,source,path,sizeof path)==GK81_ATTACH_NOT_FOUND);
 assert(memcmp(source,baseline,sizeof *source)==0&&strcmp(path,preserved)==0);
 assert(gk81_stock_attach(NULL,"/private/stock",GK77_FRESH_MONGOL_WORKRAM,GK77_ARCHIVED_WORLD_SAVERAM,source,path,sizeof path)==GK81_ATTACH_INVALID);
 assert(memcmp(source,baseline,sizeof *source)==0&&strcmp(path,preserved)==0);
 /* Exact-length but unauthenticated file MUST NOT be accepted. */
 const char *bad="gk81_bad_source.bin";
 FILE *f=fopen(bad,"wb");assert(f);
 for(int i=0;i<14*83;i++)assert(fputc(i&255,f)!=EOF);
 assert(fclose(f)==0);
 assert(gk81_stock_attach(NULL,bad,GK77_FRESH_MONGOL_WORKRAM,GK77_FRESH_MONGOL_WORKRAM,source,path,sizeof path)==GK81_ATTACH_BAD_SOURCE);
 assert(memcmp(source,baseline,sizeof *source)==0&&strcmp(path,preserved)==0);
 assert(remove(bad)==0);
 if(argc==3){ /* optional authenticated PRIVATE fixture never distributed */
  Gk77Profile p=gk77_profile_parse(argv[1]);assert(p!=0);
  assert(gk81_stock_attach(NULL,argv[2],p,p,source,path,sizeof path)==GK81_ATTACH_OK);
  assert(source->country_count==(p==GK77_ARCHIVED_WORLD_SAVERAM?27u:14u));
  assert(strcmp(path,argv[2])==0);
 }
 free(baseline);free(source);
 puts("EXEC0081 PASS: root isolation, 3 independent profiles, wrong override, missing and forged file rollback; optional private original SHA positive");
 return 0;
}
