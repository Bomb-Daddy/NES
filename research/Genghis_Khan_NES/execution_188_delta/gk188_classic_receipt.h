#ifndef GK188_CLASSIC_RECEIPT_H
#define GK188_CLASSIC_RECEIPT_H
#include "gk187_classic_assign.h"
/* Port-only source-bound Classic ASSIGN receipt. Does not contain original
 * ROM/SRAM. Requires reattachment of the exact private original checkpoint
 * to load; never claims independently generated Spring or NES parity.
 */
typedef enum { GK188_OK=0,GK188_INVALID=1,GK188_SOURCE=2,
 GK188_EXISTS=3,GK188_IO=4,GK188_CORRUPT=5 } Gk188Result;
Gk188Result gk188_save_new(const char *path,const Gk79Checkpoint *cp,
 const Gk187ClassicAssign *s);
/* On all failures, *out is byte-identical to the entry state. */
Gk188Result gk188_load(const char *path,const Gk79Checkpoint *cp,
 Gk187ClassicAssign *out);
#endif
