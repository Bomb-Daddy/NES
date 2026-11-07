/* EXEC0044: UPDATED standalone NATIVE 1280x720 cross-platform Genghis Khan shell.
 * Copyright-clean PLACEHOLDER graphics and no sound assets. NOT a complete game.
 * Linux/X11 native compilation executed; Win32/XInput code provided but NOT
 * Windows-compiled or physically controller-tested in this run.
 */
#define _POSIX_C_SOURCE 200809L
#include "gk1_app_state.h"
#include "../execution_049/gk1_scenario_factory.h"
#include "gk1_source_identity.h"
#include "gk1_pre_setup_store.h"
#include "gk1_setup_draft_store.h"
#include "gk1_manual_setup_store.h"
#include "gk1_manual_bonus_store.h"
#include "gk1_replay_store.h"
#include "gk1_path_store.h"
#include "gk1_divide_store.h"
#include "gk1_pad_router.h"
#include "gk1_snapshot_view.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
# include <windows.h>
# include <xinput.h>
#else
# include <X11/Xlib.h>
# include <time.h>
# include <errno.h>
# include <fcntl.h>
# include <unistd.h>
# include <linux/joystick.h>
#endif
#define GK1_PORT_VERSION "0.55.0-dev1"
#define WIDTH 1280
#define HEIGHT 720
#define BG UINT32_C(0x10202b)
#define EDGE UINT32_C(0xb4a16b)
#define FG UINT32_C(0xe5d7a5)
#define ACTIVE UINT32_C(0x386f58)
#define WARN UINT32_C(0x8b4743)
static Gk1App app;
/* G02 source is the original 41-country *pre-setup* dataset. A failed
 * explicit GK1_SCENARIO_CSV override must never fall back silently. */
static int scenario_loaded=0;
static Gk1TemplateDataset *scenario_original;
static const char *scenario_csv_path(void){
 const char *override=getenv("GK1_SCENARIO_CSV");
 if(override && *override)return override;
 static char sibling[2048];
#ifdef _WIN32
 DWORD n=GetModuleFileNameA(NULL,sibling,(DWORD)sizeof sibling);
 if(!n||n>=sizeof sibling)return "GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv";
 char *slash=strrchr(sibling,'\\');
#else
 ssize_t n=readlink("/proc/self/exe",sibling,sizeof sibling-1);
 if(n<=0||n>=(ssize_t)sizeof sibling)return "GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv";
 sibling[n]='\0';
 char *slash=strrchr(sibling,'/');
#endif
 if(!slash)return "GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv";
 size_t prefix=(size_t)(slash+1-sibling);
 const char *name="GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv";
 if(prefix+strlen(name)+1>sizeof sibling)return name;
 memcpy(sibling+prefix,name,strlen(name)+1);
 return sibling;
}
static int load_scenario_data(void){
 Gk1TemplateDataset *dataset=malloc(sizeof *dataset);
 if(!dataset){strcpy(app.status,"SCENARIO SOURCE ERROR: OUT OF MEMORY");return 0;}
 const char *path=scenario_csv_path();
 if(!gk1_verified_original_csv(path)){
   free(dataset);scenario_loaded=0;
   strcpy(app.status,"ORIGINAL CSV MISSING/CHANGED: SHA256 IDENTIFICATION FAILED");
   return 0;
 }
 Gk1LoadStatus status=gk1_template_load(path,dataset);
 int ok=(status==GK1_LOAD_OK && gk1_app_install_templates(&app,dataset));
 if(ok){free(scenario_original);scenario_original=dataset;}else free(dataset);
 scenario_loaded=ok;
 if(!ok){snprintf(app.status,sizeof app.status,
   "41-COUNTRY CSV UNAVAILABLE/INVALID (%u): NO COUNTRY STATE LOADED",(unsigned)status);}
 return ok;
}

static Gk1PadRouter pad;
static Gk1BattleView imported;
static int imported_ready=0;
static int imported_attempted=0;
static void load_optional_battle_view(void) {
 const char *file=getenv("GK1_BATTLE_SNAPSHOT_FILE");
 if(!file||!*file)return;
 imported_attempted=1;
 const char *profile_env=getenv("GK1_BATTLE_SNAPSHOT_PROFILE");
 const char *hash_env=getenv("GK1_BATTLE_ROM_SHA256");
 uint8_t profile,hash[32];
 if(!profile_env||!hash_env||gk1_view_parse_profile(profile_env,&profile)!=0||
    gk1_view_parse_hash(hash_env,hash)!=0)return;
 imported_ready=(gk1_view_load_file(file,profile,hash,&imported)==0);
}

static const char *divide_replay_path(void){
 const char *s=getenv("GK1_DIVIDE_REPLAY_FILE");
 return (s&&*s)?s:"gk1_mod_divide_replay_slot1.gkd";
}
static Gk1StoreResult save_divide_to(const char *path){
 Gk1StoreResult r=gk48_divide_save_new(path,&app);
 if(r==GK1_STORE_OK)strcpy(app.status,"DIVIDE REPLAY SAVED 64 BYTES - NOT CAMPAIGN");
 else if(r==GK1_STORE_EXISTS)strcpy(app.status,"DIVIDE SAVE EXISTS - NO OVERWRITE");
 else strcpy(app.status,"DIVIDE SAVE REJECTED - UNSUPPORTED STATE / I-O");
 return r;
}
static Gk1StoreResult load_divide_from(const char *path){
 Gk1StoreResult r=gk48_divide_load(path,&app);
 if(r!=GK1_STORE_OK)strcpy(app.status,r==GK1_STORE_CORRUPT?
    "DIVIDE FILE INVALID; STATE UNCHANGED":"DIVIDE SAVE NOT AVAILABLE");
 return r;
}
static const char *path_replay_path(void){
 const char *s=getenv("GK1_PATH_REPLAY_FILE");
 return (s&&*s)?s:"gk1_observed_path_slot1.gkp";
}
static Gk1StoreResult save_path_to(const char *path){
 Gk1StoreResult r=gk46_path_save_new(path,&app);
 if(r==GK1_STORE_OK)strcpy(app.status,"MODIFIED ROM PATH REPLAY SAVED (64 BYTES) - NOT CAMPAIGN");
 else if(r==GK1_STORE_EXISTS)strcpy(app.status,"PATH FILE EXISTS - NO OVERWRITE");
 else strcpy(app.status,"PATH SAVE REJECTED - WRONG STAGE / I-O");
 return r;
}
static Gk1StoreResult load_path_from(const char *path){
 Gk1StoreResult r=gk46_path_load(path,&app);
 if(r!=GK1_STORE_OK)strcpy(app.status,r==GK1_STORE_CORRUPT?
  "PATH CORRUPT / WRONG MOD PROFILE - STATE UNCHANGED":"PATH REPLAY NOT AVAILABLE");
 return r;
}
static const char *replay_path(void){
 const char *s=getenv("GK1_REPLAY_FILE");
 return (s&&*s)?s:"gk1_research_slot1.gkr";
}
static Gk1StoreResult save_replay_to(const char *path){
 if(app.screen!=GK1_SCREEN_RESEARCH_BATTLE || !app.has_session){
  strcpy(app.status,"SAVE REJECTED - RESEARCH BATTLE ONLY");return GK1_STORE_BAD;
 }
 Gk1StoreResult r=gk1_store_save_new(path,&app.session);
 if(r==GK1_STORE_OK)strcpy(app.status,"RESEARCH REPLAY SAVED - 48 BYTES, NOT CAMPAIGN");
 else if(r==GK1_STORE_EXISTS)strcpy(app.status,"REPLAY SLOT EXISTS - NO OVERWRITE; RENAME IT TO KEEP HISTORY");
 else strcpy(app.status,"RESEARCH REPLAY SAVE FAILED - SOURCE STATE OR I/O");
 return r;
}
static Gk1StoreResult load_replay_from(const char *path){
 Gk1BattleSession loaded;
 Gk1StoreResult r=gk1_store_load(path,&loaded);
 if(r!=GK1_STORE_OK){
  strcpy(app.status,r==GK1_STORE_CORRUPT?"REPLAY INVALID - STATE UNCHANGED":"REPLAY FILE NOT ACCESSIBLE");
  return r;
 }
 if(!gk1_app_restore_session(&app,&loaded)){
  strcpy(app.status,"REPLAY NOT SUPPORTED - STATE UNCHANGED");return GK1_STORE_CORRUPT;
 }
 return GK1_STORE_OK;
}

static int running=1;
#ifdef _WIN32
static HWND hwnd;
static HDC screen_dc;
static HFONT font;
#else
static Display *display;
static Window window;
static GC graphics;
static XFontStruct *font;
#endif
static void fillrect(int x,int y,int w,int h,uint32_t rgb){
#ifdef _WIN32
 HBRUSH b=CreateSolidBrush(RGB((rgb>>16)&255,(rgb>>8)&255,rgb&255));
 RECT r={x,y,x+w,y+h};FillRect(screen_dc,&r,b);DeleteObject(b);
#else
 XSetForeground(display,graphics,(unsigned long)rgb);XFillRectangle(display,window,graphics,x,y,(unsigned)w,(unsigned)h);
#endif
}
static void label(int x,int y,const char *text,uint32_t rgb){
 if(!text)return;
#ifdef _WIN32
 SetBkMode(screen_dc,TRANSPARENT);SetTextColor(screen_dc,RGB((rgb>>16)&255,(rgb>>8)&255,rgb&255));
 SelectObject(screen_dc,font);TextOutA(screen_dc,x,y,text,(int)strlen(text));
#else
 XSetForeground(display,graphics,(unsigned long)rgb);XDrawString(display,window,graphics,x,y,text,(int)strlen(text));
#endif
}
static void frame(int x,int y,int w,int h){
 fillrect(x,y,w,h,EDGE);fillrect(x+3,y+3,w-6,h-6,BG);
}
static void menu_line(int y,int selected,const char *name){
 if(selected)fillrect(150,y-32,520,48,ACTIVE);
 label(185,y,name,FG);
}
static void render(void){
 char line[160];
 fillrect(0,0,WIDTH,HEIGHT,BG);
 frame(24,18,1232,684);
 label(62,66,"GENGHIS KHAN I - NATIVE PORT / VERSION " GK1_PORT_VERSION,FG);
 label(62,96,"EXEC0055 MANUAL ABILITY DRAFT / RESEARCH PACKAGE - ORIGINAL SPRING NOT IMPLEMENTED",EDGE);
 if(app.screen==GK1_SCREEN_TITLE){
  frame(120,155,1038,405);
  menu_line(242,app.menu_index==0,"TWO ORIGINAL CAMPAIGNS - PRE-SETUP DATA");
  menu_line(314,app.menu_index==1,"PREVIOUS BATTLE RESEARCH / SNAPSHOT VIEWER");
  menu_line(386,app.menu_index==2,"CONTROLLED TWO-STEP TACTICAL PATH (MOD ROM)");
  menu_line(458,app.menu_index==3,"CONTROLLER-DRIVEN DIVIDE / ZERO-SOLDIER OUTCOME (MOD)");
  label(150,474,"XBOX DPAD / STICK NAVIGATE | A CONFIRM | Y LOAD",FG);
 }else if(app.screen==GK1_SCREEN_CAMPAIGNS){
  frame(120,155,1038,405);
  menu_line(265,app.menu_index==0,"MONGOL CONQUEST - 14 ORIGINAL COUNTRIES / PRE-SETUP");
  menu_line(339,app.menu_index==1,"WORLD CONQUEST - 27 ORIGINAL COUNTRIES / PRE-SETUP");
  snprintf(line,sizeof line,"RULE PROFILE: %s  (LEFT/RIGHT SWITCH)",
       app.rules_profile?"ENHANCED APPROVED MOD":"CLASSIC STOCK");
  label(150,417,line,EDGE);
  label(150,449,"A LOAD ORIGINAL COUNTRY ROSTER / B MAIN",FG);
  label(150,491,scenario_loaded?"41 EXACT ORIGINAL RECORDS LOADED; SPRING SETUP NOT DONE":"ERROR: ORIGINAL 41-COUNTRY CSV NOT LOADED",scenario_loaded?EDGE:WARN);
 }else if(app.screen==GK1_SCREEN_SCENARIO_COUNTRIES){
  const Gk1LoadedScenario *world=&app.worlds[app.active_world];
  const Gk1CountryTemplate *country=gk1_app_selected_country(&app);
  frame(52,131,1172,470);
  snprintf(line,sizeof line,"%s - %u ORIGINAL LOADED TEMPLATES (NOT SPRING)",
    app.active_world==0?"MONGOL CONQUEST":"WORLD CONQUEST",(unsigned)world->country_count);
  label(78,170,line,FG);
  unsigned cursor=app.country_cursor[app.active_world];
  unsigned first=cursor>=4?cursor-4:0;
  if(first+9>world->country_count)first=world->country_count>9?world->country_count-9:0;
  for(unsigned i=0;i<9&&first+i<world->country_count;i++){
   unsigned id=first+i;int y=213+(int)i*39;
   if(id==cursor)fillrect(83,y-26,480,34,ACTIVE);
   snprintf(line,sizeof line,"%02u %-26.26s",(unsigned)world->countries[id].country_id,
      world->countries[id].country_name);
   label(96,y,line,FG);
  }
  if(country){
   snprintf(line,sizeof line,"COUNTRY %u/%u: %.32s",(unsigned)country->country_id,
       (unsigned)world->country_count,country->country_name);label(604,214,line,FG);
   snprintf(line,sizeof line,"INITIAL LORD: %.28s",country->lord_name);label(604,252,line,FG);
   snprintf(line,sizeof line,"GOLD: %u    FOOD: %u",(unsigned)country->gold,(unsigned)country->goods[10]);label(604,291,line,FG);
   snprintf(line,sizeof line,"TROOPS: %u   UNDRAFTED: %u",(unsigned)country->troops,(unsigned)country->undrafted_reserve);label(604,329,line,FG);
   snprintf(line,sizeof line,"MORALE: %u  TAX: %u  ECON: %u",(unsigned)country->morale,
      (unsigned)country->tax,(unsigned)country->economy);label(604,367,line,FG);
   snprintf(line,sizeof line,"ARMS: %u SKILL: %u DEFENSE: %u",(unsigned)country->arms,
       (unsigned)country->skill,(unsigned)country->defense);label(604,405,line,FG);
   snprintf(line,sizeof line,"SPECIALTY: %.22s",country->specialty_name);label(604,443,line,FG);
  }
  snprintf(line,sizeof line,"PROFILE: %s  PHASE: PRE_SETUP",
      app.rules_profile?"ENHANCED MOD":"CLASSIC STOCK");label(604,475,line,EDGE);
  if(app.setup_candidate[app.active_world]!=255){
    unsigned ci=app.setup_candidate[app.active_world];
    snprintf(line,sizeof line,"CANDIDATE: %u %.24s (PENDING ONLY)",ci+1,
       world->countries[ci].lord_name);label(604,505,line,EDGE);
  }else label(604,505,"CANDIDATE: NONE (PENDING ONLY)",EDGE);
  label(604,537,"A MARK CANDIDATE / START SETUP / X SAVE / Y LOAD",FG);
  label(604,565,"B BACK; NO UNVERIFIED SETUP EXECUTED",FG);
  label(84,576,"PRE-SETUP SNAPSHOT ONLY; NO VERIFIED PLAYER/DIFFICULTY/SEASON SETUP",WARN);
 }else if(app.screen==GK1_SCREEN_SETUP_WIZARD){
  uint8_t w=app.active_world;
  unsigned stage=app.setup_stage[w];
  frame(80,145,1120,444);
  label(113,193,"ORIGINAL NES MANUAL SETUP - PRE-SETUP INTENT ONLY",FG);
  snprintf(line,sizeof line,"CAMPAIGN: %s | STEP %u OF 5",
     w==0?"MONGOL CONQUEST":"WORLD CONQUEST",stage+1);label(113,240,line,EDGE);
  if(stage==GK53_SETUP_PLAYERS){
   snprintf(line,sizeof line,"PLAYERS: %u (MONGOL: 1 ONLY; WORLD: 1-4)",app.setup_player_count[w]);
   label(113,298,line,FG);
  }else if(stage==GK53_SETUP_DIFFICULTY){
   snprintf(line,sizeof line,"DIFFICULTY: %u / 5 (MANUAL RANGE)",app.setup_difficulty[w]);
   label(113,298,line,FG);
  }else if(stage==GK53_SETUP_LORD){
   unsigned p=app.setup_player_slot[w];
   const Gk1CountryTemplate *c=&app.worlds[w].countries[app.country_cursor[w]];
   snprintf(line,sizeof line,"PLAYER %u/%u LORD: %s (COUNTRY %u)",p+1,
      app.setup_player_count[w],c->lord_name,(unsigned)c->country_id);label(113,298,line,FG);
   label(113,337,"ALLOWED WORLD: GENGHIS / RICHARD / ALEXIOUS / YORITOMO",EDGE);
  }else if(stage==GK53_SETUP_ABILITIES){
   snprintf(line,sizeof line,"SIX ABILITIES / FIVE CHARACTERS: %s",
     app.setup_method_cursor[w]?"MANUAL +100 POINTS":"AUTOMATIC RANDOM");label(113,298,line,FG);
  }else{
   if(app.setup_ability_method[w]==2){
    static const char *abilities[6]={"LEADERSHIP","JUDGMENT","PLANNING","CHARM","BODY","BATTLE"};
    unsigned who=app.manual_bonus_person[w],attr=app.manual_bonus_ability[w];
    snprintf(line,sizeof line,"BONUS DRAFT: %s / %s +%u",
      who==0?"LORD":who==1?"PRINCE 1":who==2?"PRINCE 2":who==3?"PRINCE 3":"PRINCE 4",
      abilities[attr],(unsigned)app.manual_bonus[w][who][attr]);
    label(113,295,line,FG);
    snprintf(line,sizeof line,"DRAFT POINTS: %u/100 ; ACTIVE: %s",
      gk55_bonus_spent(&app,w),app.manual_bonus_active[w]?"YES":"NO");
    label(113,331,line,EDGE);
    label(113,367,"START EDIT/EXIT | LEFT/RIGHT PERSON | UP/DOWN ABILITY",FG);
    label(113,403,"IN EDIT: A +1, B +10; DRAFT NOT BASE STATS",FG);
   }else label(113,296,"AUTOMATIC SELECTED - SOURCE RNG STILL UNVERIFIED",WARN);
   label(113,427,"NO ORIGINAL SEASONAL INITIALIZATION",WARN);
  }
  label(113,456,app.manual_bonus_active[w]?"A +1 / B +10 / X SAVE / Y LOAD":"DPAD / A CONFIRM / B BACK / X SAVE / Y LOAD",FG);
  label(113,500,"NO SEASONAL PROCESSING OR UNVERIFIED ABILITY ROLLS PERFORMED",WARN);
 }else if(app.screen==GK1_SCREEN_UNIMPLEMENTED){
  frame(120,155,1038,405);
  label(160,276,"GAMEPLAY NOT IMPLEMENTED; NO FABRICATED CAMPAIGN DATA",FG);
  label(160,362,"A OR B TO RETURN",FG);
 }else if(app.screen==GK1_SCREEN_DIVIDE_PICK){
  frame(80,137,1120,455);
  label(105,177,"ENHANCED APPROVED ROM - OBSERVED DIVIDE: ORIGINAL 40 / MOBILITY 20",FG);
  label(105,211,"SELECT ONLY ONE OF THE FOUR NATIVE-OBSERVED QUANTITIES",EDGE);
  for(int i=0;i<4;i++){
    int y=263+64*i;const unsigned q[]={1,6,7,8};
    if(app.divide_choice==(unsigned)i)fillrect(125,y-32,490,46,ACTIVE);
    snprintf(line,sizeof line,"%u SOLDIERS -> FOREST: %u SURVIVE",q[i],i<2?0:(unsigned)(i-1));
    label(152,y,line,FG);
  }
  label(724,313,"A ENTER QUANTITY STAGE",FG);
  label(724,359,"DPAD SELECT / B MAIN",FG);
  label(724,415,"ONLY SOURCE-MATCHED STATE",WARN);
 }else if(app.screen==GK1_SCREEN_DIVIDE_BATTLE){
  frame(52,127,1172,476);
  label(87,168,"ACTUAL DIVIDE STATE - APPROVED ENHANCED ROM, FOUR PROVEN CASES",FG);
  for(int y=0;y<10;y++)for(int x=0;x<16;x++){
   uint32_t c=((x+y)&1)?UINT32_C(0x24413b):UINT32_C(0x294943);
   if(x==8&&y==7)c=UINT32_C(0x4d6542);
   if(x==7&&y==7)c=UINT32_C(0x405c91);
   if(x==app.divide_cursor_x&&y==app.divide_cursor_y)c=EDGE;
   fillrect(90+x*38,189+y*33,37,32,c);
   if(x==7&&y==7)label(101+x*38,211+y*33,"S",FG);
   else if(x==app.divide.split_x && y==app.divide.split_y &&
           app.divide.phase>=GK47_PENDING && app.divide.phase<=GK47_DONE)
    label(101+x*38,211+y*33,app.divide.split_soldiers?"3":"X",FG);
   else if(x==app.divide_cursor_x&&y==app.divide_cursor_y)
    label(101+x*38,211+y*33,"+",BG);
  }
  snprintf(line,sizeof line,"QUANTITY: %u",(unsigned)(app.divide.qty?app.divide.qty:
     (unsigned)(app.divide_choice==0?1:app.divide_choice==1?6:app.divide_choice==2?7:8)));
  label(724,215,line,FG);
  snprintf(line,sizeof line,"SOURCE SLOT0: %u at (7,7)",(unsigned)app.divide.source_soldiers);
  label(724,254,line,FG);
  snprintf(line,sizeof line,"SPLIT SLOT3: %u at %u,%u",(unsigned)app.divide.split_soldiers,
      (unsigned)app.divide.split_x,(unsigned)app.divide.split_y);
  label(724,294,line,FG);
  snprintf(line,sizeof line,"SOURCE MOB: %u / SPLIT MOB: %u",
      (unsigned)app.divide.source_mobility,(unsigned)app.divide.split_mobility);
  label(724,334,line,FG);
  snprintf(line,sizeof line,"WORD $613E: %u",(unsigned)app.divide.completion_word);label(724,374,line,FG);
  const char *phase=app.divide.phase==GK47_QUANTITY?"QUANTITY - DPAD, A":
     app.divide.phase==GK47_DESTINATION?"DESTINATION - RIGHT, A":
     app.divide.phase==GK47_DESTINATION_ERROR?"CAN'T MOVE THERE - B":
     app.divide.phase==GK47_DEATH_PROMPT?"UNIT3 BIT THE DUST - B":
     app.divide.phase==GK47_PENDING?"COMMITTED - B FINISH":"DONE - B RETURN";
  snprintf(line,sizeof line,"PHASE: %s",phase);label(724,414,line,FG);
  snprintf(line,sizeof line,"RNG48: %012llX",(unsigned long long)app.divide.rng48);
  label(724,454,line,FG);
  label(724,490,"X SAVE / Y LOAD / DPAD + A/B",FG);
  label(724,534,"ZERO-SOLDIER SLOT IS NOT BATTLE END",WARN);
 }else if(app.screen==GK1_SCREEN_PATH_PICK){
  frame(80,137,1120,455);
  label(110,177,"APPROVED COMBINED SHARED BP/REST ROM - CONTROLLED STATE",FG);
  menu_line(253,app.path_case==0,"RAW 7 DESTINATION - REJECTION / TWO B PRESSES");
  menu_line(328,app.path_case==1,"RAW 2 DESTINATION - TWO LEGAL STEPS / B");
  menu_line(403,app.path_case==2,"ALLY40 AT (8,7) - FOREST LOSS THEN JOIN");
  label(110,497,"SOURCE-BOUND FIXTURES: A START, DPAD CHOOSE, B MAIN",EDGE);
  label(110,541,"NOT GENERAL MOVE/AI, NOT VANILLA ROM, NOT CAMPAIGN",WARN);
 }else if(app.screen==GK1_SCREEN_PATH_BATTLE){
  frame(52,127,1172,476);
  label(87,168,"CONTROLLER-MOVED CURSOR + COMMITTED SOLDIER STATE",FG);
  for(int y=0;y<10;y++)for(int x=0;x<16;x++){
   uint32_t tile=((x+y)&1)?UINT32_C(0x24413b):UINT32_C(0x294943);
   if(x==8 && y==7)tile=UINT32_C(0x4d6542); /* observed forest */
   if(x==9 && y==7)tile=app.path_case==1?UINT32_C(0x436b68):WARN;
   if(x==app.path.x && y==app.path.y)tile=UINT32_C(0x405c91);
   if(x==app.cursor_x&&y==app.cursor_y)tile=EDGE;
   fillrect(90+x*38,189+y*33,37,32,tile);
   if(x==app.cursor_x&&y==app.cursor_y)fillrect(93+x*38,192+y*33,31,26,BG);
   if(x==app.path.x&&y==app.path.y)label(102+x*38,211+y*33,"M",FG);
   else if(app.path_case==2&&app.path.ally1_soldiers>0&&
           x==app.path.ally1_x&&y==app.path.ally1_y)label(102+x*38,211+y*33,"F",FG);
   else if(x==9&&y==7)label(102+x*38,211+y*33,app.path_case==1?"2":"7",FG);
   else if(x==8&&y==7)label(102+x*38,211+y*33,"3",FG);
   else if(x==app.cursor_x&&y==app.cursor_y)label(102+x*38,211+y*33,"+",FG);
  }
  snprintf(line,sizeof line,"CASE: %s",app.path_case==0?"RAW 7 REFUSAL":
    app.path_case==1?"CONTROLLED RAW 2":"CONTROLLED FRIEND40");label(724,221,line,FG);
  snprintf(line,sizeof line,"MOVER: %u  POSITION: %u,%u",(unsigned)app.path.soldiers,
      (unsigned)app.path.x,(unsigned)app.path.y);label(724,265,line,FG);
  snprintf(line,sizeof line,"MOBILITY: %u    STEPS: %u",(unsigned)app.path.mobility,
    (unsigned)app.path.steps);label(724,306,line,FG);
  snprintf(line,sizeof line,"ALLY SLOT1: %u",(unsigned)app.path.ally1_soldiers);label(724,347,line,FG);
  snprintf(line,sizeof line,"WORD $613E: %u",(unsigned)app.path.completion_word);label(724,388,line,FG);
  snprintf(line,sizeof line,"PHASE: %s",app.path.phase==GK45_BEFORE?"SELECT STEP":
    app.path.phase==GK45_PENDING?"PENDING B / NEXT STEP":
    app.path.phase==GK45_REFUSAL_MESSAGE?"ERROR: B DISMISS":"DONE: B MENU");label(724,430,line,FG);
  label(724,473,"DPAD MOVE CURSOR / A APPLY",FG);
  label(724,510,"B FINISH / X SAVE / Y LOAD",FG);
  label(724,550,"ONLY 7,7 -> 8,7 -> 9,7 PROVEN",WARN);
 }else if(app.screen==GK1_SCREEN_RESEARCH_BATTLE){
  frame(52,127,1172,476);
  label(87,168,imported_ready?"RAW 16x10 TILE HEX: PHASE-CAPTURE, NOT TERRAIN CLASSIFICATION":
        "16x10 RESEARCH GRID - PLACEHOLDER ART, NOT NATIVE NES TILES",FG);
  for(int y=0;y<10;y++) for(int x=0;x<16;x++){
   if(imported_ready){
    uint8_t raw;uint32_t records,populated;
    (void)gk1_view_tile(&imported,(unsigned)x,(unsigned)y,&raw,&records,&populated);
    uint32_t border=populated?ACTIVE:(records?WARN:EDGE);
    fillrect(90+x*38,189+y*33,37,32,border);
    fillrect(92+x*38,191+y*33,33,28,BG);
    snprintf(line,sizeof line,"%02X",(unsigned)raw);
    label(97+x*38,211+y*33,line,FG);
   }else{
    uint32_t c=((x+y)&1)?UINT32_C(0x305149):UINT32_C(0x385b52);
    if(x==8&&y==7)c=UINT32_C(0x71826b);
    if(x==7&&y==7)c=UINT32_C(0x405c91);
    fillrect(90+x*38,189+y*33,37,32,c);
   }
  }
  label(725,222,imported_ready?"LOADED SNAPSHOT: OBSERVED PHASE":"CONTROLLED RESEARCH FIXTURE",FG);
  if(!imported_ready)label(725,258,gk1_app_case_label(app.case_index),FG);
  if(imported_ready){
   snprintf(line,sizeof line,"ROM PROFILE: %u",(unsigned)imported.capture.profile);label(725,258,line,FG);
   snprintf(line,sizeof line,"CAPTURE PHASE: %u",(unsigned)imported.capture.phase);label(725,304,line,FG);
   snprintf(line,sizeof line,"RECORDS ON MAP: %u",imported.valid_positions);label(725,344,line,FG);
   snprintf(line,sizeof line,"SOLDIER RECORDS: %u",imported.units_with_soldiers);label(725,384,line,FG);
   snprintf(line,sizeof line,"ZERO SOLDIER ON MAP: %u",imported.empty_record_positions);label(725,424,line,FG);
   snprintf(line,sizeof line,"OUTSIDE MAP: %u",imported.out_of_bounds_positions);label(725,460,line,FG);
   snprintf(line,sizeof line,"RAM $613E: %u",(unsigned)imported.capture.scratch_word);label(725,496,line,FG);
   label(725,528,"GREEN=POPULATED / RED=ZERO / GOLD=NONE",EDGE);
   label(725,560,"READ ONLY; B=RETURN; NO MOVE COMMANDS",WARN);
  }else if(app.has_session){
   snprintf(line,sizeof line,"SOLDIERS: %u",(unsigned)app.session.battle.mover.soldiers);label(725,312,line,FG);
   snprintf(line,sizeof line,"MOBILITY: %u",(unsigned)app.session.battle.mover.mobility);label(725,350,line,FG);
   snprintf(line,sizeof line,"WORD $613E: %u",(unsigned)app.session.battle.completion_word);label(725,388,line,FG);
   snprintf(line,sizeof line,"FRIENDLY SLOT: %u",(unsigned)app.session.battle.friendly_slot1.soldiers);label(725,426,line,FG);
   label(725,492,"A MOVE  /  B FINISH OR BACK",FG);
   label(725,525,"DPAD CASE  /  X SAVE  /  Y LOAD",FG);
  }
 }
 frame(62,628,1150,52);
 if(imported_attempted&&!imported_ready && app.screen==GK1_SCREEN_RESEARCH_BATTLE)
  label(78,658,"SNAPSHOT REJECTED: FORMAT / FILE / ROM HASH; PLACEHOLDER ONLY",WARN);
 else if(imported_ready && app.screen==GK1_SCREEN_RESEARCH_BATTLE)
  label(78,658,"VALIDATED FORMAT / CLAIMED ROM ID; NOT PROOF OF GAMEPLAY | B RETURN",EDGE);
 else label(78,658,app.status,(app.screen==GK1_SCREEN_UNIMPLEMENTED||app.screen==GK1_SCREEN_SCENARIO_COUNTRIES)?WARN:FG);
}
static void input(Gk1Input command){
 /* A verified snapshot is immutable reference data, NEVER a battle engine.
  * Only permit leaving the viewer. No research movement or fake completion.
  */
 if(imported_ready && app.screen==GK1_SCREEN_RESEARCH_BATTLE &&
    command!=GK1_INPUT_CANCEL)return;
 (void)gk1_app_input(&app,command);
}
static void replay_key(int save);
static void pad_actions(const Gk1PadSample *sample){
 Gk1PadAction events[8];
 size_t n=gk1_pad_step(&pad,sample,16,events,8);
 for(size_t i=0;i<n;i++)switch(events[i]){
  case GK1_PAD_ACT_UP:input(GK1_INPUT_UP);break;
  case GK1_PAD_ACT_DOWN:input(GK1_INPUT_DOWN);break;
  case GK1_PAD_ACT_LEFT:input(GK1_INPUT_LEFT);break;
  case GK1_PAD_ACT_RIGHT:input(GK1_INPUT_RIGHT);break;
  case GK1_PAD_ACT_A:input(GK1_INPUT_CONFIRM);break;
  case GK1_PAD_ACT_B:input(GK1_INPUT_CANCEL);break;
  case GK1_PAD_ACT_X:replay_key(1);break;
  case GK1_PAD_ACT_Y:replay_key(0);break;
  case GK1_PAD_ACT_MENU:input(GK1_INPUT_SETUP);break;
 }
}

static const char *pre_setup_path_override;
static const char *pre_setup_file(void){
 if(pre_setup_path_override)return pre_setup_path_override;
 const char *file=getenv("GK1_PRE_SETUP_FILE");
 return file&&*file?file:"gk1_presetup_slot1.gkp";
}
static Gk1StoreResult save_pre_setup(const char *file){
 if(!scenario_loaded||!scenario_original)return GK1_STORE_BAD;
 Gk1StoreResult r=gk55_setup_save_new(file,&app,scenario_original);
 strcpy(app.status,r==GK1_STORE_OK?"PRE_SETUP SETUP-DRAFT SAVED - NOT PLAYABLE SPRING":
   r==GK1_STORE_EXISTS?"PRE_SETUP SLOT EXISTS - NO OVERWRITE":
   "PRE_SETUP SNAPSHOT REJECTED; SOURCE OR STATE INVALID");
 return r;
}
static Gk1StoreResult load_pre_setup(const char *file){
 if(!scenario_loaded||!scenario_original)return GK1_STORE_BAD;
 Gk1StoreResult r=gk55_setup_load(file,scenario_original,&app);
 if(r!=GK1_STORE_OK)strcpy(app.status,"PRE_SETUP LOAD REFUSED; ORIGINAL STATE UNCHANGED");
 return r;
}
/* Synthetic pad events are only a native Linux/Windows SOURCE TEST convenience.
 * They are not physical Xbox controller or original NES gameplay evidence. */
static void pad_test_press(uint16_t bit){
 Gk1PadSample empty={0},press={0};press.buttons=bit;
 pad_actions(&empty);pad_actions(&press);pad_actions(&empty);
}
static void replay_key(int save){
 if(app.screen==GK1_SCREEN_CAMPAIGNS||app.screen==GK1_SCREEN_SCENARIO_COUNTRIES||app.screen==GK1_SCREEN_SETUP_WIZARD){
  if(save)(void)save_pre_setup(pre_setup_file());
  else (void)load_pre_setup(pre_setup_file());
  return;
 }
 if(imported_ready && app.screen==GK1_SCREEN_RESEARCH_BATTLE){
  strcpy(app.status,"BATTLE SNAPSHOT READ-ONLY - NO REPLAY SAVE/LOAD");
  return;
 }
 if(app.screen==GK1_SCREEN_DIVIDE_BATTLE||app.screen==GK1_SCREEN_DIVIDE_PICK){
  if(save)(void)save_divide_to(divide_replay_path());
  else (void)load_divide_from(divide_replay_path());
 }else if(app.screen==GK1_SCREEN_PATH_BATTLE||app.screen==GK1_SCREEN_PATH_PICK){
  if(save)(void)save_path_to(path_replay_path());
  else (void)load_path_from(path_replay_path());
 }else{
  if(save)(void)save_replay_to(replay_path());
  else (void)load_replay_from(replay_path());
 }
}

#ifdef _WIN32
static LRESULT CALLBACK window_proc(HWND h,UINT msg,WPARAM w,LPARAM l){
 (void)l;
 if(msg==WM_DESTROY){running=0;PostQuitMessage(0);return 0;}
 /* No keyboard gameplay or secret keyboard save paths: controller only. */
 if(msg==WM_PAINT){PAINTSTRUCT p;screen_dc=BeginPaint(h,&p);render();EndPaint(h,&p);return 0;}
 return DefWindowProcA(h,msg,w,l);
}
typedef DWORD(WINAPI *XinputGetStateFn)(DWORD,XINPUT_STATE*);
static void poll_xinput(void){
 static HMODULE dll=NULL;static XinputGetStateFn get_state=NULL;
 if(!dll){dll=LoadLibraryA("xinput1_4.dll");if(!dll)dll=LoadLibraryA("xinput9_1_0.dll");
  if(dll)get_state=(XinputGetStateFn)(void*)GetProcAddress(dll,"XInputGetState");}
 if(!get_state){gk1_pad_disconnect(&pad);return;}
 XINPUT_STATE state;memset(&state,0,sizeof state);
 if(get_state(0,&state)!=ERROR_SUCCESS){gk1_pad_disconnect(&pad);return;}
 WORD b=state.Gamepad.wButtons;
 Gk1PadSample s={0};
 if(b&XINPUT_GAMEPAD_DPAD_UP)s.buttons|=GK1_PAD_UP;
 if(b&XINPUT_GAMEPAD_DPAD_DOWN)s.buttons|=GK1_PAD_DOWN;
 if(b&XINPUT_GAMEPAD_DPAD_LEFT)s.buttons|=GK1_PAD_LEFT;
 if(b&XINPUT_GAMEPAD_DPAD_RIGHT)s.buttons|=GK1_PAD_RIGHT;
 if(b&XINPUT_GAMEPAD_A)s.buttons|=GK1_PAD_A;
 if(b&XINPUT_GAMEPAD_B)s.buttons|=GK1_PAD_B;
 if(b&XINPUT_GAMEPAD_X)s.buttons|=GK1_PAD_X;
 if(b&XINPUT_GAMEPAD_Y)s.buttons|=GK1_PAD_Y;
 if(b&XINPUT_GAMEPAD_START)s.buttons|=GK1_PAD_MENU;
 s.lx=state.Gamepad.sThumbLX;
 /* XInput Y positive = up; portable pad router Y positive = down. */
 s.ly=state.Gamepad.sThumbLY==INT16_MIN?INT16_MAX:(int16_t)-state.Gamepad.sThumbLY;
 pad_actions(&s);
}
int WINAPI WinMain(HINSTANCE instance,HINSTANCE prev,LPSTR cmd,int show){
 (void)prev;(void)cmd;
 gk1_app_init(&app);gk1_pad_init(&pad);load_optional_battle_view();(void)load_scenario_data();
 WNDCLASSA cl={0};cl.lpfnWndProc=window_proc;cl.hInstance=instance;
 cl.lpszClassName="GK1_NATIVE_RESEARCH";cl.hCursor=LoadCursor(NULL,IDC_ARROW);
 cl.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
 if(!RegisterClassA(&cl))return 2;
 RECT r={0,0,WIDTH,HEIGHT};AdjustWindowRect(&r,WS_OVERLAPPEDWINDOW,FALSE);
 hwnd=CreateWindowA(cl.lpszClassName,"Genghis Khan I - Native Port " GK1_PORT_VERSION,WS_OVERLAPPEDWINDOW,
 CW_USEDEFAULT,CW_USEDEFAULT,r.right-r.left,r.bottom-r.top,NULL,NULL,instance,NULL);
 if(!hwnd)return 3;
 font=CreateFontA(22,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_DEFAULT_PRECIS,
 CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FIXED_PITCH,"Consolas");
 ShowWindow(hwnd,show);UpdateWindow(hwnd);
 while(running){MSG message;while(PeekMessageA(&message,NULL,0,0,PM_REMOVE)){
  if(message.message==WM_QUIT)running=0;TranslateMessage(&message);DispatchMessageA(&message);}
  if(!running)break;
  poll_xinput();InvalidateRect(hwnd,NULL,FALSE);Sleep(16);
 }
 DeleteObject(font);return 0;
}
#else
/* Linux Xbox controller: js0 left stick axes 0/1, optional D-pad hat
 * axes 6/7, A/B/X/Y 0/1/2/3. Button-style D-pad 11..14 supported.
 * Hotplug and disconnect reset the router to avoid phantom commands.
 */
static int16_t js_axes[8];
static uint16_t js_buttons=0;
static Gk1PadSample joystick_sample(void){
 Gk1PadSample s={0};
 s.buttons=js_buttons;s.lx=js_axes[0];s.ly=js_axes[1];
 s.hx=js_axes[6];s.hy=js_axes[7];return s;
}
static void reset_joystick(void){
 memset(js_axes,0,sizeof js_axes);js_buttons=0;gk1_pad_disconnect(&pad);
}
static void poll_joystick(int *pfd,const char *path){
 if(*pfd<0){
  /* Reopen each frame when absent; no file writes or keyboard fallback. */
  *pfd=open(path,O_RDONLY|O_NONBLOCK);
  if(*pfd<0)return;
  reset_joystick();
 }
 struct js_event e;ssize_t n;
 while((n=read(*pfd,&e,sizeof e))==(ssize_t)sizeof e){
  unsigned type=e.type & (unsigned)~JS_EVENT_INIT;
  if(type==JS_EVENT_BUTTON){
   uint16_t bit=0;
   switch(e.number){
    case 0:bit=GK1_PAD_A;break;case 1:bit=GK1_PAD_B;break;
    case 2:bit=GK1_PAD_X;break;case 3:bit=GK1_PAD_Y;break;
    case 7:bit=GK1_PAD_MENU;break;
    case 11:bit=GK1_PAD_UP;break;case 12:bit=GK1_PAD_DOWN;break;
    case 13:bit=GK1_PAD_LEFT;break;case 14:bit=GK1_PAD_RIGHT;break;
    default:break;
   }
   if(e.value)js_buttons=(uint16_t)(js_buttons|bit);
   else js_buttons=(uint16_t)(js_buttons&~bit);
  }else if(type==JS_EVENT_AXIS && e.number<8)js_axes[e.number]=e.value;
 }
 if(n==0 || (n<0 && errno!=EAGAIN && errno!=EWOULDBLOCK && errno!=EINTR)){
  close(*pfd);*pfd=-1;reset_joystick();return;
 }
 Gk1PadSample s=joystick_sample();pad_actions(&s);
}
int main(int argc,char **argv){
 if(argc==2&&strcmp(argv[1],"--version")==0){
  puts("Genghis Khan I native research prototype " GK1_PORT_VERSION " (EXEC0055, Linux; not full game)");return 0;
 }
 if(argc==3&&strcmp(argv[1],"--scenario-report")==0){
  if(strcmp(argv[2],"1")&&strcmp(argv[2],"2"))return 4;
  gk1_app_init(&app);if(!load_scenario_data()){fprintf(stderr,"SCENARIO LOAD FAILED: %s\n",app.status);return 8;}
  input(GK1_INPUT_CONFIRM);
  if(argv[2][0]=='2')input(GK1_INPUT_DOWN);
  input(GK1_INPUT_CONFIRM);
  if(app.screen!=GK1_SCREEN_SCENARIO_COUNTRIES)return 9;
  const Gk1LoadedScenario *w=&app.worlds[app.active_world];
  printf("G02 INTEGRATED PORT_LINUX scenario=%u countries=%u phase=PRE_SETUP_TEMPLATE\n",
     (unsigned)w->scenario_id,(unsigned)w->country_count);
  for(unsigned k=0;k<w->country_count;k++){
   const Gk1CountryTemplate *c=&w->countries[k];
   printf("%u,%s,%s,gold=%u,food=%u,troops=%u,reserve=%u\n",(unsigned)c->country_id,
      c->country_name,c->lord_name,(unsigned)c->gold,(unsigned)c->goods[10],
      (unsigned)c->troops,(unsigned)c->undrafted_reserve);
  }
  return 0;
 }

 /* Two separate native Linux processes prove disk cold-load behavior without
  * X11 display or an emulator.  Test uses actual app input and disk adapter.
  */
 /* Independently executable source-manual setup, through SAME native app state.
  * Test harness executes controller-equivalent commands; not a physical pad.
  * Last step remains an honest unimplemented original ability/season gate. */
 if(argc==8 && strcmp(argv[1],"--manual-setup-export")==0){
  unsigned sc,primary,players,difficulty;
  char extra;
  if(sscanf(argv[3],"%u%c",&sc,&extra)!=1 ||
     sscanf(argv[4],"%u%c",&primary,&extra)!=1 ||
     sscanf(argv[5],"%u%c",&players,&extra)!=1 ||
     sscanf(argv[6],"%u%c",&difficulty,&extra)!=1 ||
     (sc!=1&&sc!=2)||primary<1||primary>(sc==1?14u:27u)||
     players<1||players>(sc==1?1u:4u)||difficulty<1||difficulty>5||
     (strcmp(argv[7],"automatic")&&strcmp(argv[7],"manual")))return 4;
  if(!gk53_setup_allowed_lord((uint8_t)(sc-1),(uint8_t)(primary-1)))return 4;
  gk1_app_init(&app);if(!load_scenario_data())return 8;
  input(GK1_INPUT_CONFIRM);
  if(sc==2)input(GK1_INPUT_DOWN);
  input(GK1_INPUT_CONFIRM);
  for(unsigned i=1;i<primary;i++)input(GK1_INPUT_DOWN);
  input(GK1_INPUT_CONFIRM);input(GK1_INPUT_SETUP);
  if(app.screen!=GK1_SCREEN_SETUP_WIZARD)return 9;
  for(unsigned i=1;i<players;i++)input(GK1_INPUT_UP);
  input(GK1_INPUT_CONFIRM);
  for(unsigned i=1;i<difficulty;i++)input(GK1_INPUT_UP);
  input(GK1_INPUT_CONFIRM);
  for(unsigned i=0;i<players;i++)input(GK1_INPUT_CONFIRM);
  if(!strcmp(argv[7],"manual"))input(GK1_INPUT_DOWN);
  input(GK1_INPUT_CONFIRM);
  if(app.setup_stage[app.active_world]!=GK53_SETUP_AWAIT_SOURCE)return 9;
  Gk1StoreResult r=save_pre_setup(argv[2]);
  if(r!=GK1_STORE_OK){fprintf(stderr,"MANUAL SETUP EXPORT REFUSED %d\n",r);return 6;}
  printf("EXEC0053 MANUAL SETUP EXPORT PASS scenario=%u players=%u difficulty=%u primary_country=%u abilities=%s phase=PRE_SETUP_TEMPLATE bytes=%u (NOT SPRING)\n",
     sc,players,difficulty,primary,argv[7],(unsigned)GK55_PRE_SETUP_BYTES);
  return 0;
 }
 if(argc==7 && strcmp(argv[1],"--bonus-draft-export")==0){
  unsigned w=(unsigned)atoi(argv[3]),person=(unsigned)atoi(argv[4]),ability=(unsigned)atoi(argv[5]),amount=(unsigned)atoi(argv[6]);
  if(w>1||person>4||ability>5||amount>100)return 4;
  gk1_app_init(&app);gk1_pad_init(&pad);if(!load_scenario_data())return 8;
  input(GK1_INPUT_CONFIRM);if(w)input(GK1_INPUT_DOWN);input(GK1_INPUT_CONFIRM);
  input(GK1_INPUT_CONFIRM);input(GK1_INPUT_SETUP); /* first lord */
  input(GK1_INPUT_CONFIRM);input(GK1_INPUT_CONFIRM);input(GK1_INPUT_CONFIRM); /* 1 player / difficulty / ruler */
  input(GK1_INPUT_DOWN);input(GK1_INPUT_CONFIRM); /* manual method */
  input(GK1_INPUT_SETUP); /* enter bonus editor */
  for(unsigned j=0;j<person;j++)input(GK1_INPUT_RIGHT);
  for(unsigned j=0;j<ability;j++)input(GK1_INPUT_DOWN);
  for(unsigned j=0;j<amount/10;j++)input(GK1_INPUT_CANCEL);
  for(unsigned j=0;j<amount%10;j++)input(GK1_INPUT_CONFIRM);
  input(GK1_INPUT_SETUP); /* exit editor */
  if(gk55_bonus_spent(&app,w)!=amount)return 9;
  Gk1StoreResult r=save_pre_setup(argv[2]);if(r!=GK1_STORE_OK)return 6;
  printf("BONUS DRAFT EXPORT PASS world=%u character=%u ability=%u bonus=%u spent=%u bytes=%u phase=PRE_SETUP\n",
   w,person,ability,(unsigned)app.manual_bonus[w][person][ability],gk55_bonus_spent(&app,w),(unsigned)GK55_PRE_SETUP_BYTES);
  return 0;
 }
 if(argc==3 && strcmp(argv[1],"--bonus-draft-import")==0){
  gk1_app_init(&app);if(!load_scenario_data())return 8;
  Gk1StoreResult r=load_pre_setup(argv[2]);if(r!=GK1_STORE_OK)return 7;
  unsigned w=app.active_world;
  printf("BONUS DRAFT IMPORT PASS world=%u person=%u ability=%u stored=%u total=%u active=%u phase=PRE_SETUP\n",
   w,(unsigned)app.manual_bonus_person[w],(unsigned)app.manual_bonus_ability[w],
   (unsigned)app.manual_bonus[w][app.manual_bonus_person[w]][app.manual_bonus_ability[w]],
   gk55_bonus_spent(&app,w),(unsigned)app.manual_bonus_active[w]);
  return 0;
 }
 if(argc==3 && strcmp(argv[1],"--manual-setup-import")==0){
  gk1_app_init(&app);if(!load_scenario_data())return 8;
  Gk1StoreResult r=load_pre_setup(argv[2]);
  if(r!=GK1_STORE_OK){fprintf(stderr,"MANUAL SETUP IMPORT REFUSED %d\n",r);return 7;}
  unsigned w=app.active_world;
  printf("EXEC0053 MANUAL SETUP IMPORT PASS scenario=%u players=%u difficulty=%u primary_country=%u abilities=%u stage=%u bytes=%u (NOT SPRING)\n",
     w+1,(unsigned)app.setup_player_count[w],(unsigned)app.setup_difficulty[w],
     (unsigned)app.setup_rulers[w][0]+1,(unsigned)app.setup_ability_method[w],
     (unsigned)app.setup_stage[w],(unsigned)GK55_PRE_SETUP_BYTES);
  return 0;
 }
 if(argc==5 && strcmp(argv[1],"--divide-export")==0){
  unsigned q=(unsigned)atoi(argv[3]),stage=(unsigned)atoi(argv[4]);
  if(stage>4||(q!=1&&q!=6&&q!=7&&q!=8))return 4;
  gk1_app_init(&app);
  input(GK1_INPUT_DOWN);input(GK1_INPUT_DOWN);input(GK1_INPUT_DOWN);
  input(GK1_INPUT_CONFIRM);input(GK1_INPUT_CONFIRM);
  while((app.divide_choice==0?1u:app.divide_choice==1?6u:
     app.divide_choice==2?7u:8u)!=q)input(GK1_INPUT_DOWN);
  if(stage>=1)input(GK1_INPUT_CONFIRM);
  if(stage>=2){input(GK1_INPUT_RIGHT);input(GK1_INPUT_CONFIRM);}
  if(stage>=3)input(GK1_INPUT_CANCEL);
  if(stage>=4)input(GK1_INPUT_CANCEL);
  if(app.screen!=GK1_SCREEN_DIVIDE_BATTLE)return 5;
  Gk1StoreResult r=save_divide_to(argv[2]);
  if(r!=GK1_STORE_OK){fprintf(stderr,"DIVIDE EXPORT ERROR %d: %s\n",r,app.status);return 6;}
  printf("DIVIDE EXPORT PASS q=%u stage=%u source=%u split=%u mob=%u word=%u phase=%u\n",
   q,stage,(unsigned)app.divide.source_soldiers,(unsigned)app.divide.split_soldiers,
   (unsigned)app.divide.source_mobility,(unsigned)app.divide.completion_word,
   (unsigned)app.divide.phase);
  return 0;
 }
 if(argc==3 && strcmp(argv[1],"--divide-import")==0){
  gk1_app_init(&app);Gk1StoreResult r=load_divide_from(argv[2]);
  if(r!=GK1_STORE_OK){fprintf(stderr,"DIVIDE IMPORT ERROR %d\n",r);return 7;}
  printf("DIVIDE IMPORT PASS q=%u source=%u split=%u mob=%u word=%u phase=%u\n",
   (unsigned)app.divide.qty,(unsigned)app.divide.source_soldiers,
   (unsigned)app.divide.split_soldiers,(unsigned)app.divide.source_mobility,
   (unsigned)app.divide.completion_word,(unsigned)app.divide.phase);return 0;
 }
 if(argc==5 && strcmp(argv[1],"--path-export")==0){
  /* Real app controller-style actions in a separate native process. */
  unsigned scenario=(unsigned)atoi(argv[3]);const char *script=argv[4];
  if(scenario<1||scenario>3||strlen(script)>4)return 4;
  gk1_app_init(&app);input(GK1_INPUT_DOWN);input(GK1_INPUT_DOWN);
  input(GK1_INPUT_CONFIRM);
  for(unsigned i=1;i<scenario;i++)input(GK1_INPUT_DOWN);
  input(GK1_INPUT_CONFIRM);
  for(size_t i=0;script[i];i++){
   if(script[i]=='1'||script[i]=='2'){
    input(GK1_INPUT_RIGHT);input(GK1_INPUT_CONFIRM);
   }else if(script[i]=='b')input(GK1_INPUT_CANCEL);
   else return 4;
  }
  Gk1StoreResult r=save_path_to(argv[2]);
  if(r!=GK1_STORE_OK){fprintf(stderr,"PATH EXPORT ERROR %d %s\n",r,app.status);return 6;}
  printf("PATH EXPORT PASS scenario=%u actions=%u soldiers=%u ally=%u pos=%u,%u mobility=%u word=%u\n",
    scenario,(unsigned)app.path_event_count,(unsigned)app.path.soldiers,
    (unsigned)app.path.ally1_soldiers,(unsigned)app.path.x,(unsigned)app.path.y,
    (unsigned)app.path.mobility,(unsigned)app.path.completion_word);
  return 0;
 }
 if(argc==3 && strcmp(argv[1],"--path-import")==0){
  gk1_app_init(&app);Gk1StoreResult r=load_path_from(argv[2]);
  if(r!=GK1_STORE_OK){fprintf(stderr,"PATH IMPORT ERROR %d %s\n",r,app.status);return 7;}
  printf("PATH IMPORT PASS scenario=%u actions=%u soldiers=%u ally=%u pos=%u,%u mobility=%u word=%u\n",
    (unsigned)(app.path_case+1),(unsigned)app.path_event_count,(unsigned)app.path.soldiers,
    (unsigned)app.path.ally1_soldiers,(unsigned)app.path.x,(unsigned)app.path.y,
    (unsigned)app.path.mobility,(unsigned)app.path.completion_word);
  return 0;
 }
 if(argc==5 && strcmp(argv[1],"--cold-export")==0){
  int which=atoi(argv[3]),events=atoi(argv[4]);
  if(which<0||which>5||events<0||events>2)return 4;
  gk1_app_init(&app);input(GK1_INPUT_DOWN);input(GK1_INPUT_CONFIRM);
  for(int i=0;i<which;i++)input(GK1_INPUT_RIGHT);
  if(events>=1)input(GK1_INPUT_CONFIRM);
  if(events==2){if(app.last_move!=GK1_STEP_PENDING)return 5;input(GK1_INPUT_CANCEL);}
  Gk1StoreResult r=save_replay_to(argv[2]);
  if(r!=GK1_STORE_OK){fprintf(stderr,"COLD EXPORT FAILURE %d: %s\n",r,app.status);return 6;}
  printf("COLD EXPORT PASS: case=%u events=%u soldiers=%u word=%u bytes=48\n",
   (unsigned)app.case_index,(unsigned)app.session.events,
   (unsigned)app.session.battle.mover.soldiers,(unsigned)app.session.battle.completion_word);
  return 0;
 }
 if(argc==3 && strcmp(argv[1],"--cold-import")==0){
  gk1_app_init(&app);
  Gk1StoreResult r=load_replay_from(argv[2]);
  if(r!=GK1_STORE_OK){fprintf(stderr,"COLD IMPORT FAILURE %d: %s\n",r,app.status);return 7;}
  printf("COLD IMPORT PASS: case=%u events=%u soldiers=%u word=%u bytes=48\n",
   (unsigned)app.case_index,(unsigned)app.session.events,
   (unsigned)app.session.battle.mover.soldiers,(unsigned)app.session.battle.completion_word);
  return 0;
 }
if(argc==3 && strcmp(argv[1],"--pre-setup-pad-smoke")==0){
  gk1_app_init(&app);gk1_pad_init(&pad);
  if(!load_scenario_data())return 8;
  pre_setup_path_override=argv[2];
  Gk1PadSample neutral={0};pad_actions(&neutral);
  pad_test_press(GK1_PAD_A); /* title -> campaigns */
  pad_test_press(GK1_PAD_DOWN);pad_test_press(GK1_PAD_A); /* World */
  if(app.screen!=GK1_SCREEN_SCENARIO_COUNTRIES||app.active_world!=1)return 13;
  pad_test_press(GK1_PAD_X);
  if(strcmp(app.status,"PRE_SETUP SETUP-DRAFT SAVED - NOT PLAYABLE SPRING"))return 14;
  pad_test_press(GK1_PAD_DOWN);
  if(app.country_cursor[1]!=1)return 15;
  pad_test_press(GK1_PAD_Y);
  if(app.active_world!=1||app.country_cursor[1]!=0||
     app.screen!=GK1_SCREEN_SCENARIO_COUNTRIES||
     app.worlds[1].country_count!=27)return 16;
  printf("PRE_SETUP PAD ROUTER X/Y COLD RELOAD PASS world=%u cursor=%u bytes=%u - SYNTHETIC PAD\n",
   (unsigned)app.active_world,(unsigned)app.country_cursor[1],(unsigned)GK55_PRE_SETUP_BYTES);
  return 0;
 }
 /* EXEC0052: two-process real native state and IO, controller-input-driven.
  * This records country CANDIDACY, not historical NES ownership/setup. */
 if(argc==6 && strcmp(argv[1],"--setup-draft-export")==0){
  if((strcmp(argv[3],"1")&&strcmp(argv[3],"2")) ||
     (strcmp(argv[5],"classic")&&strcmp(argv[5],"enhanced")))return 4;
  char *end=NULL;unsigned long selected=strtoul(argv[4],&end,10);
  unsigned count=argv[3][0]=='1'?14u:27u;
  if(!argv[4][0]||!end||*end||selected<1||selected>count)return 4;
  gk1_app_init(&app);if(!load_scenario_data())return 8;
  input(GK1_INPUT_CONFIRM); /* original country menu */
  if(!strcmp(argv[5],"classic"))input(GK1_INPUT_LEFT);
  if(argv[3][0]=='2')input(GK1_INPUT_DOWN);
  input(GK1_INPUT_CONFIRM);
  for(unsigned i=1;i<selected;i++)input(GK1_INPUT_DOWN);
  input(GK1_INPUT_CONFIRM); /* select candidate ONLY */
  if(app.setup_candidate[app.active_world]!=selected-1)return 9;
  Gk1StoreResult result=save_pre_setup(argv[2]);
  if(result!=GK1_STORE_OK){fprintf(stderr,"SETUP DRAFT EXPORT REFUSED code=%d\n",result);return 6;}
  printf("SETUP DRAFT EXPORT PASS world=%u candidate=%u profile=%s phase=PRE_SETUP_TEMPLATE bytes=%u\n",
    (unsigned)app.worlds[app.active_world].scenario_id,
    (unsigned)app.setup_candidate[app.active_world]+1,
    app.rules_profile?"ENHANCED":"CLASSIC",(unsigned)GK55_PRE_SETUP_BYTES);
  return 0;
 }
 if(argc==3 && strcmp(argv[1],"--setup-draft-import")==0){
  gk1_app_init(&app);if(!load_scenario_data())return 8;
  Gk1StoreResult result=load_pre_setup(argv[2]);
  if(result!=GK1_STORE_OK){fprintf(stderr,"SETUP DRAFT IMPORT REFUSED code=%d\n",result);return 7;}
  printf("SETUP DRAFT IMPORT PASS world=%u candidate_mongol=%u candidate_world=%u profile=%s phase=PRE_SETUP_TEMPLATE bytes=%u\n",
    (unsigned)app.worlds[app.active_world].scenario_id,
    app.setup_candidate[0]==255?0u:(unsigned)app.setup_candidate[0]+1u,
    app.setup_candidate[1]==255?0u:(unsigned)app.setup_candidate[1]+1u,
    app.rules_profile?"ENHANCED":"CLASSIC",(unsigned)GK55_PRE_SETUP_BYTES);
  return 0;
 }
 if(argc==4 && strcmp(argv[1],"--pre-setup-export")==0){
  if(strcmp(argv[3],"1")&&strcmp(argv[3],"2"))return 4;
  gk1_app_init(&app);if(!load_scenario_data())return 8;
  input(GK1_INPUT_CONFIRM);
  if(argv[3][0]=='2')input(GK1_INPUT_DOWN);
  input(GK1_INPUT_CONFIRM);
  Gk1StoreResult r=save_pre_setup(argv[2]);
  if(r!=GK1_STORE_OK){fprintf(stderr,"PRE_SETUP EXPORT REFUSED code=%d\n",r);return 6;}
  printf("PRE_SETUP EXPORT PASS %u countries source-phase=1 bytes=%u\n",
    (unsigned)app.worlds[app.active_world].country_count,(unsigned)GK55_PRE_SETUP_BYTES);
  return 0;
 }
 if(argc==3&&strcmp(argv[1],"--pre-setup-import")==0){
  gk1_app_init(&app);if(!load_scenario_data())return 8;
  Gk1StoreResult r=load_pre_setup(argv[2]);
  if(r!=GK1_STORE_OK){fprintf(stderr,"PRE_SETUP IMPORT REFUSED code=%d\n",r);return 7;}
  printf("PRE_SETUP IMPORT PASS scenario=%u count=%u phase=PRE_SETUP_TEMPLATE gold=%u bytes=%u\n",
   (unsigned)app.worlds[app.active_world].scenario_id,
   (unsigned)app.worlds[app.active_world].country_count,
   (unsigned)app.worlds[app.active_world].countries[0].gold,(unsigned)GK55_PRE_SETUP_BYTES);
  return 0;
 }
 int smoke=0,show_snapshot=0,show_path=0,show_divide=0,show_scenario=0,show_setup=0;
 if(argc==3 && (strcmp(argv[1],"--smoke-frames")==0 ||
                 strcmp(argv[1],"--smoke-snapshot")==0 ||
                 strcmp(argv[1],"--smoke-path")==0 ||
                 strcmp(argv[1],"--smoke-divide")==0 ||
                 strcmp(argv[1],"--smoke-scenario")==0 ||
                 strcmp(argv[1],"--smoke-setup")==0)){
  smoke=atoi(argv[2]);if(smoke<1||smoke>120)return 4;
  show_snapshot=(strcmp(argv[1],"--smoke-snapshot")==0);
  show_path=(strcmp(argv[1],"--smoke-path")==0);
  show_divide=(strcmp(argv[1],"--smoke-divide")==0);
  show_scenario=(strcmp(argv[1],"--smoke-scenario")==0);
  show_setup=(strcmp(argv[1],"--smoke-setup")==0);
 }
 else if(argc!=1){fprintf(stderr,"Usage: gk1_native_shell [--smoke-frames 1..120 | --smoke-snapshot 1..120 | --smoke-path 1..120 | --path-export path scenario 1/2/b-script | --path-import path | --cold-export path case events | --cold-import path]\n");return 4;}
 gk1_app_init(&app);gk1_pad_init(&pad);load_optional_battle_view();(void)load_scenario_data();
 if(show_setup){
  if(!scenario_loaded)return 12;
  input(GK1_INPUT_CONFIRM);input(GK1_INPUT_DOWN);input(GK1_INPUT_CONFIRM);
  for(unsigned j=0;j<6;j++)input(GK1_INPUT_DOWN); /* World: Richard */
  input(GK1_INPUT_CONFIRM);input(GK1_INPUT_SETUP);
  input(GK1_INPUT_UP);input(GK1_INPUT_CONFIRM); /* two players */
  for(unsigned j=0;j<4;j++)input(GK1_INPUT_UP);
  input(GK1_INPUT_CONFIRM);
  input(GK1_INPUT_CONFIRM);input(GK1_INPUT_CONFIRM); /* two distinct lords */
  input(GK1_INPUT_DOWN);input(GK1_INPUT_CONFIRM); /* manual ability selection pending */
  if(app.screen!=GK1_SCREEN_SETUP_WIZARD||app.setup_stage[1]!=GK53_SETUP_AWAIT_SOURCE)return 13;
  input(GK1_INPUT_SETUP);input(GK1_INPUT_CANCEL); /* manually apply TEN staged draft points */
  if(!app.manual_bonus_active[1]||gk55_bonus_spent(&app,1)!=10)return 14;
 }
 if(show_scenario){
  if(!scenario_loaded){fprintf(stderr,"SCENARIO SMOKE FAILED: %s\n",app.status);return 12;}
  input(GK1_INPUT_CONFIRM);input(GK1_INPUT_DOWN);input(GK1_INPUT_CONFIRM);
  if(app.screen!=GK1_SCREEN_SCENARIO_COUNTRIES||app.worlds[1].country_count!=27)return 13;
 }
 if(show_divide){
  input(GK1_INPUT_DOWN);input(GK1_INPUT_DOWN);input(GK1_INPUT_DOWN);
  input(GK1_INPUT_CONFIRM);input(GK1_INPUT_CONFIRM);
  input(GK1_INPUT_DOWN);input(GK1_INPUT_DOWN);input(GK1_INPUT_CONFIRM);
  input(GK1_INPUT_RIGHT);input(GK1_INPUT_CONFIRM);input(GK1_INPUT_CANCEL);
  if(app.screen!=GK1_SCREEN_DIVIDE_BATTLE||app.divide.split_soldiers!=1||
     app.divide.completion_word!=1)return 11;
 }
 if(show_path){
  input(GK1_INPUT_DOWN);input(GK1_INPUT_DOWN);input(GK1_INPUT_CONFIRM);
  input(GK1_INPUT_CONFIRM);input(GK1_INPUT_RIGHT);input(GK1_INPUT_CONFIRM);
  if(app.screen!=GK1_SCREEN_PATH_BATTLE||app.path.soldiers!=34)return 10;
 }
 if(show_snapshot){
  if(!imported_ready){fprintf(stderr,"SNAPSHOT SMOKE REFUSED: verified import required\n");return 8;}
  input(GK1_INPUT_DOWN);input(GK1_INPUT_CONFIRM);
  if(app.screen!=GK1_SCREEN_RESEARCH_BATTLE)return 9;
 }
 display=XOpenDisplay(NULL);if(!display){fprintf(stderr,"X11 DISPLAY unavailable\n");return 2;}
 int screen=DefaultScreen(display);
 window=XCreateSimpleWindow(display,RootWindow(display,screen),0,0,WIDTH,HEIGHT,0,0,0);
 XStoreName(display,window,"Genghis Khan I - Native Port " GK1_PORT_VERSION);
 XSelectInput(display,window,ExposureMask|StructureNotifyMask);
 Atom delete_window=XInternAtom(display,"WM_DELETE_WINDOW",False);
 XSetWMProtocols(display,window,&delete_window,1);
 XMapWindow(display,window);graphics=XCreateGC(display,window,0,NULL);
 font=XLoadQueryFont(display,"-misc-fixed-bold-r-normal--20-200-75-75-c-100-iso8859-1");
 if(!font)font=XLoadQueryFont(display,"fixed");
 if(font)XSetFont(display,graphics,font->fid);
 const char *pad=getenv("GK1_JOYSTICK_DEV");
 if(!pad)pad="/dev/input/js0";
 int joystick=-1;
 int frames=0;
 while(running){
  while(XPending(display)){
   XEvent e;XNextEvent(display,&e);
   if(e.type==DestroyNotify)running=0;
   else if(e.type==ClientMessage && (Atom)e.xclient.data.l[0]==delete_window)running=0;
  }
  poll_joystick(&joystick,pad);render();XFlush(display);frames++;
  if(smoke && frames>=smoke){printf("EXEC0055 X11 native smoke: %d frames, %dx%d requested, PASS; snapshot_import=%s; viewer=%s\n",frames,WIDTH,HEIGHT,imported_ready?"validated":(imported_attempted?"rejected":"absent"),show_snapshot?"shown":(show_path?"controlled path active":(show_divide?"source-bounded Divide committed":(show_scenario?"original World pre-setup 27 loaded":"not shown"))));break;}
  struct timespec t={0,16000000};nanosleep(&t,NULL);
 }
 if(joystick>=0)close(joystick);
 if(font)XFreeFont(display,font);
 XFreeGC(display,graphics);XDestroyWindow(display,window);XCloseDisplay(display);
 return 0;
}
#endif
