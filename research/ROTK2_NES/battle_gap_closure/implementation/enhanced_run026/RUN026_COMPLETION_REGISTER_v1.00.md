# Enhanced Run026: Q04/Q05 selected officer remains bound through targeting
Date 2026-11-07 | Original-KOEI Enhanced ONLY. Separate canonical NES Classic SUP-0016 and DOS unchanged.

Exact verified Run025 source parent SHA-256: 3159e3e916d1ad821e1245940a8df9e5d66bb35e4085c7c01c3c0ac306c22509
New local Run026 ZIP SHA-256: c0bc5d97f8377509fee92a1184bf1877ed2cc2e3f999312f14c80d650893929e
Source ZIP size: 148759806 bytes; 544 ZIP entries CRC clean. Exactly 542 of 543 parent entries unchanged; only third_branch/tactical_view.py production changed, new tests/test_run026_q04_q05_target_actor_identity.py added. Two deterministic builds matched.

Original behavior provenance: Battle Answer Matrix Q03/Q04/Q05 SNES source selected-unit turn and deployment. This is cross-port behavioral evidence, NOT one of 30 original NES emulator event goldens. Enhanced Move/Move enemy/Attack target modal previously could silently reassign an officer after a stale/mid-modal selection or use the attacker coordinates when the human defender's selected officer had fled. Bind selected live officer on modal opening, reject changed/absent officer atomically at confirm, preserve valid Move and B Cancel. Legacy aggregate-only saves keep their preexisting fallback.

Actual Linux Python3.13 Xvfb/Tk tests: pristine parent RED 6 fail/2 pass; corrected Run026 focused 8 pass; hostile mutation 4 intended failures/4 passes; inherited Q09/Tk 62 pass; broader original-battle/army/economy/BP 351 pass/2 skip; cold-extracted full source 62 pass; actual graphical selected-officer confirmation rejected atomically; actual .ro2 save writer + distinct-process loader fingerprint e72ed816be0973a0e7d8b614773cc036ed9cbdc69e517bd4133e79edb88dee22 (9471 bytes). Unfiltered full collection not run; private ROTK2_TEST_ROM test unavailable. No original NES30, native Windows, physical Xbox or release acceptance.

Completion Register: source VERIFIED_STATIC PASS local; source Drive upload BLOCKED; gameplay TESTED_PORT PASS scoped; GUI OBSERVED_RUNTIME PASS Linux software input; RED/GREEN/mutant TESTED_PORT PASS; 2-process save TESTED_PORT PASS; NES30, Windows/Xbox REPORTED_UNREPRODUCED NOT RUN; Q09 second tactical scheduler/four-slot order INFERRED OPEN; overall PARTIAL.

Drive original-game folder for Run026: https://drive.google.com/drive/folders/189s5tSxTJUGYGxe-YopsponuXaHPxe7P . Connector raw file upload failed BLOCKED_FILE_REFERENCE; full 18 parts not published or independently fetched. Earlier Run025 binary remains blocked; Run024 last Drive-verified. Local full source/QA archives available as conversation files, not certified Drive publication.

Next: restore safe connector source binary egress for Run025 then Run026, independent per-part SHA and ZIP CRC; obtain original Q09 automatic attacked-destination second battle scheduling/four-list event fixtures; patch against exact hash, run emulator-golden/regression/GUI/cold/physical Windows/Xbox checks before acceptance.