#include "gk1_scenario_factory.h"
#include <stdio.h>
#include <stdlib.h>
int main(int argc,char **argv){
    if(argc!=3){fprintf(stderr,"Usage: gk1_scenario_list DATA.csv 1|2\n");return 2;}
    Gk1TemplateDataset d={0};
    if(gk1_template_load(argv[1],&d)!=GK1_LOAD_OK)return 3;
    if(argv[2][0]!='1'&&argv[2][0]!='2')return 4;
    if(argv[2][1])return 4;
    Gk1LoadedScenario s;
    if(gk1_open_loaded_scenario(&d,(Gk1ScenarioId)(argv[2][0]-'0'),&s)!=GK1_LOAD_OK)return 5;
    printf("Scenario %u; %u original PRE-SETUP country templates (not a playable campaign)\n",
            s.scenario_id,s.country_count);
    for(unsigned i=0;i<s.country_count;i++){
       const Gk1CountryTemplate *c=&s.countries[i];
       printf("%2u %-22s %-18s gold=%3u food=%3u troops=%3u reserve=%3u specialty=%s\n",
              c->country_id,c->country_name,c->lord_name,c->gold,c->goods[10],
              c->troops,c->undrafted_reserve,c->specialty_name);
    }
    return 0;
}
