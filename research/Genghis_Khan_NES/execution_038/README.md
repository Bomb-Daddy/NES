# EXEC0038 — GK1 Windows tactical stage-window C11 integration (2026-11-06)

This **new compiled source** extends the existing `execution_034` tactical C11 and `execution_035` read-only NES SRAM view, without redoing accepted economy, Rest/Demobilize, audio, melee, RNG, map, or original native movement experiments.

The exported function `gk1_stage_check()` checks exactly four historical **approved modified-ROM, controlled-intervention** 3-stage SRAM captures: friend occupied, enemy occupied, low Mobility2 forest, and exact Mobility4 forest. All three snapshots must be separately controller/phase attested; it checks declared exact approved modified-ROM and checkpoint SHA256, stage VM **data** addresses (not native 6502 PCs), unchanged terrain and other 18 units, unit/word/scratch stage results. On invalid/unverified/mismatch it never modifies output. **Attestation flags are supplied by the caller; this component cannot itself authenticate logs or provenance.**

Archived friendly case: selected mover40+partner40, after attempted move mover74/partner0/word2, after idle same unit/word with scratch $614C 14->22. The *already sourced* forest casualty 40->34 leaves 34+40=74; this is numerical compatibility **not verified source join instruction order or general friendly-unit merge rule**. Enemy occupation and Mobility2 reject; exactly Mobility4 enters forest (mover34, word1). A zero-soldier record still retains position in raw SRAM.

Strict C11 `CMake/CTest 1/1 PASS`; synthetic source 168 assertions PASS; previously captured private native intervention SRAM 192 assertions PASS; ASan/UBSan 192 PASS. No new Mesen or Windows executable / physical Xbox controller run in this increment.

Build from repository `research/Genghis_Khan_NES`:
```bash
cmake -S execution_038 -B build038 -DCMAKE_BUILD_TYPE=Debug
cmake --build build038
ctest --test-dir build038 --output-on-failure
./build038/test_gk1_stage_window
./build038/test_gk1_stage_window /PATH/EXTRACTED_PRIVATE_EXEC0033
cmake -S execution_038 -B build038_san -DCMAKE_C_FLAGS='-fsanitize=address,undefined -fno-omit-frame-pointer' -DCMAKE_EXE_LINKER_FLAGS='-fsanitize=address,undefined'
cmake --build build038_san
ASAN_OPTIONS=detect_leaks=1 ./build038_san/test_gk1_stage_window /PATH/EXTRACTED_PRIVATE_EXEC0033
```
Private input directory path must contain `private/{friendly_occupied,enemy_occupied,insufficient_mobility,exact_mobility}/{selected_destination,after_attempt,after_idle}.sram`. Private archive SHA256 `aec170e8661a0011e373024caeb858f2657b716574b5f8555b63703e5cfd28f1`; original combined ROM SHA256 `4764d46359581be1e7d12bcb85bd375c84cd4497d1084d9d306a6e884afbe7c5`; source checkpoint SHA256 `e1b6266c963a48d736627812d024910b6d600c2ad8571813eb3b0999e0f2938b`. **Never publish licensed ROM, SRAM, save, screenshot or audio.**

**G03 bounded test PASS, general tactics OPEN; G04 battle endings OPEN, G11 full Windows game OPEN, G12 installed Windows 1280x720 Xbox hardware acceptance NOT RUN.** Next actual research is live developer-Mesen source breakpoint for friendly join/word2, then Entire/Divide, multistep, terminal outcomes. Previous versions and user campaign preserved.

[EXEC0038 technical handoff](https://docs.google.com/document/d/1hO1RYWJdQ8aNcMoJyMIxg5A-bJi9ANwISxYHnaw1mr0/edit) | [master v2.20](https://docs.google.com/document/d/162ZV5hNiz4expfLePoaOW4iyPe28yAw6hYUtH5YE_1Q/edit) | [incremental ledger](https://docs.google.com/document/d/1pgxpAbUBnQBPzcSuAKJXL2Nli_e36AZV93rnO-BWUUs/edit)
