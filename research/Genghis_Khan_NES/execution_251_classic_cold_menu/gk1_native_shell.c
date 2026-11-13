/* EXEC0044: UPDATED standalone NATIVE 1280x720 cross-platform Genghis Khan shell.
 * Copyright-clean PLACEHOLDER graphics and no sound assets. NOT a complete game.
 * Linux/X11 native compilation executed; Win32/XInput code provided but NOT
 * Windows-compiled or physically controller-tested in this run.
 */
#define _POSIX_C_SOURCE 200809L
#include "gk218_stock_write_trace.h"
#include "gk174_setup_boundary.h"
#include "gk91_reset_zero.h"
#include "gk206_rng_source_parity.h"
#include "gk241_archived_import_capability.h"
#include "gk208_archived_rng.h"
#include "gk209_archived_rate_ambiguity.h"
#include "gk185_stock_hotkey.h"
#include "gk80_full_checkpoint.h"
#include "gk171_enhanced_title_resume.h"
#include "gk177_enhanced_receipt.h"
#include "gk1_app_state.h"
#include "gk224_practice_save.h"
#include "gk225_practice_resume.h"
#include "gk226_practice_checkpoint.h"
#include "gk227_practice_title_preview.h"
#include "gk215_original_spring_gap.h"
#include "gk195_classic_view.h"
#include "gk196_classic_trade_status.h"
#include "gk198_tax_forecast.h"
#include "gk199_classic_tax_undo.h"
#include "gk95_stock_trade_reference.h"
#include "gk96_ppu_warmup.h"
#include "gk97_nmi_trace.h"
#include "gk98_controller_trace.h"
#include "gk99_xinput_bridge.h"
#include "gk230_win32_smoke_args.h"
#include "gk89_spring_reconcile.h"
#include "gk90_verified_finance.h"
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
#include "gk117_stock_pristine_gate.h"
#include "gk162_original_entry.h"
#include "gk163_mesen_import.h"
#include "gk164_mss_entry.h"
#include "gk165_fresh_json.h"
#include "gk193_auto_source_entry.h"
#include "gk166_fresh_entry.h"
#include "gk237_owner_fresh_win.h"
#include "gk238_owner_startup.h"
#include "gk239_owner_world_mss.h"
#include "gk245_owner_source.h"
#include "gk167_private_cold_save.h"
#include "gk188_classic_receipt.h"
#include "gk189_classic_cold_save.h"
#include "gk170_slot_preview.h"
#include "gk82_stock_cartridge.h"
#include "gk83_reset_trace.h"
#include "gk103_ntsc_ppu.h"
#include "gk84_reset_hypothesis.h"
#include "gk58_slots.h"
#include "gk59_storage.h"
#include "gk120_save_home.h"
#include "gk60_demobilize.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifdef _WIN32
# include <windows.h>
# include <xinput.h>
# include <sys/stat.h>
#else
# include <X11/Xlib.h>
# include <time.h>
# include <errno.h>
# include <fcntl.h>
# include <unistd.h>
# include <linux/joystick.h>
# include <sys/stat.h>
#endif
#define GK1_PORT_VERSION "1.99.0-dev1"
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
/* Controller-only TITLE inventory: metadata from the SAME SHA-gated private
 * cold loader, cached per selected slot; no file reads on game render frames. */
static Gk170SlotPreview private_title_preview;
static int private_title_preview_valid=0;
static uint8_t private_preview_profile=255u;
static Gk227Preview practice_title_preview;
static Gk227PreviewStatus practice_title_status=GK227_PREVIEW_MISSING;
static int practice_slot_path(char *out,size_t cap);
static void refresh_private_title_preview(void);
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
/* EXEC0120: an installed Linux executable must not depend on the launcher
 * having changed CWD to the save folder. Default saves, including source-
 * attested Enhanced labor, use XDG_DATA_HOME or HOME/.local/share. Explicit
 * user-supplied paths are unchanged; failure never falls back to CWD. */
#ifndef _WIN32
static char gk120_linux_paths[6][1024];
static const char *gk120_linux_default(unsigned which,const char *name){
 if(which>=6u)return NULL;
 char *buf=gk120_linux_paths[which];
 if(!gk120_linux_path(getenv("XDG_DATA_HOME"),getenv("HOME"),name,
                       buf,sizeof gk120_linux_paths[which]))return NULL;
 if(!gk120_prepare_linux_dir(buf))return NULL;
 return buf;
}
#endif
static const char *divide_replay_path(void){
 const char *s=getenv("GK1_DIVIDE_REPLAY_FILE");
 if(s&&*s)return s;
#ifdef _WIN32
 return gk59_profile_default(0,"gk1_mod_divide_replay_slot1.gkd");
#else
 return gk120_linux_default(0,"gk1_mod_divide_replay_slot1.gkd");
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
 return gk120_linux_default(1,"gk1_observed_path_slot1.gkp");
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
 return gk120_linux_default(2,"gk1_research_slot1.gkr");
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
  "EXEC0089 ORIGINAL SPRING SOURCE RECONCILIATION - RESEARCH ONLY":
  "RESEARCH BUILD / ORIGINAL SPRING INITIALIZATION OPEN",EDGE);
 if(app.screen==GK1_SCREEN_TITLE){
  frame(120,138,1038,466);
  menu_line(223,app.menu_index==0,"TWO ORIGINAL CAMPAIGNS - PRE-SETUP DATA");
  menu_line(289,app.menu_index==1,"PREVIOUS BATTLE RESEARCH / SNAPSHOT VIEWER");
  menu_line(355,app.menu_index==2,"CONTROLLED TWO-STEP TACTICAL PATH (MOD ROM)");
  menu_line(421,app.menu_index==3,"CONTROLLER-DRIVEN DIVIDE / ZERO-SOLDIER OUTCOME (MOD)");
  menu_line(487,app.menu_index==4,"MOD ROM SPRING REST A/B SOURCE CASE (1175)");
  label(150,536,"A SELECT / X ENHANCED / Y PRACTICE OR CLASSIC RESUME / LB RB SLOT",FG);
  if(!private_title_preview_valid||private_preview_profile!=app.rules_profile)refresh_private_title_preview();
  if(private_title_preview.status==GK170_SLOT_VERIFIED){
   snprintf(line,sizeof line,
    "PRIVATE SLOT %u: VERIFIED %s / SPRING %u / %u COUNTRIES / %u ORDERS",
    (unsigned)campaign_slot,
    gk77_profile_name((Gk77Profile)private_title_preview.profile),
    (unsigned)private_title_preview.year,
    (unsigned)private_title_preview.country_count,
    (unsigned)private_title_preview.orders_remaining);
   label(150,570,line,EDGE);
  }else{
   snprintf(line,sizeof line,"PRIVATE SLOT %u: %s (Y RESTORE FAIL-CLOSED)",
     (unsigned)campaign_slot,
     private_title_preview.status==GK170_SLOT_UNAVAILABLE?
       "MISSING OR SECURITY-REFUSED": "SOURCE-INVALID OR NO CANONICAL CSV");
   label(150,570,line,WARN);
  }
  if(app.rules_profile==1u){
   if(practice_title_status==GK227_PREVIEW_OK){
    snprintf(line,sizeof line,
     "ENHANCED PRACTICE SLOT %u: %s / COUNTRY %u / %u OF 3 ACTIONS / %u LEFT",
     (unsigned)campaign_slot,
     practice_title_preview.scenario==1u?"MONGOL":"WORLD",
     (unsigned)practice_title_preview.player_index+1u,
     (unsigned)practice_title_preview.actions_committed,
     (unsigned)practice_title_preview.orders_remaining);
    label(150,595,line,EDGE);
   }else{
    snprintf(line,sizeof line,"ENHANCED PRACTICE SLOT %u: %s (NO STOCK NES SPRING)",
      (unsigned)campaign_slot,
      practice_title_status==GK227_PREVIEW_MISSING?"EMPTY":
      practice_title_status==GK227_PREVIEW_SOURCE?"SOURCE REFUSED":
      practice_title_status==GK227_PREVIEW_CORRUPT?"CORRUPT - NO LOAD":"INACCESSIBLE");
    label(150,595,line,WARN);
   }
  }else label(150,595,"PREVIEW REVEALS NO LICENSED RAM; NOT A FULL CAMPAIGN SAVE",WARN);
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
  snprintf(line,sizeof line,"A TRANSFER / LB UNDO / X SAVE Y LOAD SLOT %u / RB NEXT",(unsigned)campaign_slot);
  label(670,438,line,FG);
  label(104,527,"START x2 = CLASSIC FINANCE / ENHANCED DEMOBILIZE PREVIEW",EDGE);
  label(104,557,"PRE-SETUP EDIT ONLY. NO ORIGINAL MOBILITY, ORDER, COST OR SPRING CLAIM.",WARN);
 }else if(app.screen==GK1_SCREEN_SPRING_FINANCE_PREVIEW){
  const Gk1LoadedScenario *world=&app.worlds[app.active_world];
  Gk217Preview v;
  frame(62,130,1152,475);
  label(100,174,"CLASSIC ORIGINAL-SOURCE SPRING FINANCE MODEL / NOT GAME START",EDGE);
  if(gk217_preview_country(world,app.finance_cursor,&v)==GK217_OK){
    const Gk1CountryTemplate *c=&world->countries[v.selected_country];
    snprintf(line,sizeof line,"COUNTRY %u/%u: %.30s",(unsigned)v.selected_country+1u,
      (unsigned)v.country_count,c->country_name);label(102,224,line,FG);
    snprintf(line,sizeof line,"GOLD %u -> %u   TAX +%u   WAGES -%u",
      (unsigned)v.before_gold,(unsigned)v.after_gold,(unsigned)v.taxes,(unsigned)v.payroll);
    label(102,276,line,FG);
    snprintf(line,sizeof line,"FOOD %u -> %u   FEEDING -%u",
      (unsigned)v.before_food,(unsigned)v.after_food,(unsigned)v.food_consumed);
    label(102,326,line,FG);
    snprintf(line,sizeof line,"TROOPS %u -> %u   STARVATION LOSS %u",
      (unsigned)v.before_troops,(unsigned)v.after_troops,(unsigned)v.troops_lost);
    label(102,376,line,FG);
    snprintf(line,sizeof line,"ALL %u: TAX %u  WAGES %u  FOOD %u  STARVED %u",
      (unsigned)v.country_count,(unsigned)v.total_taxes,(unsigned)v.total_payroll,
      (unsigned)v.total_food_consumed,(unsigned)v.countries_starved);
    label(102,428,line,EDGE);
  }else label(102,292,"COUNTRY SOURCE OR FINANCE REFUSED",WARN);
  if(app.finance_compare){
   Gk219Comparison delta;
   if(gk219_compare_finance(&app.finance_source_baseline[app.active_world],world,
      app.finance_cursor,&delta)==GK219_OK){
    snprintf(line,sizeof line,"PORT LABOR PLAN vs SOURCE: GOLD %+ld  FOOD %+ld  TROOPS %+ld",
     (long)delta.projected_gold_delta,(long)delta.projected_food_delta,
     (long)delta.projected_troops_delta);label(102,457,line,EDGE);
    snprintf(line,sizeof line,"ALL COUNTRY PLAN EFFECT: GOLD %+lld  FOOD %+lld",
     (long long)delta.all_countries_gold_delta,
     (long long)delta.all_countries_food_delta);label(102,483,line,EDGE);
   }else label(102,470,"LABOR COMPARE REFUSED: SOURCE DRIFT OR INVALID WORLD",WARN);
  }
  label(102,521,"DPAD COUNTRY / A TOGGLE LABOR COMPARISON / B OR START BACK",FG);
  label(102,560,"NO NES RNG, CHARACTER SETUP, SEASON, AI OR 3 ORDERS",WARN);
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
 }else if(app.screen==GK1_SCREEN_ENHANCED_SPRING_LAB){
  const Gk222Spring *lab=&app.port_spring;
  const Gk1CountryTemplate *c=&lab->working.countries[lab->selected_country];
  frame(78,143,1124,455);
  label(108,185,"ENHANCED PORT SPRING FINANCE PRACTICE - NOT ORIGINAL NES",WARN);
  snprintf(line,sizeof line,"REFERENCE YEAR %u | PORT ORDERS %u/3 | COUNTRY %u/%u",
    (unsigned)lab->reference_year,(unsigned)lab->remaining,
    (unsigned)lab->selected_country+1u,(unsigned)lab->working.country_count);
  label(108,243,line,FG);
  snprintf(line,sizeof line,"%s: GOLD %u FOOD %u TROOPS %u",
    c->country_name,(unsigned)c->gold,(unsigned)c->goods[10],(unsigned)c->troops);
  label(108,291,line,EDGE);
  snprintf(line,sizeof line,"TOWN %u | FOOD WORKERS %u | PORT ACTION: %s",
    (unsigned)c->town,(unsigned)c->food_workers,
    app.port_spring_command?"TOWN TO FOOD +1":"PASS");
  label(108,339,line,FG);
  snprintf(line,sizeof line,"SOURCE FINANCE TOTAL TAX %u / PAYROLL %u / FOOD USED %u",
    (unsigned)lab->taxes,(unsigned)lab->payroll,(unsigned)lab->food_used);
  label(108,386,line,EDGE);
  label(108,427,"DPAD COUNTRY/ACTION | A SPEND | LB UNDO | X SAVE / Y LOAD / B EXIT",FG);
  label(108,480,"ONLY PLAYER COUNTRY CAN ACT; 3 ORDERS. NO ORIGINAL RNG/AI/SEASON.",WARN);
  label(108,519,"PRACTICE RECEIPTS ONLY - SLOT-SAVED; CLASSIC SOURCE UNCHANGED.",WARN);
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
   /* EXEC0234: real 1-4 player setup audit on the gameplay screen.
    * Show the committed human lords only. Unchosen slots remain blank;
    * never imply an initialized ruler, original ROM RNG, or Spring turn. */
   if(w==1u){
    char picked[4][8];
    for(unsigned j=0;j<4u;j++){
     uint8_t ix=app.setup_rulers[1][j];
     if(j>=app.setup_player_count[1]||ix==255u)
      snprintf(picked[j],sizeof picked[j],"--");
     else snprintf(picked[j],sizeof picked[j],"%u",(unsigned)ix+1u);
    }
    snprintf(line,sizeof line,"COMMITTED WORLD LORD COUNTRIES: P1 %s / P2 %s / P3 %s / P4 %s",
     picked[0],picked[1],picked[2],picked[3]);
    label(113,372,line,EDGE);
   }
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
   if(app.rules_profile==1 && !app.manual_bonus_active[w])
     label(113,539,"A: ENTER ENHANCED FINANCE PRACTICE (PORT-ONLY)",EDGE);
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
 }else if(app.screen==GK1_SCREEN_STOCK_STORAGE){
  const Gk79Checkpoint *cp=&app.stock_checkpoint;
  frame(72,135,1134,473);
  label(104,174,"PORT-ONLY OTHER / OWNER-PRIVATE SPRING CHECKPOINT",EDGE);
  snprintf(line,sizeof line,"STOCK SOURCE %s / SPRING %u / %u COUNTRIES / SLOT %u",
   gk77_profile_name((Gk77Profile)cp->profile),(unsigned)cp->year,
   (unsigned)cp->world.country_count,(unsigned)campaign_slot);
  label(104,215,line,FG);
  static const char *const choices[3]={
   "SAVE NEW PRIVATE SLOT (DO NOT OVERWRITE)",
   "RESTORE THIS PRIVATE SLOT (COLD-LOAD)","RETURN TO COMMANDS"};
  for(unsigned i=0;i<3u;i++){
   snprintf(line,sizeof line,"%s %s",app.stock_storage_cursor==i?">":" ",choices[i]);
   label(104,276+(int)i*65,line,app.stock_storage_cursor==i?EDGE:FG);
  }
  label(104,496,app.stock_storage_confirm?
     "CONFIRM: A EXECUTE / B CANCEL - ORIGINAL SOURCE MUST PASS HASH GATE":
     "DPAD SELECT / A CHOOSE / B CANCEL / LB-RB SELECT PRIVATE SLOT",FG);
  label(104,541,"PORT UTILITY, NOT AUTHENTIC NES OTHER COMMAND OR FULL CAMPAIGN SAVE",WARN);
  label(104,574,"SAVE SPENDS NO ORDER; RESTORE REPLACES THE CHECKPOINT IF VERIFIED",WARN);
 }else if(app.screen==GK1_SCREEN_STOCK_COMMANDS){
  const Gk79Checkpoint *cp=&app.stock_checkpoint;
  frame(72,135,1134,473);
  label(101,166,"ORIGINAL KOEI NES 14-COMMAND SPRING MENU - AUTHENTIC SOURCE",EDGE);
  if(!cp->valid || !gk85_raw_source_intact(cp)){
   label(104,260,"ORIGINAL SOURCE INVALID: MENU INACCESSIBLE",WARN);
  }else{
   uint8_t effective=0u;
   Gk119Result order_status=gk119_effective_remaining(cp,&app.enhanced_assign,
        app.rules_profile,&effective);
   if(app.rules_profile==0u && app.classic_assign.active &&
      gk187_intact(&app.classic_assign,cp))effective=app.classic_assign.budget.remaining;
   int summer_mode=app.rules_profile==1u &&
     gk158_attestation_intact(&app.enhanced_summer,&app.enhanced_assign,cp);
   if(summer_mode)effective=app.enhanced_summer.budget.remaining;
   snprintf(line,sizeof line,"%u %s / %s %u ORDERS LEFT / COUNTRY %u / PROFILE %s",
    (unsigned)cp->year,summer_mode?"SUMMER (ENHANCED)":"SPRING",
    order_status==GK119_OK?(app.enhanced_assign.active?"ENHANCED":"BASE"):"SOURCE REFUSED",
    order_status==GK119_OK?(unsigned)effective:0u,(unsigned)cp->player_index+1u,
    gk77_profile_name((Gk77Profile)cp->profile));label(104,204,line,FG);
   for(unsigned i=0;i<14u;i++){
    unsigned column=i/7u,row=i%7u;
    snprintf(line,sizeof line,"%s %2u. %-8s %s",
     app.stock_command_cursor==i?">":" ",i+1u,
     gk87_stock_command_name(i),i==6u?"READ ONLY":i==4u?"PRICE VIEW":i==12u?"AVAILABLE":
     i==13u?"PORT SAVE":i==1u?(app.rules_profile?"ENHANCED":"MANUAL") :"LOCKED");
    label(105+(int)column*510,242+(int)row*44,line,
       app.stock_command_cursor==i?EDGE:i==6u||i==4u||i==12u?FG:WARN);
   }
   label(104,555,cp->pending_pass?"A CONFIRM PASS / B CANCEL (NO CHANGE)":
     "DPAD SELECT / A COMMAND / B COUNTRY / START VERIFY PRIVATE ROM",FG);
   label(104,583,summer_mode?"ENHANCED SUMMER LABOR+PASS ONLY; NO ORIGINAL SEASON ECONOMY OR AI":
     "CLASSIC X SAVE / Y RESTORE; OTHER=PORT STORAGE; NES ACTIONS STILL LOCKED",WARN);
  }
 }else if(app.screen==GK1_SCREEN_ENHANCED_CONTINUATION){
  static const char *pool_names[5]={"TROOPS","TOWN","MASONS","ARTISANS","FOOD WORKERS"};
  static const char *seasons[4]={"SPRING","SUMMER","FALL","WINTER"};
  const Gk176EnhancedCycle *e=&app.enhanced_continuation;
  const Gk1CountryTemplate *c=&e->mutable_world.countries[app.enhanced_assign.player_index];
  frame(72,135,1134,473);
  label(104,166,"ENHANCED LABOR CONTINUATION - NOT ORIGINAL NES GAMEPLAY",EDGE);
  snprintf(line,sizeof line,"%s %u / SEASON %u OF %u / %u ORDERS LEFT",
   seasons[e->budget.season],(unsigned)e->year,
   (unsigned)e->season_index+1u,(unsigned)GK176_MAX_SEASONS,
   (unsigned)e->budget.remaining);label(104,208,line,FG);
  snprintf(line,sizeof line,"FROM %s / TO %s / QUANTITY %u",
   pool_names[app.assign_from],pool_names[app.assign_to],
   (unsigned)app.assign_quantity);label(104,255,line,EDGE);
  snprintf(line,sizeof line,"TROOPS %u TOWN %u MASONS %u ARTISANS %u FARM %u",
   (unsigned)c->troops,(unsigned)c->town,(unsigned)c->masons,
   (unsigned)c->artisans,(unsigned)c->food_workers);label(104,311,line,FG);
  label(104,367,"DPAD POOLS / START+ VIEW- QUANTITY / A MOVE / B COMMAND MENU",FG);
  label(104,405,"COMMAND MENU > PASS: END SEASON; THEN PASS AGAIN TO ADVANCE",FG);
  label(104,448,"NO NES FOOD/TAX/RNG/AI/DYNASTY/MERCHANT OR ORIGINAL SEASON PROCESS",WARN);
  label(104,496,"CLASSIC STOCK SOURCE IMMUTABLE; CONTINUATION SAVES NOT YET IMPLEMENTED",WARN);
 }else if(app.screen==GK1_SCREEN_ENHANCED_YEAR2){
  static const char *pool_names[5]={"TROOPS","TOWN","MASONS","ARTISANS","FOOD WORKERS"};
  const Gk161EnhancedYear2 *e=&app.enhanced_year2;
  const Gk1CountryTemplate *c=&e->mutable_world.countries[app.enhanced_assign.player_index];
  frame(72,135,1134,473);
  label(104,166,"ENHANCED YEAR-2 SPRING - NOT ORIGINAL NES SPRING",EDGE);
  snprintf(line,sizeof line,"SPRING %u / %u LABOR ORDERS LEFT / ACTIONS %u / PASS %s",
    (unsigned)e->year,(unsigned)e->budget.remaining,(unsigned)e->action_count,
    e->pass_ended?"COMPLETE":"AVAILABLE");label(104,208,line,FG);
  snprintf(line,sizeof line,"FROM %s / TO %s / QUANTITY %u",pool_names[app.assign_from],
   pool_names[app.assign_to],(unsigned)app.assign_quantity);label(104,255,line,EDGE);
  snprintf(line,sizeof line,"TROOPS %u TOWN %u MASONS %u ARTISANS %u FARM %u",
   (unsigned)c->troops,(unsigned)c->town,(unsigned)c->masons,
   (unsigned)c->artisans,(unsigned)c->food_workers);label(104,311,line,FG);
  label(104,367,"DPAD POOLS / START+ VIEW- QUANTITY / A MOVE / B COMMAND MENU",FG);
  label(104,405,"COMMAND MENU > PASS: END LABOR / X EXCLUSIVE SAVE / Y COLD LOAD",FG);
  label(104,448,"NO NES TAX/FOOD/AGE/RNG/AI/DYNASTY/TRADE PROCESSING",WARN);
  label(104,496,"CLASSIC STOCK STILL LOCKED; ORIGINAL CHECKPOINT IMMUTABLE",WARN);
 }else if(app.screen==GK1_SCREEN_ENHANCED_WINTER){
  static const char *pool_names[5]={"TROOPS","TOWN","MASONS","ARTISANS","FOOD WORKERS"};
  const Gk160EnhancedWinter *e=&app.enhanced_winter;
  const Gk1CountryTemplate *c=&e->mutable_world.countries[app.enhanced_assign.player_index];
  frame(72,135,1134,473);
  label(104,166,"ENHANCED RESEARCH WINTER - NOT ORIGINAL NES COLD/AI",EDGE);
  snprintf(line,sizeof line,"WINTER %u / %u ORDERS LEFT / ACTIONS %u / PASS %s",
    (unsigned)app.enhanced_assign.year,(unsigned)e->budget.remaining,
    (unsigned)e->action_count,e->pass_ended?"COMPLETE":"AVAILABLE");label(104,208,line,FG);
  snprintf(line,sizeof line,"FROM %s / TO %s / QUANTITY %u",
    pool_names[app.assign_from],pool_names[app.assign_to],
    (unsigned)app.assign_quantity);label(104,255,line,EDGE);
  snprintf(line,sizeof line,"TROOPS %u TOWN %u MASONS %u ARTISANS %u FARM %u",
    (unsigned)c->troops,(unsigned)c->town,(unsigned)c->masons,
    (unsigned)c->artisans,(unsigned)c->food_workers);label(104,311,line,FG);
  label(104,367,"DPAD POOLS / START+ VIEW- QUANTITY / A MOVE / B COMMAND MENU",FG);
  label(104,405,"COMMAND MENU > PASS: COMPLETE WINTER / X SAVE / Y COLD LOAD",FG);
  label(104,448,"NO ORIGINAL NES COLD/AI, TAX, FOOD, STORMS, RNG, MERCHANT OR AI",WARN);
  label(104,496,"CLASSIC STILL LOCKED; AUTHENTIC STOCK SPRING IMMUTABLE",WARN);
 }else if(app.screen==GK1_SCREEN_ENHANCED_FALL){
  static const char *pool_names[5]={"TROOPS","TOWN","MASONS","ARTISANS","FOOD WORKERS"};
  const Gk159EnhancedFall *e=&app.enhanced_fall;
  const Gk1CountryTemplate *c=&e->mutable_world.countries[app.enhanced_assign.player_index];
  frame(72,135,1134,473);
  label(104,166,"ENHANCED RESEARCH FALL - NOT ORIGINAL NES HARVEST",EDGE);
  snprintf(line,sizeof line,"FALL %u / %u ORDERS LEFT / ACTIONS %u / PASS %s",
    (unsigned)app.enhanced_assign.year,(unsigned)e->budget.remaining,
    (unsigned)e->action_count,e->pass_ended?"COMPLETE":"AVAILABLE");label(104,208,line,FG);
  snprintf(line,sizeof line,"FROM %s / TO %s / QUANTITY %u",
    pool_names[app.assign_from],pool_names[app.assign_to],
    (unsigned)app.assign_quantity);label(104,255,line,EDGE);
  snprintf(line,sizeof line,"TROOPS %u TOWN %u MASONS %u ARTISANS %u FARM %u",
    (unsigned)c->troops,(unsigned)c->town,(unsigned)c->masons,
    (unsigned)c->artisans,(unsigned)c->food_workers);label(104,311,line,FG);
  label(104,367,"DPAD POOLS / START+ VIEW- QUANTITY / A MOVE / B COMMAND MENU",FG);
  label(104,405,"COMMAND MENU > PASS: COMPLETE FALL / X SAVE / Y COLD LOAD",FG);
  label(104,448,"NO ORIGINAL NES HARVEST, TAX, FOOD, STORMS, RNG, MERCHANT OR AI",WARN);
  label(104,496,"CLASSIC STILL LOCKED; AUTHENTIC STOCK SPRING IMMUTABLE",WARN);
 }else if(app.screen==GK1_SCREEN_ENHANCED_SUMMER){
  static const char *pool_names[5]={"TROOPS","TOWN","MASONS","ARTISANS","FOOD WORKERS"};
  const Gk158EnhancedSummer *e=&app.enhanced_summer;
  const Gk1CountryTemplate *c=&e->mutable_world.countries[app.enhanced_assign.player_index];
  frame(72,135,1134,473);
  label(104,166,"ENHANCED RESEARCH SUMMER - NOT ORIGINAL NES SEASONAL GAMEPLAY",EDGE);
  snprintf(line,sizeof line,"SUMMER %u / %u ORDERS LEFT / ACTIONS %u / PASS %s",
    (unsigned)app.enhanced_assign.year,(unsigned)e->budget.remaining,
    (unsigned)e->action_count,e->pass_ended?"COMPLETE":"AVAILABLE");label(104,208,line,FG);
  snprintf(line,sizeof line,"FROM %s / TO %s / QUANTITY %u",
    pool_names[app.assign_from],pool_names[app.assign_to],
    (unsigned)app.assign_quantity);label(104,255,line,EDGE);
  snprintf(line,sizeof line,"TROOPS %u TOWN %u MASONS %u ARTISANS %u FARM %u",
    (unsigned)c->troops,(unsigned)c->town,(unsigned)c->masons,
    (unsigned)c->artisans,(unsigned)c->food_workers);label(104,311,line,FG);
  label(104,367,"DPAD POOLS / START+ VIEW- QUANTITY / A MOVE / B COMMAND MENU",FG);
  label(104,405,"COMMAND MENU > PASS: COMPLETE SUMMER / X SAVE / Y COLD LOAD",FG);
  label(104,448,"NO NES TAX, PAYROLL, FOOD, GROWTH, STORMS, MERCHANT RNG OR AI",WARN);
  label(104,496,"CLASSIC STILL LOCKED; AUTHENTIC STOCK SPRING REMAINS IMMUTABLE",WARN);
 }else if(app.screen==GK1_SCREEN_CLASSIC_TAX){
  frame(72,135,1134,473);
  label(104,166,"CLASSIC TAX RATE - MANUAL-DERIVED; NES ABILITY COSTS UNKNOWN",EDGE);
  snprintf(line,sizeof line,"SPRING %u / TAX %u / ORDERS LEFT %u / ACTIONS %u",
   (unsigned)app.stock_checkpoint.year,(unsigned)app.classic_tax_selection,
   (unsigned)app.classic_assign.budget.remaining,
   (unsigned)app.classic_assign.action_count);
  label(104,230,line,FG);
  label(104,286,"UP/DOWN: +/-1  LEFT/RIGHT: +/-10 (BOUNDED 1 TO 100)",FG);
  label(104,343,"A COMMIT ONE ORDER / Y UNDO LAST TAX / B RETURN",FG);
  Gk198Forecast tax_preview;
  if(gk198_classic_tax_forecast(&app.classic_assign,
     &app.stock_checkpoint,app.classic_tax_selection,
     &tax_preview)==GK198_OK){
   snprintf(line,sizeof line,
    "SOURCE-TRACED SPRING MATH ONLY: GROSS %u -> %u / NET GOLD %u -> %u",
    (unsigned)tax_preview.committed.gross_tax,
    (unsigned)tax_preview.proposed.gross_tax,
    (unsigned)tax_preview.committed.after.gold,
    (unsigned)tax_preview.proposed.after.gold);
   label(104,386,line,EDGE);
  }else label(104,386,"FORECAST REFUSED: ORIGINAL/DERIVED SOURCE INVALID",WARN);
  label(104,410,"ORIGINAL COUNTRY RAM NEVER MODIFIED; EXACT MORALE AND",WARN);
  label(104,445,"LEADERSHIP/CHARM EFFECTS NOT YET RECONSTRUCTED",WARN);
  label(104,507,"THIS IS NOT ORIGINAL NES TAX COMMAND PARITY",WARN);
 }else if(app.screen==GK1_SCREEN_CLASSIC_ASSIGN){
  static const char *pools[5]={"TROOPS","TOWN","MASONS","ARTISANS","FOOD WORKERS"};
  const Gk187ClassicAssign *c=&app.classic_assign;
  const Gk1CountryTemplate *world=&c->mutable_world.countries[c->player_index];
  frame(72,135,1134,473);
  label(104,166,"CLASSIC ASSIGN - MANUAL-DERIVED; ORIGINAL NES PARITY NOT YET TESTED",EDGE);
  snprintf(line,sizeof line,"SPRING %u / ACTIONS %u / ORDERS LEFT %u",
     (unsigned)app.stock_checkpoint.year,(unsigned)c->action_count,(unsigned)c->budget.remaining);
  label(104,208,line,FG);
  snprintf(line,sizeof line,"FROM (UP/DOWN): %s  TO (LEFT/RIGHT): %s",
     pools[app.assign_from],pools[app.assign_to]);label(104,259,line,EDGE);
  snprintf(line,sizeof line,"TROOPS %u TOWN %u MASONS %u ARTISANS %u FARM %u",
    (unsigned)world->troops,(unsigned)world->town,(unsigned)world->masons,
    (unsigned)world->artisans,(unsigned)world->food_workers);label(104,313,line,FG);
  snprintf(line,sizeof line,"QUANTITY %u (START + / VIEW -): A COMMIT / B RETURN",
    (unsigned)app.assign_quantity);label(104,365,line,FG);
  label(104,424,"SOURCE 83-BYTE ORIGINAL RECORDS UNCHANGED. NO ORIGINAL RNG OR AI.",WARN);
  label(104,472,"Y: UNDO LAST UNPASSED ASSIGN, EVEN THIRD (PORT-ONLY) / X: PRIVATE SAVE",FG);
  label(104,503,"TITLE Y: COLD LOAD GC19 / OTHER SCREEN Y: LOAD; NEVER ORIGINAL RNG",WARN);
 }else if(app.screen==GK1_SCREEN_ENHANCED_ASSIGN){
  static const char *pools[5]={"TROOPS","TOWN","MASONS","ARTISANS","FOOD WORKERS"};
  const Gk116EnhancedAssign *e=&app.enhanced_assign;
  const Gk1CountryTemplate *c=&e->mutable_world.countries[e->player_index];
  frame(72,135,1134,473);
  label(104,166,"ENHANCED LABOR ASSIGN - NOT ORIGINAL KOEI NES RULES",EDGE);
  snprintf(line,sizeof line,"ORIGINAL SPRING %u / %u ASSIGN ACTIONS / %u ORDERS LEFT / PASS %s",
    (unsigned)e->year,(unsigned)e->action_count,(unsigned)e->budget.remaining,
    e->pass_ended?"COMPLETE":"AVAILABLE");label(104,208,line,FG);
  snprintf(line,sizeof line,"FROM (UP/DOWN): %s   TO (LEFT/RIGHT): %s",
    pools[app.assign_from],pools[app.assign_to]);label(104,259,line,EDGE);
  snprintf(line,sizeof line,"TROOPS %u   TOWN %u   MASONS %u   ARTISANS %u   FARM %u",
    (unsigned)c->troops,(unsigned)c->town,(unsigned)c->masons,
    (unsigned)c->artisans,(unsigned)c->food_workers);label(104,313,line,FG);
  snprintf(line,sizeof line,"QUANTITY %u (START + / VIEW -): A TRANSFER (ONE ENHANCED ORDER)",
    (unsigned)app.assign_quantity);label(104,365,line,FG);
  label(104,393,"B RETURN / X SAVE / Y COLD LOAD / D-PAD FROM & TO POOLS",FG);
  label(104,424,"ORIGINAL STOCK DATA REMAINS IMMUTABLE; CLASSIC ASSIGN IS STILL LOCKED",WARN);
 }else if(app.screen==GK1_SCREEN_STOCK_TRADE_REFERENCE){
  static const char *const traders[3]={"CHINA","ISLAM","UIGHUR"};
  const Gk79Checkpoint *cp=&app.stock_checkpoint;
  Gk61Quote q={0};
  Gk95QuoteResult res=gk95_stock_trade_reference(cp,
     (Gk61Merchant)app.stock_trade_merchant,app.stock_trade_item,&q);
  frame(72,135,1134,473);
  label(104,178,"ORIGINAL NES SPRING RATE - SOURCE-BOUND TRADE PRICE REFERENCE",EDGE);
  uint8_t effective=0u;
  Gk119Result order_status=gk119_effective_remaining(cp,&app.enhanced_assign,
        app.rules_profile,&effective);
  Gk196TradeStatus classic={0};
  Gk196TradeStatusResult classic_result=GK196_SOURCE;
  if(app.rules_profile==0u){
   classic_result=gk196_classic_trade_status(&app,&classic);
   if(classic_result==GK196_OK)effective=classic.remaining;
  }
  const char *budget_label=app.rules_profile==0u?
      (classic_result==GK196_OK?(classic.derived?"CLASSIC DERIVED":"CLASSIC BASE"):
       "SOURCE INVALID"):(app.enhanced_assign.active?"ENHANCED":"BASE");
  snprintf(line,sizeof line,"PROFILE %s / SPRING %u / RATE %u / %s ORDERS %u",
    gk77_profile_name((Gk77Profile)cp->profile),(unsigned)cp->year,
    (unsigned)cp->rate,budget_label,
    app.rules_profile==0u?(classic_result==GK196_OK?(unsigned)effective:0u):
    (order_status==GK119_OK?(unsigned)effective:0u));
  label(104,222,line,FG);
  snprintf(line,sizeof line,"MERCHANT %s  /  ITEM %u OF 17  /  MODE %s",
    traders[app.stock_trade_merchant],(unsigned)app.stock_trade_item+1u,
    app.stock_trade_sell?"SELL QUOTE":"BUY QUOTE");
  label(104,286,line,EDGE);
  if(res==GK95_QUOTE_OK){
    snprintf(line,sizeof line,"BUY %u.%u GOLD     SELL %u.%u GOLD",
      (unsigned)q.buy_tenths/10,(unsigned)q.buy_tenths%10,
      (unsigned)q.sell_tenths/10,(unsigned)q.sell_tenths%10);
    label(104,342,line,FG);
  }else label(104,342,"SOURCE IDENTITY INVALID: QUOTES NOT AVAILABLE",WARN);
  label(104,398,"DPAD LEFT/RIGHT MERCHANT; UP/DOWN ITEM; A BUY/SELL; B BACK",FG);
  label(104,457,"QUOTE MATH ONLY - MERCHANT PRESENCE, OFFERS AND RNG UNKNOWN",WARN);
  if(app.rules_profile==0u && classic_result==GK196_OK && classic.derived){
    snprintf(line,sizeof line,"CLASSIC ASSIGN ACTIONS %u / PASS %s; NO TRADE ORDER SPENT",
      (unsigned)classic.actions,classic.passed?"ENDED":"AVAILABLE");
    label(104,495,line,EDGE);
  }else label(104,495,"NO TRADE EXECUTED OR ORDER SPENT",FG);
  label(104,539,"ORIGINAL NES SOURCE IMMUTABLE; TRADE PRESENCE/AI UNIMPLEMENTED",WARN);
 }else if(app.screen==GK1_SCREEN_STOCK_CONTINUE){
  const Gk79Checkpoint *cp=&app.stock_checkpoint;
  frame(72,135,1134,473);
  if(cp->valid && cp->view_index<cp->world.country_count){
   const Gk1CountryTemplate *c=&cp->world.countries[cp->view_index];
   snprintf(line,sizeof line,"ORIGINAL STOCK %u SPRING CHECKPOINT - %s / %u COUNTRIES",
    (unsigned)cp->year,gk77_profile_name((Gk77Profile)cp->profile),
    (unsigned)cp->world.country_count);label(104,185,line,EDGE);
   label(104,216,"VERIFIED RECORD RESTORATION, NOT RECONSTRUCTED STARTUP / RNG",WARN);
   if(app.stock_reconcile_view==1u){
    Gk1LoadedScenario pristine;
    Gk89SpringReconciliation gap;
    Gk90Result status=GK90_TEMPLATE_DRIFT;
    if(scenario_original&&scenario_loaded&&
       gk1_open_loaded_scenario(scenario_original,cp->world.scenario_id,&pristine)==GK1_LOAD_OK)
      status=gk90_compare_spring_verified(scenario_csv_path(),&pristine,cp,cp->view_index,&gap);
    label(104,261,"SOURCE-VERIFIED FINANCE PROJECTION vs ACTUAL FIRST SPRING",EDGE);
    if(status!=GK90_OK){
     snprintf(line,sizeof line,"COMPARISON REFUSED: CODE %u / NO SOURCE MUTATED",(unsigned)status);
     label(104,320,line,WARN);
    }else{
     snprintf(line,sizeof line,"COUNTRY %u: PRE_SETUP        FOOD %3u  GOLD %3u  TROOPS %3u",
      gap.country_id,gap.template_food,gap.template_gold,gap.template_troops);label(104,309,line,FG);
     snprintf(line,sizeof line,"KNOWN FINANCE ONLY:          FOOD %3u  GOLD %3u  TROOPS %3u",
      gap.finance_food,gap.finance_gold,gap.finance_troops);label(104,354,line,FG);
     snprintf(line,sizeof line,"OBSERVED STOCK SPRING:       FOOD %3u  GOLD %3u  TROOPS %3u",
      gap.observed_food,gap.observed_gold,gap.observed_troops);label(104,398,line,FG);
     snprintf(line,sizeof line,"UNEXPLAINED RESIDUAL:        FOOD %+4ld GOLD %+4ld TROOPS %+4ld",
      (long)gap.residual_food,(long)gap.residual_gold,(long)gap.residual_troops);label(104,442,line,EDGE);
    }
    Gk111StockSpringAudit coverage;
    Gk111Result coverage_result=gk1_app_stock_spring_audit(&app,scenario_csv_path(),&coverage);
    if(coverage_result==GK111_OK){
     snprintf(line,sizeof line,"WORLD %u: MATCH FOOD %u / GOLD %u / TROOPS %u / ALL THREE %u",
      (unsigned)coverage.country_count,(unsigned)coverage.food_exact,
      (unsigned)coverage.gold_exact,(unsigned)coverage.troops_exact,
      (unsigned)coverage.all_three_exact);
     label(104,488,line,EDGE);
    }else{
     snprintf(line,sizeof line,"FULL WORLD GAP REFUSED (%u): VERIFIED STOCK SOURCE NEEDED",(unsigned)coverage_result);
     label(104,488,line,WARN);
    }

    label(104,536,"NO FIRST-SPRING INITIALIZER; NO SEASON, ORDER OR COUNTRY MUTATION",WARN);
    label(104,566,"VIEW CYCLES DECODED / RAW / FINANCE / DELTAS / GOODS; DPAD COUNTRY",FG);
   }else if(app.stock_reconcile_view==2u){
    Gk112StockDeltas setup={0};
    Gk112Status ds=gk1_app_stock_setup_deltas(&app,scenario_csv_path(),&setup);
    label(104,246,"24 SOURCE-VERIFIED FIELDS: STOCK SPRING MINUS PRE_SETUP",EDGE);
    if(ds!=GK112_OK){
     snprintf(line,sizeof line,"SOURCE DELTA REFUSED (%u): ALL COUNTRY STATE UNCHANGED",(unsigned)ds);
     label(104,330,line,WARN);
    }else{
     unsigned country=cp->view_index;
     unsigned changed=setup.fields_changed_in_country[country];
     snprintf(line,sizeof line,"COUNTRY %u / %u: %u OF 24 FIELDS CHANGED; NOT AN INITIALIZATION FORMULA",
      country+1u,(unsigned)setup.country_count,changed);
     label(104,281,line,FG);
     for(unsigned row=0;row<6u;row++){
      size_t pos=0;
      line[0]='\0';
      for(unsigned col=0;col<4u;col++){
       unsigned metric=row*4u+col;
       int32_t d=setup.delta[country][metric];
       int used=snprintf(line+pos,sizeof line-pos,"%-13.13s %+5ld   ",
        gk112_field_name(metric),(long)d);
       if(used<0||(size_t)used>=sizeof line-pos)break;
       pos+=(size_t)used;
      }
      label(104,318+(int)row*38,line,FG);
     }
     label(104,570,"DELTA = CAPTURED STOCK - VERIFIED TEMPLATE; SETUP/RNG/SEASON MIXED",WARN);
    }
   }else if(app.stock_reconcile_view==3u){
    Gk147StockGoods stock_goods={0};
    Gk147GoodsStatus gs=gk1_app_source_goods_view(&app,cp->view_index,&stock_goods);
    label(104,245,"TEN ORIGINAL STOCK SPECIALTY GOODS (RAW RECORD +0x22)",EDGE);
    if(gs!=GK147_GOODS_OK){
     snprintf(line,sizeof line,"SOURCE GOODS REFUSED (%u); NO TRADE OR ORDER EXECUTED",(unsigned)gs);
     label(104,322,line,WARN);
    }else{
     for(unsigned i=0;i<10u;i++){
      const unsigned column=i/5u,row=i%5u;
      snprintf(line,sizeof line,"%-11s %5u",gk147_goods_name(i),(unsigned)stock_goods.goods[i]);
      label(104+(int)column*460,295+(int)row*43,line,FG);
     }
     snprintf(line,sizeof line,"COUNTRY %u / %u   FOOD %u    ORIGINAL STOCK YEAR %u",
       (unsigned)stock_goods.country_index+1u,(unsigned)stock_goods.country_count,
       (unsigned)stock_goods.food,(unsigned)stock_goods.year);
     label(104,540,line,EDGE);
    }
    label(104,575,"VIEW DECODED / DPAD COUNTRY; NO MERCHANT RNG OR TRADE",WARN);
   }else if(cp->raw_view){
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
   Gk195CountryView cv={0};
   int classic_view=app.rules_profile==0u &&
    gk195_classic_country_view(&app,cp->view_index,&cv)==GK195_OK;
   if(classic_view&&cv.is_player&&cv.derived){
    static const char *pools[5]={"TROOPS","TOWN","MASONS","ARTISANS","FARM"};
    const Gk1CountryTemplate *a=&cv.original,*b=&cv.current;
    snprintf(line,sizeof line,"CLASSIC DERIVED PLAYER %u / %u - ACTIONS %u / ORDERS LEFT %u",
     (unsigned)cv.country_index+1u,(unsigned)cp->world.country_count,
     (unsigned)cv.actions,(unsigned)cv.remaining);label(104,258,line,EDGE);
    snprintf(line,sizeof line,"%s: ORIGINAL %u -> CURRENT %u",pools[0],
     (unsigned)a->troops,(unsigned)b->troops);label(104,295,line,FG);
    snprintf(line,sizeof line,"%s %u -> %u   %s %u -> %u",pools[1],
     (unsigned)a->town,(unsigned)b->town,pools[2],
     (unsigned)a->masons,(unsigned)b->masons);label(104,333,line,FG);
    snprintf(line,sizeof line,"%s %u -> %u   %s %u -> %u",pools[3],
     (unsigned)a->artisans,(unsigned)b->artisans,pools[4],
     (unsigned)a->food_workers,(unsigned)b->food_workers);label(104,371,line,FG);
    snprintf(line,sizeof line,"GOLD %u FOOD %u RATE %u - ALL UNCHANGED FROM STOCK",
     (unsigned)b->gold,(unsigned)b->goods[10],(unsigned)cp->rate);label(104,412,line,FG);
    label(104,458,cv.passed?"CLASSIC PASS COMPLETE - NO STOCK SEASON GENERATED":
     cv.remaining?"A PASS: DOUBLE CONFIRM / B CANCEL / DPAD COUNTRIES":
     "THREE ORDERS SPENT; NEXT STOCK SEASON UNIMPLEMENTED",WARN);
    label(104,502,"MANUAL-DERIVED PORT ALLOCATION, NOT ORIGINAL NES COMMAND PARITY",WARN);
    label(104,565,"VIEW CYCLES ORIGINAL RAW / FINANCE / GOODS; SOURCE RECORD IMMUTABLE",FG);
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
   uint8_t effective=0u;
   Gk119Result count_ok=gk119_effective_remaining(cp,&app.enhanced_assign,
      app.rules_profile,&effective);
   snprintf(line,sizeof line,"%s ORDERS: %u REMAIN / %u COMPLETED / ENDED %s",
    count_ok!=GK119_OK?"SOURCE INVALID":
    app.enhanced_assign.active?"ENHANCED":"BASE",
    count_ok==GK119_OK?(unsigned)effective:0u,
    app.enhanced_assign.active?(unsigned)app.enhanced_assign.action_count:
    (unsigned)cp->orders.completed,
    count_ok==GK119_OK&&effective==0u?"YES":"NO");label(104,410,line,EDGE);
   label(104,457,count_ok!=GK119_OK?
    "SOURCE CHECK FAILED: NO ORDER EXECUTION PERMITTED":
    effective==0u?"PASS COMPLETE: NO ORDERS REMAIN. B RETURN / DPAD INSPECT":
    (cp->pending_pass?"CONFIRM PASS: A YES / B CANCEL":
    "DPAD BROWSE / A PASS REMAINING ORDERS (DOUBLE CONFIRM)"),FG);
   label(104,501,"START 14-COMMAND MENU; LB/RB SLOT X SAVE Y RESTORE",WARN);
   label(104,565,"VIEW RAW / FINANCE / DELTAS / GOODS / DECODED; SOURCE UNCHANGED",FG);
   }
   }
  }else label(104,285,"STOCK CHECKPOINT INVALID: NO SIMULATED PLAY",WARN);
 }else if(app.screen==GK1_SCREEN_SPRING_ARCHIVE){
  unsigned ix=app.spring_archive_cursor;
  const Gk1LoadedScenario *w=&app.worlds[app.spring_preview_mode?app.active_world:0];
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
  label(104,181,app.spring_preview_mode?"ORIGINAL 6502 ALL-COUNTRY FOOD/TAX/PAY (STAGE ONLY)":"SPRING 1175 ARCHIVE CROSSWALK - ROM IDENTITY UNVERIFIED",EDGE);
  label(104,216,app.spring_preview_mode?"WORLD-SCALE VERIFIED-TEMPLATE PROJECTION; NOT ACTUAL FIRST SPRING":"READ ONLY: PRE_SETUP DATA VS LATER ARCHIVED WORKRAM (NOT INITIAL SPRING)",WARN);
  if(app.spring_preview_mode){
   Gk110SpringStage batch={0};
   Gk110Result rs=gk1_app_source_spring_stage(&app,scenario_csv_path(),&batch);
   if(rs==GK110_OK&&ix<batch.country_count){
    const Gk1CountryTemplate *pre=&w->countries[ix];
    const Gk75SpringResult *v=&batch.country[ix];
    snprintf(line,sizeof line,"SOURCE FINANCE STAGE 0: COUNTRY %u / %u  %.25s",
       ix+1u,(unsigned)batch.country_count,pre->country_name);label(104,264,line,EDGE);
    snprintf(line,sizeof line,"FOOD: %u -> %u  (CONSUMED %u)",(unsigned)pre->goods[10],
       (unsigned)v->after.food,(unsigned)v->food_consumed);label(104,307,line,FG);
    snprintf(line,sizeof line,"TAX: %u  PAYROLL: %u  TROOPS: %u -> %u",
       (unsigned)v->gross_tax,(unsigned)v->payroll,
       (unsigned)pre->troops,(unsigned)v->after.troops);label(104,348,line,FG);
    snprintf(line,sizeof line,"GOLD: %u -> %u  / WORLD %u COUNTRIES TAX TOTAL %u",
       (unsigned)pre->gold,(unsigned)v->after.gold,
       (unsigned)batch.country_count,(unsigned)batch.tax_total);label(104,390,line,FG);
    label(104,433,"ALL-COUNTRY STAGE COMPUTED; NO SETUP, RNG, AGE OR GROWTH",WARN);
    label(104,474,"CANNOT COMMIT STAGE TO GAME OR GRANT 3 ORDERS",WARN);
   }else{
    snprintf(line,sizeof line,"SOURCE FINANCE STAGE REFUSED (%u): ORIGINAL TEMPLATE DRIFT",
       (unsigned)rs);label(104,310,line,WARN);
    label(104,355,"NO UNIVERSAL GOLD / RATE / SPRING CONSTANTS GENERATED",WARN);
   }
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
   Gk96Warmup warm={0};
   Gk96Result wr=gk96_original_reset_warmup(path,&warm);
   if(wr==GK96_NEEDS_NMI){
    uint8_t ppu_values[256];memset(ppu_values,0x80,sizeof ppu_values);
    Gk83Trace after_nmi={0};
    Gk83Stop nmi_gate=gk97_trace_reset_one_hypothetical_nmi(
       path,ppu_values,sizeof ppu_values,30000u,&after_nmi);
    if(after_nmi.hypothetical_nmi_injections==1u&&nmi_gate==GK83_HARDWARE_GATE){
     Gk83Trace pad_trace={0};
     Gk83Stop pad_gate=gk98_trace_reset_controller_hypothesis(
       path,ppu_values,sizeof ppu_values,0u,0u,0u,40000u,&pad_trace);
     if(pad_gate!=GK83_SOURCE_REFUSED&&pad_gate!=GK83_INVALID&&
        pad_trace.hypothetical_controller_reads>0u)
      snprintf(app.status,sizeof app.status,
        "STOCK HYP NMI@%llu CYC; PAD1 %u PAD2 %u; MMC1 IGN %u RESET %u; PPU UNMODELED",
        (unsigned long long)after_nmi.nmi_requested_after_completed_cycles,
        (unsigned)pad_trace.hypothetical_controller_reads,
        (unsigned)pad_trace.hypothetical_controller2_reads,
        (unsigned)pad_trace.mapper_consecutive_ignored,
        (unsigned)pad_trace.mapper_bit7_resets);
     else snprintf(app.status,sizeof app.status,
       "AUTHENTIC ROM: PPU WARMUP + HYP NMI $%04X -> HW $%04X; NO SPRING",
       (unsigned)after_nmi.hypothetical_nmi_vector,
       (unsigned)after_nmi.last_hardware_read);
    }
    else snprintf(app.status,sizeof app.status,
      "AUTHENTIC ROM: PPU WARMUP NMI WAIT $%04X; NO PLAYABLE SPRING",
      (unsigned)warm.immediate_vblank.stop_pc);
   }else snprintf(app.status,sizeof app.status,
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
static void replay_key(int save);
static void input(Gk1Input command){
 if(app.screen==GK1_SCREEN_STOCK_COMMANDS && command==GK1_INPUT_SETUP){
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
  Gk162EntryStatus r=gk162_enter_original_checkpoint(&app,
      scenario_csv_path(),stock_source_path,stock_spring.profile,player);
  if(r!=GK162_ENTRY_OK)snprintf(app.status,sizeof app.status,
      "STOCK CHECKPOINT REFUSED (%u): CSV / BANK / RECORDS NOT VALID",(unsigned)r);
  return;
 }
 /* A verified snapshot is immutable reference data, NEVER a battle engine.
  * Only permit leaving the viewer. No research movement or fake completion.
  */
 if(imported_ready && app.screen==GK1_SCREEN_RESEARCH_BATTLE &&
    command!=GK1_INPUT_CANCEL)return;
 (void)gk1_app_input(&app,command);
 /* A-confirmed port-only OTHER request: dispatch to the already hardened
  * source-attested persistence path. Consume the one-shot BEFORE I/O, so
  * controller repeat or a failed write cannot issue a second operation. */
 if(app.stock_storage_request){
  uint8_t request=app.stock_storage_request;
  app.stock_storage_request=0u;
  if(app.screen==GK1_SCREEN_STOCK_STORAGE &&
     app.rules_profile==0u && app.stock_checkpoint.source_verified)
    replay_key(request==1u);
 }
}
static int in_campaign_screens(void){
 return app.screen==GK1_SCREEN_CAMPAIGNS||app.screen==GK1_SCREEN_SCENARIO_COUNTRIES||
        app.screen==GK1_SCREEN_SETUP_WIZARD||app.screen==GK1_SCREEN_LABOR_PLAN||app.screen==GK1_SCREEN_SPRING_FINANCE_PREVIEW||app.screen==GK1_SCREEN_DEMOBILIZE||
        app.screen==GK1_SCREEN_MARKET_EXPLORER||app.screen==GK1_SCREEN_MANUAL_RULES||
        app.screen==GK1_SCREEN_ORDER_BUDGET_RESEARCH||app.screen==GK1_SCREEN_STOCK_CONTINUE||app.screen==GK1_SCREEN_STOCK_COMMANDS||app.screen==GK1_SCREEN_STOCK_TRADE_REFERENCE||app.screen==GK1_SCREEN_STOCK_STORAGE||app.screen==GK1_SCREEN_CLASSIC_ASSIGN||app.screen==GK1_SCREEN_CLASSIC_TAX||app.screen==GK1_SCREEN_ENHANCED_ASSIGN||app.screen==GK1_SCREEN_ENHANCED_SUMMER||app.screen==GK1_SCREEN_ENHANCED_FALL||app.screen==GK1_SCREEN_ENHANCED_WINTER||app.screen==GK1_SCREEN_ENHANCED_YEAR2||app.screen==GK1_SCREEN_ENHANCED_CONTINUATION;
}
static void slot_action(int direction){
 if(app.screen==GK1_SCREEN_SPRING_ARCHIVE){cycle_stock_profile(direction);return;}
 if(!in_campaign_screens()&&app.screen!=GK1_SCREEN_TITLE)return;
 campaign_slot=gk58_slot_cycle(campaign_slot,direction);
 private_title_preview_valid=0;
 snprintf(app.status,sizeof app.status,"SOURCE-BOUND 14/27 COUNTRY SNAPSHOT SLOT %u/3 - X CREATE / Y LOAD / NO OVERWRITE",(unsigned)campaign_slot);
}
static void enhanced_title_resume(void);
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
   else if(app.screen==GK1_SCREEN_TITLE)enhanced_title_resume();
   else replay_key(1);
   break;
  case GK1_PAD_ACT_Y:
   if(app.screen==GK1_SCREEN_CLASSIC_TAX || app.screen==GK1_SCREEN_CLASSIC_ASSIGN)
    input(GK1_INPUT_DRAFT_UNDO_TEN);
   else if(app.screen==GK1_SCREEN_SETUP_WIZARD && app.manual_bonus_active[app.active_world])
    input(GK1_INPUT_DRAFT_UNDO_TEN);
   else replay_key(0);
   break;
  case GK1_PAD_ACT_MENU:input(GK1_INPUT_SETUP);break;
  case GK1_PAD_ACT_VIEW:input(GK1_INPUT_LABOR_PLAN);break;
  case GK1_PAD_ACT_LB:
   if(app.screen==GK1_SCREEN_LABOR_PLAN || app.screen==GK1_SCREEN_ENHANCED_SPRING_LAB)
     input(GK1_INPUT_DRAFT_UNDO_ONE);
   else slot_action(-1);
   break;
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
 return gk120_linux_default(3,"gk1_presetup_slot1.gkp");
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
/* Same slot name and directory as controller X/Y private save/restore. */
static int private_stock_slot_path(char *out,size_t size){
 const char *base=getenv("GK1_PRIVATE_STOCK_SAVE_FILE");
#ifdef _WIN32
 if(!base||!*base)base=gk59_profile_default(3,"gk1_original_private_slot1.gk17");
#else
 if(!base||!*base)base=gk120_linux_default(5,"gk1_original_private_slot1.gk17");
#endif
 return gk58_slot_path(base,campaign_slot,out,size);
}
/* Controller X on TITLE restores the newest *existing* Enhanced research
 * receipt using the private original GK17 as its immutable source. Every
 * receipt is interpreted by the same attestation/replay module as in-game Y;
 * no keyboard, CLI arguments or fake first Spring is required for the resume. */
/* EXEC0177: X/Y on an active authenticated Enhanced continuation; X from
 * TITLE resumes this receipt first, refusing any corrupt highest-stage save.
 * Neither routine mutates original stock RAM or creates a Classic order. */
static int continuation_path(char *out,size_t cap){
 const char *base=getenv("GK1_ENHANCED_CONTINUATION_SAVE_FILE");
#ifdef _WIN32
 if(!base||!*base)base=gk59_profile_default(3,"gk1_enhanced_continuation_slot1.gk77");
#else
 if(!base||!*base)base=gk120_linux_default(4,"gk1_enhanced_continuation_slot1.gk77");
#endif
 return gk58_slot_path(base,campaign_slot,out,cap);
}
static void continuation_io(int save){
 char path[1100];
 if(!continuation_path(path,sizeof path)){
  strcpy(app.status,"ENHANCED CONTINUATION PATH INVALID: NO STATE CHANGED");return;
 }
 Gk177Stages *st=calloc(1,sizeof *st);
 if(!st){strcpy(app.status,"ENHANCED CONTINUATION: ALLOCATION FAILED");return;}
 st->spring=app.enhanced_assign;st->summer=app.enhanced_summer;
 st->fall=app.enhanced_fall;st->winter=app.enhanced_winter;
 st->year2=app.enhanced_year2;st->continuation=app.enhanced_continuation;
 Gk177Status r=save?gk177_save_new(path,&app.stock_checkpoint,st):
    gk177_cold_load(path,&app.stock_checkpoint,app.rules_profile,st);
 if(!save&&r==GK177_OK){
  app.enhanced_assign=st->spring;app.enhanced_summer=st->summer;
  app.enhanced_fall=st->fall;app.enhanced_winter=st->winter;
  app.enhanced_year2=st->year2;app.enhanced_continuation=st->continuation;
  app.screen=GK1_SCREEN_ENHANCED_CONTINUATION;
 }
 snprintf(app.status,sizeof app.status,"ENHANCED 20-SEASON %s: %s (NO NES SEASON PARITY)",
  save?"SAVE":"LOAD",r==GK177_OK?"SOURCE-REPLAY VERIFIED":
  r==GK177_EXISTS?"EXISTS - NOT OVERWRITTEN":"REFUSED - UNCHANGED");
 memset(st,0,sizeof *st);free(st);
}
static void enhanced_title_resume(void){
 if(app.screen!=GK1_SCREEN_TITLE||!scenario_loaded)return;
 char stock_path[1100],paths[GK171_STAGES][1100];
 if(!private_stock_slot_path(stock_path,sizeof stock_path)){
  strcpy(app.status,"ENHANCED RESUME REFUSED: ORIGINAL PRIVATE SLOT PATH INVALID");return;
 }
 static const char *const names[GK171_STAGES]={
  "gk1_enhanced_assign_slot1.gk16",
  "gk1_enhanced_summer_slot1.gk18",
  "gk1_enhanced_fall_slot1.gk19",
  "gk1_enhanced_winter_slot1.gk20",
  "gk1_enhanced_year2_slot1.gk21"
 };
 static const char *const envs[GK171_STAGES]={
  "GK1_ENHANCED_ASSIGN_SAVE_FILE","GK1_ENHANCED_SUMMER_SAVE_FILE",
  "GK1_ENHANCED_FALL_SAVE_FILE","GK1_ENHANCED_WINTER_SAVE_FILE",
  "GK1_ENHANCED_YEAR2_SAVE_FILE"
 };
 /* A GC77 continuation is newer than five historical stage receipts.
  * Its presence dominates; corruption never falls back to an older stage. */
 char continuation_file[1100];
 if(!continuation_path(continuation_file,sizeof continuation_file)){
  strcpy(app.status,"CONTINUATION RESUME PATH INVALID: NO STATE CHANGED");return;
 }
#ifdef _WIN32
 struct _stat64 cont_stat;
 int has_cont=(_stat64(continuation_file,&cont_stat)==0);
#else
 struct stat cont_stat;
 int has_cont=(lstat(continuation_file,&cont_stat)==0);
#endif
#ifndef _WIN32
 if(!has_cont&&errno!=ENOENT){
  strcpy(app.status,"CONTINUATION SAVE INACCESSIBLE: NO FALLBACK OR STATE CHANGE");return;
 }
#endif
 if(has_cont){
  Gk1App *candidate=malloc(sizeof *candidate);
  Gk177Stages *stage=calloc(1,sizeof *stage);
  if(!candidate||!stage){free(candidate);free(stage);
   strcpy(app.status,"CONTINUATION RESUME ALLOCATION REFUSED");return;}
  *candidate=app;
  Gk177Status r=GK177_SOURCE;
  if(gk167_cold_load_private(stock_path,scenario_csv_path(),candidate)==GK167_OK){
   candidate->rules_profile=1u;
   r=gk177_cold_load(continuation_file,&candidate->stock_checkpoint,1u,stage);
  }
  if(r==GK177_OK){
   candidate->enhanced_assign=stage->spring;
   candidate->enhanced_summer=stage->summer;
   candidate->enhanced_fall=stage->fall;
   candidate->enhanced_winter=stage->winter;
   candidate->enhanced_year2=stage->year2;
   candidate->enhanced_continuation=stage->continuation;
   candidate->screen=GK1_SCREEN_ENHANCED_CONTINUATION;
   strcpy(candidate->status,"ENHANCED 20-SEASON COLD RESUMED: NOT ORIGINAL NES PARITY");
   app=*candidate;
  }else strcpy(app.status,"CONTINUATION SAVE REFUSED: SOURCE/RECEIPT INVALID - UNCHANGED");
  memset(stage,0,sizeof *stage);free(stage);
  memset(candidate,0,sizeof *candidate);free(candidate);return;
 }
 Gk171Paths p={0};p.source_gk17=stock_path;
 p.canonical_csv=scenario_csv_path();
 for(unsigned i=0;i<GK171_STAGES;i++){
  const char *base=getenv(envs[i]);
#ifdef _WIN32
  if(!base||!*base)base=gk59_profile_default(3,names[i]);
#else
  if(!base||!*base)base=gk120_linux_default(4,names[i]);
#endif
  if(!gk58_slot_path(base,campaign_slot,paths[i],sizeof paths[i])){
   strcpy(app.status,"ENHANCED RESUME REFUSED: RECEIPT PATH INVALID");return;
  }
  p.receipt[i]=paths[i];
 }
 unsigned stage=0;
 Gk171Result r=gk171_resume_latest(&p,&app,&stage);
 if(r==GK171_OK){private_title_preview_valid=0;return;}
 strcpy(app.status,r==GK171_NO_RECEIPT?
   "NO ENHANCED SAVE ON THIS SLOT: X SOURCE-ATTESTED RESTORE NOT STARTED":
   r==GK171_SOURCE_REFUSED?
   "ENHANCED RESTORE REFUSED: ORIGINAL PRIVATE SOURCE INVALID":
   "ENHANCED RESTORE REFUSED: RECEIPT/SOURCE INVALID; STATE UNCHANGED");
}
static void refresh_private_title_preview(void){
 char path[1100];
 memset(&private_title_preview,0,sizeof private_title_preview);
 private_title_preview.status=GK170_SLOT_REFUSED;
 if(scenario_loaded&&private_stock_slot_path(path,sizeof path))
   (void)gk170_preview_private_slot(path,scenario_csv_path(),&app,
       &private_title_preview);
 practice_title_status=GK227_PREVIEW_MISSING;
 if(app.rules_profile==1u){
  if(!scenario_loaded||!practice_slot_path(path,sizeof path))
   practice_title_status=GK227_PREVIEW_UNAVAILABLE;
  else practice_title_status=gk227_preview_practice_slot(path,
            scenario_csv_path(),app.worlds,&practice_title_preview);
 }
 private_preview_profile=app.rules_profile;
 private_title_preview_valid=1;
}
/* GC19 stores the original RAM only inside the owner's PRIVATE save home.
 * GC18 remains readable for preexisting source-reattach receipt slots. */
static int classic_cold_path(char *out,size_t cap){
 const char *base=getenv("GK1_CLASSIC_COLD_SAVE_FILE");
#ifdef _WIN32
 if(!base||!*base)base=gk59_profile_default(3,"gk1_classic_assign_slot1.gc19");
#else
 if(!base||!*base)base=gk120_linux_default(4,"gk1_classic_assign_slot1.gc19");
#endif
 return base&&*base&&gk58_slot_path(base,campaign_slot,out,cap);
}
static int classic_cold_load_title(void){
 if(!classic_cold_path(selected_slot_path,sizeof selected_slot_path)){
  strcpy(app.status,"PRIVATE CLASSIC COLD LOAD REFUSED: UNSAFE PATH");return 1;
 }
 Gk189Result r=gk189_cold_load(selected_slot_path,scenario_csv_path(),&app);
 if(r==GK189_MISSING)return 0; /* Permit older stock snapshot saves. */
 private_title_preview_valid=0;
 snprintf(app.status,sizeof app.status,"PRIVATE CLASSIC COLD RESUME: %s (SOURCE SNAPSHOT; NOT NES PARITY)",
    r==GK189_OK?"SHA-VERIFIED ACTIONS RESTORED":
    r==GK189_SOURCE?"SOURCE REFUSED - UNCHANGED":
    r==GK189_CORRUPT?"CORRUPT - UNCHANGED":"UNAVAILABLE - UNCHANGED");
 return 1;
}
static void classic_receipt_io(int save){
 /* Authenticated private source is already attached. A .gc18 receipt never
  * stores licensed RAM; Y requires that same source to be reattached. */
 if(app.rules_profile!=0u||!app.classic_assign.active||
    !gk187_intact(&app.classic_assign,&app.stock_checkpoint)){
  strcpy(app.status,"CLASSIC RECEIPT REFUSED: ORIGINAL SOURCE/REPLAY INVALID");return;
 }
 if(!classic_cold_path(selected_slot_path,sizeof selected_slot_path)){
  strcpy(app.status,"CLASSIC PRIVATE COLD SAVE REFUSED: UNSAFE PATH");return;
 }
 Gk189Result full=save?gk189_save_new(selected_slot_path,&app):
    gk189_cold_load(selected_slot_path,scenario_csv_path(),&app);
 if(save||full!=GK189_MISSING){
  snprintf(app.status,sizeof app.status,"CLASSIC PRIVATE %s: %s (SOURCE + ASSIGN; NOT NES PARITY)",
   save?"SAVE":"COLD LOAD",full==GK189_OK?"SHA-VERIFIED OK":
   full==GK189_EXISTS?"SLOT EXISTS - NO OVERWRITE":
   full==GK189_SOURCE?"SOURCE MISMATCH - UNCHANGED":
   full==GK189_CORRUPT?"CORRUPT - UNCHANGED":"UNAVAILABLE - UNCHANGED");
  return;
 }
 /* Backward compatibility: old GC18 receipt requires the source to be
  * attached in memory, and does not independently cold-load from title. */
 const char *base=getenv("GK1_CLASSIC_ASSIGN_SAVE_FILE");
#ifdef _WIN32
 if(!base||!*base)base=gk59_profile_default(3,"gk1_classic_assign_slot1.gc18");
#else
 if(!base||!*base)base=gk120_linux_default(4,"gk1_classic_assign_slot1.gc18");
#endif
 if(!base||!*base||!gk58_slot_path(base,campaign_slot,selected_slot_path,sizeof selected_slot_path)){
  strcpy(app.status,"CLASSIC RECEIPT REFUSED: UNSAFE SLOT PATH");return;
 }
 Gk188Result r=save?gk188_save_new(selected_slot_path,&app.stock_checkpoint,&app.classic_assign):
      gk188_load(selected_slot_path,&app.stock_checkpoint,&app.classic_assign);
 snprintf(app.status,sizeof app.status,"CLASSIC ASSIGN %s: %s; REATTACH ORIGINAL FOR COLD LOAD",
  save?"SAVE":"LOAD",r==GK188_OK?"SHA-VERIFIED OK":r==GK188_EXISTS?"SLOT EXISTS - NO OVERWRITE":
  r==GK188_SOURCE?"SOURCE MISMATCH - UNCHANGED":r==GK188_CORRUPT?"CORRUPT - UNCHANGED":
  "UNAVAILABLE - UNCHANGED");
}
/* EXEC0224: X/Y are working, source-locked ENHANCED PRACTICE receipts.
 * This is deliberately disjoint from authenticated Classic private state and
 * all existing research and campaign slot families. */
static int practice_slot_path(char *out,size_t cap){
 const char *base=getenv("GK1_ENHANCED_PRACTICE_SAVE_FILE");
 if(!base||!*base){
#ifdef _WIN32
  base=gk59_profile_default(3,"gk1_enhanced_practice_slot1.gk24");
#else
  base=gk120_linux_default(5,"gk1_enhanced_practice_slot1.gk24");
#endif
 }
 return base&&*base&&gk58_slot_path(base,campaign_slot,out,cap);
}
/* Y on TITLE only chooses Enhanced practice when this exact separate slot
 * exists. Missing practice preserves the prior Classic/stock Y behavior.
 * Corruption never silently loads a different save family. */
static int practice_resume_title(void){
 if(app.screen!=GK1_SCREEN_TITLE||app.rules_profile!=1u)return 0;
 if(!practice_slot_path(selected_slot_path,sizeof selected_slot_path)){
  strcpy(app.status,"ENHANCED PRACTICE TITLE PATH INVALID - NO STATE CHANGED");return 1;
 }
 FILE *probe=fopen(selected_slot_path,"rb");
 if(!probe){
  if(errno==ENOENT)return 0;
  strcpy(app.status,"ENHANCED PRACTICE SLOT INACCESSIBLE - NO OTHER RESUME");return 1;
 }
 fclose(probe);
 Gk225Status r=gk225_resume_title(&app,scenario_csv_path(),selected_slot_path);
 if(r==GK225_OK)private_title_preview_valid=0;
 snprintf(app.status,sizeof app.status,"ENHANCED PRACTICE TITLE COLD LOAD: %s (NOT NES)",
   r==GK225_OK?"SOURCE-REPLAY VERIFIED":r==GK225_SOURCE?"SOURCE MISMATCH - UNCHANGED":
   r==GK225_CORRUPT?"CORRUPT - UNCHANGED":r==GK225_MISSING?"MISSING - UNCHANGED":
   "WRONG PROFILE/PHASE - UNCHANGED");
 return 1;
}
static void practice_io(int save){
 if(app.rules_profile!=1u || app.screen!=GK1_SCREEN_ENHANCED_SPRING_LAB ||
    !app.port_spring.active || app.active_world>1u ||
    !app.world_ready[app.active_world]){
  strcpy(app.status,"PORT PRACTICE SAVE REFUSED: WRONG PROFILE/PHASE");return;
 }
 if(!practice_slot_path(selected_slot_path,sizeof selected_slot_path)){
  strcpy(app.status,"PORT PRACTICE SAVE REFUSED: NO SECURE SLOT PATH");return;
 }
 Gk224Status r;
 if(save){
  r=gk226_practice_checkpoint(selected_slot_path,scenario_csv_path(),
       &app.worlds[app.active_world],&app.port_spring,app.port_spring_command);
 }else{
  Gk222Spring loaded={0};uint8_t command=0;
  r=gk224_load(selected_slot_path,scenario_csv_path(),
       &app.worlds[app.active_world],&loaded,&command);
  if(r==GK224_OK){app.port_spring=loaded;app.port_spring_command=command;}
 }
 if(r==GK224_OK)private_title_preview_valid=0;
 snprintf(app.status,sizeof app.status,"ENHANCED PRACTICE %s: %s; NOT ORIGINAL NES",
  save?"SAVE":"LOAD",r==GK224_OK?"SOURCE-REPLAY VERIFIED":
  r==GK224_EXISTS?"SLOT EXISTS (NOT OVERWRITTEN)":
  r==GK224_SOURCE?"CSV OR WORKFORCE SOURCE MISMATCH - UNCHANGED":
  r==GK224_CORRUPT?"FILE CORRUPT - UNCHANGED":
  r==GK224_STATE?"STATE TAMPERED - UNCHANGED":"UNAVAILABLE - UNCHANGED");
}
static void replay_key(int save){
 if(app.screen==GK1_SCREEN_ENHANCED_SPRING_LAB){practice_io(save);return;}
 if(app.screen==GK1_SCREEN_TITLE&&!save && practice_resume_title())return;
 if(app.screen==GK1_SCREEN_TITLE&&!save && classic_cold_load_title())return;
 if(app.classic_assign.active && app.rules_profile==0u){
  if(app.screen==GK1_SCREEN_STOCK_CONTINUE||app.screen==GK1_SCREEN_STOCK_COMMANDS||
     app.screen==GK1_SCREEN_STOCK_TRADE_REFERENCE||app.screen==GK1_SCREEN_CLASSIC_ASSIGN||
     app.screen==GK1_SCREEN_CLASSIC_TAX||app.screen==GK1_SCREEN_STOCK_STORAGE){
   classic_receipt_io(save);return;
  }
  strcpy(app.status,"CLASSIC SAVE REFUSED OUTSIDE ORIGINAL STRATEGIC SCREEN");return;
 }

 /* EXEC0185: the last GK80 fallback was reachable from STOCK_COMMANDS
  * in Enhanced profile without a live Enhanced stage, and from forged
  * source state.  It serialized only Classic source, not the profile.
  * Refuse before choosing a path/opening a file.  Active Enhanced stages
  * keep their exclusive, existing save handlers below. */
 const Gk185HotkeyRoute stock_route=gk185_stock_hotkey_route(&app);
 if(stock_route==GK185_REFUSE){
  strcpy(app.status,"STOCK X/Y REFUSED: VERIFY ORIGINAL SOURCE AND PROFILE; NO FILE TOUCHED");
  return;
 }
 if(app.enhanced_continuation.active){continuation_io(save);return;}
 /* Self-contained original-stock save: X creates a user-private local file;
  * Y from the TITLE works after a true process restart without reattaching
  * the ROM or original snapshot, provided canonical country CSV is installed.
  * This is a limited Spring checkpoint, NOT a full campaign save. */
 const int stock_ui=app.screen==GK1_SCREEN_STOCK_CONTINUE ||
   app.screen==GK1_SCREEN_STOCK_COMMANDS ||
   app.screen==GK1_SCREEN_STOCK_TRADE_REFERENCE ||
   app.screen==GK1_SCREEN_STOCK_STORAGE;
 const int stock_cold_title=app.screen==GK1_SCREEN_TITLE&&!save;
 if(stock_cold_title || stock_route==GK185_CLASSIC_PRIVATE){
  private_title_preview_valid=0;
  if(!private_stock_slot_path(selected_slot_path,sizeof selected_slot_path)){
   strcpy(app.status,"PRIVATE STOCK SAVE PATH INVALID - NO FILE TOUCHED");return;
  }
  Gk167Result r=save?gk167_save_private_new(selected_slot_path,&app):
        gk167_cold_load_private(selected_slot_path,scenario_csv_path(),&app);
  snprintf(app.status,sizeof app.status,
    "PRIVATE ORIGINAL SPRING %s: %s (NOT FULL CAMPAIGN)",
     save?"SAVE":"COLD RESUME",
     r==GK167_OK?"SHA-VERIFIED OK":r==GK167_EXISTS?"SLOT EXISTS - NO OVERWRITE":
     r==GK167_SOURCE?"SOURCE MISMATCH - UNCHANGED":
     r==GK167_CORRUPT?"CORRUPT - UNCHANGED":"UNAVAILABLE - UNCHANGED");
  return;
 }
 if(app.enhanced_year2.active==1u){
  const char *base=getenv("GK1_ENHANCED_YEAR2_SAVE_FILE");
#ifdef _WIN32
  if(!base||!*base)base=gk59_profile_default(3,"gk1_enhanced_year2_slot1.gk21");
#else
  if(!base||!*base)base=gk120_linux_default(4,"gk1_enhanced_year2_slot1.gk21");
#endif
  if(!gk58_slot_path(base,campaign_slot,selected_slot_path,sizeof selected_slot_path)){
   strcpy(app.status,"YEAR2 EXCLUSIVE SAVE PATH INVALID");return;
  }
  Gk161Result r=save?gk161_save_new(selected_slot_path,&app.stock_checkpoint,
    &app.enhanced_assign,&app.enhanced_summer,&app.enhanced_fall,
    &app.enhanced_winter,&app.enhanced_year2):
    gk161_cold_load(selected_slot_path,&app.stock_checkpoint,app.rules_profile,
    &app.enhanced_assign,&app.enhanced_summer,&app.enhanced_fall,
    &app.enhanced_winter,&app.enhanced_year2);
  snprintf(app.status,sizeof app.status,"ENHANCED YEAR2 %s: %s (NES SOURCE IMMUTABLE)",
   save?"SAVE":"COLD LOAD",r==GK161_OK?"OK":r==GK161_EXISTS?"EXISTS":"REFUSED");
  return;
 }

 if(app.enhanced_winter.active==1u){
  const char *base=getenv("GK1_ENHANCED_WINTER_SAVE_FILE");
#ifdef _WIN32
  if(!base||!*base)base=gk59_profile_default(3,"gk1_enhanced_winter_slot1.gk20");
#else
  if(!base||!*base)base=gk120_linux_default(4,"gk1_enhanced_winter_slot1.gk20");
#endif
  if(!gk58_slot_path(base,campaign_slot,selected_slot_path,sizeof selected_slot_path)){
   strcpy(app.status,"ENHANCED WINTER SAVE PATH INVALID");return;
  }
  Gk160Result r=save?gk160_save_new(selected_slot_path,&app.stock_checkpoint,
    &app.enhanced_assign,&app.enhanced_summer,&app.enhanced_fall,&app.enhanced_winter):
    gk160_cold_load(selected_slot_path,&app.stock_checkpoint,app.rules_profile,
    &app.enhanced_assign,&app.enhanced_summer,&app.enhanced_fall,&app.enhanced_winter);
  snprintf(app.status,sizeof app.status,"ENHANCED WINTER %s: %s (ORIGINAL STOCK IMMUTABLE)",
   save?"SAVE":"COLD LOAD",r==GK160_OK?"OK":r==GK160_EXISTS?"EXISTS":"REFUSED");
  return;
 }
 if(app.enhanced_fall.active==1u){
  const char *base=getenv("GK1_ENHANCED_FALL_SAVE_FILE");
#ifdef _WIN32
  if(!base||!*base)base=gk59_profile_default(3,"gk1_enhanced_fall_slot1.gk19");
#else
  if(!base||!*base)base=gk120_linux_default(4,"gk1_enhanced_fall_slot1.gk19");
#endif
  if(!gk58_slot_path(base,campaign_slot,selected_slot_path,sizeof selected_slot_path)){
   strcpy(app.status,"ENHANCED FALL SAVE PATH INVALID");return;
  }
  Gk159Result r=save?gk159_save_new(selected_slot_path,&app.stock_checkpoint,
    &app.enhanced_assign,&app.enhanced_summer,&app.enhanced_fall):
    gk159_cold_load(selected_slot_path,&app.stock_checkpoint,app.rules_profile,
    &app.enhanced_assign,&app.enhanced_summer,&app.enhanced_fall);
  snprintf(app.status,sizeof app.status,"ENHANCED FALL %s: %s (ORIGINAL STOCK IMMUTABLE)",
   save?"SAVE":"COLD LOAD",r==GK159_OK?"OK":r==GK159_EXISTS?"EXISTS":"REFUSED");
  return;
 }
 if(app.enhanced_summer.active==1u){
  const char *base=getenv("GK1_ENHANCED_SUMMER_SAVE_FILE");
#ifdef _WIN32
  if(!base||!*base)base=gk59_profile_default(3,"gk1_enhanced_summer_slot1.gk18");
#else
  if(!base||!*base)base=gk120_linux_default(4,"gk1_enhanced_summer_slot1.gk18");
#endif
  if(!gk58_slot_path(base,campaign_slot,selected_slot_path,sizeof selected_slot_path)){
   strcpy(app.status,"ENHANCED SUMMER SAVE PATH INVALID");return;
  }
  Gk158Result r=save?gk158_save_new(selected_slot_path,&app.stock_checkpoint,
    &app.enhanced_assign,&app.enhanced_summer):
    gk158_cold_load(selected_slot_path,&app.stock_checkpoint,app.rules_profile,
    &app.enhanced_assign,&app.enhanced_summer);
  snprintf(app.status,sizeof app.status,"ENHANCED SUMMER %s: %s (ORIGINAL STOCK IMMUTABLE)",
   save?"SAVE":"COLD LOAD",r==GK158_OK?"OK":r==GK158_EXISTS?"EXISTS":"REFUSED");
  return;
 }

 /* GA16 must own ALL Enhanced-session saves, even after navigation back to
  * the stock country, command or price screen. Otherwise a GK80 stock-only
  * save silently discards assignments and Pass (a real durability defect). */
 if(gk119_receipt_screen(app.screen,&app.enhanced_assign)){
  const char *base=getenv("GK1_ENHANCED_ASSIGN_SAVE_FILE");
#ifdef _WIN32
  if(!base||!*base)base=gk59_profile_default(3,"gk1_enhanced_assign_slot1.gk16");
#else
  if(!base||!*base)base=gk120_linux_default(4,"gk1_enhanced_assign_slot1.gk16");
#endif
  if(!gk58_slot_path(base,campaign_slot,selected_slot_path,sizeof selected_slot_path)){
   strcpy(app.status,"ENHANCED ASSIGN SLOT PATH INVALID: NO FILE TOUCHED");return;
  }
  Gk116Result r=save?gk116_save_new(selected_slot_path,&app.stock_checkpoint,&app.enhanced_assign):
       gk116_cold_load(selected_slot_path,&app.stock_checkpoint,app.rules_profile,&app.enhanced_assign);
  snprintf(app.status,sizeof app.status,"ENHANCED LABOR %s %s: ORIGINAL NES SOURCE STAYS IMMUTABLE",
    save?"SAVE":"COLD LOAD",r==GK116_OK?"OK":r==GK116_EXISTS?"EXISTS":"REFUSED");
  return;
 }

 if(stock_ui){
  /* Source-only GK80 is intentionally NOT a controller save route. Its
   * standalone serializer remains linked for historical research tests. */
  strcpy(app.status,"STOCK SNAPSHOT REFUSED: SELECT PRIVATE PROFILE SLOT");
  return;
 }
 if(app.screen==GK1_SCREEN_BATTLE_PASS_DRILL || app.screen==GK1_SCREEN_SPRING_ARCHIVE){
   strcpy(app.status,"READ-ONLY SPRING / MANUAL DRILL: SAVE/LOAD DISABLED");return;
 }
 if(app.screen==GK1_SCREEN_CAMPAIGNS||app.screen==GK1_SCREEN_SCENARIO_COUNTRIES||app.screen==GK1_SCREEN_SETUP_WIZARD||app.screen==GK1_SCREEN_LABOR_PLAN||app.screen==GK1_SCREEN_SPRING_FINANCE_PREVIEW||app.screen==GK1_SCREEN_DEMOBILIZE){
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

/* EXEC0245: presentation is shell-local; the shared import decision is
 * platform-neutral, and refusal leaves all original country state intact. */
static void gk245_report_owner_status(Gk245Outcome outcome){
 switch(outcome){
  case GK245_CONFLICT:
   snprintf(app.status,sizeof app.status,
      "OWNER IMPORT REFUSED: FRESH AND ARCHIVED WORLD SOURCES CONFLICT");break;
  case GK245_ARCHIVED_REFUSED:
   snprintf(app.status,sizeof app.status,"%s",gk241_archived_refusal_message());break;
  case GK245_FRESH_REFUSED:
   snprintf(app.status,sizeof app.status,
      "OWNER FRESH SOURCE REFUSED; PRIVATE ORIGINAL WORKRAM EVIDENCE REQUIRED");break;
  case GK245_ARCHIVED_IMPORTED:
   stock_selected=GK77_ARCHIVED_WORLD_SAVERAM;break;
  case GK245_FRESH_IMPORTED:
   stock_selected=GK77_FRESH_MONGOL_WORKRAM;break;
  default:break;
 }
}

#ifdef _WIN32
/* EXEC0231: opt-in per-stage Windows GUI diagnosis; never reads gameplay bytes. */
static int gk231_diagnostic_active=0;
static void gk231_trace(const char *stage) {
 const char *path=getenv("GK1_SMOKE_TRACE");
 if(!gk231_diagnostic_active||!path||!*path)return;
 FILE *f=fopen(path,"ab");
 if(f) {
  fprintf(f,"%llu\t%s\n",(unsigned long long)GetTickCount64(),stage);
  fclose(f);
 }
}

static unsigned gk230_painted_frames=0u;
static LRESULT CALLBACK window_proc(HWND h,UINT msg,WPARAM w,LPARAM l){
 (void)l;
 if(msg==WM_DESTROY){running=0;PostQuitMessage(0);return 0;}
 /* No keyboard gameplay or secret keyboard save paths: controller only. */
 if(msg==WM_PAINT){gk231_trace("WM_PAINT entered");PAINTSTRUCT p;screen_dc=BeginPaint(h,&p);
  gk231_trace("BeginPaint returned");render();gk231_trace("render returned");EndPaint(h,&p);
  ++gk230_painted_frames;gk231_trace("WM_PAINT completed");return 0;}
 return DefWindowProcA(h,msg,w,l);
}
typedef DWORD(WINAPI *XinputGetStateFn)(DWORD,XINPUT_STATE*);
/* XInput transport remains Win32-only and must be physically verified. The
 * raw decoder/slot policy is shared with the compiled Linux hostile QA.
 * Preserve the active controller across multi-pad hotplug to avoid control
 * hijacking; gk1_pad_disconnect disarms held A/Start when it changes. */
static void poll_xinput(void){
 static HMODULE dll=NULL;static XinputGetStateFn get_state=NULL;
 static uint8_t active_slot=0xffu;
 if(!dll){dll=LoadLibraryA("xinput1_4.dll");if(!dll)dll=LoadLibraryA("xinput9_1_0.dll");
  if(dll)get_state=(XinputGetStateFn)(void*)GetProcAddress(dll,"XInputGetState");}
 if(!get_state){gk1_pad_disconnect(&pad);active_slot=0xffu;return;}
 XINPUT_STATE states[4];memset(states,0,sizeof states);
 uint8_t available=0u;
 for(unsigned i=0;i<4u;i++)
  if(get_state((DWORD)i,&states[i])==ERROR_SUCCESS)
   available=(uint8_t)(available|(uint8_t)(1u<<i));
 uint8_t selected=0xffu;
 if(!gk99_xinput_slot(available,active_slot,&selected) || selected==0xffu){
  gk1_pad_disconnect(&pad);active_slot=0xffu;return;
 }
 if(selected!=active_slot){gk1_pad_disconnect(&pad);active_slot=selected;}
 Gk99XinputRaw raw={states[selected].Gamepad.wButtons,
    states[selected].Gamepad.sThumbLX,states[selected].Gamepad.sThumbLY};
 Gk1PadSample sample;
 if(!gk99_xinput_sample(&raw,&sample)){
  gk1_pad_disconnect(&pad);active_slot=0xffu;return;
 }
 pad_actions(&sample);
}
int WINAPI WinMain(HINSTANCE instance,HINSTANCE prev,LPSTR cmd,int show){
 (void)prev;
 unsigned smoke_frames=0u;
 /* EXEC0232 scripted raw XInput values, acceptance-only; never in normal gameplay. */
 const int gk232_scripted_gamepad=strcmp(cmd,"--smoke-gamepad-nav")==0;
 /* EXEC0233: private, Enhanced-only, multi-process Windows disk test. */
 const int gk233_practice_mode=
   strcmp(cmd,"--smoke-gamepad-practice-save")==0?1:
   strcmp(cmd,"--smoke-gamepad-practice-resume")==0?2:
   strcmp(cmd,"--smoke-gamepad-practice-verify")==0?3:
   strcmp(cmd,"--smoke-gamepad-practice-corrupt")==0?4:0;
 if(!gk232_scripted_gamepad&&!gk233_practice_mode&&
    strncmp(cmd,"--smoke-gamepad",15)==0)return 4;
 const char *gk233_bounded=gk232_scripted_gamepad?"--smoke-frames=10":
   gk233_practice_mode==1?"--smoke-frames=26":
   gk233_practice_mode==2?"--smoke-frames=8":
   gk233_practice_mode?"--smoke-frames=4":cmd;
 const int smoke_mode=gk230_win32_smoke_frames(gk233_bounded,&smoke_frames);
 if(smoke_mode<0)return 4;
 gk231_diagnostic_active=smoke_mode>0;
 gk231_trace("WinMain arguments accepted");
 gk1_app_init(&app);gk1_pad_init(&pad);load_optional_battle_view();load_optional_mesen_map();load_optional_spring_archive();load_optional_stock_spring();(void)load_scenario_data();
 /* EXEC0245: one fail-closed owner-source startup path on both Linux and
  * Windows. A diagnostic run NEVER accesses owner-private original bytes.
  * No independent original NES first-Spring RNG is implied by imports. */
 Gk245Outcome owner_result=gk245_owner_source_startup(&app,scenario_csv_path(),
   scenario_loaded,smoke_mode!=0,getenv("GK1_OWNER_STOCK_FRESH_JSON"),
   getenv("GK1_OWNER_STOCK_WORLD_MSS"));
 gk245_report_owner_status(owner_result);
 gk231_trace("app and scenario initialized");
 static Gk1LoadedScenario gk232_original_worlds[2];
 if(gk232_scripted_gamepad||gk233_practice_mode){
  if(!scenario_loaded||app.screen!=GK1_SCREEN_TITLE||
     !app.world_ready[0]||!app.world_ready[1]||
     app.worlds[0].country_count!=14u||app.worlds[1].country_count!=27u||
     app.worlds[0].phase!=GK1_PRE_SETUP_TEMPLATE||
     app.worlds[1].phase!=GK1_PRE_SETUP_TEMPLATE)return 7;
  memcpy(gk232_original_worlds,app.worlds,sizeof gk232_original_worlds);
 }

 WNDCLASSA cl={0};cl.lpfnWndProc=window_proc;cl.hInstance=instance;
 cl.lpszClassName="GK1_NATIVE_RESEARCH";cl.hCursor=LoadCursor(NULL,IDC_ARROW);
 cl.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
 if(!RegisterClassA(&cl))return 2;
 RECT r={0,0,WIDTH,HEIGHT};
 DWORD style=smoke_mode?WS_POPUP:WS_OVERLAPPEDWINDOW;
 if(!smoke_mode)AdjustWindowRect(&r,WS_OVERLAPPEDWINDOW,FALSE);
 hwnd=CreateWindowA(cl.lpszClassName,"Genghis Khan I - Native Port " GK1_PORT_VERSION,style,
 smoke_mode?0:CW_USEDEFAULT,smoke_mode?0:CW_USEDEFAULT,
 r.right-r.left,r.bottom-r.top,NULL,NULL,instance,NULL);
 if(!hwnd)return 3;
 font=CreateFontA(22,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_DEFAULT_PRECIS,
 CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FIXED_PITCH,"Consolas");
 gk231_trace("before ShowWindow");
 ShowWindow(hwnd,smoke_mode?SW_SHOWNORMAL:show);
 gk231_trace("after ShowWindow, before UpdateWindow");
 UpdateWindow(hwnd);
 gk231_trace("after UpdateWindow");
 if(smoke_mode){
  RECT client={0};
  if(!IsWindow(hwnd)||!GetClientRect(hwnd,&client)||
     client.right-client.left!=WIDTH||client.bottom-client.top!=HEIGHT){
   DestroyWindow(hwnd);if(font)DeleteObject(font);return 5;
  }
  /* Deterministic actual WM_PAINT frames; no controller impersonation. */
  for(unsigned frame=0;frame<smoke_frames&&running;frame++){
   MSG m;
   while(PeekMessageA(&m,NULL,0,0,PM_REMOVE)){
    if(m.message==WM_QUIT){running=0;break;}
    TranslateMessage(&m);DispatchMessageA(&m);
   }
   if(!running)break;
   if(gk232_scripted_gamepad){
    /* 0 seed neutral, A, release, right, release, A, release, B,
     * release, A. Actual WinMain -> decoder -> router -> app dispatch. */
    static const uint16_t buttons[10]={0u,0x1000u,0u,0x0008u,0u,
                                       0x1000u,0u,0x2000u,0u,0x1000u};
    Gk99XinputRaw raw={buttons[frame],0,0};
    Gk1PadSample decoded={0};
    if(!gk99_xinput_sample(&raw,&decoded)){
     if(IsWindow(hwnd))DestroyWindow(hwnd);
     if(font)DeleteObject(font);return 8;
    }
    pad_actions(&decoded);
    const int stage_ok=
      (frame==1u?app.screen==GK1_SCREEN_CAMPAIGNS:
       frame==3u?app.screen==GK1_SCREEN_CAMPAIGNS&&app.rules_profile==0u:
       frame==5u?app.screen==GK1_SCREEN_SCENARIO_COUNTRIES&&app.active_world==0u:
       frame==7u?app.screen==GK1_SCREEN_CAMPAIGNS:
       frame==9u?app.screen==GK1_SCREEN_SCENARIO_COUNTRIES:1);
    if(!stage_ok){
     gk231_trace("EXEC0232 scripted navigation refused");
     if(IsWindow(hwnd))DestroyWindow(hwnd);
     if(font)DeleteObject(font);return 8;
    }
   }else if(gk233_practice_mode){
    /* Only test mode supplies simulated raw gamepad; ordinary play still
     * calls poll_xinput() and respects real XInput hotplug/active slot. */
    uint16_t bits=0u;
    if(gk233_practice_mode==1){
     if(frame==1u||frame==3u||frame==5u||frame==9u||
        frame==11u||frame==13u||frame==15u||frame==17u||frame==19u)
       bits=0x1000u; /* XInput A */
     else if(frame==7u)bits=0x0010u; /* XInput START */
     else if(frame==21u)bits=0x4000u; /* XInput X: save */
     else if(frame==23u)bits=0x2000u; /* XInput B: exit */
    }else{
     if(frame==1u)bits=0x8000u; /* Y on title: cold resume */
     else if(gk233_practice_mode==2 && frame==3u)bits=0x1000u;
     else if(gk233_practice_mode==2 && frame==5u)bits=0x4000u;
     else if((gk233_practice_mode==2 && frame==7u)||
             (gk233_practice_mode==3 && frame==3u))bits=0x2000u;
    }
    Gk99XinputRaw raw={bits,0,0};Gk1PadSample decoded={0};
    if(!gk99_xinput_sample(&raw,&decoded)){
     if(IsWindow(hwnd))DestroyWindow(hwnd);
     if(font)DeleteObject(font);return 9;
    }
    pad_actions(&decoded);
    char gk233_diag[320];
    snprintf(gk233_diag,sizeof gk233_diag,
      "EXEC0233 mode=%d frame=%u screen=%u stage=%u actions=%u remaining=%u status=%.150s",
      gk233_practice_mode,(unsigned)frame,(unsigned)app.screen,
      (unsigned)app.setup_stage[0],(unsigned)app.port_spring.actions_committed,
      (unsigned)app.port_spring.remaining,app.status);
    gk231_trace(gk233_diag);
    int valid=1;
    if(gk233_practice_mode==1){
     if(frame==7u)valid=app.screen==GK1_SCREEN_SETUP_WIZARD;
     if(frame==15u)valid=app.screen==GK1_SCREEN_SETUP_WIZARD&&
       app.setup_stage[0]==GK53_SETUP_AWAIT_SOURCE;
     if(frame==17u)valid=app.screen==GK1_SCREEN_ENHANCED_SPRING_LAB&&
       app.port_spring.remaining==3u;
     if(frame==19u)valid=app.port_spring.actions_committed==1u&&
       app.port_spring.remaining==2u;
     if(frame==21u)valid=strstr(app.status,"SOURCE-REPLAY VERIFIED")!=NULL;
     if(frame==23u)valid=app.screen==GK1_SCREEN_SETUP_WIZARD;
    }else if(gk233_practice_mode==2){
     if(frame==1u)valid=app.screen==GK1_SCREEN_ENHANCED_SPRING_LAB&&
       app.port_spring.actions_committed==1u&&app.port_spring.remaining==2u;
     if(frame==3u)valid=app.port_spring.actions_committed==2u&&
       app.port_spring.remaining==1u;
     if(frame==5u)valid=strstr(app.status,"SOURCE-REPLAY VERIFIED")!=NULL;
     if(frame==7u)valid=app.screen==GK1_SCREEN_SCENARIO_COUNTRIES;
    }else if(gk233_practice_mode==3){
     if(frame==1u)valid=app.screen==GK1_SCREEN_ENHANCED_SPRING_LAB&&
       app.port_spring.actions_committed==2u&&app.port_spring.remaining==1u;
     if(frame==3u)valid=app.screen==GK1_SCREEN_SCENARIO_COUNTRIES;
    }else if(gk233_practice_mode==4 && frame==1u){
     valid=app.screen==GK1_SCREEN_TITLE&&!app.port_spring.active&&
       strstr(app.status,"CORRUPT - UNCHANGED")!=NULL;
    }
    if(!valid){
     gk231_trace("EXEC0233 native Windows practice controller transition REFUSED");
     if(IsWindow(hwnd))DestroyWindow(hwnd);
     if(font)DeleteObject(font);return 9;
    }
   }else{
    gk231_trace("before XInput");poll_xinput();gk231_trace("after XInput");
   }
   InvalidateRect(hwnd,NULL,FALSE);gk231_trace("before frame UpdateWindow");
   UpdateWindow(hwnd);gk231_trace("after frame UpdateWindow");
   Sleep(1);
  }
  int ok=running&&gk230_painted_frames>=smoke_frames;
  if(gk233_practice_mode){
   ok=ok&&app.rules_profile==1u&&app.active_world==0u&&
     app.worlds[0].phase==GK1_PRE_SETUP_TEMPLATE&&
     app.worlds[1].phase==GK1_PRE_SETUP_TEMPLATE&&
     memcmp(gk232_original_worlds,app.worlds,sizeof gk232_original_worlds)==0;
   if(gk233_practice_mode==4)
     ok=ok&&app.screen==GK1_SCREEN_TITLE&&!app.port_spring.active;
   else if(gk233_practice_mode==1)
     ok=ok&&app.screen==GK1_SCREEN_SETUP_WIZARD;
   else
     ok=ok&&app.screen==GK1_SCREEN_SCENARIO_COUNTRIES;
   gk231_trace(ok?"EXEC0233 Win32 Enhanced save/cold-load/corruption stage PASS":
                   "EXEC0233 Win32 Enhanced persistence stage FAIL");
  }
  if(gk232_scripted_gamepad){
   ok=ok&&app.screen==GK1_SCREEN_SCENARIO_COUNTRIES&&
      app.rules_profile==0u&&app.active_world==0u&&
      app.worlds[0].phase==GK1_PRE_SETUP_TEMPLATE&&
      app.worlds[1].phase==GK1_PRE_SETUP_TEMPLATE&&
      memcmp(gk232_original_worlds,app.worlds,sizeof gk232_original_worlds)==0;
   gk231_trace(ok?"EXEC0232 scripted XInput menu navigation and original source immutability PASS":
                 "EXEC0232 scripted XInput menu navigation FAIL");
  }
  if(IsWindow(hwnd))DestroyWindow(hwnd);
  if(font)DeleteObject(font);
  return ok?0:6;
 }
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
 /* EXEC0218: validates a bounded, owner-side original-stock write trace.
  * Even a passing audit is syntax and ROM identity only; raw event claims
  * require independent native-Mesen evidence and are never unlock triggers. */
 if(argc==4 && strcmp(argv[1],"--audit-private-stock-spring-write-trace")==0){
  Gk218Report report={0};
  Gk218Status status=gk218_audit_fresh_trace(argv[2],argv[3],&report);
  if(status!=GK218_OK){
   fprintf(stderr,"GK218 trace REFUSED status=%d; original Spring remains unverified\n",(int)status);
   return 9;
  }
  printf("VERIFIED_STATIC owner-supplied trace schema (NOT ORIGINAL OBSERVATION): writes=%u confirmed=%u rng=%u stages=%u final_year=%u rate=%u orders=%u countries=%u workram=%u\n",
         report.writes,report.confirmed_writes,report.rng_writes,report.stages,
         report.final_year,report.final_rate,report.final_orders,
         report.final_countries,report.bank_observed_workram);
  puts("NO dynamic stock first Spring, no Mesen execution or source event attestation performed by this tool.");
  return 0;
 }
 /* EXEC0216: direct ORIGINAL Mesen capture CPU sample PCs, source gated.
  * Neither sampled PC nor a breakpoint suggestion proves executed writes. */
 if(argc==5&&strcmp(argv[1],"--audit-original-fresh-cpu-pcs")==0){
  gk1_app_init(&app);
  if(!load_scenario_data())return 9;
  app.rules_profile=0u;
  app.setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
  Gk216CpuReport report={0};
  Gk216Status status=gk1_app_original_fresh_cpu_pc(&app,
    scenario_csv_path(),argv[2],argv[3],&report);
  /* argv[4] is a required, explicit typed confirmation that private
   * Mesen evidence is *only* an archived observation. */
  if(strcmp(argv[4],"OBSERVATION_ONLY")!=0||status!=GK216_OK){
   fputs("GK216 original-stock ROM or 14-stage Mesen source refused; no application state changed\n",stderr);
   return 9;
  }
  for(unsigned i=0;i<report.count;i++){
   const Gk216CpuStage *s=&report.stages[i];
   printf("OBSERVED_RUNTIME_STOCK_FRESH_CAPTURE stage=%02u frame=%u cycles=%llu sampled_PC=$%04X cpu_window=%s change_mask=%u\n",
     i+1u,s->frame,(unsigned long long)s->cycles,s->pc,
     s->upper_program_window?"upper_4000_mapper_unknown":"lower_4000_mapper_unknown",s->change_mask);
  }
  puts("PC sampled at end of each archived stage; no runtime memory-write/RNG-dispatch chronology and no generated first Spring. NOT a new Mesen execution.");
  return 0;
 }

 /* EXEC0215: original first-Spring three-history parity GAP test, not a
  * general initialization generator. External private RAM never bundled. */
 if(argc==5&&strcmp(argv[1],"--audit-original-first-spring-gaps")==0){
  Gk1TemplateDataset *data=calloc(1,sizeof *data);
  Gk1LoadedScenario *pre=calloc(1,sizeof *pre);
  Gk79Checkpoint *source=calloc(1,sizeof *source);
  Gk215Gap *gap=calloc(1,sizeof *gap);
  int ok=data&&pre&&source&&gap&&gk1_verified_original_csv(scenario_csv_path())&&
     gk1_template_load(scenario_csv_path(),data)==GK1_LOAD_OK;
  for(unsigned p=1;p<=3u&&ok;p++){
   Gk1ScenarioId campaign=(Gk1ScenarioId)(p==3u?2u:1u);
   ok=gk1_open_loaded_scenario(data,campaign,pre)==GK1_LOAD_OK &&
      gk79_open_original(pre,argv[p+1],(Gk77Profile)p,0,source)==GK79_OK &&
      gk215_compare_original_spring(scenario_csv_path(),pre,source,gap)==GK215_OK;
   if(ok)printf("VERIFIED_STATIC profile=%u countries=%u original_spring_vs_template_finance_differs=%u gold=%u food=%u troops=%u labor=%u; never independent first Spring\n",
     p,gap->country_count,gap->differing_countries,gap->gold_mismatches,
     gap->food_mismatches,gap->troop_mismatches,gap->workforce_mismatches);
  }
  free(gap);free(source);free(pre);free(data);
  if(!ok){fputs("GK215 private source/profile refused; no game state changed\n",stderr);return 9;}
  puts("Previous ORIGINAL Mesen evidence only; NOT new Mesen execution, not generated 14/27 first Spring.");
  return 0;
 }

 /* GK214: both native campaign datasets and the source-based Spring finance
  * engine, without claiming original setup/RNG or awarding player orders. */
 if(argc==2&&strcmp(argv[1],"--audit-both-spring-finance-batches")==0){
  gk1_app_init(&app);
  if(!load_scenario_data()){
   fputs("GK214 canonical 41-country source unavailable/invalid\n",stderr);
   return 9;
  }
  for(unsigned branch=0;branch<2u;branch++){
   Gk214Batch result={0};
   if(gk1_app_spring_batch_projection(&app,(uint8_t)branch,&result)!=GK214_OK){
    fputs("GK214 source-bounded all-country batch refused\n",stderr);
    return 9;
   }
   printf("SOURCE_BOUNDED_TEMPLATE_PROJECTION original_campaign=%s countries=%u gross_tax=%u wage=%u food_consumed=%u starved=%u; source_setup=0 rng_generated=0 orders_granted=0\n",
     branch?"WORLD":"MONGOL",result.count,result.gross_taxes,result.payroll,
     result.food_consumed,result.starved_country_count);
  }
  puts("These are original bank1 finance equations applied to UNINITIALIZED templates. NOT authentic first playable Spring or independent original NES execution.");
  return 0;
 }

 /* EXEC0213 original single-history Spring arithmetic boundary, not startup.
  * Authenticates all private fresh-reset stages and canonical source CSV.
  * This diagnostic does not advance the controller or a campaign. */
 if(argc==4&&strcmp(argv[1],"--audit-original-first-spring-boundary")==0){
  Gk1TemplateDataset *all=(Gk1TemplateDataset*)calloc(1,sizeof *all);
  if(!all)return 9;
  int ok=gk1_verified_original_csv(argv[3]) &&
      gk1_template_load(argv[3],all)==GK1_LOAD_OK;
  gk1_app_init(&app);
  if(ok)ok=gk1_app_install_templates(&app,all)!=0;
  free(all);
  Gk213Boundary result={0};
  if(ok)ok=gk1_app_observed_first_spring_boundary(&app,argv[3],argv[2],&result)==GK213_OK;
  if(!ok){fputs("GK213 original first-Spring source boundary refused; no game state changed\n",stderr);return 9;}
  printf("OBSERVED_RUNTIME_STOCK_FRESH_RESET (archived evidence; no new Mesen): start Food %u, fed Food %u, tax %u, wages %u, Gold %u; troops before observed growth %u, after %u.\n",
   (unsigned)result.food_after_start_bonus,(unsigned)result.food_after_feeding,
   (unsigned)result.tax_gross,(unsigned)result.wages,
   (unsigned)result.gold_after_taxes_and_wages,
   (unsigned)result.troops_before_growth,(unsigned)result.troops_after_observed_growth);
  puts("Bounded arithmetic consistency only. Economy timing/RNG/first Spring for 14/27 countries, player turns and merchants NOT generated.");
  return 0;
 }

 /* EXEC0212: two authenticated original stock Spring histories, read-only.
  * No original country bytes or RNG seed values printed or published. */
 if(argc==4&&strcmp(argv[1],"--audit-original-mongol-variation")==0){
  gk1_app_init(&app);
  if(!load_scenario_data())return 8;
  Gk79Checkpoint *fresh=(Gk79Checkpoint*)calloc(1,sizeof *fresh);
  Gk79Checkpoint *archived=(Gk79Checkpoint*)calloc(1,sizeof *archived);
  if(!fresh||!archived){free(fresh);free(archived);return 9;}
  const Gk1LoadedScenario *original=&app.worlds[0];
  int accepted=(gk79_open_original(original,argv[2],GK77_FRESH_MONGOL_WORKRAM,0,fresh)==GK79_OK &&
    gk79_open_original(original,argv[3],GK77_ARCHIVED_MONGOL_SAVERAM,0,archived)==GK79_OK);
  Gk212Variation report={0};
  if(accepted)accepted=gk1_app_original_raw_variation(&app,scenario_csv_path(),fresh,archived,&report)==GK212_OK;
  free(fresh);free(archived);
  if(!accepted){fputs("GK212 private original source/record/RAW bank evidence refused; no state changed\n",stderr);return 9;}
  printf("VERIFIED_STATIC original stock two-history raw source variation: %u/%u byte cells differ, %u/83 offsets, %u/14 countries, %u/24 source-projected metrics.\n",
   report.differing_cells,report.cells_compared,report.offsets_differing,
   report.countries_differing,report.numeric_fields_differing);
  puts("TWO observed original histories only: NOT original first-Spring RNG reconstruction, NOT a universal starting constant, no new Mesen execution.");
  return 0;
 }

 /* Coefficients loaded only from SHA-verified PRIVATE stock cartridge.
  * This proves bounded arithmetic parity, NOT a runtime RNG invocation. */
 if(argc==3 && strcmp(argv[1],"--audit-stock-rng-parity")==0){
  Gk206Audit audit={0};
  Gk206Status status=gk206_compare_original_stock(argv[2],&audit);
  if(status!=GK206_PASS){
   fprintf(stderr,"ORIGINAL STOCK RNG PARITY REFUSED status=%u; no world state changed\n",
     (unsigned)status);
   return 8;
  }
  printf("VERIFIED_STATIC stock fixed-bank RNG C289 parity: seed_cases=%u 6502_draw_pairs=%u source_rate_pairs=%u mismatches=%u\n",
    (unsigned)audit.initial_seeds,(unsigned)audit.draw_pairs,
    (unsigned)audit.spring_rate_pairs,(unsigned)audit.mismatches);
  puts("SOURCE_BOUNDED_ONLY; no original-Mesen CPU event, source seed initialization, or playable Spring generated");
  return 0;
 }
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
 if(argc==3&&strcmp(argv[1],"--source-render-window")==0){
  Gk150Report result;
  Gk150Status status=gk150_scan_render_window(argv[2],&result);
  if(status!=GK150_BOUNDED){fprintf(stderr,"GK150 source not verified / invalid\n");return 2;}
  printf("VERIFIED_STATIC GK150: %u hypothetical origins, %u render writes accepted, %u earlier PPUSTATUS race gates; first PPUMASK write $%04X=$%02X, subsequent source stop $%04X at CPU cycles %llu. Mesen parity NOT RUN, Spring NOT GENERATED\n",
    result.origins_checked,result.render_accepted,result.legacy_race_gates,
    result.first_render_pc,result.first_render_mask,result.first_after_render_stop_pc,
    (unsigned long long)result.first_after_render_stop_cycles);
  return 0;
 }
 if(argc==5&&strcmp(argv[1],"--source-symbolic-vblank-exit")==0){
  char *end1=NULL,*end2=NULL;
  unsigned long origin=strtoul(argv[3],&end1,10),choice=strtoul(argv[4],&end2,10);
  if(!argv[3][0]||!argv[4][0]||!end1||*end1||!end2||*end2||
     origin>=GK103_FRAME_DOTS||choice<1u||choice>2u)return 2;
  gk1_app_init(&app);app.setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
  Gk83Trace trace={0};
  if(gk1_app_source_symbolic_vblank_exit(&app,argv[2],(uint32_t)origin,
      (uint8_t)choice,&trace)!=GK153_TRACE_BOUNDED)return 3;
  printf("VERIFIED_STATIC CONDITIONAL GK157: branch=%u instructions=%u "
         "status_sign_exits=%u unknown_A=%u guarded_consumer=$%04X "
         "known_overwrite=$%04X stop=$%04X gate=%u countries=%u; "
         "NO Mesen parity / original first Spring\n",
    trace.hypothetical_race_choice,trace.instructions,
    trace.source_symbolic_high_exits,trace.source_accumulator_low7_unknown,
    trace.first_symbolic_unknown_consumer,trace.first_symbolic_cleared_by_lda,
    trace.stop_pc,(unsigned)trace.stop,trace.country_writes.world_region_writes);
  return 0;
 }
 if(argc==5&&strcmp(argv[1],"--source-sign-exit-matrix")==0){
  char *end1=NULL,*end2=NULL;
  unsigned long origin=strtoul(argv[3],&end1,10),choice=strtoul(argv[4],&end2,10);
  if(!argv[3][0]||!argv[4][0]||!end1||*end1||!end2||*end2||
     origin>=GK103_FRAME_DOTS||choice<1u||choice>2u)return 2;
  gk1_app_init(&app);app.setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
  Gk156StatusMatrix matrix={0};
  if(gk1_app_source_sign_exit_matrix(&app,argv[2],(uint32_t)origin,
       (uint8_t)choice,&matrix)!=GK153_TRACE_BOUNDED)return 3;
  printf("VERIFIED_STATIC COUNTERFACTUAL GK156: 128 candidate low7 values "
         "tested=%u source-sign1 candidates=%u divergent=%u "
         "nonvacuous_equal=%u stop=$%04X gate=%u country-stores=%u; "
         "original setup and Mesen parity NOT IMPLEMENTED\n",
      matrix.variants_run,matrix.example.source_sign_exit_candidates,
      matrix.divergent_variants,matrix.all_identical,matrix.example.stop_pc,
      (unsigned)matrix.example.stop,
      matrix.example.country_writes.world_region_writes);
  return 0;
 }
 if(argc==5&&strcmp(argv[1],"--source-sign-only-poll")==0){
  char *end1=NULL,*end2=NULL;
  unsigned long origin=strtoul(argv[3],&end1,10),choice=strtoul(argv[4],&end2,10);
  if(!argv[3][0]||!argv[4][0]||!end1||*end1||!end2||*end2||
     origin>=GK103_FRAME_DOTS||choice<1u||choice>2u)return 2;
  gk1_app_init(&app);app.setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
  Gk83Trace trace={0};
  if(gk1_app_source_sign_only_poll(&app,argv[2],(uint32_t)origin,
      (uint8_t)choice,&trace)!=GK153_TRACE_BOUNDED)return 3;
  printf("VERIFIED_STATIC CONDITIONAL GK155: source-sign-only $%04X reads=%u, "
         "first PPU edge choice=%u consumed=%u, stop=$%04X gate=%u "
         "instructions=%u country stores=%u; PPU ORIGIN UNVERIFIED, NO Mesen/first Spring\n",
    trace.source_sign_only_first_pc,trace.source_sign_only_status_reads,
    trace.hypothetical_race_choice,trace.hypothetical_race_consumed,
    trace.stop_pc,(unsigned)trace.stop,trace.instructions,
    trace.country_writes.world_region_writes);
  return 0;
 }
 if(argc==5&&strcmp(argv[1],"--source-one-vblank-race")==0){
  char *end1=NULL,*end2=NULL;
  unsigned long origin=strtoul(argv[3],&end1,10),choice=strtoul(argv[4],&end2,10);
  if(!argv[3][0]||!argv[4][0]||!end1||*end1||!end2||*end2||
     origin>=GK103_FRAME_DOTS||choice<1u||choice>2u)return 2;
  gk1_app_init(&app);app.setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
  Gk83Trace trace={0};
  if(gk1_app_source_one_race_branch(&app,argv[2],(uint32_t)origin,
      (uint8_t)choice,&trace)!=GK153_TRACE_BOUNDED)return 3;
  printf("VERIFIED_STATIC COUNTERFACTUAL GK154: selected branch=%u consumed=%u "
         "source PC=$%04X, stopped PC=$%04X gate=%u, executed=%u, "
         "country stores=%u; NOT Mesen parity, NOT playable Spring\n",
   trace.hypothetical_race_choice,trace.hypothetical_race_consumed,
   trace.hypothetical_race_source_pc,trace.stop_pc,(unsigned)trace.stop,
   trace.instructions,trace.country_writes.world_region_writes);
  return 0;
 }
 if(argc==3&&strcmp(argv[1],"--source-country-write-watch")==0){
  gk1_app_init(&app);app.setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
  Gk83Trace trace={0};
  if(gk1_app_source_country_writer_trace(&app,argv[2],&trace)!=GK153_TRACE_BOUNDED){
   fprintf(stderr,"GK153: exact PRIVATE original-stock ROM and source setup stage required\n");return 2;
  }
  printf("VERIFIED_STATIC GK153: CPU-executed country PRG-RAM writes=%u (first14=%u), source PC gate=$%04X, gate=%u; HYPOTHETICAL even-frame timing; no new Mesen, original Spring still UNIMPLEMENTED\n",
   trace.country_writes.world_region_writes,trace.country_writes.mongol_region_writes,
   trace.stop_pc,(unsigned)trace.stop);
  return 0;
 }
 if(argc==3&&strcmp(argv[1],"--source-boot-boundary")==0){
  Gk174SetupBoundary p={0};
  if(!gk174_trace_setup_boundary(argv[2],50000u,&p)){
   fprintf(stderr,"GK174 exact private source unavailable or boundary changed; NO state committed\n");
   return 32;
  }
  printf("VERIFIED_STATIC ORIGINAL_SOURCE_CPU_ONLY: STRICT unknown_RAM=$%04X at_PC=$%04X instructions=%u cycles=%llu; archived_MesenCE_RamPowerOnState_1_ALLZEROS first_PPUSTATUS=$%04X at_PC=$%04X instructions=%u cycles=%llu; executed_rng_transfers=%u/%u; not Mesen emulation, first Spring, merchant, setup RNG, or Classic playable actions.\n",
   p.strict.first_unknown_ram_read,p.strict.stop_pc,p.strict.instructions,
   (unsigned long long)p.strict.completed_cpu_cycles,
   p.mesen_zero.first_hardware_read,p.mesen_zero.stop_pc,p.mesen_zero.instructions,
   (unsigned long long)p.mesen_zero.completed_cpu_cycles,
   p.strict.executed_rng_transfers.total,p.mesen_zero.executed_rng_transfers.total);
  return 0;
 }
 if(argc==3&&strcmp(argv[1],"--source-executed-rng")==0){
  /* EXEC0173: bounded private stock-ROM CPU interpretation, not static
   * three-byte matching, Mesen emulation, season setup or a campaign. */
  Gk83Trace t={0};
  Gk83Stop status=gk83_trace_original_reset(argv[2],50000u,&t);
  if(status==GK83_SOURCE_REFUSED||status==GK83_INVALID){
   fprintf(stderr,"GK173 private source refused or invalid; no game state changed\n");
   return 31;
  }
  const Gk173RngControl *e=&t.executed_rng_transfers;
  printf("VERIFIED_STATIC/SOURCE_EXECUTED_BOUNDED GK173: STOP=%d PC=$%04X instructions=%u cycles=%llu JSR_EF85=%u JSR_C289=%u fixed_C1B8_JMP_C289=%u other_JMP_C289=%u indirect_JMP_C289=%u; not Mesen parity; no independent Spring or Classic actions.\n",
   (int)status,t.stop_pc,t.instructions,
   (unsigned long long)t.completed_cpu_cycles,
   e->dispatch_jsr_ef85,e->direct_jsr_c289,
   e->fixed_trampoline_jmp_c289,e->other_jmp_c289,e->indirect_jmp_c289);
  return 0;
 }
 if(argc==3&&strcmp(argv[1],"--source-spring-writers")==0){
  gk1_app_init(&app);
  app.setup_stage[app.active_world]=GK53_SETUP_AWAIT_SOURCE;
  Gk152WriterMap report={0};
  if(gk1_app_source_spring_writer_map(&app,argv[2],&report)!=GK152_OK){
   fprintf(stderr,"GK152: authenticated original-stock ROM required; no state changed\n");
   return 2;
  }
  printf("VERIFIED_STATIC GK152: world raw candidates=%u Mongol raw candidates=%u indirect store BYTE candidates=%u pointer words=%u; NOT executed instructions, NOT first Spring\n",
   report.raw_country_world_candidates,report.raw_country_mongol_candidates,
   report.raw_indirect_store_byte_candidates,report.raw_pointer_base_candidates);
  return 0;
 }
 if(argc==3&&strcmp(argv[1],"--source-even-prerender")==0){
  gk1_app_init(&app);
  app.setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
  Gk151Report result={0};
  Gk151Status status=gk1_app_source_even_prerender(&app,argv[2],&result);
  if(status!=GK151_BOUNDED){fprintf(stderr,"GK151 original stock ROM refused or invalid\n");return 2;}
  printf("VERIFIED_STATIC GK151: %u even-frame hypotheses, %u passed one pre-render boundary, %u passed farther than GK150; first subsequent stop $%04X at cycles %llu; NOT Mesen parity, NOT original first Spring\n",
    result.origins,result.even_crossed,(unsigned)result.source_advanced_after_legacy,
    result.first_cross_stop_pc,(unsigned long long)result.first_cross_stop_cycles);
  return 0;
 }
 if(argc==3&&strcmp(argv[1],"--source-phase-scan")==0){
  gk1_app_init(&app);
  app.setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;app.setup_difficulty[0]=3u;
  Gk149PhaseScan proof={0};
  Gk149Status r=gk1_app_source_ppu_phase_scan(&app,argv[2],&proof);
  if(r!=GK149_BOUNDED)return 18;
  printf("VERIFIED_STATIC: %u explicitly HYPOTHETICAL boot phases, PPUSTATUS race %u, PPUMASK rendering gate %u, first render $%04X value $%02X; Mesen parity NOT RUN; first Spring NOT generated\n",
    (unsigned)proof.origins_checked,(unsigned)proof.ppu_race_stops,
    (unsigned)proof.render_enable_stops,(unsigned)proof.first_render_pc,
    (unsigned)proof.first_render_mask);
  return 0;
 }
 if(argc==5&&strcmp(argv[1],"--extract-original-mss")==0){
  Gk77Profile profile=gk77_profile_parse(argv[3]);
  uint8_t original[GK163_MAX_RECORDS];size_t length=0u;
  Gk163Status rc=gk163_mesen_extract(argv[2],profile,original,&length);
  if(rc!=GK163_OK){fprintf(stderr,"Original-stock Mesen source refused (code %d); output unchanged\n",(int)rc);return 17;}
#ifndef _WIN32
  mode_t prev_umask=umask(0077);
#endif
  FILE *private_out=fopen(argv[4],"wbx");
#ifndef _WIN32
  umask(prev_umask);
#endif
  if(!private_out){fprintf(stderr,"Private source output not created (exists or access denied)\n");return 18;}
  size_t wrote=fwrite(original,1,length,private_out);
  int closed=fclose(private_out);
  if(wrote!=length||closed){remove(argv[4]);fprintf(stderr,"Private source output write refused\n");return 19;}
  memset(original,0,sizeof original);
  printf("VERIFIED_STATIC GK163: exact original-stock archived Mesen profile %s, %zu authenticated private country bytes extracted; separate Spring startup/3 NES commands NOT implemented\n",gk77_profile_name(profile),length);
  return 0;
 }
 if(argc==4&&strcmp(argv[1],"--extract-original-fresh-json")==0){
  uint8_t original[GK165_FRESH_SIZE];
  Gk165Status rc=gk165_extract_fresh_json(argv[2],original);
  if(rc!=GK165_OK){fprintf(stderr,"Fresh original Mesen WorkRAM JSON refused (%d); no output\n",(int)rc);return 27;}
#ifndef _WIN32
  mode_t prior_umask=umask(0077);
#endif
  FILE *dst=fopen(argv[3],"wbx");
#ifndef _WIN32
  umask(prior_umask);
#endif
  if(!dst){memset(original,0,sizeof original);return 28;}
  size_t wrote=fwrite(original,1,sizeof original,dst);
  int closed=fclose(dst);memset(original,0,sizeof original);
  if(wrote!=GK165_FRESH_SIZE||closed){remove(argv[3]);return 29;}
  puts("GK165 authentic PRIVATE original fresh Mesen WorkRAM exported, 14x83 bytes; this is NOT NES initialization/reconstructed RNG");
  return 0;
 }
 if(argc==5&&strcmp(argv[1],"--source-archived-bank")==0){
  Gk77Profile profile=gk77_profile_parse(argv[4]);
  if(profile!=GK77_ARCHIVED_MONGOL_SAVERAM&&profile!=GK77_ARCHIVED_WORLD_SAVERAM)
   return 36;
  gk1_app_init(&app);if(!load_scenario_data())return 8;
  app.active_world=(uint8_t)(profile==GK77_ARCHIVED_WORLD_SAVERAM?1u:0u);
  app.setup_stage[app.active_world]=GK53_SETUP_AWAIT_SOURCE;
  app.setup_difficulty[app.active_world]=3u;
  Gk182SavedBankProof *proof=calloc(1,sizeof *proof);if(!proof)return 37;
  Gk182Status res=gk1_app_archived_source_bank(&app,argv[2],argv[3],profile,proof);
  if(res!=GK182_OK){free(proof);fprintf(stderr,"Source/Mesen saved-state proof refused %d\n",(int)res);return 38;}
  printf("OBSERVED_RUNTIME_STOCK_ARCHIVED_RESTORED profile=%s Mesen saved-instant lowbank=%u highbank=%u PC=$%04X original CPU seed witnessed (PRIVATE), fixed seed-writer STATIC candidates=%u lowbank candidates=%u; NO NES RESET bank, FIRST SPRING OR CLASSIC ORDERS GENERATED\n",
   gk77_profile_name(profile),proof->mapped_at_saved_instant.lower_prg_bank,
   proof->mapped_at_saved_instant.upper_prg_bank,
   proof->mapped_at_saved_instant.saved_program_counter,
   (unsigned)proof->conditional_source_sites.high_seed_candidates,
   (unsigned)proof->conditional_source_sites.low_seed_candidates);
  free(proof);return 0;
 }
 if(argc==4&&strcmp(argv[1],"--source-bank-seed-writers")==0){
  char *endptr=NULL;unsigned long chosen=strtoul(argv[3],&endptr,10);
  if(!argv[3][0]||!endptr||*endptr||chosen>14u)return 34;
  gk1_app_init(&app);if(!load_scenario_data())return 8;
  app.active_world=0u;app.setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
  app.setup_difficulty[0]=3u;
  Gk181Map map={0};
  Gk181Result rc=gk1_app_bank_seed_trace(&app,argv[2],(uint8_t)chosen,&map);
  if(rc!=GK181_OK){fprintf(stderr,"Stock ROM/bank or setup gating refused (%d)\n",(int)rc);return 35;}
  printf("VERIFIED_STATIC bank-hypothesis=%lu original stock seed writer candidates: fixed=%u, assumed-low=%u, mapper barriers=%u, indexed unresolved=%u, indirect=%u; NO original Mesen RUN, FIRST SPRING NOT GENERATED\n",
   chosen,(unsigned)map.high_seed_candidates,(unsigned)map.low_seed_candidates,
   (unsigned)map.mapper_barriers,(unsigned)map.unresolved_indexed_writes,
   (unsigned)map.indirect_jumps);
  return 0;
 }
 if(argc==3&&strcmp(argv[1],"--source-seed-writers")==0){
  gk1_app_init(&app);if(!load_scenario_data())return 8;
  app.active_world=0u;app.setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
  app.setup_difficulty[0]=3u;
  Gk180WriterMap map={0};
  Gk180Status result=gk1_app_source_seed_writers(&app,argv[2],&map);
  if(result!=GK180_OK){fprintf(stderr,"Original stock source/phase refused (%d)\n",(int)result);return 33;}
  printf("VERIFIED_STATIC original fixed-bank source writer candidates %u, RNG-internal %u, external static %u, reset decoded %u NMI decoded %u IRQ decoded %u, dynamic/unmapped %u; FIRST SPRING NOT GENERATED; Win32/physical Xbox NOT RUN\n",
  (unsigned)map.source_sites,(unsigned)map.internal_rng_sites,
  (unsigned)map.external_seed_site_candidates,(unsigned)map.decoded[0],
  (unsigned)map.decoded[1],(unsigned)map.decoded[2],(unsigned)map.unmapped_edges);
  return 0;
 }
 if(argc==2&&strcmp(argv[1],"--version")==0){
  puts("Genghis Khan I native research prototype " GK1_PORT_VERSION " (EXEC0193 private original-stock archived SaveRAM/fresh WorkRAM SHA-attested auto entry (NO original Spring RNG generated); EXEC0192 third completed unpassed Classic ASSIGN undo refunds one order, original ROM/source untouched; EXEC0191 source-only authentic fixed-bank EF85 inline dispatcher audit (NO source RNG executed); EXEC0190 Y controller source-replay undo of last unpassed manual-derived Classic ASSIGN (NOT stock NES Undo); EXEC0189 private self-contained source-attested Classic ASSIGN cold-save/restore (not independent NES first Spring); EXEC0188 source-attested Classic ASSIGN exclusive GC18 receipt, controller X save/Y cold-reattach, NO full independent cold-load; EXEC0187 source-attested Classic manual-derived ASSIGN with three real native budget actions, NO original stock NES parity or startup; EXEC0186 authenticated fixed/low-bank JSR-to-$C289 CFG audit (static negatives remain negatives; no NES run); EXEC0185 controller X/Y stock snapshot refuses source/profile mismatch before I/O; EXEC0184 production controller synthetic-source refusal (existing authentic original VIEW/PASS); EXEC0183 original checkpoint lineage isolation / protected private Classic storage; EXEC0182 authenticated archived Mesen saved-instant mapper/CPU seed (NOT RESET/first Spring); EXEC0181 explicit-bank static original seed-writer hypothesis with mapper barrier; EXEC0180 source-authenticated static seed-writer CFG (not boot execution or first Spring); EXEC0179 source-authentic RNG only after validated setup/difficulty/Classic-vs-Enhanced guards, not original first-Spring; EXEC0178 fixed-bank 6502 RNG 128-source-vector crosscheck (not NES first Spring), EXEC0177 durable source-attested Enhanced 20-season receipt/cold resume; EXEC0176 Enhanced 20-season labor-only continuation; EXEC0174 original stock CPU boot/RAM provenance boundary; EXEC0173 bounded executed RNG control-transfer telemetry; EXEC0172 static source RNG xref; EXEC0171 controller X TITLE source-attested Enhanced labor-season cold resume, no NES season parity; EXEC0170 read-only SHA-verified TITLE private slot inventory; EXEC0169 port-only controller OTHER private checkpoint save/load; EXEC0168 Linux protected owner-private cold-save reload/flush; EXEC0167 owner-private self-contained source-attested Spring save/cold resume, NOT a full campaign; EXEC0166 integrated fresh WorkRAM JSON in-memory checkpoint + EXEC0164 archived Mesen .mss; original NES initialization still NOT reconstructed; EXEC0164 in-memory archived original Mesen controller checkpoint entry (VIEW/PASS only, no independent Spring); EXEC0163 genuine archived Mesen .mss import verified against exact original stock country SHA (no NES first-Spring generation)"); puts("EXEC0162 source-attested original-stock checkpoint controller re-entry and atomic profile replacement, including clearing stale Enhanced YEAR2; VIEW/PASS only; not NES first-Spring independent generation; EXEC0161 authenticated Enhanced YEAR2 SPRING rollover after Winter, three labor-only orders and exclusive GY21 save/cold-load; NO original NES Spring economics, first Spring or character RNG; EXEC0160 Enhanced-only research Winter with Spring/Summer/Fall source-attested receipts, no original weather/AI; EXEC0159 Enhanced-only research Fall with Spring/Summer receipt chaining; EXEC0158 Enhanced-only research Summer with 3 labor orders, no original seasonal economy/RNG, preserving EXEC0157 symbolic PPUSTATUS low7 guarded trace, preserving EXEC0156 exhaustive 128-variant matrix; EXEC0155 source-verified sign-only polling of $2002 only inside a BPL loop (unverified PPU origin; first edge explicitly hypothetical; NO independent Spring), EXEC0154 explicitly hypothetical single VBlank-edge source branch; EXEC0153 executed country-write watch; EXEC0152 source-authenticated raw Spring country-store candidate inventory (not executed), EXEC0151 explicitly assumed EVEN pre-render source trace, NOT original Mesen parity; EXEC0150 bounded render-on-source progression (not Mesen parity); EXEC0149 verified stock PPU phase gate/source scan (NOT original Mesen parity); EXEC0148 private-source bank-aware original 15-stage reset/Spring chronology; EXEC0147 original stock source-backed ten-goods controller VIEW; EXEC0146 Enhanced authentic-checkpoint variable-quantity labor; EXEC0145 fresh progress gate (no Spring creation), EXEC0144 archived Mesen PRG-page mapping, EXEC0143 bank-hypothesis CFG source audit, EXEC0142 fixed-bank 6502 CFG source screen, EXEC0141 dispatcher byte-pattern audit, EXEC0140 bounded saved-state RNG lineage, EXEC0139 archived original Mesen CPU-RNG proof (read-only), EXEC0138 48-bit inverse source history, EXEC0137 source-mapped 48-bit RNG/rate proof, EXEC0136 constant-time PPU, EXEC0135 atomic PPU refusal, EXEC0134 TAX/ASSIGN controller route, EXEC0133 boot read-before-write provenance, EXEC0132 conditional F5 source gate, EXEC0131 boot provenance, EXEC0130 DMA bus, EXEC0129 source-gated CPU-RAM OAM DMA, EXEC0128 PPU OAM writes, EXEC0127 atomic I/O rollback, EXEC0126 source I/O refusal, EXEC0125 ADC/SBC, EXEC0124 2A03 compare, EXEC0123 load/store, EXEC0122 logic, EXEC0120 save routing; independent first Spring not yet implemented; not full game)");return 0;
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
/* EXEC0120: actual native input/save/coldload codepath with the default
 * per-user XDG save path, not merely a path-builder unit test. Called only
 * by explicit offline smoke command; never auto-writes on game startup. */
if(argc==2 && strcmp(argv[1],"--default-storage-pad-check")==0){
  gk1_app_init(&app);gk1_pad_init(&pad);
  if(!load_scenario_data())return 8;
  const char *default_path=pre_setup_file();
  if(!default_path||default_path[0]!='/')return 17;
  pad_test_press(GK1_PAD_A); /* title -> campaign chooser */
  pad_test_press(GK1_PAD_DOWN);pad_test_press(GK1_PAD_A); /* World */
  if(app.screen!=GK1_SCREEN_SCENARIO_COUNTRIES||app.active_world!=1)return 18;
  pad_test_press(GK1_PAD_X); /* default XDG path via native save routing */
  if(strcmp(app.status,"PRE_SETUP SETUP-DRAFT SAVED - NOT PLAYABLE SPRING"))return 19;
  FILE *saved=fopen(default_path,"rb");
  if(!saved)return 20;
  if(fseek(saved,0,SEEK_END)!=0){fclose(saved);return 21;}
  long bytes=ftell(saved);fclose(saved);
  if(bytes!=(long)GK55_PRE_SETUP_BYTES)return 22;
  pad_test_press(GK1_PAD_DOWN);
  if(app.country_cursor[1]!=1)return 23;
  pad_test_press(GK1_PAD_Y); /* real file cold load through native routing */
  if(app.active_world!=1||app.country_cursor[1]!=0||
     app.screen!=GK1_SCREEN_SCENARIO_COUNTRIES||
     app.worlds[1].country_count!=27)return 24;
  puts("EXEC0125 TESTED_PORT_LINUX inherited XDG default user-save X/Y synthetic controller route and cold-load PASS; not original first Spring");
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
 int show_stock_finance=0,show_stock_trade=0,show_stock_setup=0,show_stock_goods=0,show_enhanced_summer=0,show_enhanced_fall=0,show_enhanced_winter=0,show_enhanced_year2=0;
 int play_fresh_json=0,smoke_private_controller=0,smoke_classic_storage_original=0,smoke_classic_view_original=0,smoke_classic_trade_original=0,smoke_classic_tax_original=0,smoke_classic_three_original=0,smoke_classic_three_cold_original=0;
 int smoke=0,show_snapshot=0,show_path=0,show_divide=0,show_scenario=0,show_setup=0,show_labor=0,show_classic_finance=0,show_finance_compare=0,show_labor_undo=0,show_enhanced_lab=0,show_enhanced_lab_save=0,show_enhanced_lab_title_resume=0,show_enhanced_lab_title_reject=0,show_gk226_title_update=0,show_gk226_title_verify=0,show_gk226_title_final=0,show_demob=0,show_manual=0,show_market=0,show_clock=0,show_whatif=0,show_rest=0,show_mesen=0,show_order_budget=0,show_pass=0,show_spring=0,show_stock_spring=0,show_stock_checkpoint=0,show_stock_commands=0;
 if(argc==5&&strcmp(argv[1],"--smoke-original-private-controller")==0){
  smoke=atoi(argv[4]);if(smoke<1||smoke>120)return 4;
  play_fresh_json=1;smoke_private_controller=1;
 }else if(argc==3&&strcmp(argv[1],"--play-original-fresh-json")==0){
  play_fresh_json=1;
 }else if(argc==4&&strcmp(argv[1],"--smoke-original-fresh-json")==0){
  smoke=atoi(argv[3]);if(smoke<1||smoke>120)return 4;
  play_fresh_json=1;
 }else if(argc==3 && (strcmp(argv[1],"--smoke-frames")==0 ||
                 strcmp(argv[1],"--smoke-snapshot")==0 ||
                 strcmp(argv[1],"--smoke-path")==0 ||
                 strcmp(argv[1],"--smoke-divide")==0 ||
                 strcmp(argv[1],"--smoke-scenario")==0 ||
                 strcmp(argv[1],"--smoke-setup")==0 ||
                 strcmp(argv[1],"--smoke-labor")==0 || strcmp(argv[1],"--smoke-classic-labor-undo")==0 || strcmp(argv[1],"--smoke-enhanced-spring-lab")==0 || strcmp(argv[1],"--smoke-enhanced-practice-save")==0 || strcmp(argv[1],"--smoke-enhanced-practice-title-resume")==0 || strcmp(argv[1],"--smoke-enhanced-practice-title-reject")==0 || strcmp(argv[1],"--smoke-enhanced-practice-title-update")==0 || strcmp(argv[1],"--smoke-enhanced-practice-title-verify")==0 || strcmp(argv[1],"--smoke-enhanced-practice-title-final")==0 || strcmp(argv[1],"--smoke-classic-finance")==0 || strcmp(argv[1],"--smoke-classic-finance-delta")==0 || strcmp(argv[1],"--smoke-demob")==0 ||
                 strcmp(argv[1],"--smoke-manual-rules")==0 || strcmp(argv[1],"--smoke-order-budget")==0 || strcmp(argv[1],"--smoke-market")==0 ||
                 strcmp(argv[1],"--smoke-battle-clock")==0 ||
                 strcmp(argv[1],"--smoke-battle-whatif")==0 ||
                 strcmp(argv[1],"--smoke-battle-pass")==0 ||
                 strcmp(argv[1],"--smoke-rest")==0 ||
                 strcmp(argv[1],"--smoke-mesen-grid")==0 ||
                 strcmp(argv[1],"--smoke-spring-archive")==0 ||
                 strcmp(argv[1],"--smoke-stock-spring")==0 ||
                 strcmp(argv[1],"--smoke-stock-checkpoint")==0 ||
                 strcmp(argv[1],"--smoke-stock-commands")==0 ||
                 strcmp(argv[1],"--smoke-stock-finance")==0 ||
                 strcmp(argv[1],"--smoke-stock-setup")==0 ||
                 strcmp(argv[1],"--smoke-stock-goods")==0 ||
                 strcmp(argv[1],"--smoke-stock-trade")==0 ||
                 strcmp(argv[1],"--smoke-enhanced-summer")==0 ||
                 strcmp(argv[1],"--smoke-enhanced-fall")==0 ||
                 strcmp(argv[1],"--smoke-enhanced-winter")==0 ||
                 strcmp(argv[1],"--smoke-enhanced-year2")==0)){
  smoke=atoi(argv[2]);if(smoke<1||smoke>120)return 4;
  show_snapshot=(strcmp(argv[1],"--smoke-snapshot")==0);
  show_path=(strcmp(argv[1],"--smoke-path")==0);
  show_divide=(strcmp(argv[1],"--smoke-divide")==0);
  show_scenario=(strcmp(argv[1],"--smoke-scenario")==0);
  show_setup=(strcmp(argv[1],"--smoke-setup")==0);
  show_labor=(strcmp(argv[1],"--smoke-labor")==0);
  show_labor_undo=(strcmp(argv[1],"--smoke-classic-labor-undo")==0);
  show_enhanced_lab_save=(strcmp(argv[1],"--smoke-enhanced-practice-save")==0);
  show_enhanced_lab_title_resume=(strcmp(argv[1],"--smoke-enhanced-practice-title-resume")==0);
  show_enhanced_lab_title_reject=(strcmp(argv[1],"--smoke-enhanced-practice-title-reject")==0);
  show_gk226_title_update=(strcmp(argv[1],"--smoke-enhanced-practice-title-update")==0);
  show_gk226_title_verify=(strcmp(argv[1],"--smoke-enhanced-practice-title-verify")==0);
  show_gk226_title_final=(strcmp(argv[1],"--smoke-enhanced-practice-title-final")==0);
  show_enhanced_lab=(strcmp(argv[1],"--smoke-enhanced-spring-lab")==0)||show_enhanced_lab_save;
  show_finance_compare=(strcmp(argv[1],"--smoke-classic-finance-delta")==0);
  show_classic_finance=(strcmp(argv[1],"--smoke-classic-finance")==0)||show_finance_compare;
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
  show_stock_commands=(strcmp(argv[1],"--smoke-stock-commands")==0);
  show_stock_finance=(strcmp(argv[1],"--smoke-stock-finance")==0);
  show_stock_setup=(strcmp(argv[1],"--smoke-stock-setup")==0);
  show_stock_goods=(strcmp(argv[1],"--smoke-stock-goods")==0);
  show_stock_trade=(strcmp(argv[1],"--smoke-stock-trade")==0);
  show_enhanced_summer=(strcmp(argv[1],"--smoke-enhanced-summer")==0);
  show_enhanced_fall=(strcmp(argv[1],"--smoke-enhanced-fall")==0);
  show_enhanced_winter=(strcmp(argv[1],"--smoke-enhanced-winter")==0);
  show_enhanced_year2=(strcmp(argv[1],"--smoke-enhanced-year2")==0);
  show_stock_checkpoint=(strcmp(argv[1],"--smoke-stock-checkpoint")==0)||show_stock_commands||show_stock_finance||show_stock_setup||show_stock_goods||show_stock_trade||show_enhanced_summer||show_enhanced_fall||show_enhanced_winter||show_enhanced_year2;
  show_stock_spring=(strcmp(argv[1],"--smoke-stock-spring")==0)||show_stock_checkpoint;
 }
 else if(argc==3&&strcmp(argv[1],"--play-original-auto")==0){
  /* Auto-detect the exact private archived SaveRAM or fresh WorkRAM evidence. */
 }
 else if(argc==4&&strcmp(argv[1],"--smoke-original-auto")==0){
  smoke=atoi(argv[3]);if(smoke<1||smoke>120)return 4;
 }
 else if(argc==4&&strcmp(argv[1],"--smoke-classic-storage-original")==0){
  smoke=atoi(argv[3]);if(smoke<1||smoke>120)return 4;
  smoke_classic_storage_original=1;
 }
 else if(argc==4&&strcmp(argv[1],"--smoke-classic-view-original")==0){
  smoke=atoi(argv[3]);if(smoke<1||smoke>120)return 4;
  smoke_classic_view_original=1;
 }
 else if(argc==4&&strcmp(argv[1],"--smoke-classic-trade-original")==0){
  smoke=atoi(argv[3]);if(smoke<1||smoke>120)return 4;
  smoke_classic_trade_original=1;
 }
 else if(argc==4&&(strcmp(argv[1],"--smoke-classic-three-original")==0 ||
                         strcmp(argv[1],"--smoke-classic-three-cold-original")==0)){
  smoke=atoi(argv[3]);if(smoke<1||smoke>120)return 4;
  smoke_classic_three_original=1;
  smoke_classic_three_cold_original=(strcmp(argv[1],"--smoke-classic-three-cold-original")==0);
 }
 else if(argc==4&&strcmp(argv[1],"--smoke-classic-tax-original")==0){
  smoke=atoi(argv[3]);if(smoke<1||smoke>120)return 4;
  smoke_classic_tax_original=1;
 }
 else if(argc==4&&strcmp(argv[1],"--play-original-mss")==0){
  /* Full controller UI opens only after main loop loads the canonical 41
   * original templates, then validates the owner-private archived source. */
 }
 else if(argc==5&&strcmp(argv[1],"--smoke-original-mss")==0){
  smoke=atoi(argv[4]);if(smoke<1||smoke>120)return 4;
 }
 else if(argc==5&&strcmp(argv[1],"--audit-stock-archived-seed")==0){
  Gk77Profile profile;
  if(strcmp(argv[4],"mongol1175")==0)profile=GK77_ARCHIVED_MONGOL_SAVERAM;
  else if(strcmp(argv[4],"world1206")==0)profile=GK77_ARCHIVED_WORLD_SAVERAM;
  else {fputs("GK208 unknown archived profile\n",stderr);return 8;}
  Gk208ArchiveProof proof={0};
  Gk208Status result=gk208_archived_source_rng(argv[2],argv[3],profile,&proof);
  if(result!=GK208_OK){fprintf(stderr,"GK208 rejected source/state code=%u\n",(unsigned)result);return 8;}
  printf("OBSERVED_RUNTIME_STOCK_ARCHIVED_SNAPSHOT profile=%s bank=SaveRAM countries=%u pc=$%04X cpu_cycles=%llu archived_seed48=%012llX hypothetical_next_seed48=%012llX hypothetical_next_draw15=%u SOURCE_PARITY=PASS\n",
   argv[4],proof.country_count,proof.archived_pc,
   (unsigned long long)proof.archived_cpu_cycles,
   (unsigned long long)proof.archived_seed48,
   (unsigned long long)proof.hypothetical_next_seed48,
   proof.hypothetical_next_draw15);
  puts("NOT ORIGINAL RESET, NOT NEW MESEN EXECUTION, NOT VERIFIED DRAW ORDER, NOT PLAYABLE FIRST SPRING");
  return 0;
 }
 else if(argc==6&&strcmp(argv[1],"--audit-stock-archived-rate-ambiguity")==0){
  Gk77Profile profile=strcmp(argv[4],"mongol1175")==0?
    GK77_ARCHIVED_MONGOL_SAVERAM:strcmp(argv[4],"world1206")==0?
    GK77_ARCHIVED_WORLD_SAVERAM:0;
  char *end=0;
  unsigned long depth=strtoul(argv[5],&end,10);
  if(!profile||!argv[5][0]||!end||*end||depth<1ul||depth>65536ul){
   fputs("GK209 invalid archived profile or search depth 1..65536\n",stderr);return 8;
  }
  Gk209RateAmbiguity scan={0};
  Gk209Status result=gk209_archived_rate_ambiguity(argv[2],argv[3],profile,
    (uint32_t)depth,&scan);
  if(result!=GK209_OK){
   fprintf(stderr,"GK209 authentic archive analysis refused code=%u\n",(unsigned)result);
   return 8;
  }
  printf("VERIFIED_STATIC_RETROSPECTIVE_SOURCE_AMBIGUITY profile=%s rate=%u search_window=%u countries=%u\n",
   argv[4],(unsigned)scan.observed_spring_rate,scan.window,
   profile==GK77_ARCHIVED_WORLD_SAVERAM?27u:14u);
  for(unsigned d=0;d<5u;d++){
   const Gk138RateScan *r=&scan.by_difficulty[d];
   printf("  hypothetical_difficulty=%u backwards_matches=%u closest_offset=%s",
    d+1u,r->matches,r->matches?"AVAILABLE":"NONE");
   if(r->matches)printf(" (%u)",r->nearest_offset);
   putchar('\n');
  }
  printf("candidate_difficulties=%u single-match_difficulties=%u; chronology_proved=0 first_spring_generated=0\n",
    scan.candidate_difficulties,scan.uniquely_located_difficulties);
  puts("PRIOR AUTHENTIC MESEN SAVESTATE, NOT NEW EMULATOR EXECUTION. RETROACTIVE NUMERIC MATCHES CANNOT IDENTIFY THE ACTUAL RANDOM DRAW, CHARACTER SETUP, DIFFICULTY, OR ORIGINAL SPRING.");
  return 0;
 }
 else if(argc==3&&strcmp(argv[1],"--audit-stock-seed-writes")==0){
  Gk83Trace unknown={0},zero={0};
  Gk83Stop strict=gk83_trace_original_reset(argv[2],20000u,&unknown);
  Gk83Stop mesen=gk91_trace_reset_mesen_allzeros(argv[2],20000u,&zero);
  if(strict==GK83_INVALID||strict==GK83_SOURCE_REFUSED||
     mesen==GK83_INVALID||mesen==GK83_SOURCE_REFUSED){
   fputs("GK207 refused unauthenticated original stock ROM or trace failure\n",stderr);
   return 8;
  }
  const Gk83Trace *trace[2]={&unknown,&zero};
  const Gk83Stop stop[2]={strict,mesen};
  const char *labels[2]={"UNKNOWN_RAM_POWER_ON","DOCUMENTED_MESEN_ALLZEROS_MODE"};
  for(unsigned k=0;k<2;k++){
   const Gk207SeedWatch *w=&trace[k]->source_seed_writes;
   printf("SOURCE_BOUNDED profile=%s gate=%u pc=$%04X completed_instructions=%u completed_seed_stores=%u written_mask=%02X full_seed_written=%d\n",
       labels[k],(unsigned)stop[k],trace[k]->stop_pc,trace[k]->instructions,
       w->completed_writes,w->written_mask,gk207_source_written_complete(w));
   for(unsigned i=0;i<w->completed_writes&&i<GK207_MAX_STORED_EVENTS;i++){
    const Gk207SeedEvent *e=&w->first[i];
    printf("  source pc=$%04X ram=$%04X byte_index=%u value=$%02X cycle=%llu mapper=%s\n",
      e->pc,e->cpu_address,e->byte_index,e->value,
      (unsigned long long)e->cpu_cycle,e->bank_known?"known":"unknown");
   }
  }
  puts("NOT STOCK MESEN EXECUTION: source-CPU bounded write provenance only. Unknown seed bytes remain UNKNOWN. Original first Spring is NOT generated.");
  return 0;
 }
 else if(argc==3&&strcmp(argv[1],"--audit-stock-selector-runtime")==0){
  Gk83Trace t={0};
  Gk83Stop status=gk83_trace_original_reset(argv[2],20000u,&t);
  if(status==GK83_INVALID||status==GK83_SOURCE_REFUSED){
   fputs("GK205 refused unauthenticated original stock source\n",stderr);return 1;
  }
  printf("SOURCE_BOUNDED original reset gate=%u pc=$%04X steps=%u; completed selector $0050 writes=%u, dropped=%u; unexecuted BRK gates=%u\n",
   status,t.stop_pc,t.instructions,t.selector_runtime.completed_selector_writes,
   t.selector_runtime.dropped_event_details,t.selector_runtime.brk_gates);
  for(unsigned i=0;i<t.selector_runtime.completed_selector_writes&&i<GK205_STORED_EVENTS;i++){
   const Gk205Store *e=&t.selector_runtime.first[i];
   printf("  STORE pc=$%04X cpu=$%04X selector=%u cycle=%llu bank=%s\n",
    e->pc,e->address,e->selector_value,(unsigned long long)e->bus_cpu_cycle,
    e->bank_known?"known":"unknown");
  }
  if(t.selector_runtime.brk_gates)printf("  BRK GATE (NOT EXECUTED): pc=$%04X selector=%s\n",
   t.selector_runtime.brk_pc,t.selector_runtime.brk_selector_known?"known":"unknown");
  puts("NOT MESEN RUNTIME. Stops at first unmodelled event; cannot infer Spring RNG or first playable season.");
  return 0;
 }
 else if(argc==3&&strcmp(argv[1],"--audit-stock-selector-store-candidates")==0){
  Gk204Inventory p={0};
  if(gk204_inventory_file(argv[2],&p)!=GK204_OK){
   fputs("GK204 rejected unauthenticated original stock input\n",stderr);return 1;
  }
  printf("VERIFIED_STATIC source-only raw selector $0050 candidates=%u direct_store=%u rmw=%u indexed=%u absolute=%u\n",
   p.site_count,p.raw_direct_store_count,p.raw_direct_rmw_count,p.raw_indexed_count,p.raw_absolute_count);
  printf("  adjacent immediate load of actual RNG decimal selector %u: %u candidates\n",
   p.rng_selector,p.adjacent_lda_rng_store_count);
  for(unsigned i=0;i<p.site_count;i++){
   const Gk204Site *site=&p.sites[i];
   printf("  bank=%u offset=$%04X hypothetical_pc=$%04X opcode=$%02X raw_kind=%u%s\n",
    site->bank,site->bank_offset,site->hypothetical_pc,site->opcode,site->kind,
    site->adjacent_lda_immediate?" [adjacent LDA immediate]":"");
  }
  puts("SOURCE BYTE PATTERNS MAY BE DATA. NO MAPPER / REACHABILITY / ACTUAL BRK EXECUTION OR FIRST SPRING PROVEN.");
  return 0;
 }
 else if(argc==3&&strcmp(argv[1],"--audit-stock-rng-selector-writers")==0){
  Gk203Proof p={0};
  if(gk203_from_file(argv[2],&p)!=GK203_OK){
   fputs("GK203 source mismatch / invalid private ROM\n",stderr);return 1;
  }
  printf("VERIFIED_STATIC original IRQ RNG slot=%u, fixed trampoline sites=%u, confirmed fixed RNG selectors=%u\n",
   p.rng_slot,p.fixed_caller_count,p.fixed_rng_caller_count);
  for(unsigned i=0;i<p.fixed_caller_count;i++)
   printf("  source $%04X selector decimal%u -> dispatch $%04X, subcommand %u (NOT RNG)\n",
    p.fixed[i].pc,p.fixed[i].selector,p.fixed[i].dispatched_target,p.fixed[i].subcommand);
  printf("  raw immediate RNG-slot writes=%u (not a CPU reachability proof)\n",p.raw_immediate_rng_slot_writes);
  puts("NOT OBSERVED RUNTIME; indirect/dynamic writers, first Spring and 14/27 RNG progression unresolved.");
  return 0;
 }
 else if(argc==3&&strcmp(argv[1],"--audit-stock-brk-rng-dispatch")==0){
  Gk202Proof proof={0};
  Gk202Status status=gk202_analyze_authenticated_file(argv[2],&proof);
  if(status!=GK202_OK){fprintf(stderr,"GK202 source rejected: %d\n",(int)status);return 1;}
  printf("VERIFIED_STATIC original IRQ/BRK vector=$%04X, BRK-bit=$%02X, "
         "selector RAM=$%04X, 3-byte table=$%04X entries=%u\n",
         proof.irq_brk_vector,proof.stack_brk_bit_mask,
         proof.command_selector_ram,proof.table_first_pc,proof.table_entries);
  printf("RNG via conditional BRK selector=%u -> JMP at $%04X -> $%04X; "
         "raw direct JSR=%u, raw direct JMP=%u\n",proof.rng_slot,
         proof.rng_entry_pc,proof.rng_target_pc,
         proof.direct_jsr_rng_raw_sequences,proof.direct_jmp_rng_raw_sequences);
  puts("NOT OBSERVED RUNTIME: no source proof that setup selected this slot; "
       "no original first Spring, seasonal RNG history or merchant availability generated.");
  return 0;
 }
 else if(argc==3&&strcmp(argv[1],"--audit-original-rng-calls")==0){
  /* Static source audit only, never a playable original Spring or NES run.
   * The ROM must authenticate to the exact privately supplied SHA256.
   * All 15 lower-bank mappings are explicit alternatives, not chronology. */
  for(uint8_t b=0;b<15u;b++){
   Gk181Map *m=calloc(1,sizeof *m);
   if(!m)return 200;
   Gk181Result r=gk181_scan_authenticated(argv[2],b,m);
   if(r!=GK181_OK){free(m);return 201;}
   printf("VERIFIED_STATIC hypothetical_low_bank=%u decoded_calls_to_C289=%u fixed=%u low=%u mapper_barriers=%u\n",
      (unsigned)b,(unsigned)m->rng_call_count,(unsigned)m->fixed_rng_call_count,
      (unsigned)m->low_rng_call_count,(unsigned)m->mapper_barriers);
   for(uint16_t i=0;i<m->rng_call_count;i++)
    printf("  bank=%u PC=$%04X roots_mask=%u -> JSR $C289 (NOT OBSERVED EXECUTION)\n",
      (unsigned)m->rng_calls[i].bank,(unsigned)m->rng_calls[i].pc,(unsigned)m->rng_calls[i].roots);
   free(m);
  }
  return 0;
 }
 else if(argc!=1){fprintf(stderr,"Usage: gk1_native_shell [--smoke-frames 1..120 | --smoke-snapshot 1..120 | --smoke-path 1..120 | --path-export path scenario 1/2/b-script | --path-import path | --cold-export path case events | --cold-import path]\n");return 4;}
 gk1_app_init(&app);gk1_pad_init(&pad);load_optional_battle_view();load_optional_mesen_map();load_optional_spring_archive();load_optional_stock_spring();(void)load_scenario_data();
 /* EXEC0245: actual interactive Linux startup now honors the SAME
  * authenticated owner-private source environment as WinMain. A CLI
  * test/diagnostic never reads it. No fake default source is created. */
 if(argc==1){
  Gk245Outcome owner=gk245_owner_source_startup(&app,scenario_csv_path(),
    scenario_loaded,0,getenv("GK1_OWNER_STOCK_FRESH_JSON"),
    getenv("GK1_OWNER_STOCK_WORLD_MSS"));
  gk245_report_owner_status(owner);
 }
 if((argc==3&&strcmp(argv[1],"--play-original-auto")==0)||
    (argc==4&&strcmp(argv[1],"--smoke-original-auto")==0)||
    smoke_classic_storage_original||smoke_classic_view_original||smoke_classic_trade_original||smoke_classic_tax_original||smoke_classic_three_original){
  Gk77Profile detected=GK77_FRESH_MONGOL_WORKRAM;
  app.rules_profile=0u;
  Gk193Result joined=scenario_loaded?
    gk193_enter_original_auto(&app,scenario_csv_path(),argv[2],0u,&detected):GK193_TEMPLATE;
  if(joined!=GK193_OK){
   fprintf(stderr,"GK193 source-authenticated private Mesen bank auto-entry refused: %u; no world mutation\n",(unsigned)joined);
   return joined==GK193_UNAVAILABLE?17:21;
  }
  if(app.screen!=GK1_SCREEN_STOCK_CONTINUE||!app.stock_checkpoint.valid||
     !app.stock_checkpoint.source_verified||app.stock_checkpoint.orders.remaining!=3u){
    fprintf(stderr,"GK193 invalid original-stock continuing world\n");return 22;
  }
  printf("TESTED_PORT_LINUX GK193 verified private Mesen %s; actual original NES first-Spring initialization and commands still OPEN\n",gk77_profile_name(detected));
  if(smoke_classic_three_original){
   /* EXEC0248: actual current inherited X11 app (not an isolated model).
    * Exercise three manual-derived native Classic controller commands with
    * EACH source-authenticated original private first Spring. This does NOT
    * generate the NES's original first Spring, taxes or seasonal AI.
    * Crucially: the exact 14/27-country source RAM must remain immutable. */
   const Gk79Checkpoint original=app.stock_checkpoint;
   const unsigned original_tax=original.world.countries[0].tax;
   if(original_tax<1u||original_tax>98u)return 291;
   pad_test_press(GK1_PAD_MENU);
   if(app.screen!=GK1_SCREEN_STOCK_COMMANDS||app.stock_command_cursor!=0u)return 292;
   pad_test_press(GK1_PAD_A); /* TAX */
   if(app.screen!=GK1_SCREEN_CLASSIC_TAX)return 293;
   pad_test_press(GK1_PAD_UP);pad_test_press(GK1_PAD_A);
   if(app.classic_assign.action_count!=1u||app.classic_assign.budget.remaining!=2u)return 294;
   pad_test_press(GK1_PAD_B);
   pad_test_press(GK1_PAD_DOWN);pad_test_press(GK1_PAD_A); /* ASSIGN */
   if(app.screen!=GK1_SCREEN_CLASSIC_ASSIGN)return 295;
   pad_test_press(GK1_PAD_A);
   if(app.classic_assign.action_count!=2u||app.classic_assign.budget.remaining!=1u)return 296;
   pad_test_press(GK1_PAD_B);
   pad_test_press(GK1_PAD_UP);pad_test_press(GK1_PAD_A); /* TAX again */
   if(app.screen!=GK1_SCREEN_CLASSIC_TAX)return 297;
   pad_test_press(GK1_PAD_UP);pad_test_press(GK1_PAD_A);
   if(app.classic_assign.action_count!=3u||app.classic_assign.budget.remaining!=0u||
      app.classic_assign.mutable_world.countries[0].tax!=original_tax+2u||
      !gk187_intact(&app.classic_assign,&app.stock_checkpoint))return 298;
   const Gk187ClassicAssign completed=app.classic_assign;
   pad_test_press(GK1_PAD_UP);pad_test_press(GK1_PAD_A); /* fourth rejected */
   if(memcmp(&completed,&app.classic_assign,sizeof completed)!=0)return 299;
   if(!gk85_raw_source_intact(&app.stock_checkpoint)||
      !gk88_stock_projection_intact(&app.stock_checkpoint)||
      original.orders.remaining!=app.stock_checkpoint.orders.remaining||
      memcmp(original.stock_raw,app.stock_checkpoint.stock_raw,sizeof original.stock_raw))return 300;
   if(smoke_classic_three_cold_original){
    /* EXEC0250: the REAL native-controller path above has performed three
     * source-backed PORT orders. Save a private GC19, recreate a pristine
     * application from the installed CSV (not by copying the live app),
     * cold-load it, and verify full mutable country/action/budget equality.
     * Intentionally corrupt the resulting private file: loader MUST refuse
     * without altering the previously restored application. The temporary
     * private source is deleted; no NES records or hashes are printed. */
    char private_path[1024];
#ifdef _WIN32
    char temp_dir[MAX_PATH],temp_name[MAX_PATH];
    DWORD temp_n=GetTempPathA(MAX_PATH,temp_dir);
    if(!temp_n||temp_n>=MAX_PATH||!GetTempFileNameA(temp_dir,"g50",0,temp_name))return 301;
    if(strlen(temp_name)>=sizeof private_path||!DeleteFileA(temp_name))return 302;
    strcpy(private_path,temp_name);
#else
    strcpy(private_path,"/tmp/gk250-gc19-XXXXXX");
    int private_fd=mkstemp(private_path);
    if(private_fd<0)return 301;
    if(close(private_fd)!=0||unlink(private_path)!=0)return 302;
#endif
    Gk1TemplateDataset *templates=malloc(sizeof *templates);
    Gk1App *restored=malloc(sizeof *restored),*snapshot=malloc(sizeof *snapshot);
    if(!templates||!restored||!snapshot){free(templates);free(restored);free(snapshot);return 303;}
    gk1_app_init(restored);
    int good=(gk1_template_load(scenario_csv_path(),templates)==GK1_LOAD_OK &&
       gk1_app_install_templates(restored,templates)!=0 &&
       gk189_save_new(private_path,&app)==GK189_OK &&
       gk189_cold_load(private_path,scenario_csv_path(),restored)==GK189_OK &&
       restored->screen==GK1_SCREEN_STOCK_COMMANDS &&
       restored->stock_command_cursor==6u &&
       restored->stock_checkpoint.pending_pass==0u &&
       restored->classic_assign.action_count==3u &&
       restored->classic_assign.budget.remaining==0u &&
       restored->classic_assign.mutable_world.countries[0].tax==original_tax+2u &&
       gk187_intact(&restored->classic_assign,&restored->stock_checkpoint) &&
       memcmp(&restored->classic_assign,&app.classic_assign,sizeof completed)==0 &&
       memcmp(restored->stock_checkpoint.stock_raw,original.stock_raw,
              sizeof original.stock_raw)==0);
    if(good){
      /* Exercise the actual post-cold-load controller menu.  A starts on
       * the zero-cost VIEW command; it does not replay an exhausted Assign
       * dialog, refill orders, or mutate an original NES country record. */
      const Gk187ClassicAssign before_view=restored->classic_assign;
      good=(gk1_app_input(restored,GK1_INPUT_CONFIRM)==1 &&
        restored->screen==GK1_SCREEN_STOCK_CONTINUE &&
        !memcmp(&before_view,&restored->classic_assign,sizeof before_view) &&
        gk187_intact(&restored->classic_assign,&restored->stock_checkpoint));
    }
    if(good){
      *snapshot=*restored;
      FILE *f=fopen(private_path,"r+b");
      if(!f||fseek(f,100,SEEK_SET)!=0)good=0;
      else{
       int byte=fgetc(f);
       if(byte==EOF||fseek(f,100,SEEK_SET)!=0||
          fputc(byte^0x01,f)==EOF||fflush(f)!=0)good=0;
      }
      if(f&&fclose(f)!=0)good=0;
      if(good && (gk189_cold_load(private_path,scenario_csv_path(),restored)!=GK189_CORRUPT ||
          memcmp(restored,snapshot,sizeof *restored)!=0))good=0;
    }
    (void)remove(private_path);
    memset(templates,0,sizeof *templates);memset(restored,0,sizeof *restored);
    memset(snapshot,0,sizeof *snapshot);
    free(templates);free(restored);free(snapshot);
    if(!good)return 304;
    printf("TESTED_PORT_NATIVE EXEC0250 %s authenticated original checkpoint -> actual native 3-controller-order Tax/Assign/Tax -> private GC19 -> fresh-process-equivalent template reset + exact source/action cold-resume -> corrupt-file atomic rollback PASS; NOT NES PARITY, NOT PHYSICAL XBOX\n",gk77_profile_name(detected));
   }

   printf("TESTED_PORT_LINUX EXEC0248 %s 1280x720 X11 + INJECTED PAD THREE ORDERS TAX->ASSIGN->TAX: 3/3, FOURTH REFUSED, ORIGINAL 14/27 COUNTRIES IMMUTABLE; NO ORIGINAL NES RNG/PARITY OR PHYSICAL XBOX\n",gk77_profile_name(detected));
  }
  if(smoke_classic_tax_original){
   /* 197: true native controller dispatcher over authenticated private
    * source state; rate update is MANUAL_DERIVED, NOT observed NES parity. */
   Gk79Checkpoint original=app.stock_checkpoint;
   pad_test_press(GK1_PAD_MENU);
   if(app.screen!=GK1_SCREEN_STOCK_COMMANDS||app.stock_command_cursor!=0u)return 281;
   pad_test_press(GK1_PAD_A);
   if(app.screen!=GK1_SCREEN_CLASSIC_TAX)return 282;
   uint8_t old=app.classic_tax_selection;
   pad_test_press(GK1_PAD_UP);
   if(app.classic_tax_selection!=(uint8_t)(old+1u))return 283;
   pad_test_press(GK1_PAD_A);
   if(app.classic_assign.budget.remaining!=2u||
      app.classic_assign.action_count!=1u||
      !gk187_intact(&app.classic_assign,&app.stock_checkpoint)||
      app.classic_assign.mutable_world.countries[original.player_index].tax!=old+1u)return 284;
   if(app.stock_checkpoint.orders.remaining!=3u||
      memcmp(original.stock_raw,app.stock_checkpoint.stock_raw,sizeof original.stock_raw)!=0)return 285;
   pad_test_press(GK1_PAD_Y);
   if(app.classic_assign.budget.remaining!=3u ||
      app.classic_assign.action_count!=0u ||
      app.classic_tax_selection!=old ||
      !gk187_intact(&app.classic_assign,&app.stock_checkpoint))return 287;
   pad_test_press(GK1_PAD_B);
   if(app.screen!=GK1_SCREEN_STOCK_COMMANDS)return 286;
   printf("TESTED_PORT_LINUX EXEC0199 %s PAD TAX %u->%u; derived orders reset to 3/source orders 3; no original NES cost/RNG parity; no physical Xbox\n",
     gk77_profile_name(detected),(unsigned)old,(unsigned)(old+1u));
  }
  if(smoke_classic_trade_original){
   /* Software-injected real native pad routes: original source Spring ->
    * Classic ASSIGN -> read-only Trade. No merchant is invented. */
   Gk79Checkpoint original=app.stock_checkpoint;
   pad_test_press(GK1_PAD_MENU);
   if(app.screen!=GK1_SCREEN_STOCK_COMMANDS)return 271;
   pad_test_press(GK1_PAD_DOWN);pad_test_press(GK1_PAD_A);
   if(app.screen!=GK1_SCREEN_CLASSIC_ASSIGN)return 272;
   pad_test_press(GK1_PAD_A);
   Gk196TradeStatus derived={0};
   if(gk196_classic_trade_status(&app,&derived)!=GK196_OK||
      derived.remaining!=2u||derived.actions!=1u||!derived.derived)return 273;
   pad_test_press(GK1_PAD_B);
   if(app.screen!=GK1_SCREEN_STOCK_COMMANDS)return 274;
   for(unsigned j=0;j<3u;j++)pad_test_press(GK1_PAD_DOWN);
   if(app.stock_command_cursor!=4u)return 275;
   pad_test_press(GK1_PAD_A);
   if(app.screen!=GK1_SCREEN_STOCK_TRADE_REFERENCE)return 276;
   pad_test_press(GK1_PAD_RIGHT);pad_test_press(GK1_PAD_DOWN);
   pad_test_press(GK1_PAD_A);
   if(gk196_classic_trade_status(&app,&derived)!=GK196_OK||
      derived.remaining!=2u||derived.actions!=1u||
      app.stock_checkpoint.orders.remaining!=3u)return 277;
   if(memcmp(original.stock_raw,app.stock_checkpoint.stock_raw,
             sizeof original.stock_raw)!=0)return 278;
   printf("TESTED_PORT_LINUX EXEC0196 CLASSIC %s PAD ASSIGN -> TRADE QUOTE DERIVED 2 ORDERS / SOURCE 3; NO TRANSACTION; ORIGINAL RECORD IMMUTABLE; NOT Mesen or Xbox\n",gk77_profile_name(detected));
  }
  if(smoke_classic_view_original){
   /* Same native pad event route as a real controller, but injected via
    * QA software. Verify actual original source remains byte-identical. */
   Gk79Checkpoint original=app.stock_checkpoint;
   pad_test_press(GK1_PAD_MENU);
   if(app.screen!=GK1_SCREEN_STOCK_COMMANDS)return 261;
   pad_test_press(GK1_PAD_DOWN);pad_test_press(GK1_PAD_A);
   if(app.screen!=GK1_SCREEN_CLASSIC_ASSIGN)return 262;
   pad_test_press(GK1_PAD_A);
   if(app.classic_assign.action_count!=1u||
      app.classic_assign.budget.remaining!=2u)return 263;
   pad_test_press(GK1_PAD_B);
   for(unsigned j=0;j<5u;j++)pad_test_press(GK1_PAD_DOWN);
   if(app.stock_command_cursor!=6u)return 264;
   pad_test_press(GK1_PAD_A);
   if(app.screen!=GK1_SCREEN_STOCK_CONTINUE||
      app.stock_checkpoint.view_index!=app.stock_checkpoint.player_index)return 265;
   Gk195CountryView view={0};
   if(gk195_classic_country_view(&app,app.stock_checkpoint.view_index,&view)!=GK195_OK||
      !view.derived||view.actions!=1u||view.remaining!=2u||
      view.current.troops+1u!=view.original.troops||
      view.current.food_workers!=view.original.food_workers+1u)return 266;
   pad_test_press(GK1_PAD_A);
   if(!app.stock_checkpoint.pending_pass||app.classic_assign.budget.remaining!=2u)return 267;
   pad_test_press(GK1_PAD_A);
   if(app.stock_checkpoint.pending_pass||!app.classic_assign.passed||
      app.classic_assign.budget.remaining||app.stock_checkpoint.orders.remaining!=3u)return 268;
   if(memcmp(original.stock_raw,app.stock_checkpoint.stock_raw,sizeof original.stock_raw))return 269;
   printf("TESTED_PORT_LINUX EXEC0195 CLASSIC %s PAD ASSIGN -> DERIVED VIEW -> LIVE-BUDGET PASS PASS; STOCK ORIGINAL IMMUTABLE; NOT Mesen or physical Xbox\n",gk77_profile_name(detected));
  }
  if(smoke_classic_storage_original){
   /* EXEC0194: the integrated native pad adapter, actual OTHER confirm and
    * one-shot disk I/O, and title cold resume. Three private owner-supplied
    * Mesen profiles are individually tested, without copying their bytes. */
   const char *path=getenv("GK1_CLASSIC_COLD_SAVE_FILE");
   if(!path||!*path)return 241;
   campaign_slot=1u;
   pad_test_press(GK1_PAD_MENU);
   if(app.screen!=GK1_SCREEN_STOCK_COMMANDS)return 242;
   pad_test_press(GK1_PAD_DOWN);
   if(app.stock_command_cursor!=1u)return 243;
   pad_test_press(GK1_PAD_A);
   if(app.screen!=GK1_SCREEN_CLASSIC_ASSIGN)return 244;
   pad_test_press(GK1_PAD_A);
   if(app.classic_assign.action_count!=1u||
      app.classic_assign.budget.remaining!=2u||
      !gk187_intact(&app.classic_assign,&app.stock_checkpoint))return 245;
   pad_test_press(GK1_PAD_B);
   if(app.screen!=GK1_SCREEN_STOCK_COMMANDS)return 246;
   pad_test_press(GK1_PAD_RIGHT);
   for(unsigned j=0;j<5u;j++)pad_test_press(GK1_PAD_DOWN);
   if(app.stock_command_cursor!=13u)return 247;
   pad_test_press(GK1_PAD_A);
   if(app.screen!=GK1_SCREEN_STOCK_STORAGE)return 248;
   pad_test_press(GK1_PAD_A);
   if(app.stock_storage_confirm!=1u)return 249;
   pad_test_press(GK1_PAD_A);
   if(app.stock_storage_request||strstr(app.status,"SHA-VERIFIED OK")==NULL)return 250;
   char actual_path[1100];
   if(!classic_cold_path(actual_path,sizeof actual_path))return 251;
   FILE *saved=fopen(actual_path,"rb");
   if(!saved)return 252;
   char signature[4];
   if(fread(signature,1,4,saved)!=4||memcmp(signature,"GC19",4)!=0||fclose(saved)!=0)return 253;
   gk1_app_init(&app);gk1_pad_init(&pad);
   if(!load_scenario_data()||app.screen!=GK1_SCREEN_TITLE)return 254;
   pad_test_press(GK1_PAD_Y);
   if(app.screen!=GK1_SCREEN_CLASSIC_ASSIGN||
      !app.stock_checkpoint.source_verified||
      app.classic_assign.action_count!=1u||
      app.classic_assign.budget.remaining!=2u||
      !gk187_intact(&app.classic_assign,&app.stock_checkpoint))return 255;
   printf("TESTED_PORT_LINUX EXEC0194 ORIGINAL %s: SIMULATED PAD ASSIGN -> OTHER TWO CONFIRMS -> GC19 SAVE -> TITLE Y COLD RESUME PASS; NOT physical Xbox or NES first Spring\n",gk77_profile_name(detected));
  }
 }
 if((argc==4&&strcmp(argv[1],"--play-original-mss")==0)||
    (argc==5&&strcmp(argv[1],"--smoke-original-mss")==0)){
  Gk77Profile requested=gk77_profile_parse(argv[3]);
  app.rules_profile=0u; /* Original-stock CLI entry is Classic, never Enhanced. */
  Gk164Result joined=scenario_loaded?
    gk164_enter_original_mss(&app,scenario_csv_path(),argv[2],requested,0u):GK164_TEMPLATE;
  if(joined!=GK164_OK){
   fprintf(stderr,"GK164 private original archived Mesen entry refused: %u (no state transition)\n",(unsigned)joined);
   return joined==GK164_SOURCE_UNAVAILABLE?17:21;
  }
  if(!app.stock_checkpoint.valid||!app.stock_checkpoint.source_verified||
     app.stock_checkpoint.orders.remaining!=3u||
     app.screen!=GK1_SCREEN_STOCK_CONTINUE){
    fprintf(stderr,"GK164 hostile QA: invalid original-stock entry after validation\n");return 22;
  }
  /* All original country bytes remain in memory; no temp/source file ever
   * exists. An original ROM setup simulation is NOT claimed. */
 }


 if(play_fresh_json){
  app.rules_profile=0u; /* Source-attested original RAM => Classic controls. */
  Gk166Result joined=scenario_loaded?
   gk166_enter_original_fresh_json(&app,scenario_csv_path(),argv[2],0u):GK166_TEMPLATE;
  if(joined!=GK166_OK){
   fprintf(stderr,"GK166 owner-private fresh WorkRAM entry refused: %u\n",(unsigned)joined);
   return joined==GK166_SOURCE_UNAVAILABLE?23:24;
  }
  if(app.screen!=GK1_SCREEN_STOCK_CONTINUE||!app.stock_checkpoint.source_verified||
     app.stock_checkpoint.orders.remaining!=3u){
   fprintf(stderr,"GK166 invalid original-stock checkpoint after entry\n");return 25;
  }
  if(smoke_private_controller){
   /* Independent simulated controller pipeline, including reset-to-title.
    * Never claim this tests a physical Xbox controller or original Mesen.
    * This test-only path uses a PRIVATE owner-supplied JSON path. */
   app.rules_profile=0u;
#ifdef _WIN32
   if(_putenv_s("GK1_PRIVATE_STOCK_SAVE_FILE",argv[3])!=0)return 201;
#else
   if(setenv("GK1_PRIVATE_STOCK_SAVE_FILE",argv[3],1)!=0)return 201;
#endif
   campaign_slot=1u;
   pad_test_press(GK1_PAD_X);
   if(strstr(app.status,"SHA-VERIFIED OK")==NULL)return 202;
   gk1_app_init(&app);gk1_pad_init(&pad);
   if(!load_scenario_data()||app.screen!=GK1_SCREEN_TITLE)return 203;
   pad_test_press(GK1_PAD_Y);
   if(app.screen!=GK1_SCREEN_STOCK_CONTINUE||
      app.rules_profile!=0u||!app.stock_checkpoint.source_verified||
      app.stock_checkpoint.orders.remaining!=3u||
      app.stock_checkpoint.world.countries[0].gold!=430u||
      app.stock_checkpoint.rate!=121u)return 204;
   puts("GK167 NATIVE PRIVATE X SAVE + TITLE Y COLD RESUME PASS; synthetic controller only; original first Spring not generated");
   /* EXEC0169: controller-facing OTHER menu -> confirmed Save New on the
    * second private slot -> confirmed cold restore. Never publish these
    * original-record bytes or mislabel this as NES controller parity. */
   pad_test_press(GK1_PAD_MENU);
   if(app.screen!=GK1_SCREEN_STOCK_COMMANDS)return 205;
   pad_test_press(GK1_PAD_RIGHT);
   for(unsigned j=0;j<6;j++)pad_test_press(GK1_PAD_DOWN);
   if(app.stock_command_cursor!=13u)return 206;
   pad_test_press(GK1_PAD_A);
   if(app.screen!=GK1_SCREEN_STOCK_STORAGE)return 207;
   pad_test_press(GK1_PAD_RB);
   if(campaign_slot!=2u)return 208;
   pad_test_press(GK1_PAD_A);
   if(!app.stock_storage_confirm)return 209;
   pad_test_press(GK1_PAD_A);
   if(app.stock_storage_request||strstr(app.status,"SHA-VERIFIED OK")==NULL)return 210;
   pad_test_press(GK1_PAD_DOWN);
   if(app.stock_storage_cursor!=1u)return 211;
   pad_test_press(GK1_PAD_A);pad_test_press(GK1_PAD_A);
   if(app.stock_storage_request||app.screen!=GK1_SCREEN_STOCK_CONTINUE||
      strstr(app.status,"SHA-VERIFIED OK")==NULL||
      app.stock_checkpoint.orders.remaining!=3u||app.stock_checkpoint.rate!=121u)
     return 212;
   puts("GK169 NATIVE SIMULATED PAD OTHER -> PRIVATE SLOT2 SAVE -> COLD RESTORE PASS; original NES commands not implemented");
  }
 }
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
        if(app.screen!=GK1_SCREEN_STOCK_COMMANDS)return 32;
        input(GK1_INPUT_SETUP);
        if((strstr(app.status,"STOCK RESET $C000:")==NULL && strstr(app.status,"STOCK ROM VERIFIED: 6502")==NULL && strstr(app.status,"AUTHENTIC ROM: PPU WARMUP")==NULL) ||
           memcmp(&before,&app.stock_checkpoint,sizeof before)!=0 ||
           app.worlds[0].phase!=GK1_PRE_SETUP_TEMPLATE)return 31;
        input(GK1_INPUT_CANCEL);
        if(app.screen!=GK1_SCREEN_STOCK_CONTINUE)return 33;
      }
      if(show_stock_finance||show_stock_setup||show_stock_goods){
        const Gk79Checkpoint before=app.stock_checkpoint;
        input(GK1_INPUT_LABOR_PLAN);if(app.stock_checkpoint.raw_view!=1)return 51;
        input(GK1_INPUT_LABOR_PLAN);
        if(app.stock_checkpoint.raw_view!=0||app.stock_reconcile_view!=1)return 52;
        Gk1LoadedScenario baseline;Gk89SpringReconciliation row;
        if(!scenario_original||gk1_open_loaded_scenario(scenario_original,
              app.stock_checkpoint.world.scenario_id,&baseline)!=GK1_LOAD_OK)return 53;
        if(gk90_compare_spring_verified(scenario_csv_path(),&baseline,&app.stock_checkpoint,0,&row)!=GK90_OK)return 54;
        if(before.orders.remaining!=app.stock_checkpoint.orders.remaining||
           memcmp(before.stock_raw,app.stock_checkpoint.stock_raw,sizeof before.stock_raw)!=0)return 55;
        if(show_stock_setup||show_stock_goods){
          input(GK1_INPUT_LABOR_PLAN);
          if(app.stock_reconcile_view!=2 || app.stock_checkpoint.raw_view)return 81;
          Gk112StockDeltas crosswalk;
          if(gk1_app_stock_setup_deltas(&app,scenario_csv_path(),&crosswalk)!=GK112_OK)return 82;
          if(crosswalk.country_count!=app.stock_checkpoint.world.country_count ||
             crosswalk.setup_resolved || crosswalk.first_spring_ready || crosswalk.orders_granted)return 83;
          input(GK1_INPUT_DOWN);
          if(app.stock_checkpoint.view_index!=1)return 84;
          input(GK1_INPUT_LABOR_PLAN);
          if(app.stock_reconcile_view!=3||app.stock_checkpoint.raw_view)return 87;
          for(unsigned country=0;country<app.stock_checkpoint.world.country_count;country++){
           Gk147StockGoods goods={0};
           if(gk1_app_source_goods_view(&app,country,&goods)!=GK147_GOODS_OK)return 88;
           if(goods.country_index!=country||goods.country_count!=before.world.country_count)return 89;
           for(unsigned i=0;i<10u;i++){
            const uint8_t *raw=before.stock_raw[country]+0x22u+2u*i;
            uint16_t expected=(uint16_t)((unsigned)raw[0]|((unsigned)raw[1]<<8u));
            if(goods.goods[i]!=expected)return 90;
           }
          }
          if(app.stock_checkpoint.orders.remaining!=3u)return 91;
          if(show_stock_setup){
           input(GK1_INPUT_LABOR_PLAN);
           if(app.stock_reconcile_view!=0||app.stock_checkpoint.raw_view)return 85;
          }
          if(memcmp(&before.world,&app.stock_checkpoint.world,sizeof before.world)!=0||
             memcmp(before.stock_raw,app.stock_checkpoint.stock_raw,sizeof before.stock_raw)!=0||
             memcmp(&before.orders,&app.stock_checkpoint.orders,sizeof before.orders)!=0||
             memcmp(before.raw_sha256,app.stock_checkpoint.raw_sha256,32)!=0)return 86;
          fprintf(stdout,"GK147 native controller original stock goods viewer PASS, %u authentic countries, 10 specialties each, no orders/state changed; existing GK112 setup delta PASS\n",
           (unsigned)crosswalk.country_count);
        }
        fprintf(stdout,"GK90 source-bound pre-setup finance-only residual: Food %+ld Gold %+ld Troops %+ld; no game state changed\n",
          (long)row.residual_food,(long)row.residual_gold,(long)row.residual_troops);
      }else if(show_stock_trade){
        /* Real full X11 render of authenticated stock Rate quotes. Source
         * market offer generation still unavailable, NO purchase performed. */
        const Gk79Checkpoint before=app.stock_checkpoint;
        input(GK1_INPUT_SETUP);
        if(app.screen!=GK1_SCREEN_STOCK_COMMANDS)return 71;
        for(int j=0;j<4;j++)input(GK1_INPUT_DOWN);
        input(GK1_INPUT_CONFIRM);
        if(app.screen!=GK1_SCREEN_STOCK_TRADE_REFERENCE)return 72;
        input(GK1_INPUT_RIGHT);input(GK1_INPUT_DOWN);input(GK1_INPUT_CONFIRM);
        Gk61Quote check={0};
        if(gk95_stock_trade_reference(&app.stock_checkpoint,GK61_ISLAM,1,&check)!=GK95_QUOTE_OK)return 73;
        if(memcmp(&before,&app.stock_checkpoint,sizeof before)!=0)return 74;
       }else if(show_enhanced_summer||show_enhanced_fall||show_enhanced_winter||show_enhanced_year2){
        /* Native 1280x720 X11 and actual controller-input pipeline,
         * PRIVATELY authenticated original stock source required. Not Mesen. */
        const Gk79Checkpoint prior=app.stock_checkpoint;
        app.rules_profile=1u;
        input(GK1_INPUT_SETUP);if(app.screen!=GK1_SCREEN_STOCK_COMMANDS)return 111;
        input(GK1_INPUT_DOWN);if(app.stock_command_cursor!=1)return 112;
        input(GK1_INPUT_CONFIRM);if(app.screen!=GK1_SCREEN_ENHANCED_ASSIGN)return 113;
        input(GK1_INPUT_SETUP);if(app.assign_quantity!=5u)return 114;
        input(GK1_INPUT_CONFIRM);
        if(app.enhanced_assign.action_count!=1u)return 115;
        input(GK1_INPUT_CANCEL);
        for(int j=0;j<4;j++)input(GK1_INPUT_DOWN);
        input(GK1_INPUT_RIGHT);
        if(app.stock_command_cursor!=12u)return 116;
        input(GK1_INPUT_CONFIRM);input(GK1_INPUT_CONFIRM);
        if(!app.enhanced_assign.budget.ended||app.enhanced_assign.budget.remaining)return 117;
        input(GK1_INPUT_CONFIRM);if(app.screen!=GK1_SCREEN_ENHANCED_SUMMER)return 118;
        input(GK1_INPUT_SETUP);input(GK1_INPUT_CONFIRM);
        if(app.enhanced_summer.action_count!=1u ||
           !gk158_attestation_intact(&app.enhanced_summer,&app.enhanced_assign,&prior))return 119;
        if(memcmp(&prior,&app.stock_checkpoint,sizeof prior)!=0)return 120;
        if(show_enhanced_fall||show_enhanced_winter||show_enhanced_year2){
         input(GK1_INPUT_CANCEL);if(app.screen!=GK1_SCREEN_STOCK_COMMANDS)return 121;
         input(GK1_INPUT_CONFIRM);input(GK1_INPUT_CONFIRM);
         if(!app.enhanced_summer.budget.ended)return 122;
         input(GK1_INPUT_CONFIRM);
         if(app.screen!=GK1_SCREEN_ENHANCED_FALL||
            !gk159_attestation_intact(&app.enhanced_fall,&app.enhanced_assign,
              &app.enhanced_summer,&prior))return 123;
         input(GK1_INPUT_SETUP);input(GK1_INPUT_CONFIRM);
         if(app.enhanced_fall.action_count!=1u||
            app.enhanced_fall.budget.remaining!=2u)return 124;
         if(memcmp(&prior,&app.stock_checkpoint,sizeof prior)!=0)return 125;
         if(show_enhanced_winter||show_enhanced_year2){
          input(GK1_INPUT_CANCEL);if(app.screen!=GK1_SCREEN_STOCK_COMMANDS)return 126;
          input(GK1_INPUT_CONFIRM);input(GK1_INPUT_CONFIRM);
          if(!app.enhanced_fall.budget.ended)return 127;
          input(GK1_INPUT_CONFIRM);
          if(app.screen!=GK1_SCREEN_ENHANCED_WINTER||
             !gk160_attestation_intact(&app.enhanced_winter,&app.enhanced_assign,
              &app.enhanced_summer,&app.enhanced_fall,&prior))return 128;
          input(GK1_INPUT_SETUP);input(GK1_INPUT_CONFIRM);
          if(app.enhanced_winter.action_count!=1u||app.enhanced_winter.budget.remaining!=2u)return 129;
          if(memcmp(&prior,&app.stock_checkpoint,sizeof prior)!=0)return 130;
          if(show_enhanced_year2){
           input(GK1_INPUT_CANCEL);if(app.screen!=GK1_SCREEN_STOCK_COMMANDS)return 131;
           input(GK1_INPUT_CONFIRM);input(GK1_INPUT_CONFIRM);
           if(!app.enhanced_winter.budget.ended)return 132;
           input(GK1_INPUT_CONFIRM);
           if(app.screen!=GK1_SCREEN_ENHANCED_YEAR2||
              !gk161_attestation_intact(&app.enhanced_year2,&app.enhanced_assign,
               &app.enhanced_summer,&app.enhanced_fall,&app.enhanced_winter,&prior))return 133;
           input(GK1_INPUT_SETUP);input(GK1_INPUT_CONFIRM);
           if(app.enhanced_year2.action_count!=1u||app.enhanced_year2.budget.remaining!=2u||
              app.enhanced_year2.year!=app.enhanced_assign.year+1u)return 134;
           if(memcmp(&prior,&app.stock_checkpoint,sizeof prior)!=0)return 135;
           puts("GK161 authentic stock source -> Enhanced YEAR2 spring labor via controller/X11 PASS; NOT original NES annual processing");
          }else puts("GK160 original source -> Enhanced Spring/Summer/Fall/Winter real controller+X11 screen PASS, source immutable");
         }else puts("GK159 original checkpoint -> Enhanced Spring/Summer/Fall controller path PASS; source immutable");
        }else puts("GK158 authenticated Enhanced Spring-to-Summer native controller+X11 screen PASS; three new labor orders, original stock immutable");
       }else if(show_stock_commands){
        /* Exact private-stock authenticated native X11/controller integration.
         * SOURCE-GATED commands never mutate the 83-byte evidence or order
         * budget; test the menu ON SCREEN, leaving PASS selected and disarmed. */
        const Gk79Checkpoint before=app.stock_checkpoint;
        input(GK1_INPUT_SETUP);
        if(app.screen!=GK1_SCREEN_STOCK_COMMANDS)return 34;
        for(int j=0;j<4;j++)input(GK1_INPUT_DOWN); /* TRADE */
        if(app.stock_command_cursor!=4)return 35;
        input(GK1_INPUT_CONFIRM);
        if(app.screen!=GK1_SCREEN_STOCK_TRADE_REFERENCE)return 36;
        input(GK1_INPUT_RIGHT);input(GK1_INPUT_DOWN);input(GK1_INPUT_CONFIRM);
        input(GK1_INPUT_CANCEL);
        if(app.screen!=GK1_SCREEN_STOCK_COMMANDS)return 43;
        input(GK1_INPUT_DOWN);input(GK1_INPUT_DOWN); /* VIEW */
        input(GK1_INPUT_CONFIRM);
        if(app.screen!=GK1_SCREEN_STOCK_CONTINUE)return 37;
        input(GK1_INPUT_SETUP);
        if(app.screen!=GK1_SCREEN_STOCK_COMMANDS)return 38;
        for(int j=0;j<5;j++)input(GK1_INPUT_DOWN);
        input(GK1_INPUT_RIGHT); /* PASS: row 6, right-hand column */
        if(app.stock_command_cursor!=12)return 42;
        input(GK1_INPUT_CONFIRM);if(!app.stock_checkpoint.pending_pass)return 39;
        input(GK1_INPUT_CANCEL);if(app.stock_checkpoint.pending_pass)return 40;
        if(memcmp(&before,&app.stock_checkpoint,sizeof before)!=0||
            app.stock_checkpoint.orders.remaining!=3 ||
            app.worlds[0].phase!=GK1_PRE_SETUP_TEMPLATE ||
            app.worlds[1].phase!=GK1_PRE_SETUP_TEMPLATE)return 41;
      }else{
      input(GK1_INPUT_CONFIRM); /* arm PASS */
      if(!app.stock_checkpoint.pending_pass||app.stock_checkpoint.orders.remaining!=3)return 24;
      input(GK1_INPUT_CANCEL);  /* B cancel preserves 3 orders */
      if(app.stock_checkpoint.pending_pass||app.stock_checkpoint.orders.remaining!=3)return 25;
      input(GK1_INPUT_CONFIRM);input(GK1_INPUT_CONFIRM); /* committed PASS */
      if(!app.stock_checkpoint.orders.ended||app.stock_checkpoint.orders.remaining!=0)return 26;
      if(app.worlds[0].phase!=GK1_PRE_SETUP_TEMPLATE||app.worlds[1].phase!=GK1_PRE_SETUP_TEMPLATE)return 27;
      }
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
 if(show_gk226_title_update||show_gk226_title_verify||show_gk226_title_final){
  /* Three truly distinct native executable launches, all via TITLE Y and
   * controller A/X. Update an existing 80-byte source-verified .gk24 slot. */
  if(!scenario_loaded||app.screen!=GK1_SCREEN_TITLE||
     !app.world_ready[0]||!app.world_ready[1])return 101;
  Gk1LoadedScenario before_worlds[2];
  memcpy(before_worlds,app.worlds,sizeof before_worlds);
  unsigned expected=(show_gk226_title_update?1u:show_gk226_title_verify?2u:3u);
  /* Actual 1280x720 process must populate the same cached metadata the
   * title renderer uses, before controller Y changes the running phase. */
  if(app.rules_profile!=1u)return 106;
  refresh_private_title_preview();
  if(practice_title_status!=GK227_PREVIEW_OK ||
     practice_title_preview.actions_committed!=expected ||
     practice_title_preview.orders_remaining!=3u-expected ||
     practice_title_preview.scenario!=1u ||
     practice_title_preview.country_count!=14u)return 107;
  pad_test_press(GK1_PAD_Y);
  if(app.screen!=GK1_SCREEN_ENHANCED_SPRING_LAB||
     app.port_spring.actions_committed!=expected||
     app.port_spring.remaining!=3u-expected||
     strstr(app.status,"SOURCE-REPLAY VERIFIED")==NULL)return 102;
  if(!show_gk226_title_final){
   pad_test_press(GK1_PAD_A);
   if(app.port_spring.actions_committed!=expected+1u)return 103;
   pad_test_press(GK1_PAD_X);
   if(strstr(app.status,"SOURCE-REPLAY VERIFIED")==NULL)return 104;
   printf("EXEC0226 TESTED_PORT_LINUX native controller TITLE Y -> A -> X source-locked SAVE REPLACE at %u of 3 PASS\n",expected+1u);
  }else{
   puts("EXEC0226 TESTED_PORT_LINUX third cold-resume verifies 3/3 completed Enhanced-only receipt PASS");
  }
  if(memcmp(before_worlds,app.worlds,sizeof before_worlds)!=0)return 105;
 }
 if(show_enhanced_lab_title_resume||show_enhanced_lab_title_reject){
  /* A separate fresh process reloaded both original source templates, and
   * uses the exact title Y controller route. No synthetic initialized world. */
  if(!scenario_loaded||app.screen!=GK1_SCREEN_TITLE||
     !app.world_ready[0]||!app.world_ready[1])return 91;
  Gk1LoadedScenario template_worlds[2];
  memcpy(template_worlds,app.worlds,sizeof template_worlds);
  pad_test_press(GK1_PAD_Y);
  if(show_enhanced_lab_title_resume){
   if(app.screen!=GK1_SCREEN_ENHANCED_SPRING_LAB||
      !app.port_spring.active||app.port_spring.remaining!=2u||
      app.port_spring.actions_committed!=1u||
      strstr(app.status,"SOURCE-REPLAY VERIFIED")==NULL)return 92;
   pad_test_press(GK1_PAD_A);
   if(app.port_spring.remaining!=1u||app.port_spring.actions_committed!=2u)return 93;
   pad_test_press(GK1_PAD_B);
   if(app.screen!=GK1_SCREEN_SCENARIO_COUNTRIES)return 94;
   puts("TESTED_PORT_LINUX EXEC0225 2ND PROCESS TITLE Y -> ENHANCED PRACTICE -> A SECOND ORDER -> B country picker; NOT original NES/Win32/Xbox");
  }else{
   if(app.screen!=GK1_SCREEN_TITLE||strstr(app.status,"CORRUPT - UNCHANGED")==NULL||
      app.port_spring.active)return 95;
   puts("TESTED_PORT_LINUX EXEC0225 2ND PROCESS TITLE Y corrupted receipt rejects without fallback");
  }
  if(memcmp(template_worlds,app.worlds,sizeof template_worlds)!=0)return 96;
 }
 if(show_enhanced_lab){
  if(!scenario_loaded)return 12;
  input(GK1_INPUT_CONFIRM);input(GK1_INPUT_CONFIRM); /* enhanced Mongol country */
  input(GK1_INPUT_CONFIRM);input(GK1_INPUT_SETUP); /* setup wizard */
  input(GK1_INPUT_CONFIRM);input(GK1_INPUT_CONFIRM);
  input(GK1_INPUT_CONFIRM);input(GK1_INPUT_CONFIRM);
  if(app.screen!=GK1_SCREEN_SETUP_WIZARD||
     app.setup_stage[0]!=GK53_SETUP_AWAIT_SOURCE)return 71;
  Gk1LoadedScenario source_world=app.worlds[0];
  pad_test_press(GK1_PAD_A);
  if(app.screen!=GK1_SCREEN_ENHANCED_SPRING_LAB||
     app.port_spring.remaining!=3u)return 72;
  pad_test_press(GK1_PAD_A); /* PASS 1 */
  if(app.port_spring.remaining!=2u)return 73;
  if(show_enhanced_lab_save){
   Gk222Spring expected=app.port_spring;
   pad_test_press(GK1_PAD_X); /* create non-overwriting slot */
   if(strstr(app.status,"SOURCE-REPLAY VERIFIED")==NULL)return 80;
   pad_test_press(GK1_PAD_A); /* another real controller action */
   if(app.port_spring.remaining!=1u)return 81;
   pad_test_press(GK1_PAD_Y); /* restore original bounded receipt */
   if(strstr(app.status,"SOURCE-REPLAY VERIFIED")==NULL||
      memcmp(&expected,&app.port_spring,sizeof expected)!=0)return 82;
  }
  input(GK1_INPUT_RIGHT); /* PORT-only worker from Town to Food */
  pad_test_press(GK1_PAD_A);
  if(app.port_spring.remaining!=1u)return 74;
  input(GK1_INPUT_LEFT);
  pad_test_press(GK1_PAD_A); /* PASS 3 */
  if(app.port_spring.remaining||!app.port_spring.ended)return 75;
  pad_test_press(GK1_PAD_LB); /* physical-path synthetic LB: undo PASS */
  if(app.port_spring.remaining!=1u||app.port_spring.ended||
     app.port_spring.actions_committed!=2u)return 77;
  pad_test_press(GK1_PAD_LB); /* undo worker change and restore both pools */
  if(app.port_spring.remaining!=2u||app.port_spring.actions_committed!=1u)return 78;
  pad_test_press(GK1_PAD_A); /* replay PASS */
  pad_test_press(GK1_PAD_A); /* replay PASS */
  if(app.port_spring.remaining||!app.port_spring.ended)return 79;
  if(memcmp(&source_world,&app.worlds[0],sizeof source_world)!=0||
     app.worlds[0].phase!=GK1_PRE_SETUP_TEMPLATE)return 76;
  printf("TESTED_PORT_LINUX EXEC0222 controller-only Enhanced 3-order finance practice X11 PASS; Classic/stock remain PRE_SETUP\n");
 }
 if(show_labor_undo){
  if(!scenario_loaded)return 12;
  input(GK1_INPUT_CONFIRM); /* title -> campaigns */
  input(GK1_INPUT_LEFT); /* Classic */
  input(GK1_INPUT_CONFIRM); /* Mongol */
  input(GK1_INPUT_LABOR_PLAN);
  if(app.screen!=GK1_SCREEN_LABOR_PLAN)return 51;
  Gk1LoadedScenario saved=app.worlds[0];
  pad_test_press(GK1_PAD_A); /* controller commit */
  if(app.labor_history.count!=1u ||
     app.worlds[0].countries[0].troops!=saved.countries[0].troops-1u)return 52;
  pad_test_press(GK1_PAD_LB); /* controller Undo, NOT slot selection */
  if(app.labor_history.count!=0u ||
     memcmp(&saved,&app.worlds[0],sizeof saved)!=0 ||
     app.screen!=GK1_SCREEN_LABOR_PLAN)return 53;
  if(app.worlds[0].phase!=GK1_PRE_SETUP_TEMPLATE ||
     app.stock_checkpoint.valid)return 54;
  printf("TESTED_PORT_LINUX EXEC0220 controller A/LB workforce transfer + undo, same source world, no original NES order PASS\n");
 }
 if(show_classic_finance){
  if(!scenario_loaded)return 12;
  input(GK1_INPUT_CONFIRM); /* title -> campaign menu */
  input(GK1_INPUT_LEFT); /* CLASSIC */
  input(GK1_INPUT_CONFIRM); /* Mongol country list */
  input(GK1_INPUT_LABOR_PLAN); /* Classic labor research */
  if(show_finance_compare){
   input(GK1_INPUT_CONFIRM); /* real controller workforce edit */
   if(app.screen!=GK1_SCREEN_LABOR_PLAN)return 38;
  }
  input(GK1_INPUT_SETUP); /* old contract: blocked Enhanced Demobilize */
  if(app.screen!=GK1_SCREEN_LABOR_PLAN)return 34;
  input(GK1_INPUT_SETUP); /* deliberate second START opens actual finance UI */
  if(app.screen!=GK1_SCREEN_SPRING_FINANCE_PREVIEW)return 35;
  Gk217Preview preview;
  if(gk217_preview_country(&app.worlds[0],app.finance_cursor,&preview)!=GK217_OK||
     preview.country_count!=14||preview.original_spring_generated)return 36;
  if(show_finance_compare){
   Gk219Comparison compare;
   if(gk219_compare_finance(&app.finance_source_baseline[0],&app.worlds[0],
       app.finance_cursor,&compare)!=GK219_OK||!compare.workforce_changed)return 39;
   input(GK1_INPUT_CONFIRM); /* A toggles actual comparison overlay */
   if(!app.finance_compare)return 40;
  }
  input(GK1_INPUT_DOWN);
  if(app.finance_cursor!=1u||app.worlds[0].phase!=GK1_PRE_SETUP_TEMPLATE)return 37;
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
  if(smoke && frames>=smoke){printf("EXEC0199 integrated research X11 native smoke: %d frames, %dx%d requested, PASS; stock_spring=%s stock_checkpoint_pass=%s order_budget=%s battle_pass=%s\n",frames,WIDTH,HEIGHT,show_stock_spring?"yes":"no",show_stock_checkpoint?"yes":"no",show_order_budget?"yes":"no",show_pass?"yes":"no");break;}
  struct timespec t={0,16000000};nanosleep(&t,NULL);
 }
 if(joystick>=0)close(joystick);
 if(font)XFreeFont(display,font);
 XFreeGC(display,graphics);XDestroyWindow(display,window);XCloseDisplay(display);
 return 0;
}
#endif
