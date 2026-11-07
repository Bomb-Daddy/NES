# SUP-OG-ENH-BPF-20261106 Run015 — Q14 no-refuge ordinary release

Date: 2026-11-06, America/Detroit. **Status:** Executed Linux TESTED_PORT, PARTIAL gameplay parity. This report describes the new Original-KOEI Enhanced implementation candidate, NOT the separate canonical NES Classic branch, DOS restoration, installed Windows executable, or Director-accepted release.

## Source authority and ancestry

Director requires NES ROTK II *observable* battle performance and interface, not identical source structure; Enhanced marketplace/economy/Body Points outside combat remain approved. Governing authority: `CURRENT_AUTHORITY_AND_SUPERSESSION_REGISTER_2026-10-28`, maintained `READ_FIRST — KOEI CURRENT SOURCE AND VERSION REGISTER`, revised hostile QA plan v1.01, `SUP-OG-ENH-BPF-20261106-057`, and newest Enhanced Run014 numbered handoff 071, all read before editing. Correct separate ORIGINAL-GAME root Drive ID `1i5kLog2o1tC9c2dsjdF2CLFnXYQHVk_l`; not NES Classic SUP-0016.

Parent: **Run014** immutable full source ZIP SHA-256 `cb61dbdd5ecd26fdadb7b1c700f75734e2457c5f74c4d94ac6621e2b0a56cf06` / 148,724,467 bytes / 528 CRC-clean entries; all 18 source parts independently downloaded from prior published Drive folder and every part hash and full reassembled ZIP hash reverified this run. New **Run015** ZIP SHA-256 `0a438906a3245e1fe56a66ac6ecb4ba303f51fca39d9297faec43c2addb33ea6` / 148,726,616 bytes / 530 CRC-clean entries, built twice with exact matching ZIP hashes; 525 previous members byte-for-byte unchanged. Exactly three production source members modified: `third_branch/integrated.py`, `third_branch/officer_battle_adapter.py`, `third_branch/historical_s1.py`. Two new members: regression tests and README. Neither Classic Run021 nor DOS source edited.

## Player-observable correction

Original shared SNES `sengo $84F6` / `root $4EB0` source establishes that an ordinary captured officer **Released with no eligible refuge** becomes an unaffiliated free officer in the contested province, with zero soldiers and training reset to 100. Run014 instead raised `NO_RELEASE_DESTINATION`, disallowing a legal original decision. Run015 preserves officer identity in a separate persistent `free_officers` ledger, removes faction/loyalty/army bindings rather than inventing faction ownership, resets training to 100, and makes this real officer appear as recruitable in that actual province via the existing Enhanced command. Invalid or remote recruitment rolls back byte-identically, and corrupt dual membership or phantom army is rejected. The ruler no-refuge prohibition is retained. The existing Enhanced recruit BP and loyalty policy is unchanged (not claimed to be an NES recruitment formula).

## Actual executed tests and limitations

- Parent pristine Run014 with new tests copied in **RED: 6 failed, 0 passed**, including real Tk/Xvfb six-command prisoner UI.
- Corrected actual Run015 focused **6 passed**, including software controller View -> Prisoner fate, Release ordinary, Behead ruler, file save and reload.
- Deliberate mutant deleting training=100 RESET: **6 expected failures**; restored exact candidate **6 passed**.
- Actual Linux Python 3.13/Xvfb scoped 53-module suite **268 passed / 2 skipped**, exit 0; independent fully **COLD ZIP EXTRACTION 268 passed / 2 skipped**, exit 0. Skips preserved.
- Independent process #1 `save_file` real `.ro2` 7,746 bytes, process #2 `load_file`, SHA fingerprint `0d06c0b0e884d7ea5f355357681b12546cf9bb02601d60871902be307ff63771`, free officer, training and conservation exact.
- Unfiltered collection **717 tests collected / 2 errors**, exit2: missing private `ROTK2_TEST_ROM`, and legacy `test_ui_integrity_v19_BUILD13.rollback.py` module filename. Full suite **NOT GREEN**.
- Original NES emulator event replay and 30 original goldens **NOT RUN**. Native Windows package and current-build physical Xbox **NOT RUN**, despite older unrelated launcher tests. Director complete 1:1 acceptance **NOT GRANTED**.

## Completion Register update

| Item | Evidence class | Status |
|---|---|---|
| ENH-BPF-015-Q14-A no-refuge ordinary free-officer original source | VERIFIED_STATIC | PASS source rule, emulator replay OPEN |
| ENH-BPF-015-Q14-B real Enhanced Campaign free ledger, training 100, troop conservation | TESTED_PORT | PASS focused+scoped, partial full parity |
| ENH-BPF-015-Q14-C existing strategic recruitable list and actual recruitment | TESTED_PORT | PASS, original recruitment formula NOT VERIFIED |
| ENH-BPF-015-Q14-D controller prisoner UI and cancellation | OBSERVED_RUNTIME | PASS Linux Xvfb, current Windows/Xbox NOT RUN |
| ENH-BPF-015-Q14-E pristine red / patched green / deliberate negative | TESTED_PORT | PASS 6 RED / 6 GREEN / 6 mutant failures |
| ENH-BPF-015-Q14-F extracted source and two-process .ro2 cold-load | TESTED_PORT | PASS 268/2 and fingerprint |
| ENH-BPF-015-Q14-G 18 source parts SHA, full ZIP and Drive readback | VERIFIED_STATIC | Local PASS; DRIVE status updated on actual upload/readback, never presumed |
| ENH-BPF-015-Q14-H Q14 special flags and ruler succession | INFERRED | PARTIAL OPEN; not source-fixtured |
| ENH-BPF-015-Q09-I natural original metadata, under-attack second battle slot | REPORTED_UNREPRODUCED | OPEN |
| ENH-BPF-015-PARITY-J 30 NES original goldens, native Windows, physical Xbox, Director | REPORTED_UNREPRODUCED | NOT RUN / NOT ACCEPTED |

## Next exact executable correction

On EXACT Run015 source SHA guard, source-derive original Q14 special-flag and ruler/no-refuge successor branch (e.g. defeated Liu Biao/Cai Mao), or Q09 original ordinary source-byte hydration; implement the smallest confirmed difference in actual Enhanced Campaign with fail-before/green-after, mutation and cold-load. Original under-attack destination second simultaneous battle slot is blocked on verified four-slot topology; never invent source bytes, troop caps or fake reserves. Continue no changes to Enhanced approved economy/BP, canonical Classic Run021 or DOS. Once this source is uploaded to the verified Original-game folder, independently re-download all source parts, verify per-part SHA and full ZIP CRC and record exact Drive IDs and parent, and deliver numbered Collaboration handoff with readback. Agent read acknowledgment and physical release acceptance stay separate.
