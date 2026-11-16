# EXEC0324 — Fix real Win32 EXEC0323 SHA gate (2026-11-16)

At GitHub Actions run 43797483882 the first failure was an exact SHA mismatch for `stage323.py`, before C11 compilation. The GitHub blob itself matches the authenticated LF byte archive. Windows checkout's CRLF representation is not that archived byte string. This patch does not lower the exact source SHA gate: it rejects CRCRLF, lone CR, missing files, NUL, and any non-line-ending byte change; verifies all eight original SHA256s BEFORE modifying anything; restores the previous bytes if a write fails; then restores canonical exact LF source for existing SHA checks, hostile tests, app source stage and compile.

No real NES startup RNG, first playable Spring, full campaigns, physical Xbox testing, or release acceptance implied.
