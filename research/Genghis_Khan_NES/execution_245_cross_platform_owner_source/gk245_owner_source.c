#include "gk245_owner_source.h"
/* The original archived World checkpoint is distinct from the fresh Mongol
 * reset: SaveRAM vs WorkRAM and source-specific 83-byte record proof. */
Gk245Outcome gk245_owner_source_startup(Gk1App *app,const char *csv,
 int loaded,int diagnostic,const char *fresh,const char *world){
 /* Preserve the existing Win32 precedence, including diagnostics refusing
  * access to private evidence before conflicting-path interpretation. */
 Gk239Status w=gk239_owner_world_startup(app,csv,world,fresh,loaded,diagnostic);
 switch(w){
  case GK239_CONFLICTING_SOURCES:return GK245_CONFLICT;
  case GK239_REFUSED:return GK245_ARCHIVED_REFUSED;
  case GK239_IMPORTED:return GK245_ARCHIVED_IMPORTED;
  case GK239_SMOKE_ISOLATED:return GK245_ISOLATED;
  case GK239_UNREQUESTED:break;
  default:return GK245_ARCHIVED_REFUSED;
 }
 Gk238Startup f=gk238_owner_startup(app,csv,fresh,loaded,diagnostic);
 switch(f){
  case GK238_UNREQUESTED:return GK245_NO_REQUEST;
  case GK238_OWNER_IMPORTED:return GK245_FRESH_IMPORTED;
  case GK238_REFUSED:return GK245_FRESH_REFUSED;
  case GK238_SMOKE_ISOLATED:return GK245_ISOLATED;
  default:return GK245_FRESH_REFUSED;
 }
}
