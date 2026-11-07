#ifndef GK1_OBSERVED_TURN_045_H
#define GK1_OBSERVED_TURN_045_H
#include <stdint.h>
/* Strict observation-based integration contract for one approved combined
 * modified NES ROM checkpoint. NOT a general battle engine. All unspecified
 * situations must be rejected rather than filled in with guessed rules. */
typedef enum {
  GK45_BAD = -1, GK45_UNKNOWN = 0, GK45_ACCEPTED = 1,
  GK45_REFUSED = 2, GK45_FINISHED = 3, GK45_PROMPT_QUANTITY = 4
} Gk45Result;
typedef enum { GK45_RAW7=1, GK45_RAW2=2, GK45_FRIEND_40=3 } Gk45Case;
typedef enum { GK45_BEFORE=1, GK45_PENDING=2, GK45_REFUSAL_MESSAGE=3,
               GK45_DONE=4 } Gk45Phase;
typedef struct {
  Gk45Case fixture;
  Gk45Phase phase;
  uint16_t soldiers, ally1_soldiers, mobility, completion_word;
  uint8_t x, y, ally1_x, ally1_y, steps;
  uint64_t rng48;
} Gk45Turn;
/* Fixture init uses numeric metadata only. Friend unit at target was placed
 * by an explicit emulator memory intervention, not a natural game setup. */
Gk45Result gk45_open(Gk45Case scenario, Gk45Turn *out);
Gk45Result gk45_step_first(Gk45Turn *turn);
Gk45Result gk45_step_second(Gk45Turn *turn);
/* B after a rejection only dismisses its message, B while path pending
 * commits the path; B never rolls back an already committed step. */
Gk45Result gk45_press_b(Gk45Turn *turn);
/* This is an ENTRY-MENU-only bounded test for 40 soldiers/type0 at (7,7).
 * Mobility 10..20 was directly tested in a controlled NES state. */
Gk45Result gk45_divide_entry(uint16_t mobility);
#endif
