#include "gk1_scenario_factory.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>

static const char *const header[GK1_COUNTRY_COLUMNS]={
 "scenario_selector","scenario","stage","country_id_display","country_index_zero_based",
 "country_name","initial_lord_name","specialty_index","specialty_name",
 "country_sram_base","country_rom_file_offset","specialty_sram_address",
 "specialty_rom_file_offset","name_rom_file_offset", "Fur","Silk","Jewels",
 "Metals","Medicines","Woodworks","Ceramics","Glass","Textiles","Perfume",
 "Food","Arms","Skill","Troops","Economy","Town","Defense","Masons",
 "Commodity_history","Artisans","Food_history","Food_workers","Gold","Morale",
 "Tax","Extra_tax_used","Undrafted_residents","Draft_eligibility_percent",
 "Drafted_total","Combined_population","Eligible_draft_count"
};
static const char *const specialties[10]={
 "Fur","Silk","Jewels","Metals","Medicines","Woodworks","Ceramics",
 "Glass","Textiles","Perfume"
};
static size_t split_row(char *line,char **parts,size_t capacity){
    if(!line||!parts||capacity==0)return 0;
    size_t length=strlen(line);
    while(length && (line[length-1]=='\r'||line[length-1]=='\n'))line[--length]='\0';
    if(length==0)return 0;
    /* Published research has no quotes or embedded comma/newline fields. Reject
       quoted dialect rather than accept a shifted source row silently. */
    for(size_t i=0;i<length;i++)if(line[i]=='"')return 0;
    size_t n=1;parts[0]=line;
    for(char *p=line;*p;p++)if(*p==','){
        if(n==capacity)return 0;
        *p='\0';parts[n++]=p+1;
    }
    return n;
}
static int copy_field(char *target,size_t capacity,const char *source){
    size_t n=strlen(source);
    if(n==0||n>=capacity)return 0;
    memcpy(target,source,n+1);return 1;
}
static int parse_word(const char *s,uint16_t *out){
    if(!s||!*s||!out||*s=='+'||*s=='-'||*s==' ')return 0;
    uint32_t v=0;
    for(const char *p=s;*p;p++){
        if(*p<'0'||*p>'9')return 0;
        v=v*10u+(unsigned)(*p-'0');if(v>999u)return 0;
    }
    *out=(uint16_t)v;return 1;
}
static int parse_id(const char *s,uint8_t *out){
    uint16_t w;if(!parse_word(s,&w)||w>255u)return 0;
    *out=(uint8_t)w;return 1;
}
static int parse_hex(const char *s,uint32_t *out){
    if(!s||!out)return 0;
    if(*s=='$')s++;
    else if(s[0]=='0' && s[1]=='x')s+=2;
    else return 0;
    if(!*s)return 0;
    uint32_t v=0;unsigned n=0;
    for(;*s;s++){
        unsigned digit;
        if(*s>='0'&&*s<='9')digit=(unsigned)(*s-'0');
        else if(*s>='A'&&*s<='F')digit=(unsigned)(*s-'A'+10);
        else if(*s>='a'&&*s<='f')digit=(unsigned)(*s-'a'+10);
        else return 0;
        if(n++>=8u)return 0;
        v=(v<<4)|digit;
    }
    *out=v;return 1;
}
static int parse_country(char **v,size_t n,unsigned expected_scenario,
                          unsigned expected_id,Gk1CountryTemplate *c){
    if(n!=GK1_COUNTRY_COLUMNS||!c)return 0;
    memset(c,0,sizeof *c);
    if(!parse_id(v[0],&c->scenario_id)||c->scenario_id!=expected_scenario ||
       strcmp(v[1],expected_scenario==1?"Mongol Conquest":"World Conquest")||
       strcmp(v[2],"LOADED_TEMPLATE_BEFORE_SETUP_AND_SEASONAL_PROCESSING")||
       !parse_id(v[3],&c->country_id)||c->country_id!=expected_id||
       !parse_id(v[4],&c->index)||(unsigned)c->index+1u!=expected_id||
       !copy_field(c->country_name,sizeof c->country_name,v[5])||
       !copy_field(c->lord_name,sizeof c->lord_name,v[6])||
       !parse_id(v[7],&c->specialty_index)||c->specialty_index>=10u||
       strcmp(v[8],specialties[c->specialty_index])||
       !copy_field(c->specialty_name,sizeof c->specialty_name,v[8]))return 0;
    if(!parse_hex(v[9],&c->country_sram_base)||
       !parse_hex(v[10],&c->country_rom_offset)||
       !parse_hex(v[11],&c->specialty_sram_address)||
       !parse_hex(v[12],&c->specialty_rom_offset)||
       !parse_hex(v[13],&c->name_rom_offset))return 0;
    unsigned idx=expected_id-1u;
    if(c->country_sram_base!=0x6DCDu+83u*idx ||
       c->country_rom_offset!=(expected_scenario==1?0x1C9F7u:0x1E0A7u)+83u*idx||
       c->specialty_sram_address!=0x7692u+12u*idx||
       c->specialty_rom_offset!=(expected_scenario==1?0x1D2BCu:0x1E96Cu)+12u*idx||
       c->name_rom_offset!=(expected_scenario==1?0x1C018u:0x1D6C8u)+17u*idx)return 0;
    for(unsigned i=0;i<GK1_GOODS;i++)if(!parse_word(v[14+i],&c->goods[i]))return 0;
    uint16_t *const fields[]={&c->arms,&c->skill,&c->troops,&c->economy,&c->town,
        &c->defense,&c->masons,&c->commodity_history,&c->artisans,&c->food_history,
        &c->food_workers,&c->gold,&c->morale,&c->tax,&c->extra_tax_used,
        &c->undrafted_reserve,&c->eligibility_percent,&c->drafted_population,
        &c->combined_population,&c->eligible_draft_count};
    for(unsigned i=0;i<sizeof fields/sizeof fields[0];i++)
        if(!parse_word(v[25+i],fields[i]))return 0;
    uint32_t drafted=(uint32_t)c->troops+c->town+c->masons+c->artisans+c->food_workers;
    if(drafted!=c->drafted_population ||
       drafted+c->undrafted_reserve!=c->combined_population ||
       c->eligibility_percent>100 || c->tax>100 ||
       c->extra_tax_used>1 ||
       (uint32_t)c->undrafted_reserve*c->eligibility_percent/100u!=c->eligible_draft_count)return 0;
    return 1;
}
Gk1LoadStatus gk1_template_read(FILE *f,Gk1TemplateDataset *out){
    if(!f||!out)return GK1_LOAD_VALUE;
    Gk1TemplateDataset tmp={0};
    char row[2048],*parts[GK1_COUNTRY_COLUMNS];
    if(!fgets(row,sizeof row,f))return GK1_LOAD_FORMAT;
    size_t n=strlen(row);
    if(n==sizeof row-1&&row[n-1]!='\n')return GK1_LOAD_FORMAT;
    if(split_row(row,parts,GK1_COUNTRY_COLUMNS)!=GK1_COUNTRY_COLUMNS)return GK1_LOAD_FORMAT;
    for(size_t j=0;j<GK1_COUNTRY_COLUMNS;j++)if(strcmp(parts[j],header[j]))return GK1_LOAD_FORMAT;
    for(unsigned i=0;i<GK1_COUNTRIES_ALL;i++){
        if(!fgets(row,sizeof row,f))return GK1_LOAD_ORDER;
        n=strlen(row);if(n==sizeof row-1&&row[n-1]!='\n')return GK1_LOAD_FORMAT;
        unsigned scenario=i<GK1_COUNTRIES_MONGOL?1u:2u;
        unsigned country=i<GK1_COUNTRIES_MONGOL?i+1u:i-GK1_COUNTRIES_MONGOL+1u;
        if(split_row(row,parts,GK1_COUNTRY_COLUMNS)!=GK1_COUNTRY_COLUMNS)return GK1_LOAD_FORMAT;
        if(!parse_country(parts,GK1_COUNTRY_COLUMNS,scenario,country,&tmp.countries[i]))return GK1_LOAD_VALUE;
    }
    if(fgets(row,sizeof row,f))return GK1_LOAD_ORDER;
    if(ferror(f))return GK1_LOAD_IO;
    tmp.mongol_count=GK1_COUNTRIES_MONGOL;tmp.world_count=GK1_COUNTRIES_WORLD;
    *out=tmp;return GK1_LOAD_OK;
}
Gk1LoadStatus gk1_template_load(const char *path,Gk1TemplateDataset *out){
    if(!path||!out)return GK1_LOAD_VALUE;
    FILE *f=fopen(path,"rb");if(!f)return GK1_LOAD_IO;
    Gk1TemplateDataset tmp;
    Gk1LoadStatus s=gk1_template_read(f,&tmp);
    if(fclose(f)!=0)return GK1_LOAD_IO;
    if(s==GK1_LOAD_OK)*out=tmp;
    return s;
}
const Gk1CountryTemplate *gk1_template_country(const Gk1TemplateDataset *all,
                                                 Gk1ScenarioId scenario,uint8_t id){
    if(!all||all->mongol_count!=GK1_COUNTRIES_MONGOL||
       all->world_count!=GK1_COUNTRIES_WORLD)return NULL;
    size_t offset=0,count=0;
    if(scenario==GK1_MONGOL_CONQUEST){count=GK1_COUNTRIES_MONGOL;}
    else if(scenario==GK1_WORLD_CONQUEST){offset=GK1_COUNTRIES_MONGOL;count=GK1_COUNTRIES_WORLD;}
    else return NULL;
    if(!id||id>count)return NULL;
    const Gk1CountryTemplate *c=&all->countries[offset+id-1];
    return (c->scenario_id==(uint8_t)scenario&&c->country_id==id)?c:NULL;
}
Gk1LoadStatus gk1_open_loaded_scenario(const Gk1TemplateDataset *all,
                                      Gk1ScenarioId scenario,Gk1LoadedScenario *out){
    if(!all||!out)return GK1_LOAD_VALUE;
    unsigned count=0;
    if(scenario==GK1_MONGOL_CONQUEST)count=GK1_COUNTRIES_MONGOL;
    else if(scenario==GK1_WORLD_CONQUEST)count=GK1_COUNTRIES_WORLD;
    else return GK1_LOAD_UNSUPPORTED;
    Gk1LoadedScenario next={0};
    next.scenario_id=scenario;next.phase=GK1_PRE_SETUP_TEMPLATE;next.country_count=(uint8_t)count;
    for(unsigned id=1;id<=count;id++){
       const Gk1CountryTemplate *c=gk1_template_country(all,scenario,(uint8_t)id);
       if(!c)return GK1_LOAD_VALUE;
       next.countries[id-1]=*c;
    }
    *out=next;return GK1_LOAD_OK;
}
