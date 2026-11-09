# GK1 EXEC0058 — Real native multi-slot cold-save path

Extends the **same** existing native X11/Win32 C11 application from execution_057, without changing any NES economic formula or first-Spring data. Port-only LB/RB shoulder buttons select one of three independent **pre-setup** save slots on campaign/roster/setup/labor screens. X exclusively creates a snapshot, Y validates/loads; no overwrite, no slot deletion or rename. Slot 1 is the EXACT legacy filename, slots 2/3 append `.slot2`/`.slot3`. This is not a finished campaign save, and synthetic native tests are not physical Xbox tests. Both 14/27 templates, workforce state, manual bonus, Classic/Enhanced profile, and old replay/battle systems remain preserved.

`--slot-export FILEBASE 1..3 SCENARIO_ID COUNTRY_ID QUANTITY` and `--slot-import FILEBASE 1..3 SCENARIO_ID COUNTRY_ID` are executable Linux native process QA harnesses. Their fake controller presses traverse the actual installed pad routing and the existing source-identity-validated 2734-byte save API. Files are mode 0600 on Linux. The CLI harness is not keyboard gameplay.

Build from ZIP root: `cmake -S execution_058 -B build58 -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build58 && ctest --test-dir build58 --output-on-failure`.

**Unfinished:** original NES automatic character initialization, Spring transition, actual season economy/AI, dynasty, unrestricted battles, full saved campaigns, authentic AV, Windows native installer/QA and real physical Xbox acceptance.
