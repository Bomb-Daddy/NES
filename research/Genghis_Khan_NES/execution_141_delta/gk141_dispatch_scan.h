#ifndef GK141_DISPATCH_SCAN_H
#define GK141_DISPATCH_SCAN_H
#include <stddef.h>
#include <stdint.h>
/* ROM source inspection of the stock USA MMC1 PRG banks. The trampoline
 * at $EF85 accesses the owner-supplied byte after a JSR instruction and
 * dispatches through two 256-byte tables at fixed-bank $F788 and $F888.
 * These are byte-pattern CANDIDATES, not a decoded instruction CFG and
 * not evidence that any call executed in the real first Spring.
 * All PRG bytes remain PRIVATE, and no raw source bytes enter the result. */
#define GK141_IMAGE_SIZE 262160u
#define GK141_BANK_BYTES 16384u
#define GK141_BANK_COUNT 16u
typedef enum { GK141_OK=0,GK141_BAD_INPUT=1,GK141_SOURCE_UNVERIFIED=2 } Gk141Result;
typedef struct {
 uint32_t candidate_total, by_bank[16], by_selector[256];
 uint16_t dispatch_target[256];
 uint32_t target_below_fixed, candidates_boundary_rejected;
 /* First matching caller is a static byte location, not a proven opcode. */
 uint8_t first_bank,first_selector;
 uint16_t first_candidate_pc;
} Gk141DispatchMap;
/* The pure engine permits synthetic test ROMs; it never authenticates one.
 * Exact length, iNES PRG header and in-bounds tables required. Atomic refusal. */
Gk141Result gk141_scan_image(const uint8_t *image,size_t bytes,
                             Gk141DispatchMap *out);
/* Production gate: exact stock SHA256 through GK82; never accept modifications
 * or substitute arbitrary file bytes. Refusal preserves *out untouched. */
Gk141Result gk141_scan_authenticated(const char *private_stock_rom_path,
                                    Gk141DispatchMap *out);
#endif
