# Original-KOEI Enhanced Run039 — Completion Register v1.00

Date 2026-11-07. Product: Original-KOEI Enhanced (root `1i5kLog2o1tC9c2dsjdF2CLFnXYQHVk_l`), **not** Classic NES (SUP-0016) or DOS. No approved Enhanced economy/marketplace/body-points change. All immutable parents preserved.

Exact parent Run038 (Google Drive original-game root) complete ZIP SHA256 `1a1b193c1bb8f4aa5f5f343bbcc9c5617d4889cd2b550f5a8a03aa2041c3e14f`, 556 CRC-clean members, 148791351 bytes. New Run039 exact complete ZIP SHA256 `e2a1581af46f900fe16cc311566a27946320689ffb99e8a0ad21e72278153935`, 557 CRC-clean members, 148793567 bytes, 554/556 parent members byte-identical. Two independent deterministic builds produced same full ZIP SHA256.

| Item | Classification | Status | Evidence/limitation |
|---|---|---|---|
| Exact Run038 source ancestry | VERIFIED_STATIC | PASS | Rebuilt chain Run035→036→037→038 from pinned published patches, exact reported full archive hashes and CRC on each, parent ZIP preserved |
| Run039 complete local source | VERIFIED_STATIC | PASS | ZIP SHA256 above, 557 CRC-clean, 554/556 unchanged, deterministic duplicate SHA |
| Q08 occupied friendly unit receives Fire Trick terrain+unit check | TESTED_PORT | PASS SCOPED | Original source Q08 `hex:$818A`/table `$C99C` has occupant-dependent unit Int and casualty, does not discriminate target side in documented source; replaced artificial `FIRE_ALLY_CELL_UNVERIFIED` veto |
| Original selected cell and stale target protection | TESTED_PORT | PASS SCOPED | Real engine/coordinate controller tests; stale explicit officer ID rejected; non-Fire attacks still cannot target own side |
| Rain/controlled RNG/conservation | TESTED_PORT | PASS SCOPED | Rain no ignition and no new RNG/turn, valid casualty/conservation and state.validate tests |
| Fail-before / green-after | TESTED_PORT | PASS | True pristine Run038 new regressions 5 FAIL / 2 PASS RED; Run039 24 PASS focused and inherited |
| Mutation negative control | TESTED_PORT | PASS | Reverted ally-cell veto mutant causes 3 expected FAIL / 4 controls PASS; restored 7 PASS |
| Applicable broad regression | TESTED_PORT | PASS SELECTED | 206 PASSED Linux/Xvfb, no skips in `tests/test_run0*.py`; unrestricted suite not run |
| Complete cold extracted source | TESTED_PORT | PASS SELECTED | Independent cold full ZIP extraction, 24 PASS |
| Actual GUI six-command control | OBSERVED_RUNTIME | PASS LINUX ONLY | 1280×720 Tk software A/D-pad and Attack→Fire source-backed selected friendly occupied cell; B cancel |
| `.ro2` disk save and separate-process load | OBSERVED_RUNTIME | PASS LINUX ONLY | Seventh regression saves real file and independently loads via another Python process |
| Original ROM NES 30 event goldens | REPORTED_UNREPRODUCED | NOT RUN | No original English NES emulator comparison; friendly target controller acceptance/message still must be observed |
| Actual Windows executable or physical Xbox current build | REPORTED_UNREPRODUCED | NOT RUN | Linux software-controller only |
| Q09 automatic concurrent attacked destination tactical battle | INFERRED | OPEN | Authenticated original four-slot scheduling/event trace still needed |
| Provisional seven-province UI vs 41 original battlefields | INFERRED | OPEN | Full English NES visual/UI parity unproven |
| Unrestricted full suite and Director acceptance | INFERRED | OPEN | Source-backed scoped fix is not final 1:1 release |

Publication of binary source and QA requires independent Google Drive per-part SHA, full reassembly CRC, QA readback. Document's initial statuses are local, not claims of Drive publication; publication addendum follows after remote verification.

## VERIFIED PUBLICATION ADDENDUM — 2026-11-07

Drive original-game folder ID `1SEsq7mVnbkG2UO6JkhkvJlwaGFbj8_BK` has **18/18** raw source parts; each part was independently downloaded through the mounted Drive reader into a different `drive_readback/` directory and checked against per-part size and SHA256, all **PASS**. Byte-for-byte reassembled full source archive exact SHA256 `e2a1581af46f900fe16cc311566a27946320689ffb99e8a0ad21e72278153935`, 148793567 bytes, 557 ZIP members CRC CLEAN; full source was independently cold extracted and 206 applicable Linux/Xvfb regression tests PASS. Separately downloaded QA ZIP SHA256 `beb89a2caf33fd455a1a493bac453db5ae710fcd0d3f135784d3a2d55ec1b2fe`, CRC CLEAN. Direct original-game parent folder verified. DRIVE SOURCE status **VERIFIED_STATIC PASS**; source code scoped behavior **TESTED_PORT PASS**; genuine Windows/Xbox/original NES 30 goldens/Director release remain NOT RUN or OPEN. Do not claim these are final acceptance. See `RUN039_INDEPENDENT_DRIVE_READBACK.json` and `DRIVE_COLD_BROAD.log`.
