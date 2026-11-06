# Run021 — Q09 source-middle/tail officer unlink, Classic only
Date: 2026-11-06. Status: **TESTED_PORT PARTIAL / DRIVE SOURCE BINARY DELIVERY BLOCKED**. This research branch is NOT canonical game source and no Director release acceptance is implied.

Canonical: Google Drive Retro Gaming/ROTK2_NES_Windows (SUP-0016), project root 1ODnfdVL5R3bZBBUp4cXpocNqtTClrxIL. No Original-KOEI Enhanced or DOS source was modified. Tested immutable parent Run020 SHA256 `964d9ad55c50eaab944bf8f70ff257a230494b1ff6a799e152d88561b21ea345`, exact Drive ID `1khlKVWq0P947o_7iYIYj837rxuQBH1Wz`. New full candidate Run021 SHA256 `93a9acb7dee1819bd6f04a761b5b0aacbc315b4d8b92472269135b204b6e9eb3`, 4,335,974 bytes, 166 CRC-clean members, 164 old files unchanged.

Observable fix: actual Classic battle engine `_q09_apply_immediate_source_flee` now unlinks a fleeing middle/tail officer from its old province's **known linked resident officer list** before inserting as destination head. Previously only a departing head was unlinked, leaving predecessor pointing into another province. Unknown/malformed officer links remain unresolved rather than fabricated.

Original-source oracle VERIFIED_STATIC (not emulator): root source $50EF–$5121 (`select_via_69ea`) and $5154–$5195 (`select_officer_5154`) from SHA-pinned public decompiler [source](https://github.com/chrisaacson69/rot3k2-snes-decompiler/blob/7c2bab7d634e22a3f5870fe548d6a0556760be98/src/root.src).

Executed Linux Python 3.13 + Xvfb results:
- pristine Run020 fail-before new tests: **4 failed / 2 passed** (expected red);
- Run021 seven focused tests **7 passed**;
- deliberate relink mutant **5 expected failures / 2 passed**; restored **7 passed**;
- applicable working tree **317 passed / 37 skipped / 1 deselected**;
- cold extracted actual complete Run021 source **317 passed / 37 skipped / 1 deselected**;
- `.ro2` disk save/load, exact game state hash, invariants and actual software-controller Flee UI covered by focused tests;
- unfiltered collection still blocked by missing private `ROTK2_TEST_ROM` input, historical exact inventory test deliberately deselected.
- Native Windows, physical Xbox, original NES emulator 30 goldens and parity with separate Enhanced **NOT RUN**.

Files here are exact production diff, full focused regression and **hash-locked deterministic ZIP rebuild script**; the latter takes exact original Run020 ZIP, an output ZIP path, and adjacent new test file, and asserts whole SHA+CRC. Full source/QA candidate ZIPs are local artifacts in the execution container. Attempted correct canonical Drive child-folder publication, but `container_session_unavailable` blocked source and QA `files.manage_library` uploads; direct connector attempt returned `BLOCKED_FILE_REFERENCE`. Until independent Drive download/CRC readback, **Run020 remains latest fully Drive-published Classic**. Do not overwrite old source.

Completion categories: VERIFIED_STATIC source behavior pass; TESTED_PORT focused/broad/cold pass; OBSERVED_RUNTIME Linux software controller pass; REPORTED_UNREPRODUCED original NES/Windows/physical controller not run; INFERRED simultaneous second battle slot and missing normal-campaign officer source metadata remain open.

Next executable: regain binary connector egress and independently read back candidate; investigate attacked-destination second simultaneous battle slot with source fixtures and controlled RNG, then fail-before/green-after and real controller/cold-load checks from exact Run021 parent.
