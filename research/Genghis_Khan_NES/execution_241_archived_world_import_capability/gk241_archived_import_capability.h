#ifndef GK241_ARCHIVED_IMPORT_CAPABILITY_H
#define GK241_ARCHIVED_IMPORT_CAPABILITY_H
/* Compile-time capability of the actual native game's authenticated
 * archived World Mesen .mss loader. Never describes source validity, and
 * never grants first-Spring RNG or a campaign order. */
typedef enum {
 GK241_ARCHIVE_BACKEND_UNAVAILABLE=0,
 GK241_ARCHIVE_BACKEND_ZLIB=1
} Gk241ArchiveBackend;
Gk241ArchiveBackend gk241_archived_import_backend(void);
const char *gk241_archived_refusal_message(void);
#endif
