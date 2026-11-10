#include "gk171_enhanced_title_resume.h"
#include "gk167_private_cold_save.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>

static void scrub(void *p,size_t n){
 volatile unsigned char *q=(volatile unsigned char*)p;
 while(n--)*q++=0;
}
static int exact_path(const char *s){return s&&*s;}
static int valid_paths(const Gk171Paths *paths){
 return paths&&exact_path(paths->source_gk17)&&exact_path(paths->canonical_csv);
}
Gk171Result gk171_resume_stage(const Gk171Paths *paths,unsigned stage,
 Gk1App *live){
 if(!live||!valid_paths(paths)||stage<1u||stage>GK171_STAGES||
    !exact_path(paths->receipt[stage-1u]))return GK171_BAD_INPUT;
 /* Keep the owner-installed templates and current live application intact on
  * every error; no original licensed country bytes leave owner-private files. */
 Gk1App *candidate=malloc(sizeof *candidate);
 if(!candidate)return GK171_MEMORY;
 *candidate=*live;
 Gk171Result result=GK171_SOURCE_REFUSED;
 if(gk167_cold_load_private(paths->source_gk17,paths->canonical_csv,
     candidate)!=GK167_OK)goto done;
 if(!candidate->stock_checkpoint.source_verified ||
    candidate->stock_checkpoint.orders.remaining!=3u ||
    candidate->screen!=GK1_SCREEN_STOCK_CONTINUE)goto done;
 result=GK171_RECEIPT_REFUSED;
 candidate->rules_profile=1u;
 switch(stage){
 case 1u:
  if(gk116_cold_load(paths->receipt[0],&candidate->stock_checkpoint,1u,
       &candidate->enhanced_assign)!=GK116_OK)goto done;
  candidate->screen=GK1_SCREEN_ENHANCED_ASSIGN;break;
 case 2u:
  if(gk158_cold_load(paths->receipt[1],&candidate->stock_checkpoint,1u,
       &candidate->enhanced_assign,&candidate->enhanced_summer)!=GK158_OK)goto done;
  candidate->screen=GK1_SCREEN_ENHANCED_SUMMER;break;
 case 3u:
  if(gk159_cold_load(paths->receipt[2],&candidate->stock_checkpoint,1u,
       &candidate->enhanced_assign,&candidate->enhanced_summer,
       &candidate->enhanced_fall)!=GK159_OK)goto done;
  candidate->screen=GK1_SCREEN_ENHANCED_FALL;break;
 case 4u:
  if(gk160_cold_load(paths->receipt[3],&candidate->stock_checkpoint,1u,
       &candidate->enhanced_assign,&candidate->enhanced_summer,
       &candidate->enhanced_fall,&candidate->enhanced_winter)!=GK160_OK)goto done;
  candidate->screen=GK1_SCREEN_ENHANCED_WINTER;break;
 case 5u:
  if(gk161_cold_load(paths->receipt[4],&candidate->stock_checkpoint,1u,
       &candidate->enhanced_assign,&candidate->enhanced_summer,
       &candidate->enhanced_fall,&candidate->enhanced_winter,
       &candidate->enhanced_year2)!=GK161_OK)goto done;
  candidate->screen=GK1_SCREEN_ENHANCED_YEAR2;break;
 default: goto done;
 }
 if(!gk116_attestation_intact(&candidate->enhanced_assign,
      &candidate->stock_checkpoint))goto done;
 /* Only the distinct Enhanced mutable world changes. The immutable source
  * remains authenticated and the receipt replays its exact action sequence. */
 candidate->stock_checkpoint.pending_pass=0u;
 candidate->assign_from=0u;candidate->assign_to=4u;
 candidate->assign_quantity=1u;
 snprintf(candidate->status,sizeof candidate->status,
   "ENHANCED STAGE %u COLD RESUMED FROM PRIVATE ORIGINAL: NOT NES SEASON PARITY",
   stage);
 *live=*candidate;
 result=GK171_OK;
done:
 scrub(candidate,sizeof *candidate);free(candidate);
 return result;
}
Gk171Result gk171_resume_latest(const Gk171Paths *paths,Gk1App *live,
 unsigned *restored_stage){
 if(!live||!valid_paths(paths)||!restored_stage)return GK171_BAD_INPUT;
 /* Probe *existence* without reading content. If latest existing is
  * inaccessible/corrupted, refuse rather than rolling back silently. */
 for(unsigned stage=GK171_STAGES;stage>=1u;stage--){
  const char *path=paths->receipt[stage-1u];
  if(!exact_path(path))return GK171_BAD_INPUT;
  struct stat st;
  if(stat(path,&st)==0){
   Gk171Result r=gk171_resume_stage(paths,stage,live);
   if(r==GK171_OK)*restored_stage=stage;
   return r;
  }
  if(errno!=ENOENT)return GK171_RECEIPT_REFUSED;
 }
 return GK171_NO_RECEIPT;
}
