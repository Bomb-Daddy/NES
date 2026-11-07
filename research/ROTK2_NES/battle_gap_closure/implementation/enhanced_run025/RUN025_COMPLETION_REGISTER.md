# Original-KOEI Enhanced Run025 — Q09 Flee destination province-ID stability

Date 2026-11-07. Scope: Enhanced ONLY; canonical Classic NES (SUP-0016) and DOS unmodified. Binding Director requirement is one-to-one observable English NES battle gameplay for Classic and Enhanced under equal state/RNG, not identical source structure. No changes to approved Enhanced strategic marketplace, economy, or Body Points.

## Exact verified parent and new source
- Immutable Run024 parent complete ZIP SHA-256: `d48cdf9bf04999db31568cb303da92cdb2e402dd4cebbbcb5ba7e66e6491b0b8` (Original-game Drive folder 1fU0jc-gHBDw72kxC5R5IZqocp0Pb4zjr).
- New Run025 local complete ZIP SHA-256: `3159e3e916d1ad821e1245940a8df9e5d66bb35e4085c7c01c3c0ac306c22509`; 148,757,724 bytes, 543 CRC-good ZIP members, deterministic double-build same SHA. 541/542 old members identical; only production `third_branch/tactical_view.py` changed and a six-case test added.
- Full source ZIP and 18 numbered parts produced locally. **Drive binary delivery BLOCKED** by `container_session_unavailable` for Files upload and `BLOCKED_FILE_REFERENCE` for Drive connector upload; no independent Drive binary readback. Do not promote Run025 as Drive-published source. Run024 remains latest fully Drive-verified.
- New correctly parented original-game folder: https://drive.google.com/drive/folders/1Sb5IgzCIQE3ZcVdKEhdIZe7Eg1-ZUHSf (folder placement verified, content delivery incomplete).

## Source-backed gameplay correction
Original English NES Q09 hsovl2 D89D addresses an explicit destination province. Before correction the nested Flee UI used a live reordered legal-destination list index. Selected province p7 could silently become p6 if legal route iteration reordered; an illegalated selected p7 or previously staged whole-army destination could be silently replaced. Run025 freezes the visible destination province-ID choices, maps directional controller input against the same choices through commit, validates selected and all staged per-officer destinations before executing, and fails closed without invoking the battle command or consuming RNG when legality changed. Existing pending-withdraw API shape is preserved. Error text is provisional. This is a tested port correction, **not** an authenticated original NES emulator golden.

## Actual executed Linux Python 3.13 / Xvfb / Tk QA
- Pristine Run024 parent with new six-case test: **5 FAIL, 1 PASS (expected RED)**; final clean parent log retained.
- Corrected targeted **6 PASS**; inherited Run018–24 plus new **49 PASS**; expanded battle and genuine Tk/software-controller subset **82 PASS**.
- Deliberately reintroduced live-index mutant: **4 FAIL, 2 PASS (expected RED)**; restored fixed code: **6 PASS**.
- Clean separate full-source ZIP extraction: **82 PASS**.
- Actual nested NES-style Flee→Officer controller UI selected p7; adjacency reorders; underlying battle performs withdrawal to p7 and writes a real `.ro2`.
- OS process #1 saved, process #2 cold-loaded same fingerprint `c34c04f0c97207d014307c33a6cc80e7db330c9c1fcacee54a1d9bb74dc3497b`; save bytes 9,883 and disk SHA-256 `510d059a395ebf72fa6694962deafafb9b1653fb022ec53a2e8775ba222ad7e9`.
- Unrestricted suite NOT RUN, inherited private original ROM + rollback import blockers/long-running hangs remain. Original English NES 30 event goldens, actual native Windows and physical Xbox NOT RUN.

## Completion register classifications
RUN025-NEW-SOURCE VERIFIED_STATIC / PASS LOCAL, BLOCKED Drive independent readback.
RUN025-Q09-FLEE-DESTINATION TESTED_PORT / PASS scoped.
RUN025-TK-CONTROLLER OBSERVED_RUNTIME / PASS on Linux software input only.
RUN025-RED-GREEN-MUTANT TESTED_PORT / PASS.
RUN025-2PROCESS-SAVE TESTED_PORT / PASS.
RUN025-ORIGINAL-30-GOLDENS REPORTED_UNREPRODUCED / NOT RUN.
RUN025-WINDOWS-XBOX REPORTED_UNREPRODUCED / NOT RUN current build.
RUN025-AUTO-SECOND-BATTLE INFERRED / OPEN.
DIRECTOR 1:1 ACCEPTANCE / OPEN.
No source/QA originals overwritten.

## Exact next action
Retry binary Drive source-part delivery from a valid connector-exposed file reference, re-download all 18 parts and compare every part hash, full reconstructed SHA and ZIP CRC. Then resume original-event-backed Q09 attacked-destination second simultaneous tactical battle / four-slot turn scheduler from exact Run025 source, with parent RED, child GREEN, mutation, Tk controller, troops, 2-process disk cold-load, 30 NES golden event comparisons, Windows/Xbox acceptance. Do not invent scheduler behavior or substitute GitHub evidence for canonical original-game source.
