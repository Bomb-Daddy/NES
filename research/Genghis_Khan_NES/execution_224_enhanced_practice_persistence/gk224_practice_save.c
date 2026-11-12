#include "gk224_practice_save.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <string.h>

/* Nothing from the proprietary ROM/savestates is serialized. This digest
 * describes only the public CSV-derived source needed to replay a PORT drill.
 * Explicit little-endian fields avoid padding, alignment and Win32 issues. */
typedef struct {uint8_t bytes[32768];size_t used;int ok;} Gk224Writer;
static void put1(Gk224Writer *w,uint8_t v){
 if(w->used<sizeof w->bytes)w->bytes[w->used++]=v;else w->ok=0;
}
static void put2(Gk224Writer *w,uint16_t v){put1(w,(uint8_t)v);put1(w,(uint8_t)(v>>8));}
static void put4(Gk224Writer *w,uint32_t v){put2(w,(uint16_t)v);put2(w,(uint16_t)(v>>16));}
static void putbytes(Gk224Writer *w,const char *s,size_t n){
 for(size_t i=0;i<n;i++)put1(w,(uint8_t)s[i]);
}
static int source_digest(const Gk1LoadedScenario *source,uint8_t out[32]){
 if(!source||!out||source->phase!=GK1_PRE_SETUP_TEMPLATE||
    (source->scenario_id!=GK1_MONGOL_CONQUEST&&source->scenario_id!=GK1_WORLD_CONQUEST)||
    source->country_count!=(source->scenario_id==GK1_MONGOL_CONQUEST?14u:27u))return 0;
 Gk224Writer w={0};w.ok=1;
 put1(&w,(uint8_t)source->scenario_id);put1(&w,(uint8_t)source->phase);
 put1(&w,source->country_count);
 for(unsigned i=0;i<source->country_count;i++){
  const Gk1CountryTemplate *c=&source->countries[i];
  if(c->country_id!=i+1u||c->scenario_id!=(uint8_t)source->scenario_id)return 0;
  put1(&w,c->scenario_id);put1(&w,c->country_id);put1(&w,c->index);
  put1(&w,c->specialty_index);
  putbytes(&w,c->country_name,sizeof c->country_name);
  putbytes(&w,c->lord_name,sizeof c->lord_name);
  putbytes(&w,c->specialty_name,sizeof c->specialty_name);
  put4(&w,c->country_sram_base);put4(&w,c->country_rom_offset);
  put4(&w,c->specialty_sram_address);put4(&w,c->specialty_rom_offset);
  put4(&w,c->name_rom_offset);
  for(unsigned g=0;g<GK1_GOODS;g++)put2(&w,c->goods[g]);
  const uint16_t numeric[]={c->arms,c->skill,c->troops,c->economy,c->town,
   c->defense,c->masons,c->commodity_history,c->artisans,c->food_history,
   c->food_workers,c->gold,c->morale,c->tax,c->extra_tax_used,
   c->undrafted_reserve,c->eligibility_percent,c->drafted_population,
   c->combined_population,c->eligible_draft_count};
  for(unsigned j=0;j<sizeof numeric/sizeof *numeric;j++)put2(&w,numeric[j]);
 }
 if(!w.ok)return 0;
 gk1_sha256_bytes(w.bytes,w.used,out);return 1;
}
static const uint8_t magic[8]={'G','K','1','P','2','2','4','!'};
Gk224Status gk224_encode(const Gk1LoadedScenario *source,
 const Gk222Spring *state,uint8_t command,uint8_t out[GK224_PRACTICE_BYTES]){
 if(!source||!state||!out||command>1u)return GK224_STATE;
 uint8_t digest[32];if(!source_digest(source,digest))return GK224_SOURCE;
 if(gk222_verify_receipts(state)!=GK222_OK||
    state->scenario!=(uint8_t)source->scenario_id||
    state->selected_country>=source->country_count)return GK224_STATE;
 Gk222Spring expected={0};
 if(gk222_open(source,state->player_index,&expected)!=GK222_OK)return GK224_SOURCE;
 for(unsigned i=0;i<state->actions_committed;i++)
  if(gk222_apply(&expected,expected.player_index,
        (Gk222Action)state->order_receipts[i])!=GK222_OK)return GK224_STATE;
 expected.selected_country=state->selected_country;
 /* Source-relative replay catches edits to ANY field in the working world. */
 if(memcmp(&expected,state,sizeof expected)!=0)return GK224_STATE;
 uint8_t record[GK224_PRACTICE_BYTES]={0};
 memcpy(record,magic,sizeof magic);
 record[8]=state->scenario;record[9]=state->player_index;
 record[10]=state->actions_committed;
 memcpy(record+11,state->order_receipts,3u);
 record[14]=state->selected_country;record[15]=command;
 memcpy(record+16,digest,32u);
 gk1_sha256_bytes(record,48u,record+48);
 memcpy(out,record,sizeof record);return GK224_OK;
}
Gk224Status gk224_decode(const Gk1LoadedScenario *source,
 const uint8_t *bytes,size_t length,Gk222Spring *out,uint8_t *command){
 if(!out||!command||!bytes||length!=GK224_PRACTICE_BYTES)return GK224_CORRUPT;
 if(memcmp(bytes,magic,sizeof magic)!=0||bytes[15]>1u)return GK224_CORRUPT;
 uint8_t digest[32],proof[32];
 gk1_sha256_bytes(bytes,48u,proof);
 if(memcmp(proof,bytes+48,32u)!=0)return GK224_CORRUPT;
 if(!source_digest(source,digest))return GK224_SOURCE;
 if(memcmp(digest,bytes+16,32u)!=0)return GK224_SOURCE;
 if(bytes[8]!=(uint8_t)source->scenario_id||bytes[9]>=source->country_count||
    bytes[10]>3u||bytes[14]>=source->country_count)return GK224_CORRUPT;
 for(unsigned i=bytes[10];i<3u;i++)if(bytes[11+i]!=0u)return GK224_CORRUPT;
 Gk222Spring next={0};
 if(gk222_open(source,bytes[9],&next)!=GK222_OK)return GK224_SOURCE;
 for(unsigned i=0;i<bytes[10];i++){
  if(bytes[11+i]>GK222_ASSIGN_TOWN_TO_FOOD||
     gk222_apply(&next,next.player_index,(Gk222Action)bytes[11+i])!=GK222_OK)
    return GK224_CORRUPT;
 }
 next.selected_country=bytes[14];
 if(gk222_verify_receipts(&next)!=GK222_OK)return GK224_CORRUPT;
 *out=next;*command=bytes[15];return GK224_OK;
}
Gk224Status gk224_save_new(const char *path,const char *csv,
 const Gk1LoadedScenario *source,const Gk222Spring *state,uint8_t command){
 if(!path||!*path||!csv||!gk1_verified_original_csv(csv))return GK224_SOURCE;
 uint8_t bytes[GK224_PRACTICE_BYTES];
 Gk224Status r=gk224_encode(source,state,command,bytes);
 if(r!=GK224_OK)return r;
 /* C11 exclusive creation; never truncate user saves and never use CWD fallback. */
 FILE *f=fopen(path,"wbx");
 if(!f){FILE *check=fopen(path,"rb");if(check){fclose(check);return GK224_EXISTS;}
   return GK224_UNAVAILABLE;}
 int ok=(fwrite(bytes,1,sizeof bytes,f)==sizeof bytes);
 if(fflush(f)!=0)ok=0;
 if(fclose(f)!=0)ok=0;
 if(!ok){remove(path);return GK224_UNAVAILABLE;}
 return GK224_OK;
}
Gk224Status gk224_load(const char *path,const char *csv,
 const Gk1LoadedScenario *source,Gk222Spring *out,uint8_t *command){
 if(!path||!*path||!out||!command)return GK224_UNAVAILABLE;
 if(!csv||!gk1_verified_original_csv(csv))return GK224_SOURCE;
 FILE *f=fopen(path,"rb");if(!f)return GK224_UNAVAILABLE;
 uint8_t bytes[GK224_PRACTICE_BYTES];
 size_t n=fread(bytes,1,sizeof bytes,f);
 int trailing=fgetc(f),error=ferror(f),close_error=fclose(f);
 if(n!=sizeof bytes||trailing!=EOF||error||close_error)return GK224_CORRUPT;
 return gk224_decode(source,bytes,sizeof bytes,out,command);
}
