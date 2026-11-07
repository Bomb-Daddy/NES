# ENHANCED RUN021 COMPLETION REGISTER v1.00 — 2026-11-07

Product: SEPARATE Original-KOEI Enhanced; not canonical NES Classic, DOS or a released Windows build.
Director binding: original English NES ROTK II one-to-one player-observable BATTLE MODE, approved Enhanced marketplace/economy/BP isolated. NOT FULLY VERIFIED.

Actual direct immutable parent: Enhanced Run020 full ZIP SHA256 `0f05ba1c3f46e433609a116a903e198942049ab0ef49ab797cab5e25bc2ecc15` (538 CRC clean members, original-game Run020 folder 1Yim5DKG9XbvXo9GZQx4PvKP3wlBCfGah).
New tested Run021 full source SHA256 `8dba021a3dc5dbfab5a8ec56cb1a9b5a5d990cc410c9ad6cee22e83c3deb5337`, 148,749,312 bytes, 539 CRC-clean ZIP members; 537 of 538 parent members unchanged, modified one production file and added one test. Deterministic independent rebuild EXACT SHA.

## Gameplay correction
`third_branch/tactical_view.py` controller `_issue` previously resolved `actor=self._officer()` BEFORE correcting a stale enemy/nonfield cursor to the original human defender head. Actual campaign's Wait effect was executed for defender B9, but event journal recorded stale C2/B1, violating command identity and trace parity. Now resolve actor only AFTER selecting the real human officer. Source-derived Q09 receiving list order B9/B2/C1 is retained. No strategic economy or BP changes. No automatic concurrent second battle is claimed.

## Executed evidence (actual Linux Python3.13 Xvfb software controller)
- PRISTINE Run020 with four new *real Campaign/Tk controller* regressions: 2 FAILED (wrong actor) / 2 PASSED, expected RED, log `RUN021_PRISTINE_PARENT_EXPECTED_RED.txt`.
- Corrected Run021 plus inherited Run020: 11 passed; larger focused Run018/Run019/Run020/Run021: 25 passed.
- Mutant intentionally restored prepatch actor order: 2 EXPECTED FAIL / 2 PASS. Corrected restoration 11 PASSED.
- Selected real engine/controller/strategy broad suite: 351 PASSED / 2 SKIPPED in 6.99 sec. Cold source extraction: same 351 PASSED / 2 SKIPPED in 7.54 sec.
- Actual NES six-command Tk UI Wait + B canceled choice (1280×720 in existing Run020 GUI tests), real campaign log and disk save verified.
- Process 1 GUI Wait creates `.ro2`; process 2 independently cold-loads byte-identical campaign fingerprint `423b4bb7c390b73b01df4d7d4814da8de3869669bff3464c4570263482b49a53`; validates log actor=result=B9 and full game invariants.
- Full unrestricted collection BLOCKED by missing private ROTK2_TEST_ROM and historical rollback collector; tests excluded explicitly, never claimed all green.
- Original emulator 30 event goldens NOT RUN; native Windows, physical Xbox controller NOT RUN; Director release acceptance NOT ESTABLISHED.

## Item-level statuses
- ENH021-PARENT: VERIFIED_STATIC / PASS — exact Run020 immutable source hash & ZIP CRC.
- ENH021-CODE: VERIFIED_STATIC / PASS — one production correction, no other source modifications.
- ENH021-PARENT-RED: TESTED_PORT / PASS — 2 real failures before patch, 2 controls passed.
- ENH021-FOCUSED: TESTED_PORT / PASS — 11 restored focused passes, 25 inherited selected passes.
- ENH021-MUTANT: TESTED_PORT / PASS — 2 intended failures reproduced.
- ENH021-BROAD: TESTED_PORT / PASS PARTIAL — Linux/Xvfb 351 pass / 2 skip, same cold source.
- ENH021-UI: OBSERVED_RUNTIME / PASS scoped — software-controller Wait and cancel on real Tk; no physical controller.
- ENH021-SAVE: TESTED_PORT / PASS — actual 2-process .ro2 disk cold-load and invariant validation.
- ENH021-PARTS: VERIFIED_STATIC / PASS LOCALLY — 18-part split reconstructs exact source SHA/CRC. Drive readback remains pending unless separate publication addendum documents it.
- ENH021-ORIGINAL-SECOND-SLOT: INFERRED / OPEN — automatic simultaneous attacked-destination second tactical battle scheduling and four-list order unimplemented.
- ENH021-NES30: REPORTED_UNREPRODUCED / NOT RUN — original emulator ROM unavailable.
- ENH021-WINDOWS-XBOX: REPORTED_UNREPRODUCED / NOT RUN — Linux Xvfb only.
- ENH021-FINAL-ONE-TO-ONE: INFERRED / OPEN — Director acceptance not requested or established.

Exact executable next step: derive source/event fixture for original Q09 automatic simultaneous second tactical slot, wire real four-list turn order and controller phases, and prove with fail-before/green-after/mutant/30 original event goldens, genuine Windows + physical Xbox and save/cold-load. Current correction only fixes actual command-actor identity.
