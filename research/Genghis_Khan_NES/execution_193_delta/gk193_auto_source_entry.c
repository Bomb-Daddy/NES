#include "gk193_auto_source_entry.h"
#include "gk164_mss_entry.h"
#include "gk166_fresh_entry.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Gk193Result gk193_enter_original_auto(Gk1App *app, const char *csv,
 const char *source, uint8_t player, Gk77Profile *detected){
 if(!app||!csv||!*csv||!source||!*source||!detected||player!=0u)
  return GK193_ARGUMENT;
 /* A private stock snapshot must never be silently imported into Enhanced.
  * All three evidence profiles retain the original canonical template gate
  * within the delegated source-authenticated original importers. */
 if(app->rules_profile!=0u)return GK193_TEMPLATE;
 FILE *fp=fopen(source,"rb");
 if(!fp)return GK193_UNAVAILABLE;
 uint8_t signature[4]={0};
 size_t n=fread(signature,1,sizeof signature,fp);
 int read_error=ferror(fp),close_error=fclose(fp);
 if(read_error||close_error)return GK193_UNAVAILABLE;
 if(n!=sizeof signature)return GK193_SOURCE;
 const int mesen=memcmp(signature,"MSS\1",4)==0;
 /* The ONLY other supported evidence is fresh original Mesen JSON.
  * Neither a ROM nor a renamed arbitrary file is a recognized snapshot. */
 if(!mesen&&signature[0]!='{')return GK193_SOURCE;
 Gk1App *candidate=(Gk1App*)malloc(sizeof *candidate);
 if(!candidate)return GK193_UNAVAILABLE;
 Gk193Result result=GK193_SOURCE;
 Gk77Profile chosen=GK77_FRESH_MONGOL_WORKRAM;
 if(mesen){
  const Gk77Profile profiles[2]={
   GK77_ARCHIVED_MONGOL_SAVERAM,GK77_ARCHIVED_WORLD_SAVERAM};
  for(unsigned i=0;i<2;i++){
   memcpy(candidate,app,sizeof *candidate);
   Gk164Result r=gk164_enter_original_mss(candidate,csv,source,profiles[i],player);
   if(r==GK164_OK){chosen=profiles[i];result=GK193_OK;break;}
   if(r==GK164_TEMPLATE){result=GK193_TEMPLATE;break;}
   if(r==GK164_SOURCE_UNAVAILABLE){result=GK193_UNAVAILABLE;break;}
  }
 }else{
  memcpy(candidate,app,sizeof *candidate);
  Gk166Result r=gk166_enter_original_fresh_json(candidate,csv,source,player);
  if(r==GK166_OK)result=GK193_OK;
  else if(r==GK166_TEMPLATE)result=GK193_TEMPLATE;
  else if(r==GK166_SOURCE_UNAVAILABLE)result=GK193_UNAVAILABLE;
 }
 if(result==GK193_OK){
  /* This is the only caller-visible world mutation. Both pristine original
   * scenario templates and every inherited application subsystem survive. */
  memcpy(app,candidate,sizeof *app);
  *detected=chosen;
 }
 memset(candidate,0,sizeof *candidate);free(candidate);
 return result;
}
