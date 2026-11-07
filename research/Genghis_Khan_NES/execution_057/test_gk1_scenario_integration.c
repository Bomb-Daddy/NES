#include "gk1_app_state.h"
#include "gk1_pad_router.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
static unsigned n=0;
#define T(x) do{++n;if(!(x)){fprintf(stderr,"FAIL %s:%d: %s\n",__FILE__,__LINE__,#x);exit(1);}}while(0)
/* Controller-style input enters exact native app state; physical pad NOT tested. */
static void press(Gk1App *a,Gk1Input e){T(gk1_app_input(a,e)==1);}
int main(int argc,char **argv){
 if(argc!=2)return 2;
 Gk1TemplateDataset *data=malloc(sizeof *data);
 if(!data)return 3;
 T(gk1_template_load(argv[1],data)==GK1_LOAD_OK);
 Gk1App *app=malloc(sizeof *app);Gk1App *before=malloc(sizeof *before);
 if(!app||!before)return 3;
 gk1_app_init(app);
 T(!app->world_ready[0]);
 T(gk1_app_install_templates(app,data));
 T(app->world_ready[0]&&app->world_ready[1]);
 T(app->worlds[0].phase==GK1_PRE_SETUP_TEMPLATE);
 T(app->worlds[1].phase==GK1_PRE_SETUP_TEMPLATE);
 T(app->worlds[0].country_count==14&&app->worlds[1].country_count==27);
 T(app->worlds[0].countries[0].gold==425);
 T(app->worlds[0].countries[0].goods[10]==129);
 T(app->worlds[0].countries[0].troops==60);
 T(!strcmp(app->worlds[0].countries[0].lord_name,"Temujin"));
 T(gk1_app_selected_country(app)==NULL);
 press(app,GK1_INPUT_CONFIRM);
 T(app->screen==GK1_SCREEN_CAMPAIGNS);
 press(app,GK1_INPUT_CONFIRM);
 T(app->screen==GK1_SCREEN_SCENARIO_COUNTRIES);
 T(gk1_app_selected_country(app)==&app->worlds[0].countries[0]);
 press(app,GK1_INPUT_UP);T(app->country_cursor[0]==13);
 press(app,GK1_INPUT_DOWN);T(app->country_cursor[0]==0);
 app->worlds[0].countries[0].gold=777; /* mutate loaded world only */
 T(data->countries[0].gold==425);
 press(app,GK1_INPUT_CANCEL);T(app->screen==GK1_SCREEN_CAMPAIGNS);
 press(app,GK1_INPUT_DOWN);T(app->menu_index==1);
 press(app,GK1_INPUT_CONFIRM);
 T(app->screen==GK1_SCREEN_SCENARIO_COUNTRIES);
 T(app->active_world==1);
 T(app->worlds[1].country_count==27);
 T(app->worlds[1].countries[0].gold==data->countries[14].gold);
 T(app->worlds[0].countries[0].gold==777);
 press(app,GK1_INPUT_UP);T(app->country_cursor[1]==26);
 T(gk1_app_selected_country(app)==&app->worlds[1].countries[26]);
 T(!strcmp(gk1_app_selected_country(app)->country_name,"Dai-Viet & Ta-Li"));
 for(int i=0;i<27;i++)press(app,GK1_INPUT_DOWN);
 T(app->country_cursor[1]==26);
 press(app,GK1_INPUT_CONFIRM);T(strstr(app->status,"NOT YET PROVEN")!=NULL);
 T(app->worlds[0].countries[0].gold==777);
 memcpy(before,app,sizeof *app);
 data->countries[14].country_id=99;
 T(!gk1_app_install_templates(app,data));
 T(memcmp(before,app,sizeof *app)==0); /* no-mutation invalid dataset */
 data->countries[14].country_id=1;
 press(app,GK1_INPUT_CANCEL);press(app,GK1_INPUT_UP);
 press(app,GK1_INPUT_CONFIRM);
 T(app->active_world==0&&app->worlds[0].countries[0].gold==777);
 press(app,GK1_INPUT_CANCEL);press(app,GK1_INPUT_CANCEL);
 T(app->screen==GK1_SCREEN_TITLE);
 /* Re-enter the inherited actual battle fixture; the scenario install must not
  * interfere with prior controller inputs, combat quantities, or replay paths. */
 press(app,GK1_INPUT_DOWN);press(app,GK1_INPUT_DOWN);press(app,GK1_INPUT_DOWN);
 press(app,GK1_INPUT_CONFIRM);T(app->screen==GK1_SCREEN_DIVIDE_PICK);
 press(app,GK1_INPUT_CONFIRM);T(app->screen==GK1_SCREEN_DIVIDE_BATTLE);
 press(app,GK1_INPUT_DOWN);press(app,GK1_INPUT_DOWN);press(app,GK1_INPUT_CONFIRM);
 press(app,GK1_INPUT_RIGHT);press(app,GK1_INPUT_CONFIRM);press(app,GK1_INPUT_CANCEL);
 T(app->divide.split_soldiers==1 && app->divide.completion_word==1);
 T(app->worlds[0].countries[0].gold==777);
 T(app->worlds[1].country_count==27);
 T(gk1_app_restore_path(app,1,NULL,0));
 T(app->world_ready[0] && app->world_ready[1]);
 T(app->worlds[0].countries[0].gold==777);
 T(app->worlds[1].country_count==27);
 printf("EXEC0050 integrated country/Divide controller state %u/%u checks PASS\n",n,n);
 free(before);free(app);free(data);return 0;
}
