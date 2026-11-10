# EXEC0159 — Source-attested Enhanced Fall, controller UI and persistent replay

**Authority:** Full EXEC0158 C11 application; stock USA ROM and original Mesen Spring
country-record datasets remain PRIVATE and are never shipped or placed in GitHub.

The same existing 1280×720 C11 native X11/Win32 controller shell now has a third
consecutive **Enhanced research** labor turn. After a *privately authenticated*
original Spring checkpoint and its completed Enhanced Spring and Summer turns,
select PASS to enter Fall. Three new Enhanced labor orders are available through
DPAD, A, START/VIEW quantity controls (1/5/10/25/50/100). PASS requires a second
A, and B cancels. A source-bound `GF19` 144-byte exclusive-create receipt saves
and cold-loads **all three** seasons, replaying each action from the original
checkpoint; wrong original source, Classic profile, forged worker balances,
invalid season, overspending, overwrite, truncation or corrupt receipts fail
closed. Existing `GA16` Spring and `GS18` Summer save formats are preserved.

This is NOT a source-reproduced original NES first Spring, Fall harvest, feeding,
payroll, birth, storm, merchant schedule, battle outcome or full campaign. The
Classic game remains source-gated, and the three original-stock Spring
observations (fresh Mongol, archived Mongol, archived World) remain separate.

`cmake -S execution_159 -B build159 -DCMAKE_BUILD_TYPE=Release && cmake --build build159 -j6 && ctest --test-dir build159 --output-on-failure`

Optional privately authenticated positive tests (never publish input files):
`build159/test_gk159_enhanced_fall <41-country.csv> <fresh-1162B> <archived-mongol-1162B> <archived-world-2241B>`.

**VERIFIED_STATIC:** C11 source, tests and immutable-ROM assertions.
**TESTED_PORT_LINUX:** results recorded in QA manifest, if run.
**OBSERVED_RUNTIME_STOCK:** inherited only, no new Mesen game run.
**NOT RUN:** native Win32/NSIS and physical Xbox or full campaigns.
