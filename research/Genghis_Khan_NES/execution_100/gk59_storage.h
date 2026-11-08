#ifndef GK59_STORAGE_H
#define GK59_STORAGE_H
#include <stddef.h>
/* Installed Windows user-data path; no implicit working-directory writes.
 * This is portable path POLICY only: Windows directory creation belongs to
 * the native shell. No original NES semantics are inferred here. */
int gk59_user_file(const char *directory, const char *filename,
                   char *out, size_t capacity);
#endif
