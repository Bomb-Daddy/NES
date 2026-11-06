#ifndef GK1_BATTLE_SESSION_H
#define GK1_BATTLE_SESSION_H
/* EXEC0039: bounded, executable battle-input/replay/save vertical slice.
 * It is NOT a general movement engine or full campaign save. The source
 * profile is the approved combined Shared BP/Rest modified NES build only.
 * Copyright-clean code; no ROM/SRAM/graphics/audio content included.
 */
#include "../execution_034/gk1_tactical_slice.h"
#include <stddef.h>
#include <stdint.h>
#define GK1_SESSION_SAVE_BYTES 48u

typedef enum {GK1_SESSION_BAD=-1, GK1_SESSION_UNSUPPORTED=0, GK1_SESSION_OK=1} Gk1SessionStatus;
typedef struct {
    Gk1BattleState battle;
    uint8_t initial_mobility, initial_terrain, initial_occupant;
    uint8_t events;
    uint8_t first_x,first_y;
    int8_t first_result, second_result;
} Gk1BattleSession;

/* The fixture is *only* a source-identified test checkpoint state. */
Gk1SessionStatus gk1_session_open_fixture(uint8_t mobility, uint8_t terrain,
                          Gk1Occupancy occupant, Gk1BattleSession *out);
/* Only the native-evidenced one-cell destination (8,7), one attempted step
 * and its observed optional B completion are allowed. Failed calls are atomic.
 */
Gk1SessionStatus gk1_session_move(Gk1BattleSession *session,
                                  uint8_t x,uint8_t y, Gk1MoveResult *result);
Gk1SessionStatus gk1_session_finish(Gk1BattleSession *session, Gk1MoveResult *result);
/* Save is a canonical 48-byte *replay receipt*, not a ROM state or full game
 * save. Replays the sourced step functions on load; checksum is for accidental
 * corruption only, not authentication. Outputs unchanged on any failure.
 */
Gk1SessionStatus gk1_session_encode(const Gk1BattleSession *session,
                                    uint8_t *dst,size_t dst_len);
Gk1SessionStatus gk1_session_decode(const uint8_t *src,size_t len,
                                    Gk1BattleSession *out);
#endif
