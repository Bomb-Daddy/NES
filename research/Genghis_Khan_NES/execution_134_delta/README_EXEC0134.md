# GK1 EXEC0134 — Correct stock command routing

This is a real source correction in the same complete inherited C11 Linux X11 / Windows Win32 native game, built from the full EXEC0133 historical source. It is not an isolated app or a claimed completed playable campaign.

The native on-screen commands in 0-based order are TAX(0), ASSIGN(1), GIVE(2), TRAIN(3), TRADE(4), PEOPLE(5), VIEW(6), MOVE(7), POLICY(8), TREATY(9), SPY(10), WAR(11), PASS(12), OTHER(13). Before this correction, pressing A on TAX erroneously entered Enhanced Assign. The source patch routes original TAX to the locked-stock refusal and ONLY Enhanced ASSIGN at index1 to its properly source-attested Enhanced labor screen. Original Classic ASSIGN remains source-locked. VIEW and PASS retain previously verified behavior. TRADE remains source-verified **read-only** reference: no merchant RNG or transaction invented.

New public C11 gk134_stock_command_route.c/.h is linked into the existing gk1_app_state static library and actual controller dispatch; new test_gk134_stock_command_route.c exercises all fourteen mapping identities and negative controls, plus positive authenticated checkpoint input for fresh original 1175, archived original Mongol1175, and World1206. Previously existing test_gk119_order_bridge.c corrected its positive case from TAX index0 to ASSIGN index1. Project/package version 1.34.0-dev1.

Tests (native): Linux Release 92/92 CTest PASS, Debug 92/92 PASS, focused ASan+UBSan negative+all-three-private-authentic profiles PASS, X11/Xvfb 1280x720 120 frames PASS, Debian-extracted binary 120 frames PASS. Exact authentic private record fingerprints validated against original stored memory bank and distinct stock histories. No new Mesen gameplay run. **NOT RUN** Win32, NSIS, physical Xbox, complete campaigns. Authentic independently generated first Spring and original three orders remain unfinished.

For the exact complete integrated code, old history, test and CMake changes, use the versioned EXEC0134 full historical source ZIP (SHA256 553e2944577def62a8e26d1199a0bb8f19d5b67dd21d71e3fc9a3cd393e9c349) in verified Collaboration Drive: https://drive.google.com/file/d/1GXY_VVzbCEDVxuGYJGUGdHEtcQT3N3gQ/view . The reproducible patch and build logs are in QA ZIP: https://drive.google.com/file/d/1Yed84BU5Aqtt1KZjSq3JnJjeG5PSOpvl/view .

This GitHub public delta intentionally contains no licensed original ROM, RAM snapshots, save states, images or audio. Publication is not developer execution acknowledgement.
