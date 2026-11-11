# GK1 EXEC0199 — Native implementation and hostile-QA handoff (2026-11-11)

## Source and immediate predecessor

EXEC0199 version 1.99.0-dev1 inherits **complete EXEC0198** from the exact 46,976-entry published historical ZIP, not stale EXEC0076 or a disconnected test harness. Entire C11 app has 631 files (627 retained/modified + four added); previous ZIP entries remain byte-identical. Preserves original separate Genghis NES versus ROTK2 and original KOEI-inspired game, Classic/Enhanced separation and source-provenance gate.

## Hands-on change in existing native application

`gk199_classic_tax_undo.c/.h` is linked by the actual game `gk1_app_state` CMake target. On verified original Classic TAX, Y now replays the attested original source checkpoint and earlier TAX/ASSIGN action history and undoes the last **unpassed TAX action**, including refund of the third order; selection returns to the actual previous tax rate. The action refuses unrelated ASSIGN, Pass, empty action list and forged or corrupt source/derived state, atomically preserving the state and selection on refusal. The original 83-byte NES country records remain immutable. Live native gamepad mapping routes Y on TAX and ASSIGN correctly, and X provides private save access on TAX. This is **PORT-ONLY controller convenience**, not asserted stock-KOEI behavior. No original source RNG or merchant presence manufactured.

## Reproduce

```
cmake -S execution_199 -B build199 -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build199 -j12
ctest --test-dir build199 --output-on-failure -j16
cmake -S execution_199 -B build199_debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build199_debug -j12
ctest --test-dir build199_debug --output-on-failure -j16
./build199/test_gk199_classic_tax_undo <original_41_country_CSV> <private_fresh_original_JSON> <private_archived_mongol_MSS> <private_archived_world_MSS>
xvfb-run -a -s '-screen 0 1280x720x24' build199/gk1_native_shell --smoke-classic-tax-original <one_of_three_private_states> 120
(cd build199 && cpack -G DEB)
```

To reproduce the delta: copy the full `execution_198` source directory to `execution_199` and run `patch --directory execution_199 -p1 < GK1_EXEC0199_Complete_Integration_From0198.patch`; ensure source SHA manifest matches. Historical exact dependency folders remain sibling directories.

## QA actual outcomes

- **VERIFIED_STATIC:** 15 changed/added source files, complete 631 SHA-256 manifest, original stock ROM identity checked privately, source ZIP 46976 predecessor members preserved, ZIP CRC integrity PASS, clean patch replay **631/631 exact source hashes PASS**, independent clean-patch Release **162/162 CTest PASS**. Artifacts SHA-256 tracked.
- **TESTED_PORT_LINUX:** CMake Release **162/162**, Debug **162/162** tests; private authentic fresh WorkRAM and archived Mongol/World SaveRAM **33 assertions PASS**. Focused ASan/UBSan **33 assertions PASS**. 1280x720 Xvfb simulated gamepad three120-frame runs exit0. Actual Debian-extracted Linux executable 120-frame session exit0. Only simulation, **not physical Xbox**.
- **OBSERVED_RUNTIME_STOCK_FRESH_RESET:** earlier Mesen fresh Spring1175 country1 Gold430 Rate121 orders3 under specific reset choices; no new Mesen execution.
- **OBSERVED_RUNTIME_STOCK_ARCHIVED_RESTORED:** earlier Mesen archived original Spring1175 country1 Gold439 Rate101 and World1206 Gold193 Rate114, separately; no new Mesen execution.
- **NOT RUN:** Win32 executable, NSIS installer, hardware Xbox, authentic new stock Mesen parity, entire campaigns, daughter marriage age6/7/8, genuine natural water or war terminal.

## Outstanding original fidelity milestone

Current original 14/27 first-Spring startup/difficulty/character RNG and real strategic command execution STILL NOT independently generated. Classic TAX action currently port/manual-derived; its ability/morale effects remain unverified. Trade screen only shows quotes; merchant presence/RNG, autonomous AI, diplomacy, dynasty and battle terminal remain missing. Do not infer completion or external agent acknowledgment from publication. Continue hourly development.

## Privacy publication gate

Never upload original stock ROM SHA `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3` nor private original Mesen screenshots/states and 83-byte records to public GitHub; this submission includes only project-authored copyright-clean source, text/test code and logs.
