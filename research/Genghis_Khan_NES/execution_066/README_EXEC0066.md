# EXEC0066: Manual battle terminal what-if controller UI (not game completion)

Source baseline: exact EXEC0065 Drive archive SHA256 31ac4840732388f40b981df8272697527a87398ce067dd813e2955fe36161954. Preserves complete predecessor and all original data, combat, saves and prior manual rules.

Manual pp27–28 documented commander deaths, fleeing, starvation, surrender, capture in duel, simultaneous double KO and time expiration. This update makes the inherited manual clock a real controller-operable **explicit hypothetical terminal selector**. In the research setup wizard press View to read manual-country preview, Start to open battle timeline, D-pad Up/Down selects event, Left/Right selects explicit victim for single-commander endings, A applies, B returns. Re-entering resets the scratch timeline. Invalid/finished-event application leaves terminal state unchanged. A normal no-result side action still increments the manual 30/60/90 clock. Command-victim selection prevents accidental winner inference from acting side. Native X11 and Win32 rendering source show the event, victim and terminal outcome. No original ROM event has been observed, no battle RNG or country conquest is synthesized, the pre-setup campaign world is never mutated.

Still open: original first Spring, source merchant scheduling/trading, strategic AI/turn orders, diplomacy, dynasty, real outcomes/capture, original assets and Windows / actual Xbox acceptance. Manual contradictions daughter's marriage 6 vs 8 and Classic stock cavalry Divide 15 vs Enhanced observed 13 not resolved or generalized.

## EXEC0066 independently tested source and delivery
Full source+historical QA ZIP (1,788,564 bytes, SHA256 d29b65ca004c1f667fd6395ffb97656ed9ceedfb1db51b5b5433783b3133a84e) is attached to current conversation as GK1_EXEC_0066_Manual_Battle_WhatIf_Integrated_Native_Source_v1.00.zip. Linux .deb 79,370 bytes, SHA256 ff5f5b8182daa5bc9329a6516fe734d8574c08247d928b73a2e3b3c5724ef5db. Test result: strict GCC C11 Debug/Release/ASan-UBSan 24/24 each, independently clean ZIP Release24/24, new direct test73 checks, native Linux X11 1280x720 whatif100 frames, scenario/divide/market40 frames, Debian package extract/save/cold-load all PASS. Windows Win32 source is uncompiled. Actual X11 screenshot in Drive https://drive.google.com/file/d/1gzOxqoocXi2dkU7nPPik2X93qNSyIfZo/view. Source ZIP/.deb direct Drive upload blocked container_session_unavailable; do not claim binary publication or Drive hashes.

Full cumulative master v2.47: https://docs.google.com/document/d/1tLZsgTmsWFws0gibXm4JdzFLQDtsSyJm5BOn8wKnDvE/edit
New handoff: https://docs.google.com/document/d/1lWCms-QU_nxcaXG0epKkTABVulAFyZW3AaKIsh3ivAI/edit
Completion ledger: https://docs.google.com/document/d/1tTfmK5_ardujx88iJK1KtupOX1kjgNENsYRNWZSuAVE/edit

**Publication caveat:** This GitHub EXEC0066 directory currently has the independently uploaded TEST and README ONLY. The actual integrated C11 app and full inherited source remain in conversation ZIP; do NOT treat the test and README as buildable standalone source. Existing full previous EXEC0065 source remains separately published. No Windows/Xbox/stock Mesen acceptance claimed.
