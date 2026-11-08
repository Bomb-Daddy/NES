/* Actual EXEC0048 native C11 app-state/controller-route and transactional 64-byte
 * research persistence. Only approved ROM source-bounded cases, not NES runtime. */
#include "gk1_app_state.h"
#include "gk1_divide_store.h"
#include "gk1_pad_router.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
# include <windows.h>
#else
# include <unistd.h>
#endif
static unsigned checks=0;
#define CK(x) do {++checks;if(!(x)){fprintf(stderr,"FAIL 0048 %d: %s\n",__LINE__,#x);exit(1);}}while(0)
static void press(Gk1App *a,Gk1Input b){CK(gk1_app_input(a,b));}
static void open_divide(Gk1App *a,unsigned choice){
 gk1_app_init(a);
 press(a,GK1_INPUT_DOWN);press(a,GK1_INPUT_DOWN);press(a,GK1_INPUT_DOWN);
 CK(a->menu_index==3);press(a,GK1_INPUT_CONFIRM);
 CK(a->screen==GK1_SCREEN_DIVIDE_PICK);
 press(a,GK1_INPUT_CONFIRM);CK(a->screen==GK1_SCREEN_DIVIDE_BATTLE);
 for(unsigned n=0;n<choice;n++)press(a,GK1_INPUT_DOWN);
 CK(a->divide.phase==GK47_QUANTITY&&a->divide.source_soldiers==40);
}
static void enter(Gk1App *a){press(a,GK1_INPUT_CONFIRM);CK(a->divide.phase==GK47_DESTINATION);}
static void commit(Gk1App *a){press(a,GK1_INPUT_RIGHT);CK(a->divide_cursor_x==8);
 press(a,GK1_INPUT_CONFIRM);CK(a->divide.split_x==8&&a->divide.split_y==7);}
static void parity(unsigned choice,unsigned q,unsigned survivors){
 Gk1App s;open_divide(&s,choice);
 enter(&s);CK(s.divide.qty==q);CK(s.divide_event_count==1&&s.divide_events[0]==q);
 Gk47Divide before=s.divide;
 press(&s,GK1_INPUT_CONFIRM);/* non-target: cannot move */
 CK(s.divide.source_soldiers==40&&s.divide.rng48==before.rng48&&s.divide_event_count==1);
 press(&s,GK1_INPUT_CANCEL);CK(s.divide.phase==GK47_DESTINATION_ERROR);
 CK(s.divide.source_soldiers==40&&s.divide.completion_word==0&&s.divide.rng48==before.rng48);
 press(&s,GK1_INPUT_CANCEL);CK(s.divide.phase==GK47_QUANTITY);CK(s.divide.qty==0);
 CK(s.divide.source_soldiers==40&&s.divide.rng48==before.rng48);
 enter(&s);commit(&s);
 CK(s.divide.qty==q&&s.divide.source_soldiers==40-q&&s.divide.split_soldiers==survivors);
 CK(s.divide.rng48==UINT64_C(0xa31e4ecd8fd1));
 CK(s.divide.source_x==7&&s.divide.source_y==7);
 CK(s.divide.split_mobility==10&&s.divide.source_mobility==20);
 CK(s.divide.completion_word==0);
 if(!survivors){CK(s.divide.phase==GK47_DEATH_PROMPT);
   press(&s,GK1_INPUT_CANCEL);CK(s.divide.phase==GK47_PENDING);
   CK(s.divide.completion_word==0);
 }else CK(s.divide.phase==GK47_PENDING);
 press(&s,GK1_INPUT_CANCEL);CK(s.divide.phase==GK47_DONE);
 CK(s.divide.source_mobility==10&&s.divide.completion_word==1);
 CK(s.divide.split_soldiers==survivors);
 uint8_t save[GK48_DIVIDE_BYTES],bad[GK48_DIVIDE_BYTES],unchanged[GK48_DIVIDE_BYTES];
 CK(gk48_divide_encode(&s,save,sizeof save));
 Gk1App reloaded;gk1_app_init(&reloaded);
 CK(gk48_divide_decode(save,sizeof save,&reloaded));
 CK(reloaded.screen==GK1_SCREEN_DIVIDE_BATTLE);
 CK(reloaded.divide.qty==q&&reloaded.divide.split_soldiers==survivors);
 CK(reloaded.divide.completion_word==1&&reloaded.divide.rng48==s.divide.rng48);
 CK(reloaded.divide_event_count==s.divide_event_count);
 memcpy(bad,save,sizeof bad);bad[21]^=1;Gk1App snapshot=reloaded;
 CK(!gk48_divide_decode(bad,sizeof bad,&reloaded));CK(memcmp(&reloaded,&snapshot,sizeof reloaded)==0);
 memcpy(bad,save,sizeof bad);bad[60]^=1;
 CK(!gk48_divide_decode(bad,sizeof bad,&reloaded));
 CK(!gk48_divide_decode(save,sizeof save-1,&reloaded));
 memcpy(bad,save,sizeof bad);bad[13]=9; /* Quantity 9 unsupported */
 uint32_t crc=~0u;for(size_t i=0;i<60;i++){
  crc^=bad[i];for(unsigned j=0;j<8;j++)crc=(crc>>1)^(0xEDB88320u&(0u-(crc&1u)));
 }crc=~crc;for(int j=0;j<4;j++)bad[60+j]=(uint8_t)(crc>>(8*j));
 CK(!gk48_divide_decode(bad,sizeof bad,&reloaded));
 Gk1App forged=s;forged.divide.split_soldiers++;
 memset(unchanged,0xb7,sizeof unchanged);
 CK(!gk48_divide_encode(&forged,unchanged,sizeof unchanged));
 for(size_t i=0;i<sizeof unchanged;i++)CK(unchanged[i]==0xb7);
}
static void boundaries(void){
 Gk1App s;open_divide(&s,2);enter(&s);
 press(&s,GK1_INPUT_UP);press(&s,GK1_INPUT_CONFIRM);
 CK(s.divide.source_soldiers==40&&s.divide.rng48==UINT64_C(0xc7c3f28f2363));
 press(&s,GK1_INPUT_DOWN);commit(&s);
 CK(s.divide.split_soldiers==1);
 Gk1App before=s;press(&s,GK1_INPUT_CONFIRM);
 CK(s.divide_event_count==before.divide_event_count);
 CK(s.divide.source_soldiers==before.divide.source_soldiers);
 uint8_t illegal[]={20};Gk1App sentinel=s;
 CK(!gk1_app_restore_divide(&s,illegal,1,0,7,7));
 CK(memcmp(&s,&sentinel,sizeof s)==0);
 CK(!gk1_app_restore_divide(&s,NULL,1,0,7,7));
 CK(!gk1_app_restore_divide(&s,NULL,0,0,16,7));
 CK(!gk1_app_restore_divide(&s,NULL,0,4,7,7));
}
static void disk(void){
 Gk1App s;open_divide(&s,3);enter(&s);commit(&s);press(&s,GK1_INPUT_CANCEL);
 char path[128];
#ifdef _WIN32
 snprintf(path,sizeof path,"gk48_native_%lu.gkd",(unsigned long)GetCurrentProcessId());
#else
 snprintf(path,sizeof path,"gk48_native_%lu.gkd",(unsigned long)getpid());
#endif
 (void)remove(path);
 CK(gk48_divide_save_new(path,&s)==GK1_STORE_OK);
 CK(gk48_divide_save_new(path,&s)==GK1_STORE_EXISTS);
 Gk1App out;gk1_app_init(&out);
 CK(gk48_divide_load(path,&out)==GK1_STORE_OK);
 CK(out.divide.source_soldiers==32&&out.divide.split_soldiers==2);
 FILE *f=fopen(path,"ab");CK(f!=NULL);CK(fputc(0,f)!=EOF);CK(!fclose(f));
 Gk1App untouched=out;CK(gk48_divide_load(path,&out)==GK1_STORE_CORRUPT);
 CK(memcmp(&out,&untouched,sizeof out)==0);
 CK(!remove(path));CK(gk48_divide_load(path,&out)==GK1_STORE_IO);
}
static void simulated_pad(void){
 Gk1App s;gk1_app_init(&s);Gk1PadRouter pad;gk1_pad_init(&pad);
 Gk1PadSample sample={0};Gk1PadAction acts[8];
 (void)gk1_pad_step(&pad,&sample,16,acts,8);
 uint16_t presses[]={GK1_PAD_DOWN,GK1_PAD_DOWN,GK1_PAD_DOWN,GK1_PAD_A,GK1_PAD_A,
   GK1_PAD_DOWN,GK1_PAD_DOWN,GK1_PAD_A,GK1_PAD_RIGHT,GK1_PAD_A,GK1_PAD_B};
 for(size_t k=0;k<sizeof presses/sizeof presses[0];k++){
  sample.buttons=presses[k];size_t n=gk1_pad_step(&pad,&sample,16,acts,8);
  for(size_t i=0;i<n;i++){
   Gk1Input cmd=acts[i]==GK1_PAD_ACT_DOWN?GK1_INPUT_DOWN:
    acts[i]==GK1_PAD_ACT_RIGHT?GK1_INPUT_RIGHT:
    acts[i]==GK1_PAD_ACT_B?GK1_INPUT_CANCEL:GK1_INPUT_CONFIRM;
   press(&s,cmd);
  }
  sample.buttons=0;(void)gk1_pad_step(&pad,&sample,16,acts,8);
 }
 CK(s.screen==GK1_SCREEN_DIVIDE_BATTLE&&s.divide.qty==7);
 CK(s.divide.source_soldiers==33&&s.divide.split_soldiers==1);
 CK(s.divide.completion_word==1&&s.divide.phase==GK47_DONE);
}
int main(void){parity(0,1,0);parity(1,6,0);parity(2,7,1);parity(3,8,2);
 boundaries();disk();simulated_pad();
 printf("EXEC0048 full inherited native-app Divide, controller, disk: %u assertions PASS (Linux simulated pad, not hardware)\n",checks);
 return 0;}
