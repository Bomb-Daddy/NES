#include "gk193_auto_source_entry.h"
#include "gk164_mss_entry.h"
#include "gk166_fresh_entry.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* EXEC0274: probe the *format*, never the trust of a stock checkpoint.
 * A fresh Mesen JSON export can legitimately begin with UTF-8 BOM and/or
 * JSON whitespace. The previous four-byte dispatch refused such files
 * before the existing exact raw WorkRAM SHA and independent source gates.
 * Only byte-zero MSS\1 is binary Mesen; the JSON parser remains the sole
 * authority on the source and rejects all forged/modified country bytes.
 * Bounded scanning prevents an unbounded whitespace prefix. */
static int gk193_probe(FILE *fp,int *mesen){
 unsigned char first[4];
 if(!fp||!mesen)return 0;
 size_t n=fread(first,1,sizeof first,fp);
 if(n!=sizeof first)return 0;
 if(memcmp(first,"MSS\1",sizeof first)==0){*mesen=1;return 1;}
 if(fseek(fp,0L,SEEK_SET)!=0)return 0;
 int c=fgetc(fp);
 if(c==0xEF){
  if(fgetc(fp)!=0xBB||fgetc(fp)!=0xBF)return 0;
  c=fgetc(fp);
 }
 /* gk165's original fresh JSON input limit is 256 KiB. No wider preamble
  * is admissible; accepted JSON still undergoes its original full parser. */
 for(unsigned i=0;i<256u*1024u;i++){
  if(c=='{'){*mesen=0;return 1;}
  if(c==EOF||!isspace((unsigned char)c))return 0;
  c=fgetc(fp);
 }
 return 0;
}

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
 int mesen=0;
 const int recognized=gk193_probe(fp,&mesen);
 int read_error=ferror(fp),close_error=fclose(fp);
 if(read_error||close_error)return GK193_UNAVAILABLE;
 /* Recognition is not authentication. The delegated source importer
  * independently verifies format, bank, exact WorkRAM SHA, and template. */
 if(!recognized)return GK193_SOURCE;
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
