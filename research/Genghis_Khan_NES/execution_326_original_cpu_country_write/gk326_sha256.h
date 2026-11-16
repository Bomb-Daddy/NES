#ifndef GK326_SHA256_H
#define GK326_SHA256_H
#include <stddef.h>
#include <stdint.h>
/* Local SHA-256 implementation, no OpenSSL/Windows system dependency. */
void gk326_sha256(const uint8_t *input,size_t len,uint8_t out[32]);
#endif
