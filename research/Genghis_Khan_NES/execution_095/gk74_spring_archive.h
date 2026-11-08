#ifndef GK74_SPRING_ARCHIVE_H
#define GK74_SPRING_ARCHIVE_H
#include <stdint.h>
#include <stddef.h>
/* EXEC0074: Reconstructed integration of the documented EXEC0070 read-only
 * comparison screen. Source provenance is ARCHIVED_CAPTURE_ROM_UNVERIFIED.
 * Neither the 1175 capture nor the original CSV proves initial Spring rules.
 * No private SRAM/ROM bytes are distributed in this source tree. */
#define GK74_RAM_BYTES 8192u
#define GK74_COUNTRIES 14u
#define GK74_REC_BYTES 83u
#define GK74_FIRST_RAM_OFFSET 0x0DCDu /* NES CPU $6DCD, WorkRAM begins $6000 */
typedef struct {
 uint16_t gold,food,troops,arms,skill,economy;
} Gk74SpringCountry;
typedef struct {Gk74SpringCountry countries[GK74_COUNTRIES];} Gk74SpringArchive;
typedef enum {GK74_OK=0,GK74_BAD_INPUT=1,GK74_IO=2,GK74_SIZE=3,GK74_SHA=4} Gk74Result;
/* Raw record decode is testable without copyrighted or private source data;
 * it is NOT a claim that a fabricated array is a real archived capture. */
Gk74Result gk74_decode_record(const uint8_t *record,size_t length,Gk74SpringCountry *out);
/* Exact 8192-byte private 1175 archive and SHA256 are mandatory.
 * On every error *out is byte-for-byte unchanged. */
Gk74Result gk74_load_file(const char *path,Gk74SpringArchive *out);
#endif
