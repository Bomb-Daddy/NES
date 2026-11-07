# Enhanced Run018 recovery and Run019 publication VERIFIED_STATIC addendum

Date: 2026-11-07 (America/Detroit); destination: original-game source root, NOT canonical NES Classic or DOS.

**Run018 prior blocker RESOLVED.** All 18 exact original Run018 source-part bytes and QA ZIP were published in the existing immutable Run018 Original-game Drive folder `1jfOKUY10stIyNpSbKEve_WGitXhIodAn`, independently retrieved as a new container copy, independently SHA-256 checked part-by-part and concatenated, and all 535 ZIP CRCs validated. Exact Run018 full SHA-256: `95a9e1133684d7b18841d275bf8d32d0b45b814b30521836cbf5922dd56e3ba5`; QA SHA `6184ef11fdc7136927a0447a3c55f9f0055006d8a0b303248919e15f8672813b`.

**Run019 new implementation:** exact direct parent Run018 source above. Original-game Enhanced Run019 full source 148,745,801 bytes, 537 CRC-clean ZIP members, SHA-256 `9a47e07278d5d98a9f3cedcdc0e366a4dbb435241ce9f1777c607def32e65ad6`. 18 source parts and QA ZIP uploaded to Original-game Run019 folder `1tRlHb-rOQIHFvhURnuDYx7fdiizRYHXJ`, correct parent folder verified. An INDEPENDENT retrieval of every source part and QA file from Google Drive checked 18 individual SHA-256 and sizes; full reassembled ZIP SHA and all 537 CRCs PASSED. Independently downloaded QA ZIP SHA `145fc3f22df732a0e9dbff695e4b66ceb2671ae9bddd0e1fa6695cb0e76ab142` and CRC PASS. Re-extracted independently downloaded Drive source under Linux Xvfb and reran targeted 80 PASS.

Code changed only `third_branch/officer_battle_adapter.py` (534 unchanged inherited parent files, 1 changed, 2 additions). Source-tagged Q09 attacked-destination pending arrivals retain their original source list-HEAD order and `troops100` when a *subsequent actual campaign attack* initializes a tactical battle in recipient province; source pending removed once after real battle creation, no duplicate strategic troops; malformed state rejected. This is **NOT a simultaneous second tactical-battle scheduler**. Only one partial observable parity correction.

Evidence classes:
- ENH018-DRIVE: VERIFIED_STATIC PASS; previous 077 “BLOCKED” was true at time and is now superseded only for binary delivery.
- ENH019-PARENT/SOURCE/DRIVE: VERIFIED_STATIC PASS; 18/18 SHA and full CRC.
- ENH019-Q09-ATTACHMENT: TESTED_PORT PARTIAL PASS; old parent RED 3 failed/2 passed, corrected 7 focused PASS, 3 negative mutant regressions caught; broad Linux/Xvfb selected 329 PASS/2 SKIPPED.
- ENH019-CONTROLLER: OBSERVED_RUNTIME PASS limited to Linux Xvfb Tk software controller six-menu and B summary; physical Xbox NOT RUN.
- ENH019-SAVE: TESTED_PORT PASS: actual .ro2 save in process 1 and cold-load in separate Python process 2, exact fingerprint `8d3db534fd36e766c368b12aec964a6ab44afa3b1ce89e8a7517fd8439ea29ed`.
- ENH019-SIMULTANEOUS-TACTICAL-SLOT: INFERRED OPEN, original per-event 30 NES goldens REPORTED_UNREPRODUCED NOT RUN, native Windows/current Xbox REPORTED_UNREPRODUCED NOT RUN; full unfiltered suite remains BLOCKED by private ROM and historical import; Director acceptance OPEN.

**Next executable correction**: source-confirm, implement and fully test actual second simultaneous receiving tactical battle activation and four-slot turn topology on the exact Run019 SHA source, with fail-before/green-after/mutant, controller-visible real GUI, save/cold-load, and original event golden(s). No alternate combat mechanics allowed; preserve Enhanced economy/BP and Classic NES/DOS separation. Immutable Run017/Run018, both Run015 siblings and source-backed tests preserved.