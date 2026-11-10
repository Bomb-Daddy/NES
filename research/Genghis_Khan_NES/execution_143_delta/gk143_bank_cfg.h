#ifndef GK143_BANK_CFG_H
#define GK143_BANK_CFG_H
#include <stddef.h>
#include <stdint.h>
#include "gk141_dispatch_scan.h"
/* Hypothesis-based code discovery. Boot bank is an explicit external choice,
 * NOT inferred from the stock ROM. Never call this an authentic execution.
 * Reject possible MMC1 bank writes before following unknown mapping edges.
 * Indirect mapper writes and runtime-computed branches remain unknowable. */
typedef struct {
 uint8_t assumed_boot_bank;
 uint16_t vector_reset, vector_nmi, vector_irq;
 uint32_t decoded_instructions, scanned_bytes, dispatch_candidates;
 uint32_t by_selector[256];
 uint32_t decoded_low, decoded_fixed;
 uint32_t bank_write_barriers, unknown_indirect_jumps;
 uint32_t unsupported_opcodes, truncated_instructions;
 uint32_t unmapped_entrypoints, outside_prg_targets;
 uint32_t queue_overflow;
} Gk143BankCfg;
Gk141Result gk143_scan_bank_hypothesis(const uint8_t *image,size_t bytes,
                                       uint8_t assumed_bank,Gk143BankCfg *out);
Gk141Result gk143_scan_authenticated(const char *stock_path,
                                     uint8_t assumed_bank,Gk143BankCfg *out);
#endif
