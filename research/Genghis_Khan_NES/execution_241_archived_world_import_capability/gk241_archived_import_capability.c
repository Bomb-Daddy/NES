#include "gk241_archived_import_capability.h"
Gk241ArchiveBackend gk241_archived_import_backend(void){
#ifdef GK163_HAS_ZLIB
 return GK241_ARCHIVE_BACKEND_ZLIB;
#else
 return GK241_ARCHIVE_BACKEND_UNAVAILABLE;
#endif
}
const char *gk241_archived_refusal_message(void){
#ifdef GK163_HAS_ZLIB
 return "OWNER WORLD ARCHIVED SOURCE REFUSED; EXACT ORIGINAL SAVERAM EVIDENCE REQUIRED";
#else
 return "ARCHIVED WORLD IMPORT UNAVAILABLE: THIS BUILD LACKS ZLIB; NO GAME LOADED";
#endif
}
