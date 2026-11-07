# Run021 Original-KOEI Enhanced — Independent Drive Source Readback PASS (Nov 7, 2026)

Source direct immutable parent Run020 SHA256 `0f05ba1c3f46e433609a116a903e198942049ab0ef49ab797cab5e25bc2ecc15`. New Run021 full ZIP SHA256 `8dba021a3dc5dbfab5a8ec56cb1a9b5a5d990cc410c9ad6cee22e83c3deb5337`, 148,749,312 bytes, 539 CRC-clean ZIP entries. QA archive SHA256 `c680425c2b453357f7f0f9b2f5a4140617b7cd225a6b0768958d4f1829e97f59`.

Canonical separate Original-game Drive folder: `https://drive.google.com/drive/folders/1LmaygIYie-Yyb4XeeV7pOJVcDaEya4d7`; actual parent `1i5kLog2o1tC9c2dsjdF2CLFnXYQHVk_l`.

**INDEPENDENT READBACK:** All 18/18 uploaded complete source parts independently re-materialized from Google Drive into fresh container directory, each separately SHA256 checked against upload manifest. Bytewise concatenated parts, full ZIP SHA256 exact, 539 archive members all CRC clean. Also independently downloaded Drive QA archive, SHA256 exact, CRC clean. A fresh extraction FROM THE INDEPENDENTLY RETRIEVED DRIVE SOURCE ran selected applicable real-engine tests under actual Linux/Xvfb: **351 PASSED, 2 SKIPPED, 0 FAIL**, 7.31 seconds. Full unfiltered suite NOT green due to missing private `ROTK2_TEST_ROM` plus historical rollback collection. No original NES emulated 30 traces, Windows native or physical Xbox controller tests this run.

**Scope:** `third_branch/tactical_view.py` fixes command-actor identity stale enemy/ally cursor before controller event journal. Not an automatic second tactical battle scheduler. No approved Enhanced economy/market/BP modifications. Classic NES Run021 and DOS remain separate.

**Classification:** Run021 SOURCE `VERIFIED_STATIC / PASS`; Run021 controller real gameplay `TESTED_PORT / PASS scoped`; Run021 real Tk `OBSERVED_RUNTIME / PASS Linux/Xvfb software`; Run021 2-process .ro2 `TESTED_PORT / PASS`; Run021 original 30 event goldens/Windows/Xbox `REPORTED_UNREPRODUCED / NOT RUN`; 1:1 full Director acceptance `INFERRED / OPEN`. Not accepted final release.
