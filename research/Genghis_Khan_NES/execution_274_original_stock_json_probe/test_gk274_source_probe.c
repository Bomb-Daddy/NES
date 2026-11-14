/* EXEC0274 production-linked regression. Never embeds original ROM or RAM. */
#include "gk193_auto_source_entry.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef NDEBUG
#undef NDEBUG
#endif
static unsigned checks;
#define CK(x) do{if(!(x)){fprintf(stderr,"GK274 FAILED line %d: %s\n",__LINE__,#x);exit(3);}checks++;}while(0)
static Gk1App *app,*saved;
static void run_case(const char *csv,const unsigned char *data,size_t n,
                    Gk193Result expected){
 static unsigned serial;
 char path[48];
 snprintf(path,sizeof path,"gk274_probe_%u.tmp",serial++);
 FILE *fp=fopen(path,"wb");CK(fp!=NULL);
 CK(fwrite(data,1,n,fp)==n);CK(fclose(fp)==0);
 Gk77Profile profile=GK77_ARCHIVED_WORLD_SAVERAM;
 CK(gk193_enter_original_auto(app,csv,path,0,&profile)==expected);
 CK(profile==GK77_ARCHIVED_WORLD_SAVERAM);
 CK(memcmp(saved,app,sizeof *app)==0);
 CK(remove(path)==0);
}
int main(int argc,char **argv){
 CK(argc==2);
 app=calloc(1,sizeof *app);saved=malloc(sizeof *saved);
 Gk1TemplateDataset *source=malloc(sizeof *source);
 CK(app&&saved&&source);
 gk1_app_init(app);
 CK(gk1_template_load(argv[1],source)==GK1_LOAD_OK);
 CK(gk1_app_install_templates(app,source)==1);
 app->rules_profile=0u;
 memcpy(saved,app,sizeof *app);
 /* An invalid CSV forces an unambiguous delegation signal: FORMAT matched
  * means the trusted importer rejects the CSV as TEMPLATE before parsing
  * any purported source. The old four-byte dispatcher returned SOURCE. */
 const char *invalid_csv="/no-authenticated-csv/274";
 const unsigned char plain[]="{\"not_an_authentic_Mesen_record\":true}";
 const unsigned char spaced[]="  \t\r\n {\"not_an_authentic_Mesen_record\":true}";
 const unsigned char bom[]="\xEF\xBB\xBF{\"not_an_authentic_Mesen_record\":true}";
 const unsigned char bom_space[]="\xEF\xBB\xBF  \n{\"not_an_authentic_Mesen_record\":true}";
 const unsigned char fake_mss[]={ 'M','S','S',1,'X' };
 const unsigned char bad_mss[]={ ' ','M','S','S',1,'X' };
 const unsigned char bad_bom[]={ 0xef,0xbb,0, '{','}' };
 const unsigned char mid_bom[]={ ' ',0xef,0xbb,0xbf,'{','}' };
 const unsigned char short_text[]={ '{' };
 run_case(invalid_csv,plain,sizeof plain-1,GK193_TEMPLATE);
 run_case(invalid_csv,spaced,sizeof spaced-1,GK193_TEMPLATE);
 run_case(invalid_csv,bom,sizeof bom-1,GK193_TEMPLATE);
 run_case(invalid_csv,bom_space,sizeof bom_space-1,GK193_TEMPLATE);
 run_case(invalid_csv,fake_mss,sizeof fake_mss,GK193_TEMPLATE);
 run_case(invalid_csv,bad_mss,sizeof bad_mss,GK193_SOURCE);
 run_case(invalid_csv,bad_bom,sizeof bad_bom,GK193_SOURCE);
 run_case(invalid_csv,mid_bom,sizeof mid_bom,GK193_SOURCE);
 run_case(invalid_csv,short_text,sizeof short_text,GK193_SOURCE);
 run_case(argv[1],spaced,sizeof spaced-1,GK193_SOURCE); /* forged JSON rejected */
 run_case(argv[1],bom_space,sizeof bom_space-1,GK193_SOURCE);
 size_t huge_len=256u*1024u+5u;
 unsigned char *huge=malloc(huge_len);CK(huge!=NULL);
 memset(huge,' ',huge_len);huge[huge_len-1]='{';
 run_case(invalid_csv,huge,huge_len,GK193_SOURCE);
 free(huge);
 Gk77Profile profile=GK77_ARCHIVED_WORLD_SAVERAM;
 CK(gk193_enter_original_auto(app,argv[1],"no-274-source-file",0,&profile)==GK193_UNAVAILABLE);
 CK(memcmp(saved,app,sizeof *app)==0);
 CK(profile==GK77_ARCHIVED_WORLD_SAVERAM);
 printf("GK274 %u assertions PASS: native original-source JSON format dispatch, negative SHA/template, immutable app; no stock first-Spring RNG implemented\n",checks);
 free(source);free(saved);free(app);return 0;
}
