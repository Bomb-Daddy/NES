#ifndef GK153_COUNTRY_WRITE_WATCH_H
#define GK153_COUNTRY_WRITE_WATCH_H
#include <stdint.h>
/* A source-executed NES $6000-$7FFF PRG-RAM write watchpoint.  The original
 * 83-byte country records start at CPU $6DCD, NOT CPU work RAM $0DCD.
 * These measurements never grant turns or establish Mesen runtime parity. */
#define GK153_COUNTRY_BASE 0x6DCDu
#define GK153_COUNTRY_STRIDE 83u
#define GK153_MONGOL_COUNTRIES 14u
#define GK153_WORLD_COUNTRIES 27u
#define GK153_MONGOL_END (GK153_COUNTRY_BASE+GK153_COUNTRY_STRIDE*GK153_MONGOL_COUNTRIES)
#define GK153_WORLD_END (GK153_COUNTRY_BASE+GK153_COUNTRY_STRIDE*GK153_WORLD_COUNTRIES)
typedef enum { GK153_INVALID=0, GK153_OUTSIDE=1, GK153_RECORDED=2 } Gk153WatchResult;
typedef struct {
 uint32_t world_region_writes;
 uint32_t mongol_region_writes;
 uint16_t first_cpu_address,first_source_pc,last_cpu_address,last_source_pc;
 uint8_t first_country_index,last_country_index;
} Gk153CountryWatch;
/* An accepted source CPU store only, never a speculative byte pattern.
 * Invalid/outside/overflow writes leave the accumulator unchanged. */
Gk153WatchResult gk153_note_executed_write(Gk153CountryWatch *watch,
 uint16_t cpu_address,uint16_t source_pc);
#endif
