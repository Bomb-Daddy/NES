#ifndef GK1_DIVIDE_CONTRACT_0047_H
#define GK1_DIVIDE_CONTRACT_0047_H
#include <stdint.h>
/* Explicitly scoped to the approval-hash combined Shared BP/Rest native NES
 * test state. This is source-bounded C11 parity code, not a general game. */
typedef enum {GK47_BAD=-1,GK47_UNKNOWN=0,GK47_ACCEPTED=1,GK47_REFUSED=2,
              GK47_FINISHED=3,GK47_DEAD_MESSAGE=4} Gk47Result;
typedef enum {GK47_QUANTITY=1,GK47_DESTINATION=2,GK47_PENDING=3,
              GK47_DEATH_PROMPT=4,GK47_DONE=5,GK47_DESTINATION_ERROR=6} Gk47Phase;
typedef struct {
  Gk47Phase phase;
  uint16_t source_soldiers,split_soldiers,source_mobility,split_mobility;
  uint16_t completion_word,qty;
  uint8_t source_x,source_y,split_x,split_y,split_type;
  uint64_t rng48;
} Gk47Divide;
/* The test fixture starts with 40 soldiers at (7,7), Mobility 20, and
 * unused target slot3. All other 19 unit records, Gold and Food are guards. */
Gk47Result gk47_open(Gk47Divide *out);
Gk47Result gk47_choose_quantity(Gk47Divide *s,uint16_t qty);
Gk47Result gk47_move_to_observed_forest(Gk47Divide *s,uint8_t x,uint8_t y);
/* Distinguish B dismissing zero-soldier message versus completing movement. */
Gk47Result gk47_press_b(Gk47Divide *s);
#endif
