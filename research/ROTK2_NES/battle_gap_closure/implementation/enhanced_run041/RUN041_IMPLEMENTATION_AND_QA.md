# Enhanced Run041 — Q04 interactive placement + Q11 historical former-subject duel (2026-11-07)

**Project: separate Original-KOEI Enhanced game ONLY.** GitHub research evidence is not the canonical source. Canonical full source published and independently verified in Original-game Drive: https://drive.google.com/drive/folders/1jRtgqUvj13WtZ437OqVLcS8-X-kAMlm5 (direct parent Original-game root ID 1i5kLog2o1tC9c2dsjdF2CLFnXYQHVk_l).

## Exact real ancestry
- Historical Run039 common ancestor: SHA256 `e2a1581af46f900fe16cc311566a27946320689ffb99e8a0ad21e72278153935` (557 ZIP files).
- **Direct immutable parent**: parallel Q04 manual human placement candidate, SHA256 `03784c8b8e3698884727e21961dd893948a3ffefd89e3885961619be940544e9` (148801252 bytes, 560 entries).
- **Independent sibling donor, NOT parent**: exact Run040 Q11 former-subject duel, SHA256 `19bd16739b8a168b3c5b5a0d5fbcf9f7032777c9967f4815ea1852e1f5e7308a` (558 entries). Q11 source and regression test copied byte-exactly, no other donor content.
- **New Run041** full source SHA256 `c99691f5f0134cda74e022f5a3d34bce0822188c772ce458ef9c03ffb8543d98`, 148803455 bytes, 561 entries, 559 of 560 direct-parent members unchanged. Only production `third_branch/officer_battle_adapter.py` changed; added `tests/test_run040_q11_former_subject_duel.py`. All historical ZIPs immutable.

Q11 original hsovl52 $DD5B source-backed former subject duel War//10 damage ceiling activated only with explicit switched and original_faction opposing current ruler; unknown history not fabricated. Preserved Q04 original 41-map human deployment engine/Tk six-command controller. Q04 manual pre-phase STILL opt-in `q04_manual_deployment=True`, so final 1:1 NES acceptance OPEN. Director explicitly rejected hypothesizing a separate automatic second simultaneous tactical battle scheduler.

## Executed independent QA
Environment Linux Python 3.13.5 / Xvfb actual Tk 1280x720 software controller. Untouched Q04 parent new test RED 2 FAIL/13 controls PASS; corrected GREEN 15 PASS; mutant disabled Q11 guard 2 expected FAIL/13 controls PASS; restored GREEN 15 PASS. 273 selected broad real engine/controller tests PASS in working tree; 273 full ZIP cold PASS; 273 independently Drive-reconstructed cold ZIP PASS. Actual .ro2 disk save and two-process cold-load included. Bundled 41 source maps, 227 region checks, 1750 valid candidate cells, three invalid off-grid rejects. Unrestricted pytest collected 893 with TWO known errors (missing private `ROTK2_TEST_ROM`, obsolete historic rollback dotted import), NOT full GREEN. No original NES 30 controlled emulator goldens, no current-build native Windows, no current physical Xbox. Director full 1:1 not accepted.

**Drive readback**: ALL 18 source parts re-downloaded individually, per-part size/SHA PASS; reconstructed exactly full Run041 SHA and 561 CRCs PASS. Independently re-downloaded base QA ZIP SHA256 `7ec475032acfca09062144dc8c935aa13523e77a8c7363ca951c2007c0970e4f`, 24 CRC clean; additional post-Drive cold QA also delivered.

Numbered full agent handoff: https://docs.google.com/document/d/1j2_xJXxbt7vCI-_HUf0dtgpSmr_K_V-VJky5wJShNvc/edit (correct Collaboration parent, verified content; agent ACK unknown). Latest maintained live register https://docs.google.com/document/d/1UXDXguwS0EGKKTInRWZraSFWabRD6pSuYHq9LKTBjZE/edit.

**Next:** Source-first minimal Q04 historical player deployment observable parity gap, only on current hash-verified latest; preserve immutable source, economy/BP, Classic/DOS separation. Patch only observed source-backed behavior; RED/GREEN/mutation/controller/save/cold and 18-part independent Drive SHA readback. 
