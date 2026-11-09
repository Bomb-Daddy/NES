#ifndef GK120_SAVE_HOME_H
#define GK120_SAVE_HOME_H
#include <stddef.h>
/* Port-side Linux native save path, never a game/original-NES rule.
 * Uses absolute XDG_DATA_HOME or HOME/.local/share. Rejects unsafe relative
 * overrides, path traversal, malformed filenames, or undersized outputs.
 * On failure leaves output untouched. No implicit current-directory saves.
 */
int gk120_linux_path(const char *xdg_data_home, const char *home,
                     const char *filename, char *out, size_t capacity);
/* POSIX only: create directory components for an exact validated full path.
 * Reject invalid path/symlink-to-file; do not open or overwrite save files.
 * Windows uses the established LocalAppData routine (gk59).
 */
int gk120_prepare_linux_dir(const char *full_path);
#endif
