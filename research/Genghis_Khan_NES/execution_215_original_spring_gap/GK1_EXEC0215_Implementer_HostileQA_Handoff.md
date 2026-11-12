# GK1 EXEC0215 — ORIGINAL STOCK SPRING SOURCE GAP EXECUTION — Implementer/Hostile QA Handoff

**Date:** 2026-11-12. **Inherits:** exact complete EXEC0214 native C11 game, original v1.99.0-dev1 research build, including previous 764 archive members. **Scope:** Same standalone GK1 game; not ROTK II or original KOEI-inspired strategy game. 

## Actual executable source work

New `execution_199/gk215_original_spring_gap.c/.h`, `test_gk215_original_spring_gap.c`, linked same native game state, CLI `--audit-original-first-spring-gaps FRESH_RAW ARCHIVED_MONGOL_RAW ARCHIVED_WORLD_RAW` and new CTest. This code verifies authentic 14/27 country ORIGINAL first-Spring raw records and compares all seven source-bounded economic/workforce values for every country against the existing linked all-country Spring batch. Uses all 41 canonical PRE_SETUP templates and original checkpoint profiles, including correct fresh WorkRAM and archived SaveRAM distinction, 83-byte country records, season/order state, SHA-pinned raw source. Zero observations are made up. Does not mutate input world on any path, including adversarial source errors. A successful audit is **not** a playable Spring; no hardcoded source observation promoted to game output.

## Actual results, scope and hostile QA

| Original history | Countries with finance/workforce residual | Gold mismatches | Food mismatches | Troop mismatches | Work force mismatches | Sum(gold actual − model) | Sum(food actual − model) | Sum(troops actual − model) |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| Fresh stock reset, Spring 1175, WorkRAM | 14/14 | 14 | 14 | 14 | 14 | −253 | +9 | −156 |
| Archived stock Mongol, Spring 1175, SaveRAM | 14/14 | 14 | 14 | 14 | 14 | −367 | +60 | −188 |
| Archived stock World, Spring 1206, SaveRAM | 27/27 | 27 | 27 | 27 | 27 | −517 | +162 | −452 |

The finance-only model is evaluated on untransformed PRE_SETUP templates, so **these residuals are expected and are not evidence that the original ROM is inconsistent**. They identify incomplete setup/season/growth handling at whole-country level. A negative residual means the authentic observed stock state is lower than the model for that field. They do not isolate RNG causality.

**VERIFIED_STATIC:** The original stock ROM SHA-256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3` and three original source raw identities authenticated in private only. Source changes are five path exact patch roundtrip. 764 previous archive members preserved except precisely two intentionally modified source paths; three new source files added.

**OBSERVED_RUNTIME_STOCK_FRESH_RESET (prior capture, NOT new Mesen):** 14 countries at WorkRAM physical 0x0DCD; source country1 Gold430 Food208 Troops61 Rate121, orders3, Spring1175. **OBSERVED_RUNTIME_STOCK_ARCHIVED_RESTORED (prior capture, NOT new Mesen):** Mongol country1 Gold439 Food208 Troops61 Rate101, orders3 Spring1175; World country1 Gold193 Food332 Troops116 Rate114, orders3 Spring1206. These are *distinct history/RNG paths* and cannot be reduced to one hardcoded start constant. Correct goods offset +0x22.

**TESTED_PORT_LINUX:** Release CTest178/178, Debug CTest178/178, 91 private-source focused assertions under sanitizer (ASan+UBSan), native Linux X11 and Debian extracted both actual 120 frames at 1280×720; CPack DEB produced, extracted and executed.

**NOT RUN:** new stock original NES Mesen session; native Win32 compilation/NSIS, physical Xbox controller, complete end-to-end campaigns.

**OPEN IMPLEMENTATION:** Original setup/initial RNG chronology/difficulty/player/ruler generation and independent first-Spring gameplay with all 14/27 countries, three player orders; merchant scheduler, AI, battle ending/aftermath, diplomacy, dynasty, graphics/audio and full save/quit/cold-load and Windows acceptance. Don't call this a completed milestone; do not stop recurring execution. `source_observation_only` stays true and `generated_setup/generated_rng/generated_ai/orders_granted` remain false. Original ROM, SRAM, raw country records, Mesen states and licensed assets remain private; NEVER push to GitHub.

## Reproduce

```sh
cmake -S execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j8
ctest --test-dir build -j8 --output-on-failure
# Private source input examples stay PRIVATE; not in public ZIP:
./build/test_gk215_original_spring_gap execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv FRESH_PRIVATE.raw MONGOL_PRIVATE.raw WORLD_PRIVATE.raw
# Or the actual game binary with GK1_SCENARIO_CSV set:
GK1_SCENARIO_CSV=execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv ./build/gk1_native_shell --audit-original-first-spring-gaps FRESH_PRIVATE.raw MONGOL_PRIVATE.raw WORLD_PRIVATE.raw
```

**Reproducible source integration**: extract predecessor EXEC0214 complete copyright-clean ZIP, `patch -p0 --batch -i GK1_EXEC0215_CopyrightClean_Integration.patch`; resulting five paths are SHA-exact; no other inherited content replaced. Public QA test without private raw inputs confirms invalid-source refusal and rollback; full private positive needs authorized source in private Genghis folder `1mJ72ezJb9kzIGUHHXPTh3lXtiVxWjEL7`.

**Publication and acceptance:** Collaboration Drive parent `1d0TA1lgqsVA3AWQQ9o0FZgmvao90oCeT`; public copyright-clean GitHub `Bomb-Daddy/NES`. Publication does not imply agent read, gameplay or Director acceptance. Keep current version `1.99.0-dev1` uncertified.
