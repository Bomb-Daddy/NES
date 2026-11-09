/* Source-derived 0045 vs native-app interactive state, injection-only controller QA.
 * Not a fresh NES native experiment and not a physical Xbox test. */
#include "gk1_app_state.h"
#include "gk1_pad_router.h"
#include "gk1_path_store.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
# include <windows.h>
#else
# include <unistd.h>
#endif
static unsigned checks=0;
#define CK(x) do{checks++;if(!(x)){fprintf(stderr,"EXEC0046 FAIL line %d: %s\n",__LINE__,#x);exit(1);}}while(0)
static void action(Gk1App *a,Gk1Input k){CK(gk1_app_input(a,k)==1);}
static void start(Gk1App *a,unsigned scenario){
 gk1_app_init(a);action(a,GK1_INPUT_DOWN);action(a,GK1_INPUT_DOWN);
 CK(a->menu_index==2);action(a,GK1_INPUT_CONFIRM);CK(a->screen==GK1_SCREEN_PATH_PICK);
 for(unsigned i=0;i<scenario;i++)action(a,GK1_INPUT_DOWN);
 action(a,GK1_INPUT_CONFIRM);CK(a->screen==GK1_SCREEN_PATH_BATTLE);
 CK(a->path.soldiers==40&&a->path.mobility==10&&a->path.x==7&&a->path.y==7);
}
static void first(Gk1App *a){
 action(a,GK1_INPUT_RIGHT);CK(a->cursor_x==8);
 action(a,GK1_INPUT_CONFIRM);
 CK(a->path.x==8&&a->path.y==7&&a->path_event_count==1&&a->path_events[0]==1);
}
static void second(Gk1App *a){
 action(a,GK1_INPUT_RIGHT);CK(a->cursor_x==9);
 action(a,GK1_INPUT_CONFIRM);
 CK(a->path_event_count==2&&a->path_events[1]==2);
}
static void check_save(Gk1App *app){
 uint8_t bytes[GK46_PATH_BYTES],wrong[GK46_PATH_BYTES],sentinel[GK46_PATH_BYTES];
 memset(bytes,0xA5,sizeof bytes);CK(gk46_path_encode(app,bytes,sizeof bytes));
 Gk1App out;gk1_app_init(&out);CK(gk46_path_decode(bytes,sizeof bytes,&out));
 CK(out.path.fixture==app->path.fixture&&out.path.phase==app->path.phase);
 CK(out.path.soldiers==app->path.soldiers&&out.path.ally1_soldiers==app->path.ally1_soldiers);
 CK(out.path.mobility==app->path.mobility&&out.path.completion_word==app->path.completion_word);
 CK(out.path.rng48==app->path.rng48&&out.path_event_count==app->path_event_count);
 memcpy(wrong,bytes,sizeof wrong);wrong[9]=4;memset(&out,0xAC,sizeof out);Gk1App before=out;
 CK(!gk46_path_decode(wrong,sizeof wrong,&out));CK(memcmp(&before,&out,sizeof out)==0);
 memcpy(wrong,bytes,sizeof wrong);wrong[16]^=1;CK(!gk46_path_decode(wrong,sizeof wrong,&out));
 memcpy(wrong,bytes,sizeof wrong);wrong[60]^=1;CK(!gk46_path_decode(wrong,sizeof wrong,&out));
 CK(!gk46_path_decode(bytes,sizeof bytes-1,&out));
 CK(!gk46_path_encode(app,sentinel,sizeof sentinel-1));
 Gk1App altered=*app;altered.path.soldiers++;memset(sentinel,0xB7,sizeof sentinel);
 CK(!gk46_path_encode(&altered,sentinel,sizeof sentinel));for(size_t i=0;i<sizeof sentinel;i++)CK(sentinel[i]==0xB7);
}
static void path_cases(void){
 Gk1App s;
 start(&s,0);Gk45Turn unchanged=s.path;
 action(&s,GK1_INPUT_CONFIRM);CK(s.path.soldiers==40&&s.path.x==7&&s.path_event_count==0);
 action(&s,GK1_INPUT_DOWN);action(&s,GK1_INPUT_CONFIRM);
 CK(s.path.x==7&&s.path_event_count==0);action(&s,GK1_INPUT_UP);
 CK(s.cursor_y==7);first(&s);
 CK(s.path.soldiers==34&&s.path.mobility==6&&s.path.rng48==UINT64_C(0xa31e4ecd8fd1));
 CK(s.path.phase==GK45_PENDING&&s.path.completion_word==0);
 second(&s);CK(s.path.phase==GK45_REFUSAL_MESSAGE);
 CK(s.path.soldiers==34&&s.path.x==8&&s.path.mobility==6&&s.path.completion_word==0);
 CK(s.path.rng48==UINT64_C(0xa31e4ecd8fd1));
 action(&s,GK1_INPUT_CANCEL);CK(s.path.phase==GK45_PENDING&&s.path.completion_word==0);
 CK(s.path_event_count==3&&s.path_events[2]==3);
 action(&s,GK1_INPUT_CANCEL);CK(s.path.phase==GK45_DONE&&s.path.completion_word==1);
 CK(s.path.mobility==10&&s.path_event_count==4&&s.path_events[3]==3);
 CK(s.path.soldiers==34&&s.path.x==8);check_save(&s);
 action(&s,GK1_INPUT_CONFIRM);CK(s.path_event_count==4&&s.path.soldiers==34);
 action(&s,GK1_INPUT_CANCEL);CK(s.screen==GK1_SCREEN_PATH_PICK);
 CK(unchanged.soldiers==40);
 start(&s,1);first(&s);second(&s);
 CK(s.path.phase==GK45_PENDING&&s.path.x==9&&s.path.soldiers==34&&s.path.mobility==3);
 CK(s.path.rng48==UINT64_C(0xa31e4ecd8fd1));
 action(&s,GK1_INPUT_CANCEL);CK(s.path.phase==GK45_DONE);
 CK(s.path.mobility==10&&s.path.completion_word==1&&s.path_event_count==3);check_save(&s);
 start(&s,2);first(&s);
 CK(s.path.phase==GK45_DONE&&s.path.soldiers==74&&s.path.ally1_soldiers==0);
 CK(s.path.x==8&&s.path.ally1_x==8&&s.path.ally1_y==7&&s.path.completion_word==2);
 CK(s.path.mobility==10&&s.path_event_count==1);check_save(&s);
 uint8_t raw[GK46_PATH_BYTES];CK(gk46_path_encode(&s,raw,sizeof raw));
 raw[11]=3; /* impossible first B script; re-CRC to defeat trivial CRC-only gate */
 uint32_t crc=~0u;for(size_t i=0;i<60;i++){
  crc^=raw[i];for(int j=0;j<8;j++)crc=(crc>>1)^(0xEDB88320u&(0u-(crc&1u)));
 }crc=~crc;for(int i=0;i<4;i++)raw[60+i]=(uint8_t)(crc>>(8*i));
 Gk1App result;CK(!gk46_path_decode(raw,sizeof raw,&result));
 CK(!gk1_app_restore_path(&result,1,(uint8_t[]){2},1));
 CK(!gk1_app_restore_path(&result,1,(uint8_t[]){1,2,2},3));
 CK(!gk1_app_restore_path(&result,3,(uint8_t[]){1,3},2));
 CK(!gk1_app_restore_path(&result,2,(uint8_t[]){1,2,3,3},4));
}
static void disk_test(void){
 Gk1App s;start(&s,1);first(&s);second(&s);action(&s,GK1_INPUT_CANCEL);
 char name[128];
#ifdef _WIN32
 snprintf(name,sizeof name,"gk46_path_%lu.gkp",(unsigned long)GetCurrentProcessId());
#else
 snprintf(name,sizeof name,"gk46_path_%lu.gkp",(unsigned long)getpid());
#endif
 (void)remove(name);CK(gk46_path_save_new(name,&s)==GK1_STORE_OK);
 CK(gk46_path_save_new(name,&s)==GK1_STORE_EXISTS);
 Gk1App t;gk1_app_init(&t);CK(gk46_path_load(name,&t)==GK1_STORE_OK);
 CK(t.path.soldiers==34&&t.path.x==9&&t.path.completion_word==1);
 FILE *f=fopen(name,"ab");CK(f!=NULL);CK(fputc(0,f)!=EOF);CK(fclose(f)==0);
 Gk1App original=t;CK(gk46_path_load(name,&t)==GK1_STORE_CORRUPT);
 CK(memcmp(&t,&original,sizeof t)==0);CK(remove(name)==0);
 CK(gk46_path_load(name,&t)==GK1_STORE_IO);
}
static void controller_test(void){
 Gk1PadRouter pad;Gk1App s;gk1_pad_init(&pad);gk1_app_init(&s);
 Gk1PadSample sample={0};Gk1PadAction out[8];
 /* Seed connected, then release. */
 (void)gk1_pad_step(&pad,&sample,16,out,8);
 uint16_t presses[]={GK1_PAD_DOWN,GK1_PAD_DOWN,GK1_PAD_A,GK1_PAD_A,GK1_PAD_RIGHT,GK1_PAD_A};
 for(size_t k=0;k<sizeof presses/sizeof presses[0];k++){
  sample.buttons=presses[k];size_t n=gk1_pad_step(&pad,&sample,16,out,8);
  for(size_t i=0;i<n;i++){
   Gk1Input cmd=out[i]==GK1_PAD_ACT_DOWN?GK1_INPUT_DOWN:
     out[i]==GK1_PAD_ACT_RIGHT?GK1_INPUT_RIGHT:GK1_INPUT_CONFIRM;
   CK(gk1_app_input(&s,cmd));
  }
  sample.buttons=0;(void)gk1_pad_step(&pad,&sample,16,out,8);
 }
 CK(s.screen==GK1_SCREEN_PATH_BATTLE);CK(s.path.soldiers==34);
 CK(s.path_event_count==1&&s.path.phase==GK45_PENDING);
}
int main(void){path_cases();disk_test();controller_test();
 printf("EXEC0046 source-bounded native-shell controller/path/save QA: %u assertions PASS (software controller only)\n",checks);
 return 0;
}
