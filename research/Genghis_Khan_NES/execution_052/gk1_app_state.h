#ifndef GK1_APP_STATE_H
#define GK1_APP_STATE_H
#include "../execution_039/gk1_battle_session.h"
#include <stdint.h>
#include "../execution_045/gk1_observed_turn.h"
#include "../execution_047/gk1_divide_contract.h"
#include "../execution_049/gk1_scenario_factory.h"
/* A standalone, copyright-clean native-app vertical slice, NOT a shipped game.
 * Campaign buttons are navigation only: no campaigns are fabricated.
 * Approved combined modified-ROM one-cell checkpoint cases only.
 */
typedef enum {GK1_SCREEN_TITLE=0, GK1_SCREEN_CAMPAIGNS=1,
 GK1_SCREEN_RESEARCH_BATTLE=2, GK1_SCREEN_UNIMPLEMENTED=3, GK1_SCREEN_PATH_PICK=4, GK1_SCREEN_PATH_BATTLE=5, GK1_SCREEN_DIVIDE_PICK=6, GK1_SCREEN_DIVIDE_BATTLE=7, GK1_SCREEN_SCENARIO_COUNTRIES=8} Gk1Screen;
typedef enum {GK1_INPUT_UP=1, GK1_INPUT_DOWN=2, GK1_INPUT_LEFT=3,
 GK1_INPUT_RIGHT=4, GK1_INPUT_CONFIRM=5, GK1_INPUT_CANCEL=6} Gk1Input;
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
  char status[128];
} Gk1App;
void gk1_app_init(Gk1App *s);
/* Atomic install of both validated scenario templates: no auto-stat or Spring. */
int gk1_app_install_templates(Gk1App *s,const Gk1TemplateDataset *source);
const Gk1CountryTemplate *gk1_app_selected_country(const Gk1App *s);

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
#endif
