#ifndef GK68_OBSERVED_REST_H
#define GK68_OBSERVED_REST_H
#include <stdint.h>
/* OBSERVED_RUNTIME_MOD: ONE genuine Mesen case, NOT original stock/season
 * formula or general Rest implementation. This is separate from 41-country
 * original PRE_SETUP state, which MUST remain unchanged. */
typedef enum {GK68_REST_OK=0,GK68_REST_BAD=1,GK68_REST_WRONG_PHASE=2} Gk68RestResult;
typedef enum {GK68_REST_SPRING=0,GK68_REST_CONFIRM=1,
              GK68_REST_SUMMER=2} Gk68RestStage;
typedef struct {
 uint16_t year,bp,rate,skill,economy,gold,food,troops;
 uint8_t season,orders,stage;
} Gk68RestCase;
Gk68RestResult gk68_rest_open(Gk68RestCase *out);
/* A from Spring opens confirmation; A from confirmation commits the EXACT
 * verified source checkpoint transition, B from confirmation returns to
 * byte-identical Spring state. All invalid operations are fully atomic. */
Gk68RestResult gk68_rest_press_a(Gk68RestCase *state);
Gk68RestResult gk68_rest_press_b(Gk68RestCase *state);
int gk68_rest_valid(const Gk68RestCase *state);
#endif
