# GK1 EXEC0214 — Implementer + hostile QA handoff — 2026-11-12

## Executed actual C11 change

Starting from independently verified latest EXEC0213 complete native game, integrated gk214_spring_batch.c/.h in the existing gk1_app_state library, exposed read-only API gk1_app_spring_batch_projection and actual native CLI --audit-both-spring-finance-batches. Computes source-derived Spring food/tax/troop payroll across ALL 14 Mongol or 27 World initial templates in one atomic operation; rejects wrong 14/27 IDs, invalid values or bad world, never modifies campaign world, refuses false promotion of PRE_SETUP to PLAYABLE. Retains all inherited source/test/packaging assets, both Classic and Enhanced modes. No ROM, RAM, saves or copyrighted game image/audio published.

This is **not genuine playable original first Spring**. Starting from original PRE_SETUP country templates, the bounded bank1 source finance arithmetic yields 14-country aggregate tax857 wage957 food consumption782 and 27-country tax2098 wage2367 food consumption1868, with no starving country. This is a deterministic counterfactual template projection, not ROM-observed gameplay or a completed season. No RNG seed, ruler, setup, population, troop birth or order budget is created. Port metadata stays 1.99.0-dev1 and does not claim accepted release.

## Tested commands

```
cmake -S execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j8
ctest --test-dir build -j8 --output-on-failure
GK1_SCENARIO_CSV=execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv ./build/gk1_native_shell --audit-both-spring-finance-batches
xvfb-run -a -s '-screen 0 1280x720x24' ./build/gk1_native_shell --smoke-frames 120
cd build && cpack -G DEB
```

Run test_gk214_spring_batch in current working directory or with source CSV set. This includes 212 actual assertions verifying all 41 source records individually against legacy source-derived gk75 routine, immutability, world index, count, country ID, invalid feeding and cross-world failure atomicity. Sanitizer targeted test uses -fsanitize=address,undefined. Clean re-extraction of EXEC0213 plus exact patch must reproduce seven changed/new source paths by SHA256. The test never invokes original ROM.

## Results and classifications

- VERIFIED_STATIC: exact predecessor source, original-country CSV identities, source finance algorithm, seven patch paths by independent replay; code changes are in existing app.
- OBSERVED_RUNTIME_STOCK_FRESH_RESET: earlier stock WorkRAM Spring1175 and 14 stage captures preserved, **not** reexecuted; unrelated to new finance template projection.
- OBSERVED_RUNTIME_STOCK_ARCHIVED_RESTORED: separate prior SaveRAM Mongol/World checkpoints, no equivalence claim.
- TESTED_PORT_LINUX: predecessor176/176, new Release177/177, Debug177/177, independently patched source Release177/177, focused ASan+UBSan212, 1280x720 native and extracted-DEB 120 frames PASS.
- NOT RUN: new original NES Mesen, native Win32/NSIS, physical Xbox-only full campaign, end-to-end complete game.

## Next actual action

Use original fresh/archived emulator trace to implement source-authentic setup/seed/order-draw sequence and genuine first Spring; the new batch is only a reusable finance pass for that generator. Then player Trade merchant schedule, AI, diplomacy, battle terminal and full Windows/Xbox acceptance. No hardcoded Gold430/439 or Rate121/101. Preserve all source and user campaigns. The private original game SHA256 is ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3, kept out of GitHub/public package. Publishing is not Director/Nobunaga execution acknowledgement.
