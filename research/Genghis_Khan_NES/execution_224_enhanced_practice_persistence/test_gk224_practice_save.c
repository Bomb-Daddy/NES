/* EXEC0224 source-relative Enhanced practice 3-slot cold-load and hostile QA. */
#include "gk224_practice_save.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"GK224 FAIL %s line %d\n",#x,__LINE__);return 1;}}while(0)
static int exercise(const Gk1LoadedScenario *src,const char *csv){
 Gk222Spring *original=calloc(1,sizeof *original),*loaded=calloc(1,sizeof *loaded);
 CHECK(original&&loaded);
 CHECK(gk222_open(src,0,original)==GK222_OK);
 CHECK(gk222_apply(original,0,GK222_PASS)==GK222_OK);
 CHECK(gk222_apply(original,0,GK222_ASSIGN_TOWN_TO_FOOD)==GK222_OK);
 original->selected_country=(uint8_t)(src->country_count-1u);
 uint8_t bytes[GK224_PRACTICE_BYTES]={0},edit[GK224_PRACTICE_BYTES],command=111;
 CHECK(gk224_encode(src,original,1,bytes)==GK224_OK);
 CHECK(gk224_decode(src,bytes,sizeof bytes,loaded,&command)==GK224_OK);
 CHECK(command==1 && memcmp(original,loaded,sizeof *original)==0);
 CHECK(gk224_decode(src,bytes,sizeof bytes-1,loaded,&command)==GK224_CORRUPT);
 CHECK(memcmp(original,loaded,sizeof *original)==0&&command==1);
 for(unsigned i=0;i<GK224_PRACTICE_BYTES;i++){
  memcpy(edit,bytes,sizeof edit);edit[i]^=1u;
  CHECK(gk224_decode(src,edit,sizeof edit,loaded,&command)!=GK224_OK);
  CHECK(command==1&&memcmp(original,loaded,sizeof *original)==0);
 }
 Gk1LoadedScenario *mutated=malloc(sizeof *mutated);CHECK(mutated);
 *mutated=*src;mutated->countries[2].gold++;
 CHECK(gk224_decode(mutated,bytes,sizeof bytes,loaded,&command)==GK224_SOURCE);
 CHECK(memcmp(original,loaded,sizeof *original)==0);
 original->working.countries[1].gold++; /* nonplayer tamper caught */
 CHECK(gk224_encode(src,original,1,edit)==GK224_STATE);
 original->working.countries[1].gold--;
 original->order_receipts[1]=9u;
 CHECK(gk224_encode(src,original,1,edit)==GK224_STATE);
 original->order_receipts[1]=1u;
 CHECK(gk224_encode(src,original,2,edit)==GK224_STATE);
 /* C11 exclusive create + real file-based load, independent source re-init. */
 char path[500];
#ifdef _WIN32
 const char *temp=getenv("TEMP");if(!temp||!*temp)temp=getenv("TMP");
 if(!temp||!*temp)temp=".";
#else
 const char *temp=getenv("TMPDIR");if(!temp||!*temp)temp="/tmp";
#endif
 int name_length=snprintf(path,sizeof path,"%s/gk224_practice_%lu_%u.gk24",temp,
   (unsigned long)(uintptr_t)original,(unsigned)src->scenario_id);
 CHECK(name_length>0&&(size_t)name_length<sizeof path);
 remove(path);
 CHECK(gk224_save_new(path,csv,src,original,1)==GK224_OK);
 CHECK(gk224_save_new(path,csv,src,original,1)==GK224_EXISTS);
 Gk1LoadedScenario *fresh=malloc(sizeof *fresh);CHECK(fresh);
 *fresh=*src; /* independently reinstated after a process restart */
 memset(loaded,0,sizeof *loaded);command=0;
 CHECK(gk224_load(path,csv,fresh,loaded,&command)==GK224_OK);
 CHECK(command==1&&memcmp(original,loaded,sizeof *original)==0);
 *mutated=*fresh;mutated->countries[0].town++;
 CHECK(gk224_load(path,csv,mutated,loaded,&command)==GK224_SOURCE);
 CHECK(memcmp(original,loaded,sizeof *original)==0);
 FILE *f=fopen(path,"ab");CHECK(f);CHECK(fputc(42,f)!=EOF);CHECK(fclose(f)==0);
 CHECK(gk224_load(path,csv,fresh,loaded,&command)==GK224_CORRUPT);
 CHECK(memcmp(original,loaded,sizeof *original)==0);
 remove(path);
 CHECK(gk224_load(path,csv,fresh,loaded,&command)==GK224_UNAVAILABLE);
 CHECK(memcmp(original,loaded,sizeof *original)==0);
 free(fresh);free(mutated);free(original);free(loaded);return 0;
}
int main(int argc,char **argv){
 CHECK(argc==2&&gk1_verified_original_csv(argv[1]));
 Gk1TemplateDataset *d=calloc(1,sizeof *d);
 Gk1LoadedScenario *m=calloc(1,sizeof *m),*w=calloc(1,sizeof *w);
 CHECK(d&&m&&w);
 CHECK(gk1_template_load(argv[1],d)==GK1_LOAD_OK);
 CHECK(gk1_open_loaded_scenario(d,GK1_MONGOL_CONQUEST,m)==GK1_LOAD_OK);
 CHECK(gk1_open_loaded_scenario(d,GK1_WORLD_CONQUEST,w)==GK1_LOAD_OK);
 CHECK(exercise(m,argv[1])==0&&exercise(w,argv[1])==0);
 free(d);free(m);free(w);
 puts("EXEC0224 Enhanced 14/27 scenarios, 80-byte receipts, 160-byte mutations, cold-load, reject/tamper/atomicity PASS; NOT stock NES");return 0;
}
