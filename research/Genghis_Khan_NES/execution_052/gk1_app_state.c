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
static const uint8_t divide_qty[4]={1,6,7,8};
void gk1_app_init(Gk1App *s){if(!s)return;memset(s,0,sizeof *s);s->screen=GK1_SCREEN_TITLE;s->last_move=GK1_UNVERIFIED;s->setup_candidate[0]=s->setup_candidate[1]=255;s->rules_profile=1;strcpy(s->status,"SOURCE-BOUNDED RESEARCH BUILD - NOT FULL GAME");}
static void configure_case(Gk1App *s){
 const Case *c=&cases[s->case_index];
 Gk1BattleSession fresh;
 if(gk1_session_open_fixture(c->mobility,c->terrain,c->occupant,&fresh)==GK1_SESSION_OK){
  s->session=fresh;s->has_session=1;s->last_move=GK1_UNVERIFIED;
  strcpy(s->status,"A: try native-recorded step (8,7). B: back or finish.");
 } else {s->has_session=0;strcpy(s->status,"CASE UNAVAILABLE: SOURCE EVIDENCE REQUIRED");}
}
/* Country state belongs to this SAME native game, not a second standalone CLI.
 * Installing a dataset never alters the active battle or save/replay objects.
 * PRE_SETUP means first playable seasonal setup is explicitly NOT implemented. */
int gk1_app_install_templates(Gk1App *s,const Gk1TemplateDataset *dataset){
 if(!s||!dataset)return 0;
 Gk1LoadedScenario a,b;
 if(gk1_open_loaded_scenario(dataset,GK1_MONGOL_CONQUEST,&a)!=GK1_LOAD_OK ||
    gk1_open_loaded_scenario(dataset,GK1_WORLD_CONQUEST,&b)!=GK1_LOAD_OK)return 0;
 s->worlds[0]=a;s->worlds[1]=b;
 s->world_ready[0]=1;s->world_ready[1]=1;
 s->country_cursor[0]=s->country_cursor[1]=0;
 s->active_world=0;s->setup_candidate[0]=255;s->setup_candidate[1]=255;
 return 1;
}
const Gk1CountryTemplate *gk1_app_selected_country(const Gk1App *s){
 if(!s||s->screen!=GK1_SCREEN_SCENARIO_COUNTRIES||s->active_world>1||
    !s->world_ready[s->active_world])return NULL;
 const Gk1LoadedScenario *world=&s->worlds[s->active_world];
 uint8_t c=s->country_cursor[s->active_world];
 if(world->phase!=GK1_PRE_SETUP_TEMPLATE||!world->country_count||c>=world->country_count)return NULL;
 return &world->countries[c];
}
int gk1_app_input(Gk1App *s,Gk1Input cmd){
 if(!s || cmd<GK1_INPUT_UP || cmd>GK1_INPUT_CANCEL)return 0;
 switch(s->screen){
 case GK1_SCREEN_TITLE:
  if(cmd==GK1_INPUT_UP){s->menu_index=(uint8_t)((s->menu_index+3)%4);return 1;}
  if(cmd==GK1_INPUT_DOWN){s->menu_index=(uint8_t)((s->menu_index+1)%4);return 1;}
  if(cmd==GK1_INPUT_CONFIRM){
    if(s->menu_index!=0 && s->rules_profile==0){
      strcpy(s->status,"CLASSIC STOCK: ENHANCED MOD-ONLY TACTICS ARE UNAVAILABLE");
      return 1;
    }
    s->screen=s->menu_index==3?GK1_SCREEN_DIVIDE_PICK:(s->menu_index==2?GK1_SCREEN_PATH_PICK:(s->menu_index?GK1_SCREEN_RESEARCH_BATTLE:GK1_SCREEN_CAMPAIGNS));s->menu_index=0;
    if(s->screen==GK1_SCREEN_RESEARCH_BATTLE)configure_case(s);
    return 1;}
  return 1;
 case GK1_SCREEN_CAMPAIGNS:
  if(cmd==GK1_INPUT_LEFT||cmd==GK1_INPUT_RIGHT){
    s->rules_profile=(uint8_t)(1-s->rules_profile);
    strcpy(s->status,s->rules_profile?"ENHANCED: APPROVED MODIFIED-ROM RULE PROFILE":
      "CLASSIC: STOCK ROM RULE PROFILE; MOD FIXTURES GATED");return 1;
  }
  if(cmd==GK1_INPUT_UP||cmd==GK1_INPUT_DOWN){s->menu_index=(uint8_t)(1-s->menu_index);return 1;}
  if(cmd==GK1_INPUT_CANCEL){s->screen=GK1_SCREEN_TITLE;s->menu_index=0;return 1;}
  if(cmd==GK1_INPUT_CONFIRM){
    uint8_t which=s->menu_index;
    if(s->world_ready[which] && s->worlds[which].phase==GK1_PRE_SETUP_TEMPLATE){
     s->active_world=which;s->screen=GK1_SCREEN_SCENARIO_COUNTRIES;
     strcpy(s->status,"PRE-SETUP COUNTRY TEMPLATES - NOT ORIGINAL PLAYABLE SPRING");
    } else {
     s->screen=GK1_SCREEN_UNIMPLEMENTED;
     strcpy(s->status,"NOT IMPLEMENTED: VERIFIED CSV MISSING OR PLAYER SETUP OPEN");
    }
    return 1;
   }
  return 1;
 case GK1_SCREEN_SCENARIO_COUNTRIES:{
  if(s->active_world>1 || !s->world_ready[s->active_world])return 0;
  uint8_t count=s->worlds[s->active_world].country_count;
  if(count==0)return 0;
  uint8_t *cursor=&s->country_cursor[s->active_world];
  if(cmd==GK1_INPUT_UP||cmd==GK1_INPUT_LEFT){*cursor=(uint8_t)((*cursor+count-1)%count);return 1;}
  if(cmd==GK1_INPUT_DOWN||cmd==GK1_INPUT_RIGHT){*cursor=(uint8_t)((*cursor+1)%count);return 1;}
  if(cmd==GK1_INPUT_CANCEL){s->screen=GK1_SCREEN_CAMPAIGNS;s->menu_index=s->active_world;return 1;}
  if(cmd==GK1_INPUT_CONFIRM){
    uint8_t previous=s->setup_candidate[s->active_world];
    s->setup_candidate[s->active_world]=(previous==*cursor)?255:*cursor;
    strcpy(s->status,"CANDIDATE ONLY: ORIGINAL SETUP NOT YET PROVEN");
    return 1;
  }
  return 1;
 }
 case GK1_SCREEN_UNIMPLEMENTED:
  if(cmd==GK1_INPUT_CANCEL||cmd==GK1_INPUT_CONFIRM){s->screen=GK1_SCREEN_CAMPAIGNS;s->menu_index=0;}return 1;
 case GK1_SCREEN_DIVIDE_PICK:
  if(cmd==GK1_INPUT_UP||cmd==GK1_INPUT_LEFT){s->divide_choice=(uint8_t)((s->divide_choice+3)%4);return 1;}
  if(cmd==GK1_INPUT_DOWN||cmd==GK1_INPUT_RIGHT){s->divide_choice=(uint8_t)((s->divide_choice+1)%4);return 1;}
  if(cmd==GK1_INPUT_CANCEL){s->screen=GK1_SCREEN_TITLE;s->menu_index=3;return 1;}
  if(cmd==GK1_INPUT_CONFIRM){
   if(gk47_open(&s->divide)!=GK47_ACCEPTED)return 0;
   s->divide_event_count=0;s->divide_cursor_x=7;s->divide_cursor_y=7;
   s->screen=GK1_SCREEN_DIVIDE_BATTLE;
   strcpy(s->status,"DIVIDE QUANTITY: DPAD CHOOSE PROVEN 1/6/7/8; A SELECT");return 1;
  }return 1;
 case GK1_SCREEN_DIVIDE_BATTLE:
  if(s->divide.phase==GK47_QUANTITY){
   if(cmd==GK1_INPUT_UP||cmd==GK1_INPUT_LEFT){s->divide_choice=(uint8_t)((s->divide_choice+3)%4);return 1;}
   if(cmd==GK1_INPUT_DOWN||cmd==GK1_INPUT_RIGHT){s->divide_choice=(uint8_t)((s->divide_choice+1)%4);return 1;}
   if(cmd==GK1_INPUT_CANCEL){s->screen=GK1_SCREEN_DIVIDE_PICK;return 1;}
   if(cmd==GK1_INPUT_CONFIRM){
    if(s->divide_event_count>=sizeof s->divide_events)return 1;
    uint8_t q=divide_qty[s->divide_choice];
    if(gk47_choose_quantity(&s->divide,q)!=GK47_ACCEPTED)return 1;
    s->divide_events[s->divide_event_count++]=q;
    s->divide_cursor_x=7;s->divide_cursor_y=7;
    strcpy(s->status,"DIVIDE DESTINATION: RIGHT THEN A, OR B ERROR");return 1;
   }return 1;
  }
  if(s->divide.phase==GK47_DESTINATION){
   if(cmd==GK1_INPUT_RIGHT&&s->divide_cursor_x<15){s->divide_cursor_x++;return 1;}
   if(cmd==GK1_INPUT_LEFT&&s->divide_cursor_x>0){s->divide_cursor_x--;return 1;}
   if(cmd==GK1_INPUT_UP&&s->divide_cursor_y>0){s->divide_cursor_y--;return 1;}
   if(cmd==GK1_INPUT_DOWN&&s->divide_cursor_y<9){s->divide_cursor_y++;return 1;}
   if(cmd==GK1_INPUT_CONFIRM){
    if(s->divide_event_count>=sizeof s->divide_events)return 1;
    Gk47Result r=gk47_move_to_observed_forest(&s->divide,s->divide_cursor_x,s->divide_cursor_y);
    if(r==GK47_ACCEPTED||r==GK47_DEAD_MESSAGE){
      s->divide_events[s->divide_event_count++]=20;
      strcpy(s->status,r==GK47_DEAD_MESSAGE?"SPLIT UNIT 3 BIT THE DUST - B DISMISS":"DIVIDE COMMITTED - B COMPLETES");
    }else strcpy(s->status,"UNVERIFIED DESTINATION: NO UNIT OR RNG CHANGE");
    return 1;
   }
  }
  if(cmd==GK1_INPUT_CANCEL){
   if(s->divide.phase==GK47_DONE){s->screen=GK1_SCREEN_DIVIDE_PICK;return 1;}
   if(s->divide_event_count>=sizeof s->divide_events)return 1;
   Gk47Phase old=s->divide.phase;
   Gk47Result r=gk47_press_b(&s->divide);
   if(r==GK47_ACCEPTED||r==GK47_REFUSED||r==GK47_FINISHED){
    s->divide_events[s->divide_event_count++]=21;
    if(old==GK47_DESTINATION_ERROR){
     /* Actual native UI goes back to quantity; no gameplay writes occurred.
      * The isolated 0047 adapter returns DESTINATION here. Reopen the same
      * byte-exact precommit state for the source-visible quantity screen. */
     if(gk47_open(&s->divide)!=GK47_ACCEPTED)return 0;
     strcpy(s->status,"B DISMISSED REFUSAL; QUANTITY MENU, NO DEBIT");
    }else if(r==GK47_REFUSED)strcpy(s->status,"CAN'T MOVE THERE - B DISMISS (NO DEBIT)");
    else if(r==GK47_FINISHED)strcpy(s->status,"DIVIDE COMPLETED, SOURCE MOBILITY 10, WORD 1");
    else strcpy(s->status,"B DISMISSED DEATH MESSAGE; ANOTHER B COMPLETES");
   }return 1;
  }
  return 1;
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

/* Loading an independent tactical research receipt must not erase unrelated
 * currently loaded original country data. Campaign saves are STILL absent. */
static void retain_original_templates(Gk1App *next,const Gk1App *old){
 memcpy(next->worlds,old->worlds,sizeof next->worlds);
 memcpy(next->world_ready,old->world_ready,sizeof next->world_ready);
 memcpy(next->country_cursor,old->country_cursor,sizeof next->country_cursor);
 next->active_world=old->active_world;
 memcpy(next->setup_candidate,old->setup_candidate,sizeof next->setup_candidate);
 next->rules_profile=old->rules_profile;
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
 retain_original_templates(&next,s);
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
 retain_original_templates(&next,s);
 *s=next;return 1;
}

/* Rebuild every accepted source event in temporary state and atomically replace
 * the app. This deliberately does not reconstruct arbitrary NES RNG or slots. */
int gk1_app_restore_divide(Gk1App *s,const uint8_t *events,uint8_t count,
                           uint8_t selected,uint8_t cursor_x,uint8_t cursor_y){
 if(!s||count>8||(count&&!events)||selected>3||cursor_x>=16||cursor_y>=10)return 0;
 Gk1App candidate;gk1_app_init(&candidate);
 candidate.menu_index=3;candidate.screen=GK1_SCREEN_DIVIDE_BATTLE;
 if(gk47_open(&candidate.divide)!=GK47_ACCEPTED)return 0;
 for(uint8_t i=0;i<count;i++){
  uint8_t e=events[i];Gk47Result result=GK47_UNKNOWN;
  Gk47Phase previous=candidate.divide.phase;
  if(e==1||e==6||e==7||e==8)result=gk47_choose_quantity(&candidate.divide,e);
  else if(e==20)result=gk47_move_to_observed_forest(&candidate.divide,8,7);
  else if(e==21)result=gk47_press_b(&candidate.divide);
  if(result!=GK47_ACCEPTED&&result!=GK47_REFUSED&&
     result!=GK47_DEAD_MESSAGE&&result!=GK47_FINISHED)return 0;
  if(e==21&&previous==GK47_DESTINATION_ERROR){
    if(gk47_open(&candidate.divide)!=GK47_ACCEPTED)return 0;
  }
  candidate.divide_events[candidate.divide_event_count++]=e;
 }
 candidate.divide_choice=selected;candidate.divide_cursor_x=cursor_x;
 candidate.divide_cursor_y=cursor_y;
 strcpy(candidate.status,"DIVIDE REPLAY RESTORED - APPROVED MODIFIED-ROM ONLY");
 retain_original_templates(&candidate,s);
 *s=candidate;return 1;
}
