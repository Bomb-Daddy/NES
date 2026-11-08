# EXEC0081 — Controller-only authenticated private original-stock Spring attachment

This is an incremental development change to the **same** native C11 game, not a full game or a reimplementation of original stock initialization.

## Player-facing improvement

The installed GUI now looks for three pre-existing **private owner-provided** SHA-256-exact original-stock Mesen record exports, without requiring an environment variable/terminal. Press LB/RB in the original-stock Spring viewer to switch/reprobe the **fresh Mongol 1175**, **archived Mongol 1175**, and **archived World 1206** profiles. A enters only a SHA-verified checkpoint with the correct selected scenario. No file is auto-downloaded, fabricated, edited or exported. Existing `GK1_STOCK_SPRING_RECORDS` + `GK1_STOCK_SPRING_PROFILE` overrides remain, with strict profile matching and no silent fallback.

Place **your own** previously captured 14x83 or 27x83 `records` binary, never the ROM, at one of these filenames under `%LOCALAPPDATA%\GK1NativeResearch\` on Windows; on Linux use `${XDG_DATA_HOME:-$HOME/.local/share}/GK1NativeResearch/`:

- `stock_fresh_mongol_1175.records` (NES fresh WorkRAM, 14 x 83 B)
- `stock_archived_mongol_1175.records` (NES restored SaveRAM, 14 x 83 B)
- `stock_archived_world_1206.records` (NES restored SaveRAM, 27 x 83 B)

The directory is deliberately **not created or searched recursively**, and no private original-stock data is distributed with public source or installer. No current stock session can be relabeled as a different profile; LB/RB clears only the transient in-memory checkpoint and requires a fresh verified attachment. Saves remain source-bound, not a full campaign save.

**VERIFIED_STATIC:** the earlier original-stock evidence and manual source remain authoritative; no original NES run in EXEC0081. **TESTED_PORT_LINUX:** compile/CTest and X11 reported separately. **NOT RUN:** original source-authentic setup/RNG, real first Spring from player choices, native Win32 executable/NSIS, physical Xbox, full campaigns. This is useful input/UI functionality and does not resolve those blockers.
