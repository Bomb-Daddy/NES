#ifndef GK216_FRESH_CPU_PC_H
#define GK216_FRESH_CPU_PC_H
#include "gk211_fresh_sequence.h"
#include "gk82_stock_cartridge.h"
#include <stdint.h>
/* EXEC0216: CPU snapshots from the 14 exact already-archived original Mesen
 * captures. Each PC is the sampled PC at capture end, NOT a memory-write
 * instruction, RNG call or an NES initialization formula. Capture provenance
 * is fresh nesWorkRam, never the archived nesSaveRam. ROM/RAM not embedded. */
typedef enum {
 GK216_OK=0, GK216_BAD_ARGUMENT=1, GK216_SOURCE_REFUSED=2,
 GK216_EVIDENCE_REFUSED=3, GK216_CPU_REFUSED=4,
 GK216_CHRONOLOGY_REFUSED=5
} Gk216Status;
typedef struct {
 uint16_t pc;
 uint8_t a,x,y,sp,ps;
 uint32_t frame;
 uint64_t cycles;
 uint8_t change_mask;
 uint8_t upper_program_window; /* 0 lower $8000-BFFF, 1 upper $C000-FFFF */
} Gk216CpuStage;
typedef struct {
 Gk216CpuStage stages[GK211_STAGES];
 uint8_t count;
 uint8_t fresh_workram_only;
 uint8_t sampled_at_capture_end;
 uint8_t mapper_bank_not_derived;
 uint8_t actual_memory_writes_not_traced;
 uint8_t first_spring_not_generated;
} Gk216CpuReport;
/* Independent original ROM SHA+header and all-14 full JSON SHA proofs are
 * prerequisites. NEVER changes *out on any error. */
Gk216Status gk216_verify_fresh_cpu_pc(const char *stock_rom,
 const char *private_dir,Gk216CpuReport *out);
#endif
