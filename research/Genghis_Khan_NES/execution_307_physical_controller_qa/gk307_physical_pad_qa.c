#include "gk307_physical_pad_qa.h"
#include <string.h>
#include <limits.h>
/* XInput 16-bit buttons: only a real Windows transport supplies these in
 * the production diagnostic path. Unit tests inject samples intentionally. */
static const uint16_t required_buttons[6]={0x0001u,0x0002u,0x1000u,
 0x2000u,0x0010u,0x0020u};
static int centered(int16_t v){return v>-6000 && v<6000;}
static int neutral(const Gk99XinputRaw *r){return r->buttons==0u &&
 centered(r->thumb_lx)&&centered(r->thumb_ly);}
void gk307_physical_init(Gk307PhysicalQa *s){
 if(!s)return;
 memset(s,0,sizeof *s);
 s->active_slot=0xffu;
}
void gk307_physical_disconnect(Gk307PhysicalQa *s){
 gk307_physical_init(s);
}
int gk307_physical_step(Gk307PhysicalQa *s,uint8_t slot,
 const Gk99XinputRaw *raw){
 if(!s||!raw||slot>3u)return 0;
 if(s->active_slot!=slot){
  gk307_physical_init(s);s->active_slot=slot;
  /* New pad held Start/A MUST NOT count: first neutral sample arms. */
 }
 if(s->complete)return 1;
 if(s->wait_release){
  if(neutral(raw))s->wait_release=0u;
  return 1;
 }
 if(s->step==0u){
  if(neutral(raw))s->step=1u;
  return 1;
 }
 int match=0;
 if(s->step>=1u&&s->step<=6u){
  match=raw->buttons==required_buttons[s->step-1u]&&
     centered(raw->thumb_lx)&&centered(raw->thumb_ly);
 }else if(s->step==7u){
  match=raw->buttons==0u && raw->thumb_lx<-22000 && centered(raw->thumb_ly);
 }else if(s->step==8u){
  match=raw->buttons==0u && raw->thumb_lx>22000 && centered(raw->thumb_ly);
 }
 if(match){s->accepted_edges++;s->step++;
  if(s->step>GK307_PHYSICAL_STEPS){s->complete=1u;s->wait_release=0u;}
  else s->wait_release=1u;
 }
 return 1;
}
const char *gk307_physical_instruction(const Gk307PhysicalQa *s){
 if(!s)return "INVALID QA STATE";
 if(s->complete)return "PHYSICAL XINPUT INPUT ROUTE: 8/8 STEPS PASS - CLOSE WINDOW (NOT CAMPAIGN QA)";
 if(s->active_slot==0xffu)return "PHYSICAL QA: CONNECT AN XBOX CONTROLLER";
 if(s->wait_release)return "PHYSICAL QA: RELEASE ALL BUTTONS AND CENTER LEFT STICK";
 static const char *text[]={"CENTER STICK; RELEASE ALL BUTTONS",
   "PRESS DPAD UP","PRESS DPAD DOWN","PRESS A","PRESS B",
   "PRESS START","PRESS BACK/VIEW","MOVE LEFT STICK FULL LEFT",
   "MOVE LEFT STICK FULL RIGHT"};
 return s->step<=GK307_PHYSICAL_STEPS?text[s->step]:"INVALID QA STEP";
}
