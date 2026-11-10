#ifndef GK142_FIXED_CFG_H
#define GK142_FIXED_CFG_H
#include <stddef.h>
#include <stdint.h>
#include "gk141_dispatch_scan.h"
/* Conditional static reachability from ROM vectors in MMC1 fixed bank 15.
 * This is an over-approximation of executable paths within one PRG bank,
 * NOT an authentic trace nor proof of actual player setup execution.
 * No private ROM bytes are retained in output. */
typedef struct {
 uint16_t vector_nmi,vector_reset,vector_irq;
 uint32_t decoded_instructions, fixed_entrypoints, dynamic_entries;
 uint32_t reachable_dispatch_calls,reachable_by_selector[256];
 uint32_t switchable_targets, indirect_jumps, unsupported_opcodes;
 uint32_t truncated_instructions, examined_fixed_bytes;
} Gk142FixedCfg;
Gk141Result gk142_scan_fixed_cfg(const uint8_t *image,size_t image_bytes,
                                Gk142FixedCfg *out);
Gk141Result gk142_scan_fixed_cfg_authenticated(const char *private_stock_rom_path,
                                              Gk142FixedCfg *out);
#endif
