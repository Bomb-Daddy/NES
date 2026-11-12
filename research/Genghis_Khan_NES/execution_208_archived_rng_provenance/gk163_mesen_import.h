#ifndef GK163_MESEN_IMPORT_H
#define GK163_MESEN_IMPORT_H
#include "gk77_stock_spring.h"
#include "gk139_mesen_state.h"
#include <stddef.h>
#include <stdint.h>
/* Source-authentic original STOCK archived Mesen .mss importer.
 * This DOES NOT recreate power-on, first Spring or original commands.
 * It extracts the owner-supplied original 14/27-country NES SaveRAM block
 * and accepts it ONLY when the existing independently authenticated exact
 * stock-country digest matches. Fresh original WorkRAM is NOT a .mss profile.
 * The output buffer is untouched on any failure, including unavailable zlib.
 * No ROM, image, state, PRG, screenshot or country bytes are compiled here.
 */
#define GK163_MAX_RECORDS (GK77_MAX_COUNTRIES*GK77_RECORD_BYTES)
typedef enum {
 GK163_OK=0, GK163_ARGUMENT=1, GK163_IO=2, GK163_FORMAT=3,
 GK163_BANK=4, GK163_SHA=5, GK163_UNAVAILABLE=6
} Gk163Status;
Gk163Status gk163_mesen_extract(const char *mss_path,Gk77Profile profile,
 uint8_t out[GK163_MAX_RECORDS],size_t *out_size);
/* Extension: exposes CPU internal-RAM RNG seed ONLY after exact original
 * archived country SHA and correct SaveRAM bank are verified. Rejects
 * atomically. This is a saved-state snapshot, not original boot RNG. */
Gk163Status gk163_mesen_extract_with_proof(const char *mss_path,
 Gk77Profile profile,uint8_t out[GK163_MAX_RECORDS],size_t *out_size,
 Gk139MesenProof *proof_out);
#endif
