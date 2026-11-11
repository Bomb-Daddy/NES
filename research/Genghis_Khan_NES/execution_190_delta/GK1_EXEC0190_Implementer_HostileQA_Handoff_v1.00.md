# GK1 EXEC0190 — implementer and hostile-QA handoff — 2026-11-11

Direct predecessor: **EXEC0189 complete integrated C11** from owner Collaboration file `1QDWrZaizkzC2WNc9Yft1g0DXKfe0y698` (not obsolete EXEC0076). No original ROM, private RAM, source snapshots, user save or licensed screenshot in distributed source.

## Actual native code change

- `gk187_classic_assign.h/.c`: new atomic `gk187_undo_last()` reconstructs the previous valid source-derived mutable world and 3-order budget from original immutable authenticated Spring, instead of attempting to mutate the last transfer in reverse. Requires source SHA intact, exact source profile, unpassed state. Refuses no-actions and passed state.
- `gk1_app_state.c`: Classic screen handles input `GK1_INPUT_DRAFT_UNDO_TEN`, with explicit PORT-ONLY status; input refuses forged mutable state.
- `gk1_native_shell.c`: physical Y mapping only inside Classic ASSIGN screen, maintaining X private GC19 save; elsewhere Y cold-load behavior remains. On-screen instructions now accurately explain X/Y private continuation.
- `test_gk187_classic_assign.c` adds undo/replay/source checks. `test_gk190_classic_undo.c` runs authentic three-profile undo, controller-state injection, hash-gated self-contained GC19 save + fresh app cold-load and immutable original source.
- CMake, Debian/Windows untested authored packaging/version checks, desktop label, README and installed docs updated to1.90.0-dev1.

## Exact executed QA

- Linux Release `cmake -S execution_190 -B build190 -G Ninja -DCMAKE_BUILD_TYPE=Release; cmake --build build190 -j8; ctest --test-dir build190 --output-on-failure -j8`: **153/153 PASS**.
- Linux Debug separate: **153/153 PASS**.
- Private authentic original source fixtures: `test_gk190_classic_undo CSV private/fresh14.bin private/archived14.bin private/world27.bin`: **120/120 assertions PASS**. Inherited `test_gk187_classic_assign` **188/188 PASS**.
- Focused ASan+UBSan (not full 153-suite) both above private fixture binaries **120/120**, **188/188 PASS**, no sanitizer error.
- `xvfb-run -a ./build190/gk1_native_shell --smoke-frames 120`: native1280x720 PASS. Extracted `.deb` same120 frames PASS.
- `patch -p2` direct predecessor source independently recreated all **601/601 source SHA files** byte-for-byte; complete historical ZIP old **41,449** records retained exact by CRC/size and current 601 records added. ZIP CRC PASS.
- **NOT RUN** Windows NSIS, physical Xbox, original Mesen session, campaigns.

## Fidelity and next code priority

User-requested major milestone is still OPEN: independently-generated real NES first-Spring setup, ability/difficulty and RNG for both 14/27 countries; three source-authentic playable commands and seasonal AI. Rest of original merchant presence, diplomacy, dynasty, marriage ages6/7/8, combat ending, graphical/audio parity, Windows acceptance remain unfinished. Do NOT treat manual-derived undo, GC19 and Linux Xvfb as original NES simulation or Windows release. Preserve Classic vs Enhanced distinctions and previous artifacts.

## Reproduction / artifact identities

Full source: `GK1_EXEC0190_Complete_Historical_C11_Source_v1.00.zip`, SHA256 `e1522971bf1e0bcb58be7d20030c08ddb268726b7fa4edca3037be89978ba4a6`
Debian research installer: `GK1_EXEC0190_Linux_Research_Installer_1.90.0-dev1_amd64.deb`, SHA256 `199990fa0d4564dafe519cf989afcc7d189d361f6c78ec51c59ce402252558da`
Integration patch SHA256 `84a862775a363faf7c738342e184bf8156c69455ce8531ec19ec1add9baaddaf`, changes 16 paths.
Manifest `GK1_EXEC0190_Source_SHA256_Manifest_v1.00.json` SHA256 `bcecdf9e3732e93c2eb007b83c24ab37d862805c52af0d4b83d35e807b89a7dd`.
Publishing is not Director acceptance or independent agent acknowledgment. Recurring task remains enabled.
