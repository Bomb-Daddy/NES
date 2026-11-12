/* EXEC0228: the SAME native C11 game state and Win32/XInput adapter,
 * compiled as a Windows console smoke executable (not a substitute game).
 * Runs on Linux as negative/regression control. Neither path claims actual
 * stock first-Spring generation or a physical Xbox controller test.
 */
#include "gk1_app_state.h"
#include "gk1_source_identity.h"
#include "gk99_xinput_bridge.h"
#include "gk59_storage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif

static int check(int passed,const char *what){
 if(!passed)fprintf(stderr,"EXEC0228 FAIL: %s\n",what);
 return passed;
}
int main(int argc,char **argv){
 if(argc!=2){fprintf(stderr,"Usage: gk228_win32_smoke <verified 41-country CSV>\n");return 2;}
 /* Rejected source files must not be used, even if parseable/renumbered. */
 if(!gk1_verified_original_csv(argv[1])){
  fprintf(stderr,"EXEC0228 REFUSED: source identity mismatch\n");return 3;
 }
 Gk1TemplateDataset *source=malloc(sizeof *source);
 Gk1App *app=malloc(sizeof *app);
 Gk1App *before=malloc(sizeof *before);
 if(!source||!app||!before){free(source);free(app);free(before);return 4;}
 int ok=1;
 ok&=check(gk1_template_load(argv[1],source)==GK1_LOAD_OK,"source parse");
 if(!ok)goto finish;
 gk1_app_init(app);
 ok&=check(gk1_app_install_templates(app,source)==1,"original two-campaign install");
 if(!ok)goto finish;
 ok&=check(app->world_ready[0]&&app->world_ready[1]&&
  app->worlds[0].country_count==14&&app->worlds[1].country_count==27,
  "source 14+27 countries");
 for(unsigned world=0;world<2;world++){
  Gk1LoadedScenario *c=&app->worlds[world];
  ok&=check(c->phase==GK1_PRE_SETUP_TEMPLATE,"not fabricated Spring");
  ok&=check(app->finance_source_baseline[world].phase==GK1_PRE_SETUP_TEMPLATE,
   "immutable baseline remains pre-setup");
  for(unsigned i=0;i<c->country_count;i++)
   ok&=check(c->countries[i].country_id==i+1u,"country identity/order");
 }
 /* Direct application-level merchant gate, even with synthetic offers. */
 Gk61Market m={0};m.playable=1;m.profile=GK61_CLASSIC;m.rate=101;
 m.merchant_present[0]=1;m.buy_offer[0][2]=1;m.sell_offer[0][2]=1;
 Gk61Actor actor={0};actor.orders=3;actor.ability[1]=100;
 *before=*app;
 ok&=check(gk1_app_trade(app,&actor,&m,1,GK61_CHINA,GK61_BUY,2,1,-1)==
  GK61_NOT_PLAYABLE,"original market blocks pre-setup");
 ok&=check(memcmp(before,app,sizeof *app)==0&&actor.orders==3,
  "blocked trade atomic");
 /* The same portable XInput decoder that WinMain polls. No synthetic input
  * is misrepresented as hardware verification. */
 Gk99XinputRaw raw={0};raw.buttons=0x1010u; /* A + START */
 Gk1PadSample sample={0};
 ok&=check(gk99_xinput_sample(&raw,&sample)==1,"XInput decoder active");
 ok&=check((sample.buttons&(GK1_PAD_A|GK1_PAD_MENU))==
   (GK1_PAD_A|GK1_PAD_MENU),"XInput A+START mapping");
 uint8_t slot=0xffu;
 ok&=check(gk99_xinput_slot(0x05u,0xffu,&slot)==1&&slot==0u,
   "four-slot selection");
 char path[512]={0};
 ok&=check(gk59_user_file("C:\\Users\\QA\\AppData\\Local","checkpoint.bin",path,sizeof path)==1 &&
  strstr(path,"GK1NativeResearch\\checkpoint.bin")!=NULL,
  "Windows profile save path");
#ifdef _WIN32
 char exe[2048]={0};
 DWORD n=GetModuleFileNameA(NULL,exe,(DWORD)sizeof exe);
 ok&=check(n>0&&n<sizeof exe,"native Win32 module path");
 ok&=check(n>=4u&&strcmp(exe+n-4,".exe")==0,"Win32 executable extension");
 ok&=check(GetFileAttributesA(argv[1])!=INVALID_FILE_ATTRIBUTES,
   "Win32 filesystem verified source");
 const char *platform="NATIVE_WIN32_BUILD_EXECUTED";
#else
 const char *platform="LINUX_PORT_NEGATIVE_CONTROL";
#endif
 if(ok){
  printf("GK1_EXEC0228 PASS %s source=41 Classic=PRE_SETUP trade=REFUSED controller=software_mapping physical_Xbox=NOT_RUN stock_first_Spring=OPEN\n",platform);
 }
finish:
 free(before);free(app);free(source);
 return ok?0:5;
}
