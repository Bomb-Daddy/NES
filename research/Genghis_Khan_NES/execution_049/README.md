# Genghis Khan I — EXEC0049 original 41-country loaded-template factory

**Date:** November 6, 2026 · **Version:** 1.00 · **Owner:** Zhuge Liang, executive producer.
**Project:** standalone English NES Genghis Khan I → Windows source. This is NOT ROTK II, DOS, or the separate original KOEI-inspired game.

## Exact authoritative source

`GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv` is **byte-identical** to published `Bomb-Daddy/NES/research/Genghis_Khan_NES/v1.07/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv`.

Original public GitHub blob SHA-1: `82126d270f45f3addcbebfd2bd196d5323476f9d`.
Source file SHA-256: `fa3958f156f09d4b4c15bb559c12aa357fc617a9b1f9217505fe07e8800de581`.
Length: 10,345 bytes; **41** records with **45** columns, split into **14 Mongol Conquest** and **27 World Conquest**.
It is published numeric/officer-name metadata, **not** a licensed ROM, game imagery, SRAM, checkpoint, or audio.

**CRITICAL STAGE:** All values are `LOADED_TEMPLATE_BEFORE_SETUP_AND_SEASONAL_PROCESSING`. They are the NES's loaded economic-country templates BEFORE player/difficulty setup, automatic character rolls and seasonal passes. They MUST NOT silently be shown as first playable Spring. For example, the template for Mongol country1/Temujin has Gold425, Food129, Troops60, reserve32; the already accepted runtime Spring1175 case differs. Do not fabricate a post-setup scenario from this dataset.

## Actual C11 implementation

`gk1_scenario_factory.[ch]` imports all 45 columns into strict typed records: 11 stocks (10 commodities + Food), Arms, Skill, troops/workers, histories, Gold, Morale, Tax, reserve, eligibility, population, ruler and country names, specialty, plus source offset metadata. It validates:

- Exactly 45 named header fields, 41 rows, scenario order 14 then 27, sequential IDs, country-index consistency and the exact documented pre-setup stage.
- Expected names/specialty mapping and byte-safe numeric values.
- Original banked/physical country, specialty and name offsets against source address patterns separately per scenario.
- Five-part drafted population, drafted+reserve combined population, eligible reserve percentage, capped stock fields, and Tax/extra-tax boundary rules.
- Unknown CSV quoting, missing/extra rows, mutated name/header/stage/offset, duplicate scenario ID, invalid numeric inputs: rejected. All failures preserve the previous destination bytes.
- `gk1_open_loaded_scenario()` makes independent mutable copies for an application, explicitly `GK1_PRE_SETUP_TEMPLATE` phase. It does NOT generate artificial playable seasons, AI decisions, rulers, battle maps, diplomacy, full saves or Windows acceptance.

`gk1_scenario_list.c` is a small CLI demonstration that prints 14 or 27 records. It is not a browser game or final game UI. The intended Windows integration is to wire the factory into the EXISTING native game scenario chooser, then implement and apply the original setup and first-turn transforms using separately verified evidence. Do not build a competing game from this module.

## Reproduce and evidence categories

```sh
cmake -S source -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j4
ctest --test-dir build --output-on-failure
build/test_gk1_scenario_factory source/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv
build/gk1_scenario_list source/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv 1
build/gk1_scenario_list source/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv 2
```

Tested on Linux x86_64, GCC14.2, CMake, strict C11 `-Wall -Wextra -Wpedantic -Werror`:
**Debug 3/3 CTest PASS; Release 3/3; ASan+UBSan 3/3; direct unit run 440 checks PASS; clean extracted ZIP Release 3/3.** All failures from initial compiler-warnings-as-errors are preserved in QA logs; fixed before final verification. No Windows binary was compiled or executed. No physical Xbox controller was tested. No original NES execution occurred in this batch; original provenance comes from previously published verified v1.07 research. These classifications are `VERIFIED_STATIC` (blob/data) and `TESTED_PORT_LINUX` (new module).

## Receiving work/limits

**Owner Nobunaga**: merge G02 loader data and pre-setup stage into inherited C11 desktop game (current research shell through EXEC0046/47); preserve controller-only 1280×720 UI; add original player/difficulty/autostat scenario setup before first playable turn. **Cao Cao**: independent new-port negative QA, missing/duplicate rows, mutated all-41 source identity, and no false Spring playable claims. **Zhuge Liang**: continue original source unknowns and coordinate with hourly G03 Divide implementation, avoiding conflicting parallel edits. **Director**: no new decision required. G02 remains PARTIAL; G11 G12 still unfinished. Publication is not read/acknowledgment.
