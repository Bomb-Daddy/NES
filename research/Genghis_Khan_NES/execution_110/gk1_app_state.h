#ifndef GK1_APP_STATE_H
#define GK1_APP_STATE_H
#include "../execution_039/gk1_battle_session.h"
#include <stdint.h>
#include "../execution_045/gk1_observed_turn.h"
#include "../execution_047/gk1_divide_contract.h"
#include "gk61_market.h"
#include "gk64_manual_rules.h"
#include "gk65_battle_terminal.h"
#include "gk68_observed_rest.h"
#include "gk71_order_budget.h"
#include "gk72_battle_pass.h"
#include "gk75_spring_economy.h"
#include "gk110_spring_stage.h"
#include "gk79_stock_checkpoint.h"
#include "../execution_049/gk1_scenario_factory.h"
/* A standalone, copyright-clean native-app vertical slice, NOT a shipped game.
 * Campaign buttons are navigation only: no campaigns are fabricated.
 * Approved combined modified-ROM one-cell checkpoint cases only.
 */
typedef enum {GK1_SCREEN_TITLE=0, GK1_SCREEN_CAMPAIGNS=1,
 GK1_SCREEN_RESEARCH_BATTLE=2, GK1_SCREEN_UNIMPLEMENTED=3, GK1_SCREEN_PATH_PICK=4, GK1_SCREEN_PATH_BATTLE=5, GK1_SCREEN_DIVIDE_PICK=6, GK1_SCREEN_DIVIDE_BATTLE=7, GK1_SCREEN_SCENARIO_COUNTRIES=8, GK1_SCREEN_SETUP_WIZARD=9, GK1_SCREEN_LABOR_PLAN=10,GK1_SCREEN_DEMOBILIZE=11, GK1_SCREEN_MARKET_EXPLORER=12,
 GK1_SCREEN_MANUAL_RULES=13, GK1_SCREEN_BATTLE_TIMELINE=14,GK1_SCREEN_REST_CASE=15,GK1_SCREEN_ORDER_BUDGET_RESEARCH=32,GK1_SCREEN_BATTLE_PASS_DRILL=16,GK1_SCREEN_SPRING_ARCHIVE=33,GK1_SCREEN_STOCK_CONTINUE=34,GK1_SCREEN_STOCK_COMMANDS=35,GK1_SCREEN_STOCK_TRADE_REFERENCE=36} Gk1Screen;
typedef enum {GK1_INPUT_UP=1, GK1_INPUT_DOWN=2, GK1_INPUT_LEFT=3,
 GK1_INPUT_RIGHT=4, GK1_INPUT_CONFIRM=5, GK1_INPUT_CANCEL=6, GK1_INPUT_SETUP=7, GK1_INPUT_DRAFT_UNDO_ONE=8,
 GK1_INPUT_DRAFT_UNDO_TEN=9, GK1_INPUT_LABOR_PLAN=10} Gk1Input;
typedef enum {GK53_SETUP_PLAYERS=0, GK53_SETUP_DIFFICULTY=1,
 GK53_SETUP_LORD=2, GK53_SETUP_ABILITIES=3, GK53_SETUP_AWAIT_SOURCE=4} Gk53SetupStage;
/* Source-verified NES manual: single Mongol player, 1-4 World players,
 * 1-5 difficulty, four World selectable lords, manual/auto abilities.
 * These are setup INTENT; original stat randomization and seasonal init OPEN.
 */
int gk53_setup_allowed_lord(uint8_t world,uint8_t country_index);
uint8_t gk53_setup_country_for_slot(uint8_t world,uint8_t slot);
typedef struct {
  Gk1Screen screen;
  uint8_t menu_index, case_index;
  uint8_t has_session;
  Gk1BattleSession session;
  Gk1MoveResult last_move;
  Gk45Turn path;
  uint8_t path_case;   /* 0 raw7, 1 controlled raw2, 2 controlled ally40 */
  uint8_t cursor_x,cursor_y;
  uint8_t path_events[4]; /* 1 step1, 2 step2, 3 B */
  uint8_t path_event_count;
  Gk47Divide divide;
  uint8_t divide_choice,divide_cursor_x,divide_cursor_y;
  uint8_t divide_events[8],divide_event_count;
  Gk1LoadedScenario worlds[2]; /* independent mutable PRE_SETUP copies */
  uint8_t world_ready[2],country_cursor[2],active_world;
  uint8_t setup_candidate[2]; /* 0xFF unset; INDEX intent ONLY, not NES player setup */
  uint8_t rules_profile; /* 0 stock Classic, 1 approved Enhanced; separate oracles */
  uint8_t setup_player_count[2]; /* 1 Mongol; 1..4 World */
  uint8_t setup_difficulty[2]; /* NES manual: 1..5 */
  uint8_t setup_stage[2]; /* stages do NOT imply original game reached Spring */
  uint8_t setup_player_slot[2]; /* current player selecting a lord */
  uint8_t setup_rulers[2][4]; /* country index, 255 means unset */
  uint8_t setup_ability_method[2]; /* 0 undecided, 1 automatic, 2 manual */
  uint8_t setup_method_cursor[2]; /* 0 automatic, 1 manual */
  /* EXEC0055: controller-entered proposed manual ability point distribution.
   * Manual p.8-9 confirms SIX abilities and LORD+FOUR PRINCES.
   * These are additional-point draft quantities ONLY, never base abilities or
   * NES-authoritative initialized stats. No season/ruler transform implied. */
  uint8_t manual_bonus_active[2];
  uint8_t manual_bonus_person[2];
  uint8_t manual_bonus_ability[2];
  uint8_t manual_bonus[2][5][6];
  uint8_t labor_from, labor_to; /* EXEC0057 PRE_SETUP planner cursor ONLY */
  uint8_t market_merchant,market_item,market_sell;
  uint16_t example_rate; /* illustrative only, NOT original merchant Rate */
  uint8_t spring_stock_mode; /* EXEC0077 validated external STOCK read-only source profile */
  uint8_t spring_stock_count; /* 0 until exact hash-validated private record load */
  uint8_t spring_preview_mode; /* EXEC0075 original-source bounded what-if; never mutates country */
  uint8_t spring_archive_cursor; /* read-only 14-country archived capture; not Spring init */
  uint8_t manual_remaining_orders; /* manual battle window 1..3; NOT player turns */
  Gk65Terminal battle_timeline;
  Gk72BattlePass battle_pass; /* manual p31 Pass drill, not source unit/RNG */
  Gk68RestCase rest_case; /* isolated observed MOD checkpoint, not campaign */
  Gk79Checkpoint stock_checkpoint; /* exact source validated by gk77_load, not game startup */
  uint8_t stock_reconcile_view; /* VIEW cycles decoded / raw / finance gap (read only) */
  uint8_t stock_trade_merchant,stock_trade_item,stock_trade_sell; /* source-rate quote research only */
  uint8_t stock_command_cursor; /* authentic-stock 14-command controller menu (Pass/View only) */
  Gk71Budget order_budget_research; /* MANUAL-only, never live order in PRE_SETUP */
  /* EXEC0066: explicit manual what-if event selector, not ROM-derived gameplay. */
  uint8_t whatif_event,whatif_victim; /* event0..6, victim1=attacker 2=defender */
  uint16_t demob_quantity; /* EXEC0060 Enhanced-only PRE_SETUP preview, not season */
  char status[128];
} Gk1App;
void gk1_app_init(Gk1App *s);
/* Atomic install of both validated scenario templates: no auto-stat or Spring. */
int gk1_app_install_templates(Gk1App *s,const Gk1TemplateDataset *source);
const Gk1CountryTemplate *gk1_app_selected_country(const Gk1App *s);

/* All-country source-bounded partial Spring finance for the same native app.
 * No world mutation or setup/season unlock on either success or failure. */
Gk110Result gk1_app_source_spring_stage(const Gk1App *app,
 const char *verified_csv,Gk110SpringStage *out);
/* 1 indicates input handled; 0 rejects null/invalid. All paths controller reachable. */
int gk1_app_input(Gk1App *s,Gk1Input command);
const char *gk1_app_case_label(unsigned i);
/* Atomic state transition from a validated, replayable receipt.
 * Does not claim a complete NES campaign save. */
int gk1_app_restore_session(Gk1App *s,const Gk1BattleSession *loaded);
/* Restore from an exact validated source-event script, atomic to the app. */
int gk1_app_restore_path(Gk1App *s,uint8_t scenario,const uint8_t *events,uint8_t count);
/* Validated source-event replay of actual Divide controller phases, no campaigns. */
int gk1_app_restore_divide(Gk1App *s,const uint8_t *events,uint8_t count,
                           uint8_t selected,uint8_t cursor_x,uint8_t cursor_y);
/* G11 source transaction API: STILL GATED by true post-setup phase.
 * This is not a playable original Spring or a new controller Trade menu. */
const char *gk87_stock_command_name(unsigned command);
Gk61Result gk1_app_trade(Gk1App *app,Gk61Actor *actor,
                         const Gk61Market *market,uint8_t country_one_based,
                         Gk61Merchant merchant,Gk61Action action,uint8_t item,
                         uint16_t quantity,int hire_morale_roll11);
#endif
