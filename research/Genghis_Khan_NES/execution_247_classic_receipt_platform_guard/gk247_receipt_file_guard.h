#ifndef GK247_RECEIPT_FILE_GUARD_H
#define GK247_RECEIPT_FILE_GUARD_H
/* Check exact-sized, regular, non-linked owner-private receipt file opened
 * by the caller. Returns zero without altering fd, files or gameplay.
 * Windows checks the opened HANDLE and rejects direct reparse-point paths;
 * POSIX additionally requires sole ownership and mode 0600-or-stricter.
 * Not a replacement for SHA/source/action validation. */
int gk247_receipt_file_guard(int fd,const char *path,unsigned expected_size);
#endif
