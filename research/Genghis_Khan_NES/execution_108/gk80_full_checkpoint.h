#ifndef GK80_FULL_CHECKPOINT_H
#define GK80_FULL_CHECKPOINT_H
#include "gk79_stock_checkpoint.h"
/* EXEC0080: portable, exact-source-bound snapshot of the CURRENT LIMITED
 * authenticated original-stock first-Spring session. This is NOT a complete
 * Genghis campaign save. A new process must first reattach the exact private
 * original stock 14/27-record source via gk79_open_original; that source
 * provides the independently SHA-verified baseline. No ROM bytes are shipped.
 * Every available numeric country field is serialized explicitly as LE u16;
 * strings and raw struct layout are never persisted.
 * The SHA-256 tail detects accidental changes, not malicious forgery.
 * EXEC0086 V2 records the full 14/27*83-byte SHA-256 source identity;
 * legacy V1 saves still load with a distinct warning/status, because their
 * save format never retained the unmapped original source byte identity.
 */
typedef enum { GK80_IO_OK=0, GK80_IO_BAD=1, GK80_IO_EXISTS=2,
               GK80_IO_ERROR=3, GK80_IO_CORRUPT=4,
               GK80_IO_SOURCE_MISMATCH=5,
               GK80_IO_LEGACY_SOURCE_UNBOUND=6 } Gk80IO;
Gk80IO gk80_save_new(const char *path,const Gk79Checkpoint *s);
Gk80IO gk80_load(const char *path,Gk79Checkpoint *original_authenticated_source);
#endif
