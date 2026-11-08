/* EXEC0044: UPDATED standalone NATIVE 1280x720 cross-platform Genghis Khan shell.
 * Copyright-clean PLACEHOLDER graphics and no sound assets. NOT a complete game.
 * Linux/X11 native compilation executed; Win32/XInput code provided but NOT
 * Windows-compiled or physically controller-tested in this run.
 */
#define _POSIX_C_SOURCE 200809L
#include "gk80_full_checkpoint.h"
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
#include "gk69_mesen_ram_map.h"
#include "gk74_spring_archive.h"
#include "gk77_stock_spring.h"
#include "gk81_stock_attach.h"
#include "gk82_stock_cartridge.h"
#include "gk83_reset_trace.h"
#include "gk84_reset_hypothesis.h"
#include "gk58_slots.h"
#include "gk59_storage.h"
#include "gk60_demobilize.h"
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
#define GK1_PORT_VERSION "0.85.0-dev1"
#define WIDTH 1280
#define HEIGHT 720
#define BG UINT32_C(0x10202b)
#define EDGE UINT32_C(0xb4a16b)
#define FG UINT32_C(0xe5d7a5)
#define ACTIVE UINT32_C(0x386f58)
#define WARN UINT32_C(0x8b4743)
static Gk1App app;
static uint8_t campaign_slot=1; /* port-only; selects a separate non-overwriting pre-setup file */
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
/* EXEC0069: EXACT already-observed modified NES WorkRAM, private file only.
 * An edited/mismatched capture never becomes apparent stock ROM evidence. */
static Gk69MesenMap mesen_map;
static int mesen_ready=0,mesen_attempted=0,mesen_show_names=0;
/* EXEC0074: private archived 1175 RAM is optional; no source bytes are in
 * the public build or persisted in country/campaign state. */
static Gk74SpringArchive spring_archive;
static int spring_ready=0,spring_attempted=0;
/* EXEC0077: exact authenticated original stock Mesen country records;
 * supplied by owner externally, never embedded in binary or normal save. */
static Gk77StockSpring stock_spring;
static int stock_ready=0,stock_attempted=0;
static Gk77Profile stock_selected=GK77_FRESH_MONGOL_WORKRAM;
static char stock_source_path[2300];
static char stock_root[2100];
/* Native process user-data root. Never use executable folder, relative CWD,
 * or bundled CSV directory for private Mesen evidence. */
static const char *private_stock_root(void){
#ifdef _WIN32
 const char *p=getenv("LOCALAPPDATA");if(!p||!*p)p=getenv("APPDATA");
 if(p&&*p&&strlen(p)<sizeof stock_root){
  snprintf(stock_root,sizeof stock_root,"%s",p);return stock_root;
 }
#else
 const char *p=getenv("XDG_DATA_HOME");
 if(p&&p[0]=='/'&&strlen(p)<sizeof stock_root){
  snprintf(stock_root,sizeof stock_root,"%s",p);return stock_root;
 }
 p=getenv("HOME");
 if(p&&p[0]=='/'&&strlen(p)+sizeof("/.local/share")<sizeof stock_root){
  snprintf(stock_root,sizeof stock_root,"%s/.local/share",p);return stock_root;
 }
#endif
 return NULL;
}
/* Source selection changes only read-only evidence; no country/world mutation.
 * All three profiles must independently pass original-stock SHA before use.
 * Explicit legacy path/profile is fail-closed, never reinterpreted as another.
 */
static int try_stock_profile(Gk77Profile requested){
 const char *external=getenv("GK1_STOCK_SPRING_RECORDS");
 Gk77Profile explicit_profile=gk77_profile_parse(getenv("GK1_STOCK_SPRING_PROFILE"));
 char candidate_path[2300]={0};
 Gk77StockSpring candidate;
 Gk81AttachResult r=gk81_stock_attach(private_stock_root(),external,explicit_profile,
                                     requested,&candidate,candidate_path,sizeof candidate_path);
 stock_attempted=1;stock_selected=requested;
 if(r!=GK81_ATTACH_OK){
  stock_ready=0;stock_source_path[0]='\0';app.spring_stock_count=0;
  if(app.screen==GK1_SCREEN_SPRING_ARCHIVE){
   snprintf(app.status,sizeof app.status,
      "PRIVATE STOCK %s: %s (NO WORLD MUTATION)",gk77_profile_name(requested),
      r==GK81_ATTACH_BAD_SOURCE?"SHA / SIZE REJECTED":"FILE MISSING / PROFILE BLOCKED");
  }
  return 0;
 }
 stock_spring=candidate;
 memcpy(stock_source_path,candidate_path,strlen(candidate_path)+1u);
 stock_ready=1;app.spring_stock_count=candidate.country_count;
 app.spring_archive_cursor=0;
 if(app.screen==GK1_SCREEN_SPRING_ARCHIVE)
  snprintf(app.status,sizeof app.status,"ORIGINAL STOCK %s: SHA256 VERIFIED; A RESTORE; LB/RB SWITCH",gk77_profile_name(requested));
 return 1;
}
static void load_optional_stock_spring(void){
 const char *external=getenv("GK1_STOCK_SPRING_RECORDS");
 if(external&&*external){
  (void)try_stock_profile(gk77_profile_parse(getenv("GK1_STOCK_SPRING_PROFILE")));
  return;
 }
 for(int p=1;p<=3;p++)if(try_stock_profile((Gk77Profile)p))return;
 /* No auto-creation and no gameplay unlocked from missing files. */
 stock_selected=GK77_FRESH_MONGOL_WORKRAM;
}
static void cycle_stock_profile(int direction){
 if(app.screen!=GK1_SCREEN_SPRING_ARCHIVE)return;
 if(!app.spring_stock_mode){
  strcpy(app.status,"VIEW SWITCHES TO STOCK SOURCE MODE BEFORE LB/RB SELECTION");return;
 }
 int next=((int)stock_selected-1+(direction<0?2:1))%3+1;
 /* A previously staged checkpoint is never silently relabeled. */
 memset(&app.stock_checkpoint,0,sizeof app.stock_checkpoint);
 (void)try_stock_profile((Gk77Profile)next);
}

static void load_optional_spring_archive(void){
 const char *path=getenv("GK1_1175_RAM_FILE");
 if(!path||!*path)return;
 spring_attempted=1;
 spring_ready=(gk74_load_file(path,&spring_archive)==GK74_OK);
}

static void load_optional_mesen_map(void){
 const char *p=getenv("GK1_MESEN_WORKRAM_FILE");
 if(!p||!*p)return;
 mesen_attempted=1;
 mesen_ready=(gk69_map_from_file(p,&mesen_map)==GK69_MAP_OK);
}

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

/* Windows Program Files is read-only for ordinary users: every DEFAULT
 * mutable slot must live under the user's LocalAppData, never beside the EXE.
 * Explicit environment overrides are preserved verbatim for migrations/QA.
 * An invalid profile path does not fall back into an unsafe relative path.
 */
#ifdef _WIN32
static char gk59_win_paths[4][2300];
static const char *gk59_profile_default(unsigned which,const char *filename){
 if(which>=4)return NULL;
 const char *base=getenv("LOCALAPPDATA");
 if(!base||!*base)base=getenv("APPDATA");
 if(!base||!*base)return NULL;
 char *result=gk59_win_paths[which];
 if(!gk59_user_file(base,filename,result,sizeof gk59_win_paths[which]))return NULL;
 /* Make exactly the application directory, not arbitrary parent directories. */
 char *slash=strrchr(result,'\\');
 if(!slash)return NULL;
 *slash='\0';
 DWORD attr=GetFileAttributesA(result);
 if(attr==INVALID_FILE_ATTRIBUTES){
  if(!CreateDirectoryA(result,NULL) && GetLastError()!=ERROR_ALREADY_EXISTS){*slash='\\';return NULL;}
  attr=GetFileAttributesA(result);
 }
 *slash='\\';
 if(attr==INVALID_FILE_ATTRIBUTES||(attr&FILE_ATTRIBUTE_DIRECTORY)==0)return NULL;
 return result;
}
#endif
static const char *divide_replay_path(void){
 const char *s=getenv("GK1_DIVIDE_REPLAY_FILE");
 if(s&&*s)return s;
#ifdef _WIN32
 return gk59_profile_default(0,"gk1_mod_divide_replay_slot1.gkd");
#else
 return "gk1_mod_divide_replay_slot1.gkd";
#endif
}
static Gk1StoreResult save_divide_to(const char *path){
 if(!path){strcpy(app.status,"USER DATA FOLDER UNAVAILABLE - NO FILE TOUCHED");return GK1_STORE_BAD;}
 Gk1StoreResult r=gk48_divide_save_new(path,&app);
 if(r==GK1_STORE_OK)strcpy(app.status,"DIVIDE REPLAY SAVED 64 BYTES - NOT CAMPAIGN");
 else if(r==GK1_STORE_EXISTS)strcpy(app.status,"DIVIDE SAVE EXISTS - NO OVERWRITE");
 else strcpy(app.status,"DIVIDE SAVE REJECTED - UNSUPPORTED STATE / I-O");
 return r;
}
static Gk1StoreResult load_divide_from(const char *path){
 if(!path){strcpy(app.status,"USER DATA FOLDER UNAVAILABLE - NO FILE TOUCHED");return GK1_STORE_BAD;}
 Gk1StoreResult r=gk48_divide_load(path,&app);
 if(r!=GK1_STORE_OK)strcpy(app.status,r==GK1_STORE_CORRUPT?
    "DIVIDE FILE INVALID; STATE UNCHANGED":"DIVIDE SAVE NOT AVAILABLE");
 return r;
}
static const char *path_replay_path(void){
 const char *s=getenv("GK1_PATH_REPLAY_FILE");
 if(s&&*s)return s;
#ifdef _WIN32
 return gk59_profile_default(1,"gk1_observed_path_slot1.gkp");
#else
 return "gk1_observed_path_slot1.gkp";
#endif
}
static Gk1StoreResult save_path_to(const char *path){
 if(!path){strcpy(app.status,"USER DATA FOLDER UNAVAILABLE - NO FILE TOUCHED");return GK1_STORE_BAD;}
 Gk1StoreResult r=gk46_path_save_new(path,&app);
 if(r==GK1_STORE_OK)strcpy(app.status,"MODIFIED ROM PATH REPLAY SAVED (64 BYTES) - NOT CAMPAIGN");
 else if(r==GK1_STORE_EXISTS)strcpy(app.status,"PATH FILE EXISTS - NO OVERWRITE");
 else strcpy(app.status,"PATH SAVE REJECTED - WRONG STAGE / I-O");
 return r;
}
static Gk1StoreResult load_path_from(const char *path){
 if(!path){strcpy(app.status,"USER DATA FOLDER UNAVAILABLE - NO FILE TOUCHED");return GK1_STORE_BAD;}
 Gk1StoreResult r=gk46_path_load(path,&app);
 if(r!=GK1_STORE_OK)strcpy(app.status,r==GK1_STORE_CORRUPT?
  "PATH CORRUPT / WRONG MOD PROFILE - STATE UNCHANGED":"PATH REPLAY NOT AVAILABLE");
 return r;
}
static const char *replay_path(void){
 const char *s=getenv("GK1_REPLAY_FILE");
 if(s&&*s)return s;
#ifdef _WIN32
 return gk59_profile_default(2,"gk1_research_slot1.gkr");
#else
 return "gk1_research_slot1.gkr";
#endif
}
static Gk1StoreResult save_replay_to(const char *path){
 if(!path){strcpy(app.status,"USER DATA FOLDER UNAVAILABLE - NO FILE TOUCHED");return GK1_STORE_BAD;}
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
 if(!path){strcpy(app.status,"USER DATA FOLDER UNAVAILABLE - NO FILE TOUCHED");return GK1_STORE_BAD;}
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
 label(62,96,app.screen==GK1_SCREEN_STOCK_CONTINUE?
  "EXEC0084 STOCK CHECKPOINT + ORIGINAL ROM BOOT SOURCE VERIFICATION":
  "RESEARCH BUILD / ORIGINAL SPRING INITIALIZATION OPEN",EDGE);
 if(app.screen==GK1_SCREEN_TITLE){
  frame(120,138,1038,466);
  menu_line(223,app.menu_index==0,"TWO ORIGINAL CAMPAIGNS - PRE-SETUP DATA");
  menu_line(289,app.menu_index==1,"PREVIOUS BATTLE RESEARCH / SNAPSHOT VIEWER");
  menu_line(355,app.menu_index==2,"CONTROLLED TWO-STEP TACTICAL PATH (MOD ROM)");
  menu_line(421,app.menu_index==3,"CONTROLLER-DRIVEN DIVIDE / ZERO-SOLDIER OUTCOME (MOD)");
  menu_line(487,app.menu_index==4,"MOD ROM SPRING REST A/B SOURCE CASE (1175)");
  label(150,536,"XBOX DPAD / STICK NAVIGATE | A CONFIRM | B CANCEL",FG);
 }else if(app.screen==GK1_SCREEN_REST_CASE){
  const Gk68RestCase *r=&app.rest_case;
  frame(62,129,1152,479);
  label(93,181,"AUTHENTIC APPROVED MOD NES REST CASE - FONZARELLI 1175",FG);
  label(93,225,"ONE SOURCE SNAPSHOT ONLY. NOT STOCK, FORMULA OR PLAYABLE CAMPAIGN.",WARN);
  snprintf(line,sizeof line,"YEAR %u  SEASON %s  ORDERS %u  SHARED BP %u",r->year,
    r->season?"SUMMER":"SPRING",r->orders,r->bp);label(95,282,line,FG);
  snprintf(line,sizeof line,"RATE %u  SKILL %u  ECONOMY %u",r->rate,r->skill,r->economy);
  label(95,330,line,EDGE);
  snprintf(line,sizeof line,"GOLD %u  FOOD %u  TROOPS %u",r->gold,r->food,r->troops);
  label(95,378,line,FG);
  label(95,442,r->stage==GK68_REST_CONFIRM?"CONFIRM REST: A YES / B NO":
    r->stage==GK68_REST_SUMMER?"CAPTURED SUMMER (A NO-OP / B EXIT)":
    "A OPEN SOURCE REST PROMPT / B RETURN",FG);
  label(95,497,"CANCEL REVERTS COMPLETE FIXTURE; NEVER CHANGES 41 COUNTRY TEMPLATES",EDGE);
  label(95,549,"LONG MESEN 8000-FRAME RUN CAPTURED; CLIENT SHUTDOWN TIMED OUT",WARN);
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
  Gk64SeasonGeography weather={0};
  int weather_ok=gk64_season_geography(world,(uint8_t)cursor,&weather)==GK64_OK;
  snprintf(line,sizeof line,"WINTER COLD: %s  SUMMER STORMS: %s (ELIGIBLE ONLY)",
   !weather_ok?"UNKNOWN":(weather.winter_severe_cold_eligible?"YES":"NO"),
   !weather_ok?"UNKNOWN":(weather.summer_storm_eligible?"YES":"NO"));
  label(604,475,line,EDGE);
  if(app.setup_candidate[app.active_world]!=255){
    unsigned ci=app.setup_candidate[app.active_world];
    snprintf(line,sizeof line,"CANDIDATE: %u %.24s (PENDING ONLY)",ci+1,
       world->countries[ci].lord_name);label(604,505,line,EDGE);
  }else label(604,505,"CANDIDATE: NONE (PENDING ONLY)",EDGE);
  snprintf(line,sizeof line,"A CANDIDATE / START SETUP / VIEW LABOR / X SAVE / Y LOAD SLOT %u",(unsigned)campaign_slot);
  label(604,537,line,FG);
  label(604,565,"LB/RB SELECT SAVE SLOT 1-3 / B BACK",FG);
  label(84,576,"PRE-SETUP SNAPSHOT ONLY; NO VERIFIED PLAYER/DIFFICULTY/SEASON SETUP",WARN);
 }else if(app.screen==GK1_SCREEN_LABOR_PLAN){
  const Gk1LoadedScenario *world=&app.worlds[app.active_world];
  const Gk1CountryTemplate *c=&world->countries[app.country_cursor[app.active_world]];
  const char *names[5]={"TROOPS","TOWN","MASONS","ARTISANS","FOOD WORKERS"};
  unsigned vals[5]={c->troops,c->town,c->masons,c->artisans,c->food_workers};
  frame(62,130,1152,475);
  snprintf(line,sizeof line,"PORT-ONLY PRE_SETUP WORKFORCE PLANNER - %s / %s",
   app.active_world?"WORLD":"MONGOL",c->country_name);
  label(94,174,line,FG);
  label(94,215,"DRAFTED LABOR POOLS - NO NES ASSIGN ORDER EXECUTED",WARN);
  for(unsigned i=0;i<5;i++){
   int y=262+(int)i*55;
   if(i==app.labor_from)fillrect(103,y-27,520,37,ACTIVE);
   snprintf(line,sizeof line,"%s  %u",names[i],vals[i]);label(125,y,line,FG);
  }
  snprintf(line,sizeof line,"FROM: %s   TO: %s",names[app.labor_from],names[app.labor_to]);
  label(670,285,line,FG);
  snprintf(line,sizeof line,"DRAFTED TOTAL: %u  RESERVE: %u",c->drafted_population,c->undrafted_reserve);
  label(670,335,line,EDGE);
  label(670,396,"LEFT/RIGHT SOURCE | UP/DOWN TARGET",FG);
  snprintf(line,sizeof line,"A TRANSFER / B BACK / X SAVE Y LOAD SLOT %u / LB RB CHANGE",(unsigned)campaign_slot);
  label(670,438,line,FG);
  label(104,527,"START = ENHANCED DEMOBILIZE PREVIEW (CLASSIC BLOCKED)",EDGE);
  label(104,557,"PRE-SETUP EDIT ONLY. NO ORIGINAL MOBILITY, ORDER, COST OR SPRING CLAIM.",WARN);
 }else if(app.screen==GK1_SCREEN_DEMOBILIZE){
  const Gk1LoadedScenario *world=&app.worlds[app.active_world];
  const Gk1CountryTemplate *c=&world->countries[app.country_cursor[app.active_world]];
  frame(62,130,1152,475);
  snprintf(line,sizeof line,"ENHANCED MOD DEMOBILIZE PREVIEW - %s",c->country_name);label(94,175,line,FG);
  label(94,220,"SOURCE-VERIFIED TRANSFER ONLY; NOT NES FIRST PLAYABLE SEASON",WARN);
  snprintf(line,sizeof line,"TROOPS: %u   UNDRAFTED RESERVE: %u",(unsigned)c->troops,(unsigned)c->undrafted_reserve);
  label(94,277,line,FG);
  snprintf(line,sizeof line,"QUANTITY: %u   MAX ALLOWED: %u",(unsigned)app.demob_quantity,(unsigned)gk60_demob_max(c));
  label(94,326,line,EDGE);
  snprintf(line,sizeof line,"DRAFTED: %u   COMBINED: %u   ELIGIBLE RESERVE: %u",
   (unsigned)c->drafted_population,(unsigned)c->combined_population,(unsigned)c->eligible_draft_count);
  label(94,375,line,FG);
  label(94,424,"UP/DOWN +/-1  RIGHT/LEFT +/-10  A COMMIT  B/START BACK",FG);
  snprintf(line,sizeof line,"X SAVE / Y LOAD PRE-SETUP SLOT %u; LB/RB SLOT",(unsigned)campaign_slot);
  label(94,463,line,FG);
  label(94,557,"CLASSIC STOCK UNCHANGED. THIS IS NOT A PROVEN SEASONAL ORDER COST.",WARN);
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
  snprintf(line,sizeof line,"%s",app.manual_bonus_active[w]?"A +1 / B +10 / X -1 / Y -10":
    "DPAD / A CONFIRM / B BACK / X SAVE / Y LOAD / LB RB SLOTS");
  label(113,456,line,FG);
  label(113,500,"NO SEASONAL PROCESSING OR UNVERIFIED ABILITY ROLLS PERFORMED",WARN);
 }else if(app.screen==GK1_SCREEN_MARKET_EXPLORER){
  static const char *names[]={"CHINA","ISLAM","UIGHUR"};
  Gk61Quote quote={0};
  Gk61Result qr=gk61_quote(app.example_rate,(Gk61Merchant)app.market_merchant,app.market_item,&quote);
  frame(70,143,1130,445);
  label(104,194,"ORIGINAL 3 MERCHANTS x 17 ITEMS - RESEARCH QUOTES ONLY",FG);
  snprintf(line,sizeof line,"MERCHANT: %s   COLUMN: %u / 17   ILLUSTRATIVE RATE: %u",
   names[app.market_merchant],(unsigned)app.market_item+1,(unsigned)app.example_rate);
  label(104,251,line,EDGE);
  if(qr==GK61_OK){
   snprintf(line,sizeof line,"BUY %u.%u GOLD    SELL %u.%u GOLD",
    (unsigned)quote.buy_tenths/10,(unsigned)quote.buy_tenths%10,
    (unsigned)quote.sell_tenths/10,(unsigned)quote.sell_tenths%10);
   label(104,307,line,FG);
  }
  snprintf(line,sizeof line,"VIEW: %s (NEVER TRANSFERRED)",app.market_sell?"SELL":"BUY");label(104,357,line,FG);
  label(104,407,"LEFT/RIGHT MERCHANT  UP/DOWN ITEM  A BUY/SELL  START/VIEW RATE",FG);
  label(104,453,"B RETURN TO COUNTRY LABOR SCREEN",FG);
  label(104,511,"NO SOURCE MERCHANT PRESENCE, OFFERS, RNG, OR FIRST SPRING",WARN);
 }else if(app.screen==GK1_SCREEN_MANUAL_RULES){
  uint8_t w=app.active_world,ix=app.setup_candidate[w];
  Gk64ManualPreview m={0};
  int ok=w<=1&&app.world_ready[w]&&gk64_preview(&app.worlds[w],ix,
    app.manual_remaining_orders,&m)==GK64_OK;
  frame(72,135,1134,473);
  label(104,184,"KOEI NES MANUAL RULES - LOADED PRE-SETUP DATA ONLY",FG);
  if(ok){
   snprintf(line,sizeof line,"FIRST PLAYER ORDERS: SPRING %u  / BASE 3  / DIRECT VASSAL 1",
    (unsigned)m.first_order_year);label(104,230,line,EDGE);
   snprintf(line,sizeof line,"PEOPLE %u: FEED %u%s FOOD (ROUNDING UNRESOLVED)",
    (unsigned)m.population,(unsigned)m.feed_whole,m.feed_half?".5":"");label(104,279,line,FG);
   snprintf(line,sizeof line,"TROOPS %u: SPRING GOLD WAGE %u (BEFORE TAX/LOSSES)",
    (unsigned)m.troops,(unsigned)m.soldier_gold_wages);label(104,323,line,FG);
   snprintf(line,sizeof line,"CURRENT TEMPLATE GOLD %u / FOOD %u",(unsigned)m.current_gold,
    (unsigned)m.current_food);label(104,367,line,FG);
   snprintf(line,sizeof line,"REMAINING ORDERS: %u -> BATTLE MAX DAYS %u (30/60/90)",
    (unsigned)app.manual_remaining_orders,(unsigned)m.days_for_remaining_orders);
   label(104,414,line,EDGE);
  }else label(104,290,"SOURCE / COUNTRY / ORDER PREVIEW INVALID",WARN);
  Gk64SeasonGeography weather={0};
  if(w<=1&&app.world_ready[w]&&gk64_season_geography(&app.worlds[w],ix,&weather)==GK64_OK){
    snprintf(line,sizeof line,"MANUAL P22-23: WINTER COLD %s / SUMMER STORMS %s (ELIGIBILITY)",
       weather.winter_severe_cold_eligible?"YES":"NO",weather.summer_storm_eligible?"YES":"NO");
    label(104,459,line,EDGE);
  }
  label(104,501,"A ARCHIVED SPRING / VIEW ORDER BUDGET / START BATTLE / B BACK",FG);
  label(104,547,"NO HAZARD RNG, INITIAL SPRING OR TAX/GROWTH OUTCOMES CLAIMED",WARN);
 }else if(app.screen==GK1_SCREEN_STOCK_CONTINUE){
  const Gk79Checkpoint *cp=&app.stock_checkpoint;
  frame(72,135,1134,473);
  if(cp->valid && cp->view_index<cp->world.country_count){
   const Gk1CountryTemplate *c=&cp->world.countries[cp->view_index];
   snprintf(line,sizeof line,"ORIGINAL STOCK %u SPRING CHECKPOINT - %s / %u COUNTRIES",
    (unsigned)cp->year,gk77_profile_name((Gk77Profile)cp->profile),
    (unsigned)cp->world.country_count);label(104,185,line,EDGE);
   label(104,216,"VERIFIED RECORD RESTORATION, NOT RECONSTRUCTED STARTUP / RNG",WARN);
   if(cp->raw_view){
    snprintf(line,sizeof line,"COUNTRY %u / %u ORIGINAL 83 BYTES: PRIVATE RAM, SOURCE %s",
     (unsigned)cp->view_index+1u,(unsigned)cp->world.country_count,
     gk77_profile_name((Gk77Profile)cp->profile));label(104,250,line,EDGE);
    const uint8_t *raw=cp->stock_raw[cp->view_index];
    for(unsigned row=0;row<6u;row++){
     size_t off=row*14u,pos=0u;
     pos+=(size_t)snprintf(line,sizeof line,"+%02X:",(unsigned)off);
     for(unsigned j=0;j<14u && off+j<GK77_RECORD_BYTES;j++)
      pos+=(size_t)snprintf(line+pos,sizeof line-pos," %02X",(unsigned)raw[off+j]);
     label(104,286+(int)row*44,line,FG);
    }
    label(104,568,"VIEW DECODED / DPAD COUNTRY / A PASS / B RETURN",WARN);
   }else{
   snprintf(line,sizeof line,"COUNTRY %u / %u: %.32s   SELECTED LORD COUNTRY %u",
    (unsigned)cp->view_index+1u,(unsigned)cp->world.country_count,
    c->country_name,(unsigned)cp->player_index+1u);label(104,271,line,EDGE);
   snprintf(line,sizeof line,"GOLD %u   FOOD %u   TROOPS %u   RATE %u",
    (unsigned)c->gold,(unsigned)c->goods[10],(unsigned)c->troops,(unsigned)cp->rate);
   label(104,317,line,FG);
   snprintf(line,sizeof line,"ARMS %u  SKILL %u  ECONOMY %u  TOWN %u  MORALE %u",
    (unsigned)c->arms,(unsigned)c->skill,(unsigned)c->economy,
    (unsigned)c->town,(unsigned)c->morale);label(104,361,line,FG);
   snprintf(line,sizeof line,"BASE ORDERS: %u REMAIN / %u COMPLETED / ENDED %s",
    (unsigned)cp->orders.remaining,(unsigned)cp->orders.completed,
    cp->orders.ended?"YES":"NO");label(104,410,line,EDGE);
   label(104,457,cp->orders.ended?
    "PASS COMPLETED: 0 ORDERS REMAIN. B RETURN / DPAD INSPECT":
    (cp->pending_pass?"CONFIRM PASS: A YES / B CANCEL":
    "DPAD BROWSE / A PASS REMAINING BASE ORDERS (DOUBLE CONFIRM)"),FG);
   label(104,501,"START VERIFY PRIVATE STOCK ROM; LB/RB SLOT X SAVE Y RESTORE",WARN);
   label(104,565,"VIEW RAW 83 BYTES / B RETURN / WORLD TEMPLATES UNCHANGED",FG);
   }
  }else label(104,285,"STOCK CHECKPOINT INVALID: NO SIMULATED PLAY",WARN);
 }else if(app.screen==GK1_SCREEN_SPRING_ARCHIVE){
  unsigned ix=app.spring_archive_cursor;
  const Gk1LoadedScenario *w=&app.worlds[0];
  frame(72,135,1134,473);
  if(app.spring_stock_mode&&stock_ready&&ix<stock_spring.country_count){
   const Gk77Country *country=&stock_spring.countries[ix];
   const unsigned widx=stock_spring.scenario-1u;
   const Gk1CountryTemplate *pre=&app.worlds[widx].countries[ix];
   snprintf(line,sizeof line,"ORIGINAL STOCK NES SPRING %u / %u-COUNTRY %s PROFILE",
    stock_spring.year,(unsigned)stock_spring.country_count,gk77_profile_name(stock_spring.profile));label(104,180,line,EDGE);
   label(104,216,stock_spring.profile==GK77_FRESH_MONGOL_WORKRAM?
     "ORIGINAL STOCK NES LIVE RESET: nesWorkRam BANK, NOT PORT GAMEPLAY":
     "ORIGINAL STOCK NES ARCHIVED CHECKPOINT: nesSaveRam BANK",WARN);
   snprintf(line,sizeof line,"COUNTRY %u / %u  %.36s  RATE %u  ORDERS %u",
    ix+1,(unsigned)stock_spring.country_count,pre->country_name,
    (unsigned)stock_spring.rate,(unsigned)stock_spring.orders);label(104,267,line,EDGE);
   snprintf(line,sizeof line,"GOLD %u -> %u     FOOD %u -> %u",
    (unsigned)pre->gold,(unsigned)country->gold,
    (unsigned)pre->goods[10],(unsigned)country->food);label(104,312,line,FG);
   snprintf(line,sizeof line,"TROOPS %u -> %u   ARMS %u -> %u   SKILL %u -> %u",
    (unsigned)pre->troops,(unsigned)country->troops,
    (unsigned)pre->arms,(unsigned)country->arms,
    (unsigned)pre->skill,(unsigned)country->skill);label(104,358,line,FG);
   snprintf(line,sizeof line,"ECONOMY %u -> %u   TOWN %u -> %u   MORALE %u -> %u",
    (unsigned)pre->economy,(unsigned)country->economy,
    (unsigned)pre->town,(unsigned)country->town,
    (unsigned)pre->morale,(unsigned)country->morale);label(104,403,line,FG);
   snprintf(line,sizeof line,"FUR %u / SILK %u / FOOD WORKERS %u / UNDRAFTED %u",
    (unsigned)country->goods[0],(unsigned)country->goods[1],
    (unsigned)country->food_workers,(unsigned)country->undrafted);label(104,448,line,FG);
   label(104,491,"EXACT HASH PROVENANCE; NEVER INITIALIZES GAME OR COUNTS AS PLAYER TURN",WARN);
   label(104,566,"DPAD COUNTRY / LB-RB PRIVATE PROFILE / A RESTORE / B BACK",FG);
  }else if(app.spring_stock_mode&&!stock_ready){
   label(104,181,"PRIVATE STOCK NES SOURCE MISSING OR NOT SHA256-VERIFIED",WARN);
   label(104,235,"LB / RB: SELECT FRESH 1175, ARCHIVED 1175, WORLD 1206",FG);
   label(104,280,"LOAD YOUR OWN EXACT 83-BYTE COUNTRY RECORD EXPORTS",FG);
   label(104,326,"USER-DATA / GK1NativeResearch / stock_*.records",FG);
   label(104,372,"NO DOWNLOAD, NO GENERATED INITIAL STATS, NO GAME STATE MUTATION",WARN);
   label(104,567,"LB/RB RETRY PRIVATE PROFILE  /  VIEW ARCHIVE  /  B BACK",FG);
  }else{
  label(104,181,app.spring_preview_mode?"ORIGINAL 6502 SPRING FOOD/TAX/PAY (SOURCE-BOUNDED)":"SPRING 1175 ARCHIVE CROSSWALK - ROM IDENTITY UNVERIFIED",EDGE);
  label(104,216,app.spring_preview_mode?"WHAT-IF ON PRE_SETUP NUMBERS; NOT ACTUAL FIRST SPRING OR LIVE TURN":"READ ONLY: PRE_SETUP DATA VS LATER ARCHIVED WORKRAM (NOT INITIAL SPRING)",WARN);
  if(app.spring_preview_mode && w->phase==GK1_PRE_SETUP_TEMPLATE && w->country_count==14){
   const Gk1CountryTemplate *pre=&w->countries[ix];
   Gk75SpringResult v;
   snprintf(line,sizeof line,"COUNTRY %u / 14: %.34s",ix+1,pre->country_name);label(104,264,line,EDGE);
   if(gk75_template_projection(pre,&v)==GK75_OK){
    snprintf(line,sizeof line,"FOOD: %u -> %u  (CONSUMED %u)",pre->goods[10],v.after.food,v.food_consumed);label(104,307,line,FG);
    snprintf(line,sizeof line,"TAX RECEIPTS %u (ECONOMY %u / TAX %u / MORALE %u)",v.gross_tax,pre->economy,pre->tax,pre->morale);label(104,348,line,FG);
    snprintf(line,sizeof line,"TROOPS: %u -> %u  (PAYROLL %u / LOST %u)",pre->troops,v.after.troops,v.payroll,v.troops_lost);label(104,390,line,FG);
    snprintf(line,sizeof line,"GOLD: %u -> %u; FOOD SHORTAGE: %s",pre->gold,v.after.gold,v.starved?"YES":"NO");label(104,433,line,FG);
    label(104,474,"NO COMMON LABOR, RNG, SICKNESS, CHARACTER OR POPULATION GROWTH",WARN);
   }else label(104,313,"SOURCE VALUES OUTSIDE VERIFIED RANGE: NO PROJECTION",WARN);
  }else if(!spring_ready){
   label(104,296,spring_attempted?"CAPTURE HASH / LENGTH REJECTED - NO DATA IMPORTED":
    "NO PRIVATE 8192-BYTE ARCHIVE SUPPLIED (GK1_1175_RAM_FILE)",WARN);
  }else if(w->phase==GK1_PRE_SETUP_TEMPLATE && w->country_count==14){
   const Gk1CountryTemplate *pre=&w->countries[ix];
   const Gk74SpringCountry *arch=&spring_archive.countries[ix];
   snprintf(line,sizeof line,"COUNTRY %u OF 14: %.30s",ix+1,pre->country_name);label(104,260,line,EDGE);
   snprintf(line,sizeof line,"VALUE             PRE-SETUP TEMPLATE        ARCHIVED 1175");label(104,302,line,FG);
   snprintf(line,sizeof line,"GOLD                    %-8u                %-8u",(unsigned)pre->gold,(unsigned)arch->gold);label(104,338,line,FG);
   snprintf(line,sizeof line,"FOOD                    %-8u                %-8u",(unsigned)pre->goods[10],(unsigned)arch->food);label(104,374,line,FG);
   snprintf(line,sizeof line,"TROOPS                  %-8u                %-8u",(unsigned)pre->troops,(unsigned)arch->troops);label(104,410,line,FG);
   snprintf(line,sizeof line,"ARMS                    %-8u                %-8u",(unsigned)pre->arms,(unsigned)arch->arms);label(104,446,line,FG);
   snprintf(line,sizeof line,"SKILL                   %-8u                %-8u",(unsigned)pre->skill,(unsigned)arch->skill);label(104,482,line,FG);
   snprintf(line,sizeof line,"ECONOMY                 %-8u                %-8u",(unsigned)pre->economy,(unsigned)arch->economy);label(104,518,line,FG);
  }else label(104,301,"ORIGINAL 14-COUNTRY SOURCE INVALID - NO INFERENCE",WARN);
  label(104,567,"DPAD COUNTRY / VIEW VERIFIED STOCK / START WHAT-IF / B BACK",FG);
  } /* EXEC0077 original STOCK read-only screen branch */
 }else if(app.screen==GK1_SCREEN_ORDER_BUDGET_RESEARCH){
  Gk71Budget b=app.order_budget_research;
  const char *gov=b.governor==GK71_BASE?"BASE COUNTRY: LORD":
                  b.governor==GK71_DIRECT?"DIRECTLY GOVERNED COUNTRY":"PRINCE-RUN COUNTRY: AI";
  frame(72,135,1134,473);
  label(104,184,"MANUAL STRATEGIC ORDER BUDGET - RESEARCH, NOT PLAYABLE",FG);
  snprintf(line,sizeof line,"COUNTRY %u / %s / SPRING / GOVERNOR: %s",
   (unsigned)b.country_id,b.scenario==1?"MONGOL":"WORLD",gov);label(104,240,line,EDGE);
  snprintf(line,sizeof line,"SOURCE RULE: MAX %u ORDERS / REMAINING %u / COMPLETED %u",
   (unsigned)b.allowance,(unsigned)b.remaining,(unsigned)b.completed);label(104,290,line,FG);
  label(104,343,"LORD BASE: 3  |  DIRECT GOVERNMENT: 1  |  PRINCE: AI POLICY",EDGE);
  label(104,396,"PASS WOULD END THE COUNTRY'S ORDERS; PRE_SETUP BLOCKS IT",FG);
  label(104,449,"LEFT/RIGHT GOVERNOR VIEW   A TRY PASS (SAFE NO-OP)   B RETURN",FG);
  label(104,522,"ORIGINAL ABILITY SETUP, FIRST SPRING AND SEASONAL AI STILL OPEN",WARN);
 }else if(app.screen==GK1_SCREEN_BATTLE_PASS_DRILL){
  const Gk72BattlePass *bp=&app.battle_pass;
  frame(72,135,1134,473);
  label(104,184,"KOEI ENGLISH NES MANUAL P31 - BATTLE PASS MOBILITY DRILL",FG);
  label(104,224,"DEMO VALUES ONLY; NEVER ORIGINAL UNIT OR MAP/RNG EVIDENCE",WARN);
  snprintf(line,sizeof line,"%s CAVALRY: MOBILITY %u / CAP 15 / PASSED %s",
    bp->selected==0?">":" ",(unsigned)bp->mobility[0],bp->used[0]?"YES":"NO");
  label(104,295,line,bp->selected==0?EDGE:FG);
  snprintf(line,sizeof line,"%s INFANTRY / ARCHERS: MOBILITY %u / CAP 13 / PASSED %s",
    bp->selected==1?">":" ",(unsigned)bp->mobility[1],bp->used[1]?"YES":"NO");
  label(104,350,line,bp->selected==1?EDGE:FG);
  label(104,430,"LEFT/RIGHT SELECT; A PASS ONCE; START NEW DEMO ACTIVATION",FG);
  label(104,479,"B RETURN TO BATTLE DAY WHAT-IF; NO WORLD/SAVE CHANGES",WARN);
 }else if(app.screen==GK1_SCREEN_BATTLE_TIMELINE){
  Gk65Terminal b=app.battle_timeline;
  frame(72,135,1134,473);
  label(104,183,"ORIGINAL BATTLE DAY ORDER: ATTACKER THEN DEFENDER",FG);
  snprintf(line,sizeof line,"DAYS COMPLETED %u OF %u  |  PHASE: %s",
   (unsigned)b.days_complete,
   (unsigned)b.max_days,b.phase==GK65_ATTACKER_PHASE?"ATTACKER":
   b.phase==GK65_DEFENDER_PHASE?"DEFENDER":"TERMINAL");label(104,258,line,EDGE);
  static const char *event_names[]={"COMPLETE SIDE (NO RESULT)","COMMAND DESTROYED", "COMMAND FLED", "COMMAND STARVED", "COMMAND SURRENDERED", "COMMAND CAPTURED IN DUEL", "BOTH COMMANDERS DESTROYED"};
  unsigned event=app.whatif_event;
  snprintf(line,sizeof line,"HYPOTHETICAL: %s",event<=GK65_BOTH_COMMANDERS_DESTROYED?event_names[event]:"INVALID");
  label(104,322,line,FG);
  if(event>=GK65_COMMAND_DESTROYED&&event<=GK65_COMMAND_CAPTURED_DUEL){
   snprintf(line,sizeof line,"EXPLICIT VICTIM: %s",app.whatif_victim==GK65_ATTACKER_VICTIM?"ATTACKER":"DEFENDER");
   label(104,362,line,EDGE);
  }else label(104,362,"VICTIM: NONE (NOT APPLICABLE)",FG);
  if(b.outcome!=GK65_UNDECIDED){
   const char *result=b.outcome==GK65_ATTACKER_WON?"ATTACKER WINS":
      b.outcome==GK65_DEFENDER_WON?"DEFENDER WINS":
      b.outcome==GK65_DEFENDER_RETAINS_DOUBLE_KO?"DEFENDER RETAINS, ATTACKER CAPTURED":"TIMEOUT ATTACKER RETREATS";
   snprintf(line,sizeof line,"TERMINAL: %s",result);label(104,402,line,WARN);
  }else label(104,402,"UP/DOWN EVENT; LEFT/RIGHT VICTIM; A APPLY; B BACK",FG);
  label(104,472,"START PASS DRILL; MANUAL WHAT-IF, NOT NES CONQUEST OR CAMPAIGN",WARN);
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
  label(87,168,mesen_ready?"OBSERVED MODIFIED NES WorkRAM: 16x10 SOURCE GRID - READ ONLY":
        (imported_ready?"RAW 16x10 TILE HEX: PHASE-CAPTURE, NOT TERRAIN CLASSIFICATION":
        "16x10 RESEARCH GRID - PLACEHOLDER ART, NOT NATIVE NES TILES"),FG);
  for(int y=0;y<10;y++) for(int x=0;x<16;x++){
   if(mesen_ready){
    uint8_t raw=mesen_map.raw[y*16+x];
    uint32_t border=(mesen_map.x==x && mesen_map.y==y)?WARN:EDGE;
    fillrect(90+x*38,189+y*33,37,32,border);
    fillrect(92+x*38,191+y*33,33,28,BG);
    if(mesen_show_names)snprintf(line,sizeof line,"%s",
      raw==2?"PL":raw==3?"FR":raw==4?"MT":"??");
    else snprintf(line,sizeof line,"%02X",(unsigned)raw);
    label(97+x*38,211+y*33,line,FG);
   }else if(imported_ready){
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
  label(725,222,mesen_ready?"MOD NES: EXACT VERIFIED WORKRAM SHA256":
        (imported_ready?"LOADED SNAPSHOT: OBSERVED PHASE":"CONTROLLED RESEARCH FIXTURE"),FG);
  if(mesen_ready){
   uint8_t raw=mesen_map.raw[mesen_map.y*16+mesen_map.x];
   snprintf(line,sizeof line,"CURSOR: X=%u Y=%u   RAW=%02X",(unsigned)mesen_map.x,
            (unsigned)mesen_map.y,(unsigned)raw);label(725,265,line,FG);
   snprintf(line,sizeof line,"%s",gk69_map_label(raw));label(725,312,line,FG);
   snprintf(line,sizeof line,"RAW2 PLAINS: %u",(unsigned)mesen_map.source_plains);label(725,350,line,FG);
   snprintf(line,sizeof line,"RAW3 FOREST: %u",(unsigned)mesen_map.source_forest);label(725,388,line,FG);
   snprintf(line,sizeof line,"RAW4 MOUNTAIN: %u",(unsigned)mesen_map.source_mountain);label(725,426,line,FG);
   snprintf(line,sizeof line,"UNCLASSIFIED/HIGH-BIT: %u",(unsigned)mesen_map.unknown);label(725,464,line,FG);
   label(725,498,"DPAD: INSPECT CELL / A: VIEW FORMAT",FG);
   label(725,530,"B: RETURN / NO ORDERS OR SAVES",FG);
   label(725,562,"NOT STOCK; 0/1 NOT WATER-PROVEN",WARN);
   if(mesen_show_names)label(95,585,"PL=PLAINS FR=FOREST MT=MOUNTAIN OTHER=UNKNOWN",WARN);
  }else if(!imported_ready)label(725,258,gk1_app_case_label(app.case_index),FG);
  if(!mesen_ready && imported_ready){
   snprintf(line,sizeof line,"ROM PROFILE: %u",(unsigned)imported.capture.profile);label(725,258,line,FG);
   snprintf(line,sizeof line,"CAPTURE PHASE: %u",(unsigned)imported.capture.phase);label(725,304,line,FG);
   snprintf(line,sizeof line,"RECORDS ON MAP: %u",imported.valid_positions);label(725,344,line,FG);
   snprintf(line,sizeof line,"SOLDIER RECORDS: %u",imported.units_with_soldiers);label(725,384,line,FG);
   snprintf(line,sizeof line,"ZERO SOLDIER ON MAP: %u",imported.empty_record_positions);label(725,424,line,FG);
   snprintf(line,sizeof line,"OUTSIDE MAP: %u",imported.out_of_bounds_positions);label(725,460,line,FG);
   snprintf(line,sizeof line,"RAM $613E: %u",(unsigned)imported.capture.scratch_word);label(725,496,line,FG);
   label(725,528,"GREEN=POPULATED / RED=ZERO / GOLD=NONE",EDGE);
   label(725,560,"READ ONLY; B=RETURN; NO MOVE COMMANDS",WARN);
  }else if(!mesen_ready && app.has_session){
   snprintf(line,sizeof line,"SOLDIERS: %u",(unsigned)app.session.battle.mover.soldiers);label(725,312,line,FG);
   snprintf(line,sizeof line,"MOBILITY: %u",(unsigned)app.session.battle.mover.mobility);label(725,350,line,FG);
   snprintf(line,sizeof line,"WORD $613E: %u",(unsigned)app.session.battle.completion_word);label(725,388,line,FG);
   snprintf(line,sizeof line,"FRIENDLY SLOT: %u",(unsigned)app.session.battle.friendly_slot1.soldiers);label(725,426,line,FG);
   label(725,492,"A MOVE  /  B FINISH OR BACK",FG);
   label(725,525,"DPAD CASE  /  X SAVE  /  Y LOAD",FG);
  }
 }
 frame(62,628,1150,52);
 if(mesen_attempted&&!mesen_ready && app.screen==GK1_SCREEN_RESEARCH_BATTLE)
  label(78,658,"MESEN MAP REFUSED: ONLY EXACT APPROVED MODIFIED 8192-BYTE RAM",WARN);
 else if(mesen_ready&&app.screen==GK1_SCREEN_RESEARCH_BATTLE)
  label(78,658,"LIVE CAPTURE BY SHA; NOT STOCK/WATER/RNG PARITY. DPAD INSPECT; B RETURN",EDGE);
 else if(imported_attempted&&!imported_ready && app.screen==GK1_SCREEN_RESEARCH_BATTLE)
  label(78,658,"SNAPSHOT REJECTED: FORMAT / FILE / ROM HASH; PLACEHOLDER ONLY",WARN);
 else if(imported_ready && app.screen==GK1_SCREEN_RESEARCH_BATTLE)
  label(78,658,"VALIDATED FORMAT / CLAIMED ROM ID; NOT PROOF OF GAMEPLAY | B RETURN",EDGE);
 else label(78,658,app.status,(app.screen==GK1_SCREEN_UNIMPLEMENTED||app.screen==GK1_SCREEN_SCENARIO_COUNTRIES)?WARN:FG);
}
/* Original cartridge is owner-private. Only this SHA-verified static
 * source probe is permitted from the stock research checkpoint (START).
 * It does not change worlds, order allowance, season or controller profile. */
static void probe_original_cartridge(void){
 const char *explicit_path=getenv("GK1_ORIGINAL_STOCK_ROM");
 char derived[2300];
 const char *path=explicit_path&&*explicit_path?explicit_path:NULL;
 if(!path){
  if(!gk82_rom_path(private_stock_root(),derived,sizeof derived)){
   strcpy(app.status,"ORIGINAL NES ROM PATH INVALID; SOURCE NOT PROBED");return;
  }
  path=derived;
 }
 Gk82RomBootstrap boot;
 Gk82RomStatus r=gk82_probe_stock_rom(path,&boot);
 if(r==GK82_ROM_OK){
  Gk83Trace trace={0};
  Gk83Stop gate=gk83_trace_original_reset(path,20000u,&trace);
  if(gate==GK83_HARDWARE_GATE){
   snprintf(app.status,sizeof app.status,
    "STOCK RESET $%04X: %u CPU STEPS; HARDWARE $%04X; NOT PLAYABLE SPRING",
    (unsigned)boot.reset_vector,(unsigned)trace.instructions,
    (unsigned)trace.first_hardware_read);
  }else{
   snprintf(app.status,sizeof app.status,
    "STOCK ROM VERIFIED: 6502 %u STEPS, GATE %u AT $%04X; NO SPRING RNG",
    (unsigned)trace.instructions,(unsigned)gate,(unsigned)trace.stop_pc);
  }
 }else snprintf(app.status,sizeof app.status,
   "PRIVATE ORIGINAL ROM %s: NO PLAY OR SOURCE PHASE CHANGE",
   r==GK82_ROM_SHA?"SHA REFUSED":r==GK82_ROM_HEADER?"INES REFUSED":
   r==GK82_ROM_LENGTH?"LENGTH REFUSED":"MISSING/REFUSED");
}
static void input(Gk1Input command){
 if(app.screen==GK1_SCREEN_STOCK_CONTINUE && command==GK1_INPUT_SETUP){
   probe_original_cartridge();return;
 }
 if(mesen_ready && app.screen==GK1_SCREEN_RESEARCH_BATTLE){
  if(command==GK1_INPUT_UP)(void)gk69_map_move(&mesen_map,0,-1);
  else if(command==GK1_INPUT_DOWN)(void)gk69_map_move(&mesen_map,0,1);
  else if(command==GK1_INPUT_LEFT)(void)gk69_map_move(&mesen_map,-1,0);
  else if(command==GK1_INPUT_RIGHT)(void)gk69_map_move(&mesen_map,1,0);
  else if(command==GK1_INPUT_CONFIRM)mesen_show_names=!mesen_show_names;
  else if(command==GK1_INPUT_CANCEL)(void)gk1_app_input(&app,command);
  return;
 }
 /* A must only transition to the integrated stock checkpoint from a
  * previously exact-SHA verified PRIVATE original stock export. Neither
  * PRE_SETUP nor an incorrect ROM/profile can launch this 3-order session. */
 if(app.screen==GK1_SCREEN_SPRING_ARCHIVE && command==GK1_INPUT_CONFIRM &&
    app.spring_stock_mode){
  if(!stock_ready){strcpy(app.status,"ORIGINAL STOCK CHECKPOINT UNAVAILABLE: USE LB/RB TO IMPORT PRIVATE SOURCE");return;}
  uint8_t world=(uint8_t)(stock_spring.scenario-1u);
  uint8_t player=app.setup_candidate[world];
  if(player==255)player=0;
  Gk79Result r=gk79_open_original(&app.worlds[world],
      stock_source_path,stock_spring.profile,player,
      &app.stock_checkpoint);
  if(r!=GK79_OK){snprintf(app.status,sizeof app.status,
     "STOCK CHECKPOINT REFUSED (%u): SETUP/STOCK PROFILE NOT VALID",(unsigned)r);return;}
  app.screen=GK1_SCREEN_STOCK_CONTINUE;
  strcpy(app.status,"AUTHENTIC STOCK CHECKPOINT: PASS AVAILABLE; SEASON ENGINE UNFINISHED");
  return;
 }
 /* A verified snapshot is immutable reference data, NEVER a battle engine.
  * Only permit leaving the viewer. No research movement or fake completion.
  */
 if(imported_ready && app.screen==GK1_SCREEN_RESEARCH_BATTLE &&
    command!=GK1_INPUT_CANCEL)return;
 (void)gk1_app_input(&app,command);
}
static void replay_key(int save);
static int in_campaign_screens(void){
 return app.screen==GK1_SCREEN_CAMPAIGNS||app.screen==GK1_SCREEN_SCENARIO_COUNTRIES||
        app.screen==GK1_SCREEN_SETUP_WIZARD||app.screen==GK1_SCREEN_LABOR_PLAN||app.screen==GK1_SCREEN_DEMOBILIZE||
        app.screen==GK1_SCREEN_MARKET_EXPLORER||app.screen==GK1_SCREEN_MANUAL_RULES||
        app.screen==GK1_SCREEN_ORDER_BUDGET_RESEARCH||app.screen==GK1_SCREEN_STOCK_CONTINUE;
}
static void slot_action(int direction){
 if(app.screen==GK1_SCREEN_SPRING_ARCHIVE){cycle_stock_profile(direction);return;}
 if(!in_campaign_screens())return;
 campaign_slot=gk58_slot_cycle(campaign_slot,direction);
 snprintf(app.status,sizeof app.status,"SOURCE-BOUND 14/27 COUNTRY SNAPSHOT SLOT %u/3 - X CREATE / Y LOAD / NO OVERWRITE",(unsigned)campaign_slot);
}
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
  case GK1_PAD_ACT_X:
   if(app.screen==GK1_SCREEN_SETUP_WIZARD && app.manual_bonus_active[app.active_world])
    input(GK1_INPUT_DRAFT_UNDO_ONE);
   else replay_key(1);
   break;
  case GK1_PAD_ACT_Y:
   if(app.screen==GK1_SCREEN_SETUP_WIZARD && app.manual_bonus_active[app.active_world])
    input(GK1_INPUT_DRAFT_UNDO_TEN);
   else replay_key(0);
   break;
  case GK1_PAD_ACT_MENU:input(GK1_INPUT_SETUP);break;
  case GK1_PAD_ACT_VIEW:input(GK1_INPUT_LABOR_PLAN);break;
  case GK1_PAD_ACT_LB:slot_action(-1);break;
  case GK1_PAD_ACT_RB:slot_action(1);break;
 }
}

static const char *pre_setup_path_override;
static char selected_slot_path[1100];
static const char *pre_setup_file(void){
 if(pre_setup_path_override)return pre_setup_path_override;
 const char *file=getenv("GK1_PRE_SETUP_FILE");
 if(file&&*file)return file;
#ifdef _WIN32
 return gk59_profile_default(3,"gk1_presetup_slot1.gkp");
#else
 return "gk1_presetup_slot1.gkp";
#endif
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
 if(app.screen==GK1_SCREEN_STOCK_CONTINUE){
  const char *base=getenv("GK1_STOCK_CHECKPOINT_SAVE_FILE");
#ifdef _WIN32
  if(!base||!*base)base=gk59_profile_default(3,"gk1_stock_checkpoint_full_slot1.gk8");
#else
  if(!base||!*base)base="gk1_stock_checkpoint_full_slot1.gk8";
#endif
  if(!gk58_slot_path(base,campaign_slot,selected_slot_path,sizeof selected_slot_path)){
   strcpy(app.status,"STOCK SNAPSHOT SLOT PATH INVALID: NO FILE TOUCHED");return;
  }
  Gk80IO r=save?gk80_save_new(selected_slot_path,&app.stock_checkpoint):
                     gk80_load(selected_slot_path,&app.stock_checkpoint);
  snprintf(app.status,sizeof app.status,"FULL STOCK SPRING STATE %s: %s (SOURCE REATTACH REQUIRED)",
    save?"SAVE":"LOAD",r==GK80_IO_OK?"OK":r==GK80_IO_EXISTS?"EXISTS - NO OVERWRITE":
    r==GK80_IO_SOURCE_MISMATCH?"ORIGINAL SOURCE DIFFERS - UNCHANGED":
    r==GK80_IO_CORRUPT?"CORRUPT - STATE UNCHANGED":"UNAVAILABLE - STATE UNCHANGED");
  return;
 }
 if(app.screen==GK1_SCREEN_BATTLE_PASS_DRILL || app.screen==GK1_SCREEN_SPRING_ARCHIVE){
   strcpy(app.status,"READ-ONLY SPRING / MANUAL DRILL: SAVE/LOAD DISABLED");return;
 }
 if(app.screen==GK1_SCREEN_CAMPAIGNS||app.screen==GK1_SCREEN_SCENARIO_COUNTRIES||app.screen==GK1_SCREEN_SETUP_WIZARD||app.screen==GK1_SCREEN_LABOR_PLAN||app.screen==GK1_SCREEN_DEMOBILIZE){
  if(!gk58_slot_path(pre_setup_file(),campaign_slot,selected_slot_path,sizeof selected_slot_path)){
   strcpy(app.status,"PRE_SETUP SLOT PATH INVALID: NO FILE TOUCHED");return;
  }
  if(save)(void)save_pre_setup(selected_slot_path);
  else (void)load_pre_setup(selected_slot_path);
  return;
 }
 if((imported_ready||mesen_ready) && app.screen==GK1_SCREEN_RESEARCH_BATTLE){
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
 if(b&XINPUT_GAMEPAD_BACK)s.buttons|=GK1_PAD_VIEW;
 if(b&XINPUT_GAMEPAD_LEFT_SHOULDER)s.buttons|=GK1_PAD_LB;
 if(b&XINPUT_GAMEPAD_RIGHT_SHOULDER)s.buttons|=GK1_PAD_RB;
 s.lx=state.Gamepad.sThumbLX;
 /* XInput Y positive = up; portable pad router Y positive = down. */
 s.ly=state.Gamepad.sThumbLY==INT16_MIN?INT16_MAX:(int16_t)-state.Gamepad.sThumbLY;
 pad_actions(&s);
}
int WINAPI WinMain(HINSTANCE instance,HINSTANCE prev,LPSTR cmd,int show){
 (void)prev;(void)cmd;
 gk1_app_init(&app);gk1_pad_init(&pad);load_optional_battle_view();load_optional_mesen_map();load_optional_spring_archive();load_optional_stock_spring();(void)load_scenario_data();
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
    case 6:bit=GK1_PAD_VIEW;break;
    case 4:bit=GK1_PAD_LB;break;case 5:bit=GK1_PAD_RB;break;
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
 /* An offline reproducible ROM-derived 6502 bootstrap report. It is source
  * tracing only; no X11, NES hardware synthesis, or campaign state mutation. */
 if(argc==3 && strcmp(argv[1],"--audit-stock-reset")==0){
  Gk83Trace r={0};
  Gk83Stop status=gk83_trace_original_reset(argv[2],20000u,&r);
  if(status==GK83_SOURCE_REFUSED||status==GK83_INVALID){
   fprintf(stderr,"SOURCE REJECTED: exact private stock USA NES ROM required; no state changed\n");
   return 8;
  }
  printf("ORIGINAL_STOCK_6502_BOUNDED_RESET TRACE instructions=%u ",
   (unsigned)r.instructions);
  printf("stop=%u pc=$%04X opcode=$%02X unknown_RAM=$%04X hardware_read=$%04X ",
   (unsigned)status,(unsigned)r.stop_pc,(unsigned)r.stop_opcode,
   (unsigned)r.first_unknown_ram_read,(unsigned)r.first_hardware_read);
  printf("MMC1_serial_writes=%u commits=%u CTRL=$%02X PRG_bank=%u first_JSR=$%04X ",
   (unsigned)r.mapper_serial_writes,(unsigned)r.mapper_register_commits,
   (unsigned)r.control,(unsigned)r.prg,(unsigned)r.first_jsr);
  puts("NO PPU/RNG/INITIAL SPRING EMULATION; NO PLAYER-READY CAMPAIGN");
  return 0;
 }

 if(argc==3 && strcmp(argv[1],"--audit-stock-reset-hypotheses")==0){
  Gk83Trace exact={0};
  Gk83Stop base=gk83_trace_original_reset(argv[2],20000u,&exact);
  if(base!=GK83_UNKNOWN_RAM_GATE||exact.first_unknown_ram_read!=0x00f5u){
   fprintf(stderr,"ORIGINAL VERIFIED ROM REQUIRED WITH $00F5 UNKNOWN GATE; NO ASSUMPTIONS MADE\n");
   return 8;
  }
  unsigned gates[8]={0},at_f6=0,at_b3=0,at_bd=0,at_other=0;
  unsigned max_steps=0;Gk83Trace zero={0},two={0};
  for(unsigned guess=0;guess<=255u;guess++){
   Gk83Trace branch={0};
   Gk83Stop gate=gk84_trace_reset_single_ram_hypothesis(argv[2],0x00f5u,
     (uint8_t)guess,20000u,&branch);
   if(gate==GK83_INVALID||gate==GK83_SOURCE_REFUSED||gate>GK83_BRK_GATE){
    fprintf(stderr,"HYPOTHETICAL SOURCE TRACE REFUSED; NO GAME CHANGE\n");return 8;
   }
   gates[gate]++;
   if(branch.first_unknown_ram_read==0x00f6u)at_f6++;
   else if(branch.first_unknown_ram_read==0x00b3u)at_b3++;
   else if(branch.first_unknown_ram_read==0x00bdu)at_bd++;
   else at_other++;
   if(branch.instructions>max_steps)max_steps=branch.instructions;
   if(guess==0u)zero=branch;
   if(guess==2u)two=branch;
  }
  printf("EXACT STOCK RESET: %u CPU instructions, first UNOBSERVED RAM byte $%04X; ",
   (unsigned)exact.instructions,(unsigned)exact.first_unknown_ram_read);
  printf("256/256 COUNTERFACTUAL RAM HYPOTHESES (NOT ACTUAL NES MEMORY): ");
  printf("unknown_gate=%u hardware_gate=%u opcode_gate=%u bank_gate=%u budget_gate=%u; ",
   gates[GK83_UNKNOWN_RAM_GATE],gates[GK83_HARDWARE_GATE],gates[GK83_OPCODE_GATE],
   gates[GK83_BANK_GATE],gates[GK83_BUDGET_GATE]);
  printf("next_unknown_F6=%u next_unknown_B3=%u next_unknown_BD=%u other_gate=%u max_steps=%u; ",
   at_f6,at_b3,at_bd,at_other,max_steps);
  printf("hypothetical_F5_00=(pc:$%04X,ram:$%04X); ",
   (unsigned)zero.stop_pc,(unsigned)zero.first_unknown_ram_read);
  printf("hypothetical_F5_02=(pc:$%04X,ram:$%04X). ",
   (unsigned)two.stop_pc,(unsigned)two.first_unknown_ram_read);
  puts("NO Mesen observation, RNG, original first Spring or campaign implementation.");
  return 0;
 }

 if((argc==2||argc==3) && strcmp(argv[1],"--spring-pad-check")==0){
  gk1_app_init(&app);gk1_pad_init(&pad);
  if(!load_scenario_data())return 8;
  if(argc==3){spring_attempted=1;spring_ready=(gk74_load_file(argv[2],&spring_archive)==GK74_OK);if(!spring_ready)return 8;}
  Gk1LoadedScenario originals[2];memcpy(originals,app.worlds,sizeof originals);
  pad_test_press(GK1_PAD_A);pad_test_press(GK1_PAD_A);
  pad_test_press(GK1_PAD_A);pad_test_press(GK1_PAD_MENU);
  for(unsigned j=0;j<4;j++)pad_test_press(GK1_PAD_A);
  pad_test_press(GK1_PAD_VIEW);if(app.screen!=GK1_SCREEN_MANUAL_RULES)return 9;
  pad_test_press(GK1_PAD_A);if(app.screen!=GK1_SCREEN_SPRING_ARCHIVE)return 10;
  pad_test_press(GK1_PAD_DOWN);pad_test_press(GK1_PAD_RIGHT);
  if(app.spring_archive_cursor!=2)return 11;
  pad_test_press(GK1_PAD_B);if(app.screen!=GK1_SCREEN_MANUAL_RULES)return 12;
  if(memcmp(originals,app.worlds,sizeof originals)!=0)return 13;
  if(argc==3&&spring_archive.countries[0].gold!=439)return 14;
  puts("EXEC0074 TESTED_PORT_LINUX read-only archived Spring controller A/B/DPAD, 2 worlds unchanged PASS");return 0;
 }
 if(argc==4 && strcmp(argv[1],"--stock-spring-audit")==0){
  Gk77StockSpring original;
  Gk77Profile p=gk77_profile_parse(argv[2]);
  Gk77Result result=gk77_load(argv[3],p,&original);
  if(result!=GK77_OK){fprintf(stderr,"ORIGINAL STOCK checkpoint REFUSED code=%u (profile/bank/length/SHA)\n",(unsigned)result);return 8;}
  gk1_app_init(&app);if(!load_scenario_data())return 8;
  printf("OBSERVED_RUNTIME_STOCK %s %u Spring countries=%u bank=%s rate=%u orders=%u NOT_NATIVE_INITIALIZATION\n",
   gk77_profile_name(p),(unsigned)original.year,(unsigned)original.country_count,
   p==GK77_FRESH_MONGOL_WORKRAM?"nesWorkRam":"nesSaveRam",(unsigned)original.rate,(unsigned)original.orders);
  for(unsigned i=0;i<original.country_count;i++){
   const Gk1CountryTemplate *t=&app.worlds[original.scenario-1].countries[i];
   const Gk77Country *c=&original.countries[i];
   printf("%2u %-24.24s gold %u->%u food %u->%u troops %u->%u arms %u->%u skill %u->%u economy %u->%u fur=%u silk=%u\n",
    i+1,t->country_name,(unsigned)t->gold,(unsigned)c->gold,
    (unsigned)t->goods[10],(unsigned)c->food,(unsigned)t->troops,(unsigned)c->troops,
    (unsigned)t->arms,(unsigned)c->arms,(unsigned)t->skill,(unsigned)c->skill,
    (unsigned)t->economy,(unsigned)c->economy,(unsigned)c->goods[0],(unsigned)c->goods[1]);
  }
  if(app.worlds[0].phase!=GK1_PRE_SETUP_TEMPLATE||app.worlds[1].phase!=GK1_PRE_SETUP_TEMPLATE)return 9;
  return 0;
 }
 if(argc==4 && strcmp(argv[1],"--stock-spring-pad-check")==0){
  Gk77Profile p=gk77_profile_parse(argv[2]);
  gk1_app_init(&app);gk1_pad_init(&pad);if(!load_scenario_data())return 8;
  Gk1LoadedScenario before[2];memcpy(before,app.worlds,sizeof before);
  Gk77StockSpring sample;
  if(gk77_load(argv[3],p,&sample)!=GK77_OK)return 9;
  app.spring_stock_count=sample.country_count;
  /* Existing title -> setup -> source-locked manual -> read-only Spring. */
  pad_test_press(GK1_PAD_A);pad_test_press(GK1_PAD_A);
  pad_test_press(GK1_PAD_A);pad_test_press(GK1_PAD_MENU);
  for(unsigned j=0;j<4;j++)pad_test_press(GK1_PAD_A);
  pad_test_press(GK1_PAD_VIEW);if(app.screen!=GK1_SCREEN_MANUAL_RULES)return 10;
  pad_test_press(GK1_PAD_A);if(app.screen!=GK1_SCREEN_SPRING_ARCHIVE)return 11;
  pad_test_press(GK1_PAD_VIEW);if(!app.spring_stock_mode)return 12;
  for(unsigned i=0;i<sample.country_count-1u;i++)pad_test_press(GK1_PAD_DOWN);
  if(app.spring_archive_cursor!=sample.country_count-1u)return 13;
  pad_test_press(GK1_PAD_DOWN);if(app.spring_archive_cursor!=0)return 14;
  pad_test_press(GK1_PAD_MENU);if(app.spring_stock_mode||!app.spring_preview_mode)return 15;
  pad_test_press(GK1_PAD_VIEW);if(!app.spring_stock_mode||app.spring_archive_cursor!=0)return 16;
  pad_test_press(GK1_PAD_B);if(app.screen!=GK1_SCREEN_MANUAL_RULES)return 17;
  if(memcmp(before,app.worlds,sizeof before)!=0||app.worlds[0].phase!=GK1_PRE_SETUP_TEMPLATE||app.worlds[1].phase!=GK1_PRE_SETUP_TEMPLATE)return 18;
  puts("EXEC0077 REAL NATIVE CONTROLLER stock first-Spring proof viewer; all country indices; 2 campaign templates unchanged: PASS");return 0;
 }
 if(argc==2 && strcmp(argv[1],"--spring-source-pad-check")==0){
  gk1_app_init(&app);gk1_pad_init(&pad);
  if(!load_scenario_data())return 8;
  Gk1LoadedScenario originals[2];memcpy(originals,app.worlds,sizeof originals);
  pad_test_press(GK1_PAD_A);pad_test_press(GK1_PAD_A);
  pad_test_press(GK1_PAD_A);pad_test_press(GK1_PAD_MENU);
  for(unsigned j=0;j<4;j++)pad_test_press(GK1_PAD_A);
  pad_test_press(GK1_PAD_VIEW);if(app.screen!=GK1_SCREEN_MANUAL_RULES)return 9;
  pad_test_press(GK1_PAD_A);if(app.screen!=GK1_SCREEN_SPRING_ARCHIVE)return 10;
  pad_test_press(GK1_PAD_MENU);if(!app.spring_preview_mode)return 11;
  Gk75SpringResult probe;
  if(gk75_template_projection(&app.worlds[0].countries[0],&probe)!=GK75_OK)return 12;
  pad_test_press(GK1_PAD_DOWN);
  pad_test_press(GK1_PAD_MENU);if(app.spring_preview_mode)return 13;
  pad_test_press(GK1_PAD_B);if(app.screen!=GK1_SCREEN_MANUAL_RULES)return 14;
  if(memcmp(originals,app.worlds,sizeof originals)!=0)return 15;
  if(app.worlds[0].phase!=GK1_PRE_SETUP_TEMPLATE || app.worlds[1].phase!=GK1_PRE_SETUP_TEMPLATE)return 16;
  puts("EXEC0075 TESTED_PORT_LINUX native controller START toggles source-bounded Spring preview; both original campaigns unchanged: PASS");return 0;
 }
 if(argc==4 && strcmp(argv[1],"--spring-source-preview")==0){
  gk1_app_init(&app);if(!load_scenario_data())return 8;
  unsigned which=(unsigned)atoi(argv[2]),country=(unsigned)atoi(argv[3]);
  if(which<1||which>2||country<1||country>app.worlds[which-1].country_count)return 8;
  const Gk1CountryTemplate *t=&app.worlds[which-1].countries[country-1];
  Gk75SpringResult r;
  if(gk75_template_projection(t,&r)!=GK75_OK)return 8;
  printf("ORIGINAL_6502_SOURCE_DERIVED PRE_SETUP_ONLY WHAT_IF scenario=%u country=%u name=%s food=%u->%u gold=%u->%u troops=%u->%u tax=%u wages=%u starvation=%u\n",
      which,country,t->country_name,(unsigned)t->goods[10],(unsigned)r.after.food,
      (unsigned)t->gold,(unsigned)r.after.gold,(unsigned)t->troops,(unsigned)r.after.troops,
      (unsigned)r.gross_tax,(unsigned)r.payroll,(unsigned)r.starved);
  puts("NOT original first-playable Spring: omits setup, pre-season labor, age, sickness, growth, AI and RNG. Actual worlds untouched.");return 0;
 }
 if(argc==3 && strcmp(argv[1],"--spring-audit")==0){
  gk1_app_init(&app);if(!load_scenario_data())return 8;
  Gk74SpringArchive archive;
  Gk74Result r=gk74_load_file(argv[2],&archive);
  if(r!=GK74_OK){fprintf(stderr,"SPRING ARCHIVE REFUSED (%u); private 8192-byte exact SHA required\n",(unsigned)r);return 8;}
  for(unsigned i=0;i<GK74_COUNTRIES;i++){
   const Gk1CountryTemplate *t=&app.worlds[0].countries[i];
   const Gk74SpringCountry *n=&archive.countries[i];
   printf("%2u %-24.24s PRE[%u,%u,%u,%u,%u,%u] ARCH[%u,%u,%u,%u,%u,%u]\n",i+1,t->country_name,
    (unsigned)t->gold,(unsigned)t->goods[10],(unsigned)t->troops,(unsigned)t->arms,(unsigned)t->skill,(unsigned)t->economy,
    (unsigned)n->gold,(unsigned)n->food,(unsigned)n->troops,(unsigned)n->arms,(unsigned)n->skill,(unsigned)n->economy);
  }
  puts("ARCHIVED_SOURCE_CAPTURE_ROM_UNVERIFIED: NOT THE INITIAL SPRING TRANSFORM");return 0;
 }
 if(argc==2&&strcmp(argv[1],"--version")==0){
  puts("Genghis Khan I native research prototype " GK1_PORT_VERSION " (EXEC0084 explicit unknown-RAM reset fork, inherited limited stock checkpoint; not full game; setup unfinished)");return 0;
 }
 /* Preserve EXEC0056's tested synthetic-pad correction/export contract. */
 if(argc==3&&strcmp(argv[1],"--bonus-correction-export")==0){
  gk1_app_init(&app);gk1_pad_init(&pad);if(!load_scenario_data())return 8;
  pad_test_press(GK1_PAD_A);
  pad_test_press(GK1_PAD_DOWN);pad_test_press(GK1_PAD_A);
  pad_test_press(GK1_PAD_A);pad_test_press(GK1_PAD_MENU);
  pad_test_press(GK1_PAD_A);pad_test_press(GK1_PAD_A);pad_test_press(GK1_PAD_A);
  pad_test_press(GK1_PAD_DOWN);pad_test_press(GK1_PAD_A);
  pad_test_press(GK1_PAD_MENU);
  if(!app.manual_bonus_active[1])return 13;
  pad_test_press(GK1_PAD_B);pad_test_press(GK1_PAD_A);
  pad_test_press(GK1_PAD_X);pad_test_press(GK1_PAD_Y);pad_test_press(GK1_PAD_X);
  if(gk55_bonus_spent(&app,1)!=0||app.manual_bonus[1][0][0]!=0)return 14;
  pad_test_press(GK1_PAD_RIGHT);pad_test_press(GK1_PAD_DOWN);
  pad_test_press(GK1_PAD_B);pad_test_press(GK1_PAD_X);
  if(app.manual_bonus[1][1][1]!=9||gk55_bonus_spent(&app,1)!=9)return 15;
  pad_test_press(GK1_PAD_MENU);
  pre_setup_path_override=argv[2];pad_test_press(GK1_PAD_X);
  if(strcmp(app.status,"PRE_SETUP SETUP-DRAFT SAVED - NOT PLAYABLE SPRING"))return 6;
  printf("EXEC0056 PRESERVED TESTED_PORT_LINUX synthetic-pad correction + cold-save points=%u bytes=%u\n",
   (unsigned)app.manual_bonus[1][1][1],(unsigned)GK55_PRE_SETUP_BYTES);
  return 0;
 }
 if(argc==2&&strcmp(argv[1],"--pass-pad-check")==0){
  gk1_app_init(&app);gk1_pad_init(&pad);if(!load_scenario_data())return 8;
  Gk1LoadedScenario before[2]={app.worlds[0],app.worlds[1]};
  pad_test_press(GK1_PAD_A); /* title -> campaigns */
  pad_test_press(GK1_PAD_A); /* Mongol country list */
  pad_test_press(GK1_PAD_A); /* candidate select */
  pad_test_press(GK1_PAD_MENU); /* source manual setup wizard */
  for(unsigned i=0;i<4;i++)pad_test_press(GK1_PAD_A);
  if(app.screen!=GK1_SCREEN_SETUP_WIZARD)return 9;
  pad_test_press(GK1_PAD_VIEW);pad_test_press(GK1_PAD_MENU);
  if(app.screen!=GK1_SCREEN_BATTLE_TIMELINE)return 10;
  pad_test_press(GK1_PAD_MENU);
  if(app.screen!=GK1_SCREEN_BATTLE_PASS_DRILL)return 11;
  pad_test_press(GK1_PAD_A);pad_test_press(GK1_PAD_A);
  if(app.battle_pass.mobility[0]!=13||app.battle_pass.used[0]!=1)return 12;
  pad_test_press(GK1_PAD_RIGHT);pad_test_press(GK1_PAD_A);
  if(app.battle_pass.mobility[1]!=11||app.battle_pass.used[1]!=1)return 13;
  pad_test_press(GK1_PAD_B);
  if(app.screen!=GK1_SCREEN_BATTLE_TIMELINE||
    memcmp(before,&app.worlds,sizeof before)||app.battle_timeline.days_complete)return 14;
  puts("EXEC0072 TESTED_PORT_LINUX synthetic gamepad Pass mobility13/11, no world change PASS");
  return 0;
 }
 if(argc==3&&strcmp(argv[1],"--bonus-correction-import")==0){
  gk1_app_init(&app);if(!load_scenario_data())return 8;
  if(load_pre_setup(argv[2])!=GK1_STORE_OK)return 7;
  if(app.manual_bonus[1][1][1]!=9||gk55_bonus_spent(&app,1)!=9||
     app.worlds[1].phase!=GK1_PRE_SETUP_TEMPLATE)return 9;
  puts("EXEC0056 PRESERVED TESTED_PORT_LINUX 9-point correction cold-load PASS");return 0;
 }
 if(argc==6&&strcmp(argv[1],"--demob-preview-export")==0){
  unsigned sc=(unsigned)atoi(argv[3]),ci=(unsigned)atoi(argv[4]),q=(unsigned)atoi(argv[5]);
  if(sc<1||sc>2||ci<1||(sc==1?ci>14:ci>27)||q==0||q>999)return 4;
  gk1_app_init(&app);gk1_pad_init(&pad);if(!load_scenario_data())return 8;
  pad_test_press(GK1_PAD_A);if(sc==2)pad_test_press(GK1_PAD_DOWN);
  pad_test_press(GK1_PAD_A);
  for(unsigned i=1;i<ci;i++)pad_test_press(GK1_PAD_DOWN);
  pad_test_press(GK1_PAD_VIEW);pad_test_press(GK1_PAD_MENU);
  if(app.screen!=GK1_SCREEN_DEMOBILIZE)return 9;
  Gk1CountryTemplate *c=&app.worlds[sc-1].countries[ci-1];
  unsigned before=c->troops;
  if(gk60_demob_apply(c,(uint16_t)q)!=GK60_DEMOB_OK)return 5;
  if(before-c->troops!=q)return 9;
  Gk1StoreResult r=save_pre_setup(argv[2]);
  if(r!=GK1_STORE_OK)return 6;
  printf("EXEC0060 TESTED_PORT_LINUX Enhanced Demobilize PRE_SETUP source=%u country=%u amount=%u troops=%u reserve=%u eligible=%u combined=%u bytes=%u (NOT SPRING)\n",
   sc,ci,q,c->troops,c->undrafted_reserve,c->eligible_draft_count,c->combined_population,(unsigned)GK55_PRE_SETUP_BYTES);
  return 0;
 }
 if(argc==3&&strcmp(argv[1],"--demob-preview-import")==0){
  gk1_app_init(&app);if(!load_scenario_data())return 8;
  if(load_pre_setup(argv[2])!=GK1_STORE_OK)return 7;
  unsigned w=app.active_world,ix=app.country_cursor[w];
  const Gk1CountryTemplate *c=&app.worlds[w].countries[ix];
  printf("EXEC0060 TESTED_PORT_LINUX PRE_SETUP cold-load source=%u country=%u troops=%u reserve=%u eligible=%u combined=%u bytes=%u (NOT SPRING)\n",
   w+1,ix+1,c->troops,c->undrafted_reserve,c->eligible_draft_count,c->combined_population,(unsigned)GK55_PRE_SETUP_BYTES);
  return 0;
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

 /* Actual multi-process controller-operated three-slot persistence.
  * Slot 1 maps to the inherited filename unmodified. All snapshots are
  * pre-setup and require 41-row original CSV source validation.
  * These options are Linux native QA harnesses, NOT keyboard gameplay. */
 if(argc==7 && strcmp(argv[1],"--slot-export")==0){
  unsigned sl,sc,ci,quantity;char trailing;
  if(sscanf(argv[3],"%u%c",&sl,&trailing)!=1 ||
     sscanf(argv[4],"%u%c",&sc,&trailing)!=1 ||
     sscanf(argv[5],"%u%c",&ci,&trailing)!=1 ||
     sscanf(argv[6],"%u%c",&quantity,&trailing)!=1 ||
     sl<1||sl>GK58_SLOT_MAX||sc<1||sc>2||ci<1||ci>(sc==1?14u:27u)||quantity>1000u)return 4;
  gk1_app_init(&app);gk1_pad_init(&pad);if(!load_scenario_data())return 8;
  input(GK1_INPUT_CONFIRM);if(sc==2)input(GK1_INPUT_DOWN);
  input(GK1_INPUT_CONFIRM);
  for(unsigned i=1;i<ci;i++)input(GK1_INPUT_DOWN);
  input(GK1_INPUT_LABOR_PLAN);
  if(app.screen!=GK1_SCREEN_LABOR_PLAN)return 9;
  Gk1CountryTemplate *c=&app.worlds[sc-1].countries[ci-1];
  unsigned before=c->troops;
  for(unsigned i=0;i<quantity;i++)pad_test_press(GK1_PAD_A);
  if(c->troops != before-(quantity>before?before:quantity))return 9;
  pre_setup_path_override=argv[2];
  for(unsigned k=1;k<sl;k++)pad_test_press(GK1_PAD_RB);
  if(campaign_slot!=sl)return 9;
  pad_test_press(GK1_PAD_X);
  if(strcmp(app.status,"PRE_SETUP SETUP-DRAFT SAVED - NOT PLAYABLE SPRING"))return 6;
  printf("EXEC0058 TESTED_PORT_LINUX saved slot=%u scenario=%u country=%u troops=%u town=%u bytes=%u\n",
    sl,sc,ci,(unsigned)c->troops,(unsigned)c->town,(unsigned)GK55_PRE_SETUP_BYTES);
  return 0;
 }
 if(argc==6 && strcmp(argv[1],"--slot-import")==0){
  unsigned sl,sc,ci;char trailing;
  if(sscanf(argv[3],"%u%c",&sl,&trailing)!=1 ||
     sscanf(argv[4],"%u%c",&sc,&trailing)!=1 ||
     sscanf(argv[5],"%u%c",&ci,&trailing)!=1 ||
     sl<1||sl>GK58_SLOT_MAX||sc<1||sc>2||ci<1||ci>(sc==1?14u:27u))return 4;
  gk1_app_init(&app);gk1_pad_init(&pad);if(!load_scenario_data())return 8;
  input(GK1_INPUT_CONFIRM);if(sc==2)input(GK1_INPUT_DOWN);
  input(GK1_INPUT_CONFIRM);
  Gk1App original=app;
  pre_setup_path_override=argv[2];
  for(unsigned k=1;k<sl;k++)pad_test_press(GK1_PAD_RB);
  if(campaign_slot!=sl)return 9;
  pad_test_press(GK1_PAD_Y);
  if(strcmp(app.status,"PRE_SETUP LOAD REFUSED; ORIGINAL STATE UNCHANGED")==0){
    if(memcmp(&original.worlds,&app.worlds,sizeof original.worlds)!=0)return 9;
    return 7;
  }
  const Gk1CountryTemplate *c=&app.worlds[sc-1].countries[ci-1];
  printf("EXEC0058 TESTED_PORT_LINUX loaded slot=%u scenario=%u country=%u troops=%u town=%u phase=%u bytes=%u\n",
    sl,sc,ci,(unsigned)c->troops,(unsigned)c->town,
    (unsigned)app.worlds[sc-1].phase,(unsigned)GK55_PRE_SETUP_BYTES);
  return 0;
 }
 if(argc==6 && strcmp(argv[1],"--labor-plan-export")==0){
  unsigned sc,ci,quantity;char trailing;
  if(sscanf(argv[3],"%u%c",&sc,&trailing)!=1||
     sscanf(argv[4],"%u%c",&ci,&trailing)!=1||
     sscanf(argv[5],"%u%c",&quantity,&trailing)!=1||
     sc<1||sc>2||ci<1||ci>(sc==1?14u:27u)||quantity>1000u)return 4;
  gk1_app_init(&app);gk1_pad_init(&pad);if(!load_scenario_data())return 8;
  input(GK1_INPUT_CONFIRM);if(sc==2)input(GK1_INPUT_DOWN);input(GK1_INPUT_CONFIRM);
  for(unsigned i=1;i<ci;i++)input(GK1_INPUT_DOWN);
  Gk1CountryTemplate *c=&app.worlds[sc-1].countries[ci-1];
  unsigned from=c->troops,to=c->town,total=c->drafted_population;
  input(GK1_INPUT_LABOR_PLAN);
  if(app.screen!=GK1_SCREEN_LABOR_PLAN)return 9;
  for(unsigned i=0;i<quantity;i++)input(GK1_INPUT_CONFIRM);
  if(c->troops!=from-(quantity>from?from:quantity)||
     c->town!=to+(quantity>from?from:quantity)||c->drafted_population!=total)return 9;
  Gk1StoreResult r=save_pre_setup(argv[2]);if(r!=GK1_STORE_OK)return 6;
  printf("EXEC0057 PORT_LINUX labor-plan export scenario=%u country=%u troops=%u town=%u drafted=%u transferred=%u PRE_SETUP_ONLY bytes=%u\n",
   sc,ci,c->troops,c->town,c->drafted_population,from-c->troops,(unsigned)GK55_PRE_SETUP_BYTES);
  return 0;
 }
 if(argc==5 && strcmp(argv[1],"--labor-plan-import")==0){
  unsigned sc,ci;char trailing;
  if(sscanf(argv[3],"%u%c",&sc,&trailing)!=1||
     sscanf(argv[4],"%u%c",&ci,&trailing)!=1||sc<1||sc>2||
     ci<1||ci>(sc==1?14u:27u))return 4;
  gk1_app_init(&app);if(!load_scenario_data())return 8;
  Gk1StoreResult r=load_pre_setup(argv[2]);if(r!=GK1_STORE_OK)return 7;
  const Gk1CountryTemplate *c=&app.worlds[sc-1].countries[ci-1];
  printf("EXEC0057 PORT_LINUX labor-plan cold-import scenario=%u country=%u troops=%u town=%u drafted=%u phase=%u bytes=%u\n",
   sc,ci,c->troops,c->town,c->drafted_population,(unsigned)app.worlds[sc-1].phase,(unsigned)GK55_PRE_SETUP_BYTES);
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
 if(argc==3 && strcmp(argv[1],"--mesen-map-pad-check")==0){
  Gk69MesenMap temporary;
  if(gk69_map_from_file(argv[2],&temporary)!=GK69_MAP_OK){
   fprintf(stderr,"GK69 exact approved-modified WorkRAM rejected; no UI/game mutation\n");return 8;
  }
  gk1_app_init(&app);gk1_pad_init(&pad);
  if(!load_scenario_data())return 9;
  Gk1LoadedScenario original_worlds[2];memcpy(original_worlds,app.worlds,sizeof original_worlds);
  mesen_map=temporary;mesen_ready=1;
  pad_test_press(GK1_PAD_DOWN);pad_test_press(GK1_PAD_A);
  if(app.screen!=GK1_SCREEN_RESEARCH_BATTLE)return 10;
  pad_test_press(GK1_PAD_RIGHT);pad_test_press(GK1_PAD_DOWN);
  if(mesen_map.x!=1||mesen_map.y!=1)return 11;
  pad_test_press(GK1_PAD_A);if(!mesen_show_names)return 12;
  pad_test_press(GK1_PAD_B);
  if(app.screen!=GK1_SCREEN_TITLE)return 13;
  if(memcmp(original_worlds,app.worlds,sizeof original_worlds)!=0)return 14;
  puts("GK69 native controller A/B/DPAD + original scenario no-mutation PASS (MOD NES CAPTURE)");return 0;
 }
 if(argc==2 && strcmp(argv[1],"--merged-pad-check")==0){
  /* Same native game, one real input route: strategy View -> locked
     budget -> battle what-if -> Pass drill. Original world data immutable. */
  gk1_app_init(&app);gk1_pad_init(&pad);
  if(!load_scenario_data())return 8;
  Gk1LoadedScenario original_worlds[2];
  memcpy(original_worlds,app.worlds,sizeof original_worlds);
  pad_test_press(GK1_PAD_A);pad_test_press(GK1_PAD_A); /* title, Mongol */
  pad_test_press(GK1_PAD_A);pad_test_press(GK1_PAD_MENU);
  for(unsigned j=0;j<4;j++)pad_test_press(GK1_PAD_A);
  pad_test_press(GK1_PAD_VIEW); /* source manual */
  if(app.screen!=GK1_SCREEN_MANUAL_RULES)return 9;
  pad_test_press(GK1_PAD_VIEW); /* manual order budget */
  if(app.screen!=GK1_SCREEN_ORDER_BUDGET_RESEARCH || app.order_budget_research.playable ||
     app.order_budget_research.allowance!=3)return 10;
  pad_test_press(GK1_PAD_A); /* blocked strategic Pass PRE_SETUP */
  if(app.order_budget_research.remaining!=3)return 11;
  pad_test_press(GK1_PAD_RIGHT); /* direct governed: one */
  if(app.order_budget_research.allowance!=1)return 12;
  pad_test_press(GK1_PAD_RIGHT); /* prince AI: zero */
  if(app.order_budget_research.allowance!=0)return 13;
  pad_test_press(GK1_PAD_B); /* manual rules */
  if(app.screen!=GK1_SCREEN_MANUAL_RULES)return 14;
  pad_test_press(GK1_PAD_MENU); /* battle what-if */
  if(app.screen!=GK1_SCREEN_BATTLE_TIMELINE)return 15;
  pad_test_press(GK1_PAD_MENU); /* manual unit Pass drill */
  if(app.screen!=GK1_SCREEN_BATTLE_PASS_DRILL)return 16;
  pad_test_press(GK1_PAD_A);pad_test_press(GK1_PAD_A); /* second rejected */
  if(app.battle_pass.mobility[0]!=13 || app.battle_pass.used[0]!=1)return 17;
  pad_test_press(GK1_PAD_RIGHT);pad_test_press(GK1_PAD_A);
  if(app.battle_pass.mobility[1]!=11 || app.battle_pass.used[1]!=1)return 18;
  pad_test_press(GK1_PAD_B); /* back to battle timeline */
  if(app.screen!=GK1_SCREEN_BATTLE_TIMELINE || app.battle_timeline.days_complete)return 19;
  if(memcmp(original_worlds,app.worlds,sizeof original_worlds)!=0 ||
     app.worlds[0].phase!=GK1_PRE_SETUP_TEMPLATE ||
     app.worlds[1].phase!=GK1_PRE_SETUP_TEMPLATE)return 20;
  puts("EXEC0073 native merged controller BOTH paths PASS; world immutable PRE_SETUP");
  return 0;
 }
 if(argc==2 && strcmp(argv[1],"--order-budget-pad-check")==0){
  gk1_app_init(&app);gk1_pad_init(&pad);
  if(!load_scenario_data())return 12;
  Gk1LoadedScenario originals[2];memcpy(originals,app.worlds,sizeof originals);
  /* Controller sample route is already established via pad_test_press. */
  pad_test_press(GK1_PAD_A);pad_test_press(GK1_PAD_A); /* title, Mongol */
  pad_test_press(GK1_PAD_A);pad_test_press(GK1_PAD_MENU);
  for(int j=0;j<4;j++)pad_test_press(GK1_PAD_A);
  pad_test_press(GK1_PAD_VIEW); /* manual source screen */
  if(app.screen!=GK1_SCREEN_MANUAL_RULES)return 13;
  pad_test_press(GK1_PAD_VIEW); /* new MANUAL_DERIVED order budget screen */
  if(app.screen!=GK1_SCREEN_ORDER_BUDGET_RESEARCH)return 14;
  if(app.order_budget_research.allowance!=3||app.order_budget_research.playable)return 15;
  pad_test_press(GK1_PAD_A); /* try Pass => blocked before original Spring */
  if(app.order_budget_research.remaining!=3)return 16;
  pad_test_press(GK1_PAD_RIGHT); /* direct 1 */
  if(app.order_budget_research.allowance!=1)return 17;
  pad_test_press(GK1_PAD_RIGHT); /* prince AI 0 */
  if(app.order_budget_research.allowance!=0)return 18;
  pad_test_press(GK1_PAD_B);
  if(app.screen!=GK1_SCREEN_MANUAL_RULES)return 19;
  if(memcmp(originals,app.worlds,sizeof originals)!=0)return 20;
  puts("EXEC0071 native software controller View/A/DPAD/B PASS; original two worlds unchanged PRE_SETUP");return 0;
 }
 int smoke=0,show_snapshot=0,show_path=0,show_divide=0,show_scenario=0,show_setup=0,show_labor=0,show_demob=0,show_manual=0,show_market=0,show_clock=0,show_whatif=0,show_rest=0,show_mesen=0,show_order_budget=0,show_pass=0,show_spring=0,show_stock_spring=0,show_stock_checkpoint=0;
 if(argc==3 && (strcmp(argv[1],"--smoke-frames")==0 ||
                 strcmp(argv[1],"--smoke-snapshot")==0 ||
                 strcmp(argv[1],"--smoke-path")==0 ||
                 strcmp(argv[1],"--smoke-divide")==0 ||
                 strcmp(argv[1],"--smoke-scenario")==0 ||
                 strcmp(argv[1],"--smoke-setup")==0 ||
                 strcmp(argv[1],"--smoke-labor")==0 || strcmp(argv[1],"--smoke-demob")==0 ||
                 strcmp(argv[1],"--smoke-manual-rules")==0 || strcmp(argv[1],"--smoke-order-budget")==0 || strcmp(argv[1],"--smoke-market")==0 ||
                 strcmp(argv[1],"--smoke-battle-clock")==0 ||
                 strcmp(argv[1],"--smoke-battle-whatif")==0 ||
                 strcmp(argv[1],"--smoke-battle-pass")==0 ||
                 strcmp(argv[1],"--smoke-rest")==0 ||
                 strcmp(argv[1],"--smoke-mesen-grid")==0 ||
                 strcmp(argv[1],"--smoke-spring-archive")==0 ||
                 strcmp(argv[1],"--smoke-stock-spring")==0 ||
                 strcmp(argv[1],"--smoke-stock-checkpoint")==0)){
  smoke=atoi(argv[2]);if(smoke<1||smoke>120)return 4;
  show_snapshot=(strcmp(argv[1],"--smoke-snapshot")==0);
  show_path=(strcmp(argv[1],"--smoke-path")==0);
  show_divide=(strcmp(argv[1],"--smoke-divide")==0);
  show_scenario=(strcmp(argv[1],"--smoke-scenario")==0);
  show_setup=(strcmp(argv[1],"--smoke-setup")==0);
  show_labor=(strcmp(argv[1],"--smoke-labor")==0);
  show_demob=(strcmp(argv[1],"--smoke-demob")==0);
  show_manual=(strcmp(argv[1],"--smoke-manual-rules")==0);
  show_order_budget=(strcmp(argv[1],"--smoke-order-budget")==0);
  show_market=(strcmp(argv[1],"--smoke-market")==0);
  show_clock=(strcmp(argv[1],"--smoke-battle-clock")==0);
  show_whatif=(strcmp(argv[1],"--smoke-battle-whatif")==0);
  show_pass=(strcmp(argv[1],"--smoke-battle-pass")==0);
  show_rest=(strcmp(argv[1],"--smoke-rest")==0);
  show_mesen=(strcmp(argv[1],"--smoke-mesen-grid")==0);
  show_spring=(strcmp(argv[1],"--smoke-spring-archive")==0);
  show_stock_checkpoint=(strcmp(argv[1],"--smoke-stock-checkpoint")==0);
  show_stock_spring=(strcmp(argv[1],"--smoke-stock-spring")==0)||show_stock_checkpoint;
 }
 else if(argc!=1){fprintf(stderr,"Usage: gk1_native_shell [--smoke-frames 1..120 | --smoke-snapshot 1..120 | --smoke-path 1..120 | --path-export path scenario 1/2/b-script | --path-import path | --cold-export path case events | --cold-import path]\n");return 4;}
 gk1_app_init(&app);gk1_pad_init(&pad);load_optional_battle_view();load_optional_mesen_map();load_optional_spring_archive();load_optional_stock_spring();(void)load_scenario_data();
 if(show_mesen){
  if(!mesen_ready){fprintf(stderr,"GK69 smoke: exact approved-modified RAM not supplied\n");return 8;}
  input(GK1_INPUT_DOWN);input(GK1_INPUT_CONFIRM);
  if(app.screen!=GK1_SCREEN_RESEARCH_BATTLE)return 9;
  input(GK1_INPUT_RIGHT);input(GK1_INPUT_DOWN);
  if(mesen_map.x!=1||mesen_map.y!=1)return 10;
 }
 if(show_rest){
  for(int j=0;j<4;j++)input(GK1_INPUT_DOWN);
  input(GK1_INPUT_CONFIRM);
  if(app.screen!=GK1_SCREEN_REST_CASE)return 13;
  input(GK1_INPUT_CONFIRM);if(app.rest_case.stage!=GK68_REST_CONFIRM)return 14;
  input(GK1_INPUT_CANCEL);if(app.rest_case.stage!=GK68_REST_SPRING)return 15;
  input(GK1_INPUT_CONFIRM);input(GK1_INPUT_CONFIRM);
  if(app.rest_case.stage!=GK68_REST_SUMMER||app.rest_case.bp!=829)return 16;
  if(app.worlds[0].phase!=GK1_PRE_SETUP_TEMPLATE||app.worlds[1].phase!=GK1_PRE_SETUP_TEMPLATE)return 17;
 }
 if(show_manual||show_clock||show_whatif||show_order_budget||show_pass||show_spring||show_stock_spring){
  if(!scenario_loaded)return 12;
  input(GK1_INPUT_CONFIRM);input(GK1_INPUT_CONFIRM); /* Mongol country */
  input(GK1_INPUT_CONFIRM);input(GK1_INPUT_SETUP); /* candidacy and wizard */
  input(GK1_INPUT_CONFIRM);input(GK1_INPUT_CONFIRM);input(GK1_INPUT_CONFIRM);
  input(GK1_INPUT_CONFIRM); /* automatic -> awaiting source */
  if(app.setup_stage[0]!=GK53_SETUP_AWAIT_SOURCE)return 13;
  input(GK1_INPUT_LABOR_PLAN);
  if(app.screen!=GK1_SCREEN_MANUAL_RULES)return 14;
  if(show_order_budget){input(GK1_INPUT_LABOR_PLAN);if(app.screen!=GK1_SCREEN_ORDER_BUDGET_RESEARCH)return 18;}
  if(show_spring||show_stock_spring){input(GK1_INPUT_CONFIRM);if(app.screen!=GK1_SCREEN_SPRING_ARCHIVE)return 18;
    if(show_stock_spring){if(!stock_ready)return 20;input(GK1_INPUT_LABOR_PLAN);
      if(!app.spring_stock_mode||app.spring_stock_count!=stock_spring.country_count)return 21;}
    input(GK1_INPUT_DOWN);if(app.spring_archive_cursor!=1)return 19;
    if(show_stock_checkpoint){
      /* Native controller-only private profile switching: a missing second
       * profile must never reclassify the authenticated first profile. */
      if(!getenv("GK1_STOCK_SPRING_RECORDS")){
       pad_test_press(GK1_PAD_LB);
       if(stock_ready||app.spring_stock_count!=0)return 28;
       pad_test_press(GK1_PAD_RB);
       if(!stock_ready||stock_spring.profile!=GK77_FRESH_MONGOL_WORKRAM)return 29;
      }
      input(GK1_INPUT_CONFIRM);
      if(app.screen!=GK1_SCREEN_STOCK_CONTINUE||!app.stock_checkpoint.valid)return 22;
      if(app.stock_checkpoint.orders.remaining!=3||app.stock_checkpoint.world.country_count!=stock_spring.country_count)return 23;
      if(getenv("GK1_ORIGINAL_STOCK_ROM")){
        /* Full native app/controller-source probe, not just standalone C test:
         * original user-owned ROM verified, exact stock world stays unchanged. */
        Gk79Checkpoint before=app.stock_checkpoint;
        input(GK1_INPUT_SETUP);
        if((strstr(app.status,"STOCK RESET $C000:")==NULL && strstr(app.status,"STOCK ROM VERIFIED: 6502")==NULL) ||
           memcmp(&before,&app.stock_checkpoint,sizeof before)!=0 ||
           app.worlds[0].phase!=GK1_PRE_SETUP_TEMPLATE)return 31;
      }
      input(GK1_INPUT_CONFIRM); /* arm PASS */
      if(!app.stock_checkpoint.pending_pass||app.stock_checkpoint.orders.remaining!=3)return 24;
      input(GK1_INPUT_CANCEL);  /* B cancel preserves 3 orders */
      if(app.stock_checkpoint.pending_pass||app.stock_checkpoint.orders.remaining!=3)return 25;
      input(GK1_INPUT_CONFIRM);input(GK1_INPUT_CONFIRM); /* committed PASS */
      if(!app.stock_checkpoint.orders.ended||app.stock_checkpoint.orders.remaining!=0)return 26;
      if(app.worlds[0].phase!=GK1_PRE_SETUP_TEMPLATE||app.worlds[1].phase!=GK1_PRE_SETUP_TEMPLATE)return 27;
    }
    }
  if(show_clock||show_whatif||show_pass){input(GK1_INPUT_SETUP);if(app.screen!=GK1_SCREEN_BATTLE_TIMELINE)return 15;
    if(show_pass){input(GK1_INPUT_SETUP);if(app.screen!=GK1_SCREEN_BATTLE_PASS_DRILL)return 16;
      input(GK1_INPUT_CONFIRM);input(GK1_INPUT_RIGHT);input(GK1_INPUT_CONFIRM);
      if(app.battle_pass.mobility[0]!=13||app.battle_pass.mobility[1]!=11)return 17;
    }else if(show_whatif){input(GK1_INPUT_DOWN);input(GK1_INPUT_LEFT);input(GK1_INPUT_CONFIRM);
      if(app.battle_timeline.outcome!=GK65_DEFENDER_WON)return 16;
    }else {input(GK1_INPUT_CONFIRM);input(GK1_INPUT_CONFIRM);}}
 }
 if(show_market){
  if(!scenario_loaded)return 12;
  input(GK1_INPUT_CONFIRM);input(GK1_INPUT_CONFIRM); /* Mongol country */
  input(GK1_INPUT_LABOR_PLAN);input(GK1_INPUT_LABOR_PLAN);
  if(app.screen!=GK1_SCREEN_MARKET_EXPLORER)return 13;
 }
 if(show_demob){
  if(!scenario_loaded)return 12;
  input(GK1_INPUT_CONFIRM);input(GK1_INPUT_CONFIRM);input(GK1_INPUT_LABOR_PLAN);
  input(GK1_INPUT_SETUP);
  if(app.screen!=GK1_SCREEN_DEMOBILIZE)return 13;
 }
 if(show_labor){
  input(GK1_INPUT_CONFIRM);input(GK1_INPUT_CONFIRM);
  input(GK1_INPUT_LABOR_PLAN);
  if(app.screen!=GK1_SCREEN_LABOR_PLAN)return 13;
  input(GK1_INPUT_CONFIRM);
 }
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
  if(smoke && frames>=smoke){printf("EXEC0085 real X11 native smoke: %d frames, %dx%d requested, PASS; stock_spring=%s stock_checkpoint_pass=%s order_budget=%s battle_pass=%s\n",frames,WIDTH,HEIGHT,show_stock_spring?"yes":"no",show_stock_checkpoint?"yes":"no",show_order_budget?"yes":"no",show_pass?"yes":"no");break;}
  struct timespec t={0,16000000};nanosleep(&t,NULL);
 }
 if(joystick>=0)close(joystick);
 if(font)XFreeFont(display,font);
 XFreeGC(display,graphics);XDestroyWindow(display,window);XCloseDisplay(display);
 return 0;
}
#endif
