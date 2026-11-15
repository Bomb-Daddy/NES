# GK1 EXEC0306 — native Win32 source-gate repair (2026-11-15)

Scope: the existing complete native Genghis Khan I C11 project. No original ROM, SRAM, graphics or proprietary assets are included.

## Two independently observed failures

1. [EXEC0305 workflow 43722724286](https://github.com/Bomb-Daddy/NES/actions/runs/43722724286) failed before compilation when test_crlf_normalization re-encoded an already-CRLF Windows fixture as CRCRLF. The production source normalizer was correct to refuse this malformed input.
2. After the fixture correction, [workflow 43727432363](https://github.com/Bomb-Daddy/NES/actions/runs/43727432363) passed all 9 hostile tests, but production refused a SHA-matching real inherited app because a stale exact C return-line string was absent. The entire C file matched the authenticated expected SHA-256; this redundant internal formatting assumption was unsound.

## Implemented C11 pipeline repairs

- Changed the ACTUALLY INVOKED execution_305_win32_original_register_value/test_stage305_win32.py: canonicalize fixture before CRLF conversion; add hostile malformed CRCRLF refusal; add SHA-pinned realistic predecessor with no obsolete exact return line.
- Changed the ACTUALLY INVOKED execution_305_win32_original_register_value/stage305_win32.py: delete only the stale exact-return-line assertion while retaining exact full preimage C/H/CMake SHA-256, reviewed EXEC0304 C fragments and hashes, unique header/CMake anchors, duplicate refusal, original gate controls, and rollback.
- The new regression checks real linked C11 entrypoint gk1_app_original_store_values, not a source filename.
- The unchanged workflow uses these same paths before WinMain C11 compilation, native CTest and NSIS installation.

## Reproduction and status

With the complete inherited source and repository checkout, run: python research/Genghis_Khan_NES/execution_305_win32_original_register_value/test_stage305_win32.py ; then run the same directory's stage305_win32.py against current-gk1/execution_199. The stage requires exact predecessor source hashes.

Local independent Linux Python synthetic-fixture QA: 12/12 PASS, including malicious CRCRLF, rollback and removal of obsolete return-line assumption. This does not establish Windows binary success. Use the final newest [native Windows workflow run](https://github.com/Bomb-Daddy/NES/actions/workflows/gk1-native-windows.yml) for native Win32 acceptance; do not claim a run passed before verifying its logs.

Previously tested ROM evidence remains separate: no new Mesen capture, stock fresh vs archived not conflated; original full first Spring, all 14/27 states and three real orders remain OPEN. Physical Xbox and full campaigns NOT RUN. The game remains QA-only, not an accepted release.