# GK1 EXEC0308 — final native Windows CI224 acceptance receipt

Date: Sunday November 15, 2026; source-only implementation and independent tested Windows build. Project: current complete inherited Genghis Khan I C11 WinMain/XInput game; Classic NES behavior kept distinct from Enhanced practice.

## Precise executed change

Fixed the production `gk307_physical_pad_qa` source used by the actual WinMain diagnostic route `--physical-xbox-qa`. Prior EXEC0307 marked eight physical-input steps complete immediately upon full-right analog stick deflection. New EXEC0308 requires a later **neutral/released XInputGetState sample from the same pad**. Pending terminal state: step=9, wait_release=1, complete=0, so closing while the analog stick is still held returns diagnostic incomplete exit 10. On final all-zero buttons and both axes centered, complete=1; only then may closing return exit 0. A controller switch/disconnect invalidates previous progress. Normal actual gameplay mode unaffected. This is a device-diagnostic correctness fix, NOT an original-stock first-Spring implementation.

## Provenance and executable tests

Source code, header, hostile executable test, and production SHA256-pinned stage307.py were atomically committed on GitHub at 0f217f4986579d7766df84467c4b40e9fea70f85: https://github.com/Bomb-Daddy/NES/commit/0f217f4986579d7766df84467c4b40e9fea70f85 . Source handoff: https://github.com/Bomb-Daddy/NES/tree/main/research/Genghis_Khan_NES/execution_308_physical_final_neutral . Local Linux strict C11 executable: PASS 75 assertions; ASan+UBSan PASS 75; deliberately compiling previous EXEC0307 C source against the new test correctly FAILS at `!q.complete&&q.wait_release&&q.step==9u...`, proving the regression distinguishes old/new code. No real physical Xbox controller attached here.

**TESTED_PORT_WIN32_CI:** Real native GitHub Actions run 43737379157, job 132882859979, head source commit above, 2026-11-15 16:49:46 UTC COMPLETED SUCCESS: actual inherited C11 Visual Studio WinMain game compiled, Windows CTest **224/224 PASS**, real GUI software-controller smoke passed, NSIS built, silently installed, installed EXE launched. Source-stage SHA pins/hostile QA passed before build. Physical hardware NOT RUN. Live run: https://github.com/Bomb-Daddy/NES/actions/runs/43737379157 .

## Artifact checks and publication

Original GitHub workflow artifact id 14171231483: QA-only ZIP 579062 bytes, SHA256 d054cfde3e533217a90fd089bd6ff0d2ead0d4d9600fd3269f1a2f810dbe034f. Contains Windows NSIS installer `GK1-Native-Research-1.99.0-win64.exe` size 612654 bytes, SHA256 7ce0142492ac3f55fc635055be41951cd8e0287be2ead084d5a4c1594a431092, and exact JSON manifest whose run ID and commit match, `native_ctest_passed=true`, `winmain_software_controller_passed=true`, `nsis_installed_executable_passed=true`, `physical_xbox_tested=false`, `full_campaign_tested=false`, `release_accepted=false`. ZIP CRC passes. Independent uploaded Drive readback checked exact SHA256, MIME application/zip and Collaboration parent `1d0TA1lgqsVA3AWQQ9o0FZgmvao90oCeT`: https://drive.google.com/file/d/1ZLsVMQjntgNq0b2WsCzyj3ZS1zmOniSK/view .

Reproducible C11 source and QA archive ZIP 6615 bytes, SHA256 881bf20be8ac27efeb2e8071ee0556a090d645b46be85af02735070d6da3023b; same Collaboration folder, Drive https://drive.google.com/file/d/1WyBLcxJkJ98EP91HC4XCIYH9v1sUA2vR/view. Three new production-source SHA256: C e700b54895a13422e9c5ceb3726f4163b4174abf81f6254c20e0e92e30c394bc, H 76b2a1b64f6dfc11771de6059e16af303ef1085c854638be711d01f8babd7b02, regression test a0b2e773e55fbd89db06c5d31b3b3a271f20f776287f416fec7e2797b6704e7d. Correct Drive parent checked for all; no original ROM, private NES captures, SRAM, licensed graphics or audio in public GitHub or CI binary.

## Exact acceptance classifications

- VERIFIED_STATIC: PASS — source hashes, staged production SHA pins, source and artifact publication, byte-identical Drive upload readback.
- TESTED_PORT_LINUX_C11: focused C11 75/75 PASS plus sanitizers; this increment did NOT rerun full inherited Linux CTest suite (previous EXEC0307 Linux 236/236 eligible separate).
- TESTED_PORT_WIN32_CI: new EXEC0308 native 224/224 PASS, GUI and installed NSIS launch PASS.
- OBSERVED_RUNTIME_STOCK (archived-restored vs fresh reset): previous original ROM/Mesen evidence preserved separately, not rerun here.
- PHYSICAL XBOX INPUT: NOT RUN; CI only tests injection/source route, not physical hardware.
- FULL ORIGINAL CLASSIC 14/27-COUNTRY CAMPAIGNS: NOT RUN; source-generated original first playable Spring (character/difficulty/ability RNG, all genuine records, three usable orders), autonomous AI, Trade scheduling, battle ending/aftermath, diplomacy, dynasty and complete saves still OPEN.
- RELEASE ACCEPTANCE: FALSE. Keep recurring task enabled until director explicitly stops or full release genuinely accepted. Publication is not any implementer/agent's read/execute acknowledgement.
