#include "gk1_app_state.h"
#include "gk115_market_gate.h"
#include "gk134_stock_command_route.h"
#include "gk60_demobilize.h"
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
static const uint8_t world_player_lords[4]={0,6,10,18};
/* One canonical command table shared with controller routing, so TAX
 * cannot accidentally invoke the Enhanced-only ASSIGN mutation path. */
const char *gk87_stock_command_name(unsigned command){
 return gk134_command_name(command);
}

int gk53_setup_allowed_lord(uint8_t world,uint8_t country_index){
 if(world==0)return country_index==0;
 if(world==1)for(unsigned i=0;i<4;i++)if(world_player_lords[i]==country_index)return 1;
 return 0;
}
uint8_t gk53_setup_country_for_slot(uint8_t world,uint8_t slot){
 if(world==0&&slot==0)return 0;
 return world==1&&slot<4?world_player_lords[slot]:255;
}
static unsigned manual_spent(const Gk1App *s,uint8_t w){
 unsigned sum=0;
 for(unsigned i=0;i<5;i++)for(unsigned j=0;j<6;j++)sum+=s->manual_bonus[w][i][j];
 return sum;
}
static void reset_setup(Gk1App *s,uint8_t w){
 s->manual_bonus_active[w]=s->manual_bonus_person[w]=s->manual_bonus_ability[w]=0;
 memset(s->manual_bonus[w],0,sizeof s->manual_bonus[w]);
 s->setup_player_count[w]=1;s->setup_difficulty[w]=1;
 s->setup_stage[w]=GK53_SETUP_PLAYERS;s->setup_player_slot[w]=0;
 s->setup_ability_method[w]=0;s->setup_method_cursor[w]=0;
 for(unsigned i=0;i<4;i++)s->setup_rulers[w][i]=255;
}
#define CASE_COUNT (sizeof(cases)/sizeof(cases[0]))
const char *gk1_app_case_label(unsigned i){return i<CASE_COUNT?cases[i].label:"UNKNOWN";}
static const uint8_t divide_qty[4]={1,6,7,8};
void gk1_app_init(Gk1App *s){if(!s)return;memset(s,0,sizeof *s);s->screen=GK1_SCREEN_TITLE;s->last_move=GK1_UNVERIFIED;s->setup_candidate[0]=s->setup_candidate[1]=255;s->rules_profile=1;s->example_rate=100;s->manual_remaining_orders=3;s->whatif_event=GK65_ACT_COMPLETE;s->whatif_victim=GK65_DEFENDER_VICTIM;(void)gk68_rest_open(&s->rest_case);reset_setup(s,0);reset_setup(s,1);strcpy(s->status,"SOURCE-BOUNDED RESEARCH BUILD - NOT FULL GAME");}
Gk153TraceResult gk1_app_source_one_race_branch(const Gk1App *app,
 const char *rom,uint32_t origin,uint8_t choice,Gk83Trace *out){
 if(!app||!rom||!*rom||!out||app->active_world>1u||
    app->setup_stage[app->active_world]!=GK53_SETUP_AWAIT_SOURCE)
  return GK153_TRACE_BAD_STAGE;
 Gk83Trace result={0};
 Gk83Stop stop=gk154_trace_original_one_race_branch(rom,origin,choice,120000u,&result);
 if(stop==GK83_INVALID||stop==GK83_SOURCE_REFUSED)return GK153_TRACE_BAD_SOURCE;
 *out=result;
 return GK153_TRACE_BOUNDED;
}
Gk153TraceResult gk1_app_source_country_writer_trace(const Gk1App *app,
 const char *rom,Gk83Trace *out){
 if(!app||!rom||!*rom||!out||app->active_world>1u||
    app->setup_stage[app->active_world]!=GK53_SETUP_AWAIT_SOURCE)
  return GK153_TRACE_BAD_STAGE;
 Gk83Trace local={0};
 Gk83Stop status=gk151_trace_original_even_prerender(rom,0u,0u,120000u,&local);
 if(status==GK83_INVALID||status==GK83_SOURCE_REFUSED)
  return GK153_TRACE_BAD_SOURCE;
 *out=local;
 return GK153_TRACE_BOUNDED;
}
Gk152Status gk1_app_source_spring_writer_map(const Gk1App *app,
 const char *rom,Gk152WriterMap *out){
 if(!app||!rom||!out||app->active_world>1u||
    app->setup_stage[app->active_world]!=GK53_SETUP_AWAIT_SOURCE)
  return GK152_BAD_ARG;
 return gk152_inventory_original_rom(rom,out);
}
Gk151Status gk1_app_source_even_prerender(const Gk1App *app,
 const char *rom,Gk151Report *out){
 if(!app||!rom||!out||app->active_world>1u||
    app->setup_stage[app->active_world]!=GK53_SETUP_AWAIT_SOURCE)
  return GK151_INVALID;
 return gk151_scan_even_source(rom,out);
}
Gk145Status gk1_app_source_fresh_progress(const Gk1App *app,
 const Gk145FreshObservation *observations,size_t count,
 uint16_t original_year,uint16_t countries,Gk145FreshProgress *out){
 if(!app||app->active_world>1u||!app->world_ready[app->active_world]||
    app->worlds[app->active_world].phase!=GK1_PRE_SETUP_TEMPLATE||
    countries!=app->worlds[app->active_world].country_count)
  return GK145_ARGUMENT;
 return gk145_trace_fresh_progress(observations,count,original_year,countries,out);
}
Gk110Result gk1_app_source_spring_stage(const Gk1App *app,
 const char *verified_csv,Gk110SpringStage *out){
 if(!app||!verified_csv||!out||app->active_world>1u||
    !app->world_ready[app->active_world])return GK110_ARGUMENT;
 return gk110_prepare_source_finance(verified_csv,
          &app->worlds[app->active_world],out);
}
Gk111Result gk1_app_stock_spring_audit(const Gk1App *app,
 const char *csv,Gk111StockSpringAudit *out){
 if(!app||!csv||!out||!app->stock_checkpoint.valid)return GK111_ARGUMENT;
 const Gk79Checkpoint *stock=&app->stock_checkpoint;
 unsigned idx=stock->world.scenario_id==GK1_WORLD_CONQUEST?1u:0u;
 if(!app->world_ready[idx])return GK111_ARGUMENT;
 return gk111_compare_stock_spring_world(csv,&app->worlds[idx],stock,out);
}
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
Gk112Status gk1_app_stock_setup_deltas(const Gk1App *app,
 const char *csv,Gk112StockDeltas *out){
 if(!app||!csv||!out||!app->stock_checkpoint.valid)return GK112_ARGUMENT;
 const Gk79Checkpoint *stock=&app->stock_checkpoint;
 unsigned idx=(unsigned)stock->world.scenario_id-1u;
 if(idx>=2u||!app->world_ready[idx])return GK112_TEMPLATE_REFUSED;
 return gk112_compare_setup(csv,&app->worlds[idx],stock,out);
}
int gk1_app_install_templates(Gk1App *s,const Gk1TemplateDataset *dataset){
 if(!s||!dataset)return 0;
 Gk1LoadedScenario a,b;
 if(gk1_open_loaded_scenario(dataset,GK1_MONGOL_CONQUEST,&a)!=GK1_LOAD_OK ||
    gk1_open_loaded_scenario(dataset,GK1_WORLD_CONQUEST,&b)!=GK1_LOAD_OK)return 0;
 s->worlds[0]=a;s->worlds[1]=b;
 s->world_ready[0]=1;s->world_ready[1]=1;
 s->country_cursor[0]=s->country_cursor[1]=0;
 s->active_world=0;s->setup_candidate[0]=255;s->setup_candidate[1]=255;
 reset_setup(s,0);reset_setup(s,1);
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
Gk147GoodsStatus gk1_app_source_goods_view(const Gk1App *s,
 unsigned country_index,Gk147StockGoods *out){
 if(!s||!out)return GK147_GOODS_INVALID;
 if(s->screen!=GK1_SCREEN_STOCK_CONTINUE&&
    s->screen!=GK1_SCREEN_STOCK_COMMANDS&&
    s->screen!=GK1_SCREEN_STOCK_TRADE_REFERENCE)
   return GK147_GOODS_UNVERIFIED;
 return gk147_source_goods(&s->stock_checkpoint,&s->enhanced_assign,
     s->rules_profile,country_index,out);
}
int gk1_app_input(Gk1App *s,Gk1Input cmd){
 if(!s || cmd<GK1_INPUT_UP || cmd>GK1_INPUT_LABOR_PLAN)return 0;
 switch(s->screen){
 case GK1_SCREEN_TITLE:
  if(cmd==GK1_INPUT_UP){s->menu_index=(uint8_t)((s->menu_index+4)%5);return 1;}
  if(cmd==GK1_INPUT_DOWN){s->menu_index=(uint8_t)((s->menu_index+1)%5);return 1;}
  if(cmd==GK1_INPUT_CONFIRM){
    if(s->menu_index!=0 && s->rules_profile==0){
      strcpy(s->status,"CLASSIC STOCK: ENHANCED MOD-ONLY TACTICS ARE UNAVAILABLE");
      return 1;
    }
    s->screen=s->menu_index==4?GK1_SCREEN_REST_CASE:(s->menu_index==3?GK1_SCREEN_DIVIDE_PICK:(s->menu_index==2?GK1_SCREEN_PATH_PICK:(s->menu_index?GK1_SCREEN_RESEARCH_BATTLE:GK1_SCREEN_CAMPAIGNS)));s->menu_index=0;
    if(s->screen==GK1_SCREEN_REST_CASE){
      if(gk68_rest_open(&s->rest_case)!=GK68_REST_OK)return 0;
      strcpy(s->status,"APPROVED MOD SOURCE CASE: SPRING 1175 TWO-ORDER REST");
    }
    if(s->screen==GK1_SCREEN_RESEARCH_BATTLE)configure_case(s);
    return 1;}
  return 1;
 case GK1_SCREEN_REST_CASE:
  /* Source-observed approved modified ROM only: isolated CASE fixture.
   * Never mutate source 14/27 PRE_SETUP countries, scores or save slots. */
  if(s->rules_profile!=1)return 0;
  if(cmd==GK1_INPUT_CONFIRM){
    Gk68RestResult r=gk68_rest_press_a(&s->rest_case);
    if(r==GK68_REST_OK){
      strcpy(s->status,s->rest_case.stage==GK68_REST_CONFIRM?
        "REST CONFIRMATION: A YES / B CANCEL EXACT NO-MUTATION":
        "SOURCE CAPTURE: SUMMER 1175; 8000-FRAME RUNNER TIMED OUT AFTER CAPTURE");
    }else strcpy(s->status,"REST SOURCE CASE TERMINAL: NO ADDITIONAL SEASON INVENTED");
    return 1;
  }
  if(cmd==GK1_INPUT_CANCEL){
    if(s->rest_case.stage==GK68_REST_CONFIRM){
      if(gk68_rest_press_b(&s->rest_case)!=GK68_REST_OK)return 0;
      strcpy(s->status,"B CANCELED: SPRING CHECKPOINT BYTE-FOR-BYTE RESTORED");
    }else{s->screen=GK1_SCREEN_TITLE;s->menu_index=4;}
    return 1;
  }
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
  if(cmd==GK1_INPUT_LABOR_PLAN){
    s->labor_from=0;s->labor_to=1;s->screen=GK1_SCREEN_LABOR_PLAN;
    strcpy(s->status,"PORT PRE_SETUP LABOR PLANNING - NOT NES ASSIGN ORDER");return 1;
  }
  if(cmd==GK1_INPUT_CANCEL){s->screen=GK1_SCREEN_CAMPAIGNS;s->menu_index=s->active_world;return 1;}
  if(cmd==GK1_INPUT_CONFIRM){
    uint8_t previous=s->setup_candidate[s->active_world];
    s->setup_candidate[s->active_world]=(previous==*cursor)?255:*cursor;
    if(s->setup_candidate[s->active_world]!=previous)reset_setup(s,s->active_world);
    strcpy(s->status,"CANDIDATE ONLY: ORIGINAL SETUP NOT YET PROVEN");
    return 1;
  }
  if(cmd==GK1_INPUT_SETUP){
    uint8_t w=s->active_world, ruler=s->setup_candidate[w];
    if(ruler==255){strcpy(s->status,"SELECT A COUNTRY CANDIDATE BEFORE SETUP");return 1;}
    if(!gk53_setup_allowed_lord(w,ruler)){
      strcpy(s->status,"NES MANUAL: THIS RULER IS NOT A SELECTABLE PLAYER LORD");return 1;
    }
    s->screen=GK1_SCREEN_SETUP_WIZARD;
    strcpy(s->status,"SOURCE-MANUAL SETUP: START DOES NOT BEGIN PLAYABLE SPRING");
    return 1;
  }
  return 1;
 }
 case GK1_SCREEN_LABOR_PLAN:{
  /* An editable port planning state, not original first Spring and NOT an
   * NES Assign order execution. Country total is conserved on every transfer.
   * Mutation validates first and otherwise leaves all numeric fields alone. */
  if(s->active_world>1||!s->world_ready[s->active_world])return 0;
  Gk1LoadedScenario *world=&s->worlds[s->active_world];
  if(world->phase!=GK1_PRE_SETUP_TEMPLATE || !world->country_count)return 0;
  uint8_t selected=s->country_cursor[s->active_world];
  if(selected>=world->country_count)return 0;
  Gk1CountryTemplate *country=&world->countries[selected];
  uint16_t *fields[5]={&country->troops,&country->town,&country->masons,
                        &country->artisans,&country->food_workers};
  if(cmd==GK1_INPUT_LABOR_PLAN){
    s->screen=GK1_SCREEN_MARKET_EXPLORER;
    s->market_merchant=0;s->market_item=0;s->market_sell=0;
    if(s->example_rate<50||s->example_rate>999)s->example_rate=100;
    strcpy(s->status,"MANUAL-DERIVED QUOTE EXPLORER: NO REAL MERCHANT/TRADE");return 1;
  }
  if(cmd==GK1_INPUT_SETUP){
    if(s->rules_profile!=1){
      strcpy(s->status,"CLASSIC STOCK: ENHANCED DEMOBILIZE UNAVAILABLE");return 1;
    }
    s->demob_quantity=gk60_demob_max(country)?1u:0u;
    s->screen=GK1_SCREEN_DEMOBILIZE;
    strcpy(s->status,"ENHANCED DEMOBILIZE PREVIEW - NOT A PLAYABLE NES TURN");
    return 1;
  }
  if(cmd==GK1_INPUT_CANCEL){s->screen=GK1_SCREEN_SCENARIO_COUNTRIES;
    strcpy(s->status,"PRE_SETUP LABOR PLAN SAVED IN MUTABLE WORLD ONLY");return 1;}
  if(cmd==GK1_INPUT_LEFT){s->labor_from=(uint8_t)((s->labor_from+4)%5);return 1;}
  if(cmd==GK1_INPUT_RIGHT){s->labor_from=(uint8_t)((s->labor_from+1)%5);return 1;}
  if(cmd==GK1_INPUT_UP){s->labor_to=(uint8_t)((s->labor_to+4)%5);return 1;}
  if(cmd==GK1_INPUT_DOWN){s->labor_to=(uint8_t)((s->labor_to+1)%5);return 1;}
  if(cmd==GK1_INPUT_CONFIRM){
    uint16_t *from=fields[s->labor_from],*to=fields[s->labor_to];
    if(from==to || *from==0 || *to==UINT16_MAX){
      strcpy(s->status,"LABOR PLAN REFUSED: SAME POOL / EMPTY / OVERFLOW");return 1;
    }
    /* Population fields remain unmodified; only within-pool assignment. */
    --(*from);++(*to);
    strcpy(s->status,"ONE DRAFTED WORKER REASSIGNED - PRE_SETUP ONLY");return 1;
  }
  return 1;
 }
 case GK1_SCREEN_DEMOBILIZE:{
  /* The NES-approved modified-ROM number transfer is source-proven, but this
   * interface deliberately remains a PRE_SETUP research preview. Never turn
   * its state into fictional original Spring or assign undrafted labor. */
  if(s->active_world>1||!s->world_ready[s->active_world]||s->rules_profile!=1)return 0;
  Gk1LoadedScenario *world=&s->worlds[s->active_world];
  uint8_t ix=s->country_cursor[s->active_world];
  if(world->phase!=GK1_PRE_SETUP_TEMPLATE||ix>=world->country_count)return 0;
  Gk1CountryTemplate *country=&world->countries[ix];
  if(cmd==GK1_INPUT_CANCEL||cmd==GK1_INPUT_SETUP){
    s->screen=GK1_SCREEN_LABOR_PLAN;
    strcpy(s->status,"ENHANCED DEMOBILIZE PREVIEW CLOSED - PRE_SETUP ONLY");return 1;
  }
  uint16_t max=gk60_demob_max(country);
  if(s->demob_quantity>max)s->demob_quantity=max;
  if(cmd==GK1_INPUT_UP||cmd==GK1_INPUT_RIGHT){
    unsigned step=(cmd==GK1_INPUT_RIGHT)?10u:1u;
    s->demob_quantity=(uint16_t)(max-(unsigned)s->demob_quantity<step?
      max:s->demob_quantity+step);return 1;
  }
  if(cmd==GK1_INPUT_DOWN||cmd==GK1_INPUT_LEFT){
    unsigned step=(cmd==GK1_INPUT_LEFT)?10u:1u;
    s->demob_quantity=(uint16_t)((unsigned)s->demob_quantity>step?s->demob_quantity-step:
       (max?1u:0u));return 1;
  }
  if(cmd==GK1_INPUT_CONFIRM){
    if(gk60_demob_apply(country,s->demob_quantity)!=GK60_DEMOB_OK){
      strcpy(s->status,"DEMOBILIZE REFUSED: ZERO / CAPACITY / INVALID STATE");return 1;
    }
    max=gk60_demob_max(country);
    s->demob_quantity=max?1u:0u;
    strcpy(s->status,"ENHANCED DEMOBILIZE PREVIEW COMMITTED - NOT SPRING");return 1;
  }
  return 1;
 }
 case GK1_SCREEN_SETUP_WIZARD:{
  uint8_t w=s->active_world,stage=s->setup_stage[w];
  if(w>1||!s->world_ready[w]||s->worlds[w].phase!=GK1_PRE_SETUP_TEMPLATE)return 0;
  /* Original manual p.8-9 verifies 6 categories, Lord+4 princes, A +1,
   * B +10. Baseline character values and actual per-person award math need
   * original runtime confirmation. Therefore this is an editable, bounded
   * +100 DRAFT only, NEVER authoritative character initialization. */
  if(stage==GK53_SETUP_AWAIT_SOURCE && cmd==GK1_INPUT_LABOR_PLAN &&
     !s->manual_bonus_active[w]){
     s->manual_remaining_orders=3;s->screen=GK1_SCREEN_MANUAL_RULES;
     strcpy(s->status,"MANUAL-BASED SPRING DEMAND - READ ONLY, NOT PLAYABLE");return 1;
  }
  if(stage==GK53_SETUP_AWAIT_SOURCE && s->setup_ability_method[w]==2){
    if(cmd==GK1_INPUT_SETUP){
      s->manual_bonus_active[w]^=1;
      strcpy(s->status,s->manual_bonus_active[w]?
        "MANUAL BONUS DRAFT ACTIVE: A +1 / B +10 / START EXIT":
        "MANUAL BONUS DRAFT STORED; ORIGINAL BASE STATS UNVERIFIED");
      return 1;
    }
    if(s->manual_bonus_active[w]){
      if(cmd==GK1_INPUT_LEFT){s->manual_bonus_person[w]=(uint8_t)((s->manual_bonus_person[w]+4)%5);return 1;}
      if(cmd==GK1_INPUT_RIGHT){s->manual_bonus_person[w]=(uint8_t)((s->manual_bonus_person[w]+1)%5);return 1;}
      if(cmd==GK1_INPUT_UP){s->manual_bonus_ability[w]=(uint8_t)((s->manual_bonus_ability[w]+5)%6);return 1;}
      if(cmd==GK1_INPUT_DOWN){s->manual_bonus_ability[w]=(uint8_t)((s->manual_bonus_ability[w]+1)%6);return 1;}
      if(cmd==GK1_INPUT_DRAFT_UNDO_ONE||cmd==GK1_INPUT_DRAFT_UNDO_TEN){
        uint8_t *v=&s->manual_bonus[w][s->manual_bonus_person[w]][s->manual_bonus_ability[w]];
        unsigned n=cmd==GK1_INPUT_DRAFT_UNDO_ONE?1u:10u;
        *v=(uint8_t)(*v>(unsigned)n?*v-n:0u);
        strcpy(s->status,"BONUS DRAFT CORRECTION ONLY; NO NES STAT CLAIM");return 1;
      }
      if(cmd==GK1_INPUT_CONFIRM||cmd==GK1_INPUT_CANCEL){
        unsigned amount=cmd==GK1_INPUT_CONFIRM?1u:10u;
        uint8_t *v=&s->manual_bonus[w][s->manual_bonus_person[w]][s->manual_bonus_ability[w]];
        if(manual_spent(s,w)+amount<=100 && (unsigned)*v+amount<=99){
          *v=(uint8_t)(*v+amount);
          strcpy(s->status,"RECORDED BONUS DRAFT ONLY; NO BASE STATS OR SPRING");
        }else strcpy(s->status,"BONUS DRAFT LIMIT REACHED: NO STATE CHANGE");
        return 1;
      }
    }
  }
  if(cmd==GK1_INPUT_CANCEL){
    if(stage==GK53_SETUP_PLAYERS)s->screen=GK1_SCREEN_SCENARIO_COUNTRIES;
    else if(stage==GK53_SETUP_AWAIT_SOURCE){
      s->setup_stage[w]=GK53_SETUP_ABILITIES;s->setup_ability_method[w]=0;
      s->manual_bonus_active[w]=s->manual_bonus_person[w]=s->manual_bonus_ability[w]=0;
      memset(s->manual_bonus[w],0,sizeof s->manual_bonus[w]);
    }else if(stage==GK53_SETUP_LORD&&s->setup_player_slot[w]>0){
      s->setup_player_slot[w]--;s->setup_rulers[w][s->setup_player_slot[w]]=255;
    }else{
      s->setup_stage[w]=(uint8_t)(stage-1);
      if(stage==GK53_SETUP_ABILITIES){
        s->setup_player_slot[w]=(uint8_t)(s->setup_player_count[w]-1);
        s->setup_rulers[w][s->setup_player_slot[w]]=255;
      }
    }
    return 1;
  }
  if(stage==GK53_SETUP_PLAYERS){
    if(w==1&&(cmd==GK1_INPUT_UP||cmd==GK1_INPUT_RIGHT)){
      s->setup_player_count[w]=(uint8_t)(s->setup_player_count[w]%4+1);return 1;
    }
    if(w==1&&(cmd==GK1_INPUT_DOWN||cmd==GK1_INPUT_LEFT)){
      s->setup_player_count[w]=(uint8_t)((s->setup_player_count[w]+2)%4+1);return 1;
    }
    if(cmd==GK1_INPUT_CONFIRM){
      s->setup_player_slot[w]=0;
      for(unsigned i=0;i<4;i++)s->setup_rulers[w][i]=255;
      s->country_cursor[w]=s->setup_candidate[w];
      s->setup_stage[w]=GK53_SETUP_DIFFICULTY;return 1;
    }
    return 1;
  }
  if(stage==GK53_SETUP_DIFFICULTY){
    if(cmd==GK1_INPUT_UP||cmd==GK1_INPUT_RIGHT){
      s->setup_difficulty[w]=(uint8_t)(s->setup_difficulty[w]%5+1);return 1;
    }
    if(cmd==GK1_INPUT_DOWN||cmd==GK1_INPUT_LEFT){
      s->setup_difficulty[w]=(uint8_t)((s->setup_difficulty[w]+3)%5+1);return 1;
    }
    if(cmd==GK1_INPUT_CONFIRM){
      s->country_cursor[w]=s->setup_candidate[w];
      s->setup_stage[w]=GK53_SETUP_LORD;return 1;
    }
    return 1;
  }
  if(stage==GK53_SETUP_LORD){
    uint8_t player=s->setup_player_slot[w];
    if(player>=s->setup_player_count[w])return 0;
    if(w==0){s->country_cursor[w]=0;}
    else if(cmd==GK1_INPUT_UP||cmd==GK1_INPUT_RIGHT){
      uint8_t ix=0;for(;ix<4;ix++)if(world_player_lords[ix]==s->country_cursor[w])break;
      s->country_cursor[w]=world_player_lords[(ix+1)%4];return 1;
    }else if(cmd==GK1_INPUT_DOWN||cmd==GK1_INPUT_LEFT){
      uint8_t ix=0;for(;ix<4;ix++)if(world_player_lords[ix]==s->country_cursor[w])break;
      s->country_cursor[w]=world_player_lords[(ix+3)%4];return 1;
    }
    if(cmd==GK1_INPUT_CONFIRM){
      uint8_t candidate=s->country_cursor[w];
      if(!gk53_setup_allowed_lord(w,candidate)){
        strcpy(s->status,"INVALID PLAYER LORD: USE LISTED MANUAL FOUR");return 1;
      }
      if(player==0 && candidate!=s->setup_candidate[w]){
        strcpy(s->status,"FIRST LORD MUST MATCH SELECTED CANDIDATE");return 1;
      }
      for(unsigned i=0;i<player;i++)if(s->setup_rulers[w][i]==candidate){
        strcpy(s->status,"DUPLICATE PLAYER LORD REFUSED");return 1;
      }
      s->setup_rulers[w][player]=candidate;
      if(++s->setup_player_slot[w]>=s->setup_player_count[w]){
        s->setup_stage[w]=GK53_SETUP_ABILITIES;
      }else{
        for(unsigned j=0;j<4;j++){
          uint8_t pick=world_player_lords[j];int used=0;
          for(unsigned k=0;k<=player;k++)if(s->setup_rulers[w][k]==pick)used=1;
          if(!used){s->country_cursor[w]=pick;break;}
        }
      }
      return 1;
    }
    return 1;
  }
  if(stage==GK53_SETUP_ABILITIES){
    if(cmd==GK1_INPUT_UP||cmd==GK1_INPUT_DOWN||cmd==GK1_INPUT_LEFT||cmd==GK1_INPUT_RIGHT){
      s->setup_method_cursor[w]^=1;return 1;
    }
    if(cmd==GK1_INPUT_CONFIRM){
      s->setup_ability_method[w]=(uint8_t)(s->setup_method_cursor[w]+1);
      s->setup_stage[w]=GK53_SETUP_AWAIT_SOURCE;
      strcpy(s->status,"STATS NOT GENERATED: SOURCE NES RANDOM/100-POINT SETUP OPEN");
    }
    return 1;
  }
  /* A recorded draft never produces original attributes or Spring. */
  if(cmd==GK1_INPUT_CONFIRM||cmd==GK1_INPUT_SETUP){
    strcpy(s->status,"BLOCKED: ORIGINAL CHARACTER BASE STATS / SPRING OPEN");
  }
  return 1;
 }
 case GK1_SCREEN_MARKET_EXPLORER:
  if(cmd==GK1_INPUT_CANCEL){s->screen=GK1_SCREEN_LABOR_PLAN;return 1;}
  if(cmd==GK1_INPUT_LEFT){s->market_merchant=(uint8_t)((s->market_merchant+2u)%3u);return 1;}
  if(cmd==GK1_INPUT_RIGHT){s->market_merchant=(uint8_t)((s->market_merchant+1u)%3u);return 1;}
  if(cmd==GK1_INPUT_UP){s->market_item=(uint8_t)((s->market_item+16u)%17u);s->market_sell=0;return 1;}
  if(cmd==GK1_INPUT_DOWN){s->market_item=(uint8_t)((s->market_item+1u)%17u);s->market_sell=0;return 1;}
  if(cmd==GK1_INPUT_CONFIRM){
   if(s->market_item>=GK61_SELLABLE_COUNT){strcpy(s->status,"UNSELLABLE ITEM: NO QUOTE OR STATE CHANGE");return 1;}
   s->market_sell^=1u;return 1;
  }
  if(cmd==GK1_INPUT_SETUP){s->example_rate=(uint16_t)(s->example_rate>=990u?50u:s->example_rate+10u);return 1;}
  if(cmd==GK1_INPUT_LABOR_PLAN){s->example_rate=(uint16_t)(s->example_rate<=59u?999u:s->example_rate-10u);return 1;}
  return 1;
 case GK1_SCREEN_MANUAL_RULES:
  if(cmd==GK1_INPUT_CONFIRM){s->spring_archive_cursor=0;s->spring_preview_mode=0;s->spring_stock_mode=0;s->screen=GK1_SCREEN_SPRING_ARCHIVE;
   strcpy(s->status,"ARCHIVED 1175 CAPTURE ONLY: NO ORIGINAL SPRING TRANSFORM");return 1;}
  /* View invokes an actually wired controller-only manual order-budget ledger.
   * A is deliberately left free for the inherited EXEC0070 archived Spring
   * controller route when that later source is rebased; no new campaign turn.
   */
  if(cmd==GK1_INPUT_LABOR_PLAN){
   unsigned w=s->active_world;
   if(w>1||!s->world_ready[w])return 0;
   Gk71Budget next;
   Gk71Result res=gk71_open(&s->worlds[w],s->setup_candidate[w],
                           GK71_SPRING,GK71_BASE,&next);
   if(res!=GK71_LOCKED && res!=GK71_OK)return 0;
   s->order_budget_research=next;
   s->screen=GK1_SCREEN_ORDER_BUDGET_RESEARCH;
   strcpy(s->status,"MANUAL ORDER BUDGET VIEW - WORLD STILL PRE_SETUP");
   return 1;
  }
  if(cmd==GK1_INPUT_CANCEL){s->screen=GK1_SCREEN_SETUP_WIZARD;return 1;}
  if(cmd==GK1_INPUT_LEFT||cmd==GK1_INPUT_DOWN){s->manual_remaining_orders=(uint8_t)(s->manual_remaining_orders==1?3:s->manual_remaining_orders-1);return 1;}
  if(cmd==GK1_INPUT_RIGHT||cmd==GK1_INPUT_UP){s->manual_remaining_orders=(uint8_t)(s->manual_remaining_orders==3?1:s->manual_remaining_orders+1);return 1;}
  if(cmd==GK1_INPUT_SETUP){
   if(gk65_begin(s->manual_remaining_orders,&s->battle_timeline)!=GK65_OK)return 0;
   s->screen=GK1_SCREEN_BATTLE_TIMELINE;
   (void)gk72_pass_begin(&s->battle_pass,12,10);
   s->whatif_event=GK65_ACT_COMPLETE;s->whatif_victim=GK65_DEFENDER_VICTIM;
   strcpy(s->status,"WHAT-IF: SELECT EXPLICIT MANUAL EVENT; NOT NES OBSERVED");return 1;
  }
  return 1;
 case GK1_SCREEN_SPRING_ARCHIVE:
  if(cmd==GK1_INPUT_LABOR_PLAN){
   if(s->spring_stock_count!=14&&s->spring_stock_count!=27){
    strcpy(s->status,"NO EXACT ORIGINAL STOCK CHECKPOINT ATTACHED: READ-ONLY");return 1;
   }
   s->spring_stock_mode^=1;s->spring_preview_mode=0;s->spring_archive_cursor=0;
   strcpy(s->status,s->spring_stock_mode?"AUTHENTIC STOCK SPRING RECEIPT; READ-ONLY, NOT GAME INIT":
    "RETURNED TO PREVIOUS RESEARCH ARCHIVE; NO WORLD MUTATION");return 1;
  }
  if(cmd==GK1_INPUT_SETUP){s->spring_stock_mode=0;s->spring_preview_mode^=1;
   if(s->spring_archive_cursor>=14)s->spring_archive_cursor=0;
   strcpy(s->status,"SOURCE FOOD/TAX/PAY WHAT-IF - PRE_SETUP NOT LIVE SPRING");return 1;}
  if(cmd==GK1_INPUT_CANCEL){s->screen=GK1_SCREEN_MANUAL_RULES;return 1;}
  unsigned cap=s->spring_stock_mode?s->spring_stock_count:
    s->spring_preview_mode&&s->active_world<2u&&s->world_ready[s->active_world]?
      s->worlds[s->active_world].country_count:14u;
  if(cap==0u)return 0;
  if(cmd==GK1_INPUT_DOWN||cmd==GK1_INPUT_RIGHT){s->spring_archive_cursor=(uint8_t)((s->spring_archive_cursor+1u)%cap);return 1;}
  if(cmd==GK1_INPUT_UP||cmd==GK1_INPUT_LEFT){s->spring_archive_cursor=(uint8_t)((s->spring_archive_cursor+cap-1u)%cap);return 1;}
  return 1;
 case GK1_SCREEN_STOCK_COMMANDS:{
  Gk79Checkpoint *cp=&s->stock_checkpoint;
  if(!cp->valid || !gk85_raw_source_intact(cp))return 0;
  /* Corresponds exactly to the two seven-row columns rendered in X11.
   * D-pad vertical navigation stays in its column; horizontal changes column.
   * Every actual move disarms PASS so a stray second A cannot commit. */
  if(cmd==GK1_INPUT_UP || cmd==GK1_INPUT_DOWN){
   unsigned col=s->stock_command_cursor/7u,row=s->stock_command_cursor%7u;
   row=(row+(cmd==GK1_INPUT_UP?6u:1u))%7u;
   s->stock_command_cursor=(uint8_t)(col*7u+row);
   cp->pending_pass=0;return 1;
  }
  if(cmd==GK1_INPUT_LEFT || cmd==GK1_INPUT_RIGHT){
   if(cmd==GK1_INPUT_LEFT && s->stock_command_cursor>=7u)
     s->stock_command_cursor=(uint8_t)(s->stock_command_cursor-7u);
   if(cmd==GK1_INPUT_RIGHT && s->stock_command_cursor<7u)
     s->stock_command_cursor=(uint8_t)(s->stock_command_cursor+7u);
   cp->pending_pass=0;return 1;
  }
  if(cmd==GK1_INPUT_CANCEL){
   if(cp->pending_pass){cp->pending_pass=0;
    strcpy(s->status,"PASS CANCELED: SOURCE AND ORDERS UNCHANGED");return 1;
   }
   s->screen=GK1_SCREEN_STOCK_CONTINUE;
   strcpy(s->status,"ORIGINAL COMMAND MENU CLOSED; SOURCE COUNTRIES UNCHANGED");return 1;
  }
  if(cmd==GK1_INPUT_CONFIRM){
   const Gk134CommandRoute route=gk134_command_route(s->stock_command_cursor);
   if(route==GK134_ROUTE_VIEW){
    cp->pending_pass=0;s->screen=GK1_SCREEN_STOCK_CONTINUE;
    strcpy(s->status,"VIEW: ORIGINAL COUNTRIES / NO ORDER SPENT");return 1;
   }
   if(route==GK134_ROUTE_ENHANCED_ASSIGN){
    /* TAX (0) must never enter this path. Original Assign remains locked. This is approved ENHANCED labor only,
     * grounded in a validated original stock checkpoint, never PRE_SETUP. */
    if(!s->rules_profile){strcpy(s->status,"CLASSIC ASSIGN LOCKED: ORIGINAL RULES UNIMPLEMENTED");return 1;}
    if(!gk116_attestation_intact(&s->enhanced_assign,cp)){
     Gk116EnhancedAssign candidate;
     Gk116Result r=gk116_open(cp,1u,&candidate);
     if(r!=GK116_OK){strcpy(s->status,"ENHANCED ASSIGN REQUIRES VERIFIED ORIGINAL STOCK SPRING");return 1;}
     s->enhanced_assign=candidate;
    }
    s->assign_from=0;s->assign_to=4;s->assign_quantity=1u;
    s->screen=GK1_SCREEN_ENHANCED_ASSIGN;
    strcpy(s->status,"ENHANCED ASSIGN: UP/DOWN FROM, LEFT/RIGHT TO, A MOVE 1, B BACK");
    return 1;
   }
   if(route==GK134_ROUTE_TRADE_REFERENCE){
    if(!cp->source_verified||!gk85_raw_source_intact(cp)||
       !gk88_stock_projection_intact(cp)){
     cp->pending_pass=0;
     strcpy(s->status,"TRADE PRICE REFERENCE REFUSED: ORIGINAL SOURCE UNVERIFIED");return 1;
    }
    cp->pending_pass=0;
    s->stock_trade_merchant=s->stock_trade_item=s->stock_trade_sell=0;
    s->screen=GK1_SCREEN_STOCK_TRADE_REFERENCE;
    strcpy(s->status,"STOCK RATE PRICE REFERENCE ONLY: NO MERCHANT PRESENCE / NO ORDER");return 1;
   }
   if(route==GK134_ROUTE_PASS){
    uint8_t remaining=0u;
    Gk119Result ready=gk119_effective_remaining(cp,&s->enhanced_assign,s->rules_profile,&remaining);
    if(ready!=GK119_OK){cp->pending_pass=0;
     strcpy(s->status,"PASS SOURCE/PROFILE REFUSED: NO ORDER SPENT");return 1;}
    if(!remaining){cp->pending_pass=0;
     strcpy(s->status,"PASS REFUSED: ORDERS EXHAUSTED; NEXT SEASON NOT BUILT");return 1;}
    if(!cp->pending_pass){cp->pending_pass=1;
     snprintf(s->status,sizeof s->status,"PASS %u REMAINING %s ORDERS? A YES / B CANCEL",
       (unsigned)remaining,s->enhanced_assign.active?"ENHANCED":"BASE");return 1;
    }
    if(gk119_pass(cp,&s->enhanced_assign,s->rules_profile)!=GK119_OK){cp->pending_pass=0;
     strcpy(s->status,"PASS SOURCE/BUDGET REFUSED: ORIGINAL STATE PRESERVED");return 1;}
    cp->pending_pass=0;
    strcpy(s->status,"PASS COMMITTED: 0 EFFECTIVE ORDERS; NO NEXT SEASON OR AI");return 1;
   }
   cp->pending_pass=0;
   strcpy(s->status,"ORIGINAL COMMAND NOT YET SOURCE-IMPLEMENTED; NO ORDER SPENT");
   return 1;
  }
  return 1;
 }
 case GK1_SCREEN_ENHANCED_ASSIGN:{
  if(cmd==GK1_INPUT_CANCEL){s->screen=GK1_SCREEN_STOCK_COMMANDS;
   strcpy(s->status,"ENHANCED LABOR SCREEN CLOSED; ORIGINAL NES WORLD UNCHANGED");return 1;}
  if(!s->rules_profile||!gk116_attestation_intact(&s->enhanced_assign,&s->stock_checkpoint)){
   strcpy(s->status,"ENHANCED ASSIGN REFUSED: SOURCE ATTESTATION REQUIRED");return 1;
  }
  /* Controller Start/View selects a bounded transfer quantity.  No orders
   * are consumed until a source-attested Enhanced transfer is committed. */
  if(cmd==GK1_INPUT_SETUP||cmd==GK1_INPUT_LABOR_PLAN){
   uint16_t next=0;
   if(gk146_quantity_step(s->assign_quantity,
        cmd==GK1_INPUT_SETUP?1:-1,&next)!=GK146_QUANTITY_OK){
     strcpy(s->status,"ENHANCED QUANTITY INVALID; NO SOURCE OR ORDERS CHANGED");return 1;
   }
   s->assign_quantity=next;
   snprintf(s->status,sizeof s->status,
     "ENHANCED AMOUNT %u SELECTED; A COMMITS ONE ORDER / B CANCEL",
     (unsigned)next);
   return 1;
  }
  if(cmd==GK1_INPUT_UP){s->assign_from=(uint8_t)((s->assign_from+4u)%5u);return 1;}
  if(cmd==GK1_INPUT_DOWN){s->assign_from=(uint8_t)((s->assign_from+1u)%5u);return 1;}
  if(cmd==GK1_INPUT_LEFT){s->assign_to=(uint8_t)((s->assign_to+4u)%5u);return 1;}
  if(cmd==GK1_INPUT_RIGHT){s->assign_to=(uint8_t)((s->assign_to+1u)%5u);return 1;}
  if(cmd==GK1_INPUT_CONFIRM){
   s->stock_checkpoint.pending_pass=0;
   Gk116Result r=gk116_transfer(&s->enhanced_assign,&s->stock_checkpoint,
       s->assign_from,s->assign_to,s->assign_quantity);
   snprintf(s->status,sizeof s->status,"ENHANCED ASSIGN %s (QTY %u): %u OF 3 ORDERS USED",
       r==GK116_OK?"COMMITTED":r==GK116_NO_ORDERS?"ORDERS EXHAUSTED":
       r==GK116_NO_WORKER?"INSUFFICIENT WORKERS":"REFUSED",
       (unsigned)s->assign_quantity,(unsigned)s->enhanced_assign.action_count);
   return 1;
  }
  return 1;
 }
 case GK1_SCREEN_STOCK_TRADE_REFERENCE:{
  Gk79Checkpoint *cp=&s->stock_checkpoint;
  if(!cp->valid||!cp->source_verified||!gk85_raw_source_intact(cp)||
     !gk88_stock_projection_intact(cp))return 0;
  /* Quote navigation never runs gk61_commit, spends an order, changes an
   * offered-item mask, or makes the original merchant schedule up. */
  if(cmd==GK1_INPUT_CANCEL){s->screen=GK1_SCREEN_STOCK_COMMANDS;
    strcpy(s->status,"TRADE REFERENCE CLOSED: ORIGINAL WORLD AND ORDERS UNCHANGED");return 1;}
  if(cmd==GK1_INPUT_LEFT){s->stock_trade_merchant=(uint8_t)((s->stock_trade_merchant+2u)%3u);return 1;}
  if(cmd==GK1_INPUT_RIGHT){s->stock_trade_merchant=(uint8_t)((s->stock_trade_merchant+1u)%3u);return 1;}
  if(cmd==GK1_INPUT_UP){s->stock_trade_item=(uint8_t)((s->stock_trade_item+16u)%17u);return 1;}
  if(cmd==GK1_INPUT_DOWN){s->stock_trade_item=(uint8_t)((s->stock_trade_item+1u)%17u);return 1;}
  if(cmd==GK1_INPUT_CONFIRM){s->stock_trade_sell^=1u;return 1;}
  return 1;
 }
 case GK1_SCREEN_STOCK_CONTINUE:{
  /* Native controller VIEW: full 83-byte original record is preserved, not
   * overwritten by the 31 projected economic/save words. Read-only only. */
  if(cmd==GK1_INPUT_LABOR_PLAN){
   if(!gk85_raw_source_intact(&s->stock_checkpoint)){
    strcpy(s->status,"RAW RECORD INTEGRITY REJECTED: SOURCE REATTACH REQUIRED");return 1;
   }
   if(s->stock_reconcile_view==3u){
     s->stock_reconcile_view=0;s->stock_checkpoint.raw_view=0;
     strcpy(s->status,"DECODED ORIGINAL STOCK NUMERIC FIELDS: READ ONLY");
   }else if(s->stock_reconcile_view==2u){
     s->stock_reconcile_view=3;s->stock_checkpoint.raw_view=0;
     strcpy(s->status,"ORIGINAL STOCK TEN SPECIALTY GOODS + FOOD: READ ONLY");
   }else if(s->stock_reconcile_view==1u){
     s->stock_reconcile_view=2u;
     strcpy(s->status,"VERIFIED 24-FIELD SETUP/SEASON DELTAS; NOT RNG FORMULAS");
   }else if(s->stock_checkpoint.raw_view){
     s->stock_checkpoint.raw_view=0;s->stock_reconcile_view=1;
     strcpy(s->status,"VERIFIED FINANCE vs ORIGINAL SPRING: GAP VIEW, NOT GAMEPLAY");
   }else{
     s->stock_checkpoint.raw_view=1;
     strcpy(s->status,"OWNER-PRIVATE ORIGINAL 83-BYTE COUNTRY RECORD: READ ONLY");
   }
   return 1;
  }
  if(cmd==GK1_INPUT_SETUP){
   if(!gk85_raw_source_intact(&s->stock_checkpoint)){
    strcpy(s->status,"COMMAND MENU REFUSED: SOURCE RECORD CORRUPT");return 1;
   }
   s->stock_checkpoint.pending_pass=0;
   s->stock_command_cursor=0;s->screen=GK1_SCREEN_STOCK_COMMANDS;
   strcpy(s->status,"ORIGINAL 14 COMMANDS: VIEW / PASS ACTIVE; OTHERS SOURCE-GATED");return 1;
  }
  /* Only exact-sha verified source loader may enter this screen in production.
   * A has a second press confirmation. B cancels pending Pass byte-identically;
   * next season, other orders, and AI are explicitly NOT synthesized. */
  Gk79Checkpoint *cp=&s->stock_checkpoint;
  if(!cp->valid)return 0;
  if(cmd==GK1_INPUT_UP||cmd==GK1_INPUT_LEFT){
   cp->view_index=(uint8_t)((cp->view_index+cp->world.country_count-1u)%cp->world.country_count);return 1;
  }
  if(cmd==GK1_INPUT_DOWN||cmd==GK1_INPUT_RIGHT){
   cp->view_index=(uint8_t)((cp->view_index+1u)%cp->world.country_count);return 1;
  }
  if(cmd==GK1_INPUT_CANCEL){
   if(cp->pending_pass){cp->pending_pass=0;strcpy(s->status,"PASS CANCELED: NO ORDER OR COUNTRY CHANGED");}
   else {s->screen=GK1_SCREEN_SPRING_ARCHIVE;strcpy(s->status,"STOCK CHECKPOINT PRESERVED IN APP; NO FUTURE SEASON GENERATED");}
   return 1;
  }
  if(cmd==GK1_INPUT_CONFIRM){
   uint8_t remaining=0u;
   if(gk119_effective_remaining(cp,&s->enhanced_assign,s->rules_profile,&remaining)!=GK119_OK){
    cp->pending_pass=0;strcpy(s->status,"PASS REFUSED: SOURCE/PROFILE INVALID");return 1;}
   if(!remaining){cp->pending_pass=0;
    strcpy(s->status,"ORDERS EXHAUSTED; NEXT SEASON REQUIRES AUTHENTIC SCHEDULER");return 1;}
   if(!cp->pending_pass){cp->pending_pass=1;
    snprintf(s->status,sizeof s->status,"PASS %u EFFECTIVE ORDERS? A CONFIRM / B CANCEL",(unsigned)remaining);return 1;}
   if(gk119_pass(cp,&s->enhanced_assign,s->rules_profile)!=GK119_OK){cp->pending_pass=0;return 0;}
   cp->pending_pass=0;
   strcpy(s->status,"PASS EXECUTED: 0 EFFECTIVE ORDERS; SPRING STILL SOURCE-BOUND");return 1;
  }
  return 1;
 }
 case GK1_SCREEN_ORDER_BUDGET_RESEARCH:
  if(cmd==GK1_INPUT_CANCEL){s->screen=GK1_SCREEN_MANUAL_RULES;return 1;}
  if(cmd==GK1_INPUT_LEFT||cmd==GK1_INPUT_RIGHT){
   unsigned w=s->active_world;
   if(w>1||!s->world_ready[w])return 0;
   unsigned govt=s->order_budget_research.governor;
   govt=(govt+(cmd==GK1_INPUT_RIGHT?1u:2u))%3u;
   Gk71Budget next;
   Gk71Result res=gk71_open(&s->worlds[w],s->setup_candidate[w],
                           GK71_SPRING,(Gk71Governor)govt,&next);
   if(res!=GK71_LOCKED&&res!=GK71_OK)return 0;
   s->order_budget_research=next;
   return 1;
  }
  if(cmd==GK1_INPUT_CONFIRM){
   /* Never infer player-ready first Spring from a manual allowance. */
   Gk71Budget before=s->order_budget_research;
   Gk71Result res=gk71_pass(&s->order_budget_research);
   if(res==GK71_LOCKED){
    strcpy(s->status,"PASS BLOCKED: ORIGINAL FIRST SPRING NOT INITIALIZED");
   }else if(res!=GK71_OK)return 0;
   if(!before.playable && memcmp(&before,&s->order_budget_research,sizeof before))return 0;
   return 1;
  }
  return 1;
 case GK1_SCREEN_BATTLE_PASS_DRILL:
  /* Research interaction, accessible through the SAME game controller. The
   * starting 12/10 mobility numbers are explicitly demonstrative and never
   * read out of original RAM, campaign setup, or the existing Mesen map. */
  if(cmd==GK1_INPUT_CANCEL){s->screen=GK1_SCREEN_BATTLE_TIMELINE;
    strcpy(s->status,"PASS DRILL CLOSED: NO SOURCE UNIT OR COUNTRY CHANGED");return 1;}
  if(cmd==GK1_INPUT_LEFT||cmd==GK1_INPUT_RIGHT){
    (void)gk72_pass_select(&s->battle_pass,(uint8_t)(1-s->battle_pass.selected));return 1;
  }
  if(cmd==GK1_INPUT_CONFIRM){
    Gk72Status r=gk72_pass_apply(&s->battle_pass);
    strcpy(s->status,r==GK72_OK?"MANUAL P31 PASS +1 (CAPPED); DRILL, NOT NES ACTION":
                            "PASS ALREADY USED IN THIS DEMO ACTIVATION");return 1;
  }
  if(cmd==GK1_INPUT_SETUP){
    (void)gk72_pass_new_activation(&s->battle_pass);
    strcpy(s->status,"NEW DEMO ACTIVATION: NOT A NES BATTLE DAY");return 1;
  }
  return 1;
 case GK1_SCREEN_BATTLE_TIMELINE:
  if(cmd==GK1_INPUT_SETUP){
    s->screen=GK1_SCREEN_BATTLE_PASS_DRILL;
    strcpy(s->status,"P31 PASS: DEMO MOBILITY 12 CAV / 10 INF; A PASS B RETURN");return 1;
  }
  if(cmd==GK1_INPUT_CANCEL){s->screen=GK1_SCREEN_MANUAL_RULES;return 1;}
  if(s->battle_timeline.outcome!=GK65_UNDECIDED){
   strcpy(s->status,"TERMINAL SEALED; B BACK THEN START RESETS WHAT-IF");
   return 1;
  }
  /* The selectors model a hypothetical source-manual event. They do NOT
   * manufacture observed battle damage, a player victory, or new country state.
   * No tournament/surrender RNG and no campaign persistence are implied. */
  if(cmd==GK1_INPUT_UP||cmd==GK1_INPUT_DOWN){
   unsigned next=s->whatif_event;
   if(next>GK65_BOTH_COMMANDERS_DESTROYED)return 0;
   next=(next+(cmd==GK1_INPUT_DOWN?1u:6u))%7u;
   s->whatif_event=(uint8_t)next;
   strcpy(s->status,"HYPOTHETICAL MANUAL OUTCOME SELECTED - NOT A NES CAPTURE");
   return 1;
  }
  if(cmd==GK1_INPUT_LEFT||cmd==GK1_INPUT_RIGHT){
   if(s->whatif_event>=GK65_COMMAND_DESTROYED&&
      s->whatif_event<=GK65_COMMAND_CAPTURED_DUEL){
    s->whatif_victim=(uint8_t)(s->whatif_victim==GK65_ATTACKER_VICTIM?
      GK65_DEFENDER_VICTIM:GK65_ATTACKER_VICTIM);
   }
   return 1;
  }
  if(cmd==GK1_INPUT_CONFIRM){
   Gk65Event event=(Gk65Event)s->whatif_event;
   Gk65Victim victim=(event==GK65_ACT_COMPLETE||
      event==GK65_BOTH_COMMANDERS_DESTROYED)?GK65_NO_VICTIM:(Gk65Victim)s->whatif_victim;
   if(gk65_apply(&s->battle_timeline,event,victim)==GK65_OK){
    if(s->battle_timeline.outcome==GK65_ATTACKER_RETREATS_TIME)
     strcpy(s->status,"TIME EXPIRED: ATTACKER RETREATS; NO COUNTRY MUTATION");
    else if(s->battle_timeline.outcome!=GK65_UNDECIDED)
     strcpy(s->status,"MANUAL WHAT-IF TERMINAL ONLY; NO ROM BATTLE OR LAND CAPTURE");
    else strcpy(s->status,"MANUAL CLOCK SIDE COMPLETE; NO DAMAGE CALCULATED");
   }else strcpy(s->status,"TERMINAL OR INVALID EVENT: NO BATTLE STATE CHANGE");
   return 1;
  }
  return 1;
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
 memcpy(next->setup_player_count,old->setup_player_count,sizeof next->setup_player_count);
 memcpy(next->setup_difficulty,old->setup_difficulty,sizeof next->setup_difficulty);
 memcpy(next->setup_stage,old->setup_stage,sizeof next->setup_stage);
 memcpy(next->setup_player_slot,old->setup_player_slot,sizeof next->setup_player_slot);
 memcpy(next->setup_rulers,old->setup_rulers,sizeof next->setup_rulers);
 memcpy(next->setup_ability_method,old->setup_ability_method,sizeof next->setup_ability_method);
 memcpy(next->setup_method_cursor,old->setup_method_cursor,sizeof next->setup_method_cursor);
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

/* EXEC0062: use the EXISTING native game world, not a parallel simulated map.
 * Explicit source-phase guard prevents using pre-setup economic templates as
 * if the original Spring initialization had already occurred. No code path
 * in EXEC0062 manufactures that phase. A future original setup/season module
 * must establish post-setup and authoritative merchant Rate/offers itself. */
Gk61Result gk1_app_trade(Gk1App *app,Gk61Actor *actor,
                         const Gk61Market *market,uint8_t country_one_based,
                         Gk61Merchant merchant,Gk61Action action,uint8_t item,
                         uint16_t quantity,int hire_morale_roll11){
 if(!app||!actor||!market||app->active_world>1u)return GK61_BAD_INPUT;
 const uint8_t w=app->active_world;
 if(country_one_based==0 || country_one_based>app->worlds[w].country_count)
    return GK61_BAD_INPUT;
 /* EXEC0115: A mutable world phase and caller-supplied `market.playable=1`
  * are NOT evidence of an original Spring or an original merchant appearance.
  * In earlier source tests, forging phase 2 and one offer would actually debit
  * Gold and consume Judgment/order without ever running the NES scheduler.
  * The source numeric transaction engine remains gk61_commit and is tested
  * separately. This public game entry point fails closed until a dedicated
  * original-NES merchant-event loader supplies a SHA-pinned rate/offer/actor
  * attestation; no such original market event has yet been recorded.
  * Do not promote raw Spring country snapshots into merchant offers. */
 if(gk115_stock_market_gate(&app->stock_checkpoint,&app->worlds[w],market,
      w,country_one_based)!=GK115_MARKET_SOURCE_VERIFIED)
   return GK61_NOT_PLAYABLE;
 return gk61_commit(market,&app->worlds[w].countries[country_one_based-1u],
                    actor,merchant,action,item,quantity,hire_morale_roll11);
}

/* EXEC0114: authenticate a private original fresh-stock Spring chronology.
 * Do not claim Mesen execution here or unlock native game orders. */
Gk114Status gk1_app_original_fresh_timeline(const Gk1App *app,
 const char *verified_csv,const char *private_96bytes,Gk114Timeline *out){
 if(!app||!verified_csv||!private_96bytes||!out)return GK114_ARGUMENT;
 if(!app->world_ready[0])return GK114_TEMPLATE_DRIFT;
 return gk114_verify_private_fresh_history(verified_csv,&app->worlds[0],
  private_96bytes,out);
}

/* EXEC0148: source-attested fresh reset progression and RAM-bank guard.
 * Genuine original CPU events; never unlock original Classic commands.
 * Do not replace an archived SaveRAM checkpoint with a fresh WorkRAM trace. */
Gk148Result gk1_app_original_fresh_bank_trace(const Gk1App *app,
 const char *private_fresh_events,Gk148FreshTimeline *out){
 if(!app||!private_fresh_events||!out)return GK148_ARGUMENT;
 if(app->world_ready[0]!=1u||
    app->worlds[0].scenario_id!=GK1_MONGOL_CONQUEST||
    app->worlds[0].country_count!=14u||
    app->worlds[0].phase!=GK1_PRE_SETUP_TEMPLATE||
    app->stock_checkpoint.valid!=0u)return GK148_SETUP_GUARD;
 return gk148_verify_private_fresh(private_fresh_events,out);
}

/* EXEC0137: same native app setup, no separate synthetic game. The seed is
 * supplied explicitly by a caller, not inferred from captured Gold/Rate;
 * full original NMI and game RNG call chronology are still unknown. */
Gk137Result gk1_app_source_rate_proof(const Gk1App *app,
 const uint8_t explicit_nes_seed[6],Gk137StockRng *after,uint16_t *rate){
 if(!app||!explicit_nes_seed||!after||!rate||app->active_world>1u||
    app->setup_stage[app->active_world]!=GK53_SETUP_AWAIT_SOURCE||
    app->setup_difficulty[app->active_world]<1u||
    app->setup_difficulty[app->active_world]>5u) return GK137_BAD_INPUT;
 Gk137StockRng next;uint16_t value;
 if(gk137_from_nes_bytes(&next,explicit_nes_seed)!=GK137_OK||
    gk137_spring_rate(&next,app->setup_difficulty[app->active_world],&value)!=GK137_OK)
    return GK137_BAD_INPUT;
 *after=next;*rate=value;return GK137_OK;
}

/* EXEC0138 source RNG inverse investigation: read-only setup boundary;
 * neither a captured RATE nor a recovered pair identifies actual NMI
 * history. A true original stock first Spring is still NOT generated. */
Gk138Result gk1_app_source_rate_history(const Gk1App *app,
 const uint8_t post_seed[6],uint16_t observed_rate,uint32_t max_draws_back,
 Gk138RateScan *out){
 if(!app||!post_seed||!out||app->active_world>1u||
    app->setup_stage[app->active_world]!=GK53_SETUP_AWAIT_SOURCE||
    app->setup_difficulty[app->active_world]<1u||
    app->setup_difficulty[app->active_world]>5u)
  return GK138_BAD_INPUT;
 Gk137StockRng seed;
 if(gk137_from_nes_bytes(&seed,post_seed)!=GK137_OK)return GK138_BAD_INPUT;
 return gk138_scan_spring_rate(seed.seed48,
  app->setup_difficulty[app->active_world],observed_rate,max_draws_back,out);
}

/* EXEC0139: derive the previously missing *observed* post-setup PRNG
 * bytes from an actual MesenCE saved-state's 2KiB internal CPU RAM.
 * Neither an archived state nor a numerical reverse-RATE hit is a causal
 * proof of the original setup/season RNG draw chronology. */
Gk139Status gk1_app_source_mesen_state_probe(const Gk1App *app,
 const uint8_t *uncompressed_named_state,size_t state_len,Gk139MesenProof *out){
 if(!app||!uncompressed_named_state||!out||app->active_world>1u||
    app->setup_stage[app->active_world]!=GK53_SETUP_AWAIT_SOURCE||
    app->setup_difficulty[app->active_world]<1u||
    app->setup_difficulty[app->active_world]>5u)
   return GK139_STATE_MALFORMED;
 return gk139_parse_uncompressed_state(uncompressed_named_state,state_len,out);
}
/* EXEC0140: a state-to-state pure-PRNG *compatibility* test. It cannot
 * establish the NMI/memory-write chronology of the original setup. */
Gk140Result gk1_app_source_rng_lineage(const Gk1App *app,
 const Gk139MesenProof *earlier,const Gk139MesenProof *later,
 uint32_t maximum_draws,Gk140Lineage *out){
 if(!app||app->active_world>1u||
    app->setup_stage[app->active_world]!=GK53_SETUP_AWAIT_SOURCE||
    app->setup_difficulty[app->active_world]<1u||
    app->setup_difficulty[app->active_world]>5u)return GK140_LINEAGE_BAD_INPUT;
 return gk140_compare_mesen_seed_lineage(earlier,later,maximum_draws,out);
}


/* 141 reconstruction from published source modules: do not mutate game. */
Gk141Result gk1_app_source_dispatch_scan(const Gk1App *app,
 const char *path,Gk141DispatchMap *out){
 if(!app||!path||!out||app->active_world>1u||
    app->setup_stage[app->active_world]!=GK53_SETUP_AWAIT_SOURCE||
    app->setup_difficulty[app->active_world]<1u||
    app->setup_difficulty[app->active_world]>5u)return GK141_BAD_INPUT;
 return gk141_scan_authenticated(path,out);
}
/* 142: independent vector-rooted, fixed-bank-only CFG audit. */
Gk141Result gk1_app_source_fixed_cfg(const Gk1App *app,
 const char *path,Gk142FixedCfg *out){
 if(!app||!path||!out||app->active_world>1u||
    app->setup_stage[app->active_world]!=GK53_SETUP_AWAIT_SOURCE||
    app->setup_difficulty[app->active_world]<1u||
    app->setup_difficulty[app->active_world]>5u)return GK141_BAD_INPUT;
 return gk142_scan_fixed_cfg_authenticated(path,out);
}

/* EXEC0143: source-verified ROM bytes, explicitly hypothetical bank mapping. */
Gk141Result gk1_app_source_bank_cfg(const Gk1App *app,
 const char *path,uint8_t bank,Gk143BankCfg *out){
 if(!app||!path||!out||app->active_world>1u||
    app->setup_stage[app->active_world]!=GK53_SETUP_AWAIT_SOURCE||
    app->setup_difficulty[app->active_world]<1u||
    app->setup_difficulty[app->active_world]>5u)return GK141_BAD_INPUT;
 return gk143_scan_authenticated(path,bank,out);
}

/* EXEC0144: never promote an archived CPU mapper snapshot into first-Spring
 * source chronology, country mutation, or a playable command. */
Gk144Status gk1_app_source_archived_mapper(const Gk1App *app,
 const uint8_t *raw,size_t bytes,Gk144ArchivedMapper *out){
 if(!app||!raw||!out||app->active_world>1u||
    app->setup_stage[app->active_world]!=GK53_SETUP_AWAIT_SOURCE||
    app->setup_difficulty[app->active_world]<1u||
    app->setup_difficulty[app->active_world]>5u)
  return GK144_MAPPER_BAD_INPUT;
 return gk144_probe_archived_prg_map(raw,bytes,out);
}

/* EXEC0149: run phase-origin hypotheses only at the source-await setup
 * boundary; genuine 14/27 country startup, RNG and real orders remain locked. */
Gk149Status gk1_app_source_ppu_phase_scan(const Gk1App *app,
 const char *rom,Gk149PhaseScan *out){
 if(!app||!rom||!out||app->active_world>1u||
    app->setup_stage[app->active_world]!=GK53_SETUP_AWAIT_SOURCE||
    app->setup_difficulty[app->active_world]<1u||
    app->setup_difficulty[app->active_world]>5u) return GK149_INVALID;
 return gk149_scan_original_reset_phases(rom,out);
}

/* EXEC0150: read-only authenticated-source research only. */
Gk150Status gk1_app_source_render_window(const Gk1App *app,
 const char *rom,Gk150Report *out){
 if(!app||!rom||!out||app->active_world>1u||
    app->setup_stage[app->active_world]!=GK53_SETUP_AWAIT_SOURCE||
    app->setup_difficulty[app->active_world]<1u||
    app->setup_difficulty[app->active_world]>5u)return GK150_INVALID;
 return gk150_scan_render_window(rom,out);
}
