# Enhanced Run019 Q09 recipient actual-battle source attachment: Completion Register v1.00

Immutable original-game Enhanced parent Run018: 95a9e1133684d7b18841d275bf8d32d0b45b814b30521836cbf5922dd56e3ba5
Run019 tested complete candidate: 9a47e07278d5d98a9f3cedcdc0e366a4dbb435241ce9f1777c607def32e65ad6 (148745801 bytes, 537 ZIP entries, CRC verified).
ONE production file changes: `third_branch/officer_battle_adapter.py`; two new files: regression test and README; 534 unchanged parent members byte-for-byte, original 535 parent members retained. Source is NOT Classic NES and NOT DOS.

| Item | Evidence class | Status | Details |
|--|--|--|--|
| ENH019-PARENT | VERIFIED_STATIC | PASS | Run018 18 parts independently reconstructed from Original-game Drive, exact SHA/CRC |
| ENH019-ORDER | TESTED_PORT | PASS/PARTIAL | Attacked recipient pending HEAD list promoted into actual Campaign battle on subsequently issued `attack` command; source fixture hsovl2 D648/D69A, not NES emulator replay |
| ENH019-HUNDREDTHS | TESTED_PORT | PASS/PARTIAL | Original pending source `troops100=5075` preserved while strategic station 50; no duplicated soldiers |
| ENH019-PENDING-CONSUMPTION | TESTED_PORT | PASS/PARTIAL | Dequeue after valid existing recipient battle created, no double insertion |
| ENH019-PARENT-RED | TESTED_PORT | PASS | Old Run018: 3 failed/2 passed on five new non-Tk regressions |
| ENH019-FOCUSED | TESTED_PORT | PASS | Corrected: 7 passed, including GUI under Xvfb |
| ENH019-NEGATIVE | TESTED_PORT | PASS | 3 mutants each raised expected regression failure, rollback exact |
| ENH019-BROAD | TESTED_PORT | PASS SCOPED | 329 passed/2 skipped, Linux Xvfb Python 3.13. Full unrestricted suite not claimed. |
| ENH019-COLD-SOURCE | TESTED_PORT | PASS | Extracted new complete 537-member ZIP 7 focused pass, 80 selected pass |
| ENH019-CONTROLLER | OBSERVED_RUNTIME | PASS LINUX | Real Tk window, 1280x720, six root NES commands, software B button; NOT Windows/physical Xbox |
| ENH019-DISK-SAVE | TESTED_PORT | PASS | Actual .ro2 written in separate process then cold-loaded; fingerprint 8d3db534fd36e766c368b12aec964a6ab44afa3b1ce89e8a7517fd8439ea29ed |
| ENH019-SECOND-SIMULTANEOUS-BATTLE | INFERRED | OPEN | No original event fixture defines auto second slot; ONLY later manually started recipient battle corrected |
| ENH019-FULL-FIDELITY | REPORTED_UNREPRODUCED | OPEN | NES 30 goldens, Windows native, physical Xbox not performed; no Director acceptance |
| ENH019-FULL-PYTEST | REPORTED_UNREPRODUCED | BLOCKED/PARTIAL | Private `ROTK2_TEST_ROM` and historical rollback collector, earlier full run timeout; 329 scoped pass only |
| ENH019-PUBLICATION | VERIFIED_STATIC | LOCAL DONE, DRIVE PENDING READBACK | Verify each of 18 new Drive parts and SHA/CRC before promotion |

**Exact next executable**: Source-verify actual simultaneous second tactical receiving battle and original four-list turn scheduling, then implement using this Run019 SHA only after independent Drive readback; 30 original NES event goldens, native Windows/Xbox and natural scenario Q09 source-byte provenance remain OPEN. Save/cold-load and inventory invariants required for next correction. Do not elevate delayed recipient battle fixture into 1:1 original second battle.