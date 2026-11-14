# GK1 EXEC0288 — Independent final verification receipt

2026-11-14. **Code + tests actually executed and published**; no complete-game parity or release claim.

## Native production code and tests
- New source module: `gk288_original_rng_stream.{c,h}` integrated with unchanged inherited complete native `Gk1App` via `gk1_app_state.{c,h}` and `execution_199/CMakeLists.txt`.
- `gk288_calculate_original_stream` source-executes 1–64 consecutive NES fixed-bank `$C289` original-stock RNG draws, conditional on explicitly supplied seed and selected Classic difficulty. 15-bit draws and candidate Rate outputs are independently replayed against existing `gk137` model.
- Both campaigns guarded at PRE_SETUP template: Mongol 14 and World 27 country identity; mutation, Enhanced, checkpoint and other invalid stages refused. No actual playable Spring or real order grant claimed.
- Full native Linux Debug C11 build and executable shell **PASS**; eligible CTest **228/228 PASS** with five inherited exclusions; optional separately executed exact-owner-ROM regression **2051 assertions PASS** using authenticated private ROM; correct SHA256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`.
- Modified source in 6 paths; patch dry replay against untouched complete predecessor **PASS**, all six physical file outputs identical byte-for-byte; complete inherited source ZIP **1218 members**, CRC **PASS**.

## Published GitHub source — all seven blob SHA-1 values independently matched local tested bytes
- `gk288_original_rng_stream.c`: `9e27f69fb7079f03499ae8c56cff72b1c08439da`
- `gk288_original_rng_stream.h`: `6df6d1c8f12b3e95dcd7956dc435added5e75b41`
- `test_gk288_original_rng_stream.c`: `9e77fcf5e9ababc0d27b13b2c8f2b871a876eb3c`
- `GK1_EXEC0288_Implementation.patch`: `9ff690be0286b357d6cfd7181111e3cb85d85ed2`
- `GK1_EXEC0288_Source_Changed_SHA256.json`: `fa2cc5dee9e67e630ff592869ce46009e5d932ee`
- `README.md`: `d4e477223dafbfa187405577c0719c0dd1f79c0a`
- `GK1_EXEC0288_Implementer_HostileQA_Handoff.md`: `19f7c202f40c9d274df66826ed2788808a6f3e0d`

GitHub: https://github.com/Bomb-Daddy/NES/tree/main/research/Genghis_Khan_NES/execution_288_original_rng_stream
Final GitHub source commit: `3cab89cb798c176498bd044bcc133ada11c011cb`.

## Published private collaboration (correct parent folder id 1d0TA1lgqsVA3AWQQ9o0FZgmvao90oCeT)
- Complete inherited source ZIP: https://drive.google.com/file/d/1npdYHuycl7F0BVf92x1AeV3KfqvNA_3m/view — 5,628,895 bytes SHA256 `43c50c37a5a2809e15fc1c93e66aba27cf96d45af5836610cb087d8367c9cae4`.
- Reproducible source/tests/handoff delta ZIP: https://drive.google.com/file/d/12oB4xOGh24oWi2369nD3DL68jl07pAtN/view — 24,204 bytes SHA256 `573e69c8fb58f1176bd5cfa5923d80d17b0c419d54cd1e1ad4b5413e61102414`.
- Executable source patch: https://drive.google.com/file/d/1vkmf2Vwcer7syyImFsbIildzEfvM-pbK/view.
- Handoff: https://drive.google.com/file/d/10hIR72qCcBn_yWLCkwYBqWQk2GKhwLVs/view.
- Master addendum: https://drive.google.com/file/d/1DVyKQRV2OXwQhFbJDQKVbxus9hGBUavp/view.
- Completion ledger: https://drive.google.com/file/d/1GNMfXAjXPxSmAVxRwH-71XNxaLD5hA5l/view.
- Full source, delta, patch, handoff, and master were separately **downloaded from Drive, byte-for-byte SHA matched**, ZIP CRC verified.

## Honest acceptance
VERIFIED_STATIC: PASS. TESTED_PORT_LINUX: Debug build, 228/228 eligible CTest, optional original private ROM 2051 assertion PASS. SOURCE_EXECUTED_ORIGINAL_STOCK_ROM: PASS for original source arithmetic, **not** authentic first Spring. OBSERVED_RUNTIME_STOCK (fresh WorkRAM and archived SaveRAM): **historical only**; no new Mesen. TESTED_PORT_WIN32_CI: **NOT RUN EXEC0288**; inherited EXEC0287 Windows 216/216 retained. Physical Xbox and full campaigns: **NOT RUN**.

Critical ongoing milestone OPEN: exact NES startup seed writers and controller/NMI setup order; genuinely generated ruler/difficulty/character first Spring for all 14/27 records with real three orders. Also incomplete Trade merchant scheduler, AI, battle aftermath, diplomacy/dynasty, full campaigns. The recovered original ROM and licensed material remain private, and ongoing work continues.
