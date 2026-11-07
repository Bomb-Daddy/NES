# Enhanced Run040 — Q11 former-subject duel damage

Exact immutable parent **Enhanced Run039** Drive-retrieved SHA256: e2a1581af46f900fe16cc311566a27946320689ffb99e8a0ad21e72278153935. New full **Run040 SHA256: 19bd16739b8a168b3c5b5a0d5fbcf9f7032777c9967f4815ea1852e1f5e7308a**, 148795770 bytes, **558 CRC-clean members**, 556 of 557 original entries byte-identical.

Original v1.4 Q11 hsovl52 and ai_eval_odds_b specifies reduced War/10 duel-damage ceiling for original protected special flags **and** officers historically subordinate to the opponent's ruler. Run039 implemented only the special bits. Run040 uses explicit previously-switched plus original-faction fields for known former subjects; unknown provenance does not create a fictitious match. Production change only third_branch/officer_battle_adapter.py, new seven-case regression test. Original-game marketplace, economy, BP and Classic NES/DOS untouched.

Reproduce byte-for-byte with **rebuild_run040_from_github.py** and sibling **test_run040_q11_former_subject_duel.py**, feeding exact parent Run039 full ZIP. Builder was run and reproduced full Run040 SHA. Parent and previous ZIP immutable.

Actual tests on Linux Python3.13.5 and Xvfb: pristine source **2 FAILED, 5 PASSED** expected RED; patched **7 PASSED**; old-guard mutant **2 intended FAIL, 5 controls PASSED**; applicable broad suite **213 PASSED** (working and cold extracted source); inherited cold Q11 **16 PASSED**. Actual Tk 1280x720 six-command battle UI and duel dispatch observed; real .ro2 disk save, second-process cold load verified.

**BLOCKED: Google Drive source publication.** Correct Original-game Run040 folder created at https://drive.google.com/drive/folders/1TcVy3A1UtLEn0yFr3eXtosAvWGv4Z6cn but attempted binary egress returned container_session_unavailable; bounded retry identical and direct file reference BLOCKED_FILE_REFERENCE. Full Run040 source and QA are built/tested locally, GitHub patch/test/rebuilder published; **Run039 remains latest independently Drive-verified full source**.

Open: Q09 attacked-recipient automatic second tactical battle/four-slot scheduling needs authentic original event/source; natural original officer provenance hydration, full 41-map and six-command original UI/pixel parity, 30 NES event goldens, genuine current Windows/physical Xbox and Director acceptance NOT verified. This is TESTED_PORT/PARTIAL, not 1:1 release.
