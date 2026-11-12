#if !defined(_WIN32)
#define _POSIX_C_SOURCE 200809L
#endif
#include "gk227_practice_title_preview.h"
#include "gk224_practice_save.h"
#include "gk1_source_identity.h"
#include <string.h>
#include <errno.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#endif
Gk227PreviewStatus gk227_preview_practice_slot(const char *path,
 const char *csv,const Gk1LoadedScenario sources[2],Gk227Preview *out){
 if(!path||!*path||!csv||!*csv||!sources||!out||strlen(path)>4096u)
  return GK227_PREVIEW_UNAVAILABLE;
 /* Refuse a replayed receipt against partial or modified installed data.
  * The canonical original CSV SHA is a source-identity gate, not RNG evidence. */
 if(!gk1_verified_original_csv(csv)||
    sources[0].scenario_id!=GK1_MONGOL_CONQUEST||sources[0].country_count!=14u||
    sources[1].scenario_id!=GK1_WORLD_CONQUEST||sources[1].country_count!=27u||
    sources[0].phase!=GK1_PRE_SETUP_TEMPLATE||
    sources[1].phase!=GK1_PRE_SETUP_TEMPLATE)return GK227_PREVIEW_SOURCE;
#ifdef _WIN32
 DWORD attrs=GetFileAttributesA(path);
 if(attrs==INVALID_FILE_ATTRIBUTES){
  DWORD last=GetLastError();
  return last==ERROR_FILE_NOT_FOUND||last==ERROR_PATH_NOT_FOUND?
   GK227_PREVIEW_MISSING:GK227_PREVIEW_UNAVAILABLE;
 }
 if(attrs&(FILE_ATTRIBUTE_REPARSE_POINT|FILE_ATTRIBUTE_DIRECTORY))
  return GK227_PREVIEW_UNAVAILABLE;
#else
 struct stat info;
 if(lstat(path,&info)!=0)
  return errno==ENOENT?GK227_PREVIEW_MISSING:GK227_PREVIEW_UNAVAILABLE;
 if(!S_ISREG(info.st_mode))return GK227_PREVIEW_UNAVAILABLE;
#endif
 Gk222Spring state={0};uint8_t command=0;
 int source_mismatch=0,corrupt=0;
 for(unsigned world=0;world<2u;world++){
  Gk224Status result=gk224_load(path,csv,&sources[world],&state,&command);
  if(result==GK224_OK){
   if(state.scenario!=world+1u||state.player_index>=sources[world].country_count||
      state.actions_committed>3u||state.remaining>3u||
      state.remaining+state.actions_committed!=3u)return GK227_PREVIEW_CORRUPT;
   Gk227Preview value={0};
   value.scenario=state.scenario;
   value.country_count=sources[world].country_count;
   value.player_index=state.player_index;
   value.actions_committed=state.actions_committed;
   value.orders_remaining=state.remaining;
   value.command=command;
   *out=value;
   return GK227_PREVIEW_OK;
  }
  if(result==GK224_SOURCE)source_mismatch=1;
  else if(result==GK224_CORRUPT||result==GK224_STATE)corrupt=1;
  else if(result==GK224_UNAVAILABLE)return GK227_PREVIEW_UNAVAILABLE;
 }
 return corrupt?GK227_PREVIEW_CORRUPT:source_mismatch?
  GK227_PREVIEW_SOURCE:GK227_PREVIEW_UNAVAILABLE;
}
