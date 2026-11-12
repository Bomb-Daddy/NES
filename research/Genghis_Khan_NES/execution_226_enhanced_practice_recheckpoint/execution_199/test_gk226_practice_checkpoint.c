#if !defined(_WIN32)
#define _POSIX_C_SOURCE 200809L
#endif
#include "gk226_practice_checkpoint.h"
#include "gk1_app_state.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#define CHECK(x) do{if(!(x)){fprintf(stderr,"GK226 check failed line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static int exercise(const Gk1TemplateDataset *dataset,const char *csv,unsigned world){
 Gk1App *app=calloc(1,sizeof *app);
 Gk222Spring *current=calloc(1,sizeof *current),*loaded=calloc(1,sizeof *loaded);
 CHECK(app&&current&&loaded);
 gk1_app_init(app);CHECK(gk1_app_install_templates(app,dataset));
 const Gk1LoadedScenario *source=&app->worlds[world];
 uint8_t player=(uint8_t)(world?6u:0u);
 CHECK(gk222_open(source,player,current)==GK222_OK);
 char path[600];
#ifdef _WIN32
 const char *dir=getenv("TEMP");if(!dir||!*dir)dir=".";
 unsigned long pid=GetCurrentProcessId();
#else
 const char *dir=getenv("TMPDIR");if(!dir||!*dir)dir="/tmp";
 unsigned long pid=(unsigned long)getpid();
#endif
 CHECK(snprintf(path,sizeof path,"%s/gk226_%lu_%u.gk24",dir,pid,world)>0);
 remove(path);
 CHECK(gk226_practice_checkpoint(path,csv,source,current,0)==GK224_OK);
 uint8_t original[80],after[80];
 FILE *f=fopen(path,"rb");CHECK(f&&fread(original,1,80,f)==80&&fclose(f)==0);
 CHECK(gk222_apply(current,player,GK222_PASS)==GK222_OK);
 CHECK(gk226_practice_checkpoint(path,csv,source,current,0)==GK224_OK);
 f=fopen(path,"rb");CHECK(f&&fread(after,1,80,f)==80&&fclose(f)==0);
 CHECK(memcmp(original,after,80)!=0);
 uint8_t command=255;
 CHECK(gk224_load(path,csv,source,loaded,&command)==GK224_OK);
 CHECK(command==0&&memcmp(current,loaded,sizeof *current)==0);
 CHECK(gk222_apply(current,player,GK222_ASSIGN_TOWN_TO_FOOD)==GK222_OK);
 CHECK(gk226_practice_checkpoint(path,csv,source,current,1)==GK224_OK);
 CHECK(gk224_load(path,csv,source,loaded,&command)==GK224_OK);
 CHECK(command==1&&memcmp(current,loaded,sizeof *current)==0);
 f=fopen(path,"rb");CHECK(f&&fread(after,1,80,f)==80&&fclose(f)==0);
 /* A counterfeit in-memory state, source mismatch, or corrupt older receipt
  * must never destroy the last valid slot. */
 Gk222Spring *tampered=malloc(sizeof *tampered);CHECK(tampered);
 *tampered=*current;tampered->working.countries[world?24:11].gold++;
 CHECK(gk226_practice_checkpoint(path,csv,source,tampered,0)==GK224_STATE);
 f=fopen(path,"rb");CHECK(f&&fread(original,1,80,f)==80&&fclose(f)==0);
 CHECK(memcmp(original,after,80)==0);
 uint8_t saved=after[17];
 f=fopen(path,"r+b");CHECK(f&&fseek(f,17,SEEK_SET)==0&&fputc(saved^1u,f)!=EOF&&fclose(f)==0);
 CHECK(gk226_practice_checkpoint(path,csv,source,current,0)==GK224_CORRUPT);
 f=fopen(path,"rb");CHECK(f&&fread(original,1,80,f)==80&&fclose(f)==0);
 CHECK(original[17]==(uint8_t)(saved^1u));
 CHECK(remove(path)==0);
#ifndef _WIN32
 char target[640];CHECK(snprintf(target,sizeof target,"%s.target",path)>0);
 FILE *tf=fopen(target,"wb");CHECK(tf&&fwrite(after,1,80,tf)==80&&fclose(tf)==0);
 CHECK(symlink(target,path)==0);
 CHECK(gk226_practice_checkpoint(path,csv,source,current,0)==GK224_UNAVAILABLE);
 CHECK(unlink(path)==0);
 f=fopen(target,"rb");CHECK(f&&fread(original,1,80,f)==80&&fclose(f)==0);
 CHECK(memcmp(original,after,80)==0);CHECK(remove(target)==0);
#endif
 CHECK(gk226_practice_checkpoint(path,csv,source,current,0)==GK224_OK);
 CHECK(gk224_load(path,csv,source,loaded,&command)==GK224_OK);
 CHECK(memcmp(current,loaded,sizeof *current)==0);
 CHECK(remove(path)==0);
 free(tampered);free(current);free(loaded);free(app);return 0;
}
int main(int argc,char **argv){
 CHECK(argc==2&&gk1_verified_original_csv(argv[1]));
 Gk1TemplateDataset *d=calloc(1,sizeof *d);CHECK(d);
 CHECK(gk1_template_load(argv[1],d)==GK1_LOAD_OK);
 CHECK(exercise(d,argv[1],0u)==0);
 CHECK(exercise(d,argv[1],1u)==0);
 free(d);
 puts("EXEC0226 Linux C11 14/27 Enhanced disk-save advance and replay, rollback/corruption/symlink hostile QA PASS; no Classic or NES first Spring");
 return 0;
}
