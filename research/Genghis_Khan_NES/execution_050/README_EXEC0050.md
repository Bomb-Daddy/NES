# Genghis Khan I — EXEC0050 native integrated two-scenario source

This is the SAME existing native C11 game inherited from EXEC0048, now with EXEC0049 source-proven 41-country scenario factory integrated. It is not a separate strategy game, a completed Windows product, nor a first-playable original NES Spring state.

Implementation: gk1_app_state.c/h keeps separate mutable original 14-country Mongol Conquest and 27-country World Conquest states, reachable by Xbox-style controller input; gk1_native_shell.c renders all 41 original country and ruler names, resources, reserves and military/economic fields in existing 1280x720 desktop UI. It retains tactical path, Divide, research cold-load/save, and battlefield viewer. Original source phases remain GK1_PRE_SETUP_TEMPLATE, never fake setup. Original first-turn player/ruler/difficulty/autostat/season processing is STILL UNKNOWN. research save != campaign save.

The new standalone C11 gk1_source_identity.c/h checks exact SHA256 of public original v1.07 41-country 45-column CSV before loading, rejecting a missing or altered file. Expected source: 10,345 bytes SHA256 fa3958f156f09d4b4c15bb559c12aa357fc617a9b1f9217505fe07e8800de581, Git blob 82126d270f45f3addcbebfd2bd196d5323476f9d. Build stages dataset next to executable, path resolved via native Linux or Win32 executable path.

Build from repository root: cmake -S research/Genghis_Khan_NES/execution_050 -B build/gk1_050 -DCMAKE_BUILD_TYPE=Debug && cmake --build build/gk1_050 -j4 && ctest --test-dir build/gk1_050 --output-on-failure

TESTED_PORT_LINUX: original local integrated tree Debug/Release/ASan+UBSan CTest 10/10 PASS each; clean source ZIP Release rebuild 10/10 PASS. Actual Linux X11 native 1280x720 World country screen 120 frames PASS; inherited controlled path and Divide 30-frame renders PASS. Source reports 14/27 originals via --scenario-report 1/2; missing/mutated source, invalid selector, bad-source GUI and overwrite controls refused. Synthetic controller input is NOT physical Xbox acceptance. No new native NES, Windows build, or installer test.

GitHub exact readback: 8 new or changed tested source files match original locally compiled Git blob SHA; 20 unchanged EXEC0048 files were linked by identical Git objects and verified. This README replaces no old file; README_EXEC0048 and SHA256_MANIFEST_EXEC0048 remain historical.

Full tested local ZIP GK1_EXEC_0050_Two_Scenario_Integrated_Native_Source_v1.00.zip is 202715 bytes SHA256 9f8c4e3df9db52deab2b180f4ce9be7df21a10438cc22e5f4a773d39a8b08855, 144 entries, ZIP integrity and clean Release rebuild passed. Direct Drive binary ZIP transfer FAILED container_session_unavailable, but code is now fully committed in this folder.

Drive master v2.32: https://docs.google.com/document/d/1_YmegBwsleM7QIp65kfH3JE-12skyAhHssSzuHoVSV4/edit
Technical handoff: https://docs.google.com/document/d/1oqC5SVmMIHmYrv0ac6OI8dsLyz6Lg6dG04R0IcX0CNo/edit
Completion ledger: https://docs.google.com/document/d/1YSQKTXzrR_YQHP2d0Mqn0r9A-pHNXH7CfqYDawj5xok/edit

Remaining: authentic first Spring and character/ruler/difficulty setup, full strategic turns and AI, diplomacy/dynasty, unrestricted battles/endings, audiovisual fidelity, campaign saves, Windows packaging and actual Xbox acceptance. Preserve Classic NES and approved Enhanced modified-ROM mechanics separately.