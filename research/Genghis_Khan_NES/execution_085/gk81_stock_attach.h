#ifndef GK81_STOCK_ATTACH_H
#define GK81_STOCK_ATTACH_H
#include "gk77_stock_spring.h"
#include <stddef.h>
/* Only external, PRIVATE owner-supplied original-stock Mesen country records
 * qualify. No search/download of ROMs; this library never writes source bytes.
 * `root` is the per-user data directory, not the process CWD/install folder.
 * The three exact SHA-256 identities remain enforced by gk77_load.
 */
typedef enum {GK81_ATTACH_OK=0,GK81_ATTACH_INVALID=1,
 GK81_ATTACH_NOT_FOUND=2,GK81_ATTACH_BAD_SOURCE=3} Gk81AttachResult;
const char *gk81_stock_filename(Gk77Profile profile);
/* Pure path construction; rejects relative/empty roots and Windows/Linux
 * traversal, and preserves output on any failure. */
int gk81_stock_path(const char *root,Gk77Profile profile,
                    char *out,size_t capacity);
/* Transactional SHA-verified source attach from user-data root (or explicit
 * legacy file override only when override_profile matches requested).
 * On any failure, source and selected path are byte-identical to before.
 */
Gk81AttachResult gk81_stock_attach(const char *root,
 const char *explicit_path,Gk77Profile explicit_profile,
 Gk77Profile requested,Gk77StockSpring *source,char *selected,size_t cap);
#endif
