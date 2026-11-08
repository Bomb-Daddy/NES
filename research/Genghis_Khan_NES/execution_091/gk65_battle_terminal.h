#ifndef GK65_BATTLE_TERMINAL_H
#define GK65_BATTLE_TERMINAL_H
#include <stdint.h>
/* Printed KOEI English NES manual pp27-28. A research-only day/event reducer.
 * No RNG events are synthesized; real campaign postwar transitions OPEN.
 * Caller must provide one observed/event-sourced action per side.
 */
typedef enum {GK65_OK=0,GK65_REJECTED=1} Gk65Status;
typedef enum {GK65_ATTACKER_PHASE=0,GK65_DEFENDER_PHASE=1,GK65_DONE=2} Gk65Phase;
typedef enum {GK65_NO_VICTIM=0,GK65_ATTACKER_VICTIM=1,GK65_DEFENDER_VICTIM=2} Gk65Victim;
typedef enum {GK65_ACT_COMPLETE=0,GK65_COMMAND_DESTROYED=1,
 GK65_COMMAND_FLED=2,GK65_COMMAND_STARVED=3,GK65_COMMAND_SURRENDERED=4,
 GK65_COMMAND_CAPTURED_DUEL=5,GK65_BOTH_COMMANDERS_DESTROYED=6} Gk65Event;
typedef enum {GK65_UNDECIDED=0,GK65_ATTACKER_WON=1,GK65_DEFENDER_WON=2,
 GK65_ATTACKER_RETREATS_TIME=3,GK65_DEFENDER_RETAINS_DOUBLE_KO=4} Gk65Outcome;
typedef struct {
 uint8_t max_days,days_complete,phase,outcome,terminal_event;
 /* A capture/country ownership change is NOT automatically performed. */
 uint8_t attacker_captured;
} Gk65Terminal;
Gk65Status gk65_begin(uint8_t remaining_strategic_orders,Gk65Terminal *out);
Gk65Status gk65_apply(Gk65Terminal *state,Gk65Event event,Gk65Victim victim);
#endif
