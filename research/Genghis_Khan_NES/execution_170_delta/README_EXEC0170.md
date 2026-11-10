# GK1 EXEC0170 – Authenticated private checkpoint slot preview (research build)

This incremental release continues the exact integrated EXEC0169 C11 game; it is not a new prototype. On the controller-only TITLE screen, LB/RB select one of three private original-stock slots. The screen displays a read-only preview (`VERIFIED`, `MISSING OR SECURITY-REFUSED`, or `SOURCE-INVALID`) without loading that slot into the live game. On verified slots, it displays source profile, season/year, country count and remaining orders. The preview uses the existing strict GK167 source-attested/private 0600 same-file-descriptor cold-load validator on a temporary copy of the live Gk1App; all status paths leave live state unchanged. Slot previews refresh only on selection or after save/load, not every graphical frame. Actual X/Y save/restore retains EXEC0169 behavior and no NES command or historical source state changes.

No original ROM, original country SRAM, licensed assets or screenshots are included. The existing private attested source evidence is used in optional private positive tests only, not compiled in the public app. This is NOT independent original NES first-Spring generation or full campaign saving. Existing Classic original player actions remain VIEW/PASS, not three executable original NES orders.

Repro from this source tree:

    cmake -S execution_170 -B build170 -DCMAKE_BUILD_TYPE=Release
    cmake --build build170 -j8
    ctest --test-dir build170 --output-on-failure
    cpack -G DEB --config build170/CPackConfig.cmake

Validation: new `test_gk170_slot_preview` tests invalid arguments, absent slot, malformed privately owned checkpoint, insecure 0644 permissions and in-memory rollback. Optional local source-attested positive tests call with `(canonical csv, fresh WorkRAM raw, archived Mongol SaveRAM raw, archived World SaveRAM raw)`; never distribute these raw bytes. Mark fresh and archived observations separately. Linux Xvfb and extracted .deb smoke tests are port-only. Windows binary, NSIS, physical Xbox and full original campaigns are not run.
