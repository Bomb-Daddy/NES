#ifndef GK208_ARCHIVED_RNG_H
#define GK208_ARCHIVED_RNG_H
#include "gk77_stock_spring.h"
#include <stdint.h>
/* Authenticated *archived-state* evidence, NOT power-on seed, first-Spring
 * seed, event history, or proof a subsequent draw actually occurred. */
typedef enum {
 GK208_OK=0,GK208_ARGUMENT=1,GK208_SOURCE=2,GK208_ARCHIVE=3,
 GK208_PARITY=4,GK208_ARCHIVE_IDENTITY=5
} Gk208Status;
typedef struct {
 Gk77Profile profile;
 uint16_t archived_pc;
 uint64_t archived_cpu_cycles;
 uint64_t archived_seed48;
 uint64_t hypothetical_next_seed48;
 uint16_t hypothetical_next_draw15;
 unsigned country_count;
} Gk208ArchiveProof;
/* All failures preserve output byte-identical. No ROM/savestate/data written.
 * ROM identity and archived source-country hash are both compulsory. */
Gk208Status gk208_archived_source_rng(const char *stock_rom,
 const char *archived_mss,Gk77Profile profile,Gk208ArchiveProof *out);
#endif
