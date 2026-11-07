# Enhanced Run020 — Q09 player-defender selection, completion register v1.00
Date: November 7, 2026 (America/Detroit).

## Binding source and ancestry
Original-KOEI Enhanced (separate from NES Classic and DOS). Director demands 1:1 original English NES observable battle, not code identity. Enhanced marketplace/economy/BP outside battle remain untouched.
Exact new direct parent: independently Drive-verified Enhanced Run019 **B** ZIP SHA-256 `9a47e07278d5d98a9f3cedcdc0e366a4dbb435241ce9f1777c607def32e65ad6`, 537 CRC-clean members. Its sibling Enhanced Run019 **A** `5887f9a83bbb6f360b487195e7be7c1382a1081230be6d451bac1f231f65067f` is NOT its parent. Immutable ancestor Run018 SHA `95a9e1133684d7b18841d275bf8d32d0b45b814b30521836cbf5922dd56e3ba5`.
Run020 ZIP SHA-256 `0f05ba1c3f46e433609a116a903e198942049ab0ef49ab797cab5e25bc2ecc15`, 148747831 bytes, 538 CRC-clean members, 536/537 parent members unchanged. Only actual game module `third_branch/officer_battle_adapter.py` changed; one new test module added. Deterministic rebuild twice identical SHA.

## Concrete player defect and correction
When a human commands defending faction B and officers from Q09 source-provenance escape have arrived in an attacked province, `start_tactical_battle` previously put the attacking enemy A officer in `battle.selected_officer`, regardless of human side. Default NES Wait yielded NO_SUCH_BATTLE_OFFICER, a real player-facing control failure. Run020 initializes the battle cursor to the first live deployed, non-reserve human-side officer, preserving Run019B original D-list HEAD order. Includes ordinary defender start and unchanged attacker defaults. Does NOT add automatic second concurrent tactical-battle scheduling, four-list round order, or original NES 30 goldens.

## Actual tests (Linux Python 3.13, Xvfb software-controller only)
Unmodified parent RED: **6 FAILED / 1 PASSED** (`RUN020_RED_BEFORE.txt`). Corrected focused Run018/019/020: **21 PASSED**. Negative mutation (force A-side selected): **6 expected FAIL / 1 PASS**, then restore and 7 PASS. Selected broad source regression: **274 PASSED / 2 SKIPPED** across 56 modules. Fresh ZIP extracted independent tree: same **274 PASSED / 2 SKIPPED**. Actual Tk TacticalWindow rendered 1280x720 with NES six-command menu and controller B summary, passed under Xvfb (software event, no physical Xbox). Actual disk `.ro2` saved in Python process #1; Python process #2 cold-loaded with identical fingerprint `c5a4abc2b4decea56d510a23e1344736ce3676e40a3d990ee526156131d34fd1`, preserved selected officer B9 and successfully executed NES Wait, no duplicate strategic/tactical troops. Source ZIP CRC verified.
Full unrestricted suite NOT claimed green: historical rollback import and absent private `ROTK2_TEST_ROM` cause known collection blockers; extended exclusion run progressed beyond 75% then timed out (log preserved).

## Classification and status
- ENH020-IMMUTABLE-PARENT: **VERIFIED_STATIC PASS**, independently downloaded Run019B 18 Drive source parts, exact 9a47 ZIP SHA, 537 CRC.
- ENH020-SOURCE: **VERIFIED_STATIC PASS local**, exact 0f05 ZIP, 538 CRC; Drive upload/readback PENDING until independently verified.
- ENH020-INITIAL-DEFENDER-SELECTION: **TESTED_PORT PASS scoped**, actual source correction, no proof of total 1:1.
- ENH020-GUI: **OBSERVED_RUNTIME PASS Linux Xvfb software-controller**, 1280x720, NES root command B summary; physical hardware **REPORTED_UNREPRODUCED NOT RUN**.
- ENH020-NEGATIVE: **TESTED_PORT PASS** mutant caught.
- ENH020-COLD-LOAD: **TESTED_PORT PASS** actual `.ro2` disk, two independent processes, correct default Wait and fingerprint.
- ENH020-SIMULTANEOUS-SECOND-BATTLE: **INFERRED OPEN** original second tactical slot and four-list ordering not implemented.
- ENH020-NES30: **REPORTED_UNREPRODUCED NOT RUN**, absent source ROM access.
- ENH020-WINDOWS-XBOX: **REPORTED_UNREPRODUCED NOT RUN**, no Windows or physical controller.
- ENH020-COMPLETE-1TO1: **INFERRED OPEN**, Director acceptance not established.

## Exact next executable
From Run020 exact ZIP SHA 0f05: capture an original NES/SNES unified Q09 attacked destination fixed replay for automatic second tactical-battle slot; implement actual scheduling/4-list actor order, red-before/green-after/mutants, controller-visible Linux UI, save/cold-load, all 30 trace fixtures when available, then native Windows+physical Xbox. Preserve all immutable Enhanced siblings, Classic Run021 and DOS. Agent acknowledgment not established.