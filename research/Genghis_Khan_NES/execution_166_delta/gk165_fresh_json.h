#ifndef GK165_FRESH_JSON_H
#define GK165_FRESH_JSON_H
#include <stddef.h>
#include <stdint.h>
/* Source-attested import of an OWNER-PRIVATE original Mesen fresh-reset JSON
 * receipt. This does not reconstruct the original NES first Spring; it only
 * retrieves the actual observed 14*83 WorkRAM record without an external
 * Python extraction step. No private bytes are present in public code. */
#define GK165_FRESH_SIZE (14u*83u)
typedef enum { GK165_OK=0, GK165_ARGUMENT=1, GK165_IO=2,
 GK165_FORMAT=3, GK165_PROFILE=4, GK165_SHA=5 } Gk165Status;
Gk165Status gk165_extract_fresh_json(const char *path,
 uint8_t out[GK165_FRESH_SIZE]);
#endif
