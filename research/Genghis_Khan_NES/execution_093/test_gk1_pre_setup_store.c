#include "gk1_pre_setup_store.h"
#include "gk1_source_identity.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
static unsigned checks;
static uint32_t checksum(const uint8_t *p,size_t n){
 uint32_t c=UINT32_C(0xffffffff);
 for(size_t i=0;i<n;i++){
  c^=p[i];for(int j=0;j<8;j++)c=(c>>1)^((c&1u)?UINT32_C(0xedb88320):0u);
 }
 return ~c;
}
static void rechecksum(uint8_t *p){
 uint32_t c=checksum(p,GK51_PRE_SETUP_BYTES-4);
 for(int j=0;j<4;j++)p[GK51_PRE_SETUP_BYTES-4u+(unsigned)j]=(uint8_t)(c>>(8*j));
}
#define CK(x) do{++checks;if(!(x)){fprintf(stderr,"FAIL %s:%d: %s\n",__FILE__,__LINE__,#x);return 1;}}while(0)
int main(int argc,char **argv){
 if(argc!=2)return 2;
 CK(gk1_verified_original_csv(argv[1]));
 Gk1TemplateDataset *ds=malloc(sizeof *ds);Gk1App *a=malloc(sizeof *a),*old=malloc(sizeof *old),*b=malloc(sizeof *b);
 if(!ds||!a||!old||!b)return 3;
 CK(gk1_template_load(argv[1],ds)==GK1_LOAD_OK);
 gk1_app_init(a);CK(gk1_app_install_templates(a,ds));
 a->worlds[0].countries[0].gold=777;
 a->worlds[1].countries[26].gold=888;
 a->worlds[0].countries[1].goods[10]=314;
 a->active_world=1;a->country_cursor[0]=5;a->country_cursor[1]=26;
 uint8_t data[GK51_PRE_SETUP_BYTES],corrupt[GK51_PRE_SETUP_BYTES];
 CK(gk51_pre_setup_encode(a,ds,data,sizeof data)==GK1_STORE_OK);
 CK(memcmp(data,"GK1PRE51",8)==0);CK(sizeof data==2594);
 CK(gk51_pre_setup_encode(a,ds,data,sizeof data-1)==GK1_STORE_BAD);
 memcpy(old,a,sizeof *a);
 CK(gk51_pre_setup_decode(data,sizeof data,ds,a)==GK1_STORE_OK);
 CK(a->screen==GK1_SCREEN_SCENARIO_COUNTRIES);CK(a->active_world==1);
 CK(a->worlds[0].countries[0].gold==777);CK(a->worlds[0].countries[1].goods[10]==314);
 CK(a->worlds[1].countries[26].gold==888);
 CK(!strcmp(a->worlds[1].countries[26].country_name,"Dai-Viet & Ta-Li"));
 CK(a->country_cursor[0]==5&&a->country_cursor[1]==26);
 CK(a->worlds[1].phase==GK1_PRE_SETUP_TEMPLATE);
 *b=*a;
 for(unsigned i=0;i<sizeof data;i++){
  memcpy(corrupt,data,sizeof data);corrupt[i]^=0x80;
  CK(gk51_pre_setup_decode(corrupt,sizeof corrupt,ds,b)==GK1_STORE_CORRUPT);
  CK(memcmp(a,b,sizeof *a)==0);
 }
 memcpy(corrupt,data,sizeof data);
 corrupt[13]=2;CK(gk51_pre_setup_decode(corrupt,sizeof corrupt,ds,b)==GK1_STORE_CORRUPT);
 /* A forged attacker CAN recompute CRC. Validate semantic stage, cursor,
    historic identity and 41-country economic invariants independently. */
 memcpy(corrupt,data,sizeof data);corrupt[10]=2;rechecksum(corrupt);
 CK(gk51_pre_setup_decode(corrupt,sizeof corrupt,ds,b)==GK1_STORE_CORRUPT);
 memcpy(corrupt,data,sizeof data);corrupt[14]=14;rechecksum(corrupt);
 CK(gk51_pre_setup_decode(corrupt,sizeof corrupt,ds,b)==GK1_STORE_CORRUPT);
 memcpy(corrupt,data,sizeof data);
 /* Country0 tax is at per-country 27th field: 48 + (11+7+4+2)*2 */
 corrupt[48u+24u*2u]=101u;corrupt[48u+24u*2u+1u]=0;rechecksum(corrupt);
 CK(gk51_pre_setup_decode(corrupt,sizeof corrupt,ds,b)==GK1_STORE_CORRUPT);
 memcpy(corrupt,data,sizeof data);
 /* First country's drafted total at index 28, must equal five workers. */
 corrupt[48u+28u*2u]^=1u;rechecksum(corrupt);
 CK(gk51_pre_setup_decode(corrupt,sizeof corrupt,ds,b)==GK1_STORE_CORRUPT);
 CK(memcmp(a,b,sizeof *a)==0);
 CK(gk51_pre_setup_decode(data,sizeof data-1,ds,b)==GK1_STORE_BAD);
 CK(gk51_pre_setup_decode(data,sizeof data,NULL,b)==GK1_STORE_BAD);
 CK(memcmp(a,b,sizeof *a)==0);
 /* Hostile in-memory economic mutation cannot escape pre-setup invariants. */
 a->worlds[0].countries[0].tax=101;
 CK(gk51_pre_setup_encode(a,ds,corrupt,sizeof corrupt)==GK1_STORE_BAD);
 a->worlds[0].countries[0].tax=old->worlds[0].countries[0].tax;
 a->worlds[0].countries[0].gold=777;
 a->worlds[0].countries[0].country_id=99;
 CK(gk51_pre_setup_encode(a,ds,corrupt,sizeof corrupt)==GK1_STORE_BAD);
 a->worlds[0].countries[0].country_id=1;
 CK(gk51_pre_setup_encode(a,ds,corrupt,sizeof corrupt)==GK1_STORE_OK);
 const char *file="gk51_temp_test_slot.gkp";
 remove(file);
 CK(gk51_pre_setup_save_new(file,a,ds)==GK1_STORE_OK);
 CK(gk51_pre_setup_save_new(file,a,ds)==GK1_STORE_EXISTS);
 FILE *f=fopen(file,"rb");CK(f!=NULL);
 CK(fseek(f,0,SEEK_END)==0);CK(ftell(f)==GK51_PRE_SETUP_BYTES);CK(fclose(f)==0);
 gk1_app_init(b);
 CK(gk51_pre_setup_load(file,ds,b)==GK1_STORE_OK);
 CK(b->worlds[0].countries[0].gold==777);
 CK(b->worlds[1].countries[26].gold==888);
 CK(b->worlds[1].country_count==27);
 CK(remove(file)==0);
 CK(gk51_pre_setup_load(file,ds,b)==GK1_STORE_IO);
 CK(b->worlds[0].countries[0].gold==777);
 /* Source mismatch checked against all source identities during decode. */
 ds->countries[14].country_id=55;
 CK(gk51_pre_setup_decode(data,sizeof data,ds,b)==GK1_STORE_CORRUPT);
 CK(b->worlds[0].countries[0].gold==777);
 printf("EXEC0051 PRE_SETUP format, 41 record dual-world state, %u hostile checks PASS\n",checks);
 free(b);free(old);free(a);free(ds);return 0;
}
