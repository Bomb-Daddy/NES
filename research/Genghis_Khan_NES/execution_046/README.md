# EXEC0046 — Integrated tactical path, Linux-native playable slice

**Status:** new executable Linux-port integration, NOT new NES gameplay or Windows acceptance. The complete, locally verified copyright-clean 35-file source ZIP is `GK1_EXEC_0046_Integrated_Tactical_Path_Linux_Source_v1.00.zip` (54,610 bytes; SHA256 `beab89662b11210f364267e81ab9de46a7990e2efbec4ad419e2f8db27a1d47f`). This GitHub directory contains only the independently published subset, not the complete source bundle.

EXEC0046 actually links EXEC0045 modified-NES observed-turn adapter into the earlier 1280x720 C11 desktop shell. The native Linux UI offers three controlled two-step/combined friendly scenarios, gamepad cursor, A to attempt, B to dismiss/complete. Changes to soldiers, position, Mobility, partner count, completion word and stage are shown on the live screen. Unverified moves are refused without changing battle state. A separate 64-byte exclusive-save GK1OP046 research receipt is validated by CRC32 and canonical replay; it is not a campaign save. Previous 48-byte replay and read-only native snapshot viewer remain unchanged.

Linux GCC C11 Debug, Release and ASan+UBSan CTest all 6/6; independent source-ZIP extraction/Release rebuild CTest 6/6; Linux X11 1280x720 120-frame GUI screenshot visually inspected. Three separate Linux shell save/export and restore/import checks passed: raw7 34 soldiers at(8,7) word1, controlled raw2 34 at(9,7) word1, friendly join 74 plus partner0 word2. File overwrite, forged event chronology, wrong ROM profile and CRC damage refused.

These are **source-bounded** controlled approved combined Shared BP/Rest modified-ROM reference fixtures from genuine EXEC0045 NES research. They do NOT prove arbitrary combat, original stock Classic mode, actual Divide quantity/splitting, campaigns, battle AI/terminal, Windows compilation, physical Xbox QA or an installer. Matching emulator/checkpoint were not executed in this increment.

Drive cumulative master v2.28: https://docs.google.com/document/d/1dhZHUSdrZM4qakHNxJ8j9SEgU9IJL2lMfIfSRRNnlBU/edit
Drive handoff: https://docs.google.com/document/d/1Rz4dzmDGZr_QVxJQQgpDlovNNZBcKnpSVUNOTcZs0XM/edit
Drive completion ledger: https://docs.google.com/document/d/1zO9R1Pdyss2aFw0WLopcrUDjWOb3qVELAaWk1lXEE3k/edit

No private ROMs, source emulator binaries, checkpoints, screenshots or user campaigns are distributed publicly.
