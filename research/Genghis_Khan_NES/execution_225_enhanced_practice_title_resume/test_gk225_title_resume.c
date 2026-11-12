#include "gk225_practice_resume.h"
#include "gk224_practice_save.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ASSERT(x) do{if(!(x)){fprintf(stderr,"GK225 FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static int case_world(const Gk1TemplateDataset *dataset,const char *csv,unsigned w){
 Gk1App *original=calloc(1,sizeof *original),*fresh=calloc(1,sizeof *fresh),
        *before=malloc(sizeof *before);
 Gk222Spring *spring=calloc(1,sizeof *spring);
 ASSERT(original&&fresh&&before&&spring);
 gk1_app_init(original);ASSERT(gk1_app_install_templates(original,dataset));
 uint8_t player=(uint8_t)(w?6u:0u);
 ASSERT(gk222_open(&original->worlds[w],player,spring)==GK222_OK);
 ASSERT(gk222_apply(spring,player,GK222_PASS)==GK222_OK);
 ASSERT(gk222_apply(spring,player,GK222_ASSIGN_TOWN_TO_FOOD)==GK222_OK);
 spring->selected_country=(uint8_t)(original->worlds[w].country_count-1u);
 char path[512];
#ifdef _WIN32
 const char *tmp=getenv("TEMP");if(!tmp||!*tmp)tmp=".";
 const char slash='\\';
#else
 const char *tmp=getenv("TMPDIR");if(!tmp||!*tmp)tmp="/tmp";
 const char slash='/';
#endif
 ASSERT(snprintf(path,sizeof path,"%s%cgk225_%lu_%u.gk24",tmp,slash,
   (unsigned long)(uintptr_t)original,w)>0);
 remove(path);
 ASSERT(gk224_save_new(path,csv,&original->worlds[w],spring,1)==GK224_OK);
 gk1_app_init(fresh);ASSERT(gk1_app_install_templates(fresh,dataset));
 Gk1LoadedScenario *world_copy=malloc(2*sizeof *world_copy);
 ASSERT(world_copy);memcpy(world_copy,fresh->worlds,2*sizeof *world_copy);
 ASSERT(gk225_resume_title(fresh,csv,path)==GK225_OK);
 ASSERT(fresh->screen==GK1_SCREEN_ENHANCED_SPRING_LAB);
 ASSERT(fresh->active_world==w&&fresh->rules_profile==1u);
 ASSERT(fresh->setup_stage[w]==GK53_SETUP_PLAYERS);
 ASSERT(fresh->country_cursor[w]==player&&fresh->port_spring_command==1u);
 ASSERT(memcmp(&fresh->port_spring,spring,sizeof *spring)==0);
 ASSERT(memcmp(fresh->worlds,world_copy,2*sizeof *world_copy)==0);
 ASSERT(gk1_app_input(fresh,GK1_INPUT_CANCEL));
 ASSERT(fresh->screen==GK1_SCREEN_SCENARIO_COUNTRIES);
 ASSERT(memcmp(fresh->worlds,world_copy,2*sizeof *world_copy)==0);
 /* A valid resumed model supports continued controller-committed orders. */
 fresh->screen=GK1_SCREEN_ENHANCED_SPRING_LAB;
 fresh->port_spring.selected_country=player;
 ASSERT(gk1_app_input(fresh,GK1_INPUT_CONFIRM));
 ASSERT(fresh->port_spring.remaining==0&&fresh->port_spring.ended);
 /* A different profile must never resume Enhanced practice. */
 gk1_app_init(fresh);ASSERT(gk1_app_install_templates(fresh,dataset));
 fresh->rules_profile=0;*before=*fresh;
 ASSERT(gk225_resume_title(fresh,csv,path)==GK225_WRONG_PHASE);
 ASSERT(memcmp(before,fresh,sizeof *before)==0);
 fresh->rules_profile=1;
 /* Source mismatch, including in a non-player province, rejects atomically. */
 fresh->worlds[w].countries[3].gold++;
 *before=*fresh;
 ASSERT(gk225_resume_title(fresh,csv,path)==GK225_SOURCE);
 ASSERT(memcmp(before,fresh,sizeof *before)==0);
 /* The fail-closed original game phase is never silently upgraded. */
 fresh->worlds[w].countries[3].gold--;
 fresh->worlds[w].phase=(Gk1ScenarioPhase)2;
 *before=*fresh;
 ASSERT(gk225_resume_title(fresh,csv,path)==GK225_WRONG_PHASE);
 ASSERT(memcmp(before,fresh,sizeof *before)==0);
 gk1_app_init(fresh);ASSERT(gk1_app_install_templates(fresh,dataset));
 *before=*fresh;
 FILE *f=fopen(path,"rb+");ASSERT(f);
 ASSERT(fseek(f,12,SEEK_SET)==0);int old=fgetc(f);ASSERT(old!=EOF);
 ASSERT(fseek(f,12,SEEK_SET)==0);ASSERT(fputc(old^1,f)!=EOF);ASSERT(fclose(f)==0);
 ASSERT(gk225_resume_title(fresh,csv,path)==GK225_CORRUPT);
 ASSERT(memcmp(before,fresh,sizeof *before)==0);
 remove(path);
 ASSERT(gk225_resume_title(fresh,csv,path)==GK225_MISSING);
 ASSERT(memcmp(before,fresh,sizeof *before)==0);
 free(world_copy);free(before);free(spring);free(fresh);free(original);
 return 0;
}
int main(int argc,char **argv){
 ASSERT(argc==2&&gk1_verified_original_csv(argv[1]));
 Gk1TemplateDataset *d=calloc(1,sizeof *d);ASSERT(d);
 ASSERT(gk1_template_load(argv[1],d)==GK1_LOAD_OK);
 ASSERT(case_world(d,argv[1],0u)==0);
 ASSERT(case_world(d,argv[1],1u)==0);
 free(d);
 puts("EXEC0225 14/27 Enhanced practice TITLE cold-resume, full source replay, Y controller semantics, profile/phase/corruption rollback PASS; original NES FIRST SPRING NOT GENERATED");return 0;
}
