#ifndef GK1_APP_STATE_H
#define GK1_APP_STATE_H
#include "../execution_039/gk1_battle_session.h"
#include <stdint.h>
/* A standalone, copyright-clean native-app vertical slice, NOT a shipped game.
 * Campaign buttons are navigation only: no campaigns are fabricated.
 * Approved combined modified-ROM one-cell checkpoint cases only.
 */
typedef enum {GK1_SCREEN_TITLE=0, GK1_SCREEN_CAMPAIGNS=1,
 GK1_SCREEN_RESEARCH_BATTLE=2, GK1_SCREEN_UNIMPLEMENTED=3} Gk1Screen;
typedef enum {GK1_INPUT_UP=1, GK1_INPUT_DOWN=2, GK1_INPUT_LEFT=3,
 GK1_INPUT_RIGHT=4, GK1_INPUT_CONFIRM=5, GK1_INPUT_CANCEL=6} Gk1Input;
typedef struct {
  Gk1Screen screen;
  uint8_t menu_index, case_index;
  uint8_t has_session;
  Gk1BattleSession session;
  Gk1MoveResult last_move;
  char status[128];
} Gk1App;
void gk1_app_init(Gk1App *s);
/* 1 indicates input handled; 0 rejects null/invalid. All paths controller reachable. */
int gk1_app_input(Gk1App *s,Gk1Input command);
const char *gk1_app_case_label(unsigned i);
/* Atomic state transition from a validated, replayable receipt.
 * Does not claim a complete NES campaign save. */
int gk1_app_restore_session(Gk1App *s,const Gk1BattleSession *loaded);
#endif
