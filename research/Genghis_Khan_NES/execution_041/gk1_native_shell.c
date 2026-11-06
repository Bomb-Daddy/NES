/* EXEC0041: NEW standalone NATIVE 1280x720 cross-platform Genghis Khan shell.
 * Copyright-clean PLACEHOLDER graphics and no sound assets. NOT a complete game.
 * Linux/X11 native compilation executed; Win32/XInput code provided but NOT
 * Windows-compiled or physically controller-tested in this run.
 */
#define _POSIX_C_SOURCE 200809L
#include "gk1_app_state.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
# include <windows.h>
# include <xinput.h>
#else
# include <X11/Xlib.h>
# include <X11/keysym.h>
# include <time.h>
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
 label(62,96,"EXEC0041 NATIVE RESEARCH SHELL / NOT A PLAYABLE CAMPAIGN",EDGE);
 if(app.screen==GK1_SCREEN_TITLE){
  frame(120,155,1038,405);
  menu_line(270,app.menu_index==0,"TWO CAMPAIGNS - INCOMPLETE");
  menu_line(347,app.menu_index==1,"TESTED BATTLE MOVEMENT CASES");
  label(150,440,"D-PAD UP/DOWN  |  A CONFIRM  |  B BACK",FG);
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
   label(725,525,"DPAD CHANGE CASE",FG);
  }
 }
 frame(62,628,1150,52);
 label(78,658,app.status,(app.screen==GK1_SCREEN_UNIMPLEMENTED)?WARN:FG);
}
static void input(Gk1Input command){(void)gk1_app_input(&app,command);}
#ifdef _WIN32
static LRESULT CALLBACK window_proc(HWND h,UINT msg,WPARAM w,LPARAM l){
 (void)l;
 if(msg==WM_DESTROY){running=0;PostQuitMessage(0);return 0;}
 if(msg==WM_KEYDOWN){
  if(w==VK_UP)input(GK1_INPUT_UP);else if(w==VK_DOWN)input(GK1_INPUT_DOWN);
  else if(w==VK_LEFT)input(GK1_INPUT_LEFT);else if(w==VK_RIGHT)input(GK1_INPUT_RIGHT);
  else if(w==VK_RETURN||w==VK_SPACE)input(GK1_INPUT_CONFIRM);
  else if(w==VK_ESCAPE||w==VK_BACK)input(GK1_INPUT_CANCEL);
  InvalidateRect(h,NULL,FALSE);return 0;
 }
 if(msg==WM_PAINT){PAINTSTRUCT p;screen_dc=BeginPaint(h,&p);render();EndPaint(h,&p);return 0;}
 return DefWindowProcA(h,msg,w,l);
}
typedef DWORD(WINAPI *XinputGetStateFn)(DWORD,XINPUT_STATE*);
static void poll_xinput(void){
 static HMODULE dll=NULL;
 static XinputGetStateFn get_state=NULL;
 static WORD last_buttons=0;
 if(!dll){dll=LoadLibraryA("xinput1_4.dll");if(!dll)dll=LoadLibraryA("xinput9_1_0.dll");
 if(dll)get_state=(XinputGetStateFn)(void*)GetProcAddress(dll,"XInputGetState");}
 if(!get_state)return;
 XINPUT_STATE state;memset(&state,0,sizeof state);
 if(get_state(0,&state)!=ERROR_SUCCESS){last_buttons=0;return;}
 WORD now=state.Gamepad.wButtons;WORD edge=(WORD)(now & ~last_buttons);last_buttons=now;
 if(edge & XINPUT_GAMEPAD_DPAD_UP)input(GK1_INPUT_UP);
 if(edge & XINPUT_GAMEPAD_DPAD_DOWN)input(GK1_INPUT_DOWN);
 if(edge & XINPUT_GAMEPAD_DPAD_LEFT)input(GK1_INPUT_LEFT);
 if(edge & XINPUT_GAMEPAD_DPAD_RIGHT)input(GK1_INPUT_RIGHT);
 if(edge & XINPUT_GAMEPAD_A)input(GK1_INPUT_CONFIRM);
 if(edge & XINPUT_GAMEPAD_B)input(GK1_INPUT_CANCEL);
}
int WINAPI WinMain(HINSTANCE instance,HINSTANCE prev,LPSTR cmd,int show){
 (void)prev;(void)cmd;
 gk1_app_init(&app);
 WNDCLASSA cl={0};cl.lpfnWndProc=window_proc;cl.hInstance=instance;
 cl.lpszClassName="GK1_NATIVE_RESEARCH";cl.hCursor=LoadCursor(NULL,IDC_ARROW);
 cl.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
 if(!RegisterClassA(&cl))return 2;
 RECT r={0,0,WIDTH,HEIGHT};AdjustWindowRect(&r,WS_OVERLAPPEDWINDOW,FALSE);
 hwnd=CreateWindowA(cl.lpszClassName,"Genghis Khan I - Research Port v0.041",WS_OVERLAPPEDWINDOW,
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
static void keypress(KeySym sym){
 switch(sym){
 case XK_Up: input(GK1_INPUT_UP);break;
 case XK_Down:input(GK1_INPUT_DOWN);break;
 case XK_Left:input(GK1_INPUT_LEFT);break;
 case XK_Right:input(GK1_INPUT_RIGHT);break;
 case XK_Return:case XK_space:input(GK1_INPUT_CONFIRM);break;
 case XK_Escape:case XK_BackSpace:input(GK1_INPUT_CANCEL);break;
 default:break;
 }
}
static void poll_joystick(int fd){
 if(fd<0)return;
 struct js_event e;static int8_t last_axis[2]={0,0};
 while(read(fd,&e,sizeof e)==(ssize_t)sizeof e){
  unsigned type=e.type & (unsigned)~JS_EVENT_INIT;
  if(type==JS_EVENT_BUTTON && e.value){
   if(e.number==0)input(GK1_INPUT_CONFIRM);
   if(e.number==1)input(GK1_INPUT_CANCEL);
  }else if(type==JS_EVENT_AXIS && e.number<2){
   int8_t sign=e.value>16000?1:(e.value< -16000?-1:0);
   if(sign!=last_axis[e.number] && sign){
    if(e.number==0)input(sign<0?GK1_INPUT_LEFT:GK1_INPUT_RIGHT);
    if(e.number==1)input(sign<0?GK1_INPUT_UP:GK1_INPUT_DOWN);
   }
   last_axis[e.number]=sign;
  }
 }
}
int main(int argc,char **argv){
 int smoke=0;
 if(argc==3 && strcmp(argv[1],"--smoke-frames")==0){smoke=atoi(argv[2]);if(smoke<1||smoke>120)return 4;}
 else if(argc!=1){fprintf(stderr,"Usage: gk1_native_shell [--smoke-frames 1..120]\n");return 4;}
 gk1_app_init(&app);
 display=XOpenDisplay(NULL);if(!display){fprintf(stderr,"X11 DISPLAY unavailable\n");return 2;}
 int screen=DefaultScreen(display);
 window=XCreateSimpleWindow(display,RootWindow(display,screen),0,0,WIDTH,HEIGHT,0,0,0);
 XStoreName(display,window,"Genghis Khan I - Research Port v0.041");
 XSelectInput(display,window,ExposureMask|KeyPressMask|StructureNotifyMask);
 XMapWindow(display,window);graphics=XCreateGC(display,window,0,NULL);
 font=XLoadQueryFont(display,"-misc-fixed-bold-r-normal--20-200-75-75-c-100-iso8859-1");
 if(!font)font=XLoadQueryFont(display,"fixed");
 if(font)XSetFont(display,graphics,font->fid);
 const char *pad=getenv("GK1_JOYSTICK_DEV");
 if(!pad)pad="/dev/input/js0";
 int joystick=open(pad,O_RDONLY|O_NONBLOCK);
 int frames=0;
 while(running){
  while(XPending(display)){
   XEvent e;XNextEvent(display,&e);
   if(e.type==DestroyNotify)running=0;
   else if(e.type==KeyPress)keypress(XLookupKeysym(&e.xkey,0));
  }
  poll_joystick(joystick);render();XFlush(display);frames++;
  if(smoke && frames>=smoke){printf("EXEC0041 X11 native smoke: %d frames, %dx%d requested, PASS\n",frames,WIDTH,HEIGHT);break;}
  struct timespec t={0,16000000};nanosleep(&t,NULL);
 }
 if(joystick>=0)close(joystick);
 if(font)XFreeFont(display,font);
 XFreeGC(display,graphics);XDestroyWindow(display,window);XCloseDisplay(display);
 return 0;
}
#endif
