#ifndef GK325_OPENING_SEQUENCE_H
#define GK325_OPENING_SEQUENCE_H
#include <stddef.h>
#include <stdint.h>
/* Original-stock NES frame-observation ordering evidence. The numbers are
 * measurements, not native-port initialization constants or commands.
 * Actual port setup/RNG/season/event implementations are still required. */
typedef enum {GK325_MONGOL=1,GK325_WORLD=2} Gk325World;
typedef enum {GK325_FRESH_RESET=1,GK325_ARCHIVED_RESTORED=2} Gk325Condition;
typedef enum {GK325_WORK_RAM=1,GK325_SAVE_RAM=2} Gk325Bank;
typedef enum {GK325_OK=0,GK325_BAD_INPUT=1,GK325_WRONG_SOURCE=2,
              GK325_MISSING_PHASE=3,GK325_INCONSISTENT=4} Gk325Status;
typedef struct {
 uint32_t frame;
 uint16_t year,gold,food,troops,rate;
 uint8_t source_orders;
 uint8_t observed_event_dialog;  /* 1 only when screenshot establishes it */
 uint8_t observed_command_menu;  /* 1 only when screenshot establishes it */
} Gk325Sample;
typedef struct {
 Gk325World world;
 Gk325Condition condition;
 Gk325Bank bank;
 uint8_t country_count;
 const Gk325Sample *samples;
 size_t sample_count;
} Gk325Trace;
typedef struct {
 uint32_t setup_frame,calendar_frame,finance_frame,event_frame,command_frame;
 uint16_t source_year,source_gold,source_food,source_troops,source_rate;
 uint8_t source_orders;
 uint8_t observed_stock;   /* observation established by supplied trace only */
 uint8_t port_generated;   /* always zero: cannot certify original port RNG */
 uint8_t port_orders;      /* always zero: cannot unlock gameplay */
} Gk325Audit;
/* Atomic on failure. Fresh case requires distinct setup, year/rate,
 * finance, event dialog and 3-order menu observations in chronological order.
 * Archived case supports a restored command-menu snapshot, not fresh-run
 * provenance or a manufacturing path for an original first Spring. */
Gk325Status gk325_audit_trace(const Gk325Trace *in,Gk325Audit *out);
#endif
