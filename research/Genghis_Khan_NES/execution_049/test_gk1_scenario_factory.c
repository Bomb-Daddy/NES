#include "gk1_scenario_factory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
static unsigned checks=0;
#define CK(x) do{checks++;if(!(x)){fprintf(stderr,"EXEC0049 FAIL line%u: %s\n",(unsigned)__LINE__,#x);exit(1);}}while(0)
static char *read_all(const char *path,size_t *n){
    FILE *f=fopen(path,"rb");CK(f!=NULL);
    CK(fseek(f,0,SEEK_END)==0);long length=ftell(f);CK(length>0 && length<100000);
    CK(fseek(f,0,SEEK_SET)==0);
    char *b=malloc((size_t)length+2u);CK(b!=NULL);
    CK(fread(b,1,(size_t)length,f)==(size_t)length);
    CK(fclose(f)==0);b[length]='\0';*n=(size_t)length;return b;
}
static void expect_rejected(const char *text,size_t bytes){
    FILE *f=tmpfile();CK(f!=NULL);
    CK(fwrite(text,1,bytes,f)==bytes);CK(fflush(f)==0);rewind(f);
    Gk1TemplateDataset out;memset(&out,0xA5,sizeof out);
    Gk1TemplateDataset original=out;
    Gk1LoadStatus status=gk1_template_read(f,&out);
    CK(status!=GK1_LOAD_OK);
    CK(memcmp(&original,&out,sizeof out)==0);
    CK(fclose(f)==0);
}
static void check_all(const char *path){
    Gk1TemplateDataset d;memset(&d,0xA5,sizeof d);
    CK(gk1_template_load(path,&d)==GK1_LOAD_OK);
    CK(d.mongol_count==14 && d.world_count==27);
    CK(gk1_template_country(&d,GK1_MONGOL_CONQUEST,0)==NULL);
    CK(gk1_template_country(&d,GK1_MONGOL_CONQUEST,15)==NULL);
    CK(gk1_template_country(&d,GK1_WORLD_CONQUEST,28)==NULL);
    CK(gk1_template_country(&d,(Gk1ScenarioId)3,1)==NULL);
    CK(gk1_template_country(NULL,GK1_WORLD_CONQUEST,1)==NULL);
    const Gk1CountryTemplate *m=gk1_template_country(&d,GK1_MONGOL_CONQUEST,1);
    const Gk1CountryTemplate *w=gk1_template_country(&d,GK1_WORLD_CONQUEST,1);
    CK(m!=NULL && w!=NULL);
    CK(strcmp(m->country_name,"the Mongols")==0);
    CK(strcmp(m->lord_name,"Temujin")==0 && strcmp(w->lord_name,"Genghis Khan")==0);
    CK(m->gold==425 && m->goods[10]==129 && m->troops==60 && m->undrafted_reserve==32);
    CK(w->gold==120 && w->goods[10]==210 && w->troops==115 && w->undrafted_reserve==100);
    CK(m->drafted_population==100 && m->combined_population==132 && m->eligible_draft_count==9);
    CK(m->country_sram_base==0x6DCDu && m->country_rom_offset==0x1C9F7u);
    CK(m->specialty_sram_address==0x7692u && m->specialty_rom_offset==0x1D2BCu);
    CK(w->country_rom_offset==0x1E0A7u && w->specialty_rom_offset==0x1E96Cu);
    const Gk1CountryTemplate *baghdad=gk1_template_country(&d,GK1_WORLD_CONQUEST,22);
    CK(baghdad!=NULL && strcmp(baghdad->country_name,"Baghdad")==0);
    CK(baghdad->specialty_index==8 && strcmp(baghdad->specialty_name,"Textiles")==0);
    CK(baghdad->goods[9]>baghdad->goods[8]);/* Perfume > Textiles; specialty is explicit. */
    for(unsigned scenario=1;scenario<=2;scenario++){
        unsigned len=scenario==1?14:27;
        for(unsigned id=1;id<=len;id++){
            const Gk1CountryTemplate *c=gk1_template_country(&d,(Gk1ScenarioId)scenario,(uint8_t)id);
            CK(c!=NULL);CK(c->country_id==id && c->index==id-1u);
            CK(c->scenario_id==scenario);
            CK(c->specialty_index<10u);
            CK(c->drafted_population==(unsigned)c->troops+c->town+c->masons+c->artisans+c->food_workers);
            CK(c->combined_population==(unsigned)c->drafted_population+c->undrafted_reserve);
            CK(c->eligible_draft_count==(unsigned)c->undrafted_reserve*c->eligibility_percent/100u);
            CK(c->gold<=999 && c->goods[10]<=999);
        }
    }
    Gk1LoadedScenario first,second;
    memset(&first,0xAC,sizeof first);memset(&second,0xCE,sizeof second);
    CK(gk1_open_loaded_scenario(&d,GK1_MONGOL_CONQUEST,&first)==GK1_LOAD_OK);
    CK(gk1_open_loaded_scenario(&d,GK1_WORLD_CONQUEST,&second)==GK1_LOAD_OK);
    CK(first.phase==GK1_PRE_SETUP_TEMPLATE && second.phase==GK1_PRE_SETUP_TEMPLATE);
    CK(first.country_count==14 && second.country_count==27);
    CK(first.countries[0].gold==425 && second.countries[0].gold==120);
    first.countries[0].gold=999;
    second.countries[0].country_name[0]='X';
    CK(m->gold==425 && strcmp(w->country_name,"the Mongols")==0);
    CK(gk1_open_loaded_scenario(&d,GK1_MONGOL_CONQUEST,&first)==GK1_LOAD_OK);
    CK(first.countries[0].gold==425);
    Gk1LoadedScenario sentinel=first,old=sentinel;
    CK(gk1_open_loaded_scenario(&d,(Gk1ScenarioId)3,&sentinel)==GK1_LOAD_UNSUPPORTED);
    CK(memcmp(&sentinel,&old,sizeof old)==0);
    Gk1TemplateDataset before_fail=d;
    CK(gk1_template_load("/not/a/real/path/gk1.csv",&d)==GK1_LOAD_IO);
    CK(memcmp(&d,&before_fail,sizeof d)==0);
    CK(gk1_template_load(NULL,&d)==GK1_LOAD_VALUE);
    CK(memcmp(&d,&before_fail,sizeof d)==0);
}
static void negative_inputs(const char *path){
    size_t n;char *raw=read_all(path,&n);
    CK(raw[0]=='s' && n>9000);
    /* Incomplete records, appended unknown country, wrong header/identity,
       wrong stage, quoted dialect, bad number and structural data edits. */
    expect_rejected(raw,n-100);
    raw[n]='X';raw[n+1]='\0';expect_rejected(raw,n+1);raw[n]='\0';
    char *m=malloc(n+2);CK(m!=NULL);
    memcpy(m,raw,n+1);
    m[0]='X';expect_rejected(m,n);
    memcpy(m,raw,n+1);
    char *spot=strstr(m,"LOADED_TEMPLATE_BEFORE_SETUP_AND_SEASONAL_PROCESSING");CK(spot!=NULL);
    spot[0]='X';expect_rejected(m,n);
    memcpy(m,raw,n+1);
    spot=strstr(m,"the Mongols,Temujin,0,Fur");CK(spot!=NULL);
    spot[strlen("the Mongols,Temujin,0,")]='Z';expect_rejected(m,n);
    memcpy(m,raw,n+1);
    spot=strstr(m,",1,0,the Mongols,Temujin");CK(spot!=NULL);
    spot[1]='2';expect_rejected(m,n);
    memcpy(m,raw,n+1);
    spot=strstr(m,"the Mongols,Temujin,0,Fur");CK(spot!=NULL);
    spot[4]='"';expect_rejected(m,n);
    memcpy(m,raw,n+1);
    spot=strstr(m,"scenario_selector,scenario,");CK(spot!=NULL);
    spot[strlen("scenario_selector")]=';';expect_rejected(m,n);
    memcpy(m,raw,n+1);
    spot=strstr(m,",0x01C9F7,");CK(spot!=NULL);
    spot[5]='Z';expect_rejected(m,n);
    memcpy(m,raw,n+1);
    /* Duplicate row at end must be rejected even though syntactically valid. */
    char *end=strrchr(m,'\n');CK(end!=NULL);
    /* Last line empty because source ends CRLF; duplicate first data row. */
    char *first=strstr(raw,"\r\n");CK(first!=NULL);first+=2;
    char *following=strstr(first,"\r\n");CK(following!=NULL);
    size_t first_len=(size_t)(following-first)+2u;
    char *longer=malloc(n+first_len+1);CK(longer!=NULL);
    memcpy(longer,raw,n);memcpy(longer+n,first,first_len);longer[n+first_len]='\0';
    expect_rejected(longer,n+first_len);free(longer);
    free(m);free(raw);
}
int main(int argc,char **argv){
    if(argc!=2){fprintf(stderr,"Provide exact country CSV path\n");return 2;}
    check_all(argv[1]);negative_inputs(argv[1]);
    printf("GK1 EXEC0049 C11 strict 41-country/45-column pre-setup loader: %u checks PASS\n",checks);
    return 0;
}
