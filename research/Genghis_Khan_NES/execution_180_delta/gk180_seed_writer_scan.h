#ifndef GK180_SEED_WRITER_SCAN_H
#define GK180_SEED_WRITER_SCAN_H
#include <stddef.h>
#include <stdint.h>
/* Original KOEI USA ROM: conservative, OPCODE-ALIGNED source graph in MMC1
 * fixed PRG bank 15 only. Static reachability is not execution reachability:
 * conditional branches, JSR returns, PPU waits and dispatcher routes remain
 * unresolved. This never produces a first Spring or a chronological RNG seed.
 */
#define GK180_ROM_BYTES 262160u
#define GK180_MAX_SITES 256u
typedef enum {GK180_OK=0,GK180_BAD_INPUT=1,GK180_SOURCE=2,
              GK180_OVERFLOW=3} Gk180Status;
typedef enum {GK180_STORE=1,GK180_RMW=2} Gk180Kind;
typedef struct {
 uint16_t pc;       /* source address in immutable fixed bank */
 uint8_t address;   /* direct, UNINDEXED CPU zero-page destination $84..$89 */
 uint8_t kind;      /* STORE or read-modify-write */
 uint8_t roots;     /* bit 0 RESET, bit 1 NMI, bit 2 IRQ */
 uint8_t in_rng;    /* original fixed bank $C289..$C2EB */
} Gk180Site;
typedef struct {
 uint16_t vectors[3];
 uint32_t decoded[3],unmapped_edges,unknown_indirect_jumps;
 uint32_t unsupported_opcodes,bank_write_barriers;
 uint32_t dispatch_candidates,rng_entry_edges;
 uint32_t source_sites,internal_rng_sites,external_seed_site_candidates;
 uint32_t unverified_indexed_writes;
 Gk180Site sites[GK180_MAX_SITES];
 /* No field indicating genuine seed initialization or first-Spring readiness. */
} Gk180WriterMap;
/* Pure synthetic-testable scanner. Checks iNES header/layout, but does NOT
 * authenticate copyright-owned input. Output is committed only on success. */
Gk180Status gk180_scan_image(const uint8_t *image,size_t n,
                             Gk180WriterMap *out);
/* For product/research use only: full source SHA256 verification, exact size,
 * before source analysis. Outputs contain only positions/categories, not ROM. */
Gk180Status gk180_scan_authenticated(const char *private_stock_rom,
                                     Gk180WriterMap *out);
#endif
