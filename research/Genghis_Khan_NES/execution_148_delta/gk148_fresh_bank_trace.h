#ifndef GK148_FRESH_BANK_TRACE_H
#define GK148_FRESH_BANK_TRACE_H
#include <stddef.h>
#include <stdint.h>
/* The original NES fresh RESET chronology, not a season generator. Public
 * source embeds only SHA256 of a PRIVATE 15 x 32-byte normalized Mesen event
 * receipt (CPU/frame timings and country-1 summaries), NEVER RAM/ROM bytes.
 * This audit cannot set a playable world or advance three original orders. */
#define GK148_TICK_BYTES 32u
#define GK148_TICKS 15u
typedef enum {GK148_OK=0,GK148_ARGUMENT=1,GK148_CHRONOLOGY=2,
              GK148_SOURCE_UNVERIFIED=3,GK148_SETUP_GUARD=4} Gk148Result;
typedef struct {
 uint8_t stage,bank,countries,orders;
 uint16_t year,gold,food,troops,rate;
 uint32_t frame;
 uint64_t cycles;
} Gk148Tick;
typedef struct {
 uint8_t first_world_setup,first_setup_food_change,first_year_1175,
         first_rate,first_spring_finance,first_three_orders;
 uint8_t samples,original_fresh_attested;
 uint16_t final_gold,final_food,final_troops,final_rate;
 uint64_t first_cycles,ready_cycles;
 uint32_t first_frame,ready_frame;
 uint8_t first_spring_generated_by_port; /* MUST remain zero. */
 uint8_t original_gameplay_commands_implemented; /* MUST remain zero. */
} Gk148FreshTimeline;
/* Structural test path, never authenticates a synthetic observation. Both
 * inputs and output remain unmodified on any failed validation. */
Gk148Result gk148_analyze_ticks(const Gk148Tick *rows,size_t count,
                                 Gk148FreshTimeline *out);
/* Authenticated private Mesen receipt, 480 byte exact length/hash, strong
 * independent chronology checks and observed workRAM (not saveRAM) origin.
 * Failure leaves out untouched. Does not claim that Mesen was run now. */
Gk148Result gk148_verify_private_fresh(const char *path,Gk148FreshTimeline *out);
#endif
