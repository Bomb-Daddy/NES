/* EXEC0043: UPDATED standalone NATIVE 1280x720 cross-platform Genghis Khan shell.
 * Copyright-clean PLACEHOLDER graphics and no sound assets. NOT a complete game.
 * Linux/X11 native compilation executed; Win32/XInput code provided but NOT
 * Windows-compiled or physically controller-tested in this run.
 */
#define _POSIX_C_SOURCE 200809L
#include "gk1_app_state.h"
#include "gk1_replay_store.h"
#include "gk1_pad_router.h"
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
#define WIDTH 1280
#define HEIGHT 720
#define BG UINT32_C(0x10202b)
#define EDGE UINT32_C(0xb4a16b)
#define FG UINT32_C(0xe5d7a5)
#define ACTIVE UINT32_C(0x386f58)
#define WARN UINT32_C(0x8b4743)
static Gk1App app;
static Gk1PadRouter pad;
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
 label(62,66,"GENGHIS KHAN I - WINDOWS PORT",FG);
 label(62,96,"EXEC0043 CONTROLLER-ONLY RESEARCH SHELL / NOT A PLAYABLE CAMPAIGN",EDGE);
 if(app.screen==GK1_SCREEN_TITLE){
  frame(120,155,1038,405);
  menu_line(270,app.menu_index==0,"TWO CAMPAIGNS - INCOMPLETE");
  menu_line(347,app.menu_index==1,"TESTED BATTLE MOVEMENT CASES");
  label(150,440,"XBOX D-PAD/STICK NAVIGATE | A CONFIRM | Y LOAD",FG);
 }else if(app.screen==GK1_SCREEN_CAMPAIGNS){
  frame(120,155,1038,405);
  menu_line(265,app.menu_index==0,"MONGOL CONQUEST - NOT YET PLAYABLE");
  menu_line(339,app.menu_index==1,"WORLD CONQUEST - NOT YET PLAYABLE");
  label(150,449,"A SHOW STATUS  |  B MAIN MENU",FG);
 }else if(app.screen==GK1_SCREEN_UNIMPLEMENTED){
  frame(120,155,1038,405);
  label(160,276,"GAMEPLAY NOT IMPLEMENTED; NO FABRICATED CAMPAIGN DATA",FG);
  label(160,362,"A OR B TO RETURN",FG);
 }else if(app.screen==GK1_SCREEN_RESEARCH_BATTLE){
  frame(52,127,1172,476);
  label(87,168,"16x10 RESEARCH GRID - PLACEHOLDER ART, NOT NATIVE NES TILES",FG);
  for(int y=0;y<10;y++) for(int x=0;x<16;x++){
   uint32_t c=((x+y)&1)?UINT32_C(0x305149):UINT32_C(0x385b52);
   if(x==8&&y==7)c=UINT32_C(0x71826b);
   if(x==7&&y==7)c=UINT32_C(0x405c91);
   fillrect(90+x*38,189+y*33,37,32,c);
  }
  label(725,222,"CONTROLLED NATIVE CASE",FG);
  label(725,258,gk1_app_case_label(app.case_index),FG);
  if(app.has_session){
   snprintf(line,sizeof line,"SOLDIERS: %u",(unsigned)app.session.battle.mover.soldiers);label(725,312,line,FG);
   snprintf(line,sizeof line,"MOBILITY: %u",(unsigned)app.session.battle.mover.mobility);label(725,350,line,FG);
   snprintf(line,sizeof line,"WORD $613E: %u",(unsigned)app.session.battle.completion_word);label(725,388,line,FG);
   snprintf(line,sizeof line,"FRIENDLY SLOT: %u",(unsigned)app.session.battle.friendly_slot1.soldiers);label(725,426,line,FG);
   label(725,492,"A MOVE  /  B FINISH OR BACK",FG);
   label(725,525,"DPAD CASE  /  X SAVE  /  Y LOAD",FG);
  }
 }
 frame(62,628,1150,52);
 label(78,658,app.status,(app.screen==GK1_SCREEN_UNIMPLEMENTED)?WARN:FG);
}
static void input(Gk1Input command){(void)gk1_app_input(&app,command);}
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
 }
}

static void replay_key(int save){
 if(save)(void)save_replay_to(replay_path());
 else (void)load_replay_from(replay_path());
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
 s.lx=state.Gamepad.sThumbLX;
 /* XInput Y positive = up; portable pad router Y positive = down. */
 s.ly=state.Gamepad.sThumbLY==INT16_MIN?INT16_MAX:(int16_t)-state.Gamepad.sThumbLY;
 pad_actions(&s);
}
int WINAPI WinMain(HINSTANCE instance,HINSTANCE prev,LPSTR cmd,int show){
 (void)prev;(void)cmd;
 gk1_app_init(&app);gk1_pad_init(&pad);
 WNDCLASSA cl={0};cl.lpfnWndProc=window_proc;cl.hInstance=instance;
 cl.lpszClassName="GK1_NATIVE_RESEARCH";cl.hCursor=LoadCursor(NULL,IDC_ARROW);
 cl.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
 if(!RegisterClassA(&cl))return 2;
 RECT r={0,0,WIDTH,HEIGHT};AdjustWindowRect(&r,WS_OVERLAPPEDWINDOW,FALSE);
 hwnd=CreateWindowA(cl.lpszClassName,"Genghis Khan I - Research Port v0.043",WS_OVERLAPPEDWINDOW,
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
 /* Two separate native Linux processes prove disk cold-load behavior without
  * X11 display or an emulator.  Test uses actual app input and disk adapter.
  */
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
 int smoke=0;
 if(argc==3 && strcmp(argv[1],"--smoke-frames")==0){smoke=atoi(argv[2]);if(smoke<1||smoke>120)return 4;}
 else if(argc!=1){fprintf(stderr,"Usage: gk1_native_shell [--smoke-frames 1..120 | --cold-export path case events | --cold-import path]\n");return 4;}
 gk1_app_init(&app);gk1_pad_init(&pad);
 display=XOpenDisplay(NULL);if(!display){fprintf(stderr,"X11 DISPLAY unavailable\n");return 2;}
 int screen=DefaultScreen(display);
 window=XCreateSimpleWindow(display,RootWindow(display,screen),0,0,WIDTH,HEIGHT,0,0,0);
 XStoreName(display,window,"Genghis Khan I - Research Port v0.043");
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
  if(smoke && frames>=smoke){printf("EXEC0043 X11 native smoke: %d frames, %dx%d requested, PASS\n",frames,WIDTH,HEIGHT);break;}
  struct timespec t={0,16000000};nanosleep(&t,NULL);
 }
 if(joystick>=0)close(joystick);
 if(font)XFreeFont(display,font);
 XFreeGC(display,graphics);XDestroyWindow(display,window);XCloseDisplay(display);
 return 0;
}
#endif
