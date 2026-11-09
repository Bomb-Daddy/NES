# GK1 EXEC0129 — CPU-RAM-source OAM DMA (not a completed game)

This delta belongs to the existing complete native C11 Genghis Khan I application inherited from EXEC0128. It is not a standalone sample. Full 363-file EXEC0129 game source, all historical predecessor entries, Linux research installer, and runnable QA are available in separately verified local artifacts.

The source helper maps NES CPU $0000–$1FFF through the 2KB work-RAM mirror and admits a 256-byte OAMDMA transaction only when every source byte and OAMADDR are known and an explicit APU alignment bit is supplied. Unknown 0x20–0xFF source pages, phase, source bytes or OAM address reject atomically. Supported transfers account 513/514 stalled CPU cycles.

The integrated original-ROM source interpreter has a separate explicit phase-hypothesis path. Default source reset never guesses hardware state. The original first Spring and its three playable orders remain BLOCKED. No private original NES ROM, screenshots, raw SRAM, audio, graphics, or licensed game bytes belong in public GitHub.

TESTED_PORT_LINUX: 87/87 Release, 87/87 Debug, 87/87 ASan/UBSan (detect_leaks=0), 87/87 independently rebuilt complete source. 1280x720 X11/Xvfb native and Debian-extracted each 120-frame PASS. OBSERVED_RUNTIME_STOCK: prior Mongol fresh1175 Gold430/Rate121, archived1175 Gold439/Rate101 and archived World1206 Gold193/Rate114 are separate histories. NOT RUN: new original Mesen gameplay, native Win32, NSIS, physical Xbox, completed campaigns.

Complete source archive SHA-256: 01b739e205d349d60ac4879354b029609e71ff89fa53210b6abbe32ae0963275 (22,448,809 bytes).
Debian installer SHA-256: d98d5b8ec6b48660d4330e72e992a622e2147280a89937aed65a6ae1d90c8467 (188,824 bytes).
QA ZIP SHA-256: c1d12710cad233bf4528ef5712e4b389408505d264bcb88d80bafb4fee825c7b (22,688 bytes).

See INTEGRATION_EXEC0129.md for the exact source/CMake integration points. All original CPU, controller, country, trade and campaign logic from EXEC0128 must be retained.
