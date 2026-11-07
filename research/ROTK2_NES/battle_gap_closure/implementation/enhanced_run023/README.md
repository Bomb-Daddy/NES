# Enhanced Run023 — Q09 selected individual Flee actor integrity (2026-11-07)

STATUS: TESTED_PORT / PARTIAL, not final 1:1 or Windows release.

Canonical source location: Original-KOEI Enhanced Google Drive folder https://drive.google.com/drive/folders/16ycsr_en5zyLQ59fFFPFbWItI6VAMCUN
Source parent: Run022 immutable full ZIP SHA256 b4206359b205fd853a2f9288331611c5e2d03e5ed57d540ff370a92124bbb747
New tested Run023 full ZIP SHA256 776d20447d5d5f7820dbbca0ae2cb486e02a1c1edd33ccf902c3497937e9b157, 148753524 bytes, 541 CRC-clean files. 539 of 540 parent files unchanged. No edits to original NES Classic or DOS. No Enhanced marketplace/economy/BP changes.

One production correction: third_branch/tactical_view.py. On NES-style Flee->Officer, stale/missing/enemy selected officer used to silently redirect the order to the first available ally. This contradicts original hsovl2 D905 selection semantics. Now the action is rejected without selecting another actor or changing game state. Exact invalid-cursor message not original-NES verified.

Executed actual Linux/Xvfb:
- Parent RED 4 failed / 1 passed.
- Fixed focused 37 passed.
- Deliberately inverted guard mutant 5 failed.
- Broad applicable suite excluding missing private ROM, historical rollback import, and an independently hanging windows_port test: 762 passed / 2 skipped / 2 xfailed / 3 subtests passed.
- Cold extracted 26 passed.
- Actual Tk six-command software-controller path passed; no native Windows or physical Xbox tested.
- Two-process disk .ro2 save/load exact fingerprint 84457016766ec32b8aa966bcdfff75a3c99211c9f97c7b98873310675983eff1.
- Complete Drive original-game 18/18 parts individually re-fetched and SHA256 checked, full ZIP reconstructed SHA256 exact and 541 CRC passed. Independently Drive-retrieved QA ZIP SHA256 ba9b1a82bd890148246626be37c82b94c70191b4a9cb65718d807aae07b6719d and CRC passed.

Known open: automatic attacked-destination second tactical battle and 4-list scheduling; original 30 NES event goldens; native Windows and physical Xbox; Director release acceptance. Unfiltered suite has two inherited collection errors, not green.

This GitHub folder contains research/evidence only; entire canonical Enhanced game remains in original-game Drive. Keep older source/handoffs immutable. NEXT: authenticated original source/emulator fixtures for simultaneous Q09 receiving battle schedule and four-list turn/actor order, then separate next patch with RED/GREEN/mutant/GUI/conservation/save/cold and original event goldens.
