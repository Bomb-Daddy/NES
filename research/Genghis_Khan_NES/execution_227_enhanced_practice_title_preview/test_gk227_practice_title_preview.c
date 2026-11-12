#if !defined(_WIN32)
#define _POSIX_C_SOURCE 200809L
#endif
#include "gk227_practice_title_preview.h"
#include "gk226_practice_checkpoint.h"
#include "gk224_practice_save.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#define CHECK(x) do{if(!(x)){fprintf(stderr,"GK227 FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static int trial(const char *csv,const Gk1TemplateDataset *dataset,unsigned world){
 Gk1App *app=calloc(1,sizeof *app);CHECK(app);
 gk1_app_init(app);CHECK(gk1_app_install_templates(app,dataset));
 Gk222Spring *practice=calloc(1,sizeof *practice);CHECK(practice);
 uint8_t player=(uint8_t)(world?6:1);
 CHECK(gk222_open(&app->worlds[world],player,practice)==GK222_OK);
 char file[640];
#ifdef _WIN32
 const char *dir=getenv("TEMP");if(!dir||!*dir)dir=".";
 unsigned long pid=GetCurrentProcessId();
 const char slash='\\';
#else
 const char *dir=getenv("TMPDIR");if(!dir||!*dir)dir="/tmp";
 unsigned long pid=(unsigned long)getpid();
 const char slash='/';
#endif
 CHECK(snprintf(file,sizeof file,"%s%cgk227_%lu_%u.gk24",dir,slash,pid,world)>0);
 remove(file);
 Gk227Preview sentinel={0xEF,0xEF,0xEF,0xEF,0xEF,0xEF};
 Gk227Preview value=sentinel;
 CHECK(gk227_preview_practice_slot(file,csv,app->worlds,&value)==GK227_PREVIEW_MISSING);
 CHECK(memcmp(&value,&sentinel,sizeof value)==0);
 CHECK(gk226_practice_checkpoint(file,csv,&app->worlds[world],practice,1)==GK224_OK);
 CHECK(gk227_preview_practice_slot(file,csv,app->worlds,&value)==GK227_PREVIEW_OK);
 CHECK(value.scenario==world+1u&&value.country_count==(world?27:14)&&
       value.player_index==player&&value.actions_committed==0&&
       value.orders_remaining==3&&value.command==1);
 CHECK(gk222_apply(practice,player,GK222_ASSIGN_TOWN_TO_FOOD)==GK222_OK);
 CHECK(gk222_apply(practice,player,GK222_PASS)==GK222_OK);
 CHECK(gk226_practice_checkpoint(file,csv,&app->worlds[world],practice,0)==GK224_OK);
 CHECK(gk227_preview_practice_slot(file,csv,app->worlds,&value)==GK227_PREVIEW_OK);
 CHECK(value.actions_committed==2&&value.orders_remaining==1&&value.command==0);
 value=sentinel;
 Gk1LoadedScenario *sources=malloc(2*sizeof *sources);CHECK(sources);
 memcpy(sources,app->worlds,2*sizeof *sources);
 sources[world].countries[5].gold++;
 CHECK(gk227_preview_practice_slot(file,csv,sources,&value)==GK227_PREVIEW_SOURCE);
 CHECK(memcmp(&value,&sentinel,sizeof value)==0);
 memcpy(sources,app->worlds,2*sizeof *sources);
 sources[0].country_count=13;
 CHECK(gk227_preview_practice_slot(file,csv,sources,&value)==GK227_PREVIEW_SOURCE);
 CHECK(memcmp(&value,&sentinel,sizeof value)==0);
 memcpy(sources,app->worlds,2*sizeof *sources);
 CHECK(gk227_preview_practice_slot(file,"/definitely/nonexistent.csv",sources,&value)==GK227_PREVIEW_SOURCE);
 CHECK(memcmp(&value,&sentinel,sizeof value)==0);
 FILE *f=fopen(file,"r+b");CHECK(f);
 CHECK(fseek(f,15,SEEK_SET)==0);int original=fgetc(f);CHECK(original>=0);
 CHECK(fseek(f,15,SEEK_SET)==0&&fputc(original^1,f)!=EOF&&fclose(f)==0);
 CHECK(gk227_preview_practice_slot(file,csv,sources,&value)==GK227_PREVIEW_CORRUPT);
 CHECK(memcmp(&value,&sentinel,sizeof value)==0);
 CHECK(remove(file)==0);
 f=fopen(file,"wb");CHECK(f&&fwrite("bad",1,3,f)==3&&fclose(f)==0);
 CHECK(gk227_preview_practice_slot(file,csv,sources,&value)==GK227_PREVIEW_CORRUPT);
 CHECK(memcmp(&value,&sentinel,sizeof value)==0);
 CHECK(remove(file)==0);
#ifndef _WIN32
 char target[700];CHECK(snprintf(target,sizeof target,"%s.target",file)>0);
 f=fopen(target,"wb");CHECK(f&&fwrite("bad",1,3,f)==3&&fclose(f)==0);
 CHECK(symlink(target,file)==0);
 CHECK(gk227_preview_practice_slot(file,csv,sources,&value)==GK227_PREVIEW_UNAVAILABLE);
 CHECK(memcmp(&value,&sentinel,sizeof value)==0);
 CHECK(unlink(file)==0&&remove(target)==0);
#endif
 free(sources);free(practice);free(app);return 0;
}
int main(int argc,char **argv){
 CHECK(argc==2&&gk1_verified_original_csv(argv[1]));
 Gk1TemplateDataset *dataset=calloc(1,sizeof *dataset);CHECK(dataset);
 CHECK(gk1_template_load(argv[1],dataset)==GK1_LOAD_OK);
 CHECK(trial(argv[1],dataset,0)==0&&trial(argv[1],dataset,1)==0);
 free(dataset);
 puts("EXEC0227 SOURCE-BOUND 14/27 Enhanced title slot preview and corruption/partial/symlink nonmutation PASS; no NES Spring generated");return 0;
}
