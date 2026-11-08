#include "gk1_app_state.h"
#include "gk1_pad_router.h"
#include "gk68_observed_rest.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned n;
#define T(c) do{n++;if(!(c)){fprintf(stderr,"EXEC0068 FAIL %d: %s\n",__LINE__,#c);return 2;}}while(0)
int main(int argc,char **argv){
 if(argc!=2)return 3;
 Gk1TemplateDataset *ds=malloc(sizeof *ds);
 Gk1App *a=malloc(sizeof *a),*before=malloc(sizeof *before);
 T(ds&&a&&before);T(gk1_template_load(argv[1],ds)==GK1_LOAD_OK);
 gk1_app_init(a);T(gk1_app_install_templates(a,ds));
 /* Controller routing: fifth menu item is Enhanced-only. */
 T(gk1_app_input(a,GK1_INPUT_CONFIRM));
 T(gk1_app_input(a,GK1_INPUT_LEFT));T(a->rules_profile==0);
 T(gk1_app_input(a,GK1_INPUT_CANCEL));T(a->screen==GK1_SCREEN_TITLE);
 for(int j=0;j<4;j++)T(gk1_app_input(a,GK1_INPUT_DOWN));
 T(a->menu_index==4);*before=*a;
 T(gk1_app_input(a,GK1_INPUT_CONFIRM));T(a->screen==GK1_SCREEN_TITLE);
 T(!memcmp(&before->worlds,&a->worlds,sizeof a->worlds));
 T(gk1_app_input(a,GK1_INPUT_UP));T(a->menu_index==3);
 T(gk1_app_input(a,GK1_INPUT_DOWN));T(a->menu_index==4);
 T(gk1_app_input(a,GK1_INPUT_CANCEL)==1); /* title ignores B */
 a->rules_profile=1;*before=*a;
 T(gk1_app_input(a,GK1_INPUT_CONFIRM));T(a->screen==GK1_SCREEN_REST_CASE);
 T(a->rest_case.bp==170&&a->rest_case.orders==2);
 T(!memcmp(&a->worlds,&before->worlds,sizeof a->worlds));
 Gk68RestCase spring=a->rest_case;
 T(gk1_app_input(a,GK1_INPUT_CONFIRM));T(a->rest_case.stage==GK68_REST_CONFIRM);
 T(gk1_app_input(a,GK1_INPUT_CANCEL));T(!memcmp(&a->rest_case,&spring,sizeof spring));
 T(!memcmp(&a->worlds,&before->worlds,sizeof a->worlds));
 /* Exact Mesen Enhanced B cancel: 8192-byte source RAM unchanged in
  * original proof; native fixture must likewise revert every field. */
 T(gk1_app_input(a,GK1_INPUT_CONFIRM));T(gk1_app_input(a,GK1_INPUT_CONFIRM));
 T(a->rest_case.stage==GK68_REST_SUMMER&&a->rest_case.year==1175);
 T(a->rest_case.orders==3&&a->rest_case.bp==829&&a->rest_case.rate==97);
 T(a->rest_case.skill==115&&a->rest_case.economy==183);
 T(a->rest_case.gold==439&&a->rest_case.food==208&&a->rest_case.troops==51);
 T(!memcmp(&a->worlds,&before->worlds,sizeof a->worlds));
 Gk68RestCase summer=a->rest_case;
 T(gk1_app_input(a,GK1_INPUT_CONFIRM));T(!memcmp(&a->rest_case,&summer,sizeof summer));
 T(gk1_app_input(a,GK1_INPUT_CANCEL));T(a->screen==GK1_SCREEN_TITLE);
 T(!memcmp(&a->worlds,&before->worlds,sizeof a->worlds));
 Gk68RestCase c,old;
 T(gk68_rest_open(&c)==GK68_REST_OK);old=c;
 T(gk68_rest_press_b(&c)==GK68_REST_WRONG_PHASE);T(!memcmp(&c,&old,sizeof c));
 c.bp=171;old=c;
 T(!gk68_rest_valid(&c));T(gk68_rest_press_a(&c)==GK68_REST_BAD);
 T(gk68_rest_press_b(&c)==GK68_REST_BAD);T(!memcmp(&c,&old,sizeof c));
 T(gk68_rest_open(&c)==GK68_REST_OK);
 T(gk68_rest_press_a(&c)==GK68_REST_OK);T(gk68_rest_press_a(&c)==GK68_REST_OK);
 old=c;T(gk68_rest_press_a(&c)==GK68_REST_WRONG_PHASE);
 T(gk68_rest_press_b(&c)==GK68_REST_WRONG_PHASE);T(!memcmp(&c,&old,sizeof c));
 T(gk68_rest_open(NULL)==GK68_REST_BAD);
 /* Original template still never receives made-up original first Spring. */
 T(a->worlds[0].phase==GK1_PRE_SETUP_TEMPLATE&&a->worlds[1].phase==GK1_PRE_SETUP_TEMPLATE);
 T(a->worlds[0].country_count==14&&a->worlds[1].country_count==27);
 printf("EXEC0068 OBSERVED_RUNTIME_MOD isolated Rest A/B controller transaction %u assertions PASS; synthetic-controller TESTED_PORT_LINUX, NO stock or full first Spring\n",n);
 free(before);free(a);free(ds);return 0;
}
