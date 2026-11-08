#ifndef GK1_SOURCE_IDENTITY_H
#define GK1_SOURCE_IDENTITY_H
#include <stddef.h>
#include <stdint.h>
/* Exact public 41-country CSV identity, not an original-ROM authentication.
 * Returns 1 iff the file's bytes match the known published v1.07 SHA256.
 * Rejects even semantically valid edited numbers rather than silently
 * presenting them as the original NES pre-setup economic data. */
int gk1_verified_original_csv(const char *path);
void gk1_sha256_bytes(const uint8_t *data,size_t size,uint8_t out[32]);
#endif
