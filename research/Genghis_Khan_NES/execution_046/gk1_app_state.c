#include "gk1_app_state.h"
#include <string.h>
/* Four exact previously recorded controlled test cases; no simulated claims
 * about arbitrary movement, Entire/Divide, enemy combat or campaign play.
 */
typedef struct {uint8_t mobility,terrain;Gk1Occupancy occupant;const char *label;} Case;
static const Case cases[]={
 {10,3,GK1_OCC_EMPTY,"FOREST 10 - pending"},
 {4,3,GK1_OCC_EMPTY,"FOREST 4 - finish"},
 {2,3,GK1_OCC_EMPTY,"FOREST 2 - rejected"},
 {10,3,GK1_OCC_FRIEND_SLOT1,"FRIENDLY TILE - join"},
 {10,3,GK1_OCC_ENEMY_SLOT10,"ENEMY TILE - blocked"},
 {10,0,GK1_OCC_EMPTY,"TERRAIN 0 - finish"}
};
#define CASE_COUNT (sizeof(cases)/sizeof(cases[0]))
const char *gk1_app_case_label(unsigned i){return i<CASE_COUNT?cases[i].label:"UNKNOWN";}
void gk1_app_init(Gk1App *s){if(!s)return;memset(s,0,sizeof *s);s->screen=GK1_SCREEN_TITLE;s->last_move=GK1_UNVERIFIED;strcpy(s->status,"SOURCE-BOUNDED RESEARCH BUILD - NOT FULL GAME");}
static void configure_case(Gk1App *s){
 const Case *c=&cases[s->case_index];
 Gk1BattleSession fresh;
 if(gk1_session_open_fixture(c->mobility,c->terrain,c->occupant,&fresh)==GK1_SESSION_OK){
  s->session=fresh;s->has_session=1;s->last_move=GK1_UNVERIFIED;
  strcpy(s->status,"A: try native-recorded step (8,7). B: back or finish.");
 } else {s->has_session=0;strcpy(s->status,"CASE UNAVAILABLE: SOURCE EVIDENCE REQUIRED");}
}
int gk1_app_input(Gk1App *s,Gk1Input cmd){
 if(!s || cmd<GK1_INPUT_UP || cmd>GK1_INPUT_CANCEL)return 0;
 switch(s->screen){
 case GK1_SCREEN_TITLE:
  if(cmd==GK1_INPUT_UP){s->menu_index=(uint8_t)((s->menu_index+2)%3);return 1;}
  if(cmd==GK1_INPUT_DOWN){s->menu_index=(uint8_t)((s->menu_index+1)%3);return 1;}
  if(cmd==GK1_INPUT_CONFIRM){s->screen=s->menu_index==2?GK1_SCREEN_PATH_PICK:(s->menu_index?GK1_SCREEN_RESEARCH_BATTLE:GK1_SCREEN_CAMPAIGNS);s->menu_index=0;
    if(s->screen==GK1_SCREEN_RESEARCH_BATTLE)configure_case(s);
    return 1;}
  return 1;
 case GK1_SCREEN_CAMPAIGNS:
  if(cmd==GK1_INPUT_UP||cmd==GK1_INPUT_DOWN){s->menu_index=(uint8_t)(1-s->menu_index);return 1;}
  if(cmd==GK1_INPUT_CANCEL){s->screen=GK1_SCREEN_TITLE;s->menu_index=0;return 1;}
  if(cmd==GK1_INPUT_CONFIRM){s->screen=GK1_SCREEN_UNIMPLEMENTED;
   strcpy(s->status,"NOT IMPLEMENTED: TWO FULL PLAYABLE CAMPAIGNS OPEN");return 1;}
  return 1;
 case GK1_SCREEN_UNIMPLEMENTED:
  if(cmd==GK1_INPUT_CANCEL||cmd==GK1_INPUT_CONFIRM){s->screen=GK1_SCREEN_CAMPAIGNS;s->menu_index=0;}return 1;
 case GK1_SCREEN_PATH_PICK:
  if(cmd==GK1_INPUT_UP||cmd==GK1_INPUT_LEFT){s->path_case=(uint8_t)((s->path_case+2)%3);return 1;}
  if(cmd==GK1_INPUT_DOWN||cmd==GK1_INPUT_RIGHT){s->path_case=(uint8_t)((s->path_case+1)%3);return 1;}
  if(cmd==GK1_INPUT_CANCEL){s->screen=GK1_SCREEN_TITLE;s->menu_index=2;return 1;}
  if(cmd==GK1_INPUT_CONFIRM){
   if(gk45_open((Gk45Case)(s->path_case+1),&s->path)!=GK45_ACCEPTED){strcpy(s->status,"SOURCE FIXTURE UNAVAILABLE");return 1;}
   s->path_event_count=0;s->cursor_x=7;s->cursor_y=7;s->screen=GK1_SCREEN_PATH_BATTLE;
   strcpy(s->status,"RIGHT SELECT TARGET, A APPLY, B COMPLETE / BACK");return 1;
  }
  return 1;
 case GK1_SCREEN_PATH_BATTLE:
  if(cmd==GK1_INPUT_RIGHT && s->cursor_x<15){s->cursor_x++;return 1;}
  if(cmd==GK1_INPUT_LEFT && s->cursor_x>0){s->cursor_x--;return 1;}
  if(cmd==GK1_INPUT_UP && s->cursor_y>0){s->cursor_y--;return 1;}
  if(cmd==GK1_INPUT_DOWN && s->cursor_y<9){s->cursor_y++;return 1;}
  if(cmd==GK1_INPUT_CONFIRM){
   Gk45Result outcome=GK45_UNKNOWN;uint8_t ev=0;
   if(s->path.phase==GK45_BEFORE && s->cursor_x==8 && s->cursor_y==7){
     outcome=gk45_step_first(&s->path);ev=1;
   } else if(s->path.phase==GK45_PENDING && s->path.steps==1 && s->cursor_x==9 && s->cursor_y==7){
     outcome=gk45_step_second(&s->path);ev=2;
   }
   if(outcome==GK45_ACCEPTED || outcome==GK45_FINISHED || outcome==GK45_REFUSED){
    if(s->path_event_count>=sizeof s->path_events){strcpy(s->status,"PATH EVENT LIMIT - SOURCE REQUIRES REVIEW");return 1;}
    s->path_events[s->path_event_count++]=ev;s->cursor_x=s->path.x;s->cursor_y=s->path.y;
    if(outcome==GK45_REFUSED)strcpy(s->status,"RAW 7 REFUSED: FIRST B DISMISSES, SECOND B FINISHES");
    else if(outcome==GK45_FINISHED)strcpy(s->status,"FRIENDLY JOIN COMPLETE - 74/0, WORD 2");
    else strcpy(s->status,"STEP COMMITTED: B FINISHES, OR SELECT NEXT RIGHT TILE");
   } else strcpy(s->status,"UNVERIFIED TARGET / COMMAND: NO STATE MUTATION");
   return 1;
  }
  if(cmd==GK1_INPUT_CANCEL){
   if(s->path.phase==GK45_BEFORE || s->path.phase==GK45_DONE){
    s->screen=GK1_SCREEN_PATH_PICK;strcpy(s->status,"SOURCE-BOUND PATH SCENARIO MENU");return 1;
   }
   Gk45Result r=gk45_press_b(&s->path);
   if(r==GK45_ACCEPTED || r==GK45_FINISHED){
    if(s->path_event_count>=sizeof s->path_events){strcpy(s->status,"PATH EVENT LIMIT - SOURCE REQUIRES REVIEW");return 1;}
    s->path_events[s->path_event_count++]=3;
    if(r==GK45_FINISHED)strcpy(s->status,"B COMPLETED PATH; MOBILITY RESTORED / WORD 1");
    else strcpy(s->status,"B DISMISSED REFUSAL ONLY; PATH STILL PENDING");
   } else strcpy(s->status,"B NOT SUPPORTED IN THIS SOURCE STAGE");
   return 1;
  }
  return 1;
 case GK1_SCREEN_RESEARCH_BATTLE:
  if(cmd==GK1_INPUT_LEFT || cmd==GK1_INPUT_UP){s->case_index=(uint8_t)((s->case_index+CASE_COUNT-1)%CASE_COUNT);configure_case(s);return 1;}
  if(cmd==GK1_INPUT_RIGHT||cmd==GK1_INPUT_DOWN){s->case_index=(uint8_t)((s->case_index+1)%CASE_COUNT);configure_case(s);return 1;}
  if(cmd==GK1_INPUT_CONFIRM){
   Gk1MoveResult r=GK1_UNVERIFIED;
   if(!s->has_session || gk1_session_move(&s->session,8,7,&r)!=GK1_SESSION_OK){strcpy(s->status,"UNSUPPORTED STEP/COMMAND - NO GUESSED MECHANIC");return 1;}
   s->last_move=r;
   if(r==GK1_REJECTED)strcpy(s->status,"NATIVE CONTROL: MOVE REJECTED - NO STATE CHANGE");
   if(r==GK1_STEP_PENDING)strcpy(s->status,"NATIVE CONTROL: STEP PENDING - B FINISHES PATH");
   if(r==GK1_FINISHED)strcpy(s->status,"NATIVE CONTROL: BATTLE MOVE COMPLETED");
   return 1;
  }
  if(cmd==GK1_INPUT_CANCEL){
   Gk1MoveResult r=GK1_UNVERIFIED;
   if(s->has_session && s->session.events==1 &&
      gk1_session_finish(&s->session,&r)==GK1_SESSION_OK){s->last_move=r;strcpy(s->status,"NATIVE CONTROL: PENDING MOVE COMPLETED BY B");}
   else {s->screen=GK1_SCREEN_TITLE;s->menu_index=1;}
   return 1;
  }
  return 1;
 default:return 0;
 }
}

/* Restore only one of the six source-bounded observed case profiles.  A
 * hostile forged in-memory state is independently validated by encode/decode.
 */
int gk1_app_restore_session(Gk1App *s,const Gk1BattleSession *loaded){
 if(!s || !loaded)return 0;
 uint8_t b[GK1_SESSION_SAVE_BYTES];Gk1BattleSession verified;
 if(gk1_session_encode(loaded,b,sizeof b)!=GK1_SESSION_OK ||
    gk1_session_decode(b,sizeof b,&verified)!=GK1_SESSION_OK)return 0;
 size_t i;
 for(i=0;i<CASE_COUNT;i++){
  if(cases[i].mobility==verified.initial_mobility &&
     cases[i].terrain==verified.initial_terrain &&
     (uint8_t)cases[i].occupant==verified.initial_occupant)break;
 }
 if(i==CASE_COUNT)return 0;
 Gk1App next;gk1_app_init(&next);
 next.screen=GK1_SCREEN_RESEARCH_BATTLE;
 next.menu_index=1;next.case_index=(uint8_t)i;
 next.has_session=1;next.session=verified;
 next.last_move=verified.events==0?GK1_UNVERIFIED:
  (verified.events==2?GK1_FINISHED:(Gk1MoveResult)verified.first_result);
 strcpy(next.status,"RESEARCH REPLAY RESTORED - NO CAMPAIGN STATE");
 *s=next;return 1;
}

/* Replay a checked script into fresh local state before committing UI changes.
 * This guarantees a forged script cannot inject arbitrary soldiers/units. */
int gk1_app_restore_path(Gk1App *s,uint8_t scenario,const uint8_t *events,uint8_t count){
 if(!s || scenario<1 || scenario>3 || count>4 || (count && !events))return 0;
 Gk1App next;gk1_app_init(&next);next.path_case=(uint8_t)(scenario-1);
 next.screen=GK1_SCREEN_PATH_PICK;
 if(gk1_app_input(&next,GK1_INPUT_CONFIRM)!=1 || next.screen!=GK1_SCREEN_PATH_BATTLE)return 0;
 for(uint8_t i=0;i<count;i++){
  uint8_t e=events[i];
  Gk45Result r=GK45_UNKNOWN;
  if(e==1)r=gk45_step_first(&next.path);
  else if(e==2)r=gk45_step_second(&next.path);
  else if(e==3)r=gk45_press_b(&next.path);
  if(r!=GK45_ACCEPTED && r!=GK45_REFUSED && r!=GK45_FINISHED)return 0;
  next.path_events[next.path_event_count++]=e;
 }
 next.cursor_x=next.path.x;next.cursor_y=next.path.y;
 strcpy(next.status,"COLD-LOADED VALIDATED MODIFIED-ROM PATH REPLAY");
 *s=next;return 1;
}
