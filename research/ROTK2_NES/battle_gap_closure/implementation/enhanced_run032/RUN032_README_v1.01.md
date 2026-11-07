# Enhanced Run032 — Q10 controller-visible Switch Sides opponent/offer

Date: 2026-11-07; scope: original-game Enhanced battle mode only; not Classic NES source and not DOS.

## Exact source ancestry and immutable rollback
Parent Run031 Drive source ZIP SHA256 `6f3e6f62cd4bfd167464f727b8a942ef0fb6e0301209951d983330edcfe9c74c` (549 members); source rebuilt by direct deterministic member-preserving derivative. New Run032 SHA256 `014ae2dcdc189395ee17f6d52d4568d526952180a7d4a4311583033741bd0e95`, 148775233 bytes, 550 CRC-clean ZIP entries. Parent 548/549 member payloads unchanged; only `third_branch/tactical_view.py` changed; one seven-case test added. Two deterministic complete builds match SHA byte-for-byte. Keep Run031 immutable.

## Source-backed correction and exact confidence
Original Q10 Answer Matrix v1.4 hsovl1 `officer_list_dispatch $D5DD`: Switch Sides targets an opposing live noncommander, gold offer limited to 1..min(side gold,99), offer consumed regardless of refusal. Before Run032, Enhanced six-command in-canvas Tk controller relied on stale external `selected_target`, not D-pad selection; commander could be inadvertently targeted or correct enemy inaccessible. The source-backed corrected flow uses Tactics > Switch sides > D-pad visible opponent hex > A > offer gold D-pad > A to issue genuine `battle_bribe` with bound officer and target identity; B cancels, invalid stale targets/offer/actor are rejected atomically. Original NES exact messages/pixels unverified; no invented formula; existing battle adapter Q10 retained and approved Enhanced strategic economy/BP left unchanged.

## Executed evidence, Linux Python 3.13.5
- Unmodified Run031 source RED 7 failed (behaviorally), exit 1 (see red_test_final.log).
- Corrected Run032 GREEN 7 passed, exit 0 (green_restored.log).
- Wrong no-commander exclusion mutant RED 2 failed/5 pass, exit 1; restored all 7 passed (mutant_test.log).
- Inherited selected old real-game/Tk battle suite: 121 passed, exit 0 (broad_test.log).
- Cold-extracted complete Run032 source tested 32 passed, exit 0 (cold_tests.log).
- Actual Tk 1280x720 six-command in-canvas controller interaction via Xvfb; `Switch sides`, commander invalid, noncommander real selection, offer gold, engine spend + officer conservation and actual `.ro2` disk save with independent second Python process cold-load verified by matching Campaign fingerprint (automated test contains both subprocess and assert).
- Two full ZIP builds byte-identical, parent and new full SHA and all 550 ZIP CRC clean; source part local hashes in manifest; FULL GOOGLE DRIVE BINARY READBACK VERIFIED: all 18 part SHA, reassembled ZIP SHA and 550 CRCs, QA hash and CRC, correct folder ID 1qqxvXs_drruZeCnlpWhXQ5_C_5_9rhTS; 32 PASS from Drive-cold source.
- Unrestricted test suite, original 30 NES events, native Windows, physical Xbox controller, and release parity NOT RUN.

## Open tasks / blockers
Authenticated original-Q09 automatic second tactical battle and four-list scheduling still OPEN, original emulator event traces and complete Classic-Enhanced equal-state RNG parity OPEN. Source identity and Q10 source rules VERIFIED_STATIC, scope implementation TESTED_PORT, UI OBSERVED_RUNTIME Linux Xvfb; hardware and original goldens REPORTED_UNREPRODUCED NOT RUN; full parity INFERRED OPEN. The exact NES phrase for failed input is unverified.

## Next executable work
Source binaries are published and independently verified in original-game Drive. NEXT source-constrained Q09 attacked-recipient second tactical battle original evidence, then red-before/green-after/mutant/controller/conservation/.ro2 and emulator fixture comparisons.
