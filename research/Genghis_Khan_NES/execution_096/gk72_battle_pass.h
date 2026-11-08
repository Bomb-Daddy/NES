#ifndef GK72_BATTLE_PASS_H
#define GK72_BATTLE_PASS_H
#include <stdint.h>
/* KOEI English NES MANUAL-DERIVED battle Pass, printed p.31.
 * The controller exercise uses illustrative starting mobility 12/10,
 * NEVER original NES unit data, tactical Mesen capture, or a live campaign.
 * This component does not alter original battlefield RAM or world countries.
 */
typedef enum {GK72_OK=0,GK72_REJECTED=1} Gk72Status;
typedef struct {
 uint8_t mobility[2]; /* cavalry, infantry/archers */
 uint8_t used[2]; /* one Pass per unit activation in this manual-only drill */
 uint8_t selected; /* 0 cavalry, 1 other */
 uint8_t valid;
} Gk72BattlePass;
Gk72Status gk72_pass_begin(Gk72BattlePass *s,uint8_t cavalry_mobility,
                          uint8_t other_mobility);
Gk72Status gk72_pass_select(Gk72BattlePass *s,uint8_t selected);
Gk72Status gk72_pass_apply(Gk72BattlePass *s);
/* The caller must explicitly begin the next hypothetical activation; it is
 * not implicitly derived from a strategic season, ROM AI, or battle clock. */
Gk72Status gk72_pass_new_activation(Gk72BattlePass *s);
#endif
