# ROTK II Classic NES — Run019 Completion Register (2026-11-06)

Authority: SUP-0016. Project: Retro Gaming/ROTK2_NES_Windows only; Enhanced and DOS unchanged.

Immutable parent Run018 source SHA256: f343fbdbd13126bcda2b7ecc26d53cdff098fa767e23e3cf702c38d28186247b.
Run019 source SHA256: 59dec33f717481c93ab7791e3ddc47458acdd56c95c3c8392aa6ade0d841a056, 4,332,470 bytes, 164 ZIP entries all CRC clean; parent 161 unchanged, 2 modified, 1 added. Deterministic rebuild twice identical.

Original source VERIFIED_STATIC: root.src select_officer_5154 native $5154/$5177/$5191 writes every new fleeing officer to province+2 linked-list head, not only rulers or empty provinces. Corrected real Classic engine and historical Run014 assumption in the new Run019 regression only; prior source archives immutable. See https://github.com/chrisaacson69/rot3k2-snes-decompiler/blob/7c2bab7d634e22a3f5870fe548d6a0556760be98/src/root.src

Linux/Xvfb tested evidence: unchanged Run018 focused RED 3 failed / 1 passed; Run019 six focused GREEN; mutant 4 expected failed / 2 passed; scoped combined suite 304 passed, 37 skipped, 1 deselected; clean cold ZIP extraction same counts. Source-matched tests include existing garrison and consecutive army insertion; attacked-neighbor troop retention; actual on-disk .ro2 cold save/load; illegal destination atomic rollback; unknown source metadata compatibility; actual BattleScreen/YesNo/ListPrompt driven by software controller actions. NOT native Windows, physical Xbox, or original NES emulator trace replay. Historical Classic008 inventory excluded; private original-ROM fixture unavailable.

COMPLETION:
Q09-019A VERIFIED_STATIC / VERIFIED source code; runtime NES original replay OPEN.
Q09-019B TESTED_PORT / PASS ordinary nonruler source-known destination head.
Q09-019C TESTED_PORT / PASS consecutive whole-army flee destination head.
Q09-019D TESTED_PORT / PASS attacked destination 7455 battle-scale troop and .ro2 coldload.
Q09-019E TESTED_PORT / PASS atomic invalid destinations and legacy unknown-source fallback.
Q09-019F OBSERVED_RUNTIME / PASS Linux Xvfb controller action menus, no physical device.
Q09-019G REPORTED_UNREPRODUCED / OPEN original ROM trace and 30 goldens.
Q09-019H INFERRED / OPEN province linked successors and second concurrent attacked tactical battle slot.
Enhanced 1:1 battle REPORTED_UNREPRODUCED / OPEN; native Windows and current Xbox REPORTED_UNREPRODUCED / NOT RUN.

PUBLICATION BLOCKER: Run019 full ZIP and QA archive exist and were tested locally; canonical Google Drive Run019 folder created under NES root but binary uploads failed twice with container_session_unavailable. They are NOT Drive published or Director accepted. Latest fully canonical Drive-published Classic source remains immutable Run018. GitHub branch contains byte-identical four text files (two diffs, new test, builder), plus this report; it is research not canonical full source. Preserve old source, no silent mixing.

NEXT EXECUTABLE STEP: recover Run019 binary upload, verify independent Drive readback of source and QA SHA/ZIP CRC, then derive native second attacked-neighbor tactical slot and linked successor behavior from original source and replay, write fail-before/green-after fix against exact Run019 hash. Enhanced latest separately verified Run011 needs natural campaign Q09 original byte hydration without economy/BP changes. Full parity and Director acceptance OPEN.
