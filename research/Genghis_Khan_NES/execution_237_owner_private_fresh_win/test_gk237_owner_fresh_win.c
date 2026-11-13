#include "gk237_owner_fresh_win.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(expr) do { if(!(expr)) { fprintf(stderr,"EXEC0237 line %d: %s\n",__LINE__,#expr);return 1; } assertions++; } while(0)
static unsigned assertions;
static int run(const char *csv,const char *private_path){
 Gk1TemplateDataset *d=malloc(sizeof *d);
 Gk1App *a=malloc(sizeof *a),*before=malloc(sizeof *before);
 if(!d||!a||!before)return 2;
 gk1_app_init(a);
 CHECK(gk1_template_load(csv,d)==GK1_LOAD_OK);
 CHECK(gk1_app_install_templates(a,d));
 *before=*a;
 CHECK(gk237_owner_fresh_boot(NULL,csv,private_path)==GK237_INVALID_ARGUMENT);
 CHECK(gk237_owner_fresh_boot(a,NULL,private_path)==GK237_INVALID_ARGUMENT);
 CHECK(gk237_owner_fresh_boot(a,"",private_path)==GK237_INVALID_ARGUMENT);
 CHECK(gk237_owner_fresh_boot(a,csv,NULL)==GK237_NOT_REQUESTED);
 CHECK(gk237_owner_fresh_boot(a,csv,"")==GK237_NOT_REQUESTED);
 CHECK(memcmp(a,before,sizeof *a)==0);
 CHECK(gk237_owner_fresh_boot(a,csv,"/path/not/found/EXEC0237") ==GK237_REFUSED_SOURCE);
 CHECK(memcmp(a,before,sizeof *a)==0);
 CHECK(gk237_owner_fresh_boot(a,"/path/not/found/invalid.csv", "private") ==GK237_REFUSED_SOURCE);
 CHECK(memcmp(a,before,sizeof *a)==0);
 CHECK(gk237_owner_fresh_boot(a,csv,csv)==GK237_REFUSED_SOURCE);
 CHECK(memcmp(a,before,sizeof *a)==0);
 /* Disallow transition from non-title, already-playable or modified templates. */
 a->screen=GK1_SCREEN_CAMPAIGNS;
 Gk1App *changed=malloc(sizeof *changed);if(!changed)return 2;
 *changed=*a;
 CHECK(gk237_owner_fresh_boot(a,csv,"anything") ==GK237_REFUSED_CONTEXT);
 CHECK(memcmp(a,changed,sizeof *a)==0);
 a->screen=GK1_SCREEN_TITLE;
 a->worlds[1].phase=(Gk1ScenarioPhase)2;
 *changed=*a;
 CHECK(gk237_owner_fresh_boot(a,csv,"anything") ==GK237_REFUSED_CONTEXT);
 CHECK(memcmp(a,changed,sizeof *a)==0);
 a->worlds[1].phase=GK1_PRE_SETUP_TEMPLATE;
 a->rules_profile=2;
 *changed=*a;
 CHECK(gk237_owner_fresh_boot(a,csv,"anything") ==GK237_REFUSED_CONTEXT);
 CHECK(memcmp(a,changed,sizeof *a)==0);
 *a=*before;
 if(private_path&&*private_path){
  CHECK(gk237_owner_fresh_boot(a,csv,private_path)==GK237_IMPORTED);
  CHECK(a->rules_profile==0u);
  CHECK(a->screen==GK1_SCREEN_STOCK_CONTINUE);
  CHECK(a->stock_checkpoint.source_verified && a->stock_checkpoint.valid);
  CHECK(a->stock_checkpoint.profile==GK77_FRESH_MONGOL_WORKRAM);
  CHECK(a->stock_checkpoint.orders.remaining==3u);
  /* One specific source observation. NEVER a universal game startup constant. */
  CHECK(a->stock_checkpoint.world.countries[0].gold==430u);
  CHECK(a->stock_checkpoint.world.countries[0].goods[10]==208u);
  CHECK(a->stock_checkpoint.rate==121u);
  CHECK(memcmp(a->worlds,before->worlds,sizeof a->worlds)==0);
  *changed=*a;
  CHECK(gk237_owner_fresh_boot(a,csv,private_path)==GK237_REFUSED_CONTEXT);
  CHECK(memcmp(a,changed,sizeof *a)==0);
  CHECK(gk237_owner_fresh_boot(a,csv,"/path/not/found/EXEC0237") ==GK237_REFUSED_CONTEXT);
  CHECK(memcmp(a,changed,sizeof *a)==0);
 }
 printf("EXEC0237 %u assertions PASS: owner-local WinMain fresh WorkRAM opt-in %s; no new Mesen, original NES Spring or physical Xbox\n",assertions,private_path?"AUTHENTIC PRIVATE SOURCE":"PUBLIC NEGATIVE CONTROLS");
 free(changed);free(before);free(a);free(d);return 0;
}
int main(int argc,char **argv){if(argc!=2&&argc!=3)return 3;return run(argv[1],argc==3?argv[2]:NULL);}
