# GK1 EXEC0213 — authentic original-stock Spring boundary, source-bounded native C11

Implemented in the existing `gk1_app_state` and native Win32/X11 source, not a disconnected example. The C11 `gk213_stock_spring_boundary.c/h` module separates a source-observed country-one beginning-food bonus, Spring feeding, tax-before-wages, and a later observed troop increment. `gk1_app_observed_first_spring_boundary` authenticates unchanged original 41-country pre-setup CSV, all fourteen *private* original-USA Mesen fresh-reset event receipts, correct WorkRAM final source bytes, and computes a bounded numeric consistency result without changing source countries or granting orders.

Original observed country-one boundaries in ONE specific reset/choice history: pre-setup Food129 Gold425 Troops60; intermediate Food258; Spring Food208 Gold430 Troops61, original three orders granted later. The bounded C11 6502-derived food/tax/wage equations are consistent with source-observed economy186, morale118, tax35, yielding tax65, payroll60, Food208 and Gold430; the observed troop61 is explicitly a later **input**, not generated growth. Economy186 and economy187 can both give the same tax65 by integer truncation; matching finance is **not proof** of setup/RNG chronology or unique intermediate values. The new C11 regression checks this ambiguity and corruption/rollback guards.

## Reproduce

```sh
cmake -S execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j8
ctest --test-dir build -j8 --output-on-failure
# Private evidence never included in the public source ZIP/GitHub:
./build/test_gk213_stock_spring_boundary /PRIVATE/fresh_stage_directory execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv
./build/gk1_native_shell --audit-original-first-spring-boundary /PRIVATE/fresh_stage_directory execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv
```

Source-static consistency and 2026-11-07 previously captured original evidence, NOT a fresh Mesen run. Unresolved: original power-on RNG seed and draw order, generalized first-Spring all14/27, merchant presence, autonomous AI/diplomacy/dynasty, battles/captures, Windows NSIS and physical Xbox acceptance. Never infer a universal country bonus from this one observation. All original ROM/STATE/RAM/private JSON remain in private Genghis research.
