# Genghis Khan I English NES to Windows — EXEC0039

This is runnable, tightly bounded C11 battle session/replay code, not a complete Windows game. It links the unchanged, previously accepted EXEC0034 tactical math. Only the approved combined Shared BP/Rest + Demobilize/Fonzarelli modified-ROM controlled checkpoint fixture (7,7 to 8,7) is supported, not arbitrary actor, terrain, combat, campaign or ROM. No raw ROM, private SRAM/checkpoint, images or sounds are committed.

The new layer models one controlled Move, optional B completion and the observed exact Mobility and occupied-tile cases. The 48-byte little-endian versioned replay receipt saves a deterministic fixture event log and rebuilds it on load, checking CRC32 and a field-based state fingerprint. CRC32 detects accidental corruption, not adversarial forgery. This is a developer test-state receipt, not a real campaign save. The Linux command-line executable writes/loads real files and refuses overwrite; it is not an Xbox game UI.

Build at research/Genghis_Khan_NES:

    cmake -S execution_039 -B build039 -DCMAKE_BUILD_TYPE=Debug
    cmake --build build039
    ctest --test-dir build039 --output-on-failure
    ./build039/test_gk1_session
    ./build039/gk1_battle_replay forest
    ./build039/gk1_battle_replay friendly
    ./build039/gk1_battle_replay enemy /tmp/gk1_new_save_001.bin
    ./build039/gk1_battle_replay load /tmp/gk1_new_save_001.bin

Linux sanitizer QA:

    cmake -S execution_039 -B build039_san -DCMAKE_C_FLAGS='-fsanitize=address,undefined -fno-omit-frame-pointer' -DCMAKE_EXE_LINKER_FLAGS='-fsanitize=address,undefined'
    cmake --build build039_san
    ASAN_OPTIONS=detect_leaks=1 ./build039_san/test_gk1_session

Optional read-only old modified-ROM native archive audit (private ZIP NOT in GitHub):

    python3 execution_039/audit_private_replay.py /PRIVATE/GK1_EXEC_0033_Private_Native_Legality_Evidence_v1.00.zip ./build039/gk1_battle_replay

ACTUAL results: 76/76 strict C11 assertions PASS, 76/76 ASan+UBSan PASS, CMake CTest 1/1 PASS, independent clean source ZIP extraction Release build PASS, 36/36 checks comparing the NEW running CLI against four earlier approved modified-ROM native archive results PASS. This is old native-reference evidence reused for new code; there was NO new native NES emulator game/Windows installed application/physical Xbox test. A 48-byte file did roundtrip; overwriting was refused.

Copyright-clean local ZIP GK1_EXEC_0039_Battle_Session_Replay_Source_v1.00.zip is 15767 bytes, SHA256 7254e31f384632892b024ae42e54b7aac57baef8ead67e5cce0e4243a9fd6528, 10 entries including unchanged EXEC0034. Direct Google Drive ZIP publication failed container_session_unavailable. GitHub source files are the actual tested code; this GitHub README is a condensed summary (not byte-identical to ZIP README).

Master v2.21 https://docs.google.com/document/d/1waIP288t7lLgpN7fr11cl3CkhrsBtTXWdSm3Gm94B2c/edit
EXEC0039 handoff https://docs.google.com/document/d/1YikNMhWYgZXaT604Llb8V7Vj0v9V4_SJORLaXpItOgY/edit
G03/G04 completion ledger https://docs.google.com/document/d/1pt7wlwlutRPOORgmp8yIhK5LoDyYc1P8hC0bEDcn0r4/edit

Remaining: actual friendly joining VM caller and $613E word, Entire/Divide, multi-step movement, ranged/AI, terminal battle outcomes, full Windows game with both campaigns, authentic graphics/audio, 1280×720 controller-only Xbox UI, robust campaign saves and packaging. Do not equate bounded fixture success with a complete game.
